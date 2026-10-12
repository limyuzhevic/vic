#include "NeuralEngine.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "Neuron.cpp"
#include "Synapse.cpp"
#include "NeuralRegion.cpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct NeuralEngine::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    Brain* brain = nullptr;
    
    // Neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Neuron and synapse management
    std::vector<std::unique_ptr<Neuron>> neurons;
    std::vector<std::unique_ptr<Synapse>> synapses;
    
    // Statistics
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    size_t activeNeurons = 0;
    size_t firingNeurons = 0;
    size_t totalSpikes = 0;
    size_t pendingSpikes = 0;
    size_t pendingDelayed = 0;
    
    // Plasticity modulation
    float neuromodulationFactor = 1.0f;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg) {
        rng = std::make_unique<RandomGenerator>(42);
        neuromodulationFactor = 1.0f;
    }
};

NeuralEngine::NeuralEngine(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(config)), brain_(nullptr) {
    pImpl->rng = std::make_unique<RandomGenerator>(42);
    pImpl->neuromodulationFactor = 1.0f;
}

NeuralEngine::~NeuralEngine() = default;

NeuralEngine::NeuralEngine(NeuralEngine&& other) noexcept : pImpl(std::move(other.pImpl)), brain_(other.brain_) {
    other.brain_ = nullptr;
}

NeuralEngine& NeuralEngine::operator=(NeuralEngine&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        brain_ = other.brain_;
        other.brain_ = nullptr;
    }
    return *this;
}

bool NeuralEngine::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("NeuralEngine: Cannot initialize with null brain");
        return false;
    }
    
    brain_ = brain;
    pImpl->brain = brain;
    
    // Initialize neural computation components
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Configure STDP
    if (auto ltpWeight = config_->getOr<float>("stdp_ltp_weight", 0.01f)) {
        pImpl->stdp->configure(*ltpWeight, pImpl->stdp->getLTDWeight(), pImpl->stdp->getTau());
    }
    if (auto ltdWeight = config_->getOr<float>("stdp_ltd_weight", 0.012f)) {
        pImpl->stdp->configure(pImpl->stdp->getLTPWeight(), *ltdWeight, pImpl->stdp->getTau());
    }
    if (auto tau = config_->getOr<float>("stdp_tau", 20.0f)) {
        pImpl->stdp->configure(pImpl->stdp->getLTPWeight(), pImpl->stdp->getLTDWeight(), *tau);
    }
    
    // Configure structural plasticity
    if (auto synaptogenesisRate = config_->getOr<float>("synaptogenesis_rate", 0.0001f)) {
        pImpl->structuralPlasticity->setSynaptogenesisRate(*synaptogenesisRate);
    }
    if (auto pruningRate = config_->getOr<float>("pruning_rate", 0.00001f)) {
        pImpl->structuralPlasticity->setPruningRate(*pruningRate);
    }
    
    NLM_LOG_INFO("NeuralEngine initialized successfully");
    return true;
}

void NeuralEngine::step(SimulationStep step, Timestamp time, float timestep) {
    // Update spike system
    pImpl->spikeSystem->processDelayedSpikes(step, time);
    pImpl->spikeSystem->processSpikes(step);
    
    // Update plasticity with neuromodulation factor
    if (pImpl->stdp) {
        pImpl->stdp->setNeuromodulationFactor(pImpl->neuromodulationFactor);
    }
    if (pImpl->hebbian) {
        pImpl->hebbian->setNeuromodulationFactor(pImpl->neuromodulationFactor);
    }
    
    // Update structural plasticity periodically
    if (step % 100 == 0) {
        pImpl->structuralPlasticity->update(brain_, *pImpl->rng);
    }
    
    // Update statistics
    pImpl->pendingSpikes = pImpl->spikeSystem->getPendingSpikeCount();
    pImpl->pendingDelayed = pImpl->spikeSystem->getPendingDelayedCount();
}

void NeuralEngine::connectNeurons(NeuronId source, NeuronId destination, float weight,
                                 Delay delay, bool isExcitatory, PlasticityFlags plasticity) {
    // Create synapse and add to neural region
    auto* region = brain_->getRegion(RegionId(1));
    if (region) {
        SynapseHandle synHandle = region->addSynapse(source, destination, weight, delay, isExcitatory, plasticity);
        pImpl->synapses.push_back(std::make_unique<Synapse>(*region->getSynapse(synHandle)));
        pImpl->totalSynapses++;
        NLM_LOG_DEBUG("NeuralEngine: Connected neuron " + std::to_string(source.index()) + 
                     " to neuron " + std::to_string(destination.index()) + 
                     " with weight " + std::to_string(weight));
    }
}

void NeuralEngine::processDelayedSpikes(SimulationStep step, Timestamp time) {
    pImpl->spikeSystem->processDelayedSpikes(step, time);
}

void NeuralEngine::processSpikes(SimulationStep step) {
    pImpl->spikeSystem->processSpikes(step);
}

void NeuralEngine::updatePlasticity() {
    pImpl->spikeSystem->updatePlasticity();
}

void NeuralEngine::applyNeuromodulationFactor(float factor) {
    pImpl->neuromodulationFactor = factor;
}

void NeuralEngine::reset() {
    NLM_LOG_INFO("NeuralEngine reset");
    pImpl->neuromodulationFactor = 1.0f;
}

// Statistics implementations
size_t NeuralEngine::getTotalNeuronCount() const { return pImpl->totalNeurons; }
size_t NeuralEngine::getTotalSynapseCount() const { return pImpl->totalSynapses; }
size_t NeuralEngine::getActiveNeuronCount() const { return pImpl->activeNeurons; }
size_t NeuralEngine::getFiringNeuronCount() const { return pImpl->firingNeurons; }
float NeuralEngine::getAverageFiringRate() const { return 0.0f; }  // TODO: Implement
float NeuralEngine::getExcitationInhibitionRatio() const { return 0.0f; }  // TODO: Implement
size_t NeuralEngine::getTotalSpikeCount() const { return pImpl->totalSpikes; }
size_t NeuralEngine::getPendingSpikeEventCount() const { return pImpl->pendingSpikes + pImpl->pendingDelayed; }

} // namespace nlm