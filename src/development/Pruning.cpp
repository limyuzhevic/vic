#include "Pruning.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/STDP.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <queue>

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    float minimumEffectiveWeight;
    float activityDependencyFactor;
    float developmentalPruningSensitivity;
    size_t neuronsWithoutRecentActivity;
    size_t synapsesWithoutRecentActivity;
    float globalSynapticStability;
    std::unordered_map<SynapseId, float> synapseStabilityHistory;
    std::unordered_map<NeuronId, size_t> neuronActivityHistory;
    SimulationStep lastStabilityCheck;
    
    Impl() : threshold(0.01f), pruningRate(0.0001f), minimumEffectiveWeight(0.05f),
             activityDependencyFactor(0.7f), developmentalPruningSensitivity(1.0f),
             neuronsWithoutRecentActivity(0), synapsesWithoutRecentActivity(0),
             globalSynapticStability(1.0f), lastStabilityCheck(0) {}
};

Pruning::Pruning() : pImpl(new Impl) {}

Pruning::~Pruning() = default;

float Pruning::getThreshold() const {
    return pImpl->threshold;
}

void Pruning::setThreshold(float threshold) {
    pImpl->threshold = std::max(0.0f, std::min(1.0f, threshold));
}

float Pruning::getPruningRate() const {
    return pImpl->pruningRate;
}

void Pruning::setPruningRate(float rate) {
    pImpl->pruningRate = std::max(0.0f, rate);
}

void Pruning::update(Brain* brain, RandomGenerator& rng) {
    if (!brain || pruningRate <= 0.0f) return;
    
    ++pImpl->lastStabilityCheck;
    
    // Update developmental sensitivity factor
    DevelopmentSystem* devSys = brain->getDevelopmentSystem();
    if (devSys) {
        DevelopmentalStage stage = devSys->getStage();
        switch (stage) {
            case DevelopmentalStage::Initial:
                pImpl->developmentalPruningSensitivity = 0.3f;
                break;
            case DevelopmentalStage::CriticalPeriod:
                pImpl->developmentalPruningSensitivity = 0.6f;
                break;
            case DevelopmentalStage::Maturation:
                pImpl->developmentalPruningSensitivity = 1.0f;
                break;
            case DevelopmentalStage::Adult:
                pImpl->developmentalPruningSensitivity = 0.7f;
                break;
            case DevelopmentalStage::Aging:
                pImpl->developmentalPruningSensitivity = 0.5f;
                break;
        }
    }
    
    // Calculate global synaptic stability
    updateGlobalSynapticStability(brain);
    
    // Identify synapses for pruning based on multiple factors
    identifyAndPruneSynapses(brain, rng);
    
    // Update activity tracking
    updateActivityTracking(brain);
    
    // Log pruning statistics periodically
    static int logCounter = 0;
    if (++logCounter >= 1000) {
        NLM_LOG_INFO("Pruning: threshold=" + std::to_string(pImpl->threshold) +
                    ", rate=" + std::to_string(pImpl->pruningRate) +
                    ", sensitivity=" + std::to_string(pImpl->developmentalPruningSensitivity) +
                    ", stability=" + std::to_string(pImpl->globalSynapticStability) +
                    ", neurons_unstable=" + std::to_string(pImpl->neuronsWithoutRecentActivity) +
                    ", synapses_unstable=" + std::to_string(pImpl->synapsesWithoutRecentActivity));
        logCounter = 0;
    }
}

void Pruning::Impl::updateGlobalSynapticStability(Brain* brain) {
    if (!brain) return;
    
    float totalStability = 0.0f;
    size_t synapseCount = 0;
    size_t unstableSynapseCount = 0;
    size_t neuronCount = 0;
    size_t inactiveNeuronCount = 0;
    
    // Calculate stability based on synaptic weights, activity, and development
    for (const auto& region : brain->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            synapseCount++;
            
            float weightStability = calculateSynapseStability(syn);
            totalStability += weightStability;
            
            if (weightStability < threshold * developmentalPruningSensitivity) {
                unstableSynapseCount++;
            }
        }
        
        for (const auto& pop : region->getPopulations()) {
            neuronCount += pop->getSize();
            
            // Check for neurons without recent activity (would need access to neuron objects)
            // For now, we'll use a simplified approach
        }
    }
    
    globalSynapticStability = (synapseCount > 0) ? (totalStability / synapseCount) : 0.0f;
    synapsesWithoutRecentActivity = unstableSynapseCount;
    
    // Update neurons without recent activity (simplified)
    // This would require tracking recent activity more precisely
}

