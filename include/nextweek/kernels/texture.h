#ifndef TEXTURE_H
#define TEXTURE_H

#include "macros.h"
#include "ray.h"

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
} ImageColor;

ImageColor makeImageColor(int index, int w, int h) {
  ImageColor im;
  im.array_index = index;
  im.image_width = w;
  im.image_height = h;
  return im;
}

Color color_value_image_color(
    __read_only image2d_array_t images, ImageColor im,
    float u, float v, const Vec3 *p) {
  u = clamp(u, 0.0f, 1.0f);
  v = clamp(v, 0.0f, 1.0f);
  int image_width = get_image_width(images);
  int image_height = get_image_height(images);
  int pixel_x = (int)(u * image_width);
  int pixel_y = (int)(v * image_height);
  if (pixel_x >= image_width) {
    pixel_x = image_width - 1;
  }
  if (pixel_y >= image_height) {
    pixel_y = image_height - 1;
  }
  int4 index = (int4)(pixel_x, pixel_y, im.array_index, 0);
  float4 color = read_imagef(images, index);
  color /= 255.0f;
  return vec3(color.x, color.y, color.z);
}

typedef struct Texture {
  TextureTypes type;
  SolidColor sc;
  DoubleColor dc;
  ImageColor ic;
} Texture;

Texture makeTexture(TextureTypes ttype, Color c) {
  Texture t;
  t.type = ttype;
  if (t.type == SOLID_COLOR) {
    t.sc = makeSolidColor(c);
  } else {
    t.dc = makeDoubleColorFromColor(c);
  }
  return t;
}

Color color_value(const Texture t, float u, float v,
                  const Point3 *p) {
  if (t.type == SOLID_COLOR) {
    return color_value_solid_color(&t.sc, u, v, p);
  } else {
    return color_value_double_color(&t.dc, u, v, p);
  }
}

#endif
