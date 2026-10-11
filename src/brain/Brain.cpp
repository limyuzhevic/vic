#include "BrainAbstractions.hpp"
#include "factories.hpp"
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
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct DefaultBrainImplementation::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Component factories
    std::unique_ptr<BrainComponentFactory> componentFactory;
    std::unique_ptr<ComponentRegistry> componentRegistry;
    
    // ========== INTEGRATED SYSTEMS ==========
    std::unique_ptr<ISpikeSystem> spikeSystem;
    std::unique_ptr<IPlasticitySystem> plasticitySystem;
    std::unique_ptr<INeuromodulationSystem> neuromodulationSystem;
    std::unique_ptr<IMemorySystem> memorySystem;
    std::unique_ptr<IDevelopmentSystem> developmentSystem;
    std::unique_ptr<ISensorySystem> sensorySystem;
    std::unique_ptr<IMotorSystem> motorSystem;
    std::unique_ptr<IPredictionSystem> predictionSystem;
    std::unique_ptr<ICognitionSystem> cognitionSystem;
    std::unique_ptr<IEnvironment> environment;
    
    // ========== ORIGINAL SYSTEMS (for compatibility) ==========
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    std::unique_ptr<PredictionSystem> predictionSystemOriginal;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    std::unique_ptr<DevelopmentSystem> developmentSystemOriginal;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<SpikeSystem> spikeSystemOriginal;
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
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Configuration for neural components
    struct NeuronConfig {
        float restingPotential = -70.0f;
        float threshold = -55.0f;
        float leakConductance = 10.0f;
        uint32_t refractoryPeriod = 5;
        NeuronType type = NeuronType::Internal;
    };
    
    NeuronConfig neuronConfig;
    
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
        , replayInterval(100)
        , consolidationInterval(1000)
    {
        // Initialize core infrastructure
        rng = std::make_unique<RandomGenerator>(42);
        
        // Create component factory
        componentFactory = std::make_unique<DefaultBrainComponentFactory>(
            config, rng, nullptr, nullptr
        );
        
        // Create component registry
        componentRegistry = std::make_unique<ComponentRegistry>();
        
        // Initialize component registry with factory methods
        if (componentRegistry) {
            componentRegistry->registerFactory<BrainComponentFactory>(
                [this]() { return std::make_unique<DefaultBrainComponentFactory>(
                    config, rng, nullptr, nullptr
                ); }
            );
        }
        
        // Initialize neural components
        neuronConfig = loadNeuronConfig();
        
        // Initialize plasticity systems
        initializePlasticitySystems();
        
        // Initialize neuromodulation systems
        initializeNeuromodulationSystems();
        
        // Initialize memory systems
        initializeMemorySystems();
        
        // Initialize cognitive systems
        initializeCognitiveSystems();
        
        // Initialize development system
        developmentSystemOriginal = std::make_unique<DevelopmentSystem>();
        developmentSystem = std::make_unique<DefaultDevelopmentSystem>();
        
        // Initialize spike system
        spikeSystem = std::make_unique<SpikeSystem>();
        spikeSystemOriginal = std::make_unique<SpikeSystem>();
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
        
        NLM_LOG_INFO("DefaultBrainImplementation initialized");
    }
    
    NeuronConfig loadNeuronConfig() {
        NeuronConfig config;
        
        if (!config) {
            return config;
        }
        
        config.restingPotential = config->getOr<float>("neuron_resting_potential", -70.0f);
        config.threshold = config->getOr<float>("neuron_threshold", -55.0f);
        config.leakConductance = config->getOr<float>("neuron_leak_conductance", 10.0f);
        config.refractoryPeriod = config->getOr<uint32_t>("neuron_refractory_period", 5);
        
        return config;
    }
    
    void initializePlasticitySystems() {
        // Initialize plasticity components using factory
        if (componentFactory) {
            plasticitySystem = componentFactory->createSTDP();
            // Add other plasticity systems...
        }
        
        // Legacy compatibility
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Configure plasticity systems
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        float hebbianRate = config->getOr<float>("hebbian_learning_rate", 0.01f);
        hebbian->configure(hebbianRate);
    }
    
    void initializeNeuromodulationSystems() {
        // Initialize neuromodulation components using factory
        if (componentFactory) {
            neuromodulationSystem = componentFactory->createDopamine();
            // Add other neuromodulation systems...
        }
        
        // Legacy compatibility
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        // Configure neuromodulation systems
        float baselineDopamine = config->getOr<float>("dopamine_baseline", 0.1f);
        float curiositySensitivity = config->getOr<float>("curiosity_sensitivity", 0.5f);
        float noveltyThreshold = config->getOr<float>("novelty_threshold", 0.7f);
        
        dopamine->configure(baselineDopamine, 0.5f);
        curiosity->configure(curiositySensitivity, 0.3f);
        novelty->configure(noveltyThreshold, 0.01f);
    }
    
    void initializeMemorySystems() {
        // Initialize memory components using factory
        if (componentFactory) {
            memorySystem = componentFactory->createWorkingMemory();
            // Add other memory systems...
        }
        
        // Legacy compatibility
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        workingMemory->initialize(this);
        episodicMemory->initialize(this);
        associativeMemory->initialize(this);
        
        workingMemory->setCapacity(100);
        episodicMemory->setMaxEpisodes(1000);
    }
    
    void initializeCognitiveSystems() {
        // Initialize cognitive components using factory
        if (componentFactory) {
            cognitionSystem = componentFactory->createNeuralPlanner();
            // Add other cognitive systems...
        }
        
        // Legacy compatibility
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        planner->initialize(this);
        conceptFormation->initialize(this);
        attention->initialize(this);
        
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
    
    ~Impl() = default;
};

