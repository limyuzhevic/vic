#include "BrainPlasticity.hpp"
#include "BrainCore.hpp"
#include "../core/Types/Types.hpp"
#include "STDP.hpp"
#include "Hebbian.hpp"
#include "StructuralPlasticity.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

void BrainPlasticity::applySTDP(Synapse* syn, const std::vector<float>& preSpikes, 
                               const std::vector<float>& postSpikes, TimestepDuration timestep, float neuromodulationFactor) {
    if (!syn || preSpikes.empty() || postSpikes.empty()) return;
    
    // Calculate STDP weight changes
    for (float preTime : preSpikes) {
        for (float postTime : postSpikes) {
            float delta_t = postTime - preTime;
            float weightChange = 0.0f;
            
            if (delta_t > 0) {
                // LTP (Long-Term Potentiation): post after pre
                weightChange = currentLTPWeight * std::exp(-delta_t / currentTau);
            } else {
                // LTD (Long-Term Depression): post before pre
                weightChange = -currentLTDWeight * std::exp(delta_t / currentTau);
            }
            
            // Apply neuromodulation factor
            weightChange *= neuromodulationFactor;
            
            // Update synapse weight
            float currentWeight = syn->getWeight();
            float newWeight = currentWeight + weightChange;
            syn->setWeight(newWeight);
        }
    }
}

void BrainPlasticity::configureSTDP(float ltpWeight, float ltdWeight, float tau) {
    currentLTPWeight = ltpWeight;
    currentLTDWeight = ltdWeight;
    currentTau = tau;
}

void BrainPlasticity::applyHebbian(Synapse* syn, const std::vector<float>& preSpikes,
                                 const std::vector<float>& postSpikes, TimestepDuration timestep) {
    if (!syn || preSpikes.empty() || postSpikes.empty()) return;
    
    // Simple Hebbian rule: strengthen synapses with correlated activity
    float correlation = 0.0f;
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            correlation += preSpikes[i] * postSpikes[j];
        }
    }
    correlation /= (preSpikes.size() * postSpikes.size());
    
    // Apply Hebbian learning rule
    float weight = syn->getWeight();
    float delta = correlation * 0.1f * timestep;
    weight += delta;
    syn->setWeight(weight);
}

void BrainPlasticity::updateStructuralPlasticity(const class Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Perform synaptogenesis
    performSynaptogenesis(const_cast<class Brain*>(brain), rng);
    
    // Perform pruning
    performPruning(const_cast<class Brain*>(brain), rng);
}

void BrainPlasticity::setSynaptogenesisRate(float rate) {
    synaptogenesisRate = rate;
}

void BrainPlasticity::setPruningRate(float rate) {
    pruningRate = rate;
}

STDP* BrainPlasticity::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* BrainPlasticity::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* BrainPlasticity::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

float BrainPlasticity::getExcitationInhibitionRatio(const class Brain* brain) const {
    if (!brain) return 0.0f;
    
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t BrainPlasticity::getTotalSynapseCount(const class Brain* brain) const {
    if (!brain) return 0;
    
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

void BrainPlasticity::calculateSTDPWeightChange(float& weightChange, float preTime, float postTime, 
                                               float ltpWeight, float ltdWeight, float tau, bool isPreFirst) {
    float delta_t = postTime - preTime;
    
    if (delta_t > 0) {
        weightChange = ltpWeight * std::exp(-delta_t / tau);
    } else {
        weightChange = -ltdWeight * std::exp(delta_t / tau);
    }
}

void BrainPlasticity::updateEligibilityTraces(Synapse* syn, float preTime, float postTime, TimestepDuration timestep) {
    // Simple eligibility trace implementation
    if (syn) {
        float eligibilityTrace = syn->getEligibilityTrace();
        eligibilityTrace += std::exp(-preTime / timestep);
        syn->setEligibilityTrace(eligibilityTrace);
    }
}

void BrainPlasticity::performSynaptogenesis(class Brain* brain, RandomGenerator& rng) {
    if (!brain || !brain->pImpl) return;
    
    // Simple synaptogenesis: add new connections with probability synaptogenesisRate
    for (auto& region : brain->pImpl->regions) {
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) continue;
        
        // Randomly create new synapses
        for (size_t i = 0; i < neurons.size(); ++i) {
            for (size_t j = i + 1; j < neurons.size(); ++j) {
                if (rng.randf() < synaptogenesisRate) {
                    // Create new synapse
                    auto* source = neurons[i];
                    auto* target = neurons[j];
                    float weight = rng.randf() * 0.5f + 0.5f;
                    region->addSynapse(source->getId(), target->getId(), weight, 1);
                }
            }
        }
    }
}

void BrainPlasticity::performPruning(class Brain* brain, RandomGenerator& rng) {
    if (!brain || !brain->pImpl) return;
    
    // Simple pruning: remove weak synapses with probability pruningRate
    for (auto& region : brain->pImpl->regions) {
        auto synapses = region->getSynapses();
        for (auto* syn : synapses) {
            float weight = std::abs(syn->getWeight());
            if (weight < 0.01f && rng.randf() < pruningRate) {
                // Remove this synapse
                region->removeSynapse(syn->getId());
            }
        }
    }
}

} // namespace nlm