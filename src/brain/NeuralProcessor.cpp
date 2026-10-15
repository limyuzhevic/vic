// NeuralProcessor implementation
#include "NeuralProcessor.hpp"
#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

struct NeuralProcessor::Impl {
    std::shared_ptr<SpikeSystem> spikeSystem;
    std::shared_ptr<STDP> stdp;
    std::shared_ptr<Hebbian> hebbian;
    std::shared_ptr<StructuralPlasticity> structuralPlasticity;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::unordered_map<RegionId, InterRegionConnection> interRegionConnections;
};

NeuralProcessor::NeuralProcessor(std::shared_ptr<SpikeSystem> spikeSystem,
                               std::shared_ptr<STDP> stdp,
                               std::shared_ptr<Hebbian> hebbian,
                               std::shared_ptr<StructuralPlasticity> structuralPlasticity)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->spikeSystem = spikeSystem;
    pImpl->stdp = stdp;
    pImpl->hebbian = hebbian;
    pImpl->structuralPlasticity = structuralPlasticity;
}

void NeuralProcessor::step(SimulationStep currentStep, Timestamp currentTime) {
    // Update spike system
    pImpl->spikeSystem->step(currentStep, currentTime);
    
    // Update neural regions
    for (auto& region : pImpl->regions) {
        region->step(currentStep, currentTime);
    }
    
    // Process inter-region connections
    for (const auto& conn : pImpl->interRegionConnections) {
        NeuralRegion* source = getRegion(conn.first.sourceRegion);
        NeuralRegion* target = getRegion(conn.first.targetRegion);
        if (source && target) {
            // Transfer spikes between regions with delay
            // Implementation would involve spike propagation logic
        }
    }
}

void NeuralProcessor::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        auto* foundNeuron = region->getNeuron(neuron);
        if (foundNeuron) {
            foundNeuron->injectCurrent(current);
            return;
        }
    }
}

void NeuralProcessor::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        region->injectCurrentToNeurons(type, current);
    }
}

void NeuralProcessor::updatePlasticity() {
    // Update STDP
    pImpl->stdp->update(pImpl->spikeSystem->getRecentSpikes());
    
    // Update Hebbian plasticity
    pImpl->hebbian->update(pImpl->spikeSystem->getRecentSpikes());
    
    // Update structural plasticity
    pImpl->structuralPlasticity->update(pImpl->spikeSystem->getRecentSpikes());
}

void NeuralProcessor::develop() {
    // Apply developmental changes
    for (auto& region : pImpl->regions) {
        region->develop();
    }
}

size_t NeuralProcessor::getTotalSpikeCount() const {
    size_t totalSpikes = 0;
    for (const auto& region : pImpl->regions) {
        totalSpikes += region->getTotalSpikeCount();
    }
    return totalSpikes;
}

size_t NeuralProcessor::getTotalNeuronCount() const {
    size_t totalNeurons = 0;
    for (const auto& region : pImpl->regions) {
        totalNeurons += region->getNeuronCount();
    }
    return totalNeurons;
}

size_t NeuralProcessor::getFiringNeuronCount() const {
    size_t firingNeurons = 0;
    for (const auto& region : pImpl->regions) {
        firingNeurons += region->getFiringNeuronCount();
    }
    return firingNeurons;
}

float NeuralProcessor::getAverageFiringRate() const {
    size_t totalNeurons = getTotalNeuronCount();
    if (totalNeurons == 0) return 0.0f;
    
    float totalFiringRate = 0.0f;
    for (const auto& region : pImpl->regions) {
        totalFiringRate += region->getAverageFiringRate() * region->getNeuronCount();
    }
    return totalFiringRate / totalNeurons;
}

NeuralRegion* NeuralProcessor::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* NeuralProcessor::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

RegionId NeuralProcessor::addRegion(const std::string& name) {
    auto region = std::make_unique<NeuralRegion>();
    RegionId id = region->getId();
    if (!name.empty()) {
        region->setName(name);
    }
    pImpl->regions.push_back(std::move(region));
    return id;
}

void NeuralProcessor::addInterRegionConnection(RegionId source, RegionId target,
                                              float weight, Delay delay) {
    pImpl->interRegionConnections[{source, target}] = InterRegionConnection(source, target, weight, delay);
}

void NeuralProcessor::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase({source, target});
}

} // namespace nlm
