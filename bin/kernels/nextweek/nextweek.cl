// oneweekend kernel

__constant float EPS_TMIN = 0.001;
__constant float INF = 1.0f / 0.0f;
__constant float FMAX = 1e20;
__constant float PI = 3.1415926535897f;
#define Vec3 float3
#define Point3 float3
#define Color float3
#define vec3(a, b, c) (float3)(a, b, c)
#define v3(a) vec3(a, a, a)

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

// utilities function
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

// ------------------------ Ray ------------------

typedef struct Ray {
  float3 origin;
  float3 direction;
  float time;
} Ray;

Ray makeRay(Point3 orig, Vec3 dir, float tm) {
  Ray r;
  r.origin = orig;
  r.direction = dir;
  r.time = tm;
  return r;
}

Point3 at(Ray r, float dist) {
  return r.origin + r.direction * dist;
}

// -------------------- end Ray --------------------

// -------------------- Camera ---------------------

typedef struct Camera {
  Point3 origin;
  Vec3 lower_left_corner;
  float3 horizontal;
  float3 vertical;
  float3 w, v, u;
  float lens_radius;
  float time0, time1;
} Camera;

Camera makeCamera(Vec3 origin, Vec3 lleft, Vec3 h,
                  Vec3 vert, float lens_radius, Vec3 w,
                  Vec3 u, Vec3 v, float t0, float t1) {
  Camera cam;
  cam.origin = origin;
  cam.lower_left_corner = lleft;
  cam.horizontal = h;
  cam.vertical = vert;
  cam.w = w;
  cam.u = u;
  cam.v = v;
  cam.lens_radius = lens_radius;
  cam.time0 = t0;
  cam.time1 = t1;
  return cam;
}

Ray get_ray(Camera cam, float s, float t, float3 seed) {
  Vec3 rd = cam.lens_radius * random_in_unit_disk(seed);
  float tm =
      map_range(seed.x, -1.0f, 1.0f, cam.time0, cam.time1);
  Vec3 offset = cam.u * rd.x + cam.v * rd.y;
  return makeRay(cam.origin + offset,
                 cam.lower_left_corner +
                     s * cam.horizontal + t * cam.vertical -
                     cam.origin - offset,
                 tm);
}

// ------------------ end Camera -------------------

typedef struct Lambertian { Color color; } Lambertian;
Lambertian makeLambertian(Color c) {
  Lambertian l;
  l.color = c;
  return l;
}

typedef struct Metal {
  Color color;
  float fuzz;
} Metal;

