#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace nlm {

// Simple JSON parser (Phase 2 implementation)
class SimpleJSONParser {
public:
    static bool parse(const std::string& json, std::vector<std::pair<std::string, std::string>>& keyValues) {
        keyValues.clear();
        
        // Remove whitespace
        std::string content = json;
        content.erase(std::remove_if(content.begin(), content.end(), ::isspace), content.end());
        
        if (content.empty() || content.front() != '{' || content.back() != '}') {
            return false;
        }
        
        content = content.substr(1, content.size() - 2);
        
        size_t pos = 0;
        while (pos < content.size()) {
            // Find key
            size_t keyStart = content.find('"', pos);
            if (keyStart == std::string::npos) break;
            size_t keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            pos = keyEnd + 1;
            
            // Find separator
            size_t colon = content.find(':', pos);
            if (colon == std::string::npos) break;
            
            // Find value
            size_t valueStart = content.find_first_not_of(" \t\n\r", colon + 1);
            if (valueStart == std::string::npos) break;
            
            size_t valueEnd;
            if (content[valueStart] == '"') {
                valueEnd = content.find('"', valueStart + 1);
                if (valueEnd == std::string::npos) break;
                std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
                keyValues.push_back({key, value});
                pos = valueEnd + 1;
            } else if (content[valueStart] == '{' || content[valueStart] == '[') {
                // For complex values, skip to matching closing bracket
                int depth = 1;
                valueEnd = valueStart + 1;
                while (valueEnd < content.size() && depth > 0) {
                    if (content[valueEnd] == '{' || content[valueEnd] == '[') depth++;
                    else if (content[valueEnd] == '}' || content[valueEnd] == ']') depth--;
                    valueEnd++;
                }
                // For now, skip complex values
                pos = valueEnd;
            } else {
                // Simple value
                size_t comma = content.find(',', valueStart);
                if (comma == std::string::npos) {
                    comma = content.size();
                }
                std::string value = content.substr(valueStart, comma - valueStart);
                keyValues.push_back({key, value});
                pos = comma;
            }
            
            // Skip comma
            while (pos < content.size() && (content[pos] == ',' || content[pos] == ' ' || content[pos] == '\t')) {
                pos++;
            }
        }
        
        return !keyValues.empty();
    }
};

