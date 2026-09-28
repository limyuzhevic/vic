#include "BrainRegion.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

RegionId BrainRegion::addRegion(Brain::Impl& pImpl, const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* BrainRegion::getRegion(Brain::Impl& pImpl, RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* BrainRegion::getRegion(const Brain::Impl& pImpl, RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t BrainRegion::getRegionCount(const Brain::Impl& pImpl) const {
    return pImpl->regions.size();
}

std::vector<RegionId> BrainRegion::getRegionIds(const Brain::Impl& pImpl) const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

void BrainRegion::addInterRegionConnection(Brain::Impl& pImpl, RegionId source, RegionId target,
                                         float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void BrainRegion::removeInterRegionConnection(Brain::Impl& pImpl, RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

const std::vector<std::unique_ptr<NeuralRegion>>& BrainRegion::getRegions(const Brain::Impl& pImpl) const {
    return pImpl->regions;
}

size_t BrainRegion::getTotalNeuronCount(const Brain::Impl& pImpl) const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t BrainRegion::getTotalSynapseCount(const Brain::Impl& pImpl) const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t BrainRegion::getActiveNeuronCount(const Brain::Impl& pImpl) const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t BrainRegion::getFiringNeuronCount(const Brain::Impl& pImpl) const {
    return pImpl->getFiringNeuronCount();
}

float BrainRegion::getAverageFiringRate(const Brain::Impl& pImpl) const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

void BrainRegion::initializeNeuronsInRegions(Brain::Impl& pImpl) {
    // Initialize neurons in all regions
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto* region = pImpl->getRegion(RegionId(i + 1));
        if (region) {
            // Add populations
            size_t neuronsPerRegion = pImpl->config->getOr<size_t>("neuron_count", 1000) / pImpl->regions.size();
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
        }
    }
}

void BrainRegion::initializeConnectivityInRegions(Brain::Impl& pImpl, float connectionProbability) {
    // Initialize connectivity in all regions
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto* region = pImpl->getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void BrainRegion::collectNeuronsForIO(Brain::Impl& pImpl) {
    // Collect neurons for I/O operations
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    pImpl->motorNeurons.push_back(n);
                }
            } else if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(n);
                }
            }
        }
    }
}

void BrainRegion::logRegionInitializationStats(const Brain::Impl& pImpl) const {
    NLM_LOG_INFO("Region initialization complete");
    NLM_LOG_INFO("Total neurons: " + std::to_string(pImpl->getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(pImpl->getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
}

} // namespace nlm
