
#include "src/client/client.hpp"
#include "src/config/config.hpp"

extern std::shared_ptr<spdlog::logger> logger;

int main() {

  logger->trace("Hello Trace");
  logger->debug("Hello Debug");
  logger->info("Hello Info");
  logger->warn("Hello Warn");
  logger->error("Hello Error");
  logger->critical("Hello Critical");

  Config conf = Config("config.toml");
  int ret = 0;
  if ((ret = conf.Load())) {
    logger->error("Error Open {}", conf.fn_);
    return 1;
  }

  logger->info("Config: {}", conf.to_string());

  Client c(conf.server_address_, conf.client_id_, conf.client_name_);

  c.start();

  while (std::tolower(std::cin.get()) != 'q')
    ;

  c.exit();

  return 0;
}
