#include "process.hpp"
#include <boost/json.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <cstdio>

namespace json = boost::json;

enum class SeverityLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    UNKNOWN
};

struct LogEvent {
    int64_t seq{};
    std::string timestamp;
    std::string service;
    SeverityLevel level{SeverityLevel::UNKNOWN};
    std::string message;
    std::unordered_map<std::string, std::string> metadata;
};

static std::string severityToString(SeverityLevel lvl) {
    switch (lvl) {
        case SeverityLevel::DEBUG: return "DEBUG";
        case SeverityLevel::INFO:  return "INFO";
        case SeverityLevel::WARN:  return "WARN";
        case SeverityLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

static std::string normalizeTime(const std::string& t) {
    if (t.size() < 8) return "00:00:00";

    int hr = std::stoi(t.substr(0, 2));
    int min = std::stoi(t.substr(3, 2));
    int sec = std::stoi(t.substr(6, 2));

    min += sec / 60;
    sec %= 60;

    hr += min / 60;
    min %= 60;

    hr %= 24;

    char buffer[9];
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hr, min, sec);
    return std::string(buffer);
}

static SeverityLevel normalizeSeverity(std::string lvl) {
    std::transform(lvl.begin(), lvl.end(), lvl.begin(),
        [](unsigned char c){ return std::toupper(c); });

    if (lvl == "WARN" || lvl == "WARNING") return SeverityLevel::WARN;
    if (lvl == "ERROR" || lvl == "ERR") return SeverityLevel::ERROR;
    if (lvl == "INFO") return SeverityLevel::INFO;
    if (lvl == "DEBUG") return SeverityLevel::DEBUG;

    return SeverityLevel::UNKNOWN;
}

static std::string getString(const json::object& obj, const char* key) {
    auto it = obj.find(key);
    if (it == obj.end() || !it->value().is_string()) return "";
    return it->value().as_string().c_str();
}

static int64_t getInt(const json::object& obj, const char* key) {
    auto it = obj.find(key);
    if (it == obj.end() || !it->value().is_int64()) return 0;
    return it->value().as_int64();
}

static std::unordered_map<std::string, std::string>
extractMetadata(const LogEvent& ev) {
    std::unordered_map<std::string, std::string> meta;

    const std::string& msg = ev.message;

    if (msg.find("Query took") != std::string::npos) {
        size_t s = msg.find("Query took") + 11;
        size_t e = msg.find("ms", s);
        if (e != std::string::npos)
            meta["latency_ms"] = msg.substr(s, e - s);
    }

    if (msg.find("Batch processed") != std::string::npos) {
        size_t s = msg.find("Batch processed") + 16;
        size_t e = msg.find("records", s);
        if (e != std::string::npos)
            meta["records"] = msg.substr(s, e - s - 1);
    }

    if (ev.service == "cache" && msg.find("Cache miss") != std::string::npos)
        meta["cache_event"] = "miss";

    return meta;
}

static LogEvent toEvent(const json::object& obj) {
    LogEvent ev;

    ev.seq = getInt(obj, "seq");
    ev.timestamp = normalizeTime(getString(obj, "timestamp"));
    ev.service = getString(obj, "service");
    ev.level = normalizeSeverity(getString(obj, "level"));
    ev.message = getString(obj, "message");

    ev.metadata = extractMetadata(ev);

    return ev;
}

Process::syslogs Process::parseLogs(const std::string &jsonstr) {
    try {
        json::value val = json::parse(jsonstr);
        json::object obj = val.as_object();

        LogEvent ev = toEvent(obj);

        if (ev.message.empty()) {
            std::cerr << "Warning: Dropping invalid log\n";
            return {};
        }

        std::cout << "Forwarding seq=" << ev.seq
                  << " service=" << ev.service << std::endl;

        syslogs log;
        log.seq = ev.seq;
        log.timestamp = ev.timestamp;
        log.service = ev.service;
        log.level = severityToString(ev.level);
        log.message = ev.message;
        log.metadata = ev.metadata;

        std::cout << "seq: " << log.seq << "\n"
                  << "timestamp: " << log.timestamp << "\n"
                  << "service: " << log.service << "\n"
                  << "level: " << log.level << "\n"
                  << "message: " << log.message << "\n"
                  << std::endl;

        return log;
    }
    catch (...) {
        std::cerr << "Warning: Dropping invalid JSON log\n";
        return {};
    }
}