// main func opencl raytracing in oneweekend
#include <nextweek/camera.hpp>
#include <nextweek/color.hpp>
#include <nextweek/external.hpp>
#include <nextweek/hittable.hpp>
#include <nextweek/image.hpp>
#include <nextweek/material.hpp>
#include <nextweek/random.hpp>
#include <nextweek/sphere.hpp>
#include <nextweek/texture.hpp>
#include <nextweek/utils.hpp>

void make_scene_oneweekend(SceneHittables &sh,
                           cl::Context &context,
                           cl::CommandQueue &queue) {
  // ground material
  sh.addObject(Sphere(1000.0f, Vec3(0, -1000.0f, 0)),
               LAMBERTIAN, 0.0f, DOUBLE_COLOR, Vec3(0.2f));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      //
      float choose_mat = random_float();
      Point3 center(a + 0.9f * random_float(), 0.2f,
                    b + 0.9f * random_float());
      //
      if ((center - Vec3(4.0f, 0.2f, 0.0f)).length() >
          0.9f) {
        if (choose_mat < 0.8f) {
          // diffuse material
          Color albedo = random_vec() * random_vec();
          Point3 center2 =
              center +
              Vec3(0.0f, random_float(0, 0.5), 0.0f);
          MovingSphere s = MovingSphere(
              0.2f, center, center2, 0.0f, 1.0f);
          // Sphere s(0.2f, center2);
          sh.addObject(s, LAMBERTIAN, 0.0f, SOLID_COLOR,
                       albedo);
        } else if (choose_mat < 0.95f) {
          Color albedo = random_vec(0.5f, 1.0f);
          float fuzz = random_float(0.0f, 0.5f);
          Sphere s(0.2f, center);
          sh.addObject(s, METAL, fuzz, SOLID_COLOR, albedo);
        } else {
          Sphere s(0.2f, center);
          sh.addObject(s, DIELECTRIC, 1.5f, SOLID_COLOR,
                       Color(0.0f));
        }
      }
    }
  }
  Sphere s(1.0f, Point3(0.0f, 1.0f, 0.0f));
  sh.addObject(s, DIELECTRIC, 1.5f, SOLID_COLOR,
               Color(0.0f));

  //
  Sphere s2(1.0f, Point3(-4.0f, 1.0f, 0.0f));
  sh.addObject(s2, LAMBERTIAN, 0.0f, SOLID_COLOR,
               Color(0.4f, 0.2f, 0.1f));

  //
  Sphere s3(1.0f, Point3(4.0f, 1.0f, 0.0f));
  sh.addObject(s3, METAL, 0.0f, SOLID_COLOR,
               Color(0.7f, 0.6f, 0.5f));
  //
  sh.to_buffer(context, queue);
}

void make_scene(SceneHittables &sh, Image &img,
                cl::Context &context,
                cl::CommandQueue &queue) {
  // ground material
  img.to_buffer(context, queue);
  Vec3 img_index(0.0f, -1.0f, -1.0f);
  Sphere s(2.0f, Vec3(0.0f));
  sh.addObject(s, LAMBERTIAN, 0.0f, IMAGE_COLOR, img_index);
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
  check_error(kernel.setArg(arg_count, sm->cl_mat_type));
  arg_count++;
  check_error(kernel.setArg(arg_count, sm->cl_fuzz));
}
void set_kernel_args(cl::Kernel &kernel, int &arg_count,
                     Texture *t_ptr) {
  arg_count++;
  check_error(
      kernel.setArg(arg_count, t_ptr->cl_texture_type));
  arg_count++;
  check_error(kernel.setArg(arg_count, t_ptr->cl_color));
}
void set_kernel_args(cl::Kernel &kernel, int &arg_count,
                     SceneHittables &sh) {
  arg_count++;
  check_error(
      kernel.setArg(arg_count, sh.cl_hittable_type));
  arg_count++;
  check_error(kernel.setArg(arg_count, sh.cl_center));
  arg_count++;
  check_error(kernel.setArg(arg_count, sh.cl_center2));
  arg_count++;
  check_error(kernel.setArg(arg_count, sh.cl_times));
  arg_count++;
  check_error(kernel.setArg(arg_count, sh.cl_radius));
  arg_count++;
  check_error(kernel.setArg(arg_count, sh.size));
}

