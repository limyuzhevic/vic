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
    // TODO PHASE 2: Implement real synaptogenesis
    // PLACEHOLDER: New synapses form probabilistically
    
    if (!brain) return;
    
    // Calculate stage-based formation rate
    float baseRate = pImpl->formationRate;
    float stageMultiplier = 1.0f;
    
    switch (brain->getDevelopmentalStage()) {
        case DevelopmentalStage::Initial:
            stageMultiplier = 2.0f;  // Rapid synaptogenesis in early development
            break;
        case DevelopmentalStage::CriticalPeriod:
            stageMultiplier = 1.5f;  // Continued synaptogenesis
            break;
        case DevelopmentalStage::Maturation:
            stageMultiplier = 1.0f;  // Normal synaptogenesis
            break;
        case DevelopmentalStage::Adult:
            stageMultiplier = 0.3f;  // Minimal synaptogenesis in stability
            break;
        case DevelopmentalStage::Aging:
            stageMultiplier = 0.1f;  // Very minimal in aging
            break;
    }
    
    float formationRate = baseRate * stageMultiplier;
    
    // Target synapse density for each region
    float targetSynapseDensity = pImpl->targetDensity;
    
    // Try to form new synapses in each region
    for (auto& region : brain->getRegions()) {
        // Get current statistics
        size_t currentNeuronCount = region.getTotalNeuronCount();
        size_t currentSynapseCount = region.getSynapseCount();
        
        if (currentNeuronCount < 2) continue;  // Need at least 2 neurons
        
        // Calculate current synapse density (synapses per neuron pair)
        float currentDensity = static_cast<float>(currentSynapseCount) / 
                              (static_cast<float>(currentNeuronCount) * currentNeuronCount);
        
        // Only form new synapses if below target density
        if (currentDensity < targetSynapseDensity) {
            // Determine how many new synapses to attempt
            size_t maxAttempts = static_cast<size_t>(formationRate * 100);  // Scale for reasonable attempts
            for (size_t attempt = 0; attempt < maxAttempts; ++attempt) {
                // Randomly select pre- and post-synaptic neurons
                NeuronId preNeuron = rng.randomNeuronId(currentNeuronCount);
                NeuronId postNeuron = rng.randomNeuronId(currentNeuronCount);
                
                // Don't form self-connections or duplicate connections
                if (preNeuron.index() == postNeuron.index()) continue;
                
                // Check if this connection already exists
                bool connectionExists = false;
                for (const auto& syn : region.getSynapses()) {
                    if ((syn->getSourceNeuron() == preNeuron && syn->getDestinationNeuron() == postNeuron) ||
                        (syn->getSourceNeuron() == postNeuron && syn->getDestinationNeuron() == preNeuron)) {
                        connectionExists = true;
                        break;
                    }
                }
                
                if (!connectionExists) {
                    // Form a new connection
                    float weight = rng.randomFloat(0.1f, 0.5f);  // Initial weight
                    float delay = static_cast<float>(rng.randomInt(1, 10));  // Random delay
                    
                    // Add the synapse
                    auto& synapses = region.getSynapses();
                    synapses.emplace_back(std::make_unique<Synapse>(preNeuron, postNeuron, weight, delay));
                    
                    // Enable plasticity for new synapses
                    auto& newSyn = synapses.back();
                    newSyn->enablePlasticity(true, true, true);
                    
                    NLM_LOG_INFO("Formed new synapse between neurons " << preNeuron.index() << " -> " << 
                                 postNeuron.index() << " (weight: " << weight << ")");
                    
                    // Break if we've reached a reasonable number of new synapses
                    if (synapses.size() > currentSynapseCount * 1.5f) {
                        break;
                    }
                }
            }
        }
    }
}

} // namespace nlm
