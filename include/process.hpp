#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <boost/json.hpp>
#include <string>

class Process {
public:
  struct syslogs {
    int64_t seq;
    std::string timestamp;
    std::string service;
    std::string level;
    std::string message;
  };
  syslogs parseLogs(const std::string &jsonstr);
};

#endif
