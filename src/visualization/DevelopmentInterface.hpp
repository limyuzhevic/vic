// Development interface for interactive development tools
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <fstream>

namespace nlm {

struct DevelopmentParameters {
    std::string parameterName;
    float currentValue;
    float defaultValue;
    float minValue;
    float maxValue;
    std::string parameterType;
    std::string description;
    bool isDynamic;
    bool isControllable;
    std::vector<std::string> dependencies;
    std::string units;
};

struct NeuralParameters {
    float neuronThreshold;
    float neuronRefractoryPeriod;
    float neuronTimeConstant;
    float neuronMembraneCapacitance;
    float synapticWeightMax;
    float synapticWeightMin;
    float stdpLearningRate;
    float hebbianLearningRate;
    float structuralPlasticityRate;
};

struct MemoryParameters {
    size_t workingMemoryCapacity;
    size_t episodicMemoryCapacity;
    size_t associativeMemoryCapacity;
    float consolidationThreshold;
    float replayInterval;
    float consolidationInterval;
};

struct NeuromodulationParameters {
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionErrorLevel;
    float neuromodulationStrength;
};

struct DevelopmentStageControl {
    SimulationStep currentStep;
    Timestamp currentTime;
    DevelopmentalStage currentStage;
    float plasticityRate;
    float developmentProgress;
    float maturityLevel;
    bool isActive;
    std::vector<std::string> activeProcesses;
    std::vector<std::string> completedProcesses;
};

struct DevelopmentScript {
    std::string scriptName;
    std::string scriptType;
    std::vector<std::string> commands;
    std::map<std::string, std::string> variables;
    std::chrono::steady_clock::time_point scheduledTime;
    bool isActive;
    bool isRepeating;
    std::string description;
};

struct MemoryVisualization {
    size_t workingMemoryUsage;
    size_t episodicMemoryUsage;
    size_t associativeMemoryUsage;
    float workingMemoryEfficiency;
    float episodicMemoryEfficiency;
    float associativeMemoryEfficiency;
    std::vector<size_t> memoryUsageHistory;
};

struct DevelopmentMetrics {
    SimulationStep step;
    Timestamp timestamp;
    float plasticityEfficiency;
    float developmentSpeed;
    float maturityProgress;
    float learningRate;
    std::vector<float> stageMetrics;
    bool isDevelopmentStagnant;
    bool isDevelopmentAccelerating;
    std::string developmentStatus;
};

struct DebugScript {
    std::string scriptId;
    std::string scriptName;
    std::string scriptType;
    std::string scriptContent;
    std::map<std::string, std::string> variables;
    bool isEnabled;
    bool isRunning;
    std::chrono::steady_clock::time_point lastExecuted;
    std::vector<std::string> executionLog;
    std::string description;
};

class DevelopmentInterface {
public:
    DevelopmentInterface();
    ~DevelopmentInterface();
    
    bool initialize(std::shared_ptr<Brain> brain);
    void shutdown();
    
    void update(const VisualizationData& data);
    
    // Development stage control
    void setDevelopmentStage(DevelopmentalStage stage);
    DevelopmentalStage getDevelopmentStage() const;
    void advanceDevelopmentStage();
    void resetDevelopmentStage();
    
    // Parameter adjustment
    void adjustParameter(const std::string& parameterName, float newValue);
    float getParameter(const std::string& parameterName) const;
    bool hasParameter(const std::string& parameterName) const;
    std::vector<std::string> getAllParameterNames() const;
    
    // Neural parameter control
    void setNeuralParameters(const NeuralParameters& params);
    NeuralParameters getNeuralParameters() const;
    void adjustNeuronThreshold(float delta);
    void adjustSynapticWeights(float delta);
    
    // Memory parameter control
    void setMemoryParameters(const MemoryParameters& params);
    MemoryParameters getMemoryParameters() const;
    void adjustMemoryCapacity(const std::string& memoryType, size_t delta);
    
    // Neuromodulation control
    void setNeuromodulationParameters(const NeuromodulationParameters& params);
    NeuromodulationParameters getNeuromodulationParameters() const;
    void adjustNeuromodulationLevel(const std::string& modulator, float delta);
    
    // Development script management
    void addDevelopmentScript(const DevelopmentScript& script);
    void removeDevelopmentScript(const std::string& scriptName);
    void executeDevelopmentScript(const std::string& scriptName);
    void pauseDevelopmentScript(const std::string& scriptName);
    void resumeDevelopmentScript(const std::string& scriptName);
    std::vector<DevelopmentScript> getActiveDevelopmentScripts() const;
    
