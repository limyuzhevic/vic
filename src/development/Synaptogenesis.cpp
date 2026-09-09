#include "Synaptogenesis.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../development/DevelopmentSystem.hpp"
#include "../../plasticity/StructuralPlasticity.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float activityThreshold;  // Minimum activity for synaptogenesis
    float correlationThreshold;  // Minimum correlation for new connections
    size_t minSynapsesForCorrelation;  // Minimum synapses needed to compute correlation
    
    // Statistics
    size_t totalSynapsesCreated;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f), 
             activityThreshold(0.1f), correlationThreshold(0.3f),
             minSynapsesForCorrelation(5), totalSynapsesCreated(0) {}
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
    if (!brain) return;
    
    // Get developmental stage and plasticity modifier
    DevelopmentSystem* devSystem = brain->getDevelopmentSystem();
    if (!devSystem) return;
    
    DevelopmentalStage stage = devSystem->getStage();
    float plasticityModifier = devSystem->getPlasticityModifier();
    bool isCriticalPeriod = devSystem->isCriticalPeriod();
    float criticalPeriodProgress = devSystem->getCriticalPeriodProgress();
    
    // Adjust formation rate based on developmental stage and critical period
    float effectiveRate = pImpl->formationRate * plasticityModifier;
    
    // Enhanced formation during critical period
    if (isCriticalPeriod) {
        float criticalPeriodFactor = 1.0f + criticalPeriodProgress;  // 1.0 to 2.0
        effectiveRate *= criticalPeriodFactor;
    }
    
    // Enhanced formation during early stages
    if (stage == DevelopmentalStage::Initial || stage == DevelopmentalStage::CriticalPeriod) {
        effectiveRate *= 2.0f;
    }
    
    // Process each neural region
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        
        if (neuronCount < 2) continue;
        
        // Calculate current synaptic density
        size_t currentSynapseCount = region->getSynapseCount();
        size_t maxPossibleSynapses = neuronCount * (neuronCount - 1) / 2;  // Undirected connections
        
        if (maxPossibleSynapses == 0) continue;
        
        float currentDensity = static_cast<float>(currentSynapseCount) / static_cast<float>(maxPossibleSynapses);
        
        // Skip if we've reached or exceeded target density
        if (currentDensity >= pImpl->targetDensity) {
            continue;
        }
        
        // Collect neuron activity statistics
        struct NeuronActivity {
            NeuronId id;
            float activity;      // Normalized spike count
            float avgWeight;     // Average incoming weight
            float variance;      // Variance in membrane potential
        };
        
        std::vector<NeuronActivity> activities;
        activities.reserve(neuronCount);
        
        for (auto* neuron : neurons) {
            float activity = 0.0f;
            float avgWeight = 0.0f;
            float variance = 0.0f;
            size_t synCount = 0;
            
            // Activity based on spike history
            const auto& spikes = neuron->getSpikeHistory();
            if (!spikes.empty()) {
                // Normalize by time window (600 time units = 10 minutes)
                // More recent spikes contribute more
                activity = static_cast<float>(spikes.size()) / 10.0f;  // Max 10 Hz average
                if (activity > 1.0f) activity = 1.0f;
            }
            
            // Calculate average incoming weight
            auto incoming = region->getSynapsesTo(neuron->getId());
            for (auto* syn : incoming) {
                avgWeight += std::abs(syn->getWeight());
                ++synCount;
            }
            
            if (synCount > 0) {
                avgWeight /= static_cast<float>(synCount);
                
                // Calculate variance in membrane potential for diversity
                const auto& state = neuron->getState();
                variance = std::abs(state.membranePotential - state.restingPotential);
            }
            
            activities.push_back({neuron->getId(), activity, avgWeight, variance});
        }
        
        // Determine how many new synapses to create based on remaining capacity
        size_t maxNewSynapses = maxPossibleSynapses - currentSynapseCount;
        size_t targetNewSynapses = static_cast<size_t>(maxNewSynapses * pImpl->targetDensity);
        if (targetNewSynapses == 0) targetNewSynapses = 1;
        
        // Ensure we don't exceed the estimated number of neurons we can process
        targetNewSynapses = std::min(targetNewSynapses, neuronCount * 2);
        
        // Try to create new synapses
        for (size_t attempt = 0; attempt < targetNewSynapses; ++attempt) {
            // Probabilistic synapse formation based on effective rate
            if (!rng.bernoulli(effectiveRate * 0.1f)) continue;
            
            // Select source and destination neurons based on activity and correlation
            size_t idx1 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
            size_t idx2 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
            
            if (idx1 == idx2) continue;  // Same neuron
            
            const NeuronActivity& act1 = activities[idx1];
            const NeuronActivity& act2 = activities[idx2];
            
            // Check activity threshold
            if (act1.activity < pImpl->activityThreshold || 
                act2.activity < pImpl->activityThreshold) {
                continue;
            }
            
            // Compute correlation-like score based on complementary activity patterns
            float activityScore = 0.0f;
            
            // Higher score for complementary firing patterns (not too similar, not too different)
            float activityDiff = std::abs(act1.activity - act2.activity);
            float weightDiff = std::abs(act1.avgWeight - act2.avgWeight);
            float potentialScore = (1.0f - activityDiff) + (1.0f - weightDiff / 1.0f);
            activityScore = potentialScore / 2.0f;
            
            // Also consider diversity in membrane potential variance
            float varianceDiff = std::abs(act1.variance - act2.variance);
            activityScore += (1.0f - varianceDiff / 1.0f) * 0.5f;
            
            // Normalize to 0-1
            activityScore = std::min(1.0f, activityScore);
            
            // Check correlation threshold
            if (activityScore < pImpl->correlationThreshold) {
                continue;
            }
            
            // Apply developmental stage-specific modifications
            float stageModifier = 1.0f;
            if (stage == DevelopmentalStage::Initial) {
                stageModifier = 2.0f;  // Higher formation rate early
            } else if (stage == DevelopmentalStage::CriticalPeriod && criticalPeriodProgress < 0.5f) {
                stageModifier = 1.5f;  // Enhanced early critical period
            }
            
            activityScore *= stageModifier;
            
            // Final probability check based on activity score
            if (rng.bernoulli(effectiveRate * activityScore * 0.5f)) {
                // Create synapse with appropriate initial weight
                float weight = computeInitialWeight(act1, act2, rng);
                
                // Use StructuralPlasticity to create the synapse
                SynapseId synId = brain->getStructuralPlasticity()->createSynapse(
                    brain, act1.id, act2.id, weight
                );
                
                if (synId != INVALID_SYNAPSE_ID) {
                    ++pImpl->totalSynapsesCreated;
                }
            }
        }
    }
}

