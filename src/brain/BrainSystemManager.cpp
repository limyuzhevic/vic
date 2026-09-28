#include "BrainSystemManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <chrono>
#include <algorithm>

namespace nlm {

struct BrainSystemManager::Impl {
    Brain* brain = nullptr;
    std::shared_ptr<Config> config;
    
    // System managers
    std::unique_ptr<NeuromodulationManager> neuromodulationManager;
    std::unique_ptr<MemorySystemManager> memorySystemManager;
    std::unique_ptr<DevelopmentManager> developmentManager;
    std::unique_ptr<CheckpointManager> checkpointManager;
    std::unique_ptr<ActionSystem> actionSystem;
    std::unique_ptr<PlasticitySystem> plasticitySystem;
    
    // Core brain instance
    std::unique_ptr<Brain> brainInstance;
    
    // Simulation state
    double currentTime = 0.0;
    size_t currentStep = 0;
    bool running = false;
    bool emergencyStopRequested = false;
    
    // Configuration
    std::string checkpointDir;
    size_t maxCheckpoints = 10;
    float neuromodulationStrength = 1.0f;
    float memoryDecayRate = 0.01f;
    float developmentSpeed = 1.0f;
    float plasticityModulation = 1.0f;
    
    // Performance monitoring
    double lastStepTime = 0.0;
    double maxStepTime = 0.0;
    uint64_t totalSteps = 0;
    uint64_t totalSpikes = 0;
    uint64_t totalActions = 0;
    
    // Statistics
    struct SystemStats {
        uint64_t neuronsCreated = 0;
        uint64_t synapsesCreated = 0;
        uint64_t memoryTraces = 0;
        uint64_t episodesStored = 0;
        double totalSimulationTime = 0.0;
        double totalBrainTime = 0.0;
        std::vector<double> stepTimes;
    } stats;
    
    // State
    bool systemsInitialized = false;
    bool simulationStarted = false;
};

BrainSystemManager::BrainSystemManager() : 
    pImpl(std::make_unique<Impl>()),
    currentTime_(0.0),
    currentStep_(0),
    isRunning_(false) {}

BrainSystemManager::~BrainSystemManager() = default;

bool BrainSystemManager::initialize(std::shared_ptr<Config> config) {
    try {
        pImpl->config = config;
        pImpl->config_ = config;
        
        // Get configuration parameters
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        double timestep = pImpl->config->getOr<double>("simulation_timestep", 0.001);
        
        // Initialize core brain instance
        pImpl->brainInstance = std::make_unique<Brain>(config);
        pImpl->brain = pImpl->brainInstance.get();
        
        // Initialize system managers
        pImpl->neuromodulationManager = std::make_unique<NeuromodulationManager>();
        pImpl->memorySystemManager = std::make_unique<MemorySystemManager>();
        pImpl->developmentManager = std::make_unique<DevelopmentManager>();
        pImpl->checkpointManager = std::make_unique<CheckpointManager>();
        pImpl->actionSystem = std::make_unique<ActionSystem>();
        pImpl->plasticitySystem = std::make_unique<PlasticitySystem>();
        
        // Initialize all managers with brain reference
        pImpl->neuromodulationManager->initialize(pImpl->brain);
        pImpl->memorySystemManager->initialize(pImpl->brain);
        pImpl->developmentManager->initialize(pImpl->brain);
        pImpl->checkpointManager->initialize(pImpl->brain);
        pImpl->actionSystem->initialize(pImpl->brain);
        pImpl->plasticitySystem->initialize(pImpl->brain);
        
        // Configure with default parameters
        pImpl->checkpointManager->configure(pImpl->checkpointDir, pImpl->maxCheckpoints, true);
        pImpl->developmentManager->configure(1000.0f, 0.01f, 0.2f, 0.0001f);
        
        // Create brain regions and neurons
        for (size_t i = 0; i < regionCount; ++i) {
            pImpl->brain->addRegion("Region_" + std::to_string(i + 1));
        }
        
        // Create neurons across regions
        size_t neuronsPerRegion = neuronCount / regionCount;
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = pImpl->brain->getRegion(static_cast<RegionId>(i + 1));
            if (region) {
                // Add populations
                auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
                auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
                auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            }
        }
        
        // Initialize brain core system
        bool brainInitSuccess = pImpl->brain->initialize();
        if (!brainInitSuccess) {
            NLM_LOG_ERROR("Brain initialization failed");
            return false;
        }
        
        pImpl->systemsInitialized = true;
        pImpl->stats.neuronsCreated = pImpl->brain->getTotalNeuronCount();
        pImpl->stats.synapsesCreated = pImpl->brain->getTotalSynapseCount();
        
        NLM_LOG_INFO("BrainSystemManager initialized successfully");
        NLM_LOG_INFO("Created " + std::to_string(pImpl->stats.neuronsCreated) + " neurons");
        NLM_LOG_INFO("Created " + std::to_string(pImpl->stats.synapsesCreated) + " synapses");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during BrainSystemManager initialization: " + std::string(e.what()));
        return false;
    }
}

