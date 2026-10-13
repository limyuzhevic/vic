#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>
#include "SensoryPercept.hpp"
#include "Action.hpp"
#include "../world/SimpleWorld.hpp"

namespace nlm {

// Advanced agent commands for enhanced control and monitoring
class AdvancedAgentCommands {
public:
    struct CommandResult {
        bool success;
        std::string message;
        std::map<std::string, float> metrics;
        std::vector<std::string> warnings;
    };
    
    struct MemoryStats {
        size_t episodicMemoryCount;
        size_t workingMemorySize;
        size_t semanticMemorySize;
        size_t proceduralMemorySize;
        float episodicMemoryConsolidation;
        float workingMemoryStability;
        float semanticMemoryAccessTime;
        float proceduralMemoryExecutionTime;
    };
    
    struct NeuromodulationStats {
        float acetylcholineLevel;
        float norepinephrineLevel;
        float serotoninLevel;
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        float predictionError;
        float rewardPredictionError;
    };
    
    struct CognitiveStats {
        float conceptFormationConfidence;
        float planningConfidence;
        float attentionFocus;
        float selfModelConfidence;
        float socialLearningProgress;
        float spatialRepresentationAccuracy;
        float temporalRelationAccuracy;
    };
    
    // Command execution interface
    CommandResult executeCommand(const std::string& commandName, 
                                const std::map<std::string, std::string>& parameters = {});
    
    // Advanced agent status and monitoring
    MemoryStats getMemoryStatistics() const;
    NeuromodulationStats getNeuromodulationStatistics() const;
    CognitiveStats getCognitiveStatistics() const;
    
    // Sensory and motor control
    CommandResult injectCustomSensoryInput(const std::vector<float>& inputData, 
                                         const std::string& sensoryType = "vision");
    CommandResult setMotorBias(const std::map<ActionType, float>& biasValues);
    CommandResult calibrateSensoryMapping(const std::string& sensoryType);
    
    // Learning and development control
    CommandResult triggerDevelopmentStage(DevelopmentalStage stage);
    CommandResult setLearningRate(float rate);
    CommandResult enableAllPlasticity(bool enable);
    CommandResult configureStructuralPlasticity(float synaptogenesisRate, float pruningRate);
    
    // Memory management
    CommandResult compactMemory();
    CommandResult consolidateMemory(float threshold);
    CommandResult replayMemory(const std::string& criteria);
    CommandResult clearMemory(const std::string& memoryType);
    
    // Neuromodulation control
    CommandResult setNeuromodulatorLevel(const std::string& neuromodulator, float level);
    CommandResult applyNeuromodulationEffect(const std::string& effectType, float magnitude);
    CommandResult resetNeuromodulation(const std::string& neuromodulator);
    
    // Cognitive control
    CommandResult setPlanningDepth(size_t depth);
    CommandResult configureConceptFormation(float threshold, float stabilityThreshold);
    CommandResult activateSelfModel(const std::string& actionSet);
    CommandResult enableSocialLearning(bool enable);
    
    // Simulation control
    CommandResult pauseSimulation(bool pause);
    CommandResult stepSimulation(size_t steps);
    CommandResult resetSimulation();
    CommandResult saveSimulationState(const std::string& filename);
    CommandResult loadSimulationState(const std::string& filename);
    
    // Performance optimization
    CommandResult optimizeMemoryUsage();
    CommandResult optimizeProcessingSpeed();
    CommandResult balanceLoad(const std::string& strategy);
    CommandResult enableHardwareAcceleration(bool enable);
    
    // Monitoring and debugging
    CommandResult startMonitoring(const std::string& duration);
    CommandResult stopMonitoring();
    CommandResult getMonitoringData(const std::string& format = "json");
    CommandResult exportLogs(const std::string& filename, const std::string& format = "text");
    
    // Experiment control
    CommandResult startExperiment(const std::string& name, 
                                 const std::map<std::string, std::string>& parameters);
    CommandResult stopExperiment();
    CommandResult pauseExperiment();
    CommandResult resumeExperiment();
    std::string getExperimentStatus() const;
    
    // Advanced queries
    std::vector<std::string> getAvailableCommands() const;
    std::map<std::string, std::string> getCommandHelp(const std::string& commandName) const;
    std::vector<std::string> getRecentCommands() const;
    std::string getCommandHistory() const;
    
    // Statistics and analytics
    float getPerformanceScore() const;
    float getLearningEfficiency() const;
    float getMemoryEfficiency() const;
    float getCognitiveLoad() const;
    float getSystemHealth() const;
    
