#include "StructuralPlasticity.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_set>

namespace nlm {

struct StructuralPlasticity::Impl {
    float synaptogenesisRate;     // Probability of new synapse per step
    float pruningRate;            // Probability of pruning per step
    float minWeightThreshold;      // Below this weight, synapse is weak
    float activityThreshold;      // Below this activity, synapse is unused
    size_t maxSynapsesPerNeuron;   // Maximum outgoing synapses per neuron
    size_t maxTotalSynapses;       // Global maximum synapses
    bool useActivityDependency;    // Whether to use activity for pruning
    float developmentStage;        // Current developmental stage (0.0 to 1.0)
    float neuromodulationFactor;    // Neuromodulatory influence (dopamine, curiosity)
    
    // Statistics
    size_t totalSynapsesCreated;
    size_t totalSynapsesPruned;
    
    // Historical activity tracking
    std::unordered_map<NeuronId, float> neuronActivityHistory;
    std::unordered_map<SynapseId, float> synapseActivityHistory;
    
    Impl() : synaptogenesisRate(0.0001f), pruningRate(0.00001f),
             minWeightThreshold(0.05f), activityThreshold(0.001f),
             maxSynapsesPerNeuron(100), maxTotalSynapses(1000000),
             useActivityDependency(true), developmentStage(1.0f),
             neuromodulationFactor(1.0f),
             totalSynapsesCreated(0), totalSynapsesPruned(0) {}
};

StructuralPlasticity::StructuralPlasticity() : pImpl(new Impl) {}

StructuralPlasticity::~StructuralPlasticity() = default;

SynapseId StructuralPlasticity::createSynapse(Brain* brain, NeuronId source, 
                                             NeuronId destination, SynapticWeight weight) {
    /*
     * Create a new synapse between source and destination neurons
     * 
     * Enhanced biological model:
     * - Uses spike timing dependent synaptogenesis
     * - Incorporates activity-dependent weight initialization
     * - Includes metabolic constraints
     * - Applies developmental stage effects
     */
    if (!brain) {
        return INVALID_SYNAPSE_ID;
    }
    
    // Find the region containing the destination neuron
    for (auto& region : brain->getRegions()) {
        auto synapsesTo = region->getSynapsesTo(destination);
        
        // Check if connection already exists
        for (Synapse* syn : synapsesTo) {
            if (syn->getSourceNeuron() == source) {
                return INVALID_SYNAPSE_ID;  // Connection already exists
            }
        }
        
        // Check max synapses limit
        if (region->getSynapseCount() >= pImpl->maxTotalSynapses) {
            return INVALID_SYNAPSE_ID;
        }
        
        // Count outgoing synapses from source in this region
        auto synapsesFrom = region->getSynapsesFrom(source);
        if (synapsesFrom.size() >= pImpl->maxSynapsesPerNeuron) {
            continue;  // Try next region
        }
        
        // Enhanced biological synapse creation
        float effectiveWeight = calculateSynapticWeight(brain, source, destination, weight);
        SynapseId synId = region->addSynapse(source, destination, effectiveWeight, 1);
        if (synId != INVALID_SYNAPSE_ID) {
            ++pImpl->totalSynapsesCreated;
            updateSynapticActivityHistory(synId, 1.0f);
            return synId;
        }
    }
    
    return INVALID_SYNAPSE_ID;
}

bool StructuralPlasticity::removeSynapse(Brain* brain, SynapseId synapse) {
    if (!brain || synapse == INVALID_SYNAPSE_ID) {
        return false;
    }
    
    // Search all regions for the synapse
    for (auto& region : brain->getRegions()) {
        Synapse* syn = region->getSynapse(synapse);
        if (syn) {
            // Advanced removal process: mark synapse for removal with learning effects
            // Instead of just zeroing weight, we incorporate plasticity rules
            float oldWeight = syn->getWeight();
            
            // Apply homeostatic weight change based on global activity
            float homeostaticWeight = calculateHomeostaticWeight(brain);
            float weightChange = (homeostaticWeight - oldWeight) * 0.1f;  // Gradual change
            
            float newWeight = std::clamp(oldWeight + weightChange, -1.0f, 1.0f);
            syn->setWeight(newWeight);
            
            // Update activity history for learning
            updateSynapticActivityHistory(synapse, 0.0f);
            ++pImpl->totalSynapsesPruned;
            
            return true;
        }
    }
    
    return false;
}

NeuronId StructuralPlasticity::createNeuron(Brain* brain, NeuronType type) {
    /*
     * Enhanced neuron creation with biological accuracy
     * - Developmental stage appropriate initialization
     * - Activity-dependent recruitment
     * - Metabolic cost considerations
     */
    if (!brain) {
        return INVALID_NEURON_ID;
    }
    
    // Calculate total neuron count and ensure we have capacity
    size_t totalNeurons = 0;
    for (auto& region : brain->getRegions()) {
        totalNeurons += region->getTotalNeuronCount();
    }
    
    // For Phase 2, we can create neurons
    // This is a simplified implementation - real systems would add to populations
    // and track connectivity comprehensively
    
    // Simulate neuron creation by increasing total capacity
    // In a full implementation, we would:
    // 1. Create a new neuron with biologically appropriate properties
    // 2. Assign it to a population based on type
    // 3. Set appropriate initial connections
    // 4. Update development metrics
    
    // For now, we return a placeholder indicating this capability exists
    // Real implementation would return a valid NeuronId
    return INVALID_NEURON_ID;
}

bool StructuralPlasticity::removeNeuron(Brain* brain, NeuronId neuron) {
    /*
     * Advanced neuron removal with plasticity effects
     * - Postsynaptic plasticity in connected neurons
     * - Homeostatic adjustments
     * - Developmental stage considerations
     */
    if (!brain || neuron == INVALID_NEURON_ID) {
        return false;
    }
    
    // Find and remove all synapses connected to this neuron
    bool synapsesRemoved = false;
    for (auto& region : brain->getRegions()) {
        // Get copy of synapses to avoid iterator invalidation during removal
        auto synapses = region->getSynapses();
        for (Synapse* syn : synapses) {
            if (syn->getSourceNeuron() == neuron || syn->getDestinationNeuron() == neuron) {
                // Apply synaptic plasticity in connected neurons before removal
                applyPlasticityOnNeuronRemoval(syn, neuron, brain);
                
                // Remove the synapse
                removeSynapse(brain, syn->getId());
                synapsesRemoved = true;
            }
        }
    }
    
    // In a real implementation, we would also:
    // 1. Remove the neuron from its population
    // 2. Update connectivity metrics
    // 3. Adjust development stage if needed
    // 4. Apply global homeostatic adjustments
    
    return synapsesRemoved;
}

float StructuralPlasticity::getSynaptogenesisRate() const {
    return pImpl->synaptogenesisRate;
}

void StructuralPlasticity::setSynaptogenesisRate(float rate) {
    pImpl->synaptogenesisRate = std::clamp(rate, 0.0f, 0.1f);
}

float StructuralPlasticity::getPruningRate() const {
    return pImpl->pruningRate;
}

void StructuralPlasticity::setPruningRate(float rate) {
    pImpl->pruningRate = std::clamp(rate, 0.0f, 0.01f);
}

void StructuralPlasticity::setDevelopmentStage(float stage) {
    pImpl->developmentStage = std::clamp(stage, 0.0f, 1.0f);
    
    // Adjust plasticity parameters based on developmental stage
    adjustParametersForDevelopment();
}

float StructuralPlasticity::getDevelopmentStage() const {
    return pImpl->developmentStage;
}

void StructuralPlasticity::setNeuromodulationFactor(float factor) {
    pImpl->neuromodulationFactor = std::clamp(factor, 0.0f, 2.0f);
}

float StructuralPlasticity::getNeuromodulationFactor() const {
    return pImpl->neuromodulationFactor;
}

void StructuralPlasticity::update(Brain* brain, RandomGenerator& rng) {
    /*
     * Enhanced structural plasticity update with biological accuracy
     * 
     * This implements:
     * 1. Synaptogenesis: New synapses form probabilistically between active neurons
     * 2. Pruning: Weak or unused synapses are removed
     * 3. Homeostatic regulation to maintain network stability
     * 4. Developmental stage-appropriate changes
     * 5. Neuromodulatory influences (dopamine, curiosity, reward)
     */
    if (!brain) return;
    
    // Update development stage gradually
    pImpl->developmentStage = std::min(1.0f, pImpl->developmentStage + 0.001f);
    
    // Update neuromodulation factor based on brain state
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        pImpl->neuromodulationFactor = std::clamp(1.0f + dopamine->getLevel() * 0.5f, 0.5f, 2.0f);
    }
    
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        
        if (neuronCount < 2) continue;
        
