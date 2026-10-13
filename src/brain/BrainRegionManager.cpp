#include "BrainRegionManager.hpp"
#include "../Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

BrainRegionManager::BrainRegionManager(std::shared_ptr<Config> config)
    : config(config)
    , rng(nullptr)
    , neuronCount(0)
    , regionCount(0)
    , connectionProbability(0.0f)
    , totalNeuronCount(0)
    , totalSynapseCount(0)
    , averageSynapticWeight(0.0f)
{
    // Initialize random generator
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = config->get<uint64_t>("random_seed")) {
        seed = *seedOpt;
    }
    rng = std::make_unique<RandomGenerator>(seed);
}

void BrainRegionManager::createRegions(Brain* brain, size_t neuronCount, size_t regionCount, 
                                      float connectionProbability) {
    this->neuronCount = neuronCount;
    this->regionCount = regionCount;
    this->connectionProbability = connectionProbability;
    
    // Clear existing regions
    regions.clear();
    interRegionConnections.clear();
    
    // Create regions based on configuration
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion(brain, "Region_" + std::to_string(i + 1));
    }
    
    // Distribute neurons across regions
    distributeNeurons(brain, neuronCount, regionCount);
    
    // Setup connectivity between neurons
    setupConnectivity(brain, connectionProbability);
    
    // Collect sensory and motor neurons for I/O
    collectSensoryMotorNeurons(brain);
}

void BrainRegionManager::createNeuronsInRegions(Brain* brain, size_t neuronCount, size_t regionCount) {
    distributeNeurons(brain, neuronCount, regionCount);
}

void BrainRegionManager::setupConnectivity(Brain* brain, float connectionProbability) {
    initializeRegionConnectivity(brain, connectionProbability);
}

void BrainRegionManager::collectSensoryMotorNeurons(Brain* brain) {
    collectSensoryNeurons(brain);
    collectMotorNeurons(brain);
}

NeuralRegion* BrainRegionManager::getRegion(Brain* brain, RegionId id) {
    for (auto& region : regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* BrainRegionManager::getRegion(Brain* brain, RegionId id) const {
    for (const auto& region : regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

RegionId BrainRegionManager::addRegion(Brain* brain, const std::string& name) {
    RegionId id(nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    regions.push_back(std::move(region));
    return id;
}

void BrainRegionManager::removeRegion(Brain* brain, RegionId id) {
    regions.erase(
        std::remove_if(regions.begin(), regions.end(),
                      [id](const std::unique_ptr<NeuralRegion>& region) {
                          return region->getId() == id;
                      }),
        regions.end()
    );
}

void BrainRegionManager::addInterRegionConnection(Brain* brain, RegionId source, RegionId target, 
                                                 float weight, Delay delay) {
    interRegionConnections.emplace_back(source, target, weight, delay);
}

void BrainRegionManager::removeInterRegionConnection(Brain* brain, RegionId source, RegionId target) {
    interRegionConnections.erase(
        std::remove_if(interRegionConnections.begin(),
                      interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        interRegionConnections.end()
    );
}

void BrainRegionManager::resetRegions(Brain* brain) {
    for (auto& region : regions) {
        region->reset();
    }
}

void BrainRegionManager::initializeRegionConnectivity(Brain* brain, float connectionProbability) {
    for (auto& region : regions) {
        region->initializeRandomConnectivity(*rng, connectionProbability, 0.2f, 0.1f);
    }
}

void BrainRegionManager::distributeNeurons(Brain* brain, size_t neuronCount, size_t regionCount) {
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(brain, RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Store region statistics
            totalNeuronCount += region->getTotalNeuronCount();
            totalSynapseCount += region->getSynapseCount();
            averageSynapticWeight += region->getAverageSynapticWeight();
        }
    }
    
    if (regionCount > 0) {
        averageSynapticWeight /= regionCount;
    }
}

void BrainRegionManager::collectSensoryNeurons(Brain* brain) {
    // This will be called by BrainInitializer to collect neurons for I/O
}

void BrainRegionManager::collectMotorNeurons(Brain* brain) {
    // This will be called by BrainInitializer to collect neurons for I/O
}

void BrainRegionManager::updateRegionStatistics(Brain* brain) {
    totalNeuronCount = 0;
    totalSynapseCount = 0;
    averageSynapticWeight = 0.0f;
    
    for (const auto& region : regions) {
        totalNeuronCount += region->getTotalNeuronCount();
        totalSynapseCount += region->getSynapseCount();
        averageSynapticWeight += region->getAverageSynapticWeight();
    }
    
    if (regions.size() > 0) {
        averageSynapticWeight /= regions.size();
    }
}