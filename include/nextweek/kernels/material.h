#ifndef MATERIAL_H
#define MATERIAL_H

#include "macros.h"
#include "ray.h"
#include "texture.h"

Vec3 reflect(Vec3 vec, Vec3 normal) {
  return vec - 2 * dot(vec, normal) * normal;
}

Vec3 refract(Vec3 uv, Vec3 n, float etai_over_etat) {
  float cos_theta = dot(-uv, n);
  Vec3 r_out = etai_over_etat * (uv + cos_theta * n);
  Vec3 r_out_parallel =
      -sqrt(fabs(1.0f - dot(r_out, r_out))) * n;
  return r_out + r_out_parallel;
}

float schlick(float cosine, float ref_idx) {
  // from ray tracing in one weekend
  float r0 = (1 - ref_idx) / (1 + ref_idx);
  r0 = r0 * r0;
  return r0 + (1 - r0) * pow((1 - cosine), 5);
}

typedef struct Lambertian { Texture color; } Lambertian;

Lambertian makeLambertianFromColor(Color c) {
  Lambertian l;
  l.color = makeTexture(SOLID_COLOR, c);
  return l;
}
Lambertian makeLambertianFromTexture(Texture t) {
  Lambertian l;
  l.color = t;
  return l;
}

typedef struct Metal {
  Texture color;
  float fuzz;
} Metal;

Metal makeMetalFromColor(Color c, float f) {
  Metal m;
  m.color = makeTexture(SOLID_COLOR, c);
  m.fuzz = f > 1.0f ? 1.0f : f;
  return m;
}
Metal makeMetalFromTexture(Texture t, float f) {
  Metal m;
  m.color = t;
  m.fuzz = f > 1.0f ? 1.0f : f;
  return m;
}

typedef struct Dielectric { float ref_idx; } Dielectric;

Dielectric makeDielectric(float rid) {
  Dielectric d;
  d.ref_idx = rid;
  return d;
}

typedef enum MaterialTypes {
  LAMBERTIAN = 1,
  METAL = 2,
  DIELECTRIC = 3
} MaterialTypes;

typedef struct Material {
  MaterialTypes m_type;
  Lambertian lam;
  Metal met;
  Dielectric die;
} Material;

Material makeMaterial(MaterialTypes mtype, Texture c,
                      float f) {
  Material mat;
  if (mtype == LAMBERTIAN) {
    mat.m_type = LAMBERTIAN;
    mat.lam = makeLambertianFromTexture(c);
  } else if (mtype == METAL) {
    mat.m_type = METAL;
    mat.met = makeMetalFromTexture(c, f);
  } else if (mtype == DIELECTRIC) {
    mat.m_type = DIELECTRIC;
    mat.die = makeDielectric(f);
  }
  return mat;
}

#endif
