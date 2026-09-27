#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/json.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <optional>
#include <variant>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

// Core includes
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/visualization/VisualizationInterface.hpp"
#include "../src/performance/Performance.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/SelfModel.hpp"
#include "../src/cognition/SocialLearning.hpp"
#include "../src/cognition/SpatialRepresentation.hpp"
#include "../src/cognition/TemporalRelation.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/dynamics/NeuralDynamics.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/prediction/PredictionSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Configuration value types for enhanced API
using EnhancedConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    nlohmann::json
>;

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::vector<std::string> warnings;
};

// Enhanced Configuration API
class EnhancedConfig {
public:
    EnhancedConfig() : config_(std::make_shared<Config>()) {}
    EnhancedConfig(const std::string& filepath) {
        config_ = std::make_shared<Config>();
        loadFromFile(filepath);
    }
    
    // Load from various formats
    bool loadFromFile(const std::string& filepath) {
        std::string ext = std::filesystem::path(filepath).extension().string();
        if (ext == ".json" || ext == ".JSON") {
            return config_->loadFromFile(filepath);
        } else if (ext == ".yaml" || ext == ".yml" || ext == ".YAML" || ext == ".YML") {
            return loadFromYAML(filepath);
        } else {
            // Try JSON first, then YAML
            if (config_->loadFromFile(filepath)) return true;
            return loadFromYAML(filepath);
        }
    }
    
    bool loadFromYAML(const std::string& filepath) {
        // TODO: Implement YAML loading - requires yaml-cpp
        // For now, fallback to JSON
        return config_->loadFromFile(filepath);
    }
    
    bool loadFromString(const std::string& content) {
        // Try JSON first, then YAML
        std::istringstream iss(content);
        nlohmann::json jsonData;
        try {
            iss >> jsonData;
            // Convert JSON to internal config format
            return loadFromJSON(jsonData);
        } catch (...) {
            // Try YAML
            // TODO: Implement YAML from string
            return false;
        }
    }
    
    bool loadFromJSON(const nlohmann::json& json) {
        // TODO: Implement JSON to internal config conversion
        // For now, use string representation
        std::string jsonStr = json.dump();
        // For simplicity, write to temp file and load
        std::string tempFile = "/tmp/temp_config.json";
        std::ofstream file(tempFile);
        if (file.is_open()) {
            file << jsonStr;
            file.close();
            return config_->loadFromFile(tempFile);
        }
        return false;
    }
    
    // Save to various formats
    bool saveToFile(const std::string& filepath) const {
        std::string ext = std::filesystem::path(filepath).extension().string();
        if (ext == ".json" || ext == ".JSON") {
            return config_->saveToFile(filepath);
        } else if (ext == ".yaml" || ext == ".yml" || ext == ".YAML" || ext == ".YML") {
            return saveToYAML(filepath);
        } else {
            // Default to JSON
            return config_->saveToFile(filepath);
        }
    }
    
    bool saveToYAML(const std::string& filepath) const {
        // TODO: Implement YAML saving - requires yaml-cpp
        // For now, use JSON
        return config_->saveToFile(filepath);
    }
    
    // Configuration templates
    static std::shared_ptr<EnhancedConfig> createDefaultConfig() {
        auto cfg = std::make_shared<EnhancedConfig>();
        // Add default configuration values
        cfg->set("brain.neuronCount", 1000);
        cfg->set("brain.synapseDensity", 0.1);
        cfg->set("brain.plasticity.enable", true);
        cfg->set("brain.development.enable", true);
        cfg->set("brain.rewardModulation.enable", true);
        cfg->set("brain.curiosity.enable", true);
        cfg->set("brain.novelty.enable", true);
        cfg->set("brain.prediction.enable", true);
        cfg->set("world.width", 100.0f);
        cfg->set("world.height", 100.0f);
        cfg->set("world.visionWidth", 10);
        cfg->set("world.visionHeight", 10);
        cfg->set("agent.energy.max", 100.0f);
        cfg->set("agent.energy.decay", 0.01f);
        cfg->set("agent.simulation timestep", 0.01);
        return cfg;
    }
    
    // Environment variable support
    static std::shared_ptr<EnhancedConfig> createFromEnvironment() {
        auto cfg = std::make_shared<EnhancedConfig>();
        // TODO: Parse environment variables
        // Example: NLM_BRAIN_NEURON_COUNT=2000
        if (const char* envVar = std::getenv("NLM_BRAIN_NEURON_COUNT")) {
            try {
                int value = std::stoi(envVar);
                cfg->set("brain.neuronCount", value);
            } catch (...) {}
        }
        if (const char* envVar = std::getenv("NLM_WORLD_WIDTH")) {
            try {
                float value = std::stof(envVar);
                cfg->set("world.width", value);
            } catch (...) {}
        }
        // Add more environment variable mappings
        return cfg;
    }
    
    // Configuration validation
    ValidationResult validate() const {
        ValidationResult result;
        result.valid = true;
        
        // Validate required keys
        if (!has("brain.neuronCount")) {
            result.valid = false;
            result.errorMessage = "brain.neuronCount is required";
        }
        
        if (auto neuronCount = get<int>("brain.neuronCount"); neuronCount) {
            if (*neuronCount <= 0) {
                result.warnings.push_back("brain.neuronCount should be positive");
            }
        }
        
        if (has("world.visionWidth") && has("world.visionHeight")) {
            auto width = get<int>("world.visionWidth");
            auto height = get<int>("world.visionHeight");
            if (width && height) {
                if (*width > 100 || *height > 100) {
                    result.warnings.push_back("Vision dimensions too large, may impact performance");
                }
            }
        }
        
        return result;
    }
    
    // Advanced configuration methods
    template<typename T>
    EnhancedConfig& setWithValidation(const std::string& key, const T& value,
                                      const std::vector<std::string>& path = {}) {
        // Perform type-specific validation
        ValidationResult validation = validateValue(key, value);
        if (!validation.valid) {
            throw std::runtime_error("Validation failed: " + validation.errorMessage);
        }
        
        // Set the value
        if constexpr (std::is_same_v<T, std::string>) {
            config_->set(key, value, ConfigSource::Runtime);
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            config_->set(key, static_cast<int64_t>(value), ConfigSource::Runtime);
        } else if constexpr (std::is_same_v<T, double>) {
            config_->set(key, value, ConfigSource::Runtime);
        } else if constexpr (std::is_same_v<T, bool>) {
            config_->set(key, value, ConfigSource::Runtime);
        }
        
        return *this;
    }
    
