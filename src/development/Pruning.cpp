#include "Pruning.hpp"
#include "../core/Logger/Logger.hpp"

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
    // Phase 2: Implement real pruning (elimination of synapses)
    // Activity-dependent synaptic elimination of weak or unused connections
    // Competitive mechanisms based on neural activity and developmental stage
    
    // Collect all synapses for evaluation with activity-based prioritization
    std::vector<Synapse*> synapsesToEvaluate;
    collectAllSynapses(brain, synapsesToEvaluate);
    
    // Calculate pruning criteria based on multiple biological factors
    std::vector<std::pair<SynapseId, float>> synapsePruningScores;
    calculatePruningScores(brain, synapsesToEvaluate, synapsePruningScores, rng);
    
    // Sort synapses by pruning score (higher score = more likely to be pruned)
    std::sort(synapsePruningScores.begin(), synapsePruningScores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Apply pruning based on developmental stage and network homeostasis
    int synapsesPruned = applyPruning(brain, synapsePruningScores);
    
    // Log pruning activity
    if (synapsesPruned > 0) {
        NLM_LOG_INFO("Pruning eliminated " + std::to_string(synapsesPruned) + " synapses");
    }
}

void Pruning::collectAllSynapses(Brain* brain, std::vector<Synapse*>& synapses) const {
    // Collect all synapses from all regions for evaluation
    for (auto& region : brain->getRegions()) {
        auto& regionSynapses = region->getSynapses();
        synapses.insert(synapses.end(), regionSynapses.begin(), regionSynapses.end());
    }
}

void Pruning::calculatePruningScores(Brain* brain, const std::vector<Synapse*>& synapses,
                                     std::vector<std::pair<SynapseId, float>>& scores,
                                     RandomGenerator& rng) const {
    // Calculate pruning scores based on multiple biological criteria
    for (const auto* synapse : synapses) {
        if (!synapse) continue;
        
        // Calculate composite pruning score
        float pruningScore = calculateSingleSynapsePruningScore(brain, synapse, rng);
        
        // Add stochastic component for natural variability
        float stochasticFactor = 1.0f + (rng.randomFloat() - 0.5f) * 0.2f; // ±10% random variation
        pruningScore *= stochasticFactor;
        
        // Ensure score is within valid range
        pruningScore = std::max(0.0f, std::min(pruningScore, 10.0f));
        
        scores.emplace_back(synapse->getId(), pruningScore);
    }
}

float Pruning::calculateSingleSynapsePruningScore(Brain* brain, Synapse* synapse,
                                                 RandomGenerator& rng) const {
    // Calculate pruning score for a single synapse using multiple criteria
    
    // Get synapse properties
    SynapticWeight weight = synapse->getWeight();
    SynapseType type = synapse->getType();
    NeuronId sourceId = synapse->getSourceNeuron();
    NeuronId destId = synapse->getDestinationNeuron();
    
    // Find the neurons to calculate their activities
    const Neuron* sourceNeuron = findNeuron(brain, sourceId);
    const Neuron* destNeuron = findNeuron(brain, destId);
    
    if (!sourceNeuron || !destNeuron) return 0.0f;
    
    // 1. Weight-based pruning: Weak synapses are likely unused
    float weightScore = calculateWeightPruningScore(weight, type);
    
    // 2. Activity-based pruning: Synapses with low correlated activity
    float activityScore = calculateActivityPruningScore(sourceNeuron, destNeuron, synapse);
    
    // 3. Developmental stage-based pruning
    float developmentalScore = calculateDevelopmentalPruningScore();
    
    // 4. Competition-based pruning: Strong synapses compete and eliminate weaker ones
    float competitionScore = calculateCompetitionPruningScore(brain, synapse, sourceId, destId);
    
    // 5. Synaptic history-based pruning
    float historyScore = calculateHistoryPruningScore(synapse);
    
    // Combine scores with weighted factors
    float totalScore = weightScore * 0.3f + activityScore * 0.25f +
                      developmentalScore * 0.15f + competitionScore * 0.2f + historyScore * 0.1f;
    
    // Apply threshold-based filtering
    if (totalScore < threshold) {
        return 0.0f;
    }
    
    return totalScore;
}

float Pruning::calculateWeightPruningScore(SynapticWeight weight, SynapseType type) const {
    // Weight-based pruning score
    float absoluteWeight = std::abs(weight);
    
    // Adjust threshold based on synapse type
    float weightThreshold = threshold;
    if (type == SynapseType::Excitatory) {
        weightThreshold *= 0.5f; // Stricter threshold for excitatory
    } else if (type == SynapseType::Inhibitory) {
        weightThreshold *= 0.8f; // Softer threshold for inhibitory
    }
    
    if (absoluteWeight < weightThreshold) {
        return 10.0f * (weightThreshold - absoluteWeight) / weightThreshold; // Higher score for weaker synapses
    }
    return 0.0f;
}

