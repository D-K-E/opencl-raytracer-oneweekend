#ifndef AABB_HPP
#define AABB_HPP
#include <nextweek/external.hpp>
#include <nextweek/ray.hpp>
#include <nextweek/vec3.hpp>

class Aabb {
public:
  Point3 _min;
  Point3 _max;

public:
  Aabb() {}
  Aabb(const Point3 &a, const Point3 &b)
      : _min(a), _max(b) {}
  Point3 min() const { return _min; }
  Point3 max() const { return _max; }
};
#endif