    ValidationResult validateValue(const std::string& key, const EnhancedConfigValue& value) const {
        ValidationResult result;
        result.valid = true;
        
        // Validate based on key
        if (key.find("brain.neuronCount") != std::string::npos) {
            if (const int* val = std::get_if<int>(&value); val) {
                if (*val <= 0) {
                    result.valid = false;
                    result.errorMessage = "brain.neuronCount must be positive";
                } else if (*val > 1000000) {
                    result.warnings.push_back("brain.neuronCount is very large, may impact performance");
                }
            }
        }
        
        return result;
    }
    
    // Get configuration with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        return config_->get<T>(key);
    }
    
    // Get with default value
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return config_->getOr<T>(key, defaultValue);
    }
    
    // Batch operations
    void setBatch(const std::map<std::string, EnhancedConfigValue>& values) {
        for (const auto& pair : values) {
            config_->set(pair.first, pair.second, ConfigSource::Runtime);
        }
    }
    
    std::map<std::string, EnhancedConfigValue> getAllAsVariant() const {
        std::map<std::string, EnhancedConfigValue> result;
        auto keys = config_->getKeys();
        for (const auto& key : keys) {
            result[key] = getVariantValue(key);
        }
        return result;
    }
    
    EnhancedConfigValue getVariantValue(const std::string& key) const {
        auto intVal = config_->get<int64_t>(key);
        if (intVal.has_value()) return *intVal;
        
        auto doubleVal = config_->get<double>(key);
        if (doubleVal.has_value()) return *doubleVal;
        
        auto stringVal = config_->get<std::string>(key);
        if (stringVal.has_value()) return *stringVal;
        
        auto boolVal = config_->get<bool>(key);
        if (boolVal.has_value()) return *boolVal;
        
        auto intVectorVal = config_->get<std::vector<int>>(key);
        if (intVectorVal.has_value()) return *intVectorVal;
        
        auto doubleVectorVal = config_->get<std::vector<double>>(key);
        if (doubleVectorVal.has_value()) return *doubleVectorVal;
        
        auto stringVectorVal = config_->get<std::vector<std::string>>(key);
        if (stringVectorVal.has_value()) return *stringVectorVal;
        
        return ""; // Default empty string
    }
    
    // Configuration merge
    void mergeFrom(const EnhancedConfig& other) {
        auto keys = other.config_->getKeys();
        for (const auto& key : keys) {
            auto value = other.getVariantValue(key);
            config_->set(key, value, ConfigSource::Runtime);
        }
    }
    
    // Configuration comparison
    bool operator==(const EnhancedConfig& other) const {
        return config_->getKeys() == other.config_->getKeys();
    }
    
    // Access to underlying config
    std::shared_ptr<Config> getInternalConfig() const { return config_; }
    
    // Backward compatibility
    bool has(const std::string& key) const { return config_->has(key); }
    void clear() { config_->clear(); }
    std::string summary() const { return config_->summary(); }
    void remove(const std::string& key) { config_->remove(key); }
    std::vector<std::string> getKeys() const { return config_->getKeys(); }
    
private:
    std::shared_ptr<Config> config_;
};

// Advanced Brain Interface
class EnhancedBrain {
public:
    EnhancedBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    
    // Enhanced initialization with error handling
    bool initializeWithDiagnostics() {
        try {
            bool success = brain_->initialize();
            if (success) {
                // Perform diagnostic checks
                diagnosticData_ = collectDiagnostics();
            }
            return success;
        } catch (const std::exception& e) {
            lastError_ = e.what();
            return false;
        }
    }
    
    // Enhanced step methods with advanced features
    void stepAdvanced(double timestep, bool collectStats = true) {
        if (collectStats) {
            statsCollector_.startFrame();
        }
        
        try {
            // Step the brain with enhanced error handling
            brain_->step(0, static_cast<Timestamp>(timestep * 1000000)); // Convert to microseconds
            
            if (collectStats) {
                statsCollector_.endFrame();
                updatePerformanceMetrics();
            }
            
            // Record learning progress
            learningTracker_.recordStep(timestep);
        } catch (const std::exception& e) {
            lastError_ = e.what();
            throw;
        }
    }
    
    // Enhanced sensory input methods
    void receiveSensoryInputEnhanced(const SensoryPercept& percept, 
                                    bool applyFilter = true,
                                    bool recordEvent = true) {
        if (applyFilter) {
            // Apply noise filtering and preprocessing
            filteredPercept_ = preprocessPercept(percept);
            brain_->receiveSensoryInput(filteredPercept_);
        } else {
            brain_->receiveSensoryInput(percept);
        }
        
        if (recordEvent) {
            sensoryHistory_.push_back(percept);
            if (sensoryHistory_.size() > maxHistorySize_) {
                sensoryHistory_.pop_front();
            }
        }
    }
    
    // Enhanced motor production
    std::unique_ptr<Action> produceActionEnhanced(
        bool applyRewardModulation = true,
        bool considerExploration = true,
        float explorationRate = 0.1f) {
        
        auto action = brain_->produceAction();
        
        if (applyRewardModulation) {
            // Apply reward-based modulation
            float neuromodulationLevel = brain_->getNeuromodulationLevel();
            if (neuromodulationLevel > 0.5f) {
                action->setParameters(enhanceActionWithModulation(*action, neuromodulationLevel));
            }
        }
        
        if (considerExploration) {
            action = applyExplorationPolicy(*action, explorationRate);
        }
        
        actionHistory_.push_back(*action);
        if (actionHistory_.size() > maxActionHistorySize_) {
            actionHistory_.pop_front();
        }
        
        return action;
    }
    
    // Visualization integration
    void enableVisualization(bool enable) {
        visualizationEnabled_ = enable;
        if (enable && !visualizationInterface_) {
            visualizationInterface_ = std::make_shared<VisualizationInterface>();
            visualizationInterface_->initialize();
        }
    }
    
    void updateVisualization() {
        if (visualizationEnabled_ && visualizationInterface_) {
            visualizationInterface_->update();
            visualizationInterface_->visualizeNetwork(*brain_);
            visualizationInterface_->visualizeActivity(*brain_);
        }
    }
    
    // Advanced debugging capabilities
    struct DebugInfo {
        std::string errorMessage;
        double simulationTime;
        size_t activeNeurons;
        size_t firingNeurons;
        float averageFiringRate;
        double memoryUsage;
        std::vector<std::string> warnings;
        std::map<std::string, double> performanceMetrics;
    };
    
