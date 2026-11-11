// Brain Initialization Implementation

#include "BrainInitialization.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cstring>
#include <sstream>

namespace nlm {

void BrainInitialization::initialize(Impl& impl, std::shared_ptr<Config> config) {
    // Initialize random generator with seed from config
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = config->get<int64_t>("random_seed")) {
        seed = *seedOpt;
    }
    impl.rng = std::make_unique<RandomGenerator>(seed);
    
    // Initialize plasticity systems
    impl.spikeSystem = std::make_unique<SpikeSystem>();
    impl.stdp = std::make_unique<STDP>();
    impl.hebbian = std::make_unique<Hebbian>();
    impl.structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Initialize memory systems
    impl.workingMemory = std::make_unique<NeuralWorkingMemory>();
    impl.episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    impl.associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Initialize prediction system
    impl.predictionSystem = std::make_unique<PredictionSystem>();
    
    // Initialize cognition systems
    impl.planner = std::make_unique<NeuralPlanner>();
    impl.conceptFormation = std::make_unique<ConceptFormation>();
    impl.attention = std::make_unique<AttentionalSelection>();
    
    // Initialize development system
    impl.developmentSystem = std::make_unique<DevelopmentSystem>();
    
    // Initialize neuromodulation systems
    impl.dopamine = std::make_unique<Dopamine>();
    impl.curiosity = std::make_unique<Curiosity>();
    impl.predictionError = std::make_unique<PredictionError>();
    impl.novelty = std::make_unique<Novelty>();
    
    // Configure STDP parameters
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    impl.stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    impl.structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    impl.structuralPlasticity->setPruningRate(pruningRate);
    
    // Get timestep
    impl.timestep = config->getOr<double>("simulation_timestep", 0.001);
    
    // Get integration intervals from config
    impl.replayInterval = static_cast<size_t>(config->getOr<int64_t>("replay_interval", 100));
    impl.consolidationInterval = static_cast<size_t>(config->getOr<int64_t>("consolidation_interval", 1000));
    
    // Initialize checkpoint manager
    impl.checkpointManager = std::make_unique<CheckpointManager>();
}

void BrainInitialization::createRegions(Impl& impl, size_t neuronCount, size_t regionCount, float connectionProbability) {
    // Clear existing regions
    impl.regions.clear();
    impl.sensoryNeurons.clear();
    impl.motorNeurons.clear();
    impl.neuronById.clear();
    impl.nextRegionId = RegionId(1);
    impl.totalSpikesTotal = 0;
    impl.totalSpikesThisStep = 0;
    impl.currentStep = 0;
    impl.currentTime = 0.0;
    impl.isResting = false;
    impl.stepsSinceLastEpisode = 0;
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        RegionId regionId = impl.nextRegionId++;
        auto region = std::make_unique<NeuralRegion>(regionId, "Region_" + std::to_string(i + 1));
        NeuralRegion* regionPtr = region.get();
        impl.regions.push_back(std::move(region));
        
        // Create neurons across regions
        size_t neuronsPerRegion = neuronCount / regionCount;
        size_t nextNeuronIndex = 1;  // globally unique IDs across regions
        region->setNextNeuronIndex(nextNeuronIndex);
        
        // Add populations with improved balance
        auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
        size_t internalCount = neuronsPerRegion / 2;
        size_t internalExcitatory = internalCount * 4 / 5;
        auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
        auto inhibitoryPopId = region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
        auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
        
        // Collect sensory and motor neurons for I/O
        auto* sensoryPop = region->getPopulation(sensoryPopId);
        auto* motorPop = region->getPopulation(motorPopId);
        if (sensoryPop) {
            for (auto* neuron : sensoryPop->getNeurons()) {
                impl.sensoryNeurons.push_back(neuron);
            }
        }
        if (motorPop) {
            for (auto* neuron : motorPop->getNeurons()) {
                impl.motorNeurons.push_back(neuron);
            }
        }
        
        nextNeuronIndex += region->getTotalNeuronCount();
        
        // Initialize random connectivity with improved parameters
        region->initializeRandomConnectivity(*impl.rng, connectionProbability, 0.2f, 0.1f);
        
        // Build the neuron lookup used by synaptic delivery and injection.
        for (auto* neuron : region->getAllNeurons()) {
            impl.neuronById[neuron->getId().index()] = neuron;
        }
    }
}

void BrainInitialization::initializeIntegratedSystems(Impl& impl, size_t neuronCount, std::shared_ptr<Config> config) {
    // Initialize working memory
    impl.workingMemory->initialize(impl.rng.get());
    impl.workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    impl.episodicMemory->initialize(impl.rng.get());
    impl.episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    impl.associativeMemory->initialize(impl.rng.get());
    
    // Initialize cognition systems
    impl.planner->initialize(impl.rng.get());
    impl.planner->setPlanningDepth(5);
    
    impl.conceptFormation->initialize(impl.rng.get());
    
    impl.attention->initialize(impl.rng.get());
    impl.attention->setInhibitionStrength(0.5f);
    impl.attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    impl.novelty->initialize(impl.rng.get());
    impl.curiosity->initialize(impl.rng.get());
    
    // Register spike handlers once; re-initialising must rebuild, not append
    if (!impl.spikeHandlersRegistered) {
        impl.spikeHandlersRegistered = true;
        // Register spike handlers for event-driven processing
        impl.spikeSystem->registerHandler([&impl](const DetailedSpikeEvent& event) {
            // Count spikes
            ++impl.totalSpikesThisStep;
            ++impl.totalSpikesTotal;
        });
        
        // Register delayed spike handler to deliver synaptic input
        impl.spikeSystem->registerDelayedHandler([&impl](const DelayedSpikeEvent& event) {
            auto it = impl.neuronById.find(event.destination_neuron.index());
            if (it != impl.neuronById.end()) {
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
    
    // Configure checkpoint manager
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    impl.checkpointManager->configure(checkpointDir, 10000, 5, true);
}

void BrainInitialization::registerSpikeHandlers(Impl& impl) {
    // Register spike handlers once; re-initialising used to register them
    // again and double-count every spike.
    if (!impl.spikeHandlersRegistered) {
        impl.spikeHandlersRegistered = true;
        // Register spike handlers for event-driven processing
        impl.spikeSystem->registerHandler([&impl](const DetailedSpikeEvent& event) {
            // Count spikes
            ++impl.totalSpikesThisStep;
            ++impl.totalSpikesTotal;
        });
        
        // Register delayed spike handler to deliver synaptic input
        impl.spikeSystem->registerDelayedHandler([&impl](const DelayedSpikeEvent& event) {
            auto it = impl.neuronById.find(event.destination_neuron.index());
            if (it != impl.neuronById.end()) {
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

} // namespace nlm
