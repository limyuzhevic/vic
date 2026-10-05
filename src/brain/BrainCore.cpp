#include "BrainCore.hpp"
#include "Brain.hpp"
#include "NeuralRegion.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"

namespace nlm {

BrainCore::BrainCore(std::shared_ptr<Config> cfg)
    : config(cfg)
    , timestep(0.001)
    , currentStep(0)
    , currentTime(0.0)
    , totalSpikesThisStep(0)
    , totalSpikesTotal(0)
    , isResting(false)
    , stepsSinceLastEpisode(0)
    , replayInterval(100)
    , consolidationInterval(1000)
{
    // Initialize systems
    initializeRandomGenerator();
    initializePlasticitySystems();
    initializeSpikeSystem();
    initializeMemorySystems(nullptr);
    initializePredictionSystem();
    initializeCognitionSystems(nullptr);
    initializeNeuromodulationSystems(nullptr);
    configureSTDP();
    configureStructuralPlasticity();
    configureTimestep();
    configureIntegrationIntervals();
    initializeCheckpointManager();
}

void BrainCore::initializeRandomGenerator() {
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = config->get<uint64_t>("random_seed")) {
        seed = *seedOpt;
    }
    rng = std::make_unique<RandomGenerator>(seed);
}

void BrainCore::initializePlasticitySystems() {
    spikeSystem = std::make_unique<SpikeSystem>();
    stdp = std::make_unique<STDP>();
    hebbian = std::make_unique<Hebbian>();
    structuralPlasticity = std::make_unique<StructuralPlasticity>();
}

void BrainCore::initializeSpikeSystem() {
    // Spike system is already initialized in initializePlasticitySystems
    // Additional spike system configuration can be added here
}

void BrainCore::initializeMemorySystems(Brain* parent) {
    workingMemory = std::make_unique<NeuralWorkingMemory>();
    episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    if (parent) {
        workingMemory->initialize(parent);
        episodicMemory->initialize(parent);
        associativeMemory->initialize(parent);
    }
}

void BrainCore::initializePredictionSystem() {
    predictionSystem = std::make_unique<PredictionSystem>();
    // Note: PredictionSystem doesn't have initialize method currently
}

void BrainCore::initializeCognitionSystems(Brain* parent) {
    planner = std::make_unique<NeuralPlanner>();
    conceptFormation = std::make_unique<ConceptFormation>();
    attention = std::make_unique<AttentionalSelection>();
    
    if (parent) {
        planner->initialize(parent);
        planner->setPlanningDepth(5);
        
        conceptFormation->initialize(parent);
        
        attention->initialize(parent);
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
}

void BrainCore::initializeNeuromodulationSystems(Brain* parent) {
    dopamine = std::make_unique<Dopamine>();
    curiosity = std::make_unique<Curiosity>();
    predictionError = std::make_unique<PredictionError>();
    novelty = std::make_unique<Novelty>();
    
    if (parent) {
        novelty->initialize(parent);
        curiosity->initialize(parent);
    }
}

void BrainCore::configureSTDP() {
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    stdp->configure(ltpWeight, ltdWeight, tau);
}

void BrainCore::configureStructuralPlasticity() {
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    structuralPlasticity->setPruningRate(pruningRate);
}

void BrainCore::configureTimestep() {
    timestep = config->getOr<double>("simulation_timestep", 0.001);
}

void BrainCore::configureIntegrationIntervals() {
    replayInterval = config->getOr<size_t>("replay_interval", 100);
    consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
}

void BrainCore::initializeCheckpointManager() {
    checkpointManager = std::make_unique<CheckpointManager>();
}

void BrainCore::createRegions(size_t regionCount) {
    for (size_t i = 0; i < regionCount; ++i) {
        // Create region with unique ID
        RegionId regionId(i + 1);
        regions.push_back(std::make_unique<NeuralRegion>(regionId, "Region_" + std::to_string(i + 1)));
    }
}

void BrainCore::createNeurons(size_t neuronCount, size_t regionCount) {
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    for (size_t i = 0; i < regions.size(); ++i) {
        auto& region = regions[i];
        
        // Add populations based on configuration
        size_t sensoryCount = neuronsPerRegion / 4;
        size_t internalCount = neuronsPerRegion / 2;
        size_t motorCount = neuronsPerRegion / 4;
        
        auto sensoryPopId = region->addPopulation(sensoryCount, NeuronType::Sensory);
        auto internalPopId = region->addPopulation(internalCount, NeuronType::Internal);
        auto motorPopId = region->addPopulation(motorCount, NeuronType::Motor);
        
        // Collect sensory and motor neurons for I/O
        auto* sensoryPop = region->getPopulation(sensoryPopId);
        auto* motorPop = region->getPopulation(motorPopId);
        
        if (sensoryPop) {
            for (auto* neuron : sensoryPop->getNeurons()) {
                sensoryNeurons.push_back(neuron);
            }
        }
        
        if (motorPop) {
            for (auto* neuron : motorPop->getNeurons()) {
                motorNeurons.push_back(neuron);
            }
        }
    }
}

void BrainCore::initializeConnectivity(float connectionProbability) {
    for (auto& region : regions) {
        region->initializeRandomConnectivity(*rng, connectionProbability, 0.2f, 0.1f);
    }
}

} // namespace nlm