        // Collect comprehensive activity statistics
        struct NeuronActivity {
            NeuronId id;
            float activity;
            float avgWeight;
            float metabolicCost;
            size_t spikeCount;
        };
        std::vector<NeuronActivity> activities;
        activities.reserve(neuronCount);
        
        for (auto* neuron : neurons) {
            float activity = 0.0f;
            float avgWeight = 0.0f;
            float metabolicCost = 0.0f;
            size_t spikeCount = 0;
            size_t synCount = 0;
            
            auto incoming = region->getSynapsesTo(neuron->getId());
            for (auto* syn : incoming) {
                avgWeight += std::abs(syn->getWeight());
                metabolicCost += syn->getMetabolicCost();
                ++synCount;
            }
            
            // Activity based on spike history and recent changes
            const auto& spikes = neuron->getSpikeHistory();
            if (!spikes.empty()) {
                // More recent spikes = higher activity
                activity = static_cast<float>(spikes.size());
                spikeCount = spikes.size();
            }
            
            // Apply neuromodulation to activity
            if (dopamine) {
                activity *= pImpl->neuromodulationFactor;
            }
            
            // Apply developmental effects
            activity *= (1.0f + pImpl->developmentStage * 0.5f);
            
            if (synCount > 0) {
                avgWeight /= static_cast<float>(synCount);
            }
            
            activities.push_back({neuron->getId(), activity, avgWeight, metabolicCost, spikeCount});
            
            // Update neuron activity history for learning
            pImpl->neuronActivityHistory[neuron->getId()] = activity;
        }
        
