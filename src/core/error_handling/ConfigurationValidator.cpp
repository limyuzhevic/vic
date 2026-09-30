#include "ConfigurationValidator.hpp"
#include "exceptions/BrainException.hpp"
#include "core/Types/Types.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void ConfigurationValidator::validateConfigParameters(const std::shared_ptr<Config>& config) {
    if (!config) {
        throw ConfigurationException("Configuration object is null", "ConfigurationValidator");
    }
    
    // Validate essential configuration parameters
    std::vector<std::string> requiredKeys = {
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    for (const auto& key : requiredKeys) {
        if (!config->has(key)) {
            std::map<std::string, std::string> context;
            context["missing_key"] = key;
            context["available_keys"] = "TODO: list available keys";
            throw ConfigurationException(
                "Required configuration parameter is missing",
                context,
                "ConfigurationValidator"
            );
        }
    }
    
    // Validate parameter ranges
    if (auto timestep = config->get<double>("simulation_timestep")) {
        Validator::validateRange(*timestep, 1e-6, 1.0, "simulation_timestep");
    }
    
    if (auto neuronCount = config->get<size_t>("neuron_count")) {
        Validator::validateRange(*neuronCount, 1, 1000000, "neuron_count");
    }
    
    if (auto regionCount = config->get<size_t>("region_count")) {
        Validator::validateRange(*regionCount, 1, 100, "region_count");
    }
}

void ConfigurationValidator::validateNeuronParameters(const std::map<std::string, double>& params) {
    // Validate neuron-specific parameters
    if (params.find("threshold") != params.end()) {
        double threshold = params.at("threshold");
        Validator::validateRange(threshold, -100.0, 0.0, "neuron_threshold");
    }
    
    if (params.find("resting_potential") != params.end()) {
        double resting = params.at("resting_potential");
        Validator::validateRange(resting, -90.0, -50.0, "neuron_resting_potential");
    }
    
    if (params.find("leak_conductance") != params.end()) {
        double leak = params.at("leak_conductance");
        Validator::validateRange(leak, 0.0, 100.0, "neuron_leak_conductance");
    }
}

void ConfigurationValidator::validateSynapseParameters(const std::map<std::string, double>& params) {
    // Validate synapse-specific parameters
    if (params.find("weight") != params.end()) {
        double weight = params.at("weight");
        Validator::validateRange(weight, -1.0, 1.0, "synapse_weight");
    }
    
    if (params.find("delay") != params.end()) {
        double delay = params.at("delay");
        Validator::validateRange(delay, 0.0, 1000.0, "synapse_delay");
    }
}

void ConfigurationValidator::validateMemoryParameters(const std::map<std::string, size_t>& params) {
    // Validate memory-related parameters
    if (params.find("working_memory_capacity") != params.end()) {
        size_t capacity = params.at("working_memory_capacity");
        Validator::validateRange(capacity, 1, 1000000, "working_memory_capacity");
    }
}

void ConfigurationValidator::validatePlasticityParameters(const std::map<std::string, double>& params) {
    // Validate plasticity-related parameters
    if (params.find("hebbian_learning_rate") != params.end()) {
        double rate = params.at("hebbian_learning_rate");
        Validator::validateRange(rate, 0.0, 1.0, "hebbian_learning_rate");
    }
    
    if (params.find("stdp_ltp_weight") != params.end()) {
        double ltp = params.at("stdp_ltp_weight");
        Validator::validateRange(ltp, -1.0, 1.0, "stdp_ltp_weight");
    }
    
    if (params.find("stdp_ltd_weight") != params.end()) {
        double ltd = params.at("stdp_ltd_weight");
        Validator::validateRange(ltd, -1.0, 1.0, "stdp_ltd_weight");
    }
}

void ConfigurationValidator::validateNeuromodulationParameters(const std::map<std::string, double>& params) {
    // Validate neuromodulation-related parameters
    if (params.find("dopamine_level") != params.end()) {
        double dopamine = params.at("dopamine_level");
        Validator::validateRange(dopamine, 0.0, 10.0, "dopamine_level");
    }
    
    if (params.find("curiosity_level") != params.end()) {
        double curiosity = params.at("curiosity_level");
        Validator::validateRange(curiosity, 0.0, 10.0, "curiosity_level");
    }
    
    if (params.find("novelty_level") != params.end()) {
        double novelty = params.at("novelty_level");
        Validator::validateRange(novelty, 0.0, 10.0, "novelty_level");
    }
}

} // namespace nlm
