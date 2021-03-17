#include "squareplane.h"
#include <warpfunctions.h>

float SquarePlane::Area() const
{
    //TODO
    return (transform.getScale()).x * (transform.getScale()).y;
}

bool SquarePlane::Intersect(const Ray &ray, Intersection *isect) const
{
    //Transform the ray
    Ray r_loc = ray.GetTransformedCopy(transform.invT());

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    Point3f P = Point3f(t * r_loc.direction + r_loc.origin);
    //Check that P is within the bounds of the square
    if(t > 0 && P.x >= -0.5f && P.x <= 0.5f && P.y >= -0.5f && P.y <= 0.5f)
    {
        InitializeIntersection(isect, t, P);
        return true;
    }
    return false;
}

void SquarePlane::ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const
{
    *nor = glm::normalize(transform.invTransT() * Normal3f(0,0,1));
    CoordinateSystem(*nor, tan, bit);
}


Point2f SquarePlane::GetUVCoordinates(const Point3f &point) const
{
    return Point2f(point.x + 0.5f, point.y + 0.5f);
}

Intersection SquarePlane::Sample(const Point2f &xi, Float *pdf) const
{
    Point2f xiRemapped = xi - Point2f(0.5f);
    Point3f p = Point3f(xiRemapped.x, xiRemapped.y, 0.f);
    Intersection isect;
    Matrix4x4 objectToWorld = transform.T();
    ComputeTBN(p, &(isect.normalGeometric), &(isect.tangent), &(isect.bitangent));
    isect.point = Point3f(objectToWorld * glm::vec4(p, 1.f));

    *pdf = 1 / Area();

    return isect;
}