float Pruning::calculateActivityPruningScore(const Neuron* sourceNeuron, const Neuron* destNeuron,
                                           Synapse* synapse) const {
    // Activity-based pruning score
    if (!sourceNeuron || !destNeuron) return 0.0f;
    
    // Get spike history
    const auto& preSpikes = synapse->getPreSpikeHistory();
    const auto& postSpikes = synapse->getPostSpikeHistory();
    
    // Count spikes
    size_t preSpikeCount = preSpikes.size();
    size_t postSpikeCount = postSpikes.size();
    
    float activityScore = 0.0f;
    
    // If either neuron hasn't spiked recently, score heavily
    if (preSpikeCount == 0 || postSpikeCount == 0) {
        return 8.0f; // High pruning score
    }
    
    // If activity is very low, also consider pruning
    if (preSpikeCount < 2 && postSpikeCount < 2) {
        activityScore = 5.0f * (rng.randomFloat() * pruningRate * 10.0f);
    }
    
    // Calculate spike timing correlation (STDP-like)
    if (preSpikes.size() > 1 && postSpikes.size() > 1) {
        float preSpikeTime = preSpikes.back();
        float postSpikeTime = postSpikes.back();
        float timeDifference = std::abs(postSpikeTime - preSpikeTime);
        
        // Strong pruning if spikes are poorly timed (anti-Hebbian)
        if (timeDifference > 50.0f) {
            activityScore += 3.0f;
        }
    }
    
    return activityScore;
}

float Pruning::calculateDevelopmentalPruningScore() const {
    // Developmental stage-based pruning score
    // More pruning in later stages for refinement
    float stageMultiplier = 1.0f + maturationProgress; // Higher in maturation stages
    
    // Peak pruning during critical refinement phase (middle maturation)
    if (maturationProgress > 0.3f && maturationProgress < 0.7f) {
        stageMultiplier *= 1.5f;
    }
    
    return stageMultiplier;
}

float Pruning::calculateCompetitionPruningScore(Brain* brain, Synapse* synapse,
                                                NeuronId sourceId, NeuronId destId) const {
    // Competition-based pruning score
    // Strong synapses compete and eliminate weaker ones in their neighborhood
    
    float competitionScore = 0.0f;
    
    // Find competing synapses between the same neuron pair
    int competingSynapses = 0;
    int strongCompetingSynapses = 0;
    
    for (const auto& region : brain->getRegions()) {
        const auto& regionSynapses = region->getSynapses();
        for (const auto* otherSynapse : regionSynapses) {
            if (!otherSynapse || otherSynapse == synapse) continue;
            
            // Check if this synapse connects the same neurons (possibly reversed)
            if ((otherSynapse->getSourceNeuron() == sourceId && otherSynapse->getDestinationNeuron() == destId) ||
                (otherSynapse->getSourceNeuron() == destId && otherSynapse->getDestinationNeuron() == sourceId)) {
                competingSynapses++;
                
                // If competing synapse is stronger, increase pruning score
                if (std::abs(otherSynapse->getWeight()) > std::abs(synapse->getWeight())) {
                    strongCompetingSynapses++;
                }
            }
        }
    }
    
    // Higher score if there are strong competing synapses
    if (strongCompetingSynapses > 0) {
        competitionScore = 4.0f * (strongCompetingSynapses / (competingSynapses + 1.0f));
    }
    
    return competitionScore;
}

float Pruning::calculateHistoryPruningScore(Synapse* synapse) const {
    // Synaptic history-based pruning score
    if (!synapse) return 0.0f;
    
    const auto& preSpikes = synapse->getPreSpikeHistory();
    const auto& postSpikes = synapse->getPostSpikeHistory();
    
    // Check if synapse has been active recently
    if (preSpikes.empty() || postSpikes.empty()) {
        return 6.0f; // Very high pruning score if inactive
    }
    
    // Calculate temporal distribution of spikes
    float preSpikeAge = 0.0f;
    float postSpikeAge = 0.0f;
    
    if (!preSpikes.empty()) {
        preSpikeAge = preSpikes.back();
    }
    if (!postSpikes.empty()) {
        postSpikeAge = postSpikes.back();
    }
    
    // Penalize synapses with very old spikes
    float agePenalty = 0.0f;
    if (preSpikeAge > 100.0f) agePenalty += 2.0f;
    if (postSpikeAge > 100.0f) agePenalty += 2.0f;
    
    return agePenalty;
}

int Pruning::applyPruning(Brain* brain, const std::vector<std::pair<SynapseId, float>>& synapsesToPrune) {
    // Apply pruning based on sorted scores
    int synapsesPruned = 0;
    
    for (const auto& synapsePair : synapsesToPrune) {
        SynapseId synapseId = synapsePair.first;
        float pruningScore = synapsePair.second;
        
        // Skip synapses with low scores (below threshold)
        if (pruningScore < threshold) {
            continue;
        }
        
        // Determine if synapse should be pruned based on score and random chance
        float pruneProbability = std::min(1.0f, pruningScore / 10.0f); // Normalize to [0, 1]
        
        if (rng.randomFloat() < pruneProbability) {
            if (removeSynapse(brain, synapseId)) {
                synapsesPruned++;
            }
        }
    }
    
    return synapsesPruned;
}

bool Pruning::removeSynapse(Brain* brain, SynapseId synapseId) {
    // Remove synapse from brain regions
    for (auto& region : brain->getRegions()) {
        auto& synapses = region->getSynapses();
        
        for (auto it = synapses.begin(); it != synapses.end(); ++it) {
            if ((*it)->getId() == synapseId) {
                synapses.erase(it);
                return true;
            }
        }
    }
    return false;
}

const Neuron* Pruning::findNeuron(Brain* brain, NeuronId neuronId) const {
    // Find neuron by ID in the brain
    for (const auto& region : brain->getRegions()) {
        const auto& neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            if (neuron->getId() == neuronId) {
                return neuron.get();
            }
        }
    }
    return nullptr;
}

} // namespace nlm
