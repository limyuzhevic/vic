// Config.cpp - Enhanced Configuration System with JSON/YAML Support

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <vector>
#include <map>
#include <stdexcept>
#include <cctype>

namespace nlm {

// Forward declarations for JSON parsing
namespace json {
    using JsonValue = std::variant<bool, int64_t, double, std::string, std::vector<JsonValue>, std::map<std::string, JsonValue>>;
    class JsonParser {
    public:
        static JsonValue parse(const std::string& jsonStr);
        static std::string toString(const JsonValue& value);
    };
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool autoCreateDirs;
    std::string defaultFormat;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->autoCreateDirs = true;
    pImpl->defaultFormat = "txt"; // txt, json, yaml
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Create directory if it doesn't exist
        std::filesystem::path path(filepath);
        if (pImpl->autoCreateDirs && !std::filesystem::exists(path.parent_path())) {
            std::filesystem::create_directories(path.parent_path());
        }

        if (!std::filesystem::exists(path)) {
            return false;
        }

        std::string extension = path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        if (extension == ".json") {
            return loadFromJsonFile(filepath);
        } else if (extension == ".yaml" || extension == ".yml") {
            return loadFromYamlFile(filepath);
        } else {
            // Default to simple key=value format
            return loadFromSimpleFile(filepath);
        }
    } catch (const std::exception& e) {
        return false;
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

    try {
        auto jsonData = json::JsonParser::parse(content);
        return jsonToConfig(jsonData, ConfigSource::File);
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromYamlFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    try {
        // TODO: Implement YAML parsing
        // For now, fallback to simple parsing
        return loadFromSimpleFile(filepath);
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
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
        } else {
            // Invalid format, skip with warning
            // In a real implementation, we would log this
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
        // Handle boolean flags
        else if (arg.size() > 2 && arg[0] == '-' && arg[1] == '-') {
            std::string key = arg.substr(2);
            set(key, "true", ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Create directory if it doesn't exist
        std::filesystem::path path(filepath);
        if (pImpl->autoCreateDirs && !std::filesystem::exists(path.parent_path())) {
            std::filesystem::create_directories(path.parent_path());
        }

        std::string extension = path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }

        if (extension == ".json") {
            return saveToJsonFile(filepath);
        } else if (extension == ".yaml" || extension == ".yml") {
            return saveToYamlFile(filepath);
        } else {
            // Default to simple key=value format
            return saveToSimpleFile(filepath);
        }
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::saveToSimpleFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        file << entry.key << " = ";

        // Format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) file << ", ";
                    first = false;
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << item << "\"";
                    } else {
                        file << item;
                    }
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);

        file << "\n\n";
    }

    return true;
}

bool Config::saveToJsonFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        if (!entry.description.empty()) {
            file << "  // " << entry.description << "\n";
        }
        file << "  \"" << entry.key << "\": ";

        // Format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) file << ", ";
                    first = false;
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << item << "\"";
                    } else {
                        file << item;
                    }
                }
                file << "]";
            } else {
                file << "null";
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

bool Config::saveToYamlFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    file << "%YAML 1.2\n---\n";
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        file << entry.key << ": ";

        // Format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Remove trailing zeros for cleaner YAML
                std::stringstream ss;
                ss << std::fixed << std::setprecision(6) << arg;
                std::string str = ss.str();
                // Remove trailing zeros and potential decimal point
                size_t pos = str.find_last_not_of('0');
                if (pos != std::string::npos && str[pos] == '.') {
                    pos++;
                }
                file << str.substr(0, pos + 1);
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) file << ", ";
                    first = false;
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << item << "\"";
                    } else {
                        file << item;
                    }
                }
                file << "]";
            } else {
                file << "null";
            }
        }, entry.value);
        file << "\n";
    }
    file << "...\n";

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

// JSON parsing implementation
namespace json {
    JsonValue JsonParser::parse(const std::string& jsonStr) {
        // Simplified JSON parser - in production, use a proper JSON library
        std::string str = jsonStr;
        // Remove whitespace
        str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
        
        if (str.empty()) {
            throw std::runtime_error("Empty JSON string");
        }
        
        return parseValue(str);
    }
    