// Simple YAML parser (Phase 2 implementation)
class SimpleYAMLParser {
public:
    static bool parse(const std::string& yaml, std::vector<std::pair<std::string, std::string>>& keyValues) {
        keyValues.clear();
        
        size_t pos = 0;
        while (pos < yaml.size()) {
            // Find line with key: value
            size_t lineEnd = yaml.find('\n', pos);
            if (lineEnd == std::string::npos) lineEnd = yaml.size();
            
            std::string line = yaml.substr(pos, lineEnd - pos);
            
            // Remove leading whitespace
            size_t firstNonSpace = line.find_first_not_of(" \t");
            if (firstNonSpace == std::string::npos) {
                pos = lineEnd + 1;
                continue;
            }
            
            // Find colon
            size_t colon = line.find(':', firstNonSpace);
            if (colon == std::string::npos) {
                pos = lineEnd + 1;
                continue;
            }
            
            // Extract key (remove quotes if present)
            std::string key = line.substr(firstNonSpace, colon - firstNonSpace);
            // Remove surrounding quotes
            if (key.size() >= 2 && ((key.front() == '"' && key.back() == '"') ||
                                   (key.front() == '\'' && key.back() == '\''))) {
                key = key.substr(1, key.size() - 2);
            }
            
            // Extract value
            size_t valueStart = line.find_first_not_of(" \t", colon + 1);
            if (valueStart == std::string::npos) {
                pos = lineEnd + 1;
                continue;
            }
            
            std::string value = line.substr(valueStart);
            
            // Remove quotes from value if present
            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                     (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Skip empty lines and comments
            if (!key.empty() && !value.empty()) {
                keyValues.push_back({key, value});
            }
            
            pos = lineEnd + 1;
        }
        
        return !keyValues.empty();
    }
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// Simple JSON parser (Phase 2 implementation)
bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON configuration file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    
    std::vector<std::pair<std::string, std::string>> keyValues;
    if (!SimpleJSONParser::parse(jsonContent, keyValues)) {
        NLM_LOG_ERROR("Failed to parse JSON configuration file: " + filepath);
        return false;
    }
    
    // Convert parsed key-values to configuration entries
    clear();
    for (const auto& kv : keyValues) {
        // Determine type based on content
        std::string value = kv.second;
        bool isBool = (value == "true" || value == "false");
        bool isInt = true;
        for (char c : value) {
            if (!std::isdigit(c) && c != '-' && c != '+') {
                isInt = false;
                break;
            }
        }
        bool isFloat = isInt;
        if (isInt) {
            for (char c : value) {
                if (c == '.') {
                    isFloat = false;
                    break;
                }
            }
        }
        
        ConfigSource source = ConfigSource::File;
        ConfigValue configValue;
        
        if (isBool) {
            configValue = value == "true";
        } else if (isFloat) {
            try {
                configValue = std::stod(value);
            } catch (...) {
                configValue = value;  // Fall back to string
            }
        } else if (isInt) {
            try {
                configValue = std::stoll(value);
            } catch (...) {
                configValue = value;  // Fall back to string
            }
        } else {
            configValue = value;
        }
        
        set(kv.first, configValue, source);
    }
    
    NLM_LOG_INFO("Successfully loaded JSON configuration with " + std::to_string(keyValues.size()) + " entries");
    return true;
}

// Simple YAML parser (Phase 2 implementation)
bool Config::loadFromYAMLLFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open YAML configuration file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string yamlContent = buffer.str();
    
    std::vector<std::pair<std::string, std::string>> keyValues;
    if (!SimpleYAMLParser::parse(yamlContent, keyValues)) {
        NLM_LOG_ERROR("Failed to parse YAML configuration file: " + filepath);
        return false;
    }
    
    // Convert parsed key-values to configuration entries
    clear();
    for (const auto& kv : keyValues) {
        // Determine type based on content
        std::string value = kv.second;
        bool isBool = (value == "true" || value == "false");
        bool isInt = true;
        for (char c : value) {
            if (!std::isdigit(c) && c != '-' && c != '+') {
                isInt = false;
                break;
            }
        }
        bool isFloat = isInt;
        if (isInt) {
            for (char c : value) {
                if (c == '.') {
                    isFloat = false;
                    break;
                }
            }
        }
        
        ConfigSource source = ConfigSource::File;
        ConfigValue configValue;
        
        if (isBool) {
            configValue = value == "true";
        } else if (isFloat) {
            try {
                configValue = std::stod(value);
            } catch (...) {
                configValue = value;  // Fall back to string
            }
        } else if (isInt) {
            try {
                configValue = std::stoll(value);
            } catch (...) {
                configValue = value;  // Fall back to string
            }
        } else {
            configValue = value;
        }
        
        set(kv.first, configValue, source);
    }
    
    NLM_LOG_INFO("Successfully loaded YAML configuration with " + std::to_string(keyValues.size()) + " entries");
    return true;
}

// Simple key=value file parser (Phase 1)
bool Config::loadFromSimpleFile(const std::string& filepath) {
    return loadFromFile(filepath);
}

// Implementation of existing loadFromFile method (backward compatibility)
bool Config::loadFromFile(const std::string& filepath) {
    // Check if file has .json or .yaml extension for proper parsing
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return loadFromJSONFile(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYAMLLFile(filepath);
    } else {
        // Fallback to simple key=value format for backward compatibility
        return loadFromSimpleFile(filepath);
    }
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file has .json or .yaml extension for proper parsing
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return loadFromJSONFile(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYAMLLFile(filepath);
    } else {
        // Fallback to simple key=value format for backward compatibility
        return loadFromSimpleFile(filepath);
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
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
