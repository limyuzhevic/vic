// BrainCore.cpp
#include "BrainCore.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>

namespace nlm {

struct BrainCore::Impl {
    std::shared_ptr<Config> config;
    double timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg), timestep(0.001), currentStep(0), currentTime(0.0), 
                                       totalSpikesThisStep(0), totalSpikesTotal(0) {}
    ~Impl() = default;
};

BrainCore::BrainCore(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

BrainCore::~BrainCore() = default;

BrainCore::BrainCore(BrainCore&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

BrainCore& BrainCore::operator=(BrainCore&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void BrainCore::setConfig(std::shared_ptr<Config> config) {
    pImpl->config = config;
}

size_t BrainCore::getTotalNeuronCount() const {
    if (!pImpl->config) return 0;
    
    size_t neuronCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("region_count", 1));
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    return neuronCount;
}

size_t BrainCore::getTotalSynapseCount() const {
    if (!pImpl->config) return 0;
    
    size_t neuronCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("region_count", 1));
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    // Estimate synapses: each neuron has connection_probability * neuronsPerRegion connections
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    return static_cast<size_t>(neuronCount * neuronsPerRegion * connectionProbability);
}

size_t BrainCore::getFiringNeuronCount() const {
    // This would need to query actual neurons from the brain
    // For now return 0 as this is a simplified implementation
    return 0;
}

float BrainCore::getAverageFiringRate() const {
    // This would need to query actual neurons from the brain
    // For now return 0.0 as this is a simplified implementation
    return 0.0f;
}

void BrainCore::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

} // namespace nlm