    JsonValue JsonParser::parseValue(const std::string& str) {
        if (str.empty()) {
            throw std::runtime_error("Empty JSON value");
        }
        
        char first = str[0];
        if (first == '{') {
            return parseObject(str);
        } else if (first == '[') {
            return parseArray(str);
        } else if (first == '"') {
            return parseString(str);
        } else if (first == 't' || first == 'f') {
            return parseBoolean(str);
        } else if (first == 'n') {
            return parseNull(str);
        } else {
            // Number
            size_t pos;
            return parseNumber(str, pos);
        }
    }
    
    std::map<std::string, JsonValue> JsonParser::parseObject(const std::string& str) {
        std::map<std::string, JsonValue> result;
        size_t pos = 1;
        
        while (pos < str.size() && str[pos] != '}') {
            // Skip whitespace
            while (pos < str.size() && std::isspace(str[pos])) pos++;
            
            if (pos >= str.size() || str[pos] != '"') {
                throw std::runtime_error("Expected quoted string in object");
            }
            
            size_t keyEnd = str.find('"', pos + 1);
            if (keyEnd == std::string::npos) {
                throw std::runtime_error("Unterminated string in object");
            }
            
            std::string key = str.substr(pos + 1, keyEnd - pos - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace
            while (pos < str.size() && std::isspace(str[pos])) pos++;
            
            if (pos >= str.size() || str[pos] != ':') {
                throw std::runtime_error("Expected colon in object");
            }
            
            pos++;
            while (pos < str.size() && std::isspace(str[pos])) pos++;
            
            // Find the end of the value
            int braceCount = 0;
            int bracketCount = 0;
            size_t valueEnd = pos;
            for (; valueEnd < str.size(); valueEnd++) {
                if (str[valueEnd] == '{') braceCount++;
                else if (str[valueEnd] == '}') braceCount--;
                else if (str[valueEnd] == '[') bracketCount++;
                else if (str[valueEnd] == ']') bracketCount--;
                else if (str[valueEnd] == ',' && braceCount == 0 && bracketCount == 0) {
                    break;
                }
            }
            
            if (valueEnd >= str.size()) {
                throw std::runtime_error("Unterminated value in object");
            }
            
            std::string valueStr = str.substr(pos, valueEnd - pos);
            result[key] = parseValue(valueStr);
            
            pos = valueEnd + 1;
        }
        
        return result;
    }
    
    std::vector<JsonValue> JsonParser::parseArray(const std::string& str) {
        std::vector<JsonValue> result;
        size_t pos = 1;
        
        while (pos < str.size() && str[pos] != ']') {
            // Skip whitespace
            while (pos < str.size() && std::isspace(str[pos])) pos++;
            
            // Find the end of this value
            int braceCount = 0;
            int bracketCount = 0;
            size_t valueEnd = pos;
            for (; valueEnd < str.size(); valueEnd++) {
                if (str[valueEnd] == '{') braceCount++;
                else if (str[valueEnd] == '}') braceCount--;
                else if (str[valueEnd] == '[') bracketCount++;
                else if (str[valueEnd] == ']') bracketCount--;
                else if (str[valueEnd] == ',' && braceCount == 0 && bracketCount == 0) {
                    break;
                }
            }
            
            if (valueEnd >= str.size()) {
                throw std::runtime_error("Unterminated value in array");
            }
            
            std::string valueStr = str.substr(pos, valueEnd - pos);
            result.push_back(parseValue(valueStr));
            
            pos = valueEnd + 1;
        }
        
        return result;
    }
    
    std::string JsonParser::parseString(const std::string& str) {
        if (str.size() < 2 || str[0] != '"' || str[str.size() - 1] != '"') {
            throw std::runtime_error("Invalid string format");
        }
        
        std::string result;
        for (size_t i = 1; i < str.size() - 1; ++i) {
            if (str[i] == '\\' && i + 1 < str.size() - 1) {
                i++;
                switch (str[i]) {
                    case '\\': result += '\\'; break;
                    case '\"': result += '"'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u': {
                        // Unicode escape - simplified
                        if (i + 4 >= str.size() - 1) {
                            throw std::runtime_error("Invalid Unicode escape");
                        }
                        // Extract hex digits
                        std::string hex = str.substr(i + 1, 4);
                        // Convert to UTF-8 (simplified)
                        char32_t codePoint = static_cast<char32_t>(std::stoul(hex, nullptr, 16));
                        // Simple UTF-8 conversion (works for basic characters)
                        if (codePoint < 0x80) {
                            result += static_cast<char>(codePoint);
                        } else {
                            result += "?"; // Fallback for complex Unicode
                        }
                        i += 4;
                        break;
                    }
                    default: result += str[i]; break;
                }
            } else {
                result += str[i];
            }
        }
        
        return result;
    }
    
