// main func opencl raytracing in oneweekend

#include <CL/cl.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

const int image_width = 640;
const int image_height = 480;

cl_float4 *cpu_output;
cl::CommandQueue queue;
cl::Kernel kernel;
cl::Context context;
cl::Program program;
cl::Buffer cl_output;
cl::Device device;

void pickPlatform(cl::Platform &platform,
                  const std::vector<cl::Platform> &platforms) {
  // taken from
  // https://github.com/straaljager/OpenCL-path-tracing-tutorial-2-Part-1-Raytracing-a-sphere/blob/master/main.cpp

  if (platforms.size() == 1)
    platform = platforms[0];
  else {
    unsigned int input = 0;
    std::cout << "\nChoose an OpenCL platform: ";
    std::cin >> input;

    // handle incorrect user input
    while (input < 1 || input > platforms.size()) {
      std::cin.clear(); // clear errors/bad flags on cin
      std::cin.ignore(std::cin.rdbuf()->in_avail(),
                      '\n'); // ignores exact number of chars in cin buffer
      std::cout << "No such option. Choose an OpenCL platform: ";
      std::cin >> input;
    }
    platform = platforms[input - 1];
  }
}

void pickDevice(cl::Device &dev, const std::vector<cl::Device> &devices) {
  // taken from
  // https://github.com/straaljager/OpenCL-path-tracing-tutorial-2-Part-1-Raytracing-a-sphere/blob/master/main.cpp

  if (devices.size() == 1) {
    dev = devices[0];
  } else {
    unsigned int input = 0;
    std::cout << "\nChoose an OpenCL device: ";
    std::cin >> input;

    // handle incorrect user input
    while (input < 1 || input > devices.size()) {
      std::cin.clear(); // clear errors/bad flags on cin
      std::cin.ignore(std::cin.rdbuf()->in_avail(),
                      '\n'); // ignores exact number of chars in cin buffer
      std::cout << "No such option. Choose an OpenCL device: ";
      std::cin >> input;
    }
    dev = devices[input - 1];
  }
}

void printError(const cl::Program &prog, cl::Device &device) {
  //
  std::string buildlog = prog.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
  std::cerr << "Build log:" << std::endl << buildlog << std::endl;

  // Print the error log to a file
  std::ofstream error_file;
  error_file.open("errorlog.txt");
  error_file << "log" << std::endl << buildlog << std::endl;
  error_file.close();
  std::cout << "log" << std::endl << buildlog << std::endl;
  std::cout << "Error log saved in 'errorlog.txt'" << std::endl;
  exit(1);
}

std::string read_file(const char *path) {
  //
  std::ifstream kernelst(path);
  std::stringstream sbuffer;
  sbuffer << kernelst.rdbuf();
  std::string kernelSource = sbuffer.str();
  return kernelSource;
}

inline float clamp(float x, float min, float max) {
  bool xmin = x < min;
  bool xmax = x > max;
  if (xmin)
    return min;
  if (xmax)
    return max;
  return x;
}

inline int cast_color(float x) { return int(clamp(x, 0, 1) * 255); }

void save_to_ppm(const char *ppm_path, int imwidth, int imheight,
                 cl_float4 *cpu_out) {
  std::ofstream ppm;
  ppm.open(ppm_path);

  ppm << "P3" << std::endl;
  ppm << imwidth << " " << imheight << std::endl;
  ppm << "255" << std::endl;
  for (int i = 0; i < imwidth * imheight; i++) {
    int ir = cast_color(cpu_out[i].s[0]);
    int ig = cast_color(cpu_out[i].s[1]);
    int ib = cast_color(cpu_out[i].s[2]);
    ppm << ir << " " << ig << " " << ib << std::endl;
  }

  ppm.close();
}

void init_opencl(const char *kernel_path, const char *kernel_name) {
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  for (unsigned int i = 0; i < platforms.size(); i++) {
    std::cout << i + 1
              << ":Platform: " << platforms[i].getInfo<CL_PLATFORM_NAME>()
              << std::endl;
  }
  //
  cl::Platform platform;
  pickPlatform(platform, platforms);
  std::cout << platform.getInfo<CL_PLATFORM_NAME>() << " platform name"
            << std::endl;

  std::vector<cl::Device> devices;
  platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
  for (unsigned int i = 0; i < devices.size(); i++) {
    std::cout << i + 1 << ":Device: " << devices[i].getInfo<CL_DEVICE_NAME>()
              << std::endl;
  }
  pickDevice(device, devices);
  std::cout << ":Device Name: " << device.getInfo<CL_DEVICE_NAME>()
            << std::endl;

  context = cl::Context(device);
  std::string kernelSourceRaw = read_file(kernel_path);

  queue = cl::CommandQueue(context, device);
  program = cl::Program(context, kernelSourceRaw.c_str());
  cl_int result = program.build({device});
  if (result == CL_BUILD_PROGRAM_FAILURE) {
    printError(program, device);
  }
  kernel = cl::Kernel(program, kernel_name);
}

void clean_up(cl_float4 *cpu_out) { delete cpu_out; }

int main() {
  cpu_output = new cl_float3[image_width * image_height];

  //
  init_opencl("kernels/oneweekend/gradient.cl", "ray_color");

  //
  cl_output = cl::Buffer(context, CL_MEM_WRITE_ONLY,
                         image_width * image_height * sizeof(cl_float3));

  kernel.setArg(0, cl_output);
  kernel.setArg(1, image_width);
  kernel.setArg(2, image_height);

  std::size_t global_work_size = image_height * image_width;
  /*
  std::size_t local_work_size =
      kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device);
  if (global_work_size % local_work_size != 0) {
    global_work_size =
        (global_work_size / local_work_size + 1) * local_work_size;
  }
  */

  std::size_t local_work_size = 32;
  queue.enqueueNDRangeKernel(kernel, NULL, global_work_size, local_work_size);
  queue.finish();

  queue.enqueueReadBuffer(cl_output, CL_TRUE, 0,
                          image_width * image_height * sizeof(cl_float3),
                          cpu_output);

  save_to_ppm("media/ppm/gradiento.ppm", image_width, image_height, cpu_output);
  std::cout << "rendering done" << std::endl;

  //
  clean_up(cpu_output);
}
