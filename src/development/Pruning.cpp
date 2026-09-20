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
        // Phase 2: Real pruning implementation
        // Remove weak synapses based on threshold and rate
        if (!brain) return;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        const float threshold = getThreshold();
        const float pruningRate = getPruningRate();
        
        // Iterate through synapses and prune weak ones
        auto& synapses = region->getSynapses();
        std::vector<Synapse*> synapsesToRemove;
        
        for (auto& syn : synapses) {
            if (syn->getWeight() < threshold) {
                // Determine if this synapse gets pruned
                if (rng.getFloat() < pruningRate) {
                    synapsesToRemove.push_back(syn.get());
                }
            }
        }
        
        // Remove pruned synapses
        for (auto* synapse : synapsesToRemove) {
            pruneSynapse(synapse);
        }
        
        if (!synapsesToRemove.empty()) {
            NLM_LOG_INFO("Pruned " + std::to_string(synapsesToRemove.size()) + " weak synapses");
        }
    }

bool Pruning::pruneSynapse(Synapse* synapse) {
        if (!synapse) return false;
        
        // Get the parent region
        auto* region = synapse->getOwner();
        if (!region) return false;
        
        // Create synapse removal request
        SynapseId synapseId = synapse->getId();
        
        // Remove from region's synapse list
        region->removeSynapse(synapseId);
        
        // Mark synapse for removal in brain's synaptic management
        if (auto* brain = region->getBrain()) {
            brain->markSynapseForRemoval(synapseId);
        }
        
        NLM_LOG_INFO("Pruned synapse " + std::to_string(synapseId.value));
        return true;
    }

} // namespace nlm
