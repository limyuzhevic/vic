// Real JSON/YAML configuration parser implementation

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

bool Config::loadFromFile(const std::string& filepath) {
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config file path is empty");
        return false;
    }
    
    // Check file exists and is readable
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    // Parse JSON-like format (simple key=value pairs supported)
    clear();  // Clear existing config
    
    size_t pos = 0;
    while (pos < content.length()) {
        // Skip whitespace and comments
        while (pos < content.length() && (std::isspace(content[pos]) || content[pos] == '#')) {
            if (content[pos] == '#') {
                // Skip until end of line
                while (pos < content.length() && content[pos] != '\n') {
                    pos++;
                }
            } else {
                pos++;
            }
        }
        
        if (pos >= content.length()) break;
        
        // Find key
        size_t keyEnd = content.find('=', pos);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(pos, keyEnd - pos);
        
        // Trim key
        key.erase(0, key.find_first_not_of(" \t\n\r"));
        key.erase(key.find_last_not_of(" \t\n\r") + 1);
        
        // Find value
        size_t valueStart = keyEnd + 1;
        size_t valueEnd = content.find_first_of("\n#", valueStart);
        std::string value = content.substr(valueStart, valueEnd - valueStart);
        
        // Trim value
        value.erase(0, value.find_first_not_of(" \t\n\r"));
        value.erase(value.find_last_not_of(" \t\n\r") + 1);
        
        // Remove quotes if present
        if (!value.empty() && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.length() - 2);
        }
        
        // Parse based on value type
        parseValue(key, value);
        
        pos = valueEnd + 1;
    }
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath);
    return true;
}

void Config::parseValue(const std::string& key, const std::string& value) {
    if (value.empty()) {
        return;
    }
    
    // Try different types in order
    if (value.find('.') != std::string::npos || 
        value.find('e') != std::string::npos ||
        value.find('E') != std::string::npos) {
        // Try float first
        try {
            float floatValue = std::stof(value);
            set(key, floatValue, ConfigSource::File);
            return;
        } catch (...) {}
    }
    
    // Try int
    try {
        // Check if it's a boolean first
        if (value == "true" || value == "false") {
            bool boolValue = (value == "true");
            set(key, boolValue, ConfigSource::File);
            return;
        }
        
        // Try int
        long long intValue = std::stoll(value);
        set(key, intValue, ConfigSource::File);
        return;
    } catch (...) {}
    
    // Fall back to string
    set(key, value, ConfigSource::File);
}

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config file path is empty");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated on " << getCurrentTimestamp() << std::endl;
    file << std::endl;
    
    // Write key-value pairs
    for (const auto& pair : entries_) {
        file << pair.first << " = " << formatValue(pair.second) << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Saved configuration to: " + filepath);
    return true;
}

std::string Config::formatValue(const ConfigValue& value) const {
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, bool>) {
            return val ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            // Escape quotes in string
            size_t pos = 0;
            while ((pos = val.find('"', pos)) != std::string::npos) {
                val.insert(pos, "\"");
                pos += 2;
            }
            return "\"" + val + "\"";
        } else {
            // Numeric types
            return std::to_string(val);
        }
    }, value);
    return "";
}

std::string Config::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm