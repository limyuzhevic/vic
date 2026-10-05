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
    if (!brain) return;
    
    // Get developmental stage for rate modulation
    float plasticityMod = 1.0f;
    if (brain->getDevelopmentSystem()) {
        plasticityMod = brain->getDevelopmentSystem()->getPlasticityModifier();
    }
    
    float actualPruningRate = pruningRate * plasticityMod;
    
    /*
     * Implement real pruning:
     * 
     * 1. Weak or unused synapses are removed probabilistically
     * 2. Pruning rate is developmentally modulated (higher in some stages)
     * 3. Activity-dependent pruning (Hebbian-like removal of unused connections)
     * 4. Integration with neuromodulatory systems (dopamine, novelty)
     * 5. Consideration of metabolic constraints and wiring efficiency
     */
    
    // Collect synapse statistics and activity data
    struct SynapseStats {
        SynapseId id;
        NeuronId source;
        NeuronId destination;
        float weight;
        float eligibilityTrace;
        float activity;
        float age;
        NeuronType sourceType;
        NeuronType destType;
        bool isRecent;
    };
    
    std::vector<SynapseStats> synapseStats;
    
    // Scan all regions and collect synapse data
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        
        for (auto& syn : region->getSynapses()) {
            SynapseStats stats;
            stats.id = syn->getId();
            stats.source = syn->getSourceNeuron();
            stats.destination = syn->getDestinationNeuron();
            stats.weight = std::abs(syn->getWeight());
            stats.eligibilityTrace = std::abs(syn->getEligibilityTrace());
            stats.age = currentStep - static_cast<SimulationStep>(syn->getCreationTime());
            
            // Find source and destination neuron types
            for (auto* neuron : neurons) {
                if (neuron->getId() == stats.source) {
                    // Find population type
                    for (auto& pop : region->getPopulations()) {
                        if (pop->hasNeuron(neuron)) {
                            stats.sourceType = pop->getNeuronType();
                            break;
                        }
                    }
                }
                if (neuron->getId() == stats.destination) {
                    for (auto& pop : region->getPopulations()) {
                        if (pop->hasNeuron(neuron)) {
                            stats.destType = pop->getNeuronType();
                            break;
                        }
                    }
                }
            }
            
            // Calculate activity based on eligibility trace and recent use
            stats.activity = stats.eligibilityTrace;
            stats.isRecent = stats.age < 100;  // Recent if created within last 100 steps
            
            synapseStats.push_back(stats);
        }
    }
    
    if (synapseStats.empty()) return;
    
    // Prune synapses based on multiple criteria
    
    // Phase 1: Activity-dependent pruning (Hebbian-like)
    // Unused or weakly active synapses are preferentially pruned
    std::vector<bool> shouldPrune(synapseStats.size(), false);
    
    for (size_t i = 0; i < synapseStats.size(); ++i) {
        SynapseStats& stats = synapseStats[i];
        
        // Check basic pruning conditions
        bool weakSynapse = stats.weight < threshold;
        bool inactiveSynapse = stats.activity < activityThreshold;
        
        // Integration with neuromodulation systems
        bool neuromodModulation = true;
        if (brain->getDopamine()) {
            float dopamineLevel = brain->getDopamine()->getLevel();
            // High dopamine protects synapses from pruning (reward-based retention)
            if (dopamineLevel > 0.7f) {
                neuromodModulation = false;
            }
        }
        
        if (brain->getNovelty()) {
            float noveltyLevel = brain->getNovelty()->getLevel();
            // High novelty protects synapses (newly learned connections)
            if (noveltyLevel > 0.7f) {
                neuromodModulation = false;
            }
        }
        
        // Developmental stage-specific pruning patterns
        bool pruneDueToStage = false;
        if (brain->getDevelopmentSystem()) {
            DevelopmentalStage stage = brain->getDevelopmentSystem()->getStage();
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    // Initial burst - minimal pruning to allow exploration
                    pruneDueToStage = false;
                    break;
                    
                case DevelopmentalStage::CriticalPeriod:
                    // Critical period - selective refinement
                    if (weakSynapse && !stats.isRecent) {
                        pruneDueToStage = rng.bernoulli(actualPruningRate * 2.0f);  // Aggressive pruning
                    }
                    break;
                    
                case DevelopmentalStage::Maturation:
                    // Maturation - steady pruning for efficiency
                    if (weakSynapse && !stats.isRecent) {
                        pruneDueToStage = rng.bernoulli(actualPruningRate);
                    }
                    break;
                    
                case DevelopmentalStage::Adult:
                    // Adult - selective maintenance of strong connections
                    if (stats.weight < threshold * 2.0f && !stats.isRecent && stats.activity < activityThreshold * 0.5f) {
                        pruneDueToStage = rng.bernoulli(actualPruningRate * 0.7f);
                    }
                    break;
                    
                case DevelopmentalStage::Aging:
                    // Aging - targeted preservation of important connections
                    if (weakSynapse && !stats.isRecent && stats.activity < activityThreshold * 0.3f) {
                        pruneDueToStage = rng.bernoulli(actualPruningRate * 1.5f);  // Aggressive
                    }
                    break;
            }
        }
        
        // Spatial efficiency pruning
        bool spatialInefficient = false;
        if (stats.sourceType != stats.destType && stats.weight < threshold * 0.5f) {
            // Cross-region connections with weak weights are inefficient
            spatialInefficient = rng.bernoulli(actualPruningRate * 0.5f);
        }
        
        // Learning history pruning
        bool lowLearningValue = false;
        if (!stats.isRecent) {
            // Old synapses with low activity are low value
            if (stats.activity < activityThreshold * 0.2f && stats.age > 500) {
                lowLearningValue = rng.bernoulli(actualPruningRate * 0.8f);
            }
        }
        
        // Determine if synapse should be pruned
        if (!neuromodModulation) {
            // Protected by neuromodulation
            shouldPrune[i] = false;
        } else if (pruneDueToStage || weakSynapse || spatialInefficient || lowLearningValue || inactiveSynapse) {
            shouldPrune[i] = true;
        }
    }
    
    // Apply pruning
    size_t prunedCount = 0;
    for (size_t i = 0; i < synapseStats.size(); ++i) {
        if (shouldPrune[i] && brain->getPruning()) {
            bool success = brain->getPruning()->pruneSynapse(brain, synapseStats[i].id);
            if (success) {
                ++prunedCount;
            }
        }
    }
    
    // Log pruning statistics
    if (prunedCount > 0) {
        NLM_LOG_INFO("Development pruning: removed " + std::to_string(prunedCount) + " synapses");
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) return false;
    
    // TODO PHASE 2: Implement real synapse removal
    // Real synapse removal would involve:
    // 1. Finding and removing the synapse from the neural region
    // 2. Updating neuron connection statistics
    // 3. Recording the pruning event for metaplasticity
    // 4. Potentially triggering homeostatic mechanisms
    
    // For now, we mark the synapse for removal by zeroing its weight
    // Actual removal would require modifying the region's synapse storage
    syn->setWeight(0.0f);
    return true;
}

} // namespace nlm