void BrainSystemManager::run(double duration) {
    if (!pImpl->systemsInitialized) {
        NLM_LOG_ERROR("BrainSystemManager not initialized. Cannot run simulation.");
        return;
    }
    
    pImpl->running = true;
    pImpl->simulationStarted = true;
    
    NLM_LOG_INFO("BrainSystemManager simulation started: duration=" + std::to_string(duration) + "s");
    
    double endTime = pImpl->currentTime + duration;
    
    while (pImpl->running && pImpl->currentTime < endTime && !pImpl->emergencyStopRequested) {
        // Calculate timestep
        double timestep = pImpl->config->getOr<double>("simulation_timestep", 0.001);
        
        // Calculate target time for this step
        double targetTime = std::min(pImpl->currentTime + timestep, endTime);
        
        // Record step start time
        auto stepStart = std::chrono::high_resolution_clock::now();
        
        // Execute simulation step
        step(targetTime);
        
        // Record step completion
        auto stepEnd = std::chrono::high_resolution_clock::now();
        auto stepDuration = std::chrono::duration_cast<std::chrono::microseconds>(stepEnd - stepStart);
        
        double stepTimeMs = stepDuration.count() / 1000.0;
        pImpl->lastStepTime = stepTimeMs;
        pImpl->maxStepTime = std::max(pImpl->maxStepTime, stepTimeMs);
        
        pImpl->stats.stepTimes.push_back(stepTimeMs);
        if (pImpl->stats.stepTimes.size() > 1000) {
            pImpl->stats.stepTimes.erase(pImpl->stats.stepTimes.begin());
        }
        
        pImpl->totalSteps++;
        pImpl->stats.totalSimulationTime += stepTimeMs / 1000.0; // Convert to seconds
        
        // Update checkpoint if needed
        pImpl->checkpointManager->update(pImpl->currentStep, pImpl->currentTime);
        
        // Check for emergency stop request
        if (pImpl->emergencyStopRequested) {
            break;
        }
        
        // Log progress periodically
        if (pImpl->currentStep % 1000 == 0) {
            NLM_LOG_INFO("BrainSystemManager progress: step=" + std::to_string(pImpl->currentStep) +
                        ", time=" + std::to_string(pImpl->currentTime) + "s");
        }
    }
    
    pImpl->running = false;
    
    NLM_LOG_INFO("BrainSystemManager simulation completed: steps=" + std::to_string(pImpl->currentStep) +
                ", time=" + std::to_string(pImpl->currentTime) + "s");
}

void BrainSystemManager::step(double currentTime) {
    if (!pImpl->running || pImpl->emergencyStopRequested) {
        return;
    }
    
    // Update current time and step
    double timestep = currentTime - pImpl->currentTime;
    pImpl->currentTime = currentTime;
    pImpl->currentStep_ = static_cast<size_t>(pImpl->currentTime / 0.001); // Approximate from timestep
    
    // Execute core simulation step
    pImpl->brain->step(pImpl->currentStep_, pImpl->currentTime);
    
    // Update system managers
    pImpl->neuromodulationManager->update(static_cast<float>(timestep));
    pImpl->memorySystemManager->update(static_cast<float>(timestep));
    pImpl->developmentManager->update(static_cast<float>(timestep));
    pImpl->plasticitySystem->update(static_cast<float>(timestep));
    pImpl->actionSystem->update(static_cast<float>(timestep));
    
    // Update statistics
    pImpl->totalSpikes = pImpl->brain->getTotalSpikeCount();
    pImpl->totalActions = pImpl->actionSystem->getTotalActionsProduced();
}

