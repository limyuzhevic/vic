#pragma once

#include "Brain.hpp"
#include "BrainDebugger.hpp"
#include "BrainSimulator.hpp"
#include "../experiments/Experiment.hpp"
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <deque>
#include <algorithm>

namespace nlm {

class ExperimentDesigner {
public:
    ExperimentDesigner(std::shared_ptr<Brain> brain, std::shared_ptr<BrainDebugger> debugger,
                     std::shared_ptr<BrainSimulator> simulator);
    ~ExperimentDesigner();
    
    // Disable copying, enable moving
    ExperimentDesigner(const ExperimentDesigner&) = delete;
    ExperimentDesigner& operator=(const ExperimentDesigner&) = delete;
    ExperimentDesigner(ExperimentDesigner&&) noexcept = default;
    ExperimentDesigner& operator=(ExperimentDesigner&&) noexcept = default;
    
    // Initialization and shutdown
    bool initialize();
    void shutdown();
    
    // Experiment design and configuration
    struct ExperimentConfig {
        std::string name;
        std::string description;
        std::string author;
        std::string version;
        std::string tags;
        double startTime;
        double endTime;
        SimulationConfig simulationConfig;
        LearningParameters learningParameters;
        NeuromodulationConfig neuromodulationConfig;
        DevelopmentConfig developmentConfig;
        std::vector<SimulationVariable> variables;
        std::vector<SimulationEvent> events;
        std::vector<std::string> conditions;
        std::vector<std::string> hypotheses;
        std::vector<std::string> objectives;
        std::vector<std::string> metrics;
        
        ExperimentConfig()
            : name("Untitled Experiment"), description(""), author(""), version("1.0"),
              tags(""), startTime(0.0), endTime(0.0) {}
    };
    
    // Experiment API
    bool createExperiment(const ExperimentConfig& config);
    bool createExperiment(const std::string& name, const std::string& description);
    bool createStandardExperiment(const std::string& type);
    bool createLearningExperiment(const std::string& learningType);
    bool createMemoryExperiment(const std::string& memoryType);
    bool createPlasticityExperiment(const std::string& plasticityType);
    bool createDevelopmentExperiment(const std::string& developmentType);
    bool createNeuromodulationExperiment(const std::string& neuromodulationType);
    
    // Experiment management
    bool runExperiment(const std::string& name);
    bool runExperiment(const ExperimentConfig& config);
    bool pauseExperiment();
    bool resumeExperiment();
    bool stopExperiment();
    bool resetExperiment();
    
    // Experiment status
    bool isExperimentRunning() const;
    bool isExperimentPaused() const;
    bool isExperimentStopped() const;
    std::string getCurrentExperimentName() const;
    ExperimentConfig getCurrentExperimentConfig() const;
    ExperimentResult getExperimentResult() const;
    
    // Experiment data
    bool saveExperimentData(const std::string& filename);
    bool loadExperimentData(const std::string& filename);
    std::vector<ExperimentData> getExperimentHistory() const;
    bool deleteExperiment(const std::string& name);
    
    // Experiment analysis
    void analyzeExperimentPerformance();
    void analyzeExperimentLearning();
    void analyzeExperimentMemory();
    void analyzeExperimentPlasticity();
    void analyzeExperimentDevelopment();
    void analyzeExperimentNeuromodulation();
    
    // Experiment comparison
    void compareExperiments(const std::vector<std::string>& experimentNames);
    void compareExperimentResults(const std::vector<ExperimentResult>& results);
    void generateExperimentReport(const std::string& filename = "experiment_report.txt");
    
    // Experiment configuration
    bool addExperimentVariable(const SimulationVariable& variable);
    bool modifyExperimentVariable(const std::string& name, float value);
    bool removeExperimentVariable(const std::string& name);
    std::vector<SimulationVariable> getExperimentVariables() const;
    
