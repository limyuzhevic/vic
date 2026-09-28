#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, size_t> keyIndex;  // For faster lookup
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasError = false;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        try {
            // Parse simple key=value pairs with validation
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                NLM_LOG_ERROR("Invalid config format at line " + std::to_string(lineNumber) + ": missing '='");
                hasError = true;
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_ERROR("Invalid config format at line " + std::to_string(lineNumber) + ": empty key");
                hasError = true;
                continue;
            }
            
            // Validate value
            if (value.empty()) {
                NLM_LOG_WARNING("Config value for key '" + key + "' is empty at line " + std::to_string(lineNumber));
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error parsing config file " + filepath + " at line " + std::to_string(lineNumber) + ": " + e.what());
            hasError = true;
            continue;
        }
    }
    
    if (hasError) {
        NLM_LOG_WARNING("Config file " + filepath + " had parsing errors but was partially loaded");
    }
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath);
    return !hasError || !pImpl->entries.empty();  // Return true if at least one entry was loaded
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc < 1 || argv == nullptr) {
        return false;
    }
    
    bool hasError = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        try {
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    // Validate key
                    if (key.empty()) {
                        NLM_LOG_ERROR("Empty config key in argument: " + arg);
                        hasError = true;
                        continue;
                    }
                    
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_ERROR("Invalid command line argument format: " + arg);
                    hasError = true;
                }
            }
            // Handle -key value format
            else if (arg[0] == '-') {
                std::string key = arg.substr(1);
                
                if (key.empty()) {
                    NLM_LOG_ERROR("Empty config key in argument: " + arg);
                    hasError = true;
                    continue;
                }
                
                if (i + 1 < argc) {
                    std::string value = argv[++i];
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_ERROR("Missing value for config key: " + key);
                    hasError = true;
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error processing command line argument " + arg + ": " + e.what()));
            hasError = true;
        }
    }
    
    if (hasError) {
        NLM_LOG_ERROR("Command line arguments had parsing errors");
    }
    
    return !hasError;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated at runtime\n\n";
    
    size_t validEntries = 0;
    for (const auto& entry : pImpl->entries) {
        // Skip placeholder entries
        if (entry.value.index() == 0 && std::get<std::string>(entry.value).empty()) {
            continue;
        }
        
        try {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n\n";
            ++validEntries;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to write config entry " + entry.key + " to file: " + e.what());
        }
    }
    
    if (validEntries == 0) {
        NLM_LOG_WARNING("No valid configuration entries to save to file");
    }
    
    NLM_LOG_INFO("Saved " + std::to_string(validEntries) + " configuration entries to: " + filepath);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        return std::nullopt;
    }
    
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const auto& entry = pImpl->entries[index];
    
    try {
        return std::get<T>(entry.value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Type mismatch for config key '" + key + "': expected " + typeid(T).name());
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    }
    
    // Log when using default value
    if (!key.empty()) {
        NLM_LOG_DEBUG("Using default value for config key '" + key + "': " + std::to_string(defaultValue));
    }
    
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
    // Update or create entry
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        size_t index = it->second;
        if (index < pImpl->entries.size()) {
            pImpl->entries[index].value = value;
            pImpl->entries[index].source = source;
        }
    } else {
        pImpl->entries.emplace_back(key, value, source);
        pImpl->keyIndex[key] = pImpl->entries.size() - 1;
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
    return !key.empty() && pImpl->keyIndex.find(key) != pImpl->keyIndex.end();
}

void Config::remove(const std::string& key) {
    if (key.empty()) return;
    
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        size_t index = it->second;
        
        // Update key index for remaining entries
        for (auto& pair : pImpl->keyIndex) {
            if (pair.second > index) {
                pair.second--;
            }
        }
        
        pImpl->entries.erase(pImpl->entries.begin() + index);
        pImpl->keyIndex.erase(it);
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->keyIndex.size());
    for (const auto& pair : pImpl->keyIndex) {
        keys.push_back(pair.first);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->keyIndex.clear();
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
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
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
