// BrainInitializer.cpp - Implementation of modular Brain initialization

#include "BrainInitializer.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Initialize basic neural components
void initializePlasticitySystems(Brain::Impl* impl) {
    impl->spikeSystem = std::make_unique<SpikeSystem>();
    impl->stdp = std::make_unique<STDP>();
    impl->hebbian = std::make_unique<Hebbian>();
    impl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
}

// Setup all integrated memory systems
void setupMemorySystems(Brain::Impl* impl, Brain* brain, size_t neuronCount) {
    // Initialize working memory with capacity based on neuron count
    impl->workingMemory->initialize(brain);
    impl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory with episode capacity
    impl->episodicMemory->initialize(brain);
    impl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory for pattern associations
    impl->associativeMemory->initialize(brain);
}

// Setup all cognition and attention systems
void setupCognitionSystems(Brain::Impl* impl, Brain* brain) {
    // Initialize neural planner with specific planning depth
    impl->planner->initialize(brain);
    impl->planner->setPlanningDepth(5);
    
    // Initialize concept formation system
    impl->conceptFormation->initialize(brain);
    
    // Initialize attentional selection system with balanced inhibition/excitation
    impl->attention->initialize(brain);
    impl->attention->setInhibitionStrength(0.5f);
    impl->attention->setExcitationStrength(1.5f);
}

// Setup all neuromodulation and motivation systems
void setupNeuromodulationSystems(Brain::Impl* impl, Brain* brain) {
    // Initialize novelty detection system
    impl->novelty->initialize(brain);
    
    // Initialize curiosity-driven exploration system
    impl->curiosity->initialize(brain);
}

// Setup event-driven spike processing handlers
void setupSpikeHandlers(Brain::Impl* impl, Brain* brain) {
    // Register simple spike counting handler
    impl->spikeSystem->registerHandler([brain](const DetailedSpikeEvent& event) {
        // Increment spike counters for statistics
        ++brain->pImpl->totalSpikesThisStep;
        ++brain->pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler for synaptic input delivery
    impl->spikeSystem->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
        // Find target neuron and apply synaptic input
        for (auto& region : brain->pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Scale synaptic weight and apply as current injection
                    MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                    if (event.is_excitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    return;  // Stop after finding first match
                }
            }
        }
    });
}

// Configure checkpoint system for state saving/loading
void configureCheckpointManager(Brain::Impl* impl) {
    std::string checkpointDir = impl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    impl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

// Initialize all integrated systems for Phase 6
bool initializeBrainSystems(Brain::Impl* impl, Brain* brain, size_t neuronCount, 
                           size_t regionCount, float connectionProbability, Timestamp currentTime) {
    // Initialize plasticity mechanisms
    initializePlasticitySystems(impl);
    
    // Setup neural configuration
    configureSTDP(impl);
    configureStructuralPlasticity(impl);
    
    // Setup all memory systems (working, episodic, associative)
    setupMemorySystems(impl, brain, neuronCount);
    
    // Setup all cognitive and attentional systems
    setupCognitionSystems(impl, brain);
    
    // Setup all neuromodulation and motivation systems
    setupNeuromodulationSystems(impl, brain);
    
    // Setup event-driven spike processing
    setupSpikeHandlers(impl, brain);
    
    // Configure checkpoint system
    configureCheckpointManager(impl);
    
    return true;
}

} // namespace nlm