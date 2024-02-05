cmake_minimum_required(VERSION 3.10)
project(ImageHub VERSION 1.0)

add_subdirectory(src)

add_executable(ImageHub main.cpp)

set_target_properties(ImageHub PROPERTIES
                      INSTALL_RPATH "/usr/lib;/usr/local/lib"
                      BUILD_WITH_INSTALL_RPATH TRUE)


target_link_libraries(ImageHub src_lib)

configure_file(
${CMAKE_SOURCE_DIR}/config.toml
${CMAKE_BINARY_DIR}/config.toml COPYONLY)

## Custom Commands
find_package(Git REQUIRED)
add_custom_target(pack
    COMMAND ${GIT_EXECUTABLE} archive --format=zip HEAD
    > ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}.zip
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Creating archive of src code"
)
