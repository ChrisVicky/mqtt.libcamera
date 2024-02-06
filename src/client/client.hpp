#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <mqtt/async_client.h>
#include <mqtt/callback.h>
#include <mqtt/connect_options.h>
#include <mqtt/exception.h>

#include <mqtt/iaction_listener.h>
#include <mqtt/message.h>
#include <spdlog/spdlog.h>

#include "../loop/loop.hpp"

#include "camera/camera.hpp"
#include "mqtt/mqtt.hpp"

extern std::shared_ptr<spdlog::logger> logger;

class Client {

private:
  CameraClient *camera_;
  MqttClient *mqtt_;

  std::thread thread_;
  std::mutex locker_;
  std::vector<uint8_t> rgb_;

  bool run_ = false;

  Loop loop_;

  std::map<std::string, std::function<void(const mqtt::const_message_ptr msg)>>
      funcMap_ = {
          {"/hello",
           [this](const mqtt::const_message_ptr msg) {
             logger->info("Hello There");
           }},
          {"/start",
           [this](const mqtt::const_message_ptr msg) {
             logger->info("Start Pipelining");
             loop_.start();
           }},
          {"/stop",
           [this](const mqtt::const_message_ptr msg) {
             logger->info("Stop Pipelining");
             loop_.stop();
           }},
  };

public:
  std::function<void(const mqtt::const_message_ptr msg)> GetHandler();
  Client();
  Client(std::string addr, std::string id, std::string name);
  void start() {

    mqtt_->connect();

    logger->info("Setting Routine, Publishing image data via `/data`");
    loop_.routine([=]() {
      rgb_ = camera_->GetImageMqtt(); // Last two Bytes are Height and Width
      mqtt_->publish("/data", std::string(rgb_.begin(), rgb_.end()));
      logger->trace("Publish Data through /data");
    });
  }

  void exit() { mqtt_->disconnect(); }
};

#endif // !__CALLBACK_HPP__