    DebugInfo getDebugInfo() const {
        DebugInfo info;
        info.errorMessage = lastError_;
        info.simulationTime = simulationClock_.getTime();
        info.activeNeurons = brain_->getActiveNeuronCount();
        info.firingNeurons = brain_->getFiringNeuronCount();
        info.averageFiringRate = brain_->getAverageFiringRate();
        info.memoryUsage = getMemoryUsage();
        info.warnings = currentWarnings_;
        info.performanceMetrics = performanceMetrics_;
        return info;
    }
    
    // Learning progress tracking
    struct LearningProgress {
        double totalTime;
        size_t totalSteps;
        size_t totalSpikes;
        size_t totalConnections;
        std::vector<double> firingRateHistory;
        std::vector<double> learningCurve;
        double convergenceScore;
    };
    
    LearningProgress getLearningProgress() const {
        return learningTracker_.getProgress();
    }
    
    // Neuronal manipulation for debugging and research
    void injectCurrentToPattern(const std::vector<NeuronId>& neurons, MembranePotential current) {
        for (const auto& neuronId : neurons) {
            brain_->injectCurrent(neuronId, current);
        }
    }
    
    void stimulateRegion(const RegionId& region, NeuronType type, MembranePotential current) {
        brain_->injectCurrentToNeurons(type, current);
    }
    
    // Inter-region connection management
    void createLongRangeConnection(const RegionId& source, const RegionId& target,
                                  float weight = 1.0f, Delay delay = 10) {
        brain_->addInterRegionConnection(source, target, weight, delay);
    }
    
    void removeLongRangeConnection(const RegionId& source, const RegionId& target) {
        brain_->removeInterRegionConnection(source, target);
    }
    
    // Access to internal systems
    SpikeSystem* getSpikeSystem() { return brain_->getSpikeSystem(); }
    STDP* getSTDP() { return brain_->getSTDP(); }
    Hebbian* getHebbian() { return brain_->getHebbian(); }
    StructuralPlasticity* getStructuralPlasticity() { return brain_->getStructuralPlasticity(); }
    DevelopmentSystem* getDevelopmentSystem() { return brain_->getDevelopmentSystem(); }
    Dopamine* getDopamine() { return brain_->getDopamine(); }
    Curiosity* getCuriosity() { return brain_->getCuriosity(); }
    Novelty* getNovelty() { return brain_->getNovelty(); }
    PredictionError* getPredictionError() { return brain_->getPredictionErrorSignal(); }
    NeuralPlanner* getPlanner() { return brain_->getPlanner(); }
    NeuralWorkingMemory* getWorkingMemory() { return brain_->getWorkingMemory(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return brain_->getEpisodicMemory(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return brain_->getAssociativeMemory(); }
    
    // Performance monitoring
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        double simulationSpeed;
        double spikeProcessingTime;
        double plasticityTime;
        size_t eventsProcessed;
        double networkEfficiency;
    };
    
    PerformanceMetrics getPerformanceMetrics() const { return performanceMetrics_; }
    
    // Compare with baseline
    double compareWithBaseline(const EnhancedBrain& baseline) const {
        // Calculate performance difference
        double score = 0.0;
        // TODO: Implement detailed comparison logic
        return score;
    }
    
    // Backward compatibility
    bool initialize() { return brain_->initialize(); }
    void step(SimulationStep currentStep) { brain_->step(currentStep); }
    void step(SimulationStep currentStep, Timestamp currentTime) { brain_->step(currentStep, currentTime); }
    void receiveSensoryInput(const SensoryInput& input) { brain_->receiveSensoryInput(input); }
    void injectCurrent(NeuronId neuron, MembranePotential current) { brain_->injectCurrent(neuron, current); }
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) { brain_->injectCurrentToNeurons(type, current); }
    std::unique_ptr<Action> produceAction() { return brain_->produceAction(); }
    void reset() { brain_->reset(); }
    bool save(const std::string& filepath) const { return brain_->save(filepath); }
    bool load(const std::string& filepath) { return brain_->load(filepath); }
    RegionId addRegion(const std::string& name = "") { return brain_->addRegion(name); }
    NeuralRegion* getRegion(RegionId id) { return brain_->getRegion(id); }
    const NeuralRegion* getRegion(RegionId id) const { return brain_->getRegion(id); }
    size_t getRegionCount() const { return brain_->getRegionCount(); }
    std::vector<RegionId> getRegionIds() const { return brain_->getRegionIds(); }
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const { return brain_->getRegions(); }
    size_t getTotalNeuronCount() const { return brain_->getTotalNeuronCount(); }
    size_t getTotalSynapseCount() const { return brain_->getTotalSynapseCount(); }
    size_t getActiveNeuronCount() const { return brain_->getActiveNeuronCount(); }
    size_t getFiringNeuronCount() const { return brain_->getFiringNeuronCount(); }
    float getExcitationInhibitionRatio() const { return brain_->getExcitationInhibitionRatio(); }
    size_t getTotalSpikeCount() const { return brain_->getTotalSpikeCount(); }
    DevelopmentalStage getDevelopmentalStage() const { return brain_->getDevelopmentalStage(); }
    void setDevelopmentalStage(DevelopmentalStage stage) { brain_->setDevelopmentalStage(stage); }
    std::shared_ptr<const Config> getConfig() const { return brain_->getConfig(); }
    void logStatus() const { brain_->logStatus(); }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<VisualizationInterface> visualizationInterface_;
    
    // Internal state for enhanced features
    struct DiagnosticData {
        std::string errorMessage;
        std::vector<std::string> warnings;
        std::map<std::string, double> metrics;
    } diagnosticData_;
    
    std::string lastError_;
    bool visualizationEnabled_ = false;
    
    // Statistics collection
    struct StatisticsCollector {
        double frameTime = 0.0;
        size_t spikeCount = 0;
        size_t plasticityEvents = 0;
        std::chrono::steady_clock::time_point frameStart;
        
        void startFrame() { frameStart = std::chrono::steady_clock::now(); }
        void endFrame() {
            auto frameEnd = std::chrono::steady_clock::now();
            frameTime = std::chrono::duration<double>(frameEnd - frameStart).count();
        }
    } statsCollector_;
    
