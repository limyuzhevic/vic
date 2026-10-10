#pragma once

#include "PerformanceTracker.hpp"
#include "LogEntry.hpp"
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <functional>
#include <vector>
#include <algorithm>
#include <ctime>
#include <zlib.h>
#include <fstream>
#include <filesystem>
#include <ctime>

namespace nlm {

class FileLogger : public ILogDestination {
public:
    FileLogger(const std::string& basePath, size_t maxFileSize = 10 * 1024 * 1024, 
               std::chrono::hours maxFileAge = std::chrono::hours(24));
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void flush() override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    bool isEnabled(LogLevel level) const override;
    
    void setRotationStrategy(const std::string& strategy);
    void setCompression(bool enabled);
    void setMaxFileSize(size_t maxSize);
    void setMaxFileAge(std::chrono::hours maxAge);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm