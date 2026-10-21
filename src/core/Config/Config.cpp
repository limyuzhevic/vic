#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigConstraint> constraints;
    std::map<std::string, ConfigValue> defaults;
    std::map<std::string, std::vector<ConfigEntry>> changeHistory;
};

Config::Config() : pImpl(std::make_unique<Impl>()), validationErrors() {
    // Register default parameters with validation constraints
    registerParameter(ConfigConstraint("random_seed", ConfigType::INT64, 1, 1000000000, "Random seed for reproducible experiments", true));
    registerParameter(ConfigConstraint("simulation_timestep", ConfigType::DOUBLE, 0.0001, 1.0, "Simulation timestep in seconds", true));
    registerParameter(ConfigConstraint("neuron_count", ConfigType::INT, 10, 1000000, "Number of neurons in the brain", true));
    registerParameter(ConfigConstraint("region_count", ConfigType::INT, 1, 100, "Number of brain regions", true));
    registerParameter(ConfigConstraint("connection_probability", ConfigType::DOUBLE, 0.0, 1.0, "Connection probability for random connectivity", true));
    registerParameter(ConfigConstraint("plasticity_learning_rate", ConfigType::DOUBLE, 0.0, 1.0, "Plasticity learning rate", false));
    registerParameter(ConfigConstraint("stdp_ltp_weight", ConfigType::DOUBLE, -1.0, 1.0, "STDP LTP weight", false));
    registerParameter(ConfigConstraint("stdp_ltd_weight", ConfigType::DOUBLE, -1.0, 1.0, "STDP LTD weight", false));
    registerParameter(ConfigConstraint("stdp_time_constant", ConfigType::DOUBLE, 1.0, 100.0, "STDP time constant", false));
    registerParameter(ConfigConstraint("development_synaptogenesis_rate", ConfigType::DOUBLE, 0.0, 1.0, "Development synaptogenesis rate", false));
    registerParameter(ConfigConstraint("development_pruning_rate", ConfigType::DOUBLE, 0.0, 1.0, "Development pruning rate", false));
    registerParameter(ConfigConstraint("dopamine_baseline", ConfigType::DOUBLE, -1.0, 1.0, "Dopamine baseline level", false));
    registerParameter(ConfigConstraint("reward_discount_factor", ConfigType::DOUBLE, 0.0, 1.0, "Reward discount factor", false));
    registerParameter(ConfigConstraint("environment_name", ConfigType::STRING, "", "", "Environment name", false));
    registerParameter(ConfigConstraint("environment_width", ConfigType::INT, 1, 1000, "Environment width", false));
    registerParameter(ConfigConstraint("environment_height", ConfigType::INT, 1, 1000, "Environment height", false));
    registerParameter(ConfigConstraint("log_level", ConfigType::STRING, "", "", "Log level (DEBUG, INFO, WARN, ERROR)", false));
    registerParameter(ConfigConstraint("log_to_file", ConfigType::BOOL, "", "", "Log to file", false));
    registerParameter(ConfigConstraint("max_simulation_steps", ConfigType::INT, 1, 1000000, "Maximum simulation steps", false));
    registerParameter(ConfigConstraint("visualization_enabled", ConfigType::BOOL, "", "", "Enable visualization", false));
    
    // Set default values
    setDefault("random_seed", static_cast<int64_t>(42));
    setDefault("simulation_timestep", 0.001);
    setDefault("neuron_count", 1000);
    setDefault("region_count", 2);
    setDefault("connection_probability", 0.1f);
    setDefault("plasticity_learning_rate", 0.01f);
    setDefault("stdp_ltp_weight", 0.01f);
    setDefault("stdp_ltd_weight", 0.012f);
    setDefault("stdp_time_constant", 20.0f);
    setDefault("development_synaptogenesis_rate", 0.001f);
    setDefault("development_pruning_rate", 0.0001f);
    setDefault("dopamine_baseline", 0.1f);
    setDefault("reward_discount_factor", 0.99f);
    setDefault("environment_name", std::string("GridWorld"));
    setDefault("environment_width", 10);
    setDefault("environment_height", 10);
    setDefault("log_level", std::string("INFO"));
    setDefault("log_to_file", false);
    setDefault("max_simulation_steps", 10000);
    setDefault("visualization_enabled", false);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        validationErrors.emplace_back("config_file", "Failed to open configuration file: " + filepath, ConfigSource::File);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with enhanced error handling
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
            
            // Store change in history
            if (has(key)) {
                auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&key](const ConfigEntry& e) { return e.key == key; });
                if (it != pImpl->entries.end()) {
                    pImpl->changeHistory[key].push_back(*it);
                }
            }
            
            set(key, value, ConfigSource::File);
        } else {
            validationErrors.emplace_back(key, "Invalid line format in config file: " + line, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format with validation
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate parameter exists before setting
                if (!isParameterRegistered(key) && hasDefault(key)) {
                    // Allow setting unknown keys if they have defaults
                    set(key, value, ConfigSource::CommandLine);
                } else if (isParameterRegistered(key)) {
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    validationErrors.emplace_back(key, "Unknown configuration parameter: " + key, ConfigSource::CommandLine);
                }
            } else {
                validationErrors.emplace_back(arg.substr(2), "Missing value for parameter: " + arg.substr(2), ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate parameter exists before setting
            if (!isParameterRegistered(key) && hasDefault(key)) {
                // Allow setting unknown keys if they have defaults
                set(key, value, ConfigSource::CommandLine);
            } else if (isParameterRegistered(key)) {
                set(key, value, ConfigSource::CommandLine);
            } else {
                validationErrors.emplace_back(key, "Unknown configuration parameter: " + key, ConfigSource::CommandLine);
            }
        }
    }
    return !validationErrors.empty();
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        validationErrors.emplace_back("save_file", "Failed to create configuration file: " + filepath, ConfigSource::Runtime);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM on " << __TIMESTAMP__ << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << (entry.description.empty() ? "" : entry.description) << "\n";
        file << entry.key << " = ";
        
        // Format the value based on type
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
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });

    if (it == pImpl->entries.end()) {
        // Try to get from defaults if key not found
        if (hasDefault(key)) {
            auto defaultIt = pImpl->defaults.find(key);
            if (defaultIt != pImpl->defaults.end()) {
                try {
                    return std::get<T>(defaultIt->second);
                } catch (const std::bad_variant_access&) {
                    return std::nullopt;
                }
            }
        }
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
    // Store change in history
    if (has(key)) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });n
        if (it != pImpl->entries.end()) {
            pImpl->changeHistory[key].push_back(*it);
        }
    }
    
    // Validate if constraint exists
    bool isValid = true;
    if (isParameterRegistered(key)) {
        std::vector<ConfigValidationError> errors;
        isValid = pImpl->constraints.at(key).validate(value, errors);
        validationErrors.insert(validationErrors.end(), errors.begin(), errors.end());
    }
    
    if (isValid) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });n
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
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
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it != pImpl->entries.end()) {
        pImpl->changeHistory[key].push_back(*it);
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
    for (const auto& entry : pImpl->entries) {
        pImpl->changeHistory[entry.key].push_back(entry);
    }
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " << validationErrors.size() << " validation errors):\n";
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (isParameterRegistered(entry.key)) {
            auto& constraint = pImpl->constraints.at(entry.key);
            if (constraint.required) {
                oss << " *REQUIRED*";
            }
        }
        oss << "\n";
    }
    return oss.str();
}

