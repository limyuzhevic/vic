#include "ValidatedConfig.hpp"

namespace nlm {

// Implementation of ValidatedConfig methods

ValidationResult ValidatedConfig::validate() const {
    ValidationResult result;
    result.isValid = true;
    
    // Check for required keys
    std::unordered_set<std::string> requiredKeys = {
        "random_seed", "simulation_timestep", "neuron_count", "region_count"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            result.errors.push_back("Missing required configuration key: '" + key + "'");
            result.isValid = false;
        }
    }
    
    // Validate specific values
    auto validateInt = [this](const std::string& key, int64_t min, int64_t max, 
                             const std::string& description) {
        auto val = get<int64_t>(key);
        if (val.has_value()) {
            if (*val < min || *val > max) {
                throw ConfigValidationError(description + " must be between " + 
                                           std::to_string(min) + " and " + 
                                           std::to_string(max) + ", got " + 
                                           std::to_string(*val));
            }
        }
    };
    
    auto validateFloat = [this](const std::string& key, double min, double max,
                                const std::string& description) {
        auto val = get<double>(key);
        if (val.has_value()) {
            if (*val < min || *val > max) {
                throw ConfigValidationError(description + " must be between " + 
                                           std::to_string(min) + " and " + 
                                           std::to_string(max) + ", got " + 
                                           std::to_string(*val));
            }
        }
    };
    
    auto validateProbability = [this](const std::string& key, 
                                       const std::string& description) {
        auto val = get<double>(key);
        if (val.has_value()) {
            if (*val < 0.0 || *val > 1.0) {
                throw ConfigValidationError(description + " must be between 0.0 and 1.0, " +
                                           "got " + std::to_string(*val));
            }
        }
    };
    
    try {
        // Validate neuron_count
        validateInt("neuron_count", 1, 1000000, "neuron_count");
        
        // Validate region_count
        validateInt("region_count", 1, 100, "region_count");
        
        // Validate simulation_timestep
        validateFloat("simulation_timestep", 0.000001, 0.01, "simulation_timestep");
        
        // Validate connection_probability
        validateProbability("connection_probability", "connection_probability");
        
        // Validate plasticity parameters
        validateFloat("stdp_ltp_weight", 0.0, 1.0, "stdp_ltp_weight");
        validateFloat("stdp_ltd_weight", 0.0, 1.0, "stdp_ltd_weight");
        validateFloat("stdp_tau", 1.0, 1000.0, "stdp_tau");
        
        // Validate developmental parameters
        validateFloat("synaptogenesis_rate", 0.0, 0.1, "synaptogenesis_rate");
        validateFloat("pruning_rate", 0.0, 0.1, "pruning_rate");
        
    } catch (const ConfigValidationError& e) {
        ValidationResult errorResult;
        errorResult.isValid = false;
        errorResult.errors.push_back(e.what());
        return errorResult;
    }
    
    // Validate consistency between related parameters
    auto neuronCount = get<int64_t>("neuron_count");
    auto regionCount = get<int64_t>("region_count");
    
    if (neuronCount.has_value() && regionCount.has_value()) {
        if (*neuronCount < *regionCount * 10) {
            result.warnings.push_back("neuron_count should be at least 10x region_count for good coverage");
        }
    }
    
    return result;
}

// Implementation of ValidatedNeuron methods

bool ValidatedNeuron::validateState() const {
    const auto& state = getState();
    
    // Validate membrane potential
    if (state.membranePotential < -100.0f || state.membranePotential > 50.0f) {
        NLM_LOG_WARNING(std::string("Unusual membrane potential: ") + 
                      std::to_string(state.membranePotential) + " mV");
    }
    
    // Validate threshold
    if (state.threshold < -100.0f || state.threshold > 50.0f) {
        throw NeuronValidationError("Invalid threshold: " + std::to_string(state.threshold) + " mV");
    }
    
    // Validate refractory period
    if (state.refractoryPeriod > 1000) {
        NLM_LOG_WARNING(std::string("Long refractory period: ") + 
                      std::to_string(state.refractoryPeriod) + " steps");
    }
    
    return true;
}

