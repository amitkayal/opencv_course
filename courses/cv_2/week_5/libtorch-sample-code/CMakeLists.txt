cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(sample)

find_package(Torch REQUIRED)

add_executable(sample sample.cpp)
target_link_libraries(sample "${TORCH_LIBRARIES}")
set_property(TARGET sample PROPERTY CXX_STANDARD 14)

if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET sample
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:sample>)
endif (MSVC)