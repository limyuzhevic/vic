#include "core/Config/Config.hpp"
#include <algorithm>
#include <sstream>
#include <cstring>

namespace nlm {

struct ConfigValidator::Impl {
    std::vector<ConfigSchemaEntry> schemaEntries;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}

ConfigValidator::~ConfigValidator() = default;

ConfigValidator::ConfigValidator(ConfigValidator&&) noexcept = default;

ConfigValidator& ConfigValidator::operator=(ConfigValidator&&) noexcept = default;

void ConfigValidator::addSchemaEntry(const ConfigSchemaEntry& entry) {
    pImpl->schemaEntries.push_back(entry);
}

std::vector<ValidationError> ConfigValidator::validate(const Config& config) const {
    std::vector<ValidationError> errors;
    
    // Check all schema entries
    for (const auto& schemaEntry : pImpl->schemaEntries) {
        ValidationResult result = ValidationResult::Success;
        
        // Check if value exists
        if (!config.has(schemaEntry.key)) {
            if (schemaEntry.required) {
                errors.emplace_back(schemaEntry.key, 
                    "Required configuration key is missing: " + schemaEntry.key, 
                    ValidationResult::Error);
            } else {
                // Warning for missing optional keys
                errors.emplace_back(schemaEntry.key,
                    "Optional configuration key is missing: " + schemaEntry.key,
                    ValidationResult::Warning);
            }
            continue; // Can't validate missing value
        }
        
        // Validate the value
        std::optional<ConfigValue> valueOpt;
        if (schemaEntry.type == "int") {
            valueOpt = config.get<int>(schemaEntry.key);
        } else if (schemaEntry.type == "int64_t") {
            valueOpt = config.get<int64_t>(schemaEntry.key);
        } else if (schemaEntry.type == "double" || schemaEntry.type == "float") {
            valueOpt = config.get<double>(schemaEntry.key);
        } else if (schemaEntry.type == "bool") {
            valueOpt = config.get<bool>(schemaEntry.key);
        } else if (schemaEntry.type == "string") {
            valueOpt = config.get<std::string>(schemaEntry.key);
        } else if (schemaEntry.type == "vector<int>") {
            valueOpt = config.get<std::vector<int>>(schemaEntry.key);
        } else if (schemaEntry.type == "vector<double>") {
            valueOpt = config.get<std::vector<double>>(schemaEntry.key);
        } else if (schemaEntry.type == "vector<string>") {
            valueOpt = config.get<std::vector<std::string>>(schemaEntry.key);
        }
        
        if (!valueOpt.has_value()) {
            errors.emplace_back(schemaEntry.key,
                "Configuration key " + schemaEntry.key + " has incompatible type",
                ValidationResult::Error);
            continue;
        }
        
        // Apply custom validator if present
        if (schemaEntry.validator) {
            result = schemaEntry.validator(valueOpt.value());
            if (result != ValidationResult::Success) {
                std::string severity = (result == ValidationResult::Warning) ? "Warning" : "Error";
                errors.emplace_back(schemaEntry.key,
                    severity + " validating " + schemaEntry.key + ": ", result);
            }
        }
    }
    
    return errors;
}

ValidationResult ConfigValidator::validateValue(const std::string& key, const ConfigValue& value) const {
    // Find schema entry for this key
    auto it = std::find_if(pImpl->schemaEntries.begin(), pImpl->schemaEntries.end(),
        [&key](const ConfigSchemaEntry& entry) { return entry.key == key; });
    
    if (it == pImpl->schemaEntries.end()) {
        return ValidationResult::Warning; // Unknown key
    }
    
    // Apply custom validator if present
    if (it->validator) {
        return it->validator(value);
    }
    
    return ValidationResult::Success;
}

const std::vector<ConfigSchemaEntry>& ConfigValidator::getSchemaEntries() const {
    return pImpl->schemaEntries;
}

void ConfigValidator::clearSchema() {
    pImpl->schemaEntries.clear();
}

void ConfigValidator::loadDefaultSchema() {
    clearSchema();
    
    // Add basic schema entries for commonly used keys
    // Random seed validation
    addSchemaEntry(ConfigSchemaEntry(
        "random_seed", "int", "Random seed for reproducible experiments", 42, true,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<int>(v);
            return (val >= 0 && val <= INT_MAX) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
    
    // Simulation timestep validation
    addSchemaEntry(ConfigSchemaEntry(
        "simulation_timestep", "double", "Simulation timestep in seconds", 0.001, true,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<double>(v);
            return (val > 0.0 && val < 1.0) ? ValidationResult::Success : ValidationResult::Warning;
        }
    ));
    
    // Neuron count validation
    addSchemaEntry(ConfigSchemaEntry(
        "neuron_count", "int64_t", "Number of neurons in the brain", static_cast<int64_t>(1000), true,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<int64_t>(v);
            return (val > 0) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
    
    // Region count validation
    addSchemaEntry(ConfigSchemaEntry(
        "region_count", "int64_t", "Number of brain regions", static_cast<int64_t>(2), true,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<int64_t>(v);
            return (val > 0) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
    
    // Connection probability validation
    addSchemaEntry(ConfigSchemaEntry(
        "connection_probability", "double", "Connection probability for random connectivity", 0.1, true,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<double>(v);
            return (val >= 0.0 && val <= 1.0) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
    
    // Plasticity parameters
    addSchemaEntry(ConfigSchemaEntry(
        "plasticity_learning_rate", "double", "Plasticity learning rate", 0.01, true,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<double>(v);
            return (val >= 0.0) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
    
    addSchemaEntry(ConfigSchemaEntry(
        "stdp_ltp_weight", "double", "STDP LTP weight", 0.01, true,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<double>(v);
            return (val >= 0.0) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
    
    addSchemaEntry(ConfigSchemaEntry(
        "stdp_ltd_weight", "double", "STDP LTD weight", 0.012, true,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<double>(v);
            return (val >= 0.0) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
    
    addSchemaEntry(ConfigSchemaEntry(
        "stdp_time_constant", "double", "STDP time constant", 20.0, true,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<double>(v);
            return (val > 0.0) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
    
    // Development parameters
    addSchemaEntry(ConfigSchemaEntry(
        "development_synaptogenesis_rate", "double", "Synaptogenesis rate", 0.001, true,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<double>(v);
            return (val >= 0.0 && val <= 1.0) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
    
    addSchemaEntry(ConfigSchemaEntry(
        "development_pruning_rate", "double", "Pruning rate", 0.0001, true,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<double>(v);
            return (val >= 0.0 && val <= 1.0) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
    
    // Neuromodulation
    addSchemaEntry(ConfigSchemaEntry(
        "dopamine_baseline", "double", "Dopamine baseline value", 0.1, true,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<double>(v);
            return (val >= 0.0 && val <= 1.0) ? ValidationResult::Success : ValidationResult::Warning;
        }
    ));
    
    // Reward settings
    addSchemaEntry(ConfigSchemaEntry(
        "reward_discount_factor", "double", "Reward discount factor", 0.99, true,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<double>(v);
            return (val >= 0.0 && val <= 1.0) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
    
    // Environment settings
    addSchemaEntry(ConfigSchemaEntry(
        "environment_name", "string", "Environment name", "GridWorld", false
    ));
    
    addSchemaEntry(ConfigSchemaEntry(
        "environment_width", "int64_t", "Environment width", static_cast<int64_t>(10), false,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<int64_t>(v);
            return (val > 0) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
    
    addSchemaEntry(ConfigSchemaEntry(
        "environment_height", "int64_t", "Environment height", static_cast<int64_t>(10), false,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<int64_t>(v);
            return (val > 0) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
    
    // Logging settings
    addSchemaEntry(ConfigSchemaEntry(
        "log_level", "string", "Log level", "INFO", false
    ));
    
    addSchemaEntry(ConfigSchemaEntry(
        "log_to_file", "bool", "Log to file", false, false
    ));
    
    addSchemaEntry(ConfigSchemaEntry(
        "log_filename", "string", "Log filename", "nlm.log", false
    ));
    
    // Simulation settings
    addSchemaEntry(ConfigSchemaEntry(
        "max_simulation_steps", "int64_t", "Maximum simulation steps", static_cast<int64_t>(10000), false,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<int64_t>(v);
            return (val >= 0) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
    
    addSchemaEntry(ConfigSchemaEntry(
        "simulation_time_limit", "double", "Simulation time limit", 0.0, false,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<double>(v);
            return (val >= 0.0) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
    
    // Visualization settings
    addSchemaEntry(ConfigSchemaEntry(
        "visualization_enabled", "bool", "Enable visualization", false, false
    ));
    
    addSchemaEntry(ConfigSchemaEntry(
        "visualization_update_rate", "double", "Visualization update rate", 30.0, false,
        [](const ConfigValue& v) -> ValidationResult {
            auto val = std::get<double>(v);
            return (val > 0.0) ? ValidationResult::Success : ValidationResult::Error;
        }
    ));
}

} // namespace nlm