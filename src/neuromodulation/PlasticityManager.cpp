#include "PlasticityManager.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

PlasticityManager::PlasticityManager(std::unique_ptr<STDP> stdp,
                                     std::unique_ptr<Hebbian> hebbian,
                                     std::unique_ptr<StructuralPlasticity> structuralPlasticity,
                                     NeuralDynamics* neuralDynamics)
    : stdp_(std::move(stdp))
    , hebbian_(std::move(hebbian))
    , structuralPlasticity_(std::move(structuralPlasticity))
    , neuralDynamics_(neuralDynamics ? neuralDynamics : nullptr)
    , stdpLearningRate_(0.01f)
    , hebbianLearningRate_(0.01f)
    , structuralPlasticityInterval_(100)
    , totalSTDPUpdates_(0)
    , totalHebbianUpdates_(0)
    , totalStructuralUpdates_(0)
    , currentTime_(0.0f) {}

void PlasticityManager::applyPlasticity(Synapse* synapse,
                                       const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt,
                                       float neuromodulationFactor) {
    if (!synapse) return;
    
    const auto& flags = synapse->getPlasticityFlags();
    
    // Apply STDP with neuromodulation
    if (flags.stdp && !preSpikes.empty() && !postSpikes.empty()) {
        stdp_->update(synapse, preSpikes, postSpikes, dt);
        
        // Apply neuromodulation factor
        float weight = synapse->getWeight();
        float weightChange = (weight > 0 ? 1.0f : -1.0f) * (neuromodulationFactor - 1.0f) * 0.001f;
        synapse->addToWeight(weightChange);
        
        // Clamp weight to valid range
        clampWeight(synapse);
        
        totalSTDPUpdates_++;
    }
    
    // Apply Hebbian learning
    if (flags.hebbian && !preSpikes.empty() && !postSpikes.empty()) {
        hebbian_->update(synapse, preSpikes, postSpikes, dt);
        totalHebbianUpdates_++;
    }
    
    // Apply reward modulation if enabled
    if (flags.reward_modulated) {
        // Update eligibility trace based on spike pairs
        if (!preSpikes.empty() && !postSpikes.empty()) {
            // Simple eligibility trace update
            float eligibilityTrace = synapse->getEligibilityTrace();
            // Reset eligibility for this update cycle
            synapse->setEligibilityTrace(0.0f);
        }
    }
}

void PlasticityManager::applyToAllRegions(std::vector<NeuralRegion*>& regions,
                                          TimestepDuration dt,
                                          float neuromodulationFactor,
                                          SimulationStep currentStep) {
    Timestamp currentTime = static_cast<Timestamp>(currentStep) * dt;
    
    for (auto& region : regions) {
        for (auto& synapse : region->getSynapses()) {
            if (!synapse) continue;
            
            const auto& preSpikes = synapse->getPreSpikeHistory();
            const auto& postSpikes = synapse->getPostSpikeHistory();
            
            // Only process synapses with recent activity
            if (!preSpikes.empty() && !postSpikes.empty()) {
                applyPlasticity(synapse, preSpikes, postSpikes, dt, neuromodulationFactor);
            }
            
            // Update synapse state
            updateSynapseState(synapse, currentTime);
        }
    }
}

bool PlasticityManager::shouldApplyStructuralPlasticity(SimulationStep currentStep) const {
    return (currentStep % structuralPlasticityInterval_) == 0;
}

void PlasticityManager::setStdpLearningRate(float rate) {
    stdpLearningRate_ = std::max(0.0f, std::min(rate, 1.0f));
    if (stdp_) {
        // Pass learning rate to STDP implementation if it supports it
        // This depends on STDP class interface
    }
}

float PlasticityManager::getStdpLearningRate() const {
    return stdpLearningRate_;
}

void PlasticityManager::setHebbianLearningRate(float rate) {
    hebbianLearningRate_ = std::max(0.0f, std::min(rate, 1.0f));
    if (hebbian_) {
        // Pass learning rate to Hebbian implementation if it supports it
        // This depends on Hebbian class interface
    }
}

float PlasticityManager::getHebbianLearningRate() const {
    return hebbianLearningRate_;
}

void PlasticityManager::setStructuralPlasticityInterval(size_t steps) {
    structuralPlasticityInterval_ = std::max(1u, steps);
}

size_t PlasticityManager::getStructuralPlasticityInterval() const {
    return structuralPlasticityInterval_;
}

void PlasticityManager::updateEligibilityTrace(Synapse* synapse, float rewardPrediction) {
    if (!synapse) return;
    
    float trace = synapse->getEligibilityTrace();
    // Simple exponential decay and update
    trace *= 0.95f;  // Decay
    trace += rewardPrediction * 0.05f;  // Update based on prediction error
    synapse->setEligibilityTrace(trace);
}

void PlasticityManager::decayEligibilityTraces(float decayRate) {
    decayRate = std::max(0.0f, std::min(decayRate, 1.0f));
    
    // This would typically iterate through all synapses
    // For modularity, we rely on external iteration
    // decayRate implementation left for integration
}

void PlasticityManager::clampWeight(Synapse* synapse, float minWeight, float maxWeight) const {
    if (!synapse) return;
    
    float weight = synapse->getWeight();
    weight = std::max(minWeight, std::min(weight, maxWeight));
    synapse->setWeight(weight);
}

void PlasticityManager::resetEligibilityTraces() {
    // Reset all synapse eligibility traces
    // Implementation would require access to all synapses
}

bool PlasticityManager::isSynapseEligible(Synapse* synapse) const {
    if (!synapse) return false;
    
    const auto& flags = synapse->getPlasticityFlags();
    float eligibilityTrace = synapse->getEligibilityTrace();
    
    // Check if synapse has eligibility trace and required plasticity flags
    return (flags.hebbian || flags.stdp || flags.reward_modulated) && 
           (eligibilityTrace > 0.01f || flags.eligible);
}

void PlasticityManager::updateSynapseState(Synapse* synapse, Timestamp currentTime) {
    if (synapse) {
        synapse->step(currentTime);
    }
}

void PlasticityManager::applyStructuralPlasticityToRegions(std::vector<NeuralRegion*>& regions,
                                                         TimestepDuration dt,
                                                         SimulationStep currentStep) {
    if (structuralPlasticity_) {
        // For now, call structural plasticity update on the Brain instance
        // This would need to be modified based on the actual StructuralPlasticity interface
        // StructuralPlasticity is typically applied globally, not per-region
        
        // Note: Structural plasticity typically operates on the brain as a whole
        // to create/delete synapses and modify connectivity patterns
        // For modularity, we could pass the brain pointer or structural connectivity info
        
        // Placeholder implementation - would need to be adapted to actual requirements
        // structuralPlasticity_->update(nullptr, dt);
        
        totalStructuralUpdates_++;
    }
}

} // namespace nlm