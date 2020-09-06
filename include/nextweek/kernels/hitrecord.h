#ifndef HITRECORD_H
#define HITRECORD_H

#include "macros.h"
#include "material.h"
#include "ray.h"

typedef struct HitRecord {
  Point3 p;    // hit point
  Vec3 normal; // hit point normal
  float t;     // distance to hitpoint
  float u;     // coordinate in screen space
  float v;     // coordinate in screen space
  bool front_face;
  Material mat_ptr;
} HitRecord;

void set_front_face(HitRecord *rec, const Ray *r,
                    const Vec3 *normal) {
  bool face = dot(r->direction, *normal) < 0.0f;
  rec->front_face = face;
  rec->normal =
      rec->front_face ? *normal : -1.0f * (*normal);
}

#endif
