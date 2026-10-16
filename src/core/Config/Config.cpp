#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json" || ext == "jsonl") {
        return loadFromJSONFile(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYAMLFile(filepath);
    } else {
        // Default to simple key=value format for .cfg, .config, .txt files
        return loadFromKeyValueFile(filepath);
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

bool Config::stringEndsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool Config::loadFromKeyValueFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            // Malformed line - skip with warning
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            // Missing key - skip with warning
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, ConfigSource::File);
    }
    
    return true;
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool inObject = false;
    bool inArray = false;
    std::string currentKey;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line.find("//") == 0 || line.find("#") == 0) {
            continue;
        }
        
        // Remove comments after code
        size_t commentPos = line.find("//");
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
            line = trim(line);
        }
        
        // Handle JSON objects and arrays
        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            
            if (c == '{') {
                inObject = true;
                continue;
            } else if (c == '}') {
                inObject = false;
                currentKey.clear();
                continue;
            } else if (c == '[') {
                inArray = true;
                continue;
            } else if (c == ']') {
                inArray = false;
                continue;
            }
            
            if (inObject && c == '"') {
                // Parse key
                size_t keyEnd = line.find('"', i + 1);
                if (keyEnd != std::string::npos) {
                    currentKey = line.substr(i + 1, keyEnd - i - 1);
                    i = keyEnd;
                    
                    // Find colon
                    size_t colonPos = line.find(':', i);
                    if (colonPos != std::string::npos) {
                        // Find value
                        size_t valueStart = colonPos + 1;
                        while (valueStart < line.size() && (line[valueStart] == ' ' || line[valueStart] == '\t')) {
                            valueStart++;
                        }
                        
                        std::string valueStr;
                        if (line[valueStart] == '"') {
                            // String value
                            size_t valueEnd = line.find('"', valueStart + 1);
                            if (valueEnd != std::string::npos) {
                                valueStr = line.substr(valueStart + 1, valueEnd - valueStart - 1);
                                set(currentKey, valueStr, ConfigSource::File);
                            }
                        } else if (line[valueStart] == '[') {
                            // Array - skip for now
                            // TODO: Implement array parsing
                            continue;
                        } else {
                            // Simple value
                            size_t valueEnd = valueStart;
                            while (valueEnd < line.size() && line[valueEnd] != ',' && line[valueEnd] != '}') {
                                valueEnd++;
                            }
                            valueStr = line.substr(valueStart, valueEnd - valueStart);
                            valueStr = trim(valueStr);
                            set(currentKey, valueStr, ConfigSource::File);
                        }
                    }
                }
            }
        }
    }
    
    return true;
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trimYAML(line);
        if (line.empty() || line.find("#") == 0 || line.find("%") == 0) {
            continue;
        }
        
        // Handle YAML key-value pairs
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trimYAML(line.substr(0, colonPos));
            std::string value = trimYAML(line.substr(colonPos + 1));
            
            if (key.empty()) {
                continue;
            }
            
            // Parse YAML value
            ConfigValue parsedValue = parseYAMLValue(value);
            set(key, parsedValue, ConfigSource::File);
        }
    }
    
    return true;
}

std::string Config::parseJSONString(const std::string& line, size_t& pos) {
    std::string result;
    bool inEscape = false;
    
    for (size_t i = pos; i < line.size(); ++i) {
        char c = line[i];
        
        if (inEscape) {
            result += c;
            inEscape = false;
        } else if (c == '\\') {
            inEscape = true;
        } else if (c == '"') {
            pos = i + 1;
            return result;
        } else {
            result += c;
        }
    }
    
    return result;
}

ConfigValue Config::parseJSONValue(const std::string& line, size_t& pos) {
    std::string valueStr = trim(line.substr(pos));
    pos = line.size();
    
    if (valueStr.empty() || valueStr == "null") {
        return std::string("null");
    } else if (valueStr == "true") {
        return true;
    } else if (valueStr == "false") {
        return false;
    } else if (valueStr.front() == '"' && valueStr.back() == '"') {
        return parseJSONString(line, pos);
    } else if (valueStr.find('.') != std::string::npos || 
               valueStr.find('e') != std::string::npos || 
               valueStr.find('E') != std::string::npos) {
        try {
            return std::stod(valueStr);
        } catch (...) {
            return valueStr;
        }
    } else {
        try {
            return std::stoll(valueStr);
        } catch (...) {
            return valueStr;
        }
    }
}

std::string Config::parseJSONKey(const std::string& line, size_t& pos) {
    std::string key;
    bool inEscape = false;
    
    for (size_t i = pos; i < line.size(); ++i) {
        char c = line[i];
        
        if (inEscape) {
            key += c;
            inEscape = false;
        } else if (c == '\\') {
            inEscape = true;
        } else if (c == '"') {
            pos = i + 1;
            return key;
        } else {
            key += c;
        }
    }
    
    return key;
}

std::string Config::trimYAML(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

ConfigValue Config::parseYAMLValue(const std::string& value) {
    std::string val = trimYAML(value);
    
    if (val.empty() || val == "null") {
        return std::string("null");
    } else if (val == "true") {
        return true;
    } else if (val == "false") {
        return false;
    } else if (val.front() == '"' && val.back() == '"') {
        return val.substr(1, val.size() - 2);
    } else if (val.front() == '\'' && val.back() == '\'') {
        return val.substr(1, val.size() - 2);
    } else if (val.find('.') != std::string::npos || 
               val.find('e') != std::string::npos || 
               val.find('E') != std::string::npos) {
        try {
            return std::stod(val);
        } catch (...) {
            return val;
        }
    } else {
        try {
            return std::stoll(val);
        } catch (...) {
            try {
                return std::stod(val);
            } catch (...) {
                return val;
            }
        }
    }
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