void BrainSystemManager::receiveSensoryInput(const SensoryInput& input) {
    if (pImpl->brain) {
        pImpl->brain->receiveSensoryInput(input);
        
        // Also update memory system with input
        pImpl->memorySystemManager->processSensoryInput(input);
    }
}

std::unique_ptr<Action> BrainSystemManager::getAction() {
    if (pImpl->actionSystem) {
        return pImpl->actionSystem->produceAction();
    }
    return std::make_unique<Action>(ActionType::Wait);
}

bool BrainSystemManager::save(const std::string& filepath) const {
    if (pImpl->brain) {
        return pImpl->brain->save(filepath);
    }
    return false;
}

bool BrainSystemManager::load(const std::string& filepath) {
    if (pImpl->brain) {
        return pImpl->brain->load(filepath);
    }
    return false;
}

void BrainSystemManager::reset() {
    if (pImpl->brain) {
        pImpl->brain->reset();
    }
    
    if (pImpl->neuromodulationManager) {
        pImpl->neuromodulationManager->reset();
    }
    
    if (pImpl->memorySystemManager) {
        pImpl->memorySystemManager->reset();
    }
    
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->reset();
    }
    
    if (pImpl->actionSystem) {
        pImpl->actionSystem->reset();
    }
    
    if (pImpl->plasticitySystem) {
        pImpl->plasticitySystem->reset();
    }
    
    // Reset simulation state
    pImpl->currentTime = 0.0;
    pImpl->currentStep_ = 0;
    pImpl->running = false;
    pImpl->emergencyStopRequested = false;
    
    NLM_LOG_INFO("BrainSystemManager reset");
}

void BrainSystemManager::logStatus() const {
    NLM_LOG_INFO("=== BrainSystemManager Status ===");
    NLM_LOG_INFO("Simulation time: " + std::to_string(pImpl->currentTime) + "s");
    NLM_LOG_INFO("Simulation step: " + std::to_string(pImpl->currentStep_));
    NLM_LOG_INFO("Running: " + std::to_string(pImpl->running));
    NLM_LOG_INFO("Emergency stop requested: " + std::to_string(pImpl->emergencyStopRequested));
    
    if (pImpl->brain) {
        NLM_LOG_INFO("Brain status:");
        pImpl->brain->logStatus();
    }
    
    if (pImpl->neuromodulationManager) {
        NLM_LOG_INFO("Neuromodulation status:");
        NLM_LOG_INFO("  Dopamine: " + std::to_string(pImpl->neuromodulationManager->getDopamineLevel()));
        NLM_LOG_INFO("  Curiosity: " + std::to_string(pImpl->neuromodulationManager->getCuriosityLevel()));
        NLM_LOG_INFO("  Novelty: " + std::to_string(pImpl->neuromodulationManager->getNoveltyLevel()));
    }
    
    if (pImpl->memorySystemManager) {
        NLM_LOG_INFO("Memory system status:");
        NLM_LOG_INFO("  Working memory traces: " + std::to_string(pImpl->memorySystemManager->getActiveMemoryTraces()));
        NLM_LOG_INFO("  Episodic episodes: " + std::to_string(pImpl->memorySystemManager->getEpisodeCount()));
        NLM_LOG_INFO("  Active associations: " + std::to_string(pImpl->memorySystemManager->getActiveAssociations()));
    }
    
    if (pImpl->checkpointManager) {
        NLM_LOG_INFO("Checkpoint status:");
        NLM_LOG_INFO("  Directory: " + pImpl->checkpointManager->getCheckpointDir());
        NLM_LOG_INFO("  Last checkpoint: " + pImpl->checkpointManager->getLastCheckpointPath());
    }
    
    NLM_LOG_INFO("System performance:");
    NLM_LOG_INFO("  Total steps: " + std::to_string(pImpl->totalSteps));
    NLM_LOG_INFO("  Total spikes: " + std::to_string(pImpl->totalSpikes));
    NLM_LOG_INFO("  Total actions: " + std::to_string(pImpl->totalActions));
    NLM_LOG_INFO("  Average step time: " + std::to_string(pImpl->lastStepTime) + "ms");
    NLM_LOG_INFO("  Max step time: " + std::to_string(pImpl->maxStepTime) + "ms");
}

