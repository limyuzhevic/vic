#include "BrainStatistics.hpp"
#include "Brain.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

BrainStatistics::BrainStatistics(Brain::Impl* pImpl) : pImpl(pImpl) {}

float BrainStatistics::performExcitationInhibitionRatio() const {
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

size_t BrainStatistics::performTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t BrainStatistics::performPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

size_t BrainStatistics::performTotalNeuronCount() const {
    size_t count = 0;
    for (const auto& region : pImpl->regions) {
        count += region->getTotalNeuronCount();
    }
    return count;
}

size_t BrainStatistics::performTotalSynapseCount() const {
    size_t count = 0;
    for (const auto& region : pImpl->regions) {
        count += region->getSynapseCount();
    }
    return count;
}

size_t BrainStatistics::performActiveNeuronCount() const {
    size_t count = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring()) {
                    count++;
                }
            }
        }
    }
    return count;
}

size_t BrainStatistics::performFiringNeuronCount() const {
    size_t count = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring() || neuron->getState().refractoryRemaining > 0) {
                    count++;
                }
            }
        }
    }
    return count;
}

float BrainStatistics::performAverageFiringRate() const {
    float totalRate = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                totalRate += neuron->getFiringRate();
                neuronCount++;
            }
        }
    }
    
    if (neuronCount >  dégâts) {
        return totalRate / neuronCount;
    }
    return 0.0f;
}

void BrainStatistics::performLogStatus() const {
    NLM_LOG_INFO("Brain Status:");
    NLM_LOG_INFO("  Total neurons: " + std::to_string(performTotalNeuronCount()));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(performTotalSynapseCount()));
    NLM_LOG_INFO("  Active neurons: " + std::to_string(performActiveNeuronCount()));
    NLM_LOG_INFO("  Firing neurons: " + std::to_string(performFiringNeuronCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(performAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("  E/I ratio: " + std::to_string(performExcitationInhibitionRatio()));
    NLM_LOG_INFO("  Total spikes: " + std::to_string(performTotalSpikeCount()));
    NLM_LOG_INFO("  Pending spikes: " + std::to_string(performPendingSpikeEventCount()));
}

} // namespace nlm