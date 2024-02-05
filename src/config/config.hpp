#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__
#include <string>

const std::string SERVER_ADDRESS = "mqtt://127.0.0.1:1883";
const std::string CLIENT_ID = "async_client";
const std::string CLIENT_NAME = "ImageClient";

class Config {

public:
  std::string fn_ = "config.toml";
  std::string server_address_, client_id_, client_name_;

  std::string to_string();

  Config();
  Config(std::string fn);
  int Load();
};

#endif // ! __CONFIG_HPP__
