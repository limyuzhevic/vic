// brain/BrainAccessors.cpp - Brain accessor implementations
// Contains all accessor methods for the Brain interface

#include "brain/BrainAccessors.hpp"
#include <cmath>
#include <limits>

namespace nlm {

float Brain::Impl::getExcitationInhibitionRatio() const {
    try {
        float totalExcitatory = 0.0f;
        float totalInhibitory = 0.0f;
        
        for (const auto& region : regions) {
            for (const auto& syn : region->getSynapses()) {
                try {
                    float weight = syn->getWeight();
                    if (weight > 0.0f) {
                        totalExcitatory += weight;
                    } else if (weight < 0.0f) {
                        totalInhibitory += std::abs(weight);
                    }
                    // Invalid weights (NaN, infinity) are ignored
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Error getting synapse weight: " + std::string(e.what()));
                    // Continue with next synapse
                }
            }
        }
        
        if (totalInhibitory > 0.0f) {
            return totalExcitatory / totalInhibitory;
        } else if (totalExcitatory > 0.0f) {
            return std::numeric_limits<float>::infinity();
        } else {
            return 0.0f;  // No connections or all weights are zero
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in getExcitationInhibitionRatio: ") + e.what());
        return 0.0f;  // Safe default
    }
}

size_t Brain::Impl::getTotalNeuronCount() const {
    try {
        size_t count = 0;
        for (const auto& region : regions) {
            try {
                count += region->getNeuronCount();
            } catch (const std::exception& e) {
                NLM_LOG_WARNING(std::string("Error getting neuron count for region: ") + e.what());
                // Continue with other regions
            }
        }
        return count;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in getTotalNeuronCount: ") + e.what());
        return 0;  // Safe default
    }
}

size_t Brain::Impl::getTotalSynapseCount() const {
    try {
        size_t count = 0;
        for (const auto& region : regions) {
            for (const auto& neuron : region->getNeurons()) {
                try {
                    count += neuron->getSynapseCount();
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error getting synapse count for neuron: ") + e.what());
                    // Continue with other neurons
                }
            }
        }
        return count;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in getTotalSynapseCount: ") + e.what());
        return 0;  // Safe default
    }
}

size_t Brain::Impl::getActiveNeuronCount() const {
    try {
        size_t count = 0;
        for (const auto& region : regions) {
            for (const auto& neuron : region->getNeurons()) {
                try {
                    if (neuron->isActive()) {
                        count++;
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING(std::string("Error checking neuron active state: ") + e.what());
                    // Continue with other neurons
                }
            }
        }
        return count;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in getActiveNeuronCount: ") + e.what());
        return 0;  // Safe default
    }
}

size_t Brain::Impl::getAverageFiringRate() const {
    try {
        size_t totalSpikes = totalSpikesTotal;
        size_t totalTimeSteps = currentStep + 1;
        
        if (totalTimeSteps > 0) {
            // Ensure no division by zero
            return totalSpikes / totalTimeSteps;
        } else {
            return 0;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in getAverageFiringRate: ") + e.what());
        return 0;  // Safe default
    }
}

size_t Brain::Impl::getTotalSpikeCount() const {
    try {
        return totalSpikesTotal;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in getTotalSpikeCount: ") + e.what());
        return 0;  // Safe default
    }
}

size_t Brain::Impl::getPendingSpikeEventCount() const {
    try {
        if (!spikeSystem) {
            NLM_LOG_WARNING("Spike system not initialized for pending spike count");
            return 0;
        }
        
        try {
            size_t pendingSpikes = spikeSystem->getPendingSpikeCount();
            size_t delayedSpikes = spikeSystem->getPendingDelayedCount();
            return pendingSpikes + delayedSpikes;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error getting spike system counts: ") + e.what());
            return 0;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in getPendingSpikeEventCount: ") + e.what());
        return 0;  // Safe default
    }
}

// ========== MEMORY SYSTEM ACCESSORS ==========
NeuralWorkingMemory* Brain::Impl::getWorkingMemory() {
    return workingMemory.get();
}

NeuralEpisodicMemory* Brain::Impl::getEpisodicMemory() {
    return episodicMemory.get();
}

NeuralAssociativeMemory* Brain::Impl::getAssociativeMemory() {
    return associativeMemory.get();
}

// ========== PREDICTION SYSTEM ACCESSORS ==========
PredictionSystem* Brain::Impl::getPredictionSystem() {
    return predictionSystem.get();
}

// ========== COGNITION SYSTEM ACCESSORS ==========
NeuralPlanner* Brain::Impl::getPlanner() {
    return planner.get();
}

ConceptFormation* Brain::Impl::getConceptFormation() {
    return conceptFormation.get();
}

AttentionalSelection* Brain::Impl::getAttention() {
    return attention.get();
}

// ========== DEVELOPMENT SYSTEM ACCESSORS ==========
DevelopmentSystem* Brain::Impl::getDevelopmentSystem() {
    return developmentSystem.get();
}

DevelopmentalStage Brain::Impl::getDevelopmentalStage() const {
    return developmentalStage;
}

void Brain::Impl::setDevelopmentalStage(DevelopmentalStage stage) {
    developmentalStage = stage;
}

// ========== NEUROMODULATION SYSTEM ACCESSORS ==========
Dopamine* Brain::Impl::getDopamine() {
    return dopamine.get();
}

Curiosity* Brain::Impl::getCuriosity() {
    return curiosity.get();
}

Novelty* Brain::Impl::getNovelty() {
    return novelty.get();
}

PredictionError* Brain::Impl::getPredictionErrorSignal() {
    return predictionError.get();
}

// ========== STATISTICS ==========
std::shared_ptr<const Config> Brain::Impl::getConfig() const {
    return config;
}

RandomGenerator* Brain::Impl::getRandomGenerator() {
    return rng.get();
}

SpikeSystem* Brain::Impl::getSpikeSystem() {
    return spikeSystem.get();
}

STDP* Brain::Impl::getSTDP() {
    return stdp.get();
}

Hebbian* Brain::Impl::getHebbian() {
    return hebbian.get();
}

StructuralPlasticity* Brain::Impl::getStructuralPlasticity() {
    return structuralPlasticity.get();
}

// ========== REGION ACCESSORS ==========
size_t Brain::Impl::getRegionCount() const {
    return regions.size();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::Impl::getRegions() const {
    return regions;
}

std::vector<RegionId> Brain::Impl::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(regions.size());
    for (const auto& region : regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

NeuralRegion* Brain::Impl::getRegion(RegionId id) {
    for (const auto& region : regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::Impl::getRegion(RegionId id) const {
    for (const auto& region : regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

RegionId Brain::Impl::addRegion(const std::string& name) {
    RegionId newId = nextRegionId++;
    regions.push_back(std::make_unique<NeuralRegion>(newId, name));
    return newId;
}

void Brain::Impl::addInterRegionConnection(RegionId source, RegionId target,
                                         float weight, Delay delay) {
    interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::Impl::removeInterRegionConnection(RegionId source, RegionId target) {
    auto it = std::remove_if(interRegionConnections.begin(), interRegionConnections.end(),
        [source, target](const InterRegionConnection& conn) {
            return conn.sourceRegion == source && conn.targetRegion == target;
        });
    interRegionConnections.erase(it, interRegionConnections.end());
}

} // namespace nlm