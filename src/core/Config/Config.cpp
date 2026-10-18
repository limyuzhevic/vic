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

// Validation implementation
bool Config::validate(std::string& errorMessage) const {
    std::vector<std::string> errors;
    
    // Validate known parameters
    if (has("neuron_count")) {
        auto val = get<int64_t>("neuron_count");
        if (!validateNeuronCount(val.value(), errorMessage)) {
            errors.push_back(errorMessage);
        }
    }
    
    if (has("connection_probability")) {
        auto val = get<double>("connection_probability");
        if (!validateConnectionProbability(static_cast<float>(val.value()), errorMessage)) {
            errors.push_back(errorMessage);
        }
    }
    
    if (has("simulation_timestep")) {
        auto val = get<double>("simulation_timestep");
        if (!validateTimestep(val.value(), errorMessage)) {
            errors.push_back(errorMessage);
        }
    }
    
    if (has("random_seed")) {
        auto val = get<int64_t>("random_seed");
        // Random seed validation - should be reasonable range
        if (val.value() < 0 || val.value() > 2000000000) {
            errorMessage = "Random seed must be between 0 and 2,000,000,000";
            errors.push_back(errorMessage);
        }
    }
    
    if (has("stdp_ltp_weight")) {
        auto val = get<double>("stdp_ltp_weight");
        if (!validateWeight(static_cast<float>(val.value()), errorMessage)) {
            errors.push_back(errorMessage);
        }
    }
    
    if (has("stdp_ltd_weight")) {
        auto val = get<double>("stdp_ltd_weight");
        if (!validateWeight(static_cast<float>(val.value()), errorMessage)) {
            errors.push_back(errorMessage);
        }
    }
    
    if (has("stdp_time_constant")) {
        auto val = get<double>("stdp_time_constant");
        if (!validateTimestep(val.value(), errorMessage)) {
            errors.push_back(errorMessage);
        }
    }
    
    if (has("synaptogenesis_rate")) {
        auto val = get<double>("synaptogenesis_rate");
        if (!validateRate(static_cast<float>(val.value()), errorMessage)) {
            errors.push_back(errorMessage);
        }
    }
    
    if (has("pruning_rate")) {
        auto val = get<double>("pruning_rate");
        if (!validateRate(static_cast<float>(val.value()), errorMessage)) {
            errors.push_back(errorMessage);
        }
    }
    
    // If we have any errors, construct the combined message
    if (!errors.empty()) {
        errorMessage = "Configuration validation failed:\n";
        for (const auto& err : errors) {
            errorMessage += "  - " + err + "\n";
        }
        return false;
    }
    
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    std::string errorMessage;
    
    if (!validate(errorMessage)) {
        // Parse errorMessage to extract individual errors
        // For now, just add the full error message as one error
        errors.push_back(errorMessage);
    }
    
    return errors;
}

bool Config::isValid(const std::string& key) const {
    if (!has(key)) {
        return true; // Non-existent keys are considered valid
    }
    
    std::string errorMessage;
    
    // Type-specific validation
    auto val = get<std::string>(key);
    if (val.has_value()) {
        return validateString(val.value(), errorMessage);
    }
    
    auto valInt = get<int64_t>(key);
    if (valInt.has_value()) {
        if (key == "neuron_count") {
            return validateNeuronCount(valInt.value(), errorMessage);
        }
    }
    
    auto valDouble = get<double>(key);
    if (valDouble.has_value()) {
        if (key == "connection_probability") {
            return validateConnectionProbability(static_cast<float>(valDouble.value()), errorMessage);
        } else if (key == "simulation_timestep") {
            return validateTimestep(valDouble.value(), errorMessage);
        } else if (key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || 
                   key == "synaptogenesis_rate" || key == "pruning_rate") {
            return validateRate(static_cast<float>(valDouble.value()), errorMessage);
        } else if (key == "stdp_time_constant") {
            return validateTimestep(valDouble.value(), errorMessage);
        }
    }
    
    return true; // Default for unknown parameter types
}

// Static validation implementations
bool Config::validateNeuronCount(int64_t count, std::string& errorMessage) {
    if (count <= 0) {
        errorMessage = "Neuron count must be positive, got " + std::to_string(count);
        return false;
    }
    if (count > 100000) {
        errorMessage = "Neuron count seems too large: " + std::to_string(count);
        return false;
    }
    return true;
}

bool Config::validateConnectionProbability(float probability, std::string& errorMessage) {
    if (probability < 0.0f || probability > 1.0f) {
        errorMessage = "Connection probability must be between 0.0 and 1.0, got " + std::to_string(probability);
        return false;
    }
    return true;
}

bool Config::validateTimestep(double timestep, std::string& errorMessage) {
    if (timestep <= 0.0) {
        errorMessage = "Simulation timestep must be positive, got " + std::to_string(timestep);
        return false;
    }
    if (timestep > 1.0) {
        errorMessage = "Simulation timestep seems too large: " + std::to_string(timestep) + "s";
        return false;
    }
    return true;
}

bool Config::validateWeight(float weight, std::string& errorMessage) {
    if (weight < -10.0f || weight > 10.0f) {
        errorMessage = "Weight value seems unreasonable: " + std::to_string(weight);
        return false;
    }
    return true;
}

bool Config::validateRate(float rate, std::string& errorMessage) {
    if (rate < 0.0f || rate > 1.0f) {
        errorMessage = "Rate must be between 0.0 and 1.0, got " + std::to_string(rate);
        return false;
    }
    return true;
}

bool Config::validateString(const std::string& value, std::string& errorMessage) {
    if (value.empty()) {
        return true; // Empty strings are valid for some settings
    }
    if (value.length() > 100) {
        errorMessage = "String value too long (max 100 characters): " + value.substr(0, 50) + "...";
        return false;
    }
    return true;
}
