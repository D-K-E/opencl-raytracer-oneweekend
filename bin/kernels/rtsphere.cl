/*ray traced sphere*/
/* OpenCL based simple sphere path tracer by Sam Lapere, 2016*/

__constant float EPSILON = 0.00003f; 
__constant float PI = 3.14159265359f;
__constant float INF = 1e20f;
__constant int SAMPLE_NB = 256;


typedef struct Ray{
    float3 orig;
    float3 dir;
} Ray;


typedef struct Sphere{
	float radius;
	float3 center;
	float3 color;
	float3 emission;
} Sphere;

static float get_random(unsigned int *seed0, unsigned int *seed1) {

	/* hash the seeds using bitwise AND operations and bitshifts */
	*seed0 = 36969 * ((*seed0) & 65535) + ((*seed0) >> 16);  
	*seed1 = 18000 * ((*seed1) & 65535) + ((*seed1) >> 16);

	unsigned int ires = ((*seed0) << 16) + (*seed1);

	/* use union struct to convert int to float */
	union {
		float f;
		unsigned int ui;
	} res;

	res.ui = (ires & 0x007fffff) | 0x40000000;  /* bitwise AND, bitwise OR */
	return (res.f - 2.0f) / 2.0f;
}


Ray get_ray(const int x_coord, const int y_coord,
        const int width, const int height){
    float fx = (float)x_coord / (float)width;
    float fy = (float)y_coord / (float)height;

    float aspect_ratio = (float)width / (float)height;
    float fx2 = (fx - 0.5f) * aspect_ratio;
    float fy2 = fy - 0.5f;

    float3 px_pos = (float3)(fx2, -fy2, 0.0f);

    /* create ray */
    Ray r;
    r.orig = (float3)(0.0f, 0.1f, 2.0f);
    r.dir = normalize(px_pos - r.orig);

    return r;
}

float intersect_sphere(const Sphere* sphere,
                       const Ray* ray,
                       float* hit_distance){
	float3 rayToCenter = sphere->center - ray->orig;
	float b = dot(rayToCenter, ray->dir);
	float c = dot(rayToCenter, rayToCenter) - sphere->radius*sphere->radius;
	float disc = b * b - c;

	if (disc < 0.0f){
        return 0.0f;
    }else{ disc = sqrt(disc);}

	if ((b - disc) > EPSILON) {
        return b - disc;
    }
	if ((b + disc) > EPSILON){
        return b + disc;
    }
	return 0.0f;
}
bool hit_sphere(const Sphere* s,
        const Ray* r, float* hit_distance){
    float t_min = 0.001;
    float t_max = INF;

    float3 rtoc = s->center - r->orig;
    float a = dot(r->dir, r->dir);
    float b = 2 * dot(rtoc, r->dir);
    float c = dot(rtoc, rtoc) - s->radius * s->radius;
    float discriminant = b*b - 4*a*c;
    bool isHit = (discriminant > 0);
    if (isHit){
        float root = sqrt(discriminant);
        float margin = (-b/2 - root) / a; 
        if(margin < t_max && margin > t_min){
            *hit_distance = margin;
            return true;
        }
        margin = (-b/2 + root) / a;
        if(margin < t_max && margin > t_min){
            *hit_distance = margin;
            return true;
        }
    }
    return isHit;
}


bool intersect_scene(__constant Sphere* spheres, const Ray* ray, float* t, int* sphere_id, const int sphere_count)
{
	*t = INF;
    bool anyHit = false;

	/* check if the ray intersects each sphere in the scene */
	for (int i = 0; i < sphere_count; i++)  {
		
		Sphere sphere = spheres[i]; /* create local copy of sphere */
		
		float hitdistance = intersect_sphere(&sphere, ray, &hitdistance);
		if (hitdistance != 0.0f && hitdistance < *t) {
			*t = hitdistance;
			*sphere_id = i;
            anyHit = true;
		}
	}
	return anyHit;
}


float3 trace(__constant Sphere* spheres, const Ray* camray, const int sphere_count, const int* seed0, const int* seed1){

	Ray ray = *camray;

	float3 accum_color = (float3)(0.0f, 0.0f, 0.0f);
	float3 mask = (float3)(1.0f, 1.0f, 1.0f);

	for (int bounces = 0; bounces < 8; bounces++){

		float t;
		int hitsphere_id = 0;

		if (!intersect_scene(spheres, &ray, &t, &hitsphere_id, sphere_count))
			return accum_color += mask * (float3)(0.15f, 0.15f, 0.25f);

		Sphere hitsphere = spheres[hitsphere_id];
		float3 hitpoint = ray.orig + ray.dir * t;
		
		float3 normal = normalize(hitpoint - hitsphere.center); 
		float3 normal_facing = dot(normal, ray.dir) < 0.0f ? normal : normal * (-1.0f);

		float rand1 = 2.0f * PI * get_random(seed0, seed1);
		float rand2 = get_random(seed0, seed1);
		float rand2s = sqrt(rand2);

		float3 w = normal_facing;
		float3 axis = fabs(w.x) > 0.1f ? (float3)(0.0f, 1.0f, 0.0f) : (float3)(1.0f, 0.0f, 0.0f);
		float3 u = normalize(cross(axis, w));
		float3 v = cross(w, u);

		float3 newdir = normalize(u * cos(rand1)*rand2s + v*sin(rand1)*rand2s + w*sqrt(1.0f - rand2));

		ray.orig = hitpoint + normal_facing * EPSILON;
		ray.dir = newdir;

		accum_color += mask * hitsphere.emission; 

		mask *= hitsphere.color; 
		
		mask *= dot(newdir, normal_facing); 
	}

	return accum_color;
}

__kernel void render_kernel(__constant Sphere* spheres,
        const int width,
        const int height,
        const int sphere_count,
        __global float3* out){
    const int gid = get_global_id(0);
    uint xcoord = gid % width;
    uint ycoord = gid / width;

    const int seed0 = (uint)xcoord;
    const int seed1 = (uint)ycoord;

    Ray cam_ray = get_ray(xcoord, ycoord, width, height);

    float3 color = (float3)(0.0f, 0.0f, 0.0f);

    float sample_factor = 1.0f / SAMPLE_NB;

    for(int i = 0; i < SAMPLE_NB; i++){
        color += trace(spheres, &cam_ray, 
                sphere_count,
                &seed0,
                &seed1
               );
    }
    color = sqrt(color * sample_factor);
    /*Debugging aid*/
    bool isX = color.x == 0.0f;
    bool isY = color.y == 0.0f;
    bool isZ = color.z == 0.0f;
    if (isX && isY && isZ){
        color = (float3)(1.0f, 0.0f, 0.0f);
    }
    out[gid] = color;
}
