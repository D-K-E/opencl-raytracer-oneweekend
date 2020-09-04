#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <nextweek/external.hpp>
#include <nextweek/ray.hpp>
#include <nextweek/utils.hpp>
#include <nextweek/vec3.hpp>

enum MaterialType : cl_int {
  LAMBERTIAN = 1,
  METAL = 2,
  DIELECTRIC = 3,
};

struct Lambertian {
  cl_float3 color;
  Lambertian(Color c) { color = c.e; }
};

struct Metal {
  cl_float3 color;
  cl_float fuzz;
  Metal(Color c, float f) {
    fuzz = f;
    color = c.e;
  }
};

struct Dielectric {
  cl_float ref_idx;
  Dielectric(float ridx) { ref_idx = ridx; }
};

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
  void addMaterial(Dielectric die) {
    mat_type.push_back(DIELECTRIC);
    fuzz.push_back(die.ref_idx);
    color.push_back(Vec3(0.0f).e);
    size++;
  }
  void addMaterial(Dielectric die, const int index) {
    mat_type[index] = DIELECTRIC;
    fuzz[index] = die.ref_idx;
    color[index] = Vec3(0.0f).e;
  }
  void addMaterial(MaterialType mtype, cl_float3 v,
                   cl_float f) {
    mat_type.push_back(mtype);
    fuzz.push_back(f);
    color.push_back(v);
    size++;
  }
  void addMaterial(MaterialType mtype, cl_float3 v,
                   cl_float f, const int index) {
    mat_type[index] = mtype;
    fuzz[index] = f;
    color[index] = v;
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
