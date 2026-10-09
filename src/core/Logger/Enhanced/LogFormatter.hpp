#pragma once

#include <string>
#include <chrono>
#include <map>
#include <sstream>
#include <memory>
#include <iomanip>
#include "Logger.hpp"

namespace nlm {

// Enhanced log message with additional metadata
struct EnhancedLogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::map<std::string, std::string> tags;
    std::map<std::string, std::string> metadata;
    std::uint64_t sequenceNumber;
};

// Enhanced formatter base class
class LogFormatter {
public:
    virtual ~LogFormatter() = default;
    virtual std::string format(const EnhancedLogEntry& entry) const = 0;
    virtual std::string getName() const = 0;
};

// Template-based formatter using curly braces
class TemplateLogFormatter : public LogFormatter {
public:
    explicit TemplateLogFormatter(const std::string& templateStr) : templateStr_(templateStr) {}
    
    std::string format(const EnhancedLogEntry& entry) const override {
        std::string result = templateStr_;
        replacePlaceholder(result, "{timestamp}", formatTimestamp(entry.timestamp));
        replacePlaceholder(result, "{level}", levelToString(entry.level));
        replacePlaceholder(result, "{message}", entry.message);
        replacePlaceholder(result, "{file}", entry.file);
        replacePlaceholder(result, "{line}", std::to_string(entry.line));
        replacePlaceholder(result, "{function}", entry.function);
        replacePlaceholder(result, "{thread_id}", entry.threadId);
        for (const auto& tag : entry.tags) {
            replacePlaceholder(result, "{" + tag.first + "}", tag.second);
        }
        for (const auto& meta : entry.metadata) {
            replacePlaceholder(result, "{" + meta.first + "}", meta.second);
        }
        return result;
    }
    
    std::string getName() const override { return "TemplateLogFormatter"; }
    
    void setTemplate(const std::string& templateStr) { templateStr_ = templateStr; }
    std::string getTemplate() const { return templateStr_; }
    
private:
    void replacePlaceholder(std::string& str, const std::string& placeholder, const std::string& value) const {
        size_t pos = 0;
        while ((pos = str.find(placeholder, pos)) != std::string::npos) {
            str.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%f");
        return ss.str();
    }
    
    std::string templateStr_;
};

// JSON formatter for structured logging
class JsonLogFormatter : public LogFormatter {
public:
    std::string format(const EnhancedLogEntry& entry) const override {
        std::stringstream json;
        json << "{";
        json << "\"timestamp\": \"" << formatTimestamp(entry.timestamp) << "\", ";
        json << "\"level\": \"" << levelToString(entry.level) << "\", ";
        json << "\"message\": \"" << escapeJson(entry.message) << "\", ";
        json << "\"file\": \"" << escapeJson(entry.file) << "\", ";
        json << "\"line\": " << entry.line << ", ";
        json << "\"function\": \"" << escapeJson(entry.function) << "\", ";
        json << "\"thread_id\": \"" << escapeJson(entry.threadId) << "\", ";
        
        if (!entry.tags.empty()) {
            json << "\"tags\": {";
            bool first = true;
            for (const auto& tag : entry.tags) {
                if (!first) json << ", ";
                first = false;
                json << "\"" << escapeJson(tag.first) << "\": \"" << escapeJson(tag.second) << "\"";
            }
            json << "}, ";
        }
        
        if (!entry.metadata.empty()) {
            json << "\"metadata\": {";
            bool first = true;
            for (const auto& meta : entry.metadata) {
                if (!first) json << ", ";
                first = false;
                json << "\"" << escapeJson(meta.first) << "\": \"" << escapeJson(meta.second) << "\"";
            }
            json << "}, ";
        }
        
        json << "\"sequence\": " << entry.sequenceNumber;
        json << "}";
        return json.str();
    }
    
    std::string getName() const override { return "JsonLogFormatter"; }
    
private:
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%dT%H:%M:%S.%fZ");
        return ss.str();
    }
    
    std::string escapeJson(const std::string& str) const {
        std::string escaped;
        for (char c : str) {
            switch (c) {
                case '\\': escaped += "\\\\"; break;
                case '\"': escaped += "\\\""; break;
                case '\b': escaped += "\\b"; break;
                case '\f': escaped += "\\f"; break;
                case '\n': escaped += "\\n"; break;
                case '\r': escaped += "\\r"; break;
                case '\t': escaped += "\\t"; break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20) {
                        escaped += "\\u" + std::to_string(static_cast<int>(c));
                    } else {
                        escaped += c;
                    }
                    break;
            }
        }
        return escaped;
    }
};

// CSV formatter for log analysis
class CsvLogFormatter : public LogFormatter {
public:
    std::string format(const EnhancedLogEntry& entry) const override {
        std::stringstream csv;
        csv << "\"" << escapeCsv(formatTimestamp(entry.timestamp)) << "\",\"";
        csv << escapeCsv(levelToString(entry.level)) << "\",\"";
        csv << escapeCsv(entry.message) << "\",\"";
        csv << escapeCsv(entry.file) << "\",\"";
        csv << entry.line << "\",\"";
        csv << escapeCsv(entry.function) << "\",\"";
        csv << escapeCsv(entry.threadId) << "\",\"";
        
        std::string tagStr;
        for (const auto& tag : entry.tags) {
            if (!tagStr.empty()) tagStr += ";";
            tagStr += tag.first + "=" + tag.second;
        }
        csv << "\"" << escapeCsv(tagStr) << "\",\"";
        
        csv << entry.sequenceNumber;
        return csv.str();
    }
    
