// BrainInitializer.cpp - Implementation of BrainInitializer class

#include "BrainInitializer.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

void BrainInitializer::initialize(Brain& brain) {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");

    // Re-initialising must rebuild, not append: a previous run left
    // reset()+initialize() reporting 1000 neurons for a 500-neuron brain.
    brain.pImpl->regions.clear();
    brain.pImpl->sensoryNeurons.clear();
    brain.pImpl->motorNeurons.clear();
    brain.pImpl->neuronById.clear();
    brain.pImpl->nextRegionId = RegionId(1);
    brain.pImpl->totalSpikesTotal = 0;
    brain.pImpl->totalSpikesThisStep = 0;
    brain.pImpl->currentStep = 0;
    brain.pImpl->currentTime = 0.0;

    // Get configuration values
    size_t neuronCount = static_cast<size_t>(brain.pImpl->config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(brain.pImpl->config->getOr<int64_t>("region_count", 1));
    float connectionProbability = brain.pImpl->config->getOr<float>("connection_probability", 0.1f);

    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " +
                 std::to_string(regionCount) + " regions");

    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        brain.addRegion("Region_" + std::to_string(i + 1));
    }

    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t nextNeuronIndex = 1;  // globally unique IDs across regions (see setNextNeuronIndex)
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain.getRegion(RegionId(i + 1));
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
                    brain.pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    brain.pImpl->motorNeurons.push_back(neuron);
                }
            }

            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) +
                         ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                         std::to_string(region->getTotalNeuronCount()) + " neurons");
            nextNeuronIndex += region->getTotalNeuronCount();
        }
    }

    // Build the neuron lookup used by synaptic delivery and injection.
    for (auto& region : brain.pImpl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            brain.pImpl->neuronById[neuron->getId().index()] = neuron;
        }
    }

    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain.getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*brain.pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }

    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========

    // Initialize working memory
    brain.pImpl->workingMemory->initialize(&brain);
    brain.pImpl->workingMemory->setCapacity(neuronCount / 10);

    // Initialize episodic memory
    brain.pImpl->episodicMemory->initialize(&brain);
    brain.pImpl->episodicMemory->setMaxEpisodes(1000);

    // Initialize associative memory
    brain.pImpl->associativeMemory->initialize(&brain);

    // Initialize cognition systems
    brain.pImpl->planner->initialize(&brain);
    brain.pImpl->planner->setPlanningDepth(5);

    brain.pImpl->conceptFormation->initialize(&brain);

    brain.pImpl->attention->initialize(&brain);
    brain.pImpl->attention->setInhibitionStrength(0.5f);
    brain.pImpl->attention->setExcitationStrength(1.5f);

    // Initialize neuromodulation
    brain.pImpl->novelty->initialize(&brain);
    brain.pImpl->curiosity->initialize(&brain);

    // Register spike handlers once; re-initialising used to register them
    // again and double-count every spike.
    if (!brain.pImpl->spikeHandlersRegistered) {
        brain.pImpl->spikeHandlersRegistered = true;
        // Register spike handlers for event-driven processing
        brain.pImpl->spikeSystem->registerHandler([](const DetailedSpikeEvent& event) {
            // Count spikes
            ++brain.pImpl->totalSpikesThisStep;
            ++brain.pImpl->totalSpikesTotal;
        });

        // Register delayed spike handler to deliver synaptic input
        brain.pImpl->spikeSystem->registerDelayedHandler([](const DelayedSpikeEvent& event) {
            auto it = brain.pImpl->neuronById.find(event.destination_neuron.index());
            if (it != brain.pImpl->neuronById.end()) {
                Neuron* neuron = it->second;
                MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                if (event.is_excitatory) {
                    neuron->receiveExcitatoryInput(synapticCurrent);
                } else {
                    neuron->receiveInhibitoryInput(-synapticCurrent);
                }
            }
        });
    }  // spikeHandlersRegistered

    // Configure checkpoint manager
    std::string checkpointDir = brain.pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    brain.pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);

    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(brain.pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(brain.pImpl->motorNeurons.size()));
}

void BrainInitializer::loadConfiguration(Brain& brain) {
    // Get timestep
    brain.pImpl->timestep = brain.pImpl->config->getOr<double>("simulation_timestep", 0.001);

    // Get integration intervals from config
    brain.pImpl->replayInterval = static_cast<size_t>(brain.pImpl->config->getOr<int64_t>("replay_interval", 100));
    brain.pImpl->consolidationInterval = static_cast<size_t>(brain.pImpl->config->getOr<int64_t>("consolidation_interval", 1000));
}

void BrainInitializer::setupRegions(Brain& brain) {
    // Regions are created in the main initialize method
}

void BrainInitializer::setupConnectivity(Brain& brain) {
    // Connectivity setup is done in the main initialize method
}

void BrainInitializer::initializeIntegratedSystems(Brain& brain) {
    // All integrated systems are initialized in the main initialize method
}

void BrainInitializer::registerSpikeHandlers(Brain& brain) {
    // Spike handlers are registered in the main initialize method
}

void BrainInitializer::configurePlasticitySystems(Brain& brain) {
    // Configure STDP parameters
    float ltpWeight = brain.pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = brain.pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = brain.pImpl->config->getOr<float>("stdp_tau", 20.0f);
    brain.pImpl->stdp->configure(ltpWeight, ltdWeight, tau);

    // Configure structural plasticity
    float synaptogenesisRate = brain.pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = brain.pImpl->config->getOr<float>("pruning_rate", 0.00001f);
    brain.pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    brain.pImpl->structuralPlasticity->setPruningRate(pruningRate);
}

void BrainInitializer::configureCheckpointManager(Brain& brain) {
    // Checkpoint manager is configured in the main initialize method
}

void BrainInitializer::setupMemorySystems(Brain& brain) {
    // Memory systems are initialized in the main initialize method
}

void BrainInitializer::setupPredictionSystem(Brain& brain) {
    // Prediction system is initialized in the main initialize method
}

void BrainInitializer::setupCognitionSystems(Brain& brain) {
    // Cognition systems are initialized in the main initialize method
}

void BrainInitializer::setupNeuromodulationSystems(Brain& brain) {
    // Neuromodulation systems are initialized in the main initialize method
}

void BrainInitializer::setupDevelopmentSystem(Brain& brain) {
    // Development system is initialized in the main initialize method
}

void BrainInitializer::setupRandomGenerator(Brain& brain) {
    // Initialize random generator with seed from config
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = brain.pImpl->config->get<int64_t>("random_seed")) {
        seed = *seedOpt;
    }
    brain.pImpl->rng = std::make_unique<RandomGenerator>(seed);
}

void BrainInitializer::setupPlasticitySystems(Brain& brain) {
    // Plasticity systems are set up in the main initialize method
}

void BrainInitializer::setupInterRegionConnections(Brain& brain) {
    // Inter-region connections are set up later in the step method
}
