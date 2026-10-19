#include "BrainInitializer.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct BrainInitializer::Impl {
    // Helper methods
    void createRegions(Brain* brain, size_t regionCount, const std::string& baseName);
    void createNeurons(Brain* brain, size_t neuronCount, size_t regionCount);
    void setupConnectivity(Brain* brain, float connectionProbability);
    void collectSensoryMotorNeurons(Brain* brain);
};

// Helper implementations
void BrainInitializer::Impl::createRegions(Brain* brain, size_t regionCount, const std::string& baseName) {
    for (size_t i = 0; i < regionCount; ++i) {
        brain->addRegion(baseName + "_" + std::to_string(i + 1));
    }
}

void BrainInitializer::Impl::createNeurons(Brain* brain, size_t neuronCount, size_t regionCount) {
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(static_cast<RegionId>(i + 1));
        if (!region) continue;
        
        // Add populations of different neuron types
        size_t sensoryNeurons = neuronsPerRegion / 4;
        size_t internalNeurons = neuronsPerRegion / 2;
        size_t motorNeurons = neuronsPerRegion / 4;
        
        if (sensoryNeurons > 0) {
            region->addPopulation(sensoryNeurons, NeuronType::Sensory);
        }
        if (internalNeurons > 0) {
            region->addPopulation(internalNeurons, NeuronType::Internal);
        }
        if (motorNeurons > 0) {
            region->addPopulation(motorNeurons, NeuronType::Motor);
        }
    }
}

void BrainInitializer::Impl::setupConnectivity(Brain* brain, float connectionProbability) {
    size_t regionCount = brain->getRegionCount();
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(static_cast<RegionId>(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*brain->getRandomGenerator(), 
                                                 connectionProbability, 0.2f, 0.1f);
        }
    }
}

void BrainInitializer::Impl::collectSensoryMotorNeurons(Brain* brain) {
    // Collect sensory neurons for input injection
    for (size_t i = 0; i < brain->getRegionCount(); ++i) {
        auto* region = brain->getRegion(static_cast<RegionId>(i + 1));
        if (!region) continue;
        
        // In real implementation, would get sensory neuron populations
        // and add them to brain's input handling
    }
}

BrainInitializer::BrainInitializer()
    : pImpl(new Impl)
{
}

BrainInitializer::~BrainInitializer() = default;

bool BrainInitializer::initialize(Brain* brain) {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = brain->getConfig()->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = brain->getConfig()->getOr<size_t>("region_count", 1);
    float connectionProbability = brain->getConfig()->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions and neurons
    setupRegions(brain, regionCount, "Region_");
    createNeurons(brain, neuronCount, regionCount);
    collectSensoryMotorNeurons(brain);
    
    // Initialize connectivity with random weights
    setupConnectivity(brain, connectionProbability);
    
    // Display region creation summary
    NLM_LOG_INFO("Brain regions created:");
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(static_cast<RegionId>(i + 1));
        if (region) {
            NLM_LOG_INFO("  Region " + std::to_string(i + 1) + 
                         ": " + std::to_string(region->getPopulationCount()) + 
                         " populations, " + std::to_string(region->getTotalNeuronCount()) + 
                         " neurons");
        }
    }
    
    NLM_LOG_INFO("Brain initialization complete.");
    return true;
}

void BrainInitializer::setupRegions(Brain* brain, size_t regionCount, const std::string& baseName) {
    for (size_t i = 0; i < regionCount; ++i) {
        brain->addRegion(baseName + "_" + std::to_string(i + 1));
    }
}

void BrainInitializer::createNeurons(Brain* brain, size_t neuronCount, size_t regionCount) {
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(static_cast<RegionId>(i + 1));
        if (!region) continue;
        
        // Add populations of different neuron types
        size_t sensoryNeurons = neuronsPerRegion / 4;
        size_t internalNeurons = neuronsPerRegion / 2;
        size_t motorNeurons = neuronsPerRegion / 4;
        
        if (sensoryNeurons > 0) {
            region->addPopulation(sensoryNeurons, NeuronType::Sensory);
        }
        if (internalNeurons > 0) {
            region->addPopulation(internalNeurons, NeuronType::Internal);
        }
        if (motorNeurons > 0) {
            region->addPopulation(motorNeurons, NeuronType::Motor);
        }
    }
}

void BrainInitializer::initializeIntegratedSystems(Brain* brain, size_t neuronCount) {
    // This method would be implemented in the integration files
    // For now, it's a placeholder
}

void BrainInitializer::configurePlasticitySystems(Brain* brain) {
    // This method would be implemented in the integration files
    // For now, it's a placeholder
}

void BrainInitializer::configureIntegrationIntervals(Brain* brain) {
    // This method would be implemented in the integration files
    // For now, it's a placeholder
}

void BrainInitializer::configureCheckpointManager(Brain* brain) {
    // This method would be implemented in the integration files
    // For now, it's a placeholder
}

void BrainInitializer::logInitializationStatus(Brain* brain) {
    // This method would be implemented in the integration files
    // For now, it's a placeholder
}

} // namespace nlm