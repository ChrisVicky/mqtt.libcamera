# spdlog
find_package(spdlog REQUIRED)
add_library(log_lib log.cpp)

target_link_libraries(log_lib spdlog::spdlog_header_only)
