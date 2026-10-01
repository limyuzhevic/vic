#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <iomanip>

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
    
    // Try to detect if it's JSON format (starts with { or [)
    std::string firstChar;
    file >> firstChar;
    file.clear(); // Reset file state
    file.seekg(0);
    
    bool isJSON = (firstChar == "{");
    
    if (isJSON) {
        // Try to parse as JSON
        return loadFromJSONFile(filepath);
    } else {
        // Parse as simple key=value format
        std::string line;
        int lineNum = 0;
        while (std::getline(file, line)) {
            lineNum++;
            line = trim(line);
            
            // Skip empty lines and comments
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
                std::cerr << "WARNING: Invalid config line " << lineNum << ": " << line << std::endl;
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

bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Simple JSON object parsing (supports basic key-value pairs)
    // For production, use a proper JSON library like nlohmann/json
    if (content.empty() || content[0] != '{') {
        std::cerr << "WARNING: File " << filepath << " is not a valid JSON object" << std::endl;
        return false;
    }
    
    size_t pos = 1;
    while (pos < content.size() - 1) {
        // Find next key
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        pos = keyEnd + 2;  // Skip " and :
        
        // Find value
        size_t valueEnd = content.find(',', pos);
        if (valueEnd == std::string::npos) {
            valueEnd = content.find('}', pos);
        }
        if (valueEnd == std::string::npos) break;
        
        std::string valueStr = content.substr(pos, valueEnd - pos);
        valueStr = trim(valueStr);
        
        // Parse value based on type
        ConfigValue configValue = jsonStringToConfigValue(valueStr);
        set(key, configValue, ConfigSource::File);
        
        pos = valueEnd + 1;
        if (content[pos] == ',') pos++; // Skip comma
    }
    
    return true;
}

ConfigValue Config::jsonStringToConfigValue(const std::string& jsonStr) {
    std::string trimmed = trim(jsonStr);
    
    // Handle strings
    if ((trimmed.front() == '"' && trimmed.back() == '"') ||
        (trimmed.front() == '\'' && trimmed.back() == '\'')) {
        return trimmed.substr(1, trimmed.size() - 2);
    }
    
    // Handle numbers
    if (trimmed.find('.') != std::string::npos || 
        trimmed.find('e') != std::string::npos || 
        trimmed.find('E') != std::string::npos) {
        try {
            size_t pos;
            double val = std::stod(trimmed, pos);
            if (pos == trimmed.size()) return val;
        } catch (...) {}
    } else {
        try {
            size_t pos;
            int64_t val = std::stoll(trimmed, pos);
            if (pos == trimmed.size()) return val;
        } catch (...) {}
    }
    
    // Handle booleans
    if (trimmed == "true" || trimmed == "false") {
        return trimmed == "true";
    }
    
    // Handle arrays (simplified)
    if (trimmed.front() == '[' && trimmed.back() == ']') {
        std::string arrayContent = trimmed.substr(1, trimmed.size() - 2);
        std::vector<std::string> elements;
        
        size_t start = 0;
        size_t end = 0;
        while ((end = arrayContent.find(',', start)) != std::string::npos) {
            elements.push_back(trim(arrayContent.substr(start, end - start)));
            start = end + 1;
        }
        elements.push_back(trim(arrayContent.substr(start)));
        
        // Try to determine array element type
        bool allInts = true;
        bool allDoubles = true;
        bool allStrings = true;
        
        for (const auto& elem : elements) {
            if (!isNumericInt(elem)) allInts = false;
            if (!isNumericDouble(elem)) allDoubles = false;
            allStrings = false; // Strings always valid
        }
        
        if (allInts) {
            std::vector<int> intArray;
            for (const auto& elem : elements) {
                intArray.push_back(std::stoi(elem));
            }
            return intArray;
        } else if (allDoubles) {
            std::vector<double> doubleArray;
            for (const auto& elem : elements) {
                doubleArray.push_back(std::stod(elem));
            }
            return doubleArray;
        } else {
            return elements;
        }
    }
    
    // Default to string
    return trimmed;
}

bool Config::isNumericInt(const std::string& str) {
    if (str.empty()) return false;
    size_t pos;
    int64_t val;
    try {
        val = std::stoll(str, pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

bool Config::isNumericDouble(const std::string& str) {
    if (str.empty()) return false;
    size_t pos;
    double val;
    try {
        val = std::stod(str, pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
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

// JSON processing implementation templates
ConfigValue Config::jsonStringToConfigValue(const std::string& jsonStr) {
    std::string trimmed = trim(jsonStr);
    
    // Handle strings
    if ((trimmed.front() == '"' && trimmed.back() == '"') ||
        (trimmed.front() == '\'' && trimmed.back() == '\'')) {
        return trimmed.substr(1, trimmed.size() - 2);
    }
    
    // Handle numbers
    if (trimmed.find('.') != std::string::npos || 
        trimmed.find('e') != std::string::npos || 
        trimmed.find('E') != std::string::npos) {
        try {
            size_t pos;
            double val = std::stod(trimmed, pos);
            if (pos == trimmed.size()) return val;
        } catch (...) {}
    } else {
        try {
            size_t pos;
            int64_t val = std::stoll(trimmed, pos);
            if (pos == trimmed.size()) return val;
        } catch (...) {}
    }
    
    // Handle booleans
    if (trimmed == "true" || trimmed == "false") {
        return trimmed == "true";
    }
    
    // Handle arrays (simplified)
    if (trimmed.front() == '[' && trimmed.back() == ']') {
        std
::string arrayContent = trimmed.substr(1, trimmed.size() - 2);
        std::vector<std::string> elements;
        
        size_t start = 0;
        size_t end = 0;
        while ((end = arrayContent.find(',', start)) != std::string::npos) {
            elements.push_back(trim(arrayContent.substr(start, end - start)));
            start = end + 1;
        }
        elements.push_back(trim(arrayContent.substr(start)));
        
        // Try to determine array element type
        bool allInts = true;
        bool allDoubles = true;
        bool allStrings = true;
        
        for (const auto& elem : elements) {
            if (!isNumericInt(elem)) allInts = false;
            if (!isNumericDouble(elem)) allDoubles = false;
            allStrings = false; // Strings always valid
        }
        
        if (allInts) {
            std::vector<int> intArray;
            for (const auto& elem : elements) {
                intArray.push_back(std::stoi(elem));
            }
            return intArray;
        } else if (allDoubles) {
            std::vector<double> doubleArray;
            for (const auto& elem : elements) {
                doubleArray.push_back(std::stod(elem));
            }
            return doubleArray;
        } else {
            return elements;
        }
    }
    
    // Default to string
    return trimmed;
}

bool Config::isNumericInt(const std::string& str) {
    if (str.empty()) return false;
    size_t pos;
    int64_t val;
    try {
        val = std::stoll(str, pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

bool Config::isNumericDouble(const std::string& str) {
    if (str.empty()) return false;
    size_t pos;
    double val;
    try {
        val = std::stod(str, pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}
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
