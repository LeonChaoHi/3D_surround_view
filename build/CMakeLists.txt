cmake_minimum_required(VERSION 3.19)
project(texture)

set(CMAKE_CXX_STANDARD 11)
set(GL_LIBS
    "/usr/lib/x86_64-linux-gnu/libGL.so"
    "/usr/lib/x86_64-linux-gnu/libGLU.so"
    "/usr/lib/x86_64-linux-gnu/libglut.so"
)

add_executable(texture ../main1.cpp ../model.h ../bowl.h ../newbowl.h ../coordinate.h ../main.h ../coord_calculator.cpp ../coord_calculator.h ../refinedbowl.h)#../rawcode.cpp)

find_package(OpenCV REQUIRED)
target_link_libraries(texture
    ${GL_LIBS}
    ${OpenCV_LIBS}
)
