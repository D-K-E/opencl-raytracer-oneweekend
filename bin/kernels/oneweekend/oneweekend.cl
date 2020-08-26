// gradient kernel

// ------------------------ Ray ------------------

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
} HitRecord;

// -------------------- Sphere --------------------

typedef struct Sphere{
    float3 center;
    float radius;
} Sphere;



// ----------------- end Sphere -------------------


bool hit_sphere(float3 center, float radius, const Ray* r){
    float3 oc = r->origin - center;
    float a = dot(r->direction, r->direction);
    float b = 2.0f * dot(oc, r->direction);
    float c = dot(oc, oc) - (radius * radius);
    bool disc = (b*b - 4.0f * a * c) > 0.0f;
    return disc;
}

float3 trace(const Ray* r){
    float3 center = (float3)(0.0f, 0.0f, -1.0f);
    float ra = 0.5f;
    if (hit_sphere(center, ra, r)){
        return (float3)(1.0f, 0.3f, 0.8f);
    }
    float3 udir = normalize(r->direction);
    float d = 0.5f * (udir.y + 1.0f);
    float3 b1 = (1.0f - d) * (float3)(1.0f, 1.0f, 1.0f);
    float3 b2 = d * (float3)(0.3f, 0.7f, 1.0f);
    return b1 + b2;
}

__kernel void ray_color(__global float3* out, 
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
    float3 rcolor = trace(&cam_ray);

    out[gid] = rcolor;
}
