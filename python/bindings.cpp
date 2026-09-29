// Enhanced NLM Python Bindings
// Provides a comprehensive Python API for the NLM neural simulation framework
// with improved usability, method chaining, and advanced features for both beginners
// and advanced users.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <cmath>
#include <algorithm>

// Include NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced exceptions for Python
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError("Configuration Error: " + message) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& message) : NLMError("Simulation Error: " + message) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message) : NLMError("Memory Error: " + message) {}
};

// Configuration builder for creating predefined configurations
class ConfigBuilder {
private:
    std::shared_ptr<Config> config;
    
public:
    ConfigBuilder() : config(std::make_shared<Config>()) {}
    
    // Set simulation parameters
    ConfigBuilder& withNeuronCount(size_t count) {
        config->set("neuron_count", static_cast<int64_t>(count));
        return *this;
    }
    
    ConfigBuilder& withRegionCount(size_t count) {
        config->set("region_count", static_cast<int64_t>(count));
        return *this;
    }
    
    ConfigBuilder& withConnectionProbability(float prob) {
        config->set("connection_probability", prob);
        return *this;
    }
    
    ConfigBuilder& withTimestep(double timestep) {
        config->set("simulation_timestep", timestep);
        return *this;
    }
    
    ConfigBuilder& withRandomSeed(uint64_t seed) {
        config->set("random_seed", static_cast<int64_t>(seed));
        return *this;
    }
    
    // Set plasticity parameters
    ConfigBuilder& withSTDP(float ltpWeight = 0.02f, float ltdWeight = 0.015f, float tau = 20.0f) {
        config->set("stdp_ltp_weight", ltpWeight);
        config->set("stdp_ltd_weight", ltdWeight);
        config->set("stdp_tau", tau);
        return *this;
    }
    
    ConfigBuilder& withHebbianLearning(float learningRate = 0.01f) {
        config->set("hebbian_learning_rate", learningRate);
        return *this;
    }
    
    // Set structural plasticity
    ConfigBuilder& withSynaptogenesis(float rate = 0.0001f) {
        config->set("synaptogenesis_rate", rate);
        return *this;
    }
    
    ConfigBuilder& withPruning(float rate = 0.00001f) {
        config->set("pruning_rate", rate);
        return *this;
    }
    
    // Set development parameters
    ConfigBuilder& withDevelopment(bool enabled = true, float maturationTime = 600.0) {
        config->set("development_enabled", enabled);
        config->set("development_maturation_time", maturationTime);
        return *this;
    }
    
    // Set neuromodulation
    ConfigBuilder& withNeuromodulation(bool dopamine = true, bool curiosity = true, bool novelty = true) {
        config->set("dopamine_enabled", dopamine);
        config->set("curiosity_enabled", curiosity);
        config->set("novelty_enabled", novelty);
        return *this;
    }
    
    // Set memory system parameters
    ConfigBuilder& withMemoryCapacity(size_t workingCapacity = 500, size_t episodicMax = 1000) {
        config->set("working_memory_capacity", static_cast<int64_t>(workingCapacity));
        config->set("episodic_memory_max_episodes", static_cast<int64_t>(episodicMax));
        return *this;
    }
    
    // Set prediction system
    ConfigBuilder& withPrediction(bool enabled = true, float predictionGain = 0.5f) {
        config->set("prediction_enabled", enabled);
        config->set("prediction_gain", predictionGain);
        return *this;
    }
    
    // Set cognition
    ConfigBuilder& withCognition(bool planner = true, bool conceptFormation = true) {
        config->set("planner_enabled", planner);
        config->set("concept_formation_enabled", conceptFormation);
        return *this;
    }
    
    // Apply preset configurations
    ConfigBuilder& presetResearch() {
        return this->withNeuronCount(2000)
                    .withRegionCount(4)
                    .withConnectionProbability(0.1f)
                    .withSTDP(0.015f, 0.018f, 25.0f)
                    .withHebbianLearning(0.005f)
                    .withSynaptogenesis(0.00005f)
                    .withPruning(0.000005f)
                    .withDevelopment(true, 900.0)
                    .withNeuromodulation(true, true, true)
                    .withMemoryCapacity(1000, 2000)
                    .withPrediction(true, 0.7f)
                    .withCognition(true, true);
    }
    
    ConfigBuilder& presetMinimal() {
        return this->withNeuronCount(100)
                    .withRegionCount(1)
                    .withConnectionProbability(0.05f)
                    .withSTDP(0.01f, 0.012f, 20.0f)
                    .withNeuromodulation(false, false, false)
                    .withDevelopment(false);
    }
    
