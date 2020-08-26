/*ray traced sphere*/

// several constants
__constant float EPS2 = 0.00003f;
__constant float INF2 = 1e20f;
__constant float PI2 = 3.14159265359f;
__constant int SAMPLE_NB2 = 128;

/*-------- Random number generators --- */

float degree_to_radian(float degree) {
    //
    return degree * PI / 180.0;
}
float rand(float2 co) {
    // random gen
    float a = 12.9898;
    float b = 78.233;
    float c = 43758.5453;
    float dt = dot(co.xy, (float2)(a, b));
    float sn = sin(dt);
    sn *= c;
    return sn - floor(sn);
}
float random_double() {
    // random double
    return rand((float2)(0.0f, 1.0f));
}
float double_random(float mi, float mx) {
    // random double
    return rand((float2)(mi, mx));
}
float3 random_vec() {
    // random vector
    return (float3)(random_double(), random_double(), random_double());
}
float3 vec_random(float mi, float ma) {
    // random vector in given seed
    return (float3)(double_random(mi, ma), double_random(mi, ma),
            double_random(mi, ma));
}
float3 random_in_unit_sphere() {
    // random in unit sphere
    while (true) {
        //
        float3 v = vec_random(-1, 1);
        if (dot(v, v) >= 1) {
            continue;
        }
        return v;
    }
}
float3 random_unit_vector() {
    // unit vector
    float a = double_random(0, 2 * PI);
    float z = double_random(-1, 1);
    float r = sqrt(1 - z * z);
    return (float3)(r * cos(a), r * sin(a), z);
}
float3 random_in_hemisphere(float3 normal) {
    // normal ekseninde dagilan yon
    float3 unit_sphere_dir = random_in_unit_sphere();
    if (dot(unit_sphere_dir, normal) > 0.0) {
        return unit_sphere_dir;
    } else {
        return -1 * unit_sphere_dir;
    }
}
float3 random_in_unit_disk() {
    // lens yakinsamasi için gerekli
    while (true) {
        float3 point = (float3)(double_random(-1, 1), double_random(-1, 1), 0);
        if (dot(point, point) >= 1) {
            continue;
        }
        return point;
    }
}

/*-------------------------------------*/

typedef struct Ray2{
    float3 orig;
    float3 dir;
} Ray2;

