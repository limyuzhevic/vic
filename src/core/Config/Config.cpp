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
    std::string format = detectFileFormat(filepath);
    
    if (format == "json" || format == "JSON") {
        return loadFromJsonFile(filepath);
    } else if (format == "yaml" || format == "yml" || format == "YAML" || format == "YML") {
        return loadFromYamlFile(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
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
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    }
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Simple JSON parsing - looks for "key": "value" pairs
    // This is a minimal implementation focused on the basic needs
    size_t pos = 0;
    while (pos < content.length()) {
        // Find next " key": pattern
        pos = content.find('"', pos);
        if (pos == std::string::npos) break;
        
        size_t keyEnd = content.find('"', pos + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(pos + 1, keyEnd - pos - 1);
        
        // Skip to colon
        size_t colonPos = content.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        // Skip whitespace
        size_t valueStart = content.find_first_not_of(" \t\n\r", colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        // Parse value based on type
        ConfigValue value;
        
        if (valueStart < content.length() && content[valueStart] == '"') {
            // String value
            size_t valueEnd = content.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;
            
            std::string stringValue = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            set(key, stringValue, ConfigSource::File);
            
            pos = valueEnd + 1;
        } else if (valueStart < content.length() && (content[valueStart] == 't' || content[valueStart] == 'f')) {
            // Boolean value
            size_t valueEnd = content.find_first_of(",}\n", valueStart);
            if (valueEnd == std::string::npos) break;
            
            std::string boolStr = content.substr(valueStart, valueEnd - valueStart);
            bool boolValue = (boolStr == "true");
            set(key, boolValue, ConfigSource::File);
            
            pos = valueEnd;
        } else if (valueStart < content.length() && (std::isdigit(content[valueStart]) || content[valueStart] == '-' || content[valueStart] == '.')) {
            // Numeric value
            size_t valueEnd = content.find_first_of(",}\n", valueStart);
            if (valueEnd == std::string::npos) break;
            
            std::string numStr = content.substr(valueStart, valueEnd - valueStart);
            try {
                // Try double first (handles both int and float)
                size_t decimalPos = numStr.find('.');
                if (decimalPos != std::string::npos) {
                    double doubleValue = std::stod(numStr);
                    set(key, doubleValue, ConfigSource::File);
                } else {
                    // Check if it's actually an int
                    bool isInt = true;
                    for (char c : numStr) {
                        if (!std::isdigit(c)) {
                            isInt = false;
                            break;
                        }
                    }
                    if (isInt) {
                        int intValue = std::stoi(numStr);
                        set(key, intValue, ConfigSource::File);
                    } else {
                        double doubleValue = std::stod(numStr);
                        set(key, doubleValue, ConfigSource::File);
                    }
                }
            } catch (const std::exception&) {
                // Skip if conversion fails
            }
            
            pos = valueEnd;
        } else {
            // Skip to next key
            pos = keyEnd + 1;
        }
    }
    
    return true;
}

bool Config::loadFromYamlFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentKey;
    
    while (std::getline(file, line)) {
        // Trim whitespace
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Check if line is a key-value pair (key: value)
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            currentKey = trim(line.substr(0, colonPos));
            std::string valueStr = trim(line.substr(colonPos + 1));
            
            // Remove quotes from value
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            set(currentKey, valueStr, ConfigSource::File);
        }
        // Handle YAML arrays (simplified)
        else if (line.find('[') != std::string::npos && line.find(']') != std::string::npos) {
            if (!currentKey.empty()) {
                // Very basic array parsing - this is simplified
                size_t bracketStart = line.find('[');
                size_t bracketEnd = line.find_last_of(']');
                
                if (bracketEnd > bracketStart) {
                    std::string arrayContent = line.substr(bracketStart + 1, bracketEnd - bracketStart - 1);
                    std::vector<std::string> tokens;
                    
                    // Simple tokenization
                    size_t start = 0;
                    size_t end = arrayContent.find(",");
                    while (end != std::string::npos) {
                        std::string token = trim(arrayContent.substr(start, end - start));
                        if (!token.empty()) {
                            tokens.push_back(token);
                        }
                        start = end + 1;
                        end = arrayContent.find(",", start);
                    }
                    
                    if (start < arrayContent.length()) {
                        std::string token = trim(arrayContent.substr(start));
                        if (!token.empty()) {
                            tokens.push_back(token);
                        }
                    }
                    
                    // Try to parse tokens as different types
                    if (!tokens.empty()) {
                        // Try to determine array type
                        bool allInts = true;
                        bool allDoubles = true;
                        bool allStrings = true;
                        
                        for (const std::string& token : tokens) {
                            if (token.empty()) continue;
                            
                            // Check if it's a string (contains non-numeric characters)
                            bool isString = false;
                            for (char c : token) {
                                if (!std::isdigit(c) && c != '.' && c != '-') {
                                    isString = true;
                                    break;
                                }
                            }
                            
                            if (isString) {
                                allInts = false;
                                allDoubles = false;
                            } else {
                                allStrings = false;
                                // Try to parse as number
                                try {
                                    int intVal = std::stoi(token);
                                } catch (...) {
                                    allInts = false;
                                }
                                try {
                                    double doubleVal = std::stod(token);
                                } catch (...) {
                                    allDoubles = false;
                                }
                            }
                        }
                        
                        ConfigValue arrayValue;
                        if (allInts && !tokens.empty()) {
                            std::vector<int> intArray;
                            for (const std::string& token : tokens) {
                                try {
                                    intArray.push_back(std::stoi(token));
                                } catch (...) {
                                    allInts = false;
                                    break;
                                }
                            }
                            if (allInts) {
                                arrayValue = intArray;
                            }
                        }
                        
                        if (!allInts && allDoubles && !tokens.empty()) {
                            std::vector<double> doubleArray;
                            for (const std::string& token : tokens) {
                                try {
                                    doubleArray.push_back(std::stod(token));
                                } catch (...) {
                                    allDoubles = false;
                                    break;
                                }
                            }
                            if (allDoubles) {
                                arrayValue = doubleArray;
                            }
                        }
                        
                        if (allStrings && !tokens.empty()) {
                            std::vector<std::string> stringArray = tokens;
                            arrayValue = stringArray;
                        }
                        
                        if (std::holds_alternative<ConfigValue>(arrayValue)) {
                            set(currentKey, arrayValue, ConfigSource::File);
                        }
                    }
                }
            }
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
    
    // Determine file format by extension
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    if (extension == "json" || extension == "JSON") {
        return saveToJsonFile(filepath);
    } else if (extension == "yaml" || extension == "yml" || extension == "YAML" || extension == "YML") {
        return saveToYamlFile(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            // Convert ConfigValue back to string
            std::string valueStr;
            std::visit([&valueStr](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    valueStr = "\"" + arg + "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    valueStr = arg ? "true" : "false";
                } else {
                    valueStr = std::to_string(arg);
                }
            }, entry.value);
            
            file << entry.key << " = " << valueStr << "\n";
        }
    }
    
    return true;
}

bool Config::saveToJsonFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Convert ConfigValue to JSON
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                bool firstVec = true;
                for (int val : arg) {
                    if (!firstVec) file << ", ";
                    firstVec = false;
                    file << val;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                bool firstVec = true;
                for (double val : arg) {
                    if (!firstVec) file << ", ";
                    firstVec = false;
                    file << val;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                bool firstVec = true;
                for (const std::string& val : arg) {
                    if (!firstVec) file << ", ";
                    firstVec = false;
                    file << "\"" << val << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
    }
    
    file << "\n}\n";
    return true;
}

bool Config::saveToYamlFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << ": ";
        
        // Convert ConfigValue to YAML format
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Check if string contains special characters
                bool needsQuoting = false;
                for (char c : arg) {
                    if (std::isspace(c) || c == ':' || c == '{' || c == '}' || c == '[' || c == ']' || c == ',' || c == '#' || c == '?' || c == '|' || c == '>' || c == '"') {
                        needsQuoting = true;
                        break;
                    }
                }
                if (needsQuoting) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";  // Simple array notation (not fully YAML compliant)
                bool firstVec = true;
                for (int val : arg) {
                    if (!firstVec) file << ", ";
                    firstVec = false;
                    file << val;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                bool firstVec = true;
                for (double val : arg) {
                    if (!firstVec) file << ", ";
                    firstVec = false;
                    file << val;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                bool firstVec = true;
                for (const std::string& val : arg) {
                    if (!firstVec) file << ", ";
                    firstVec = false;
                    // Quote string elements in arrays
                    bool needsQuoting = false;
                    for (char c : val) {
                        if (std::isspace(c) || c == ':' || c == '{' || c == '}' || c == '[' || c == ']' || c == ',' || c == '#' || c == '?' || c == '|' || c == '>' || c == '"') {
                            needsQuoting = true;
                            break;
                        }
                    }
                    if (needsQuoting) {
                        file << "\"" << val << "\"";
                    } else {
                        file << val;
                    }
                }
                file << "]";
            } else {
                file << arg;
            }
            file << "\n";
        }, entry.value);
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

std::string Config::detectFileFormat(const std::string& filepath) {
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    std::string lowerExt = toLower(extension);
    if (lowerExt == "json") {
        return "json";
    } else if (lowerExt == "yaml" || lowerExt == "yml") {
        return "yaml";
    }
    return "unknown";
}

bool Config::isJsonFile(const std::string& filepath) {
    return detectFileFormat(filepath) == "json";
}

bool Config::isYamlFile(const std::string& filepath) {
    std::string format = detectFileFormat(filepath);
    return format == "yaml" || format == "yml";
}

} // namespace nlm
