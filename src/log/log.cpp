#include "log.hpp"
#include <spdlog/common.h>

std::shared_ptr<spdlog::logger> logger = LoggerSetup();

std::shared_ptr<spdlog::logger> LoggerSetup() {

  const std::string spd_fmt = "%^[%Y-%m-%d %H:%M:%S.%e] [%t] [%l] %v%$";
  std::shared_ptr<spdlog::logger> logger;

  std::string LogFile("log.txt");
  spdlog::init_thread_pool(8192, 1);

  auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  stdout_sink->set_pattern(spd_fmt);
  stdout_sink->set_level(spdlog::level::debug);

  auto file_sink =
      std::make_shared<spdlog::sinks::basic_file_sink_mt>(LogFile, false);
  file_sink->set_level(spdlog::level::trace);
  file_sink->set_pattern(spd_fmt);

  std::vector<spdlog::sink_ptr> sinks{stdout_sink, file_sink};

#ifdef Async
  logger = std::make_shared<spdlog::async_logger>(
      "Main", sinks.begin(), sinks.end(), spdlog::thread_pool(),
      spdlog::async_overflow_policy::block);

  spdlog::register_logger(logger);
#else
  logger = std::make_shared<spdlog::logger>("Main", sinks.begin(), sinks.end());
#endif

  logger->set_level(spdlog::level::trace);

  spdlog::register_logger(logger);
  return logger;
}
