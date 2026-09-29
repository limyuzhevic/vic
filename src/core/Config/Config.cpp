#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <cctype>
#include <cstring>

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
    // Supports: int, int64_t, double, bool, string, arrays of these types
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    // Simple JSON parser - read entire file
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    if (content.empty()) {
        NLM_LOG_WARNING("Configuration file is empty: " + filepath);
        return false;
    }
    
    size_t pos = 0;
    while (pos < content.length()) {
        // Find next key (looking for "key": value pattern)
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) {
            NLM_LOG_ERROR("Unterminated string in configuration file at position " + std::to_string(keyStart));
            return false;
        }
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        pos = keyEnd + 1;
        
        // Find colon
        size_t colonPos = content.find(':', pos);
        if (colonPos == std::string::npos) {
            NLM_LOG_ERROR("Missing colon after key '" + key + "' in configuration file");
            return false;
        }
        
        pos = colonPos + 1;
        
        // Skip whitespace
        while (pos < content.length() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t')) {
            pos++;
        }
        
        // Parse value
        std::string valueStr;
        if (pos >= content.length()) break;
        
        if (content[pos] == '"') {
            // String value
            size_t valueEnd = content.find('"', pos + 1);
            if (valueEnd == std::string::npos) {
                NLM_LOG_ERROR("Unterminated string value for key '" + key + "' in configuration file");
                return false;
            }
            valueStr = content.substr(pos + 1, valueEnd - pos - 1);
            pos = valueEnd + 1;
        } else if (content[pos] == 't' || content[pos] == 'f') {
            // Boolean value
            size_t valueEnd = content.find_first_of(",}\n", pos);
            if (valueEnd == std::string::npos) {
                NLM_LOG_ERROR("Invalid boolean value for key '" + key + "' in configuration file");
                return false;
            }
            valueStr = content.substr(pos, valueEnd - pos);
            
            // Validate boolean value
            std::string boolStr = valueStr;
            std::transform(boolStr.begin(), boolStr.end(), boolStr.begin(), ::tolower);
            if (boolStr != "true" && boolStr != "false") {
                NLM_LOG_ERROR("Invalid boolean value '" + boolStr + "' for key '" + key + "' in configuration file");
                return false;
            }
            
            pos = valueEnd;
        } else if (content[pos] == 'n') {
            // Null value (skip)
            size_t valueEnd = content.find_first_of(",}\n", pos);
            if (valueEnd == std::string::npos) break;
            pos = valueEnd;
        } else if (isdigit(content[pos]) || content[pos] == '-' || content[pos] == '+') {
            // Number value
            size_t valueEnd = content.find_first_of(",}\n", pos);
            if (valueEnd == std::string::npos) {
                NLM_LOG_ERROR("Invalid number value for key '" + key + "' in configuration file");
                return false;
            }
            valueStr = content.substr(pos, valueEnd - pos);
            
            // Validate number format (basic check)
            std::string numStr = valueStr;
            bool hasDecimal = false;
            bool hasExponent = false;
            for (char c : numStr) {
                if (c == '.') hasDecimal = true;
                else if (c == 'e' || c == 'E') hasExponent = true;
            }
            
            pos = valueEnd;
        } else if (content[pos] == '[') {
            // Array value - simple parsing
            int bracketCount = 1;
            size_t valueEnd = pos;
            while (valueEnd < content.length() && bracketCount > 0) {
                valueEnd++;
                if (content[valueEnd] == '[') bracketCount++;
                else if (content[valueEnd] == ']') bracketCount--;
            }
            if (bracketCount != 0) {
                NLM_LOG_ERROR("Unterminated array for key '" + key + "' in configuration file");
                return false;
            }
            valueStr = content.substr(pos, valueEnd - pos + 1);
            pos = valueEnd + 1;
        } else {
            // Unknown type, skip
            size_t valueEnd = content.find_first_of(",}\n", pos);
            if (valueEnd == std::string::npos) break;
            valueStr = content.substr(pos, valueEnd - pos);
            pos = valueEnd;
        }
        
        // Trim whitespace from value
        size_t start = valueStr.find_first_not_of(" \t\n\r");
        if (start != std::string::npos) {
            size_t end = valueStr.find_last_not_of(" \t\n\r");
            valueStr = valueStr.substr(start, end - start + 1);
            set(key, valueStr, ConfigSource::File);
        }
        
        // Skip comma
        if (pos < content.length() && content[pos] == ',') {
            pos++;
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
    
    // Write JSON format configuration
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format double without trailing zeros
                file << std::fixed << std::setprecision(6) << arg;
                // Clean up formatting
                std::string str = std::to_string(arg);
                if (str.find('.') != std::string::npos) {
                    // Remove trailing zeros
                    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
                    if (str.back() == '.') {
                        str.pop_back();
                    }
                }
                file << str;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(arg) << "\"";
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
                    // Format double without trailing zeros
                    std::string str = std::to_string(arg[j]);
                    if (str.find('.') != std::string::npos) {
                        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
                        if (str.back() == '.') {
                            str.pop_back();
                        }
                    }
                    file << str;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << escapeString(arg[j]) << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
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

std::string Config::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 0x20 || c == 0x7f) {
                    result += "\\u";
                    char hex[5];
                    snprintf(hex, sizeof(hex), "%04x", (unsigned char)c);
                    result += hex;
                } else {
                    result += c;
                }
                break;
        }
    }
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