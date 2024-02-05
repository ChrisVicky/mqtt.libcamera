cmake_minimum_required(VERSION 3.10)
project(ImageIoTClient)

add_subdirectory(src)

add_executable(ImageHub main.cpp)

set_target_properties(ImageHub PROPERTIES
                      INSTALL_RPATH "/usr/lib;/usr/local/lib"
                      BUILD_WITH_INSTALL_RPATH TRUE)


target_link_libraries(ImageHub src_lib)

configure_file(
${CMAKE_SOURCE_DIR}/config.toml
${CMAKE_BINARY_DIR}/config.toml COPYONLY)
