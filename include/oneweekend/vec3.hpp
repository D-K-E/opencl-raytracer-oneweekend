#ifndef VEC3_HPP
#define VEC3_HPP

#include <oneweekend/external.hpp>
#include <oneweekend/utils.hpp>

class Vec3 {
public:
  cl_float3 e;

public:
  Vec3() { e = {{0.0f, 0.0f, 0.0f}}; }
  Vec3(float e1, float e2, float e3) { e = {{e1, e2, e3}}; }
  Vec3(float e1) { e = {{e1, e1, e1}}; }
  Vec3(cl_float3 e1) { e = e1; }
  cl_float x() const { return e.s[0]; }
  cl_float y() const { return e.s[1]; }
  cl_float z() const { return e.s[2]; }

  Vec3 operator-() const {
    return Vec3(-e.s[0], -e.s[1], -e.s[2]);
  }
  cl_float operator[](int i) const { return e.s[i]; }
  cl_float &operator[](int i) { return e.s[i]; }

  Vec3 &operator+=(const Vec3 &v) {
    e.s[0] += v.e.s[0];
    e.s[1] += v.e.s[1];
    e.s[2] += v.e.s[2];
    return *this;
  }

  Vec3 &operator*=(const cl_float t) {
    e.s[0] *= t;
    e.s[1] *= t;
    e.s[2] *= t;
    return *this;
  }

  Vec3 &operator/=(const cl_float t) {
    return *this *= 1.0f / t;
  }

  cl_float length() const {
    return std::sqrt(length_squared());
  }

  cl_float length_squared() const {
    return e.s[0] * e.s[0] + e.s[1] * e.s[1] +
           e.s[2] * e.s[2];
  }
};
using Point3 = Vec3;
using Color = Vec3;

inline std::ostream &operator<<(std::ostream &out,
                                const Vec3 &v) {
  return out << v.e.s[0] << ' ' << v.e.s[1] << ' '
             << v.e.s[2];
}

inline Vec3 operator+(const Vec3 &u, const Vec3 &v) {
  return Vec3(u.e.s[0] + v.e.s[0], u.e.s[1] + v.e.s[1],
              u.e.s[2] + v.e.s[2]);
}

inline Vec3 operator-(const Vec3 &u, const Vec3 &v) {
  return Vec3(u.e.s[0] - v.e.s[0], u.e.s[1] - v.e.s[1],
              u.e.s[2] - v.e.s[2]);
}

inline Vec3 operator*(const Vec3 &u, const Vec3 &v) {
  return Vec3(u.e.s[0] * v.e.s[0], u.e.s[1] * v.e.s[1],
              u.e.s[2] * v.e.s[2]);
}

inline Vec3 operator*(cl_float t, const Vec3 &v) {
  return Vec3(t * v.e.s[0], t * v.e.s[1], t * v.e.s[2]);
}

inline Vec3 operator*(const Vec3 &v, cl_float t) {
  return t * v;
}

inline Vec3 operator/(Vec3 v, cl_float t) {
  return (1 / t) * v;
}

inline cl_float dot(const Vec3 &u, const Vec3 &v) {
  return u.e.s[0] * v.e.s[0] + u.e.s[1] * v.e.s[1] +
         u.e.s[2] * v.e.s[2];
}

inline Vec3 cross(const Vec3 &u, const Vec3 &v) {
  return Vec3(u.e.s[1] * v.e.s[2] - u.e.s[2] * v.e.s[1],
              u.e.s[2] * v.e.s[0] - u.e.s[0] * v.e.s[2],
              u.e.s[0] * v.e.s[1] - u.e.s[1] * v.e.s[0]);
}

inline Vec3 unit_vector(Vec3 v) { return v / v.length(); }

#endif