    // Debug and diagnostic
    std::string diagnoseSystem() const;
    std::vector<std::string> getSystemWarnings() const;
    std::vector<std::string> getSystemErrors() const;
    CommandResult clearSystemCache();
    CommandResult recompileScripts();
    
    // Configuration management
    CommandResult loadConfiguration(const std::string& filename);
    CommandResult saveConfiguration(const std::string& filename);
    CommandResult resetConfiguration();
    std::map<std::string, std::string> getConfiguration() const;
    CommandResult setConfiguration(const std::string& key, const std::string& value);
    
    // Advanced agent capabilities
    CommandResult enableAdvancedMode(bool enable);
    CommandResult configureAdvancedFeatures(const std::map<std::string, bool>& features);
    std::vector<std::string> getAdvancedFeatures() const;
    bool isAdvancedModeEnabled() const;
    
    // Performance monitoring
    struct PerformanceMetrics {
        double timestamp;
        float cpuUsage;
        float memoryUsage;
        float networkUsage;
        float simulationSpeed;
        float learningRate;
        float convergenceRate;
        std::map<std::string, float> customMetrics;
    };
    
    PerformanceMetrics getCurrentPerformanceMetrics() const;
    std::vector<PerformanceMetrics> getPerformanceHistory(size_t maxPoints = 100) const;
    
    // Alert and notification system
    struct Alert {
        enum class Level { Info, Warning, Error, Critical };
        std::string message;
        Level level;
        double timestamp;
        std::string source;
        std::map<std::string, std::string> metadata;
    };
    
    using AlertCallback = std::function<void(const Alert&)>
;    
    void setAlertCallback(AlertCallback callback);
    std::vector<Alert> getActiveAlerts() const;
    std::vector<Alert> getRecentAlerts(size_t maxCount = 10) const;
    void clearAlerts();
    
    // Advanced agent capabilities
    CommandResult enableRealTimeMonitoring(bool enable);
    CommandResult configureAlertThresholds(const std::map<std::string, float>& thresholds);
    CommandResult enableAutoRecovery(bool enable);
    CommandResult setRecoveryStrategy(const std::string& strategy);
    
private:
    AdvancedAgentCommands(std::shared_ptr<class AgentBrain> agentBrain);
    ~AdvancedAgentCommands();
    
    // Disable copying
    AdvancedAgentCommands(const AdvancedAgentCommands&) = delete;
    AdvancedAgentCommands& operator=(const AdvancedAgentCommands&) = delete;
    
    // Internal helpers
    CommandResult executeInternalCommand(const std::string& commandName,
                                        const std::map<std::string, std::string>& parameters);
    bool validateCommandParameters(const std::string& commandName,
                                   const std::map<std::string, std::string>& parameters) const;
    CommandResult executeSensoryControl(const std::map<std::string, std::string>& parameters);
    CommandResult executeMotorControl(const std::map<std::string, std::string>& parameters);
    CommandResult executeLearningControl(const std::map<std::string, std::string>& parameters);
    CommandResult executeMemoryControl(const std::map<std::string, std::string>& parameters);
    CommandResult executeNeuromodulationControl(const std::map<std::string, std::string>& parameters);
    CommandResult executeCognitiveControl(const std::map<std::string, std::string>& parameters);
    CommandResult executeSimulationControl(const std::map<std::string, std::string>& parameters);
    CommandResult executeMonitoringControl(const std::map<std::string, std::string>& parameters);
    CommandResult executeExperimentControl(const std::map<std::string, std::string>& parameters);
    
    // Command execution registry
    std::map<std::string, std::function<CommandResult(const std::map<std::string, std::string>&)>> commandRegistry_;
    
    // State
    std::shared_ptr<class AgentBrain> agentBrain_;
    bool advancedModeEnabled_;
    bool realTimeMonitoringEnabled_;
    bool autoRecoveryEnabled_;
    std::string recoveryStrategy_;
    
    // Monitoring data
    std::vector<PerformanceMetrics> performanceHistory_;
    std::vector<Alert> alerts_;
    AlertCallback alertCallback_;
    
    // Command history
    std::vector<std::string> commandHistory_;
    std::map<std::string, std::map<std::string, std::string>> commandHelp_;
    
    // Statistics
    double startTime_;
    size_t totalCommandsExecuted_;
    float totalExecutionTime_;
    
    // Configuration
    std::map<std::string, std::string> configuration_;
    std::map<std::string, float> alertThresholds_;
    
    // Singleton interface
    static AdvancedAgentCommands& getInstance(std::shared_ptr<class AgentBrain> agentBrain = nullptr);
    static void shutdown();
};

} // namespace nlm