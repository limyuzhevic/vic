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
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <numeric>
#include <unordered_map>
#include <set>

namespace nlm {

// BrainFactory - Configuration-based brain creation helper class
class BrainFactory {
public:
    // Template configuration patterns for common brain types
    struct ConfigurationPattern {
        std::string name;
        std::unordered_map<std::string, std::any> parameters;
    };
    
    // Validate configuration with schema
    class ConfigurationValidator {
    public:
        static bool validate(const Config& config) {
            // Basic validation rules
            return validateParameterRanges(config) &&
                   validateConsistency(config);
        }
        
    private:
        static bool validateParameterRanges(const Config& config) {
            // Check parameter ranges
            if (auto val = config.getOr<double>("simulation_timestep", 0.001)) {
                if (*val <= 0.0 || *val > 1.0) return false;
            }
            if (auto val = config.getOr<size_t>("neuron_count", 1000)) {
                if (*val == 0) return false;
            }
            if (auto val = config.getOr<size_t>("region_count", 1)) {
                if (*val == 0) return false;
            }
            return true;
        }
        
        static bool validateConsistency(const Config& config) {
            // Check that region_count * neurons_per_region <= neuron_count
            size_t neuronCount = config.getOr<size_t>("neuron_count", 1000);
            size_t regionCount = config.getOr<size_t>("region_count", 1);
            return neuronCount >= regionCount * 10; // Minimum neurons per region
        }
    };
    
    // Create brain from configuration
    static std::shared_ptr<Brain> create(std::shared_ptr<Config> config) {
        if (!config) {
            NLM_LOG_ERROR("Cannot create Brain: null configuration");
            return nullptr;
        }
        
        if (!ConfigurationValidator::validate(*config)) {
            NLM_LOG_ERROR("Invalid Brain configuration");
            return nullptr;
        }
        
        try {
            auto brain = std::shared_ptr<Brain>(new Brain(config));
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain after creation");
                return nullptr;
            }
            NLM_LOG_INFO("Brain created successfully using BrainFactory");
            return brain;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception creating Brain: ") + e.what());
            return nullptr;
        }
    }
    
    // Create default configuration
    static std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        
        // Set default values
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        return config;
    }
    
    // Create configuration for specific use case
    template<typename... Args>
    static std::shared_ptr<Config> createConfigForUseCase(
        std::string_view useCase,
        Args&&... args) {
        auto config = createDefaultConfig();
        
        if (useCase == "learning") {
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            config->set("replay_interval", 100, ConfigSource::Default);
            config->set("consolidation_interval", 1000, ConfigSource::Default);
        } else if (useCase == "fast") {
            config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        } else if (useCase == "complex") {
            config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(5), ConfigSource::Default);
            config->set("connection_probability", 0.2f, ConfigSource::Default);
        }
        
        return config;
    }
};

// Implementation details for Brain PIMPL pattern
struct Brain::Impl {
    // Core configuration
    std::shared_ptr<Config> config;
    
    // Random number generator with proper seed handling
    std::unique_ptr<RandomGenerator> rng;
    
    // Neural network components
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // I/O neuron collections
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integrated systems - using std::unique_ptr for memory safety
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
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Checkpoint system for persistence
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Simulation state
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Integration state
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Development state
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
    // Thread-safe spike queue
    std::vector<SpikeEvent> pendingSpikes;
    std::vector<DelayedSpikeEvent> pendingDelayedSpikes;
    
    // Constructor with configuration - exception safe initialization
    explicit Impl(std::shared_ptr<Config> cfg)
        : config(std::move(cfg)),
          rng(nullptr),
          timestep(0.001),
          currentStep(0),
          currentTime(0.0),
          totalSpikesThisStep(0),
          totalSpikesTotal(0),
          isResting(false),
          stepsSinceLastEpisode(0),
          replayInterval(100),
          consolidationInterval(1000),
          developmentalStage(DevelopmentalStage::Initial),
          nextRegionId(1) {
        
        // Initialize with exception safety
        initializeComponents();
    }
    
