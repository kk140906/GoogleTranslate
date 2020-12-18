//
// Created by KK on 2020/12/18.
//

#ifndef GOOGLETRANSLATE_LOGGER_H
#define GOOGLETRANSLATE_LOGGER_H

#include "spdlog//spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"

struct Log {
    static std::shared_ptr<spdlog::logger> logger;

    static void Init() {
        logger = spdlog::daily_logger_mt("logger", "logs/daily_log");
        logger->set_level(spdlog::level::info);
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        logger->flush_on(spdlog::level::info);
    }
};


#endif //GOOGLETRANSLATE_LOGGER_H
