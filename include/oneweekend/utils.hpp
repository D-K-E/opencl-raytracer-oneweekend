#ifndef UTILS_HPP
#define UTILS_HPP

#include <random>

inline float random_float() {
  static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
  static std::mt19937 gen;
  return dist(gen);
}

inline float random_float(float min, float max) {
  return min + (max - min) * random_float();
}

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

void init_opencl(const char *kernel_path, const char *kernel_name,
                 cl::Device &device, cl::CommandQueue &queue,
                 cl::Kernel &kernel, cl::Program &program,
                 cl::Context &context) {
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
void launch_kernel(cl::Kernel &kernel, 
        int global_size, int local_size,
        cl::Device device,
                   cl::CommandQueue queue) {
  std::size_t global_work_size = global_size;
  /*
  std::size_t local_work_size =
      kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device);
  if (global_work_size % local_work_size != 0) {
    global_work_size =
        (global_work_size / local_work_size + 1) * local_work_size;
  }
  */
  std::size_t local_work_size = local_size;
  queue.enqueueNDRangeKernel(kernel, NULL, global_work_size, local_work_size);
  queue.finish();
}

void clean_up(cl_float4 *cpu_out) { delete cpu_out; }

#endif
