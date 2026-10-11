#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    mutable ConfigError lastError = ConfigError::Success;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

ConfigError Config::getLastError() const {
    return pImpl->lastError;
}

const char* Config::getErrorString(ConfigError error) {
    switch (error) {
        case ConfigError::Success:
            return "Success";
        case ConfigError::FileNotFound:
            return "File not found";
        case ConfigError::FileCannotOpen:
            return "Cannot open file for writing";
        case ConfigError::FileParseError:
            return "File parsing error";
        case ConfigError::InvalidFormat:
            return "Invalid format";
        case ConfigError::KeyNotFound:
            return "Key not found";
        case ConfigError::TypeMismatch:
            return "Type mismatch";
        case ConfigError::ConversionError:
            return "Conversion error";
        case ConfigError::MemoryError:
            return "Memory error";
        case ConfigError::ValidationError:
            return "Validation error";
        default:
            return "Unknown error";
    }
}

bool Config::validate() const {
    pImpl->validationErrors.clear();
    
    for (const auto& entry : pImpl->entries) {
        // Validate key is not empty
        if (entry.key.empty()) {
            pImpl->validationErrors.push_back("Empty key found");
        }
        
        // Validate required keys exist
        if (entry.source == ConfigSource::File && entry.key == "neuron_count" && 
            get<int>("neuron_count").has_value() && 
            get<int>("neuron_count").value() <= 0) {
            pImpl->validationErrors.push_back("Invalid neuron_count: must be positive");
        }
        
        if (entry.source == ConfigSource::File && entry.key == "simulation_timestep" &&
            get<double>("simulation_timestep").has_value() &&
            get<double>("simulation_timestep").value() <= 0.0) {
            pImpl->validationErrors.push_back("Invalid simulation_timestep: must be positive");
        }
    }
    
    pImpl->lastError = pImpl->validationErrors.empty() ? ConfigError::Success : ConfigError::ValidationError;
    return pImpl->validationErrors.empty();
}

ConfigError Config::saveToFileWithError(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = ConfigError::FileCannotOpen;
        return ConfigError::FileCannotOpen;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Serialize the key=value pair with actual value
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[\"";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << "\", \";
                }
                file << "\"]";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    pImpl->lastError = ConfigError::Success;
    return ConfigError::Success;
}
    std::ofstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = ConfigError::FileCannotOpen;
        return ConfigError::FileCannotOpen;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Serialize the key=value pair with actual value
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[\"";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << "\", \";
                }
                file << "\"]";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    pImpl->lastError = ConfigError::Success;
    return ConfigError::Success;
}

template<typename T>
std::pair<std::optional<T>, ConfigError> Config::getWithError(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        pImpl->lastError = ConfigError::KeyNotFound;
        return {std::nullopt, ConfigError::KeyNotFound};
    }
    
    try {
        T value = std::get<T>(it->value);
        pImpl->lastError = ConfigError::Success;
        return {value, ConfigError::Success};
    } catch (const std::bad_variant_access&) {
        pImpl->lastError = ConfigError::TypeMismatch;
        return {std::nullopt, ConfigError::TypeMismatch};
    }
}

ConfigError Config::setWithValidation(const std::string& key, const ConfigValue& value, 
                                      ConfigSource source) {
    // Validate key
    if (key.empty()) {
        pImpl->lastError = ConfigError::InvalidFormat;
        return ConfigError::InvalidFormat;
    }
    
    // Remove existing entry if present
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    pImpl->lastError = ConfigError::Success;
    return ConfigError::Success;
}

ConfigError Config::loadFromFile(const std::string& filepath, bool validate) {
    if (filepath.empty()) {
        pImpl->lastError = ConfigError::InvalidFormat;
        return ConfigError::InvalidFormat;
    }
    
    // Check file extension for format detection
    std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
    bool isJson = (ext == "json" || ext == "JSON");
    bool isYaml = (ext == "yaml" || ext == "yml" || ext == "YAML");
    
    // For now, continue with simple key=value format
    // TODO PHASE 2: Implement proper JSON/YAML parser
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = ConfigError::FileNotFound;
        return ConfigError::FileNotFound;
    }
    
    clear();
    std::string line;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            pImpl->lastError = ConfigError::InvalidFormat;
            return ConfigError::InvalidFormat;
        }
        
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
    
    if (validate && !this->validate()) {
        pImpl->lastError = ConfigError::ValidationError;
        return ConfigError::ValidationError;
    }
    
    pImpl->lastError = ConfigError::Success;
    return ConfigError::Success;
}

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFileWithError(filepath, true) == ConfigError::Success;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto result = getWithError<T>(key);
    return result.first;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto result = getWithError<T>(key);
    if (result.first.has_value()) {
        return result.first.value();
    }
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    setWithValidation(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    setWithValidation(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    setWithValidation(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    setWithValidation(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    setWithValidation(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it);
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
    pImpl->validationErrors.clear();
    pImpl->lastError = ConfigError::Success;
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
    if (!pImpl->validationErrors.empty()) {
        oss << "\nValidation Errors:";
        for (const auto& error : pImpl->validationErrors) {
            oss << "\n  - " << error;
        }
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
