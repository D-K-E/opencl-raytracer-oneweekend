#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <oneweekend/external.hpp>
#include <oneweekend/utils.hpp>
#include <oneweekend/vec3.hpp>

Vec3 random_vec() {
  return Vec3(random_float(), random_float(),
              random_float());
}
Vec3 random_vec(float mn, float mx) {
  return Vec3(random_float(mn, mx), random_float(mn, mx),
              random_float(mn, mx));
}
Vec3 random_in_unit_sphere() {
  while (true) {
    Vec3 p = random_vec(-1, 1);
    if (p.length_squared() >= 1.0f) {
      return p;
    }
  }
}

enum RandomType : cl_int {
  RANDOM_FLOAT = 1,
  RANDOM_UNIT_VEC = 2,
  RANDOM_UNIT_SPHERE = 3,
  RANDOM_UNIT_HEMISPHERE = 4,
  RANDOM_UNIT_DISK = 5,
  RANDOM_COSINE_DIRECTION = 6,
  RANDOM_TO_SPHERE = 7,
};

template <class T> class SceneRandom {
public:
  std::vector<T> rand_vals;
  std::vector<cl_int> rand_type;
  int size;
  cl::Buffer cl_rand_vals;
  cl::Buffer cl_rand_type;

public:
  SceneRandom() : size(0) {}
  SceneRandom(const int obj_size) : size(obj_size) {
    std::vector<cl_int> types(size);
    std::vector<T> rvals(size);
    rand_vals = rvals;
    rand_type = types;
  }
  void addRandom(RandomType rtype, T val) {
    size++;
    rand_type.push_back(rtype);
    rand_vals.push_back(val);
  }
  void addRandom(RandomType rtype, T val, const int index) {
    rand_type[index] = rtype;
    rand_vals[index] = val;
  }
  void to_buffer(cl::Context &context,
                 cl::CommandQueue &queue) {
    std::cout << "random size: " << size << std::endl;
    make_buffer<T>(cl_rand_vals, context, queue, size,
                   rand_vals.data());
    make_buffer<cl_int>(cl_rand_type, context, queue, size,
                        rand_type.data());
  }
};

#endif
