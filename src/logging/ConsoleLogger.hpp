#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <functional>
#include <vector>
#include <algorithm>
#include <ctime>

namespace nlm {

class ConsoleLoggerImpl {
public:
    ConsoleLoggerImpl() : level(LogLevel::INFO), useColors(true) {}
    
    void log(const LogEntry& entry, std::ostream& output = std::cout) {
        if (entry.level < level) {
            return;
        }
        
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        oss << " " << entry.message;
        
        if (!entry.metadata.empty()) {
            oss << " [";
            bool first = true;
            for (const auto& pair : entry.metadata) {
                if (!first) oss << ", ";
                oss << pair.first << "=" << pair.second;
                first = false;
            }
            oss << "]";
        }
        
        std::string outputStr = oss.str();
        
        if (useColors) {
            output << levelToColor(entry.level) << outputStr << "\033[0m";
        } else {
            output << outputStr;
        }
        output << std::endl;
    }
    
    void setLevel(LogLevel lvl) { level = lvl; }
    LogLevel getLevel() const { return level; }
    void setUseColors(bool uc) { useColors = uc; }
    bool getUseColors() const { return useColors; }
    
private:
    LogLevel level;
    bool useColors;
    mutable std::mutex mutex;
    
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::TRACE: return "TRACE";
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARN: return "WARN";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
    std::string levelToColor(LogLevel level) const {
        switch (level) {
            case LogLevel::TRACE: return "\033[37m";    // White
            case LogLevel::DEBUG: return "\033[36m";    // Cyan
            case LogLevel::INFO: return "\033[32m";     // Green
            case LogLevel::WARN: return "\033[33m";     // Yellow
            case LogLevel::ERROR: return "\033[31m";    // Red
            case LogLevel::FATAL: return "\033[1;31m";  // Bold Red
            default: return "\033[0m";
        }
    }
};

class ConsoleLogger : public ILogDestination {
public:
    ConsoleLogger() : pImpl(std::make_unique<ConsoleLoggerImpl>()) {}
    explicit ConsoleLogger(LogLevel level) : pImpl(std::make_unique<ConsoleLoggerImpl>()) {
        pImpl->setLevel(level);
    }
    
    void log(const LogEntry& entry) override {
        pImpl->log(entry);
    }
    
    void flush() override {}
    
    void setLevel(LogLevel level) override {
        pImpl->setLevel(level);
    }
    
    LogLevel getLevel() const override {
        return pImpl->getLevel();
    }
    
    bool isEnabled(LogLevel level) const override {
        return level >= pImpl->getLevel();
    }
    
    void setUseColors(bool useColors) {
        pImpl->setUseColors(useColors);
    }
    
    bool getUseColors() const {
        return pImpl->getUseColors();
    }
    
private:
    std::unique_ptr<ConsoleLoggerImpl> pImpl;
};

} // namespace nlm