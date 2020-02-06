cmake_minimum_required(VERSION 3.0)
project(CNN)

SET(OpenCV_DIR D:\\prem\\study\\technical\\opencv_course\\dev\\opencv-4.1.0\\Installation\\x64\\vc15\\staticlib)
set(CMAKE_CXX_FLAGS_RELEASE "/MT")

find_package(Torch REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(CNN CNN.cpp)

target_link_libraries(CNN ${OpenCV_LIBS})
target_link_libraries(CNN "${TORCH_LIBRARIES}")

set_property(TARGET CNN PROPERTY CXX_STANDARD 14)

if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET CNN
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:CNN>)
endif (MSVC)