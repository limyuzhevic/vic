#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <map>
#include <algorithm>
#include <cmath>
#include <random>
#include <sstream>
#include <iomanip>

namespace nlm {

// Forward declarations for Python-specific types
class PyObject;
class PyDict;

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // === CONVENIENCE METHODS ===
    
    // Batch processing methods for multiple steps
    void processPercepts(const std::vector<SensoryPercept>& percepts);
    std::vector<MotorCommand> decodeCommands(const std::vector<size_t>& steps);
    std::vector<MotorCommand> stepMultiple(const std::vector<SensoryPercept>& percepts, 
                                          const std::vector<float>& rewards,
                                          const std::vector<float>& predictedRewards,
                                          size_t maxSteps = 100);
    
    // State checkpoint/save/load utilities
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);
    void setCheckpoint(float threshold = 0.5f);
    void loadCheckpoint(const std::string& name);
    std::vector<std::string> getAvailableCheckpoints() const;
    
    // Simulation statistics and metrics collection
    struct SimulationMetrics {
        double totalSimulationTime;
        size_t totalSteps;
        float avgReward;
        float maxReward;
        float minReward;
        float avgPredictionError;
        float avgCuriosity;
        float avgNovelty;
        size_t totalActions;
        std::map<MotorCommand, size_t> actionCounts;
        double learningRate;
        double explorationRate;
    };
    SimulationMetrics collectMetrics() const;
    void resetMetrics();
    std::string getMetricsSummary() const;
    
    // High-level learning control methods
    void enableLearning(bool enable);
    void setLearningRate(float rate);
    void setExplorationRate(float rate);
    void setRewardShaping(float shapingFactor);
    void setCuriosityExploration(float exploration);
    void triggerLearningEpoch();
    void triggerCriticalPeriod();
    void triggerAdultStage();
    
    // === ADVANCED FEATURES ===
    
    // === ADVANCED LEARNING ALGORITHMS ===
    
    // Advanced learning algorithms
    void enableCurriculumLearning(bool enable, const std::string& strategy = "difficulty_adaptive");
    void setTransferLearningTarget(std::shared_ptr<AgentBrain> sourceBrain);
    void enableMetaLearning(bool enable, float metaLearningRate = 0.01f);
    void setCustomPlasticityRule(const std::string& ruleName, const std::map<std::string, float>& parameters);
    void applyAdvancedRewardShaping(float baseReward, float shapingFactor = 1.0f, 
                                   const std::string& strategy = "exponential");
    void triggerMetaLearningCycle();
    
    // === MEMORY MANAGEMENT ===
    
    // Enhanced memory management
    void enableMemoryPooling(bool enable, size_t poolSize = 1024);
    void setCustomMemoryAllocator(std::function<void*(size_t)> allocator,
                                  std::function<void(void*)> deallocator);
    void enableMemoryEfficientCheckpointing(bool enable, const std::string& compression = "gzip");
    void enablePersistentMemory(const std::string& persistentPath);
    void optimizeMemoryUsage();
    size_t getMemoryFootprint() const;
    bool monitorMemoryUsage(float thresholdMB = 1000.0f);
    
    // === ADVANCED NEURAL DYNAMICS ===
    
    // Advanced neural dynamics
    void registerCustomNeuronModel(const std::string& modelName, 
                                  std::function<float(float, float)> activationFunc,
                                  std::function<float(float)> derivativeFunc);
    void setNetworkTopology(const std::vector<std::vector<float>>& connectivityMatrix);
    void enableSparseConnectivity(float sparsityLevel = 0.95f);
    void optimizeNetworkTopology();
    void setStructuralPlasticityControl(float structuralLearningRate = 0.01f,
                                        float structuralDecayRate = 0.001f);
    void configureNeuromodulation(float dopamineGain = 1.0f, float curiosityGain = 1.0f,
                                  float noveltyGain = 1.0f);
    
    // === PERFORMANCE OPTIMIZATION ===
    
    // Performance optimization
    void enableParallelProcessing(bool enable, int numThreads = std::thread::hardware_concurrency());
    void enableVectorization(bool enable);
    void enableGPUEnabled(bool enable);
    void setCustomCompilationFlags(const std::string& flags);
    void enablePerformanceProfiling(bool enable, float samplingInterval = 0.1f);
    void optimizeForLatency();
    void optimizeForThroughput();
    float getLatency() const;
    float getThroughput() const;
    
    // === ADVANCED SIMULATION FEATURES ===
    
    // Advanced simulation features
    void enableMultiAgentCoordination(bool enable);
    void addAgentToTeam(std::shared_ptr<AgentBrain> agent, const std::string& teamId = "default");
    void enableTeamLearning(const std::string& teamId, const std::string& learningStrategy = "collaborative");
    void enableTeamCompetition(const std::string& competitionType = "cooperative");
    void configureCommunicationProtocol(const std::string& protocol, 
                                       const std::map<std::string, float>& parameters);
    void enableSocialLearning(bool enable, float socialLearningRate = 0.1f);
    void analyzeEmergentBehaviors();
    std::map<std::string, float> getEmergentBehaviorMetrics() const;
    
    // === EXPERT TOOLS AND UTILITIES ===
    
    // Expert tools and utilities
    void createVisualizationSnapshot(const std::string& outputPath = "visualization.png");
    void enableExperimentTracking(const std::string& trackingId = "default");
    void configureHyperparameterTuning(const std::map<std::string, std::vector<float>>& searchSpace,
                                      const std::string& optimizationAlgorithm = "bayesian");
    void triggerHyperparameterOptimization();
    std::map<std::string, float> getHyperparameterImportance() const;
    void enableModelInterpretability(bool enable);
    std::string generateModelExplanation() const;
    void runPerformanceBenchmark(const std::string& benchmarkSuite = "default");
    std::map<std::string, float> getBenchmarkResults() const;
    
    // Episode management and reset utilities
    bool isEpisodeActive() const;
    void startEpisode(size_t episodeId = 0, const std::string& episodeType = "default");
    void endEpisode(float finalReward);
    void forceReset();
    size_t getCurrentEpisode() const;
    std::string getEpisodeType() const;
    float getEpisodeScore() const;
    void recordEpisodeAction(MotorCommand action, float reward);
    
    // Memory introspection and debugging tools
    struct MemoryStats {
        size_t neuronCount;
        size_t synapseCount;
        size_t activeNeurons;
        size_t firingNeurons;
        float totalCurrent;
        float averageFiringRate;
        float excitationInhibitionRatio;
        float totalSpikeCount;
        double memoryUtilization;
    };
    MemoryStats introspectMemory() const;
    std::vector<float> getNeuronActivityProfile() const;
    std::vector<std::pair<size_t, float>> getActiveNeurons() const;
    std::vector<std::pair<float, float>> getSynapticStrengths() const;
    
    // Performance profiling utilities
    struct PerformanceStats {
        double lastStepTime;
        double averageStepTime;
        double maxStepTime;
        double minStepTime;
        size_t stepsPerSecond;
        size_t memoryUsageBytes;
        size_t peakMemoryUsageBytes;
        std::chrono::high_resolution_clock::time_point sessionStartTime;
        double sessionDurationSeconds;
        double processingEfficiency;
    };
    PerformanceStats profilePerformance() const;
    double getProcessingEfficiency() const;
    double getFPS() const;
    size_t getMemoryUsage() const;
    double getSessionUptime() const;
    
    // === ERROR HANDLING AND VALIDATION ===
    
    // Better error messages with context
    struct ErrorContext {
        std::string operation;
        std::string timestamp;
        std::map<std::string, std::string> details;
        std::string getFormattedMessage() const;
    };
    
    static ErrorContext createError(const std::string& operation, const std::string& message,
                                   const std::map<std::string, std::string>& details = {});
    void handleError(const ErrorContext& error);
    
    // Input validation methods
    bool validateSensoryPercept(const SensoryPercept& percept) const;
    bool validateRewardValue(float reward) const;
    bool validateMotorCommand(MotorCommand cmd) const;
    bool validateWorldState(const SimpleWorld& world) const;
    
    // State validation utilities
    bool validateBrainState() const;
    bool validateNeuromodulationState() const;
    bool validateDevelopmentState() const;
    bool validateMemoryState() const;
    
    // === PYTHON-SPECIFIC FEATURES ===
    
    // Type conversion utilities for Python interop
    std::vector<float> perceptToPythonVector(const SensoryPercept& percept) const;
    SensoryPercept pythonVectorToPercept(const std::vector<float>& data) const;
    MotorCommand getMotorCommandFromIndex(size_t index) const;
    size_t getMotorCommandIndex(MotorCommand cmd) const;
    std::vector<MotorCommand> pythonListToMotorCommands(const std::vector<size_t>& indices) const;
    std::vector<size_t> motorCommandsToPythonList(const std::vector<MotorCommand>& commands) const;
    
    // Serialization methods for Python objects
    std::string serializeToJSON() const;
    std::string serializeToMsgPack() const;
    bool deserializeFromJSON(const std::string& jsonStr);
    bool deserializeFromMsgPack(const std::string& msgpackStr);
    
    // Callback mechanism support
    void addRewardCallback(std::function<void(float)> callback);
    void addActionCallback(std::function<void(MotorCommand)> callback);
    void addLearningCallback(std::function<void(float)> callback);
    void removeRewardCallback(size_t id);
    void removeActionCallback(size_t id);
    void removeLearningCallback(size_t id);
    void clearAllCallbacks();
    
    // Debug and introspection utilities
    std::string getStateSummary() const;
    std::string debugInfo() const;
    void enableDebugMode(bool enable);
    bool isDebugModeEnabled() const;
    void logMetrics() const;
    
    // Utility methods for Python
    float normalizeReward(float reward, float min = -1.0f, float max = 1.0f) const;
    float denormalizeReward(float normalizedReward, float min = -1.0f, float max = 1.0f) const;
    MotorCommand selectActionWithProbability(const std::vector<MotorCommand>& actions,
                                             const std::vector<float>& probabilities) const;
    float computeDistance(const std::vector<float>& vec1, const std::vector<float>& vec2) const;
    
    // === EXISTING METHODS (for compatibility) ===
    
    void processSensoryInput(const SensoryPercept& percept, bool computeNovelty);
    void applyAdvancedModulation(float reward, float predictedReward, float shapingFactor = 1.0f);
    void updateDevelopmentalStage(double timestep);

private:
    // === PRIVATE MEMBERS ===
    
    // Core brain interface
    std::shared_ptr<Brain> brain_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool learningEnabled_;
    bool debugModeEnabled_;
    bool episodeActive_;
    
    // State management
    float rewardShapingFactor_;
    float learningRate_;
    float explorationRate_;
    float curiosityExploration_;
    size_t currentEpisode_;
    std::string episodeType_;
    float episodeScore_;
    
    // Reward and penalty tracking
    float totalReward_;
    std::vector<std::pair<float, std::string>> penalties_;
    std::vector<float> episodeRewards_;
    std::vector<MotorCommand> episodeActions_;
    
    // Performance tracking
    double totalSimulationTime_;
    size_t totalSteps_;
    size_t stepsSinceReset_;
    double startTime_;
    double stepStartTime_;
    double lastStepTime_;
    
    // Statistics collection
    SimulationMetrics currentMetrics_;
    PerformanceStats currentPerformance_;
    MemoryStats currentMemoryStats_;
    
    // Callbacks
    size_t nextCallbackId_;
    std::vector<std::pair<size_t, std::function<void(float)>>> rewardCallbacks_;
    std::vector<std::pair<size_t, std::function<void(MotorCommand)>>> actionCallbacks_;
    std::vector<std::pair<size_t, std::function<void(float)>>> learningCallbacks_;
    
    // Checkpoint management
    std::map<std::string, std::string> checkpoints_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    float totalPenalty_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Internal helper methods
    void updateMetrics(MotorCommand action, float reward);
    void updatePerformance(double timestep);
    void updateMemoryStats() const;
    float computeCurrentEfficiency() const;
    bool validateInputRange(float value, float min, float max) const;
    std::string getCurrentTimestamp() const;
    
    // Callback management
    template<typename CallbackType>
    size_t addCallback(std::vector<std::pair<size_t, CallbackType>>& callbackList,
                       CallbackType callback);
    
    template<typename CallbackType>
    void removeCallback(std::vector<std::pair<size_t, CallbackType>>& callbackList, size_t id);
    
    template<typename... Args>
    void invokeCallbacks(const std::vector<std::pair<size_t, std::function<Args...>>>& callbacks,
                         Args&&... args);
};

// Python-specific type aliases
using PyMotorCommand = MotorCommand;
using PySensoryPercept = SensoryPercept;
using PyBrainPtr = std::shared_ptr<Brain>;
using PyAgentBrainPtr = std::shared_ptr<AgentBrain>;
using PyCallbackId = size_t;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
