#ifndef SPHERE_H
#define SPHERE_H
#include "aabb.h"
#include "constants.h"
#include "hitrecord.h"
#include "macros.h"
#include "ray.h"

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

Sphere makeSphere(float r, Vec3 c) {
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

Point3 get_center_ms(MovingSphere *s, float time) {
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

#endif
