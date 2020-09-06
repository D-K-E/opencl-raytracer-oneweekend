#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <nextweek/external.hpp>
#include <nextweek/ray.hpp>
#include <nextweek/texture.hpp>
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
  std::vector<cl_float> fuzz;
  Texture *texture_ptr;
  int size;

  cl::Buffer cl_mat_type;
  cl::Buffer cl_fuzz;

public:
  SceneMaterial() : size(0) {
    texture_ptr = new Texture(size);
  }
  ~SceneMaterial() { delete texture_ptr; }
  SceneMaterial(const int obj_size) : size(obj_size) {
    std::vector<cl_int> mtype(size);
    std::vector<cl_float> _fuzz(size);
    mat_type = mtype;
    fuzz = _fuzz;
    texture_ptr = new Texture(size);
  }
  void addMaterial(MaterialType mtype, cl_float f,
                   TextureType ttype, Color c) {
    size++;
    mat_type.push_back(mtype);
    fuzz.push_back(f);
    texture_ptr->addTexture(ttype, c);
  }
  void addMaterial(MaterialType mtype, cl_float f,
                   TextureType ttype, Color c,
                   const int index) {
    mat_type[index] = mtype;
    fuzz[index] = f;
    texture_ptr->addTexture(ttype, c, index);
  }
  void to_buffer(cl::Context &context,
                 cl::CommandQueue &queue) {
    std::cout << "material size: " << size << std::endl;
    make_buffer<cl_float>(cl_fuzz, context, queue, size,
                          fuzz.data());
    make_buffer<cl_int>(cl_mat_type, context, queue, size,
                        mat_type.data());
    texture_ptr->to_buffer(context, queue);
  }
};

#endif
