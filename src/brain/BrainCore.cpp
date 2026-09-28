#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <limits>

namespace nlm {

struct BrainCore::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Simulation parameters
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    
    Impl(std::shared_ptr<Config> cfg, std::unique_ptr<RandomGenerator> r)
        : config(cfg), rng(std::move(r)) {}
    
    void createRegions() {
        regions.clear();
        
        for (size_t i = 0; i < regionCount; ++i) {
            RegionId id(i + 1);
            auto region = std::make_unique<NeuralRegion>(id, "Region_" + std::to_string(i + 1));
            regions.push_back(std::move(region));
        }
    }
    
    void createNeurons() {
        size_t neuronsPerRegion = neuronCount / regionCount;
        
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = regions[i].get();
            if (!region) continue;
            
            // Create populations
            size_t sensoryCount = neuronsPerRegion / 4;
            size_t internalCount = neuronsPerRegion / 2;
            size_t motorCount = neuronsPerRegion / 4;
            
            region->addPopulation(sensoryCount, NeuronType::Sensory);
            region->addPopulation(internalCount, NeuronType::Internal);
            region->addPopulation(motorCount, NeuronType::Motor);
        }
    }
    
    void initializeConnectivity() {
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = regions[i].get();
            if (region) {
                region->initializeRandomConnectivity(*rng, connectionProbability, 0.2f, 0.1f);
            }
        }
    }
};

BrainCore::BrainCore(std::shared_ptr<Config> config, std::unique_ptr<RandomGenerator> rng)
    : pImpl(std::make_unique<Impl>(config, std::move(rng))) {}
{
}

bool BrainCore::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain Core...");
    
    // Create regions
    pImpl->createRegions();
    
    // Create neurons in regions
    pImpl->createNeurons();
    
    // Initialize connectivity
    pImpl->initializeConnectivity();
    
    NLM_LOG_INFO("Brain Core initialization complete");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    
    return true;
}

RegionId BrainCore::addRegion(const std::string& name) {
    RegionId id(pImpl->regions.size() + 1);
    auto region = std::make_unique<NeuralRegion>(id, name.empty() ? "Region_" + std::to_string(id.index()) : name);
    auto* rawPtr = region.get();
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* BrainCore::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* BrainCore::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t BrainCore::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> BrainCore::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

size_t BrainCore::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t BrainCore::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t BrainCore::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

float BrainCore::getExcitationInhibitionRatio() const {
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

const std::vector<std::unique_ptr<NeuralRegion>>& BrainCore::getRegions() const {
    return pImpl->regions;
}

void BrainCore::addInterRegionConnection(RegionId source, RegionId target, 
                                        float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void BrainCore::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

std::shared_ptr<const Config> BrainCore::getConfig() const {
    return pImpl->config;
}

RandomGenerator* BrainCore::getRandomGenerator() {
    return pImpl->rng.get();
}

const RandomGenerator* BrainCore::getRandomGenerator() const {
    return pImpl->rng.get();
}

void BrainCore::reset() {
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    pImpl->interRegionConnections.clear();
}

void BrainCore::setNeuronCount(size_t count) {
    pImpl->neuronCount = count;
}

void BrainCore::setRegionCount(size_t count) {
    pImpl->regionCount = count;
}

void BrainCore::setConnectionProbability(float prob) {
    pImpl->connectionProbability = prob;
}

} // namespace nlm