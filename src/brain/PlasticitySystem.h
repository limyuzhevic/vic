// PlasticitySystem.cpp - Implementation of plasticity system
#include "PlasticitySystem.h"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

struct PlasticitySystem::Impl {
    Brain* brain_ = nullptr;
    std::shared_ptr<Config> config;
    SimulationStep lastSTDPUpdate = 0;
    SimulationStep lastHebbianUpdate = 0;
    SimulationStep lastStructuralUpdate = 0;
};

PlasticitySystem::PlasticitySystem() : pImpl(std::make_unique<Impl>()) {}

PlasticitySystem::~PlasticitySystem() = default;

bool PlasticitySystem::initialize(Brain* brain) {
    if (!brain) return false;
    
    pImpl->brain_ = brain;
    pImpl->config = brain->getConfig();
    pImpl->lastSTDPUpdate = 0;
    pImpl->lastHebbianUpdate = 0;
    pImpl->lastStructuralUpdate = 0;
    
    // Initialize plasticity components
    spikeSystem_ = std::make_unique<SpikeSystem>();
    stdp_ = std::make_unique<STDP>();
    hebbian_ = std::make_unique<Hebbian>();
    structuralPlasticity_ = std::make_unique<StructuralPlasticity>();
    
    // Configure STDP parameters from config
    float ltpWeight = pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = pImpl->config->getOr<float>("stdp_tau", 20.0f);
    stdp_->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity parameters
    float synaptogenesisRate = pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = pImpl->config->getOr<float>("pruning_rate", 0.00001f);
    structuralPlasticity_->setSynaptogenesisRate(synaptogenesisRate);
    structuralPlasticity_->setPruningRate(pruningRate);
    
    initialized_ = true;
    NLM_LOG_INFO("PlasticitySystem initialized");
    return true;
}

void PlasticitySystem::update(const TimestepDuration& dt) {
    if (!initialized_ || !pImpl->brain_) return;
    
    pImpl->brain_->getSpikeSystem()->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes for statistics
        // This would be integrated with brain statistics
    });
    
    // Register delayed spike handler to deliver synaptic input
    pImpl->brain_->getSpikeSystem()->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        // This is handled in the main brain step function
    });
    
    // Update structural plasticity periodically
    pImpl->lastStructuralUpdate++;
    if (pImpl->lastStructuralUpdate >= 100) {  // Update every 100 steps
        structuralPlasticity_->update(pImpl->brain_, *pImpl->brain_->getRandomGenerator());
        pImpl->lastStructuralUpdate = 0;
    }
    
    NLM_LOG_TRACE("PlasticitySystem updated");
}

void PlasticitySystem::reset() {
    if (spikeSystem_) spikeSystem_->reset();
    if (stdp_) stdp_->reset();
    if (hebbian_) hebbian_->reset();
    if (structuralPlasticity_) structuralPlasticity_->reset();
    
    pImpl->lastSTDPUpdate = 0;
    pImpl->lastHebbianUpdate = 0;
    pImpl->lastStructuralUpdate = 0;
    
    initialized_ = false;
    NLM_LOG_INFO("PlasticitySystem reset");
}

void PlasticitySystem::logStatus() const {
    NLM_LOG_INFO("=== Plasticity System Status ===");
    NLM_LOG_INFO("Pending spikes: " + std::to_string(getPendingSpikeCount()));
    NLM_LOG_INFO("Pending delayed spikes: " + std::to_string(getPendingDelayedCount()));
}

void PlasticitySystem::queueSpike(const SpikeEvent& event) {
    if (spikeSystem_) {
        spikeSystem_->queueSpike(event);
    }
}

void PlasticitySystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    if (spikeSystem_) {
        spikeSystem_->queueDelayedSpike(event);
    }
}

void PlasticitySystem::processSpikes(SimulationStep step) {
    if (spikeSystem_) {
        spikeSystem_->processSpikes(step);
    }
}

void PlasticitySystem::processDelayedSpikes(SimulationStep step, Timestamp time) {
    if (spikeSystem_) {
        spikeSystem_->processDelayedSpikes(step, time);
    }
}

size_t PlasticitySystem::getPendingSpikeCount() const {
    return spikeSystem_ ? spikeSystem_->getPendingSpikeCount() : 0;
}

size_t PlasticitySystem::getPendingDelayedCount() const {
    return spikeSystem_ ? spikeSystem_->getPendingDelayedCount() : 0;
}

void PlasticitySystem::applySTDP(Synapse* synapse, const std::vector<float>& preSpikes, const std::vector<float>& postSpikes) {
    if (stdp_ && synapse && !preSpikes.empty() && !postSpikes.empty()) {
        stdp_->update(synapse, preSpikes, postSpikes, 0.001);
    }
}

void PlasticitySystem::applyHebbianLearning(Synapse* synapse, const std::vector<float>& preSpikes, const std::vector<float>& postSpikes) {
    if (hebbian_ && synapse && !preSpikes.empty() && !postSpikes.empty()) {
        hebbian_->update(synapse, preSpikes, postSpikes, 0.001);
    }
}

void PlasticitySystem::updateStructuralPlasticity() {
    if (structuralPlasticity_ && pImpl->brain_ && pImpl->brain_->getRandomGenerator()) {
        structuralPlasticity_->update(pImpl->brain_, *pImpl->brain_->getRandomGenerator());
    }
}

} // namespace nlm
