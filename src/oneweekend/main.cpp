// main func opencl raytracing in oneweekend
#include <oneweekend/camera.hpp>
#include <oneweekend/color.hpp>
#include <oneweekend/external.hpp>
#include <oneweekend/sphere.hpp>
#include <oneweekend/utils.hpp>

void make_scene(Sphere *spheres, cl::Buffer &cl_sphere, cl::Context &context,
                cl::CommandQueue &queue, const int sphere_count) {
  spheres[0].radius = 0.5f;
  spheres[0].center = {{0.0f, 0.0f, -1.0f}};

  spheres[1].radius = 100.0f;
  spheres[1].center = {{0.0f, 100.5f, -1.0f}};

  cl_sphere =
      cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(Sphere) * sphere_count);
  queue.enqueueWriteBuffer(cl_sphere, CL_TRUE, 0, sphere_count * sizeof(Sphere),
                           spheres);
}

void make_camera(cl::Buffer &cl_camera, cl::Context &context,
                 cl::CommandQueue &queue) {
  Camera cam = makeCamera();
  cl_camera = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(Camera));
  queue.enqueueWriteBuffer(cl_camera, CL_TRUE, 0, sizeof(Camera), &cam);
}

void make_sample_per_pixel_random(cl::Buffer &cl_sample_random,
                                  cl::Context &context, cl::CommandQueue &queue,
                                  const int samples_per_pixel,
                                  const int image_size) {
  const int random_size = samples_per_pixel * image_size * 2;
  std::vector<cl_float> rands(random_size);
  for (int i = 0; i < random_size; i++) {
    //
    rands[i] = random_float();
  }
  cl_sample_random =
      cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(cl_float) * random_size);
  queue.enqueueWriteBuffer(cl_sample_random, CL_TRUE, 0,
                           random_size * sizeof(cl_float), rands.data());
}

int main() {
  // --------------- Image Related ------------------
  const float aspect_ratio = 16.0f / 9.0;
  const int image_width = 640;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 30;

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
  init_opencl("kernels/oneweekend/oneweekend.cl", "ray_color", device, queue,
              kernel, program, context);

  // create output buffer
  const int output_size = image_width * image_height;
  cpu_output = new cl_float3[output_size];
  cl_output =
      cl::Buffer(context, CL_MEM_WRITE_ONLY, output_size * sizeof(cl_float3));

  // make spheres
  const int sphere_count = 2;
  Sphere spheres[sphere_count];
  make_scene(spheres, cl_sphere, context, queue, sphere_count);

  // make camera
  make_camera(cl_camera, context, queue);

  // make sample random
  make_sample_per_pixel_random(cl_sample_random, context, queue,
                               samples_per_pixel, output_size);

  kernel.setArg(0, cl_output);
  kernel.setArg(1, cl_sphere);
  kernel.setArg(2, cl_camera);
  kernel.setArg(3, cl_sample_random);
  kernel.setArg(4, sphere_count);
  kernel.setArg(5, image_width);
  kernel.setArg(6, image_height);
  kernel.setArg(7, samples_per_pixel);
  kernel.setArg(8, aspect_ratio);

  const int global_size = image_height * image_width;
  const int local_size = 32;

  launch_kernel(kernel, global_size, local_size, queue);

  queue.enqueueReadBuffer(cl_output, CL_TRUE, 0,
                          image_width * image_height * sizeof(cl_float3),
                          cpu_output);

  save_to_ppm("media/ppm/oneweekend.ppm", image_width, image_height,
              cpu_output);
  std::cout << "rendering done" << std::endl;

  //
  clean_up(cpu_output);
}
