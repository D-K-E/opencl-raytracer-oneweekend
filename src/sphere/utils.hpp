#ifndef UTILS_HPP
#define UTILS_HPP

const int IMWIDTH = 640;
const int IMHEIGHT = 480;

cl_float4 *cpu_out;
cl::CommandQueue queue;
cl::Kernel kernel;
cl::Context contxt;
cl::Program prog;
cl::Buffer cl_outp;
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
  //
  bool xmin = x < min;
  bool xmax = x > max;
  if (xmin) {
    return min;
  }
  if (xmax) {
    return max;
  }
  return x;
}

// convert RGB float in range [0,1] to int in range [0, 255]
inline int cast_color(float x) { return int(clamp(x, 0, 1) * 256); }

void save_to_ppm(const char *ppmpath) {
  std::ofstream ppm;
  ppm.open(ppmpath);

  ppm << "P3" << std::endl;
  ppm << IMWIDTH << " " << IMHEIGHT << std::endl;
  ppm << "255" << std::endl;
  for (int i = 0; i < IMWIDTH * IMHEIGHT; i++) {
    int ir = cast_color(cpu_out[i].s[0]);
    int ig = cast_color(cpu_out[i].s[1]);
    int ib = cast_color(cpu_out[i].s[2]);
    ppm << ir << " " << ig << " " << ib << std::endl;
  }

  ppm.close();
}

void clean_up() { delete cpu_out; }

void init_opencl(const char *kernel_source_path, const char *kernel_func_name) {
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  std::cout << "OpenCL platforms" << std::endl;
  for (unsigned int i = 0; i < platforms.size(); i++) {
    std::cout << platforms[i].getInfo<CL_PLATFORM_NAME>() << std::endl;
  }

  cl::Platform plat;
  pickPlatform(plat, platforms);
  std::cout << plat.getInfo<CL_PLATFORM_NAME>() << std::endl;

  std::cout << "OpenCL device" << std::endl;
  std::vector<cl::Device> devices;
  plat.getDevices(CL_DEVICE_TYPE_ALL, &devices);

  for (unsigned int i = 0; i < devices.size(); i++) {
    std::cout << "Device Name :: " << devices[i].getInfo<CL_DEVICE_NAME>()
              << std::endl;
    std::cout << "Device No :: " << i + 1 << std::endl;
  }

  pickDevice(device, devices);
  std::cout << "Device Name :: " << device.getInfo<CL_DEVICE_NAME>()
            << std::endl
            << "Device maximum compute unit :: " << std::endl
            << "Device maximum work group size :: " << std::endl;

  contxt = cl::Context(device);
  queue = cl::CommandQueue(contxt, device);

  std::string kernelSourceRaw = read_file(kernel_source_path);
  prog = cl::Program(contxt, kernelSourceRaw.c_str());
  cl_int result = prog.build({device});

  if (result == CL_BUILD_PROGRAM_FAILURE) {
    printError(prog, device);
  }

  kernel = cl::Kernel(prog, kernel_func_name);
}

#endif
