#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <system_error>
#include <regex>
#include <iostream>

namespace nlm {

// Static member initialization
std::string Config::lastError = "";

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() {
    // Clean up any resources
    clear();
}

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Clear any previous error
    clearLastError();
    
    // Validate filepath
    if (filepath.empty()) {
        lastError = "Configuration file path cannot be empty";
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        lastError = "Failed to open configuration file: " + filepath + ". Error: " +
            std::strerror(errno);
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with enhanced error handling
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            lastError = "Invalid configuration format at line " + std::to_string(lineNum) +
                ": missing '=' separator. Line: " + line;
            return false;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            lastError = "Empty key found at line " + std::to_string(lineNum);
            return false;
        }
        
        if (!validateKeyPattern(key)) {
            lastError = "Invalid key pattern at line " + std::to_string(lineNum) +
                ": key must match pattern ^[a-zA-Z][a-zA-Z0-9_]*(\.[a-zA-Z][a-zA-Z0-9_]*)*$: " + key;
            return false;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
        } catch (const ConfigException& e) {
            lastError = "Failed to set configuration key " + key + " at line " +
                std::to_string(lineNum) + ": " + e.what();
            return false;
        } catch (const std::exception& e) {
            lastError = "Unexpected error setting configuration key " + key +
                " at line " + std::to_string(lineNum) + ": " + e.what();
            return false;
        }
    }
    
    // Validate loaded configuration
    try {
        validateConfig();
    } catch (const ConfigValidationException& e) {
        lastError = "Configuration validation failed after loading from " + filepath +
            ": " + e.what();
        return false;
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    clearLastError();
    
    if (!argv) {
        lastError = "Invalid command line arguments: argv is null";
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            lastError = "Null argument pointer at position " + std::to_string(i);
            return false;
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key before setting
                if (!validateKeyPattern(key)) {
                    lastError = "Invalid key pattern in command line argument: " + key;
                    return false;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                lastError = "Invalid command line argument format: missing '=' in --key=value: " + arg;
                return false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key before setting
            if (!validateKeyPattern(key)) {
                lastError = "Invalid key pattern in command line argument: " + key;
                return false;
            }
            
            set(key, value, ConfigSource::CommandLine);
        } else {
            lastError = "Invalid command line argument format: " + arg;
            return false;
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    clearLastError();
    
    // Validate filepath
    if (filepath.empty()) {
        lastError = "Configuration file path cannot be empty for saving";
        return false;
    }
    
    // Check if directory exists and is writable
    std::filesystem::path filePath(filepath);
    std::filesystem::path parentDir = filePath.parent_path();
    
    if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
        lastError = "Configuration directory does not exist: " + parentDir.string();
        return false;
    }
    
    std::ofstream file(filepath, std::ios::trunc);
    if (!file.is_open()) {
        lastError = "Failed to open configuration file for writing: " + filepath + ". Error: " +
            std::strerror(errno);
        return false;
    }
    
    try {
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            // Convert value to string safely
            std::ostringstream valueStream;
            std::visit([&valueStream](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    valueStream << '"' << arg << '"';
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    valueStream << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) valueStream << ", ";
                        valueStream << arg[i];
                    }
                    valueStream << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    valueStream << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) valueStream << ", ";
                        valueStream << arg[i];
                    }
                    valueStream << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    valueStream << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) valueStream << ", ";
                        valueStream << '"' << arg[i] << '"';
                    }
                    valueStream << "]";
                } else {
                    valueStream << arg;
                }
            }, entry.value);
            
            file << entry.key << " = " << valueStream.str() << "\n";
        }
        
        file << "# Configuration saved at: " << std::filesystem::current_path() << "\n";
        
    } catch (const std::exception& e) {
        lastError = "Failed to write configuration to file " + filepath + ": " + e.what();
        return false;
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        lastError = "Cannot get value for empty key";
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        lastError = "Configuration key not found: " + key;
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        lastError = "Type mismatch when retrieving key " + key + ": expected " +
            typeid(T).name();
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        clearLastError(); // Clear the "not found" error
        return defaultValue;
    }
    return val.value();
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key pattern
    if (key.empty()) {
        throw ConfigValidationException("Cannot set configuration with empty key");
    }
    
    if (!validateKeyPattern(key)) {
        throw ConfigValidationException("Invalid key pattern: " + key +
            ". Key must match pattern ^[a-zA-Z][a-zA-Z0-9_]*(\\.[a-zA-Z][a-zA-Z0-9_]*)*$");
    }
    
    // Validate value range based on key
    if (!validateValueRange(key, value)) {
        throw ConfigValidationException("Invalid value for key " + key +
            ": value " + (std::holds_alternative<std::string>(value) ? std::get<std::string>(value) : "") +
            " is out of acceptable range");
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
    if (key.empty()) {
        lastError = "Cannot check empty key";
        return false;
    }
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    if (key.empty()) {
        lastError = "Cannot remove configuration with empty key";
        return;
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
    clearLastError();
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

void Config::validateConfig() const {
    // Check for required keys
    const std::vector<std::string> requiredKeys = {
        "brain.neuron_count",
        "brain.synapse_density",
        "brain.v_thresh"
    };
    
    std::vector<std::string> missingKeys;
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            missingKeys.push_back(key);
        }
    }
    
    if (!missingKeys.empty()) {
        throw ConfigValidationException("Missing required configuration keys: " +
            (missingKeys.size() > 1 ? "[" + joinStrings(missingKeys, ", ") + "]" : missingKeys[0]));
    }
    
    // Validate specific key ranges
    auto validateNeuronCount = [this]() {
        auto optCount = get<int64_t>("brain.neuron_count");
        if (optCount && *optCount <= 0) {
            throw ConfigValidationException("brain.neuron_count must be positive, got: " +
                std::to_string(*optCount));
        }
    };
    
    auto validateSynapseDensity = [this]() {
        auto optDensity = get<double>("brain.synapse_density");
        if (optDensity) {
            if (*optDensity < 0.0 || *optDensity > 1.0) {
                throw ConfigValidationException("brain.synapse_density must be between 0.0 and 1.0, got: " +
                    std::to_string(*optDensity));
            }
        }
    };
    
    auto validateThresholds = [this]() {
        auto optVThresh = get<double>("brain.v_thresh");
        if (optVThresh) {
            double vThresh = *optVThresh;
            if (vThresh < -100.0 || vThresh > 0.0) {
                throw ConfigValidationException("brain.v_thresh must be between -100.0 and 0.0, got: " +
                    std::to_string(vThresh));
            }
        }
        
        auto optVRest = get<double>("brain.v_rest");
        if (optVRest) {
            double vRest = *optVRest;
            if (vRest < -100.0 || vRest > 0.0) {
                throw ConfigValidationException("brain.v_rest must be between -100.0 and 0.0, got: " +
                    std::to_string(vRest));
            }
        }
    };
    
    validateNeuronCount();
    validateSynapseDensity();
    validateThresholds();
}

bool Config::validateKeyPattern(const std::string& key) {
    // Strict validation: keys must match pattern ^[a-zA-Z][a-zA-Z0-9_]*$ or with dots
    static const std::regex keyPattern("^[a-zA-Z][a-zA-Z0-9_]*(\.[a-zA-Z][a-zA-Z0-9_]*)*$");
    return std::regex_match(key, keyPattern);
}

bool Config::validateValueRange(const std::string& key, const ConfigValue& value) {
    // Validate specific key-value combinations
    if (key == "brain.neuron_count" && std::holds_alternative<int64_t>(value)) {
        return std::get<int64_t>(value) > 0;
    }
    
    if (key == "brain.synapse_density" && std::holds_alternative<double>(value)) {
        double density = std::get<double>(value);
        return density >= 0.0 && density <= 1.0;
    }
    
    if (key == "brain.v_thresh" && std::holds_alternative<double>(value)) {
        double vThresh = std::get<double>(value);
        return vThresh >= -100.0 && vThresh <= 0.0;
    }
    
    if (key == "brain.v_rest" && std::holds_alternative<double>(value)) {
        double vRest = std::get<double>(value);
        return vRest >= -100.0 && vRest <= 0.0;
    }
    
    if (key == "brain.tau_mem" && std::holds_alternative<double>(value)) {
        double tauMem = std::get<double>(value);
        return tauMem > 0.0;
    }
    
    if (key == "brain.tau_ref" && std::holds_alternative<double>(value)) {
        double tauRef = std::get<double>(value);
        return tauRef > 0.0 && tauRef < 100.0;
    }
    
    if (key == "brain.learning_rate" && std::holds_alternative<double>(value)) {
        double lr = std::get<double>(value);
        return lr >= 0.0 && lr <= 1.0;
    }
    
    return true; // No specific validation for this key
}

std::string Config::getLastError() {
    return lastError;
}

void Config::clearLastError() {
    lastError.clear();
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

std::string Config::joinStrings(const std::vector<std::string>& strings, const std::string& delimiter) {
    std::ostringstream oss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) oss << delimiter;
        oss << strings[i];
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
