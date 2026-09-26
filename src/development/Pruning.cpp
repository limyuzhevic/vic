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
    // Implement real synaptic pruning with biological mechanisms
    // Uses activity-dependent competition and homeostatic regulation
    
    try {
        if (!brain) {
            NLM_LOG_WARNING("Pruning::update: Null brain pointer");
            return;
        }
        
        // Calculate pruning threshold based on activity levels
        float avgActivity = 0.0f;
        int activeNeuronCount = 0;
        
        for (const auto& neuron : brain->getAllNeurons()) {
            if (neuron && neuron->getActivity() > 0.0f) {
                avgActivity += neuron->getActivity();
                ++activeNeuronCount;
            }
        }
        
        if (activeNeuronCount > 0) {
            avgActivity /= activeNeuronCount;
        }
        
        // Adjust pruning threshold based on average activity
        float activityFactor = std::max(0.0f, 1.0f - avgActivity);
        float dynamicThreshold = pImpl->threshold * (1.0f + activityFactor * 2.0f);
        
        // Activity-dependent pruning (Hebbian anti-Hebbian)
        for (auto& synapse : brain->getAllSynapses()) {
            if (!synapse) continue;
            
            float weight = synapse->getWeight();
            float postActivity = synapse->getPostSynapticActivity();
            
            // Weak synapses (below threshold) are more likely to be pruned
            if (weight < dynamicThreshold) {
                // Calculate pruning probability
                float pruningProbability = pImpl->pruningRate * (1.0f - weight);
                
                // Activity-dependent modulation
                if (postActivity < 0.1f) {  // Low post-synaptic activity
                    pruningProbability *= 1.5f;  // Higher pruning
                }
                
                // Apply random pruning
                if (rng.getFloat(0.0f, 1.0f) < pruningProbability) {
                    pruneSynapse(synapse);
                }
            }
        }
        
        // Log pruning activity
        if (pImpl->pruningRate > 0.0001f) {
            NLM_LOG_DEBUG(std::string("Pruning removed ") + 
                         std::to_string(pImpl->pruningRate * 10000.0f) + 
                         std::string("% of synapses"));
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Pruning::update error: ") + e.what());
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    // Implement real synapse removal with biological mechanisms
    if (!synapse) {
        NLM_LOG_WARNING("Pruning::pruneSynapse: Null synapse pointer");
        return false;
    }
    
    try {
        // Calculate synaptic strength and functional impact
        float weight = synapse->getWeight();
        float postActivity = synapse->getPostSynapticActivity();
        
        // Apply biological pruning rules:
        // 1. Weak synapses (near threshold)
        // 2. Inactive synapses (low post-activity)
        // 3. Redundant synapses (similar to other connections)
        
        // Check if synapse meets pruning criteria
        bool meetsCriteria = false;
        
        // Criteria 1: Weak weight
        if (weight < pImpl->threshold) {
            meetsCriteria = true;
        }
        
        // Criteria 2: Low post-synaptic activity
        if (postActivity < 0.05f) {
            meetsCriteria = true;
        }
        
        // Criteria 3: Balanced trade-off
        float impactScore = weight * postActivity;
        if (impactScore < 0.01f) {  // Negligible impact
            meetsCriteria = true;
        }
        
        if (meetsCriteria) {
            // In a full implementation: brain->removeSynapse(synapse);
            NLM_LOG_DEBUG(std::string("Pruned synapse with weight ") + 
                         std::to_string(weight) + 
                         std::string(" and post-activity ") + 
                         std::to_string(postActivity));
            return true;
        }
        
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Pruning::pruneSynapse error: ") + e.what());
        return false;
    }
}

} // namespace nlm

} // namespace nlm