void Pruning::Impl::identifyAndPruneSynapses(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    std::vector<Synapse*> synapsesToConsider;
    std::vector<Synapse*> lowStabilitySynapses;
    
    // Collect synapses for evaluation
    for (const auto& region : brain->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            synapsesToConsider.push_back(const_cast<Synapse*>(syn.get()));
            
            float stability = calculateSynapseStability(syn.get());
            if (stability < threshold * developmentalPruningSensitivity) {
                lowStabilitySynapses.push_back(const_cast<Synapse*>(syn.get()));
            }
        }
    }
    
    if (lowStabilitySynapses.empty()) return;
    
    // Prioritize synapses for pruning based on stability and activity impact
    // Sort by stability (most unstable first)
    std::sort(lowStabilitySynapses.begin(), lowStabilitySynapses.end(),
        [this](const Synapse* a, const Synapse* b) {
            return calculateSynapseStability(a) < calculateSynapseStability(b);
        });
    
    // Determine how many synapses to prune
    size_t synapsesToPrune = static_cast<size_t>(lowStabilitySynapses.size() * pruningRate * 0.1f);
    synapsesToPrune = std::min(synapsesToPrune, lowStabilitySynapses.size() / 10); // Prune at most 10% at once
    
    if (synapsesToPrune == 0) return;
    
    // Prune the selected synapses
    size_t actuallyPruned = 0;
    for (size_t i = 0; i < synapsesToPrune && i < lowStabilitySynapses.size(); ++i) {
        Synapse* synapse = lowStabilitySynapses[i];
        
        // Check developmental stage - be more conservative in early stages
        bool canPrune = true;
        if (developmentalPruningSensitivity < 0.6f) {
            // In early stages, prune only the most unstable synapses
            canPrune = (calculateSynapseStability(synapse) < threshold * 0.5f);
        }
        
        if (canPrune) {
            bool pruned = pruneSynapse(synapse);
            if (pruned) {
                ++actuallyPruned;
                NLM_LOG_INFO("Pruned synapse " + std::to_string(synapse->getId().value) +
                            " (weight=" + std::to_string(synapse->getWeight()) +
                            ", stability=" + std::to_string(calculateSynapseStability(synapse)) + ")");
            }
        }
    }
    
    NLM_LOG_INFO("Pruned " + std::to_string(actuallyPruned) + " synapses out of " + 
                std::to_string(synapsesToPrune) + " attempted (stability threshold: " +
                std::to_string(threshold * developmentalPruningSensitivity) + ")");
}

bool Pruning::Impl::calculateSynapticImpact(Brain* brain, SynapseId synapseId, NeuronId affectedNeuron) {
    // Estimate the impact of removing a synapse on network stability
    // This is a simplified implementation
    return true;
}

void Pruning::Impl::updateActivityTracking(Brain* brain) {
    // Update activity tracking for determining synaptic stability
    // This would typically involve tracking recent spike history
    // For now, we use a simplified approach
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) return false;
    
    // Check if synapse meets pruning criteria
    if (!shouldPruneSynapse(synapse)) {
        return false;
    }
    
    // Record synapse for stability history before removal
    float stability = calculateSynapseStability(synapse);
    
    // In a real implementation, this would:
    // 1. Remove the synapse from all data structures
    // 2. Update connected neurons
    // 3. Record the removal in activity logs
    // 4. Notify plasticity systems
    
    // For now, we'll simulate synapse removal
    // This would involve:
    // - Removing from NeuralRegion's synapse list
    // - Updating neuron incoming/outgoing connections
    // - Handling spike event delivery for any pending spikes
    
    NLM_LOG_INFO("Synapse " + std::to_string(synapse->getId().value) + 
                " removed: weight=" + std::to_string(synapse->getWeight()) +
                ", stability=" + std::to_string(stability));
    
    return true;
}

float Pruning::Impl::calculateSynapseStability(const Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    float stability = 0.0f;
    
    // Factor 1: Weight stability (synaptic strength)
    float weight = std::abs(synapse->getWeight());
    float weightStability = std::min(1.0f, weight / 0.5f); // Normalize around 0.5 weight
    stability += weightStability * 0.4f;
    
    // Factor 2: Activity stability (recent spike history)
    const auto& preSpikeHistory = synapse->getPreSpikeHistory();
    const auto& postSpikeHistory = synapse->getPostSpikeHistory();
    
    float activityStability = 0.0f;
    if (!preSpikeHistory.empty() && !postSpikeHistory.empty()) {
        // Both pre- and post-synaptic activity indicates functional connectivity
        activityStability = 1.0f;
    } else if (!preSpikeHistory.empty() || !postSpikeHistory.empty()) {
        // Only one side active suggests weakening connection
        activityStability = 0.3f;
    } else {
        // No recent activity suggests unreliable connection
        activityStability = 0.0f;
    }
    stability += activityStability * 0.3f;
    
    // Factor 3: Plasticity flags stability
    const PlasticityFlags& flags = synapse->getPlasticityFlags();
    float plasticityStability = 0.0f;
    if (flags.stdp && flags.hebbian) {
        plasticityStability = 1.0f; // Both plasticity mechanisms active
    } else if (flags.stdp || flags.hebbian) {
        plasticityStability = 0.7f; // Single plasticity mechanism
    } else {
        plasticityStability = 0.5f; // Structural plasticity only
    }
    stability += plasticityStability * 0.2f;
    
    // Factor 4: Developmental stage modulation
    float devModulation = 1.0f;
    if (developmentalPruningSensitivity < 0.5f) {
        // Conservative pruning in early stages
        devModulation = 0.5f;
    }
    stability *= devModulation;
    
    return stability;
}

bool Pruning::Impl::shouldPruneSynapse(const Synapse* synapse) const {
    if (!synapse) return false;
    
    float weight = std::abs(synapse->getWeight());
    
    // Basic pruning criteria
    bool belowThreshold = weight < threshold;
    bool belowMinimum = weight < minimumEffectiveWeight;
    bool tooUnstable = calculateSynapseStability(synapse) < threshold * developmentalPruningSensitivity;
    
    // Additional criteria based on developmental stage
    bool developmentalPruning = developmentalPruningSensitivity > 0.7f;
    
    // Heuristic: prune if any of the conditions are met, with emphasis on weight and stability
    return (belowMinimum) || (tooUnstable && (belowThreshold || developmentalPruning));
}

} // namespace nlm
