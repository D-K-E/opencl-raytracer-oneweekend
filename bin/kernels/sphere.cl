/* sphere kernel */

struct Ray{
    float3 orig;
    float3 dir;
};

struct Sphere{
    float3 center;
    float radius;
    float3 color;
};

bool intersect_sphere(const struct Sphere* sphere,
        const struct Ray* r, float* t){

    float3 rtoc = sphere->center - r->orig;
    float b = dot(rtoc, r->dir);
    float c = dot(rtoc, rtoc) - sphere->radius * sphere->radius;
    float discriminant = b*b - c;

    if (discriminant < 0.0f){ return false;
    }else{
        *t = b - sqrt(discriminant);
    }

    if (*t < 0.0){
        *t = b + sqrt(discriminant);
        if (*t < 0.0f) return false;
    }
    return true;
}

struct Ray get_ray(const int x_coord, const int y_coord,
        const int width, const int height){
    float fx = (float)x_coord / (float)width;
    float fy = (float)y_coord / (float)height;

    float aspect_ratio = (float)width / (float)height;
    float fx2 = (fx - 0.5f) * aspect_ratio;
    float fy2 = fy - 0.5f;

    float3 px_pos = (float3)(fx2, -fy2, 0.0f);

    /* create ray */
    struct Ray r;
    r.orig = (float3)(0.0f, 0.0f, 40.0f);
    r.dir = normalize(px_pos - r.orig);

    return r;
}

__kernel void render_sphere_with_normal(__global float3* output,
        int width, int height){
    const int workId = get_global_id(0);
    int x_coord = workId % width;
    int y_coord = workId / width;

    struct Ray r = get_ray(x_coord, y_coord, width, height);

    /* create sphere */
    struct Sphere s1;
    s1.radius = 0.4f;
    s1.center = (float3)(0.0f, 0.0f, 3.1f);
    s1.color = (float3)(0.2f, 0.8f, 0.0f); 

    float t = 1e20;
    bool hasIntersected = intersect_sphere(&s1, &r, &t);
    float fx = (float)x_coord / (float)width;
    float fy = (float)y_coord / (float)height;
    float3 background = (float3)(0.8, 0.7, 0.4);
    if( hasIntersected == true){
        float3 hit_point = r.orig + r.dir * t;
        float3 hit_normal = normalize(hit_point - s1.center);
        float3 cos_theta = dot(hit_normal, r.dir) * -1.0;
        output[workId] = s1.color * cos_theta;
    }else{
        output[workId] = background;
    }
}

__kernel void render_normal(__global float3* output,
        int width, int height){
    const int workId = get_global_id(0);
    int x_coord = workId % width;
    int y_coord = workId / width;

    struct Ray r = get_ray(x_coord, y_coord, width, height);

    /* create sphere */
    struct Sphere s1;
    s1.radius = 0.4f;
    s1.center = (float3)(0.0f, 0.0f, 3.1f);
    s1.color = (float3)(0.2f, 0.8f, 0.0f); 

    float t = 1e20;
    bool hasIntersected = intersect_sphere(&s1, &r, &t);
    float fx = (float)x_coord / (float)width;
    float fy = (float)y_coord / (float)height;
    float3 background = (float3)(0.8, 0.9, 0.8);
    if( hasIntersected == true){
        float3 hit_point = r.orig + r.dir * t;
        float3 hit_normal = normalize(hit_point - s1.center);
        output[workId] = hit_normal * 0.5f + (float3)(0.5f, 0.5f, 0.5f);
    }else{
        output[workId] = background;
    }
}



__kernel void render_sphere(__global float3* output,
        int width, int height){
    const int workId = get_global_id(0);
    int x_coord = workId % width;
    int y_coord = workId / width;

    struct Ray r = get_ray(x_coord, y_coord, width, height);

    /* create sphere */
    struct Sphere s1;
    s1.radius = 0.4f;
    s1.center = (float3)(0.0f, 0.0f, 3.1f);
    s1.color = (float3)(0.2f, 0.8f, 0.0f); 

    float t = 1e20;
    bool hasIntersected = intersect_sphere(&s1, &r, &t);
    float fx = (float)x_coord / (float)width;
    float fy = (float)y_coord / (float)height;
    float3 background = (float3)(0.8, 0.7, 0.4);
    if( hasIntersected == true){
        output[workId] = s1.color;
    }else{
        output[workId] = background;
    }
}

