#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string lastError;
    size_t maxEntrySize;
    bool validateTypes;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->maxEntrySize = 1024;
    pImpl->validateTypes = true;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Failed to open file: " + filepath;
        return false;
    }
    
    // Clear existing entries before loading
    clear();
    
    try {
        // Read entire file content
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        if (content.empty()) {
            pImpl->lastError = "Configuration file is empty: " + filepath;
            return false;
        }
        
        // Try to parse as JSON first
        try {
            nlohmann::json jsonConfig = nlohmann::json::parse(content);
            
            if (!jsonConfig.is_object()) {
                pImpl->lastError = "JSON root must be an object (key-value pairs)";
                return false;
            }
            
            // Load entries from JSON
            for (auto& member : jsonConfig.items()) {
                std::string key = member.key();
                
                // Validate key
                if (key.empty()) {
                    pImpl->lastError = "Empty key found in JSON configuration";
                    return false;
                }
                
                if (key.size() > pImpl->maxEntrySize) {
                    pImpl->lastError = "Key too long: " + key;
                    return false;
                }
                
                ConfigValue value;
                const nlohmann::json& val = member.value();
                
                // Convert JSON value to ConfigValue
                if (val.is_string()) {
                    value = val.get<std::string>();
                } else if (val.is_number_integer()) {
                    value = val.get<int64_t>();
                } else if (val.is_number_unsigned()) {
                    value = static_cast<int64_t>(val.get<uint64_t>());
                } else if (val.is_number_float()) {
                    value = val.get<double>();
                } else if (val.is_boolean()) {
                    value = val.get<bool>();
                } else if (val.is_array()) {
                    // Handle array values
                    std::vector<double> doubleArray;
                    for (const auto& item : val) {
                        if (item.is_number()) {
                            doubleArray.push_back(item.get<double>());
                        } else {
                            pImpl->lastError = "Array contains non-numeric value at key: " + key;
                            return false;
                        }
                    }
                    value = doubleArray;
                } else {
                    pImpl->lastError = "Unsupported JSON type for key: " + key;
                    return false;
                }
                
                set(key, value, ConfigSource::File);
            }
            
            return true;
        } catch (const nlohmann::json::parse_error& e) {
            // If JSON parsing fails, fall back to simple key=value format
            pImpl->lastError = ""; // Clear JSON error, fall back to simple format
        }
        
        // Fallback to simple key=value format
        std::vector<std::string> lines;
        std::stringstream lineStream(content);
        std::string line;
        while (std::getline(lineStream, line)) {
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos && pos > 0) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Validate key and value
                if (key.empty()) {
                    pImpl->lastError = "Empty key found in configuration file";
                    return false;
                }
                
                if (key.size() > pImpl->maxEntrySize) {
                    pImpl->lastError = "Key too long: " + key;
                    return false;
                }
                
                if (value.size() > pImpl->maxEntrySize) {
                    pImpl->lastError = "Value too long for key: " + key;
                    return false;
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Exception during configuration loading: " + std::string(e.what());
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Validate arguments
    if (!argv || argc <= 1) {
        return true; // No arguments provided, use defaults
    }
    
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Validate argument format
        if (arg.empty()) {
            continue;
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos && pos > 2) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key and value
                if (key.empty()) {
                    pImpl->lastError = "Empty key in command line argument: " + arg;
                    success = false;
                    continue;
                }
                
                if (value.size() > pImpl->maxEntrySize) {
                    pImpl->lastError = "Value too long for key: " + key;
                    success = false;
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                pImpl->lastError = "Invalid command line argument format: " + arg;
                success = false;
            }
        } 
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc && argv[i + 1][0] != '-') {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key and value
            if (key.empty()) {
                pImpl->lastError = "Empty key in command line argument: " + arg;
                success = false;
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
        } else if (arg[0] == '-') {
            pImpl->lastError = "Missing value for command line argument: " + arg;
            success = false;
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Failed to open file for writing: " + filepath;
        return false;
    }
    
    // Write as formatted JSON for better readability
    nlohmann::json jsonConfig;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to nlohmann::json
        std::visit([&jsonConfig](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                jsonConfig[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                jsonConfig[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                jsonConfig[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonConfig[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonConfig[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonConfig[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonConfig[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonConfig[entry.key] = arg;
            }
        }, entry.value);
    }
    
    // Write JSON to file with pretty formatting
    try {
        file << jsonConfig.dump(2); // Indent with 2 spaces
        return !file.fail();
    } catch (const std::exception& e) {
        pImpl->lastError = "Exception while writing JSON to file: " + std::string(e.what());
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
        pImpl->lastError = "Type mismatch for key: " + key;
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
    if (key.empty()) {
        pImpl->lastError = "Empty key not allowed";
        return;
    }
    
    if (key.size() > pImpl->maxEntrySize) {
        pImpl->lastError = "Key too long: " + key;
        return;
    }
    
    // Validate value size for strings
    if (std::holds_alternative<std::string>(value) && 
        std::get<std::string>(value).size() > pImpl->maxEntrySize) {
        pImpl->lastError = "Value too long for key: " + key;
        return;
    }
    
    if (pImpl->validateTypes) {
        // Additional validation can be added here
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
    pImpl->lastError.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    if (pImpl->lastError.empty() && !pImpl->entries.empty()) {
        oss << "✓ Configuration is valid\n";
    } else if (!pImpl->lastError.empty()) {
        oss << "⚠ Configuration issues:\n" << "  " << pImpl->lastError << "\n";
    }
    
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