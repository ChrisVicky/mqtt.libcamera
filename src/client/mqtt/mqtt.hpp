#ifndef __MQTTCLIENT_HPP__
#define __MQTTCLIENT_HPP__

#include <mqtt/async_client.h>
#include <mqtt/callback.h>
#include <mqtt/connect_options.h>

#include <mqtt/message.h>
#include <spdlog/logger.h>

extern std::shared_ptr<spdlog::logger> logger;

class MqttClient {
  mqtt::async_client *client_;
  mqtt::connect_options connOpts;
  std::string server_address_;
  std::string client_id_;
  std::string client_name_;

public:
  MqttClient(std::string addr, std::string id, std::string name,
             mqtt::async_client::message_handler cb);
  MqttClient(){};
  void connect();
  void disconnect();
  void set_callback(mqtt::callback &cb);
  void set_message_handler(
      std::function<void(const mqtt::const_message_ptr msg)>);
  void publish(std::string url, std::string payload) {
    client_->publish(url, payload);
  }

  void handle();
};

#endif // !__MQTTCLIENT_HPP__
