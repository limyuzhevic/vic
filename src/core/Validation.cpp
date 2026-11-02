// Validation implementation for NLM
#include "Validation.h"
#include <limits>
#include <cmath>

namespace nlm {

// Implementation of Validator methods

// Range validation
void Validator::validateNeuronCount(size_t count) {
    if (count == 0 || count > 1000000) {
        throw RangeError("neuron_count", static_cast<double>(count), 1.0, 1000000.0);
    }
}

void Validator::validateConnectionProbability(float prob) {
    if (prob < 0.0f || prob > 1.0f) {
        throw RangeError("connection_probability", prob, 0.0f, 1.0f);
    }
}

void Validator::validateTimestep(double timestep) {
    if (timestep < 1e-6 || timestep > 0.1) {
        throw RangeError("simulation_timestep", timestep, 1e-6, 0.1);
    }
}

void Validator::validateConfig(const Config& config) {
    try {
        // Validate neuron count
        if (config.has("neuron_count")) {
            auto countOpt = config.get<int64_t>("neuron_count");
            if (countOpt) {
                validateNeuronCount(static_cast<size_t>(*countOpt));
            }
        }
        
        // Validate connection probability
        if (config.has("connection_probability")) {
            auto probOpt = config.get<float>("connection_probability");
            if (probOpt) {
                validateConnectionProbability(*probOpt);
            }
        }
        
        // Validate simulation timestep
        if (config.has("simulation_timestep")) {
            auto timestepOpt = config.get<double>("simulation_timestep");
            if (timestepOpt) {
                validateTimestep(*timestepOpt);
            }
        }
        
        // Validate random seed
        if (config.has("random_seed")) {
            auto seedOpt = config.get<int64_t>("random_seed");
            if (seedOpt) {
                if (*seedOpt < 0 || *seedOpt > 1000000000) {
                    throw RangeError("random_seed", static_cast<double>(*seedOpt), 0.0, 1e9);
                }
            }
        }
        
        // Validate STDP parameters
        if (config.has("stdp_ltp_weight")) {
            auto ltpOpt = config.get<float>("stdp_ltp_weight");
            if (ltpOpt) {
                if (*ltpOpt < 0.0f || *ltpOpt > 1.0f) {
                    throw RangeError("stdp_ltp_weight", *ltpOpt, 0.0f, 1.0f);
                }
            }
        }
        
        if (config.has("stdp_ltd_weight")) {
            auto ltdOpt = config.get<float>("stdp_ltd_weight");
            if (ltdOpt) {
                if (*ltdOpt < 0.0f || *ltdOpt > 1.0f) {
                    throw RangeError("stdp_ltd_weight", *ltdOpt, 0.0f, 1.0f);
                }
            }
        }
        
        if (config.has("stdp_tau")) {
            auto tauOpt = config.get<float>("stdp_tau");
            if (tauOpt) {
                if (*tauOpt < 0.0f || *tauOpt > 1000.0f) {
                    throw RangeError("stdp_tau", *tauOpt, 0.0f, 1000.0f);
                }
            }
        }
        
    } catch (const ValidationError& e) {
        NLM_LOG_ERROR(std::string("Configuration validation failed: ") + e.what());
        throw;
    }
}

void Validator::validateBrainInitialization(const Config& config) {
    try {
        // Ensure basic configuration is present
        if (!config.has("neuron_count")) {
            throw ConfigurationError("neuron_count configuration required");
        }
        
        if (!config.has("region_count")) {
            throw ConfigurationError("region_count configuration required");
        }
        
        if (!config.has("connection_probability")) {
            throw ConfigurationError("connection_probability configuration required");
        }
        
        if (!config.has("random_seed")) {
            throw ConfigurationError("random_seed configuration required");
        }
        
        // Validate neuron count
        auto countOpt = config.get<int64_t>("neuron_count");
        if (countOpt && *countOpt <= 0) {
            throw ConfigurationError("neuron_count must be positive");
        }
        
        // Validate region count
        auto regionCountOpt = config.get<int64_t>("region_count");
        if (regionCountOpt && *regionCountOpt <= 0) {
            throw ConfigurationError("region_count must be positive");
        }
        
        // Validate connection probability
        auto probOpt = config.get<float>("connection_probability");
        if (probOpt && (*probOpt < 0.0f || *probOpt > 1.0f)) {
            throw ConfigurationError("connection_probability must be between 0.0 and 1.0");
        }
        
    } catch (const ValidationError& e) {
        NLM_LOG_ERROR(std::string("Brain initialization validation failed: ") + e.what());
        throw;
    }
}

} // namespace nlm
