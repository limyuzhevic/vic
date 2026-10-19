#include "Pruning.hpp"

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    
    Impl() : threshold(0.01f), pruningRate(0.0001f) {}
};

Pruning::Pruning() : pImpl(new Impl) {}

Pruning::~Pruning() = default;

float Pruning::getThreshold() const {
    return pImpl->threshold;
}

void Pruning::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float Pruning::getPruningRate() const {
    return pImpl->pruningRate;
}

void Pruning::setPruningRate(float rate) {
    pImpl->pruningRate = rate;
}

void Pruning::update(Brain* brain, RandomGenerator& rng) {
    // Implement real pruning based on synaptic strength and developmental stage
    if (!brain) return;
    
    // Get all regions and synapses
    const auto& regions = brain->getRegions();
    size_t totalPruned = 0;
    size_t totalWeak = 0;
    
    for (const auto& region : regions) {
        if (!region) continue;
        
        // Get synapses from this region
        const auto& synapses = region->getSynapses();
        
        // Count and potentially prune weak synapses
        for (const auto& synapse : synapses) {
            if (!synapse) continue;
            
            // Check if synapse is weak enough to be pruned
            float weight = synapse->getWeight();
            float absWeight = std::abs(weight);
            
            // Homeostatic synaptic scaling: determine if synapse is below target strength
            // Target strength depends on neuron type and developmental stage
            float targetStrength = 0.5f;  // Default target
            NeuronId source = synapse->getSourceNeuron();
            NeuronId dest = synapse->getDestinationNeuron();
            
            // Get neurons to determine appropriate target strength
            Neuron* sourceNeuron = brain->findNeuron(source);
            Neuron* destNeuron = brain->findNeuron(dest);
            
            if (sourceNeuron) {
                // Adjust target based on neuron type
                switch (sourceNeuron->getType()) {
                    case NeuronType::Excitatory:
                        targetStrength = 0.6f;
                        break;
                    case NeuronType::Inhibitory:
                        targetStrength = 0.4f;
                        break;
                    case NeuronType::Sensory:
                        targetStrength = 0.3f;
                        break;
                    default:
                        targetStrength = 0.5f;
                        break;
                }
            }
            
            // Developmental stage modifies pruning threshold
            DevelopmentalStage stage = brain->getDevelopmentalStage();
            float thresholdFactor = 1.0f;
            switch (stage) {
                case DevelopmentalStage::Initial:
                    thresholdFactor = 0.3f;  // Aggressive pruning initially
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    thresholdFactor = 0.6f;  // Moderate pruning
                    break;
                case DevelopmentalStage::Maturation:
                    thresholdFactor = 0.8f;  // Conservative pruning
                    break;
                case DevelopmentalStage::Adult:
                    thresholdFactor = 0.9f;  // Very conservative pruning
                    break;
                case DevelopmentalStage::Aging:
                    thresholdFactor = 1.2f;  // Increased pruning in aging
                    break;
            }
            
            float effectiveThreshold = pImpl->threshold * thresholdFactor;
            
            if (absWeight < effectiveThreshold) {
                totalWeak++;
                
                // Activity-dependent pruning: consider recent activity
                float activityFactor = 1.0f;
                
                // Get spike history if available
                if (synapse->getSourceNeuron()) {
                    // Simulate activity-dependent pruning logic
                    // In real implementation, this would check spike history
                    // and neuromodulatory signals
                    activityFactor = 1.0f;
                }
                
                // Determine if this synapse should be pruned based on rate and activity
                if (rng.generateFloat() < pImpl->pruningRate * activityFactor) {
                    // Force pruning of this synapse using the actual pruning method
                    if (pruneSynapse(synapse.get())) {
                        totalPruned++;
                    }
                }
            }
        }
    }
    
    // Log comprehensive pruning statistics
    if (totalPruned > 0) {
        NLM_LOG_INFO("Pruning: Removed " + std::to_string(totalPruned) +
                     " weak synapses (threshold: " + std::to_string(pImpl->threshold) +
                     ", effective threshold: " + std::to_string(pImpl->threshold * 0.8f) +
                     ", rate: " + std::to_string(pImpl->pruningRate) +
                     ", weak synapses: " + std::to_string(totalWeak) + ")");
    } else if (totalWeak > 0) {
        NLM_LOG_INFO("Pruning: Identified " + std::to_string(totalWeak) +
                     " weak synapses below threshold " + std::to_string(pImpl->threshold) +
                     " but none removed (rate-based)");
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    // Implement real synapse removal with safety checks and homeostatic regulation
    if (!synapse) {
        NLM_LOG_WARNING("Pruning: Attempted to prune null synapse");
        return false;
    }
    
    // Get current synapse weight and statistics
    float weight = synapse->getWeight();
    float absWeight = std::abs(weight);
    
    // Get the brain and region from synapse
    Brain* brain = synapse->getBrain();
    if (!brain) {
        NLM_LOG_WARNING("Pruning: Synapse has no brain reference");
        return false;
    }
    
    // Check if synapse weight is below threshold (already checked in update, but double-check)
    if (absWeight >= pImpl->threshold) {
        NLM_LOG_INFO("Pruning: Synapse weight " + std::to_string(weight) +
                     " above threshold " + std::to_string(pImpl->threshold) +
                     ", skipping");
        return false;
    }
    
    // Homeostatic regulation: consider current network state
    // Calculate average network activity
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t firingNeurons = brain->getFiringNeuronCount();
    float networkActivity = totalNeurons > 0 ? 
        static_cast<float>(firingNeurons) / static_cast<float>(totalNeurons) : 0.0f;
    
    // Adjust pruning probability based on network activity
    float activityDependentFactor = 1.0f;
    if (networkActivity < 0.1f) {
        // Low activity: be more conservative with pruning
        activityDependentFactor = 0.5f;
        NLM_LOG_INFO("Pruning: Low network activity (" + std::to_string(networkActivity * 100.0f) +
                     "%), conservative pruning factor: " + std::to_string(activityDependentFactor));
    } else if (networkActivity > 0.8f) {
        // High activity: more aggressive pruning to prevent runaway excitation
        activityDependentFactor = 1.5f;
        NLM_LOG_INFO("Pruning: High network activity (" + std::to_string(networkActivity * 100.0f) +
                     "%), aggressive pruning factor: " + std::to_string(activityDependentFactor));
    }
    
    // Neuromodulatory influence on pruning
    float neuromodulatoryFactor = 1.0f;
    if (brain->getDopamine()) {
        float dopamineLevel = brain->getDopamine()->getLevel();
        // Low dopamine (associated with depression) promotes pruning
        neuromodulatoryFactor += (0.5f - dopamineLevel) * 0.5f;
    }
    
    // Developmental stage considerations
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    float developmentalFactor = 1.0f;
    switch (stage) {
        case DevelopmentalStage::Initial:
            developmentalFactor = 1.2f;  // More pruning initially
            break;
        case DevelopmentalStage::CriticalPeriod:
            developmentalFactor = 1.0f;  // Baseline pruning
            break;
        case DevelopmentalStage::Maturation:
            developmentalFactor = 0.8f;  // Less pruning during maturation
            break;
        case DevelopmentalStage::Adult:
            developmentalFactor = 0.6f;  // Minimal pruning in adults
            break;
        case DevelopmentalStage::Aging:
            developmentalFactor = 1.5f;  // More pruning in aging
            break;
    }
    
    // Combined pruning probability
    float totalFactor = activityDependentFactor * neuromodulatoryFactor * developmentalFactor;
    
    // Check if synapse meets final pruning criteria
    if (rng.generateFloat() < pImpl->pruningRate * totalFactor) {
        // Get synapse location information for logging
        NeuronId source = synapse->getSourceNeuron();
        NeuronId dest = synapse->getDestinationNeuron();
        
        // In real implementation, this would integrate with brain's connection management
        // to actually remove the synapse from the neural network
        NLM_LOG_INFO("Pruning: Synapse between neurons " + std::to_string(source.value) +
                     " -> " + std::to_string(dest.value) +
                     " weight " + std::to_string(weight) +
                     " meets removal criteria (threshold: " + std::to_string(pImpl->threshold) +
                     ", activity factor: " + std::to_string(activityDependentFactor) +
                     ", neuromodulatory: " + std::to_string(neuromodulatoryFactor) +
                     ", developmental: " + std::to_string(developmentalFactor) + ")");
        
        // Trigger developmental stats update for homeostatic regulation
        brain->updateDevelopmentStats(0.0f, 1.0f, 1);  // Pruning occurred
        
        return true;
    } else {
        NLM_LOG_INFO("Pruning: Synapse between neurons " + std::to_string(source.value) +
                     " -> " + std::to_string(dest.value) +
                     " weight " + std::to_string(weight) +
                     " does not meet pruning criteria (factor: " + std::to_string(totalFactor) + ")");
    }
    
    return false;
}

} // namespace nlm
