#include "NeuralComputationSystem.hpp"
#include "Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct NeuralComputationSystem::Impl {
    Brain* brain;
    
    // Neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Neural collections
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Simulation parameters
    float timestep;
    float connectionProbability;
    size_t totalNeuronCount;
    size_t totalSynapseCount;
    
    // Statistics
    size_t activeNeurons;
    size_t firingNeurons;
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Configuration
    bool initialized;
};

NeuralComputationSystem::NeuralComputationSystem()
    : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = false;
    pImpl->timestep = 0.001f;
    pImpl->connectionProbability = 0.1f;
    pImpl->totalNeuronCount = 0;
    pImpl->totalSynapseCount = 0;
    pImpl->activeNeurons = 0;
    pImpl->firingNeurons = 0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
}

NeuralComputationSystem::~NeuralComputationSystem() = default;

bool NeuralComputationSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Create neural computation components
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Configure STDP parameters from config
    if (pImpl->brain && pImpl->brain->getConfig()) {
        auto config = pImpl->brain->getConfig();
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>"stdp_tau", 20.0f);
        pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
    }
    
    // Configure structural plasticity
    pImpl->structuralPlasticity->setSynaptogenesisRate(0.0001f);
    pImpl->structuralPlasticity->setPruningRate(0.00001f);
    
    pImpl->initialized = true;
    
    // Register spike handlers
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    return true;
}

void NeuralComputationSystem::update(float dt) {
    if (!pImpl->initialized || !pImpl->brain) {
        return;
    }
    
    pImpl->timestep = dt;
    pImpl->totalSpikesThisStep = 0;
    
    // Update neural populations (simulate LIF dynamics)
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                neuron->stepLIF(pImpl->brain->getCurrentTime(), dt);
            }
        }
    }
    
    // Detect and queue spikes
    pImpl->brain->detectAndQueueSpikes(pImpl->brain->getCurrentStep(), pImpl->brain->getCurrentTime());
    
    // Process spikes
    pImpl->spikeSystem->processSpikes(pImpl->brain->getCurrentStep());
    
    // Process delayed spikes
    pImpl->spikeSystem->processDelayedSpikes(pImpl->brain->getCurrentStep(), pImpl->brain->getCurrentTime());
    
    // Update statistics
    pImpl->activeNeurons = 0;
    pImpl->firingNeurons = 0;
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            pImpl->activeNeurons += population->getActiveNeuronCount();
            pImpl->firingNeurons += population->getFiringNeuronCount();
        }
    }
}

void NeuralComputationSystem::reset() {
    pImpl->totalSpikesThisStep = 0;
    pImpl->activeNeurons = 0;
    pImpl->firingNeurons = 0;
    pImpl->spikeSystem->reset();
}

void NeuralComputationSystem::logStatus() const {
    NLM_LOG_INFO("=== NeuralComputationSystem Status ===");
    NLM_LOG_INFO("Initialized: " + std::string(pImpl->initialized ? "Yes" : "No"));
    NLM_LOG_INFO("Total neurons: " + std::to_string(pImpl->totalNeuronCount));
    NLM_LOG_INFO("Total synapses: " + std::to_string(pImpl->totalSynapseCount));
    NLM_LOG_INFO("Active neurons: " + std::to_string(pImpl->activeNeurons));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(pImpl->firingNeurons));
    NLM_LOG_INFO("Total spikes (this step): " + std::to_string(pImpl->totalSpikesThisStep));
    NLM_LOG_INFO("Total spikes (all time): " + std::to_string(pImpl->totalSpikesTotal));
    NLM_LOG_INFO("Timestep: " + std::to_string(pImpl->timestep) + " s");
}

SpikeSystem* NeuralComputationSystem::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* NeuralComputationSystem::getSTDP() const {
    return pImpl->stdp.get();
}

Hebbian* NeuralComputationSystem::getHebbian() const {
    return pImpl->hebbian.get();
}

StructuralPlasticity* NeuralComputationSystem::getStructuralPlasticity() const {
    return pImpl->structuralPlasticity.get();
}

void NeuralComputationSystem::simulateNeurons(SimulationStep step, Timestamp time) {
    if (!pImpl->initialized) return;
    
    // Simulate LIF dynamics for all neurons
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                neuron->stepLIF(time, pImpl->timestep);
            }
        }
    }
}

void NeuralComputationSystem::processSpikes(SimulationStep step) {
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processSpikes(step);
    }
}

void NeuralComputationSystem::deliverDelayedSpikes(SimulationStep step, Timestamp time) {
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processDelayedSpikes(step, time);
    }
}

void NeuralComputationSystem::computeSynapticInputs() {
    // This would deliver synaptic inputs to postsynaptic neurons
    // Implementation depends on the specific synaptic model
}

void NeuralComputationSystem::detectAndQueueSpikes(SimulationStep step, Timestamp time) {
    // Detect neurons that have crossed firing threshold
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                if (neuron->checkThreshold()) {
                    // Queue spike event
                    SpikeEvent event(neuron->getId(), time, step);
                    pImpl->spikeSystem->queueSpike(event);
                    
                    // Record spike for plasticity
                    neuron->recordSpike(time);
                }
            }
        }
    }
}

void NeuralComputationSystem::configure(float timestep, float connectionProbability) {
    pImpl->timestep = timestep;
    pImpl->connectionProbability = connectionProbability;
}

float NeuralComputationSystem::getTimestep() const {
    return pImpl->timestep;
}

size_t NeuralComputationSystem::getTotalNeuronCount() const {
    return pImpl->totalNeuronCount;
}

size_t NeuralComputationSystem::getTotalSynapseCount() const {
    return pImpl->totalSynapseCount;
}

size_t NeuralComputationSystem::getActiveNeuronCount() const {
    return pImpl->activeNeurons;
}

size_t NeuralComputationSystem::getFiringNeuronCount() const {
    return pImpl->firingNeurons;
}

const std::vector<Neuron*>& NeuralComputationSystem::getSensoryNeurons() const {
    return pImpl->sensoryNeurons;
}

const std::vector<Neuron*>& NeuralComputationSystem::getMotorNeurons() const {
    return pImpl->motorNeurons;
}

void NeuralComputationSystem::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto* neuronPtr : region->getAllNeurons()) {
            if (neuronPtr->getId() == neuron) {
                neuronPtr->injectCurrent(current);
                return;
            }
        }
    }
}

void NeuralComputationSystem::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            if (population->getNeuronType() == type) {
                for (auto* neuron : population->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

float NeuralComputationSystem::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto* syn : region->getSynapses()) {
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

float NeuralComputationSystem::getAverageFiringRate() const {
    float sum = 0.0f;
    size_t count = 0;
    
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            sum += population->getAverageFiringRate();
            count++;
        }
    }
    
    if (count > 0) {
        return sum / static_cast<float>(count);
    }
    return 0.0f;
}

size_t NeuralComputationSystem::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t NeuralComputationSystem::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

} // namespace nlm
