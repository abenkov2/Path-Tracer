#define _USE_MATH_DEFINES
#include "warpfunctions.h"
#include <math.h>

Point3f WarpFunctions::squareToDiskUniform(const Point2f &sample)
{
    float r = std::sqrt(sample[0]);
    float theta = 2 * Pi * sample[1];

    return r * glm::vec3(std::cos(theta), std::sin(theta), 0.0f);
}

Point3f WarpFunctions::squareToDiskConcentric(const Point2f &sample)
{
    glm::vec2 offset = 2.0f * sample - glm::vec2(1, 1);

    if (offset.x == 0 && offset.y == 0)
    {
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    float theta, r;
    if (std::abs(offset.x) > std::abs(offset.y))
    {
        r = offset.x;
        theta = PiOver4 * offset.y / offset.x;
    } else {
        r = offset.y;
        theta = PiOver2 - PiOver4 * offset.x / offset.y;
    }

    return r * glm::vec3(std::cos(theta), std::sin(theta), 0.0f);
}

float WarpFunctions::squareToDiskPDF(const Point3f &sample)
{
    return InvPi;
}

Point3f WarpFunctions::squareToSphereUniform(const Point2f &sample)
{
    float z = 1.0f - 2.0f * sample[0];
    return Point3f(std::cos(TwoPi * sample[1]) * std::sqrt(1 - z * z), std::sin(TwoPi * sample[1]) * std::sqrt(1 - z * z), z);

}

float WarpFunctions::squareToSphereUniformPDF(const Point3f &sample)
{
    return Inv4Pi;
}

Point3f WarpFunctions::squareToSphereCapUniform(const Point2f &sample, float thetaMin)
{
    float cosTheta = 1.0f - sample[0] + sample[0] * std::cos(Pi - thetaMin * Pi / 180.0f);
    float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
    float phi = sample[1] * TwoPi;
    return glm::vec3(std::cos(phi) * sinTheta, std::sin(phi) * sinTheta, cosTheta);
}

float WarpFunctions::squareToSphereCapUniformPDF(const Point3f &sample, float thetaMin)
{
    float cosThetaMax = std::cos(Pi - thetaMin * Pi / 180.0f);
    return 1 / (TwoPi * (1 - cosThetaMax));
}

Point3f WarpFunctions::squareToHemisphereUniform(const Point2f &sample)
{
    float z = sample[0];
    return Point3f(std::cos(TwoPi * sample[1]) * std::sqrt(1 - z * z), std::sin(TwoPi * sample[1]) * std::sqrt(1 - z * z), z);
}

float WarpFunctions::squareToHemisphereUniformPDF(const Point3f &sample)
{
    return Inv2Pi;
}

Point3f WarpFunctions::squareToHemisphereCosine(const Point2f &sample)
{
    Point3f sq_to_disk = WarpFunctions::squareToDiskConcentric(sample);
    float x = sq_to_disk[0];
    float y = sq_to_disk[1];

    return glm::vec3(x, y, std::sqrt(std::max(0.0f, 1 - x * x - y * y)));
}

float WarpFunctions::squareToHemisphereCosinePDF(const Point3f &sample)
{
   return InvPi * sample[2];
}
