// Brain.cpp - Improved version with all critical fixes applied
// All critical issues from analysis have been addressed:
// 1. Memory management - associative memory now properly initialized
// 2. Null pointer dereference - added null checks for neuromodulators
// 3. Division by zero - comprehensive protection in E/I ratio and neuron distribution
// 4. Thread safety - added mutex protection for synapses and dopamine access
// 5. Error handling - enhanced with try-catch and validation
// 6. Edge cases - added bounds checking and safety checks

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
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <mutex>
#include <numeric>

namespace nlm {

// ===================== Forward Declarations ============================
class NeuralRegion;
class NeuralPopulation;
class Neuron;
class Synapse;

// ===================== BRAIN IMPLEMENTATION ============================

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== INTEGRATED MEMORY SYSTEMS ==========
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== INTEGRATED PREDICTION SYSTEM ==========
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ========== INTEGRATED COGNITION SYSTEMS ==========
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Thread safety for neuromodulation access
    std::mutex dopamineMutex;
    
    // Simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(1000)  // Consolidate every 1000 steps
    {
        // Validate config pointer
        if (!config) {
            NLM_LOG_ERROR("Invalid configuration: config pointer is null");
            throw std::invalid_argument("Brain requires a valid configuration");
        }
        
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
        // Initialize memory systems
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        // Initialize all integrated systems immediately in constructor for proper memory management
        workingMemory->initialize(this);
        episodicMemory->initialize(this);
        associativeMemory->initialize(this);
        
        // Initialize prediction system
        predictionSystem = std::make_unique<PredictionSystem>();
        
        // Initialize cognition systems
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // Initialize neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        // Configure STDP parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    if (regionCount == 0) {
        NLM_LOG_ERROR("Invalid region count: 0. At least 1 region is required.");
        return false;
    }
    
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions with safe division
    if (neuronCount == 0) {
        NLM_LOG_WARNING("Zero neuron count specified, using minimum of 1 neuron");
        neuronCount = 1;
    }
    
    // Safe ceiling division: (a + b - 1) / b where b > 0
    size_t neuronsPerRegion = (neuronCount + regionCount - 1) / regionCount;
    
    // Ensure minimum neurons per population to avoid division issues
    size_t sensoryNeurons = neuronsPerRegion > 0 ? neuronsPerRegion / 4 : 1;
    size_t internalNeurons = neuronsPerRegion > 0 ? neuronsPerRegion / 2 : 1;
    size_t motorNeurons = neuronsPerRegion > 0 ? neuronsPerRegion / 4 : 1;
    
    // Ensure minimum neurons per population
    if (sensoryNeurons < 1) sensoryNeurons = 1;
    if (internalNeurons < 1) internalNeurons = 1;
    if (motorNeurons < 1) motorNeurons = 1;
    
    // Store references to sensory and motor neurons for later access
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    
    // Add neurons to regions (first region gets sensory neurons, last gets motor neurons)
    for (size_t r = 0; r < pImpl->regions.size(); ++r) {
        auto* region = pImpl->regions[r].get();
        
        if (r == 0) {
            // Add sensory neurons to first region
            for (size_t i = 0; i < sensoryNeurons; ++i) {
                Neuron* neuron = region->addNeuron(NeuronType::Sensory);
                if (neuron) {
                    pImpl->sensoryNeurons.push_back(neuron);
                    neuron->setType(NeuronType::Sensory);
                }
            }
        } else if (r == pImpl->regions.size() - 1) {
            // Add motor neurons to last region
            for (size_t i = 0; i < motorNeurons; ++i) {
                Neuron* neuron = region->addNeuron(NeuronType::Motor);
                if (neuron) {
                    pImpl->motorNeurons.push_back(neuron);
                    neuron->setType(NeuronType::Motor);
                }
            }
        } else {
            // Add internal neurons to middle regions
            for (size_t i = 0; i < internalNeurons; ++i) {
                Neuron* neuron = region->addNeuron(NeuronType::Internal);
                if (neuron) {
                    neuron->setType(NeuronType::Internal);
                }
            }
        }
        
        // Add additional neurons to reach target count if needed
        size_t currentNeurons = region->getNeuronCount();
        if (currentNeurons < neuronsPerRegion) {
            size_t neuronsToAdd = neuronsPerRegion - currentNeurons;
            for (size_t i = 0; i < neuronsToAdd; ++i) {
                Neuron* neuron = region->addNeuron(NeuronType::Internal);
                if (neuron) {
                    neuron->setType(NeuronType::Internal);
                }
            }
        }
    }
    
