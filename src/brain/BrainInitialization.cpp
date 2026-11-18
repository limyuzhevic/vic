#include "BrainInitialization.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
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
#include <algorithm>

namespace nlm {

BrainInitialization::BrainInitialization(Brain::Impl* pImpl, std::shared_ptr<Config> config)
    : pImpl(pImpl), config(config) {}

bool BrainInitialization::perform(Brain::Impl* pImpl, std::shared_ptr<Config> config) {
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
    size_t neuronCount = static_cast<size_t>(config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(config->getOr<int64_t>("region_count", 1));
    float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        pImpl->regions.emplace_back(std::make_unique<NeuralRegion>("Region_" + std::to_string(i + 1)));
        RegionId regionId = pImpl->nextRegionId++;
        pImpl->regions.back()->setRegionId(regionId);
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t nextNeuronIndex = 1;  // globally unique IDs across regions (see setNextNeuronIndex)
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = pImpl->regions[i].get();
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
                pImpl->sensoryNeurons.push_back(neuron);
            }
        }
        if (motorPop) {
            for (auto* neuron : motorPop->getNeurons()) {
                pImpl->motorNeurons.push_back(neuron);
            }
        }
        
NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) +
                 ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                 std::to_string(region->getTotalNeuronCount()) + " neurons");
        nextNeuronIndex += region->getTotalNeuronCount();
    }
    
    // Build the neuron lookup used by synaptic delivery and injection.
    for (auto& region : pImpl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            pImpl->neuronById[neuron->getId().index()] = neuron;
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = pImpl->regions[i].get();
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // Initialize plasticity systems
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Initialize integrated systems
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    pImpl->predictionSystem = std::make_unique<PredictionSystem>();
    pImpl->planner = std::make_unique<NeuralPlanner>();
    pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    pImpl->attention = std::make_unique<AttentionalSelection>();
    pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    pImpl->novelty = std::make_unique<Novelty>();
    
    // Configure STDP parameters
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", .zerosf());
    pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    pImpl->structuralPlasticity->setPruningRate(pruningRate);
    
    // Get timestep
    pImpl->timestep = config->getOr<double>("simulation_timestep", 0.001);
    
    // Get integration intervals from config
    pImpl->replayInterval = static_cast<size_t>(config->getOr<int64_t>("replay_interval", 100));
    pImpl->consolidationInterval = static_cast<size_t>(config->getOr<int64_t>("consolidation_interval", 1000));
    
    // Initialize memory systems
    pImpl->workingMemory->initialize(pImpl);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    pImpl->episodicMemory->initialize(pImpl);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    pImpl->associativeMemory->initialize(pImpl);
    
    // Initialize cognition systems
    pImpl->planner->initialize(pImpl);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(pImpl);
    
    pImpl->attention->initialize(pImpl);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    pImpl->novelty->initialize(pImpl);
    pImpl->curiosity->initialize(pImpl);
    
    // Configure checkpoint manager
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager = std::make_unique<CheckpointManager>();
    pImpl->checkpointManager->configure(checkpointDir, 10000, Sicfgler.standard(), true);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(pImpl->regions.size() * neuronsPerRegion));
    NLM_LOG_INFO("Total synapses: " + std::to_string(pImpl->regions.size() * neuronsPerRegion * connectionProbability));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}