// BrainUtils.cpp - Implementation of Brain utility functions

#include "BrainUtils.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Create brain regions with specified count
void createRegions(Brain::Impl* impl, size_t regionCount) {
    for (size_t i = 0; i < regionCount; ++i) {
        impl->addRegion("Region_" + std::to_string(i + 1));
    }
}

// Setup neurons across regions with default population distribution
void setupNeurons(Brain::Impl* impl, size_t neuronCount, size_t regionCount) {
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = impl->getRegion(RegionId(i + 1));
        if (region) {
            // Add populations: sensory (25%), internal (50%), motor (25%)
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            collectSensoryAndMotorNeurons(impl, neuronsPerRegion, i);
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
}

// Setup connectivity between neurons with random weights
void setupConnectivity(Brain::Impl* impl, size_t regionCount, float connectionProbability) {
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = impl->getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*impl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

// Collect sensory and motor neurons for external input/output
void collectSensoryAndMotorNeurons(Brain::Impl* impl, size_t neuronsPerRegion, size_t regionIndex) {
    auto* region = impl->getRegion(RegionId(regionIndex + 1));
    if (!region) return;
    
    // Find and collect sensory neurons
    auto sensoryPop = region->getPopulation(region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory));
    if (sensoryPop) {
        for (Neuron* n : sensoryPop->getNeurons()) {
            impl->sensoryNeurons.push_back(n);
        }
    }
    
    // Find and collect motor neurons
    auto motorPop = region->getPopulation(region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor));
    if (motorPop) {
        for (Neuron* n : motorPop->getNeurons()) {
            impl->motorNeurons.push_back(n);
        }
    }
}

// Log comprehensive brain status for debugging and monitoring
void logBrainStatus(const Brain::Impl* impl) {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Modular) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(impl->regions.size()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount(impl)));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount(impl)));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount(impl)));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount(impl)));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount(impl)));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount(impl)));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate(impl)));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio(impl)));
    
    // Log region-specific information
    for (const auto& region : impl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(getAverageSynapticWeight(impl)));
    }
}

// Format log messages with consistent styling
std::string formatLogMessage(const std::string& message) {
    // Add timestamp and formatting
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "[" << std::put_time(std::localtime(&time), "%H:%M:%S") << "] " << message;
    return ss.str();
}

// Validate brain configuration
bool validateConfiguration(Brain::Impl* impl) {
    // Check if configuration has required parameters
    if (!impl->config->has("neuron_count")) {
        NLM_LOG_ERROR("Missing required configuration: neuron_count");
        return false;
    }
    
    if (!impl->config->has("region_count")) {
        NLM_LOG_ERROR("Missing required configuration: region_count");
        return false;
    }
    
    // Validate ranges
    size_t neuronCount = impl->config->getOr<size_t>("neuron_count", 0);
    size_t regionCount = impl->config->getOr<size_t>("region_count", 0);
    
    if (neuronCount == 0) {
        NLM_LOG_ERROR("Invalid configuration: neuron_count must be > 0");
        return false;
    }
    
    if (regionCount == 0) {
        NLM_LOG_ERROR("Invalid configuration: region_count must be > 0");
        return false;
    }
    
    if (regionCount > neuronCount) {
        NLM_LOG_WARNING("Region count exceeds neuron count, may result in empty regions");
    }
    
    return true;
}

// Performance optimization utilities
namespace BrainPerformance {
    
    // Calculate connection density
    float calculateConnectionDensity(const Brain::Impl* impl) {
        size_t totalNeurons = getTotalNeuronCount(impl);
        size_t totalPossibleConnections = totalNeurons * (totalNeurons - 1);
        size_t actualConnections = getTotalSynapseCount(impl);
        
        return totalPossibleConnections > 0 ? 
            static_cast<float>(actualConnections) / totalPossibleConnections : 0.0f;
    }
    
    // Get memory usage estimate
    size_t estimateMemoryUsage(const Brain::Impl* impl) {
        // Rough estimate based on data structures
        size_t regionMemory = impl->regions.size() * sizeof(std::unique_ptr<NeuralRegion>);
        size_t synapseMemory = impl->interRegionConnections.size() * sizeof(InterRegionConnection);
        size_t neuronMemory = 0;
        
        for (const auto& region : impl->regions) {
            neuronMemory += region->estimateMemoryUsage();
        }
        
        return regionMemory + synapseMemory + neuronMemory;
    }
}

} // namespace nlm