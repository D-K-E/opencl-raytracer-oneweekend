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

struct MovingSphere {
  cl_float radius;
  cl_float3 center1;
  cl_float3 center2;
  cl_float time0;
  cl_float time1;
  MovingSphere(float rad, Vec3 cent, Vec3 cent2, float t0,
               float t1)
      : radius(rad), center1(cent.e), center2(cent2.e),
        time0(t0), time1(t1) {}
};

#endif