    ConfigBuilder& presetAgent() {
        return this->withNeuronCount(1500)
                    .withRegionCount(3)
                    .withConnectionProbability(0.12f)
                    .withSTDP(0.02f, 0.025f, 30.0f)
                    .withHebbianLearning(0.02f)
                    .withSynaptogenesis(0.0002f)
                    .withDevelopment(true, 300.0)
                    .withNeuromodulation(true, true, true)
                    .withMemoryCapacity(500, 1000)
                    .withPrediction(true, 0.8f)
                    .withCognition(true, true);
    }
    
    // Apply experimental settings
    ConfigBuilder& experimental(bool enableAll = true) {
        config->set("experimental_mode", enableAll);
        config->set("verbose_logging", true);
        config->set("checkpoint_interval", 100);
        config->set("enable_replay", true);
        return *this;
    }
    
    // Build the configuration
    std::shared_ptr<Config> build() {
        return config;
    }
    
    // Get configuration for fluent interface
    std::shared_ptr<Config> getConfig() {
        return config;
    }
};

// Configuration presets for common use cases
namespace ConfigPresets {
    std::shared_ptr<Config> research() {
        return ConfigBuilder().presetResearch().build();
    }
    
    std::shared_ptr<Config> minimal() {
        return ConfigBuilder().presetMinimal().build();
    }
    
    std::shared_ptr<Config> agent() {
        return ConfigBuilder().presetAgent().build();
    }
    
    std::shared_ptr<Config> experimental() {
        return ConfigBuilder().presetResearch().experimental().build();
    }
}

// Enhanced Brain class with Pythonic API
class EnhancedBrain {
private:
    std::shared_ptr<Brain> brain;
    bool initialized;
    
public:
    EnhancedBrain(std::shared_ptr<Config> config) : brain(std::make_shared<Brain>(config)), initialized(false) {}
    
    // Initialize brain with optional callback for progress
    EnhancedBrain& initialize(std::function<void(float)> progressCallback = nullptr) {
        if (!brain) {
            throw NLMError("Brain not properly initialized");
        }
        
        // Simulate initialization progress if callback provided
        if (progressCallback) {
            progressCallback(0.0f);
            if (!brain->initialize()) {
                throw NLMError("Brain initialization failed");
            }
            progressCallback(1.0f);
        } else {
            if (!brain->initialize()) {
                throw NLMError("Brain initialization failed");
            }
        }
        
        initialized = true;
        return *this;
    }
    
    // Run simulation with monitoring and statistics collection
    struct SimulationResult {
        size_t stepsCompleted;
        size_t totalSpikes;
        float averageFiringRate;
        float eIratio;
        double elapsedTime;
        bool completed;
        
        SimulationResult() : stepsCompleted(0), totalSpikes(0), averageFiringRate(0.0f), 
                           eIratio(0.0f), elapsedTime(0.0), completed(false) {}
    };
    
    SimulationResult runSteps(size_t steps, std::function<void(float)> progressCallback = nullptr) {
        if (!initialized) {
            throw SimulationError("Brain not initialized. Call initialize() first.");
        }
        
        SimulationResult result;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
            result.stepsCompleted++;
            
            if (progressCallback && steps > 0) {
                float progress = static_cast<float>(i) / static_cast<float>(steps);
                progressCallback(progress);
            }
            
            // Collect statistics every 100 steps
            if ((i + 1) % 100 == 0) {
                result.totalSpikes = brain->getTotalSpikeCount();
                result.averageFiringRate = brain->getAverageFiringRate();
                result.eIratio = brain->getExcitationInhibitionRatio();
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        result.elapsedTime = duration.count();
        result.completed = true;
        
        // Final statistics
        result.totalSpikes = brain->getTotalSpikeCount();
        result.averageFiringRate = brain->getAverageFiringRate();
        result.eIratio = brain->getExcitationInhibitionRatio();
        
        return result;
    }
    
    // Run simulation with sensory input
    SimulationResult runWithSensory(
        std::vector<std::vector<float>> sensoryInputs,
        std::function<void(float)> progressCallback = nullptr,
        double timestep = 0.001
    ) {
        if (!initialized) {
            throw SimulationError("Brain not initialized.");
        }
        
        if (sensoryInputs.size() > brain->getTotalNeuronCount() / 4) {
            throw SimulationError("Too many sensory inputs for available neurons");
        }
        
        SimulationResult result;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < sensoryInputs.size(); ++i) {
            // Create vision input from sensory data
            Vision vision(8, 8, 3);
            
            // Inject current into sensory neurons
            for (size_t j = 0; j < std::min(sensoryInputs[i].size(), size_t(20)); ++j) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, sensoryInputs[i][j] * 10.0f);
            }
            
