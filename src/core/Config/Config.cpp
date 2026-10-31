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
    // Enhanced file loading with JSON/YAML support
    // Try to detect file format and parse accordingly
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file content
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    if (content.empty()) {
        return false;
    }
    
    // Detect file format based on extension and content
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    // Try to parse as JSON first (try-catch to fallback)
    try {
        parseJSON(content, ConfigSource::File);
        return true;
    } catch (const std::exception&) {
        // JSON parsing failed, try other formats
    }
    
    // Try to parse as YAML
    try {
        parseYAML(content, ConfigSource::File);
        return true;
    } catch (const std::exception&) {
        // YAML parsing failed, try legacy format
    }
    
    // Fallback to legacy key=value format
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
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
    
    // Write configuration in modern format with proper type handling
    file << "# NLM Configuration File (Generated)" << std::endl;
    file << "# Format: key = value  # description" << std::endl;
    file << "# Type support: int, double, bool, string, arrays" << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        // Skip runtime-only settings (user overrides)
        if (entry.source == ConfigSource::Runtime) {
            continue;
        }
        
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert ConfigValue to string representation
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                // For numeric types
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
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

void Config::parseJSON(const std::string& content, ConfigSource source) {
    // Simplified JSON parsing for common NLM configuration patterns
    // This is a basic implementation that can be extended with a proper JSON library
    
    // Look for common NLM configuration patterns
    // JSON object format: {"key": "value", "number": 123, "bool": true, "array": [1,2,3]}
    
    // Simple key-value pairs (no nested objects)
    size_t pos = 0;
    while (pos < content.size()) {
        // Find next " or {
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        key = trim(key);
        
        // Find value
        size_t colonPos = content.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        size_t valueStart = content.find_first_not_of(' ', colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        // Try to find end of value
        size_t valueEnd = std::string::npos;
        char delim = '\0';
        
        if (content[valueStart] == '"') {
            // String value
            delim = '"';
            valueEnd = content.find('"', valueStart + 1);
        } else if (content[valueStart] == '[') {
            // Array value
            delim = ']';
            valueEnd = content.find(']', valueStart + 1);
            // Find matching brackets for nested arrays
            int bracketDepth = 1;
            for (size_t scan = valueStart + 1; scan < content.size(); ++scan) {
                if (content[scan] == '[') bracketDepth++;
                else if (content[scan] == ']') {
                    bracketDepth--;
                    if (bracketDepth == 0) {
                        valueEnd = scan;
                        break;
                    }
                }
            }
        } else {
            // Number or boolean
            for (size_t scan = valueStart; scan < content.size(); ++scan) {
                if (content[scan] == ',' || content[scan] == '}') {
                    valueEnd = scan;
                    break;
                }
            }
        }
        
        if (valueEnd == std::string::npos) break;
        
        std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
        valueStr = trim(valueStr);
        
        // Remove surrounding quotes
        if (valueStr.size() >= 2 && 
            ((valueStr.front() == '"' && valueStr.back() == '"') ||
             (valueStr.front() == '\'' && valueStr.back() == '\''))) {
            valueStr = valueStr.substr(1, valueStr.size() - 2);
        }
        
        // Convert string value to appropriate type
        ConfigValue configValue;
        bool parsed = false;
        
        // Try to parse as int
        try {
            size_t pos;
            int intVal = std::stoi(valueStr, &pos);
            if (pos == valueStr.size()) {
                configValue = intVal;
                parsed = true;
            }
        } catch (...) {}
        
        // Try to parse as double
        if (!parsed) {
            try {
                size_t pos;
                double doubleVal = std::stod(valueStr, &pos);
                if (pos == valueStr.size()) {
                    configValue = doubleVal;
                    parsed = true;
                }
            } catch (...) {}
        }
        
        // Try to parse as bool
        if (!parsed) {
            if (valueStr == "true" || valueStr == "false") {
                configValue = (valueStr == "true");
                parsed = true;
            }
        }
        
        // Try to parse as array
        if (!parsed && valueStr.front() == '[') {
            std::vector<int> intArray;
            std::vector<double> doubleArray;
            std::vector<std::string> stringArray;
            
            size_t arrayPos = valueStart + 1;
            size_t arrayEnd = valueEnd;
            
            // Simple array parsing (no nested arrays for now)
            size_t itemStart = arrayPos;
            while (itemStart < arrayEnd) {
                size_t itemEnd = content.find_first_of(",]", itemStart);
                if (itemEnd == std::string::npos) break;
                
                std::string item = content.substr(itemStart, itemEnd - itemStart);
                item = trim(item);
                
                // Remove quotes
                if (item.size() >= 2 && 
                    ((item.front() == '"' && item.back() == '"') ||
                     (item.front() == '\'' && item.back() == '\''))) {
                    item = item.substr(1, item.size() - 2);
                }
                
                // Try to parse as int
                try {
                    int intItem = std::stoi(item);
                    intArray.push_back(intItem);
                } catch (...) {
                    // Try double
                    try {
                        double doubleItem = std::stod(item);
                        doubleArray.push_back(doubleItem);
                    } catch (...) {
                        // String
                        stringArray.push_back(item);
                    }
                }
                
                itemStart = content.find_first_not_of(", ", itemEnd + 1);
            }
            
            // Prefer the most appropriate type
            if (!stringArray.empty()) {
                configValue = stringArray;
                parsed = true;
            } else if (!doubleArray.empty()) {
                configValue = doubleArray;
                parsed = true;
            } else if (!intArray.empty()) {
                configValue = intArray;
                parsed = true;
            }
        }
        
        if (!parsed) {
            // Default to string
            configValue = valueStr;
        }
        
        set(key, configValue, source);
        
        // Advance position
        pos = valueEnd + 1;
    }
}

void Config::parseYAML(const std::string& content, ConfigSource source) {
    // Simplified YAML parsing for common NLM configuration patterns
    // This is a basic implementation that can be extended with a proper YAML library
    
    // Simple key: value format (YAML block)
    size_t pos = 0;
    while (pos < content.size()) {
        // Find next non-whitespace character
        size_t keyStart = content.find_first_not_of(" \t\n\r", pos);
        if (keyStart == std::string::npos) break;
        
        // Find end of key (colon or newline)
        size_t keyEnd = content.find_first_of(":\n", keyStart);
        if (keyEnd == std::string::npos) break;
        
        if (content[keyEnd] == ':') {
            std::string key = content.substr(keyStart, keyEnd - keyStart);
            key = trim(key);
            
            // Find value after colon
            size_t valueStart = content.find_first_not_of(" ", keyEnd + 1);
            if (valueStart == std::string::npos) break;
            
            size_t valueEnd = content.find_first_of("\n", valueStart);
            if (valueEnd == std::string::npos) valueEnd = content.size();
            
            std::string value = content.substr(valueStart, valueEnd - valueStart);
            value = trim(value);
            
            // Handle YAML-specific types
            ConfigValue configValue;
            bool parsed = false;
            
            // Handle YAML boolean
            if (value == "true" || value == "false") {
                configValue = (value == "true");
                parsed = true;
            }
            // Handle YAML null
            else if (value == "null") {
                // Create empty string for null (could be better)
                configValue = std::string("");
                parsed = true;
            }
            // Handle YAML numbers (including scientific notation)
            else {
                // Try to parse as int
                try {
                    size_t pos;
                    int intVal = std::stoi(value, &pos);
                    if (pos == value.size()) {
                        configValue = intVal;
                        parsed = true;
                    }
                } catch (...) {}
                
                // Try to parse as double
                if (!parsed) {
                    try {
                        size_t pos;
                        double doubleVal = std::stod(value, &pos);
                        if (pos == value.size()) {
                            configValue = doubleVal;
                            parsed = true;
                        }
                    } catch (...) {}
                }
                
                // Handle YAML quoted strings
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    std::string unquoted = value.substr(1, value.size() - 2);
                    configValue = unquoted;
                    parsed = true;
                }
            }
            
            if (!parsed) {
                configValue = value;
            }
            
            set(key, configValue, source);
            
            // Advance position
            pos = valueEnd + 1;
        } else {
            // Not a key-value pair, skip to next line
            size_t lineEnd = content.find_first_of("\n", keyEnd);
            if (lineEnd == std::string::npos) break;
            pos = lineEnd + 1;
        }
    }
}

std::string Config::configValueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            // For numeric types
            oss << arg;
        }
    }, value);
    
    return oss.str();
}

} // namespace nlm
