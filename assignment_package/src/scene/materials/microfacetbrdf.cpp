#include "microfacetbrdf.h"

Color3f MicrofacetBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    Float cosThetaO = AbsCosTheta(wo), cosThetaI = AbsCosTheta(wi);
    Vector3f wh = wi + wo;
    // Handle degenerate cases for microfacet reflection
    if (cosThetaI == 0 || cosThetaO == 0) return Color3f(0.f);
    if (wh.x == 0 && wh.y == 0 && wh.z == 0) return Color3f(0.f);
    wh = glm::normalize(wh);
    Color3f F = fresnel->Evaluate(glm::dot(wi, wh));
    float D = distribution->D(wh);
    float G = distribution->G(wo, wi);
    return R * D * G * F /
            (4 * cosThetaI * cosThetaO);
}

Color3f MicrofacetBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi, Float *pdf, BxDFType *sampledType) const
{
    // Sample microfacet orientation $\wh$ and reflected direction $\wi$
    if (wo.z == 0) return Color3f(0.f);
    Vector3f wh = distribution->Sample_wh(wo, xi);
    *wi = glm::reflect(-wo, wh);
    if (!SameHemisphere(wo, *wi)) return Color3f(0.f);

    // Compute PDF of _wi_ for microfacet reflection
    *pdf = distribution->Pdf(wo, wh) / (4 * glm::dot(wo, wh));
    return f(wo, *wi);
}

float MicrofacetBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    if (!SameHemisphere(wo, wi)) return 0;
    Vector3f wh = glm::normalize(wo + wi);
    return distribution->Pdf(wo, wh) / (4 * glm::dot(wo, wh));
}
