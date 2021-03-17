#include "shape.h"
#include <QDateTime>

pcg32 Shape::colorRNG = pcg32(QDateTime::currentMSecsSinceEpoch());


void Shape::InitializeIntersection(Intersection *isect, float t, Point3f pLocal) const
{
    isect->point = Point3f(transform.T() * glm::vec4(pLocal, 1));
    ComputeTBN(pLocal, &(isect->normalGeometric), &(isect->tangent), &(isect->bitangent));
    isect->uv = GetUVCoordinates(pLocal);
    isect->t = t;
}

Intersection Shape::Sample(const Intersection &ref, const Point2f &xi, float *pdf) const
{
    //TODO
    Intersection isect = Sample(xi, pdf);
    Vector3f lightRay = isect.point - ref.point;
    Float dist2 = glm::length2(lightRay);
    Vector3f wi = glm::normalize(lightRay);

    if (AbsDot(isect.normalGeometric, -wi) * Area() == 0.f)
    {
        *pdf = 0.f;
    } else {
        *pdf = dist2 / (AbsDot(isect.normalGeometric, -wi) * Area());
    }

    return isect;
}

float Shape::Pdf(const Intersection &ref, const Vector3f &wi) const
{
    Ray ray = ref.SpawnRay(wi);
    Intersection it;
    if (!Intersect(ray, &it)) {
        return 0;
    }
    Float pdf = glm::length2(it.point - ref.point) / (AbsDot(it.normalGeometric, -wi) * Area());
    return pdf;
}
