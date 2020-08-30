#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <oneweekend/external.hpp>
#include <oneweekend/ray.hpp>
#include <oneweekend/vec3.hpp>

enum MaterialType : cl_int {
  LAMBERTIAN = 1,
  METAL = 2,
  DIELECTRIC = 3,
};

struct Lambertian {
  cl_float3 color;
};
Lambertian makeLambertian(Color color) {
  Lambertian lamb;
  lamb.color = color.e;
  return lamb;
}

struct Metal {
  cl_float3 color;
  cl_float fuzz;
};
Metal makeMetal(Color color, float fuzz) {
  Metal met;
  met.color = color.e;
  met.fuzz = fuzz;
  return met;
}

class SceneMaterial {
public:
  std::vector<cl_int> mat_type;
  std::vector<cl_float3> color;
  std::vector<cl_float> fuzz;
  int size;

  cl::Buffer cl_mat_type;
  cl::Buffer cl_color;
  cl::Buffer cl_fuzz;

public:
  SceneMaterial() : size(0) {}
  SceneMaterial(const int obj_size) : size(obj_size) {
    std::vector<cl_int> mtype(size);
    std::vector<cl_float3> _color(size);
    std::vector<cl_float> _fuzz(size);
    mat_type = mtype;
    color = _color;
    fuzz = _fuzz;
  }
  void addMaterial(Lambertian lamb) {
    mat_type.push_back(LAMBERTIAN);
    fuzz.push_back(0.0f);
    color.push_back(lamb.color);
    size++;
  }
  void addMaterial(Lambertian lamb, const int index) {
    mat_type[index] = LAMBERTIAN;
    fuzz[index] = 0.0f;
    color[index] = lamb.color;
  }
  void addMaterial(Metal met) {
    mat_type.push_back(METAL);
    fuzz.push_back(met.fuzz);
    color.push_back(met.color);
    size++;
  }
  void addMaterial(Metal met, const int index) {
    mat_type[index] = METAL;
    fuzz[index] = met.fuzz;
    color[index] = met.color;
  }
  void to_buffer(cl::Context &context,
                 cl::CommandQueue &queue) {
    std::cout << "material size: " << size << std::endl;
    make_buffer<cl_float>(cl_fuzz, context, queue, size,
                          fuzz.data());
    make_buffer<cl_int>(cl_mat_type, context, queue, size,
                        mat_type.data());
    make_buffer<cl_float3>(cl_color, context, queue, size,
                           color.data());
  }
};

#endif
