#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <oneweekend/external.hpp>
#include <oneweekend/ray.hpp>
#include <oneweekend/vec3.hpp>

struct Camera {
  cl_float3 origin;
  cl_float3 lower_left_corner;
  cl_float3 horizontal;
  cl_float3 vertical;
};

Vec3 getCameraAttr(Camera cam, const std::string name) {
  if (name == "origin") {
    return Vec3(cam.origin);
  } else if (name == "horizontal") {
    return Vec3(cam.horizontal);
  } else if (name == "vertical") {
    return Vec3(cam.vertical);
  } else {
    return Vec3(cam.lower_left_corner);
  }
}

Camera makeCamera() {
  //
  float aspect_ratio = 16.0f / 9.0f;
  float view_height = 2.0f;
  float view_width = aspect_ratio * view_height;

  float focal_length = 1.0f;

  Point3 cam_origin = Vec3(0.0f, 0.0f, 0.0f);
  Vec3 horizontal(view_width, 0.0f, 0.0f);
  Vec3 vertical(0.0f, view_height, 0.0f);
  Vec3 focal_vec(0.0f, 0.0f, focal_length);
  Vec3 lleft = cam_origin - (horizontal / 2) - vertical / 2 - focal_vec;
  Camera cam;
  cam.origin = cam_origin.e;
  cam.horizontal = horizontal.e;
  cam.vertical = vertical.e;
  cam.lower_left_corner = lleft.e;
  return cam;
}
Ray get_ray(Camera cam, float u, float v) {
  Ray r;
  r.origin = cam.origin;
  Vec3 corigin = getCameraAttr(cam, "origin");
  Vec3 ch = getCameraAttr(cam, "horizontal");
  Vec3 cv = getCameraAttr(cam, "vertical");
  Vec3 cll = getCameraAttr(cam, "lower_left_corner");
  Vec3 rdir = cll + u * ch + v * cv - corigin;
  r.direction = rdir.e;
  return r;
}

#endif
