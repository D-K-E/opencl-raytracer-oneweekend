#ifndef IMAGE_HPP
#define IMAGE_HPP
#include "nextweek/utils.hpp"
#include <nextweek/external.hpp>

class Image {
public:
  std::vector<cl_float> image_data;
  int bytes_per_line;
  int image_width;
  int image_height;
  int bytes_per_pixel;
  int nb_images;
  cl::Buffer cl_img;

public:
  Image() {}
  Image(const char *impath) {
    int width, height, nrComponents;

    unsigned char *data = stbi_load(impath, &width, &height,
                                    &nrComponents, 0);
    image_data = imload(data, width, height, nrComponents);
    nb_images = 1;
    set_im_values(width, height, nrComponents);
  }
  Image(std::vector<std::string> impaths) {
    int width, height, nrComponents;
    nb_images = impaths.size();
    std::vector<cl_float> imc;
    for (const std::string &impath : impaths) {
      unsigned char *data =
          stbi_load(impath.c_str(), &width, &height,
                    &nrComponents, 0);
      imload(data, width, height, nrComponents, imc);
    }
    set_im_values(width, height, nrComponents);
  }
  std::vector<cl_float> imload(unsigned char *data,
                               int width, int height,
                               int nbComponents) {
    std::vector<cl_float> imc(width * height *
                              nbComponents);
    for (int i = 0; i < width * height * nbComponents;
         i++) {
      imc[i] = static_cast<cl_float>(data[i]) / 255.0f;
    }
    return imc;
  }

  std::vector<cl_float> imload(unsigned char *data,
                               int width, int height,
                               int nbComponents,
                               std::vector<cl_float> &imc) {
    for (int i = 0; i < width * height * nbComponents;
         i++) {
      imc.push_back(static_cast<cl_float>(data[i]) /
                    255.0f);
    }
    return imc;
  }
  void set_im_values(int width, int height, int per_pixel) {
    image_width = width;
    image_height = height;
    bytes_per_pixel = per_pixel;
    bytes_per_line = per_pixel * width;
  }
  void to_buffer(cl::Context &context,
                 cl::CommandQueue &queue) {
    //
    std::size_t arr_size = image_data.size();
    int flag = bytes_per_pixel == 3 ? CL_RGB : CL_RGBA;
    cl::ImageFormat format(flag, CL_UNSIGNED_INT8);
    make_buffer<cl_float>(cl_img, context, queue, arr_size,
                          image_data.data());
  }
};

#endif