DefaultBrainImplementation::DefaultBrainImplementation(std::unique_ptr<Config> config)
    : config_(std::move(config)), currentStep_(0), currentTime_(0) {
    pImpl = std::make_unique<Impl>(config_.get());
}

DefaultBrainImplementation::~DefaultBrainImplementation() = default;

void DefaultBrainImplementation::initialize(const Config& config) {
    config_->updateFrom(config);
    currentStep_ = 0;
    currentTime_ = 0;
    
    // Re-initialize components with updated config
    if (pImpl) {
        pImpl->currentStep = 0;
        pImpl->currentTime = 0;
        pImpl->totalSpikesThisStep = 0;
        pImpl->totalSpikesTotal = 0;
    }
}

void DefaultBrainImplementation::step(SimulationStep step, Timestamp time) {
    currentStep_ = step;
    currentTime_ = time;
    
    if (!pImpl) {
        return;
    }
    
    pImpl->currentStep = step;
    pImpl->currentTime = time;
    pImpl->totalSpikesThisStep = 0;
    
    // Process all integrated systems
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->update(step, time);
    }
    
    if (pImpl->plasticitySystem) {
        pImpl->plasticitySystem->update(step, time);
    }
    
    if (pImpl->neuromodulationSystem) {
        pImpl->neuromodulationSystem->update(step);
    }
    
    if (pImpl->memorySystem) {
        pImpl->memorySystem->update(step);
    }
    
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(step);
    }
    
    if (pImpl->sensorySystem) {
        pImpl->sensorySystem->update(step);
    }
    
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->update(step);
    }
    
    if (pImpl->cognitionSystem) {
        pImpl->cognitionSystem->update(step);
    }
    
    pImpl->totalSpikesTotal += pImpl->totalSpikesThisStep;
}

void DefaultBrainImplementation::reset() {
    if (pImpl) {
        pImpl->currentStep = 0;
        pImpl->currentTime = 0;
        pImpl->totalSpikesThisStep = 0;
        pImpl->totalSpikesTotal = 0;
        pImpl->isResting = false;
        pImpl->stepsSinceLastEpisode = 0;
        
        // Reset all integrated systems
        if (pImpl->spikeSystem) pImpl->spikeSystem->reset();
        if (pImpl->plasticitySystem) pImpl->plasticitySystem->reset();
        if (pImpl->neuromodulationSystem) pImpl->neuromodulationSystem->reset();
        if (pImpl->memorySystem) pImpl->memorySystem->reset();
        if (pImpl->developmentSystem) pImpl->developmentSystem->reset();
        if (pImpl->sensorySystem) pImpl->sensorySystem->reset();
        if (pImpl->predictionSystem) pImpl->predictionSystem->reset();
        if (pImpl->cognitionSystem) pImpl->cognitionSystem->reset();
        
        // Clear regions
        pImpl->regions.clear();
        pImpl->interRegionConnections.clear();
    }
}

float DefaultBrainImplementation::getExcitationInhibitionRatio() const {
    if (!pImpl) return 0.0f;
    if (pImpl->spikeSystem) {
        return pImpl->spikeSystem->getExcitationInhibitionRatio();
    }
    return 0.0f;
}

size_t DefaultBrainImplementation::getTotalNeuronCount() const {
    if (!pImpl) return 0;
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t DefaultBrainImplementation::getTotalSynapseCount() const {
    if (!pImpl) return 0;
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}
size_t DefaultBrainImplementation::getActiveNeuronCount() const {
    if (!pImpl) return 0;
    if (pImpl->spikeSystem) {
        return pImpl->spikeSystem->getActiveNeuronCount();
    }
    return 0;
}

size_t DefaultBrainImplementation::getFiringNeuronCount() const {
    return pImpl ? pImpl->totalSpikesThisStep : 0;
}

float DefaultBrainImplementation::getAverageFiringRate() const {
    if (!pImpl) return 0.0f;
    if (pImpl->spikeSystem) {
        return pImpl->spikeSystem->getAverageFiringRate();
    }
    return 0.0f;
}

// Brain factory function
Brain::Brain(std::shared_ptr<Config> config)
    : pImpl(new Impl(config)) {
    NLM_LOG_INFO("Brain created with config");
}

Brain::~Brain() {
    delete pImpl;
}

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
    if (!pImpl) {
        NLM_LOG_ERROR("[ERROR] Brain::initialize: pImpl is null");
        return false;
    }
    
    NLM_LOG_INFO("Initializing NLM Brain with factory pattern...");
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    // Create regions using factory
    for (size_t i = 0; i < regionCount; ++i) {
        auto region = createRegion();
        if (region) {
            pImpl->regions.push_back(std::move(region));
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // Collect sensory and motor neurons for I/O
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (auto* neuron : pop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            } else if (pop->getNeuronType() == NeuronType::Motor) {
                for (auto* neuron : pop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
    }
    
    NLM_LOG_INFO("Brain initialization complete with factory pattern");
    NLM_LOG_INFO("Total regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    
    return true;
}

std::unique_ptr<NeuralRegion> Brain::Impl::createRegion() {
    if (!componentFactory) {
        return nullptr;
    }
    
    return componentFactory->createRegion("GeneratedRegion");
}

NeuralRegion* Brain::getRegion(RegionId id) {
    if (!pImpl) return nullptr;
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    if (!pImpl) return nullptr;
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return pImpl ? pImpl->regions.size() : 0;
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Factory Pattern) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
}

} // namespace nlm