            brain->step(i, i * timestep);
            result.stepsCompleted++;
            
            if (progressCallback) {
                float progress = static_cast<float>(i) / static_cast<float>(sensoryInputs.size());
                progressCallback(progress);
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        result.elapsedTime = duration.count();
        result.completed = true;
        
        return result;
    }
    
    // Get brain reference
    std::shared_ptr<Brain> getBrain() {
        return brain;
    }
    
    // Check if brain is initialized
    bool isInitialized() const {
        return initialized;
    }
    
    // Reset brain
    EnhancedBrain& reset() {
        if (brain) {
            brain->reset();
            initialized = false;
        }
        return *this;
    }
    
    // Python context manager support
    void __enter__() {
        if (!initialized) {
            initialize();
        }
    }
    
    void __exit__(py::object, py::object, py::object) {
        reset();
    }
};

// Enhanced AgentBrain with Pythonic API
class EnhancedAgentBrain {
private:
    std::shared_ptr<AgentBrain> agentBrain;
    std::shared_ptr<SimpleWorld> world;
    
public:
    EnhancedAgentBrain(std::shared_ptr<Brain> brain) : agentBrain(std::make_shared<AgentBrain>(brain)) {}
    
    // Initialize with world
    EnhancedAgentBrain& initialize(std::shared_ptr<SimpleWorld> w) {
        world = w;
        agentBrain->initialize(*world);
        return *this;
    }
    
    // Process sensory input with optional preprocessing
    EnhancedAgentBrain& processInput(
        std::vector<float> visionData,
        std::vector<float> touchData = std::vector<float>(8),
        std::vector<float> internalData = std::vector<float>(4),
        std::vector<float> proprioceptionData = std::vector<float>(6)
    ) {
        if (!world) {
            throw SimulationError("Agent not initialized with world");
        }
        
        // Create sensory percept
        SensoryPercept percept;
        
        // Process vision (assuming 8x8 with 3 channels = 192 values)
        if (visionData.size() >= 192) {
            Vision vision(8, 8, 3);
            // Copy data (simplified)
            percept.setVision(vision);
        }
        
        // Process other sensory modalities
        // ... (simplified for brevity)
        
        agentBrain->processSensoryInput(percept);
        return *this;
    }
    
    // Get action with optional exploration
    Action decodeAction(bool enableCuriosity = true, float curiosityThreshold = 0.3f) {
        if (!agentBrain) {
            throw SimulationError("Agent brain not properly initialized");
        }
        
        MotorCommand cmd = agentBrain->decodeMotorCommand();
        
        // Convert to Action
        Action action;
        switch (cmd) {
            case MotorCommand::MoveForward: action = Action(ActionType::MoveForward); break;
            case MotorCommand::MoveBackward: action = Action(ActionType::MoveBackward); break;
            case MotorCommand::TurnLeft: action = Action(ActionType::TurnLeft); break;
            case MotorCommand::TurnRight: action = Action(ActionType::TurnRight); break;
            case MotorCommand::Interact: action = Action(ActionType::Interact); break;
            default: action = Action(ActionType::Wait);
        }
        
        return action;
    }
    
    // Apply reward with prediction error
    EnhancedAgentBrain& applyReward(float reward, float predictedReward = 0.0f) {
        agentBrain->applyRewardModulation(reward, predictedReward);
        return *this;
    }
    
    // Update development
    EnhancedAgentBrain& updateDevelopment(double timestep = 0.1) {
        agentBrain->updateDevelopment(timestep);
        return *this;
    }
    
    // Enable features
    EnhancedAgentBrain& enableFeature(std::string feature, bool enable = true) {
        if (feature == "reward_modulation") {
            agentBrain->enableRewardModulation(enable);
        } else if (feature == "structural_plasticity") {
            agentBrain->enableStructuralPlasticity(enable);
        } else if (feature == "development") {
            agentBrain->enableDevelopment(enable);
        } else if (feature == "curiosity") {
            agentBrain->enableCuriosity(enable);
        }
        return *this;
    }
    
    // Get brain reference
    std::shared_ptr<Brain> getBrain() {
        return agentBrain->getBrain();
    }
    
    // Get statistics
    float getCuriosityLevel() const { return agentBrain->getCuriosityLevel(); }
    float getNoveltyLevel() const { return agentBrain->getNoveltyLevel(); }
    float getPredictionError() const { return agentBrain->getPredictionError(); }
    DevelopmentalStage getDevelopmentalStage() const { return agentBrain->getDevelopmentalStage(); }
};

// Simulation manager for running complete scenarios
class SimulationManager {
private:
    std::shared_ptr<Config> config;
    std::shared_ptr<EnhancedBrain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<EnhancedAgentBrain> agent;
    
public:
    SimulationManager() {
        // Create components
        config = std::make_shared<Config>();
        world = std::make_shared<SimpleWorld>();
        brain = std::make_shared<EnhancedBrain>(std::make_shared<Brain>(config));
        agent = std::make_shared<EnhancedAgentBrain>(brain->getBrain());
    }
    
    SimulationManager& createConfig(const std::string& preset = "minimal") {
        if (preset == "research") {
            config = ConfigPresets::research();
        } else if (preset == "agent") {
            config = ConfigPresets::agent();
        } else if (preset == "minimal") {
            config = ConfigPresets::minimal();
        }
        
        // Update brain's config
        brain->getBrain()->setConfig(config);
        return *this;
    }
    
    SimulationManager& createWorld(float width = 20.0f, float height = 20.0f, 
                                   size_t visionWidth = 8, size_t visionHeight = 8) {
        world->configure(width, height, visionWidth, visionHeight);
        world->reset();
        world->setAgentStart(width / 2, height / 2);
        
        // Initialize agent with world
        agent->initialize(world);
        return *this;
    }
    
    SimulationManager& initializeBrain() {
        brain->initialize();
        return *this;
    }
    
    // Run complete agent simulation
    struct AgentSimulationResult {
        size_t steps;
        float totalReward;
        float finalCuriosity;
        float finalNovelty;
        float finalPredictionError;
        float averageFiringRate;
        double elapsedTime;
        std::vector<std::vector<float>> trajectory;  // x, y positions over time
        
        AgentSimulationResult() : steps(0), totalReward(0.0f), finalCuriosity(0.0f),
                                 finalNovelty(0.0f), finalPredictionError(0.0f),
                                 averageFiringRate(0.0f), elapsedTime(0.0) {}
    };
    
    AgentSimulationResult runAgentSimulation(size_t steps, bool enableLearning = true) {
        if (!brain->isInitialized() || !world) {
            throw SimulationError("Simulation not properly initialized");
        }
        
        AgentSimulationResult result;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Enable learning features
        if (enableLearning) {
            agent->enableFeature("reward_modulation", true)
                     .enableFeature("structural_plasticity", true)
                     .enableFeature("development", true)
                     .enableFeature("curiosity", true);
        }
        
        for (size_t i = 0; i < steps; ++i) {
            // Update world
            world->update(0.1);
            
            // Get sensory percept
            const SensoryPercept& percept = world->getSensoryPercept();
            
            // Process input (simplified)
            agent->processInput(std::vector<float>(192)); // Full vision data
            
            // Brain step
            brain->getBrain()->step(i);
            
            // Get action
            Action action = agent->decodeAction();
            
            // Apply action to world
            world->applyMotorCommand(action.getType(), world->getSimulationTime());
            
            // Apply reward (simplified - use agent body energy as reward)
            if (enableLearning) {
                float reward = world->getAgentBody().energy / world->getMaxEnergy();
                agent->applyReward(reward);
                result.totalReward += reward;
            }
            
            // Track trajectory
            if (i % 10 == 0) { // Sample every 10 steps
                result.trajectory.push_back({world->getAgentBody().x, world->getAgentBody().y});
            }
            
            // Update development
            agent->updateDevelopment(0.1);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        result.elapsedTime = duration.count();
        result.steps = steps;
        result.finalCuriosity = agent->getCuriosityLevel();
        result.finalNovelty = agent->getNoveltyLevel();
        result.finalPredictionError = agent->getPredictionError();
        result.averageFiringRate = brain->getBrain()->getAverageFiringRate();
        
        return result;
    }
    
    // Get references to components
    std::shared_ptr<EnhancedBrain> getBrain() { return brain; }
    std::shared_ptr<SimpleWorld> getWorld() { return world; }
    std::shared_ptr<EnhancedAgentBrain> getAgent() { return agent; }
    std::shared_ptr<Config> getConfig() { return config; }
};

// Performance monitoring and profiling
class PerformanceMonitor {
private:
    struct Metrics {
        double cpuTime;
        double wallTime;
        size_t memoryUsage;
        size_t spikeCount;
        float firingRate;
        std::chrono::high_resolution_clock::time_point startTime;
        
