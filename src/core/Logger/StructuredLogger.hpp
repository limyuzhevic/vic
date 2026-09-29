// Structured logger implementation - Enhanced
#pragma once

#include "Logger.hpp"
#include <string>
#include <nlohmann/json.hpp>

namespace nlm {

using json = nlohmann::json;

class StructuredLogger : public BaseLogger {
public:
    StructuredLogger();
    explicit StructuredLogger(LogLevel level);
    ~StructuredLogger() override;
    
    void doCustomLog(const LogEntry& entry) override;
    void flush() override;
    
    // Structured output helpers
    void logJson(const std::string& jsonString);
    void logXml(const std::string& xmlString);
    void logKeyValue(const std::unordered_map<std::string, std::string>& kvPairs);
    
    // Format conversion
    void setJsonOutput(bool enable);
    void setXmlOutput(bool enable);
    void setKeyValueOutput(bool enable);
    
    // JSON helpers
    json logEntryToJson(const LogEntry& entry) const;
    std::string jsonToString(const json& j) const;
    
    // XML helpers
    std::string logEntryToXml(const LogEntry& entry) const;
    std::string xmlEscape(const std::string& input) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm