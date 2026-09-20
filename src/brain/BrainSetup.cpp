// BrainSetup.cpp - handles brain initialization and region setup
#include "BrainSetup.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/NeuralRegion.hpp"

namespace nlm {

bool BrainSetup::initializeBrain(Brain::Impl* pImpl) {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    if (!createRegions(pImpl, neuronCount, regionCount, connectionProbability)) {
        return false;
    }
    
    collectSensoryMotorNeurons(pImpl);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(pImpl->regions.size() > 0 ? 
        pImpl->regions[0]->getTotalNeuronCount() : 0));
    NLM_LOG_INFO("Total synapses: " + std::to_string(pImpl->regions.size() > 0 ? 
        pImpl->regions[0]->getSynapseCount() : 0));
    
    return true;
}

bool BrainSetup::createRegions(Brain::Impl* pImpl, size_t neuronCount, size_t regionCount, float connectionProbability) {
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        RegionId id(pImpl->nextRegionId++);
        auto region = std::make_unique<NeuralRegion>(id, "Region_" + std::to_string(i + 1));
        pImpl->regions.push_back(std::move(region));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = pImpl->regions[i].get();
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = pImpl->regions[i].get();
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    return true;
}

void BrainSetup::collectSensoryMotorNeurons(Brain::Impl* pImpl) {
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            // Check neuron type by examining the neuron (simplified approach)
            // In a real implementation, we'd need a better way to identify neuron types
            if (neuron->getId().index() % 4 == 0) {
                pImpl->sensoryNeurons.push_back(neuron);
            } else if (neuron->getId().index() % 4 == 2) {
                pImpl->motorNeurons.push_back(neuron);
            }
        }
    }
}

} // namespace nlm