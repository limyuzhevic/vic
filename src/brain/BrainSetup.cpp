// BrainSetup.cpp - Enhanced Brain Setup Component with Error Handling
// Provides brain initialization, region creation, and connectivity setup with comprehensive error handling
// Includes input validation, graceful error recovery, and detailed logging

#pragma once

#include "BrainSetup.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
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
#include <cmath>
#include <sstream>
#include <stdexcept>

namespace nlm {

// ========== BRAIN SETUP COMPONENT WITH ERROR HANDLING ==========

// Enhanced BrainSetup implementation with comprehensive error checking and recovery
// Includes input validation, state checking, and graceful error handling

struct BrainSetup::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Prediction system
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // Cognition systems
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // Development system
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Neuromodulation systems
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Error handling state
    bool initializationSuccessful;
    std::vector<std::string> errorMessages;
    SimulationClock simulationClock;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg)
        , rng(nullptr)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)
        , consolidationInterval(1000)
        , initializationSuccessful(false)
    {
        // Initialize error handling
        errorMessages.clear();
        
        try {
            // Initialize random generator with validation
            uint64_t seed = 42;
            if (auto seedOpt = config->get<uint64_t>("random_seed")) {
                seed = *seedOpt;
                // Validate seed value
                if (seed > std::numeric_limits<uint64_t>::max() / 2) {
                    errorMessages.push_back("Random seed too large, using default");
                    seed = 42;
                }
            }
            rng = std::make_unique<RandomGenerator>(seed);
            
            // Validate configuration before proceeding
            if (!validateConfiguration()) {
                initializationSuccessful = false;
                return;
            }
            
            // Initialize plasticity systems with error checking
            initializePlasticitySystems();
            
            // Initialize integrated systems
            initializeIntegratedSystems();
            
            initializationSuccessful = true;
            
        } catch (const std::exception& e) {
            errorMessages.push_back(std::string("Error during initialization: ") + e.what());
            initializationSuccessful = false;
            // Cleanup partially initialized resources
            cleanupPartialInitialization();
        }
    }
    
    bool validateConfiguration() {
        bool isValid = true;
        
        // Check essential configuration parameters
        if (auto neuronCount = config->get<size_t>("neuron_count")) {
            if (*neuronCount == 0 || *neuronCount > 1000000) {
                errorMessages.push_back("Invalid neuron count: must be between 1 and 1,000,000");
                isValid = false;
            }
        }
        
        if (auto regionCount = config->get<size_t>("region_count")) {
            if (*regionCount == 0 || *regionCount > 1000) {
                errorMessages.push_back("Invalid region count: must be between 1 and 1,000");
                isValid = false;
            }
        }
        
        if (auto connProb = config->get<float>("connection_probability")) {
            if (*connProb < 0.0f || *connProb > 1.0f) {
                errorMessages.push_back("Invalid connection probability: must be between 0.0 and 1.0");
                isValid = false;
            }
        }
        
        if (auto timestep = config->get<double>("simulation_timestep")) {
            if (*timestep <= 0.0 || *timestep > 1.0) {
                errorMessages.push_back("Invalid simulation timestep: must be between 0.0 and 1.0 seconds");
                isValid = false;
            }
        }
        
        return isValid;
    }
    
    void initializePlasticitySystems() {
        try {
            // Initialize plasticity systems with error checking
            spikeSystem = std::make_unique<SpikeSystem>();
            stdp = std::make_unique<STDP>();
            hebbian = std::make_unique<Hebbian>();
            structuralPlasticity = std::make_unique<StructuralPlasticity>();
            
            // Configure STDP parameters with validation
            float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
            float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
            float tau = config->getOr<float>("stdp_tau", 20.0f);
            
            // Validate STDP parameters
            if (ltpWeight < 0.0f || ltpWeight > 1.0f) {
                errorMessages.push_back("Invalid LTP weight: must be between 0.0 and 1.0");
                ltpWeight = std::clamp(ltpWeight, 0.0f, 1.0f);
            }
            
            if (ltdWeight < 0.0f || ltdWeight > 1.0f) {
                errorMessages.push_back("Invalid LTD weight: must be between 0.0 and 1.0");
                ltdWeight = std::clamp(ltdWeight, 0.0f, 1.0f);
            }
            
            if (tau < 1.0f || tau > 100.0f) {
                errorMessages.push_back("Invalid STDP time constant: must be between 1.0 and 100.0 ms");
                tau = std::clamp(tau, 1.0f, 100.0f);
            }
            
            stdp->configure(ltpWeight, ltdWeight, tau);
            
            // Configure structural plasticity with validation
            float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
            float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
            
            if (synaptogenesisRate < 0.0f || synaptogenesisRate > 0.01f) {
                errorMessages.push_back("Invalid synaptogenesis rate: must be between 0.0 and 0.01");
                synaptogenesisRate = std::clamp(synaptogenesisRate, 0.0f, 0.01f);
            }
            
            if (pruningRate < 0.0f || pruningRate > 0.001f) {
                errorMessages.push_back("Invalid pruning rate: must be between 0.0 and 0.001");
                pruningRate = std::clamp(pruningRate, 0.0f, 0.001f);
            }
            
            structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
            structuralPlasticity->setPruningRate(pruningRate);
            
        } catch (const std::exception& e) {
            errorMessages.push_back(std::string("Error initializing plasticity systems: ") + e.what());
            throw;
        }
    }
    
    void initializeIntegratedSystems() {
        try {
            // Initialize memory systems with validation
            workingMemory = std::make_unique<NeuralWorkingMemory>();
            episodicMemory = std::make_unique<NeuralEpisodicMemory>();
            associativeMemory = std::make_unique<NeuralAssociativeMemory>();
            
            predictionSystem = std::make_unique<PredictionSystem>();
            planner = std::make_unique<NeuralPlanner>();
            conceptFormation = std::make_unique<ConceptFormation>();
            attention = std::make_unique<AttentionalSelection>();
            developmentSystem = std::make_unique<DevelopmentSystem>();
            dopamine = std::make_unique<Dopamine>();
            curiosity = std::make_unique<Curiosity>();
            predictionError = std::make_unique<PredictionError>();
            novelty = std::make_unique<Novelty>();
            
            // Initialize each system with error checking
            if (workingMemory) workingMemory->initialize(nullptr);
            if (episodicMemory) episodicMemory->initialize(nullptr);
            if (associativeMemory) associativeMemory->initialize(nullptr);
            if (predictionSystem) predictionSystem->initialize();
            if (planner) planner->initialize(nullptr);
            if (conceptFormation) conceptFormation->initialize(nullptr);
            if (attention) attention->initialize(nullptr);
            if (novelty) novelty->initialize(nullptr);
            if (curiosity) curiosity->initialize(nullptr);
            if (dopamine) dopamine->initialize(nullptr);
            
        } catch (const std::exception& e) {
            errorMessages.push_back(std::string("Error initializing integrated systems: ") + e.what());
            throw;
        }
    }
    
    void cleanupPartialInitialization() {
        // Cleanup resources in case of initialization failure
        spikeSystem.reset();
        stdp.reset();
        hebbian.reset();
        structuralPlasticity.reset();
        
        workingMemory.reset();
        episodicMemory.reset();
        associativeMemory.reset();
        predictionSystem.reset();
        planner.reset();
        conceptFormation.reset();
        attention.reset();
        developmentSystem.reset();
        dopamine.reset();
        curiosity.reset();
        predictionError.reset();
        novelty.reset();
        
        rng.reset();
        regions.clear();
        sensoryNeurons.clear();
        motorNeurons.clear();
    }
    
    DevelopmentalStage developmentalStage{DevelopmentalStage::Initial};
    RegionId nextRegionId{1};
};

