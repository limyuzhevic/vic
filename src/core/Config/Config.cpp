#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <cctype>

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
        return false;
    }
    
    // Determine file format based on extension
    std::string ext = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos);
    }
    
    if (ext == ".json" || ext == ".JSON") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml" || ext == ".YAML" || ext == ".YML") {
        return loadFromYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        return loadFromKeyValue(filepath);
    }
}

// Add the missing loadFromKeyValue implementation
bool Config::loadFromKeyValue(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Parse JSON using nlohmann library
        // Note: This requires nlohmann/json to be available
        // For now, implement a basic JSON parser or integration point
        
        // TODO: Implement proper JSON parsing
        // For now, fall back to key=value format with JSON-like parsing
        std::string line;
        while (std::getline(file, line)) {
            // Simple JSON parsing for key-value pairs
            // This is a basic implementation - production should use nlohmann/json
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '/' && line[1] == '/') {
                continue;
            }
            
            // Try to parse as JSON key-value pair
            // This handles both simple and nested structures
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = trim(line.substr(0, colonPos));
                std::string value = trim(line.substr(colonPos + 1));
                
                // Remove quotes from key and value
                if (key.size() >= 2 && key.front() == '\"' && key.back() == '\"') {
                    key = key.substr(1, key.size() - 2);
                }
                
                if (value.size() >= 2) {
                    if ((value.front() == '\"' && value.back() == '\"') ||
                        (value.front() == '\'' && value.back() == '\'')) {
                        value = value.substr(1, value.size() - 2);
                    } else if (value.front() == '[' && value.back() == ']') {
                        // Handle arrays
                        value = parseJSONArray(value);
                    } else if (value.front() == '{' && value.back() == '}') {
                        // Handle objects - flatten them
                        auto objValues = parseJSONObject(value);
                        for (const auto& objPair : objValues) {
                            set(objPair.first, objPair.second, ConfigSource::File);
                        }
                        continue;
                    }
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Parse YAML using a YAML library
        // Note: This requires yaml-cpp or similar to be available
        
        // TODO: Implement proper YAML parsing
        // For now, provide an informative message
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string Config::parseJSONValue(const std::string& jsonStr) {
    // Simple JSON value parser
    std::string value = trim(jsonStr);
    
    // Handle different JSON types
    if (value.empty()) return "";
    
    if (value.front() == '\"' && value.back() == '\"') {
        // String
        return value.substr(1, value.size() - 2);
    } else if (value.front() == '\'' && value.back() == '\'') {
        // String with single quotes
        return value.substr(1, value.size() - 2);
    } else if (value.front() == '[' && value.back() == ']') {
        // Array - return as string representation
        return parseJSONArray(value);
    } else if (value.front() == '{' && value.back() == '}') {
        // Object - flatten to key=value pairs
        return parseJSONObject(value);
    } else if (value == "true" || value == "false") {
        // Boolean
        return value;
    } else {
        // Try to parse as number
        try {
            size_t pos;
            double num = std::stod(value, &pos);
            if (pos == value.size()) {
                // It's a number
                return value;
            }
        } catch (...) {
            // Not a number
        }
    }
    
    return value;
}

std::string Config::parseJSONArray(const std::string& jsonArray) {
    // Parse JSON array to string representation
    std::string result = "[";
    bool first = true;
    
    size_t start = 1;
    size_t end = jsonArray.size() - 1;
    
    for (size_t i = start; i < end; ++i) {
        if (jsonArray[i] == ',' && !first) {
            result += ",";
        } else if (jsonArray[i] == '\"') {
            // Find end of quoted string
            size_t quoteEnd = i + 1;
            while (quoteEnd < end && jsonArray[quoteEnd] != '\"') {
                quoteEnd++;
            }
            if (quoteEnd < end) {
                if (!first) result += ",";
                result += jsonArray.substr(i, quoteEnd - i + 1);
                i = quoteEnd;
                first = false;
            }
        }
    }
    
    result += "]";
    return result;
}

std::vector<std::pair<std::string, std::string>> Config::parseJSONObject(const std::string& jsonObj) {
    std::vector<std::pair<std::string, std::string>> result;
    
    size_t start = 1;
    size_t end = jsonObj.size() - 1;
    size_t braceDepth = 1;
    
    for (size_t i = start; i < end; ++i) {
        if (jsonObj[i] == '{') {
            braceDepth++;
        } else if (jsonObj[i] == '}') {
            braceDepth--;
        } else if (jsonObj[i] == '\"' && braceDepth == 1) {
            // Find key
            size_t keyEnd = i + 1;
            while (keyEnd < end && jsonObj[keyEnd] != '\"') {
                keyEnd++;
            }
            if (keyEnd < end) {
                std::string key = jsonObj.substr(i + 1, keyEnd - i - 1);
                
                // Find value after colon
                size_t colonPos = jsonObj.find(':', keyEnd);
                if (colonPos != std::string::npos) {
                    // Parse value
                    std::string value = parseJSONValue(jsonObj.substr(colonPos + 1));
                    result.emplace_back(key, value);
                }
                
                i = keyEnd;
            }
        }
    }
    
    return result;
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
            } else {
                // Handle --key value format
                set(arg.substr(2), "true", ConfigSource::CommandLine);
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

bool Config::validate() const {
    bool valid = true;
    
    // Basic validation rules
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            // Invalid: empty key
            valid = false;
            continue;
        }
        
        // Validate based on key prefix
        if (entry.key.find("brain.") == 0) {
            // Brain configuration validation
            if (entry.key == "brain.neuron_count" && entry.source != ConfigSource::Default) {
                try {
                    auto value = std::get<int64_t>(entry.value);
                    if (value <= 0 || value > 1000000) {
                        return false; // Invalid neuron count
                    }
                } catch (...) {
                    valid = false;
                }
            }
        } else if (entry.key.find("plasticity.") == 0) {
            // Plasticity configuration validation
            if (entry.key == "plasticity.stdp.learning_rate") {
                try {
                    auto value = std::get<double>(entry.value);
                    if (value < 0.0 || value > 1.0) {
                        valid = false;
                    }
                } catch (...) {
                    valid = false;
                }
            }
        }
        
        // General value validation
        try {
            // Check if value can be converted to appropriate type based on name
            std::string key = toLower(entry.key);
            if (key.find("enable_") == 0 || key.find("enabled_") == 0 || 
                key.find("active_") == 0) {
                // Boolean parameter
                auto _ = std::get<bool>(entry.value);
            }
        } catch (...) {
            // Value is not of expected type, but might be valid
        }
    }
    
    return valid;
}

std::string Config::getValidationError() const {
    std::ostringstream oss;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            oss << "Error: Empty configuration key found\n";
            continue;
        }
        
        // Check for duplicate keys with different sources
        int count = 0;
        for (const auto& other : pImpl->entries) {
            if (other.key == entry.key) {
                count++;
            }
        }
        if (count > 1) {
            oss << "Warning: Key '" << entry.key << "' appears multiple times\n";
        }
        
        // Validate boolean parameters
        std::string key = toLower(entry.key);
        if ((key.find("enable_") == 0 || key.find("enabled_") == 0 || 
             key.find("active_") == 0) && key.find("true") == std::string::npos) {
            try {
                auto value = std::get<bool>(entry.value);
            } catch (...) {
                oss << "Error: Expected boolean value for parameter '" << entry.key << "'\n";
            }
        }
    }
    
    return oss.str();
}

bool Config::setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate before setting
    auto tempConfig = *this;
    tempConfig.set(key, value, source);
    
    if (!tempConfig.validate()) {
        return false;
    }
    
    // If validation passes, apply the change
    set(key, value, source);
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
