// oneweekend kernel

__constant float EPS_TMIN = 0.001;
__constant float INF = 1.0f / 0.0f;
#define Vec3 float3
#define Point3 float3
#define Color float3
#define vec3(a, b, c) (float3)(a, b, c)
#define v3(a) vec3(a, a, a)

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

float3 at(Ray r, float dist) {
  return r.origin + r.direction * dist;
}

// -------------------- end Ray --------------------

// -------------------- Camera ---------------------

typedef struct Camera {
  float3 origin;
  float3 lower_left_corner;
  float3 horizontal;
  float3 vertical;
} Camera;

Ray get_ray(__constant Camera *cam, float u, float v) {
  Ray r;
  r.origin = cam->origin;
  Vec3 corigin = cam->origin;
  Vec3 ch = cam->horizontal;
  Vec3 cv = cam->vertical;
  Vec3 cll = cam->lower_left_corner;
  Vec3 rdir = cll + u * ch + v * cv - corigin;
  r.direction = rdir;
  return r;
}

// ------------------ end Camera -------------------

typedef struct HitRecord {
  Point3 p;    // hit point
  Vec3 normal; // hit point normal
  float t;     // distance to hitpoint
  bool front_face;
} HitRecord;

void set_front_face(HitRecord *rec, const Ray *r,
                    Vec3 normal) {
  rec->front_face = dot(r->direction, normal) < 0.0f;
  rec->normal = rec->front_face ? normal : -1.0f * normal;
}

// -------------------- Sphere --------------------

typedef struct Sphere {
  float radius;
  float3 center;
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
  HitRecord temp;

  if (disc > 0.0f) {
    float root = sqrt(disc);

    float margin = (-hb - root) / a;
    if (margin < t_max && margin > t_min) {
      temp.t = margin;
      temp.p = at(*r, temp.t);
      Vec3 normal = (temp.p - s->center) / s->radius;
      set_front_face(&temp, r, normal);
      *rec = temp;
      return true;
    }

    margin = (-hb + root) / a;
    if (margin < t_max && margin > t_min) {
      temp.t = margin;
      temp.p = at(*r, temp.t);
      float3 normal = (temp.p - s->center) / s->radius;
      set_front_face(&temp, r, normal);
      *rec = temp;
      return true;
    }
  }
  return false;
}

// ----------------- end Sphere -------------------

typedef enum HittableType { SPHERE = 1 } HittableType;

typedef struct SceneHittable {
  HittableType h_type;
  Sphere sph;
} SceneHittable;

SceneHittable makeSphereHittable(Vec3 c, float r) {
  SceneHittable sh;
  sh.h_type = SPHERE;
  sh.sph = makeSphere(r, c);
  return sh;
}

bool hit(SceneHittable h, const Ray *r, float t_min,
         float t_max, HitRecord *rec) {
  if (h.h_type == SPHERE) {
    return hit_sphere(&h.sph, r, t_min, t_max, rec);
  }
  return false;
}

bool hit_scene(__constant float3 *centers,
               __constant float *radiuss, const Ray *r,
               float t_min, float t_max, int sphere_count,
               HitRecord *rec) {
  HitRecord temp;
  bool anyHit = false;
  float closest = t_max;
  for (int i = 0; i < sphere_count; i++) {
    SceneHittable h =
        makeSphereHittable(centers[i], radiuss[i]);
    bool isHit = hit(h, r, t_min, closest, &temp);
    if (isHit) {
      anyHit = true;
      closest = temp.t;
      *rec = temp;
    }
  }
  return anyHit;
}

Color trace(Ray *r, __constant float3 *centers,
            __constant float *radiuss,
            __constant Vec3 *random_vals, int start_index,
            int sphere_count, int depth) {
  int end_index = start_index + depth;
  Ray r_in = *r;
  float attenuation = 1.0f;
  for (int i = start_index; i < end_index; i++) {
    HitRecord rec;
    if (hit_scene(centers, radiuss, &r_in, EPS_TMIN, INF,
                  sphere_count, &rec)) {
      Vec3 target = rec.p + rec.normal + random_vals[i];
      r_in = makeRay(rec.p, target - rec.p);
      attenuation *= 0.5f;
    } else {
      Vec3 udir = normalize(r_in.direction);
      float d = 0.5f * (udir.y + 1.0f);
      Vec3 b1 = (1.0f - d) * v3(1.0f);
      Vec3 b2 = d * vec3(0.3f, 0.7f, 1.0f);
      return (b1 + b2) * attenuation;
    }
  }
  return v3(0.0f);
}

void get_random_uv(float *ru, float *rv, int i,
                   __constant float *rand_arr) {
  *ru = rand_arr[i];
  *rv = rand_arr[i + 1];
}

__kernel void
ray_color(__global float3 *out, __constant float3 *centers,
          __constant float *radiuss, __constant Camera *cam,
          __constant float *sample_random_vals,
          __constant int *sample_random_types,
          __constant Vec3 *depth_random_vals,
          __constant int *depth_random_types,
          int sphere_count, int depth, int imwidth,
          int imheight, int samples_per_pixel,
          float aspect_ratio) {
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
    get_random_uv(&ru, &rv, i, sample_random_vals);

    float u = ((float)xc + ru) / (float)(imwidth - 1);
    float v = ((float)yc + rv) / (float)(imheight - 1);
    // camera
    int depth_start_index = (int)(i / 2);

    Ray cam_ray = get_ray(cam, u, v);
    rcolor +=
        trace(&cam_ray, centers, radiuss, depth_random_vals,
              depth_start_index, sphere_count, depth);
  }

  out[gid] = (rcolor / samples_per_pixel);
}
