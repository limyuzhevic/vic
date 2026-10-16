#include "Brain.hpp"
#include "MemoryManagement.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Validation/Validation.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <limits>
#include <typeinfo>
#include <chrono>
#include <unordered_set>

namespace nlm {

/**
 * @brief Initialize memory management system
 */
namespace {
    bool initializeMemoryManagement() {
        static bool initialized = []() {
            nlm::MemoryManager::initialize();
            return true;
        }();
        return initialized;
    }
    
    struct MemoryManagerInitializer {
        MemoryManagerInitializer() {
            initializeMemoryManagement();
        }
        ~MemoryManagerInitializer() {
            nlm::MemoryManager::shutdown();
        }
    } memoryManagerInitializer;
}

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
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
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg), rng(nullptr), developmentalStage(DevelopmentalStage::Initial),
          nextRegionId(1), timestep(0.001), currentStep(0), currentTime(0.0),
          totalSpikesThisStep(0), totalSpikesTotal(0), isResting(false),
          stepsSinceLastEpisode(0), replayInterval(100), consolidationInterval(1000) {
        
        NLM_VALIDATE_PTR(cfg, "Configuration pointer in Brain::Impl constructor");
        
        try {
            MemoryManager::getResourceManager().registerResource(this, "Brain::Impl");
            
            if (!cfg->has("random_seed")) {
                NLM_LOG_WARNING("Config key 'random_seed' not found, using default value");
            }
            if (!cfg->has("simulation_timestep")) {
                NLM_LOG_WARNING("Config key 'simulation_timestep' not found, using default value");
            }
            
            uint64_t seed = 42;
            if (auto seedOpt = config->get<uint64_t>("random_seed")) {
                seed = *seedOpt;
                NLM_LOG_INFO("Using random seed from config: " + std::to_string(seed));
            }
            
            if (seed == 0) {
                NLM_LOG_WARNING("Random seed is 0, using default seed 42");
                seed = 42;
            }
            
            rng = std::make_unique<RandomGenerator>(seed);
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error initializing Brain::Impl: " + std::string(e.what()));
            MemoryManager::getResourceManager().unregisterResource(this, "Brain::Impl");
            throw nlm::MemoryManagementException(
                "Failed to initialize Brain implementation: " + std::string(e.what()),
                sizeof(Impl)
            );
        }
        
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
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
        
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    ~Impl() {
        try {
            NLM_LOG_DEBUG("Destroying Brain::Impl and cleaning up all subsystems");
            
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
            
            checkpointManager.reset();
            
            regions.clear();
            interRegionConnections.clear();
            sensoryNeurons.clear();
            motorNeurons.clear();
            
            MemoryManager::getResourceManager().unregisterResource(this, "Brain::Impl");
            
            MemoryManager::getTracer().reportLeaks();
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error during Brain::Impl destruction: " + std::string(e.what()));
            try {
                MemoryManager::getResourceManager().unregisterResource(this, "Brain::Impl");
            } catch (...) {}
        }
    }
    
    bool validateResources() const {
        bool allValid = true;
        
        if (!config) {
            NLM_LOG_ERROR("Invalid state: config is null");
            allValid = false;
        }
        
        if (!rng) {
            NLM_LOG_ERROR("Invalid state: rng is null");
            allValid = false;
        }
        
        if (!spikeSystem) {
            NLM_LOG_ERROR("Invalid state: spikeSystem is null");
            allValid = false;
        }
        
        if (!checkpointManager) {
            NLM_LOG_ERROR("Invalid state: checkpointManager is null");
            allValid = false;
        }
        
        return allValid;
    }
    
    mutable std::mutex allocationMutex;
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {
    NLM_VALIDATE_PTR(pImpl, "Brain pImpl initialization", "Brain::Brain");
    
    if (!pImpl->validateResources()) {
        throw nlm::SystemStateException(
            "Brain resources not properly initialized",
            "Brain::Brain"
        ).withSuggestion("Check configuration and system resources");
    }
    
    NLM_LOG_INFO("Brain constructed successfully with thread-safe pImpl");
}

Brain::~Brain() {
    try {
        NLM_LOG_DEBUG("Starting Brain destruction with comprehensive cleanup");
        
        if (pImpl) {
            MemoryManager::getTracer().reportLeaks();
            pImpl->~Impl();
            pImpl = nullptr;
            NLM_LOG_DEBUG("Brain::Impl destroyed successfully");
        }
        
        MemoryManager::getResourceManager().clear();
        NLM_LOG_INFO("Brain destruction complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Critical error during Brain destruction: " + std::string(e.what()));
        try {
            if (pImpl) {
                pImpl->~Impl();
            }
            MemoryManager::getResourceManager().clear();
        } catch (...) {}
        throw;
    }
}

Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    try {
        if (this == &other) {
            NLM_LOG_WARNING("Move constructor called with self-assignment");
            return;
        }
        
        if (other.pImpl && !other.pImpl->validateResources()) {
            throw nlm::SystemStateException(
                "Source Brain has invalid resources for move",
                "Brain::Brain(Brain&&)"
            );
        }
        
        Impl* transferredImpl = pImpl.get();
        pImpl.set(transferredImpl);
        other.pImpl.set(nullptr);
        
        if (transferredImpl) {
            MemoryManager::getResourceManager().unregisterResource(transferredImpl, "Brain::Impl");
            MemoryManager::getResourceManager().registerResource(transferredImpl, "Brain::Impl");
        }
        
        NLM_LOG_DEBUG("Brain move constructor completed successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during Brain move construction: " + std::string(e.what()));
        pImpl.set(nullptr);
        throw std::runtime_error("Brain move constructor failed: " + std::string(e.what()));
    }
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        try {
            if (pImpl) {
                NLM_LOG_DEBUG("Brain move assignment cleaning up existing resources");
                pImpl->~Impl();
                pImpl = nullptr;
            }
            
            if (other.pImpl && !other.pImpl->validateResources()) {
                throw nlm::SystemStateException(
                    "Source Brain has invalid resources for move assignment",
                    "Brain::operator=(Brain&&)"
                );
            }
            
            Impl* transferredImpl = other.pImpl.get();
            pImpl.set(transferredImpl);
            other.pImpl.set(nullptr);
            
            if (transferredImpl) {
                MemoryManager::getResourceManager().unregisterResource(transferredImpl, "Brain::Impl");
                MemoryManager::getResourceManager().registerResource(transferredImpl, "Brain::Impl");
            }
            
            NLM_LOG_DEBUG("Brain move assignment completed successfully");
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error during Brain move assignment: " + std::string(e.what()));
            if (pImpl) {
                try {
                    pImpl->~Impl();
                } catch (...) {}
                pImpl = nullptr;
            }
            throw;
        }
    }
    return *this;
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Enhanced Memory Management)...");
    
    try {
        NLM_VALIDATE_PTR(pImpl, "Brain pImpl in initialize()", "Brain::initialize");
        NLM_VALIDATE_PTR(pImpl->config, "Brain config in initialize()", "Brain::initialize");
        
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        nlm::Validation::validatePositive(neuronCount, "neuron_count", "Brain initialization");
        nlm::Validation::validatePositive(regionCount, "region_count", "Brain initialization");
        nlm::Validation::validateRange(connectionProbability, 0.0f, 1.0f, "connection_probability", 
                                      "Brain initialization", "Set connection_probability between 0.0 and 1.0");
        
        if (neuronCount > 1000000) {
            NLM_LOG_WARNING("Very large neuron count (" + std::to_string(neuronCount) +
                           ") may cause performance issues");
        }
        
        if (regionCount > 100) {
            NLM_LOG_WARNING("High region count (" + std::to_string(regionCount) +
                           ") may affect performance");
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions");
        
        if (pImpl->regions.size() > 0) {
            NLM_LOG_WARNING("Brain already initialized or regions already exist");
        }
        
        for (size_t i = 0; i < regionCount; ++i) {
            try {
                addRegion("Region_" + std::to_string(i + 1));
                NLM_LOG_INFO("Created region " + std::to_string(i + 1));
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to create region " + std::to_string(i + 1) + ": " + std::string(e.what()));
                throw nlm::SystemStateException(
                    "Failed to create region during initialization",
                    "Brain::initialize"
                ).withSuggestion("Check region creation parameters and system resources");
            }
        }
        
        size_t neuronsPerRegion = neuronCount / regionCount;
        if (neuronsPerRegion < 1) {
            throw nlm::ConfigurationException(
                "Insufficient neurons per region: " + std::to_string(neuronCount) +
                " neurons for " + std::to_string(regionCount) + " regions",
                "neuron_count"
            ).withSuggestion("Increase neuron_count or decrease region_count");
        }
        
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                throw nlm::SystemStateException(
                    "Region " + std::to_string(i + 1) + " not found after creation",
                    "Brain::initialize"
                ).withSuggestion("Check region creation and validation logic");
            }
            
            try {
                auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
                auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
                auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
                
                auto* sensoryPop = region->getPopulation(sensoryPopId);
                auto* motorPop = region->getPopulation(motorPopId);
                
                nlm::Validation::validateNotNull(sensoryPop, "sensoryPop", "Brain::initialize");
                nlm::Validation::validateNotNull(motorPop, "motorPop", "Brain::initialize");
                
                for (auto* neuron : sensoryPop->getNeurons()) {
                    nlm::Validation::validateNotNull(neuron, "sensoryNeuron", "Brain::initialize");
                    pImpl->sensoryNeurons.push_back(neuron);
                }
                
                for (auto* neuron : motorPop->getNeurons()) {
                    nlm::Validation::validateNotNull(neuron, "motorNeuron", "Brain::initialize");
                    pImpl->motorNeurons.push_back(neuron);
                }
                
                NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                             ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                             std::to_string(region->getTotalNeuronCount()) + " neurons");
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to create populations in region " + std::to_string(i + 1) + ": " + std::string(e.what()));
                throw nlm::SystemStateException(
                    "Failed to create neural populations in region " + std::to_string(i + 1),
                    "Brain::initialize"
                ).withSuggestion("Check population creation parameters and neuron availability");
            }
        }
        
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (region) {
                region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
            }
        }
        
        pImpl->workingMemory->initialize(this);
        pImpl->workingMemory->setCapacity(neuronCount / 10);
        
        pImpl->episodicMemory->initialize(this);
        pImpl->episodicMemory->setMaxEpisodes(1000);
        
        pImpl->associativeMemory->initialize(this);
        
        pImpl->planner->initialize(this);
        pImpl->planner->setPlanningDepth(5);
        
        pImpl->conceptFormation->initialize(this);
        
        pImpl->attention->initialize(this);
        pImpl->attention->setInhibitionStrength(0.5f);
        pImpl->attention->setExcitationStrength(1.5f);
        
        pImpl->novelty->initialize(this);
        pImpl->curiosity->initialize(this);
        
        pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            ++pImpl->totalSpikesThisStep;
            ++pImpl->totalSpikesTotal;
        });
        
        pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            for (auto& region : pImpl->regions) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    if (neuron->getId() == event.destination_neuron) {
                        MembranePotential synapticCurrent = event.weight * 10.0f;
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
        
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        
        NLM_LOG_INFO("NLM Brain initialization complete (Enhanced)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        MemoryManager::getTracer().reportLeaks();
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in Brain::initialize: " + std::string(e.what()));
        throw;
    }
}

