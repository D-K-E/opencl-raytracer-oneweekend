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

// random functions

// https://software.intel.com/content/www/us/en/develop/articles/parallel-noise-and-random-functions-for-opencl-kernels.html
// Wang Hash based RNG
//  Has at least 20 separate cycles,
//  shortest cycle is <
//  7500 long.
//  But it yields random looking 2D noise
//  when fed OpenCL
//  work item IDs,
//  and that short cycle should only be
//  hit for one work
//  item in about 500K.
unsigned int ParallelRNG(unsigned int x) {
  unsigned int value = x;

  value = (value ^ 61) ^ (value >> 16);
  value *= 9;
  value ^= value << 4;
  value *= 0x27d4eb2d;
  value ^= value >> 15;

  return value;
}

unsigned int ParallelRNG2(unsigned int x, unsigned int y) {
  unsigned int value = ParallelRNG(x);

  value = ParallelRNG(y ^ value);

  return value;
}

constant uint default_perm[256] = {
    151, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194,
    233, 7,   225, 99,  37,  8,   240, 21,  10,  23,  190,
    6,   148, 247, 120, 234, 75,  0,   26,  160, 137, 35,
    11,  32,  57,  177, 33,  88,  237, 149, 56,  87,  174,
    20,  125, 136, 171, 134, 139, 48,  27,  166, 77,  146,
    158, 231, 83,  111, 229, 122, 60,  211, 133, 55,  46,
    245, 40,  244, 102, 143, 54,  65,  25,  63,  161, 1,
    216, 80,  73,  18,  169, 200, 196, 135, 130, 116, 188,
    159, 86,  164, 100, 109, 198, 173, 186, 250, 124, 123,
    5,   202, 38,  147, 118, 126, 255, 82,  85,  212, 207,
    206, 59,  189, 28,  42,  223, 183, 170, 213, 119, 248,
    152, 2,   44,  154, 163, 70,  221, 43,  172, 9,   129,
    22,  39,  253, 19,  98,  108, 110, 79,  113, 224, 232,
    178, 97,  228, 251, 34,  242, 193, 238, 210, 144, 12,
    191, 179, 162, 241, 81,  51,  107, 49,  192, 214, 31,
    181, 199, 106, 157, 184, 84,  204, 176, 115, 121, 50,
    138, 236, 205, 93,  222, 114, 67,  29,  24,  72,  243,
    141, 128, 195, 78,  66,  140, 36,  103, 30,  227, 47,
    16,  58,  69,  17,  209, 76,  132, 187, 45,  127, 197,
    62,  94,  252, 153, 101, 155, 167, 219, 182, 3,   64,
    52,  217, 215, 61,  168, 68,  175, 74,  185, 112, 104,
    218, 165, 246, 4,   150, 208, 254, 142, 71,  230, 220,
    105, 92,  145, 235, 249, 14,  41,  239, 156, 180, 226,
    89,  203, 117};

constant float2 grads2d[16] = {
    {-0.195090322f, -0.98078528f},
    {-0.555570233f, -0.831469612f},
    {-0.831469612f, -0.555570233f},
    {-0.98078528f, -0.195090322f},
    {-0.98078528f, 0.195090322f},
    {-0.831469612f, 0.555570233f},
    {-0.555570233f, 0.831469612f},
    {-0.195090322f, 0.98078528f},
    {0.195090322f, 0.98078528f},
    {0.555570233f, 0.831469612f},
    {0.831469612f, 0.555570233f},
    {0.98078528f, 0.195090322f},
    {0.98078528f, -0.195090322f},
    {0.831469612f, -0.555570233f},
    {0.555570233f, -0.831469612f},
    {0.195090322f, -0.98078528f}};

float weight_poly5(float weight) {
  return weight * weight * weight *
         (weight * (weight * 6 - 15) +
          10); // Perlin's improved interp equation
}

#define WEIGHT(w) weight_poly5(w)

