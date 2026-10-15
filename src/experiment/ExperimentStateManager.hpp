#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include "../core/Config/Config.hpp"

namespace nlm {

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

namespace nlm {

// Experiment state management configuration
struct ExperimentStateConfig {
    std::string name;
    std::chrono::system_clock::time_point creationTime;
    std::chrono::system_clock::time_point lastUpdateTime;
    ExperimentState currentState;
    ExperimentState previousState;
    std::string stateDescription;
    std::map<std::string, std::string> stateMetadata;
    std::vector<std::string> errorMessages;
    std::vector<std::string> warningMessages;
    std::map<std::string, ConfigValue> stateParameters;
    
    ExperimentStateConfig() 
        : creationTime(std::chrono::system_clock::now()), 
          lastUpdateTime(std::chrono::system_clock::now()),
          currentState(ExperimentState::Pending) {}
};

// State transition information
struct StateTransition {
    ExperimentState fromState;
    ExperimentState toState;
    std::chrono::system_clock::time_point timestamp;
    std::string reason;
    std::map<std::string, std::string> transitionData;
};

class ExperimentStateManager {
public:
    ExperimentStateManager();
    ~ExperimentStateManager();
    
    // Disable copying, enable moving
    ExperimentStateManager(const ExperimentStateManager&) = delete;
    ExperimentStateManager& operator=(const ExperimentStateManager&) = delete;
    ExperimentStateManager(ExperimentStateManager&&) noexcept;
    ExperimentStateManager& operator=(ExperimentStateManager&&) noexcept;
    
    // State initialization and cleanup
    bool initialize(const std::string& experimentName);
    bool shutdown(const std::string& experimentName);
    
    // State management
    bool setExperimentState(const std::string& experimentName, ExperimentState newState, 
                           const std::string& reason = "");
    ExperimentState getExperimentState(const std::string& experimentName) const;
    bool hasExperimentState(const std::string& experimentName) const;
    
    // State history and recovery
    std::vector<StateTransition> getStateTransitions(const std::string& experimentName) const;
    std::vector<ExperimentState> getStateHistory(const std::string& experimentName, size_t limit = 100) const;
    bool rollbackState(const std::string& experimentName, size_t steps);
    bool checkpointState(const std::string& experimentName);
    
    // State persistence and recovery
    bool saveState(const std::string& experimentName, const std::string& filepath) const;
    bool loadState(const std::string& experimentName, const std::string& filepath);
    bool backupState(const std::string& experimentName, const std::string& backupName);
    bool restoreState(const std::string& experimentName, const std::string& backupName);
    
    // State metadata management
    void setStateMetadata(const std::string& experimentName, const std::string& key, 
                         const std::string& value);
    std::string getStateMetadata(const std::string& experimentName, const std::string& key) const;
    std::map<std::string, std::string> getAllStateMetadata(const std::string& experimentName) const;
    
    // State monitoring
    std::chrono::system_clock::time_point getExperimentCreationTime(const std::string& experimentName) const;
    std::chrono::system_clock::time_point getExperimentLastUpdateTime(const std::string& experimentName) const;
    std::chrono::seconds getExperimentLifetime(const std::string& experimentName) const;
    
    // Error and warning management
    bool addError(const std::string& experimentName, const std::string& errorMessage);
    bool addWarning(const std::string& experimentName, const std::string& warningMessage);
    std::vector<std::string> getErrors(const std::string& experimentName) const;
    std::vector<std::string> getWarnings(const std::string& experimentName) const;
    void clearErrors(const std::string& experimentName);
    void clearWarnings(const std::string& experimentName);
    
    // State statistics
    size_t getStateTransitionCount(const std::string& experimentName) const;
    double getStateUptimePercentage(const std::string& experimentName, 
                                   const std::chrono::system_clock::time_point& referenceTime) const;
    std::map<ExperimentState, size_t> getStateDistribution(const std::string& experimentName) const;
    
    // State validation
    bool isValidStateTransition(const std::string& experimentName, 
                               ExperimentState fromState, 
                               ExperimentState toState) const;
    std::vector<ExperimentState> getValidNextStates(const std::string& experimentName, 
                                                   ExperimentState currentState) const;
    
    // Cleanup and maintenance
    bool cleanupExpiredStates(const std::chrono::system_clock::time_point& cutoffTime);
    bool cleanupExperiment(const std::string& experimentName);
    std::vector<std::string> getAllExperimentNames() const;
    size_t getExperimentCount() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm