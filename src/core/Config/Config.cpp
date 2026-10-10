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

bool Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the value before setting
    if (!validateValue(key, value)) {
        validationErrors.push_back("Invalid configuration value for key: " + key);
        return false;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    return true;
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

bool Config::validate() const {
    validationErrors.clear();
    
    // Validate all configuration entries against schema
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.key, entry.value)) {
            validationErrors.push_back("Invalid configuration for key: " + entry.key);
        }
    }
    
    // Validate required schema entries
    for (const auto& schemaEntry : schema) {
        if (schemaEntry.required && !has(schemaEntry.key)) {
            validationErrors.push_back("Required configuration key missing: " + schemaEntry.key);
        }
    }
    
    return validationErrors.empty();
}

std::string Config::getValidationErrors() const {
    std::ostringstream oss;
    if (validationErrors.empty()) {
        oss << "Configuration is valid.";
    } else {
        oss << "Configuration validation errors:" << std::endl;
        for (const auto& error : validationErrors) {
            oss << "  - " << error << std::endl;
        }
    }
    return oss.str();
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Find validation function for this key
    for (const auto& schemaEntry : schema) {
        if (schemaEntry.key == key && schemaEntry.validate) {
            return schemaEntry.validate(key, value);
        }
    }
    
    // No specific validation found, use default validation based on key
    return defaultValidation(key, value);
}

bool Config::defaultValidation(const std::string& key, const ConfigValue& value) const {
    // Convert key to lowercase for case-insensitive matching
    std::string lowerKey = toLower(key);
    
    // Validate based on key name
    if (lowerKey == "neuron_count") {
        return validateNeuronCount(key, value);
    } else if (lowerKey == "region_count") {
        return validateRegionCount(key, value);
    } else if (lowerKey == "connection_probability") {
        return validateConnectionProbability(key, value);
    } else if (lowerKey == "simulation_timestep") {
        return validatePositive(key, value);
    } else if (lowerKey == "plasticity_learning_rate") {
        return validateNonNegative(key, value);
    } else if (lowerKey == "random_seed") {
        return validateNonNegative(key, value);
    } else if (lowerKey == "max_simulation_steps") {
        return validateNonNegative(key, value);
    } else if (lowerKey == "environment_width" || lowerKey == "environment_height") {
        return validatePositive(key, value);
    }
    
    // For array values, validate elements
    return validateArrayElements(key, value);
}

bool Config::validatePositive(const std::string& key, const ConfigValue& value) const {
    try {
        double val = std::get<double>(value);
        return val > 0.0;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

bool Config::validateNonNegative(const std::string& key, const ConfigValue& value) const {
    try {
        double val = std::get<double>(value);
        return val >= 0.0;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

bool Config::validateRange(const std::string& key, const ConfigValue& value, double min, double max) const {
    try {
        double val = std::get<double>(value);
        return val >= min && val <= max;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

bool Config::validateNeuronCount(const std::string& key, const ConfigValue& value) const {
    try {
        auto val = std::get<int64_t>(value);
        return val > 0 && val <= 1000000; // Reasonable upper limit
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

bool Config::validateRegionCount(const std::string& key, const ConfigValue& value) const {
    try {
        auto val = std::get<int64_t>(value);
        return val > 0 && val <= 100; // Reasonable upper limit
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

bool Config::validateConnectionProbability(const std::string& key, const ConfigValue& value) const {
    try {
        double val = std::get<double>(value);
        return val >= 0.0 && val <= 1.0;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

bool Config::validateArrayElements(const std::string& key, const ConfigValue& value) const {
    try {
        if (std::holds_alternative<std::vector<int>>(value)) {
            const auto& vec = std::get<std::vector<int>>(value);
            return !vec.empty(); // Non-empty arrays are valid
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& vec = std::get<std::vector<double>>(value);
            return !vec.empty(); // Non-empty arrays are valid
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            const auto& vec = std::get<std::vector<std::string>>(value);
            return !vec.empty(); // Non-empty arrays are valid
        }
    } catch (const std::bad_variant_access&) {
        return false;
    }
    
    return true;
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