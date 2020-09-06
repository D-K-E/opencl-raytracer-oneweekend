#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.h"
#include "hitrecord.h"
#include "macros.h"
#include "material.h"
#include "ray.h"
#include "sphere.h"
#include "texture.h"

typedef enum HittableType {
  SPHERE = 1,
  MOVING_SPHERE = 2
} HittableType;

typedef struct SceneHittable {
  HittableType h_type;
  Sphere sph;
  MovingSphere msph;
  Material mat;
} SceneHittable;

SceneHittable makeSphereHittable(Point3 c, float r) {
  SceneHittable sh;
  sh.h_type = SPHERE;
  sh.sph = makeSphere(r, c);

  return sh;
}
SceneHittable makeMovingSphereHittable(Point3 c1,
                                       Point3 c2, //
                                       float r,   //
                                       float t0,  //
                                       float t1   //
                                       ) {
  SceneHittable sh;
  sh.h_type = MOVING_SPHERE;
  sh.msph = makeMovingSphere(r, c1, c2, t0, t1);

  return sh;
}
SceneHittable makeSHittable(int hittable_type, Point3 c1,
                            Point3 c2, float r, float t0,
                            float t1,
                            Texture material_color,
                            float material_fuzz,
                            int material_type) {
  SceneHittable s;
  if (hittable_type == MOVING_SPHERE) {
    s = makeMovingSphereHittable(c1, c2, r, t0, t1);
  } else {
    s = makeSphereHittable(c1, r);
  }
  s.mat = makeMaterial(material_type, material_color,
                       material_fuzz);
  return s;
}

SceneHittable makeJustHittable(
    // ---------------- Hittables --------------
    int hittable_type, // hittable type
    Point3 c1,         // center1
    Point3 c2,         // center2
    float r,           // radius
    float t0,          // time0
    float t1           // time1
    ) {
  if (hittable_type == MOVING_SPHERE) {
    return makeMovingSphereHittable(c1, c2, r, t0, t1);
  } else {
    return makeSphereHittable(c1, r);
  }
}

bool bounding_box_shittable(SceneHittable h, float tmin,
                            float tmax, Aabb *aabb) {
  if (h.h_type == SPHERE) {
    return bounding_box_sphere(&h.sph, tmin, tmax, aabb);
  } else {
    return bounding_box_moving_sphere(&h.msph, tmin, tmax,
                                      aabb);
  }
}

bool hit(SceneHittable h, const Ray *r, float t_min,
         float t_max, HitRecord *rec) {
  if (h.h_type == MOVING_SPHERE) {
    return hit_moving_sphere(&h.msph, r, t_min, t_max, rec);
  } else {
    return hit_sphere(&h.sph, r, t_min, t_max, rec);
  }
  return false;
}

bool hit_scene(
    // -------------- Hittables ---------------
    __constant int *htypes,      //
    __constant Point3 *centers,  //
    __constant Point3 *centers2, //
    __constant float *radiuss,   //
    __constant float2 *times,    //
    // ------------- Materials ----------------
    __constant int *material_types,
    __constant float *material_fuzzs,
    // ------------- Textures -----------------
    __constant int *texture_types,
    __constant Color *texture_colors,
    const Ray *r, //
    float t_min, float t_max, int sphere_count,
    HitRecord *rec) {
  HitRecord temp;
  bool anyHit = false;
  float closest = t_max;
  for (int i = 0; i < sphere_count; i++) {
    SceneHittable h = makeJustHittable(
        htypes[i], centers[i], centers2[i], radiuss[i],
        times[i].x, times[i].y);
    if (hit(h, r, t_min, closest, &temp)) {
      anyHit = true;
      closest = temp.t;
      Texture t = makeTexture(texture_types[i],
                              texture_colors[i], v3(-1.0f));
      temp.mat_ptr = makeMaterial(material_types[i], t,
                                  material_fuzzs[i]);
      *rec = temp;
    }
  }
  return anyHit;
}

bool bounding_box_scene(
    // ---------- hittables -------------------
    __constant int *htypes, __constant Point3 *centers,
    __constant Point3 *centers2, __constant float *radiuss,
    __constant float2 *times,
    //
    float t_min, float t_max, int sphere_count,
    Aabb *outbox) {
  if (sphere_count == 0)
    return false;

  Aabb temp;
  bool first_box = true;
  for (int i = 0; i < sphere_count; i++) {
    SceneHittable s = makeJustHittable(
        htypes[i], centers[i], centers2[i], radiuss[i],
        times[i].x, times[i].y);
    if (!bounding_box_shittable(s, t_min, t_max, &temp)) {
      return false;
    }
    *outbox =
        first_box ? temp : surrounding_box(*outbox, temp);
    first_box = false;
  }
  return true;
}

#endif
