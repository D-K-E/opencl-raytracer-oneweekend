#ifndef HITTABLE_H
#define HITTABLE_H

// ------- most basic ----------
#include "material.h"

// ----------------- Hittable.h ------------------

// ----------------------- AABB ----------------------

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

// ---------------- end AABB --------------------
// ---------------- Sphere ----------------------

void get_sphere_uv(const Vec3 p, float *u, float *v) {
  float phi = atan2(p.z, p.x);
  float theta = asin(p.y);
  *u = 1.0f - (phi + PI) / (2.0f * PI);
  *v = (theta + PI / 2.0f) / PI;
}

typedef struct Sphere {
  float radius;
  Point3 center;
} Sphere;

Sphere makeSphere(float r, Point3 c) {
  Sphere s;
  s.radius = r;
  s.center = c;
  return s;
}

bool hit_sphere(const Sphere *s, const Ray *r, float t_min,
                float t_max, HitRecord *rec) {
  float3 oc = r->origin - s->center;
  float a = dot(r->direction, r->direction);
  float hb = dot(oc, r->direction);
  float b = 2.0f * hb;
  float c = dot(oc, oc) - (s->radius * s->radius);
  float disc = (hb * hb) - (a * c);

  if (disc > 0.0f) {
    float root = sqrt(disc);

    float margin = (-hb - root) / a;
    if (margin < t_max && margin > t_min) {
      rec->t = margin;
      rec->p = at(*r, rec->t);
      Vec3 normal = (rec->p - s->center) / s->radius;
      set_front_face(rec, r, &normal);
      float u, v;
      get_sphere_uv((rec->p - s->center), &u, &v);
      rec->u = u;
      rec->v = v;
      return true;
    }

    margin = (-hb + root) / a;
    if (margin < t_max && margin > t_min) {
      rec->t = margin;
      rec->p = at(*r, rec->t);
      float3 normal = (rec->p - s->center) / s->radius;
      set_front_face(rec, r, &normal);
      float u, v;
      get_sphere_uv((rec->p - s->center), &u, &v);
      rec->u = u;
      rec->v = v;
      return true;
    }
  }
  return false;
}

typedef struct MovingSphere {
  float radius;
  Point3 center1;
  Point3 center2;
  float time0;
  float time1;
} MovingSphere;

bool bounding_box_sphere(const Sphere *s, float tmin,
                         float tmax, Aabb *aabb) {
  *aabb = makeAabb(s->center - v3(s->radius),
                   s->center + v3(s->radius));
  return true;
}

MovingSphere makeMovingSphere(float r, Point3 c1, Point3 c2,
                              float t0, float t1) {
  MovingSphere s;
  s.radius = r;
  s.center1 = c1;
  s.center2 = c2;
  s.time0 = t0;
  s.time1 = t1;
  return s;
}

Point3 get_center_ms(const MovingSphere *s, float time) {
  Vec3 center =
      ((time - s->time0) / (s->time1 - s->time0)) *
      (s->center2 - s->center1);
  return s->center1 + center;
}

bool hit_moving_sphere(const MovingSphere *s, const Ray *r,
                       float t_min, float t_max,
                       HitRecord *rec) {
  Vec3 center = get_center_ms(s, r->time);
  float3 oc = r->origin - center;
  float a = dot(r->direction, r->direction);
  float hb = dot(oc, r->direction);
  float b = 2.0f * hb;
  float c = dot(oc, oc) - (s->radius * s->radius);
  float disc = (hb * hb) - (a * c);

  if (disc > 0.0f) {
    float root = sqrt(disc);

    float margin = (-hb - root) / a;
    if (margin < t_max && margin > t_min) {
      rec->t = margin;
      rec->p = at(*r, rec->t);
      Vec3 normal = (rec->p - center) / s->radius;
      set_front_face(rec, r, &normal);
      float u, v;
      get_sphere_uv((rec->p - center), &u, &v);
      rec->u = u;
      rec->v = v;

      return true;
    }

    margin = (-hb + root) / a;
    if (margin < t_max && margin > t_min) {
      rec->t = margin;
      rec->p = at(*r, rec->t);
      float3 normal = (rec->p - center) / s->radius;
      set_front_face(rec, r, &normal);
      float u, v;
      get_sphere_uv((rec->p - center), &u, &v);
      rec->u = u;
      rec->v = v;

      return true;
    }
  }
  return false;
}

bool bounding_box_moving_sphere(const MovingSphere *s,
                                float tmin, float tmax,
                                Aabb *aabb) {
  Point3 c1 = get_center_ms(s, tmin);
  Point3 c2 = get_center_ms(s, tmax);
  Aabb aabb1 =
      makeAabb(c1 - v3(s->radius), c1 + v3(s->radius));
  Aabb aabb2 =
      makeAabb(c2 - v3(s->radius), c2 + v3(s->radius));
  *aabb = surrounding_box(aabb1, aabb2);
  return true;
}

// ----------------- end Sphere -------------------

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
    // ------------- images -------------------
    ImageInfo info, __constant float *images,
    //
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
                              texture_colors[i], info);
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

// ----------------- end Hittable.h --------------

#endif
