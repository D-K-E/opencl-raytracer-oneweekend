#ifndef MATERIAL_H
#define MATERIAL_H
// ------------------- Material.h ----------------

#include "texture.h"
//
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

Lambertian makeLambertianFromTexture(Texture t) {
  Lambertian l;
  l.color = t;
  return l;
}

typedef struct Metal {
  Texture color;
  float fuzz;
} Metal;

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
typedef struct HitRecord {
  Point3 p;    // hit point
  Vec3 normal; // hit point normal
  float t;     // distance to hitpoint
  float u;     // coordinate in screen space
  float v;     // coordinate in screen space
  bool front_face;
  Material mat_ptr;
} HitRecord;

void set_front_face(HitRecord *rec, const Ray *r,
                    const Vec3 *normal) {
  bool face = dot(r->direction, *normal) < 0.0f;
  rec->front_face = face;
  rec->normal =
      rec->front_face ? *normal : -1.0f * (*normal);
}

bool scatter_lambert(Lambertian lamb, Ray r, HitRecord rec,
                     Color *attenuation, Ray *r_out,
                     float3 seed,
                     __constant float *images) {
  // Vec3 target = rec.normal + random_unit_vector(seed);
  Vec3 target = random_in_hemisphere(seed, rec.normal);
  *r_out = makeRay(rec.p, target, r.time);
  *attenuation =
      color_value(lamb.color, rec.u, rec.v, &rec.p, images);
  return true;
}

bool scatter_metal(Metal met, Ray r, HitRecord rec,
                   Color *attenuation, Ray *r_out,
                   float3 seed, __constant float *images) {
  Vec3 refdir = reflect(normalize(r.direction), rec.normal);
  Vec3 rand_dir = seed;
  *r_out =
      makeRay(rec.p, refdir + met.fuzz * rand_dir, r.time);
  *attenuation =
      color_value(met.color, rec.u, rec.v, &rec.p, images);
  return (dot(r_out->direction, rec.normal) > 0.0f);
}

bool scatter_dielectric(Dielectric die, Ray r,
                        HitRecord rec, Color *attenuation,
                        Ray *r_out, float3 seed,
                        __constant float *images) {
  *attenuation = v3(1.0f);
  float eeta =
      rec.front_face ? (1.0f / die.ref_idx) : die.ref_idx;
  Vec3 udir = normalize(r.direction);
  float cos_theta = fmin(dot(-udir, rec.normal), 1.0f);
  float sin_theta = sqrt(1.0f - (cos_theta * cos_theta));
  float refl_prob = schlick(cos_theta, eeta);
  float rval = map_range(seed.x, -1.0f, 1.0f, 0.0f, 1.0f);
  if ((eeta * sin_theta > 1.0f) || (rval < refl_prob)) {
    Vec3 refle = reflect(udir, rec.normal);
    *r_out = makeRay(rec.p, refle, r.time);
    return true;
  }
  Vec3 refrec = refract(udir, rec.normal, eeta);
  *r_out = makeRay(rec.p, refrec, r.time);
  return true;
}

bool scatter_material(Material mat, Ray r, HitRecord rec,
                      Color *attenuation, Ray *r_out,
                      Vec3 rand_val,
                      __constant float *images) {
  if (mat.m_type == LAMBERTIAN) {
    return scatter_lambert(mat.lam, r, rec, attenuation,
                           r_out, rand_val, images);
  } else if (mat.m_type == METAL) {
    return scatter_metal(mat.met, r, rec, attenuation,
                         r_out, rand_val, images);
  } else if (mat.m_type == DIELECTRIC) {
    return scatter_dielectric(mat.die, r, rec, attenuation,
                              r_out, rand_val, images);
  }
  return false;
}

// ------------------- end Material.h ------------

#endif
