#include "brain/BrainManagement.hpp"

namespace nlm {

bool BrainManagement::initialize(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                const Config& config,
                                std::vector<Neuron*>& sensoryNeurons,
                                std::vector<Neuron*>& motorNeurons) {
    try {
        // Validate input
        if (regions.empty()) {
            NLM_LOG_ERROR("Cannot initialize brain with no regions");
            return false;
        }
        
        // Create regions
        for (size_t i = 0; i < regions.size(); ++i) {
            auto regionId = RegionId(i + 1);
            auto region = std::make_unique<NeuralRegion>(regionId, "Region_" + std::to_string(i + 1));
            
            // Get configuration values for this region
            size_t neuronCount = config.getOr<size_t>("neuron_count", 1000);
            size_t regionCount = config.getOr<size_t>("region_count", regions.size());
            size_t neuronsPerRegion = neuronCount / regionCount;
            
            // Add populations with balanced neuron types
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    motorNeurons.push_back(neuron);
                }
            }
            
            // Store region in output vector
            regions[i] = std::move(region);
            
            NLM_LOG_INFO("Created region " + std::to_string(regionId.index()) + 
                        ": " + std::to_string(regions[i]->getPopulationCount()) + 
                        " populations, " + std::to_string(regions[i]->getTotalNeuronCount()) + 
                        " neurons");
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain initialization error: ") + e.what());
        return false;
    }
}

void BrainManagement::initializeConnectivity(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                            RandomGenerator& rng,
                                            float connectionProbability,
                                            float meanWeight,
                                            float stdDevWeight) {
    for (auto& region : regions) {
        if (region) {
            region->initializeRandomConnectivity(rng, connectionProbability, meanWeight, stdDevWeight);
        }
    }
}

void BrainManagement::collectIOElements(std::vector<Neuron*>& sensoryNeurons,
                                       std::vector<Neuron*>& motorNeurons,
                                       std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    // Clear existing lists
    sensoryNeurons.clear();
    motorNeurons.clear();
    
    // Collect neurons from all regions
    for (auto& region : regions) {
        if (!region) continue;
        
        for (auto& population : region->getPopulations()) {
            if (!population) continue;
            
            NeuronType type = population->getNeuronType();
            for (auto* neuron : population->getNeurons()) {
                if (type == NeuronType::Sensory) {
                    sensoryNeurons.push_back(neuron);
                } else if (type == NeuronType::Motor) {
                    motorNeurons.push_back(neuron);
                }
            }
        }
    }
}

}