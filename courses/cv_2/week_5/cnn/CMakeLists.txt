cmake_minimum_required(VERSION 3.0)
project(CNN)

find_package(Torch REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(CNN CNN.cpp)

target_link_libraries(CNN ${OpenCV_LIBS})
target_link_libraries(CNN "${TORCH_LIBRARIES}")

set_property(TARGET CNN PROPERTY CXX_STANDARD 14)
