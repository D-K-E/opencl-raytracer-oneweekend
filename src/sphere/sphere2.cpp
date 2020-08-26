// see LICENSE
#include <custom/external.hpp>
#include "utils.hpp"

const int IMWIDTH = 640;
const int IMHEIGHT = 480;

cl_float4 *cpu_out;
cl::CommandQueue queue;
cl::Kernel kernel;
cl::Context contxt;
cl::Program prog;
cl::Buffer cl_outp;


void init_opencl() {
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

  cl::Device device;
  pickDevice(device, devices);
  std::cout << "Device Name :: " << device.getInfo<CL_DEVICE_NAME>()
            << std::endl
            << "Device maximum compute unit :: " << std::endl
            << "Device maximum work group size :: " << std::endl;

  contxt = cl::Context(device);
  queue = cl::CommandQueue(contxt, device);

  std::string kernelSourceRaw = read_file("kernels/sphere2.cl");
  prog = cl::Program(contxt, kernelSourceRaw.c_str());
  cl_int result = prog.build({device});

  if (result == CL_BUILD_PROGRAM_FAILURE) {
    printError(prog, device);
  }

  kernel = cl::Kernel(prog, "render_sphere");
}