float3 at(const Ray* r, float t){
    return r->orig + r->dir * t;
}
float3 facing_normal(const Ray* r, float3 normal){
    return dot(normal, r->dir) < 0.0f? normal: -1.0f * normal;
}
/*

typedef struct Sphere3{
    float radius;
    float3 center;
    float3 color;
    float3 emit;
} Sphere3;

typedef struct HitRecord{
    float3 point;
    float3 normal;
    float distance;
    int scene_id;
} HitRecord;

bool hit_sphere(const Sphere* s,
        const Ray* r, float t_min, float* t_max, HitRecord* rec){

    float3 rtoc = s->center - r->orig;
    float a = dot(r->dir, r->dir);
    float b = 2 * dot(rtoc, r->dir);
    float c = dot(rtoc, rtoc) - s->radius * s->radius;
    float discriminant = b*b - 4*a*c;
    bool isHit = (discriminant > 0);
    HitRecord temp;
    if (isHit){
        float root = sqrt(discriminant);
        float margin = (-b/2 - root) / a; 
        if(margin < *t_max && margin > t_min){
            temp.distance = margin;
            temp.point = at(r, temp.distance);
            float3 normal = (temp.point - s->center) / s->radius;
            temp.normal = facing_normal(r, normal);
            *rec = temp;
            return true;
        }
        margin = (-b/2 + root) / a;
        if(margin < *t_max && margin > t_min){
            temp.distance = margin;
            temp.point = at(r, temp.distance);
            float3 normal = (temp.point - s->center) / s->radius;
            temp.normal = facing_normal(r, normal);
            *rec = temp;
            return true;
        }
    }
    return isHit;
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

bool hit_scene(__constant Sphere* spheres, 
        const Ray* r, HitRecord* rec, float t_min,
        float t_max,
        const int sphere_count){
    HitRecord temp;
    Ray tr = *r;
    float closest_distance = t_max;
    bool anyHit = false;
    for (int i = 0; i < sphere_count; i++){
        Sphere s = spheres[i];
        bool isHit = hit_sphere(&s,
                &tr, t_min,
                &closest_distance,
                &temp);
        if (isHit == true){
            // keep track of the closest distance object
            closest_distance = temp.distance;
            temp.scene_id = i;
            *rec = temp;
            anyHit = true;
        }
    }
    return anyHit;
}

static float rand2(const uint *s0, const uint *s1, const int* gid){
    uint seed = *s0 + *gid;
    uint t = seed ^ (seed << 11);
    uint result = *s1 ^ (*s1 >> 19) ^ (t ^ (t >> 8));
    return (float)result;
}


void get_uv_from_w(float3 w, float3* u, float3* v){
    bool isH = fabs(w.x) > 0.1f;
    float3 yaxis = (float3)(0.0f, 1.0f, 0.0f);
    float3 xaxis = (float3)(1.0f, 0.0f, 0.0f);
    float3 axis = isH?  yaxis: xaxis; 
    *u = normalize(cross(axis, w));
    *v = cross(w, *u);
}

float3 get_next_dir(float3 w,
        float3 v,
        float3 u,
        float r1,
        float r2,
        float r3){
    //
    float3 x = u * cos(r1) * r3;
    float3 y = v * sin(r1) * r3;
    float3 z = w * sqrt(1.0f - r2);
    return normalize(x+y+z);
}

float3 path_trace(__constant Sphere* spheres,
        const Ray* r,
        const int sphere_count,
        const uint* seed0,
        const uint* seed1,
        const int* gid){

    // accumulating variables
    float3 result = (float3)(0.0f, 0.0f, 0.0f);
    float3 current_attenuation = (float3)(1.0f, 1.0f, 1.0f);

    HitRecord rec;
    Ray r_in = *r;
    for (int bounce_nb = 0; bounce_nb < 8; bounce_nb++){
        // bounce_nb is usually called depth

        if (!hit_scene(spheres, &r_in, &rec, 0.001, INF, sphere_count)){
            // we don't have a hit
            float d = 0.5 * (r_in.dir.y + 1.0);
            float3 blend1 = (1-d) * (float3)(1.0f, 1.0f, 1.0f);
            float3 blend2 = d * (float3)(0.5f, 0.7f, 1.0f);
            float3 background = blend1 + blend2;

            current_attenuation *= background;
            return (current_attenuation + result);
        }
        // we have a hit
        Sphere s = spheres[rec.scene_id];
        float3 target = rec.point + random_in_hemisphere(rec.normal);
        r_in.dir = normalize(target - rec.point);
        r_in.orig = rec.point + rec.normal + EPS;

        result += current_attenuation * s.emit;
        current_attenuation *= s.color;
        current_attenuation *= dot(r_in.dir, rec.normal);

    }
    return result;
}

__kernel void render_kernel2(__constant Sphere* spheres,
        const int width,
        const int height,
        const int sphere_count,
        __global float3* out){
    const int gid = get_global_id(0);
    uint xcoord = gid % width;
    uint ycoord = gid / width;

    uint seed0 = (uint)xcoord;
    uint seed1 = (uint)ycoord;

    Ray cam_ray = get_ray(xcoord, ycoord, width, height);

    float3 color = (float3)(0.0f, 0.0f, 0.0f);

    float sample_factor = 1.0f / SAMPLE_NB;

    for(int i = 0; i < SAMPLE_NB; i++){
        color += path_trace(spheres, &cam_ray, 
                sphere_count,
                &seed0,
                &seed1,
                &gid);
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
*/

/*----------------- Code from Sam Lapere -------------*/


__constant float EPSILON = 0.00003f; /* required to compensate for limited float precision */
__constant float PI = 3.14159265359f;
__constant int SAMPLES = 128;

typedef struct Ray{
	float3 origin;
	float3 dir;
} Ray;

