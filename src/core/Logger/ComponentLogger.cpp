#include "ComponentLogger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>

namespace nlm {

struct ComponentLogger::Impl {
    std::string componentName;
    std::string parentComponent;
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::vector<std::shared_ptr<ComponentLogger>> childLoggers;
    std::function<bool(const std::string&, const LogEntry&)> filter;
    bool enabled = true;
    std::string format;
};

ComponentLogger::ComponentLogger(const std::string& componentName, const std::string& parentComponent)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->componentName = componentName;
    pImpl->parentComponent = parentComponent;
}

ComponentLogger::ComponentLogger(const std::string& componentName, const std::string& parentComponent, LogLevel level)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->componentName = componentName;
    pImpl->parentComponent = parentComponent;
    pImpl->level = level;
}

void ComponentLogger::log(const LogEntry& entry) {
    if (!pImpl->enabled) return;
    if (entry.level < pImpl->level) return;
    if (pImpl->filter && !pImpl->filter(pImpl->componentName, entry)) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if this component should log this entry based on hierarchy
    if (!shouldLog(entry.component)) return;
    
    std::string formatted = formatEntry(entry);
    
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << formatted << "\033[0m\n";
    } else {
        std::cout << formatted << "\n";
    }
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formatted << "\n";
        pImpl->fileOut.flush();
    }
}

void ComponentLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ComponentLogger::getLevel() const {
    return pImpl->level;
}

void ComponentLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ComponentLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ComponentLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ComponentLogger::getUseColors() const {
    return pImpl->useColors;
}

void ComponentLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

void ComponentLogger::setMinimumLevel(LogLevel level) {
    pImpl->level = level;
}

const std::string& ComponentLogger::getComponentName() const {
    return pImpl->componentName;
}

const std::string& ComponentLogger::getParentComponent() const {
    return pImpl->parentComponent;
}

void ComponentLogger::addChildLogger(std::shared_ptr<ComponentLogger> childLogger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->childLoggers.push_back(childLogger);
}

void ComponentLogger::clearChildLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->childLoggers.clear();
}

const std::vector<std::shared_ptr<ComponentLogger>>& ComponentLogger::getChildLoggers() const {
    return pImpl->childLoggers;
}

bool ComponentLogger::shouldLog(const std::string& componentName) const {
    if (componentName.empty() || componentName == pImpl->componentName) {
        return true;
    }
    
    // Check child components
    for (const auto& child : pImpl->childLoggers) {
        if (child->shouldLog(componentName)) {
            return true;
        }
    }
    
    return false;
}

void ComponentLogger::setFilter(const std::function<bool(const std::string&, const LogEntry&)>& filter) {
    pImpl->filter = filter;
}

void ComponentLogger::setEnabled(bool enabled) {
    pImpl->enabled = enabled;
}

bool ComponentLogger::isEnabled() const {
    return pImpl->enabled;
}

std::string ComponentLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string ComponentLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

std::string ComponentLogger::formatEntry(const LogEntry& entry) const {
    if (pImpl->format.empty()) {
        // Default format
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        oss << " [" << entry.component << "] " << entry.message;
        return oss.str();
    }
    
    // For simplicity, just use default format for custom format
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << " [" << entry.component << "] " << entry.message;
    return oss.str();
}

} // namespace nlm
