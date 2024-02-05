#include "config.hpp"
#include "toml.hpp"
#include <iostream>
#include <sstream>

Config::Config(std::string fn) { fn_ = fn; }

int Config::Load() {
  try {
    auto c = toml::parse_file(fn_);

    server_address_ = c["server"].value_or(SERVER_ADDRESS);
    client_id_ = c["id"].value_or(CLIENT_ID);
    client_name_ = c["name"].value_or(CLIENT_NAME);

  } catch (toml::parse_error &e) {
    std::cerr << e << std::endl;
    return 1;
  }
  return 0;
}

std::string Config::to_string() {
  std::stringstream strstr;
  strstr << "{fn:`" << fn_ << "`, server:`" << server_address_ << "`, id:`"
         << client_id_ << "`, name:`" << client_name_ << "`}";
  return strstr.str();
}
