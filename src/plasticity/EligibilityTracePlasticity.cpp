#include "EligibilityTracePlasticity.hpp"

namespace nlm {

struct EligibilityTracePlasticity::Impl {
    float traceDecayRate;        // Rate at which eligibility trace decays (per ms)
    float traceLearningRate;     // Learning rate for eligibility trace updates
    float rewardScale;           // Scaling factor for reward signals
    float maxTrace;              // Maximum eligibility trace value
    
    Impl() : traceDecayRate(0.01f), traceLearningRate(0.1f), 
             rewardScale(1.0f), maxTrace(1.0f) {}
};

EligibilityTracePlasticity::EligibilityTracePlasticity() : pImpl(new Impl) {}

EligibilityTracePlasticity::~EligibilityTracePlasticity() {
    delete pImpl;
}

void EligibilityTracePlasticity::update(Synapse* synapse,
                                        const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes,
                                        TimestepDuration dt) {
    if (!synapse) return;
    
    // Update eligibility trace based on spike timing
    updateEligibilityTrace(synapse, preSpikes, postSpikes, dt);
}

void EligibilityTracePlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply weight change directly (reward modulation handled separately)
    synapse->addToWeight(delta);
}

const char* EligibilityTracePlasticity::getName() const {
    return "EligibilityTracePlasticity";
}

void EligibilityTracePlasticity::setTraceDecayRate(float decayRate) {
    pImpl->traceDecayRate = decayRate;
}

float EligibilityTracePlasticity::getTraceDecayRate() const {
    return pImpl->traceDecayRate;
}

void EligibilityTracePlasticity::setTraceLearningRate(float learningRate) {
    pImpl->traceLearningRate = learningRate;
}

float EligibilityTracePlasticity::getTraceLearningRate() const {
    return pImpl->traceLearningRate;
}

void EligibilityTracePlasticity::setRewardScale(float scale) {
    pImpl->rewardScale = scale;
}

float EligibilityTracePlasticity::getRewardScale() const {
    return pImpl->rewardScale;
}

void EligibilityTracePlasticity::setMaxTrace(float maxTrace) {
    pImpl->maxTrace = maxTrace;
}

float EligibilityTracePlasticity::getMaxTrace() const {
    return pImpl->maxTrace;
}

void EligibilityTracePlasticity::updateEligibilityTrace(Synapse* synapse,
                                                         const std::vector<Timestamp>& preSpikes,
                                                         const std::vector<Timestamp>& postSpikes,
                                                         TimestepDuration dt) {
    if (!synapse) return;
    
    // Calculate trace update based on spike timing
    float traceUpdate = 0.0f;
    
    // Implement eligibility trace update rule:
    // For each pre-post spike pair within learning window:
    //   trace += learningRate * exp(-|Δt| / τ)
    // Where Δt = t_post - t_pre is spike timing difference
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float deltaT = static_cast<float>(postTime - preTime);
            
            // Use exponential decay for spike timing differences
            float timingFactor = std::exp(-std::abs(deltaT) / 10.0f);  // 10ms time constant
            
            traceUpdate += pImpl->traceLearningRate * timingFactor;
        }
    }
    
    // Apply trace update to synapse
    float currentTrace = getEligibilityTrace(synapse);
    float newTrace = std::min(currentTrace + traceUpdate, pImpl->maxTrace);
    
    // Store eligibility trace in synapse (assuming synapse has this capability)
    synapse->setEligibilityTrace(newTrace);
}

void EligibilityTracePlasticity::applyRewardSignal(float reward, const std::vector<Synapse*>& synapses) {
    // Apply reward-modulated learning:
    // weight change = eligibility_trace * reward * reward_scale
    
    for (Synapse* synapse : synapses) {
        if (!synapse) continue;
        
        float eligibility = getEligibilityTrace(synapse);
        float weightChange = eligibility * reward * pImpl->rewardScale;
        
        if (std::abs(weightChange) > 1e-6f) {
            applyWeightChange(synapse, weightChange);
            // Decay trace after applying reward
            synapse->setEligibilityTrace(eligibility * (1.0f - pImpl->traceDecayRate));
        }
    }
}

void EligibilityTracePlasticity::resetEligibilityTrace(Synapse* synapse) {
    if (synapse) {
        synapse->setEligibilityTrace(0.0f);
    }
}

float EligibilityTracePlasticity::getEligibilityTrace(Synapse* synapse) const {
    if (synapse) {
        return synapse->getEligibilityTrace();
    }
    return 0.0f;
}

void EligibilityTracePlasticity::updateBatch(const std::vector<std::pair<Synapse*, std::vector<Timestamp>>>& preSpikes,
                                             const std::vector<std::pair<Synapse*, std::vector<Timestamp>>>& postSpikes,
                                             TimestepDuration dt) {
    // Batch update for simulation efficiency
    for (const auto& prePair : preSpikes) {
        Synapse* synapse = prePair.first;
        if (!synapse) continue;
        
        // Find corresponding post spikes
        for (const auto& postPair : postSpikes) {
            if (postPair.first == synapse) {
                updateEligibilityTrace(synapse, prePair.second, postPair.second, dt);
                break;
            }
        }
    }
}

void EligibilityTracePlasticity::applyRewardBatch(float reward, const std::vector<Synapse*>& synapses) {
    // Batch reward application
    applyRewardSignal(reward, synapses);
}

void EligibilityTracePlasticity::configureFromConfig(std::shared_ptr<Config> config) {
    if (!config) return;
    
    // Configure from system configuration
    setTraceDecayRate(config->getOr<float>("eligibility_trace_decay", 0.01f));
    setTraceLearningRate(config->getOr<float>("eligibility_trace_learning_rate", 0.1f));
    setRewardScale(config->getOr<float>("reward_scale", 1.0f));
    setMaxTrace(config->getOr<float>("max_eligibility_trace", 1.0f));
}

} // namespace nlm
