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
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    // Enhanced version with better error handling and format detection
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Cannot open config file: " + filepath);
        return false;
    }
    
    // Detect file format by first non-comment line
    std::string line;
    bool isJSON = false;
    bool isYAML = false;
    
    // Read first 10 lines to detect format
    for (int i = 0; i < 10 && std::getline(file, line); ++i) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Check for JSON format (starts with { or [)
        if (line[0] == '{' || line[0] == '[') {
            isJSON = true;
            break;
        }
        
        // Check for YAML format (key: value)
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos && colonPos > 0) {
            size_t keyLen = line.find_last_not_of(' ', colonPos - 1) - colonPos + 1;
            if (keyLen > 0) {
                isYAML = true;
                break;
            }
        }
        break;
    }
    
    // Reset file to beginning
    file.clear();
    file.seekg(0, std::ios::beg);
    
    if (isJSON) {
        return loadFromJSON(file, filepath);
    } else if (isYAML) {
        return loadFromYAML(file, filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        return loadFromSimpleFormat(file, filepath);
    }
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

} // namespace nlm

// JSON parsing implementation
namespace {

// Simple JSON parser for basic NLM configuration
bool parseJSONString(const std::string& jsonStr, std::string& key, ConfigValue& value) {
    // Remove whitespace
    std::string trimmed = trim(jsonStr);
    
    if (trimmed.empty() || trimmed[0] != '"') {
        return false;
    }
    
    size_t keyEnd = trimmed.find('"', 1);
    if (keyEnd == std::string::npos) {
        return false;
    }
    
    key = trimmed.substr(1, keyEnd - 1);
    
    // Find value after colon
    size_t colonPos = trimmed.find(':', keyEnd + 1);
    if (colonPos == std::string::npos) {
        return false;
    }
    
    std::string valueStr = trim(trimmed.substr(colonPos + 1));
    
    // Parse value based on type
    if (valueStr.front() == '\"' && valueStr.back() == '\"') {
        // String value
        value = trim(valueStr.substr(1, valueStr.size() - 2));
    } else if (valueStr == "true" || valueStr == "false") {
        // Boolean value
        value = (valueStr == "true");
    } else if (std::isdigit(valueStr.front()) || (valueStr.front() == '-' && std::isdigit(valueStr[1]))) {
        // Numeric value
        if (valueStr.find('.') != std::string::npos) {
            // Float value
            value = std::stod(valueStr);
        } else {
            // Integer value (try int64_t first)
            try {
                value = std::stoll(valueStr);
            } catch (...) {
                // Fall back to double
                value = std::stod(valueStr);
            }
        }
    } else {
        // Unknown type, treat as string
        value = valueStr;
    }
    
    return true;
}

} // namespace

bool Config::loadFromJSON(std::ifstream& file, const std::string& filepath) {
    NLM_LOG_INFO("Loading JSON configuration from: " + filepath);
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Simple JSON object parsing (for basic key-value pairs)
    // This is a minimal implementation - in production would use nlohmann::json
    
    // Remove whitespace and newlines
    std::string jsonStr = trim(content);
    if (jsonStr.empty() || jsonStr[0] != '{') {
        NLM_LOG_ERROR("Invalid JSON format in config file: " + filepath);
        return false;
    }
    
    // Remove braces
    jsonStr = jsonStr.substr(1, jsonStr.size() - 2);
    
    // Parse key-value pairs (simplified)
    size_t pos = 0;
    while (pos < jsonStr.size()) {
        // Find next key
        size_t keyStart = jsonStr.find('"', pos);
        if (keyStart == std::string::npos) {
            break;
        }
        
        size_t keyEnd = jsonStr.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) {
            break;
        }
        
        std::string key = jsonStr.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Find colon after key
        size_t colonPos = jsonStr.find(':', keyEnd + 1);
        if (colonPos == std::string::npos) {
            break;
        }
        
        // Find value
        size_t valueStart = jsonStr.find_first_not_of(' ', colonPos + 1);
        if (valueStart == std::string::npos) {
            break;
        }
        
        // Parse value based on first character
        ConfigValue value;
        if (valueStart < jsonStr.size() && jsonStr[valueStart] == '"') {
            // String value
            size_t valueEnd = jsonStr.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) {
                break;
            }
            std::string valueStr = jsonStr.substr(valueStart + 1, valueEnd - valueStart - 1);
            value = valueStr;
            pos = valueEnd + 1;
        } else if (valueStart < jsonStr.size() && jsonStr[valueStart] == 't') {
            // Boolean true
            value = true;
            pos = valueStart + 4; // "true"
        } else if (valueStart < jsonStr.size() && jsonStr[valueStart] == 'f') {
            // Boolean false
            value = false;
            pos = valueStart + 5; // "false"
        } else {
            // Number - parse until comma or }
            size_t valueEnd = jsonStr.find_first_of(",}", valueStart);
            if (valueEnd == std::string::npos) {
                valueEnd = jsonStr.size();
            }
            std::string valueStr = trim(jsonStr.substr(valueStart, valueEnd - valueStart));
            
            try {
                // Try to parse as integer
                if (valueStr.find('.') != std::string::npos) {
                    value = std::stod(valueStr);
                } else {
                    value = std::stoll(valueStr);
                }
            } catch (...) {
                // Fall back to string
                value = valueStr;
            }
            
            pos = valueEnd;
        }
        
        set(key, value, ConfigSource::File);
        
        // Skip comma or whitespace
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++;
        }
        
        // Skip whitespace
        while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\n')) {
            pos++;
        }
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(getKeys().size()) + " configuration entries from JSON");
    return true;
}

