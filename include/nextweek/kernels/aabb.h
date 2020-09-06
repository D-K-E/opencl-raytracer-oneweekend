#ifndef AABB_H
#define AABB_H

#include "macros.h"
#include "ray.h"

typedef struct Aabb {
  Point3 min;
  Point3 max;
} Aabb;
Aabb makeAabb(Point3 min, Point3 max) {
  Aabb aabb;
  aabb.min = min;
  aabb.max = max;
  return aabb;
}

void get_minmax_dir_orig(int index, float *min, float *max,
                         float *orig, float *dir, Vec3 minv,
                         Vec3 maxv, Vec3 origv, Vec3 dirv) {
  if (index == 0) {
    *min = minv.x;
    *max = maxv.x;
    *orig = origv.x;
    *dir = dirv.x;
  } else if (index == 1) {
    *min = minv.y;
    *max = maxv.y;
    *orig = origv.y;
    *dir = dirv.y;
  } else {
    *min = minv.z;
    *max = maxv.z;
    *orig = origv.z;
    *dir = dirv.z;
  }
}

bool hit_aabb(const Aabb *aabb, const Ray *r, float tmin,
              float tmax) {
  for (int a = 0; a < 3; a++) {
    float min, max, orig, dir;
    get_minmax_dir_orig(a, &min, &max, &orig, &dir,
                        aabb->min, aabb->max, r->origin,
                        r->direction);
    float invd = 1.0f / dir;
    float t0 =
        fmin((min - orig) * invd, (max - orig) * invd);
    float t1 =
        fmax((min - orig) * invd, (max - orig) * invd);
    if (invd < 0.0f) {
      float t = t1;
      t1 = t0;
      t0 = t;
    }
    tmin = t0 > tmin ? t0 : tmin;
    tmax = t1 > tmax ? t1 : tmax;
    if (tmax <= tmin) {
      return false;
    }
  }
  return true;
}
Aabb surrounding_box(Aabb box0, Aabb box1) {
  Point3 small = vec3(fmin(box0.min.x, box1.min.x),
                      fmin(box0.min.y, box1.min.y),
                      fmin(box0.min.z, box1.min.z));
  Point3 big = vec3(fmax(box0.max.x, box1.max.x),
                    fmax(box0.max.y, box1.max.y),
                    fmax(box0.max.z, box1.max.z));
  return makeAabb(small, big);
}

#endif
