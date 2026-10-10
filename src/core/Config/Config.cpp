#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Try to parse as JSON first
    auto jsonContent = readFileContent(filepath);
    if (!jsonContent.empty()) {
        if (parseJSONConfig(jsonContent)) {
            NLM_LOG_INFO("Successfully loaded configuration from JSON file: " + filepath);
            return true;
        }
    }
    
    // Fall back to simple key=value format
    NLM_LOG_INFO("Loading configuration from simple file format (fallback): " + filepath);
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Enhanced configuration loading methods
std::string Config::readFileContent(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool Config::parseJSONConfig(const std::string& jsonContent) {
    // Simple JSON parsing for basic key-value pairs and arrays
    // In a real implementation, this would use a proper JSON library
    
    if (jsonContent.empty()) {
        return false;
    }
    
    // For now, implement a very simple JSON parser that handles:
    // - { "key": "value" }
    // - { "key": 123 }
    // - { "key": true/false }
    // - { "key": [1, 2, 3] }
    
    try {
        size_t pos = 0;
        std::string content = jsonContent;
        
        while (pos < content.size()) {
            // Find next key
            pos = content.find('"', pos);
            if (pos == std::string::npos) break;
            
            size_t keyEnd = content.find('"', pos + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(pos + 1, keyEnd - pos - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace and :
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            if (pos >= content.size() || content[pos] != ':') break;
            pos++;
            
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            
            // Parse value
            ConfigValue value;
            bool parsed = false;
            
            // String value
            if (pos < content.size() && content[pos] == '\"') {
                size_t valueEnd = content.find('\"', pos + 1);
                if (valueEnd != std::string::npos) {
                    std::string strValue = content.substr(pos + 1, valueEnd - pos - 1);
                    value = strValue;
                    pos = valueEnd + 1;
                    parsed = true;
                }
            }
            // Boolean value
            else if (content.substr(pos, 4) == "true") {
                value = true;
                pos += 4;
                parsed = true;
            }
            else if (content.substr(pos, 5) == "false") {
                value = false;
                pos += 5;
                parsed = true;
            }
            // Number value
            else if (std::isdigit(content[pos]) || content[pos] == '-') {
                size_t numEnd = pos;
                while (numEnd < content.size() && (std::isdigit(content[numEnd]) || content[numEnd] == '.' || content[numEnd] == 'e' || content[numEnd] == 'E' || content[numEnd] == '+' || content[numEnd] == '-')) {
                    numEnd++;
                }
                
                std::string numStr = content.substr(pos, numEnd - pos);
                try {
                    // Try double first
                    size_t decimalPos = numStr.find('.');
                    if (decimalPos != std::string::npos) {
                        value = std::stod(numStr);
                    } else {
                        // Try integer
                        if (numStr.find('e') != std::string::npos || numStr.find('E') != std::string::npos) {
                            value = std::stod(numStr);
                        } else {
                            // Try int64_t
                            try {
                                value = std::stoll(numStr);
                            } catch (...) {
                                value = std::stod(numStr);
                            }
                        }
                    }
                    pos = numEnd;
                    parsed = true;
                } catch (...) {
                    break;
                }
            }
            // Array value
            else if (pos < content.size() && content[pos] == '[') {
                size_t arrayEnd = content.find(']', pos);
                if (arrayEnd != std::string::npos) {
                    std::string arrayStr = content.substr(pos + 1, arrayEnd - pos - 1);
                    
                    // Parse array elements
                    std::vector<double> arrayValues;
                    size_t arrayPos = 0;
                    while (arrayPos < arrayStr.size()) {
                        // Skip whitespace
                        while (arrayPos < arrayStr.size() && std::isspace(arrayStr[arrayPos])) arrayPos++;
                        if (arrayPos >= arrayStr.size()) break;
                        
                        size_t elementEnd = arrayStr.find(',', arrayPos);
                        if (elementEnd == std::string::npos) {
                            elementEnd = arrayStr.size();
                        }
                        
                        std::string elementStr = arrayStr.substr(arrayPos, elementEnd - arrayPos);
                        try {
                            arrayValues.push_back(std::stod(elementStr));
                        } catch (...) {
                            arrayValues.push_back(0.0);
                        }
                        
                        arrayPos = elementEnd + 1;
                    }
                    
                    // Convert to appropriate vector type based on element types
                    // For now, use std::vector<double> as fallback
                    value = arrayValues;
                    pos = arrayEnd + 1;
                    parsed = true;
                }
            }
            
            if (parsed) {
                set(key, value, ConfigSource::File);
                
                // Skip whitespace and comma
                while (pos < content.size() && std::isspace(content[pos])) pos++;
                if (pos < content.size() && content[pos] == ',') pos++;
            } else {
                // Skip to next comma or }
                while (pos < content.size() && content[pos] != ',' && content[pos] != '}') pos++;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
