#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <nextweek/external.hpp>
#include <nextweek/vec3.hpp>

enum TextureType : cl_int {
  SOLID_COLOR = 1,
  DOUBLE_COLOR = 2,
  IMAGE_COLOR = 3
};

class SolidColor {
public:
  Color color;
  SolidColor() : color(Vec3(0.0f)) {}
  SolidColor(Color c) : color(c) {}
};

class DoubleColor {
public:
  SolidColor left;
  SolidColor right;

public:
  DoubleColor() {}
  DoubleColor(SolidColor l) : left(l) {
    right = SolidColor(1.0f - left.color);
  }
  DoubleColor(Color c1)
      : left(SolidColor(c1)), right(SolidColor(1.0f - c1)) {
  }
};

struct ImageColor {
  cl_float3 image_index; // x,y in pixel z in array
};

class Texture {
public:
  std::vector<cl_int> types;
  std::vector<cl_float3> colors;

  cl::Buffer cl_color;
  cl::Buffer cl_texture_type;
  int size;

  Texture() : size(0) {}
  Texture(int obj_size) : size(obj_size) {
    std::vector<cl_int> ttypes(size);
    std::vector<cl_float3> cs(size);
    types = ttypes;
    colors = cs;
  }
  void addTexture(TextureType ttype, Color c) {
    size++;
    types.push_back(ttype);
    colors.push_back(c.e);
  }
  void addTexture(TextureType ttype, Color c,
                  const int index) {
    types[index] = ttype;
    colors[index] = c.e;
  }
  void to_buffer(cl::Context &context,
                 cl::CommandQueue &queue) {
    std::cout << "texture size: " << size << std::endl;
    make_buffer<cl_int>(cl_texture_type, context, queue,
                        size, types.data());
    make_buffer<cl_float3>(cl_color, context, queue, size,
                           colors.data());
  }
};

#endif
