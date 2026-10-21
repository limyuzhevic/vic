#include "ConfigValidator.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <unordered_set>

namespace nlm {

struct ConfigValidator::Impl {
    std::unordered_set<std::string> validKeys;
    std::unordered_map<std::string, std::pair<double, double>> validRanges;
    std::vector<std::string> validationRules;
    
    Impl() {
        // Initialize with valid configuration keys
        validKeys = {
            "random_seed",
            "simulation_timestep",
            "neuron_count",
            "region_count",
            "connection_probability",
            "stdp_ltp_weight",
            "stdp_ltd_weight",
            "stdp_tau",
            "synaptogenesis_rate",
            "pruning_rate",
            "replay_interval",
            "consolidation_interval",
            "plasticity_increase_rate",
            "maturation_time",
            "critical_period_start",
            "critical_period_end",
            "dopamine_baseline",
            "dopamine_strength",
            "curiosity_rate",
            "novelty_threshold",
            "development_enabled",
            "curiosity_enabled",
            "reward_modulation_enabled",
            "structural_plasticity_enabled",
            "enable_debug_logging"
        };
        
        // Initialize valid ranges
        validRanges = {
            {"random_seed", {0, std::numeric_limits<uint64_t>::max()}},
            {"simulation_timestep", {1e-6, 1.0}},
            {"neuron_count", {10, 100000}},
            {"region_count", {1, 100}},
            {"connection_probability", {0.0, 1.0}},
            {"stdp_ltp_weight", {0.0, 1.0}},
            {"stdp_ltd_weight", {0.0, 1.0}},
            {"stdp_tau", {1.0, 1000.0}},
            {"synaptogenesis_rate", {0.0, 0.1}},
            {"pruning_rate", {0.0, 0.01}},
            {"replay_interval", {1, 10000}},
            {"consolidation_interval", {1, 10000}},
            {"plasticity_increase_rate", {0.0, 1.0}},
            {"maturation_time", {0.0, 1000.0}},
            {"critical_period_start", {0.0, 1000.0}},
            {"critical_period_end", {0.0, 1000.0}},
            {"dopamine_baseline", {0.0, 10.0}},
            {"dopamine_strength", {0.0, 10.0}},
            {"curiosity_rate", {0.0, 10.0}},
            {"novelty_threshold", {0.0, 10.0}},
        };
        
        // Initialize validation rules
        validationRules = {
            "Required keys: random_seed, simulation_timestep, neuron_count, region_count",
            "Neuron count must be between 10 and 100,000",
            "Region count must be between 1 and 100",
            "Connection probability must be between 0.0 and 1.0",
            "STDP weights LTP and LTD should be similar for stability",
            "Synaptogenesis rate should be much lower than pruning rate for stability",
            "All development parameters should be positive if specified",
            "All neuromodulation parameters should be between 0.0 and 10.0"
        };
    }
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}

ConfigValidator::~ConfigValidator() = default;

ConfigValidator::ConfigValidator(ConfigValidator&&) noexcept = default;

ConfigValidator& ConfigValidator::operator=(ConfigValidator&&) noexcept = default;

bool ConfigValidator::validate(const Config& config, std::string& errorMessage) const {
    // Check for required keys first
    std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    if (!validateRequiredKeys(config, requiredKeys, errorMessage)) {
        return false;
    }
    
    // Validate each configuration key
    for (const auto& key : config.getKeys()) {
        if (!isValidKey(key)) {
            errorMessage = "Invalid configuration key: " + key;
            return false;
        }
        
        // Validate range if defined
        auto it = pImpl->validRanges.find(key);
        if (it != pImpl->validRanges.end()) {
            double min = it->second.first;
            double max = it->second.second;
            
            if (key == "random_seed") {
                // Handle int64_t type
                auto val = config.get<int64_t>(key);
                if (val.has_value() && (val.value() < min || val.value() > max)) {
                    errorMessage = "Configuration key '" + key + "' has invalid value " +
                                 std::to_string(val.value()) + ". Must be in range [" +
                                 std::to_string(min) + ", " + std::to_string(max) + "]";
                    return false;
                }
            } else if (key == "neuron_count" || key == "region_count" ||
                      key == "replay_interval" || key == "consolidation_interval") {
                // Handle size_t type
                auto val = config.getOr<size_t>(key, 0);
                if (val < min || val > max) {
                    errorMessage = "Configuration key '" + key + "' has invalid value " +
                                 std::to_string(val) + ". Must be in range [" +
                                 std::to_string(min) + ", " + std::to_string(max) + "]";
                    return false;
                }
            } else {
                // Handle double/float type
                auto val = config.getOr<double>(key, 0.0);
                if (val < min || val > max) {
                    errorMessage = "Configuration key '" + key + "' has invalid value " +
                                 std::to_string(val) + ". Must be in range [" +
                                 std::to_string(min) + ", " + std::to_string(max) + "]";
                    return false;
                }
            }
        }
    }
    
    // Validate specific system parameters
    if (!validateSTDPParameters(config, errorMessage)) return false;
    if (!validateStructuralPlasticityParameters(config, errorMessage)) return false;
    if (!validateDevelopmentParameters(config, errorMessage)) return false;
    if (!validateNeuromodulationParameters(config, errorMessage)) return false;
    
    return true;
}

bool ConfigValidator::validateRequiredKeys(const Config& config,
                                         const std::vector<std::string>& requiredKeys,
                                         std::string& errorMessage) const {
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            errorMessage = "Missing required configuration key: " + key;
            return false;
        }
    }
    return true;
}

