#include "Synaptogenesis.hpp"

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = rate;
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = density;
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    // Real synaptogenesis based on developmental principles
    // Implements activity-dependent synapse formation following Hebbian principles
    
    if (!brain) {
        return;
    }
    
    // Get current brain state
    float currentDensity = brain->getCurrentSynapseDensity();
    float targetDensity = getTargetDensity();
    
    // Calculate current progress towards target
    float progress = currentDensity / targetDensity;
    setProgress(progress);
    
    // Activity-dependent synaptogenesis rules
    // Strong co-activation leads to synapse formation (Hebbian)
    // Modulated by developmental stage and neurochemical signals
    
    // Get global neuromodulatory state (dopamine, acetylcholine, etc.)
    auto* dopamine = brain->getDopamine();
    float daLevel = dopamine ? dopamine->getLevel() : 0.5f;
    
    // Calculate formation rate based on multiple factors
    float baseFormationRate = getFormationRate();
    
    // Activity-dependent component
    float activityFactor = std::min(1.0f, brain->getExcitatoryActivityRatio() * 2.0f);
    
    // Neuromodulatory modulation (dopamine promotes synaptogenesis)
    float neuromodulatoryFactor = 1.0f + daLevel * 0.5f;
    
    // Developmental stage factor (slower in early stages)
    float developmentalFactor = std::max(0.1f, 1.0f - getProgress());
    
    // Calculate final formation rate
    float formationRate = baseFormationRate * activityFactor * neuromodulatoryFactor * developmentalFactor;
    
    // Random fluctuations for biological realism
    float randomFactor = rng.uniformReal(0.8f, 1.2f);
    formationRate *= randomFactor;
    
    // Form new synapses based on calculated rate
    if (formationRate > 0.0f && progress < 1.0f) {
        // Estimate how many new synapses to form
        float potentialNewSynapses = formationRate * 100.0f; // Scale factor for simulation
        
        // Create new synapses between active neurons
        auto activeNeurons = brain->getActiveNeurons();
        
        if (activeNeurons.size() >= 2) {
            // Select pairs of co-active neurons for new synapses
            size_t maxNewSynapses = std::min(static_cast<size_t>(potentialNewSynapses), activeNeurons.size() * 2);
            
            for (size_t i = 0; i < maxNewSynapses; ++i) {
                // Random pair selection
                size_t sourceIdx = rng.uniformInt(0, activeNeurons.size() - 1);
                size_t destIdx = rng.uniformInt(0, activeNeurons.size() - 1);
                
                if (sourceIdx != destIdx) {
                    // Create new excitatory synapse
                    SynapseId newSynapseId = synapseCounter++; // Increment global synapse counter
                    
                    // Initialize with appropriate weight
                    float initialWeight = rng.uniformReal(0.1f, 0.3f);
                    
                    // Create synapse and add to brain
                    brain->createSynapse(newSynapseId, 
                                        activeNeurons[sourceIdx]->getId(),
                                        activeNeurons[destIdx]->getId(),
                                        initialWeight);
                }
            }
        }
    }
    
    // Update formation rate based on current progress
    // Slower formation as brain approaches target density
    if (progress > 0.8f) {
        // Rapid deceleration phase
        setFormationRate(formationRate * 0.1f);
    } else if (progress > 0.5f) {
        // Slowdown phase
        setFormationRate(formationRate * 0.5f);
    }
}

} // namespace nlm
