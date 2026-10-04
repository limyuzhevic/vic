#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <limits>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Load configuration from file with enhanced parser support
    // Supports both simple key=value format and JSON format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to detect file format - if it starts with { or [, assume JSON
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    // Simple heuristic to detect JSON format
    bool isJsonFormat = false;
    if (!firstLine.empty() && (firstLine[0] == '{' || firstLine[0] == '[')) {
        isJsonFormat = true;
    }
    
    if (isJsonFormat) {
        // JSON format parsing
        return loadFromJsonFile(filepath);
    } else {
        // Legacy simple key=value format
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    // Load configuration from JSON file
    // For now, basic implementation - could be enhanced with proper JSON library
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, implement simple JSON parsing
    // In a full implementation, we would use a proper JSON library like nlohmann::json
    // For this fix, we'll implement a basic parser that can handle simple key-value pairs in JSON format
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Simple JSON parsing - remove braces and parse key: value pairs
    // This is a basic implementation that could be enhanced
    size_t pos = 0;
    while (pos < content.size()) {
        // Find key
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) break;
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Find value
        size_t colonPos = content.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        size_t valueStart = content.find_first_not_of(" \t\n\r:\", colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        size_t valueEnd;
        if (content[valueStart] == '"') {
            // String value
            valueEnd = content.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;
        } else if (content[valueStart] == '[') {
            // Array value - simplified parsing
            valueEnd = content.find(']', valueStart + 1);
            if (valueEnd == std::string::npos) break;
        } else if (content[valueStart] == '{') {
            // Object - skip for now
            valueEnd = content.find('}', valueStart + 1);
            if (valueEnd == std::string::npos) break;
        } else {
            // Number or boolean - parse until comma or }
            valueEnd = content.find_first_of(",}\", valueStart);
            if (valueEnd == std::string::npos) break;
        }
        
        std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
        
        // Parse the value
        parseValueFromString(key, valueStr, ConfigSource::File);
        
        // Move past this entry
        pos = valueEnd;
        // Skip to next comma or closing brace
        while (pos < content.size() && content[pos] != ',' && content[pos] != '}') {
            pos++;
        }
        if (pos < content.size() && content[pos] == ',') pos++;
    }
    
    return true;
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == \'#\' || line[0] == \'/\') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find(\'=\');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == \'"\' && value.back() == \'"\') ||
                 (value.front() == \'\'\' && value.back() == \'\'\'))) {
                value = value.substr(1, value.size() - 2);
            }
            
            parseValueFromString(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

void Config::parseValueFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Parse string representation of value and set it
    if (valueStr == "true" || valueStr == "false") {
        bool boolVal = (valueStr == "true");
        set(key, boolVal, source);
    } else {
        // Try to parse as number
        try {
            // Try double first (includes int)
            size_t pos;
            double doubleVal = std::stod(valueStr, &pos);
            if (pos == valueStr.size()) {
                // Check if it would fit in int
                size_t pos2;
                long long intVal = std::stoll(valueStr, &pos2);
                if (pos2 == valueStr.size() && intVal >= std::numeric_limits<int>::min() && 
                    intVal <= std::numeric_limits<int>::max()) {
                    set(key, static_cast<int>(intVal), source);
                } else {
                    set(key, doubleVal, source);
                }
                return;
            }
        } catch (...) {
            // Not a number, treat as string
            set(key, valueStr, source);
        }
    }
}

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Load configuration from file with enhanced parser support
    // Supports both simple key=value format and JSON format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to detect file format - if it starts with { or [, assume JSON
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    // Simple heuristic to detect JSON format
    bool isJsonFormat = false;
    if (!firstLine.empty() && (firstLine[0] == '{' || firstLine[0] == '[')) {
        isJsonFormat = true;
    }
    
    if (isJsonFormat) {
        // JSON format parsing
        return loadFromJsonFile(filepath);
    } else {
        // Legacy simple key=value format
        return loadFromSimpleFile(filepath);
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
        file << entry.key << " = ";
        
        // Serialize the ConfigValue based on its type
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, double>) {
                file << value;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << value[i] << "\"";
                }
                file << "]";
            } else {
                file << "unknown_type";
            }
        }, entry.value);
        
        file << "\n";
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
