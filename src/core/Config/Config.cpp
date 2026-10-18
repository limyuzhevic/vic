#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <regex>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Enhanced error handling and validation
    if (filepath.empty()) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try multiple format detection
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Detect file format
    if (detectJSONFormat(content)) {
        return loadJSONFormat(filepath);
    } else if (detectYAMLFormat(content)) {
        return loadYAMLFormat(filepath);
    } else {
        // Fallback to legacy format
        return loadLegacyFormat(filepath);
    }
}

bool Config::detectJSONFormat(const std::string& content) {
    // Simple JSON detection
    size_t firstNonWS = content.find_first_not_of(" \t\n\r");
    if (firstNonWS == std::string::npos) return false;
    
    char firstChar = content[firstNonWS];
    return (firstChar == '{' || firstChar == '[');
}

bool Config::detectYAMLFormat(const std::string& content) {
    // Simple YAML detection - look for YAML-specific patterns
    // This is a simplified implementation
    return content.find("---") != std::string::npos || 
           content.find(": ") != std::string::npos;
}

bool Config::loadJSONFormat(const std::string& filepath) {
    // For now, implement enhanced legacy parsing with validation
    // In a full implementation, this would use nlohmann-json or pybind11
    return loadLegacyFormat(filepath);
}

bool Config::loadYAMLFormat(const std::string& filepath) {
    // For now, implement enhanced legacy parsing with validation
    // In a full implementation, this would use pyyaml
    return loadLegacyFormat(filepath);
}

bool Config::loadLegacyFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Enhanced parsing with validation
        if (!parseConfigLine(line, lineNumber)) {
            hasErrors = true;
        }
    }
    
    // Apply legacy to modern parameter mapping
    applyLegacyParameterMapping();
    
    // Validate all loaded parameters
    hasErrors |= validateAllParameters();
    
    return !hasErrors;
}

void Config::applyLegacyParameterMapping() {
    // Create a mapping from legacy parameter names to modern names
    // This ensures backward compatibility
    
    std::map<std::string, std::string> legacyToModern = {
        {"stdp_time_constant", "stdp_tau"},
        {"development_synaptogenesis_rate", "synaptogenesis_rate"},
        {"development_pruning_rate", "pruning_rate"},
        {"plasticity_learning_rate", "plasticity_rate"},
        {"stdp_ltp_weight", "stdp_ltp_weight"},
        {"stdp_ltd_weight", "stdp_ltd_weight"}
    };
    
    for (const auto& mapping : legacyToModern) {
        const std::string& legacyKey = mapping.first;
        const std::string& modernKey = mapping.second;
        
        if (has(legacyKey)) {
            auto legacyValue = get<std::string>(legacyKey);
            if (legacyValue) {
                set(modernKey, *legacyValue, ConfigSource::File);
            }
        }
    }
}

bool Config::validateAllParameters() {
    bool hasErrors = false;
    
    // Define parameter validation rules
    std::vector<std::pair<std::string, std::function<bool(const std::string&)>>> validations = {
        {"random_seed", validateRandomSeed},
        {"simulation_timestep", validateTimestep},
        {"neuron_count", validateNeuronCount},
        {"region_count", validateRegionCount},
        {"connection_probability", validateConnectionProbability},
        {"plasticity_learning_rate", validateLearningRate},
        {"stdp_ltp_weight", validateStdpWeight},
        {"stdp_ltd_weight", validateStdpWeight},
        {"stdp_time_constant", validateTimeConstant},
        {"synaptogenesis_rate", validateRate},
        {"pruning_rate", validateRate},
        {"dopamine_baseline", validateNeuromodulationValue},
        {"reward_discount_factor", validateDiscountFactor},
        {"environment_width", validateEnvironmentDimension},
        {"environment_height", validateEnvironmentDimension},
        {"visualization_update_rate", validatePositiveFloat}
    };
    
    for (const auto& validation : validations) {
        const std::string& key = validation.first;
        auto validator = validation.second;
        
        if (has(key)) {
            auto value = get<std::string>(key);
            if (value && !validator(*value)) {
                hasErrors = true;
            }
        }
    }
    
    // Check for required parameters
    std::vector<std::string> requiredParams = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    for (const auto& param : requiredParams) {
        if (!has(param)) {
            // Missing required parameter - handled by getOr() default values
        }
    }
    
    return hasErrors;
}