    bool JsonParser::parseBoolean(const std::string& str) {
        if (str == "true") return true;
        if (str == "false") return false;
        throw std::runtime_error("Invalid boolean value");
    }
    
    bool JsonParser::parseNull(const std::string& str) {
        if (str == "null") {
            // Return a default-constructed JsonValue
            return false; // This shouldn't actually be reached
        }
        throw std::runtime_error("Invalid null value");
    }
    
    JsonValue JsonParser::parseNumber(const std::string& str, size_t& pos) {
        size_t end = str.find_first_not_of("0123456789.-+eE", 0);
        std::string numStr = str.substr(0, end);
        
        try {
            // Check if it's an integer
            size_t dotPos = numStr.find('.');
            size_t ePos = numStr.find_first_of("eE");
            
            if (dotPos == std::string::npos && ePos == std::string::npos) {
                // Integer
                int64_t intVal = std::stoll(numStr);
                return intVal;
            } else {
                // Double
                double doubleVal = std::stod(numStr);
                return doubleVal;
            }
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid number format");
        }
    }
    
    std::string JsonParser::toString(const JsonValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<JsonValue>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    first = false;
                    oss << toString(item);
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::map<std::string, JsonValue>>) {
                oss << "{";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    first = false;
                    oss << "\"" << item.first << "\": " << toString(item.second);
                }
                oss << "}";
            } else {
                oss << "null";
            }
        }, value);
        return oss.str();
    }
    
    bool Config::jsonToConfig(const json::JsonValue& jsonData, ConfigSource source) {
        if (std::holds_alternative<std::map<std::string, json::JsonValue>>(jsonData)) {
            const auto& obj = std::get<std::map<std::string, json::JsonValue>>(jsonData);
            for (const auto& pair : obj) {
                std::string key = pair.first;
                const json::JsonValue& value = pair.second;
                
                // Convert JSON value to ConfigValue
                ConfigValue configValue = jsonToConfigValue(value);
                set(key, configValue, source);
            }
            return true;
        }
        return false;
    }
    
    ConfigValue Config::jsonToConfigValue(const json::JsonValue& jsonValue) {
        if (std::holds_alternative<bool>(jsonValue)) {
            return std::get<bool>(jsonValue);
        } else if (std::holds_alternative<int64_t>(jsonValue)) {
            return std::get<int64_t>(jsonValue);
        } else if (std::holds_alternative<double>(jsonValue)) {
            return std::get<double>(jsonValue);
        } else if (std::holds_alternative<std::string>(jsonValue)) {
            return std::get<std::string>(jsonValue);
        } else if (std::holds_alternative<std::vector<json::JsonValue>>(jsonValue)) {
            const auto& arr = std::get<std::vector<json::JsonValue>>(jsonValue);
            std::vector<std::string> strVec;
            for (const auto& item : arr) {
                if (std::holds_alternative<std::string>(item)) {
                    strVec.push_back(std::get<std::string>(item));
                } else if (std::holds_alternative<int64_t>(item)) {
                    strVec.push_back(std::to_string(std::get<int64_t>(item)));
                } else if (std::holds_alternative<double>(item)) {
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(6) << std::get<double>(item);
                    strVec.push_back(ss.str());
                } else if (std::holds_alternative<bool>(item)) {
                    strVec.push_back(std::get<bool>(item) ? "true" : "false");
                } else {
                    strVec.push_back("#");
                }
            }
            return strVec;
        }
        return std::string("#"); // Default for unknown types
    }
    
    std::string Config::configValueToJson(const ConfigValue& configValue) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    first = false;
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        oss << "\"" << item << "\"";
                    } else {
                        oss << item;
                    }
                }
                oss << "]";
            } else {
                oss << "null";
            }
        }, configValue);
        return oss.str();
    }
} // namespace json

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