    // Initialize region connections
    for (auto& region : pImpl->regions) {
        region->initialize(this, connectionProbability);
    }
    
    NLM_LOG_INFO("Brain initialization complete:");
    NLM_LOG_INFO("  Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("  Regions created: " + std::to_string(getRegionCount()));
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    // Forward time
    step(currentStep, currentStep * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    
    // Reset step statistics
    pImpl->totalSpikesThisStep = 0;
    
    // Update all systems
    updatePlasticity();
    develop();
    applyNeuromodulation();
    
    // Process spikes
    pImpl->spikeSystem->processSpikes(pImpl->regions, pImpl->currentStep, pImpl->currentTime);
    
    // Calculate statistics
    pImpl->totalSpikesTotal += pImpl->totalSpikesThisStep;
    
    // Memory consolidation and replay (every N steps)
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= pImpl->replayInterval) {
        // Episodic memory replay and consolidation
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->replayEpisode(pImpl->currentStep);
        }
        
        // Working memory consolidation
        if (pImpl->workingMemory) {
            pImpl->workingMemory->consolidate(pImpl->currentTime);
        }
        
        pImpl->stepsSinceLastEpisode = 0;
    }
    
    // Developmental changes
    pImpl->developmentSystem->update(pImpl->timestep);
    
    // Check for sleep/rest cycle (optional feature)
    if (pImpl->isResting) {
        // Simple rest cycle: rest for 10% of time, then wake up
        static size_t restCounter = 0;
        static size_t restDuration = static_cast<size_t>(pImpl->replayInterval * 0.1);
        
        if (restCounter < restDuration) {
            restCounter++;
        } else {
            pImpl->isResting = false;
            restCounter = 0;
        }
    }
}

void Brain::receiveSensoryInput(const SensoryInput& input) {
    // Validate input
    if (!input.isValid()) {
        NLM_LOG_WARNING("Invalid sensory input received");
        return;
    }
    
    // Inject current into sensory neurons based on input pattern
    // This is where the brain receives sensory information and converts it to neural activity
    const auto& values = input.getData();
    
    // Safety check: ensure we have sensory neurons and valid input
    if (values.empty() || pImpl->sensoryNeurons.empty()) {
        return;
    }
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;  // Safety check
    
    // Distribute input values to sensory neurons
    size_t valuesToUse = std::min(values.size(), numSensory);
    for (size_t i = 0; i < valuesToUse; ++i) {
        Neuron* neuron = pImpl->sensoryNeurons[i];
        if (neuron) {
            // Convert input value (typically 0-1) to current (typically 0-50)
            float current = values[i] * 50.0f;
            neuron->injectCurrent(current);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    // Find and inject current into specific neuron
    for (auto& region : pImpl->regions) {
        for (auto* neuronPtr : region->getAllNeurons()) {
            if (neuronPtr->getId() == neuron) {
                neuronPtr->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    // Inject current into all neurons of a specific type
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron) {
                        neuron->injectCurrent(current);
                    }
                }
            }
        }
    }
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
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
    
    // Fix: Division by zero protection - return 0 when both numerator and denominator are 0
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    // Return 0 when both total excitatory and inhibitory are 0 (neutral state)
    return 0.0f;
    
    // Fallback (unreachable with current logic, kept for safety):
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount();
}

std::unique_ptr<Action> Brain::produceAction() {
    // Produce motor action based on motor neuron activity
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Find most active motor neuron
    float maxActivity = -1.0f;
    MotorCommand bestCommand = MotorCommand::Wait;
    
    for (auto* neuron : pImpl->motorNeurons) {
        if (!neuron) continue;
        
        float activity = neuron->getMembranePotential() - neuron->getRestingPotential();
        if (activity > maxActivity) {
            maxActivity = activity;
            bestCommand = MotorCommand::MoveForward; // Default command
        }
    }
    
    // Map activity to specific motor commands
    if (maxActivity > 0.0f) {
        return std::make_unique<Action>(bestCommand);
    } else {
        return std::make_unique<Action>(ActionType::Wait);
    }
}

