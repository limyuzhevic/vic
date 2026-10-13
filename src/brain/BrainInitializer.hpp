#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

namespace BrainInitializer {

// Initialization constants
struct InitializationConstants {
    static constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;
    static constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000;
    static constexpr float DEFAULT_SYNAPTONGENESIS_RATE = 0.0001f;
    static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    static constexpr float DEFAULT_STDP_TAU = 20.0f;
    static constexpr float DEFAULT_PLANNING_DEPTH = 5;
    static constexpr float DEFAULT_INHIBITION_STRENGTH = 0.5f;
    static constexpr float DEFAULT_EXCITATION_STRENGTH = 1.5f;
};

// Configure STDP parameters
inline void configureSTDP(Brain::Impl* impl) {
    float ltpWeight = impl->config->getOr<float>("stdp_ltp_weight", 
                                                InitializationConstants::DEFAULT_STDP_LTP_WEIGHT);
    float ltdWeight = impl->config->getOr<float>("stdp_ltd_weight", 
                                                 InitializationConstants::DEFAULT_STDP_LTD_WEIGHT);
    float tau = impl->config->getOr<float>("stdp_tau", InitializationConstants::DEFAULT_STDP_TAU);
    impl->stdp->configure(ltpWeight, ltdWeight, tau);
}

// Configure structural plasticity
inline void configureStructuralPlasticity(Brain::Impl* impl) {
    float synaptogenesisRate = impl->config->getOr<float>("synaptogenesis_rate", 
                                                        InitializationConstants::DEFAULT_SYNAPTONGENESIS_RATE);
    float pruningRate = impl->config->getOr<float>("pruning_rate", 
                                                  InitializationConstants::DEFAULT_PRUNING_RATE);
    impl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    impl->structuralPlasticity->setPruningRate(pruningRate);
}

// Initialize basic neural components
inline void initializePlasticitySystems(Brain::Impl* impl) {
    impl->spikeSystem = std::make_unique<SpikeSystem>();
    impl->stdp = std::make_unique<STDP>();
    impl->hebbian = std::make_unique<Hebbian>();
    impl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
}

// Setup memory systems
inline void setupMemorySystems(Brain::Impl* impl, Brain* brain, size_t neuronCount) {
    // Initialize working memory
    impl->workingMemory->initialize(brain);
    impl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    impl->episodicMemory->initialize(brain);
    impl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    impl->associativeMemory->initialize(brain);
}

// Setup cognition systems
inline void setupCognitionSystems(Brain::Impl* impl, Brain* brain) {
    // Initialize cognition systems
    impl->planner->initialize(brain);
    impl->planner->setPlanningDepth(InitializationConstants::DEFAULT_PLANNING_DEPTH);
    
    impl->conceptFormation->initialize(brain);
    
    impl->attention->initialize(brain);
    impl->attention->setInhibitionStrength(InitializationConstants::DEFAULT_INHIBITION_STRENGTH);
    impl->attention->setExcitationStrength(InitializationConstants::DEFAULT_EXCITATION_STRENGTH);
}

// Setup neuromodulation systems
inline void setupNeuromodulationSystems(Brain::Impl* impl, Brain* brain) {
    // Initialize neuromodulation systems
    impl->novelty->initialize(brain);
    impl->curiosity->initialize(brain);
}

// Setup spike handlers for event-driven processing
inline void setupSpikeHandlers(Brain::Impl* impl, Brain* brain) {
    impl->spikeSystem->registerHandler([brain](const DetailedSpikeEvent& event) {
        // Count spikes
        ++brain->pImpl->totalSpikesThisStep;
        ++brain->pImpl->totalSpikesTotal;
    });
    
    impl->spikeSystem->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : brain->pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Apply synaptic weight as current
                    MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                    if (event.is_excitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    return;
                }
            }
        }
    });
}

// Configure checkpoint manager
inline void configureCheckpointManager(Brain::Impl* impl) {
    std::string checkpointDir = impl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    impl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

} // namespace BrainInitializer