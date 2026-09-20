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
    if (!brain || rng.generateFloat(0.0001f) > getPruningRate()) {  // 0.01% probability per update
        return;
    }
    
    // Get synapses that could be pruned
    std::vector<Synapse*> candidateSynapses;
    for (const auto& region : brain->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            // Prune weak synapses
            if (std::abs(syn->getWeight()) < getThreshold()) {
                candidateSynapses.push_back(syn);
            }
        }
    }
    
    // Prune some weak synapses
    size_t toPrune = std::min(size_t(1), candidateSynapses.size());
    for (size_t i = 0; i < toPrune; ++i) {
        size_t idx = rng.uniformInt(0, candidateSynapses.size() - 1);
        pruneSynapse(candidateSynapses[idx]);
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) {
        return false;
    }
    
    // Mark synapse as pruned (in real implementation, would remove from neural regions)
    // For now, just set weight to zero as a placeholder
    synapse->setWeight(0.0f);
    
    // Decay eligibility trace for plasticity
    synapse->decayEligibilityTrace(1.0f);
    
    return true;
}

} // namespace nlm
