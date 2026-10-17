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
    // Real synaptogenesis: biologically-inspired synapse formation
    if (!brain || pImpl->formationRate <= 0.0f) {
        return;
    }
    
    // Calculate current connectivity density
    const auto& regions = brain->getRegions();
    if (regions.empty()) return;
    
    float totalConnections = 0.0f;
    float totalPossibleConnections = 0.0f;
    
    for (const auto& region : regions) {
        const auto& neurons = region.getNeurons();
        size_t neuronCount = neurons.size();
        
        // Count existing connections within and between regions
        for (const auto& neuron : neurons) {
            const auto& outputs = neuron.getOutputs();
            totalConnections += outputs.size();
            totalPossibleConnections += neurons.size() - 1; // Exclude self-connections
        }
        
        // Add inter-region connections
        for (size_t i = 0; i < regions.size(); ++i) {
            if (i == &region - &regions[0]) continue;
            
            const auto& otherNeurons = regions[i].getNeurons();
            totalPossibleConnections += neuronCount * otherNeurons.size();
        }
    }
    
    // Calculate current density
    float currentDensity = 0.0f;
    if (totalPossibleConnections > 0.0f) {
        currentDensity = totalConnections / totalPossibleConnections;
    }
    
    // Synaptogenesis only occurs if below target density
    if (currentDensity >= pImpl->targetDensity) {
        return; // Target density reached
    }
    
    // Determine how many new synapses to form
    size_t neuronsToProcess = std::min(static_cast<size_t>(brain->getNeuronCount() * 0.01f), 
                                     regions.front().getNeurons().size());
    
    if (neuronsToProcess == 0) return;
    
    // Select random neurons for synaptogenesis
    std::vector<const Neuron*> selectedNeurons;
    selectedNeurons.reserve(neuronsToProcess);
    
    // Simple reservoir sampling approach
    for (size_t i = 0; i < neuronsToProcess; ++i) {
        size_t randomIndex = rng.nextInt(static_cast<int>(regions.front().getNeurons().size()));
        selectedNeurons.push_back(regions.front().getNeurons()[randomIndex]);
    }
    
    // Form new synapses
    for (const Neuron* neuron : selectedNeurons) {
        // Determine target region for new synapse
        if (regions.size() > 1) {
            size_t targetRegionIndex = rng.nextInt(static_cast<int>(regions.size()));
            const auto& targetRegion = regions[targetRegionIndex];
            
            // Select random target neuron (different from source)
            const auto& targetNeurons = targetRegion.getNeurons();
            if (targetNeurons.size() <= 1) continue;
            
            size_t targetIndex = rng.nextInt(static_cast<int>(targetNeurons.size()));
            const Neuron* targetNeuron = targetNeurons[targetIndex];
            
            // Don't create self-connections
            if (neuron == targetNeuron) continue;
            
            // Create new synapse with biologically realistic properties
            float initialWeight = 0.01f + rng.nextFloat() * 0.02f; // Small initial weight
            Timestamp formationTime = brain->getCurrentTime();
            
            // Create the synapse
            brain->createSynapse(neuron, targetNeuron, initialWeight, formationTime);
        }
    }
}

} // namespace nlm
