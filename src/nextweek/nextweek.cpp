// main func opencl raytracing in oneweekend
#include <nextweek/camera.hpp>
#include <nextweek/color.hpp>
#include <nextweek/external.hpp>
#include <nextweek/hittable.hpp>
#include <nextweek/material.hpp>
#include <nextweek/random.hpp>
#include <nextweek/sphere.hpp>
#include <nextweek/utils.hpp>

void make_scene(SceneHittables &sh, cl::Context &context,
                cl::CommandQueue &queue) {
  // ground material
  sh.addObject(Sphere(1000.0f, Vec3(0, -1000.0f, 0)),
               LAMBERTIAN, Vec3(0.5f), 0.0f);

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      float choose_mat = random_float();
      Point3 center(a + 0.9f * random_float(), 0.2f,
                    b + 0.9f * random_float());
      if ((center - Vec3(4.0f, 0.2f, 0.0f)).length() >
          0.9f) {
        if (choose_mat < 0.8f) {
          // diffuse material
          Color albedo = random_vec() * random_vec();
          sh.addObject(Sphere(0.2f, center), LAMBERTIAN,
                       albedo, 0.0f);
        } else if (choose_mat < 0.95f) {
          Color albedo = random_vec(0.5f, 1.0f);
          float fuzz = random_float(0.0f, 0.5f);
          sh.addObject(Sphere(0.2f, center), METAL, albedo,
                       fuzz);
        } else {
          sh.addObject(Sphere(0.2f, center), DIELECTRIC,
                       Color(0.0f), 1.5f);
        }
      }
    }
  }
  sh.addObject(Sphere(1.0f, Point3(0.0f, 1.0f, 0.0f)),
               DIELECTRIC, Color(0.0f), 1.5f);

  //
  sh.addObject(Sphere(1.0f, Point3(-4.0f, 1.0f, 0.0f)),
               LAMBERTIAN, Color(0.4f, 0.2f, 0.1f), 0.0f);

  //
  sh.addObject(Sphere(1.0f, Point3(4.0f, 1.0f, 0.0f)),
               METAL, Color(0.7f, 0.6f, 0.5f), 0.0f);
  //
  sh.to_buffer(context, queue);
}

Camera make_camera(float aspect_ratio) {
  Point3 orig(13.0f, 2.0f, 3.0f);
  Point3 target(0.0f);
  Vec3 wup(0.0f, 1.0f, 0.0f);
  float vfov = 20.0f;
  float focus_dist = 10.0f;
  float aperature = 0.1f;
  float t0 = 0.0f;
  float t1 = 1.0f;
  Camera cam(orig, target, wup, vfov, aspect_ratio,
             aperature, focus_dist, t0, t1);
  // Camera cam(vfov, aspect_ratio);
  return cam;
}

void make_sample_per_pixel_random(
    SceneRandom<cl_float3> &sr, cl::Context &context,
    cl::CommandQueue &queue) {
  for (int i = 0; i < sr.size; i++) {
    sr.addRandom(random_in_unit_sphere().e, i);
  }
  sr.to_buffer(context, queue);
}
// void setArgs(cl::Kernel);

void set_kernel_args(cl::Kernel &kernel, int &arg_count,
                     SceneMaterial *sm) {
  arg_count++;
  kernel.setArg(arg_count, sm->cl_mat_type);
  arg_count++;
  kernel.setArg(arg_count, sm->cl_color);
  arg_count++;
  kernel.setArg(arg_count, sm->cl_fuzz);
}
void set_kernel_args(cl::Kernel &kernel, int &arg_count,
                     SceneHittables &sh) {
  arg_count++;
  kernel.setArg(arg_count, sh.cl_hittable_type);
  arg_count++;
  kernel.setArg(arg_count, sh.cl_center);
  arg_count++;
  kernel.setArg(arg_count, sh.cl_center2);
  arg_count++;
  kernel.setArg(arg_count, sh.cl_times);
  arg_count++;
  kernel.setArg(arg_count, sh.cl_radius);
  arg_count++;
  kernel.setArg(arg_count, sh.size);
}

void set_kernel_args(
    cl::Kernel &kernel, int &arg_count,
    SceneRandom<cl_float3> &sample_random) {
  arg_count++;
  kernel.setArg(arg_count, sample_random.cl_rand_vals);
}

