// Brain initialization - extracted from monolithic Brain.cpp
#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Acetylcholine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "performance/CheckpointSystem.hpp"

namespace nlm {

// Extract from Brain::Brain(std::shared_ptr<Config> config) constructor
Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool Brain::initialize() {
    // Extract from original Brain::initialize()
    NLM_LOG_INFO("Initializing NLM Brain...");
    
    // Initialize spike system
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    
    // Initialize plasticity systems
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Initialize regions
    for (size_t i = 0; i < pImpl->regionCount; ++i) {
        pImpl->regions.push_back(std::make_unique<NeuralRegion>(RegionId(i + 1)));
    }
    
    // Connect regions with inter-region connections
    for (const auto& conn : pImpl->interRegionConnections) {
        auto* srcRegion = getRegion(conn.sourceRegion);
        auto* tgtRegion = getRegion(conn.targetRegion);
        if (srcRegion && tgtRegion) {
            // Connect regions
        }
    }
    
    // Initialize brain neurons based on configuration
    initializeNeurons();
    
    // Register spike handlers
    registerSpikeHandlers();
    
    NLM_LOG_INFO("Brain initialized successfully");
    return true;
}

void Brain::initializeNeurons() {
    // Extract neuron initialization logic
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    pImpl->neuronById.clear();
    
    for (auto& region : pImpl->regions) {
        region->initializeNeurons(pImpl->config, *pImpl->rng);
        
        for (auto* neuron : region->getAllNeurons()) {
            pImpl->neuronById[neuron->getId().index()] = neuron;
            
            if (neuron->getType() == NeuronType::Sensory) {
                pImpl->sensoryNeurons.push_back(neuron);
            } else if (neuron->getType() == NeuronType::Motor) {
                pImpl->motorNeurons.push_back(neuron);
            }
        }
    }
}

void Brain::registerSpikeHandlers() {
    // Extract spike handler registration
    pImpl->spikeHandlersRegistered = false;
    
    for (auto& region : pImpl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            pImpl->spikeSystem->registerSpikeHandler(
                neuron->getId(),
                [this, neuron](Timestamp time) {
                    handleNeuronSpike(neuron, time);
                }
            );
        }
    }
    
    pImpl->spikeHandlersRegistered = true;
}

void Brain::handleNeuronSpike(Neuron* neuron, Timestamp time) {
    // Extract neuron spike handling logic
    // Process spike for STDP and Hebbian plasticity
    for (auto* syn : neuron->getOutgoingSynapses()) {
        syn->processPreSpike(time);
    }
    
    // Schedule post-synaptic spike delivery
    for (auto* syn : neuron->getOutgoingSynapses()) {
        pImpl->spikeSystem->scheduleDelayedSpike(
            syn->getDestinationNeuron(),
            syn->getDelay(),
            time
        );
    }
}

} // namespace nlm