typedef struct Sphere{
	float radius;
	float3 pos;
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

Ray createCamRay(const int x_coord, const int y_coord, const int width, const int height){

	float fx = (float)x_coord / (float)width;  /* convert int in range [0 - width] to float in range [0-1] */
	float fy = (float)y_coord / (float)height; /* convert int in range [0 - height] to float in range [0-1] */

	/* calculate aspect ratio */
	float aspect_ratio = (float)(width) / (float)(height);
	float fx2 = (fx - 0.5f) * aspect_ratio;
	float fy2 = fy - 0.5f;

	/* determine position of pixel on screen */
	float3 pixel_pos = (float3)(fx2, -fy2, 0.0f);

	/* create camera ray*/
	Ray ray;
	ray.origin = (float3)(0.0f, 0.1f, 2.0f); /* fixed camera position */
	ray.dir = normalize(pixel_pos - ray.origin); /* vector from camera to pixel on screen */

	return ray;
}

				/* (__global Sphere* sphere, const Ray* ray) */
float intersect_sphere(const Sphere* sphere, const Ray* ray) /* version using local copy of sphere */
{
	float3 rayToCenter = sphere->pos - ray->origin;
	float b = dot(rayToCenter, ray->dir);
	float c = dot(rayToCenter, rayToCenter) - sphere->radius*sphere->radius;
	float disc = b * b - c;

	if (disc < 0.0f) return 0.0f;
	else disc = sqrt(disc);

	if ((b - disc) > EPSILON) return b - disc;
	if ((b + disc) > EPSILON) return b + disc;

	return 0.0f;
}

bool intersect_scene(__constant Sphere* spheres, const Ray* ray, float* t, int* sphere_id, const int sphere_count)
{
	/* initialise t to a very large number, 
	so t will be guaranteed to be smaller
	when a hit with the scene occurs */

	float inf = 1e20f;
	*t = inf;

	/* check if the ray intersects each sphere in the scene */
	for (int i = 0; i < sphere_count; i++)  {
		
		Sphere sphere = spheres[i]; /* create local copy of sphere */
		
		/* float hitdistance = intersect_sphere(&spheres[i], ray); */
		float hitdistance = intersect_sphere(&sphere, ray);
		/* keep track of the closest intersection and hitobject found so far */
		if (hitdistance != 0.0f && hitdistance < *t) {
			*t = hitdistance;
			*sphere_id = i;
		}
	}
	return *t < inf; /* true when ray interesects the scene */
}


/* the path tracing function */
/* computes a path (starting from the camera) with a defined number of bounces, accumulates light/color at each bounce */
/* each ray hitting a surface will be reflected in a random direction (by randomly sampling the hemisphere above the hitpoint) */
/* small optimisation: diffuse ray directions are calculated using cosine weighted importance sampling */

float3 trace(__constant Sphere* spheres, const Ray* camray, const int
sphere_count, float rval1, float rval2){

	Ray ray = *camray;

	float3 accum_color = (float3)(0.0f, 0.0f, 0.0f);
	float3 mask = (float3)(1.0f, 1.0f, 1.0f);

	for (int bounces = 0; bounces < 8; bounces++){

		float t;   /* distance to intersection */
		int hitsphere_id = 0; /* index of intersected sphere */

		/* if ray misses scene, return background colour */
		if (!intersect_scene(spheres, &ray, &t, &hitsphere_id, sphere_count))
			return accum_color += mask * (float3)(0.15f, 0.15f, 0.25f);

		/* else, we've got a hit! Fetch the closest hit sphere */
		Sphere hitsphere = spheres[hitsphere_id]; /* version with local copy of sphere */

		/* compute the hitpoint using the ray equation */
		float3 hitpoint = ray.origin + ray.dir * t;
		
		/* compute the surface normal and flip it if necessary to face the incoming ray */
		float3 normal = normalize(hitpoint - hitsphere.pos); 
		float3 normal_facing = dot(normal, ray.dir) < 0.0f ? normal : normal * (-1.0f);

		/* compute two random numbers to pick a random point on the hemisphere above the hitpoint*/
		float rand1 = 2.0f * PI * get_random(seed0, seed1);
		float rand2 = get_random(seed0, seed1);
		float rand2s = sqrt(rand2);

		/* create a local orthogonal coordinate frame centered at the hitpoint */
		float3 w = normal_facing;
		float3 axis = fabs(w.x) > 0.1f ? (float3)(0.0f, 1.0f, 0.0f) : (float3)(1.0f, 0.0f, 0.0f);
		float3 u = normalize(cross(axis, w));
		float3 v = cross(w, u);

		/* use the coordinte frame and random numbers to compute the next ray direction */
		float3 newdir = normalize(u * cos(rand1)*rand2s + v*sin(rand1)*rand2s + w*sqrt(1.0f - rand2));

		/* add a very small offset to the hitpoint to prevent self intersection */
		ray.origin = hitpoint + normal_facing * EPSILON;
		ray.dir = newdir;

		/* add the colour and light contributions to the accumulated colour */
		accum_color += mask * hitsphere.emission; 

		/* the mask colour picks up surface colours at each bounce */
		mask *= hitsphere.color; 
		
		/* perform cosine-weighted importance sampling for diffuse surfaces*/
		mask *= dot(newdir, normal_facing); 
	}

	return accum_color;
}

__kernel void render_kernel(__constant Sphere* spheres, const int width, const int height, const int sphere_count,
        const float** randoms, const int random_size,
    __global float3* output)
{
	unsigned int work_item_id = get_global_id(0);	/* the unique global id of the work item for the current pixel */
	unsigned int x_coord = work_item_id % width;			/* x-coordinate of the pixel */
	unsigned int y_coord = work_item_id / width;			/* y-coordinate of the pixel */
	
	/* seeds for random number generator */
    float* random_values = randoms[work_item_id];
    float rand_val1 = *random_values[0]
    float rand_val2 = *random_values[1]

	Ray camray = createCamRay(x_coord, y_coord, width, height);

	/* add the light contribution of each sample and average over all samples*/
	float3 finalcolor = (float3)(0.0f, 0.0f, 0.0f);
	float invSamples = 1.0f / SAMPLES;

	for (int i = 0; i < SAMPLES; i++)
		finalcolor += trace(spheres, &camray, sphere_count,
        rand_val1,rand_val2) * invSamples;

	/* store the pixelcolour in the output buffer */
	output[work_item_id] = finalcolor;
}