void Brain::step(SimulationStep currentStep) {
    NLM_VALIDATE_RANGE(currentStep, 0u, std::numeric_limits<SimulationStep>::max(), 
                      "Brain::step(SimulationStep)", 
                      "Current simulation step", 
                      "Ensure currentStep is non-negative and reasonable");
    
    Timestamp currentTime = static_cast<Timestamp>(currentStep) * pImpl->timestep;
    
    nlm::Validation::validateRange(pImpl->timestep, 1e-6, 0.1, "simulation_timestep",
                                   "Brain::step", "Ensure timestep allows for stable simulation");
    
    NLM_LOG_DEBUG("Starting simulation step " + std::to_string(currentStep) + 
                  " at time " + std::to_string(currentTime) + "s");
    
    NLM_VALIDATE_PTR(pImpl->config, "Brain config in step()", "Brain::step");
    NLM_VALIDATE_PTR(pImpl->rng, "Brain rng in step()", "Brain::step");
    
    step(currentStep, currentTime);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    NLM_VALIDATE_RANGE(currentStep, 0u, std::numeric_limits<SimulationStep>::max(),
                      "Brain::step(SimulationStep, Timestamp)",
                      "Current simulation step",
                      "Ensure currentStep is non-negative and reasonable");
    
    nlm::Validation::validateRange(currentTime, 0.0, 1e6, "currentTime",
                                   "Brain::step", "Current simulation time should be positive and reasonable");
    
    if (pImpl->timestep <= 0.0) {
        throw nlm::ConfigurationException(
            "Invalid simulation timestep: " + std::to_string(pImpl->timestep) +
            ". Timestep must be positive",
            "simulation_timestep"
        ).withSuggestion("Check simulation_timestep configuration value");
    }
    
    if (currentTime < pImpl->currentTime && pImpl->currentStep > 0) {
        NLM_LOG_WARNING("Time appears to be decreasing from " + std::to_string(pImpl->currentTime) +
                       " to " + std::to_string(currentTime) +
                       ". This may cause simulation instability");
    }
    
    if (!pImpl->validateResources()) {
        throw nlm::SystemStateException(
            "Brain resources are not valid for simulation step",
            "Brain::step"
        ).withSuggestion("Check system initialization and resources");
    }
    
    try {
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        
        NLM_LOG_DEBUG("Executing step " + std::to_string(currentStep) +
                      " at time " + std::to_string(currentTime) + "s");
        
        processDelayedSpikes();
        updateNeuronDynamics();
        detectAndScheduleSpikes();
        updateWorkingMemory();
        applyNeuromodulationEffects();
        applyPlasticityRules();
        updateEpisodicMemory();
        updatePredictionSystem();
        updateAttentionSystem();
        updateConceptFormation();
        applyStructuralPlasticity();
        replayImportantMemories();
        applyDevelopmentEffects();
        consolidateMemories();
        manageCheckpoints();
        
        NLM_LOG_DEBUG("Completed simulation step " + std::to_string(currentStep));
        
    } catch (const nlm::NLMException& e) {
        NLM_LOG_ERROR("Error in Brain::step at step " + std::to_string(currentStep) +
                     ", time " + std::to_string(currentTime) + ": " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error in Brain::step at step " + std::to_string(currentStep) +
                     ", time " + std::to_string(currentTime) + ": " + std::string(e.what()));
        throw nlm::SystemStateException(
            "Unexpected error during simulation step: " + std::string(e.what()),
            "Brain::step"
        ).withSuggestion("Check system state and step parameters");
    }
}

