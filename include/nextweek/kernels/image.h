#ifndef IMAGE_H
#define IMAGE_H
//
#include "camera.h"

typedef struct ImageInfo {
  int width;
  int height;
  int bytes_per_line;
  int bytes_per_pixel;
  int nb_images;
  int size;
} ImageInfo;

ImageInfo makeImageInfo(int w, int h, int pl, int pp,
                        int nbi) {
  ImageInfo info;
  info.width = w;
  info.height = h;
  info.bytes_per_line = pl;
  info.bytes_per_pixel = pp;
  info.nb_images = nbi;
  info.size = info.bytes_per_line * info.height;
  return info;
}

void put_pixel(__constant float *imarr, Color *c,
               int pixel_index, int channel) {
  if (channel == 0) {
    c->x = imarr[pixel_index];
  } else if (channel == 1) {
    c->y = imarr[pixel_index];
  } else if (channel == 2) {
    c->z = imarr[pixel_index];
  }
}

Color read_pixel(__constant float *imarr, int xi, int yj,
                 int image_index, ImageInfo info) {
  int start_index = info.size * image_index;
  int row = yj * info.bytes_per_line;
  int col = xi * info.bytes_per_pixel;
  int pixel_start = row + col + start_index;
  Color pixel;
  for (int i = 0; i < info.bytes_per_pixel; i++) {
    put_pixel(imarr, &pixel, i + pixel_start, i);
  }
  return pixel;
  // return vec3(1.0f, 0.5f, 0.3f);
}

#endif
