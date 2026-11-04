// BrainInitializer.cpp - Implementation of BrainInitializer class

#include "BrainInitializer.hpp"

namespace nlm {

bool BrainInitializer::initializeBrain(std::shared_ptr<Impl> impl) {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Reset brain to clean state
    resetBrain(impl);
    
    // Load simulation parameters from config
    loadSimulationParameters(impl);
    
    // Create regions
    createRegions(impl);
    
    // Create neural populations across regions
    createNeuralPopulations(impl);
    
    // Setup neuron lookup system for O(1) access
    setupNeuronLookup(impl);
    
    // Initialize connectivity with random weights
    float connectionProbability = impl->config->getOr<float>("connection_probability", 0.1f);
    initializeConnectivity(impl, connectionProbability);
    
    // Initialize all integrated systems
    initializeIntegratedSystems(impl);
    
    // Setup spike system and handlers
    setupSpikeSystem(impl);
    
    // Configure checkpoint manager
    configureCheckpointSystem(impl);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(impl->regions.size() * 250)); // Approximate
    NLM_LOG_INFO("Total synapses: " + std::to_string(impl->regions.size() * 1250)); // Approximate
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(impl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(impl->motorNeurons.size()));
    
    return true;
}

void BrainInitializer::resetBrain(std::shared_ptr<Impl> impl) {
    impl->regions.clear();
    impl->sensoryNeurons.clear();
    impl->motorNeurons.clear();
    impl->neuronById.clear();
    impl->nextRegionId = RegionId(1);
    impl->totalSpikesTotal = 0;
    impl->totalSpikesThisStep = 0;
    impl->currentStep = 0;
    impl->currentTime = 0.0;
}

void BrainInitializer::loadSimulationParameters(std::shared_ptr<Impl> impl) {
    // Get timestep
    impl->timestep = impl->config->getOr<double>("simulation_timestep", 0.001);
    
    // Get integration intervals from config
    impl->replayInterval = static_cast<size_t>(impl->config->getOr<int64_t>("replay_interval", 100));
    impl->consolidationInterval = static_cast<size_t>(impl->config->getOr<int64_t>("consolidation_interval", 1000));
}

void BrainInitializer::createRegions(std::shared_ptr<Impl> impl) {
    // Get configuration values
    size_t neuronCount = static_cast<size_t>(impl->config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(impl->config->getOr<int64_t>("region_count", 1));
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        impl->regions.push_back(std::make_unique<NeuralRegion>("Region_" + std::to_string(i + 1)));
    }
}

void BrainInitializer::createNeuralPopulations(std::shared_ptr<Impl> impl) {
    // Get configuration values
    size_t neuronCount = static_cast<size_t>(impl->config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = impl->regions.size();
    
    if (regionCount == 0) return;
    
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t nextNeuronIndex = 1;  // globally unique IDs across regions
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = impl->regions[i].get();
        region->setNextNeuronIndex(nextNeuronIndex);
        
        // Add populations. Split the internal population 80/20 into excitatory and inhibitory neurons.
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

void BrainInitializer::setupNeuronLookup(std::shared_ptr<Impl> impl) {
    // Build the neuron lookup used by synaptic delivery and injection.
    for (auto& region : impl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            impl->neuronById[neuron->getId().index()] = neuron;
        }
    }
}

void BrainInitializer::initializeConnectivity(std::shared_ptr<Impl> impl, float connectionProbability) {
    // Initialize connectivity with random weights
    for (size_t i = 0; i < impl->regions.size(); ++i) {
        auto* region = impl->regions[i].get();
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*impl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void BrainInitializer::initializeIntegratedSystems(std::shared_ptr<Impl> impl) {
    // Get configuration values
    size_t neuronCount = static_cast<size_t>(impl->config->getOr<int64_t>("neuron_count", 1000));
    
    // Initialize working memory
    impl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    impl->workingMemory->initialize(nullptr); // TODO: pass brain pointer
    impl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    impl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    impl->episodicMemory->initialize(nullptr); // TODO: pass brain pointer
    impl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    impl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    impl->associativeMemory->initialize(nullptr); // TODO: pass brain pointer
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
    impl->planner = std::make_unique<NeuralPlanner>();
    impl->planner->initialize(nullptr); // TODO: pass brain pointer
    impl->planner->setPlanningDepth(5);
    
    impl->conceptFormation = std::make_unique<ConceptFormation>();
    impl->conceptFormation->initialize(nullptr); // TODO: pass brain pointer
    
    impl->attention = std::make_unique<AttentionalSelection>();
    impl->attention->initialize(nullptr); // TODO: pass brain pointer
    impl->attention->setInhibitionStrength(0.5f);
    impl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    impl->novelty = std::make_unique<Novelty>();
    impl->novelty->initialize(nullptr); // TODO: pass brain pointer
    
    impl->curiosity = std::make_unique<Curiosity>();
    impl->curiosity->initialize(nullptr); // TODO: pass brain pointer
    
    impl->dopamine = std::make_unique<Dopamine>();
    impl->dopamine->initialize(nullptr); // TODO: pass brain pointer
    
    impl->predictionError = std::make_unique<PredictionError>();
    impl->predictionError->initialize(nullptr); // TODO: pass brain pointer
    
    // Initialize development system
    impl->developmentSystem = std::make_unique<DevelopmentSystem>();
    impl->developmentSystem->initialize(nullptr); // TODO: pass brain pointer
    
    // Configure neuromodulation systems
    configureNeuromodulationSystems(impl);
}

void BrainInitializer::setupSpikeSystem(std::shared_ptr<Impl> impl) {
    // Configure STDP parameters
    float ltpWeight = impl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = impl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = impl->config->getOr<float>("stdp_tau", 20.0f);
    impl->stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity
    float synaptogenesisRate = impl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = impl->config->getOr<float>("pruning_rate", 0.00001f);
    impl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    impl->structuralPlasticity->setPruningRate(pruningRate);
    
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

void BrainInitializer::configureCheckpointSystem(std::shared_ptr<Impl> impl) {
    // Configure checkpoint manager
    std::string checkpointDir = impl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    impl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

void BrainInitializer::configureNeuromodulationSystems(std::shared_ptr<Impl> impl) {
    // Apply neuromodulation configurations
    applyPlasticityConfigurations(impl);
}

void BrainInitializer::applyPlasticityConfigurations(std::shared_ptr<Impl> impl) {
    // Apply STDP configurations from config
    float ltpWeight = impl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = impl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = impl->config->getOr<float>("stdp_tau", 20.0f);
    impl->stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Apply structural plasticity configurations
    float synaptogenesisRate = impl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = impl->config->getOr<float>("pruning_rate", 0.00001f);
    impl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    impl->structuralPlasticity->setPruningRate(pruningRate);
}

} // namespace nlm