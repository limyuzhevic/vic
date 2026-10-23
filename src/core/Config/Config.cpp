#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <stdexcept>
#include <regex>
#include <cmath>

// Add logging support
#ifdef NLM_LOGGING_AVAILABLE
#include "core/Logger/Logger.hpp"
#else
#include <iostream>
#define NLM_LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << std::endl
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Enhanced configuration file parser with multiple format support
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        try {
            // Enhanced parsing with better error handling
            parseLine(line, lineNumber, ConfigSource::File);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Config file " + filepath + " line " + std::to_string(lineNumber) + ": " + e.what());
            // Continue parsing other lines instead of failing entirely
        }
    }
    
    return true;
}

void Config::parseLine(const std::string& line, size_t lineNumber, ConfigSource source) {
    // Enhanced line parsing with better error handling and validation
    // Supports: key = value, key: value, key value
    
    // Skip empty lines and comments (already done in loadFromFile)
    
    // Find delimiter
    size_t eqPos = line.find('=');
    size_t colonPos = line.find(':');
    
    // Determine delimiter (prefer = over :)
    size_t delimiterPos;
    std::string delimiter;
    
    if (eqPos != std::string::npos && (colonPos == std::string::npos || eqPos < colonPos)) {
        delimiterPos = eqPos;
        delimiter = "=";
    } else if (colonPos != std::string::npos) {
        delimiterPos = colonPos;
        delimiter = ":";
    } else {
        // No delimiter found - treat as boolean flag
        std::string key = trim(line);
        if (key.empty()) {
            return;
        }
        // Convert to boolean if key is a known boolean flag
        bool value = true;
        if (key == "false" || key == "no" || key == "off" || key == "disable") {
            value = false;
        }
        set(key, value, source);
        return;
    }
    
    std::string key = trim(line.substr(0, delimiterPos));
    std::string valueStr = trim(line.substr(delimiterPos + 1));
    
    if (key.empty()) {
        throw std::invalid_argument("Empty key name");
    }
    
    // Remove inline comments
    size_t commentPos = valueStr.find('#');
    if (commentPos != std::string::npos) {
        valueStr = trim(valueStr.substr(0, commentPos));
    }
    
    // Skip empty values
    if (valueStr.empty()) {
        return;
    }
    
    // Enhanced value parsing with type detection
    auto parsedValue = parseConfigValue(valueStr, key, lineNumber);
    set(key, parsedValue, source);
}

ConfigValue Config::parseConfigValue(const std::string& valueStr, const std::string& key, size_t lineNumber) {
    // Intelligent type detection and parsing
    std::string lowerValue = toLower(valueStr);
    
    // Boolean values
    if (lowerValue == "true" || lowerValue == "yes" || lowerValue == "on" || lowerValue == "enable") {
        return true;
    }
    if (lowerValue == "false" || lowerValue == "no" || lowerValue == "off" || lowerValue == "disable") {
        return false;
    }
    
    // Integer values (decimal, hex, octal)
    try {
        if (lowerValue.substr(0, 2) == "0x") {
            // Hex
            size_t pos;
            int64_t hexVal = std::stoll(valueStr.substr(2), &pos, 16);
            if (pos == valueStr.size() - 2) {
                return hexVal;
            }
        } else if (lowerValue.substr(0, 1) == "0" && valueStr.size() > 1) {
            // Octal (if contains only octal digits)
            bool isOctal = true;
            for (size_t i = 1; i < valueStr.size(); ++i) {
                if (valueStr[i] < '0' || valueStr[i] > '7') {
                    isOctal = false;
                    break;
                }
            }
            if (isOctal) {
                size_t pos;
                int64_t octVal = std::stoll(valueStr.substr(1), &pos, 8);
                if (pos == valueStr.size() - 1) {
                    return octVal;
                }
            }
        } else {
            // Decimal
            size_t pos;
            int64_t intVal = std::stoll(valueStr, &pos);
            if (pos == valueStr.size()) {
                return intVal;
            }
        }
    } catch (...) {
        // Not an integer, continue to other types
    }
    
    // Float values
    try {
        size_t pos;
        double floatVal = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            return floatVal;
        }
    } catch (...) {
        // Not a float, continue to string
    }
    
    // String value (remove quotes)
    std::string result = valueStr;
    if ((valueStr.size() >= 2 && 
         ((valueStr.front() == '\"' && valueStr.back() == '\"') ||
          (valueStr.front() == '\'' && valueStr.back() == '\'')))) {
        result = valueStr.substr(1, valueStr.size() - 2);
    }
    
    return result;
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