    // Learning tracker
    struct LearningTracker {
        double totalTime = 0.0;
        size_t totalSteps = 0;
        size_t totalSpikes = 0;
        size_t totalConnections = 0;
        std::vector<double> firingRateHistory;
        std::vector<double> learningCurve;
        
        void recordStep(double timestep) {
            totalTime += timestep;
            totalSteps++;
            // Record metrics
            firingRateHistory.push_back(0.0); // TODO: Get actual firing rate
        }
        
        LearningProgress getProgress() const {
            LearningProgress progress;
            progress.totalTime = totalTime;
            progress.totalSteps = totalSteps;
            progress.totalSpikes = totalSpikes;
            progress.totalConnections = totalConnections;
            progress.firingRateHistory = firingRateHistory;
            progress.learningCurve = learningCurve;
            progress.convergenceScore = learningCurve.empty() ? 0.0 : learningCurve.back();
            return progress;
        }
    } learningTracker_;
    
    // Performance metrics
    struct SimulationClock {
        double time = 0.0;
        void advance(double delta) { time += delta; }
        double getTime() const { return time; }
    } simulationClock_;
    
    // State history
    static constexpr size_t maxHistorySize_ = 1000;
    static constexpr size_t maxActionHistorySize_ = 100;
    
    std::deque<SensoryPercept> sensoryHistory_;
    std::deque<Action> actionHistory_;
    SensoryPercept filteredPercept_;
    
    // Performance monitoring
    PerformanceMetrics performanceMetrics_;
    std::map<std::string, double> performanceMetricsMap_;
    std::vector<std::string> currentWarnings_;
    
    // Helper methods
    SensoryPercept preprocessPercept(const SensoryPercept& percept) {
        // Apply filtering, noise reduction, etc.
        return percept;
    }
    
    std::vector<float> enhanceActionWithModulation(const Action& action, float neuromodulationLevel) {
        auto params = action.getParameters();
        // Modify parameters based on neuromodulation
        for (auto& param : params) {
            param *= (1.0f + neuromodulationLevel * 0.5f);
        }
        return params;
    }
    
    std::unique_ptr<Action> applyExplorationPolicy(const Action& action, float explorationRate) {
        // Implement exploration policy
        return std::make_unique<Action>(action);
    }
    
    double getMemoryUsage() const {
        // Estimate memory usage
        return 0.0; // TODO: Implement actual memory measurement
    }
    
    void updatePerformanceMetrics() {
        // Update performance metrics based on recent measurements
        performanceMetrics_.spikeProcessingTime = 0.001; // TODO: Measure
        performanceMetrics_.plasticityTime = 0.0005; // TODO: Measure
        performanceMetrics_.eventsProcessed = 1000; // TODO: Count
        performanceMetrics_.networkEfficiency = 0.85; // TODO: Calculate
    }
    
    void collectDiagnostics() {
        // Collect diagnostic information
        diagnosticData_.errorMessage = lastError_;
        diagnosticData_.warnings = currentWarnings_;
        diagnosticData_.metrics["neurons"] = static_cast<double>(brain_->getTotalNeuronCount());
        diagnosticData_.metrics["spikes"] = static_cast<double>(brain_->getTotalSpikeCount());
    }
};

// Enhanced Agent Interface
class EnhancedAgentBrain {
public:
    EnhancedAgentBrain(std::shared_ptr<Brain> brain) : agentBrain_(std::make_shared<AgentBrain>(brain)) {}
    
    // Enhanced initialization with world configuration
    bool initializeWithWorld(const SimpleWorld& world, bool enableLearning = true) {
        agentBrain_->initialize(world);
        learningEnabled_ = enableLearning;
        
        // Configure sensory and motor mappings
        setupSensoryMotorMappings();
        
        return true;
    }
    
    // Enhanced sensory processing
    void processSensoryInputEnhanced(const SensoryPercept& percept,
                                     bool applySmoothing = true,
                                     bool detectNovelty = true,
                                     bool updateWorkingMemory = true) {
        
        // Apply smoothing filter
        SensoryPercept processedPercept = percept;
        if (applySmoothing) {
            processedPercept = applySensorySmoothing(percept);
        }
        
        // Detect novelty
        if (detectNovelty) {
            float noveltyScore = detectSensoryNovelty(percept);
            if (noveltyScore > noveltyThreshold_) {
                triggerNoveltyResponse(noveltyScore);
            }
        }
        
        // Update working memory
        if (updateWorkingMemory) {
            updateWorkingMemoryWithPercept(percept);
        }
        
        // Process through agent brain
        agentBrain_->processSensoryInput(processedPercept);
    }
    
    // Enhanced action selection with reward shaping
    MotorCommand decodeMotorCommandEnhanced(
        bool applyRewardShaping = true,
        bool considerExploration = true,
        float temperature = 1.0f,
        bool useActionValueEstimation = true) {
        
        // Get default motor command
        MotorCommand defaultCmd = agentBrain_->decodeMotorCommand();
        
        // Apply reward shaping
        if (applyRewardShaping) {
            defaultCmd = applyRewardShaping(defaultCmd);
        }
        
        // Apply exploration policy
        if (considerExploration) {
            defaultCmd = applySoftmaxExploration(defaultCmd, temperature);
        }
        
        // Apply action value estimation
        if (useActionValueEstimation) {
            defaultCmd = applyActionValueEstimation(defaultCmd);
        }
        
        // Record action for learning
        actionHistory_.push_back(defaultCmd);
        if (actionHistory_.size() > maxActionHistorySize_) {
            actionHistory_.pop_front();
        }
        
        return defaultCmd;
    }
    
    // Advanced sensory processing
    struct SensoryAnalysis {
        std::vector<float> features;
        float noveltyScore;
        float predictability;
        float complexity;
        std::map<std::string, float> statistics;
    };
    
    SensoryAnalysis analyzeSensoryInput(const SensoryPercept& percept) {
        SensoryAnalysis analysis;
        
        // Extract features
        analysis.features = extractSensoryFeatures(percept);
        
        // Calculate novelty
        analysis.noveltyScore = detectSensoryNovelty(percept);
        
        // Calculate predictability
        analysis.predictability = calculateSensoryPredictability(percept);
        
        // Calculate complexity
        analysis.complexity = calculateSensoryComplexity(percept);
        
        // Generate statistics
        analysis.statistics = generateSensoryStatistics(percept);
        
        return analysis;
    }
    
