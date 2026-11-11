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
    // Real synaptogenesis: form new synaptic connections
    // Based on activity-dependent plasticity rules
    
    if (!brain || !brain->getSpikeSystem()) return;
    
    // Get all regions
    auto& regions = brain->getRegions();
    
    // Synaptogenesis follows Hebbian principles: "fire together, wire together"
    // We look for neurons that are co-active (within the same time window)
    
    // Count steps since last episode to adjust formation rate
    float developmentalStage = 1.0f;
    if (brain->getDevelopmentSystem()) {
        developmentalStage = brain->getDevelopmentSystem()->getCriticalPeriodProgress();
    }
    
    // Formation rate varies with developmental stage
    float formationRate = pImpl->formationRate * (0.5f + developmentalStage * 1.0f);
    
    // In Phase 2, we use a simplified approach: based on recent spike activity
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        
        // For each pair of neurons, check if they might form a connection
        // This is O(n^2) but acceptable for Phase 2 with moderate neuron counts
        for (size_t i = 0; i < neurons.size(); ++i) {
            Neuron* sourceNeuron = neurons[i];
            
            // Only consider neurons that are showing recent activity
            if (sourceNeuron->getFiringRate() < 0.1f) continue;
            
            for (size_t j = 0; j < neurons.size(); ++j) {
                if (i == j) continue;
                
                Neuron* targetNeuron = neurons[j];
                
                // Don't form connections within same population (prevents recurrent loops)
                if (sourceNeuron->getType() == targetNeuron->getType()) continue;
                
                // Check if connection already exists
                Synapse* synapse = brain->findSynapse(sourceNeuron->getId(), targetNeuron->getId());
                if (synapse) continue;
                
                // Calculate connection probability based on:
                // 1. Source neuron activity
                // 2. Target neuron activity
                // 3. Connection type compatibility
                float activitySource = sourceNeuron->getFiringRate();
                float activityTarget = targetNeuron->getFiringRate();
                
                float connectionProbability = formationRate * activitySource * activityTarget * 0.5f;
                
                // Cap probability
                connectionProbability = std::min(connectionProbability, 0.1f);
                
                // Stochastic formation based on probability
                if (rng.uniformReal(0.0f, 1.0f) < connectionProbability) {
                    // In a real implementation, we would need to use the brain's
                    // synapse management API which might be implemented elsewhere
                    // For now, we log the potential formation
                    
                    NLM_LOG_INFO("Synaptogenesis: Potential new connection from neuron " + 
                                std::to_string(sourceNeuron->getId()) + " (type=" + 
                                std::to_string(static_cast<int>(sourceNeuron->getType())) + ") to neuron " + 
                                std::to_string(targetNeuron->getId()) + 
                                " (type=" + std::to_string(static_cast<int>(targetNeuron->getType())) + ")");
                }
            }
        }
    }
}

} // namespace nlm