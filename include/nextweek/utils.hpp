#ifndef UTILS_HPP
#define UTILS_HPP
#include <nextweek/external.hpp>

const float PI = 3.1415926535897f;
const bool debug = true;
float degrees_to_radians(float degrees) {
  return degrees * PI / 180.0;
}
void check_error(int err) {
  std::string s1(" File :: ");
  s1 += __FILE__;
  s1 += " :: Line ::";
  s1 += __LINE__;
  s1 += " ";
  std::string s;
  switch (err) {
  case -30:
    s = "CL_INVALID_VALUE";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -31:
    s = "CL_INVALID_DEVICE_TYPE";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -32:
    s = "CL_INVALID_PLATFORM";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -33:
    s = "CL_INVALID_DEVICE ";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -34:
    s = "CL_INVALID_CONTEX";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -35:
    s = "CL_INVALID_QUEUE_PROPERTIES";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -36:
    s = "CL_INVALID_COMMAND_QUEUE ";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -37:
    s = "CL_INVALID_HOST_PTR";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -38:
    s = "CL_INVALID_MEM_OBJECT";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -39:
    s = "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -40:
    s = "CL_INVALID_IMAGE_SIZE";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -41:
    s = "CL_INVALID_SAMPLER ";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -42:
    s = "CL_INVALID_BINARY";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -43:
    s = "CL_INVALID_BUILD_OPTIONS";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -44:
    s = "CL_INVALID_PROGRAM";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -45:
    s = "CL_INVALID_PROGRAM_EXECUTABLE";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -46:
    s = "CL_INVALID_KERNEL_NAME";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -47:
    s = "CL_INVALID_KERNEL_DEFINITION";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -48:
    s = "CL_INVALID_KERNEL";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -49:
    s = "CL_INVALID_ARG_INDEX";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -50:
    s = "CL_INVALID_ARG_VALUE";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -51:
    s = "CL_INVALID_ARG_SIZE";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -52:
    s = "CL_INVALID_KERNEL_ARGS";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -53:
    s = "CL_INVALID_WORK_DIMENSION";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -54:
    s = "CL_INVALID_WORK_GROUP_SIZE";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -55:
    s = "CL_INVALID_WORK_ITEM_SIZE";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -56:
    s = "CL_INVALID_GLOBAL_OFFSET";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -57:
    s = "CL_INVALID_EVENT_WAIT_LIST";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -58:
    s = "CL_INVALID_EVENT";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -59:
    s = "CL_INVALID_OPERATION";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -60:
    s = "CL_INVALID_GL_OBJECT";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -61:
    s = "CL_INVALID_BUFFER_SIZE";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -62:
    s = "CL_INVALID_MIP_LEVEL";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -63:
    s = "CL_INVALID_GLOBAL_WORK_SIZE";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -64:
    s = "CL_INVALID_PROPERTY";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -65:
    s = "CL_INVALID_IMAGE_DESCRIPTOR";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -66:
    s = "CL_INVALID_COMPILER_OPTIONS";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -67:
    s = "CL_INVALID_LINKER_OPTIONS ";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -68:
    s = "CL_INVALID_DEVICE_PARTITION_COUNT";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -69:
    s = "CL_INVALID_PIPE_SIZE";
    s += s1;
    throw std::runtime_error(s);
    break;
  case -70:
    s = "CL_INVALID_DEVICE_QUEUE";
    s += s1;
    throw std::runtime_error(s);
    break;
  }
}

template <typename T>
void make_buffer(cl::Buffer &cl_buffer,
                 cl::Context &context,
                 cl::CommandQueue &queue,
                 const int buffer_size, T *data) {
  int err;
  cl_buffer = cl::Buffer(context, CL_MEM_READ_ONLY,
                         sizeof(T) * buffer_size, &err);
  check_error(err);
  queue.enqueueWriteBuffer(cl_buffer, CL_TRUE, 0,
                           buffer_size * sizeof(T), data);
}

inline cl_float random_float() {
  static std::uniform_real_distribution<cl_float> dist(
      0.0f, 1.0f);
  static std::mt19937 gen;
  return dist(gen);
}

inline cl_int random_int(int max) {
  static std::uniform_int_distribution<int> dist(0, max);
  static std::mt19937 gen;
  return dist(gen);
}

inline cl_float random_float(cl_float min, cl_float max) {
  return min + (max - min) * random_float();
}

void pickPlatform(
    cl::Platform &platform,
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
                      '\n'); // ignores exact number of
                             // chars in cin buffer
      std::cout
          << "No such option. Choose an OpenCL platform: ";
      std::cin >> input;
    }
    platform = platforms[input - 1];
  }
}

void pickDevice(cl::Device &dev,
                const std::vector<cl::Device> &devices) {
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
                      '\n'); // ignores exact number of
                             // chars in cin buffer
      std::cout
          << "No such option. Choose an OpenCL device: ";
      std::cin >> input;
    }
    dev = devices[input - 1];
  }
}

void printError(const cl::Program &prog,
                cl::Device &device) {
  //
  std::string buildlog =
      prog.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
  std::cerr << "Build log:" << std::endl
            << buildlog << std::endl;

  // Print the error log to a file
  std::ofstream error_file;
  error_file.open("errorlog.txt");
  error_file << "log" << std::endl << buildlog << std::endl;
  error_file.close();
  std::cout << "log" << std::endl << buildlog << std::endl;
  std::cout << "Error log saved in 'errorlog.txt'"
            << std::endl;
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

void init_opencl(const char *kernel_path,
                 const char *kernel_name,
                 const char *include_dir,
                 cl::Device &device,
                 cl::CommandQueue &queue,
                 cl::Kernel &kernel, cl::Program &program,
                 cl::Context &context) {
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  for (unsigned int i = 0; i < platforms.size(); i++) {
    std::cout << i + 1 << ":Platform: "
              << platforms[i].getInfo<CL_PLATFORM_NAME>()
              << std::endl;
  }
  //
  cl::Platform platform;
  pickPlatform(platform, platforms);
  std::cout << platform.getInfo<CL_PLATFORM_NAME>()
            << " platform name" << std::endl;

  std::vector<cl::Device> devices;
  platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
  for (unsigned int i = 0; i < devices.size(); i++) {
    std::cout << i + 1 << ":Device: "
              << devices[i].getInfo<CL_DEVICE_NAME>()
              << std::endl;
  }
  pickDevice(device, devices);
  std::cout << ":Device Name: "
            << device.getInfo<CL_DEVICE_NAME>()
            << std::endl;

  context = cl::Context(device);
  std::string kernelSourceRaw = read_file(kernel_path);

  queue = cl::CommandQueue(context, device);
  program = cl::Program(context, kernelSourceRaw.c_str());
  //
  std::string include_flag = "-I ";

  //
  cl_int result = program.build(
      {device}, (include_flag + include_dir).c_str());

  //
  if (result == CL_BUILD_PROGRAM_FAILURE) {
    printError(program, device);
  }
  kernel = cl::Kernel(program, kernel_name);
}
void launch_kernel(cl::Kernel &kernel,
                   const int global_size,
                   const int local_size,
                   cl::CommandQueue queue) {
  std::size_t global_work_size = global_size;
  std::size_t local_work_size = local_size;
  queue.enqueueNDRangeKernel(kernel, NULL, global_work_size,
                             local_work_size);
  queue.finish();
}

void clean_up(cl_float4 *cpu_out) { delete cpu_out; }

#endif
