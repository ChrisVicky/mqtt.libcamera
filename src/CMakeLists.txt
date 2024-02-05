add_subdirectory(log)
add_subdirectory(loop)
add_subdirectory(config)
add_subdirectory(client)

add_library(src_lib INTERFACE)

target_link_libraries(src_lib INTERFACE log_lib client_lib loop_lib config_lib)