float Synaptogenesis::Impl::computeInitialWeight(const NeuronActivity& act1, 
                                               const NeuronActivity& act2, 
                                               RandomGenerator& rng) const {
    // Determine synapse type based on neuron types and activities
    NeuronType type1 = act1.id.value < 100 ? NeuronType::Excitatory : NeuronType::Inhibitory;
    NeuronType type2 = act2.id.value < 100 ? NeuronType::Excitatory : NeuronType::Inhibitory;
    
    float baseWeight = 0.1f;
    
    // Adjust based on neuron types
    if ((type1 == NeuronType::Excitatory && type2 == NeuronType::Inhibitory) || 
        (type1 == NeuronType::Inhibitory && type2 == NeuronType::Excitatory)) {
        baseWeight = 0.05f;  // Weaker cross-type connections
    }
    
    // Adjust based on activity levels
    float activityFactor = (act1.activity + act2.activity) / 2.0f;
    if (activityFactor > 0.5f) {
        baseWeight *= 1.2f;  // Stronger connections for active neurons
    }
    
    // Add developmental stage-specific weight modulation
    baseWeight *= (1.0f + rng.uniformReal(-0.2f, 0.2f));  // ±20% variance
    
    // Clamp to reasonable range
    return std::clamp(baseWeight, 0.01f, 0.5f);
}

} // namespace nlm