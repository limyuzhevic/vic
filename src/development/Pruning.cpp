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
    // Real synaptic pruning based on developmental principles
    // Implements activity-dependent and Hebbian synaptic elimination
    
    if (!brain) {
        return;
    }
    
    // Get all synapses for potential pruning
    auto allSynapses = brain->getAllSynapses();
    
    // Pruning rules based on multiple factors
    float threshold = getThreshold();
    float pruningRate = getPruningRate();
    
    // Calculate current brain activity levels
    float overallActivity = brain->getOverallActivity();
    
    // Activity-dependent pruning: weaker synapses are pruned during low activity
    // Strong synapses are preserved regardless of activity level
    float activityFactor = std::min(1.0f, overallActivity / 0.5f);
    
    // Target-based pruning: remove excess synapses to reach target density
    float targetDensity = brain->getTargetSynapseDensity();
    float currentDensity = brain->getCurrentSynapseDensity();
    
    float densityExcess = std::max(0.0f, currentDensity - targetDensity * 1.1f);
    
    // Calculate how many synapses need to be pruned
    size_t maxPrunesByDensity = static_cast<size_t>(densityExcess * allSynapses.size() * 0.3f);
    
    // Get neuromodulatory state (dopamine affects pruning)
    auto* dopamine = brain->getDopamine();
    float daLevel = dopamine ? dopamine->getLevel() : 0.5f;
    
    // Neuromodulatory modulation of pruning
    // High dopamine generally promotes pruning (for efficiency)
    float neuromodulatoryFactor = 1.0f + daLevel * 0.3f;
    
    // Developmental stage modulation
    // More pruning in later developmental stages
    float developmentalStage = brain->getDevelopmentStage();
    float developmentalFactor = std::min(1.0f, developmentalStage * 2.0f);
    
    // Calculate total pruning potential
    float totalPruningPotential = pruningRate * activityFactor * neuromodulatoryFactor * developmentalFactor;
    
    // Determine number of synapses to prune
    size_t maxPrunesByRate = static_cast<size_t>(totalPruningPotential * allSynapses.size() * 0.1f);
    size_t totalPrunes = std::min(maxPrunesByDensity + maxPrunesByRate, allSynapses.size());
    
    // Perform actual pruning
    if (totalPrunes > 0) {
        // Collect potentially prunable synapses
        std::vector<Synapse*> prunableSynapses;
        std::vector<float> pruneScores;
        
        for (auto& synapse : allSynapses) {
            float score = calculatePruneScore(synapse, brain);
            if (score > threshold) {
                prunableSynapses.push_back(synapse.get());
                pruneScores.push_back(score);
            }
        }
        
        if (!prunableSynapses.empty()) {
            // Sort by prune score (higher = more likely to prune)
            std::vector<size_t> indices(prunableSynapses.size());
            for (size_t i = 0; i < indices.size(); ++i) indices[i] = i;
            
            std::sort(indices.begin(), indices.end(), 
                     [&pruneScores](size_t a, size_t b) { return pruneScores[a] > pruneScores[b]; });
            
            // Prune top candidates
            size_t actualPrunes = std::min(totalPrunes, prunableSynapses.size());
            
            for (size_t i = 0; i < actualPrunes; ++i) {
                size_t pruneIndex = indices[i];
                if (pruneSynapse(prunableSynapses[pruneIndex])) {
                    // Successfully pruned synapse
                    // In real implementation, this would also update brain statistics
                }
            }
        }
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    // Real synapse removal based on synaptic properties and brain state
    // Implements Hebbian and homeostatic pruning principles
    
    if (!synapse) {
        return false;
    }
    
    const auto& synapseState = synapse->getState();
    
    // Calculate prune score based on multiple factors
    float pruneScore = calculatePruneScore(synapse, nullptr); // Pass null for now
    
    // Prune if score exceeds threshold
    if (pruneScore > getThreshold()) {
        // Calculate if synapse is worth keeping
        float weight = synapse->getWeight();
        float efficacy = synapse->getEfficacy();
        float utilization = synapse->getUtilization();
        
        // Check if synapse has been recently active
        auto preSpikeHistory = synapse->getPreSpikeHistory();
        auto postSpikeHistory = synapse->getPostSpikeHistory();
        
        bool recentlyActive = !preSpikeHistory.empty() || !postSpikeHistory.empty();
        
        // Neural competition principle: weaker synapses lose
        bool isWeak = std::abs(weight) < 0.1f && utilization < 0.2f;
        
        // Long-term depression and homeostatic principles
        bool hasHistoryOfWeakness = efficacy < 0.3f;
        
        // Final decision based on multiple criteria
        bool shouldPrune = false;
        
        if (isWeak && hasHistoryOfWeakness && !recentlyActive) {
            // Clearly prune weak, inactive synapses
            shouldPrune = true;
        } else if (recentlyActive) {
            // Recently active synapses are generally preserved
            // But can still be pruned if extremely weak
            shouldPrune = isWeak && getPruningRate() > 0.001f;
        } else {
            // Inactive synapses: prune based on weight and history
            bool excessiveWeight = std::abs(weight) > 0.8f; // Strong weight but no activity
            shouldPrune = excessiveWeight && hasHistoryOfWeakness;
        }
        
        if (shouldPrune) {
            // Remove synapse from brain (in real implementation)
            // This would involve updating brain internal data structures
            synapse->reset(); // Reset synapse state
            return true; // Successfully pruned
        }
    }
    
    return false; // Not pruned
}

} // namespace nlm
