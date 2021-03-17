#include "fulllightingintegrator.h"

Color3f FullLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
    //TODO
    Ray r(ray);
    Color3f L(0.f), beta(1.f);
    bool specularBounce = false;
    Float lightPdf = 0;
    Float brdfPdf = 0;
    BxDFType bsdfFlags = BSDF_ALL;
    int maxDepth = depth;


    while (depth > 0)
    {
        Color3f Ld(0.f);
        Intersection isect;
        bool foundIntersection = scene.Intersect(r, &isect);

        if (!foundIntersection)
        {
            break;
        }

        bool hasBSDF = isect.ProduceBSDF();
        if (depth == maxDepth || !hasBSDF || specularBounce)
        {
            L += beta * isect.Le(-r.direction);
            if (!hasBSDF)
            {
                break;
            }
        }

        Vector3f wo = -r.direction, wi;

        //Randomly select light to sample
        int nLights = int(scene.lights.size());
        if (nLights == 0)
        {
            return Color3f(0.f);
        }
        int lightNum = std::min((int)(sampler->Get1D() * nLights), nLights - 1);
        const std::shared_ptr<Light> &light = scene.lights[lightNum];

        Point2f uLight = sampler->Get2D();

        Color3f Li = light->Sample_Li(isect, uLight, &wi, &lightPdf);

        if (lightPdf > 0 && !IsBlack(Li))
        {
            Color3f f;
            f = isect.bsdf->f(wo, wi)* AbsDot(wi, isect.normalGeometric);
            brdfPdf = isect.bsdf->Pdf(wo, wi, bsdfFlags);

            if (!IsBlack(f))
            {
                Intersection isect2;
                Ray vis = isect.SpawnRay(wi);
                bool hitSurface = scene.Intersect(vis, &isect2);

                if (!hitSurface || (!isect2.ProduceBSDF() && isect2.objectHit->GetAreaLight() != light.get()))
                {
                   Li = Color3f(0.f);
                }

                if (!IsBlack((Li)))
                {
                    //comment out power heuristic line and uncomment balance heuristic line as needed to test either:
                    Float weight = PowerHeuristic(1, lightPdf, 1, brdfPdf);
                    //Float weight = BalanceHeuristic(1, lightPdf, 1, brdfPdf);
                    Ld += f * Li * weight * float (nLights) / lightPdf;
                }
            }
        }

        //Brdf:
        Color3f f;
        bool sampledSpecular = false;
        BxDFType sampledType;
        Point2f uBrdf = sampler->Get2D();

        f = isect.bsdf->Sample_f(wo, &wi, uBrdf, &brdfPdf, bsdfFlags, &sampledType);
        f *= AbsDot(wi, isect.normalGeometric);
        sampledSpecular = sampledType & BSDF_SPECULAR;

        bool posLightPDF = true;
        if (!IsBlack(f) && brdfPdf > 0)
        {
            Float weight = 1;
            if (!sampledSpecular)
            {
                lightPdf = light->Pdf_Li(isect, wi);
                if (lightPdf == 0)
                {
                    posLightPDF = false;
                }
                //comment out power heuristic line and uncomment balance heuristic line as needed to test either:
                weight = PowerHeuristic(1, brdfPdf, 1, lightPdf);
                //weight = BalanceHeuristic(1, brdfPdf, 1, lightPdf);
            }

            Intersection lightIsect;
            Ray ray = isect.SpawnRay(wi);

            foundIntersection = scene.Intersect(ray, &lightIsect);
            Li = Color3f(0.f);
            if (foundIntersection)
            {
                if (lightIsect.objectHit->GetAreaLight() == light.get())
                {
                    Li = lightIsect.Le(-wi);
                }
            } else {
                Li = light->Le(ray);
            }

            if (!IsBlack(Li) and posLightPDF)
            {
                Ld += f * Li * weight * float (nLights) / brdfPdf;
            }
        }

        //Ray bounce and global illumination


        Float pdf;
        BxDFType flags;
        Vector3f wi_n;
        f = isect.bsdf->Sample_f(wo, &wi_n, sampler->Get2D(), &pdf, BSDF_ALL, &flags);
        if (IsBlack(f) || pdf == 0.f)
        {
            break;
        }
        beta *= f * AbsDot(wi_n, isect.normalGeometric) / pdf;
        specularBounce = (flags & BSDF_SPECULAR) != 0;
        r = isect.SpawnRay(wi_n);


        L += beta * Ld;
        //Russian roulette check
        Float lum = std::max(beta.x, std::max(beta.y, beta.z));
        Float q = std::max((Float).05, 1 - lum);
        if (sampler->Get1D() < q)
        {
            break;
        }
        beta /= 1 - q;

        depth--;
    }

    return L;
}

float BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    if (nf * fPdf + ng * gPdf == 0)
    {
        return 0.f;
    }
    return (nf * fPdf) / (nf * fPdf + ng * gPdf);
}

float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    Float f = nf * fPdf, g = ng * gPdf;
    if (f * f + g * g == 0)
    {
        return 0.f;
    }
    return (f * f) / (f * f + g * g);
}

