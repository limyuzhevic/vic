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
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file: " + filepath);
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
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Validate key
                if (key.empty()) {
                    NLM_LOG_WARNING("Empty key found in config file: " + filepath + " at line " + std::to_string(lineNumber));
                    continue;
                }
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Invalid config line (no '='): " + line + " in file " + filepath + " at line " + std::to_string(lineNumber));
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 0 || argv == nullptr) {
        NLM_LOG_ERROR("Invalid arguments for config loading");
        return false;
    }
    
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    if (key.empty()) {
                        NLM_LOG_WARNING("Empty key in command line argument: " + arg);
                        continue;
                    }
                    
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_WARNING("Invalid command line argument format (no '='): " + arg);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                if (key.empty()) {
                    NLM_LOG_WARNING("Empty key in command line argument: " + arg);
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Skipping unrecognized command line argument: " + arg);
            }
        }
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading config from command line: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
            return false;
        }
        
        // Set stream to write mode and check for write errors
        file << std::fixed;
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Properly serialize different value types
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "["; // Would need to implement proper vector serialization
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "["; // Would need to implement proper vector serialization
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "["; // Would need to implement proper vector serialization
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception saving config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_ERROR("Empty key provided to Config::get()");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Config key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Config value type mismatch for key: " + key + ". Expected type " + typeid(T).name() + ".");
        return std::nullopt;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception accessing config key " + key + ": " + std::string(e.what()));
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    if (key.empty()) {
        NLM_LOG_ERROR("Empty key provided to Config::getOr()");
        return defaultValue;
    }
    
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        NLM_LOG_DEBUG("Using default value for config key: " + key);
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config with empty key");
        return;
    }
    
    try {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception setting config key " + key + ": " + std::string(e.what()));
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
    if (key.empty()) {
        NLM_LOG_ERROR("Empty key provided to Config::has()");
        return false;
    }
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}
void Config::remove(const std::string& key) {
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot remove config with empty key");
        return;
    }
    
    try {
        size_t removedCount = pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(),
                          pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; }),
            pImpl->entries.end()
        );
        
        if (removedCount > 0) {
            NLM_LOG_DEBUG("Removed " + std::to_string(removedCount) + " config entry(s) with key: " + key);
        } else {
            NLM_LOG_DEBUG("No config entries found with key to remove: " + key);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception removing config key " + key + ": " + std::string(e.what()));
    }
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
    try {
        std::ostringstream oss;
        oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
        
        for (const auto& entry : pImpl->entries) {
            oss << "  " << entry.key << " = [";
            
            try {
                std::visit([&oss](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        oss << "\"" << arg << "\"";
                    } else {
                        oss << arg;
                    }
                }, entry.value);
                
                oss << "] (" << static_cast<int>(entry.source) << ")\n";
            } catch (const std::exception& e) {
                oss << "ERROR: " << e.what() << "] (" << static_cast<int>(entry.source) << ")\n";
            }
        }
        
        return oss.str();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception generating config summary: " + std::string(e.what()));
        return "Configuration summary generation failed.";
    }
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
