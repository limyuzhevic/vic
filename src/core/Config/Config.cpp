#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <numeric>
#include <cctype>

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
        
        // Helper function to write ConfigValue to file
        auto writeValue = [&file](const ConfigValue& val) {
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
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
            }, val);
        };
        
        writeValue(entry.value);
        file << "\n";
    }
    
    return true;
}

// Type conversion utilities
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

// Enhanced get function with automatic type detection
template<typename T>
std::optional<T> Config::getAuto(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    const std::string& strValue = std::get<std::string>(it->value);
    
    if constexpr (std::is_same_v<T, int>) {
        return stringToInt(strValue);
    } else if constexpr (std::is_same_v<T, double>) {
        return stringToDouble(strValue);
    } else if constexpr (std::is_same_v<T, bool>) {
        return stringToBool(strValue);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return stringToString(strValue);
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
        return stringToVectorInt(strValue);
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        return stringToVectorDouble(strValue);
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        return stringToVectorString(strValue);
    } else {
        return std::nullopt;
    }
}

// Helper function implementations for type conversion
inline std::optional<int> Config::stringToInt(const std::string& str) {
    try {
        size_t pos;
        int value = std::stoi(str, &pos);
        if (pos == str.size()) return value;
        return std::nullopt;
    } catch (...) {
        return std::nullopt;
    }
}

inline std::optional<double> Config::stringToDouble(const std::string& str) {
    try {
        size_t pos;
        double value = std::stod(str, &pos);
        if (pos == str.size()) return value;
        return std::nullopt;
    } catch (...) {
        return std::nullopt;
    }
}

inline std::optional<bool> Config::stringToBool(const std::string& str) {
    if (str == "true" || str == "1" || str == "yes") return true;
    if (str == "false" || str == "0" || str == "no") return false;
    return std::nullopt;
}

inline std::optional<std::string> Config::stringToString(const std::string& str) {
    // Remove surrounding quotes
    if (str.size() >= 2 && ((str.front() == '"' && str.back() == '"') ||
                           (str.front() == '\'' && str.back() == '\''))) {
        return str.substr(1, str.size() - 2);
    }
    return str;
}

inline std::optional<std::vector<int>> Config::stringToVectorInt(const std::string& str) {
    std::vector<int> result;
    size_t pos = 0;
    size_t start = 0;
    
    while (pos < str.size()) {
        if (str[pos] == ',' || str[pos] == ' ') {
            if (pos > start) {
                std::string token = str.substr(start, pos - start);
                auto val = stringToInt(token);
                if (val.has_value()) {
                    result.push_back(val.value());
                } else {
                    return std::nullopt;
                }
            }
            // Skip delimiters
            while (pos < str.size() && (str[pos] == ',' || str[pos] == ' ' || str[pos] == '[' || str[pos] == ']')) {
                ++pos;
            }
            start = pos;
        } else {
            ++pos;
        }
    }
    if (start < str.size()) {
        std::string token = str.substr(start);
        auto val = stringToInt(token);
        if (val.has_value()) {
            result.push_back(val.value());
        } else {
            return std::nullopt;
        }
    }
    return result;
}

inline std::optional<std::vector<double>> Config::stringToVectorDouble(const std::string& str) {
    std::vector<double> result;
    size_t pos = 0;
    size_t start = 0;
    
    while (pos < str.size()) {
        if (str[pos] == ',' || str[pos] == ' ') {
            if (pos > start) {
                std::string token = str.substr(start, pos - start);
                auto val = stringToDouble(token);
                if (val.has_value()) {
                    result.push_back(val.value());
                } else {
                    return std::nullopt;
                }
            }
            // Skip delimiters
            while (pos < str.size() && (str[pos] == ',' || str[pos] == ' ' || str[pos] == '[' || str[pos] == ']')) {
                ++pos;
            }
            start = pos;
        } else {
            ++pos;
        }
    }
    if (start < str.size()) {
        std::string token = str.substr(start);
        auto val = stringToDouble(token);
        if (val.has_value()) {
            result.push_back(val.value());
        } else {
            return std::nullopt;
        }
    }
    return result;
}

inline std::optional<std::vector<std::string>> Config::stringToVectorString(const std::string& str) {
    std::vector<std::string> result;
    size_t pos = 0;
    size_t start = 0;
    
    while (pos < str.size()) {
        if (str[pos] == ',') {
            if (pos > start) {
                std::string token = str.substr(start, pos - start);
                auto val = stringToString(token);
                if (val.has_value()) {
                    result.push_back(val.value());
                } else {
                    return std::nullopt;
                }
            }
            // Skip comma and spaces
            while (pos < str.size() && (str[pos] == ',' || str[pos] == ' ')) {
                ++pos;
            }
            start = pos;
        } else if (str[pos] == ' ') {
            // Skip spaces between elements
            while (pos < str.size() && str[pos] == ' ') {
                ++pos;
            }
            start = pos;
        } else {
            ++pos;
        }
    }
    if (start < str.size()) {
        std::string token = str.substr(start);
        auto val = stringToString(token);
        if (val.has_value()) {
            result.push_back(val.value());
        } else {
            return std::nullopt;
        }
    }
    return result;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
T Config::getOrAuto(const std::string& key, const T& defaultValue) const {
    auto val = getAuto<T>(key);
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
                    oss << "\"" << arg[i] << "\"";
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

// Explicit template instantiations
// Basic types
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// Vector types
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

// getOr templates
template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Vector getOr templates
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

// getAuto templates
template std::optional<int> Config::getAuto<int>(const std::string&) const;
template std::optional<double> Config::getAuto<double>(const std::string&) const;
template std::optional<bool> Config::getAuto<bool>(const std::string&) const;
template std::optional<std::string> Config::getAuto<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::getAuto<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::getAuto<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::getAuto<std::vector<std::string>>(const std::string&) const;

// getOrAuto templates
template int Config::getOrAuto<int>(const std::string&, const int&) const;
template double Config::getOrAuto<double>(const std::string&, const double&) const;
template bool Config::getOrAuto<bool>(const std::string&, const bool&) const;
template std::string Config::getOrAuto<std::string>(const std::string&, const std::string&) const;
template std::vector<int> Config::getOrAuto<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOrAuto<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOrAuto<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm