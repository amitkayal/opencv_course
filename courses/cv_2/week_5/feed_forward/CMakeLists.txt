cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(ffnet)

SET(OpenCV_DIR D:\\prem\\study\\technical\\opencv_course\\dev\\opencv-4.1.0\\Installation\\x64\\vc15\\staticlib)
set(CMAKE_CXX_FLAGS_RELEASE "/MT")

find_package(Torch REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(ffnet feedforward.cpp read-mnist.h)
target_link_libraries(ffnet ${OpenCV_LIBS})
target_link_libraries(ffnet "${TORCH_LIBRARIES}")
set_property(TARGET ffnet PROPERTY CXX_STANDARD 14)

if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET ffnet
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:ffnet>)
endif (MSVC)