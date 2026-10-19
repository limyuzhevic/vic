#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Simple config file format: key = value
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            continue;
        }
        
        std::string key = trim(line.substr(0, equalPos));
        std::string valueStr = trim(line.substr(equalPos + 1));
        
        // Try to parse boolean
        if (valueStr == "true" || valueStr == "1") {
            set(key, true, ConfigSource::File);
        } else if (valueStr == "false" || valueStr == "0") {
            set(key, false, ConfigSource::File);
        } else {
            // Try to parse integer
            try {
                size_t pos;
                int intVal = std::stoi(valueStr, &pos);
                if (pos == valueStr.size()) {
                    set(key, intVal, ConfigSource::File);
                    continue;
                }
            } catch (...) {}
            
            // Try to parse float
            try {
                size_t pos;
                float floatVal = std::stof(valueStr, &pos);
                if (pos == valueStr.size()) {
                    set(key, floatVal, ConfigSource::File);
                    continue;
                }
            } catch (...) {}
            
            // Try to parse double
            try {
                size_t pos;
                double doubleVal = std::stod(valueStr, &pos);
                if (pos == valueStr.size()) {
                    set(key, doubleVal, ConfigSource::File);
                    continue;
                }
            } catch (...) {}
            
            // Treat as string (remove quotes if present)
            if (valueStr.size() >= 2 && valueStr.front() == '"' && valueStr.back() == '"') {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            set(key, valueStr, ConfigSource::File);
        }
    }
    
    file.close();
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 2) == "--") {
            size_t equalPos = arg.find('=');
            if (equalPos != std::string::npos) {
                std::string key = arg.substr(2, equalPos - 2);
                std::string value = arg.substr(equalPos + 1);
                // Use private set method to avoid recursion
                setPrivate(key, value, ConfigSource::CommandLine);
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
        
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t> ||
                                std::is_same_v<T, double> ||
                                std::is_same_v<T, bool>) {
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
                file << "UNKNOWN_TYPE";
            }
        }, entry.value);
        
        file << "  # Command: " << static_cast<int>(entry.source) << "\n";
    }
    
    file.close();
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            try {
                if (std::holds_alternative<T>(entry.value)) {
                    return std::get<T>(entry.value);
                }
            } catch (...) {}
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
    // Use private set method to avoid recursion
    setPrivate(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    setPrivate(key, value, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    setPrivate(key, value, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    setPrivate(key, value, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    setPrivate(key, value, source);
}

// Private method to avoid recursion
void Config::setPrivate(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Remove existing entry with same key
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& entry) { return entry.key == key; }),
        pImpl->entries.end()
    );
    
    pImpl->entries.emplace_back(key, value, source);
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
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& entry) { return entry.key == key; }),
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
                oss << "UNKNOWN_TYPE";
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

} // namespace nlm