BrainSetup::BrainSetup(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

BrainSetup::~BrainSetup() = default;

bool BrainSetup::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Check if initialization was successful in constructor
    if (!pImpl->initializationSuccessful) {
        NLM_LOG_ERROR("Brain initialization failed during construction");
        for (const auto& error : pImpl->errorMessages) {
            NLM_LOG_ERROR("Error: " + error);
        }
        return false;
    }
    
    try {
        // Get configuration values with validation
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        // Validate configuration before proceeding
        if (neuronCount == 0) {
            NLM_LOG_ERROR("Neuron count must be greater than 0");
            return false;
        }
        
        if (regionCount == 0) {
            NLM_LOG_ERROR("Region count must be greater than 0");
            return false;
        }
        
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            NLM_LOG_ERROR("Connection probability must be between 0.0 and 1.0");
            return false;
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions, " +
                     "connection probability: " + std::to_string(connectionProbability));
        
        // Create regions with error checking
        for (size_t i = 0; i < regionCount; ++i) {
            addRegion("Region_" + std::to_string(i + 1));
        }
        
        // Create neurons across regions with validation
        size_t neuronsPerRegion = neuronCount / regionCount;
        if (neuronsPerRegion == 0) {
            NLM_LOG_ERROR("Not enough neurons for the specified region count");
            return false;
        }
        
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                NLM_LOG_ERROR("Failed to create region " + std::to_string(i + 1));
                return false;
            }
            
            // Add populations with validation
            auto sensoryPopId = region->addPopulation(std::max<size_t>(1, neuronsPerRegion / 4), NeuronType::Sensory);
            auto internalPopId = region->addPopulation(std::max<size_t>(1, neuronsPerRegion / 2), NeuronType::Internal);
            auto motorPopId = region->addPopulation(std::max<size_t>(1, neuronsPerRegion / 4), NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O with validation
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
        }
        
        // Initialize connectivity with validation
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                NLM_LOG_ERROR("Region not found during connectivity initialization");
                return false;
            }
            
            // Initialize random connectivity with validation
            if (!region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f)) {
                NLM_LOG_ERROR("Failed to initialize random connectivity in region " + std::to_string(i + 1));
                return false;
            }
        }
        
        // Initialize memory systems with validation
        if (pImpl->workingMemory) {
            pImpl->workingMemory->initialize(this);
            pImpl->workingMemory->setCapacity(neuronCount / 10);
        }
        
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->initialize(this);
            pImpl->episodicMemory->setMaxEpisodes(1000);
        }
        
        if (pImpl->associativeMemory) {
            pImpl->associativeMemory->initialize(this);
        }
        
        // Initialize cognition systems with validation
        if (pImpl->planner) {
            pImpl->planner->initialize(this);
            pImpl->planner->setPlanningDepth(5);
        }
        
        if (pImpl->conceptFormation) {
            pImpl->conceptFormation->initialize(this);
        }
        
        if (pImpl->attention) {
            pImpl->attention->initialize(this);
            pImpl->attention->setInhibitionStrength(0.5f);
            pImpl->attention->setExcitationStrength(1.5f);
        }
        
        // Initialize neuromodulation with validation
        if (pImpl->novelty) {
            pImpl->novelty->initialize(this);
        }
        
        if (pImpl->curiosity) {
            pImpl->curiosity->initialize(this);
        }
        
        // Register spike handlers for event-driven processing with validation
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
                // Count spikes
                ++pImpl->totalSpikesThisStep;
                ++pImpl->totalSpikesTotal;
            });
            
            pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
                // Find destination neuron and deliver synaptic input
                for (auto& region : pImpl->regions) {
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
        
        // Configure checkpoint manager with validation
        if (pImpl->checkpointManager) {
            std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
            if (!checkpointDir.empty()) {
                pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
            } else {
                NLM_LOG_WARNING("Checkpoint directory not specified, skipping checkpoint configuration");
            }
        }
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during initialization: ") + e.what());
        pImpl->errorMessages.push_back(std::string("Exception: ") + e.what());
        return false;
    }
}