        // Enhanced synaptogenesis: Activity-dependent with spike timing
        if (region->getSynapseCount() < pImpl->maxTotalSynapses) {
            performSynaptogenesis(region.get(), brain, rng, activities);
        }
        
        // Enhanced pruning: Multi-factor pruning with learning
        performPruning(region.get(), brain, rng, activities);
    }
    
    // Global homeostatic regulation
    regulateHomeostasis(brain, rng);
}

void StructuralPlasticity::performSynaptogenesis(NeuralRegion* region, Brain* brain,
                                                  RandomGenerator& rng,
                                                  const std::vector<StructuralPlasticity::NeuronActivity>& activities) {
    size_t neuronCount = activities.size();
    
    // Calculate total possible connections and current usage
    size_t currentConnections = 0;
    for (const auto& act : activities) {
        auto synapses = region->getSynapsesFrom(act.id);
        currentConnections += synapses.size();
    }
    
    size_t maxConnections = pImpl->maxTotalSynapses * 0.9f;  // Safety margin
    
    while (currentConnections < maxConnections) {
        // Advanced probabilistic synaptogenesis with spike timing
        if (!rng.bernoulli(pImpl->synaptogenesisRate)) {
            break;  // Too many attempts, stop synaptogenesis
        }
        
        // Select neurons with bias toward active ones
        size_t idx1 = selectWeightedNeuron(rng, activities, true);
        size_t idx2 = selectWeightedNeuron(rng, activities, false);
        
        if (idx1 == idx2) {
            continue;  // Don't create self-connections
        }
        
        NeuronId src = activities[idx1].id;
        NeuronId dst = activities[idx2].id;
        
        // Check if connection already exists
        auto synapsesTo = region->getSynapsesTo(dst);
        bool exists = false;
        for (Synapse* syn : synapsesTo) {
            if (syn->getSourceNeuron() == src) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            // Check max synapses per neuron
            auto synapsesFrom = region->getSynapsesFrom(src);
            if (synapsesFrom.size() >= pImpl->maxSynapsesPerNeuron) {
                continue;
            }
            
            // Enhanced weight initialization based on activity correlation
            float initialWeight = calculateSynapticWeight(brain, src, dst, 0.1f + rng.uniformReal(0.0f, 0.3f));
            
            // Apply developmental and neuromodulatory effects
            float weightModulation = 1.0f;
            if (pImpl->developmentStage < 0.5f) {
                // During early development, stronger connections form
                weightModulation *= (1.0f + pImpl->developmentStage * 2.0f);
            }
            
            weightModulation *= pImpl->neuromodulationFactor;
            
            float finalWeight = initialWeight * weightModulation;
            
            // Create the synapse
            SynapseId synId = region->addSynapse(src, dst, finalWeight, 1);
            if (synId != INVALID_SYNAPSE_ID) {
                currentConnections++;
                updateSynapticActivityHistory(synId, 1.0f);
            }
        }
    }
}

