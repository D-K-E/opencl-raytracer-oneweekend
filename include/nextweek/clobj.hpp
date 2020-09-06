#ifndef CLOBJ_HPP
#define CLOBJ_HPP
#include <nextweek/external.hpp>

class ClObj {
public:
  int size;

  virtual void to_buffer(cl::Context &context,
                         cl::CommandQueue &queue) = 0;
};

#endif
