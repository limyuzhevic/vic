#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <iomanip>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Helper methods for parsing
    static bool isWhitespace(char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }
    
    static std::string trim(const std::string& str) {
        size_t start = 0;
        size_t end = str.size();
        
        while (start < end && isWhitespace(str[start])) start++;
        while (end > start && isWhitespace(str[end - 1])) end--;
        
        return str.substr(start, end - start);
    }
    
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    // JSON parsing helpers
    static std::string trimJsonString(const std::string& str) {
        if (str.size() < 2) return str;
        char quote = str[0];
        if ((quote == '"' || quote == '\'') && str.back() == quote) {
            return str.substr(1, str.size() - 2);
        }
        return str;
    }
    
    static bool parseJsonValue(const std::string& line, std::string& key, ConfigValue& value, std::string& description) {
        // Remove leading/trailing whitespace
        std::string trimmed = trim(line);
        
        // Skip empty lines and comments
        if (trimmed.empty() || trimmed[0] == '#') {
            return false;
        }
        
        // Find key-value separator
        size_t colonPos = trimmed.find(':');
        if (colonPos == std::string::npos || colonPos == 0) {
            return false;
        }
        
        key = trim(trimmed.substr(0, colonPos - 1));
        std::string valueStr = trim(trimmed.substr(colonPos + 1));
        
        // Parse value based on first character
        if (valueStr.empty()) {
            return false;
        }
        
        char first = valueStr[0];
        
        if (first == '"' || first == '\'') {
            // String value
            value = trimJsonString(valueStr);
            description = "JSON string value";
            return true;
        } else if (first == '[' && valueStr.back() == ']') {
            // Array value - simplified parsing
            std::string arrayContent = valueStr.substr(1, valueStr.size() - 2);
            std::vector<std::string> stringElements;
            
            size_t pos = 0;
            while (pos < arrayContent.size()) {
                size_t commaPos = arrayContent.find(',', pos);
                if (commaPos == std::string::npos) {
                    commaPos = arrayContent.size();
                }
                
                std::string element = trim(arrayContent.substr(pos, commaPos - pos));
                if (!element.empty()) {
                    // Try to parse as number or boolean
                    try {
                        if (element.find('.') != std::string::npos) {
                            double num = std::stod(element);
                            stringElements.push_back(std::to_string(num));
                        } else {
                            int num = std::stoi(element);
                            stringElements.push_back(std::to_string(num));
                        }
                    } catch (...) {
                        // Keep as string
                        stringElements.push_back(element);
                    }
                }
                
                pos = commaPos + 1;
                while (pos < arrayContent.size() && arrayContent[pos] == ' ') pos++;
            }
            
            // Determine array type
            bool allInts = true;
            bool allDoubles = true;
            for (const auto& elem : stringElements) {
                try {
                    size_t pos;
                    int intVal = std::stoi(elem, &pos);
                    if (pos != elem.size()) allInts = false;
                } catch (...) {
                    allInts = false;
                }
                
                try {
                    size_t pos;
                    double doubleVal = std::stod(elem, &pos);
                    if (pos != elem.size()) allDoubles = false;
                } catch (...) {
                    allDoubles = false;
                }
            }
            
            if (allInts) {
                std::vector<int> intArray;
                for (const auto& elem : stringElements) {
                    intArray.push_back(std::stoi(elem));
                }
                value = intArray;
                description = "JSON integer array";
            } else if (allDoubles) {
                std::vector<double> doubleArray;
                for (const auto& elem : stringElements) {
                    doubleArray.push_back(std::stod(elem));
                }
                value = doubleArray;
                description = "JSON float array";
            } else {
                std::vector<std::string> stringArray;
                for (const auto& elem : stringElements) {
                    stringArray.push_back(elem);
                }
                value = stringArray;
                description = "JSON string array";
            }
            
            return true;
        } else if (first == '{' && valueStr.back() == '}') {
            // Object value - skip for now, treat as string
            value = valueStr;
            description = "JSON object (not fully supported)";
            return true;
        } else if (first == 't' || first == 'f') {
            // Boolean value
            if (valueStr == "true") {
                value = true;
                description = "JSON boolean (true)";
            } else if (valueStr == "false") {
                value = false;
                description = "JSON boolean (false)";
            } else {
                return false;
            }
            return true;
        } else {
            // Number or string
            try {
                if (valueStr.find('.') != std::string::npos) {
                    double num = std::stod(valueStr);
                    value = num;
                    description = "JSON float value";
                } else {
                    int num = std::stoi(valueStr);
                    value = num;
                    description = "JSON integer value";
                }
                return true;
            } catch (...) {
                // Treat as string
                value = valueStr;
                description = "JSON string value";
                return true;
            }
        }
    }
    
    static bool parseJsonFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // Parse JSON object
        size_t objStart = content.find('{');
        size_t objEnd = content.rfind('}');
        
        if (objStart == std::string::npos || objEnd == std::string::npos || objEnd < objStart) {
            return false;
        }
        
        std::string jsonObj = content.substr(objStart + 1, objEnd - objStart - 1);
        
        // Simple JSON parsing - extract key:value pairs
        size_t pos = 0;
        while (pos < jsonObj.size()) {
            // Find key
            size_t keyStart = jsonObj.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = jsonObj.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = jsonObj.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find value
            size_t valueStart = jsonObj.find(':', keyEnd);
            if (valueStart == std::string::npos) break;
            valueStart++;
            
            // Find next key or end of object
            size_t nextKey = jsonObj.find('"', valueStart);
            size_t nextComma = jsonObj.find(',', valueStart);
            
            size_t valueEnd;
            if (nextKey == std::string::npos && nextComma == std::string::npos) {
                valueEnd = objEnd - 1;
            } else if (nextKey == std::string::npos) {
                valueEnd = nextComma;
            } else if (nextComma == std::string::npos) {
                valueEnd = nextKey;
            } else {
                valueEnd = std::min(nextKey, nextComma);
            }
            
            std::string valueStr = jsonObj.substr(valueStart, valueEnd - valueStart);
            
            // Trim whitespace
            key = trim(key);
            valueStr = trim(valueStr);
            
            ConfigValue parsedValue;
            std::string description;
            
            if (parseJsonValue("dummy:" + valueStr, key, parsedValue, description)) {
                entries.emplace_back(key, parsedValue, ConfigSource::File, description);
            }
            
            pos = valueEnd + 1;
            while (pos < jsonObj.size() && (jsonObj[pos] == ',' || jsonObj[pos] == ' ' || jsonObj[pos] == '\n' || jsonObj[pos] == '\r')) pos++;
        }
        
        return !entries.empty();
    }
    
    static bool parseIniFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        std::string currentSection;
        
        while (std::getline(file, line)) {
            // Remove leading/trailing whitespace
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Check for section
            if (line[0] == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size() - 2);
                continue;
            }
            
            // Parse key=value pair
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
                
                // Add section prefix if present
                std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;
                
                entries.emplace_back(fullKey, value, ConfigSource::File, "INI configuration value");
            }
        }
        
        return !entries.empty();
    }
    
    static bool parseYamlFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
        // YAML parsing is more complex, implement basic support
        // For now, treat as INI format with improved parsing
        return parseIniFile(filepath, entries);
    }
    
    static ConfigValue parseValue(const std::string& valueStr) {
        // Try to parse as different types
        if (valueStr.empty()) {
            return std::string();
        }
        
        // Check for boolean
        if (valueStr == "true") return true;
        if (valueStr == "false") return false;
        
        // Check for number
        try {
            if (valueStr.find('.') != std::string::npos) {
                return std::stod(valueStr);
            } else {
                return std::stoi(valueStr);
            }
        } catch (...) {
            // Return as string
            return valueStr;
        }
    }
    
    static std::string getFileExtension(const std::string& filename) {
        size_t dotPos = filename.find_last_of('.');
        if (dotPos == std::string::npos) return "";
        return filename.substr(dotPos);
    }
    
    static bool isJsonFile(const std::string& filename) {
        std::string ext = toLower(getFileExtension(filename));
        return ext == ".json";
    }
    
    static bool isIniFile(const std::string& filename) {
        std::string ext = toLower(getFileExtension(filename));
        return ext == ".ini" || ext == ".cfg" || ext == ".conf";
    }
    
    static bool isYamlFile(const std::string& filename) {
        std::string ext = toLower(getFileExtension(filename));
        return ext == ".yaml" || ext == ".yml";
    }
}; // struct Impl

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension to determine format
    std::string ext = pImpl->toLower(pImpl->getFileExtension(filepath));
    
    bool success = false;
    
    if (ext == ".json") {
        success = pImpl->parseJsonFile(filepath, pImpl->entries);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = pImpl->parseYamlFile(filepath, pImpl->entries);
    } else if (ext == ".ini" || ext == ".cfg" || ext == ".conf") {
        success = pImpl->parseIniFile(filepath, pImpl->entries);
    } else {
        // Try auto-detection
        std::ifstream file(filepath);
        if (file.is_open()) {
            std::string firstLine;
            std::getline(file, firstLine);
            file.close();
            
            if (firstLine.find('{') != std::string::npos) {
                success = pImpl->parseJsonFile(filepath, pImpl->entries);
            } else if (firstLine.find('[') != std::string::npos) {
                success = pImpl->parseIniFile(filepath, pImpl->entries);
            }
        }
    }
    
    if (!success) {
        NLM_LOG_WARNING("Failed to load configuration from: " + filepath);
    }
    
    return success;
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
    
    // Write in JSON format (recommended for interoperability)
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << std::setprecision(6) << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << arg[j] << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "}\n";
    
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
        pImpl->entries.emplace_back(key, value, source, "Dynamic configuration");
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
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
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