Metal makeMetal(Color c, float f) {
  Metal m;
  m.color = c;
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

Material makeMaterial(MaterialTypes mtype, Color c,
                      float f) {
  Material mat;
  if (mtype == LAMBERTIAN) {
    mat.m_type = LAMBERTIAN;
    mat.lam = makeLambertian(c);
  } else if (mtype == METAL) {
    mat.m_type = METAL;
    mat.met = makeMetal(c, f);
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
                     float3 seed) {
  // Vec3 target = rec.normal + random_unit_vector(seed);
  Vec3 target = random_in_hemisphere(seed, rec.normal);
  *r_out = makeRay(rec.p, target, r.time);
  *attenuation = lamb.color;
  return true;
}

bool scatter_metal(Metal met, Ray r, HitRecord rec,
                   Color *attenuation, Ray *r_out,
                   float3 seed) {
  Vec3 refdir = reflect(normalize(r.direction), rec.normal);
  Vec3 rand_dir = seed;
  *r_out =
      makeRay(rec.p, refdir + met.fuzz * rand_dir, r.time);
  *attenuation = met.color;
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

// -------------------- Sphere --------------------

typedef struct Sphere {
  float radius;
  Point3 center;
} Sphere;

Sphere makeSphere(float r, Vec3 c) {
  Sphere s;
  s.radius = r;
  s.center = c;
  return s;
}

bool hit_sphere(const Sphere *s, const Ray *r, float t_min,
                float t_max, HitRecord *rec) {
  float3 oc = r->origin - s->center;
  float a = dot(r->direction, r->direction);
  float hb = dot(oc, r->direction);
  float b = 2.0f * hb;
  float c = dot(oc, oc) - (s->radius * s->radius);
  float disc = (hb * hb) - (a * c);

  if (disc > 0.0f) {
    float root = sqrt(disc);

    float margin = (-hb - root) / a;
    if (margin < t_max && margin > t_min) {
      rec->t = margin;
      rec->p = at(*r, rec->t);
      Vec3 normal = (rec->p - s->center) / s->radius;
      set_front_face(rec, r, &normal);
      return true;
    }

    margin = (-hb + root) / a;
    if (margin < t_max && margin > t_min) {
      rec->t = margin;
      rec->p = at(*r, rec->t);
      float3 normal = (rec->p - s->center) / s->radius;
      set_front_face(rec, r, &normal);
      return true;
    }
  }
  return false;
}

typedef struct MovingSphere {
  float radius;
  Point3 center1;
  Point3 center2;
  float time0;
  float time1;
} MovingSphere;

MovingSphere makeMovingSphere(float r, Point3 c1, Point3 c2,
                              float t0, float t1) {
  MovingSphere s;
  s.radius = r;
  s.center1 = c1;
  s.center2 = c2;
  s.time0 = t0;
  s.time1 = t1;
  return s;
}

Point3 get_center_ms(MovingSphere *s, float time) {
  Vec3 center =
      ((time - s->time0) / (s->time1 - s->time0)) *
      (s->center2 - s->center1);
  return s->center1 + center;
}

bool hit_moving_sphere(const MovingSphere *s, const Ray *r,
                       float t_min, float t_max,
                       HitRecord *rec) {
  Vec3 center = get_center_ms(s, r->time);
  float3 oc = r->origin - center;
  float a = dot(r->direction, r->direction);
  float hb = dot(oc, r->direction);
  float b = 2.0f * hb;
  float c = dot(oc, oc) - (s->radius * s->radius);
  float disc = (hb * hb) - (a * c);

  if (disc > 0.0f) {
    float root = sqrt(disc);

    float margin = (-hb - root) / a;
    if (margin < t_max && margin > t_min) {
      rec->t = margin;
      rec->p = at(*r, rec->t);
      Vec3 normal = (rec->p - center) / s->radius;
      set_front_face(rec, r, &normal);
      return true;
    }

    margin = (-hb + root) / a;
    if (margin < t_max && margin > t_min) {
      rec->t = margin;
      rec->p = at(*r, rec->t);
      float3 normal = (rec->p - center) / s->radius;
      set_front_face(rec, r, &normal);
      return true;
    }
  }
  return false;
}

// ----------------- end Sphere -------------------

typedef enum HittableType {
  SPHERE = 1,
  MOVING_SPHERE = 2
} HittableType;

typedef struct SceneHittable {
  HittableType h_type;
  Sphere sph;
  MovingSphere msph;
  Material mat;
} SceneHittable;

SceneHittable makeSphereHittable(Point3 c, float r,
                                 Color material_color,
                                 float material_fuzz,
                                 int material_type) {
  SceneHittable sh;
  sh.h_type = SPHERE;
  sh.sph = makeSphere(r, c);
  sh.mat = makeMaterial(material_type, material_color,
                        material_fuzz);

  return sh;
}
SceneHittable makeMovingSphereHittable(Point3 c1, Point3 c2,
                                       float r, float t0,
                                       float t1,
                                       Color material_color,
                                       float material_fuzz,
                                       int material_type) {
  SceneHittable sh;
  sh.h_type = MOVING_SPHERE;
  sh.msph = makeMovingSphere(r, c1, c2, t0, t1);
  sh.mat = makeMaterial(material_type, material_color,
                        material_fuzz);

  return sh;
}
SceneHittable makeSHittable(int hittable_type, Point3 c1,
                            Point3 c2, float r, float t0,
                            float t1, Color material_color,
                            float material_fuzz,
                            int material_type) {
  if (hittable_type == MOVING_SPHERE) {
    return makeMovingSphereHittable(
        c1, c2, r, t0, t1, material_color, material_fuzz,
        material_type);
  } else {
    return makeSphereHittable(c1, r, material_color,
                              material_fuzz, material_type);
  }
}

bool hit(SceneHittable h, const Ray *r, float t_min,
         float t_max, HitRecord *rec) {
  bool isHit = false;
  if (h.h_type == SPHERE) {
    isHit = hit_sphere(&h.sph, r, t_min, t_max, rec);
  } else {
    isHit =
        hit_moving_sphere(&h.msph, r, t_min, t_max, rec);
  }
  if (isHit) {
    rec->mat_ptr = h.mat;
  }
  return isHit;
}

bool hit_scene(__constant int *htypes,
               __constant Vec3 *centers,
               __constant Vec3 *centers2,
               __constant float *radiuss,
               __constant float2 *times,
               __constant Vec3 *material_colors,
               __constant float *material_fuzzs,
               __constant int *material_types, const Ray *r,
               float t_min, float t_max, int sphere_count,
               HitRecord *rec) {
  HitRecord temp;
  bool anyHit = false;
  float closest = t_max;
  for (int i = 0; i < sphere_count; i++) {
    SceneHittable h = makeSHittable(
        htypes[i], centers[i], centers2[i], radiuss[i],
        times[i].x, times[i].y, material_colors[i],
        material_fuzzs[i], material_types[i]);
    if (hit(h, r, t_min, closest, &temp)) {
      anyHit = true;
      closest = temp.t;
      *rec = temp;
    }
  }
  return anyHit;
}

Color trace(Ray *r,
            // ---------- hittables -------------------
            __constant int *htypes,
            __constant float3 *centers,
            __constant float3 *centers2,
            __constant float *radiuss,
            __constant float2 *times,
            // ---------- materials ------------------
            __constant Vec3 *material_colors,
            __constant float *material_fuzzs,
            __constant int *material_types,
            // --------- random -----------------------
            __constant float3 *seeds,
            //
            int start_index,  // ray start index for seeds
            int sphere_count, //
            int depth) {
  int end_index = start_index + depth;
  Ray r_in = *r;
  Color accumulated = v3(1.0f);
  for (int i = start_index; i < end_index; i++) {
    HitRecord rec;
    if (hit_scene(htypes, centers, centers2, radiuss, times,
                  material_colors, material_fuzzs,
                  material_types, &r_in, EPS_TMIN, INF,
                  sphere_count, &rec)) {
      Color attenuation;
      Ray r_out;
      float3 seed = seeds[i];

      if (scatter_material(rec.mat_ptr, r_in, rec,
                           &attenuation, &r_out, seed)) {
        r_in = r_out;
        accumulated *= attenuation;
      } else {
        return v3(0.0f);
      }
    } else {
      Vec3 udir = normalize(r_in.direction);
      float d = 0.5f * (udir.y + 1.0f);
      Vec3 b1 = (1.0f - d) * v3(1.0f);
      Vec3 b2 = d * vec3(0.3f, 0.7f, 1.0f);
      return (b1 + b2) * accumulated;
    }
  }
  return v3(0.0f);
}

void get_random_uv(float *ru, float *rv, float3 seed) {

  *ru = map_range((seed.x + seed.y) / 2.0f, -1.0f, 1.0f,
                  0.0f, 1.0f);
  *rv = map_range((seed.y + seed.z) / 2.0f, -1.0f, 1.0f,
                  0.0f, 1.0f);
}

__kernel void
ray_color(__global float3 *out,
          // ---------- hittables -------------------
          __constant int *htypes,      //
          __constant float3 *centers,  //
          __constant float3 *centers2, //
          __constant float2 *times,    //
          __constant float *radiuss,   //
          int sphere_count,            //
          // --------- random -----------------------
          __constant float3 *seeds, //
          // --------- material ---------------------
          __constant int *material_types,   //
          __constant Vec3 *material_colors, //
          __constant float *material_fuzzs, //
          // --------- camera -----------------------
          float3 origin,                //
          float3 lower_left_corner,     //
          float3 horizontal,            //
          float3 vertical,              //
          float3 w, float3 u, float3 v, //
          float lens_radius,            //
          float cam_t0, float cam_t1,   //
          // -------- other ------------------------
          int depth,             //
          int imwidth,           //
          int imheight,          //
          int samples_per_pixel, //
          float aspect_ratio) {
  const int gid = get_global_id(0);
  int xc = gid % imwidth;
  int yc = gid / imwidth;

  // take random value from random array
  const int spp = samples_per_pixel;
  const int start_index = gid * spp;
  const int end_index = start_index + spp;
  Color rcolor = v3(0.0f);

  for (int i = start_index; i < end_index; i++) {
    float ru, rv;

    get_random_uv(&ru, &rv, seeds[i]);

    float u = ((float)xc + ru) / (float)(imwidth - 1);
    float v = ((float)yc + rv) / (float)(imheight - 1);
    // camera
    int depth_start_index = (int)(i / 2);
    Camera cam = makeCamera(
        origin, lower_left_corner, horizontal, vertical,
        lens_radius, w, u, v, cam_t0, cam_t1);

    Ray cam_ray = get_ray(cam, u, v, seeds[i]);
    rcolor += trace(
        &cam_ray,
        // ---------- hittables -------------------
        htypes, centers, centers2, radiuss, times,
        // ---------- materials -------------------
        material_colors, material_fuzzs, material_types,
        // --------- randoms ----------------------
        seeds,
        // --------- others -----------------------
        depth_start_index, // depth count
        sphere_count, depth);
  }

  out[gid] = sqrt(rcolor / samples_per_pixel);
}
