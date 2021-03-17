#include "orennayarbrdf.h"
#include <warpfunctions.h>

Color3f OrenNayarBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    Float Cos2ThetaI = CosTheta(wi) * CosTheta(wi);
    Float Cos2ThetaO = CosTheta(wo) * CosTheta(wo);
    Float sinThetaI = std::sqrt(std::max(Float(0), Float(1 - Cos2ThetaI)));
    Float sinThetaO = std::sqrt(std::max(Float(0), Float(1 - Cos2ThetaO)));;

    Float maxCos = 0;
    if (sinThetaI > 1e-4 && sinThetaO > 1e-4)
    {
        Float sinPhiI = (sinThetaI == 0) ? 0 : glm::clamp(wi.y / sinThetaI, -1.f, 1.f);
        Float cosPhiI = (sinThetaI == 0) ? 1 : glm::clamp(wi.x / sinThetaI, -1.f, 1.f);
        Float sinPhiO = (sinThetaO == 0) ? 0 : glm::clamp(wo.y / sinThetaO, -1.f, 1.f);
        Float cosPhiO = (sinThetaO == 0) ? 1 : glm::clamp(wo.x / sinThetaO, -1.f, 1.f);
        Float dCos = cosPhiI * cosPhiO + sinPhiI * sinPhiO;
        maxCos = std::max((Float)0, dCos);
    }

    Float sinAlpha, tanBeta;
    if (AbsCosTheta(wi) > AbsCosTheta(wo))
    {
        sinAlpha = sinThetaO;
        tanBeta = sinThetaI / AbsCosTheta(wi);
    } else {
        sinAlpha = sinThetaI;
        tanBeta = sinThetaO / AbsCosTheta(wo);
    }

    return R * InvPi * (A + B * maxCos * sinAlpha * tanBeta);
}

Color3f OrenNayarBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
                        Float *pdf, BxDFType *sampledType) const
{
    //Uniform:
//    *wi = WarpFunctions::squareToHemisphereUniform(u);
    //Cosine-weighted:
    *wi = WarpFunctions::squareToHemisphereCosine(u);

    if (wo.z < 0)
    {
        wi->z *= -1;
    }
    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);
}

float OrenNayarBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //Uniform:
//    return SameHemisphere(wo, wi) ? Inv2Pi : 0;
    //Cosine-weighted:
    return SameHemisphere(wo, wi) ? AbsCosTheta(wi) * InvPi : 0;
}

