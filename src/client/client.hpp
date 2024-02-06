#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <chrono>
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

using namespace std;
using namespace std::chrono;
class FPSCounter {

private:
  high_resolution_clock::time_point lastTime;
  int frameCount;
  double fps;

public:
  FPSCounter() : frameCount(0), fps(0.0) {
    lastTime = high_resolution_clock::now();
  }

  // 更新帧计数器并计算FPS（如果适用）
  double update() {
    ++frameCount;

    auto currentTime = high_resolution_clock::now();
    double elapsedTime =
        duration_cast<duration<double>>(currentTime - lastTime).count();

    // 如果超过1秒，则计算FPS
    if (elapsedTime >= 1.0) {
      fps = frameCount / elapsedTime;
      frameCount = 0;
      lastTime = currentTime;
    }

    return fps; // 返回最新的FPS值（如果还没更新则返回上一秒的FPS）
  }
};

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

  FPSCounter fps_;

public:
  std::function<void(const mqtt::const_message_ptr msg)> GetHandler();
  Client();
  Client(std::string addr, std::string id, std::string name);

  void start() {

    mqtt_->connect();

    for (auto &[k, v] : funcMap_) {
      mqtt_->subscribe(k);
    }

    logger->info("Setting Routine, Publishing image data via `/data`");
    loop_.routine([=]() {
      logger->info("Fps: {}", fps_.update());
      rgb_ = camera_->GetImageMqtt(); // Last two Bytes are Height and Width
      logger->debug("Publishing Data through `/data`");
      mqtt_->publish("/data", std::string(rgb_.begin(), rgb_.end()));
      logger->trace("`/data` Published");
    });
  }

  void exit() { mqtt_->disconnect(); }
};

#endif // !__CALLBACK_HPP__