bool Config::loadFromYAML(std::ifstream& file, const std::string& filepath) {
    NLM_LOG_INFO("Loading YAML configuration from: " + filepath);
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse YAML key: value format
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos || colonPos == 0) {
            continue;
        }
        
        std::string key = trim(line.substr(0, colonPos));
        std::string valueStr = trim(line.substr(colonPos + 1));
        
        ConfigValue value;
        
        // Parse value based on type
        if (valueStr.empty()) {
            continue;
        } else if (valueStr.front() == '\"' && valueStr.back() == '\"') {
            // String value (quoted)
            value = valueStr.substr(1, valueStr.size() - 2);
        } else if (valueStr == "true" || valueStr == "false") {
            // Boolean value
            value = (valueStr == "true");
        } else if (std::isdigit(valueStr.front()) || 
                   (valueStr.front() == '-' && std::isdigit(valueStr[1]))) {
            // Numeric value
            if (valueStr.find('.') != std::string::npos || 
                valueStr.find('e') != std::string::npos || 
                valueStr.find('E') != std::string::npos) {
                value = std::stod(valueStr);
            } else {
                try {
                    value = std::stoll(valueStr);
                } catch (...) {
                    value = std::stod(valueStr);
                }
            }
        } else {
            // Try to parse as list (YAML array)
            if (valueStr.front() == '[' && valueStr.back() == ']') {
                std::string listStr = valueStr.substr(1, valueStr.size() - 2);
                std::vector<double> list;
                
                size_t listPos = 0;
                while (listPos < listStr.size()) {
                    size_t nextComma = listStr.find(',', listPos);
                    if (nextComma == std::string::npos) {
                        nextComma = listStr.size();
                    }
                    
                    std::string item = trim(listStr.substr(listPos, nextComma - listPos));
                    if (!item.empty()) {
                        try {
                            if (item.find('.') != std::string::npos || 
                                item.find('e') != std::string::npos || 
                                item.find('E') != std::string::npos) {
                                list.push_back(std::stod(item));
                            } else {
                                list.push_back(std::stoll(item));
                            }
                        } catch (...) {
                            // Skip invalid items
                        }
                    }
                    
                    listPos = nextComma + 1;
                    while (listPos < listStr.size() && 
                           (listStr[listPos] == ' ' || listStr[listPos] == '\t')) {
                        listPos++;
                    }
                }
                
                if (!list.empty()) {
                    value = list;
                }
            } else {
                // Unknown type, treat as string
                value = valueStr;
            }
        }
        
        set(key, value, ConfigSource::File);
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(getKeys().size()) + " configuration entries from YAML");
    return true;
}

bool Config::loadFromSimpleFormat(std::ifstream& file, const std::string& filepath) {
    NLM_LOG_INFO("Loading simple format configuration from: " + filepath);
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs (backward compatibility)
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(getKeys().size()) + " configuration entries from simple format");
    return true;
}
