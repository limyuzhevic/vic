#include "Logger.hpp"
#include <json/json.h> // Assuming nlohmann/json is available
#include <tinyxml2.h> // Assuming tinyxml2 is available
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace nlm {

struct StructLogger::Impl {
    OutputFormat outputFormat = OutputFormat::JSON;
    bool enableCompression = false;
    int indentation = 2;
    
    // Category filtering
    std::unordered_map<std::string, bool> categoryEnabled;
    LogLevel level = LogLevel::Info;
    std::shared_ptr<LogFormatter> formatter;
};

StructLogger::StructLogger()
    : pImpl(std::make_unique<Impl>()) {
    // Initialize category filters
    pImpl->categoryEnabled[LogCategory::Default] = true;
    pImpl->categoryEnabled[LogCategory::Core] = true;
    pImpl->categoryEnabled[LogCategory::Memory] = true;
    pImpl->categoryEnabled[LogCategory::Brain] = true;
    pImpl->categoryEnabled[LogCategory::Motor] = true;
    pImpl->categoryEnabled[LogCategory::Sensory] = true;
    pImpl->categoryEnabled[LogCategory::Cognition] = true;
    pImpl->categoryEnabled[LogCategory::Development] = true;
    pImpl->categoryEnabled[LogCategory::Plasticity] = true;
    pImpl->categoryEnabled[LogCategory::Neuromodulation] = true;
    pImpl->categoryEnabled[LogCategory::Prediction] = true;
    pImpl->categoryEnabled[LogCategory::ErrorHandling] = true;
    pImpl->categoryEnabled[LogCategory::Config] = true;
    pImpl->categoryEnabled[LogCategory::Plugin] = true;
    pImpl->categoryEnabled[LogCategory::Performance] = true;
    pImpl->categoryEnabled[LogCategory::MemoryAllocation] = true;
    pImpl->categoryEnabled[LogCategory::Threading] = true;
    pImpl->categoryEnabled[LogCategory::Backtrace] = true;
}

StructLogger::StructLogger(OutputFormat format)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->outputFormat = format;
    
    // Initialize category filters
    pImpl->categoryEnabled[LogCategory::Default] = true;
    pImpl->categoryEnabled[LogCategory::Core] = true;
    pImpl->categoryEnabled[LogCategory::Memory] = true;
    pImpl->categoryEnabled[LogCategory::Brain] = true;
    pImpl->categoryEnabled[LogCategory::Motor] = true;
    pImpl->categoryEnabled[LogCategory::Sensory] = true;
    pImpl->categoryEnabled[LogCategory::Cognition] = true;
    pImpl->categoryEnabled[LogCategory::Development] = true;
    pImpl->categoryEnabled[LogCategory::Plasticity] = true;
    pImpl->categoryEnabled[LogCategory::Neuromodulation] = true;
    pImpl->categoryEnabled[LogCategory::Prediction] = true;
    pImpl->categoryEnabled[LogCategory::ErrorHandling] = true;
    pImpl->categoryEnabled[LogCategory::Config] = true;
    pImpl->categoryEnabled[LogCategory::Plugin] = true;
    pImpl->categoryEnabled[LogCategory::Performance] = true;
    pImpl->categoryEnabled[LogCategory::MemoryAllocation] = true;
    pImpl->categoryEnabled[LogCategory::Threading] = true;
    pImpl->categoryEnabled[LogCategory::Backtrace] = true;
}

StructLogger::~StructLogger() = default;

void StructLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    if (!isCategoryEnabled(entry.category)) {
        return;
    }
    
    std::string formattedMessage;
    if (pImpl->formatter) {
        formattedMessage = pImpl->formatter->format(entry);
    } else {
        // Use built-in formatting based on output format
        switch (pImpl->outputFormat) {
            case OutputFormat::JSON:
                formattedMessage = formatAsJSON(entry);
                break;
            case OutputFormat::XML:
                formattedMessage = formatAsXML(entry);
                break;
            case OutputFormat::YAML:
                formattedMessage = formatAsYAML(entry);
                break;
        }
    }
    
    // In a real implementation, this would send to the actual logging system
    // For now, we just track metrics
    PerformanceMetrics::recordLog(entry, false, 0.0);
}

void StructLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel StructLogger::getLevel() const {
    return pImpl->level;
}

void StructLogger::setCategoryFilter(const std::string& category) {
    // Simple implementation: enable/disable specific category
    for (auto& pair : pImpl->categoryEnabled) {
        pair.second = (pair.first == category);
    }
}

void StructLogger::addCategory(const std::string& category) {
    pImpl->categoryEnabled[category] = true;
}