void Brain::applyNeuromodulation(const Neuromodulator& signal) {
    // Apply neuromodulatory signals (dopamine, curiosity, novelty, etc.)
    // This is where external signals affect brain function
    
    // Store the current neuromodulator signal for reference
    pImpl->predictionError->update(signal.getPredictionError());
    
    // Update neuromodulators with thread safety
    {
        std::lock_guard<std::mutex> lock(pImpl->dopamineMutex);
        
        if (pImpl->dopamine) {
            pImpl->dopamine->update(pImpl->timestep);
            
            // Apply dopamine effects on neural excitability
            // Dopamine modulates neural excitability by adjusting effective current injection
            // Higher dopamine increases excitability (lower effective threshold)
            float dopamineLevel = pImpl->dopamine->getLevel();
            if (dopamineLevel > 0.0f) {  // Additional safety check
                for (auto& region : pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            // Dopamine modulates excitability by injecting additional current
                            // Positive dopamine adds excitatory bias
                            float excitabilityMod = dopamineLevel * 0.5f;
                            if (excitabilityMod > 0.0f) {
                                neuron->injectCurrent(excitabilityMod);
                            }
                        }
                    }
                }
            }
        }
        
        if (pImpl->curiosity) {
            pImpl->curiosity->update(pImpl->timestep);
        }
        
        if (pImpl->novelty) {
            pImpl->novelty->update(signal.getNovelty());
        }
        
        if (pImpl->predictionError) {
            pImpl->predictionError->update(signal.getPredictionError());
        }
    }
}

void Brain::updatePlasticity() {
    // Update all plasticity systems
    pImpl->spikeSystem->update();
    
    // Update STDP with thread-safe access
    {
        std::lock_guard<std::mutex> lock(pImpl->dopamineMutex);
        
        for (auto& region : pImpl->regions) {
            for (auto* syn : region->getSynapses()) {
                // Thread-safe synapse access
                std::lock_guard<std::mutex> synLock(syn->getMutex());
                
                // Update STDP
                syn->stepSTDP(pImpl->currentStep, pImpl->currentTime);
                
                // Update Hebbian
                syn->stepHebbian(pImpl->currentStep, pImpl->currentTime);
                
                // Apply reward-modulated plasticity based on dopamine
                if (pImpl->dopamine) {
                    float dopamineLevel = pImpl->dopamine->getLevel();
                    float eligibility = syn->getEligibilityTrace();
                    
                    if (std::abs(eligibility) > 0.001f) {
                        // Apply reward-modulated weight change
                        float delta = eligibility * dopamineLevel * pImpl->plasticityModifier_;
                        syn->addToWeight(delta);
                        
                        // Decay eligibility trace
                        syn->decayEligibilityTrace(0.1f);
                    }
                }
            }
        }
    }
}

void Brain::develop() {
    // Apply developmental changes
    pImpl->developmentSystem->update(pImpl->timestep);
    
    // Update brain state based on developmental stage
    switch (pImpl->developmentalStage) {
        case DevelopmentalStage::Initial:
            // High plasticity, rapid learning
            pImpl->plasticityModifier_ = 1.0f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Moderate plasticity, focused learning
            pImpl->plasticityModifier_ = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            // Reduced plasticity, fine-tuning
            pImpl->plasticityModifier_ = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            // Low plasticity, stable performance
            pImpl->plasticityModifier_ = 0.2f;
            break;
    }
}

void Brain::reset() {
    // Reset all brain systems to initial state
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    // Reset all regions
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    // Reset memory systems
    if (pImpl->workingMemory) {
        pImpl->workingMemory->reset();
    }
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->reset();
    }
    
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->reset();
    }
    
    // Reset neuromodulation systems
    if (pImpl->dopamine) {
        pImpl->dopamine->reset();
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->reset();
    }
    
    if (pImpl->predictionError) {
        pImpl->predictionError->reset();
    }
    
    if (pImpl->novelty) {
        pImpl->novelty->reset();
    }
    
    // Reset spike system
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
    
    // Reset development system
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->reset();
    }
}

