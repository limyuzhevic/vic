#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();

    // ========== ADVANCED COMMANDS FOR POWER USERS ==========
    
    // Configuration and Parameter Management
    template<typename T>
    T getAdvancedConfig(const std::string& key) const;
    
    template<typename T>
    void setAdvancedConfig(const std::string& key, const T& value);
    
    // Performance Monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    std::unordered_map<std::string, double> getPerformanceMetrics();
    
    // Memory Analysis
    std::vector<NeuronId> getMemoryNeurons() const;
    std::vector<float> getMemoryActivations() const;
    float getWorkingMemoryLoad() const;
    std::vector<std::string> analyzeMemoryPatterns() const;
    
    // Learning and Adaptation Control
    void setLearningRate(float rate);
    void setPlasticityThreshold(float threshold);
    void enableAdvancedLearning(bool enable);
    
    // Neuromodulation Control
    void setDopamineLevel(float level);
    void setCuriosityLevel(float level);
    void setNoveltySensitivity(float sensitivity);
    
    // Export and Import
    bool exportBrainState(const std::string& filepath) const;
    bool importBrainState(const std::string& filepath);
    
    // Scriptability and Automation
    std::string exportBrainAsJSON() const;
    bool importBrainFromJSON(const std::string& jsonData);
    
    // Advanced Visualization and Analysis
    std::string generateNetworkTopologyReport() const;
    std::vector<std::string> analyzeBehaviorPatterns() const;
    float getNetworkEfficiency() const;
    
    // Real-time Monitoring
    void enableRealTimeMonitoring(bool enable);
    bool isRealTimeMonitoringEnabled() const;
    std::unordered_map<std::string, double> getSystemDiagnostics() const;
    
    // Batch Operations for Performance
    void batchProcessNeurons(std::function<void(Neuron*)> processor);
    void batchProcessSynapses(std::function<void(Synapse*)> processor);
    
    // Custom Learning Scenarios
    void loadCustomScenario(const std::string& scenarioName);
    void saveCustomScenario(const std::string& scenarioName);
    std::vector<std::string> getAvailableScenarios() const;
    
    // Advanced Error Correction and Recovery
    void enableErrorCorrection(bool enable);
    bool isErrorCorrectionEnabled() const;
    void applyErrorCorrection();
    
    // Performance Optimization
    void optimizeForPerformance();
    void enableMemoryOptimization(bool enable);
    void enableSpeedOptimization(bool enable);
    
    // Debug and Diagnostic Tools
    void enableDebugMode(bool enable);
    bool isDebugModeEnabled() const;
    std::string getDebugInformation() const;
    
    // System Health Monitoring
    bool checkSystemHealth() const;
    std::string getHealthReport() const;
    
    // Advanced Checkpoint Management
    void setCheckpointInterval(size_t steps);
    void setCheckpointCompressionLevel(CompressionLevel level);
    void enableAutoCheckpoint(bool enable);
    
    // Custom Plasticity Rules
    void addCustomPlasticityRule(const std::string& ruleName, std::function<float(const Synapse*, const SpikeEvent&)> rule);
    void removeCustomPlasticityRule(const std::string& ruleName);
    
    // Advanced Sequence Learning
    void enableSequenceLearning(bool enable);
    void setSequenceLearningWindow(size_t windowSize);
    std::vector<std::vector<NeuronId>> getLearnedSequences() const;
    
    // Custom Reward Functions
    void setCustomRewardFunction(std::function<float(const Action*, const SensoryInput&)> rewardFunc);
    void resetCustomRewardFunction();
    
    // Advanced Environment Interaction
    void enableEnvironmentAdaptation(bool enable);
    void setEnvironmentParameters(const std::unordered_map<std::string, float>& params);
    
    // Performance Profiling
    void startPerformanceProfiling();
    void stopPerformanceProfiling();
    std::string getPerformanceProfile() const;
    
    // Memory Management
    void forceMemoryCleanup();
    void setMemoryLimit(size_t bytes);
    size_t getMemoryUsage() const;
    
    // Advanced Network Configuration
    void setNetworkTopology(const std::string& topology);
    void addCustomConnection(const std::string& type, std::function<void(Brain*)> connectionBuilder);
    std::string getCurrentNetworkTopology() const;
    
    // Custom Development Patterns
    void setDevelopmentPattern(const std::string& pattern);
    std::string getDevelopmentPattern() const;
    void enableDevelopment(bool enable);
    
    // Advanced Visualization Export
    std::string exportForVisualization(const std::string& format = "graphml") const;
    std::string exportNeuralActivityData(const std::string& filepath) const;
    
    // Batch Learning
    void batchLearnSkills(const std::vector<std::pair<std::string, std::vector<NeuronId>>>& skills);
    void batchAssociatePatterns(const std::vector<std::tuple<NeuronId, NeuronId, float>>& associations);
    
    // Advanced Error Handling
    void setErrorHandlingPolicy(const std::string& policy);
    void enableGracefulDegradation(bool enable);
    
    // Custom Monitoring
    void addMonitoringHook(std::function<void(const std::string&)> hook);
    void removeMonitoringHook(const std::string& hookName);
    
    // Performance Tuning
    void tuneForLatency();
    void tuneForThroughput();
    void tuneForMemory();
    
    // Advanced Analysis
    std::vector<std::string> detectAnomalies() const;
    float calculateSystemStability() const;
    std::string analyzeComputationalComplexity() const;
