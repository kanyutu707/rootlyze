#include "process.hpp"
#include <iostream>

namespace json = boost::json;

Process::syslogs Process::parseLogs(const std::string &jsonstr) {
    json::value val = json::parse(jsonstr);
    json::object obj = val.as_object();

    syslogs log;

    log.seq = obj["seq"].as_int64();
    log.timestamp = obj["timestamp"].as_string().c_str();
    log.service = obj["service"].as_string().c_str();
    log.level = obj["level"].as_string().c_str();
    log.message = obj["message"].as_string().c_str();

    std::cout << "seq: " << log.seq << "\n"
              << "timestamp: " << log.timestamp << "\n"
              << "service: " << log.service << "\n"
              << "level: " << log.level << "\n"
              << "message: " << log.message << "\n"
              << std::endl;

    return log;
}