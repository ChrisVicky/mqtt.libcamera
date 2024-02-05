find_package(PkgConfig)
find_package(JPEG REQUIRED)
find_package(spdlog REQUIRED)

if(JPEG_FOUND)
  message(STATUS "JPEG library found")
  include_directories(${JPEG_INCLUDE_DIR})
else()
  message(FATAL_ERROR "JPEG library not found")
endif()

pkg_check_modules(LIBCAMERA REQUIRED IMPORTED_TARGET libcamera)
message(STATUS "libcamera library found:")
message(STATUS "    version: ${LIBCAMERA_VERSION}")
message(STATUS "    libraries: ${LIBCAMERA_LINK_LIBRARIES}")
message(STATUS "    include path: ${LIBCAMERA_INCLUDE_DIRS}")

include_directories(${CMAKE_SOURCE_DIR} ${LIBCAMERA_INCLUDE_DIRS} ${LIBEVENT_INCLUDE_DIRS})

add_library(camera_lib camera.cpp)

target_link_libraries(camera_lib loop_lib)
target_link_libraries(camera_lib PkgConfig::LIBCAMERA)
target_link_libraries(camera_lib ${JPEG_LIBRARIES})
target_link_libraries(camera_lib spdlog::spdlog_header_only)

