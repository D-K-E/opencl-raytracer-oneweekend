#ifndef TEXTURE_H
#define TEXTURE_H

#include "image.h"
//

typedef enum TextureTypes {
  SOLID_COLOR = 1,
  DOUBLE_COLOR = 2,
  IMAGE_COLOR = 3
} TextureTypes;

typedef struct SolidColor { Color color; } SolidColor;

SolidColor makeSolidColor(Color c) {
  SolidColor sc;
  sc.color = c;
  return sc;
}

Color color_value_solid_color(const SolidColor *s, float u,
                              float v, const Point3 *p) {
  return s->color;
}

typedef struct DoubleColor {
  SolidColor left;
  SolidColor right;
} DoubleColor;

DoubleColor makeDoubleColorFromColors(Color c1, Color c2) {
  DoubleColor d;
  float min = fmin(c2.x, c2.y);
  min = fmin(c2.z, min);

  d.left = makeSolidColor(c1);
  d.right = makeSolidColor(c2);
  return d;
}
DoubleColor makeDoubleColorFromColor(Color c1) {
  DoubleColor d;
  d.left = makeSolidColor(c1);
  d.right = makeSolidColor(1.0f - c1);
  return d;
}

DoubleColor makeDoubleColorFromSolidColors(SolidColor c1,
                                           SolidColor c2) {
  DoubleColor d;
  d.left = c1;
  d.right = c2;
  return d;
}

Color color_value_double_color(const DoubleColor *d,
                               float u, float v,
                               const Point3 *p) {
  float sines =
      sin(10 * p->x) * sin(10 * p->y) * sin(10 * p->z);
  if (sines < 0.0f) {
    return color_value_solid_color(&d->left, u, v, p);
  } else {
    return color_value_solid_color(&d->right, u, v, p);
  }
}

typedef struct ImageColor {
  int array_index; // index in image array
  ImageInfo info;
} ImageColor;

ImageColor makeImageColor(int index, ImageInfo info) {
  ImageColor im;
  im.array_index = index;
  im.info = info;
  return im;
}

Color color_value_image_color(__constant float *images,
                              const ImageColor *im, float u,
                              float v, const Vec3 *p) {
  u = clamp(u, 0.0f, 1.0f);
  v = 1 - clamp(v, 0.0f, 1.0f); // 1 - flips y coordinate
  int image_width = im->info.width;
  int image_height = im->info.height;
  int pixel_xi = (int)(u * image_width);
  int pixel_yj = (int)(v * image_height);
  if (pixel_xi >= image_width) {
    pixel_xi = image_width - 1;
  }
  if (pixel_yj >= image_height) {
    pixel_yj = image_height - 1;
  }
  Color c = read_pixel(images, pixel_xi, pixel_yj,
                       im->array_index, im->info);
  return c;
}

typedef struct Texture {
  TextureTypes type;
  SolidColor sc;
  DoubleColor dc;
  ImageColor ic;
} Texture;

Texture makeTexture(TextureTypes ttype, Color c,
                    ImageInfo info) {
  Texture t;
  t.type = ttype;
  if (t.type == SOLID_COLOR) {
    t.sc = makeSolidColor(c);
  } else if (t.type == DOUBLE_COLOR) {
    t.dc = makeDoubleColorFromColor(c);
  } else {
    t.ic = makeImageColor((int)c.x, info);
  }
  return t;
}

Color color_value(const Texture t, float u, float v,
                  const Point3 *p,
                  __constant float *images) {
  if (t.type == SOLID_COLOR) {
    return color_value_solid_color(&t.sc, u, v, p);
  } else if (t.type == DOUBLE_COLOR) {
    return color_value_double_color(&t.dc, u, v, p);
  } else {
    return color_value_image_color(images, &t.ic, u, v, p);
  }
}

#endif