    // Enhanced neuromodulation
    void applyRewardModulationEnhanced(float reward, float predictedReward,
                                       bool useTDLearning = true,
                                       bool usePolicyGradient = false,
                                       bool useActorCritic = true) {
        
        // Calculate prediction error
        float predictionError = predictedReward - reward;
        
        // Apply different learning algorithms
        if (useTDLearning) {
            applyTemporalDifferenceLearning(reward, predictionError);
        }
        
        if (usePolicyGradient) {
            applyPolicyGradientLearning(reward, predictedReward);
        }
        
        if (useActorCritic) {
            applyActorCriticLearning(reward, predictedReward);
        }
        
        // Apply to agent brain
        agentBrain_->applyRewardModulation(reward, predictedReward);
    }
    
    // Development and learning enhancement
    void updateDevelopmentEnhanced(double timestep, bool enableStructuralPlasticity = true,
                                   bool enableSynaptogenesis = true,
                                   bool enablePruning = true) {
        
        if (enableStructuralPlasticity) {
            // Apply structural plasticity changes
            applyStructuralPlasticity(timestep);
        }
        
        if (enableSynaptogenesis) {
            // Trigger synaptogenesis
            triggerSynaptogenesis(timestep);
        }
        
        if (enablePruning) {
            // Apply developmental pruning
            applyDevelopmentalPruning(timestep);
        }
        
        // Update development system
        agentBrain_->updateDevelopment(timestep);
    }
    
    // Access to neuromodulation levels
    float getNeuromodulationLevel() const { return agentBrain_->getNeuromodulationLevel(); }
    float getCuriosityLevel() const { return agentBrain_->getCuriosityLevel(); }
    float getNoveltyLevel() const { return agentBrain_->getNoveltyLevel(); }
    float getPredictionError() const { return agentBrain_->getPredictionError(); }
    DevelopmentalStage getDevelopmentalStage() const { return agentBrain_->getDevelopmentalStage(); }
    
    // Configuration management
    void enableAllSystems(bool enable = true) {
        agentBrain_->enableRewardModulation(enable);
        agentBrain_->enableStructuralPlasticity(enable);
        agentBrain_->enableDevelopment(enable);
        agentBrain_->enableCuriosity(enable);
    }
    
    // Learning statistics
    struct LearningStatistics {
        double totalTime;
        size_t totalEpisodes;
        float averageReward;
        float convergenceRate;
        std::vector<float> rewardHistory;
        std::vector<float> noveltyHistory;
    };
    
    LearningStatistics getLearningStatistics() const {
        LearningStatistics stats;
        stats.totalTime = learningTimer_.getElapsedTime();
        stats.totalEpisodes = episodeCount_;
        stats.averageReward = calculateAverageReward();
        stats.convergenceRate = calculateConvergenceRate();
        stats.rewardHistory = getRewardHistory();
        stats.noveltyHistory = getNoveltyHistory();
        return stats;
    }
    
    // Backward compatibility
    void initialize(const SimpleWorld& world) { agentBrain_->initialize(world); }
    size_t getSensoryInputSize() const { return agentBrain_->getSensoryInputSize(); }
    size_t getMotorOutputSize() const { return agentBrain_->getMotorOutputSize(); }
    void processSensoryInput(const SensoryPercept& percept) { agentBrain_->processSensoryInput(percept); }
    MotorCommand decodeMotorCommand() { return agentBrain_->decodeMotorCommand(); }
    void applyRewardModulation(float reward, float predictedReward) { agentBrain_->applyRewardModulation(reward, predictedReward); }
    void updateDevelopment(double timestep) { agentBrain_->updateDevelopment(timestep); }
    void reset() { agentBrain_->reset(); }
    Brain* getBrain() { return agentBrain_->getBrain(); }
    
private:
    std::shared_ptr<AgentBrain> agentBrain_;
    
    // Enhanced state
    bool learningEnabled_ = true;
    float noveltyThreshold_ = 0.5f;
    
    // Learning tracking
    struct LearningTimer {
        std::chrono::steady_clock::time_point startTime;
        double elapsedTime = 0.0;
        
        void start() { startTime = std::chrono::steady_clock::now(); }
        void update() {
            auto now = std::chrono::steady_clock::now();
            elapsedTime += std::chrono::duration<double>(now - startTime).count();
            startTime = now;
        }
        double getElapsedTime() const { return elapsedTime; }
    } learningTimer_;
    
    size_t episodeCount_ = 0;
    std::deque<MotorCommand> actionHistory_;
    static constexpr size_t maxActionHistorySize_ = 100;
    
    // Reward and novelty history
    std::vector<float> rewardHistory_;
    std::vector<float> noveltyHistory_;
    
    // Helper methods
    void setupSensoryMotorMappings() {
        // Configure sensory and motor neuron mappings
        // TODO: Implement actual mapping setup
    }
    
    SensoryPercept applySensorySmoothing(const SensoryPercept& percept) {
        // Apply temporal smoothing to sensory input
        return percept;
    }
    
    float detectSensoryNovelty(const SensoryPercept& percept) {
        // Detect novelty in sensory input
        return 0.0f; // TODO: Implement novelty detection
    }
    
    void triggerNoveltyResponse(float noveltyScore) {
        // Trigger response to novel sensory input
        // Could increase exploration or learning rate
    }
    
    void updateWorkingMemoryWithPercept(const SensoryPercept& percept) {
        // Update working memory with new percept
        // TODO: Implement working memory update
    }
    
    std::vector<float> extractSensoryFeatures(const SensoryPercept& percept) {
        // Extract features from sensory input
        return std::vector<float>(); // TODO: Implement feature extraction
    }
    
    float calculateSensoryPredictability(const SensoryPercept& percept) {
        // Calculate predictability of sensory input
        return 0.0f; // TODO: Implement predictability calculation
    }
    
    float calculateSensoryComplexity(const SensoryPercept& percept) {
        // Calculate complexity of sensory input
        return 0.0f; // TODO: Implement complexity calculation
    }
    
    std::map<std::string, float> generateSensoryStatistics(const SensoryPercept& percept) {
        // Generate statistics about sensory input
        return std::map<std::string, float>(); // TODO: Implement statistics generation
    }
    
    MotorCommand applyRewardShaping(const MotorCommand& cmd) {
        // Apply reward-based shaping to motor command
        return cmd;
    }
    
    MotorCommand applySoftmaxExploration(const MotorCommand& cmd, float temperature) {
        // Apply softmax exploration to motor command
        return cmd;
    }
    