    std::string getName() const override { return "CsvLogFormatter"; }
    
private:
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%f");
        return ss.str();
    }
    
    std::string escapeCsv(const std::string& str) const {
        if (str.find(',') != std::string::npos || str.find('"') != std::string::npos || str.find('\\') != std::string::npos) {
            return "\"" + escapeCsvForQuotes(str) + "\"";
        }
        return str;
    }
    
    std::string escapeCsvForQuotes(const std::string& str) const {
        std::string escaped;
        for (char c : str) {
            if (c == '\"') escaped += "\"\"";
            else escaped += c;
        }
        return escaped;
    }
};

// XML formatter for log integration
class XmlLogFormatter : public LogFormatter {
public:
    std::string format(const EnhancedLogEntry& entry) const override {
        std::stringstream xml;
        xml << "<log_entry>";
        xml << "<timestamp>" << escapeXml(formatTimestamp(entry.timestamp)) << "</timestamp>";
        xml << "<level>" << escapeXml(levelToString(entry.level)) << "</level>";
        xml << "<message>" << escapeXml(entry.message) << "</message>";
        xml << "<file>" << escapeXml(entry.file) << "</file>";
        xml << "<line>" << entry.line << "</line>";
        xml << "<function>" << escapeXml(entry.function) << "</function>";
        xml << "<thread_id>" << escapeXml(entry.threadId) << "</thread_id>";
        
        if (!entry.tags.empty()) {
            xml << "<tags>";
            for (const auto& tag : entry.tags) {
                xml << "<tag name=\"" << escapeXml(tag.first) << "\">" << escapeXml(tag.second) << "</tag>";
            }
            xml << "</tags>";
        }
        
        if (!entry.metadata.empty()) {
            xml << "<metadata>";
            for (const auto& meta : entry.metadata) {
                xml << "<meta key=\"" << escapeXml(meta.first) << "\">" << escapeXml(meta.second) << "</meta>";
            }
            xml << "</metadata>";
        }
        
        xml << "<sequence>" << entry.sequenceNumber << "</sequence>";
        xml << "</log_entry>";
        return xml.str();
    }
    
    std::string getName() const override { return "XmlLogFormatter"; }
    
private:
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%dT%H:%M:%S.%fZ");
        return ss.str();
    }
    
    std::string escapeXml(const std::string& str) const {
        std::string escaped;
        for (char c : str) {
            switch (c) {
                case '&': escaped += "&amp;"; break;
                case '<': escaped += "&lt;"; break;
                case '>': escaped += "&gt;"; break;
                case '\"': escaped += "&quot;"; break;
                case '\'': escaped += "&apos;"; break;
                default: escaped += c; break;
            }
        }
        return escaped;
    }
};

// ANSI color themes
class AnsiColorTheme {
public:
    AnsiColorTheme() {
        colors[LogLevel::Debug] = "\033[36m";    // Cyan
        colors[LogLevel::Info] = "\033[32m";     // Green
        colors[LogLevel::Warning] = "\033[33m";  // Yellow
        colors[LogLevel::Error] = "\033[31m";    // Red
        colors[LogLevel::Critical] = "\033[35m"; // Magenta
        reset = "\033[0m";
    }
    
    void setColor(LogLevel level, const std::string& color) {
        colors[level] = color;
    }
    
    std::string getColor(LogLevel level) const {
        auto it = colors.find(level);
        return (it != colors.end()) ? it->second : "";
    }
    
    void setReset(const std::string& reset) { this->reset = reset; }
    std::string getReset() const { return reset; }
    
private:
    std::map<LogLevel, std::string> colors;
    std::string reset;
};

// Theme-based ANSI formatter
class ThemeLogFormatter : public LogFormatter {
public:
    explicit ThemeLogFormatter(const AnsiColorTheme& theme) : theme_(theme) {}
    
    std::string format(const EnhancedLogEntry& entry) const override {
        std::string color = theme_.getColor(entry.level);
        std::string reset = theme_.getReset();
        
        std::stringstream formatted;
        formatted << color << "[" << levelToString(entry.level) << "] ";
        formatted << formatTimestamp(entry.timestamp) << " ";
        formatted << entry.message << reset << "\n";
        
        if (!entry.file.empty() && entry.line > 0) {
            formatted << color << "  File: " << entry.file << ", Line: " << entry.line << reset << "\n";
        }
        if (!entry.function.empty()) {
            formatted << color << "  Function: " << entry.function << reset << "\n";
        }
        if (!entry.threadId.empty()) {
            formatted << color << "  Thread: " << entry.threadId << reset << "\n";
        }
        
        return formatted.str();
    }
    
    std::string getName() const override { return "ThemeLogFormatter"; }
    
private:
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    AnsiColorTheme theme_;
};

// Log formatter factory for easy formatter creation
class LogFormatterFactory {
public:
    static std::unique_ptr<LogFormatter> createFormatter(const std::string& name, const std::string& config = "") {
        if (name == "Template") {
            return std::make_unique<TemplateLogFormatter>(config);
        } else if (name == "JSON") {
            return std::make_unique<JsonLogFormatter>();
        } else if (name == "CSV") {
            return std::make_unique<CsvLogFormatter>();
        } else if (name == "XML") {
            return std::make_unique<XmlLogFormatter>();
        } else if (name == "Theme") {
            return std::make_unique<ThemeLogFormatter>(AnsiColorTheme());
        } else {
            return std::make_unique<TemplateLogFormatter>(config);
        }
    }
};

} // namespace nlm