void StructLogger::removeCategory(const std::string& category) {
    pImpl->categoryEnabled.erase(category);
}

bool StructLogger::isCategoryEnabled(const std::string& category) const {
    auto it = pImpl->categoryEnabled.find(category);
    return (it != pImpl->categoryEnabled.end()) && it->second;
}

void StructLogger::setFormatter(std::shared_ptr<LogFormatter> formatter) {
    pImpl->formatter = formatter;
}

std::shared_ptr<LogFormatter> StructLogger::getFormatter() const {
    return pImpl->formatter;
}

void StructLogger::setStructuredFormat(OutputFormat format) {
    pImpl->outputFormat = format;
}

void StructLogger::enableCompression(bool enabled) {
    pImpl->enableCompression = enabled;
}

void StructLogger::setIndentation(int spaces) {
    pImpl->indentation = spaces;
}

std::string StructLogger::formatAsJSON(const LogEntry& entry) {
    // Using rapidjson for JSON formatting
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    
    writer.StartObject();
    writer.Key("timestamp");
    writer.String(entry.getFormattedTimestamp().c_str());
    
    writer.Key("level");
    writer.Int(static_cast<int>(entry.level));
    
    writer.Key("category");
    writer.String(entry.category.c_str());
    
    writer.Key("message");
    writer.String(entry.message.c_str());
    
    if (!entry.file.empty()) {
        writer.Key("file");
        writer.String(entry.file.c_str());
    }
    
    if (entry.line > 0) {
        writer.Key("line");
        writer.Int(entry.line);
    }
    
    if (!entry.function.empty()) {
        writer.Key("function");
        writer.String(entry.function.c_str());
    }
    
    if (!entry.threadId.empty()) {
        writer.Key("thread_id");
        writer.String(entry.threadId.c_str());
    }
    
    if (!entry.metadata.empty()) {
        writer.Key("metadata");
        writer.StartObject();
        for (const auto& pair : entry.metadata) {
            writer.Key(pair.first.c_str());
            writer.String(pair.second.c_str());
        }
        writer.EndObject();
    }
    
    writer.EndObject();
    
    return buffer.GetString();
}

std::string StructLogger::formatAsXML(const LogEntry& entry) {
    // Simple XML formatting (in real implementation use proper XML library)
    std::stringstream ss;
    ss << "<log>" << std::endl;
    ss << "  <timestamp>" << entry.getFormattedTimestamp() << "</timestamp>" << std::endl;
    ss << "  <level>" << static_cast<int>(entry.level) << "</level>" << std::endl;
    ss << "  <category>" << entry.category << "</category>" << std::endl;
    ss << "  <message>" << entry.message << "</message>" << std::endl;
    
    if (!entry.file.empty()) {
        ss << "  <file>" << entry.file << "</file>" << std::endl;
    }
    
    if (entry.line > 0) {
        ss << "  <line>" << entry.line << "</line>" << std::endl;
    }
    
    if (!entry.function.empty()) {
        ss << "  <function>" << entry.function << "</function>" << std::endl;
    }
    
    if (!entry.threadId.empty()) {
        ss << "  <thread_id>" << entry.threadId << "</thread_id>" << std::endl;
    }
    
    if (!entry.metadata.empty()) {
        ss << "  <metadata>" << std::endl;
        for (const auto& pair : entry.metadata) {
            ss << "    <" << pair.first << ">" << pair.second << "</" << pair.first << ">" << std::endl;
        }
        ss << "  </metadata>" << std::endl;
    }
    
    ss << "</log>" << std::endl;
    
    return ss.str();
}

std::string StructLogger::formatAsYAML(const LogEntry& entry) {
    // Simple YAML formatting
    std::stringstream ss;
    ss << "timestamp: " << entry.getFormattedTimestamp() << std::endl;
    ss << "level: " << static_cast<int>(entry.level) << std::endl;
    ss << "category: " << entry.category << std::endl;
    ss << "message: " << entry.message << std::endl;
    
    if (!entry.file.empty()) {
        ss << "file: " << entry.file << std::endl;
    }
    
    if (entry.line > 0) {
        ss << "line: " << entry.line << std::endl;
    }
    
    if (!entry.function.empty()) {
        ss << "function: " << entry.function << std::endl;
    }
    
    if (!entry.threadId.empty()) {
        ss << "thread_id: " << entry.threadId << std::endl;
    }
    
    if (!entry.metadata.empty()) {
        ss << "metadata:" << std::endl;
        for (const auto& pair : entry.metadata) {
            ss << "  " << pair.first << ": " << pair.second << std::endl;
        }
    }
    
    return ss.str();
}