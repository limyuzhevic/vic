#include "brain/NeuralCore.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <algorithm>
#include <numeric>
#include <random>

namespace nlm {

struct NeuralCore::Impl {
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Random generator for connectivity
    std::unique_ptr<RandomGenerator> rng;
    
    // Configuration
    float connectionProbability;
    float synapticExcitationWeight;
    float synapticInhibitionWeight;
    
    Impl()
        : connectionProbability(0.1f), synapticExcitationWeight(0.2f), synapticInhibitionWeight(0.1f) {
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Initialize random generator
        std::random_device rd;
        std::mt19937 gen(rd());
        rng = std::make_unique<RandomGenerator>(std::uniform_int_distribution<uint64_t>(0, UINT64_MAX)(gen));
    }
    
    uint64_t getSeed() const {
        // For now, use fixed seed. In practice, should come from config
        return 42;
    }
};

NeuralCore::NeuralCore() : pImpl(std::make_unique<Impl>()) {}
NeuralCore::~NeuralCore() = default;

NeuralCore::NeuralCore(NeuralCore&& other) noexcept = default;
NeuralCore& NeuralCore::operator=(NeuralCore&& other) noexcept = default;

RegionId NeuralCore::addRegion(const std::string& name) {
    RegionId id(static_cast<RegionId::Index>(pImpl->regions.size() + 1));
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* NeuralCore::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* NeuralCore::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t NeuralCore::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> NeuralCore::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& NeuralCore::getRegions() const {
    return pImpl->regions;
}

void NeuralCore::addInterRegionConnection(RegionId source, RegionId target, 
                                         float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void NeuralCore::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

size_t NeuralCore::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t NeuralCore::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t NeuralCore::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t NeuralCore::getFiringNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getFiringNeuronCount();
    }
    return total;
}

float NeuralCore::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

float NeuralCore::getExcitationInhibitionRatio() const {
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

void NeuralCore::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void NeuralCore::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

std::vector<Neuron*> NeuralCore::getNeuronsByType(NeuronType type) const {
    std::vector<Neuron*> neurons;
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neurons.push_back(neuron);
                }
            }
        }
    }
    return neurons;
}

std::vector<Neuron*> NeuralCore::getAllNeurons() const {
    std::vector<Neuron*> neurons;
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neurons.push_back(neuron);
            }
        }
    }
    return neurons;
}

SpikeSystem* NeuralCore::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* NeuralCore::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* NeuralCore::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* NeuralCore::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* NeuralCore::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

void NeuralCore::stepNeurons(Timestamp currentTime, TimestepDuration timestep) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void NeuralCore::detectAndProcessSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // This is a simplified version - real implementation would integrate with SpikeSystem
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < 0.002f * 2.0f);

                if (justFired) {
                    // In real implementation, this would queue through SpikeSystem
                    // For now, just record statistics
                }
            }
        }
    }
}

void NeuralCore::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
}

void NeuralCore::updateSynapticEligibility(SimulationStep currentStep, Timestamp currentTime) {
    // STDP and Hebbian plasticity updates would go here
    // This is extracted from Brain::step() method
}

void NeuralCore::resetAllRegions() {
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
}

void NeuralCore::collectNeuralStatistics() const {
    // Statistics collection extracted from Brain::logStatus()
}

void NeuralCore::setConnectionProbability(float probability) {
    pImpl->connectionProbability = probability;
}

void NeuralCore::setSynapticWeights(float excitation, float inhibition) {
    pImpl->synapticExcitationWeight = excitation;
    pImpl->synapticInhibitionWeight = inhibition;
}

} // namespace nlm