float hash_grad_dot2(uint hash, float2 xy) // 2d gradient
                                           // look up and
                                           // dot product
                                           // with vector
{
  uint indx = hash & 0x0f;

  // look up a unit vector gradient
  float2 grad2 = grads2d[indx];

  return dot(xy, grad2);
}
#define interp(w, b, c) mix((b), (c), (w))

float Noise_2d(float x, float y) {
  float X = floor(x); // lower grid coordinates
  float Y = floor(y);

  float2 vxy;
  vxy.x = x - X; // vector from lower grid coordinates
  vxy.y = y - Y;

  float2 vXy = vxy;
  vXy.x -= 1.0f;
  float2 vxY = vxy;
  vxY.y -= 1.0f;
  float2 vXY = vXy;
  vXY.y -= 1.0f;

  int ux = (int)(X);
  int uy = (int)(Y);
  int uX = ux + 1;
  int uY = uy + 1;

  uint px =
      default_perm[ux &
                   0x0FF]; // generate permutation grads
  uint pX = default_perm[uX & 0x0FF];

  uint pxy = default_perm[(px + uy) & 0x0FF];
  uint pXy = default_perm[(pX + uy) & 0x0FF];
  uint pxY = default_perm[(px + uY) & 0x0FF];
  uint pXY = default_perm[(pX + uY) & 0x0FF];

  float gxy = hash_grad_dot2(pxy, vxy);
  float gXy = hash_grad_dot2(pXy, vXy);
  float gxY = hash_grad_dot2(pxY, vxY);
  float gXY = hash_grad_dot2(pXY, vXY);

  float wx = WEIGHT(vxy.x);
  float wy = WEIGHT(vxy.y);

  return interp(wy, interp(wx, gxy, gXy),
                interp(wx, gxY, gXY));
}

float random_float_minmax(float2 seed, float min,
                          float max) {
  return map_range(Noise_2d(seed.x, seed.y), -1.0f, 1.0f,
                   min, max);
}

float random_float(float2 seed) {

  return random_float_minmax(seed, 0.0f, 1.0f);
}

Vec3 random_vec_minmax(float2 seed, float mn, float mx) {
  return vec3(random_float_minmax(seed, mn, mx),
              random_float_minmax(seed, mn, mx),
              random_float_minmax(seed, mn, mx));
}

