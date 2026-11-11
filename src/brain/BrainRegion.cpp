#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void Brain::Impl::addRegion(std::unique_ptr<NeuralRegion> region) {
    if (region) {
        regions.push_back(std::move(region));
    }
}

const NeuralRegion* Brain::Impl::getRegion(RegionId regionId) const {
    auto it = std::find_if(regions.begin(), regions.end(),
        [regionId](const std::unique_ptr<NeuralRegion>& r) { return r->getId() == regionId; });
    return it != regions.end() ? it->get() : nullptr;
}

void Brain::Impl::addInterRegionConnection(const InterRegionConnection& connection) {
    interRegionConnections.push_back(connection);
}

void Brain::Impl::removeInterRegionConnection(const InterRegionConnection& connection) {
    auto it = std::find_if(interRegionConnections.begin(), interRegionConnections.end(),
        [&connection](const InterRegionConnection& c) { return c.sourceId == connection.sourceId && c.targetId == connection.targetId; });
    if (it != interRegionConnections.end()) {
        interRegionConnections.erase(it);
    }
}

std::vector<RegionId> Brain::Impl::getRegionIds() const {
    std::vector<RegionId> ids;
    for (const auto& region : regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

void Brain::Impl::removeRegion(RegionId regionId) {
    auto it = std::remove_if(regions.begin(), regions.end(),
        [regionId](const std::unique_ptr<NeuralRegion>& r) { return r->getId() == regionId; });
    if (it != regions.end()) {
        regions.erase(it, regions.end());
    }
}

void Brain::Impl::addNeuronToRegion(NeuronId neuronId, RegionId regionId, PopulationId populationId) {
    const NeuralRegion* region = getRegion(regionId);
    if (region) {
        region->addNeuron(neuronId, populationId);
    }
}

size_t Brain::Impl::getTotalNeuronCount() const {
    size_t count = 0;
    for (const auto& region : regions) {
        for (const auto& population : region->getPopulations()) {
            count += population->getNeuronCount();
        }
    }
    return count;
}

size_t Brain::Impl::getTotalSynapseCount() const {
    size_t count = 0;
    for (const auto& region : regions) {
        for (const auto& population : region->getPopulations()) {
            count += population->getSynapseCount();
        }
    }
    return count;
}

float Brain::Impl::getAverageFiringRate() const {
    if (getTotalNeuronCount() == 0) return 0.0f;
    
    float totalRate = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& region : regions) {
        for (const auto& population : region->getPopulations()) {
            for (const Neuron* neuron : population->getNeurons()) {
                if (neuron) {
                    totalRate += neuron->getFiringRate();
                    neuronCount++;
                }
            }
        }
    }
    
    return totalRate / neuronCount;
}

float Brain::Impl::getExcitatoryInhibitionRatio() const {
    float excitatory = 0.0f, inhibitory = 0.0f;
    
    for (const auto& region : regions) {
        for (const auto& population : region->getPopulations()) {
            for (const Neuron* neuron : population->getNeurons()) {
                if (neuron) {
                    const NeuralPopulation* pop = population.get();
                    if (pop->getNeuronType() == NeuronType::Excitatory) {
                        excitatory += neuron->getFiringRate();
                    } else if (pop->getNeuronType() == NeuronType::Inhibitory) {
                        inhibitory += neuron->getFiringRate();
                    }
                }
            }
        }
    }
    
    return inhibitory > 0.0f ? excitatory / inhibitory : 0.0f;
}

std::size_t Brain::Impl::getMostActiveNeuronCount() const {
    std::size_t count = 0;
    float maxRate = -1.0f;
    
    for (const auto& region : regions) {
        for (const auto& population : region->getPopulations()) {
            for (const Neuron* neuron : population->getNeurons()) {
                if (neuron) {
                    float rate = neuron->getFiringRate();
                    if (rate > maxRate) {
                        maxRate = rate;
                        count = 1;
                    } else if (std::abs(rate - maxRate) < 0.01f) {
                        count++;
                    }
                }
            }
        }
    }
    
    return count;
}

} // namespace nlm