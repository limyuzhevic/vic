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
    // Enhanced configuration file loading with better error handling
    // Supports JSON-like format with enhanced validation
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == ';') {
            continue;
        }
        
        // Parse key=value pairs with enhanced validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid configuration line " + std::to_string(lineNumber) + 
                           ": missing '=' in: " + line);
            success = false;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARNING("Invalid configuration line " + std::to_string(lineNumber) + 
                           ": empty key in: " + line);
            success = false;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Set with validation
        if (!setWithValidation(key, value, ConfigSource::File)) {
            NLM_LOG_WARNING("Failed to set configuration key " + key + 
                           " on line " + std::to_string(lineNumber));
            success = false;
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
    } else {
        NLM_LOG_WARNING("Configuration file " + filepath + " has syntax errors");
    }
    
    return success;
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
        return std::get<T>(it->value);
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

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Advanced configuration methods implementation

bool Config::setWithValidation(const std::string& key, const std::string& value, ConfigSource source) {
    // Advanced validation for configuration values
    if (!validateKey(key)) {
        NLM_LOG_WARNING("Invalid configuration key: " + key);
        return false;
    }
    
    if (!validateValue(key, value)) {
        NLM_LOG_WARNING("Invalid configuration value for key " + key + ": " + value);
        return false;
    }
    
    set(key, value, source);
    return true;
}

bool Config::validateKey(const std::string& key) const {
    // Basic key validation
    if (key.empty() || key.length() > 100) {
        return false; // Empty or too long
    }
    
    // Check for valid characters (alphanumeric, underscore, dash)
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    // Common configuration keys
    std::vector<std::string> requiredKeys = {
        "random_seed", "simulation_timestep", "neuron_count",
        "region_count", "connection_probability", "learning_rate"
    };
    
    // Check if key is in required keys (optional)
    bool isValid = false;
    for (const auto& requiredKey : requiredKeys) {
        if (toLower(key) == toLower(requiredKey)) {
            isValid = true;
            break;
        }
    }
    
    // Allow any key for future extensibility
    return isValid || key.find('.') != std::string::npos; // Allow dot notation
}

bool Config::validateValue(const std::string& key, const std::string& value) const {
    // Basic value validation
    if (value.empty()) {
        return true; // Empty values are allowed
    }
    
    // Key-specific validation
    std::string lowerKey = toLower(key);
    
    if (lowerKey == "random_seed") {
        try {
            size_t pos;
            int64_t seed = std::stoll(value, &pos);
            return pos == value.length() && seed >= 0;
        } catch (...) {
            return false;
        }
    } else if (lowerKey == "simulation_timestep") {
        try {
            size_t pos;
            double timestep = std::stod(value, &pos);
            return pos == value.length() && timestep > 0.0 && timestep < 10.0;
        } catch (...) {
            return false;
        }
    } else if (lowerKey == "neuron_count" || lowerKey == "region_count") {
        try {
            size_t pos;
            int count = std::stoi(value, &pos);
            return pos == value.length() && count > 0 && count <= 10000;
        } catch (...) {
            return false;
        }
    } else if (lowerKey == "connection_probability" || lowerKey == "learning_rate") {
        try {
            size_t pos;
            double prob = std::stod(value, &pos);
            return pos == value.length() && prob >= 0.0 && prob <= 1.0;
        } catch (...) {
            return false;
        }
    } else if (lowerKey == "enabled" || lowerKey == "debug") {
        return value == "true" || value == "false" || value == "1" || value == "0";
    } else if (lowerKey.find("stdp_") == 0) {
        // STDP parameters
        try {
            size_t pos;
            float param = std::stof(value, &pos);
            return pos == value.length() && param >= 0.0f && param <= 10.0f;
        } catch (...) {
            return false;
        }
    }
    
    // Default validation - accept any non-empty value
    return true;
}

std::string Config::getValidationError(const std::string& key, const std::string& value) const {
    if (validateKey(key) && validateValue(key, value)) {
        return "";
    }
    
    std::string error = "Configuration validation failed:\n";
    
    if (!validateKey(key)) {
        error += "  Invalid key: " + key + "\n";
    }
    
    if (!validateValue(key, value)) {
        error += "  Invalid value for key '" + key + "': '" + value + "'\n";
    }
    
    return error;
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

