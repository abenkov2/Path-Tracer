#include "fresnel.h"

Color3f FresnelDielectric::Evaluate(float cosThetaI) const
{
    //TODO
    cosThetaI = glm::clamp(cosThetaI, -1.f, 1.f);
    bool entering = cosThetaI > 0.f;
    Float etaI_t = etaI;
    Float etaT_t = etaT;
    if (!entering) {
        std::swap(etaI_t, etaT_t);
        cosThetaI = std::abs(cosThetaI);
    }

    Float sinThetaI = std::sqrt(std::max((Float)0, 1 - cosThetaI * cosThetaI));
    Float sinThetaT = etaI_t / etaT_t * sinThetaI;

    if (sinThetaT >= 1)
    {
        return Color3f(1.f);
    }

    Float cosThetaT = std::sqrt(std::max((Float)0, 1 - sinThetaT * sinThetaT));

    Float Rparl = ((etaT_t * cosThetaI) - (etaI_t * cosThetaT)) / ((etaT_t * cosThetaI) + (etaI_t * cosThetaT));
    Float Rperp = ((etaI_t * cosThetaI) - (etaT_t * cosThetaT)) / ((etaI_t * cosThetaI) + (etaT_t * cosThetaT));

    return Color3f((Rparl * Rparl + Rperp * Rperp) / 2.f);
}
