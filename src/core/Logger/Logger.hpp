#pragma once

#include "Logger.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

namespace nlm {

class ConsoleLogger : public Logger {
public:
    ConsoleLogger(LogLevel level = LogLevel::Info);
    ~ConsoleLogger() override;
    
    void log(const std::string& message, LogLevel level = LogLevel::Info) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Format timestamp
    std::string formatTimestamp() const;
    
    // Format log level
    std::string formatLogLevel(LogLevel level) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class FileLogger : public Logger {
public:
    FileLogger(const std::string& filepath, LogLevel level = LogLevel::Info);
    ~FileLogger() override;
    
    void log(const std::string& message, LogLevel level = LogLevel::Info) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Flush logs to file
    void flush() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm