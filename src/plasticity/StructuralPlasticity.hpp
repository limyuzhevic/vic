#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <random>
#include <cmath>
#include <limits>

namespace nlm {

// Structural plasticity parameters based on biological models
struct StructuralPlasticityParameters {
    // Synaptogenesis parameters
    float synaptogenesisRate;          // Base rate of synapse creation per timestep
    float activityThreshold;           // Minimum activity level for new synapses
    float connectivityTarget;          // Target connectivity density (0-1)
    
    // Pruning parameters
    float pruningRate;                 // Base rate of synapse removal per timestep
    float strengthThreshold;            // Minimum synaptic strength for survival
    float redundancyThreshold;          // Threshold for eliminating redundant connections
    float competitionFactor;            // Strength of competition for resources
    
    // Neuron creation parameters
    float neuronCreationRate;           // Rate of new neuron addition
    float neuronMaturationTime;         // Time required for new neurons to mature
    float neurotrophicFactor;           // Growth factor supporting neuron survival
    
    // Pruning parameters
    float neuronPruningRate;            // Rate of neuron removal
    float neuronActivityThreshold;      // Minimum activity for neuron survival
    float metabolicCost;                // Metabolic cost of maintaining neurons
    
    // Development parameters
    float developmentalStage;           // Current developmental stage (0-1)
    float plasticityModulation;         // Effect of development on structural plasticity
    float activityDependentFactor;      // Weight of activity-dependent structural changes
    
    // Network properties
    float minDistance;                  // Minimum distance for new connections
    float maxDistance;                  // Maximum distance for new connections
    float connectionCost;               // Cost of creating long-range connections
    
    StructuralPlasticityParameters() 
        : synaptogenesisRate(0.0001f), activityThreshold(0.1f), connectivityTarget(0.15f),
          pruningRate(0.00001f), strengthThreshold(0.05f), redundancyThreshold(0.1f),
          competitionFactor(0.2f), neuronCreationRate(0.00001f), neuronMaturationTime(1000.0f),
          neurotrophicFactor(1.0f), neuronPruningRate(0.000001f), neuronActivityThreshold(0.01f),
          metabolicCost(0.001f), developmentalStage(0.5f), plasticityModulation(1.0f),
          activityDependentFactor(0.7f), minDistance(1.0f), maxDistance(50.0f),
          connectionCost(0.5f) {}
};

// Synapse creation events
struct SynapseCreationEvent {
    SynapseId id;
    NeuronId source;
    NeuronId target;
    SynapticWeight weight;
    float creationTime;
    float efficacy;
    bool mature;  // Whether the synapse has reached functional maturity
    
    SynapseCreationEvent(SynapseId synId, NeuronId src, NeuronId tgt, 
                        float weight, float time)
        : id(synId), source(src), target(tgt), weight(weight), 
          creationTime(time), efficacy(1.0f), mature(false) {}
};

// Pruning events
struct PruningEvent {
    SynapseId synapseId;
    float reason;  // 0 = weak connection, 1 = redundant, 2 = competition
    float weightChange;
    float time;
    
    PruningEvent(SynapseId synId, float r, float wc, float t)
        : synapseId(synId), reason(r), weightChange(wc), time(t) {}
};

// Neuron creation events
struct NeuronCreationEvent {
    NeuronId id;
    NeuronType type;
    RegionId region;
    float creationTime;
    float maturationTime;
    bool mature;
    float initialExcitability;
    
    NeuronCreationEvent(NeuronId nid, NeuronType t, RegionId r, float time, float initExcitability)
        : id(nid), type(t), region(r), creationTime(time), maturationTime(0.0f),
          mature(false), initialExcitability(initExcitability) {}
};

// Enhanced Structural Plasticity implementation
class EnhancedStructuralPlasticity {
public:
    EnhancedStructuralPlasticity() : pImpl(new Impl()) {
        pImpl->parameters = StructuralPlasticityParameters();
        pImpl->nextSynapseId = 1;
        pImpl->nextNeuronId = 1;
        pImpl->timeSinceLastScaling = 0.0f;
        pImpl->globalExcitatoryRatio = 0.5f;
        pImpl->totalSynapseCount = 0;
        pImpl->totalNeuronCount = 0;
    }
    
    ~EnhancedStructuralPlasticity() {
        delete pImpl;
    }
    
    // Disable copying
    EnhancedStructuralPlasticity(const EnhancedStructuralPlasticity&) = delete;
    EnhancedStructuralPlasticity& operator=(const EnhancedStructuralPlasticity&) = delete;
    
    // Enable moving
    EnhancedStructuralPlasticity(EnhancedStructuralPlasticity&& other) noexcept : pImpl(other.pImpl) {
        other.pImpl = nullptr;
    }
    
    EnhancedStructuralPlasticity& operator=(EnhancedStructuralPlasticity&& other) noexcept {
        if (this != &other) {
            delete pImpl;
            pImpl = other.pImpl;
            other.pImpl = nullptr;
        }
        return *this;
    }
    
    // Synapse creation
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight) {
        if (!brain) return INVALID_SYNAPSE_ID;
        
        SynapseId newId = pImpl->nextSynapseId++;
        float currentTime = pImpl->simulationTime;
        
        // Check if neurons exist
        if (!brain->hasNeuron(source) || !brain->hasNeuron(destination)) {
            return INVALID_SYNAPSE_ID;
        }
        
        // Check distance constraints
        float distance = computeDistance(brain, source, destination);
        if (distance > pImpl->parameters.maxDistance) {
            // Penalize long-range connections
            weight *= (1.0f - pImpl->parameters.connectionCost);
        }
        
        // Create synapse
        SynapseCreationEvent event(newId, source, destination, weight, currentTime);
        pImpl->pendingCreations.push_back(event);
        pImpl->totalSynapseCount++;
        
        return newId;
    }
    
    // Synapse removal
    bool removeSynapse(Brain* brain, SynapseId synapse) {
        if (!brain) return false;
        
        PruningEvent event(synapse, 0.0f, -1.0f, pImpl->simulationTime);
        pImpl->pendingRemovals.push_back(event);
        pImpl->totalSynapseCount--;
        
        return true;
    }
    
    // Neuron creation
    NeuronId createNeuron(Brain* brain, NeuronType type) {
        if (!brain) return INVALID_NEURON_ID;
        
        NeuronId newId = pImpl->nextNeuronId++;
        RegionId region = getOptimalRegionForNewNeuron(brain, type);
        
        // Determine initial excitability based on neuron type
        float initialExcitability = computeInitialExcitability(type);
        
        NeuronCreationEvent event(newId, type, region, pImpl->simulationTime, initialExcitability);
        pImpl->pendingNeuronCreations.push_back(event);
        pImpl->totalNeuronCount++;
        
        return newId;
    }
    
    // Neuron removal
    bool removeNeuron(Brain* brain, NeuronId neuron) {
        if (!brain) return false;
        
        // Check if neuron has any incoming or outgoing synapses
        auto* region = brain->getRegion(brain->getRegionIdForNeuron(neuron));
        if (region) {
            // Check for synapses connected to this neuron
            for (auto& synapse : region->getSynapses()) {
                if (synapse->getSourceNeuron() == neuron || 
                    synapse->getDestinationNeuron() == neuron) {
                    return false; // Cannot remove neuron with active synapses
                }
            }
        }
        
        // Remove neuron
        brain->removeNeuron(neuron);
        pImpl->totalNeuronCount--;
        
        return true;
    }
    
    // Get synaptogenesis rate
    float getSynaptogenesisRate() const {
        return pImpl->parameters.synaptogenesisRate;
    }
    
    void setSynaptogenesisRate(float rate) {
        pImpl->parameters.synaptogenesisRate = std::max(0.0f, rate);
    }
    
    // Get pruning rate
    float getPruningRate() const {
        return pImpl->parameters.pruningRate;
    }
    
    void setPruningRate(float rate) {
        pImpl->parameters.pruningRate = std::max(0.0f, rate);
    }
    
    // Get neuron creation rate
    float getNeuronCreationRate() const {
        return pImpl->parameters.neuronCreationRate;
    }
    
    void setNeuronCreationRate(float rate) {
        pImpl->parameters.neuronCreationRate = std::max(0.0f, rate);
    }
    
    // Get neuron pruning rate
    float getNeuronPruningRate() const {
        return pImpl->parameters.neuronPruningRate;
    }
    
    void setNeuronPruningRate(float rate) {
        pImpl->parameters.neuronPruningRate = std::max(0.0f, rate);
    }
    
    // Set developmental stage
    void setDevelopmentalStage(float stage) {
        pImpl->parameters.developmentalStage = std::clamp(stage, 0.0f, 1.0f);
        updateParametersForDevelopment();
    }
    
    float getDevelopmentalStage() const {
        return pImpl->parameters.developmentalStage;
    }
    
    // Update structural plasticity
    void update(Brain* brain, class RandomGenerator& rng) {
        if (!brain) return;
        
        pImpl->simulationTime = pImpl->simulationTime + 0.001f;  // Increment time
        pImpl->timeSinceLastScaling += 0.001f;
        
        // Process pending synapse creations
        processSynapseCreations(brain, rng);
        
        // Process pending synapse removals
        processSynapseRemovals(brain);
        
        // Process pending neuron creations
        processNeuronCreations(brain, rng);
        
        // Process neuron pruning
        processNeuronPruning(brain, rng);
        
        // Apply synaptic scaling if needed
        if (pImpl->timeSinceLastScaling > 100.0f) {
            applySynapticScaling(brain);
            pImpl->timeSinceLastScaling = 0.0f;
        }
        
        // Update network statistics
        updateNetworkStatistics(brain);
    }
    
    // Get structural plasticity statistics
    float getTotalSynapseCount() const {
        return pImpl->totalSynapseCount;
    }
    
    float getTotalNeuronCount() const {
        return pImpl->totalNeuronCount;
    }
    
    float getExcitatoryInhibitoryRatio() const {
        return pImpl->globalExcitatoryRatio;
    }
    
    std::vector<SynapseCreationEvent> getSynapseCreations() const {
        return pImpl->completedCreations;
    }
    
    std::vector<PruningEvent> getPruningEvents() const {
        return pImpl->completedRemovals;
    }
    
    std::vector<NeuronCreationEvent> getNeuronCreations() const {
        return pImpl->completedNeuronCreations;
    }
    
    // Reset structural plasticity
    void reset() {
        pImpl->pendingCreations.clear();
        pImpl->pendingRemovals.clear();
        pImpl->pendingNeuronCreations.clear();
        pImpl->completedCreations.clear();
        pImpl->completedRemovals.clear();
        pImpl->completedNeuronCreations.clear();
        pImpl->totalSynapseCount = 0;
        pImpl->totalNeuronCount = 0;
        pImpl->globalExcitatoryRatio = 0.5f;
    }
    
    // Get current parameters
    const StructuralPlasticityParameters& getParameters() const {
        return pImpl->parameters;
    }
    
    void setParameters(const StructuralPlasticityParameters& params) {
        pImpl->parameters = params;
        updateParametersForDevelopment();
    }
    
private:
    struct Impl {
        StructuralPlasticityParameters parameters;
        float simulationTime;
        float timeSinceLastScaling;
        
        // Event queues
        std::vector<SynapseCreationEvent> pendingCreations;
        std::vector<PruningEvent> pendingRemovals;
        std::vector<NeuronCreationEvent> pendingNeuronCreations;
        
        // Completed events (history)
        std::vector<SynapseCreationEvent> completedCreations;
        std::vector<PruningEvent> completedRemovals;
        std::vector<NeuronCreationEvent> completedNeuronCreations;
        
        // Statistics
        size_t nextSynapseId;
        size_t nextNeuronId;
        float globalExcitatoryRatio;
        size_t totalSynapseCount;
        size_t totalNeuronCount;
        
        // Activity tracking
        std::unordered_map<NeuronId, float> neuronActivity;
        std::unordered_map<SynapseId, float> synapseWeights;
        
        Impl() : simulationTime(0.0f), timeSinceLastScaling(0.0f),
                 nextSynapseId(1), nextNeuronId(1),
                 globalExcitatoryRatio(0.5f), totalSynapseCount(0), totalNeuronCount(0) {}
    };
    
    Impl* pImpl;
    
    // Helper functions
    float computeDistance(Brain* brain, NeuronId neuron1, NeuronId neuron2) {
        if (!brain) return 0.0f;
        
        auto* region1 = brain->getRegion(brain->getRegionIdForNeuron(neuron1));
        auto* region2 = brain->getRegion(brain->getRegionIdForNeuron(neuron2));
        
        if (!region1 || !region2) return 1000.0f;  // Large distance
        
        // Simple distance calculation
        auto* neuronA = region1->getNeuron(neuron1);
        auto* neuronB = region2->getNeuron(neuron2);
        
        if (!neuronA || !neuronB) return 1000.0f;
        
        // Euclidean distance based on coordinates
        float dx = neuronA->getX() - neuronB->getX();
        float dy = neuronA->getY() - neuronB->getY();
        return std::sqrt(dx*dx + dy*dy);
    }
    
    RegionId getOptimalRegionForNewNeuron(Brain* brain, NeuronType type) {
        if (!brain) return INVALID_REGION_ID;
        
        // For now, return the first region
        // In a real implementation, this would choose based on neuron type and current network state
        std::vector<RegionId> regionIds = brain->getRegionIds();
        if (!regionIds.empty()) {
            return regionIds[0];
        }
        
        return brain->addRegion();
    }
    
    float computeInitialExcitability(NeuronType type) {
        switch (type) {
            case NeuronType::Excitatory:
                return 1.0f;
            case NeuronType::Inhibitory:
                return 0.5f;
            case NeuronType::Sensory:
                return 0.8f;
            case NeuronType::Motor:
                return 0.7f;
            case NeuronType::Internal:
                return 0.6f;
            default:
                return 0.5f;
        }
    }
    
    void updateParametersForDevelopment() {
        float stage = pImpl->parameters.developmentalStage;
        
        // Adjust parameters based on developmental stage
        // Early development: more synaptogenesis, less pruning
        // Late development: more pruning, less synaptogenesis
        
        if (stage < 0.3f) {
            // Early development
            pImpl->parameters.synaptogenesisRate *= (1.0f + stage * 2.0f);
            pImpl->parameters.pruningRate *= (1.0f - stage);
            pImpl->parameters.neuronCreationRate *= (1.0f + stage * 1.5f);
        } else if (stage < 0.7f) {
            // Mid development
            pImpl->parameters.synaptogenesisRate *= 0.8f;
            pImpl->parameters.pruningRate *= 1.2f;
            pImpl->parameters.neuronCreationRate *= 0.8f;
        } else {
            // Late development
            pImpl->parameters.synaptogenesisRate *= 0.5f;
            pImpl->parameters.pruningRate *= 1.5f;
            pImpl->parameters.neuronCreationRate *= 0.5f;
        }
        
        // Apply plasticity modulation
        pImpl->parameters.synaptogenesisRate *= pImpl->parameters.plasticityModulation;
        pImpl->parameters.pruningRate *= pImpl->parameters.plasticityModulation;
    }
    
    void processSynapseCreations(Brain* brain, RandomGenerator& rng) {
        for (auto& creation : pImpl->pendingCreations) {
            // Check if source and target neurons exist and are mature
            if (isNeuronMature(brain, creation.source) && isNeuronMature(brain, creation.target)) {
                // Create the synapse in the brain
                brain->createSynapse(creation.source, creation.target, creation.weight);
                
                // Mark as mature based on developmental stage
                creation.mature = (pImpl->parameters.developmentalStage > 0.5f);
                creation.efficacy = computeInitialEfficacy(creation.weight, creation.source, creation.target);
                
                pImpl->completedCreations.push_back(creation);
            }
        }
        
        pImpl->pendingCreations.clear();
    }
    
    void processSynapseRemovals(Brain* brain) {
        for (auto& removal : pImpl->pendingRemovals) {
            // Remove synapse from brain
            brain->removeSynapse(removal.synapseId);
            pImpl->completedRemovals.push_back(removal);
        }
        
        pImpl->pendingRemovals.clear();
    }
    
    void processNeuronCreations(Brain* brain, RandomGenerator& rng) {
        for (auto& creation : pImpl->pendingNeuronCreations) {
            if (pImpl->parameters.developmentalStage > 0.5f || 
                rng.uniformReal(0.0f, 1.0f) < pImpl->parameters.neuronCreationRate) {
                
                // Create neuron in brain
                brain->createNeuron(creation.type, creation.region, creation.initialExcitability);
                creation.mature = false;
                creation.maturationTime = pImpl->parameters.neuronMaturationTime;
                
                pImpl->completedNeuronCreations.push_back(creation);
            }
        }
        
        pImpl->pendingNeuronCreations.clear();
    }
    
