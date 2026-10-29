#include "PlasticityRule.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    float ltpTimeConstant;
    float ltdTimeConstant;
    float maxWeight;
    float minWeight;
    float maxWeightChange;
    
    Impl() : learningRate(0.01f), ltpTimeConstant(20.0f), ltdTimeConstant(20.0f), 
             maxWeight(1.0f), minWeight(0.0f), maxWeightChange(0.1f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

// ============================================================================
// Error handling and validation for PlasticityRule operations
// ============================================================================

namespace {
    const float MAX_VALID_WEIGHT = 100.0f;
    const float MIN_VALID_WEIGHT = -100.0f;
    const float MAX_LEARNING_RATE = 10.0f;
    const float MIN_LEARNING_RATE = 0.0001f;
    const float MAX_TIME_CONSTANT = 1000.0f;
    const float MIN_TIME_CONSTANT = 0.001f;
    
    bool validateSynapseParameters(const Synapse* synapse) {
        if (!synapse) {
            return false;
        }
        
        float weight = synapse->getWeight();
        if (std::isnan(weight) || std::isinf(weight)) {
            return false;
        }
        
        if (weight > MAX_VALID_WEIGHT || weight < MIN_VALID_WEIGHT) {
            return false;
        }
        
        return true;
    }
    
    bool validateSpikeVectors(const std::vector<Timestamp>& preSpikes, 
                              const std::vector<Timestamp>& postSpikes,
                              TimestepDuration dt) {
        if (preSpikes.empty() || postSpikes.empty()) {
            return false;
        }
        
        if (dt <= 0.0) {
            return false;
        }
        
        // Check for valid spike times
        for (Timestamp spike : preSpikes) {
            if (std::isnan(spike) || std::isinf(spike)) {
                return false;
            }
        }
        
        for (Timestamp spike : postSpikes) {
            if (std::isnan(spike) || std::isinf(spike)) {
                return false;
            }
        }
        
        return true;
    }
    
    void logError(const std::string& operation, const std::string& details) {
        // Basic error logging - in a real implementation, this would use
        // the proper logging system (nlm::Logger)
        std::cerr << "[ERROR] PlasticityRule::" << operation << ": " << details << std::endl;
    }
    
    bool validatePlasticityParameters() const {
        if (pImpl->learningRate < MIN_LEARNING_RATE || pImpl->learningRate > MAX_LEARNING_RATE) {
            logError("setLearningRate", "Learning rate " + std::to_string(pImpl->learningRate) + " is out of valid range [" + 
                     std::to_string(MIN_LEARNING_RATE) + ", " + std::to_string(MAX_LEARNING_RATE) + "]");
            return false;
        }
        
        if (pImpl->ltpTimeConstant < MIN_TIME_CONSTANT || pImpl->ltpTimeConstant > MAX_TIME_CONSTANT) {
            logError("setLTPTimeConstant", "LTP time constant " + std::to_string(pImpl->ltpTimeConstant) + 
                     " is out of valid range [" + std::to_string(MIN_TIME_CONSTANT) + ", " + 
                     std::to_string(MAX_TIME_CONSTANT) + "]");
            return false;
        }
        
        if (pImpl->ltdTimeConstant < MIN_TIME_CONSTANT || pImpl->ltdTimeConstant > MAX_TIME_CONSTANT) {
            logError("setLTDTimeConstant", "LTD time constant " + std::to_string(pImpl->ltdTimeConstant) + 
                     " is out of valid range [" + std::to_string(MIN_TIME_CONSTANT) + ", " + 
                     std::to_string(MAX_TIME_CONSTANT) + "]");
            return false;
        }
        
        if (pImpl->maxWeight < MIN_VALID_WEIGHT || pImpl->maxWeight > MAX_VALID_WEIGHT) {
            logError("setMaxWeight", "Max weight " + std::to_string(pImpl->maxWeight) + " is out of valid range [" + 
                     std::to_string(MIN_VALID_WEIGHT) + ", " + std::to_string(MAX_VALID_WEIGHT) + "]");
            return false;
        }
        
        if (pImpl->minWeight < MIN_VALID_WEIGHT || pImpl->minWeight > MAX_VALID_WEIGHT) {
            logError("setMinWeight", "Min weight " + std::to_string(pImpl->minWeight) + " is out of valid range [" + 
                     std::to_string(MIN_VALID_WEIGHT) + ", " + std::to_string(MAX_VALID_WEIGHT) + "]");
            return false;
        }
        
        if (pImpl->maxWeightChange < 0.0f) {
            logError("setMaxWeightChange", "Max weight change must be non-negative, got " + std::to_string(pImpl->maxWeightChange));
            return false;
        }
        
        return true;
    }
} // anonymous namespace

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // ============================================================================
    // Enhanced STDP-based Hebbian learning with comprehensive error checking
    // ============================================================================
    
    // Validate input parameters
    if (!validateSynapseParameters(synapse)) {
        logError("update", "Invalid synapse parameters");
        return;
    }
    
    if (!validateSpikeVectors(preSpikes, postSpikes, dt)) {
        logError("update", "Invalid spike vector parameters");
        return;
    }
    
    // Ensure plasticity rule is enabled
    if (!isEnabled()) {
        logError("update", "Hebbian plasticity rule is not enabled");
        return;
    }
    
    // Process each spike pair for STDP learning
    for (Timestamp preSpike : preSpikes) {
        for (Timestamp postSpike : postSpikes) {
            double delta_t = static_cast<double>(postSpike) - static_cast<double>(preSpike);
            
            // Apply STDP learning rules with validation
            if (delta_t > 0) {
                // Post-synaptic spike after pre-synaptic spike -> LTP
                // Exponential decay of LTP effect with time difference
                double ltpWeight = pImpl->learningRate * std::exp(-delta_t / pImpl->ltpTimeConstant);
                
                // Validate the weight change
                if (!std::isnan(ltpWeight) && !std::isinf(ltpWeight)) {
                    synapse->addToWeight(static_cast<float>(ltpWeight));
                } else {
                    logError("update", "Computed invalid LTP weight: " + std::to_string(ltpWeight));
                }
            } else {
                // Post-synaptic spike before pre-synaptic spike -> LTD  
                // Exponential decay of LTD effect with time difference
                double ltdWeight = pImpl->learningRate * std::exp(delta_t / pImpl->ltdTimeConstant);
                
                // Validate the weight change
                if (!std::isnan(ltdWeight) && !std::isinf(ltdWeight)) {
                    synapse->addToWeight(-static_cast<float>(ltdWeight));  // Negative for depression
                } else {
                    logError("update", "Computed invalid LTD weight: " + std::to_string(ltdWeight));
                }
            }
        }
    }
    
    // Apply global normalization to prevent runaway excitation with validation
    float currentWeight = synapse->getWeight();
    
    // Validate current weight
    if (std::isnan(currentWeight) || std::isinf(currentWeight)) {
        logError("update", "Invalid current weight: " + std::to_string(currentWeight));
        return;
    }
    
    if (currentWeight > pImpl->maxWeight) {
        // Log warning before clamping
        logError("update", "Weight " + std::to_string(currentWeight) + " exceeds maximum " + 
                 std::to_string(pImpl->maxWeight) + ", clamping to maximum");
        synapse->setWeight(pImpl->maxWeight);
    } else if (currentWeight < pImpl->minWeight) {
        // Log warning before clamping
        logError("update", "Weight " + std::to_string(currentWeight) + " below minimum " + 
                 std::to_string(pImpl->minWeight) + ", clamping to minimum");
        synapse->setWeight(pImpl->minWeight);
    }
    
    // Verify weight is within valid range after clamping
    float finalWeight = synapse->getWeight();
    if (finalWeight > MAX_VALID_WEIGHT || finalWeight < MIN_VALID_WEIGHT) {
        logError("update", "Final weight " + std::to_string(finalWeight) + " is out of valid range [" + 
                 std::to_string(MIN_VALID_WEIGHT) + ", " + std::to_string(MAX_VALID_WEIGHT) + "]");
        // Attempt to recover by clamping to valid range
        if (finalWeight > MAX_VALID_WEIGHT) {
            synapse->setWeight(MAX_VALID_WEIGHT);
        } else {
            synapse->setWeight(MIN_VALID_WEIGHT);
        }
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setLTPTimeConstant(float tau) {
    pImpl->ltpTimeConstant = tau;
}

float HebbianRule::getLTPTimeConstant() const {
    return pImpl->ltpTimeConstant;
}

void HebbianRule::setLTDTimeConstant(float tau) {
    pImpl->ltdTimeConstant = tau;
}

float HebbianRule::getLTDTimeConstant() const {
    return pImpl->ltdTimeConstant;
}

void HebbianRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = maxWeight;
}

float HebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void HebbianRule::setMinWeight(float minWeight) {
    pImpl->minWeight = minWeight;
}

float HebbianRule::getMinWeight() const {
    return pImpl->minWeight;
}

void HebbianRule::setMaxWeightChange(float maxChange) {
    pImpl->maxWeightChange = maxChange;
}

float HebbianRule::getMaxWeightChange() const {
    return pImpl->maxWeightChange;
}

} // namespace nlm
