#ifndef __LOG_HPP__
#define __LOG_HPP__
#include <spdlog/async.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

std::shared_ptr<spdlog::logger> LoggerSetup();

#endif // __LOG_HPP__
