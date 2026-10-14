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
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Validate file exists and is readable
        if (!std::filesystem::exists(filepath)) {
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            ++lineNumber;
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Validate line format
            if (line.find('=') == std::string::npos) {
                // Invalid line format
                continue;
            }
            
            // Parse simple key=value pairs
            size_t pos = line.find('=');
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key
            if (key.empty() || key.find(' ') != std::string::npos) {
                continue; // Skip invalid keys
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == \'\' && value.back() == \'\'))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate value
            if (value.empty()) {
                continue; // Skip empty values
            }
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error
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
        T value = std::get<T>(it->value);
        
        // Validate the retrieved value
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            if (value < std::numeric_limits<T>::min() || value > std::numeric_limits<T>::max()) {
                return std::nullopt; // Invalid integer value
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::isnan(value) || std::isinf(value)) {
                return std::nullopt; // Invalid double value
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (value.empty() || value.find('\0') != std::string::npos) {
                return std::nullopt; // Invalid string value
            }
        }
        
        return value;
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
        // Validate key
        if (key.empty() || key.find(' ') != std::string::npos || key.find('=') != std::string::npos) {
            return; // Skip invalid keys
        }
        
        // Validate value
        if (!validateValue(value)) {
            return; // Skip invalid values
        }
        
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
    // Validate key
    if (key.empty() || key.find(' ') != std::string::npos || key.find('=') != std::string::npos) {
        return; // Skip invalid keys
    }
    
    // Validate value
    if (value.empty() || value.find('\0') != std::string::npos) {
        return; // Skip invalid values
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Validate key
    if (key.empty() || key.find(' ') != std::string::npos || key.find('=') != std::string::npos) {
        return; // Skip invalid keys
    }
    
    // Validate integer value
    if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max()) {
        return; // Skip invalid values
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Validate key
    if (key.empty() || key.find(' ') != std::string::npos || key.find('=') != std::string::npos) {
        return; // Skip invalid keys
    }
    
    // Validate double value
    if (std::isnan(value) || std::isinf(value)) {
        return; // Skip invalid values
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    // Validate key
    if (key.empty() || key.find(' ') != std::string::npos || key.find('=') != std::string::npos) {
        return; // Skip invalid keys
    }
    
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

    // Internal validation helpers
    bool validateKey(const std::string& key) {
        return !key.empty() && key.find(' ') == std::string::npos && key.find('=') == std::string::npos;
    }
    
    bool validateValue(const ConfigValue& value) {
        // Check if value is valid variant
        return std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                // Validate integer range
                return arg >= std::numeric_limits<int>::min() && arg <= std::numeric_limits<int>::max();
            } else if constexpr (std::is_same_v<T, int64_t>) {
                // Validate int64_t range
                return true; // int64_t has large range, assume valid
            } else if constexpr (std::is_same_v<T, double>) {
                // Validate double range
                return !std::isnan(arg) && !std::isinf(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                // bool is always valid
                return true;
            } else if constexpr (std::is_same_v<T, std::string>) {
                // Validate string content
                return !arg.empty() && arg.find('\0') == std::string::npos;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                // Validate integer vector
                for (const auto& item : arg) {
                    if (item < std::numeric_limits<int>::min() || item > std::numeric_limits<int>::max()) {
                        return false;
                    }
                }
                return true;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                // Validate double vector
                for (const auto& item : arg) {
                    if (std::isnan(item) || std::isinf(item)) {
                        return false;
                    }
                }
                return true;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                // Validate string vector
                for (const auto& item : arg) {
                    if (item.empty() || item.find('\0') != std::string::npos) {
                        return false;
                    }
                }
                return true;
            } else {
                return true; // Unknown type, assume valid
            }
        }, value);
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