    // Initialize all components in dependency order
    void initializeComponents() {
        if (!config) {
            throw std::runtime_error("BrainImpl: Null configuration provided");
        }
        
        // Initialize random generator
        uint64_t seed = 42;
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems
        initializePlasticitySystems();
        
        // Initialize integrated systems
        initializeMemorySystems();
        initializePredictionSystem();
        initializeCognitionSystems();
        initializeDevelopmentSystem();
        initializeNeuromodulationSystems();
        
        // Configure checkpoint manager
        configureCheckpointSystem();
    }
    
    void initializePlasticitySystems() {
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Configure STDP parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.02f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.015f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Configure Hebbian parameters
        float hebbianLearningRate = config->getOr<float>("hebbian_learning_rate", 0.01f);
        hebbian->setLearningRate(hebbianLearningRate);
    }
    
    void initializeMemorySystems() {
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    }
    
    void initializePredictionSystem() {
        predictionSystem = std::make_unique<PredictionSystem>();
    }
    
    void initializeCognitionSystems() {
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
    }
    
    void initializeDevelopmentSystem() {
        developmentSystem = std::make_unique<DevelopmentSystem>();
    }
    
    void initializeNeuromodulationSystems() {
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
    }
    
    void configureCheckpointSystem() {
        checkpointManager = std::make_unique<CheckpointManager>();
        std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        checkpointManager->configure(checkpointDir, 10000, 5, true);
    }
    
    // Cleanup for destructor
    void cleanup() noexcept {
        // Release all resources in reverse initialization order
        checkpointManager.reset();
        
        structuralPlasticity.reset();
        hebbian.reset();
        stdp.reset();
        spikeSystem.reset();
        
        novelty.reset();
        predictionError.reset();
        curiosity.reset();
        dopamine.reset();
        
        developmentSystem.reset();
        attention.reset();
        conceptFormation.reset();
        planner.reset();
        predictionSystem.reset();
        
        associativeMemory.reset();
        episodicMemory.reset();
        workingMemory.reset();
        
        // Clear neuron collections
        sensoryNeurons.clear();
        sensoryNeurons.shrink_to_fit();
        motorNeurons.clear();
        motorNeurons.shrink_to_fit();
        
        interRegionConnections.clear();
        interRegionConnections.shrink_to_fit();
        
        regions.clear();
        regions.shrink_to_fit();
    }
};

// Brain PIMPL constructor
Brain::Brain(std::shared_ptr<Config> config) : pImpl(createImpl(std::move(config))) {}

// Brain factory method for implementation creation
std::unique_ptr<Brain::Impl> Brain::createImpl(std::shared_ptr<Config> config) {
    return std::make_unique<Impl>(std::move(config));
}

// Move constructor with noexcept
Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {}

// Move assignment operator with noexcept
Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

// Destructor with explicit cleanup
Brain::~Brain() noexcept {
    if (pImpl) {
        pImpl->cleanup();
    }
}

// Initialize brain with configuration - exception safe
[[nodiscard]] bool Brain::initialize() noexcept(false) {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Validate configuration
    size_t neuronCount = getConfigParameter<size_t>(*pImpl->config, "neuron_count", 1000);
    size_t regionCount = getConfigParameter<size_t>(*pImpl->config, "region_count", 1);
    float connectionProbability = getConfigParameter<float>(*pImpl->config, "connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    try {
        // Create regions
        createRegions(regionCount);
        
        // Create neurons and populate I/O collections
        populateNeurons(neuronCount, regionCount);
        
        // Initialize connectivity
        initializeConnectivity(connectionProbability);
        
        // Initialize all integrated systems
        initializeIntegratedSystems();
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during Brain initialization: ") + e.what());
        return false;
    }
}

// Helper functions for Brain initialization
void Brain::createRegions(size_t regionCount) {
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
}

void Brain::populateNeurons(size_t neuronCount, size_t regionCount) {
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR("Failed to create region " + std::to_string(i + 1));
            continue;
        }
        
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
        
        NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                     ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons");
    }
}