        Metrics() : cpuTime(0.0), wallTime(0.0), memoryUsage(0), spikeCount(0), 
                   firingRate(0.0f), startTime(std::chrono::high_resolution_clock::now()) {}
    };
    
    Metrics metrics;
    std::vector<double> cpuHistory;
    std::vector<double> wallTimeHistory;
    
public:
    PerformanceMonitor() {
        cpuHistory.reserve(1000);
        wallTimeHistory.reserve(1000);
    }
    
    void startMeasurement() {
        metrics.startTime = std::chrono::high_resolution_clock::now();
    }
    
    void stopMeasurement() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - metrics.startTime);
        metrics.wallTime = duration.count();
    }
    
    void recordCPUTime(double time) {
        metrics.cpuTime = time;
        cpuHistory.push_back(time);
    }
    
    void recordWallTime(double time) {
        metrics.wallTime = time;
        wallTimeHistory.push_back(time);
    }
    
    void recordMemoryUsage(size_t bytes) {
        metrics.memoryUsage = bytes;
    }
    
    void recordSpikeCount(size_t count) {
        metrics.spikeCount = count;
    }
    
    void recordFiringRate(float rate) {
        metrics.firingRate = rate;
    }
    
    struct PerformanceReport {
        double avgCPUTime;
        double avgWallTime;
        double cpuEfficiency;  // wallTime / cpuTime
        size_t maxMemoryUsage;
        size_t totalSpikes;
        float avgFiringRate;
        size_t measurementCount;
        
        PerformanceReport() : avgCPUTime(0.0), avgWallTime(0.0), cpuEfficiency(0.0),
                             maxMemoryUsage(0), totalSpikes(0), avgFiringRate(0.0f),
                             measurementCount(0) {}
    };
    
    PerformanceReport generateReport() {
        PerformanceReport report;
        
        if (!cpuHistory.empty()) {
            double sumCPU = std::accumulate(cpuHistory.begin(), cpuHistory.end(), 0.0);
            double sumWall = std::accumulate(wallTimeHistory.begin(), wallTimeHistory.end(), 0.0);
            
            report.avgCPUTime = sumCPU / cpuHistory.size();
            report.avgWallTime = sumWall / wallTimeHistory.size();
            
            if (report.avgCPUTime > 0.0) {
                report.cpuEfficiency = report.avgWallTime / report.avgCPUTime;
            }
        }
        
        report.maxMemoryUsage = metrics.memoryUsage;
        report.totalSpikes = metrics.spikeCount;
        report.avgFiringRate = metrics.firingRate;
        report.measurementCount = static_cast<size_t>(cpuHistory.size());
        
        return report;
    }
    
    void reset() {
        cpuHistory.clear();
        wallTimeHistory.clear();
        metrics = Metrics();
    }
};

// Advanced debugging tools
class DebugTools {
public:
    // Visualize neuron connectivity
    static std::string visualizeConnectivity(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "=== NLM Connectivity Visualization ===" << std::endl;
        
        if (!brain) {
            ss << "Error: Brain is null" << std::endl;
            return ss.str();
        }
        
        ss << "Regions: " << brain->getRegionCount() << std::endl;
        ss << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        ss << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        ss << std::endl;
        
        // Show region details
        for (const auto& regionId : brain->getRegionIds()) {
            if (auto* region = brain->getRegion(regionId)) {
                ss << "Region " << regionId.index() << " (" << region->getName() << "):" << std::endl;
                ss << "  Populations: " << region->getPopulationCount() << std::endl;
                ss << "  Neurons: " << region->getTotalNeuronCount() << std::endl;
                ss << "  Synapses: " << region->getSynapseCount() << std::endl;
                ss << "  Avg Weight: " << region->getAverageSynapticWeight() << std::endl;
                ss << std::endl;
            }
        }
        
        return ss.str();
    }
    