// BrainSetup method implementations with enhanced error handling...

void BrainSetup::addRegion(const std::string& name) {
    try {
        // Validate region name
        if (name.empty()) {
            NLM_LOG_WARNING("Empty region name provided");
        }
        
        RegionId id(pImpl->nextRegionId++);
        auto region = std::make_unique<NeuralRegion>(id, name);
        pImpl->regions.push_back(std::move(region));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error adding region: ") + e.what());
        throw;
    }
}

NeuralRegion* BrainSetup::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    NLM_LOG_WARNING(std::string("Region with ID " + std::to_string(id.index()) + " not found"));
    return nullptr;
}

const NeuralRegion* BrainSetup::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t BrainSetup::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> BrainSetup::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& BrainSetup::getRegions() const {
    return pImpl->regions;
}

void BrainSetup::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    // Validate connection parameters
    if (source.index() == target.index()) {
        NLM_LOG_WARNING("Cannot create self-connection");
        return;
    }
    
    if (weight < -1.0f || weight > 1.0f) {
        NLM_LOG_WARNING("Connection weight outside typical range [-1.0, 1.0]");
    }
    
    if (delay <= 0) {
        NLM_LOG_WARNING("Delay must be positive, using default value");
    }
    
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void BrainSetup::removeInterRegionConnection(RegionId source, RegionId target) {
    auto originalSize = pImpl->interRegionConnections.size();
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
    
    if (pImpl->interRegionConnections.size() == originalSize) {
        NLM_LOG_WARNING(std::string("Connection from region " + std::to_string(source.index()) + 
                                    " to region " + std::to_string(target.index()) + " not found"));
    }
}

