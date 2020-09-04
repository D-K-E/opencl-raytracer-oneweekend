#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <nextweek/external.hpp>
#include <nextweek/ray.hpp>
#include <nextweek/utils.hpp>
#include <nextweek/vec3.hpp>

class Camera {
public:
  cl_float3 origin;
  cl_float3 lower_left_corner;
  cl_float3 horizontal;
  cl_float3 vertical;
  cl_float3 w, v, u;
  cl_float lens_radius;
  cl_float time0;
  cl_float time1;

public:
  Camera(Point3 lookfrom, Point3 lookat, Vec3 worldUp,
         float vfov, float aspect_ratio, float aperture,
         float focus_dist, float t0 = 0.0f,
         float t1 = 0.0f) {
    lens_radius = aperture / 2.0f;
    time0 = t0;
    time1 = t1;
    float theta = degrees_to_radians(vfov);
    float h = tan(theta / 2.0f);

    float view_height = 2.0f * h;
    float view_width = aspect_ratio * view_height;

    Vec3 wv = unit_vector(lookfrom - lookat);
    Vec3 uv = unit_vector(cross(worldUp, wv));
    Vec3 vv = cross(wv, uv);

    Vec3 corig = Vec3(lookfrom.e);
    Vec3 hori = view_width * uv * focus_dist;
    Vec3 vert = view_height * vv * focus_dist;
    Vec3 lleft = corig - (hori / 2.0f) - (vert / 2.0f) -
                 (focus_dist * wv);

    // pass values to attributes
    vertical = vert.e;
    horizontal = hori.e;
    lower_left_corner = lleft.e;
    origin = corig.e;
    w = wv.e;
    u = uv.e;
    v = vv.e;
  }
};

#endif
