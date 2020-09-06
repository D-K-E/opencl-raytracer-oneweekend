#ifndef SCATTERING_H
#define SCATTERING_H

#include "hitrecord.h"
#include "macros.h"
#include "material.h"
#include "ray.h"

bool scatter_lambert(Lambertian lamb, Ray r, HitRecord rec,
                     Color *attenuation, Ray *r_out,
                     float3 seed) {
  // Vec3 target = rec.normal + random_unit_vector(seed);
  Vec3 target = random_in_hemisphere(seed, rec.normal);
  *r_out = makeRay(rec.p, target, r.time);
  *attenuation =
      color_value(lamb.color, rec.u, rec.v, &rec.p);
  return true;
}

bool scatter_metal(Metal met, Ray r, HitRecord rec,
                   Color *attenuation, Ray *r_out,
                   float3 seed) {
  Vec3 refdir = reflect(normalize(r.direction), rec.normal);
  Vec3 rand_dir = seed;
  *r_out =
      makeRay(rec.p, refdir + met.fuzz * rand_dir, r.time);
  *attenuation =
      color_value(met.color, rec.u, rec.v, &rec.p);
  return (dot(r_out->direction, rec.normal) > 0.0f);
}

bool scatter_dielectric(Dielectric die, Ray r,
                        HitRecord rec, Color *attenuation,
                        Ray *r_out, float3 seed) {
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
                      float3 rand_val) {
  if (mat.m_type == LAMBERTIAN) {
    return scatter_lambert(mat.lam, r, rec, attenuation,
                           r_out, rand_val);
  } else if (mat.m_type == METAL) {
    return scatter_metal(mat.met, r, rec, attenuation,
                         r_out, rand_val);
  } else if (mat.m_type == DIELECTRIC) {
    return scatter_dielectric(mat.die, r, rec, attenuation,
                              r_out, rand_val);
  }
  return false;
}

#endif