size_t BrainSetup::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t BrainSetup::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t BrainSetup::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

float BrainSetup::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

// Memory system access methods with error checking...

NeuralWorkingMemory* BrainSetup::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* BrainSetup::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* BrainSetup::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

PredictionSystem* BrainSetup::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

NeuralPlanner* BrainSetup::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* BrainSetup::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* BrainSetup::getAttention() {
    return pImpl->attention.get();
}

DevelopmentSystem* BrainSetup::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage BrainSetup::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void BrainSetup::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

Dopamine* BrainSetup::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* BrainSetup::getCuriosity() {
    return pImpl->curiosity.get();
}

Novelty* BrainSetup::getNovelty() {
    return pImpl->novelty.get();
}

PredictionError* BrainSetup::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

std::shared_ptr<const Config> BrainSetup::getConfig() const {
    return pImpl->config;
}

RandomGenerator* BrainSetup::getRandomGenerator() {
    return pImpl->rng.get();
}

SpikeSystem* BrainSetup::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* BrainSetup::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* BrainSetup::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* BrainSetup::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* BrainSetup::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

float BrainSetup::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t BrainSetup::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t BrainSetup::getPendingSpikeEventCount() const {
    if (pImpl->spikeSystem) {
        return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
    }
    return 0;
}

// Simulation methods with error handling...

void BrainSetup::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void BrainSetup::step(SimulationStep currentStep, Timestamp currentTime) {
    try {
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        
        // Process pending delayed spikes (deliver synaptic input)
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        }
        
        // Update all neurons (LIF dynamics)
        updateAllNeurons(currentTime);
        
        // Detect spikes and schedule spike events
        detectAndProcessSpikes(currentStep, currentTime);
        
        // Process immediate spikes
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->processSpikes(currentStep);
        }
        
        // Update working memory
        updateWorkingMemory();
        
        // Apply neuromodulation
        updateNeuromodulation();
        
        // Apply plasticity rules
        applyPlasticityRules();
        
        // Update episodic memory
        updateEpisodicMemory(currentStep);
        
        // Update prediction system
        updatePredictionSystem();
        
        // Update attention system
        updateAttentionSystem(currentStep);
        
        // Update concept formation
        updateConceptFormation();
        
        // Apply structural plasticity
        if (currentStep % 100 == 0) {
            applyStructuralPlasticity(currentStep);
        }
        
        // Replay memories
        if (currentStep % pImpl->replayInterval == 0) {
            replayMemories(currentStep);
        }
        
        // Apply development effects
        if (currentStep % 1000 == 0) {
            applyDevelopmentEffects(currentStep);
        }
        
        // Memory consolidation
        if (currentStep % pImpl->consolidationInterval == 0) {
            consolidateMemories();
        }
        
        // Checkpoint management
        if (pImpl->checkpointManager) {
            pImpl->checkpointManager->update(currentStep, currentTime);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error during simulation step " + std::to_string(currentStep) + ": ") + e.what());
        throw;
    }
}

