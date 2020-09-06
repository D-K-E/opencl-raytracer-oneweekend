#ifndef KUTILS_H
#define KUTILS_H
#include "macros.h"

// map function
// https://stackoverflow.com/a/1969274/7330813

float map_range(float value, float leftMin, float leftMax,
                float rightMin, float rightMax) {
  // Figure out how 'wide' each range is
  float leftSpan = leftMax - leftMin;
  float rightSpan = rightMax - rightMin;

  // Convert the left range into a 0-1 range
  // (float)
  float valueScaled =
      (float)(value - leftMin) / (float)(leftSpan);

  // Convert the 0 - 1 range into a value in the right
  // range.
  return rightMin + (valueScaled * rightSpan);
}

Vec3 map_range_vec(Vec3 v, float leftMin, float leftMax,
                   float rightMin, float rightMax) {
  // Figure out how 'wide' each range is
  float leftSpan = leftMax - leftMin;
  float rightSpan = rightMax - rightMin;

  // Convert the left range into a 0-1 range
  Vec3 valueScaled = (v - leftMin) / (float)(leftSpan);

  // Convert the 0 - 1 range into a value in the right
  // range.
  return rightMin + (valueScaled * rightSpan);
}

Vec3 random_unit_vector(float3 unit_sphere) {
  float a =
      map_range(unit_sphere.x, -1.0f, 1.0f, 0, 2 * PI);
  float z = unit_sphere.y;
  float r = sqrt(1 - z * z);
  return vec3(r * cos(a), r * sin(a), z);
}
// Vec3 random_in_unit_sphere(float3 seed) {
//  while (true) {
//    Vec3 p = random_vec_minmax(seed, -1.0f, 1.0f);
//  if (dot(p, p) < 1.0f) {
//    return p;
//  }
// }
//}
Vec3 random_in_unit_disk(float3 unit_sphere) {
  Vec3 p = unit_sphere;
  p.z = 0.0f;
  return p;
}
Vec3 random_in_hemisphere(float3 unit_sphere, Vec3 normal) {
  if (dot(unit_sphere, normal) > 0.0) {
    return unit_sphere;
  } else {
    return -unit_sphere;
  }
}

#endif
