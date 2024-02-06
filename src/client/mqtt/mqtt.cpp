#include "mqtt.hpp"
#include <MQTTAsync.h>
#include <mqtt/async_client.h>
#include <mqtt/callback.h>
#include <mqtt/message.h>

MqttClient::MqttClient(std::string addr, std::string id, std::string name,
                       mqtt::async_client::message_handler cb) {
  server_address_ = addr;
  client_id_ = id;
  client_name_ = name;

  client_ = new mqtt::async_client(server_address_, client_id_);

  auto lwt = mqtt::message(
      "/exit", "<<<" + client_name_ + " was disconnected>>>", 1, false);

  connOpts = mqtt::connect_options_builder().will(std::move(lwt)).finalize();

  // Set callback
  client_->set_connection_lost_handler([this](const std::string &cause) {
    logger->error("Connection lost");
    if (!cause.empty())
      logger->info("cause: {}", cause);
    logger->info("Reconnecting...");
    client_->reconnect();
  });

  client_->set_connected_handler([this](const std::string &cause) {
    logger->info("Connected to {}", server_address_);
  });

  logger->info("Setup Mes Callback");
  client_->set_message_callback(cb);
}

void MqttClient::set_callback(mqtt::callback &cb) { client_->set_callback(cb); }

void MqttClient::connect() {
  try {
    logger->info("Connecting {} ", server_address_);
    client_->connect(connOpts)->wait();
  } catch (const mqtt::exception &exc) {
    logger->error("ERROR: Unable to connect to MQTT server: '{}' {}",
                  server_address_, exc.to_string());
    exit(1);
  }
}

void MqttClient::disconnect() {
  try {
    logger->info("Disconnecting...");
    client_->disconnect()->wait();
  } catch (const mqtt::exception &exc) {
    logger->error("disconnect Error: {}", exc.to_string());
    exit(1);
  }
}
