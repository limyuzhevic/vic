#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string lastError;
    bool validateKeys;
};

// Helper: Validate key name
bool Config::validateKey(const std::string& key) {
    if (key.empty()) {
        pImpl->lastError = "Empty key name not allowed";
        return false;
    }
    
    // Check for invalid characters
    if (key.find(' ') != std::string::npos) {
        pImpl->lastError = "Key name cannot contain spaces: " + key;
        return false;
    }
    
    if (key.find('=') != std::string::npos) {
        pImpl->lastError = "Key name cannot contain '=': " + key;
        return false;
    }
    
    if (key.find('\n') != std::string::npos || key.find('\r') != std::string::npos) {
        pImpl->lastError = "Key name cannot contain newlines: " + key;
        return false;
    }
    
    return true;
}

// Helper: Validate value format
bool Config::validateValue(const std::string& value) {
    if (value.empty()) {
        return true; // Empty string is valid
    }
    
    // Check for quotes and ensure they're balanced
    if (value.front() == '"' && value.back() == '"') {
        // Extract content between quotes
        std::string content = value.substr(1, value.size() - 2);
        if (content.find('"') != std::string::npos) {
            pImpl->lastError = "Unmatched quotes in value: " + value;
            return false;
        }
    } else if (value.front() == '\'' && value.back() == '\'') {
        std::string content = value.substr(1, value.size() - 2);
        if (content.find('\'') != std::string::npos) {
            pImpl->lastError = "Unmatched quotes in value: " + value;
            return false;
        }
    }
    
    return true;
}

// Helper: Convert string to appropriate type
ConfigValue Config::parseValue(const std::string& value) {
    // Try boolean first
    if (value == "true" || value == "false") {
        return value == "true";
    }
    
    // Try integer
    try {
        size_t pos;
        long long intVal = std::stoll(value, &pos);
        if (pos == value.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try unsigned integer
    try {
        size_t pos;
        unsigned long long uintVal = std::stoull(value, &pos);
        if (pos == value.size()) {
            return static_cast<uint64_t>(uintVal);
        }
    } catch (...) {}
    
    // Try float/double
    try {
        size_t pos;
        double floatVal = std::stod(value, &pos);
        if (pos == value.size()) {
            // Check if it's actually an integer
            if (std::floor(floatVal) == floatVal) {
                return static_cast<int64_t>(floatVal);
            }
            return floatVal;
        }
    } catch (...) {}
    
    // Return as string if all else fails
    return value;
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->validateKeys = true;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

std::string Config::getLastError() const {
    return pImpl->lastError;
}

void Config::clearError() const {
    pImpl->lastError.clear();
}

bool Config::loadFromFile(const std::string& filepath) {
    // Clear previous error
    pImpl->lastError.clear();
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        pImpl->lastError = "Configuration file not found: " + filepath;
        return false;
    }
    
    // Check if it's a file (not a directory)
    if (!std::filesystem::is_regular_file(filepath)) {
        pImpl->lastError = "Path is not a file: " + filepath;
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open configuration file: " + filepath;
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            pImpl->lastError = "Invalid line " + std::to_string(lineNum) + " in config file: missing '='";
            return false;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key and value
        if (pImpl->validateKeys && !validateKey(key)) {
            pImpl->lastError = "Invalid key in config file line " + std::to_string(lineNum) + ": " + getLastError();
            return false;
        }
        
        if (!validateValue(value)) {
            pImpl->lastError = "Invalid value in config file line " + std::to_string(lineNum) + ": " + getLastError();
            return false;
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

bool Config::loadFromArgs(int argc, char** argv) {
    pImpl->lastError.clear();
    
    if (!argv || argc <= 1) {
        return true; // Nothing to do
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (!validateKey(key)) {
                    pImpl->lastError = "Invalid key in command line: " + getLastError();
                    return false;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                pImpl->lastError = "Invalid command line argument format: " + arg;
                return false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            if (i + 1 >= argc) {
                pImpl->lastError = "Missing value for key: " + arg;
                return false;
            }
            
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (!validateKey(key)) {
                pImpl->lastError = "Invalid key in command line: " + getLastError();
                return false;
            }
            
            set(key, value, ConfigSource::CommandLine);
        } else {
            pImpl->lastError = "Invalid command line argument: " + arg;
            return false;
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (!validateKey(filepath) && !filepath.empty()) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot save configuration to file: " + filepath;
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Use appropriate formatting based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_arithmetic_v<T>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        file << "\"" << arg[i] << "\"";
                    } else {
                        file << arg[i];
                    }
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "\n";
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    pImpl->lastError.clear();
    
    if (!validateKey(key)) {
        return; // Error already set
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
    if (!validateKey(key)) {
        return; // Error already set
    }
    
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
    pImpl->lastError.clear();
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
    
    if (!pImpl->lastError.empty()) {
        oss << "ERROR: " << pImpl->lastError << "\n";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (pImpl->validateKeys && !validateKey(key)) {
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
