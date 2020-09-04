#ifndef RAY_HPP
#define RAY_HPP

#include <nextweek/external.hpp>
#include <nextweek/vec3.hpp>

struct Ray {
  cl_float3 origin;
  cl_float3 direction;
  cl_float time;
};

#endif
