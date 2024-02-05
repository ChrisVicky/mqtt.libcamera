# Paho mqtt cpp dependencies
find_package(PahoMqttCpp REQUIRED)
find_package(eclipse-paho-mqtt-c REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog REQUIRED)

add_subdirectory(camera)
add_subdirectory(mqtt)

add_library(client_lib client.cpp)

target_link_libraries(client_lib PahoMqttCpp::paho-mqttpp3)
target_link_libraries(client_lib log_lib)
target_link_libraries(client_lib mqtt_lib)
target_link_libraries(client_lib camera_lib)
target_link_libraries(client_lib spdlog::spdlog_header_only)