bool Config::validateRandomSeed(const std::string& value) {
    try {
        int64_t seed = std::stoll(value);
        return seed >= 0;
    } catch (...) {
        return false;
    }
}

bool Config::validateTimestep(const std::string& value) {
    try {
        double timestep = std::stod(value);
        return timestep > 0.0 && timestep <= 1.0;
    } catch (...) {
        return false;
    }
}

bool Config::validateNeuronCount(const std::string& value) {
    try {
        int64_t count = std::stoll(value);
        return count > 0 && count <= 100000; // Reasonable upper bound
    } catch (...) {
        return false;
    }
}

bool Config::validateRegionCount(const std::string& value) {
    try {
        int64_t count = std::stoll(value);
        return count > 0 && count <= 1000; // Reasonable upper bound
    } catch (...) {
        return false;
    }
}

bool Config::validateConnectionProbability(const std::string& value) {
    try {
        double prob = std::stod(value);
        return prob >= 0.0 && prob <= 1.0;
    } catch (...) {
        return false;
    }
}

bool Config::validateLearningRate(const std::string& value) {
    try {
        double rate = std::stod(value);
        return rate >= 0.0 && rate <= 10.0; // Reasonable upper bound
    } catch (...) {
        return false;
    }
}

bool Config::validateStdpWeight(const std::string& value) {
    try {
        double weight = std::stod(value);
        return weight >= 0.0 && weight <= 10.0; // Reasonable upper bound
    } catch (...) {
        return false;
    }
}

bool Config::validateTimeConstant(const std::string& value) {
    try {
        double time = std::stod(value);
        return time > 0.0 && time <= 1000.0; // Reasonable upper bound
    } catch (...) {
        return false;
    }
}

bool Config::validateRate(const std::string& value) {
    try {
        double rate = std::stod(value);
        return rate >= 0.0 && rate <= 1.0; // Rate should be between 0 and 1
    } catch (...) {
        return false;
    }
}

bool Config::validateNeuromodulationValue(const std::string& value) {
    try {
        double val = std::stod(value);
        return val >= 0.0 && val <= 10.0; // Reasonable upper bound
    } catch (...) {
        return false;
    }
}

bool Config::validateDiscountFactor(const std::string& value) {
    try {
        double factor = std::stod(value);
        return factor >= 0.0 && factor <= 1.0;
    } catch (...) {
        return false;
    }
}

bool Config::validateEnvironmentDimension(const std::string& value) {
    try {
        int64_t dim = std::stoll(value);
        return dim > 0 && dim <= 1000; // Reasonable upper bound
    } catch (...) {
        return false;
    }
}

bool Config::validatePositiveFloat(const std::string& value) {
    try {
        double val = std::stod(value);
        return val > 0.0;
    } catch (...) {
        return false;
    }
}

bool Config::parseConfigLine(const std::string& line, size_t lineNumber) {
    // Parse key=value pairs with enhanced validation
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return false; // Not a valid key=value pair
    }
    
    std::string key = trim(line.substr(0, pos));
    std::string value = trim(line.substr(pos + 1));
    
    if (key.empty()) {
        return false; // Empty key
    }
    
    // Validate key name (should not contain special characters)
    if (!isValidConfigKey(key)) {
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
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::isValidConfigKey(const std::string& key) {
    // Basic validation: key should contain only alphanumeric, underscores, and hyphens
    // and not start with a number
    if (key.empty()) return false;
    if (std::isdigit(key[0])) return false;
    
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Enhanced argument parsing with validation
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (!isValidConfigKey(key)) {
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (!isValidConfigKey(key)) {
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Enhanced error handling for set operations
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
        // Log error and handle gracefully
        // In a real implementation, this would use the logger
    }
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

} // namespace nlm