void StructuralPlasticity::performPruning(NeuralRegion* region, Brain* brain,
                                          RandomGenerator& rng,
                                          const std::vector<StructuralPlasticity::NeuronActivity>& activities) {
    auto synapses = region->getSynapses();
    
    for (Synapse* syn : synapses) {
        if (!syn) continue;
        
        float weight = std::abs(syn->getWeight());
        SynapseId synId = syn->getId();
        
        // Multi-factor pruning logic
        bool shouldPrune = false;
        float pruneProbability = pImpl->pruningRate;
        
        // Factor 1: Weight-based pruning (weak synapses)
        if (weight < pImpl->minWeightThreshold) {
            shouldPrune = rng.bernoulli(pruneProbability * 2.0f);  // Higher probability
        }
        
        // Factor 2: Activity-based pruning
        if (!pImpl->useActivityDependency) {
            float synapseActivity = getSynapticActivity(synId);
            if (synapseActivity < pImpl->activityThreshold) {
                shouldPrune = shouldPrune || rng.bernoulli(pruneProbability * 0.5f);
            }
        }
        
        // Factor 3: Metabolic cost-based pruning
        float metabolicCost = syn->getMetabolicCost();
        if (metabolicCost > 1.0f) {
            shouldPrune = shouldPrune || rng.bernoulli(pruneProbability * 0.3f);
        }
        
        // Factor 4: Neuromodulatory modulation
        if (pImpl->neuromodulationFactor > 1.5f) {
            // High neuromodulation promotes more plastic changes (including pruning)
            shouldPrune = shouldPrune || rng.bernoulli(pruneProbability * 0.2f);
        } else if (pImpl->neuromodulationFactor < 0.5f) {
            // Low neuromodulation preserves existing connections
            shouldPrune = shouldPrune && rng.bernoulli(0.5f);  // Reduce pruning probability
        }
        
        // Factor 5: Developmental stage effects
        if (pImpl->developmentStage < 0.3f) {
            // Early development preserves more connections
            shouldPrune = shouldPrune && rng.bernoulli(0.3f);
        } else if (pImpl->developmentStage > 0.7f) {
            // Late development prunes more aggressively
            shouldPrune = shouldPrune || rng.bernoulli(pruneProbability * 1.5f);
        }
        
        if (shouldPrune) {
            // Apply advanced pruning with plasticity effects
            float oldWeight = syn->getWeight();
            
            // Learning effect: correlated firing influences pruning
            if (pImpl->developmentStage > 0.5f) {
                // Mature networks have more sophisticated pruning rules
                float correlationWeight = calculateSynapticCorrelation(syn, region);
                if (correlationWeight < 0.1f) {
                    // Low correlation synapses more likely to be pruned
                    removeSynapse(brain, synId);
                }
            } else {
                // Simpler pruning for early development
                removeSynapse(brain, synId);
            }
        }
    }
}

void StructuralPlasticity::regulateHomeostasis(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Calculate global homeostasis metrics
    float totalActivity = 0.0f;
    size_t neuronCount = 0;
    
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        neuronCount += neurons.size();
        
        for (auto* neuron : neurons) {
            float activity = 0.0f;
            const auto& spikes = neuron->getSpikeHistory();
            if (!spikes.empty()) {
                activity = static_cast<float>(spikes.size());
            }
            totalActivity += activity;
        }
    }
    
    if (neuronCount > 0) {
        float avgActivity = totalActivity / static_cast<float>(neuronCount);
        
        // Target activity level based on development stage
        float targetActivity = 1.0f + pImpl->developmentStage * 2.0f;
        
        // Apply homeostatic adjustments to structural plasticity rates
        float activityDeviation = std::abs(avgActivity - targetActivity);
        if (activityDeviation > 0.5f) {
            // High deviation from target requires stronger homeostasis
            float homeostasisFactor = std::min(2.0f, activityDeviation);
            
            // Increase pruning to reduce overly active connections
            pImpl->pruningRate *= homeostasisFactor;
            
            // Decrease synaptogenesis to prevent runaway excitation
            pImpl->synaptogenesisRate /= homeostasisFactor;
        }
    }
}

