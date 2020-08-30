// main func opencl raytracing in oneweekend
#include <oneweekend/camera.hpp>
#include <oneweekend/color.hpp>
#include <oneweekend/external.hpp>
#include <oneweekend/hittable.hpp>
#include <oneweekend/random.hpp>
#include <oneweekend/sphere.hpp>
#include <oneweekend/utils.hpp>

void make_scene(SceneHittables &sh, cl::Context &context,
                cl::CommandQueue &queue) {
  sh.addObject(Sphere(0.5f, Vec3(0.0f, 0.0f, -1.0f)), 0);
  sh.addObject(Sphere(100.0f, Vec3(0.0f, 100.5f, -1.0f)),
               1);
  sh.to_buffer(context, queue);
}

void make_camera(cl::Buffer &cl_camera,
                 cl::Context &context,
                 cl::CommandQueue &queue) {
  Camera cam = makeCamera();
  make_buffer<Camera>(cl_camera, context, queue, 1, &cam);
}

void make_sample_per_pixel_random(SceneRandom<cl_float> &sr,
                                  cl::Context &context,
                                  cl::CommandQueue &queue) {
  for (int i = 0; i < sr.size; i++) {
    sr.addRandom(RANDOM_FLOAT, random_float(), i);
  }
  sr.to_buffer(context, queue);
}

void make_depth_random(SceneRandom<cl_float3> &sr,
                       cl::Context &context,
                       cl::CommandQueue &queue) {
  for (int i = 0; i < sr.size; i++) {
    //
    sr.addRandom(RANDOM_UNIT_SPHERE,
                 random_in_unit_sphere().e, i);
  }
  sr.to_buffer(context, queue);
}

int main() {
  // --------------- Image Related ------------------
  const float aspect_ratio = 16.0f / 9.0;
  const int image_width = 320;
  const int image_height =
      static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 30;
  const int depth = 10;

  // --------------- OpenCL Related ------------------
  cl_float4 *cpu_output;
  cl::CommandQueue queue;
  cl::Kernel kernel;
  cl::Context context;
  cl::Program program;
  cl::Buffer cl_output;
  cl::Buffer cl_sphere;
  cl::Buffer cl_camera;
  cl::Buffer cl_sample_random;
  cl::Device device;

  // -------------------------------------------------

  // initialize opencl objects
  init_opencl("kernels/oneweekend/oneweekend.cl",
              "ray_color", device, queue, kernel, program,
              context);

  // create output buffer
  const int output_size = image_width * image_height;
  cpu_output = new cl_float3[output_size];
  cl_output = cl::Buffer(context, CL_MEM_WRITE_ONLY,
                         output_size * sizeof(cl_float3));

  // make spheres
  const int sphere_count = 2;
  SceneHittables sh(sphere_count);
  make_scene(sh, context, queue);

  // make camera
  make_camera(cl_camera, context, queue);

  // make sample random
  const int sample_size = samples_per_pixel * output_size;
  const int rand_size = sample_size * 2;
  SceneRandom<cl_float> sr(rand_size);
  make_sample_per_pixel_random(sr, context, queue);

  // make depth random
  const int depth_rand_size = sample_size * depth;
  SceneRandom<cl_float3> depth_random(depth_rand_size);
  make_depth_random(depth_random, context, queue);

  kernel.setArg(0, cl_output);
  kernel.setArg(1, sh.cl_center);
  kernel.setArg(2, sh.cl_radius);
  kernel.setArg(3, cl_camera);
  kernel.setArg(4, sr.cl_rand_vals);
  kernel.setArg(5, sr.cl_rand_type);
  kernel.setArg(6, depth_random.cl_rand_vals);
  kernel.setArg(7, depth_random.cl_rand_type);
  kernel.setArg(8, sh.size);
  kernel.setArg(9, depth);
  kernel.setArg(10, image_width);
  kernel.setArg(11, image_height);
  kernel.setArg(12, samples_per_pixel);
  kernel.setArg(13, aspect_ratio);

  const int global_size = image_height * image_width;
  const int local_size = 10;

  launch_kernel(kernel, global_size, local_size, queue);

  queue.enqueueReadBuffer(cl_output, CL_TRUE, 0,
                          image_width * image_height *
                              sizeof(cl_float3),
                          cpu_output);

  save_to_ppm("media/ppm/oneweekend.ppm", image_width,
              image_height, cpu_output);
  std::cout << "rendering done" << std::endl;

  //
  clean_up(cpu_output);
}
