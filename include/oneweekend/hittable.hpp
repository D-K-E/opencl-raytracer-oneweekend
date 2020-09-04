#ifndef HITTABLE_HPP
#define HITTABLE_HPP
#include <oneweekend/external.hpp>
#include <oneweekend/material.hpp>
#include <oneweekend/sphere.hpp>
#include <oneweekend/utils.hpp>

enum HittableType : cl_int { SPHERE = 1 };

class SceneHittables {
public:
  std::vector<cl_int> types;
  std::vector<cl_float3> center;
  std::vector<cl_float> radius;
  SceneMaterial *mat_ptr;

  int size;
  cl::Buffer cl_hittable_type;
  cl::Buffer cl_center;
  cl::Buffer cl_radius;
  SceneHittables() : size(0) {
    mat_ptr = new SceneMaterial();
  }
  ~SceneHittables() { delete mat_ptr; }

  SceneHittables(int obj_count) : size(obj_count) {
    std::vector<cl_int> obj_types(size);
    std::vector<cl_float3> obj_center(size);
    std::vector<cl_float> obj_r(size);
    mat_ptr = new SceneMaterial(size);
    types = obj_types;
    center = obj_center;
    radius = obj_r;
  }
  void addObject(Sphere s, MaterialType mtype, Color c,
                 float fuzz) {
    size++;
    types.push_back(SPHERE);
    center.push_back(s.center);
    radius.push_back(s.radius);
    mat_ptr->addMaterial(mtype, c.e, fuzz);
  }
  void addObject(Sphere s, MaterialType mtype, Color c,
                 float fuzz, const int index) {
    types[index] = SPHERE;
    center[index] = s.center;
    radius[index] = s.radius;

    mat_ptr->addMaterial(mtype, c.e, fuzz, index);
  }
  void to_buffer(cl::Context &context,
                 cl::CommandQueue &queue) {
    std::cout << "hittable size: " << size << std::endl;
    std::cout << "material size: " << mat_ptr->size
              << std::endl;
    make_buffer<cl_int>(cl_hittable_type, context, queue,
                        size, types.data());
    make_buffer<cl_float3>(cl_center, context, queue, size,
                           center.data());
    make_buffer<cl_float>(cl_radius, context, queue, size,
                          radius.data());
    mat_ptr->to_buffer(context, queue);
  }
};

#endif
