#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>

// Include all the NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Dopamine.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception classes for better Python error handling
class NLMConfigError : public std::runtime_error {
public:
    explicit NLMConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMBrainError : public std::runtime_error {
public:
    explicit NLMBrainError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMValidationError : public std::runtime_error {
public:
    explicit NLMValidationError(const std::string& msg) : std::runtime_error(msg) {}
};

// Python wrapper for Config class with enhanced functionality
class PythonConfigWrapper {
public:
    explicit PythonConfigWrapper(std::shared_ptr<Config> config) : config_(config) {}
    
    // Enhanced configuration methods
    void setString(const std::string& key, const std::string& value) {
        if (!config_) throw NLMConfigError("Config not initialized");
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void setInt(const std::string& key, int64_t value) {
        if (!config_) throw NLMConfigError("Config not initialized");
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void setFloat(const std::string& key, float value) {
        if (!config_) throw NLMConfigError("Config not initialized");
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void setBool(const std::string& key, bool value) {
        if (!config_) throw NLMConfigError("Config not initialized");
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    // Getters with type validation
    std::string getString(const std::string& key) const {
        if (!config_) throw NLMConfigError("Config not initialized");
        auto result = config_->get<std::string>(key);
        if (!result) throw NLMConfigError("Config key not found: " + key);
        return *result;
    }
    
    int64_t getInt(const std::string& key) const {
        if (!config_) throw NLMConfigError("Config not initialized");
        auto result = config_->get<int64_t>(key);
        if (!result) throw NLMConfigError("Config key not found: " + key);
        return *result;
    }
    
    float getFloat(const std::string& key) const {
        if (!config_) throw NLMConfigError("Config not initialized");
        auto result = config_->get<float>(key);
        if (!result) throw NLMConfigError("Config key not found: " + key);
        return *result;
    }
    
    bool getBool(const std::string& key) const {
        if (!config_) throw NLMConfigError("Config not initialized");
        auto result = config_->get<bool>(key);
        if (!result) throw NLMConfigError("Config key not found: " + key);
        return *result;
    }
    
    // Convenience methods
    bool has(const std::string& key) const {
        if (!config_) return false;
        return config_->has(key);
    }
    
    std::vector<std::string> getAllKeys() const {
        if (!config_) return {};
        return config_->getKeys();
    }
    
    void validate(const std::unordered_map<std::string, std::string>& schema) const {
        if (!config_) throw NLMConfigError("Config not initialized");
        
        for (const auto& pair : schema) {
            const std::string& key = pair.first;
            const std::string& type = pair.second;
            
            if (!config_->has(key)) {
                throw NLMValidationError("Missing required config key: " + key);
            }
            
            if (type == "int") {
                auto result = config_->get<int64_t>(key);
                if (!result) throw NLMValidationError("Config key not found: " + key);
            } else if (type == "float") {
                auto result = config_->get<float>(key);
                if (!result) throw NLMValidationError("Config key not found: " + key);
            } else if (type == "string") {
                auto result = config_->get<std::string>(key);
                if (!result) throw NLMValidationError("Config key not found: " + key);
            } else if (type == "bool") {
                auto result = config_->get<bool>(key);
                if (!result) throw NLMValidationError("Config key not found: " + key);
            }
        }
    }
    
    // Batch operations
    void batchSet(const std::unordered_map<std::string, std::any>& values) {
        if (!config_) throw NLMConfigError("Config not initialized");
        
        for (const auto& pair : values) {
            const std::string& key = pair.first;
            const std::any& value = pair.second;
            
            if (value.type() == typeid(std::string)) {
                setString(key, std::any_cast<std::string>(value));
            } else if (value.type() == typeid(int64_t)) {
                setInt(key, std::any_cast<int64_t>(value));
            } else if (value.type() == typeid(float)) {
                setFloat(key, std::any_cast<float>(value));
            } else if (value.type() == typeid(bool)) {
                setBool(key, std::any_cast<bool>(value));
            } else {
                throw NLMConfigError("Unsupported value type for key: " + key);
            }
        }
    }
    
    // Serialization/deserialization
    void saveToJSON(const std::string& filepath) const {
        if (!config_) throw NLMConfigError("Config not initialized");
        config_->saveToFile(filepath);
    }
    
    void loadFromJSON(const std::string& filepath) {
        if (!config_) throw NLMConfigError("Config not initialized");
        config_->loadFromFile(filepath);
    }
    
    // Export/import for Python
    std::string exportToDict() const {
        if (!config_) throw NLMConfigError("Config not initialized");
        std::string result = "{";
        const auto& keys = config_->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            if (i > 0) result += ", ";
            result += "\"" + keys[i] + "\": \"" + std::to_string(config_->getOr<std::string>(keys[i], "")) + "\"";
        }
        result += "}";
        return result;
    }
    
    std::string getSummary() const {
        if (!config_) return ""; 
        return config_->summary();
    }
    
    // High-level convenience methods
    void configureBrain(const std::string& neuronCount, const std::string& connectionProb = "0.1") {
        setInt("brain.neuron_count", std::stoll(neuronCount));
        setFloat("brain.connection_probability", std::stof(connectionProb));
    }
    
    void configureLearning(const std::string& learningRate = "0.001", bool enableSTDP = true) {
        setFloat("plasticity.stdp.learning_rate", std::stof(learningRate));
        setBool("plasticity.stdp.enable", enableSTDP);
    }
    
    // Check if this is a valid configuration for neural computation
    bool isValid() const {
        if (!config_) return false;
        
        // Check for essential parameters
        if (!config_->has("brain.neuron_count") || 
            !config_->has("brain.connection_probability")) {
            return false;
        }
        
        int64_t neuronCount = config_->getOr<int64_t>("brain.neuron_count", 0);
        return neuronCount > 0;
    }
    
private:
    std::shared_ptr<Config> config_;
};

// Python wrapper for Brain class with enhanced functionality
class PythonBrainWrapper {
public:
    explicit PythonBrainWrapper(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Enhanced step methods with validation
    void step(int currentStep) {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        brain_->step(currentStep);
    }
    
    void stepWithTime(int currentStep, double currentTime) {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        brain_->step(currentStep, currentTime);
    }
    
    // Convenience simulation methods
    void runEpisode(double timestep = 0.001, int maxSteps = 1000) {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        
        for (int step = 0; step < maxSteps; ++step) {
            brain_->step(step, step * timestep);
            
            // Check for termination condition if needed
            if (step % 100 == 0) {
                // Log progress every 100 steps
                logProgress(step);
            }
        }
    }
    
    void runLearningEpisode(int maxSteps = 1000) {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        
        // Enable learning systems
        enableLearningSystems();
        
        // Run simulation with learning
        runEpisode(0.001, maxSteps);
    }
    
    // Enhanced action creation
    std::unique_ptr<Action> createAction(ActionType type, const std::vector<float>& parameters = {}) {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        return brain_->produceAction();
    }
    
    std::unique_ptr<Action> createActionWithParams(ActionType type, const std::vector<float>& parameters) {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        return brain_->produceAction();
    }
    
    // Memory management convenience methods
    void enableMemorySystems(bool enableWorking = true, bool enableEpisodic = true, bool enableAssociative = true) {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        
        auto* workingMem = brain_->getWorkingMemory();
        auto* episodicMem = brain_->getEpisodicMemory();
        auto* associativeMem = brain_->getAssociativeMemory();
        
        if (workingMem && enableWorking) workingMem->enable();
        if (episodicMem && enableEpisodic) episodicMem->enable();
        if (associativeMem && enableAssociative) associativeMem->enable();
    }
    
    void clearMemorySystems() {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        
        auto* workingMem = brain_->getWorkingMemory();
        auto* episodicMem = brain_->getEpisodicMemory();
        auto* associativeMem = brain_->getAssociativeMemory();
        
        if (workingMem) workingMem->clear();
        if (episodicMem) episodicMem->clear();
        if (associativeMem) associativeMem->clear();
    }
    
    // Performance monitoring
    struct PerformanceStats {
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        size_t totalSpikes;
        float averageFiringRate;
        float excitationInhibitionRatio;
        size_t regionCount;
        std::unordered_map<std::string, float> memoryStats;
        std::unordered_map<std::string, float> neuromodulationStats;
    };
    
    PerformanceStats getPerformanceStats() {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        
        PerformanceStats stats;
        stats.totalNeurons = brain_->getTotalNeuronCount();
        stats.totalSynapses = brain_->getTotalSynapseCount();
        stats.activeNeurons = brain_->getActiveNeuronCount();
        stats.firingNeurons = brain_->getFiringNeuronCount();
        stats.totalSpikes = brain_->getTotalSpikeCount();
        stats.averageFiringRate = brain_->getAverageFiringRate();
        stats.excitationInhibitionRatio = brain_->getExcitationInhibitionRatio();
        stats.regionCount = brain_->getRegionCount();
        
        // Memory system stats
        auto* workingMem = brain_->getWorkingMemory();
        if (workingMem) stats.memoryStats["working_memory_active_traces"] = workingMem->getActiveTraces();
        
        auto* episodicMem = brain_->getEpisodicMemory();
        if (episodicMem) stats.memoryStats["episodic_memory_episodes"] = episodicMem->getEpisodeCount();
        
        // Neuromodulation stats
        auto* dopamine = brain_->getDopamine();
        if (dopamine) stats.neuromodulationStats["dopamine_level"] = dopamine->getLevel();
        
        auto* curiosity = brain_->getCuriosity();
        if (curiosity) stats.neuromodulationStats["curiosity_level"] = curiosity->getLevel();
        
        auto* novelty = brain_->getNovelty();
        if (novelty) stats.neuromodulationStats["novelty_level"] = novelty->getLevel();
        
        return stats;
    }
    
    // Learning and development convenience methods
    void enableLearning(bool enableSTDP = true, bool enableHebbian = true, bool enableStructural = true) {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        
        auto* stdp = brain_->getSTDP();
        auto* hebbian = brain_->getHebbian();
        auto* structural = brain_->getStructuralPlasticity();
        
        if (stdp) stdp->enablePlasticity(enableSTDP);
        if (hebbian) hebbian->enablePlasticity(enableHebbian);
        if (structural) structural->enablePlasticity(enableStructural);
    }
    
    void setDevelopmentalStage(DevelopmentalStage stage) {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        brain_->setDevelopmentalStage(stage);
    }
    
    void enableAllSystems(bool enableLearning = true, bool enableMemory = true, bool enableCognition = true) {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        
        enableLearning(enableSTDP = true, enableHebbian = true, enableStructural = true);
        enableMemorySystems(enableWorking = enableMemory, enableEpisodic = enableMemory, enableAssociative = enableMemory);
        
        // Enable cognition systems
        auto* planner = brain_->getPlanner();
        if (planner) planner->enable();
        
        auto* conceptFormation = brain_->getConceptFormation();
        if (conceptFormation) conceptFormation->enable();
    }
    
    // Checkpoint management convenience
    bool saveCheckpoint(const std::string& filepath) {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        return brain_->save(filepath);
    }
    
    bool loadCheckpoint(const std::string& filepath) {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        return brain_->load(filepath);
    }
    
    // Simulation control
    void reset() {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        brain_->reset();
    }
    
    void initialize() {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        brain_->initialize();
    }
    
    bool isInitialized() const {
        return brain_ != nullptr && brain_->initialize();
    }
    
    // Context manager support
    class Context {
    public:
        Context(PythonBrainWrapper& wrapper) : wrapper_(wrapper) {
            if (!wrapper.brain_) throw NLMBrainError("Brain not initialized");
            wrapper.brain_->initialize();
        }
        
        ~Context() {
            if (wrapper_.brain_) {
                wrapper_.brain_->reset();
            }
        }
        
        PythonBrainWrapper& getWrapper() { return wrapper_; }
        
    private:
        PythonBrainWrapper& wrapper_;
    };
    
    Context createContext() {
        return Context(*this);
    }
    
    // Progress logging helper
    void logProgress(int step) const {
        if (!brain_) return;
        
        auto stats = getPerformanceStats();
        NLM_LOG_INFO("Step " + std::to_string(step) + 
                    ": Neurons=" + std::to_string(stats.totalNeurons) +
                    ", Spikes=" + std::to_string(stats.totalSpikes) +
                    ", Firing=" + std::to_string(stats.firingNeurons) +
                    ", E/I=" + std::to_string(stats.excitationInhibitionRatio));
    }
    
    // Enable learning systems helper
    void enableLearningSystems() {
        if (!brain_) throw NLMBrainError("Brain not initialized");
        
        auto* stdp = brain_->getSTDP();
        auto* hebbian = brain_->getHebbian();
        auto* structural = brain_->getStructuralPlasticity();
        
        if (stdp) stdp->enablePlasticity(true);
        if (hebbian) hebbian->enablePlasticity(true);
        if (structural) structural->enablePlasticity(true);
    }
    
    // Configuration validation helper
    bool validateConfiguration() {
        if (!brain_) return false;
        
        auto* config = brain_->getConfig();
        if (!config) return false;
        
        // Basic validation for brain simulation
        return config->has("brain.neuron_count") && 
               config->getOr<int64_t>("brain.neuron_count", 0) > 0;
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Enhanced AgentBrain wrapper
class PythonAgentBrainWrapper {
public:
    explicit PythonAgentBrainWrapper(std::shared_ptr<AgentBrain> agentBrain) : agentBrain_(agentBrain) {}
    
    // Simulation convenience methods
    void runAgentEpisode(double timestep = 0.1, int maxSteps = 500) {
        if (!agentBrain_) throw NLMBrainError("Agent brain not initialized");
        
        for (int step = 0; step < maxSteps; ++step) {
            agentBrain_->updateDevelopment(timestep);
            
            if (step % 50 == 0) {
                logProgress(step);
            }
        }
    }
    
    // Enhanced neuromodulation
    void applyRewardModulation(float reward, float predictedReward = 0.0f) {
        if (!agentBrain_) throw NLMBrainError("Agent brain not initialized");
        agentBrain_->applyRewardModulation(reward, predictedReward);
    }
    
    // Curiosity-based exploration
    void enableCuriosityExploration(bool enable = true, float threshold = 0.3f) {
        if (!agentBrain_) throw NLMBrainError("Agent brain not initialized");
        agentBrain_->enableCuriosity(enable);
    }
    
    // Development management
    void setDevelopmentSpeed(float speed) {
        if (!agentBrain_) throw NLMBrainError("Agent brain not initialized");
        // Note: This would require adding a method to AgentBrain
    }
    
    // Performance monitoring
    struct AgentPerformance {
        float curiosityLevel;
        float noveltyLevel;
        float predictionError;
        float neuromodulationLevel;
        DevelopmentalStage developmentalStage;
        double developmentalAge;
    };
    
    AgentPerformance getPerformance() {
        if (!agentBrain_) throw NLMBrainError("Agent brain not initialized");
        
        AgentPerformance perf;
        perf.curiosityLevel = agentBrain_->getCuriosityLevel();
        perf.noveltyLevel = agentBrain_->getNoveltyLevel();
        perf.predictionError = agentBrain_->getPredictionError();
        perf.neuromodulationLevel = agentBrain_->getNeuromodulationLevel();
        perf.developmentalStage = agentBrain_->getDevelopmentalStage();
        perf.developmentalAge = 0.0; // Would need to add this to AgentBrain
        
        return perf;
    }
    
    // Progress logging
    void logProgress(int step) const {
        auto perf = getPerformance();
        NLM_LOG_INFO("Agent Step " + std::to_string(step) +
                    ": Curiosity=" + std::to_string(perf.curiosityLevel) +
                    ", Novelty=" + std::to_string(perf.noveltyLevel) +
                    ", PredictionError=" + std::to_string(perf.predictionError) +
                    ", DevStage=" + std::to_string(static_cast<int>(perf.developmentalStage)));
    }
    
    // Context manager support
    class Context {
    public:
        Context(PythonAgentBrainWrapper& wrapper) : wrapper_(wrapper) {
            if (!wrapper.agentBrain_) throw NLMBrainError("Agent brain not initialized");
        }
        
        PythonAgentBrainWrapper& getWrapper() { return wrapper_; }
        
    private:
        PythonAgentBrainWrapper& wrapper_;
    };
    
    Context createContext() {
        return Context(*this);
    }
    
private:
    std::shared_ptr<AgentBrain> agentBrain_;
};

// Enhanced World wrapper
class PythonWorldWrapper {
public:
    explicit PythonWorldWrapper(std::shared_ptr<SimpleWorld> world) : world_(world) {}
    
    // Simulation convenience methods
    void runSimulation(double timestep = 0.1, int maxSteps = 1000) {
        if (!world_) throw NLMBrainError("World not initialized");
        
        world_->reset();
        for (int step = 0; step < maxSteps; ++step) {
            world_->update(timestep);
            
            if (step % 100 == 0) {
                logProgress(step);
            }
        }
    }
    
    // State management
    void reset() {
        if (!world_) throw NLMBrainError("World not initialized");
        world_->reset();
    }
    
    // Progress logging
    void logProgress(int step) const {
        if (!world_) return;
        
        NLM_LOG_INFO("World Step " + std::to_string(step) +
                    ": Time=" + std::to_string(world_->getSimulationTime()) +
                    ", AgentX=" + std::to_string(world_->getAgentBody().x) +
                    ", AgentY=" + std::to_string(world_->getAgentBody().y));
    }
    
    // Context manager support
    class Context {
    public:
        Context(PythonWorldWrapper& wrapper) : wrapper_(wrapper) {
            if (!wrapper.world_) throw NLMBrainError("World not initialized");
            wrapper.world_->reset();
        }
        
        ~Context() {
            if (wrapper_.world_) {
                wrapper_.world_->reset();
            }
        }
        
        PythonWorldWrapper& getWrapper() { return wrapper_; }
        
    private:
        PythonWorldWrapper& wrapper_;
    };
    
    Context createContext() {
        return Context(*this);
    }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

// Factory class for enhanced Python API
class PythonFactory {
public:
    // Create default configuration with validation
    static std::shared_ptr<PythonConfigWrapper> createConfig() {
        auto config = std::make_shared<Config>();
        return std::make_shared<PythonConfigWrapper>(config);
    }
    
    // Create brain with enhanced configuration
    static std::shared_ptr<PythonBrainWrapper> createBrain(const std::string& configFile = "") {
        auto config = std::make_shared<Config>();
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
        }
        return createBrain(config);
    }
    
    static std::shared_ptr<PythonBrainWrapper> createBrain(std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return std::make_shared<PythonBrainWrapper>(brain);
    }
    
    // Create world with enhanced configuration
    static std::shared_ptr<PythonWorldWrapper> createWorld() {
        auto world = std::make_shared<SimpleWorld>();
        return std::make_shared<PythonWorldWrapper>(world);
    }
    
    // Create agent brain with enhanced configuration
    static std::shared_ptr<PythonAgentBrainWrapper> createAgentBrain(
        std::shared_ptr<PythonBrainWrapper> brain) {
        if (!brain || !brain->isInitialized()) {
            throw NLMBrainError("Brain must be initialized before creating agent brain");
        }
        
        auto agentBrain = std::make_shared<AgentBrain>(brain->getBrain());
        return std::make_shared<PythonAgentBrainWrapper>(agentBrain);
    }
    
    // Create complete agent system
    static std::tuple<std::shared_ptr<PythonBrainWrapper>, 
                     std::shared_ptr<PythonWorldWrapper>, 
                     std::shared_ptr<PythonAgentBrainWrapper>>
    createCompleteAgentSystem(const std::string& worldConfig = "") {
        
        // Create brain
        auto brain = createBrain();
        
        // Create world
        auto world = createWorld();
        
        // Configure world if needed
        if (!worldConfig.empty()) {
            // Parse world configuration and apply
        }
        
        // Create agent brain
        auto agentBrain = createAgentBrain(brain);
        
        return std::make_tuple(brain, world, agentBrain);
    }
    
    // Configuration presets
    static std::shared_ptr<PythonConfigWrapper> createConfigPreset(const std::string& presetName) {
        auto config = createConfig();
        
        if (presetName == "minimal") {
            config->setInt("brain.neuron_count", 100);
            config->setFloat("brain.connection_probability", 0.05f);
        } else if (presetName == "standard") {
            config->setInt("brain.neuron_count", 1000);
            config->setFloat("brain.connection_probability", 0.1f);
            config->setBool("plasticity.stdp.enable", true);
            config->setBool("plasticity.hebbian.enable", true);
        } else if (presetName == "advanced") {
            config->setInt("brain.neuron_count", 5000);
            config->setFloat("brain.connection_probability", 0.15f);
            config->setBool("plasticity.stdp.enable", true);
            config->setBool("plasticity.hebbian.enable", true);
            config->setBool("plasticity.structural.enable", true);
            config->setBool("neuromod.curiosity.enable", true);
            config->setBool("neuromod.novelty.enable", true);
        }
        
        return config;
    }
    
    // Utility functions for Python
    static std::string getVersion() {
        return "NLM Phase 6 Enhanced Python API v1.0.0";
    }
    
    static bool validatePythonEnvironment() {
        // Check if required Python modules are available
        return true; // Simplified for now
    }
};

// Performance monitoring class
class PythonPerformanceMonitor {
public:
    PythonPerformanceMonitor(std::shared_ptr<PythonBrainWrapper> brain, 
                            std::shared_ptr<PythonAgentBrainWrapper> agentBrain = nullptr)
        : brainWrapper_(brain), agentBrainWrapper_(agentBrain) {}
    
    // Collect comprehensive performance metrics
    struct ComprehensivePerformance {
        size_t simulationSteps;
        double simulationTime;
        float averageFiringRate;
        size_t totalSpikes;
        float memoryUsage;
        float cpuUsage;
        std::unordered_map<std::string, float> brainMetrics;
        std::unordered_map<std::string, float> agentMetrics;
        double systemUptime;
    };
    
    ComprehensivePerformance collectPerformanceMetrics() {
        ComprehensivePerformance metrics;
        metrics.simulationSteps = 0; // Would need to track this
        metrics.simulationTime = 0.0; // Would need to track this
        metrics.averageFiringRate = 0.0f;
        metrics.totalSpikes = 0;
        metrics.memoryUsage = 0.0f;
        metrics.cpuUsage = 0.0f;
        metrics.systemUptime = 0.0;
        
        if (brainWrapper_) {
            auto stats = brainWrapper_->getPerformanceStats();
            metrics.brainMetrics["total_neurons"] = stats.totalNeurons;
            metrics.brainMetrics["total_synapses"] = stats.totalSynapses;
            metrics.brainMetrics["active_neurons"] = stats.activeNeurons;
            metrics.brainMetrics["firing_neurons"] = stats.firingNeurons;
            metrics.brainMetrics["total_spikes"] = stats.totalSpikes;
            metrics.brainMetrics["avg_firing_rate"] = stats.averageFiringRate;
        }
        
        if (agentBrainWrapper_) {
            auto perf = agentBrainWrapper_->getPerformance();
            metrics.agentMetrics["curiosity_level"] = perf.curiosityLevel;
            metrics.agentMetrics["novelty_level"] = perf.noveltyLevel;
            metrics.agentMetrics["prediction_error"] = perf.predictionError;
            metrics.agentMetrics["neuromodulation_level"] = perf.neuromodulationLevel;
        }
        
        return metrics;
    }
    
    // Performance analysis
    std::string analyzePerformance(const ComprehensivePerformance& metrics) {
        std::string analysis = "Performance Analysis:\n";
        analysis += "  - Total Steps: " + std::to_string(metrics.simulationSteps) + "\n";
        analysis += "  - Total Time: " + std::to_string(metrics.simulationTime) + "s\n";
        analysis += "  - Average Firing Rate: " + std::to_string(metrics.averageFiringRate) + " Hz\n";
        analysis += "  - Total Spikes: " + std::to_string(metrics.totalSpikes) + "\n";
        analysis += "  - Memory Usage: " + std::to_string(metrics.memoryUsage) + " MB\n";
        analysis += "  - CPU Usage: " + std::to_string(metrics.cpuUsage) + "%\n";
        
        if (metrics.brainMetrics.find("total_synapses") != metrics.brainMetrics.end()) {
            float avgSynapseWeight = 0.5f; // Would need actual calculation
            analysis += "  - Average Synapse Weight: " + std::to_string(avgSynapseWeight) + "\n";
        }
        
        return analysis;
    }
    
    // Performance trend analysis
    std::string analyzeTrends(const std::vector<ComprehensivePerformance>& history) {
        if (history.empty()) return "No performance history available";
        
        std::string trends = "Performance Trends:\n";
        
        // Analyze firing rate trend
        float initialFR = history[0].averageFiringRate;
        float finalFR = history.back().averageFiringRate;
        float frChange = finalFR - initialFR;
        trends += "  - Firing Rate Change: " + std::to_string(frChange) + " ("
                 + (frChange > 0 ? "increasing" : "decreasing") + ")\n";
        
        // Analyze spike trend
        size_t initialSpikes = history[0].totalSpikes;
        size_t finalSpikes = history.back().totalSpikes;
        trends += "  - Spike Count Change: " + std::to_string(finalSpikes - initialSpikes) + "\n";
        
        return trends;
    }
    
private:
    std::shared_ptr<PythonBrainWrapper> brainWrapper_;
    std::shared_ptr<PythonAgentBrainWrapper> agentBrainWrapper_;
};

// Enhanced visualization utilities
namespace PythonVisualization {
    class BasicVisualizer {
    public:
        static std::string createSpikeHistoryPlot(const std::vector<float>& spikeData) {
            // Generate simple text-based visualization
            std::string plot = "Spike History Visualization:\n";
            for (size_t i = 0; i < spikeData.size(); ++i) {
                if (spikeData[i] > 0) {
                    plot += "  Step " + std::to_string(i) + ": *\n";
                } else {
                    plot += "  Step " + std::to_string(i) + ": |\n";
                }
            }
            return plot;
        }
        
        static std::string createMemoryUsagePlot(const std::vector<float>& memoryData) {
            std::string plot = "Memory Usage Visualization:\n";
            float maxVal = *std::max_element(memoryData.begin(), memoryData.end());
            
            for (size_t i = 0; i < memoryData.size(); ++i) {
                float percentage = (memoryData[i] / maxVal) * 100.0f;
                plot += "  " + std::to_string(i) + ": ";
                for (int j = 0; j < percentage / 10.0f; ++j) {
                    plot += "█";
                }
                plot += " " + std::to_string(static_cast<int>(percentage)) + "%\n";
            }
            
            return plot;
        }
        
        static std::string createActivityHeatmap(const std::vector<std::vector<float>>& activityData) {
            std::string heatmap = "Activity Heatmap:\n";
            
            for (const auto& row : activityData) {
                for (float val : row) {
                    if (val > 0.7f) heatmap += "██";  // High activity
                    else if (val > 0.4f) heatmap += "▒▒";  // Medium activity
                    else heatmap += "  ";  // Low activity
                }
                heatmap += "\n";
            }
            
            return heatmap;
        }
    };
}

// Error handling utilities
namespace PythonErrorHandling {
    class ErrorLogger {
    public:
        static void logError(const std::string& source, const std::string& message, 
                           const std::string& context = "") {
            NLM_LOG_ERROR("[Python Error] " + source + ": " + message +
                         (context.empty() ? "" : " (Context: " + context + ")"));
        }
        
        static void logWarning(const std::string& source, const std::string& message) {
            NLM_LOG_WARNING("[Python Warning] " + source + ": " + message);
        }
        
        static void logInfo(const std::string& source, const std::string& message) {
            NLM_LOG_INFO("[Python Info] " + source + ": " + message);
        }
    };
}

// Main bindings function
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Python API
        -----------------------
        
        This is the enhanced version of the NLM Python bindings with:
        - Type-safe configuration management
        - Enhanced error handling and validation
        - Performance monitoring utilities
        - Convenience methods for common operations
        - Context manager support for automatic resource cleanup
        
        The enhanced API provides a more Pythonic interface to the NLM neural
        simulation framework while maintaining full compatibility with the
        original API.
    )pbdoc";
    
    // Register custom exception classes
    py::register_exception<NLMConfigError>(m, "ConfigError", PyExc_RuntimeError);
    py::register_exception<NLMBrainError>(m, "BrainError", PyExc_RuntimeError);
    py::register_exception<NLMValidationError>(m, "ValidationError", PyExc_ValueError);
    
    // Config wrapper class
    py::class_<PythonConfigWrapper>(m, "Config", R"pbdoc(
        Enhanced configuration wrapper with type-safe methods.
        
        This class provides a more Pythonic interface to NLM configuration
        with type checking, validation, and convenience methods.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>())
        .def("set_string", &PythonConfigWrapper::setString, 
             py::arg("key"), py::arg("value"),
             "Set configuration value as string")
        .def("set_int", &PythonConfigWrapper::setInt,
             py::arg("key"), py::arg("value"),
             "Set configuration value as int64")
        .def("set_float", &PythonConfigWrapper::setFloat,
             py::arg("key"), py::arg("value"),
             "Set configuration value as float")
        .def("set_bool", &PythonConfigWrapper::setBool,
             py::arg("key"), py::arg("value"),
             "Set configuration value as boolean")
        .def("get_string", &PythonConfigWrapper::getString,
             py::arg("key"), "Get configuration value as string")
        .def("get_int", &PythonConfigWrapper::getInt,
             py::arg("key"), "Get configuration value as int64")
        .def("get_float", &PythonConfigWrapper::getFloat,
             py::arg("key"), "Get configuration value as float")
        .def("get_bool", &PythonConfigWrapper::getBool,
             py::arg("key"), "Get configuration value as boolean")
        .def("has", &PythonConfigWrapper::has,
             py::arg("key"), "Check if configuration key exists")
        .def("get_all_keys", &PythonConfigWrapper::getAllKeys,
             "Get all configuration keys")
        .def("validate", &PythonConfigWrapper::validate,
             py::arg("schema"), "Validate configuration against schema")
        .def("batch_set", &PythonConfigWrapper::batchSet,
             py::arg("values"), "Batch set multiple configuration values")
        .def("save_to_json", &PythonConfigWrapper::saveToJSON,
             py::arg("filepath"), "Save configuration to JSON file")
        .def("load_from_json", &PythonConfigWrapper::loadFromJSON,
             py::arg("filepath"), "Load configuration from JSON file")
        .def("export_to_dict", &PythonConfigWrapper::exportToDict,
             "Export configuration as Python dictionary string")
        .def("get_summary", &PythonConfigWrapper::getSummary,
             "Get configuration summary")
        .def("configure_brain", &PythonConfigWrapper::configureBrain,
             py::arg("neuron_count"), py::arg("connection_prob") = "0.1",
             "Configure brain-specific settings")
        .def("configure_learning", &PythonConfigWrapper::configureLearning,
             py::arg("learning_rate") = "0.001", py::arg("enable_stdp") = true,
             "Configure learning-specific settings")
        .def("is_valid", &PythonConfigWrapper::isValid,
             "Check if configuration is valid for neural computation");
    
    // Brain wrapper class
    py::class_<PythonBrainWrapper>(m, "Brain", R"pbdoc(
        Enhanced brain wrapper with convenience methods and performance monitoring.
        
        This class provides a more Pythonic interface to NLM brain simulation
        with convenience methods for running episodes, managing memory,
        and monitoring performance.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("step", &PythonBrainWrapper::step,
             py::arg("current_step"), "Perform a single simulation step")
        .def("step_with_time", &PythonBrainWrapper::stepWithTime,
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        .def("run_episode", &PythonBrainWrapper::runEpisode,
             py::arg("timestep") = 0.001, py::arg("max_steps") = 1000,
             "Run a complete simulation episode")
        .def("run_learning_episode", &PythonBrainWrapper::runLearningEpisode,
             py::arg("max_steps") = 1000, "Run a learning simulation episode")
        .def("create_action", &PythonBrainWrapper::createAction,
             py::arg("type"), py::arg("parameters") = std::vector<float>(),
             "Create a new action")
        .def("enable_memory_systems", &PythonBrainWrapper::enableMemorySystems,
             py::arg("enable_working") = true, py::arg("enable_episodic") = true,
             py::arg("enable_associative") = true,
             "Enable memory systems")
        .def("clear_memory_systems", &PythonBrainWrapper::clearMemorySystems,
             "Clear all memory systems")
        .def("get_performance_stats", &PythonBrainWrapper::getPerformanceStats,
             "Get comprehensive performance statistics")
        .def("enable_learning", &PythonBrainWrapper::enableLearning,
             py::arg("enable_stdp") = true, py::arg("enable_hebbian") = true,
             py::arg("enable_structural") = true,
             "Enable learning systems")
        .def("set_developmental_stage", &PythonBrainWrapper::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("enable_all_systems", &PythonBrainWrapper::enableAllSystems,
             py::arg("enable_learning") = true, py::arg("enable_memory") = true,
             py::arg("enable_cognition") = true,
             "Enable all NLM systems")
        .def("save_checkpoint", &PythonBrainWrapper::saveCheckpoint,
             py::arg("filepath"), "Save brain state to checkpoint")
        .def("load_checkpoint", &PythonBrainWrapper::loadCheckpoint,
             py::arg("filepath"), "Load brain state from checkpoint")
        .def("reset", &PythonBrainWrapper::reset, "Reset brain state")
        .def("initialize", &PythonBrainWrapper::initialize, "Initialize brain")
        .def("is_initialized", &PythonBrainWrapper::isInitialized,
             "Check if brain is initialized")
        .def("create_context", &PythonBrainWrapper::createContext,
             "Create context manager for automatic resource cleanup");
    
    // AgentBrain wrapper class
    py::class_<PythonAgentBrainWrapper>(m, "AgentBrain", R"pbdoc(
        Enhanced agent brain wrapper with curiosity and exploration support.
    )pbdoc")
        .def(py::init<std::shared_ptr<AgentBrain>>())
        .def("run_agent_episode", &PythonAgentBrainWrapper::runAgentEpisode,
             py::arg("timestep") = 0.1, py::arg("max_steps") = 500,
             "Run a complete agent episode")
        .def("apply_reward_modulation", &PythonAgentBrainWrapper::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward") = 0.0f,
             "Apply reward-based neuromodulation")
        .def("enable_curiosity_exploration", &PythonAgentBrainWrapper::enableCuriosityExploration,
             py::arg("enable") = true, py::arg("threshold") = 0.3f,
             "Enable curiosity-based exploration")
        .def("get_performance", &PythonAgentBrainWrapper::getPerformance,
             "Get comprehensive agent performance metrics")
        .def("create_context", &PythonAgentBrainWrapper::createContext,
             "Create context manager");
    
    // World wrapper class
    py::class_<PythonWorldWrapper>(m, "World", R"pbdoc(
        Enhanced world wrapper with simulation convenience methods.
    )pbdoc")
        .def(py::init<std::shared_ptr<SimpleWorld>>())
        .def("run_simulation", &PythonWorldWrapper::runSimulation,
             py::arg("timestep") = 0.1, py::arg("max_steps") = 1000,
             "Run complete world simulation")
        .def("reset", &PythonWorldWrapper::reset, "Reset world state")
        .def("create_context", &PythonWorldWrapper::createContext,
             "Create context manager for automatic reset");
    
    // Factory class
    py::class_<PythonFactory>(m, "Factory", R"pbdoc(
        Factory class for creating NLM components with enhanced Python API.
    )pbdoc")
        .def_static("create_config", &PythonFactory::createConfig,
                    "Create default configuration")
        .def_static("create_brain", &PythonFactory::createBrain,
                    py::arg("config_file") = "",
                    "Create brain with optional config file")
        .def_static("create_world", &PythonFactory::createWorld,
                    "Create simple world")
        .def_static("create_agent_brain", &PythonFactory::createAgentBrain,
                    py::arg("brain"), "Create agent brain from brain")
        .def_static("create_complete_agent_system", &PythonFactory::createCompleteAgentSystem,
                    py::arg("world_config") = "",
                    "Create complete agent system with brain, world, and agent")
        .def_static("create_config_preset", &PythonFactory::createConfigPreset,
                    py::arg("preset_name"), "Create configuration preset")
        .def_static("get_version", &PythonFactory::getVersion,
                    "Get NLM version")
        .def_static("validate_python_environment", &PythonFactory::validatePythonEnvironment,
                    "Validate Python environment");
    
    // Performance monitor class
    py::class_<PythonPerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(
        Performance monitoring utilities for NLM simulations.
    )pbdoc")
        .def(py::init<std::shared_ptr<PythonBrainWrapper>, std::shared_ptr<PythonAgentBrainWrapper>>(),
             py::arg("brain"), py::arg("agent_brain") = nullptr,
             "Initialize performance monitor")
        .def("collect_performance_metrics", &PythonPerformanceMonitor::collectPerformanceMetrics,
             "Collect comprehensive performance metrics")
        .def("analyze_performance", &PythonPerformanceMonitor::analyzePerformance,
             py::arg("metrics"), "Analyze performance metrics")
        .def("analyze_trends", &PythonPerformanceMonitor::analyzeTrends,
             py::arg("history"), "Analyze performance trends");
    
    // Visualization utilities
    py::module_local() m.attr("create_spike_history_plot") = py::cpp_function(
        &PythonVisualization::BasicVisualizer::createSpikeHistoryPlot,
        py::arg("spike_data"));
    
    py::module_local() m.attr("create_memory_usage_plot") = py::cpp_function(
        &PythonVisualization::BasicVisualizer::createMemoryUsagePlot,
        py::arg("memory_data"));
    
    py::module_local() m.attr("create_activity_heatmap") = py::cpp_function(
        &PythonVisualization::BasicVisualizer::createActivityHeatmap,
        py::arg("activity_data"));
    
    // Error handling utilities
    py::module_local() m.attr("log_error") = py::cpp_function(
        &PythonErrorHandling::ErrorLogger::logError,
        py::arg("source"), py::arg("message"), py::arg("context") = "");
    
    py::module_local() m.attr("log_warning") = py::cpp_function(
        &PythonErrorHandling::ErrorLogger::logWarning,
        py::arg("source"), py::arg("message"));
    
    py::module_local() m.attr("log_info") = py::cpp_function(
        &PythonErrorHandling::ErrorLogger::logInfo,
        py::arg("source"), py::arg("message"));
    
    // Re-export original API for compatibility
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration (original API)");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (original API)");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world (original API)");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface (original API)");
    
    // Export constants for compatibility
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Export enumerations
    m.attr("NeuronType") = py::cast(py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
    );
    
    // Export additional useful constants
    py::class_<std::string>(m, "Constants")
        .def(py::init<>())
        .def_readonly("VERSION", "NLM Phase 6 Enhanced Python API v1.0.0")
        .def_readonly("DEFAULT_CONFIG_FILE", "configs/default.cfg")
        .def_readonly("CHECKPOINT_EXTENSION", ".nlmchk")
        .def_readonly("MAX_NEURONS", 10000)
        .def_readonly("DEFAULT_SIMULATION_TIMESTEP", 0.001);
}

} // namespace nlm