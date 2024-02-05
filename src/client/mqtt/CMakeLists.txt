# Paho mqtt cpp dependencies
find_package(PahoMqttCpp REQUIRED)
find_package(eclipse-paho-mqtt-c REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog REQUIRED)

add_library(mqtt_lib mqtt.cpp)

target_include_directories(mqtt_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(mqtt_lib spdlog::spdlog_header_only)