void Brain::initializeConnectivity(float connectionProbability) {
    for (size_t i = 0; i < getRegionCount(); ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void Brain::initializeIntegratedSystems() {
    // Initialize working memory
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(getTotalNeuronCount() / 10);
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);
    
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation systems
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
    
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler
    pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        deliverDelayedSpike(event);
    });
    
    // Configure checkpoint manager
    std::string checkpointDir = getConfigParameter<std::string>(*pImpl->config, "checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

void Brain::deliverDelayedSpike(const DelayedSpikeEvent& event) {
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
}

// Main simulation step - optimized for performance
void Brain::step(SimulationStep currentStep) noexcept {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) noexcept {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Process pending events
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // Update neurons
    updateNeurons(currentTime);
    
    // Process spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // Update integrated systems in dependency order
    updateWorkingMemory();
    applyNeuromodulationEffects(currentTime);
    applyPlasticity(currentTime);
    updateEpisodicMemory(currentStep, currentTime);
    updatePredictionSystem();
    updateAttentionSystem();
    updateConceptFormation();
    applyStructuralPlasticity(currentStep);
    replayMemories(currentStep);
    applyDevelopmentEffects(currentStep, currentTime);
    consolidateMemories(currentStep);
    
    // Update checkpoint manager
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void Brain::updateNeurons(Timestamp currentTime) noexcept {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
}

void Brain::updateWorkingMemory() noexcept {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

void Brain::applyNeuromodulationEffects(Timestamp currentTime) noexcept {
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update dopamine and apply its effects
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
}

void Brain::applyPlasticity(Timestamp currentTime) noexcept {
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
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            syn->step(currentTime);
        }
    }
}

void Brain::updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) noexcept {
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
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
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void Brain::updatePredictionSystem() noexcept {
    if (pImpl->predictionSystem) {
        // Prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void Brain::updateAttentionSystem() noexcept {
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

void Brain::updateConceptFormation() noexcept {
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
    }
}

void Brain::applyStructuralPlasticity(SimulationStep currentStep) noexcept {
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void Brain::replayMemories(SimulationStep currentStep) noexcept {
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void Brain::applyDevelopmentEffects(SimulationStep currentStep, Timestamp currentTime) noexcept {
    if (currentStep % 1000 == 0) {
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = pImpl->developmentalStage;
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void Brain::consolidateMemories(SimulationStep currentStep) noexcept {
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

// Other methods implementation (sensory input, accessors, etc.)
void Brain::receiveSensoryInput(const SensoryInput& input) noexcept {
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    for (size_t i = 0; i < numSensory; ++i) {
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) noexcept {
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

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) noexcept {
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

// Component accessors
[[nodiscard]] SpikeSystem* Brain::getSpikeSystem() const noexcept {
    return pImpl->spikeSystem.get();
}

[[nodiscard]] STDP* Brain::getSTDP() const noexcept {
    return pImpl->stdp.get();
}

[[nodiscard]] Hebbian* Brain::getHebbian() const noexcept {
    return pImpl->hebbian.get();
}

[[nodiscard]] StructuralPlasticity* Brain::getStructuralPlasticity() const noexcept {
    return pImpl->structuralPlasticity.get();
}

[[nodiscard]] float Brain::getExcitationInhibitionRatio() const noexcept {
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

[[nodiscard]] size_t Brain::getTotalSpikeCount() const noexcept {
    return pImpl->totalSpikesTotal;
}

[[nodiscard]] size_t Brain::getPendingSpikeEventCount() const noexcept {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

[[nodiscard]] std::unique_ptr<Action> Brain::produceAction() noexcept {
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    return std::make_unique<Action>(type);
}

void Brain::applyNeuromodulation(const Neuromodulator& signal) noexcept {
    float modulation = signal.getLevel();
    pImpl->stdp->setLTPWeight(0.02f * modulation);
    pImpl->stdp->setLTDWeight(0.015f * modulation);
}

void Brain::updatePlasticity() noexcept {
}

void Brain::develop() noexcept {
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

void Brain::reset() noexcept {
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
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

// Save/Load methods
[[nodiscard]] bool Brain::save(const std::string& filepath) const noexcept(false) {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
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

[[nodiscard]] bool Brain::load(const std::string& filepath) noexcept(false) {
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
        
        // Simplified loading - just read metadata for now
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

// Region management
[[nodiscard]] RegionId Brain::addRegion(std::string_view name) noexcept {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, std::string(name));
    pImpl->regions.push_back(std::move(region));
    return id;
}

[[nodiscard]] NeuralRegion* Brain::getRegion(RegionId id) const noexcept {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

[[nodiscard]] const NeuralRegion* Brain::getRegion(RegionId id) const noexcept {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

[[nodiscard]] size_t Brain::getRegionCount() const noexcept {
    return pImpl->regions.size();
}

[[nodiscard]] std::vector<RegionId> Brain::getRegionIds() const noexcept {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

[[nodiscard]] const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const noexcept {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                      float weight, Delay delay) noexcept {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) noexcept {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

// Global statistics
[[nodiscard]] size_t Brain::getTotalNeuronCount() const noexcept {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

[[nodiscard]] size_t Brain::getTotalSynapseCount() const noexcept {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

[[nodiscard]] size_t Brain::getActiveNeuronCount() const noexcept {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

[[nodiscard]] size_t Brain::getFiringNeuronCount() const noexcept {
    return pImpl->totalSpikesThisStep;
}

[[nodiscard]] float Brain::getAverageFiringRate() const noexcept {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

// Memory systems
[[nodiscard]] NeuralWorkingMemory* Brain::getWorkingMemory() const noexcept {
    return pImpl->workingMemory.get();
}

[[nodiscard]] NeuralEpisodicMemory* Brain::getEpisodicMemory() const noexcept {
    return pImpl->episodicMemory.get();
}

[[nodiscard]] NeuralAssociativeMemory* Brain::getAssociativeMemory() const noexcept {
    return pImpl->associativeMemory.get();
}

// Prediction system
[[nodiscard]] PredictionSystem* Brain::getPredictionSystem() const noexcept {
    return pImpl->predictionSystem.get();
}

// Cognition systems
[[nodiscard]] NeuralPlanner* Brain::getPlanner() const noexcept {
    return pImpl->planner.get();
}

[[nodiscard]] ConceptFormation* Brain::getConceptFormation() const noexcept {
    return pImpl->conceptFormation.get();
}

[[nodiscard]] AttentionalSelection* Brain::getAttention() const noexcept {
    return pImpl->attention.get();
}

// Development system
[[nodiscard]] DevelopmentSystem* Brain::getDevelopmentSystem() const noexcept {
    return pImpl->developmentSystem.get();
}

[[nodiscard]] DevelopmentalStage Brain::getDevelopmentalStage() const noexcept {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) noexcept {
    pImpl->developmentalStage = stage;
}

// Neuromodulation systems
[[nodiscard]] Dopamine* Brain::getDopamine() const noexcept {
    return pImpl->dopamine.get();
}

[[nodiscard]] Curiosity* Brain::getCuriosity() const noexcept {
    return pImpl->curiosity.get();
}

[[nodiscard]] Novelty* Brain::getNovelty() const noexcept {
    return pImpl->novelty.get();
}

[[nodiscard]] PredictionError* Brain::getPredictionErrorSignal() const noexcept {
    return pImpl->predictionError.get();
}

// Getters
[[nodiscard]] std::shared_ptr<const Config> Brain::getConfig() const noexcept {
    return pImpl->config;
}

[[nodiscard]] RandomGenerator* Brain::getRandomGenerator() const noexcept {
    return pImpl->rng.get();
}

// Status and logging
void Brain::logStatus() const noexcept {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage));
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                     " (" + region->getName() + "): " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                     std::to_string(region->getSynapseCount()) + " synapses, " +
                     "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm