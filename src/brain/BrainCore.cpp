#include "BrainCore.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../performance/CheckpointManager.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct BrainCore::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::unordered_map<RegionId, size_t> regionLookup;  // RegionId -> index in regions vector
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
    
    // Development tracking
    double developmentalAge;
    double plasticityModifier;
    bool rewardModulationEnabled;
    bool structuralPlasticityEnabled;
    bool developmentEnabled;
    bool curiosityEnabled;
    
    // Checkpointing
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Development parameters
    double synaptogenesisRate;
    double pruningRate;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , developmentalAge(0.0)
        , plasticityModifier(1.0)
        , rewardModulationEnabled(true)
        , structuralPlasticityEnabled(true)
        , developmentEnabled(true)
        , curiosityEnabled(true)
        , synaptogenesisRate(0.0001)
        , pruningRate(0.00001)
        , replayInterval(100)
        , consolidationInterval(1000)
    {
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
};

BrainCore::BrainCore(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

BrainCore::~BrainCore() = default;

BrainCore::BrainCore(BrainCore&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

BrainCore& BrainCore::operator=(BrainCore&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool BrainCore::initialize() {
    NLM_LOG_INFO("Initializing NLM BrainCore...");
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
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
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    pImpl->workingMemory->initialize(nullptr);  // Will be initialized later when Brain is created
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(nullptr);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(nullptr);
    
    // Initialize prediction system
    pImpl->predictionSystem->initialize(nullptr);
    
    // Initialize cognition systems
    pImpl->planner->initialize(nullptr);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(nullptr);
    
    pImpl->attention->initialize(nullptr);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    pImpl->novelty->initialize(nullptr);
    pImpl->curiosity->initialize(nullptr);
    
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
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
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    
    NLM_LOG_INFO("NLM BrainCore initialization complete");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

RegionId BrainCore::addRegion(const std::string& name) {
    /**
     * @brief Creates a new neural region with the specified name
     * 
     * Creates a new neural region and adds it to the brain core's region collection.
     * Each region contains neurons organized into populations (sensory, internal, motor).
     * If no name is provided, the region will be named "Region_N" where N is the index.
     * 
     * @param name Optional name for the region. If empty, auto-generates "Region_N"
     * @return RegionId Unique identifier for the created region
     */
    RegionId id(pImpl->regionLookup.size() + 1);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    pImpl->regionLookup[id] = pImpl->regions.size() - 1;  // Add to lookup table
    return id;
}

NeuralRegion* BrainCore::getRegion(RegionId id) {
    // Fast lookup using regionLookup map for O(1) performance
    auto it = pImpl->regionLookup.find(id);
    if (it != pImpl->regionLookup.end()) {
        return pImpl->regions[it->second].get();
    }
    return nullptr;
}

const NeuralRegion* BrainCore::getRegion(RegionId id) const {
    /**
     * @brief Retrieves a const neural region by its ID
     * 
     * Thread-safe version of getRegion that returns a const pointer to the neural region.
     * Used when accessing regions in read-only contexts or from const methods.
     * 
     * @param id The unique identifier of the region to retrieve
     * @return const NeuralRegion* Const pointer to the region if found, nullptr otherwise
     */
    auto it = pImpl->regionLookup.find(id);
    if (it != pImpl->regionLookup.end()) {
        return pImpl->regions[it->second].get();
    }
    return nullptr;
}

/**
 * @brief Gets the total number of regions in the brain core
 * 
 * @return size_t Number of regions currently in the brain core
 */
size_t BrainCore::getRegionCount() const {
    return pImpl->regions.size();
}

/**
 * @brief Retrieves all region IDs in the brain core
 * 
 * Returns a vector containing all RegionId values for regions that have been
 * added to the brain core. The order corresponds to the internal storage order.
 * 
 * @return std::vector<RegionId> Vector of all region IDs
 */
std::vector<RegionId> BrainCore::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

/**
 * @brief Returns a const reference to all regions in the brain core
 * 
 * Provides access to the complete collection of neural regions for iteration
 * or bulk operations. This is the most efficient way to access all regions
 * when you need to iterate over them.
 * 
 * @return const std::vector<std::unique_ptr<NeuralRegion>>& Const reference to regions vector
 */
const std::vector<std::unique_ptr<NeuralRegion>>& BrainCore::getRegions() const {
    return pImpl->regions;
}

/**
 * @brief Adds an inter-region connection between two brain regions
 * 
 * Creates a long-range synaptic connection from one region to another.
 * Inter-region connections enable communication between different brain regions
 * and are used to model distributed neural processing. Each connection includes
 * a weight (strength) and delay (transmission time in simulation steps).
 * 
 * @param source Source region ID
 * @param target Target region ID
 * @param weight Synaptic weight of the connection (positive for excitation,
 *              negative for inhibition). Default is 0.0
 * @param delay Transmission delay in simulation steps (Default: 1 step)
 */
void BrainCore::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

/**
 * @brief Removes an inter-region connection between two brain regions
 * 
 * Finds and removes the connection from source to target region. If multiple
 * connections exist between the same regions, only one matching connection
 * is removed.
 * 
 * @param source Source region ID
 * @param target Target region ID
 */
void BrainCore::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

/**
 * @brief Calculates the total number of neurons across all brain regions
 * 
 * Iterates through all regions and sums the total neuron count. This includes
 * all neuron types (Sensory, Internal, Motor) across all regions.
 * 
 * @return size_t Total neuron count in the brain core
 */
size_t BrainCore::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

/**
 * @brief Calculates the total number of synapses across all brain regions
 * 
 * Iterates through all regions and sums the total synapse count, including
 * both intra-region synapses (within regions) and inter-region synapses (between
 * regions). This is a key metric for brain connectivity.
 * 
 * @return size_t Total synapse count in the brain core
 */
size_t BrainCore::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

/**
 * @brief Calculates the number of currently active neurons
 * 
 * Iterates through all regions and sums the active neuron counts. Active neurons
 * are those that have received input and are ready to potentially spike in the
 * next timestep.
 * 
 * @return size_t Number of active neurons across all regions
 */
size_t BrainCore::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

/**
 * @brief Calculates the average firing rate across all regions
 * 
 * Computes the average firing rate across all regions by summing each region's
 * individual firing rates and dividing by the total number of regions.
 * Returns 0 if no regions exist.
 * 
 * @return float Average firing rate across all regions (Hz)
 */
float BrainCore::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

std::shared_ptr<const Config> BrainCore::getConfig() const {
    return pImpl->config;
}

RandomGenerator* BrainCore::getRandomGenerator() {
    return pImpl->rng.get();
}

const RandomGenerator* BrainCore::getRandomGenerator() const {
    return pImpl->rng.get();
}

SpikeSystem* BrainCore::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* BrainCore::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* BrainCore::getSTDP() {
    return pImpl->stdp.get();
}

const STDP* BrainCore::getSTDP() const {
    return pImpl->stdp.get();
}

Hebbian* BrainCore::getHebbian() {
    return pImpl->hebbian.get();
}

const Hebbian* BrainCore::getHebbian() const {
    return pImpl->hebbian.get();
}

StructuralPlasticity* BrainCore::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

const StructuralPlasticity* BrainCore::getStructuralPlasticity() const {
    return pImpl->structuralPlasticity.get();
}

NeuralWorkingMemory* BrainCore::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* BrainCore::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* BrainCore::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

PredictionSystem* BrainCore::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

const PredictionSystem* BrainCore::getPredictionSystem() const {
    return pImpl->predictionSystem.get();
}

NeuralPlanner* BrainCore::getPlanner() {
    return pImpl->planner.get();
}

const NeuralPlanner* BrainCore::getPlanner() const {
    return pImpl->planner.get();
}

ConceptFormation* BrainCore::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

const ConceptFormation* BrainCore::getConceptFormation() const {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* BrainCore::getAttention() {
    return pImpl->attention.get();
}

const AttentionalSelection* BrainCore::getAttention() const {
    return pImpl->attention.get();
}

DevelopmentSystem* BrainCore::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

const DevelopmentSystem* BrainCore::getDevelopmentSystem() const {
    return pImpl->developmentSystem.get();
}

Dopamine* BrainCore::getDopamine() {
    return pImpl->dopamine.get();
}

const Dopamine* BrainCore::getDopamine() const {
    return pImpl->dopamine.get();
}

Curiosity* BrainCore::getCuriosity() {
    return pImpl->curiosity.get();
}

const Curiosity* BrainCore::getCuriosity() const {
    return pImpl->curiosity.get();
}

Novelty* BrainCore::getNovelty() {
    return pImpl->novelty.get();
}

const Novelty* BrainCore::getNovelty() const {
    return pImpl->novelty.get();
}

PredictionError* BrainCore::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

const PredictionError* BrainCore::getPredictionErrorSignal() const {
    return pImpl->predictionError.get();
}

TimestepDuration BrainCore::getTimestep() const {
    return pImpl->timestep;
}

SimulationStep BrainCore::getCurrentStep() const {
    return pImpl->currentStep;
}

// Get the timestep used for simulation
    /**
     * @brief Retrieves the current simulation timestep
     * 
     * Returns the timestep (time duration per simulation step) used by the brain core.
     * This determines the temporal resolution of the neural simulation.
     * 
     * @return TimestepDuration The simulation timestep in seconds
     */
    TimestepDuration BrainCore::getTimestep() const {
        return pImpl->timestep;
    }

    /**
     * @brief Retrieves the current simulation step number
     * 
     * Returns the current simulation step number, which represents the discrete time
     * index in the simulation. Each increment represents one complete simulation cycle
     * with all neural computations, plasticity updates, and system state changes.
     * 
     * @return SimulationStep The current simulation step number
     */
    SimulationStep BrainCore::getCurrentStep() const {
        return pImpl->currentStep;
    }

    /**
     * @brief Retrieves the current simulation time
     * 
     * Returns the absolute current time in the simulation, based on the initial time
     * and the timestep multiplied by the current step number. This provides a
     * continuous time coordinate for the simulation.
     * 
     * @return Timestamp The current simulation time in seconds
     */
    Timestamp BrainCore::getCurrentTime() const {
        return pImpl->currentTime;
    }

    /**
     * @brief Checks if the brain core is in resting state
     * 
     * Returns true if the brain core is in resting mode (sleep/rest cycle).
     * Resting mode can be used to temporarily suspend neural processing while
     * preserving the current network state for later resumption.
     * 
     * @return bool true if in resting state, false otherwise
     */
    bool BrainCore::isResting() const {
        return pImpl->isResting;
    }

    /**
     * @brief Gets the number of steps since the last episode
     * 
     * Returns the number of simulation steps that have elapsed since the last
     * episode boundary or reset. This is useful for tracking episode duration
     * and timing episodic processes.
     * 
     * @return size_t Number of steps since last episode
     */
    size_t BrainCore::getStepsSinceLastEpisode() const {
        return pImpl->stepsSinceLastEpisode;
    }

    /**
     * @brief Gets the brain's developmental age
     * 
     * Returns the brain's developmental age in simulation time units. This tracks
     * how much the brain has "grown" and developed throughout the simulation,
     * affecting plasticity and learning rates.
     * 
     * @return double The developmental age in simulation time units
     */
    double BrainCore::getDevelopmentalAge() const {
        return pImpl->developmentalAge;
    }

void BrainCore::reset() {
    /**
     * @brief Resets the entire brain core to its initial state
     * 
     * Performs a complete reset of all brain state variables and neural components:
     * - Resets simulation counters (step, time, spike counts)
     * - Resets developmental tracking variables
     * - Clears all neural regions and their neurons/synapses
     * - Resets the spike system
     * - Clears all memory systems (working, episodic, associative memory)
     * - Resets attentional selection system
     * 
     * This method is typically called at the beginning of a new episode or when
     * restarting the simulation. After reset, the brain core can be re-initialized
     * and used for a fresh simulation run.
     */
    NLM_LOG_INFO("Resetting BrainCore...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    pImpl->developmentalAge = 0.0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("BrainCore reset complete");
}

const std::vector<Neuron*>& BrainCore::getSensoryNeurons() const {
    return pImpl->sensoryNeurons;
}

const std::vector<Neuron*>& BrainCore::getMotorNeurons() const {
    return pImpl->motorNeurons;
}

size_t BrainCore::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t BrainCore::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

float BrainCore::getExcitationInhibitionRatio() const {
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

} // namespace nlm