bool ValidatedNeuron::safeStepLIF(Timestamp currentTime, TimestepDuration dt) {
    try {
        // Validate state before stepping
        if (!validateState()) {
            return false;
        }
        
        // Record spike history for debugging
        if (getState().firingState == FiringState::Refractory) {
            NLM_LOG_DEBUG(std::string("Neuron ") + std::to_string(getId().value) + 
                        std::string(" in refractory state, last spike: ") + 
                        std::to_string(getLastSpikeTime()));
        }
        
        // Perform the step
        bool fired = stepLIF(currentTime, dt);
        
        // Validate state after stepping
        if (!validateState()) {
            NLM_LOG_ERROR(std::string("State validation failed after step for neuron ") + 
                        std::to_string(getId().value));
        }
        
        return fired;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in safeStepLIF for neuron ") + 
                    std::to_string(getId().value) + ": " + std::string(e.what()));
        return false;
    }
}

// Implementation of ValidatedBrain methods

ValidatedBrain::BrainHealth ValidatedBrain::assessHealth() const {
    BrainHealth health;
    health.isHealthy = true;
    health.neuronHealth = 1.0f;
    health.synapseHealth = 1.0f;
    health.memoryHealth = 1.0f;
    health.neuromodulationHealth = 1.0f;
    
    // Check neuron health
    size_t totalNeurons = getTotalNeuronCount();
    size_t activeNeurons = getActiveNeuronCount();
    size_t firingNeurons = getFiringNeuronCount();
    
    if (totalNeurons == 0) {
        health.neuronHealth = 0.0f;
        health.issues.push_back("No neurons in brain");
        health.isHealthy = false;
    } else {
        health.neuronHealth = static_cast<float>(activeNeurons) / totalNeurons;
        if (health.neuronHealth < 0.1f) {
            health.issues.push_back("Very few active neurons (health: " + 
                                  std::to_string(health.neuronHealth) + ")");
        }
    }
    
    // Check synapse health
    size_t totalSynapses = getTotalSynapseCount();
    if (totalSynapses == 0) {
        health.synapseHealth = 0.0f;
        health.issues.push_back("No synapses in brain");
        health.isHealthy = false;
    }
    
    // Check for excessive firing (potential instability)
    if (firingNeurons > totalNeurons * 0.5f) {
        health.issues.push_back("Excessive firing: " + 
                              std::to_string(firingNeurons) + " of " + 
                              std::to_string(totalNeurons) + " neurons firing");
    }
    
    // Check for dead neurons (no activity)
    if (totalNeurons > 10 && activeNeurons == 0) {
        health.issues.push_back("No neural activity detected");
    }
    
    return health;
}

bool ValidatedBrain::safeStep(SimulationStep currentStep, Timestamp currentTime) {
    try {
        // Assess brain health before stepping
        auto health = assessHealth();
        
        if (!health.isHealthy) {
            NLM_LOG_WARNING("Brain health issues detected before stepping:");
            for (const auto& issue : health.issues) {
                NLM_LOG_WARNING("  - " + issue);
            }
        }
        
        // Perform the step
        Brain::step(currentStep, currentTime);
        
        // Assess health after stepping
        auto postStepHealth = assessHealth();
        
        if (postStepHealth.issues.size() > health.issues.size()) {
            NLM_LOG_WARNING("New health issues detected after stepping:");
            for (const auto& issue : postStepHealth.issues) {
                if (std::find(health.issues.begin(), health.issues.end(), issue) == health.issues.end()) {
                    NLM_LOG_WARNING("  - " + issue);
                }
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in safeStep: ") + std::string(e.what()));
        return false;
    }
}

std::unique_ptr<Action> ValidatedBrain::safeProduceAction() {
    try {
        // Check for emergency conditions
        auto health = assessHealth();
        
        if (health.neuronHealth < 0.05f) {
            NLM_LOG_WARNING("Critical neural activity - producing safe action");
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Check for excessive firing (potential runaway)
        if (health.issues.size() > 2) {
            NLM_LOG_WARNING("Multiple health issues - producing conservative action");
            return std::make_unique<Action>(ActionType::Rest);
        }
        
        // Normal action production
        auto action = Brain::produceAction();
        
        if (!action) {
            NLM_LOG_WARNING("Action production returned null, using default");
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        return action;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in safeProduceAction: ") + std::string(e.what()));
        return std::make_unique<Action>(ActionType::Wait);
    }
}

} // namespace nlm