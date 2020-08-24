// see LICENSE
#include <custom/external.hpp>
// simple color gradient

const int IMWIDTH = 640;
const int IMHEIGHT = 480;

cl_float4 *cpu_out;
cl::CommandQueue queue;
cl::Kernel kernel;
cl::Context contxt;
cl::Program prog;
cl::Buffer cl_outp;

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

void init_opencl() {
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  for (unsigned int i = 0; i < platforms.size(); i++) {
    std::cout << "\t" << i + 1 << ": "
              << platforms[i].getInfo<CL_PLATFORM_NAME>() << std::endl;
  }

  // we have seen all let's ours
  cl::Platform platform;
  pickPlatform(platform, platforms);

  // show devices
  std::vector<cl::Device> devices;
  platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

  // print devices
  for (unsigned int i = 0; i < devices.size(); i++) {
    std::cout << devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;
  }
  std::cout << "foo device" << std::endl;
  cl::Device device;
  pickDevice(device, devices);
  std::cout << "\nUsing OpenCL device: \t" << device.getInfo<CL_DEVICE_NAME>()
            << std::endl;
  std::cout << "\t\t\tMax compute units: "
            << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
  std::cout << "\t\t\tMax work group size: "
            << device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << std::endl;

  contxt = cl::Context(device);
  queue = cl::CommandQueue(contxt, device);
  std::string kernelSource = read_file("kernels/gradient.cl");

  // create program
  prog = cl::Program(contxt, kernelSource.c_str());
  cl_int result = prog.build({device});
  if (result) {
    std::cout << "OpenCL Error :: (" << result << ")" << std::endl;
  }
  if (result == CL_BUILD_PROGRAM_FAILURE) {
    printError(prog, device);
  }
  kernel = cl::Kernel(prog, "color_gradient");
}

void save_to_ppm() {
  std::ofstream ppm;
  ppm.open("media/ppm/gradient.ppm");

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

int main() {

  // allocate memory
  cpu_out = new cl_float3[IMWIDTH * IMHEIGHT];
  std::cout << "mem alloc" << std::endl;

  // initialize opencl
  init_opencl();
  std::cout << "init opencl" << std::endl;

  // create image buffer on device
  cl_outp = cl::Buffer(contxt, CL_MEM_WRITE_ONLY,
                       IMWIDTH * IMHEIGHT * sizeof(cl_float3));

  kernel.setArg(0, cl_outp);
  kernel.setArg(1, IMWIDTH);
  kernel.setArg(2, IMHEIGHT);

  std::size_t global_work_size = IMWIDTH * IMHEIGHT;
  std::size_t local_work_size = 32;

  clock_t baslar, biter;
  baslar = clock();

  // launch the kernel
  queue.enqueueNDRangeKernel(kernel, NULL, global_work_size, local_work_size);
  queue.finish();

  // read and copy OpenCL output to CPU
  queue.enqueueReadBuffer(cl_outp, CL_TRUE, 0,
                          IMWIDTH * IMHEIGHT * sizeof(cl_float3), cpu_out);

  biter = clock();

  double saniyeler = ((double)(biter - baslar)) / CLOCKS_PER_SEC;
  std::cout << "Islem " << saniyeler << " saniye surdu" << std::endl;

  // save result to ppm
  save_to_ppm();
  std::cout << "rendering done saved" << std::endl;

  // clean up release memory
  clean_up();
  return 0;
}
