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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Write the actual value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
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
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n";
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

ConfigValidationResult Config::validate() const {
    std::vector<ConfigValidationResult> errors;
    
    // Check for duplicate keys
    std::map<std::string, size_t> keyCounts;
    for (const auto& entry : pImpl->entries) {
        keyCounts[entry.key]++;
    }
    
    for (const auto& pair : keyCounts) {
        if (pair.second > 1) {
            errors.emplace_back(ConfigValidationError::DuplicateKey, 
                               "Duplicate configuration key: " + pair.first, pair.first);
        }
    }
    
    // Validate each entry
    for (const auto& entry : pImpl->entries) {
        auto entryError = validateEntry(entry);
        if (entryError.error != ConfigValidationError::None) {
            errors.push_back(entryError);
        }
    }
    
    // Check for required keys
    auto requiredKeys = getRequiredKeys();
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            errors.emplace_back(ConfigValidationError::MissingRequiredKey,
                               "Missing required configuration key: " + key, key);
        }
    }
    
    // Return first error if any, or success
    if (errors.empty()) {
        return ConfigValidationResult();
    } else {
        return errors[0];
    }
}

bool Config::validateKey(const std::string& key) const {
    auto result = validateEntry(ConfigEntry(key, ConfigValue(std::string("placeholder")), ConfigSource::Default));
    return result.error == ConfigValidationError::None;
}

std::vector<ConfigValidationResult> Config::getValidationErrors() const {
    std::vector<ConfigValidationResult> errors;
    
    // Check for duplicate keys
    std::map<std::string, size_t> keyCounts;
    for (const auto& entry : pImpl->entries) {
        keyCounts[entry.key]++;
    }
    
    for (const auto& pair : keyCounts) {
        if (pair.second > 1) {
            errors.emplace_back(ConfigValidationError::DuplicateKey, 
                               "Duplicate configuration key: " + pair.first, pair.first);
        }
    }
    
    // Validate each entry
    for (const auto& entry : pImpl->entries) {
        auto entryError = validateEntry(entry);
        if (entryError.error != ConfigValidationError::None) {
            errors.push_back(entryError);
        }
    }
    
    // Check for required keys
    auto requiredKeys = getRequiredKeys();
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            errors.emplace_back(ConfigValidationError::MissingRequiredKey,
                               "Missing required configuration key: " + key, key);
        }
    }
    
    return errors;
}

// Validation helper implementations
bool Config::isValidKey(const std::string& key) {
    if (key.empty() || key.find('=') != std::string::npos) {
        return false;
    }
    // Basic key format: alphanumeric with underscores and dashes
    return std::all_of(key.begin(), key.end(), [](char c) {
        return std::isalnum(c) || c == '_' || c == '-';
    });
}

ConfigValidationResult Config::validateEntry(const ConfigEntry& entry) {
    // Check key format
    if (!isValidKey(entry.key)) {
        return ConfigValidationError::InvalidType,
               "Invalid configuration key format: " + entry.key, entry.key;
    }
    
    // Validate based on key and value type
    if (!isValidRange(entry.key, entry.value)) {
        return ConfigValidationError::InvalidRange,
               "Invalid value range for key: " + entry.key, entry.key;
    }
    
    return ConfigValidationResult();
}

std::vector<std::string> Config::getRequiredKeys() {
    return {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count",
        "connection_probability"
    };
}

bool Config::isValidRange(const std::string& key, const ConfigValue& value) {
    // Basic validation based on key type
    if (key == "random_seed") {
        if (std::holds_alternative<int64_t>(value)) {
            auto val = std::get<int64_t>(value);
            return val >= 0 && val <= 2147483647;
        }
        return false;
    } else if (key == "simulation_timestep") {
        if (std::holds_alternative<double>(value)) {
            auto val = std::get<double>(value);
            return val > 0.0 && val <= 1.0;
        }
        return false;
    } else if (key == "neuron_count") {
        if (std::holds_alternative<int64_t>(value)) {
            auto val = std::get<int64_t>(value);
            return val > 0 && val <= 100000;
        }
        return false;
    } else if (key == "region_count") {
        if (std::holds_alternative<int64_t>(value)) {
            auto val = std::get<int64_t>(value);
            return val > 0 && val <= 100;
        }
        return false;
    } else if (key == "connection_probability") {
        if (std::holds_alternative<double>(value)) {
            auto val = std::get<double>(value);
            return val >= 0.0 && val <= 1.0;
        }
        return false;
    } else if (key == "plasticity_learning_rate") {
        if (std::holds_alternative<double>(value)) {
            auto val = std::get<double>(value);
            return val >= 0.0 && val <= 10.0;
        }
        return false;
    }
    
    // For other keys, basic type validation
    return true;
}

// Explicit template instantiations

