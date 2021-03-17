#include "naiveintegrator.h"

Color3f NaiveIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
    Color3f L(0.f);
    Ray r(ray);
    Intersection isect;
    bool foundIntersection = scene.Intersect(r, &isect);

    if (!foundIntersection)
    {
        return L;
    }

    if (depth == 0 || !isect.ProduceBSDF())
    {
        return isect.Le(-r.direction);
    }

    Vector3f wo = -r.direction, wi;
    Float pdf;
    BxDFType flags;
    Color3f f = isect.bsdf->Sample_f(wo, &wi, sampler->Get2D(), &pdf, BSDF_ALL, &flags);

    if (IsBlack(f) || pdf == 0.f)
    {
        return L;
    }

    r = isect.SpawnRay(wi);
    L = (f * AbsDot(wi, isect.normalGeometric) / pdf) * Li(r, scene, sampler, depth - 1);

    return L;
}
