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
    // Real pruning: remove ineffective synaptic connections
    // Based on activity-dependent rules: "fire together, wire together" applies
    // Weak synapses are eliminated to maintain network efficiency
    
    if (!brain || !brain->getSpikeSystem()) return;
    
    // Get all regions
    auto& regions = brain->getRegions();
    
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        
        for (const auto& synapseHandle : synapses) {
            Synapse* synapse = synapseHandle.get();
            if (!synapse) continue;
            
            // Calculate synapse effectiveness based on recent activity
            float effectiveness = calculateSynapseEffectiveness(brain, synapse);
            
            // Check if synapse should be pruned
            float pruneProbability = calculatePruneProbability(effectiveness, rng);
            
            if (rng.uniformReal(0.0f, 1.0f) < pruneProbability) {
                // Remove this synapse
                bool removed = pruneSynapse(synapse);
                if (removed) {
                    NLM_LOG_INFO("Pruning: Removed ineffective synapse from neuron " + 
                                std::to_string(synapse->getSourceNeuron().index()) + " to neuron " + 
                                std::to_string(synapse->getDestinationNeuron().index()) + 
                                " (effectiveness=" + std::to_string(effectiveness) + ")");
                }
            }
        }
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    // Real synapse removal with integration into brain's synapse management
    if (!synapse) return false;
    
    // Get the region containing this synapse
    auto& regions = synapse->getBrain()->getRegions();
    
    // Find the region that contains this synapse
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        
        // Find and remove this synapse from the region
        for (auto it = synapses.begin(); it != synapses.end(); ++it) {
            if ((*it).get() == synapse) {
                region->removeSynapse((*it).get());
                return true;
            }
        }
    }
    
    return false;
}

// Helper method to calculate synapse effectiveness
float Pruning::calculateSynapseEffectiveness(Brain* brain, Synapse* synapse) {
    if (!synapse || !brain) return 0.0f;
    
    // Effectiveness based on:
    // 1. Synaptic weight (stronger = more effective)
    // 2. Recent spike correlation (Hebbian component)
    // 3. Target neuron activity (integration component)
    
    float weight = synapse->getWeight();
    
    // Get spike history for pre and post neurons
    auto& preHistory = synapse->getPreSpikeHistory();
    auto& postHistory = synapse->getPostSpikeHistory();
    
    // Calculate recent spike correlation
    float correlation = 0.0f;
    if (!preHistory.empty() && !postHistory.empty()) {
        // Simple correlation: overlapping recent spikes
        // This is a simplified version - real implementation would use
        // more sophisticated spike train analysis
        float recentPre = preHistory.back();
        float recentPost = postHistory.back();
        
        // Check if both neurons spiked recently (within same simulation window)
        correlation = (recentPre > 0 && recentPost > 0) ? 1.0f : 0.0f;
    }
    
    // Get target neuron activity
    Neuron* targetNeuron = brain->getNeuron(synapse->getDestinationNeuron());
    float targetActivity = targetNeuron ? targetNeuron->getFiringRate() : 0.0f;
    
    // Combine factors for effectiveness
    float effectiveness = std::abs(weight) * 0.5f + correlation * 0.3f + targetActivity * 0.2f;
    
    return effectiveness;
}

// Helper method to calculate pruning probability
float Pruning::calculatePruneProbability(float effectiveness, RandomGenerator& rng) {
    // Higher effectiveness = lower pruning probability
    float baseProbability = pImpl->pruningRate;
    
    // Strong correlation with target neuron activity = lower pruning
    float activityFactor = 1.0f - effectiveness;
    
    // Time-dependent factor: more pruning in later developmental stages
    float timeFactor = 1.0f; // Simplified - could be based on simulation time
    
    float probability = baseProbability * activityFactor * timeFactor;
    
    // Stochastic component
    probability *= rng.uniformReal(0.5f, 1. privataf);
    
    return std::min(probability, 0.1f); // Cap at 10%
}

} // namespace nlm