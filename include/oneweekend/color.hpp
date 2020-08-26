#ifndef COLOR_HPP
#define COLOR_HPP

#include <oneweekend/external.hpp>

inline float clamp(float x, float min, float max) {
  bool xmin = x < min;
  bool xmax = x > max;
  if (xmin)
    return min;
  if (xmax)
    return max;
  return x;
}

inline int cast_color(float x) { return int(clamp(x, 0, 1) * 255); }

inline cl_int3 cast_color(cl_float4 incolor) {
  cl_int3 color = {{cast_color((float)incolor.s[0]),
                    cast_color((float)incolor.s[1]),
                    cast_color((float)incolor.s[2])}};
  return color;
}
void save_to_ppm(const char *ppm_path, int imwidth, int imheight,
                 cl_float4 *cpu_out) {
  std::ofstream ppm;
  ppm.open(ppm_path);

  ppm << "P3" << std::endl;
  ppm << imwidth << " " << imheight << std::endl;
  ppm << "255" << std::endl;
  for (int i = 0; i < imwidth * imheight; i++) {
    cl_int3 color = cast_color(cpu_out[i]);
    int ir = color.s[0];
    int ig = color.s[1];
    int ib = color.s[2];
    ppm << ir << " " << ig << " " << ib << std::endl;
  }

  ppm.close();
}

#endif
