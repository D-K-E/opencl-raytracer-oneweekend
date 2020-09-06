#ifndef CAMERA_H
#define CAMERA_H
#include "kutils.h"
#include "macros.h"
#include "ray.h"
// -------------------- Camera ---------------------

typedef struct Camera {
  Point3 origin;
  Vec3 lower_left_corner;
  float3 horizontal;
  float3 vertical;
  float3 w, v, u;
  float lens_radius;
  float time0, time1;
} Camera;

Camera makeCamera(Vec3 origin, Vec3 lleft, Vec3 h,
                  Vec3 vert, float lens_radius, Vec3 w,
                  Vec3 u, Vec3 v, float t0, float t1) {
  Camera cam;
  cam.origin = origin;
  cam.lower_left_corner = lleft;
  cam.horizontal = h;
  cam.vertical = vert;
  cam.w = w;
  cam.u = u;
  cam.v = v;
  cam.lens_radius = lens_radius;
  cam.time0 = t0;
  cam.time1 = t1;
  return cam;
}

Ray get_ray(Camera cam, float s, float t, float3 seed) {
  Vec3 rd = cam.lens_radius * random_in_unit_disk(seed);
  float tm =
      map_range(seed.x, -1.0f, 1.0f, cam.time0, cam.time1);
  Vec3 offset = cam.u * rd.x + cam.v * rd.y;
  return makeRay(cam.origin + offset,
                 cam.lower_left_corner +
                     s * cam.horizontal + t * cam.vertical -
                     cam.origin - offset,
                 tm);
}

// ------------------ end Camera -------------------

#endif
