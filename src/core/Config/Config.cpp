#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cmath>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> schema;  // key -> type constraints
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize basic schema validation for common configuration keys
    initializeSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeSchema() {
    // Basic schema for known configuration keys with type and range validation
    pImpl->schema["random_seed"] = "int64:0-999999999";
    pImpl->schema["simulation_timestep"] = "double:0.0001-1.0";
    pImpl->schema["neuron_count"] = "int64:1-1000000";
    pImpl->schema["region_count"] = "int64:1-10000";
    pImpl->schema["connection_probability"] = "double:0.0-1.0";
    pImpl->schema["v_thresh"] = "double:-100.0-0.0";
    pImpl->schema["v_rest"] = "double:-100.0-0.0";
    pImpl->schema["v_reset"] = "double:-100.0-0.0";
    pImpl->schema["tau_mem"] = "double:1.0-1000.0";
    pImpl->schema["tau_ref"] = "double:0.1-100.0";
    pImpl->schema["plasticity.stdp.enable"] = "bool";
    pImpl->schema["plasticity.stdp.learning_rate"] = "double:0.0-10.0";
    pImpl->schema["plasticity.stdp.tau_plus"] = "double:1.0-1000.0";
    pImpl->schema["plasticity.stdp.tau_minus"] = "double:1.0-1000.0";
    pImpl->schema["neuromod.dopamine.scale"] = "double:0.0-10.0";
    pImpl->schema["neuromod.curiosity.enable"] = "bool";
    pImpl->schema["neuromod.novelty.enable"] = "bool";
}

bool Config::validateKey(const std::string& key, const ConfigValue& value) const {
    auto schemaIt = pImpl->schema.find(key);
    if (schemaIt == pImpl->schema.end()) {
        return true;  // No schema defined, allow any value
    }
    
    const std::string& typeConstraint = schemaIt->second;
    return validateValue(value, typeConstraint);
}

bool Config::validateValue(const ConfigValue& value, const std::string& typeConstraint) const {
    if (typeConstraint.empty()) return true;
    
    // Parse type constraint (e.g., "int64:0-999999999" or "double:0.0-1.0")
    size_t colonPos = typeConstraint.find(':');
    if (colonPos == std::string::npos) {
        return true;  // Invalid format, skip validation
    }
    
    std::string typeStr = typeConstraint.substr(0, colonPos);
    std::string rangeStr = typeConstraint.substr(colonPos + 1);
    
    // Type validation
    bool typeMatch = false;
    double doubleValue = 0.0;
    
    if (typeStr == "int" || typeStr == "int32") {
        typeMatch = std::holds_alternative<int>(value);
        if (typeMatch) doubleValue = static_cast<double>(std::get<int>(value));
    } else if (typeStr == "int64") {
        typeMatch = std::holds_alternative<int64_t>(value);
        if (typeMatch) doubleValue = static_cast<double>(std::get<int64_t>(value));
    } else if (typeStr == "double") {
        typeMatch = std::holds_alternative<double>(value);
        if (typeMatch) doubleValue = std::get<double>(value);
    } else if (typeStr == "bool") {
        typeMatch = std::holds_alternative<bool>(value);
    } else if (typeStr == "string") {
        typeMatch = std::holds_alternative<std::string>(value);
    } else if (typeStr == "vector<int>") {
        typeMatch = std::holds_alternative<std::vector<int>>(value);
    } else if (typeStr == "vector<double>") {
        typeMatch = std::holds_alternative<std::vector<double>>(value);
    }
    
    if (!typeMatch) {
        return false;
    }
    
    // Range validation
    if (!rangeStr.empty() && rangeStr != "any") {
        // Parse range like "0-1.0" or "-100.0-0.0"
        size_t dashPos = rangeStr.find('-');
        if (dashPos != std::string::npos) {
            std::string minStr = rangeStr.substr(0, dashPos);
            std::string maxStr = rangeStr.substr(dashPos + 1);
            
            try {
                double minVal = std::stod(minStr);
                double maxVal = std::stod(maxStr);
                
                if (std::isnan(doubleValue) || doubleValue < minVal || doubleValue > maxVal) {
                    return false;
                }
            } catch (...) {
                return false;
            }
        }
    }
    
    return true;
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open configuration file: " + filepath);
        }
        
        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
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
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Validate key before setting
                if (!validateKeyFormat(key)) {
                    throw std::runtime_error("Invalid configuration key format at line " + 
                                           std::to_string(lineNumber) + ": " + key);
                }
                
                set(key, value, ConfigSource::File);
            } else {
                throw std::runtime_error("Invalid configuration format at line " + 
                                       std::to_string(lineNumber) + ": " + line);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error and return failure
        return false;
    }
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
                
                // Validate key before setting
                if (!validateKeyFormat(key)) {
                    throw std::runtime_error("Invalid configuration key format in command line: " + key);
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                throw std::runtime_error("Invalid command line argument format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key before setting
            if (!validateKeyFormat(key)) {
                throw std::runtime_error("Invalid configuration key format in command line: " + key);
            }
            
            set(key, value, ConfigSource::CommandLine);
        } else {
            throw std::runtime_error("Invalid command line argument: " + arg);
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
    // Validate key format
    if (!validateKeyFormat(key)) {
        throw std::invalid_argument("Invalid configuration key format: " + key);
    }
    
    // Validate value against schema
    if (!validateKey(key, value)) {
        throw std::invalid_argument("Invalid configuration value for key '" + key + "' with value " + 
                                   getValueString(value) + ". " +
                                   getSchemaInfo(key));
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

bool Config::validateKeyFormat(const std::string& key) const {
    if (key.empty()) {
        return false;
    }
    
    // Check for valid characters (alphanumeric, dots, underscores)
    for (char c : key) {
        if (!std::isalnum(c) && c != '.' && c != '_' && c != '-') {
            return false;
        }
    }
    
    // Check for valid key structure (no leading/trailing dots)
    if (key.front() == '.' || key.back() == '.') {
        return false;
    }
    
    return true;
}

std::string Config::getValueString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::string Config::getSchemaInfo(const std::string& key) const {
    auto schemaIt = pImpl->schema.find(key);
    if (schemaIt == pImpl->schema.end()) {
        return "No schema validation defined for this key.";
    }
    return "Expected type: " + schemaIt->second;
}

std::string Config::getSchemaSummary() const {
    std::ostringstream oss;
    oss << "Configuration Schema Validation:" << std::endl;
    for (const auto& pair : pImpl->schema) {
        oss << "  " << pair.first << " -> " << pair.second << std::endl;
    }
    return oss.str();
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