    bool addExperimentEvent(const SimulationEvent& event);
    bool modifyExperimentEvent(size_t eventId, const SimulationEvent& newEvent);
    bool removeExperimentEvent(size_t eventId);
    std::vector<SimulationEvent> getExperimentEvents() const;
    
    // Experiment validation
    bool validateExperimentConfig(const ExperimentConfig& config) const;
    bool validateExperimentVariables() const;
    bool validateExperimentEvents() const;
    bool validateExperimentPerformance() const;
    std::vector<std::string> getExperimentValidationErrors() const;
    
    // Experiment replication
    bool replicateExperiment(const std::string& sourceExperiment, const std::string& targetExperiment);
    bool cloneExperiment(const std::string& sourceExperiment);
    bool modifyExperiment(const std::string& experimentName, const ExperimentConfig& newConfig);
    
    // Experiment metadata
    bool setExperimentMetadata(const std::string& key, const std::string& value);
    std::string getExperimentMetadata(const std::string& key) const;
    std::unordered_map<std::string, std::string> getExperimentMetadata() const;
    
    // Experiment statistics
    struct ExperimentStatistics {
        std::string experimentName;
        double duration;
        size_t totalSteps;
        double finalAccuracy;
        double finalLoss;
        double convergenceTime;
        float finalLearningRate;
        float totalReward;
        size_t memoryUsage;
        double cpuUsage;
        
        ExperimentStatistics()
            : experimentName(""), duration(0.0), totalSteps(0), finalAccuracy(0.0),
              finalLoss(1.0), convergenceTime(0.0), finalLearningRate(0.01f),
              totalReward(0.0f), memoryUsage(0), cpuUsage(0.0) {}
    };
    
    ExperimentStatistics getExperimentStatistics(const std::string& experimentName) const;
    std::vector<ExperimentStatistics> getAllExperimentStatistics() const;
    
    // Configuration
    void loadExperimentConfiguration(const std::string& configFile);
    void saveExperimentConfiguration() const;
    void resetExperimentConfiguration();
    ExperimentConfig getCurrentExperimentConfiguration() const;
    
    // Thread safety
    void lockExperimentDesigner() const;
    void unlockExperimentDesigner() const;
    bool tryLockExperimentDesigner() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<BrainDebugger> debugger_;
    std::shared_ptr<BrainSimulator> simulator_;
    std::shared_ptr<Config> config_;
    
    mutable std::mutex designerMutex_;
    std::atomic<bool> isRunning_;
    std::atomic<bool> isPaused_;
    std::atomic<bool> isStopped_;
    std::thread experimentThread_;
    
    // Experiment state
    std::string currentExperimentName_;
    ExperimentConfig currentExperimentConfig_;
    ExperimentResult currentExperimentResult_;
    
    // Experiment data storage
    std::unordered_map<std::string, ExperimentConfig> experimentConfigs_;
    std::unordered_map<std::string, ExperimentResult> experimentResults_;
    std::unordered_map<std::string, std::vector<ExperimentData>> experimentHistory_;
    
    // Experiment variables and events
    std::vector<SimulationVariable> experimentVariables_;
    std::vector<SimulationEvent> experimentEvents_;
    
    // Experiment metadata
    std::unordered_map<std::string, std::string> experimentMetadata_;
    
    // Internal helper methods
    void cleanup();
    void runExperimentLoop(const ExperimentConfig& config);
    void executeExperimentStep(double timestep);
    void updateExperimentState();
    void processExperimentEvents();
    void executeExperimentEvent(const SimulationEvent& event);
    void evaluateExperimentConditions();
    ExperimentResult captureExperimentResult() const;
    void restoreExperimentResult(const ExperimentResult& result);
    void updateExperimentStatistics();
    void logExperimentStep();
    void exportExperimentData() const;
    void generateExperimentReports() const;
    void validateExperimentSetup() const;
    void setupExperimentEnvironment();
    void cleanupExperimentEnvironment();
    void handleExperimentError(const std::string& error);
    void syncWithDebugger();
};

} // namespace nlm