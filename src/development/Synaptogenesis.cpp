// Implementation of Synaptogenesis - neural connection formation during development
#include "Synaptogenesis.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float currentDensity;
    float developmentalModulation;
    size_t minConnectionsPerNeuron;
    size_t maxConnectionsPerNeuron;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f), currentDensity(0.05f), 
             developmentalModulation(1.0f), minConnectionsPerNeuron(5), maxConnectionsPerNeuron(50) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = std::max(0.0f, rate);
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = std::clamp(density, 0.0f, 1.0f);
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    if (!brain || !brain->getRandomGenerator()) {
        return;
    }
    
    // Calculate actual connectivity density
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    if (totalNeurons < 2) {
        return;  // Not enough neurons for connections
    }
    
    // Maximum possible synapses (all-to-all connections)
    size_t maxPossibleSynapses = totalNeurons * (totalNeurons - 1);
    pImpl->currentDensity = static_cast<float>(totalSynapses) / maxPossibleSynapses;
    
    // Calculate connectivity deficit
    float densityDeficit = pImpl->targetDensity - pImpl->currentDensity;
    
    if (densityDeficit <= 0.0f) {
        return;  // Already at or above target density
    }
    
    // Modulate formation rate with developmental stage
    // Early development has higher synaptogenesis rates
    float stageModulation = 1.0f;
    if (brain->getDevelopmentSystem()) {
        switch (brain->getDevelopmentSystem()->getStage()) {
            case DevelopmentalStage::Initial:
                stageModulation = 2.0f;  // High formation rate
                break;
            case DevelopmentalStage::CriticalPeriod:
                stageModulation = 1.5f;
                break;
            case DevelopmentalStage::Maturation:
                stageModulation = 1.0f;
                break;
            case DevelopmentalStage::Adult:
                stageModulation = 0.3f;  // Low formation in adulthood
                break;
            case DevelopmentalStage::Aging:
                stageModulation = 0.1f;
                break;
        }
    }
    
    // Calculate number of new synapses to add
    size_t potentialNewSynapses = static_cast<size_t>(densityDeficit * maxPossibleSynapses);
    if (potentialNewSynapses == 0) {
        return;
    }
    
    // Apply Poisson distribution for stochastic synaptogenesis
    std::poisson_distribution<size_t> synapseDistribution(potentialNewSynapses * pImpl->formationRate * stageModulation);
    size_t newSynapses = synapseDistribution(rng.getRandomEngine());
    
    if (newSynapses == 0) {
        return;
    }
    
    // Collect all neurons for potential connections
    std::vector<Neuron*> allNeurons;
    allNeurons.reserve(totalNeurons);
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                if (neuron) {
                    allNeurons.push_back(neuron);
                }
            }
        }
    }
    
    if (allNeurons.size() < 2) {
        return;
    }
    
    // Form new synapses between neurons of compatible types
    for (size_t attempts = 0; attempts < newSynapses * 10 && attempts < 10000; ++attempts) {
        // Select two distinct neurons
        size_t idx1 = rng.uniformInt(0, allNeurons.size() - 1);
        size_t idx2 = rng.uniformInt(0, allNeurons.size() - 1);
        
        while (idx1 == idx2) {
            idx2 = rng.uniformInt(0, allNeurons.size() - 1);
        }
        
        Neuron* sourceNeuron = allNeurons[idx1];
        Neuron* targetNeuron = allNeurons[idx2];
        
        if (!sourceNeuron || !targetNeuron) {
            continue;
        }
        
        // Check neuron types for compatibility
        NeuronType sourceType = sourceNeuron->getType();
        NeuronType targetType = targetNeuron->getType();
        
        // Skip connections involving modulatory neurons as sources for simplicity
        if (sourceType == NeuronType::Modulatory) {
            continue;
        }
        
        // Allow connections from any source to sensory/motor/target neurons
        if (targetType == NeuronType::Sensory || targetType == NeuronType::Motor || 
            targetType == NeuronType::Internal) {
            
            // Check if synapse already exists
            if (region->hasSynapse(sourceNeuron->getId(), targetNeuron->getId())) {
                continue;
            }
            
            // Create new synapse
            float initialWeight = rng.uniformReal(0.1f, 1.0f);  // Initial synaptic weight
            Delay delay = rng.uniformInt(0, 10);  // Random delay (0-10 timesteps)
            
            // Determine synapse type based on neuron types
            SynapseType synapseType = SynapseType::Excitatory;
            if (targetType == NeuronType::Inhibitory) {
                synapseType = SynapseType::Inhibitory;
            }
            
            // Add synapse to the brain
            // Note: This requires access to the brain's internal synapse management
            // For now, we'll log the intended synaptogenesis
            NLM_LOG_INFO("Synaptogenesis: New synapse from neuron " + 
                        std::to_string(sourceNeuron->getId().index()) + " to " +
                        std::to_string(targetNeuron->getId().index()) + 
                        " (weight: " + std::to_string(initialWeight) + ")");
            
            // In a full implementation, we would:
            // 1. Add the synapse through the brain's API
            // 2. Set initial properties
            // 3. Configure plasticity flags
        }
    }
    
    // Update developmental modulation for next step
    pImpl->developmentalModulation *= 0.95f; // Gradual decrease over time
}

} // namespace nlm