bool ConfigValidator::validateRange(const Config& config, const std::string& key,
                                   double min, double max, const std::string& description,
                                   std::string& errorMessage) const {
    if (config.has(key)) {
        double value = config.getOr<double>(key, 0.0);
        if (value < min || value > max) {
            errorMessage = "Configuration key \"" + key + "\"";
            if (!description.empty()) {
                errorMessage += " (\"" + description + "\")";
            }
            errorMessage += " has invalid value " + std::to_string(value) +
                           ". Must be in range [" + std::to_string(min) + ", " +
                           std::to_string(max) + "]";
            return false;
        }
    }
    return true;
}

bool ConfigValidator::validateNeuronCount(const Config& config, std::string& errorMessage) const {
    auto val = config.getOr<size_t>("neuron_count", 0);
    if (val < 10 || val > 100000) {
        errorMessage = "Neuron count must be between 10 and 100,000, got " + std::to_string(val);
        return false;
    }
    return true;
}

bool ConfigValidator::validateRegionCount(const Config& config, std::string& errorMessage) const {
    auto val = config.getOr<size_t>("region_count", 0);
    if (val < 1 || val > 100) {
        errorMessage = "Region count must be between 1 and 100, got " + std::to_string(val);
        return false;
    }
    return true;
}

bool ConfigValidator::validateConnectionProbability(const Config& config, std::string& errorMessage) const {
    auto val = config.getOr<float>("connection_probability", 0.1f);
    if (val < 0.0f || val > 1.0f) {
        errorMessage = "Connection probability must be between 0.0 and 1.0, got " + std::to_string(val);
        return false;
    }
    return true;
}

bool ConfigValidator::validateSTDPParameters(const Config& config, std::string& errorMessage) const {
    if (config.has("stdp_ltp_weight") && config.has("stdp_ltd_weight")) {
        float ltp = config.getOr<float>("stdp_ltp_weight", 0.0f);
        float ltd = config.getOr<float>("stdp_ltd_weight", 0.0f);
        
        if (std::abs(ltp - ltd) > 0.5f) {
            errorMessage = "STDP weights LTP and LTD differ too much (LTP=" + std::to_string(ltp) +
                         ", LTD=" + std::to_string(ltd) + "). "
                         "Consider making them more similar for stable learning.";
            return false;
        }
    }
    return true;
}

bool ConfigValidator::validateStructuralPlasticityParameters(const Config& config, std::string& errorMessage) const {
    if (config.has("synaptogenesis_rate") && config.has("pruning_rate")) {
        float synaptogenesis = config.getOr<float>("synaptogenesis_rate", 0.0f);
        float pruning = config.getOr<float>("pruning_rate", 0.0f);
        
        if (synaptogenesis > 0.001f && pruning > 0.0001f) {
            errorMessage = "Synaptogenesis rate (" + std::to_string(synaptogenesis) +
                         ") is too high relative to pruning rate (" + std::to_string(pruning) +
                         "). Consider reducing synaptogenesis or increasing pruning for stability.";
            return false;
        }
    }
    return true;
}

bool ConfigValidator::validateDevelopmentParameters(const Config& config, std::string& errorMessage) const {
    // Check for development-related parameters
    std::vector<std::string> devKeys = {
        "plasticity_increase_rate",
        "maturation_time",
        "critical_period_start",
        "critical_period_end"
    };
    
    for (const auto& key : devKeys) {
        if (config.has(key)) {
            double value = config.getOr<double>(key, 0.0);
            if (value < 0.0 || value > 1000.0) {
                errorMessage = "Development parameter \"" + key + "\" has invalid value " + std::to_string(value);
                return false;
            }
        }
    }
    return true;
}

bool ConfigValidator::validateNeuromodulationParameters(const Config& config, std::string& errorMessage) const {
    // Check for neuromodulation-related parameters
    std::vector<std::string> neuromodKeys = {
        "dopamine_baseline",
        "dopamine_strength",
        "curiosity_rate",
        "novelty_threshold"
    };
    
    for (const auto& key : neuromodKeys) {
        if (config.has(key)) {
            double value = config.getOr<double>(key, 0.0);
            if (value < 0.0 || value > 10.0) {
                errorMessage = "Neuromodulation parameter \"" + key + "\" has invalid value " + std::to_string(value);
                return false;
            }
        }
    }
    return true;
}

std::vector<std::string> ConfigValidator::getValidationRules() const {
    return pImpl->validationRules;
}

bool ConfigValidator::isValidKey(const std::string& key) const {
    return pImpl->validKeys.find(key) != pImpl->validKeys.end();
}

std::string ConfigValidator::getValidRanges(const std::string& key) const {
    auto it = pImpl->validRanges.find(key);
    if (it == pImpl->validRanges.end()) {
        return "No range defined for key: " + key;
    }
    
    std::ostringstream oss;
    oss << "Range: [" << it->second.first << ", " << it->second.second << "]";
    return oss.str();
}

}} // namespace nlm