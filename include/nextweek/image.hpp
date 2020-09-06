#ifndef IMAGE_HPP
#define IMAGE_HPP
#include <nextweek/external.hpp>

class Image {
public:
  std::vector<float *> image_data;
  std::size_t bytes_per_line;
  std::size_t image_width;
  std::size_t image_height;
  std::size_t bytes_per_pixel;
  cl::Image2DArray imarr;

public:
  Image() {}
  Image(const char *impath) {
    int width, height, nrComponents;

    float *data = stbi_loadf(impath, &width, &height,
                             &nrComponents, 0);
    image_data.push_back(data);
  }
  Image(std::vector<std::string> impaths) {
    int width, height, nrComponents;
    for (const std::string &path : impaths) {
      float *data = stbi_loadf(impath, &width, &height,
                               &nrComponents, 0);
      image_data.push_back(data);
    }
    image_width = static_cast<std::size_t>(width);
    image_height = static_cast<std::size_t>(height);
    bytes_per_pixel =
        static_cast<std::size_t>(nrComponents);

    bytes_per_pixel =
        static_cast<std::size_t>(nrComponents);
    bytes_per_line = static_cast<std::size_t>( //
        nrComponents * image_width             //
        );
  }
  void to_arr(cl::Context &context) {
    //
    std::size_t arr_size = image_data.size();
    cl::ImageFormat format(CL_RGBA, CL_FLOAT);
    int err;
    imarr = cl::Image2DArray(
        context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        format, arr_size, image_width, image_height,
        bytes_per_line, bytes_per_pixel, image_data.data(),
        &err);
    if (err != 0) {
      std::cout << "OpenCL Error :: " << err << std::endl;
    }
  }
};

#endif
