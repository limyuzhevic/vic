#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <ctime>
#include <iostream>

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <ctime>
#include <iostream>

// Simple JSON parser implementation (basic support for strings, numbers, arrays)
namespace nlm {
    namespace {
        bool parseJsonValue(const std::string& str, ConfigValue& outValue) {
            // Trim whitespace
            size_t start = str.find_first_not_of(" \t\n\r");
            size_t end = str.find_last_not_of(" \t\n\r");
            if (start == std::string::npos || end == std::string::npos) return false;
            
            std::string content = str.substr(start, end - start + 1);
            
            // Parse JSON values
            if (content.empty()) return false;
            
            if (content.front() == '\"' && content.back() == '\"') {
                // String
                outValue = content.substr(1, content.size() - 2);
            } else if (content == "true" || content == "false") {
                // Boolean
                outValue = (content == "true");
            } else if (std::isdigit(content[0]) || (content[0] == '-' && content.size() > 1)) {
                // Number
                if (content.find('.') != std::string::npos) {
                    // Double
                    double val = std::stod(content);
                    outValue = val;
                } else {
                    // Integer
                    int val = std::stoi(content);
                    outValue = val;
                }
            } else if (content.front() == '[' && content.back() == ']') {
                // Array (not fully implemented for complex types)
                // For now, just skip complex array parsing
                return false;
            }
            
            return true;
        }
        
        bool parseJsonObject(const std::string& str, std::unordered_map<std::string, std::string>& outMap) {
            if (str.front() != '{' || str.back() != '}') return false;
            
            std::string content = str.substr(1, str.size() - 2);
            size_t pos = 0;
            
            while (pos < content.size()) {
                // Find next key
                size_t keyStart = content.find('\"', pos);
                if (keyStart == std::string::npos) break;
                size_t keyEnd = content.find('\"', keyStart + 1);
                if (keyEnd == std::string::npos) break;
                
                std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
                
                // Find colon
                size_t colonPos = content.find(':', keyEnd);
                if (colonPos == std::string::npos) break;
                
                // Find value (could be string, number, or object)
                size_t valueStart = content.find_first_not_of(' ', colonPos + 1);
                if (valueStart == std::string::npos) break;
                
                // Simple value parsing
                if (content[valueStart] == '\"') {
                    size_t valueEnd = content.find('\"', valueStart + 1);
                    if (valueEnd == std::string::npos) break;
                    std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
                    outMap[key] = value;
                    pos = valueEnd + 2;
                } else if (std::isdigit(content[valueStart]) || content[valueStart] == '-') {
                    // Find number end
                    size_t valueEnd = content.find_first_of(",}", valueStart);
                    if (valueEnd == std::string::npos) valueEnd = content.size();
                    std::string value = content.substr(valueStart, valueEnd - valueStart);
                    outMap[key] = value;
                    pos = valueEnd;
                }
                
                // Skip comma
                if (pos < content.size() && content[pos] == ',') {
                    pos++;
                }
            }
            
            return true;
        }
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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

bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Simple JSON parsing
    // Parse key-value pairs from JSON object { "key": "value", ... }
    try {
        // Remove whitespace for simpler parsing
        std::string cleaned;
        for (char c : content) {
            if (!std::isspace(c)) {
                cleaned += c;
            }
        }
        
        // Basic JSON object parsing: { "key1": value1, "key2": value2, ... }
        if (cleaned.empty() || cleaned.front() != '{' || cleaned.back() != '}') {
            NLM_LOG_ERROR("Invalid JSON format: not a JSON object");
            return false;
        }
        
        std::string objectContent = cleaned.substr(1, cleaned.size() - 2);
        size_t pos = 0;
        
        while (pos < objectContent.size()) {
            // Find key
            size_t keyStart = objectContent.find('\"', pos);
            if (keyStart == std::string::npos) break;
            size_t keyEnd = objectContent.find('\"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = objectContent.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find colon after key
            size_t colonPos = objectContent.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            // Find value start
            size_t valuePos = objectContent.find_first_not_of(' ,', colonPos + 1);
            if (valuePos == std::string::npos) break;
            
            // Parse value based on its type
            ConfigValue configValue;
            bool valueParsed = false;
            
            // String value
            if (objectContent[valuePos] == '\"') {
                size_t valueEnd = objectContent.find('\"', valuePos + 1);
                if (valueEnd != std::string::npos) {
                    std::string strValue = objectContent.substr(valuePos + 1, valueEnd - valuePos - 1);
                    configValue = strValue;
                    valueParsed = true;
                    pos = valueEnd + 2;
                }
            }
            // Number value (int or double)
            else if (std::isdigit(objectContent[valuePos]) || objectContent[valuePos] == '-' || objectContent[valuePos] == '+') {
                size_t valueEnd = objectContent.find_first_of(',}', valuePos);
                if (valueEnd != std::string::npos) {
                    std::string numStr = objectContent.substr(valuePos, valueEnd - valuePos);
                    try {
                        if (numStr.find('.') != std::string::npos) {
                            double val = std::stod(numStr);
                            configValue = val;
                        } else {
                            int val = std::stoi(numStr);
                            configValue = val;
                        }
                        valueParsed = true;
                        pos = valueEnd;
                    } catch (...) {
                        // Skip invalid number
                    }
                }
            }
            // Boolean value
            else if (objectContent.substr(valuePos, 4) == "true" || objectContent.substr(valuePos, 5) == "false") {
                bool boolValue = (objectContent.substr(valuePos, 4) == "true");
                configValue = boolValue;
                valueParsed = true;
                // Skip "true" or "false"
                pos = valuePos + (boolValue ? 4 : 5);
            }
            
            if (!valueParsed) {
                // Skip to next key
                size_t commaPos = objectContent.find(',', valuePos);
                if (commaPos != std::string::npos) {
                    pos = commaPos + 1;
                } else {
                    break;
                }
                continue;
            }
            
            // Skip comma if present
            if (pos < objectContent.size() && objectContent[pos] == ',') {
                pos++;
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Successfully parsed JSON configuration file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON configuration file: " + std::string(e.what()));
        return false;
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
    
    // Determine format based on file extension
    bool isJSON = (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json");
    
    if (isJSON) {
        // Save in JSON format
        json j;
        for (const auto& entry : pImpl->entries) {
            j[entry.key] = std::visit([](const auto& val) -> json {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    return val;
                } else if constexpr (std::is_same_v<T, int> ||
                                   std::is_same_v<T, int64_t> ||
                                   std::is_same_v<T, uint64_t>) {
                    return static_cast<int64_t>(val);
                } else if constexpr (std::is_same_v<T, float> ||
                                   std::is_same_v<T, double>) {
                    return static_cast<double>(val);
                } else if constexpr (std::is_same_v<T, bool>) {
                    return val;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    return json(val);
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    return json(val);
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    return json(val);
                } else {
                    return "";
                }
            }, entry.value);
            
            if (!entry.description.empty()) {
                j["# " + entry.key] = entry.description;
            }
        }
        
        file << j.dump(2);
    } else {
        // Save in legacy key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Convert value to string
            std::visit([&file](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << val << "\"";
                } else if constexpr (std::is_same_v<T, int> ||
                                   std::is_same_v<T, int64_t> ||
                                   std::is_same_v<T, uint64_t>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, float> ||
                                   std::is_same_v<T, double>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (val ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                   std::is_same_v<T, std::vector<double>> ||
                                   std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i) file << ", ";
                        file << val[i];
                    }
                    file << "]";
                }
            }, entry.value);
            
            file << "\n\n";
        }
    }
    
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