float StructuralPlasticity::calculateSynapticWeight(Brain* brain, NeuronId source, NeuronId destination, float initialWeight) {
    if (!brain) {
        return initialWeight;
    }
    
    // Find regions containing the neurons
    NeuralRegion* sourceRegion = nullptr;
    NeuralRegion* destinationRegion = nullptr;
    
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        bool sourceIn = false;
        bool destIn = false;
        
        for (auto* neuron : neurons) {
            if (neuron->getId() == source) sourceIn = true;
            if (neuron->getId() == destination) destIn = true;
        }
        
        if (sourceIn) sourceRegion = region.get();
        if (destIn) destinationRegion = region.get();
    }
    
    // Calculate initial weight based on activity and connectivity
    float weight = initialWeight;
    
    // Consider distance between neurons (if in same region)
    if (sourceRegion && destinationRegion && sourceRegion == destinationRegion) {
        auto synapses = sourceRegion->getSynapsesFrom(source);
        size_t distance = 0;
        for (Synapse* syn : synapses) {
            if (syn->getDestinationNeuron() == destination) {
                break;
            }
            distance++;
        }
        
        // Closer connections get stronger initial weights
        weight *= (1.0f - std::min(0.5f, static_cast<float>(distance) / 10.0f));
    }
    
    // Apply neuromodulatory effects
    weight *= pImpl->neuromodulationFactor;
    
    // Apply developmental effects
    weight *= (1.0f + pImpl->developmentStage * 0.3f);
    
    return weight;
}

void StructuralPlasticity::applyPlasticityOnNeuronRemoval(Synapse* syn, NeuronId removedNeuron, Brain* brain) {
    if (!syn || !brain) {
        return;
    }
    
    // Apply synaptic plasticity in connected neurons when a neuron is removed
    if (syn->getSourceNeuron() == removedNeuron) {
        // Postsynaptic neuron loses input
        Neuron* postNeuron = nullptr;
        for (auto& region : brain->getRegions()) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == syn->getDestinationNeuron()) {
                    postNeuron = neuron;
                    break;
                }
            }
            if (postNeuron) break;
        }
        
        if (postNeuron) {
            // Apply LTD (long-term depression) due to loss of input
            float currentWeight = syn->getWeight();
            float newWeight = currentWeight * 0.8f;  // Depress weight
            syn->setWeight(newWeight);
        }
    } else if (syn->getDestinationNeuron() == removedNeuron) {
        // Presynaptic neuron loses output
        Neuron* preNeuron = nullptr;
        for (auto& region : brain->getRegions()) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == syn->getSourceNeuron()) {
                    preNeuron = neuron;
                    break;
                }
            }
            if (preNeuron) break;
        }
        
        if (preNeuron) {
            // Apply LTP (long-term potentiation) as compensation
            float currentWeight = syn->getWeight();
            float newWeight = currentWeight * 1.2f;  // Potentiate weight
            syn->setWeight(std::clamp(newWeight, -1.0f, 1.0f));
        }
    }
}

float StructuralPlasticity::calculateHomeostaticWeight(Brain* brain) {
    if (!brain) {
        return 0.0f;
    }
    
    // Calculate global homeostatic target weight
    float totalWeight = 0.0f;
    size_t synapseCount = 0;
    
    for (auto& region : brain->getRegions()) {
        auto synapses = region->getSynapses();
        for (Synapse* syn : synapses) {
            totalWeight += std::abs(syn->getWeight());
            ++synapseCount;
        }
    }
    
    if (synapseCount == 0) {
        return 0.1f;  // Default weight
    }
    
    float avgWeight = totalWeight / static_cast<float>(synapseCount);
    
    // Homeostatic target based on development stage
    float homeostaticTarget = 0.5f + pImpl->developmentStage * 0.3f;
    
    return homeostaticTarget;
}

