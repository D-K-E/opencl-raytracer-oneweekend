#ifndef RAY_H
#define RAY_H

#include "kutils.h"

typedef struct Ray {
  Point3 origin;
  Vec3 direction;
  float time;
} Ray;

Ray makeRay(Point3 orig, Vec3 dir, float tm) {
  Ray r;
  r.origin = orig;
  r.direction = dir;
  r.time = tm;
  return r;
}

Point3 at(Ray r, float dist) {
  return r.origin + r.direction * dist;
}

#endif