bool Config::validateAll() const {
    validationErrors.clear();
    
    // Check for required parameters
    for (const auto& constraint : pImpl->constraints) {
        if (constraint.second.required && !has(constraint.first)) {
            if (hasDefault(constraint.first)) {
                // Use default
                set(constraint.first, pImpl->defaults.at(constraint.first), ConfigSource::Default);
            } else {
                validationErrors.emplace_back(constraint.first, 
                    "Required configuration parameter is missing: " + constraint.first, 
                    ConfigSource::Default);
            }
        }
    }
    
    // Validate all set values
    for (const auto& entry : pImpl->entries) {
        if (isParameterRegistered(entry.key)) {
            auto& constraint = pImpl->constraints.at(entry.key);
            validateValue(constraint, entry.value, entry.source);
        }
    }
    
    return validationErrors.empty();
}

std::vector<ConfigValidationError> Config::getValidationErrors() const {
    return validationErrors;
}

void Config::clearValidationErrors() {
    validationErrors.clear();
}

void Config::registerParameter(const ConfigConstraint& constraint) {
    pImpl->constraints[constraint.key] = constraint;
}

bool Config::isParameterRegistered(const std::string& key) const {
    return pImpl->constraints.find(key) != pImpl->constraints.end();
}

void Config::setDefault(const std::string& key, const ConfigValue& value) {
    pImpl->defaults[key] = value;
}

