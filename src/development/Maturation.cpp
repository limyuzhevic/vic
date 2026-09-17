#include "Maturation.hpp"

namespace nlm {

struct Maturation::Impl {
    float progress;
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f) {}
};

Maturation::Maturation() : pImpl(new Impl) {}

Maturation::~Maturation() = default;

float Maturation::getProgress() const {
    return pImpl->progress;
}

void Maturation::setProgress(float progress) {
    pImpl->progress = std::clamp(progress, 0.0f, 1.0f);
}

void Maturation::update(Brain* brain, SimulationStep currentStep) {
    // TODO PHASE 2: Implement real maturation
    // Real implementation: Neuron and synapse maturation over developmental time
    
    if (!brain) return;
    
    // Maturation progresses over time based on developmental stage
    float maturationRate = 0.0001f * (currentStep * 0.001f);  // Gradual progression
    pImpl->progress = std::min(1.0f, pImpl->progress + maturationRate);
    
    // Get all neurons in the brain
    auto regions = brain->getRegions();
    
    if (pImpl->progress >= 0.5f) {
        // Advanced maturation stage: refine synaptic connections and neuron properties
        
        for (auto& region : regions) {
            auto neurons = region->getAllNeurons();
            auto& synapses = region->getSynapses();
            
            // 1. Refine neuron properties toward mature state
            for (auto& neuron : neurons) {
                // Adjust neuron properties toward mature values
                float maturationFactor = pImpl->progress;
                
                // Move toward mature threshold
                float targetThreshold = pImpl->matureThreshold;
                float currentThreshold = neuron->getThreshold();
                float newThreshold = currentThreshold + (targetThreshold - currentThreshold) * maturationFactor * 0.01f;
                neuron->setThreshold(newThreshold);
                
                // Move toward mature resting potential
                float targetResting = pImpl->matureRestingPotential;
                float currentResting = neuron->getRestingPotential();
                float newResting = currentResting + (targetResting - currentResting) * maturationFactor * 0.01f;
                neuron->setRestingPotential(newResting);
                
                // Adjust time constant toward mature value
                // Note: The actual time constant would be set through configuration
                // neuron->setTimeConstant(pImpl->matureTimeConstant * maturationFactor);
                
                // Reduce initial variability in neuron parameters
                neuron->setRefractoryPeriod(static_cast<uint32_t>(5 + maturationFactor * 5));
            }
            
            // 2. Refine synaptic connections
            for (auto& synapse : synapses) {
                // Move synapse weight toward optimal range based on type
                float targetWeight = 0.0f;
                if (synapse->isExcitatory()) {
                    targetWeight = 0.2f + maturationFactor * 0.2f;  // Move toward moderate excitation
                } else {
                    targetWeight = -0.2f - maturationFactor * 0.2f;  // Move toward moderate inhibition
                }
                
                float currentWeight = synapse->getWeight();
                float newWeight = currentWeight + (targetWeight - currentWeight) * maturationFactor * 0.02f;
                synapse->setWeight(newWeight);
                
                // Adjust synaptic delay toward mature values (typically shorter)
                float targetDelay = 1;  // Default 1 step
                float currentDelay = synapse->getDelay();
                float newDelay = currentDelay + (targetDelay - currentDelay) * maturationFactor * 0.01f;
                synapse->setDelay(static_cast<Delay>(newDelay));
                
                // Adjust synapse efficacy toward mature values
                float targetEfficacy = 1.0f;
                float currentEfficacy = synapse->getEfficacy();
                float newEfficacy = currentEfficacy + (targetEfficacy - currentEfficacy) * maturationFactor * 0.01f;
                synapse->setEfficacy(newEfficacy);
            }
        }
    } else if (pImpl->progress >= 0.2f) {
        // Early maturation: initialize basic properties
        for (auto& region : regions) {
            auto neurons = region->getAllNeurons();
            
            for (auto& neuron : neurons) {
                // Set initial parameters closer to mature values
                // Neurons become more stable and less noisy
                float stabilityFactor = (pImpl->progress - 0.2f) * 5.0f;
                
                // Neurons become more predictable in their firing patterns
                // (In a real implementation, would affect neural dynamics)
                // neuron->setNoiseLevel(0.1f * (1.0f - stabilityFactor));
            }
        }
    }
}

MembranePotential Maturation::getMatureThreshold() const {
    return pImpl->matureThreshold;
}

MembranePotential Maturation::getMatureRestingPotential() const {
    return pImpl->matureRestingPotential;
}

float Maturation::getMatureTimeConstant() const {
    return pImpl->matureTimeConstant;
}

} // namespace nlm
