#include "ConfigValidation.hpp"
#include "Config.hpp"
#include "ErrorTypes.hpp"
#include <sstream>

namespace nlm {

bool ConfigValidator::validate(const Config& config, std::vector<std::string>& errors) {
    errors.clear();
    
    // Check required configuration values
    std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count",
        "connection_probability",
        "simulation_timestep"
    };
    
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            errors.push_back("Required configuration key is missing: " + key);
        }
    }
    
    if (!errors.empty()) {
        return false;
    }
    
    // Validate each required value
    auto neuronCount = config.getOr<size_t>("neuron_count", 0);
    if (!validateNeuronCount(neuronCount)) {
        errors.push_back("Invalid neuron_count: " + std::to_string(neuronCount) +
                        ". Must be between 1 and 1000000.");
    }
    
    auto regionCount = config.getOr<size_t>("region_count", 0);
    if (!validateRegionCount(regionCount)) {
        errors.push_back("Invalid region_count: " + std::to_string(regionCount) +
                        ". Must be between 1 and 10.");
    }
    
    auto connectionProbability = config.getOr<float>("connection_probability", -1.0f);
    if (!validateConnectionProbability(connectionProbability)) {
        errors.push_back("Invalid connection_probability: " + std::to_string(connectionProbability) +
                        ". Must be between 0.0 and 1.0.");
    }
    
    auto timestep = config.getOr<double>("simulation_timestep", 0.0);
    if (!validateRange("simulation_timestep", timestep, 1e-6, 1.0)) {
        errors.push_back("Invalid simulation_timestep: " + std::to_string(timestep) +
                        ". Must be between 1e-6 and 1.0.");
    }
    
    // Validate complex configurations
    if (!validatePlasticityParameters(config)) {
        errors.push_back("Invalid plasticity configuration.");
    }
    
    if (!validateDevelopmentParameters(config)) {
        errors.push_back("Invalid development configuration.");
    }
    
    if (!validateNeuromodulationParameters(config)) {
        errors.push_back("Invalid neuromodulation configuration.");
    }
    
    if (!validateMemoryParameters(config)) {
        errors.push_back("Invalid memory configuration.");
    }
    
    if (!validateCheckpointParameters(config)) {
        errors.push_back("Invalid checkpoint configuration.");
    }
    
    return errors.empty();
}

bool ConfigValidator::validateValueType(const std::string& key, const ConfigValue& value,
                                      const std::type_info& expectedType) {
    bool typeMatches = false;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        typeMatches = std::is_same_v<T, std::decay_t<decltype(expectedType)>>;
    }, value);
    
    return typeMatches;
}

bool ConfigValidator::validateRange(const std::string& key, double value, double min, double max) {
    return value >= min && value <= max;
}

bool ConfigValidator::validateConfigSpecific(const std::string& key, const ConfigValue& value) {
    // Custom validation for specific configuration keys
    if (key == "neuron_count") {
        auto val = std::get<size_t>(value);
        return val >= 1 && val <= 1000000;
    } else if (key == "region_count") {
        auto val = std::get<size_t>(value);
        return val >= 1 && val <= 10;
    } else if (key == "connection_probability") {
        auto val = std::get<float>(value);
        return val >= 0.0f && val <= 1.0f;
    } else if (key == "simulation_timestep") {
        auto val = std::get<double>(value);
        return val >= 1e-6 && val <= 1.0;
    }
    
    return true; // Default: accept any value
}

bool ConfigValidator::validateFilePath(const std::string& path, bool requireWritable) {
    if (path.empty()) {
        return false;
    }
    
    // Check if path is valid
    std::error_code ec;
    if (!std::filesystem::exists(path, ec)) {
        // If path doesn't exist and requireWritable, we can't check writability
        if (requireWritable) {
            return false;
        }
        return true; // Don't require existing path
    }
    
    // Check if it's a directory
    if (!std::filesystem::is_directory(path, ec)) {
        // If it's not a directory, check file writability
        return requireWritable ? std::filesystem::is_writable(path, ec) : true;
    }
    
    // Check if directory is writable
    return requireWritable ? std::filesystem::is_writable(path, ec) : true;
}

bool ConfigValidator::validateNeuronCount(size_t value) {
    return value >= 1 && value <= 1000000;
}

bool ConfigValidator::validateRegionCount(size_t value) {
    return value >= 1 && value <= 10;
}

bool ConfigValidator::validateConnectionProbability(float value) {
    return value >= 0.0f && value <= 1.0f;
}

bool ConfigValidator::validatePlasticityParameters(const Config& config) {
    // Validate plasticity-related parameters
    auto ltpWeight = config.getOr<float>("stdp_ltp_weight", 0.01f);
    auto ltdWeight = config.getOr<float>("stdp_ltd_weight", 0.012f);
    auto timeConstant = config.getOr<float>("stdp_time_constant", 20.0f);
    
    if (!validateRange("stdp_ltp_weight", ltpWeight, 0.0f, 1.0f)) {
        return false;
    }
    
    if (!validateRange("stdp_ltd_weight", ltdWeight, 0.0f, 1.0f)) {
        return false;
    }
    
    if (!validateRange("stdp_time_constant", timeConstant, 1.0f, 1000.0f)) {
        return false;
    }
    
    // Ensure LTP weight > LTD weight for stability
    if (ltpWeight <= ltdWeight) {
        return false;
    }
    
    return true;
}

bool ConfigValidator::validateDevelopmentParameters(const Config& config) {
    // Validate development-related parameters
    auto synaptogenesisRate = config.getOr<float>("development_synaptogenesis_rate", 0.001f);
    auto pruningRate = config.getOr<float>("development_pruning_rate", 0.0001f);
    
    if (!validateRange("development_synaptogenesis_rate", synaptogenesisRate, 0.0f, 1.0f)) {
        return false;
    }
    
    if (!validateRange("development_pruning_rate", pruningRate, 0.0f, 1.0f)) {
        return false;
    }
    
    // Ensure synaptogenesis rate > pruning rate for balanced development
    if (synaptogenesisRate <= pruningRate) {
        return false;
    }
    
    return true;
}

bool ConfigValidator::validateNeuromodulationParameters(const Config& config) {
    // Validate neuromodulation-related parameters
    auto dopamineBaseline = config.getOr<float>("dopamine_baseline", 0.1f);
    
    if (!validateRange("dopamine_baseline", dopamineBaseline, 0.0f, 1.0f)) {
        return false;
    }
    
    return true;
}

bool ConfigValidator::validateMemoryParameters(const Config& config) {
    // Validate memory-related parameters
    auto workingMemoryCapacity = config.getOr<size_t>("working_memory_capacity", 100);
    auto maxEpisodes = config.getOr<size_t>("episodic_memory_max_episodes", 1000);
    
    if (!validateRange("working_memory_capacity", workingMemoryCapacity, 1, 10000)) {
        return false;
    }
    
    if (!validateRange("episodic_memory_max_episodes", maxEpisodes, 1, 100000)) {
        return false;
    }
    
    return true;
}

bool ConfigValidator::validateCheckpointParameters(const Config& config) {
    // Validate checkpoint-related parameters
    auto checkpointDir = config.getOr<std::string>("checkpoint_dir", "./checkpoints");
    
    if (!validateFilePath(checkpointDir, false)) {
        return false;
    }
    
    return true;
}

} // namespace nlm
