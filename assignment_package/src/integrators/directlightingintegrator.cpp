#include "directlightingintegrator.h"


Color3f DirectLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
    //TODO
    Color3f Ld(0.f);
    Ray r(ray);
    Intersection isect;
    bool foundIntersection = scene.Intersect(r, &isect);

    if (!foundIntersection)
    {
        return Color3f(0.f);
    }

    if (!isect.ProduceBSDF())
    {
        return isect.Le(-r.direction);
    }

    Vector3f wo = -r.direction, wi;
    Float lightPdf = 0;
    Float brdfPdf = 0;
    BxDFType bsdfFlags = BSDF_ALL;

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
            Ray ray2 = isect.SpawnRay(wi);
            bool hitSurface = scene.Intersect(ray2, &isect2);

            if (!hitSurface || (isect2.objectHit->GetAreaLight() != light.get()))
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

    if (!IsBlack(f) && brdfPdf > 0)
    {
        Float weight = 1;
        if (!sampledSpecular)
        {
            lightPdf = light->Pdf_Li(isect, wi);
            if (lightPdf == 0)
            {
                return Ld;
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

        if (!IsBlack(Li))
        {
            Ld += f * Li * weight * float (nLights) / brdfPdf;
        }
    }

    return Ld;
}

float DirectLightingIntegrator::BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf) const
{
    if (nf * fPdf + ng * gPdf == 0)
    {
        return 0.f;
    }
    return (nf * fPdf) / (nf * fPdf + ng * gPdf);
}

float DirectLightingIntegrator::PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf) const
{
    Float f = nf * fPdf, g = ng * gPdf;
    if (f * f + g * g == 0)
    {
        return 0.f;
    }
    return (f * f) / (f * f + g * g);
}