void BrainSystemManager::configure(const std::string& checkpointDir, size_t maxCheckpoints,
                                  float neuromodulationStrength, float memoryDecayRate,
                                  float developmentSpeed, float plasticityModulation) {
    pImpl->checkpointDir = checkpointDir;
    pImpl->maxCheckpoints = maxCheckpoints;
    pImpl->neuromodulationStrength = neuromodulationStrength;
    pImpl->memoryDecayRate = memoryDecayRate;
    pImpl->developmentSpeed = developmentSpeed;
    pImpl->plasticityModulation = plasticityModulation;
    
    // Apply configuration to systems
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->configure(checkpointDir, maxCheckpoints, true);
    }
    
    if (pImpl->memorySystemManager) {
        pImpl->memorySystemManager->setEfficiencyParameters(memoryDecayRate, 1000.0f);
    }
    
    if (pImpl->developmentManager) {
        pImpl->developmentManager->setDevelopmentSpeed(developmentSpeed);
    }
    
    if (pImpl->plasticitySystem) {
        pImpl->plasticitySystem->applyDevelopmentModifiers(plasticityModulation);
    }
    
    NLM_LOG_INFO("BrainSystemManager configured");
}

size_t BrainSystemManager::getTotalNeuronCount() const {
    return pImpl->brain ? pImpl->brain->getTotalNeuronCount() : 0;
}

size_t BrainSystemManager::getTotalSynapseCount() const {
    return pImpl->brain ? pImpl->brain->getTotalSynapseCount() : 0;
}

size_t BrainSystemManager::getActiveNeuronCount() const {
    return pImpl->brain ? pImpl->brain->getActiveNeuronCount() : 0;
}

size_t BrainSystemManager::getFiringNeuronCount() const {
    return pImpl->brain ? pImpl->brain->getFiringNeuronCount() : 0;
}

float BrainSystemManager::getAverageFiringRate() const {
    return pImpl->brain ? pImpl->brain->getAverageFiringRate() : 0.0f;
}

float BrainSystemManager::getExcitationInhibitionRatio() const {
    return pImpl->brain ? pImpl->brain->getExcitationInhibitionRatio() : 0.0f;
}

bool BrainSystemManager::isSystemHealthy() const {
    if (!pImpl->systemsInitialized) return false;
    
    // Check if brain is responsive
    bool brainHealthy = pImpl->brain && pImpl->brain->getTotalNeuronCount() > 0;
    
    // Check if critical systems are operational
    bool neuromodulationHealthy = pImpl->neuromodulationManager != nullptr;
    bool memoryHealthy = pImpl->memorySystemManager != nullptr;
    bool actionHealthy = pImpl->actionSystem != nullptr;
    
    // Check for excessive stall time
    bool notStalled = pImpl->actionSystem ? !pImpl->actionSystem->isActionStalled() : true;
    
    return brainHealthy && neuromodulationHealthy && memoryHealthy && actionHealthy && notStalled;
}

std::string BrainSystemManager::getSystemStatus() const {
    std::string status = "BrainSystemManager Status:\n";
    
    status += "  Simulation: " + std::string(pImpl->running ? "RUNNING" : "STOPPED") + "\n";
    status += "  Time: " + std::to_string(pImpl->currentTime) + "s\n";
    status += "  Step: " + std::to_string(pImpl->currentStep_) + "\n";
    status += "  System healthy: " + std::string(isSystemHealthy() ? "YES" : "NO") + "\n";
    
    if (pImpl->brain) {
        status += "  Neurons: " + std::to_string(pImpl->brain->getTotalNeuronCount()) + "\n";
        status += "  Synapses: " + std::to_string(pImpl->brain->getTotalSynapseCount()) + "\n";
        status += "  Active neurons: " + std::to_string(pImpl->brain->getActiveNeuronCount()) + "\n";
    }
    
    return status;
}

void BrainSystemManager::emergencyStop() {
    NLM_LOG_WARN("BrainSystemManager emergency stop requested");
    pImpl->emergencyStopRequested = true;
    pImpl->running = false;
}

} // namespace nlm
