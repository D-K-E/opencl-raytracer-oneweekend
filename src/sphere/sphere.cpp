// render sphere in opencl
#include <custom/external.hpp>
//
#include "utils.hpp"

int main() {

  // 1. bellekte alan aç
  cpu_out = new cl_float3[IMWIDTH * IMHEIGHT];

  // 2. opencl baglamini baslat
  init_opencl("kernels/sphere.cl", "render_sphere_with_normal");

  // 3. OpenCL Buffer'ini ayarla
  cl_outp = cl::Buffer(contxt, CL_MEM_WRITE_ONLY,
                       IMWIDTH * IMHEIGHT * sizeof(cl_float3));

  // 4. Kernel argumanlarini olustur
  kernel.setArg(0, cl_outp);
  kernel.setArg(1, IMWIDTH);
  kernel.setArg(2, IMHEIGHT);

  // 5. Thread ve Bloklara dayali Veri hatti olustur
  std::size_t global_work_size = IMWIDTH * IMHEIGHT;
  std::size_t local_work_size = 32;

  clock_t baslar, biter;
  baslar = clock();

  queue.enqueueNDRangeKernel(kernel, NULL, global_work_size, local_work_size);
  queue.finish();

  // 6. Cikti al
  queue.enqueueReadBuffer(cl_outp, CL_TRUE, 0,
                          IMWIDTH * IMHEIGHT * sizeof(cl_float3), cpu_out);
  biter = clock();

  double saniyeler = ((double)(biter - baslar)) / CLOCKS_PER_SEC;
  std::cout << "Islem " << saniyeler << " saniye surdu" << std::endl;

  save_to_ppm("media/ppm/sphere.ppm");

  // 7. Temizle ==  Bellegi bosalt
  clean_up();

  return 0;
}
