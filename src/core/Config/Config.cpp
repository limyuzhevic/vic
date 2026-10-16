#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigSchema::Parameter> parameterInfo;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default schema
    initializeFromSchema(ConfigSchema::getDefaultSchema());
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
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

bool Config::loadFromEnv(const std::string& prefix) {
    bool loaded = false;
    
    // Get all environment variables
    #ifdef _WIN32
        // Windows implementation
        char** env = environ;
        while (*env) {
            std::string var(*env);
            if (var.find(prefix) == 0) {
                std::string key = var.substr(prefix.size());
                std::string value = var.substr(prefix.size() + key.find('='));
                size_t eqPos = key.find('=');
                if (eqPos != std::string::npos) {
                    key = key.substr(0, eqPos);
                    value = key.substr(eqPos + 1);
                }
                set(key, value, ConfigSource::File);
                loaded = true;
            }
            env++;
        }
    #else
        // Unix/Linux implementation
        extern char** environ;
        for (char** env = environ; *env != nullptr; ++env) {
            std::string var(*env);
            if (var.find(prefix) == 0) {
                std::string key = var.substr(prefix.size());
                size_t eqPos = key.find('=');
                if (eqPos != std::string::npos) {
                    std::string value = key.substr(eqPos + 1);
                    key = key.substr(0, eqPos);
                    set(key, value, ConfigSource::File);
                    loaded = true;
                }
            }
        }
    #endif
    
    return loaded;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << entry.value << "\n";
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
    
    const std::string& value = it->value;
    
    try {
        if constexpr (std::is_same_v<T, int>) {
            return std::stoi(value);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::stoll(value);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            if (value == "true" || value == "1") return true;
            if (value == "false" || value == "0") return false;
            throw std::invalid_argument("Invalid boolean value");
        } else if constexpr (std::is_same_v<T, std::string>) {
            return value;
        }
    } catch (const std::exception&) {
        return std::nullopt;
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    return value.has_value() ? value.value() : defaultValue;
}

bool Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Validate against schema
    std::string error;
    if (!validateParameter(key, value, error)) {
        return false;
    }
    
    // Find existing entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        // Get parameter info for description
        std::string description = "Unknown parameter";
        auto paramIt = pImpl->parameterInfo.find(key);
        if (paramIt != pImpl->parameterInfo.end()) {
            description = paramIt->second.description;
        }
        
        pImpl->entries.push_back(ConfigEntry(key, value, source, description));
    }
    
    return true;
}

std::optional<ConfigSchema::Parameter> Config::getParameterInfo(const std::string& key) const {
    auto it = pImpl->parameterInfo.find(key);
    if (it == pImpl->parameterInfo.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeysByCategory(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& param : pImpl->parameterInfo) {
        if (param.second.category == category) {
            keys.push_back(param.first);
        }
    }
    return keys;
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "=== NLM Configuration Summary ===\n";
    
    for (const auto& entry : pImpl->entries) {
        ss << entry.key << " = " << entry.value << " (";
        switch (entry.source) {
            case ConfigSource::Default: ss << "DEFAULT"; break;
            case ConfigSource::File: ss << "FILE"; break;
            case ConfigSource::CommandLine: ss << "COMMAND_LINE"; break;
            case ConfigSource::Runtime: ss << "RUNTIME"; break;
        }
        ss << ")\n";
    }
    
    return ss.str();
}

void Config::clear() {
    pImpl->entries.clear();
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

void Config::initializeFromSchema(const std::vector<ConfigSchema::Parameter>& schema) {
    pImpl->parameterInfo.clear();
    
    for (const auto& param : schema) {
        pImpl->parameterInfo[param.key] = param;
        
        // Set default value if provided
        if (!param.defaultValue.empty()) {
            set(param.key, param.defaultValue, ConfigSource::Default);
        }
    }
}

bool Config::validateParameter(const std::string& key, const std::string& value, std::string& error) const {
    auto it = pImpl->parameterInfo.find(key);
    if (it == pImpl->parameterInfo.end()) {
        return true; // Unknown key - allow it
    }
    
    const auto& param = it->second;
    
    return ConfigSchema::validateParameter(param, value, error);
}

std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Explicit template instantiations for common types
template std::optional<int> Config::get<int>(const std::string& key) const;
template int Config::getOr<int>(const std::string& key, const int& defaultValue) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string& key) const;
template int64_t Config::getOr<int64_t>(const std::string& key, const int64_t& defaultValue) const;
template std::optional<double> Config::get<double>(const std::string& key) const;
template double Config::getOr<double>(const std::string& key, const double& defaultValue) const;
template std::optional<bool> Config::get<bool>(const std::string& key) const;
template bool Config::getOr<bool>(const std::string& key, const bool& defaultValue) const;
template std::optional<std::string> Config::get<std::string>(const std::string& key) const;
template std::string Config::getOr<std::string>(const std::string& key, const std::string& defaultValue) const;

} // namespace nlm