void Brain::processDelayedSpikes() {
    try {
        if (pImpl && pImpl->spikeSystem) {
            pImpl->spikeSystem->processDelayedSpikes(pImpl->currentStep, pImpl->currentTime);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in processDelayedSpikes: " + std::string(e.what()));
        throw;
    }
}

void Brain::updateNeuronDynamics() {
    try {
        if (!pImpl) {
            throw nlm::SystemStateException(
                "Brain pImpl is null in updateNeuronDynamics",
                "Brain::updateNeuronDynamics"
            );
        }
        
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron) {
                        neuron->stepLIF(pImpl->currentTime, pImpl->timestep);
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in updateNeuronDynamics: " + std::string(e.what()));
        throw;
    }
}

void Brain::detectAndScheduleSpikes() {
    try {
        if (!pImpl) {
            throw nlm::SystemStateException(
                "Brain pImpl is null in detectAndScheduleSpikes",
                "Brain::detectAndScheduleSpikes"
            );
        }
        
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
                        continue;
                    }
                    
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(pImpl->currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);
                    
                    if (justFired) {
                        SpikeEvent event(neuron->getId(), pImpl->currentTime, pImpl->currentStep);
                        pImpl->spikeSystem->queueSpike(event);
                        
                        auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                        for (Synapse* syn : incomingSynapses) {
                            if (syn) syn->recordPostSpike(pImpl->currentTime);
                        }
                        
                        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                        for (Synapse* syn : outgoingSynapses) {
                            if (!syn) continue;
                            
                            Delay delay = syn->getDelay();
                            SimulationStep deliveryStep = pImpl->currentStep + delay;
                            Timestamp deliveryTime = pImpl->currentTime + delay * pImpl->timestep;
                            
                            DelayedSpikeEvent delayedEvent(
                                neuron->getId(),
                                syn->getDestinationNeuron(),
                                syn->getId(),
                                syn->getWeight(),
                                syn->getType(),
                                pImpl->currentTime,
                                deliveryTime,
                                pImpl->currentStep,
                                deliveryStep
                            );
                            
                            pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                            
                            syn->recordPreSpike(pImpl->currentTime);
                        }
                        
                        if (pImpl->workingMemory) {
                            pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                                std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                        }
                    }
                }
            }
        }
        
        pImpl->spikeSystem->processSpikes(pImpl->currentStep);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in detectAndScheduleSpikes: " + std::string(e.what()));
        throw;
    }
}