    // Export brain state for analysis
    static bool exportBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw NLMError("Cannot export null brain");
        }
        
        try {
            return brain->save(filepath);
        } catch (const std::exception& e) {
            throw NLMError(std::string("Failed to export brain state: ") + e.what());
        }
    }
    
    // Import brain state
    static bool importBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw NLMError("Cannot import to null brain");
        }
        
        try {
            return brain->load(filepath);
        } catch (const std::exception& e) {
            throw NLMError(std::string("Failed to import brain state: ") + e.what());
        }
    }
    
    // Generate debug statistics
    static std::string generateDebugStats(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "=== NLM Debug Statistics ===" << std::endl;
        
        if (!brain) {
            ss << "Error: Brain is null" << std::endl;
            return ss.str();
        }
        
        ss << "Simulation Step: " << brain->getTotalSpikeCount() << std::endl;
        ss << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        ss << "Pending Spike Events: " << brain->getPendingSpikeEventCount() << std::endl;
        ss << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        ss << "Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        ss << std::endl;
        
        // Developmental stage
        ss << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        ss << "Region Count: " << brain->getRegionCount() << std::endl;
        
        // Memory system stats
        if (auto* workingMem = brain->getWorkingMemory()) {
            ss << "Working Memory Traces: " << workingMem->getActiveTraces() << std::endl;
        }
        
        if (auto* episodicMem = brain->getEpisodicMemory()) {
            ss << "Episodic Episodes: " << episodicMem->getEpisodeCount() << std::endl;
        }
        
        ss << std::endl;
        ss << "=== End Debug Stats ===" << std::endl;
        
        return ss.str();
    }
};

// Main bindings module
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        ==============================
        
        This module provides an enhanced Python API for the NLM (Neural Learning Machine)
        neural simulation framework. It offers:
        
        Features:
        - Pythonic API with method chaining and builder patterns
        - Configuration presets for common use cases
        - Advanced simulation management
        - Performance monitoring and profiling tools
        - Debug and visualization utilities
        - Context managers for automatic resource cleanup
        - Enhanced error handling with custom exceptions
        
        Key Classes:
        - Config: Configuration management with presets
        - ConfigBuilder: Fluent interface for configuration
        - EnhancedBrain: Advanced brain simulation API
        - EnhancedAgentBrain: Agent brain interface
        - SimulationManager: Complete simulation orchestration
        - PerformanceMonitor: Performance tracking and profiling
        - DebugTools: Debugging and analysis utilities
        
        Usage Examples:
        
        # Create a simple brain
        brain = pynlm.createBrain(pynlm.ConfigPresets.minimal())
        brain.initialize()
        
        # Run simulation with monitoring
        result = brain.runSteps(1000, progress=lambda p: print(f"Progress: {p*100:.1f}%") )
        
        # Create complete agent simulation
        sim = pynlm.SimulationManager()
        sim.createConfig("agent").createWorld(30, 30)
        sim.initializeBrain()
        agent_result = sim.runAgentSimulation(2000)
        
        # Advanced usage with performance monitoring
        monitor = pynlm.PerformanceMonitor()
        brain = pynlm.createBrain(pynlm.ConfigPresets.research())
        brain.initialize()
        
        monitor.startMeasurement()
        result = brain.runSteps(500)
        monitor.stopMeasurement()
        
        report = monitor.generateReport()
        print(f"Average CPU time: {report.avgCPUTime:.3f}s")
        print(f"CPU efficiency: {report.cpuEfficiency:.2f}")
        
    )pbdoc";

    // Enhanced exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Exception classes
    py::class_<NLMError, std::runtime_error>(m, "NLMError")
        .def(py::init<const std::string&>());

    py::class_<ConfigurationError, NLMError>(m, "ConfigurationError")
        .def(py::init<const std::string&>());

    py::class_<SimulationError, NLMError>(m, "SimulationError")
        .def(py::init<const std::string&>());

    py::class_<MemoryError, NLMError>(m, "MemoryError")
        .def(py::init<const std::string&>());

    // Configuration system enhancements
    py::class_<ConfigBuilder>(m, "ConfigBuilder")
        .def(py::init<>())
        .def("withNeuronCount", &ConfigBuilder::withNeuronCount)
        .def("withRegionCount", &ConfigBuilder::withRegionCount)
        .def("withConnectionProbability", &ConfigBuilder::withConnectionProbability)
        .def("withTimestep", &ConfigBuilder::withTimestep)
        .def("withRandomSeed", &ConfigBuilder::withRandomSeed)
        .def("withSTDP", &ConfigBuilder::withSTDP)
        .def("withHebbianLearning", &ConfigBuilder::withHebbianLearning)
        .def("withSynaptogenesis", &ConfigBuilder::withSynaptogenesis)
        .def("withPruning", &ConfigBuilder::withPruning)
        .def("withDevelopment", &ConfigBuilder::withDevelopment)
        .def("withNeuromodulation", &ConfigBuilder::withNeuromodulation)
        .def("withMemoryCapacity", &ConfigBuilder::withMemoryCapacity)
        .def("withPrediction", &ConfigBuilder::withPrediction)
        .def("withCognition", &ConfigBuilder::withCognition)
        .def("presetResearch", &ConfigBuilder::presetResearch)
        .def("presetMinimal", &ConfigBuilder::presetMinimal)
        .def("presetAgent", &ConfigBuilder::presetAgent)
        .def("experimental", &ConfigBuilder::experimental)
        .def("build", &ConfigBuilder::build)
        .def("getConfig", &ConfigBuilder::getConfig);

    // Configuration presets
    m.def("ConfigPresets", []() {
        py::module m = py::module::import("pynlm");
        return m;
    }, "Configuration presets module");

    py::class_<std::shared_ptr<Config>>(m, "Config", py::module_local())
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile)
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        })
        .def("saveToFile", &Config::saveToFile)
        .def("has", &Config::has)
        .def("getKeys", &Config::getKeys)
        .def("clear", &Config::clear)
        .def("summary", &Config::summary)
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced Brain class
    py::class_<EnhancedBrain>(m, "EnhancedBrain")
        .def(py::init<std::shared_ptr<Config>>())
        .def("initialize", &EnhancedBrain::initialize)
        .def("runSteps", &EnhancedBrain::runSteps)
        .def("runWithSensory", &EnhancedBrain::runWithSensory)
        .def("getBrain", &EnhancedBrain::getBrain)
        .def("isInitialized", &EnhancedBrain::isInitialized)
        .def("reset", &EnhancedBrain::reset)
        .def("__enter__", &EnhancedBrain::__enter__)
        .def("__exit__", &EnhancedBrain::__exit__);

    // Simulation result types
    py::class_<EnhancedBrain::SimulationResult>(m, "SimulationResult")
        .def_readonly("stepsCompleted", &EnhancedBrain::SimulationResult::stepsCompleted)
        .def_readonly("totalSpikes", &EnhancedBrain::SimulationResult::totalSpikes)
        .def_readonly("averageFiringRate", &EnhancedBrain::SimulationResult::averageFiringRate)
        .def_readonly("eIratio", &EnhancedBrain::SimulationResult::eIratio)
        .def_readonly("elapsedTime", &EnhancedBrain::SimulationResult::elapsedTime)
        .def_readonly("completed", &EnhancedBrain::SimulationResult::completed);

    // Enhanced AgentBrain class
    py::class_<EnhancedAgentBrain>(m, "EnhancedAgentBrain")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("initialize", &EnhancedAgentBrain::initialize)
        .def("processInput", &EnhancedAgentBrain::processInput)
        .def("decodeAction", &EnhancedAgentBrain::decodeAction)
        .def("applyReward", &EnhancedAgentBrain::applyReward)
        .def("updateDevelopment", &EnhancedAgentBrain::updateDevelopment)
        .def("enableFeature", &EnhancedAgentBrain::enableFeature)
        .def("getBrain", &EnhancedAgentBrain::getBrain)
        .def("getCuriosityLevel", &EnhancedAgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &EnhancedAgentBrain::getNoveltyLevel)
        .def("getPredictionError", &EnhancedAgentBrain::getPredictionError)
        .def("getDevelopmentalStage", &EnhancedAgentBrain::getDevelopmentalStage);

    // Action class (already existing, add enhancements)
    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>())
        .def(py::init<ActionType, std::vector<float>>())
        .def("getType", &Action::getType)
        .def("setType", &Action::setType)
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters)
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& a) {
            return "<Action: " + a.getName() + " parameters=" + std::to_string(a.getParameters().size()) + ">";
        });

    // Simulation manager
    py::class_<SimulationManager>(m, "SimulationManager")
        .def(py::init<>())
        .def("createConfig", &SimulationManager::createConfig)
        .def("createWorld", &SimulationManager::createWorld)
        .def("initializeBrain", &SimulationManager::initializeBrain)
        .def("runAgentSimulation", &SimulationManager::runAgentSimulation)
        .def("getBrain", &SimulationManager::getBrain)
        .def("getWorld", &SimulationManager::getWorld)
        .def("getAgent", &SimulationManager::getAgent)
        .def("getConfig", &SimulationManager::getConfig);

    // Agent simulation result
    py::class_<SimulationManager::AgentSimulationResult>(m, "AgentSimulationResult")
        .def_readonly("steps", &SimulationManager::AgentSimulationResult::steps)
        .def_readonly("totalReward", &SimulationManager::AgentSimulationResult::totalReward)
        .def_readonly("finalCuriosity", &SimulationManager::AgentSimulationResult::finalCuriosity)
        .def_readonly("finalNovelty", &SimulationManager::AgentSimulationResult::finalNovelty)
        .def_readonly("finalPredictionError", &SimulationManager::AgentSimulationResult::finalPredictionError)
        .def_readonly("averageFiringRate", &SimulationManager::AgentSimulationResult::averageFiringRate)
        .def_readonly("elapsedTime", &SimulationManager::AgentSimulationResult::elapsedTime)
        .def_readonly("trajectory", &SimulationManager::AgentSimulationResult::trajectory);

    // Performance monitor
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor")
        .def(py::init<>())
        .def("startMeasurement", &PerformanceMonitor::startMeasurement)
        .def("stopMeasurement", &PerformanceMonitor::stopMeasurement)
        .def("recordCPUTime", &PerformanceMonitor::recordCPUTime)
        .def("recordWallTime", &PerformanceMonitor::recordWallTime)
        .def("recordMemoryUsage", &PerformanceMonitor::recordMemoryUsage)
        .def("recordSpikeCount", &PerformanceMonitor::recordSpikeCount)
        .def("recordFiringRate", &PerformanceMonitor::recordFiringRate)
        .def("generateReport", &PerformanceMonitor::generateReport)
        .def("reset", &PerformanceMonitor::reset);

    // Performance report
    py::class_<PerformanceMonitor::PerformanceReport>(m, "PerformanceReport")
        .def_readonly("avgCPUTime", &PerformanceMonitor::PerformanceReport::avgCPUTime)
        .def_readonly("avgWallTime", &PerformanceMonitor::PerformanceReport::avgWallTime)
        .def_readonly("cpuEfficiency", &PerformanceMonitor::PerformanceReport::cpuEfficiency)
        .def_readonly("maxMemoryUsage", &PerformanceMonitor::PerformanceReport::maxMemoryUsage)
        .def_readonly("totalSpikes", &PerformanceMonitor::PerformanceReport::totalSpikes)
        .def_readonly("avgFiringRate", &PerformanceMonitor::PerformanceReport::avgFiringRate)
        .def_readonly("measurementCount", &PerformanceMonitor::PerformanceReport::measurementCount);

    // Debug tools
    py::class_<DebugTools>(m, "DebugTools")
        .def_static("visualizeConnectivity", &DebugTools::visualizeConnectivity)
        .def_static("exportBrainState", &DebugTools::exportBrainState)
        .def_static("importBrainState", &DebugTools::importBrainState)
        .def_static("generateDebugStats", &DebugTools::generateDebugStats);

    // Configuration presets (function bindings)
    m.def("createConfigBuilder", []() {
        return ConfigBuilder();
    }, "Create a configuration builder for fluent configuration");

    m.def("ConfigPresets", []() {
        // This will be handled by the existing ConfigPresets module
        return py::module::import("pynlm").attr("ConfigPresets");
    }, "Access configuration presets");

    // Factory functions for common use cases
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        return EnhancedBrain(brain);
    }, "Create an enhanced brain with configuration");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return EnhancedAgentBrain(brain);
    }, "Create an enhanced agent brain");

    m.def("createSimulationManager", []() {
        return SimulationManager();
    }, "Create a simulation manager for complete simulations");

    m.def("createPerformanceMonitor", []() {
        return PerformanceMonitor();
    }, "Create a performance monitor for tracking simulation performance");

    // Debug tools functions
    m.def("visualizeBrainConnectivity", &DebugTools::visualizeConnectivity);
    m.def("exportBrainState", &DebugTools::exportBrainState);
    m.def("importBrainState", &DebugTools::importBrainState);
    m.def("generateBrainStats", &DebugTools::generateDebugStats);

    // Convenience functions
    m.def("createDefaultBrain", []() {
        auto config = ConfigPresets::minimal();
        return EnhancedBrain(config);
    }, "Create a brain with minimal configuration");

    m.def("createResearchBrain", []() {
        auto config = ConfigPresets::research();
        return EnhancedBrain(config);
    }, "Create a brain with research configuration");

    m.def("createAgentBrain", []() {
        auto config = ConfigPresets::agent();
        auto brain = std::make_shared<Brain>(config);
        return EnhancedAgentBrain(brain);
    }, "Create an agent brain with agent configuration");

    // Add some constants for convenience
    m.attr("VERSION") = "2.0.0";
    m.attr("BUILD_TYPE") = "Release";

    // Module initialization
    m.def("initializeNLM", []() {
        NLM_LOG_INFO("Enhanced NLM Python bindings initialized");
    }, "Initialize the enhanced NLM bindings");

    m.def("shutdownNLM", []() {
        NLM_LOG_INFO("Enhanced NLM Python bindings shutting down");
    }, "Shutdown the enhanced NLM bindings");
}

} // namespace nlm