void set_kernel_args(
    cl::Kernel &kernel, int &arg_count,
    SceneRandom<cl_float3> &sample_random) {
  arg_count++;
  check_error(
      kernel.setArg(arg_count, sample_random.cl_rand_vals));
}

void set_kernel_args(cl::Kernel &kernel, int &arg_count,
                     Camera &cam) {
  arg_count++;
  check_error(kernel.setArg(arg_count, cam.origin));
  arg_count++;
  check_error(
      kernel.setArg(arg_count, cam.lower_left_corner));
  arg_count++;
  check_error(kernel.setArg(arg_count, cam.horizontal));
  arg_count++;
  check_error(kernel.setArg(arg_count, cam.vertical));
  arg_count++;
  check_error(kernel.setArg(arg_count, cam.w));
  arg_count++;
  check_error(kernel.setArg(arg_count, cam.u));
  arg_count++;
  check_error(kernel.setArg(arg_count, cam.v));
  arg_count++;
  check_error(kernel.setArg(arg_count, cam.lens_radius));
  arg_count++;
  check_error(kernel.setArg(arg_count, cam.time0));
  arg_count++;
  check_error(kernel.setArg(arg_count, cam.time1));
}

void set_kernel_args(cl::Kernel &kernel, int &arg_count,
                     int depth, int image_width,
                     int image_height,
                     int samples_per_pixel,
                     float aspect_ratio) {
  arg_count++;
  check_error(kernel.setArg(arg_count, depth));
  arg_count++;
  check_error(kernel.setArg(arg_count, image_width));
  arg_count++;
  check_error(kernel.setArg(arg_count, image_height));
  arg_count++;
  check_error(kernel.setArg(arg_count, samples_per_pixel));
  arg_count++;
  check_error(kernel.setArg(arg_count, aspect_ratio));
}

void set_kernel_args(cl::Kernel &kernel, int &arg_count,
                     Image &img) {
  arg_count++;
  check_error(kernel.setArg(arg_count, img.cl_img));
  arg_count++;
  check_error(kernel.setArg(arg_count, img.bytes_per_line));
  arg_count++;
  check_error(
      kernel.setArg(arg_count, img.bytes_per_pixel));
  arg_count++;
  check_error(kernel.setArg(arg_count, img.image_width));
  arg_count++;
  check_error(kernel.setArg(arg_count, img.image_height));
  arg_count++;
  check_error(kernel.setArg(arg_count, img.nb_images));
}

void set_kernel_arguments(
    cl::Kernel &kernel, cl::Buffer &cl_output,
    SceneHittables &sh, const int depth,
    SceneRandom<cl_float3> &sample_random,
    const int image_width, const int image_height,
    const int samples_per_pixel, const float aspect_ratio,
    Camera &cam, Image &img) {
  int arg_count = 0;
  check_error(kernel.setArg(arg_count, cl_output));
  //
  set_kernel_args(kernel, arg_count, sh);
  //
  set_kernel_args(kernel, arg_count, sample_random);
  //
  set_kernel_args(kernel, arg_count, sh.mat_ptr);
  set_kernel_args(kernel, arg_count,
                  sh.mat_ptr->texture_ptr);
  set_kernel_args(kernel, arg_count, cam);
  set_kernel_args(kernel, arg_count, depth, image_width,
                  image_height, samples_per_pixel,
                  aspect_ratio);
  set_kernel_args(kernel, arg_count, img);
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
  init_opencl("kernels/nextweek/nextweek.cl", // kernel path
              "ray_color", // kernel entry function
              "../include/nextweek/kernels/", // include dir
                                              // for kernel
              device,                         // device
              queue,                          // queue
              kernel,  // kernel object
              program, // program
              context  // opencl context
              );

  // create output buffer
  const int output_size = image_width * image_height;
  cpu_output = new cl_float3[output_size];
  cl_output = cl::Buffer(context, CL_MEM_WRITE_ONLY,
                         output_size * sizeof(cl_float3));

  // make spheres
  SceneHittables sh;
  Image img("media/earthmap.png");
  make_scene(sh, img, context, queue);

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
                       aspect_ratio, cam, img);

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
