#ifndef SPHERE_HPP
#define SPHERE_HPP
#include <oneweekend/external.hpp>
#include <oneweekend/vec3.hpp>

struct Sphere {
  cl_float radius;
  cl_float3 center;
  Sphere(float rad, Vec3 cent) {
    radius = rad;
    center = cent.e;
  }
};

#endif