Vec3 random_vec(float2 seed) {
  return random_vec_minmax(seed, 0.0f, 1.0f);
}
Vec3 random_unit_vector(float2 seed) {
  float a = random_float_minmax(seed, 0, 2 * PI);
  float z = random_float_minmax(seed, -1, 1);
  float r = sqrt(1 - z * z);
  return vec3(r * cos(a), r * sin(a), z);
}
Vec3 random_in_unit_sphere(float2 seed) {
  while (true) {
    Vec3 p = random_vec_minmax(seed, -1.0f, 1.0f);
    if (dot(p, p) < 1.0f) {
      return p;
    }
  }
}
Vec3 random_in_unit_disk(float2 seed) {
  while (true) {
    Vec3 p = random_vec_minmax(seed, -1.0f, 1.0f);
    p.z = 0.0f;
    if (dot(p, p) < 1.0f) {
      return p;
    }
  }
}
Vec3 random_in_hemisphere(float2 seed, Vec3 normal) {
  Vec3 in_unit_sphere = random_in_unit_sphere(seed);
  if (dot(in_unit_sphere, normal) > 0.0) {
    return in_unit_sphere;
  } else {
    return -in_unit_sphere;
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
} Ray;

Ray makeRay(Point3 orig, Vec3 dir) {
  Ray r;
  r.origin = orig;
  r.direction = dir;
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
} Camera;

Camera makeCamera(Vec3 origin, Vec3 lleft, Vec3 h,
                  Vec3 vert, float lens_radius, Vec3 w,
                  Vec3 u, Vec3 v) {
  Camera cam;
  cam.origin = origin;
  cam.lower_left_corner = lleft;
  cam.horizontal = h;
  cam.vertical = vert;
  cam.w = w;
  cam.u = u;
  cam.v = v;
  cam.lens_radius = lens_radius;
  return cam;
}

Ray get_ray(Camera cam, float s, float t, float2 seed) {
  Vec3 rd = cam.lens_radius * random_in_unit_disk(seed);
  Vec3 offset = cam.u * rd.x + cam.v * rd.y;
  return makeRay(cam.origin + offset,
                 cam.lower_left_corner +
                     s * cam.horizontal + t * cam.vertical -
                     cam.origin - offset);
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
                     float2 seed) {
  Vec3 target = random_in_hemisphere(seed, rec.normal);
  *r_out = makeRay(rec.p, target);
  *attenuation = lamb.color;
  return true;
}

bool scatter_metal(Metal met, Ray r, HitRecord rec,
                   Color *attenuation, Ray *r_out,
                   float2 seed) {
  Vec3 refdir = reflect(normalize(r.direction), rec.normal);
  Vec3 rand_dir = random_in_unit_sphere(seed);
  *r_out = makeRay(rec.p, refdir + met.fuzz * rand_dir);
  *attenuation = met.color;
  return (dot(r_out->direction, rec.normal) > 0.0f);
}

bool scatter_dielectric(Dielectric die, Ray r,
                        HitRecord rec, Color *attenuation,
                        Ray *r_out, float2 seed) {
  *attenuation = v3(1.0f);
  float eeta = rec.front_face == true ? (1.0f / die.ref_idx)
                                      : die.ref_idx;
  Vec3 udir = normalize(r.direction);
  float cos_theta = fmin(dot(-udir, rec.normal), 1.0f);
  float sin_theta = sqrt(1.0f - (cos_theta * cos_theta));
  if (eeta * sin_theta > 1.0f) {
    Vec3 refle = reflect(udir, rec.normal);
    *r_out = makeRay(rec.p, refle);
    return true;
  }
  float refl_prob = schlick(cos_theta, eeta);
  float r_val = random_float(seed);
  if (r_val < refl_prob) {
    Vec3 refle = reflect(udir, rec.normal);
    *r_out = makeRay(rec.p, refle);
    return true;
  }
  Vec3 refrec = refract(udir, rec.normal, eeta);
  *r_out = makeRay(rec.p, refrec);
  return true;
}

bool scatter_material(Material mat, Ray r, HitRecord rec,
                      Color *attenuation, Ray *r_out,
                      float2 rand_val) {
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
typedef struct MovingSphere {
  float radius;
  Point3 center;
  Point3 center2;
  float time0;
  float time1;
} MovingSphere;

MovingSphere makeMSphere(Point3 cent1, Point3 cent2,
                         float rad, float t0, float t1) {
  MovingSphere s;
  s.center = cent1;
  s.center2 = cent2;
  s.radius = rad;
  s.time0 = t0;
  s.time1 = t1;
  return s;
}

// Vec3 get_moving_center(const MovingSphere *s, float time)
// {
//  return s->center +
//         ((time - s->time0) / (s->time1 - s->time0)) *
//             (s->center2 - s->center);
//}

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

// bool hit_moving_sphere(const MovingSphere *s, const Ray
// *r,
//                     float t_min, float t_max,
//                     HitRecord *rec) {
// Vec3 s_center = get_moving_center(s, r->time);
// float3 oc = r->origin - s_center;
// float a = dot(r->direction, r->direction);
// float hb = dot(oc, r->direction);
// float b = 2.0f * hb;
// float c = dot(oc, oc) - (s->radius * s->radius);
// float disc = (hb * hb) - (a * c);

// if (disc > 0.0f) {
//  float root = sqrt(disc);

//  float margin = (-hb - root) / a;
//  if (margin < t_max && margin > t_min) {
//    rec->t = margin;
//    rec->p = at(*r, rec->t);
//    Vec3 normal = (rec->p - s_center) / s->radius;
//    set_front_face(rec, r, &normal);
//    return true;
//  }

//  margin = (-hb + root) / a;
//  if (margin < t_max && margin > t_min) {
//    rec->t = margin;
//    rec->p = at(*r, rec->t);
//    float3 normal = (rec->p - s_center) / s->radius;
//    set_front_face(rec, r, &normal);
//    return true;
//  }
//}
// return false;
//}

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
  sh.msph = makeMSphere(c1, c2, r, t0, t1);

  sh.mat = makeMaterial(material_type, material_color,
                        material_fuzz);
  return sh;
}

bool hit(SceneHittable h, const Ray *r, float t_min,
         float t_max, HitRecord *rec) {
  if (h.h_type == SPHERE) {
    bool isHit = hit_sphere(&h.sph, r, t_min, t_max, rec);
    if (isHit) {
      rec->mat_ptr = h.mat;
    }
    return isHit;
  }
  return false;
}

bool hit_scene(__constant Vec3 *centers,
               __constant float *radiuss,
               __constant Vec3 *material_colors,
               __constant float *material_fuzzs,
               __constant int *material_types, const Ray *r,
               float t_min, float t_max, int sphere_count,
               HitRecord *rec) {
  HitRecord temp;
  bool anyHit = false;
  float closest = t_max;
  for (int i = 0; i < sphere_count; i++) {
    SceneHittable h = makeSphereHittable(
        centers[i], radiuss[i], material_colors[i],
        material_fuzzs[i], material_types[i]);
    bool isHit = hit(h, r, t_min, closest, &temp);
    if (isHit) {
      anyHit = true;
      closest = temp.t;
      *rec = temp;
    }
  }
  return anyHit;
}

Color trace(Ray *r, __constant int *htypes,
            __constant float3 *centers,
            __constant float *radiuss,
            __constant Vec3 *material_colors,
            __constant float *material_fuzzs,
            __constant int *material_types,
            __constant float *random_vals, int start_index,
            int sphere_count, int depth) {
  int end_index = start_index + depth;
  Ray r_in = *r;
  Color accumulated = v3(1.0f);
  for (int i = start_index; i < end_index; i++) {
    HitRecord rec;
    if (hit_scene(centers, radiuss, material_colors,
                  material_fuzzs, material_types, &r_in,
                  EPS_TMIN, INF, sphere_count, &rec)) {
      Color attenuation;
      Ray r_out;
      float2 seed =
          (float2)(random_vals[i], random_vals[i + 1]);
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

void get_random_uv(float *ru, float *rv, int i,
                   __constant float *rand_arr) {
  *ru = rand_arr[i];
  *rv = rand_arr[i + 1];
}

__kernel void ray_color(
    __global float3 *out, __constant int *htypes,
    __constant float3 *centers, __constant float *radiuss,
    int sphere_count, __constant float *seeds,
    __constant int *seedtypes,
    __constant int *material_types,
    __constant Vec3 *material_colors,
    __constant float *material_fuzzs, float3 origin,
    float3 lower_left_corner, float3 horizontal,
    float3 vertical, float3 w, float3 u, float3 v,
    float lens_radius, int depth, int imwidth, int imheight,
    int samples_per_pixel, float aspect_ratio) {
  const int gid = get_global_id(0);
  int xc = gid % imwidth;
  int yc = gid / imwidth;

  // take random value from random array
  const int spp = samples_per_pixel * 2;
  const int start_index = gid * spp;
  const int end_index = start_index + spp;
  Color rcolor = v3(0.0f);
  for (int i = start_index; i < end_index; i += 2) {
    float ru, rv;
    get_random_uv(&ru, &rv, i, seeds);

    float u = ((float)xc + ru) / (float)(imwidth - 1);
    float v = ((float)yc + rv) / (float)(imheight - 1);
    // camera
    int depth_start_index = (int)(i / 2);
    Camera cam =
        makeCamera(origin, lower_left_corner, horizontal,
                   vertical, lens_radius, w, u, v);

    Ray cam_ray = get_ray(cam, u, v, (float2)(ru, rv));
    rcolor += trace(&cam_ray, htypes, centers, radiuss,
                    material_colors, material_fuzzs,
                    material_types, seeds,
                    depth_start_index, sphere_count, depth);
  }

  out[gid] = sqrt(rcolor / samples_per_pixel);
}