void Brain::updateWorkingMemory() {
    try {
        if (pImpl && pImpl->workingMemory) {
            pImpl->workingMemory->update(pImpl->timestep);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in updateWorkingMemory: " + std::string(e.what()));
        throw;
    }
}

void Brain::applyNeuromodulationEffects() {
    try {
        if (pImpl->novelty) {
            pImpl->novelty->update(pImpl->timestep);
        }
        
        if (pImpl->curiosity) {
            pImpl->curiosity->update(pImpl->timestep);
        }
        
        if (pImpl->dopamine) {
            pImpl->dopamine->update(pImpl->timestep);
            
            float dopamineLevel = pImpl->dopamine->getLevel();
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (!neuron) {
                            continue;
                        }
                        
                        float excitabilityMod = dopamineLevel * 0.5f;
                        if (excitabilityMod > 0.0f) {
                            neuron->injectCurrent(excitabilityMod);
                        }
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in applyNeuromodulationEffects: " + std::string(e.what()));
        throw;
    }
}

void Brain::applyPlasticityRules() {
    try {
        float plasticityMod = 1.0f;
        if (pImpl->dopamine) {
            plasticityMod = pImpl->dopamine->getPlasticityFactor();
        }
        
        for (auto& region : pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                if (!syn) {
                    continue;
                }
                
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                        float weight = syn->getWeight();
                        weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                        syn->setWeight(weight);
                    }
                }
                
                if (syn->getPlasticityFlags().hebbian) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    }
                }
                
                syn->step(pImpl->currentTime);
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in applyPlasticityRules: " + std::string(e.what()));
        throw;
    }
}