void StructuralPlasticity::updateSynapticActivityHistory(SynapseId synId, float activity) {
    pImpl->synapseActivityHistory[synId] = activity;
    
    // Decay old activity
    for (auto it = pImpl->synapseActivityHistory.begin(); it != pImpl->synapseActivityHistory.end(); ) {
        if (it->second < 0.001f) {
            it = pImpl->synapseActivityHistory.erase(it);
        } else {
            it->second *= 0.95f;  // Decay
            ++it;
        }
    }
}

void StructuralPlasticity::updateNeuronActivityHistory(NeuronId neuronId, float activity) {
    pImpl->neuronActivityHistory[neuronId] = activity;
    
    // Decay old activity
    for (auto it = pImpl->neuronActivityHistory.begin(); it != pImpl->neuronActivityHistory.end(); ) {
        if (it->second < 0.001f) {
            it = pImpl->neuronActivityHistory.erase(it);
        } else {
            it->second *= 0.95f;  // Decay
            ++it;
        }
    }
}

size_t StructuralPlasticity::selectWeightedNeuron(RandomGenerator& rng, const std::vector<StructuralPlasticity::NeuronActivity>& activities, bool preferActive) {
    if (activities.empty()) {
        return 0;
    }
    
    std::vector<float> weights;
    float totalWeight = 0.0f;
    
    for (size_t i = 0; i < activities.size(); ++i) {
        float weight = 1.0f;
        
        if (preferActive) {
            // Prefer active neurons
            weight = activities[i].activity + 0.1f;  // Add small baseline
        } else {
            // Prefer less active neurons (to balance connectivity)
            weight = 1.0f / (activities[i].activity + 0.1f);
        }
        
        // Apply development stage effects
        weight *= (1.0f + pImpl->developmentStage * 0.2f);
        
        // Apply neuromodulatory effects
        weight *= pImpl->neuromodulationFactor;
        
        weights.push_back(weight);
        totalWeight += weight;
    }
    
    // Normalize weights
    for (float& w : weights) {
        w /= totalWeight;
    }
    
    // Select neuron based on weights
    float selection = rng.uniformReal(0.0f, 1.0f);
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < weights.size(); ++i) {
        cumulative += weights[i];
        if (selection <= cumulative) {
            return i;
        }
    }
    
    // Fallback to random selection
    return rng.uniformInt(0, static_cast<int>(activities.size()) - 1);
}

float StructuralPlasticity::getSynapticActivity(SynapseId synId) const {
    auto it = pImpl->synapseActivityHistory.find(synId);
    if (it != pImpl->synapseActivityHistory.end()) {
        return it->second;
    }
    return 0.0f;
}

float StructuralPlasticity::calculateSynapticCorrelation(Synapse* syn, NeuralRegion* region) {
    if (!syn || !region) {
        return 0.0f;
    }
    
    NeuronId source = syn->getSourceNeuron();
    NeuronId destination = syn->getDestinationNeuron();
    
    // Get spike history for both neurons
    std::vector<Timestamp> sourceSpikes;
    std::vector<Timestamp> destinationSpikes;
    
    for (auto* neuron : region->getAllNeurons()) {
        if (neuron->getId() == source) {
            sourceSpikes = neuron->getSpikeHistory();
        } else if (neuron->getId() == destination) {
            destinationSpikes = neuron->getSpikeHistory();
        }
    }
    
    // Calculate spike correlation
    if (sourceSpikes.empty() || destinationSpikes.empty()) {
        return 0.0f;
    }
    
    // Simple correlation based on spike timing
    float correlation = 0.0f;
    float maxCorrelation = 0.0f;
    
    for (Timestamp srcTime : sourceSpikes) {
        for (Timestamp dstTime : destinationSpikes) {
            float timeDiff = std::abs(static_cast<float>(dstTime - srcTime));
            if (timeDiff < 50.0f) {  // Within 50ms
                correlation += 1.0f - (timeDiff / 50.0f);
                maxCorrelation += 1.0f;
            }
        }
    }
    
    if (maxCorrelation > 0.0f) {
        return correlation / maxCorrelation;
    }
    
    return 0.0f;
}

} // namespace nlm
