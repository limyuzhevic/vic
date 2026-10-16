#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
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
            
            // Try to parse as different types
            bool parsed = false;
            
            // Try as int
            try {
                size_t pos2;
                int int_val = std::stoi(value, &pos2);
                if (pos2 == value.size()) {
                    set(key, int_val);
                    parsed = true;
                }
            } catch (...) {}
            
            // Try as double
            if (!parsed) {
                try {
                    size_t pos2;
                    double double_val = std::stod(value, &pos2);
                    if (pos2 == value.size()) {
                        set(key, double_val);
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try as bool
            if (!parsed) {
                if (value == "true") {
                    set(key, true);
                    parsed = true;
                } else if (value == "false") {
                    set(key, false);
                    parsed = true;
                }
            }
            
            // Try as string
            if (!parsed) {
                set(key, value);
                parsed = true;
            }
            
            if (!parsed) {
                // Couldn't parse, treat as string
                set(key, value);
            }
        }
    }
    
    file.close();
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    clear();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        size_t pos = arg.find('=');
        if (pos != std::string::npos && pos > 0) {
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            
            // Remove -- prefix if present
            if (key.size() >= 2 && key.substr(0, 2) == "--") {
                key = key.substr(2);
            }
            
            // Try to parse as different types (same as above)
            bool parsed = false;
            
            try {
                size_t pos2;
                int int_val = std::stoi(value, &pos2);
                if (pos2 == value.size()) {
                    set(key, int_val);
                    parsed = true;
                }
            } catch (...) {}
            
            if (!parsed) {
                try {
                    size_t pos2;
                    double double_val = std::stod(value, &pos2);
                    if (pos2 == value.size()) {
                        set(key, double_val);
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            if (!parsed) {
                if (value == "true") {
                    set(key, true);
                    parsed = true;
                } else if (value == "false") {
                    set(key, false);
                    parsed = true;
                }
            }
            
            if (!parsed) {
                set(key, value);
            }
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
        file << entry.key << " = ";
        
        std::visit([&file](const auto& arg) {
            if constexpr (std::is_same_v<decltype(arg), std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<decltype(arg), int> ||
                                 std::is_same_v<decltype(arg), int64_t> ||
                                 std::is_same_v<decltype(arg), double> ||
                                 std::is_same_v<decltype(arg), bool>) {
                file << arg;
            } else if constexpr (std::is_same_v<decltype(arg), std::vector<int>>) {
                file << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) file << ", ";
                    file << item;
                    first = false;
                }
                file << "]";
            } else if constexpr (std::is_same_v<decltype(arg), std::vector<double>>) {
                file << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) file << ", ";
                    file << std::setprecision(6) << item;
                    first = false;
                }
                file << "]";
            } else if constexpr (std::is_same_v<decltype(arg), std::vector<std::string>>) {
                file << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) file << ", ";
                    file << "\"" << item << "\"";
                    first = false;
                }
                file << "]";
            }
        }, entry.value);
        
        file << "\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Search for the key
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key && std::holds_alternative<T>(entry.value)) {
            return std::get<T>(entry.value);
        }
    }
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    return value ? *value : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Check if key already exists
    for (auto& entry : pImpl->entries) {
        if (entry.key == key) {
            entry.value = value;
            entry.source = source;
            return;
        }
    }
    
    // Add new entry
    pImpl->entries.push_back(ConfigEntry(key, value, source));
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, static_cast<ConfigValue>(value), source);
}

bool Config::has(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            return true;
        }
    }
    return false;
}

void Config::remove(const std::string& key) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                            [&key](const ConfigEntry& entry) { return entry.key == key; });
    pImpl->entries.erase(it, pImpl->entries.end());
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

// Helper function to format ConfigValue as string
std::string formatConfigValue(const ConfigValue& value) {
    std::ostringstream oss;
    
    std::visit([&oss](const auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, int> ||
                             std::is_same_v<T, int64_t> ||
                             std::is_same_v<T, double> ||
                             std::is_same_v<T, bool>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                oss << item;
                first = false;
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                oss << std::setprecision(6) << item;
                first = false;
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                oss << "\"" << item << "\"";
                first = false;
            }
            oss << "]";
        }
    }, value);
    
    return oss.str();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = " << formatConfigValue(entry.value) << " (" << static_cast<int>(entry.source) << ")\n";
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

} // namespace nlm