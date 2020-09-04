#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <nextweek/external.hpp>
#include <nextweek/utils.hpp>
#include <nextweek/vec3.hpp>

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
    if (p.length_squared() < 1.0f) {
      return p;
    }
  }
}

enum RandomType : cl_int {
  RANDOM_INT = 1,
  RANDOM_FLOAT = 2,
  RANDOM_UINT = 3,
  RANDOM_UINT3 = 4
};

template <class T> class SceneRandom {
public:
  std::vector<T> rand_vals;
  int size;
  cl::Buffer cl_rand_vals;

public:
  SceneRandom() : size(0) {}
  SceneRandom(const int obj_size) : size(obj_size) {
    std::vector<T> rvals(size);
    rand_vals = rvals;
  }
  void addRandom(T val) {
    size++;
    rand_vals.push_back(val);
  }
  void addRandom(T val, const int index) {
    rand_vals[index] = val;
  }
  void to_buffer(cl::Context &context,
                 cl::CommandQueue &queue) {
    std::cout << "random size: " << size << std::endl;
    make_buffer<T>(cl_rand_vals, context, queue, size,
                   rand_vals.data());
  }
};

#endif
