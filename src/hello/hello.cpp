// simple hello world to opencl
// most of the code is adapted from
// https://github.com/straaljager/OpenCL-path-tracing-tutorial-1-Getting-started/blob/master/main.cpp
// see LICENSE
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <vector>
//

#include <CL/cl.hpp>

int main() {
  // Find all available OpenCL platforms (e.g. AMD OpenCL, Nvidia CUDA, Intel
  // OpenCL)
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);

  // Show the names of all available OpenCL platforms
  std::cout << "Available OpenCL platforms: \n\n";
  for (unsigned int i = 0; i < platforms.size(); i++)
    std::cout << "\t" << i + 1 << ": "
              << platforms[i].getInfo<CL_PLATFORM_NAME>() << std::endl;

  // choose a platform
  cl::Platform platform = platforms[0];

  // show devices
  std::vector<cl::Device> devices;
  platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

  // print device
  std::cout << devices[0].getInfo<CL_DEVICE_NAME>() << std::endl;

  cl::Device device = devices[0];

  // context for opencl
  cl::Context contxt = cl::Context(device);

  // get kernel
  std::ifstream kernelst("kernels/hello.cl");
  std::stringstream sbuffer;
  sbuffer << kernelst.rdbuf();
  std::string kernelSource = sbuffer.str();

  // create program
  cl::Program prog = cl::Program(contxt, kernelSource.c_str());

  // build program and check for errors
  cl_int result = prog.build({device}, "");
  if (result) {
    std::cout << "Error during compilation :: (" << result << ")" << std::endl;
  }

  // Create a kernel (entry point in the OpenCL program)
  // kernels are the basic units of executable code that run on the OpenCL
  // device
  // the kernel forms the starting point into the OpenCL program, analogous to
  // main() in CPU code
  // kernels can be called from the host (CPU)
  // kernel
  cl::Kernel kernel = cl::Kernel(prog, "parallel_add");

  // Create input data arrays on the host (= CPU)
  const int numElements = 10;
  float cpuArrayA[numElements] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f,
                                  5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
  float cpuArrayB[numElements] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f,
                                  0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
  float cpuOutput[numElements] = {};
  // empty array for storing the results of the OpenCL program

  // Create buffers (memory objects) on the OpenCL device, allocate memory and
  // copy input data to device.
  // Flags indicate how the buffer should be used e.g. read-only, write-only,
  // read-write

  cl::Buffer clBufferA =
      cl::Buffer(contxt, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                 numElements * sizeof(cl_int), cpuArrayA);

  cl::Buffer clBufferB =
      cl::Buffer(contxt, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                 numElements * sizeof(cl_int), cpuArrayB);

  cl::Buffer clOutputBuffer =
      cl::Buffer(contxt, CL_MEM_WRITE_ONLY, numElements * sizeof(cl_int), NULL);

  // Specify the arguments for OpenCL kernel
  // (the arguments are __global float* x, __global float* y, __global float* z)

  kernel.setArg(0, clBufferA);
  kernel.setArg(1, clBufferB);
  kernel.setArg(2, clOutputBuffer);

  // Create a command queue for the OpenCL device
  // the command queue allows kernel execution commands to be sent to the device
  cl::CommandQueue queue = cl::CommandQueue(contxt, device);

  // Determine the global and local number of "work items"
  // The global work size is the total number of work items (threads) that
  // execute in parallel
  // Work items executing together on the same compute unit are grouped into
  // "work groups"
  // The local work size defines the number of work items in each work group
  // Important: global_work_size must be an integer multiple of local_work_size
  std::size_t global_work_size = numElements;
  std::size_t local_work_size = 10; // could also be 1, 2 or 5 in this example
  // when local_work_size equals 10, all ten number pairs from both arrays will
  // be added together in one go
  queue.enqueueNDRangeKernel(kernel, NULL, global_work_size, local_work_size);

  // Read and copy OpenCL output to CPU
  // the "CL_TRUE" flag blocks the read operation until all work items have
  // finished their computation
  queue.enqueueReadBuffer(clOutputBuffer, CL_TRUE, 0,
                          numElements * sizeof(cl_float), cpuOutput);

  // Print results to console
  for (int i = 0; i < numElements; i++) {
    std::cout << cpuArrayA[i] << " + " << cpuArrayB[i] << " = " << cpuOutput[i]
              << std::endl;
  }
  //
  return 0;
}
