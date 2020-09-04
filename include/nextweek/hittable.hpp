#ifndef HITTABLE_HPP
#define HITTABLE_HPP
#include <nextweek/external.hpp>
#include <nextweek/material.hpp>
#include <nextweek/sphere.hpp>
#include <nextweek/utils.hpp>

enum HittableType : cl_int {
  SPHERE = 1,
  MOVING_SPHERE = 2
};

class SceneHittables {
public:
  std::vector<cl_int> types;
  std::vector<cl_float3> center;
  std::vector<cl_float3> center2;
  std::vector<cl_float> radius;
  std::vector<cl_float2> times;
  SceneMaterial *mat_ptr;

  int size;
  cl::Buffer cl_hittable_type;
  cl::Buffer cl_center;
  cl::Buffer cl_center2;
  cl::Buffer cl_radius;
  cl::Buffer cl_times;
  SceneHittables() : size(0) {
    mat_ptr = new SceneMaterial();
  }
  ~SceneHittables() { delete mat_ptr; }

  SceneHittables(int obj_count) : size(obj_count) {
    std::vector<cl_int> obj_types(size);
    std::vector<cl_float3> obj_center(size);
    std::vector<cl_float3> obj_center2(size);
    std::vector<cl_float> obj_r(size);
    std::vector<cl_float2> time_(size);
    mat_ptr = new SceneMaterial(size);
    types = obj_types;
    center = obj_center;
    center2 = obj_center2;
    radius = obj_r;
    times = time_;
  }
  void addObject(HittableType htype, Point3 centr1,
                 Point3 centr2, float rad, float t0,
                 float t1, MaterialType mtype, Color c,
                 float fuzz, const int index) {
    types[index] = htype;
    center[index] = centr1.e;
    center2[index] = centr2.e;
    radius[index] = rad;
    times[index] = {{t0, t1}};

    mat_ptr->addMaterial(mtype, c.e, fuzz, index);
  }
  void addObject(HittableType htype, Point3 centr1,
                 Point3 centr2, float rad, float t0,
                 float t1, MaterialType mtype, Color c,
                 float fuzz) {
    size++;
    types.push_back(htype);
    center.push_back(centr1.e);
    center2.push_back(centr2.e);
    radius.push_back(rad);
    times.push_back({{t0, t1}});
    mat_ptr->addMaterial(mtype, c.e, fuzz);
  }
  void addObject(Sphere s, MaterialType mtype, Color c,
                 float fuzz) {
    addObject(SPHERE, s.center, Vec3(0.0f), s.radius, 0, 0,
              mtype, c, fuzz);
  }
  void addObject(Sphere s, MaterialType mtype, Color c,
                 float fuzz, const int index) {
    addObject(SPHERE, s.center, Vec3(0.0f), s.radius, 0, 0,
              mtype, c, fuzz, index);
  }
  void addObject(MovingSphere s, MaterialType mtype,
                 Color c, float fuzz) {
    addObject(MOVING_SPHERE, s.center1, s.center2, s.radius,
              s.time0, s.time1, mtype, c, fuzz);
  }
  void addObject(MovingSphere s, MaterialType mtype,
                 Color c, float fuzz, const int index) {
    addObject(MOVING_SPHERE, s.center1, s.center2, s.radius,
              s.time0, s.time1, mtype, c, fuzz, index);
  }

  void to_buffer(cl::Context &context,
                 cl::CommandQueue &queue) {
    std::cout << "hittable size: " << size << std::endl;
    std::cout << "material size: " << mat_ptr->size
              << std::endl;
    make_buffer<cl_int>(cl_hittable_type, context, queue,
                        size, types.data());
    make_buffer<cl_float3>(cl_center2, context, queue, size,
                           center2.data());
    make_buffer<cl_float3>(cl_center, context, queue, size,
                           center.data());
    make_buffer<cl_float2>(cl_times, context, queue, size,
                           times.data());
    make_buffer<cl_float>(cl_radius, context, queue, size,
                          radius.data());

    mat_ptr->to_buffer(context, queue);
  }
};

#endif
