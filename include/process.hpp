#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <boost/json.hpp>
#include <string>
#include <unordered_map>
#include <cstdint>

class Process {
public:
    struct syslogs {
        int64_t seq;
        std::string timestamp;
        std::string service;
        std::string level;
        std::string message;
        std::unordered_map<std::string, std::string> metadata;
    };

    syslogs parseLogs(const std::string &jsonstr);
};

#endif