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

SceneHittable makeSphereHittable(Point3 c, float r,
                                 Texture material_color,
                                 float material_fuzz,
                                 int material_type) {
  SceneHittable sh;
  sh.h_type = SPHERE;
  sh.sph = makeSphere(r, c);
  sh.mat = makeMaterial(material_type, material_color,
                        material_fuzz);

  return sh;
}
SceneHittable
makeMovingSphereHittable(Point3 c1,
                         Point3 c2, //
                         float r,   //
                         float t0,  //
                         float t1,  //
                         Texture material_color,
                         float material_fuzz,
                         int material_type) {
  SceneHittable sh;
  sh.h_type = MOVING_SPHERE;
  sh.msph = makeMovingSphere(r, c1, c2, t0, t1);
  sh.mat = makeMaterial(material_type, material_color,
                        material_fuzz);

  return sh;
}
SceneHittable makeSHittable(int hittable_type, Point3 c1,
                            Point3 c2, float r, float t0,
                            float t1,
                            Texture material_color,
                            float material_fuzz,
                            int material_type) {
  if (hittable_type == MOVING_SPHERE) {
    return makeMovingSphereHittable(
        c1, c2, r, t0, t1, material_color, material_fuzz,
        material_type);
  } else {
    return makeSphereHittable(c1, r, material_color,
                              material_fuzz, material_type);
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
  bool isHit = false;
  if (h.h_type == SPHERE) {
    isHit = hit_sphere(&h.sph, r, t_min, t_max, rec);
  } else {
    isHit =
        hit_moving_sphere(&h.msph, r, t_min, t_max, rec);
  }
  if (isHit) {
    rec->mat_ptr = h.mat;
  }
  return isHit;
}

bool hit_scene(__constant int *htypes,
               __constant Vec3 *centers,
               __constant Vec3 *centers2,
               __constant float *radiuss,
               __constant float2 *times,
               __constant Vec3 *material_colors,
               __constant float *material_fuzzs,
               __constant int *material_types, const Ray *r,
               float t_min, float t_max, int sphere_count,
               HitRecord *rec) {
  HitRecord temp;
  bool anyHit = false;
  float closest = t_max;
  for (int i = 0; i < sphere_count; i++) {
    Texture material_color;
    TextureTypes ttype;
    ttype = i % 2 == 1 ? SOLID_COLOR : DOUBLE_COLOR;
    material_color =
        makeTexture(ttype, material_colors[i], v3(-5.0f));
    SceneHittable h = makeSHittable(
        htypes[i], centers[i], centers2[i], radiuss[i],
        times[i].x, times[i].y, material_color,
        material_fuzzs[i], material_types[i]);
    if (hit(h, r, t_min, closest, &temp)) {
      anyHit = true;
      closest = temp.t;
      *rec = temp;
    }
  }
  return anyHit;
}

bool bounding_box_scene(__constant int *htypes,
                        __constant Vec3 *centers,
                        __constant Vec3 *centers2,
                        __constant float *radiuss,
                        __constant float2 *times,
                        __constant Vec3 *material_colors,
                        __constant float *material_fuzzs,
                        __constant int *material_types,
                        float t_min, float t_max,
                        int sphere_count, Aabb *outbox) {
  if (sphere_count == 0)
    return false;

  Aabb temp;
  bool first_box = true;
  for (int i = 0; i < sphere_count; i++) {
    Texture material_color;
    TextureTypes ttype;
    ttype = i % 2 == 1 ? SOLID_COLOR : DOUBLE_COLOR;
    material_color =
        makeTexture(ttype, material_colors[i], v3(-5.0f));

    SceneHittable s = makeSHittable(
        htypes[i], centers[i], centers2[i], radiuss[i],
        times[i].x, times[i].y, material_color,
        material_fuzzs[i], material_types[i]);
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
