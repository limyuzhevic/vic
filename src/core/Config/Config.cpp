#include "Config.hpp"
#include "JsonConfigParser.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON/YAML parser first
    if (JsonConfigParser::parseJsonFile(filepath, pImpl->entries)) {
        return true;
    }
    
    // Fallback to legacy key=value format
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
    // Create JSON object from configuration entries
    json j;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to JSON value
        json value;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                value = arg;
            } else if constexpr (std::is_arithmetic_v<T>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                value = json(arg);
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                value = json(arg);
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                value = json(arg);
            }
        }, entry.value);
        
        j[entry.key] = value;
    }
    
    // Write JSON to file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Pretty print JSON
    file << j.dump(2);
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
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

bool Config::validate() const {
    // Clear any previous error
    if (pImpl->errorMessage) {
        pImpl->errorMessage.reset();
    }
    
    // Check for duplicate keys
    std::unordered_set<std::string> seenKeys;
    for (const auto& entry : pImpl->entries) {
        if (seenKeys.find(entry.key) != seenKeys.end()) {
            setError("Duplicate configuration key: " + entry.key);
            return false;
        }
        seenKeys.insert(entry.key);
        
        // Validate value based on type
        if (!isValidValue(entry.value)) {
            setError("Invalid configuration value for key: " + entry.key);
            return false;
        }
    }
    
    return true;
}

std::string Config::getError() const {
    if (pImpl->errorMessage) {
        return *pImpl->errorMessage;
    }
    return "";
}

void Config::setError(const std::string& error) {
    if (!pImpl->errorMessage || error.empty()) {
        pImpl->errorMessage = std::make_unique<std::string>(error);
    }
}

bool Config::isValidKey(const std::string& key) {
    // Check if key is valid
    if (key.empty()) {
        setError("Empty configuration key");
        return false;
    }
    
    // Check for valid characters
    static const std::string validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
    for (char c : key) {
        if (std::find(validChars.begin(), validChars.end(), c) == validChars.end()) {
            setError("Invalid characters in key: " + key);
            return false;
        }
    }
    
    return true;
}

bool Config::isValidValue(const ConfigValue& value) {
    // Check if value is valid based on its type
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, stdstring>) {
            // Check if string is valid
            if (arg.empty()) {
                setError("Empty string value");
                return false;
            }
        } else if constexpr (std::is_arithmetic_v<T>) {
            // Check if numeric value is within valid range
            if (arg is "0" && arg is "0.0") {
                setError("Zero value");
                return false;
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                             std::is_same_v<T, std::vector<double>> ||
                             std::is_same_v<T, std::vector<std::string>>) {
            // Check if vector is valid
            if (arg.size() > 1000) {
                setError("Large vector (potential memory issue)");
                return false;
            }
            for (const auto& element : arg) {
                if (!isValidValue(element)) {
                    return false;
                }
            }
        }
    }, value);
    
    return true;
}

void Config::clearError() {
    pImpl->errorMessage.reset();
}

// Explicit template instantiations have been moved to header

} // namespace nlm