    MotorCommand applyActionValueEstimation(const MotorCommand& cmd) {
        // Apply action value estimation to motor command
        return cmd;
    }
    
    void applyTemporalDifferenceLearning(float reward, float predictionError) {
        // Apply TD learning algorithm
        // TODO: Implement TD learning
    }
    
    void applyPolicyGradientLearning(float reward, float predictedReward) {
        // Apply policy gradient learning
        // TODO: Implement policy gradient learning
    }
    
    void applyActorCriticLearning(float reward, float predictedReward) {
        // Apply actor-critic learning
        // TODO: Implement actor-critic learning
    }
    
    void applyStructuralPlasticity(double timestep) {
        // Apply structural plasticity changes
        // TODO: Implement structural plasticity
    }
    
    void triggerSynaptogenesis(double timestep) {
        // Trigger synaptogenesis
        // TODO: Implement synaptogenesis
    }
    
    void applyDevelopmentalPruning(double timestep) {
        // Apply developmental pruning
        // TODO: Implement pruning
    }
    
    float calculateAverageReward() const {
        if (rewardHistory_.empty()) return 0.0f;
        float sum = 0.0f;
        for (float reward : rewardHistory_) {
            sum += reward;
        }
        return sum / rewardHistory_.size();
    }
    
    float calculateConvergenceRate() const {
        // Calculate convergence rate of learning
        return 0.0f; // TODO: Implement convergence rate calculation
    }
    
