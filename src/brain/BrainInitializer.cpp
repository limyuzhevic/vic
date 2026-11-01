#include "BrainInitializer.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>

namespace nlm {

bool BrainInitializer::initializeBrain(Brain* brain, std::shared_ptr<Config> config) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Initializing NLM Brain components...");

    // Store config reference
    brain->pImpl->config = config;

    // Initialize random generator
    uint64_t seed = 42;
    if (auto seedOpt = config->get<int64_t>("random_seed")) {
        seed = *seedOpt;
    }
    brain->pImpl->rng = std::make_unique<RandomGenerator>(seed);

    // Configure timestep
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    brain->pImpl->timestep = timestep;

    // Get integration intervals from config
    brain->pImpl->replayInterval = static_cast<size_t>(config->getOr<int64_t>("replay_interval", 100));
    brain->pImpl->consolidationInterval = static_cast<size_t>(config->getOr<int64_t>("consolidation_interval", 1000));

    // Initialize plasticity systems
    brain->pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    brain->pImpl->stdp = std::make_unique<STDP>();
    brain->pImpl->hebbian = std::make_unique<Hebbian>();
    brain->pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();

    // Initialize checkpoint manager
    brain->pImpl->checkpointManager = std::make_unique<CheckpointManager>();

    NLM_LOG_INFO("Brain component initialization complete.");

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    stats.initializationTime = duration;

    return true;
}

void BrainInitializer::setupRegions(Brain* brain) {
    NLM_LOG_INFO("Setting up neural regions...");

    // Get configuration
    size_t neuronCount = static_cast<size_t>(brain->pImpl->config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(brain->pImpl->config->getOr<int64_t>("region_count", 1));
    float connectionProbability = brain->pImpl->config->getOr<float>("connection_probability", 0.1f);

    // Clear existing regions
    brain->pImpl->regions.clear();

    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        brain->addRegion("Region_" + std::to_string(i + 1));
    }

    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t nextNeuronIndex = 1;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (region) {
            region->setNextNeuronIndex(nextNeuronIndex);
            
            // Add populations with neuron type distribution
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            size_t internalCount = neuronsPerRegion / 2;
            size_t internalExcitatory = internalCount * 4 / 5;
            auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
            auto inhibitoryPopId = region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);

            (void)internalPopId; (void)inhibitoryPopId;

            // Collect sensory and motor neurons for I/O
            collectIONeurons(brain, i);

            nextNeuronIndex += region->getTotalNeuronCount();
        }
    }

    // Build the neuron lookup used by synaptic delivery and injection
    for (auto& region : brain->pImpl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            brain->pImpl->neuronById[neuron->getId().index()] = neuron;
        }
    }

    // Initialize connectivity
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*brain->pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }

    // Update stats
    stats.neuronCount = brain->getTotalNeuronCount();
    stats.synapseCount = brain->getTotalSynapseCount();

    NLM_LOG_INFO("Region setup complete: " + std::to_string(regionCount) + " regions, " + 
                 std::to_string(stats.neuronCount) + " neurons, " + 
                 std::to_string(stats.synapseCount) + " synapses");
}

void BrainInitializer::initializeIntegratedSystems(Brain* brain) {
    NLM_LOG_INFO("Initializing integrated brain systems...");

    // Initialize memory systems
    brain->pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    brain->pImpl->workingMemory->initialize(brain);
    brain->pImpl->workingMemory->setCapacity(stats.neuronCount / 10);

    brain->pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    brain->pImpl->episodicMemory->initialize(brain);
    brain->pImpl->episodicMemory->setMaxEpisodes(1000);

    brain->pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    brain->pImpl->associativeMemory->initialize(brain);

    // Initialize prediction system
    brain->pImpl->predictionSystem = std::make_unique<PredictionSystem>();

    // Initialize cognition systems
    brain->pImpl->planner = std::make_unique<NeuralPlanner>();
    brain->pImpl->planner->initialize(brain);
    brain->pImpl->planner->setPlanningDepth(5);

    brain->pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    brain->pImpl->conceptFormation->initialize(brain);

    brain->pImpl->attention = std::make_unique<AttentionalSelection>();
    brain->pImpl->attention->initialize(brain);
    brain->pImpl->attention->setInhibitionStrength(0.5f);
    brain->pImpl->attention->setExcitationStrength(1.5f);

    // Initialize development system
    brain->pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();

    // Initialize neuromodulation systems
    brain->pImpl->dopamine = std::make_unique<Dopamine>();
    brain->pImpl->curiosity = std::make_unique<Curiosity>();
    brain->pImpl->predictionError = std::make_unique<PredictionError>();
    brain->pImpl->novelty = std::make_unique<Novelty>();
    brain->pImpl->reward = std::make_unique<Reward>();

    // Initialize novelty and curiosity
    brain->pImpl->novelty->initialize(brain);
    brain->pImpl->curiosity->initialize(brain);
    brain->pImpl->reward->initialize(brain);

    NLM_LOG_INFO("Integrated systems initialization complete.");
}

void BrainInitializer::configurePlasticity(Brain* brain) {
    // Configure STDP parameters
    float ltpWeight = brain->pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = brain->pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = brain->pImpl->config->getOr<float>("stdp_tau", 20.0f);
    brain->pImpl->stdp->configure(ltpWeight, ltdWeight, tau);

    // Configure structural plasticity
    float synaptogenesisRate = brain->pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = brain->pImpl->config->getOr<float>("pruning_rate", 0.00001f);
    brain->pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    brain->pImpl->structuralPlasticity->setPruningRate(pruningRate);
}

void BrainInitializer::setupSpikeSystem(Brain* brain) {
    // Register spike handlers once
    if (!brain->pImpl->spikeHandlersRegistered) {
        brain->pImpl->spikeHandlersRegistered = true;
        
        // Register spike handlers for event-driven processing
        brain->pImpl->spikeSystem->registerHandler([brain](const DetailedSpikeEvent& event) {
            // Count spikes
            ++brain->pImpl->totalSpikesThisStep;
            ++brain->pImpl->totalSpikesTotal;
        });

        // Register delayed spike handler to deliver synaptic input
        brain->pImpl->spikeSystem->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
            auto it = brain->pImpl->neuronById.find(event.destination_neuron.index());
            if (it != brain->pImpl->neuronById.end()) {
                Neuron* neuron = it->second;
                MembranePotential synapticCurrent = event.weight * 10.0f;
                if (event.is_excitatory) {
                    neuron->receiveExcitatoryInput(synapticCurrent);
                } else {
                    neuron->receiveInhibitoryInput(-synapticCurrent);
                }
            }
        });
    }
}

void BrainInitializer::configureCheckpointSystem(Brain* brain) {
    std::string checkpointDir = brain->pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    brain->pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

void BrainInitializer::createRegion(Brain* brain, size_t regionIndex, size_t neuronsPerRegion) {
    auto* region = brain->getRegion(RegionId(regionIndex + 1));
    if (!region) return;

    // Create populations with the same distribution as in Brain::initialize()
    auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
    size_t internalCount = neuronsPerRegion / 2;
    size_t internalExcitatory = internalCount * 4 / 5;
    auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
    auto inhibitoryPopId = region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
    auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);

    (void)internalPopId; (void)inhibitoryPopId; (void)sensoryPopId; (void)motorPopId;
    
    collectIONeurons(brain, regionIndex);
}

void BrainInitializer::collectIONeurons(Brain* brain, size_t regionIndex) {
    auto* region = brain->getRegion(RegionId(regionIndex + 1));
    if (!region) return;

    auto* sensoryPop = region->getPopulation(PopulationId(1));  // First population is sensory
    auto* motorPop = region->getPopulation(PopulationId(4));    // Fourth population is motor

    if (sensoryPop) {
        for (auto* neuron : sensoryPop->getNeurons()) {
            brain->pImpl->sensoryNeurons.push_back(neuron);
        }
    }
    if (motorPop) {
        for (auto* neuron : motorPop->getNeurons()) {
            brain->pImpl->motorNeurons.push_back(neuron);
        }
    }
}

void BrainInitializer::setupMemorySystem(Brain* brain) {
    if (brain->pImpl->workingMemory) {
        brain->pImpl->workingMemory->setCapacity(stats.neuronCount / 10);
    }
}

void BrainInitializer::setupCognitionSystem(Brain* brain) {
    if (brain->pImpl->planner) {
        brain->pImpl->planner->setPlanningDepth(5);
    }
    if (brain->pImpl->attention) {
        brain->pImpl->attention->setInhibitionStrength(0.5f);
        brain->pImpl->attention->setExcitationStrength(1.5f);
    }
}

void BrainInitializer::setupNeuromodulationSystems(Brain* brain) {
    if (brain->pImpl->novelty) {
        brain->pImpl->novelty->initialize(brain);
    }
    if (brain->pImpl->curiosity) {
        brain->pImpl->curiosity->initialize(brain);
    }
    if (brain->pImpl->reward) {
        brain->pImpl->reward->initialize(brain);
    }
}