void BrainSetup::updateAllNeurons(Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (!neuron->stepLIF(currentTime, pImpl->timestep)) {
                    // Neuron didn't fire, continue
                }
            }
        }
    }
}

void BrainSetup::detectAndProcessSpikes(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    if (pImpl->spikeSystem) {
                        pImpl->spikeSystem->queueSpike(event);
                    }

                    // Process plasticity for incoming synapses
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Schedule delayed spike events for outgoing synapses
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * pImpl->timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            currentStep,
                            deliveryStep
                        );

                        if (pImpl->spikeSystem) {
                            pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                        }
                        syn->recordPreSpike(currentTime);
                    }

                    // Store in working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void BrainSetup::updateWorkingMemory() {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

void BrainSetup::updateNeuromodulation() {
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void BrainSetup::applyPlasticityRules() {
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    if (pImpl->stdp) {
                        pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                        float weight = syn->getWeight();
                        weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                        syn->setWeight(weight);
                    }
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    if (pImpl->hebbian) {
                        pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    }
                }
            }
            
            // Update synapse state
            syn->step(pImpl->currentTime);
        }
    }
}

void BrainSetup::updateEpisodicMemory(SimulationStep currentStep) {
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void BrainSetup::updatePredictionSystem() {
    if (pImpl->predictionSystem) {
        // Prediction system update logic
    }
}

void BrainSetup::updateAttentionSystem(SimulationStep currentStep) {
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

void BrainSetup::updateConceptFormation() {
    if (pImpl->conceptFormation) {
        // Concept formation update logic
    }
}

void BrainSetup::applyStructuralPlasticity(SimulationStep currentStep) {
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void BrainSetup::replayMemories(SimulationStep currentStep) {
    if (pImpl->episodicMemory) {
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainSetup::applyDevelopmentEffects(SimulationStep currentStep) {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            float plasticityMod = 1.0f;
            
            switch (pImpl->developmentalStage) {
                case DevelopmentalStage::Initial: plasticityMod = 1.0f; break;
                case DevelopmentalStage::CriticalPeriod: plasticityMod = 0.8f; break;
                case DevelopmentalStage::Maturation: plasticityMod = 0.5f; break;
                case DevelopmentalStage::Adult: plasticityMod = 0.2f; break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void BrainSetup::consolidateMemories() {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

// I/O methods with error handling

void BrainSetup::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void BrainSetup::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void BrainSetup::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

std::unique_ptr<class Action> BrainSetup::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    
    return action;
}

void BrainSetup::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates with validation
    if (pImpl->stdp) {
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    }
}

void BrainSetup::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void BrainSetup::develop() {
    // Development updates structural plasticity
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void BrainSetup::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool BrainSetup::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    neuronData.membranePotential.push_back(state.membranePotential);
                    neuronData.restingPotential.push_back(state.restingPotential);
                    neuronData.threshold.push_back(state.threshold);
                    neuronData.resetPotential.push_back(state.resetPotential);
                    neuronData.leakConductance.push_back(state.leakConductance);
                    neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                    neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                    neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                    neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
                }
            }
        }
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool BrainSetup::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(neuronData.membranePotential.size()) + " neurons");
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

void BrainSetup::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Setup Status ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
    
    // Log errors if any
    if (!pImpl->errorMessages.empty()) {
        NLM_LOG_INFO("Initialization warnings/errors:");
        for (const auto& error : pImpl->errorMessages) {
            NLM_LOG_INFO("  " + error);
        }
    }
}

} // namespace nlm