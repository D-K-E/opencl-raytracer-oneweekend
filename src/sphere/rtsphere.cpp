#include <custom/external.hpp>
//
#include "utils.hpp"
#include <random>

cl::Buffer cl_spheres; // the global for passing spheres
cl::Buffer cl_randoms;

inline cl_float random_float() {
  static std::uniform_real_distribution<float> distr(0.0f, 1.0f);
  static thread_local std::mt19937 gen;
  return distr(gen);
}
inline cl_float random_float(cl_float min, cl_float max) {
  return min + (max - min) * random_float();
}
std::vector<cl_float> random_floats(int size) {
  std::vector<cl_float> rands(size);
  for (int i = 0; i < size; i++) {
    rands[i] = (float)random_float();
  }
  return rands;
}
std::vector<cl_float> random_floats(int size, cl_float min, cl_float max) {
  std::vector<cl_float> rands;
  for (int i = 0; i < size; i++) {
    rands.push_back(random_float(min, max));
  }
  return rands;
}
std::vector<std::vector<cl_float>> random_float_vec(int size, int vsize) {
  std::vector<std::vector<cl_float>> rands(size);
  for (int i = 0; i < size; i++) {
    rands[i] = random_floats(vsize);
  }
  return rands;
}
std::vector<std::vector<cl_float>>
random_float_vec(int size, int vsize, cl_float min, cl_float max) {
  std::vector<std::vector<cl_float>> rands(size);
  for (int i = 0; i < size; i++) {
    rands[i] = random_floats(vsize, min, max);
  }
  return rands;
}

struct Sphere {
  // the memory alignment is important
  cl_float radius;
  cl_float dummy1;
  cl_float dummy2;
  cl_float dummy3;
  cl_float3 center;
  cl_float3 color;
  cl_float3 emit;
};

void make_scene(Sphere *cpu_spheres) {
  // left wall
  cpu_spheres[0].radius = 200.0f;
  cpu_spheres[0].center = {{-200.6f, 0.0f, 0.0f}};
  cpu_spheres[0].color = {{0.75f, 0.25f, 0.25f}};
  cpu_spheres[0].emit = {{0.0f, 0.0f, 0.0f}};

  // right wall
  cpu_spheres[1].radius = 20.0f;
  cpu_spheres[1].center = {{200.6f, 0.0f, 0.0f}};
  cpu_spheres[1].color = {{0.25f, 0.25f, 0.75f}};
  cpu_spheres[1].emit = {{0.0f, 0.0f, 0.0f}};

  // floor
  cpu_spheres[2].radius = 200.0f;
  cpu_spheres[2].center = {{0.0f, -200.4f, 0.0f}};
  cpu_spheres[2].color = {{0.9f, 0.8f, 0.7f}};
  cpu_spheres[2].emit = {{0.0f, 0.0f, 0.0f}};

  // ceiling
  cpu_spheres[3].radius = 200.0f;
  cpu_spheres[3].center = {{0.0f, 200.4f, 0.0f}};
  cpu_spheres[3].color = {{0.9f, 0.8f, 0.7f}};
  cpu_spheres[3].emit = {{0.0f, 0.0f, 0.0f}};

  // back wall
  cpu_spheres[4].radius = 200.0f;
  cpu_spheres[4].center = {{0.0f, 0.0f, -200.4f}};
  cpu_spheres[4].color = {{0.9f, 0.8f, 0.7f}};
  cpu_spheres[4].emit = {{0.0f, 0.0f, 0.0f}};

  // front wall
  cpu_spheres[5].radius = 200.0f;
  cpu_spheres[5].center = {{0.0f, 0.0f, 202.0f}};
  cpu_spheres[5].color = {{0.9f, 0.8f, 0.7f}};
  cpu_spheres[5].emit = {{0.0f, 0.0f, 0.0f}};

  // left sphere
  cpu_spheres[6].radius = 0.16f;
  cpu_spheres[6].center = {{-0.25f, -0.24f, -0.1f}};
  cpu_spheres[6].color = {{0.9f, 0.8f, 0.7f}};
  cpu_spheres[6].emit = {{0.0f, 0.0f, 0.0f}};

  // right sphere
  cpu_spheres[7].radius = 0.16f;
  cpu_spheres[7].center = {{0.25f, -0.24f, 0.1f}};
  cpu_spheres[7].color = {{0.9f, 0.8f, 0.7f}};
  cpu_spheres[7].emit = {{0.0f, 0.0f, 0.0f}};

  // lightsource
  cpu_spheres[8].radius = 1.0f;
  cpu_spheres[8].center = {{0.0f, 1.36f, 0.0f}};
  cpu_spheres[8].color = {{0.0f, 0.0f, 0.0f}};
  cpu_spheres[8].emit = {{9.0f, 8.0f, 6.0f}};
}

int main() {

  // 1. bellekte alan aç
  cpu_out = new cl_float3[IMWIDTH * IMHEIGHT];

  // 2. opencl baglamini baslat
  init_opencl("kernels/rtsphere.cl", "render_kernel");

  // 3. OpenCL Buffer'ini ayarla
  cl_outp = cl::Buffer(contxt, CL_MEM_WRITE_ONLY,
                       IMWIDTH * IMHEIGHT * sizeof(cl_float3));

  // init scene
  const int sphere_count = 9;
  Sphere spheres[sphere_count];
  make_scene(spheres);

  // init random array
  const int stride = 2;
  std::vector<cl_float> rands = random_floats(IMWIDTH * IMHEIGHT * stride);
  cl_randoms =
      cl::Buffer(contxt, CL_MEM_READ_ONLY, sizeof(cl_float) * rands.size());
  queue.enqueueWriteBuffer(cl_randoms, CL_TRUE, 0,
                           rands.size() * sizeof(cl_float), rands.data());

  cl_spheres =
      cl::Buffer(contxt, CL_MEM_READ_ONLY, sizeof(Sphere) * sphere_count);

  queue.enqueueWriteBuffer(cl_spheres, CL_TRUE, 0,
                           sphere_count * sizeof(Sphere), spheres);

  // 4. Kernel argumanlarini olustur
  kernel.setArg(0, cl_spheres);
  kernel.setArg(1, IMWIDTH);
  kernel.setArg(2, IMHEIGHT);
  kernel.setArg(3, sphere_count);
  kernel.setArg(4, cl_outp);

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

  save_to_ppm("media/ppm/rtsphere.ppm");

  // 7. Temizle ==  Bellegi bosalt
  clean_up();

  return 0;
}