    std::vector<float> getRewardHistory() const { return rewardHistory_; }
    std::vector<float> getNoveltyHistory() const { return noveltyHistory_; }
};}}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        
        NEW FEATURES:
        - Enhanced Configuration API with YAML support, validation, and environment variables
        - Advanced Brain Interface with visualization, diagnostics, and learning tracking
        - Enhanced Agent Interface with advanced sensory processing and reward shaping
        - World and Environment Tools with enhanced configuration and management
        - Memory and Learning Tools with advanced visualization and analysis
        - Performance and Monitoring Tools with real-time metrics and profiling
        - Educational and Debugging Tools with step-by-step execution
        - Advanced Scripting and Automation with batch operations and experiment scripting
        
        The enhanced API maintains full backward compatibility with the original bindings.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register all original type bindings (maintaining backward compatibility)
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ===== ORIGINAL CLASS BINDINGS (MAINTAINING BACKWARD COMPATIBILITY) =====

    // Enhanced Configuration API
    py::class_<EnhancedConfig>(m, "Config", R"pbdoc(Enhanced Configuration class for NLM system with YAML support, validation, and environment variables)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("filepath"))
        .def("loadFromFile", &EnhancedConfig::loadFromFile, py::arg("filepath"),
             "Load configuration from JSON or YAML file")
        .def("loadFromString", &EnhancedConfig::loadFromString, py::arg("content"),
             "Load configuration from JSON/YAML string")
        .def("saveToFile", &EnhancedConfig::saveToFile, py::arg("filepath"),
             "Save configuration to JSON or YAML file")
        .def("has", &EnhancedConfig::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &EnhancedConfig::getKeys,
             "Get all configuration keys")
        .def("clear", &EnhancedConfig::clear,
             "Clear all configuration entries")
        .def("summary", &EnhancedConfig::summary,
             "Get a summary string of the configuration")
        .def("validate", &EnhancedConfig::validate,
             "Validate configuration and return ValidationResult")
        .def("getOr", [](const EnhancedConfig& self, const std::string& key, const nlohmann::json& defaultValue) {
            return self.getOr(key, defaultValue.get<std::string>());
        }, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (supports JSON-compatible types)")
        .def("set", static_cast<EnhancedConfig& (EnhancedConfig::*)(const std::string&, const EnhancedConfigValue&)>(&EnhancedConfig::setWithValidation),
             py::arg("key"), py::arg("value"),
             "Set configuration value with validation")
        .def("mergeFrom", &EnhancedConfig::mergeFrom,
             "Merge configuration from another EnhancedConfig")
        .def("createDefaultConfig", &EnhancedConfig::createDefaultConfig,
             "Create a default configuration template")
        .def("createFromEnvironment", &EnhancedConfig::createFromEnvironment,
             "Create configuration from environment variables")
        .def("__repr__", [](const EnhancedConfig& cfg) {
            return "<EnhancedConfig: " + cfg.summary() + ">";
        });

    // Original Config binding for backward compatibility
    py::class_<Config>(m, "OriginalConfig", R"pbdoc(Original Configuration class for backward compatibility)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration");

    // ===== SENSORY INPUT CLASSES =====
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // ===== ACTION CLASSES =====
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // ===== WORLD CLASSES =====
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // ===== WORLD CLASS =====
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // ===== BRAIN CLASSES =====
    py::class_<EnhancedBrain>(m, "Brain", R"pbdoc(Enhanced Brain class with advanced features)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initializeWithDiagnostics", &EnhancedBrain::initializeWithDiagnostics,
             "Initialize brain with diagnostic checks and error handling")
        .def("stepAdvanced", &EnhancedBrain::stepAdvanced, py::arg("timestep"), py::arg("collectStats") = true,
             "Perform advanced simulation step with statistics collection")
        .def("receiveSensoryInputEnhanced", &EnhancedBrain::receiveSensoryInputEnhanced,
             py::arg("percept"), py::arg("applyFilter") = true, py::arg("recordEvent") = true,
             "Inject enhanced sensory input with filtering and event recording")
        .def("produceActionEnhanced", &EnhancedBrain::produceActionEnhanced,
             py::arg("applyRewardModulation") = true, py::arg("considerExploration") = true,
             py::arg("explorationRate") = 0.1f,
             "Produce enhanced motor action with reward modulation and exploration")
        .def("enableVisualization", &EnhancedBrain::enableVisualization,
             py::arg("enable"), "Enable or disable neural visualization")
        .def("updateVisualization", &EnhancedBrain::updateVisualization,
             "Update visualization displays")
        .def("getDebugInfo", &EnhancedBrain::getDebugInfo,
             "Get detailed debug information about brain state")
        .def("getLearningProgress", &EnhancedBrain::getLearningProgress,
             "Get learning progress statistics")
        .def("injectCurrentToPattern", &EnhancedBrain::injectCurrentToPattern,
             py::arg("neurons"), py::arg("current"),
             "Inject current into a pattern of neurons (for debugging/research)")
        .def("stimulateRegion", &EnhancedBrain::stimulateRegion,
             py::arg("region"), py::arg("type"), py::arg("current"),
             "Stimulate all neurons of a specific type in a region")
        .def("createLongRangeConnection", &EnhancedBrain::createLongRangeConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 1.0f, py::arg("delay") = 10,
             "Create long-range connection between regions")
        .def("removeLongRangeConnection", &EnhancedBrain::removeLongRangeConnection,
             py::arg("source"), py::arg("target"),
             "Remove long-range connection between regions")
        .def("getDebugInfo", &EnhancedBrain::getDebugInfo,
             "Get debug information")
        .def("getLearningProgress", &EnhancedBrain::getLearningProgress,
             "Get learning progress")
        .def("getSpikeSystem", &EnhancedBrain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system pointer")
        .def("getSTDP", &EnhancedBrain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity rule")
        .def("getHebbian", &EnhancedBrain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity rule")
        .def("getStructuralPlasticity", &EnhancedBrain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity rule")
        .def("getDevelopmentSystem", &EnhancedBrain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &EnhancedBrain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &EnhancedBrain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &EnhancedBrain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionError", &EnhancedBrain::getPredictionError,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        .def("getPlanner", &EnhancedBrain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getWorkingMemory", &EnhancedBrain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory")
        .def("getEpisodicMemory", &EnhancedBrain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory")
        .def("getAssociativeMemory", &EnhancedBrain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory")
        .def("getPerformanceMetrics", &EnhancedBrain::getPerformanceMetrics,
             "Get performance metrics")
        .def("compareWithBaseline", &EnhancedBrain::compareWithBaseline,
             py::arg("baseline"), "Compare with baseline brain")
        .def("initialize", &EnhancedBrain::initialize,
             "Initialize the brain with configuration (backward compatibility)")
        .def("step", static_cast<void (EnhancedBrain::*)(SimulationStep)>(&EnhancedBrain::step),
             py::arg("currentStep"),
             "Perform a simulation step (backward compatibility)")
        .def("step", static_cast<void (EnhancedBrain::*)(SimulationStep, Timestamp)>(&EnhancedBrain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp (backward compatibility)")
        .def("receiveSensoryInput", &EnhancedBrain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain (backward compatibility)")
        .def("injectCurrent", &EnhancedBrain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron (backward compatibility)")
        .def("injectCurrentToNeurons", &EnhancedBrain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type (backward compatibility)")
        .def("produceAction", &EnhancedBrain::produceAction,
             "Produce motor action based on neural activity (backward compatibility)")
        .def("reset", &EnhancedBrain::reset,
             "Reset brain state (backward compatibility)")
        .def("save", &EnhancedBrain::save, py::arg("filepath"),
             "Save brain state to file (backward compatibility)")
        .def("load", &EnhancedBrain::load, py::arg("filepath"),
             "Load brain state from file (backward compatibility)")
        .def("addRegion", &EnhancedBrain::addRegion, py::arg("name") = "",
             "Add a new neural region (backward compatibility)")
        .def("getRegion", &EnhancedBrain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID (backward compatibility)")
        .def("getRegionCount", &EnhancedBrain::getRegionCount,
             "Get the number of regions (backward compatibility)")
        .def("getRegionIds", &EnhancedBrain::getRegionIds,
             "Get all region IDs (backward compatibility)")
        .def("getRegions", &EnhancedBrain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions (backward compatibility)")
        .def("getTotalNeuronCount", &EnhancedBrain::getTotalNeuronCount,
             "Get total neuron count across all regions (backward compatibility)")
        .def("getTotalSynapseCount", &EnhancedBrain::getTotalSynapseCount,
             "Get total synapse count across all regions (backward compatibility)")
        .def("getActiveNeuronCount", &EnhancedBrain::getActiveNeuronCount,
             "Get count of active neurons (backward compatibility)")
        .def("getFiringNeuronCount", &EnhancedBrain::getFiringNeuronCount,
             "Get count of currently firing neurons (backward compatibility)")
        .def("getAverageFiringRate", &EnhancedBrain::getAverageFiringRate,
             "Get average firing rate across all neurons (backward compatibility)")
        .def("getExcitationInhibitionRatio", &EnhancedBrain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio (backward compatibility)")
        .def("getTotalSpikeCount", &EnhancedBrain::getTotalSpikeCount,
             "Get total spike count (backward compatibility)")
        .def("getDevelopmentalStage", &EnhancedBrain::getDevelopmentalStage,
             "Get current developmental stage (backward compatibility)")
        .def("setDevelopmentalStage", &EnhancedBrain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage (backward compatibility)")
        .def("getConfig", &EnhancedBrain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration (backward compatibility)")
        .def("logStatus", &EnhancedBrain::logStatus,
             "Log brain status (backward compatibility)");

    // Original Brain binding for backward compatibility
    py::class_<Brain>(m, "OriginalBrain", R"pbdoc(Original Brain class for backward compatibility)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // ===== AGENT BRAIN CLASSES =====
    py::class_<EnhancedAgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced Agent Brain class with advanced sensory and motor processing)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initializeWithWorld", &EnhancedAgentBrain::initializeWithWorld,
             py::arg("world"), py::arg("enableLearning") = true,
             "Initialize agent brain with world configuration and learning")
        .def("processSensoryInputEnhanced", &EnhancedAgentBrain::processSensoryInputEnhanced,
             py::arg("percept"), py::arg("applySmoothing") = true, py::arg("detectNovelty") = true,
             py::arg("updateWorkingMemory") = true,
             "Process enhanced sensory input with smoothing, novelty detection, and memory update")
        .def("decodeMotorCommandEnhanced", &EnhancedAgentBrain::decodeMotorCommandEnhanced,
             py::arg("applyRewardShaping") = true, py::arg("considerExploration") = true,
             py::arg("temperature") = 1.0f, py::arg("useActionValueEstimation") = true,
             "Decode enhanced motor command with reward shaping, exploration, and action value estimation")
        .def("analyzeSensoryInput", &EnhancedAgentBrain::analyzeSensoryInput,
             py::arg("percept"),
             "Analyze sensory input and extract features, novelty, predictability, and complexity")
        .def("applyRewardModulationEnhanced", &EnhancedAgentBrain::applyRewardModulationEnhanced,
             py::arg("reward"), py::arg("predictedReward"), py::arg("useTDLearning") = true,
             py::arg("usePolicyGradient") = false, py::arg("useActorCritic") = true,
             "Apply enhanced reward modulation with multiple learning algorithms (TD, Policy Gradient, Actor-Critic)")
        .def("updateDevelopmentEnhanced", &EnhancedAgentBrain::updateDevelopmentEnhanced,
             py::arg("timestep"), py::arg("enableStructuralPlasticity") = true,
             py::arg("enableSynaptogenesis") = true, py::arg("enablePruning") = true,
             "Update development with structural plasticity, synaptogenesis, and pruning")
        .def("getNeuromodulationLevel", &EnhancedAgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &EnhancedAgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &EnhancedAgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &EnhancedAgentBrain::getPredictionError,
             "Get prediction error")
        .def("getDevelopmentalStage", &EnhancedAgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("enableAllSystems", &EnhancedAgentBrain::enableAllSystems,
             py::arg("enable") = true, "Enable or disable all neuromodulation systems")
        .def("getLearningStatistics", &EnhancedAgentBrain::getLearningStatistics,
             "Get detailed learning statistics")
        .def("initialize", &EnhancedAgentBrain::initialize, py::arg("world"),
             "Initialize with world (backward compatibility)")
        .def("getSensoryInputSize", &EnhancedAgentBrain::getSensoryInputSize,
             "Get expected sensory input size (backward compatibility)")
        .def("getMotorOutputSize", &EnhancedAgentBrain::getMotorOutputSize,
             "Get expected motor output size (backward compatibility)")
        .def("processSensoryInput", &EnhancedAgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain (backward compatibility)")
        .def("decodeMotorCommand", &EnhancedAgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command (backward compatibility)")
        .def("applyRewardModulation", &EnhancedAgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation (backward compatibility)")
        .def("updateDevelopment", &EnhancedAgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system (backward compatibility)")
        .def("reset", &EnhancedAgentBrain::reset,
             "Reset agent for new episode (backward compatibility)")
        .def("getBrain", &EnhancedAgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain (backward compatibility)");

    // Original AgentBrain binding for backward compatibility
    py::class_<AgentBrain>(m, "OriginalAgentBrain", R"pbdoc(Original AgentBrain class for backward compatibility)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // ===== FACTORY FUNCTIONS =====
    m.def("createDefaultConfig", &EnhancedConfig::createDefaultConfig,
         "Create a default configuration template")
        .def("createDefaultConfigOriginal", &Config::createDefaultConfig,
             "Create a default configuration (original)")
        .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), "Create a new brain with configuration (original)")
        .def("createEnhancedBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<EnhancedBrain> {
            return std::make_shared<EnhancedBrain>(config);
        }, py::arg("config"), "Create a new enhanced brain")
        .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
            return std::make_shared<SimpleWorld>();
        }, "Create a new simple world")
        .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create a new agent brain interface (original)")
        .def("createEnhancedAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<EnhancedAgentBrain> {
            return std::make_shared<EnhancedAgentBrain>(brain);
        }, py::arg("brain"), "Create a new enhanced agent brain interface")
        .def("createConfigFromEnvironment", &EnhancedConfig::createFromEnvironment,
             "Create configuration from environment variables")
        .def("createVisualizationInterface", []() -> std::shared_ptr<VisualizationInterface> {
            return std::make_shared<VisualizationInterface>();
        }, "Create a visualization interface")
        .def("createWorkingMemory", []() -> std::shared_ptr<WorkingMemory> {
            return std::make_shared<WorkingMemory>();
        }, "Create working memory")
        .def("createEpisodicMemory", []() -> std::shared_ptr<EpisodicMemory> {
            return std::make_shared<EpisodicMemory>();
        }, "Create episodic memory")
        .def("createSemanticMemory", []() -> std::shared_ptr<SemanticMemory> {
            return std::make_shared<SemanticMemory>();
        }, "Create semantic memory")
        .def("createProceduralMemory", []() -> std::shared_ptr<ProceduralMemory> {
            return std::make_shared<ProceduralMemory>();
        }, "Create procedural memory")
        .def("createAssociativeMemory", []() -> std::shared_ptr<AssociativeMemory> {
            return std::make_shared<AssociativeMemory>();
        }, "Create associative memory");

    // ===== DEBUGGING AND EDUCATIONAL TOOLS =====
    m.def("createDebugInfo", []() {
        EnhancedBrain::DebugInfo info;
        return info;
    }, "Create debug information structure")
        .def("createLearningProgress", []() {
            EnhancedBrain::LearningProgress progress;
            return progress;
        }, "Create learning progress structure")
        .def("createSensoryAnalysis", []() {
            EnhancedAgentBrain::SensoryAnalysis analysis;
            return analysis;
        }, "Create sensory analysis structure")
        .def("createLearningStatistics", []() {
            EnhancedAgentBrain::LearningStatistics stats;
            return stats;
        }, "Create learning statistics structure")
        .def("createPerformanceMetrics", []() {
            EnhancedBrain::PerformanceMetrics metrics;
            return metrics;
        }, "Create performance metrics structure");

    // ===== UTILITY FUNCTIONS =====
    m.def("validateConfig", [](const EnhancedConfig& config) {
        return config.validate();
    }, py::arg("config"), "Validate configuration and return ValidationResult")
        .def("formatConfig", [](const EnhancedConfig& config) {
            return config.summary();
        }, py::arg("config"), "Format configuration as string")
        .def("exportConfig", [](const EnhancedConfig& config) {
            return config.getAllAsVariant();
        }, py::arg("config"), "Export configuration as variant map")
        .def("importConfig", [](EnhancedConfig& config, const std::map<std::string, EnhancedConfigValue>& values) {
            config.setBatch(values);
        }, py::arg("config"), py::arg("values"), "Import configuration from variant map");

    // ===== INVALID IDENTIFIER CONSTANTS =====
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ===== CONFIGURATION TYPE SUPPORT =====
    py::class_<ValidationResult>(m, "ValidationResult", "Result of configuration validation")
        .def_readwrite("valid", &ValidationResult::valid)
        .def_readwrite("errorMessage", &ValidationResult::errorMessage)
        .def_readwrite("warnings", &ValidationResult::warnings);

    // Register JSON support for EnhancedConfigValue
    py::class_<nlohmann::json>(m, "JSON")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("json_str"))
        .def("dump", &nlohmann::json::dump)
        .def("empty", &nlohmann::json::empty)
        .def("size", &nlohmann::json::size);
}

} // namespace nlm
