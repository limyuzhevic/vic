#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cctype>
#include <regex>
#include <iostream>

namespace nlm {

// JSON parsing helper functions
namespace JsonParser {
    // Simple JSON value parser (supports basic types)
    ConfigValue parseJsonValue(const std::string& valueStr) {
        std::string trimmed = Config::trim(valueStr);
        
        // Remove surrounding whitespace
        if (trimmed.empty()) {
            return std::string();
        }
        
        // Parse string
        if ((trimmed.front() == '"' && trimmed.back() == '"') ||
            (trimmed.front() == '\'' && trimmed.back() == '\'')) {
            return trimmed.substr(1, trimmed.size() - 2);
        }
        
        // Parse boolean
        if (trimmed == "true") return true;
        if (trimmed == "false") return false;
        
        // Parse numbers
        try {
            // Check for float
            size_t pos;
            double d = std::stod(trimmed, &pos);
            if (pos == trimmed.size()) {
                return d;
            }
            
            // Check for int64_t
            int64_t l = std::stoll(trimmed, &pos);
            if (pos == trimmed.size()) {
                return l;
            }
            
            // Check for int
            int i = std::stoi(trimmed, &pos);
            if (pos == trimmed.size()) {
                return i;
            }
        } catch (...) {
            // If parsing fails, return as string
        }
        
        // Try to parse arrays
        if (trimmed.front() == '[' && trimmed.back() == ']') {
            std::string inner = trimmed.substr(1, trimmed.size() - 2);
            std::vector<double> array;
            
            size_t start = 0;
            size_t pos = 0;
            while ((pos = inner.find(',', start)) != std::string::npos) {
                std::string item = Config::trim(inner.substr(start, pos - start));
                try {
                    double val = std::stod(item);
                    array.push_back(val);
                } catch (...) {
                    // Skip invalid items
                }
                start = pos + 1;
            }
            
            // Add last item
            if (start < inner.size()) {
                std::string item = Config::trim(inner.substr(start));
                try {
                    double val = std::stod(item);
                    array.push_back(val);
                } catch (...) {
                    // Skip invalid items
                }
            }
            
            return array;
        }
        
        // If all else fails, return as string
        return trimmed;
    }
    
    // Parse JSON object from file
    bool parseJsonFromFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        // Remove whitespace for simplicity (in real implementation, would parse properly)
        std::string cleaned;
        for (char c : content) {
            if (!std::isspace(c)) {
                cleaned += c;
            }
        }
        
        // Simple JSON object parsing: {"key1": "value1", "key2": 123, "key3": [1,2,3]}
        if (cleaned.front() != '{' || cleaned.back() != '}') {
            return false;
        }
        
        std::string inner = cleaned.substr(1, cleaned.size() - 2);
        size_t pos = 0;
        
        while (pos < inner.size()) {
            // Find key
            size_t keyStart = inner.find('"', pos);
            if (keyStart == std::string::npos) break;
            size_t keyEnd = inner.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = inner.substr(keyStart + 1, keyEnd - keyStart - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace
            while (pos < inner.size() && std::isspace(inner[pos])) pos++;
            
            if (pos >= inner.size() || inner[pos] != ':') break;
            pos++;
            
            // Skip whitespace
            while (pos < inner.size() && std::isspace(inner[pos])) pos++;
            
            // Find value end
            size_t valueEnd = inner.find(',', pos);
            if (valueEnd == std::string::npos) {
                valueEnd = inner.size();
            }
            
            std::string valueStr = inner.substr(pos, valueEnd - pos);
            ConfigValue value = parseJsonValue(Config::trim(valueStr));
            
            entries.emplace_back(key, value, ConfigSource::File);
            
            pos = valueEnd + 1;
            while (pos < inner.size() && std::isspace(inner[pos])) pos++;
        }
        
        return true;
    }
    
    // Format value as JSON string
    std::string formatJsonValue(const ConfigValue& value) {
        std::ostringstream oss;
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << escapeString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                oss << arg;
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
                    oss << "\"" << escapeString(arg[i]) << "\"";
                }
                oss << "]";
            }
        }, value);
        
        return oss.str();
    }
    
    // Escape string for JSON
    std::string escapeString(const std::string& str) {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '\\': oss << "\\\\"; break;
                case '\"': oss << "\\\""; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default:
                    if (c < 32 || c > 126) {
                        oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    } else {
                        oss << c;
                    }
                    break;
            }
        }
        return oss.str();
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // For future validation and schema support
    std::map<std::string, std::function<bool(const ConfigValue&)>> validators;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // First try to parse as JSON
    std::vector<ConfigEntry> jsonEntries;
    if (JsonParser::parseJsonFromFile(filepath, jsonEntries)) {
        // Clear existing entries and load JSON entries
        clear();
        pImpl->entries = std::move(jsonEntries);
        return true;
    }
    
    // Fall back to simple key=value format
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
            
            // Try to parse as number or boolean
            set(key, JsonParser::parseJsonValue(value), ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, JsonParser::parseJsonValue(value), ConfigSource::CommandLine);
            } else {
                std::cerr << "Error: Missing value for argument " << arg << std::endl;
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, JsonParser::parseJsonValue(value), ConfigSource::CommandLine);
        } else {
            std::cerr << "Error: Invalid argument format " << arg << std::endl;
            success = false;
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Start JSON object
    file << "{" << std::endl;
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        file << "  \"" << entry.key << "\": " << JsonParser::formatJsonValue(entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << "," << std::endl;
        } else {
            file << std::endl;
        }
    }
    
    file << "}" << std::endl;
    
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
    // Validate value type
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry
        it->value = value;
        it->source = source;
        
        // Run validator if any
        auto validatorIt = pImpl->validators.find(key);
        if (validatorIt != pImpl->validators.end() && !validatorIt->second(value)) {
            std::cerr << "Warning: Value for key '" << key << "' failed validation" << std::endl;
        }
    } else {
        // Add new entry
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
    
    // Remove validator if any
    pImpl->validators.erase(key);
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
    pImpl->validators.clear();
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