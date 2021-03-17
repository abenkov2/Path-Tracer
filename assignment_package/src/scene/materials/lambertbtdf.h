#ifndef LAMBERTBTDF_H
#define LAMBERTBTDF_H
#include "bsdf.h"

class LambertBTDF : public BxDF
{
public:
    LambertBTDF(const Color3f &R)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) {}

    Color3f f(const Vector3f &wo, const Vector3f &wi) const;

    virtual Color3f Sample_f(const Vector3f &wo, Vector3f *wi,
                              const Point2f &sample, Float *pdf,
                              BxDFType *sampledType = nullptr) const;
    virtual float Pdf(const Vector3f &wo, const Vector3f &wi) const;


  private:
    const Color3f R;
};

#endif // LAMBERTBTDF_H
