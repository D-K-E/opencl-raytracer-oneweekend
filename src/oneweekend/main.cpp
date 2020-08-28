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

int main() {
  // --------------- Image Related ------------------
  const int image_width = 640;
  const int image_height = 480;

  // --------------- OpenCL Related ------------------
  cl_float4 *cpu_output;
  cl::CommandQueue queue;
  cl::Kernel kernel;
  cl::Context context;
  cl::Program program;
  cl::Buffer cl_output;
  cl::Buffer cl_sphere;
  cl::Buffer cl_camera;
  cl::Device device;

  // -------------------------------------------------

  cpu_output = new cl_float3[image_width * image_height];

  init_opencl("kernels/oneweekend/oneweekend.cl", "ray_color", device, queue,
              kernel, program, context);

  cl_output = cl::Buffer(context, CL_MEM_WRITE_ONLY,
                         image_width * image_height * sizeof(cl_float3));

  // make spheres
  const int sphere_count = 2;
  Sphere spheres[sphere_count];
  make_scene(spheres, cl_sphere, context, queue, sphere_count);

  // make camera
  make_camera(cl_camera, context, queue);

  kernel.setArg(0, cl_output);
  kernel.setArg(1, cl_sphere);
  kernel.setArg(2, cl_camera);
  kernel.setArg(3, sphere_count);
  kernel.setArg(4, image_width);
  kernel.setArg(5, image_height);
  kernel.setArg(6, (float)image_width / (float)image_height);

  launch_kernel(kernel, image_height * image_width, device, queue);

  queue.enqueueReadBuffer(cl_output, CL_TRUE, 0,
                          image_width * image_height * sizeof(cl_float3),
                          cpu_output);

  save_to_ppm("media/ppm/oneweekend.ppm", image_width, image_height,
              cpu_output);
  std::cout << "rendering done" << std::endl;

  //
  clean_up(cpu_output);
}
