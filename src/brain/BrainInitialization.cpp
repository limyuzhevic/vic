// NLM Brain Initialization - Focused on brain setup and configuration

#include "BrainImpl.hpp"
#include "brain/NeuralRegion.hpp"

namespace nlm {

// Helper functions for brain initialization
inline void setupRegions(Brain::Impl* impl);
inline void setupNeurons(Brain::Impl* impl, size_t neuronCount, size_t regionCount);
inline void setupConnectivity(Brain::Impl* impl, size_t regionCount, float connectionProbability);
inline void setupMemorySystems(Brain::Impl* impl, size_t neuronCount);
inline void setupPredictionSystem(Brain::Impl* impl);
inline void setupCognitionSystems(Brain::Impl* impl);
inline void setupNeuromodulationSystems(Brain::Impl* impl);
inline void setupPlasticitySystems(Brain::Impl* impl);
inline void setupDevelopmentSystem(Brain::Impl* impl);
inline void registerSpikeHandlers(Brain::Impl* impl);
inline void configureCheckpointManager(Brain::Impl* impl);

// Brain initialization implementation
bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");

    // Re-initialising must rebuild, not append: a previous run left
    // reset()+initialize() reporting 1000 neurons for a 500-neuron brain.
    pImpl->regions.clear();
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    pImpl->neuronById.clear();
    pImpl->nextRegionId = RegionId(1);
    pImpl->totalSpikesTotal = 0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;

    // Get configuration values
    size_t neuronCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("region_count", 1));
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);

    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " +
                 std::to_string(regionCount) + " regions");

    // Setup brain components in logical order
    setupRegions(pImpl);
    setupNeurons(pImpl, neuronCount, regionCount);
    setupConnectivity(pImpl, regionCount, connectionProbability);
    setupMemorySystems(pImpl, neuronCount);
    setupPredictionSystem(pImpl);
    setupCognitionSystems(pImpl);
    setupNeuromodulationSystems(pImpl);
    setupPlasticitySystems(pImpl);
    setupDevelopmentSystem(pImpl);
    registerSpikeHandlers(pImpl);
    configureCheckpointManager(pImpl);

    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));

    return true;
}

inline void setupRegions(Brain::Impl* impl) {
    size_t regionCount = static_cast<size_t>(impl->config->getOr<int64_t>("region_count", 1));
    for (size_t i = 0; i < regionCount; ++i) {
        impl->addRegion("Region_" + std::to_string(i + 1));
    }
}

inline void setupNeurons(Brain::Impl* impl, size_t neuronCount, size_t regionCount) {
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t nextNeuronIndex = 1;  // globally unique IDs across regions (see setNextNeuronIndex)
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = impl->getRegion(RegionId(i + 1));
        if (region) {
            region->setNextNeuronIndex(nextNeuronIndex);
            // Add populations. The old layout had no inhibitory neurons at
            // all (Sensory/Internal/Motor are all treated as excitatory by
            // initializeRandomConnectivity), so any input caused runaway
            // excitation and every weight saturated. Split the internal
            // population 80/20 into excitatory and inhibitory neurons.
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            size_t internalCount = neuronsPerRegion / 2;
            size_t internalExcitatory = internalCount * 4 / 5;
            auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
            auto inhibitoryPopId = region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
            (void)internalPopId; (void)inhibitoryPopId;
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);

            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    impl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    impl->motorNeurons.push_back(neuron);
                }
            }

            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) +
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
            nextNeuronIndex += region->getTotalNeuronCount();
        }
    }
}

inline void setupConnectivity(Brain::Impl* impl, size_t regionCount, float connectionProbability) {
    // Build the neuron lookup used by synaptic delivery and injection.
    for (auto& region : impl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            impl->neuronById[neuron->getId().index()] = neuron;
        }
    }

    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = impl->getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*impl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

inline void setupMemorySystems(Brain::Impl* impl, size_t neuronCount) {
    // Initialize working memory
    impl->workingMemory->initialize(nullptr);  // Brain pointer set later
    impl->workingMemory->setCapacity(neuronCount / 10);

    // Initialize episodic memory
    impl->episodicMemory->initialize(nullptr);  // Brain pointer set later
    impl->episodicMemory->setMaxEpisodes(1000);

    // Initialize associative memory
    impl->associativeMemory->initialize(nullptr);  // Brain pointer set later
}

inline void setupPredictionSystem(Brain::Impl* impl) {
    // PredictionSystem doesn't have initialize method currently
    (void)impl;
}

inline void setupCognitionSystems(Brain::Impl* impl) {
    impl->planner->initialize(nullptr);  // Brain pointer set later
    impl->planner->setPlanningDepth(5);

    impl->conceptFormation->initialize(nullptr);  // Brain pointer set later

    impl->attention->initialize(nullptr);  // Brain pointer set later
    impl->attention->setInhibitionStrength(0.5f);
    impl->attention->setExcitationStrength(1.5f);
}

inline void setupNeuromodulationSystems(Brain::Impl* impl) {
    impl->novelty->initialize(nullptr);  // Brain pointer set later
    impl->curiosity->initialize(nullptr);  // Brain pointer set later
}

inline void setupPlasticitySystems(Brain::Impl* impl) {
    // Plasticity systems already initialized in constructor
}

inline void setupDevelopmentSystem(Brain::Impl* impl) {
    impl->developmentSystem->initialize(nullptr);  // Brain pointer set later
}

inline void registerSpikeHandlers(Brain::Impl* impl) {
    // Register spike handlers once; re-initialising used to register them
    // again and double-count every spike.
    if (!impl->spikeHandlersRegistered) {
        impl->spikeHandlersRegistered = true;
        // Register spike handlers for event-driven processing
        impl->spikeSystem->registerHandler([impl](const DetailedSpikeEvent& event) {
            // Count spikes
            ++impl->totalSpikesThisStep;
            ++impl->totalSpikesTotal;
        });

        // Register delayed spike handler to deliver synaptic input
        impl->spikeSystem->registerDelayedHandler([impl](const DelayedSpikeEvent& event) {
            auto it = impl->neuronById.find(event.destination_neuron.index());
            if (it != impl->neuronById.end()) {
                Neuron* neuron = it->second;
                MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                if (event.is_excitatory) {
                    neuron->receiveExcitatoryInput(synapticCurrent);
                } else {
                    neuron->receiveInhibitoryInput(-synapticCurrent);
                }
            }
        });
    }
}

inline void configureCheckpointManager(Brain::Impl* impl) {
    // Configure checkpoint manager
    std::string checkpointDir = impl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    impl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

} // namespace nlm