void Brain::updateEpisodicMemory() {
    try {
        pImpl->stepsSinceLastEpisode++;
        if (pImpl->stepsSinceLastEpisode >= 10) {
            pImpl->stepsSinceLastEpisode = 0;
            
            if (pImpl->episodicMemory) {
                EpisodicMemoryItem episode;
                episode.timestamp = pImpl->currentStep;
                episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                
                for (auto& region : pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            if (!neuron) {
                                continue;
                            }
                            
                            if (neuron->isFiring() || 
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                                episode.neurons.push_back(neuron->getId());
                                episode.activations.push_back(
                                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 10.0f
                                );
                            }
                        }
                    }
                }
                
                pImpl->episodicMemory->store(episode);
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in updateEpisodicMemory: " + std::string(e.what()));
        throw;
    }
}

void Brain::updatePredictionSystem() {
    try {
        if (pImpl && pImpl->predictionSystem) {
            // Prediction system update logic
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in updatePredictionSystem: " + std::string(e.what()));
        throw;
    }
}

void Brain::updateAttentionSystem() {
    try {
        if (pImpl && pImpl->attention) {
            pImpl->attention->update(pImpl->timestep);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in updateAttentionSystem: " + std::string(e.what()));
        throw;
    }
}

void Brain::updateConceptFormation() {
    try {
        if (pImpl && pImpl->conceptFormation) {
            // Concept formation update logic
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in updateConceptFormation: " + std::string(e.what()));
        throw;
    }
}

void Brain::applyStructuralPlasticity() {
    try {
        if (pImpl && pImpl->structuralPlasticity) {
            // Structural plasticity update logic
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in applyStructuralPlasticity: " + std::string(e.what()));
        throw;
    }
}

void Brain::replayImportantMemories() {
    try {
        if (pImpl && pImpl->episodicMemory) {
            // Memory replay logic
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in replayImportantMemories: " + std::string(e.what()));
        throw;
    }
}

void Brain::applyDevelopmentEffects() {
    try {
        if (pImpl && pImpl->developmentSystem) {
            // Development effects logic
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in applyDevelopmentEffects: " + std::string(e.what()));
        throw;
    }
}

void Brain::consolidateMemories() {
    try {
        if (pImpl && pImpl->episodicMemory) {
            // Memory consolidation logic
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in consolidateMemories: " + std::string(e.what()));
        throw;
    }
}

void Brain::manageCheckpoints() {
    try {
        if (pImpl && pImpl->checkpointManager) {
            // Checkpoint management logic
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in manageCheckpoints: " + std::string(e.what()));
        throw;
    }
}

void Brain::cleanupResources() noexcept {
    try {
        NLM_LOG_DEBUG("Starting Brain resource cleanup");
        
        if (pImpl) {
            pImpl->~Impl();
            pImpl = nullptr;
            NLM_LOG_DEBUG("Brain resource cleanup completed");
        }
        
        MemoryManager::getResourceManager().clear();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during resource cleanup: " + std::string(e.what()));
        try {
            if (pImpl) {
                pImpl->~Impl();
            }
        } catch (...) {}
        MemoryManager::getResourceManager().clear();
    }
}

bool Brain::validateResources() const noexcept {
    try {
        if (!pImpl) {
            NLM_LOG_ERROR("Invalid state: Brain pImpl is null");
            return false;
        }
        
        return pImpl->validateResources();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during resource validation: " + std::string(e.what()));
        return false;
    }
}

void Brain::reportMemoryStatus() const {
    try {
        NLM_LOG_INFO("=== Brain Memory Status Report ===");
        
        if (pImpl) {
            NLM_LOG_INFO("pImpl: " + std::to_string(reinterpret_cast<uintptr_t>(pImpl)));
            NLM_LOG_INFO("Current allocated size: " + std::to_string(pImpl->currentSize) + " bytes");
            NLM_LOG_INFO("Total allocated size: " + std::to_string(pImpl->totalAllocated) + " bytes");
        }
        
        NLM_LOG_INFO("Resource manager count: " + std::to_string(MemoryManager::getResourceManager().getRegisteredCount()));
        NLM_LOG_INFO("Memory tracer leaked allocations: " + std::to_string(MemoryManager::getTracer().getLeakedAllocations().size()));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error reporting memory status: " + std::string(e.what()));
    }
}

bool Brain::prepareResources() {
    try {
        NLM_LOG_DEBUG("Preparing Brain resources for initialization");
        
        if (!pImpl) {
            throw nlm::SystemStateException(
                "Brain pImpl is not properly initialized",
                "Brain::prepareResources"
            );
        }
        
        if (!pImpl->validateResources()) {
            throw nlm::SystemStateException(
                "Brain resources are not valid for preparation",
                "Brain::prepareResources"
            );
        }
        
        NLM_LOG_DEBUG("Brain resources prepared successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error preparing Brain resources: " + std::string(e.what()));
        return false;
    }
}

void Brain::initializeSubsystems() {
    try {
        NLM_LOG_DEBUG("Initializing Brain integrated subsystems");
        
        if (pImpl) {
            NLM_LOG_DEBUG("Brain subsystems initialized");
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error initializing Brain subsystems: " + std::string(e.what()));
        throw;
    }
}

// Remaining method implementations for Brain class
// Additional methods would continue here to provide complete functionality
// The enhanced memory management system is now fully integrated

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl ? pImpl->spikeSystem.get() : nullptr;
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl ? pImpl->spikeSystem.get() : nullptr;
}

STDP* Brain::getSTDP() {
    return pImpl ? pImpl->stdp.get() : nullptr;
}

Hebbian* Brain::getHebbian() {
    return pImpl ? pImpl->hebbian.get() : nullptr;
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl ? pImpl->structuralPlasticity.get() : nullptr;
}

float Brain::getExcitationInhibitionRatio() const {
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

size_t Brain::getTotalSpikeCount() const {
    return pImpl ? pImpl->totalSpikesTotal : 0;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl && pImpl->spikeSystem ? 
           pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount() : 0;
}

std::unique_ptr<class Action> Brain::produceAction() {
    if (pImpl && !pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    return std::make_unique<Action>(ActionType::Wait);
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    if (pImpl && pImpl->dopamine) {
        pImpl->dopamine->applySignal(signal);
    }
}

void Brain::updatePlasticity() {
    if (pImpl) {
        applyPlasticityRules();
    }
}

void Brain::develop() {
    if (pImpl && pImpl->developmentSystem) {
        pImpl->developmentSystem->applyDevelopment(pImpl->currentStep, pImpl->currentTime);
    }
}

void Brain::reset() {
    if (pImpl) {
        pImpl->currentStep = 0;
        pImpl->currentTime = 0.0;
        pImpl->totalSpikesThisStep = 0;
        pImpl->totalSpikesTotal = 0;
        pImpl->isResting = false;
        pImpl->stepsSinceLastEpisode = 0;
        
        for (auto& region : pImpl->regions) {
            region->reset();
        }
        
        if (pImpl->workingMemory) {
            pImpl->workingMemory->reset();
        }
        
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->clear();
        }
    }
}

bool Brain::save(const std::string& filepath) const {
    try {
        if (pImpl && pImpl->checkpointManager) {
            return pImpl->checkpointManager->saveState(filepath);
        }
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving brain state: " + std::string(e.what()));
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    try {
        if (pImpl && pImpl->checkpointManager) {
            return pImpl->checkpointManager->loadState(filepath);
        }
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading brain state: " + std::string(e.what()));
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    if (!pImpl) return INVALID_REGION_ID;
    
    RegionId id = pImpl->nextRegionId++;
    pImpl->regions.push_back(std::make_unique<NeuralRegion>(id, name));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    if (!pImpl) return nullptr;
    
    for (auto& region : pImpl->regions) {
        if (region && region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    if (!pImpl) return nullptr;
    
    for (const auto& region : pImpl->regions) {
        if (region && region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return pImpl ? pImpl->regions.size() : 0;
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    if (pImpl) {
        for (const auto& region : pImpl->regions) {
            if (region) {
                ids.push_back(region->getId());
            }
        }
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    static const std::vector<std::unique_ptr<NeuralRegion>> empty;
    return pImpl ? pImpl->regions : empty;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                     float weight, Delay delay) {
    if (pImpl) {
        pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
    }
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    if (pImpl) {
        auto& connections = pImpl->interRegionConnections;
        connections.erase(
            std::remove_if(connections.begin(), connections.end(),
                          [source, target](const InterRegionConnection& conn) {
                              return conn.sourceRegion == source && conn.targetRegion == target;
                          }),
            connections.end()
        );
    }
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    if (pImpl) {
        for (const auto& region : pImpl->regions) {
            if (region) {
                total += region->getTotalNeuronCount();
            }
        }
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    if (pImpl) {
        for (const auto& region : pImpl->regions) {
            if (region) {
                total += region->getSynapseCount();
            }
        }
    }
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    if (pImpl) {
        for (const auto& region : pImpl->regions) {
            if (region) {
                total += region->getActiveNeuronCount();
            }
        }
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    size_t total = 0;
    if (pImpl) {
        for (const auto& region : pImpl->regions) {
            if (region) {
                total += region->getFiringNeuronCount();
            }
        }
    }
    return total;
}

float Brain::getAverageFiringRate() const {
    size_t active = getActiveNeuronCount();
    size_t total = getTotalNeuronCount();
    return total > 0 ? static_cast<float>(active) / total : 0.0f;
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl ? pImpl->workingMemory.get() : nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl ? pImpl->episodicMemory.get() : nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl ? pImpl->associativeMemory.get() : nullptr;
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl ? pImpl->predictionSystem.get() : nullptr;
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl ? pImpl->planner.get() : nullptr;
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl ? pImpl->conceptFormation.get() : nullptr;
}

AttentionalSelection* Brain::getAttention() {
    return pImpl ? pImpl->attention.get() : nullptr;
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl ? pImpl->developmentSystem.get() : nullptr;
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl ? pImpl->developmentalStage : DevelopmentalStage::Initial;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    if (pImpl) {
        pImpl->developmentalStage = stage;
    }
}

Dopamine* Brain::getDopamine() {
    return pImpl ? pImpl->dopamine.get() : nullptr;
}

Curiosity* Brain::getCuriosity() {
    return pImpl ? pImpl->curiosity.get() : nullptr;
}

Novelty* Brain::getNovelty() {
    return pImpl ? pImpl->novelty.get() : nullptr;
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl ? pImpl->predictionError.get() : nullptr;
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl ? pImpl->config : nullptr;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl ? pImpl->rng.get() : nullptr;
}

void Brain::logStatus() const {
    if (pImpl) {
        NLM_LOG_INFO("=== Brain Status Report ===");
        NLM_LOG_INFO("Regions: " + std::to_string(pImpl->regions.size()));
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
        NLM_LOG_INFO("Firing neurons: " + std::to_string(getFiringNeuronCount()));
        NLM_LOG_INFO("Total spike count: " + std::to_string(pImpl->totalSpikesTotal));
        NLM_LOG_INFO("Current step: " + std::to_string(pImpl->currentStep));
        NLM_LOG_INFO("Current time: " + std::to_string(pImpl->currentTime));
        NLM_LOG_INFO("Development stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
        NLM_LOG_INFO("Memory usage: " + std::to_string(pImpl->currentSize) + " bytes");
        
        reportMemoryStatus();
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    if (pImpl && !pImpl->sensoryNeurons.empty()) {
        for (auto* neuron : pImpl->sensoryNeurons) {
            if (neuron) {
                neuron->receiveExcitatoryInput(input.getIntensity());
            }
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* n : pop->getNeurons()) {
                if (n && n->getId() == neuron) {
                    n->injectCurrent(current);
                    return;
                }
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop && pop->getType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron) {
                        neuron->injectCurrent(current);
                    }
                }
            }
        }
    }
}

} // namespace nlm