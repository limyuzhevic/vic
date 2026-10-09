// brain/BrainAccessors.cpp - Brain accessor implementations
// Contains all accessor methods for the Brain interface

#include "brain/BrainAccessors.hpp"
#include <cmath>
#include <limits>
#include <optional>

// Modern C++ constants for better code readability and maintainability
constexpr float EPSILON_FLOAT = 1e-6f;
constexpr float NANOS_TO_SECONDS = 1e-9f;
constexpr float DEFAULT_WEIGHT_THRESHOLD = 0.01f;

namespace nlm {

/**
 * @brief Calculate the excitation/inhibition ratio of the network
 * 
 * This ratio provides insight into the overall balance of the neural network:
 * - Ratio > 1.0: Excitatory influence dominates
 * - Ratio < 1.0: Inhibitory influence dominates
 * - Ratio = 1.0: Balanced excitation and inhibition
 * - Ratio = infinity: Only excitatory connections present
 * - Ratio = 0.0: Only inhibitory connections present or no connections
 * 
 * The E/I ratio is important for:
 * - Network stability analysis
 * - Understanding computational capabilities
 * - Predicting dynamic behaviors
 * - Monitoring learning progress
 * 
 * @return Excitation/inhibition ratio. Returns infinity if only excitatory
 *         connections exist, 0.0 if only inhibitory connections exist or no
 *         connections, or the actual ratio otherwise.
 * 
 * @note This metric is used to monitor network health and computational state.
 *       Healthy networks typically have ratios between 0.8 and 2.0, though
 *       this varies by task and network architecture.
 * 
 * @see nlm::Brain::Impl::getExcitationInhibitionRatio()
 */
float Brain::Impl::getExcitationInhibitionRatio() const {
    try {
        // ========== SYNAPTIC ANALYSIS ==========
        // Calculate excitation-to-inhibition ratio by summing synaptic weights
        // Excitation: positive weights, Inhibition: negative weights (absolute value)
        
        float totalExcitatory = 0.0f;
        float totalInhibitory = 0.0f;
        
        // Iterate through all regions and analyze synaptic connections
        for (const auto& region : regions) {
            // Process all synapses in the current region
            for (const auto& synapse : region->getSynapses()) {
                // Extract synaptic weight with bounds checking
                try {
                    float weight = synapse->getWeight();
                    
                    if (weight > EPSILON_FLOAT) {
                        // Excitatory synapse: positive contribution to total excitation
                        totalExcitatory += weight;
                    } else if (weight < -EPSILON_FLOAT) {
                        // Inhibitory synapse: absolute value contributes to total inhibition
                        totalInhibitory += std::abs(weight);
                    }
                    
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Error accessing synapse weight: " + std::string(e.what()));
                    // Continue processing other synapses
                }
            }
        }
        
        // Calculate final excitation-inhibition ratio with edge case handling
        if (totalInhibitory > EPSILON_FLOAT) {
            return totalExcitatory / totalInhibitory;
        } else if (totalExcitatory > EPSILON_FLOAT) {
            return std::numeric_limits<float>::infinity();
        } else {
            return 0.0f;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("System error in excitation-inhibition ratio calculation: ") + e.what());
        return 0.0f;
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
std::shared_ptr<NeuralWorkingMemory> Brain::Impl::getWorkingMemory() {
    return workingMemory.)
}

std::shared_ptr<NeuralEpisodicMemory> Brain::Impl::getEpisodicMemory() {
    return episodicMemory.)
}

std::shared_ptr<NeuralAssociativeMemory> Brain::Impl::getAssociativeMemory() {
    return associativeMemory.)
}

// ========== PREDICTION SYSTEM ACCESSORS ==========
std::shared_ptr<PredictionSystem> Brain::Impl::getPredictionSystem() {
    return predictionSystem.)
}

// ========== COGNITION SYSTEM ACCESSORS ==========
std::shared_ptr<NeuralPlanner> Brain::Impl::getPlanner() {
    return planner.)
}

std::shared_ptr<ConceptFormation> Brain::Impl::getConceptFormation() {
    return conceptFormation.)
}

std::shared_ptr<AttentionalSelection> Brain::Impl::getAttention() {
    return attention.)
}

// ========== DEVELOPMENT SYSTEM ACCESSORS ==========
std::shared_ptr<DevelopmentSystem> Brain::Impl::getDevelopmentSystem() {
    return developmentSystem.)
}

DevelopmentalStage Brain::Impl::getDevelopmentalStage() const {
    return developmentalStage;
}

void Brain::Impl::setDevelopmentalStage(DevelopmentalStage stage) {
    developmentalStage = stage;
}

// ========== NEUROMODULATION SYSTEM ACCESSORS ==========
std::shared_ptr<Dopamine> Brain::Impl::getDopamine() {
    return dopamine.)
}

std::shared_ptr<Curiosity> Brain::Impl::getCuriosity() {
    return curiosity.get();
}

std::shared_ptr<Novelty> Brain::Impl::getNovelty() {
    return novelty.get();
}

std::shared_ptr<PredictionError> Brain::Impl::getPredictionErrorSignal() {
    return predictionError.get();
}

// ========== STATISTICS ==========
std::shared_ptr<const Config> Brain::Impl::getConfig() const {
    return config;
}

std::shared_ptr<RandomGenerator> Brain::Impl::getRandomGenerator() {
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