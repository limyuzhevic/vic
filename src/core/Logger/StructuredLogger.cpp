// Structured logger implementation - Enhanced
#include "StructuredLogger.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <nlohmann/json.hpp>

namespace nlm {

using json = nlohmann::json;

struct StructuredLogger::Impl {
    bool jsonOutput = false;
    bool xmlOutput = false;
    bool keyValueOutput = false;
    std::mutex mutex;
};

StructuredLogger::StructuredLogger() : BaseLogger() {
    pImpl.reset(new Impl);
}

StructuredLogger::StructuredLogger(LogLevel level) : BaseLogger(level) {
    pImpl.reset(new Impl);
}

StructuredLogger::~StructuredLogger() = default;

void StructuredLogger::doCustomLog(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Convert to requested format
    std::string output;
    if (pImpl->jsonOutput) {
        output = jsonToString(logEntryToJson(entry));
    } else if (pImpl->xmlOutput) {
        output = logEntryToXml(entry);
    } else if (pImpl->keyValueOutput) {
        // Convert to key-value format
        std::ostringstream oss;
        oss << entry.message;
        if (!entry.context.empty()) {
            oss << " {";
            bool first = true;
            for (const auto& pair : entry.context) {
                if (!first) oss << ", ";
                oss << pair.first << "=" << pair.second;
                first = false;
            }
            oss << "}";
        }
        output = oss.str();
    } else {
        // Default to formatted text
        output = getFormatter()->format(entry);
    }
    
    std::cout << output << std::endl;
}

void StructuredLogger::flush() {
    // No buffering in this implementation
}

void StructuredLogger::logJson(const std::string& jsonString) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::cout << jsonString << std::endl;
}

void StructuredLogger::logXml(const std::string& xmlString) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::cout << xmlString << std::endl;
}

void StructuredLogger::logKeyValue(const std::unordered_map<std::string, std::string>& kvPairs) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    bool first = true;
    for (const auto& pair : kvPairs) {
        if (!first) oss << " ";
        oss << pair.first << "=" << pair.second;
        first = false;
    }
    std::cout << oss.str() << std::endl;
}

void StructuredLogger::setJsonOutput(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->jsonOutput = enable;
    if (enable) {
        pImpl->xmlOutput = false;
        pImpl->keyValueOutput = false;
    }
}

void StructuredLogger::setXmlOutput(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->xmlOutput = enable;
    if (enable) {
        pImpl->jsonOutput = false;
        pImpl->keyValueOutput = false;
    }
}

void StructuredLogger::setKeyValueOutput(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->keyValueOutput = enable;
    if (enable) {
        pImpl->jsonOutput = false;
        pImpl->xmlOutput = false;
    }
}

json StructuredLogger::logEntryToJson(const LogEntry& entry) const {
    json j;
    j["timestamp"] = entry.getTimestamp();
    j["level"] = levelToString(entry.level);
    j["message"] = entry.message;
    j["file"] = entry.file;
    j["line"] = entry.line;
    j["function"] = entry.function;
    j["thread_id"] = entry.threadId;
    j["process_id"] = entry.processId;
    
    if (!entry.context.empty()) {
        j["context"] = entry.context;
    }
    
    return j;
}

std::string StructuredLogger::jsonToString(const json& j) const {
    return j.dump(2);
}

std::string StructuredLogger::logEntryToXml(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "<log>";
    oss << "<timestamp>" << xmlEscape(entry.getTimestamp()) << "</timestamp>";
    oss << "<level>" << xmlEscape(levelToString(entry.level)) << "</level>";
    oss << "<message>" << xmlEscape(entry.message) << "</message>";
    oss << "<file>" << xmlEscape(entry.file) << "</file>";
    oss << "<line>" << entry.line << "</line>";
    oss << "<function>" << xmlEscape(entry.function) << "</function>";
    oss << "<thread_id>" << entry.threadId << "</thread_id>";
    oss << "<process_id>" << entry.processId << "</process_id>";
    
    if (!entry.context.empty()) {
        oss << "<context>";
        for (const auto& pair : entry.context) {
            oss << "<" << xmlEscape(pair.first) << ">" << xmlEscape(pair.second) << "</" << xmlEscape(pair.first) << ">";
        }
        oss << "</context>";
    }
    
    oss << "</log>";
    return oss.str();
}

std::string StructuredLogger::xmlEscape(const std::string& input) const {
    std::ostringstream oss;
    for (char c : input) {
        switch (c) {
            case '&': oss << "&amp;"; break;
            case '<': oss << "&lt;"; break;
            case '>': oss << "&gt;"; break;
            case '"': oss << "&quot;"; break;
            case '\'': oss << "&apos;"; break;
            default: oss << c;
        }
    }
    return oss.str();
}

} // namespace nlm