    // Debug script management
    void addDebugScript(const DebugScript& script);
    void removeDebugScript(const std::string& scriptId);
    void executeDebugScript(const std::string& scriptId);
    std::vector<DebugScript> getAllDebugScripts() const;
    std::string executeDebugScriptContent(const std::string& content);
    
    // Visualization
    MemoryVisualization getMemoryVisualization() const;
    DevelopmentMetrics getDevelopmentMetrics() const;
    DevelopmentStageControl getDevelopmentStageControl() const;
    
    // Real-time control
    void enableRealTimeControl(bool enable);
    bool isRealTimeControlEnabled() const;
    void startRealTimeThread();
    void stopRealTimeThread();
    bool isRealTimeThreadActive() const;
    
    // Scriptable debugging
    std::string runDebugScript(const std::string& script);
    std::vector<std::string> getDebugHistory() const;
    void clearDebugHistory();
    
    // Configuration
    void setParameterUpdateRate(double rate);
    void setDevelopmentStageUpdateRate(double rate);
    void setScriptExecutionTimeout(double timeout);
    
    // Configuration getters
    double getParameterUpdateRate() const;
    double getDevelopmentStageUpdateRate() const;
    double getScriptExecutionTimeout() const;
    
    // Export
    void exportDevelopmentParameters(const std::string& filepath) const;
    void exportDevelopmentScripts(const std::string& filepath) const;
    void exportDebugScripts(const std::string& filepath) const;
    void exportDevelopmentMetrics(const std::string& filepath) const;
    
    // Visualization preparation
    void prepareDevelopmentDataForVisualization(std::vector<float>& devData) const;
    void prepareParameterDataForVisualization(std::vector<std::string>& paramData) const;
    void prepareScriptDataForVisualization(std::vector<std::string>& scriptData) const;

private:
    std::shared_ptr<Brain> brain_;
    
    // Configuration
    double parameterUpdateRate_;
    double developmentStageUpdateRate_;
    double scriptExecutionTimeout_;
    bool enableRealTimeControl_;
    
    // Development state
    DevelopmentalStage currentStage_;
    std::map<std::string, float> parameters_;
    NeuralParameters neuralParams_;
    MemoryParameters memoryParams_;
    NeuromodulationParameters neuromodParams_;
    
    // Development scripts
    std::vector<DevelopmentScript> developmentScripts_;
    std::vector<DebugScript> debugScripts_;
    std::vector<std::string> debugHistory_;
    
    // Metrics
    DevelopmentMetrics currentMetrics_;
    DevelopmentStageControl stageControl_;
    
    // Background thread
    std::thread realTimeThread_;
    std::atomic<bool> realTimeThreadActive_;
    
    // Analysis helpers
    void updateDevelopmentParameters();
    void updateDevelopmentStage();
    void analyzeDevelopmentProgress() const;
    void updateDevelopmentMetrics() const;
    
    // Parameter management helpers
    void validateParameter(const std::string& parameterName, float value) const;
    void applyParameterChange(const std::string& parameterName, float newValue);
    void notifyParameterChange(const std::string& parameterName, float oldValue, float newValue);
    
    // Script execution helpers
    void executeDevelopmentScriptInternal(const DevelopmentScript& script);
    void executeDebugScriptInternal(const DebugScript& script);
    void parseAndExecuteScript(const std::string& scriptContent, 
                              std::map<std::string, std::string>& variables);
    std::string evaluateScriptExpression(const std::string& expression,
                                         const std::map<std::string, std::string>& variables);
    
    // Visualization helpers
    void computeMemoryVisualization(MemoryVisualization& memViz) const;
    void computeDevelopmentMetrics() const;
    void updateStageControl() const;
    
    // Analysis helpers
    void assessDevelopmentProgress() const;
    void detectDevelopmentStagnation() const;
    void detectDevelopmentAcceleration() const;
    void updateDevelopmentStatus() const;
    
    // Export helpers
    void exportDevelopmentParametersToJSON(const std::string& filepath) const;
    void exportDevelopmentParametersToCSV(const std::string& filepath) const;
    void exportDevelopmentParametersToText(const std::string& filepath) const;
    
    mutable std::mutex mutex_;
};