    void processNeuronPruning(Brain* brain, RandomGenerator& rng) {
        if (!brain) return;
        
        // Identify neurons to prune based on activity and metabolic cost
        std::vector<NeuronId> neuronsToPrune;
        
        for (RegionId regionId : brain->getRegionIds()) {
            auto* region = brain->getRegion(regionId);
            if (!region) continue;
            
            for (auto& neuron : region->getAllNeurons()) {
                NeuronId neuronId = neuron->getId();
                
                // Check if neuron meets pruning criteria
                if (shouldPruneNeuron(brain, neuronId, rng)) {
                    neuronsToPrune.push_back(neuronId);
                }
            }
        }
        
        // Prune neurons
        for (NeuronId neuronId : neuronsToPrune) {
            removeNeuron(brain, neuronId);
        }
    }
    
    bool shouldPruneNeuron(Brain* brain, NeuronId neuronId, RandomGenerator& rng) {
        if (!brain) return false;
        
        // Get neuron activity
        float activity = getNeuronActivity(neuronId);
        
        // Prune based on activity threshold
        if (activity < pImpl->parameters.neuronActivityThreshold) {
            return true;
        }
        
        // Prune based on metabolic cost vs benefit
        float metabolicCost = pImpl->parameters.metabolicCost;
        float expectedBenefit = computeNeuronExpectedBenefit(brain, neuronId);
        
        if (metabolicCost > expectedBenefit && rng.uniformReal(0.0f, 1.0f) < 0.3f) {
            return true;
        }
        
        return false;
    }
    
    float getNeuronActivity(NeuronId neuronId) {
        auto it = pImpl->neuronActivity.find(neuronId);
        if (it != pImpl->neuronActivity.end()) {
            return it->second;
        }
        return 0.0f;
    }
    
    void setNeuronActivity(NeuronId neuronId, float activity) {
        pImpl->neuronActivity[neuronId] = activity;
    }
    
    float computeNeuronExpectedBenefit(Brain* brain, NeuronId neuronId) {
        // Compute expected benefit of neuron based on connectivity and activity
        float benefit = 0.0f;
        
        if (!brain) return benefit;
        
        // Get all neurons connected to this neuron
        auto* region = brain->getRegion(brain->getRegionIdForNeuron(neuronId));
        if (!region) return benefit;
        
        for (auto& synapse : region->getSynapses()) {
            if (synapse->getSourceNeuron() == neuronId || 
                synapse->getDestinationNeuron() == neuronId) {
                
                float weight = synapse->getWeight();
                float activity = getNeuronActivity(synapse->getSourceNeuron()) + 
                               getNeuronActivity(synapse->getDestinationNeuron());
                
                benefit += std::abs(weight) * activity;
            }
        }
        
        return benefit;
    }
    
    bool isNeuronMature(Brain* brain, NeuronId neuronId) {
        if (!brain) return false;
        
        // Check if neuron exists and has been active
        auto* region = brain->getRegion(brain->getRegionIdForNeuron(neuronId));
        if (!region) return false;
        
        auto* neuron = region->getNeuron(neuronId);
        if (!neuron) return false;
        
        // Simple maturity check based on neuron type and activity
        float activity = getNeuronActivity(neuronId);
        return activity > pImpl->parameters.activityThreshold;
    }
    
    float computeInitialEfficacy(SynapticWeight weight, NeuronId source, NeuronId target) {
        // Compute initial efficacy based on weight and neuron types
        float efficacy = std::abs(weight);
        
        // Adjust based on neuron types (simplified)
        if (weight > 0.0f) {
            efficacy *= 1.2f;  // Excitatory synapses higher efficacy
        } else {
            efficacy *= 0.8f;  // Inhibitory synapses lower efficacy
        }
        
        // Clamp to reasonable range
        return std::clamp(efficacy, 0.0f, 2.0f);
    }
    
    void applySynapticScaling(Brain* brain) {
        if (!brain) return;
        
        // Compute global weight statistics
        float totalWeight = 0.0f;
        size_t synapseCount = 0;
        
        for (RegionId regionId : brain->getRegionIds()) {
            auto* region = brain->getRegion(regionId);
            if (!region) continue;
            
            for (auto& synapse : region->getSynapses()) {
                totalWeight += std::abs(synapse->getWeight());
                synapseCount++;
            }
        }
        
        if (synapseCount == 0) return;
        
        float averageWeight = totalWeight / synapseCount;
        
        // Apply scaling to bring weights closer to average
        for (RegionId regionId : brain->getRegionIds()) {
            auto* region = brain->getRegion(regionId);
            if (!region) continue;
            
            for (auto& synapse : region->getSynapses()) {
                float currentWeight = synapse->getWeight();
                float scaledWeight = currentWeight + 
                                   (averageWeight - currentWeight) * 
                                   pImpl->parameters.weightNormalizationFactor;
                
                // Clamp to reasonable bounds
                scaledWeight = std::clamp(scaledWeight, 
                                         pImpl->parameters.strengthThreshold,
                                         pImpl->parameters.redundancyThreshold);
                
                synapse->setWeight(scaledWeight);
            }
        }
    }
    
    void updateNetworkStatistics(Brain* brain) {
        if (!brain) return;
        
        // Update global excitatory/inhibitory ratio
        float totalExcitatory = 0.0f;
        float totalInhibitory = 0.0f;
        
        for (RegionId regionId : brain->getRegionIds()) {
            auto* region = brain->getRegion(regionId);
            if (!region) continue;
            
            for (auto& synapse : region->getSynapses()) {
                if (synapse->isExcitatory()) {
                    totalExcitatory += std::abs(synapse->getWeight());
                } else if (synapse->isInhibitory()) {
                    totalInhibitory += std::abs(synapse->getWeight());
                }
            }
        }
        
        float total = totalExcitatory + totalInhibitory;
        if (total > 0.0f) {
            pImpl->globalExcitatoryRatio = totalExcitatory / total;
        }
    }
}; 

// Original Structural Plasticity class (maintains backward compatibility)
class OriginalStructuralPlasticity {
public:
    OriginalStructuralPlasticity() : pImpl(new Impl()) {}
    
    ~OriginalStructuralPlasticity() {
        delete pImpl;
    }
    
    OriginalStructuralPlasticity(const OriginalStructuralPlasticity&) = delete;
    OriginalStructuralPlasticity& operator=(const OriginalStructuralPlasticity&) = delete;
    
    OriginalStructuralPlasticity(OriginalStructuralPlasticity&& other) noexcept : pImpl(other.pImpl) {
        other.pImpl = nullptr;
    }
    
    OriginalStructuralPlasticity& operator=(OriginalStructuralPlasticity&& other) noexcept {
        if (this != &other) {
            delete pImpl;
            pImpl = other.pImpl;
            other.pImpl = nullptr;
        }
        return *this;
    }
    
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight) {
        if (!brain) return INVALID_SYNAPSE_ID;
        
        // Simple synapse creation
        return brain->createSynapse(source, destination, weight);
    }
    
    bool removeSynapse(Brain* brain, SynapseId synapse) {
        if (!brain) return false;
        
        return brain->removeSynapse(synapse);
    }
    
    NeuronId createNeuron(Brain* brain, NeuronType type) {
        if (!brain) return INVALID_NEURON_ID;
        
        // Simple neuron creation
        return brain->createNeuron(type);
    }
    
    bool removeNeuron(Brain* brain, NeuronId neuron) {
        if (!brain) return false;
        
        return brain->removeNeuron(neuron);
    }
    
    float getSynaptogenesisRate() const {
        return pImpl->parameters.synaptogenesisRate;
    }
    
    void setSynaptogenesisRate(float rate) {
        pImpl->parameters.synaptogenesisRate = std::max(0.0f, rate);
    }
    
    float getPruningRate() const {
        return pImpl->parameters.pruningRate;
    }
    
    void setPruningRate(float rate) {
        pImpl->parameters.pruningRate = std::max(0.0f, rate);
    }
    
    void update(Brain* brain, RandomGenerator& rng) {
        // Basic structural plasticity implementation
        if (!brain) return;
        
        pImpl->parameters.synaptogenesisRate *= 0.999f;  // Slow decay
        pImpl->parameters.pruningRate *= 1.001f;  // Slow growth
    }
    
private:
    struct Impl {
        StructuralPlasticityParameters parameters;
        
        Impl() {
            parameters = StructuralPlasticityParameters();
        }
    };
    
    Impl* pImpl;
};

// Factory function to create appropriate structural plasticity variant
std::unique_ptr<OriginalStructuralPlasticity> createStructuralPlasticity(const std::string& type) {
    if (type == "original" || type == "default") {
        return std::make_unique<OriginalStructuralPlasticity>();
    } else if (type == "enhanced" || type == "advanced") {
        return std::make_unique<EnhancedStructuralPlasticity>();
    } else {
        // Default to original for backward compatibility
        return std::make_unique<OriginalStructuralPlasticity>();
    }
}

} // namespace nlm
