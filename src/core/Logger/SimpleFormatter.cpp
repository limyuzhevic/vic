#include "Logger.hpp"
#include <algorithm>

namespace nlm {

std::string SimpleFormatter::format(const LogEntry& entry) {
    std::stringstream ss;
    
    // Add timestamp
    ss << "[" << entry.getFormattedTimestamp() << "] ";
    
    // Add level
    switch (entry.level) {
        case LogLevel::Debug: ss << "[DEBUG] "; break;
        case LogLevel::Info: ss << "[INFO] "; break;
        case LogLevel::Warning: ss << "[WARN] "; break;
        case LogLevel::Error: ss << "[ERROR] "; break;
        case LogLevel::Critical: ss << "[CRIT] "; break;
    }
    
    // Add category if not default
    if (entry.category != LogCategory::Default) {
        ss << "[" << entry.category << "] ";
    }
    
    // Add thread ID if available
    if (!entry.threadId.empty()) {
        ss << "[Thread:" << entry.threadId << "] ";
    }
    
    // Add message
    ss << entry.message;
    
    // Add file/line info if available
    if (!entry.file.empty() && entry.line > 0) {
        ss << " (" << entry.file << ":" << entry.line << ")";
    }
    
    return ss.str();
}

std::string SimpleFormatter::format(const std::string& message) {
    LogEntry entry;
    entry.message = message;
    return format(entry);
}

std::string JSONFormatter::format(const LogEntry& entry) {
    // Simple JSON formatting (in real implementation would use proper JSON library)
    std::stringstream ss;
    ss << "{";
    ss << "\"timestamp\": \"" << entry.getFormattedTimestamp() << "\", ";
    ss << "\"level\": " << static_cast<int>(entry.level) << ", ";
    ss << "\"category\": \"" << entry.category << "\", ";
    ss << "\"message\": \"" << escapeJSON(entry.message) << "\"";
    
    if (!entry.file.empty()) {
        ss << ", \"file\": \"" << escapeJSON(entry.file) << "\"";
    }
    
    if (entry.line > 0) {
        ss << ", \"line\": " << entry.line;
    }
    
    if (!entry.function.empty()) {
        ss << ", \"function\": \"" << escapeJSON(entry.function) << "\"";
    }
    
    if (!entry.threadId.empty()) {
        ss << ", \"thread_id\": \"" << entry.threadId << "\"";
    }
    
    if (!entry.metadata.empty()) {
        ss << ", \"metadata\": {";
        bool first = true;
        for (const auto& pair : entry.metadata) {
            if (!first) ss << ", ";
            first = false;
            ss << "\"" << pair.first << "\": \"" << escapeJSON(pair.second) << "\"";
        }
        ss << "}";
    }
    
    ss << "}";
    return ss.str();
}

std::string JSONFormatter::format(const std::string& message) {
    LogEntry entry;
    entry.message = message;
    return format(entry);
}

std::string XMLFormatter::format(const LogEntry& entry) {
    // Simple XML formatting
    std::stringstream ss;
    ss << "<log>" << std::endl;
    ss << "  <timestamp>" << entry.getFormattedTimestamp() << "</timestamp>" << std::endl;
    ss << "  <level>" << static_cast<int>(entry.level) << "</level>" << std::endl;
    ss << "  <category>" << escapeXML(entry.category) << "</category>" << std::endl;
    ss << "  <message>" << escapeXML(entry.message) << "</message>" << std::endl;
    
    if (!entry.file.empty()) {
        ss << "  <file>" << escapeXML(entry.file) << "</file>" << std::endl;
    }
    
    if (entry.line > 0) {
        ss << "  <line>" << entry.line << "</line>" << std::endl;
    }
    
    if (!entry.function.empty()) {
        ss << "  <function>" << escapeXML(entry.function) << "</function>" << std::endl;
    }
    
    if (!entry.threadId.empty()) {
        ss << "  <thread_id>" << escapeXML(entry.threadId) << "</thread_id>" << std::endl;
    }
    
    if (!entry.metadata.empty()) {
        ss << "  <metadata>" << std::endl;
        for (const auto& pair : entry.metadata) {
            ss << "    <" << pair.first << ">" << escapeXML(pair.second) << "</" << pair.first << ">" << std::endl;
        }
        ss << "  </metadata>" << std::endl;
    }
    
    ss << "</log>" << std::endl;
    return ss.str();
}

std::string XMLFormatter::format(const std::string& message) {
    LogEntry entry;
    entry.message = message;
    return format(entry);
}

std::string JSONFormatter::escapeJSON(const std::string& str) {
    std::stringstream ss;
    for (char c : str) {
        switch (c) {
            case '\"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default:
                if (c < 32 || c > 126) {
                    ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    ss << c;
                }
        }
    }
    return ss.str();
}

std::string XMLFormatter::escapeXML(const std::string& str) {
    std::stringstream ss;
    for (char c : str) {
        switch (c) {
            case '\"': ss << "&quot;"; break;
            case '\'': ss << "&apos;"; break;
            case '&': ss << "&amp;"; break;
            case '<': ss << "&lt;"; break;
            case '>': ss << "&gt;"; break;
            default: ss << c;
        }
    }
    return ss.str();
}
}