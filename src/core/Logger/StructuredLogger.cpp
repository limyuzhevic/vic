#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>

namespace nlm {

StructuredLogger::StructuredLogger(const std::string& filepath, bool append) 
    : filepath(filepath), appendMode(append) {
    std::ofstream file(filepath, append ? std::ios::app : std::ios::trunc);
    if (file.is_open()) {
        file.close();
    }
    
    fileOut.open(filepath, std::ios::app);
    lastFlushTime = std::chrono::system_clock::now();
}

StructuredLogger::~StructuredLogger() {
    flush();
    if (fileOut.is_open()) {
        fileOut.close();
    }
}

void StructuredLogger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex);
    fileOut << entry.toJson().dump() << "\n";
    fileOut.flush();
    
    // Auto-flush every second
    auto now = std::chrono::system_clock::now;
    if (std::chrono::duration_cast<std::chrono::seconds>(now() - lastFlushTime).count() >= 1) {
        flush();
        lastFlushTime = now();
    }
}

void StructuredLogger::setLevel(LogLevel level) {
    this->level = level;
}

LogLevel StructuredLogger::getLevel() const {
    return level;
}

void StructuredLogger::logJson(const json& entry) {
    std::lock_guard<std::mutex> lock(mutex);
    fileOut << entry.dump() << "\n";
    fileOut.flush();
}

void StructuredLogger::flush() {
    std::lock_guard<std::mutex> lock(mutex);
    fileOut.flush();
}

void StructuredLogger::close() {
    std::lock_guard<std::mutex> lock(mutex);
    if (fileOut.is_open()) {
        fileOut.close();
    }
}

bool StructuredLogger::isOpen() const {
    return fileOut.is_open();
}

} // namespace nlm
