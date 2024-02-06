#include "client.hpp"
#include "camera/camera.hpp"
#include <mqtt/message.h>

std::function<void(const mqtt::const_message_ptr msg)> Client::GetHandler() {
  return [this](const mqtt::const_message_ptr msg) {
    std::string url = msg->get_topic();
    logger->info("Receive Msg from: {}", url);
    if (funcMap_.find(url) != funcMap_.end()) {
      funcMap_[url](msg);
    } else {
      logger->warn("No matching handler for {}", url);
    }
  };
}

Client::Client() {}

Client::Client(std::string addr, std::string id, std::string name) {
  mqtt_ = new MqttClient(addr, id, name, GetHandler());
  camera_ = new CameraClient();
}
