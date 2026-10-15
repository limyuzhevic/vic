#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>
#include "ExperimentProtocol.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Experiment state enumeration
enum class ExperimentState {
    Pending,
    Initializing,
    Running,
    Paused,
    Analyzing,
    Reporting,
    Completed,
    Failed,
    Cancelled,
    Unknown
};

// Experiment progress structure
struct ExperimentProgress {
    std::string experimentName;
    double progressPercentage;
    std::string currentStage;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::map<std::string, double> stageProgress;
    std::string currentError;
    
    ExperimentProgress() 
        : progressPercentage(0.0), startTime(std::chrono::system_clock::now()) {}
};

// Experiment lifecycle configuration
struct ExperimentLifecycleConfig {
    std::string name;
    std::string description;
    std::chrono::seconds maxDuration;
    bool autoSave;
    bool enableMonitoring;
    bool enableRecovery;
    std::string outputDirectory;
    std::map<std::string, std::string> metadata;
    
    ExperimentLifecycleConfig() 
        : maxDuration(std::chrono::seconds(3600)), autoSave(true), 
          enableMonitoring(true), enableRecovery(true) {}
};

class ExperimentLifecycleManager {
public:
    static ExperimentLifecycleManager& getInstance();
    
    // Lifecycle management
    bool createExperiment(const std::string& name, const ExperimentConfig& config);
    bool runExperiment(const std::string& name);
    bool pauseExperiment(const std::string& name);
    bool resumeExperiment(const std::string& name);
    bool stopExperiment(const std::string& name);
    bool deleteExperiment(const std::string& name);
    bool resetExperiment(const std::string& name);
    
    // State management
    ExperimentState getExperimentState(const std::string& name) const;
    std::vector<ExperimentState> getAllExperimentStates() const;
    bool saveExperimentState(const std::string& name);
    bool loadExperimentState(const std::string& name);
    bool isExperimentRunning(const std::string& name) const;
    
    // Progress tracking
    ExperimentProgress getExperimentProgress(const std::string& name) const;
    std::function<void(const ExperimentProgress&)> setProgressCallback;
    
    // Protocol management
    bool addProtocol(const std::string& experimentName, std::shared_ptr<ExperimentProtocol> protocol);
    bool removeProtocol(const std::string& experimentName, const std::string& protocolName);
    std::vector<std::string> getExperimentProtocols(const std::string& name) const;
    
    // Monitoring
    bool enableExperimentMonitoring(const std::string& name, bool enable);
    bool isExperimentMonitoringEnabled(const std::string& name) const;
    std::string getExperimentStatus(const std::string& name) const;
    std::chrono::system_clock::time_point getExperimentStartTime(const std::string& name) const;
    double getExperimentDuration(const std::string& name) const;
    
    // Error handling and recovery
    std::vector<std::string> getExperimentErrors(const std::string& name) const;
    bool clearExperimentErrors(const std::string& name);
    bool enableAutoRecovery(const std::string& name, bool enable);
    bool isAutoRecoveryEnabled(const std::string& name) const;
    
    // Configuration management
    void setExperimentConfig(const std::string& name, const ExperimentConfig& config);
    ExperimentConfig getExperimentConfig(const std::string& name) const;
    void addExperimentParameter(const std::string& name, const std::string& key, const ConfigValue& value);
    ConfigValue getExperimentParameter(const std::string& name, const std::string& key) const;
    
    // Resource management
    bool acquireResources(const std::string& name, const std::map<std::string, int>& resources);
    bool releaseResources(const std::string& name);
    bool checkResourcesAvailable(const std::string& name, const std::map<std::string, int>& resources) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm