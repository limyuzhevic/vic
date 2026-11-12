// BrainInit.cpp - Implementation of Brain initialization handler
#include "BrainInit.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/Memory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <algorithm>

namespace nlm {

BrainInit::BrainInit(Brain* brain, std::shared_ptr<Config> config)
    : brain_(brain), config_(config) {
}

BrainInit::~BrainInit() = default;

bool BrainInit::initialize() {
    setupLogging();
    loadConfiguration();
    setupRandomGenerator();
    createRegions();
    createNeurons();
    setupConnectivity();
    initializeMemorySystems();
    initializePredictionSystem();
    initializeCognitionSystems();
    initializeNeuromodulation();
    setupSpikeHandlers();
    configureCheckpointManager();
    return true;
}

void BrainInit::setupLogging() {
    logger_ = std::make_unique<Logger>();
    consoleLogger_ = std::make_unique<ConsoleLogger>(LogLevel::Info);
    logger_->addLogger(consoleLogger_);
    Logger::setGlobal(logger_);
}

void BrainInit::loadConfiguration() {
    // Load configuration from file
    std::string configFile = "configs/default.cfg";
    if (config_->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Set default values for Phase 2
    config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config_->set("connection_probability", 0.15f, ConfigSource::Default);
}

void BrainInit::setupRandomGenerator() {
    uint64_t seed = 42;
    if (auto seedOpt = config_->get<int64_t>("random_seed")) {
        seed = *seedOpt;
    }
    rng_ = std::make_unique<RandomGenerator>(seed);
}

void BrainInit::createRegions() {
    size_t regionCount = static_cast<size_t>(config_->getOr<int64_t>("region_count", 1));
    for (size_t i = 0; i < regionCount; ++i) {
        brain_->addRegion("Region_" + std::to_string(i + 1));
    }
}

void BrainInit::createNeurons() {
    size_t neuronCount = static_cast<size_t>(config_->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(config_->getOr<int64_t>("region_count", 1));
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    size_t nextNeuronIndex = 1;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain_->getRegion(RegionId(i + 1));
        if (region) {
            region->setNextNeuronIndex(nextNeuronIndex);
            
            // Create populations with proper distribution
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            size_t internalCount = neuronsPerRegion / 2;
            size_t internalExcitatory = internalCount * 4 / 5;
            auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
            auto inhibitoryPopId = region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Store sensory and motor neurons for I/O
            if (auto* sensoryPop = region->getPopulation(sensoryPopId)) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    brain_->storeSensoryNeuron(neuron);
                }
            }
            if (auto* motorPop = region->getPopulation(motorPopId)) {
                for (auto* neuron : motorPop->getNeurons()) {
                    brain_->storeMotorNeuron(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) +
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
            nextNeuronIndex += region->getTotalNeuronCount();
        }
    }
}

void BrainInit::setupConnectivity() {
    size_t regionCount = static_cast<size_t>(config_->getOr<int64_t>("region_count", 1));
    float connectionProbability = config_->getOr<float>("connection_probability", 0.1f);
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain_->getRegion(RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*rng_, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void BrainInit::initializeMemorySystems() {
    auto* workingMemory = brain_->getWorkingMemory();
    auto* episodicMemory = brain_->getEpisodicMemory();
    auto* associativeMemory = brain_->getAssociativeMemory();
    
    if (workingMemory) {
        workingMemory->initialize(brain_);
        size_t neuronCount = static_cast<size_t>(config_->getOr<int64_t>("neuron_count", 1000));
        workingMemory->setCapacity(neuronCount / 10);
    }
    
    if (episodicMemory) {
        episodicMemory->initialize(brain_);
        episodicMemory->setMaxEpisodes(1000);
    }
    
    if (associativeMemory) {
        associativeMemory->initialize(brain_);
    }
}

void BrainInit::initializePredictionSystem() {
    auto* predictionSystem = brain_->getPredictionSystem();
    if (predictionSystem) {
        // Initialize prediction system
    }
}

void BrainInit::initializeCognitionSystems() {
    auto* planner = brain_->getPlanner();
    auto* conceptFormation = brain_->getConceptFormation();
    auto* attention = brain_->getAttention();
    
    if (planner) {
        planner->initialize(brain_);
        planner->setPlanningDepth(5);
    }
    
    if (conceptFormation) {
        conceptFormation->initialize(brain_);
    }
    
    if (attention) {
        attention->initialize(brain_);
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
}

void BrainInit::initializeNeuromodulation() {
    auto* novelty = brain_->getNovelty();
    auto* curiosity = brain_->getCuriosity();
    auto* dopamine = brain_->getDopamine();
    auto* predictionError = brain_->getPredictionErrorSignal();
    
    if (novelty) {
        novelty->initialize(brain_);
    }
    
    if (curiosity) {
        curiosity->initialize(brain_);
    }
    
    if (dopamine) {
        // Dopamine initialization if needed
    }
    
    if (predictionError) {
        // Prediction error initialization if needed
    }
}

void BrainInit::setupSpikeHandlers() {
    auto* spikeSystem = brain_->getSpikeSystem();
    if (spikeSystem && !brain_->areSpikeHandlersRegistered()) {
        brain_->setSpikeHandlersRegistered(true);
        
        // Register spike handlers for event-driven processing
        spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes
            ++brain_->getImpl()->totalSpikesThisStep;
            ++brain_->getImpl()->totalSpikesTotal;
        });
        
        // Register delayed spike handler to deliver synaptic input
        spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            // Deliver synaptic input
            brain_->deliverSynapticInput(event);
        });
    }
}

void BrainInit::configureCheckpointManager() {
    std::string checkpointDir = config_->getOr<std::string>("checkpoint_dir", "./checkpoints");
    if (auto* checkpointManager = brain_->getCheckpointManager()) {
        checkpointManager->configure(checkpointDir, 10000, 5, true);
    }
}

} // namespace nlm