void set_kernel_args(cl::Kernel &kernel, int &arg_count,
                     Camera &cam) {
  arg_count++;
  kernel.setArg(arg_count, cam.origin);
  arg_count++;
  kernel.setArg(arg_count, cam.lower_left_corner);
  arg_count++;
  kernel.setArg(arg_count, cam.horizontal);
  arg_count++;
  kernel.setArg(arg_count, cam.vertical);
  arg_count++;
  kernel.setArg(arg_count, cam.w);
  arg_count++;
  kernel.setArg(arg_count, cam.u);
  arg_count++;
  kernel.setArg(arg_count, cam.v);
  arg_count++;
  kernel.setArg(arg_count, cam.lens_radius);
  arg_count++;
  kernel.setArg(arg_count, cam.time0);
  arg_count++;
  kernel.setArg(arg_count, cam.time1);
}

void set_kernel_args(cl::Kernel &kernel, int &arg_count,
                     int depth, int image_width,
                     int image_height,
                     int samples_per_pixel,
                     float aspect_ratio) {
  arg_count++;
  kernel.setArg(arg_count, depth);
  arg_count++;
  kernel.setArg(arg_count, image_width);
  arg_count++;
  kernel.setArg(arg_count, image_height);
  arg_count++;
  kernel.setArg(arg_count, samples_per_pixel);
  arg_count++;
  kernel.setArg(arg_count, aspect_ratio);
}

void set_kernel_arguments(
    cl::Kernel &kernel, cl::Buffer &cl_output,
    SceneHittables &sh, const int depth,
    SceneRandom<cl_float3> &sample_random,
    const int image_width, const int image_height,
    const int samples_per_pixel, const float aspect_ratio,
    Camera &cam) {
  int arg_count = 0;
  kernel.setArg(arg_count, cl_output);
  //
  set_kernel_args(kernel, arg_count, sh);
  //
  set_kernel_args(kernel, arg_count, sample_random);
  //
  set_kernel_args(kernel, arg_count, sh.mat_ptr);
  set_kernel_args(kernel, arg_count, cam);
  set_kernel_args(kernel, arg_count, depth, image_width,
                  image_height, samples_per_pixel,
                  aspect_ratio);
}

int main() {
  // --------------- Image Related ------------------
  const float aspect_ratio = 16.0f / 9.0;
  const int image_width = 320;
  const int image_height =
      static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 20;
  const int depth = 10;

  // --------------- OpenCL Related ------------------
  cl_float4 *cpu_output;
  cl::CommandQueue queue;
  cl::Kernel kernel;
  cl::Context context;
  cl::Program program;
  cl::Buffer cl_output;
  cl::Device device;

  // -------------------------------------------------

  // initialize opencl objects
  init_opencl("kernels/nextweek/nextweek.cl", "ray_color",
              device, queue, kernel, program, context);

  // create output buffer
  const int output_size = image_width * image_height;
  cpu_output = new cl_float3[output_size];
  cl_output = cl::Buffer(context, CL_MEM_WRITE_ONLY,
                         output_size * sizeof(cl_float3));

  // make spheres
  SceneHittables sh;
  make_scene(sh, context, queue);

  // make camera
  Camera cam = make_camera(aspect_ratio);

  // make sample random
  const int sample_size = samples_per_pixel * output_size;
  const int rand_size = sample_size * depth;
  SceneRandom<cl_float3> sample_random(rand_size);
  make_sample_per_pixel_random(sample_random, context,
                               queue);

  // set kernel arguments
  set_kernel_arguments(kernel, cl_output, sh, depth,
                       sample_random, image_width,
                       image_height, samples_per_pixel,
                       aspect_ratio, cam);

  const int global_size = image_height * image_width;
  const int local_size = 10;

  launch_kernel(kernel, global_size, local_size, queue);

  queue.enqueueReadBuffer(cl_output, CL_TRUE, 0,
                          image_width * image_height *
                              sizeof(cl_float3),
                          cpu_output);

  save_to_ppm("media/ppm/nextweek.ppm", image_width,
              image_height, cpu_output);
  std::cout << "rendering done" << std::endl;

  //
  clean_up(cpu_output);
}
