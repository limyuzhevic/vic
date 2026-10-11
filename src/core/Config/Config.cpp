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
    // PHASE 2: Implement proper JSON/YAML parser
    // This is a basic parser that supports both JSON and YAML formats
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Try JSON first (starts with {)
    if (!content.empty() && content.find('{') == 0) {
        return parseJSON(content);
    }
    // Try YAML (key: value pairs)
    else {
        return parseYAML(content);
    }
}

bool Config::parseJSON(const std::string& json) {
    // Basic JSON parser for key-value pairs
    // Supports: {"key": "value", "number": 123, "boolean": true}
    size_t pos = 1; // Skip opening {
    while (pos < json.size()) {
        // Find next key
        size_t keyStart = json.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = json.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = json.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Find colon after key
        size_t colonPos = json.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        // Find value
        size_t valueStart = json.find_first_not_of(' ', colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        char quoteChar = 0;
        if (valueStart < json.size() && (json[valueStart] == '"' || json[valueStart] == '\'')) {
            quoteChar = json[valueStart];
            size_t valueEnd = json.find(quoteChar, valueStart + 1);
            if (valueEnd == std::string::npos) break;
            
            std::string value = json.substr(valueStart + 1, valueEnd - valueStart - 1);
            set(key, value, ConfigSource::File);
            
            pos = valueEnd + 1;
        } else {
            // Try to parse number or boolean
            size_t valueEnd = json.find_first_of",\n ", valueStart);
            if (valueEnd == std::string::npos) valueEnd = json.size();
            
            std::string valueStr = json.substr(valueStart, valueEnd - valueStart);
            
            // Trim whitespace
            valueStr = trim(valueStr);
            
            // Try to parse as different types
            try {
                if (valueStr == "true" || valueStr == "false") {
                    bool boolValue = (valueStr == "true");
                    set(key, boolValue, ConfigSource::File);
                } else if (valueStr.find('.') != std::string::npos) {
                    // Parse as double
                    double doubleValue = std::stod(valueStr);
                    set(key, doubleValue, ConfigSource::File);
                } else {
                    // Try as int64_t
                    try {
                        int64_t intValue = std::stoll(valueStr);
                        set(key, intValue, ConfigSource::File);
                    } catch (...) {
                        // Fall back to string
                        set(key, valueStr, ConfigSource::File);
                    }
                }
            } catch (...) {
                // Fall back to string
                set(key, valueStr, ConfigSource::File);
            }
            
            pos = valueEnd;
        }
        
        // Skip comma if present
        while (pos < json.size() && (json[pos] == ',' || json[pos] == ' ')) {
            pos++;
        }
        
        // Skip closing brace if reached
        if (pos < json.size() && json[pos] == '}') {
            break;
        }
    }
    
    return true;
}

