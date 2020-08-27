// gradient kernel

// ------------------------ Ray ------------------
__constant float EPS_TMIN = 0.001;
__constant float INF = 1.0f / 0.0f;

typedef struct Ray{
    float3 origin;
    float3 direction;
} Ray;

Ray makeRay(float3 orig, float3 dir){
    Ray r;
    r.origin = orig;
    r.direction = dir;
    return r;
}

float3 at(Ray r, float dist){
    return r.origin + r.direction * dist;
}

// -------------------- end Ray --------------------

typedef struct HitRecord{
    float3 p; // hit point
    float3 normal; // hit point normal
    float t; // distance to hitpoint
    bool front_face;
} HitRecord;

void set_front_face(HitRecord* rec, const Ray*r, float3 normal){
    rec->front_face = dot(r->direction, normal) < 0.0f;
    rec->normal = rec->front_face? normal: -1.0f * normal;
}

// -------------------- Sphere --------------------

typedef struct Sphere{
    float radius;
    float3 center;
} Sphere;

bool hit_sphere(const Sphere* s, const Ray* r, 
                float t_min, float t_max, HitRecord* rec){
    float3 oc = r->origin - s->center;
    float a = dot(r->direction, r->direction);
    float hb = dot(oc, r->direction);
    float b = 2.0f * hb;
    float c = dot(oc, oc) - (s->radius * s->radius);
    float disc = (hb * hb) - (a * c);
    HitRecord temp;

    if (disc > 0.0f){
        float root = sqrt(disc);

        float margin = (-hb - root) / a;
        if (margin < t_max && margin > t_min){
            temp.t = margin;
            temp.p = at(*r, temp.t);
            float3 normal = (temp.p - s->center) / s->radius;
            set_front_face(&temp,r, normal);
            *rec = temp;
            return true;
        }

        margin = (-hb + root) / a;
        if (margin < t_max && margin > t_min){
            temp.t = margin;
            temp.p = at(*r, temp.t);
            float3 normal = (temp.p - s->center) / s->radius;
            set_front_face(&temp,r, normal);
            *rec = temp;
            return true;
        }
    }
    return false;
}

// ----------------- end Sphere -------------------

bool hit_spheres(__constant Sphere* spheres,
                 const Ray* r,
                 float t_min,
                 float t_max, 
                 int sphere_count,
                 HitRecord* rec){
    HitRecord temp;
    bool anyHit = false;
    float closest = t_max;
    for (int i=0; i < sphere_count; i++){
        Sphere s = spheres[i];
        bool isHit = hit_sphere(&s, r, t_min, closest,
                                &temp);
        if(isHit){
            anyHit = true;
            closest = temp.t;
            *rec = temp;
        }
    }
    return anyHit;
}


float3 trace(const Ray* r,
             __constant Sphere* spheres, int sphere_count){
    HitRecord rec;
    if (hit_spheres(spheres, r, EPS_TMIN, INF, sphere_count,&rec)){
        return 0.5f * (rec.normal + (float3)(1.0f, 1.0f, 1.0f));
    }
    float3 udir = normalize(r->direction);
    float d = 0.5f * (udir.y + 1.0f);
    float3 b1 = (1.0f - d) * (float3)(1.0f, 1.0f, 1.0f);
    float3 b2 = d * (float3)(0.3f, 0.7f, 1.0f);
    return b1 + b2;
}

__kernel void ray_color(__global float3* out, 
                        __constant Sphere* spheres,
                        int sphere_count,
                        int imwidth,
                        int imheight,
                        float aspect_ratio
                        ){
    const int gid = get_global_id(0);
    int xc = gid % imwidth;
    int yc = gid / imwidth;

    float fx = (float)xc / (float)imwidth;
    float fy = (float)yc / (float)imheight;

    // camera

    float view_height = 2.0f;
    float view_width = aspect_ratio * view_height;

    float focal_length = 1.0f;

    float3 cam_origin = (float3)(0.0f, 0.0f, 0.0f);
    float3 horizontal = (float3)(view_width, 0.0f, 0.0f);
    float3 vertical = (float3)(0.0f, view_height, 0.0f);
    float3 lleft = cam_origin - horizontal / 2 - vertical / 2 - (float3)(0.0f,0.0f, focal_length);
    float3 rdir = lleft + fx*horizontal + fy*vertical - cam_origin;
    Ray cam_ray = makeRay(cam_origin, rdir);
    float3 rcolor = trace(&cam_ray, spheres, sphere_count);

    out[gid] = rcolor;
}
