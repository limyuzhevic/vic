// Error handling and validation implementation for NLM (熙然) Neural Learning Machine
// Phase 6: Final Integration - Comprehensive Error Management

#include "ErrorHandling.hpp"

namespace nlm {

// Initialize static validators and handlers
ValidationResult ConfigValidator::validate(const std::shared_ptr<Config>& config) {
    ValidationResult result;
    
    if (!config) {
        result.addError("Configuration is null");
        return result;
    }
    
    // Validate basic parameters
    auto validateFloat = [&](const std::string& key, float min, float max, bool required = true) {
        auto valueOpt = config->get<float>(key);
        if (required && !valueOpt) {
            result.addError("Required parameter missing: " + key);
            return;
        }
        if (valueOpt) {
            float value = *valueOpt;
            if (value < min || value > max) {
                result.addError("Parameter " + key + " out of range. Expected: [" + 
                              std::to_string(min) + ", " + std::to_string(max) + "], got: " + std::to_string(value));
            }
        }
    };
    
    auto validateInt = [&](const std::string& key, int64_t min, int64_t max, bool required = true) {
        auto valueOpt = config->get<int64_t>(key);
        if (required && !valueOpt) {
            result.addError("Required parameter missing: " + key);
            return;
        }
        if (valueOpt) {
            int64_t value = *valueOpt;
            if (value < min || value > max) {
                result.addError("Parameter " + key + " out of range. Expected: [" + 
                              std::to_string(min) + ", " + std::to_string(max) + "], got: " + std::to_string(value));
            }
        }
    };
    
    auto validatePositive = [&](const std::string& key, bool required = true) {
        auto valueOpt = config->get<float>(key);
        if (required && !valueOpt) {
            result.addError("Required parameter missing: " + key);
            return;
        }
        if (valueOpt) {
            if (*valueOpt < 0.0f) {
                result.addError("Parameter " + key + " must be non-negative, got: " + std::to_string(*valueOpt));
            }
        }
    };
    
    // Validate specific configuration parameters
    validateFloat("simulation_timestep", 0.0001f, 1.0f, true);
    validateFloat("connection_probability", 0.0f, 1.0f, true);
    validateFloat("stdp_ltp_weight", 0.0f, 1.0f, false);
    validateFloat("stdp_ltd_weight", 0.0f, 1.0f, false);
    validateFloat("synaptogenesis_rate", 0.0f, 1.0f, false);
    validateFloat("pruning_rate", 0.0f, 1.0f, false);
    
    validateInt("neuron_count", 1, 1000000, true);
    validateInt("region_count", 1, 100, true);
    validateInt("random_seed", 0, std::numeric_limits<int64_t>::max(), false);
    
    validatePositive("replay_interval", false);
    validatePositive("consolidation_interval", false);
    
    return result;
}

ValidationResult MemoryValidator::validateNeurons(const Brain* brain) {
    ValidationResult result;
    
    if (!brain) {
        result.addError("Brain is null");
        return result;
    }
    
    size_t totalNeurons = brain->getTotalNeuronCount();
    if (totalNeurons == 0) {
        result.addWarning("No neurons in brain - simulation may not be functional");
    } else if (totalNeurons > 1000000) {
        result.addWarning("Large number of neurons (" + std::to_string(totalNeurons) + ") may impact performance");
    }
    
    // Check neuron validity
    auto regions = brain->getRegions();
    for (const auto& region : regions) {
        if (!region) {
            result.addError("Null region found");
            continue;
        }
        
        auto populations = region->getPopulations();
        for (const auto& pop : populations) {
            if (!pop) {
                result.addError("Null population found in region");
                continue;
            }
            
            auto neurons = pop->getNeurons();
            for (const auto* neuron : neurons) {
                if (!neuron) {
                    result.addError("Null neuron found in population");
                    continue;
                }
                
                // Validate neuron state
                const auto& state = neuron->getState();
                if (state.membranePotential < -100.0f || state.membranePotential > 100.0f) {
                    result.addWarning("Unusual membrane potential: " + std::to_string(state.membranePotential) + " mV");
                }
                
                if (state.threshold < 0.0f || state.threshold > 100.0f) {
                    result.addError("Invalid neuron threshold: " + std::to_string(state.threshold));
                }
            }
        }
    }
    
    return result;
}

ValidationResult MemoryValidator::validateSynapses(const Brain* brain) {
    ValidationResult result;
    
    if (!brain) {
        result.addError("Brain is null");
        return result;
    }
    
    size_t totalSynapses = brain->getTotalSynapseCount();
    if (totalSynapses == 0) {
        result.addWarning("No synapses in brain - neural connectivity may be impaired");
    }
    
    // Check synapse validity
    auto regions = brain->getRegions();
    for (const auto& region : regions) {
        if (!region) continue;
        
        auto synapses = region->getSynapses();
        for (const auto* syn : synapses) {
            if (!syn) {
                result.addError("Null synapse found");
                continue;
            }
            
            float weight = syn->getWeight();
            if (std::abs(weight) > 1000.0f) {
                result.addWarning("Extreme synaptic weight: " + std::to_string(weight));
            }
            
            Delay delay = syn->getDelay();
            if (delay < 0 || delay > 1000) {
                result.addError("Invalid synapse delay: " + std::to_string(delay));
            }
        }
    }
    
    return result;
}

void ErrorHandling::reportError(const std::string& message, const std::string& context) {
    std::stringstream ss;
    ss << "[" << context << "] " << message;
    NLM_LOG_ERROR(ss.str());
}

ValidationResult ValidationCoordinator::validateBrain(const Brain* brain, const std::string& context) {
    ValidationResult result;
    std::string ctx = "validateBrain";
    if (!context.empty()) ctx += " [" + context + "]";
    
    if (!brain) {
        result.addError("Brain is null");
        return result;
    }
    
    // Validate memory systems
    auto memoryValidation = MemoryValidator::validateNeurons(brain);
    for (const auto& error : memoryValidation.getErrors()) {
        result.addError(error);
    }
    for (const auto& warning : memoryValidation.getWarnings()) {
        result.addWarning(warning);
    }
    
    // Validate synapses
    auto synapseValidation = MemoryValidator::validateSynapses(brain);
    for (const auto& error : synapseValidation.getErrors()) {
        result.addError(error);
    }
    for (const auto& warning : synapseValidation.getWarnings()) {
        result.addWarning(warning);
    }
    
    return result;
}

ValidationResult ValidationCoordinator::validateExperiment(const CommandLineArgs& args) {
    ValidationResult result;
    
    if (args.experiment != "all" && args.experiment != "basic" && args.experiment != "plasticity" &&
        args.experiment != "stdp" && args.experiment != "phase2" && args.experiment != "phase3" &&
        args.experiment != "phase4" && args.experiment != "phase6") {
        result.addError("Unknown experiment mode: " + args.experiment);
    }
    
    if (args.agentProfile != "default" && args.agentProfile != "explorer" &&
        args.agentProfile != "worker" && args.agentProfile != "social" &&
        args.agentProfile != "conservative" && args.agentProfile != "aggressive" &&
        args.agentProfile != "adaptive") {
        result.addError("Unknown agent profile: " + args.agentProfile);
    }
    
    if (args.benchmarkMode != "off" && args.benchmarkMode != "speed" &&
        args.benchmarkMode != "accuracy" && args.benchmarkMode != "memory" &&
        args.benchmarkMode != "learning") {
        result.addError("Unknown benchmark mode: " + args.benchmarkMode);
    }
    
    if (args.numRuns < 1 || args.numRuns > 100) {
        result.addError("Number of runs must be between 1 and 100, got: " + std::to_string(args.numRuns));
    }
    
    if (args.timeLimit < 0.0) {
        result.addError("Time limit must be non-negative, got: " + std::to_string(args.timeLimit));
    }
    
    if (args.saveCheckpoint && args.checkpointFile.empty()) {
        result.addError("Checkpoint filename is required when saving checkpoint");
    }
    
    return result;
}

} // namespace nlm

(End of file - total 451 lines)