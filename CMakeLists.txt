cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project("OpenCL Path Tracer")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER "clang++")


set(CMAKE_CXX_FLAGS "-lstdc++ -ldl -ggdb -Wall -Wextra -pedantic -Wextra-tokens -std=c++11")


#find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)


set ( ALL_LIBS
    #${GLFW_SHARED_LIB}
    #${ASSIMP_SHARED_LIB}
    ${OpenCL_LIBRARY}
)

include_directories(
    "./include/"
    ${OpenCL_INCLUDE_DIR}
    #${OpenGL_INCLUDE_DIR}
)
link_directories(${OpenCL_LIBRARY})

add_executable(
    hello.out
    #"src/glad.c"
    "src/hello/hello.cpp"
)
add_executable(
    gradient.out
    #"src/glad.c"
    "src/gradient/gradient.cpp"
    "src/sphere/utils.hpp"
)
add_executable(
    sphere.out
    #"src/glad.c"
    "src/sphere/sphere.cpp"
    "src/sphere/utils.hpp"
)
add_executable(
    rtsphere.out
    #"src/glad.c"
    "src/sphere/rtsphere.cpp"
    "src/sphere/utils.hpp"
)

add_executable(
    oneweekend.out
    "src/oneweekend/main.cpp"
)

target_link_libraries(hello.out ${ALL_LIBS})
target_link_libraries(gradient.out ${ALL_LIBS})
target_link_libraries(sphere.out ${ALL_LIBS})
target_link_libraries(rtsphere.out ${ALL_LIBS})
target_link_libraries(oneweekend.out ${ALL_LIBS})

install(TARGETS hello.out DESTINATION "${PROJECT_SOURCE_DIR}/bin")
install(TARGETS gradient.out DESTINATION "${PROJECT_SOURCE_DIR}/bin")
install(TARGETS sphere.out DESTINATION "${PROJECT_SOURCE_DIR}/bin")
install(TARGETS rtsphere.out DESTINATION "${PROJECT_SOURCE_DIR}/bin")
install(TARGETS oneweekend.out DESTINATION "${PROJECT_SOURCE_DIR}/bin")