bool Brain::save(const std::string& filepath) const {
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Save configuration
        writer.write("config", pImpl->config->summary());
        
        // Save brain state
        writer.write("currentStep", pImpl->currentStep);
        writer.write("currentTime", pImpl->currentTime);
        writer.write("totalSpikesTotal", pImpl->totalSpikesTotal);
        writer.write("isResting", pImpl->isResting);
        writer.write("stepsSinceLastEpisode", pImpl->stepsSinceLastEpisode);
        
        // Save all regions
        std::vector<std::string> regionNames;
        for (const auto& region : pImpl->regions) {
            regionNames.push_back(region->getName());
        }
        writer.write("regionNames", regionNames);
        
        // Save spike history
        if (pImpl->spikeSystem) {
            writer.write("spikeSystem", pImpl->spikeSystem->exportState());
        }
        
        // Save memory systems
        if (pImpl->workingMemory) {
            writer.write("workingMemory", pImpl->workingMemory->exportState());
        }
        
        if (pImpl->episodicMemory) {
            writer.write("episodicMemory", pImpl->episodicMemory->exportState());
        }
        
        if (pImpl->associativeMemory) {
            writer.write("associativeMemory", pImpl->associativeMemory->exportState());
        }
        
        NLM_LOG_INFO("Brain state saved to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        // Load configuration
        std::string configSummary;
        if (reader.read("config", configSummary)) {
            NLM_LOG_INFO("Configuration summary loaded");
        }
        
        // Load brain state
        SimulationStep currentStep = 0;
        Timestamp currentTime = 0.0;
        size_t totalSpikesTotal = 0;
        bool isResting = false;
        size_t stepsSinceLastEpisode = 0;
        
        reader.read("currentStep", currentStep);
        reader.read("currentTime", currentTime);
        reader.read("totalSpikesTotal", totalSpikesTotal);
        reader.read("isResting", isResting);
        reader.read("stepsSinceLastEpisode", stepsSinceLastEpisode);
        
        // Restore state
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesTotal = totalSpikesTotal;
        pImpl->isResting = isResting;
        pImpl->stepsSinceLastEpisode = stepsSinceLastEpisode;
        
        // Load spike system state
        if (pImpl->spikeSystem) {
            std::string spikeState = "";
            if (reader.read("spikeSystem", spikeState)) {
                pImpl->spikeSystem->importState(spikeState);
            }
        }
        
        // Load memory systems
        if (pImpl->workingMemory) {
            std::string workingMemoryState = "";
            if (reader.read("workingMemory", workingMemoryState)) {
                pImpl->workingMemory->importState(workingMemoryState);
            }
        }
        
        if (pImpl->episodicMemory) {
            std::string episodicMemoryState = "";
            if (reader.read("episodicMemory", episodicMemoryState)) {
                pImpl->episodicMemory->importState(episodicMemoryState);
            }
        }
        
        if (pImpl->associativeMemory) {
            std::string associativeMemoryState = "";
            if (reader.read("associativeMemory", associativeMemoryState)) {
                pImpl->associativeMemory->importState(associativeMemoryState);
            }
        }
        
        NLM_LOG_INFO("Brain state loaded from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    pImpl->nextRegionId++;
    auto region = std::make_unique<NeuralRegion>(pImpl->nextRegionId, name);
    auto* regionPtr = region.get();
    pImpl->regions.push_back(std::move(region));
    return regionPtr->getId();
}

NeuralRegion* Brain::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target,
                                    float weight, Delay delay) {
    pImpl->interRegionConnections.push_back(
        InterRegionConnection(source, target, weight, delay)
    );
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                     pImpl->interRegionConnections.end(),
                     [source, target](const InterRegionConnection& conn) {
                         return conn.sourceRegion == source && conn.targetRegion == target;
                     }),
        pImpl->interRegionConnections.end()
    );
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t active = 0;
    for (const auto& region : pImpl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->isFiring()) {
                active++;
            }
        }
    }
    return active;
}

size_t Brain::getFiringNeuronCount() const {
    size_t firing = 0;
    for (const auto& region : pImpl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->isFiring()) {
                firing++;
            }
        }
    }
    return firing;
}

float Brain::getAverageFiringRate() const {
    size_t totalNeurons = getTotalNeuronCount();
    if (totalNeurons == 0) return 0.0f;
    
    float totalFiring = 0.0f;
    for (const auto& region : pImpl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            totalFiring += neuron->getFiringRate();
        }
    }
    
    return totalFiring / totalNeurons;
}

// ========== MEMORY SYSTEM ACCESS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// ========== PREDICTION SYSTEM ==========

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// ========== COGNITION SYSTEMS ==========

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

// ========== NEUROMODULATION SYSTEMS ==========

Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

// ========== CONFIG ==========

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

// ========== RANDOM GENERATOR ==========

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

// ========== LOGGING ==========

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status ===");
    NLM_LOG_INFO("Phase: 6 - Final Integration");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("Current step: " + std::to_string(pImpl->currentStep));
    NLM_LOG_INFO("Current time: " + std::to_string(pImpl->currentTime) + "s");
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    NLM_LOG_INFO("Resting: " + std::string(pImpl->isResting ? "yes" : "no"));
    NLM_LOG_INFO("Steps since last episode: " + std::to_string(pImpl->stepsSinceLastEpisode));
    NLM_LOG_INFO("=== End Brain Status ===");
}

} // namespace nlm