bool Config::parseYAML(const std::string& yaml) {
    // Basic YAML parser for key: value pairs
    // Supports: key: value\nkey2: value2
    size_t pos = 0;
    while (pos < yaml.size()) {
        // Find next key (until colon)
        size_t colonPos = yaml.find(':', pos);
        if (colonPos == std::string::npos) break;
        
        // Find key (before colon)
        size_t keyEnd = yaml.find_last_not_of(' ', colonPos - 1);
        size_t keyStart = yaml.find_last_not_of(' \t\n\r', keyEnd);
        while (keyStart > pos && (yaml[keyStart] == ' ' || yaml[keyStart] == '\t')) {
            keyStart--;
        }
        
        std::string key = yaml.substr(keyStart + 1, keyEnd - keyStart);
        
        // Find value (after colon)
        size_t valueStart = yaml.find_first_not_of(' ', colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        // Find end of line
        size_t valueEnd = yaml.find_first_of("\n\r", valueStart);
        if (valueEnd == std::string::npos) {
            valueEnd = yaml.size();
        }
        
        std::string value = yaml.substr(valueStart, valueEnd - valueStart);
        
        // Trim whitespace and quotes from value
        value = trim(value);
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Parse value type
        try {
            if (value == "true" || value == "false") {
                bool boolValue = (value == "true");
                set(key, boolValue, ConfigSource::File);
            } else if (value.find('.') != std::string::npos) {
                double doubleValue = std::stod(value);
                set(key, doubleValue, ConfigSource::File);
            } else {
                try {
                    int64_t intValue = std::stoll(value);
                    set(key, intValue, ConfigSource::File);
                } catch (...) {
                    set(key, value, ConfigSource::File);
                }
            }
        } catch (...) {
            set(key, value, ConfigSource::File);
        }
        
        pos = valueEnd + 1;
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
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

// JSON/YAML parser implementations

bool Config::parseJSON(const std::string& json) {
    // Basic JSON parser for key-value pairs
    // Supports: {"key": "value", "number": 123, "boolean": true}
    size_t pos = 1; // Skip opening {
    while (pos < json.size()) {
        // Find next key
        size_t keyStart = json.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = json.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = json.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Find colon after key
        size_t colonPos = json.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        // Find value
        size_t valueStart = json.find_first_not_of(' ', colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        char quoteChar = 0;
        if (valueStart < json.size() && (json[valueStart] == '"' || json[valueStart] == '\'')) {
            quoteChar = json[valueStart];
            size_t valueEnd = json.find(quoteChar, valueStart + 1);
            if (valueEnd == std::string::npos) break;
            
            std::string value = json.substr(valueStart + 1, valueEnd - valueStart - 1);
            set(key, value, ConfigSource::File);
            
            pos = valueEnd + 1;
        } else {
            // Try to parse number or boolean
            size_t valueEnd = json.find_first_of",\n ", valueStart);
            if (valueEnd == std::string::npos) valueEnd = json.size();
            
            std::string valueStr = json.substr(valueStart, valueEnd - valueStart);
            
            // Trim whitespace
            valueStr = trim(valueStr);
            
            // Try to parse as different types
            try {
                if (valueStr == "true" || valueStr == "false") {
                    bool boolValue = (valueStr == "true");
                    set(key, boolValue, ConfigSource::File);
                } else if (valueStr.find('.') != std::string::npos) {
                    // Parse as double
                    double doubleValue = std::stod(valueStr);
                    set(key, doubleValue, ConfigSource::File);
                } else {
                    // Try as int64_t
                    try {
                        int64_t intValue = std::stoll(valueStr);
                        set(key, intValue, ConfigSource::File);
                    } catch (...) {
                        // Fall back to string
                        set(key, valueStr, ConfigSource::File);
                    }
                }
            } catch (...) {
                // Fall back to string
                set(key, valueStr, ConfigSource::File);
            }
            
            pos = valueEnd;
        }
        
        // Skip comma if present
        while (pos < json.size() && (json[pos] == ',' || json[pos] == ' ')) {
            pos++;
        }
        
        // Skip closing brace if reached
        if (pos < json.size() && json[pos] == '}') {
            break;
        }
    }
    
    return true;
}

bool Config::parseYAML(const std::string& yaml) {
    // Basic YAML parser for key: value pairs
    // Supports: key: value\nkey2: value2
    size_t pos = 0;
    while (pos < yaml.size()) {
        // Find next key (until colon)
        size_t colonPos = yaml.find(':', pos);
        if (colonPos == std::string::npos) break;
        
        // Find key (before colon)
        size_t keyEnd = yaml.find_last_not_of(' ', colonPos - 1);
        size_t keyStart = yaml.find_last_not_of(' \t\n\r', keyEnd);
        while (keyStart > pos && (yaml[keyStart] == ' ' || yaml[keyStart] == '\t')) {
            keyStart--;
        }
        
        std::string key = yaml.substr(keyStart + 1, keyEnd - keyStart);
        
        // Find value (after colon)
        size_t valueStart = yaml.find_first_not_of(' ', colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        // Find end of line
        size_t valueEnd = yaml.find_first_of("\n\r", valueStart);
        if (valueEnd == std::string::npos) {
            valueEnd = yaml.size();
        }
        
        std::string value = yaml.substr(valueStart, valueEnd - valueStart);
        
        // Trim whitespace and quotes from value
        value = trim(value);
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Parse value type
        try {
            if (value == "true" || value == "false") {
                bool boolValue = (value == "true");
                set(key, boolValue, ConfigSource::File);
            } else if (value.find('.') != std::string::npos) {
                double doubleValue = std::stod(value);
                set(key, doubleValue, ConfigSource::File);
            } else {
                try {
                    int64_t intValue = std::stoll(value);
                    set(key, intValue, ConfigSource::File);
                } catch (...) {
                    set(key, value, ConfigSource::File);
                }
            }
        } catch (...) {
            set(key, value, ConfigSource::File);
        }
        
        pos = valueEnd + 1;
    }
    
    return true;
}
