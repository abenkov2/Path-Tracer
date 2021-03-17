#include "diffusearealight.h"

Color3f DiffuseAreaLight::L(const Intersection &isect, const Vector3f &w) const
{
    //TODO
    if (!twoSided)
    {
        return glm::dot(isect.normalGeometric, w) > 0.f ? emittedLight : Color3f(0.f);
    }

    return emittedLight;
}

Color3f DiffuseAreaLight::Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const
{
    Intersection isect = shape->Sample(ref, xi, pdf);
    float dist = glm::length(isect.point - ref.point);
    if (*pdf == 0 || dist < RayEpsilon)
    {
        return Color3f(0.f);
    }

    *wi = glm::normalize(isect.point - ref.point);

    return L(isect, -*wi);

}

float DiffuseAreaLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const
{
//    Ray ray = ref.SpawnRay(wi);
//    Intersection isectLight;
//    if (!shape->Intersect(ray, &isectLight)) {
//        return 0;
//    }
//    Float pdf = glm::length2(isectLight.point - ref.point) / (AbsDot(isectLight.normalGeometric, -wi) * shape->Area());
//    return pdf;
    return shape->Pdf(ref, wi);
}

