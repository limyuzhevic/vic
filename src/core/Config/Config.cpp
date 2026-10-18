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

// Helper functions for type conversion
namespace {
    bool tryParseBool(const std::string& str, bool& result) {
        std::string lower = toLower(str);
        if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
            result = true;
            return true;
        } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
            result = false;
            return true;
        }
        return false;
    }
    
    bool tryParseInt(const std::string& str, int64_t& result) {
        try {
            size_t pos;
            result = std::stoll(str, &pos);
            return pos == str.size();
        } catch (...) {
            return false;
        }
    }
    
    bool tryParseDouble(const std::string& str, double& result) {
        try {
            size_t pos;
            result = std::stod(str, &pos);
            return pos == str.size();
        } catch (...) {
            return false;
        }
    }
    
    std::vector<int> tryParseIntList(const std::string& str) {
        std::vector<int> result;
        if (str.empty() || str[0] != '[' || str.back() != ']') {
            return result;
        }
        
        std::string content = str.substr(1, str.size() - 2);
        std::stringstream ss(content);
        std::string item;
        
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            if (!item.empty()) {
                try {
                    result.push_back(std::stoi(item));
                } catch (...) {
                    // Skip invalid numbers
                }
            }
        }
        return result;
    }
    
    std::vector<double> tryParseDoubleList(const std::string& str) {
        std::vector<double> result;
        if (str.empty() || str[0] != '[' || str.back() != ']') {
            return result;
        }
        
        std::string content = str.substr(1, str.size() - 2);
        std::stringstream ss(content);
        std::string item;
        
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            if (!item.empty()) {
                try {
                    result.push_back(std::stod(item));
                } catch (...) {
                    // Skip invalid numbers
                }
            }
        }
        return result;
    }
    
    std::vector<std::string> tryParseStringList(const std::string& str) {
        std::vector<std::string> result;
        if (str.empty() || str[0] != '[' || str.back() != ']') {
            return result;
        }
        
        std::string content = str.substr(1, str.size() - 2);
        std::stringstream ss(content);
        std::string item;
        
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            if (!item.empty()) {
                // Remove quotes
                if (item.size() >= 2 && ((item.front() == '"' && item.back() == '"') ||
                                          (item.front() == '\'' && item.back() == '\''))) {
                    item = item.substr(1, item.size() - 2);
                }
                result.push_back(item);
            }
        }
        return result;
    }
} // namespace

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
        
        // Convert ConfigValue back to string representation
        std::visit([&file, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            file << entry.key << " = ";
            
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
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

// Enhanced set method with automatic type conversion from string
void Config::set(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Auto-detect type from string value
    bool boolVal;
    int64_t intVal;
    double doubleVal;
    std::vector<int> intListVal;
    std::vector<double> doubleListVal;
    std::vector<std::string> stringListVal;
    
    ConfigValue convertedValue;
    
    // Try different types in order of preference
    if (tryParseBool(valueStr, boolVal)) {
        convertedValue = boolVal;
    } else if (tryParseInt(valueStr, intVal)) {
        convertedValue = intVal;
    } else if (tryParseDouble(valueStr, doubleVal)) {
        convertedValue = doubleVal;
    } else if ((intListVal = tryParseIntList(valueStr)).size() > 0) {
        convertedValue = intListVal;
    } else if ((doubleListVal = tryParseDoubleList(valueStr)).size() > 0) {
        convertedValue = doubleListVal;
    } else {
        // Default to string
        convertedValue = valueStr;
    }
    
    set(key, convertedValue, source);
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