bool Config::hasDefault(const std::string& key) const {
    return pImpl->defaults.find(key) != pImpl->defaults.end();
}

void Config::logChanges() const {
    NLM_LOG_INFO("=== Configuration Change History ===");
    for (const auto& pair : pImpl->changeHistory) {
        if (pair.second.size() > 1) {
            NLM_LOG_INFO("Parameter: " + pair.first);
            for (size_t i = 0; i < pair.second.size() - 1; ++i) {
                NLM_LOG_INFO("  " + std::to_string(i) + ": " + pair.second[i].key + " = " + 
                           (std::get<std::string>(pair.second[i].value) != "" ? std::get<std::string>(pair.second[i].value) : 
                            (std::get<double>(pair.second[i].value) != 0.0 ? std::to_string(std::get<double>(pair.second[i].value)) : 
                             std::to_string(std::get<bool>(pair.second[i].value)))));
            }
            NLM_LOG_INFO("  " + std::to_string(pair.second.size() - 1) + ": CURRENT");
        }
    }
}

bool ConfigConstraint::validate(const ConfigValue& value, std::vector<ConfigValidationError>& errors) const {
    bool isValid = true;
    
    // Type checking
    bool typeMatches = false;
    switch (type) {
        case ConfigType::INT:
            typeMatches = std::holds_alternative<int>(value);
            break;
        case ConfigType::INT64:
            typeMatches = std::holds_alternative<int64_t>(value);
            break;
        case ConfigType::DOUBLE:
            typeMatches = std::holds_alternative<double>(value);
            break;
        case ConfigType::BOOL:
            typeMatches = std::holds_alternative<bool>(value);
            break;
        case ConfigType::STRING:
            typeMatches = std::holds_alternative<std::string>(value);
            break;
        case ConfigType::INT_VECTOR:
            typeMatches = std::holds_alternative<std::vector<int>>(value);
            break;
        case ConfigType::DOUBLE_VECTOR:
            typeMatches = std::holds_alternative<std::vector<double>>(value);
            break;
        case ConfigType::STRING_VECTOR:
            typeMatches = std::holds_alternative<std::vector<std::string>>(value);
            break;
        case ConfigType::NONE:
            typeMatches = true;
            break;
    }
    
    if (!typeMatches) {
        errors.emplace_back(key, "Type mismatch for parameter '" + key + "'. Expected type not found.", ConfigSource::Runtime);
        isValid = false;
    }
    
    // Range validation
    if (isValid && (type == ConfigType::INT || type == ConfigType::INT64 || type == ConfigType::DOUBLE || 
                   type == ConfigType::BOOL)) {
        double numValue = 0.0;
        if (type == ConfigType::INT) {
            numValue = std::get<int>(value);
        } else if (type == ConfigType::INT64) {
            numValue = static_cast<double>(std::get<int64_t>(value));
        } else if (type == ConfigType::DOUBLE) {
            numValue = std::get<double>(value);
        } else if (type == ConfigType::BOOL) {
            numValue = std::get<bool>(value) ? 1.0 : 0.0;
        }
        
        if (minValue.has_value() && numValue < minValue.value()) {
            errors.emplace_back(key, "Value " + std::to_string(numValue) + " for parameter '" + key + 
                               "' is below minimum " + std::to_string(minValue.value()), ConfigSource::Runtime);
            isValid = false;
        }
        
        if (maxValue.has_value() && numValue > maxValue.value()) {
            errors.emplace_back(key, "Value " + std::to_string(numValue) + " for parameter '" + key + 
                               "' exceeds maximum " + std::to_string(maxValue.value()), ConfigSource::Runtime);
            isValid = false;
        }
    }
    
    return isValid;
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

bool Config::validateValue(const ConfigConstraint& constraint, const ConfigValue& value, ConfigSource source) const {
    std::vector<ConfigValidationError> errors;
    bool isValid = constraint.validate(value, errors);
    
    if (!isValid) {
        validationErrors.insert(validationErrors.end(), errors.begin(), errors.end());
    }
    
    return isValid;
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
