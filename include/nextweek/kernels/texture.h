#ifndef TEXTURE_H
#define TEXTURE_H

#include "macros.h"
#include "ray.h"

typedef enum TextureTypes {
  SOLID_COLOR = 1,
  DOUBLE_COLOR = 2
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

typedef struct Texture {
  TextureTypes type;
  SolidColor sc;
  DoubleColor dc;
} Texture;

Texture makeTexture(TextureTypes ttype, Color c, Color c2) {
  Texture t;
  t.type = ttype;
  if (t.type == SOLID_COLOR) {
    t.sc = makeSolidColor(c);
  } else {
    float min = fmin(c2.x, c2.y);
    min = fmin(c2.z, min);
    if (min > 0.0f) {
      t.dc = makeDoubleColorFromColors(c, c2);
    } else {
      t.dc = makeDoubleColorFromColor(c);
    }
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
