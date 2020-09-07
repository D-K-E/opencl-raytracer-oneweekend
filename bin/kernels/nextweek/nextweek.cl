// oneweekend kernel
// ------------------- most basics ---------------
#include "hittable.h"
// utilities function

//
// -------------------- Sphere --------------------
Color trace(Ray *r,
            // ---------- hittables -------------------
            __constant int *htypes,
            __constant Point3 *centers,
            __constant Point3 *centers2,
            __constant float *radiuss,
            __constant float2 *times,
            // ---------- materials ------------------
            __constant float *material_fuzzs,
            __constant int *material_types,
            // --------- texture ----------------------
            __constant int *texture_types,
            __constant Color *texture_colors,
            // --------- random -----------------------
            __constant float3 *seeds,
            //
            int start_index,  // ray start index for seeds
            int sphere_count, //
            int depth,
            // ---------- images ---------------------
            ImageInfo info, __constant float *images) {
  int end_index = start_index + depth;
  Ray r_in = *r;
  Color accumulated = v3(1.0f);
  for (int i = start_index; i < end_index; i++) {
    HitRecord rec;
    if (hit_scene(
            // ----------- hittables -------------
            htypes,   //
            centers,  //
            centers2, //
            radiuss,  //
            times,    //
            // ----------- materials -------------
            material_types, //
            material_fuzzs, //
            // ----------- textures --------------
            texture_types,  //
            texture_colors, //
            // ----------- images ----------------
            info, images,
            //
            &r_in, EPS_TMIN, INF, sphere_count, &rec)) {
      Color attenuation;
      Ray r_out;
      float3 seed = seeds[i];

      if (scatter_material(rec.mat_ptr, r_in, rec,
                           &attenuation, &r_out, seed,
                           images)) {
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
ray_color(__global Color *out,
          // ---------- hittables -------------------
          __constant int *htypes,      //
          __constant Point3 *centers,  //
          __constant Point3 *centers2, //
          __constant float2 *times,    //
          __constant float *radiuss,   //
          int sphere_count,            //
          // --------- random -----------------------
          __constant Vec3 *seeds, //
          // --------- material ---------------------
          __constant int *material_types,   //
          __constant float *material_fuzzs, //
          // --------- texture ----------------------
          __constant int *texture_types,    //
          __constant Color *texture_colors, //
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
          float aspect_ratio,    //
          // -------- image -----------------------
          __constant float *imarr, int bytes_per_line,
          int bytes_per_pixel, int imarr_width,
          int imarr_height, int nb_images) {
  const int gid = get_global_id(0);
  int xc = gid % imwidth;
  int yc = gid / imwidth;

  ImageInfo info = makeImageInfo(
      imarr_width, imarr_height, bytes_per_line,
      bytes_per_pixel, nb_images);

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
    rcolor +=
        trace(&cam_ray,
              // ---------- hittables -------------------
              htypes, centers, centers2, radiuss, times,
              // ---------- materials -------------------
              material_fuzzs, material_types,
              // ---------- textures --------------------
              texture_types, texture_colors,
              // --------- randoms ----------------------
              seeds,
              // --------- others -----------------------
              depth_start_index, // depth count
              sphere_count, depth,
              // --------- images -----------------------
              info, imarr);
  }

  out[gid] = sqrt(rcolor / samples_per_pixel);
}
