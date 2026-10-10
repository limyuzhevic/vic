#pragma once

#include "Brain.hpp"
#include "BrainDebugger.hpp"
#include "BrainSimulator.hpp"
#include "../core/Config/Config.hpp"
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
#include <map>

namespace nlm {

class SimulationRunner {
public:
    SimulationRunner(std::shared_ptr<Brain> brain, std::shared_ptr<BrainDebugger> debugger);
    ~SimulationRunner();
    
    // Disable copying, enable moving
    SimulationRunner(const SimulationRunner&) = delete;
    SimulationRunner& operator=(const SimulationRunner&) = delete;
    SimulationRunner(SimulationRunner&&) noexcept = default;
    SimulationRunner& operator=(SimulationRunner&&) noexcept = default;
    
    // Initialization and shutdown
    bool initialize();
    void shutdown();
    
    // Custom simulation execution
    struct CustomSimulation {
        std::string name;
        std::string description;
        SimulationConfig config;
        SimulationParameters parameters;
        std::vector<SimulationStep> customSteps;
        std::vector<SimulationVariable> initialVariables;
        std::vector<SimulationEvent> events;
        std::function<bool(SimulationStep, double, const SimulationState&)> condition;
        std::function<void(SimulationStep, double, SimulationState&)> action;
        std::function<void(const SimulationResult&)> completionCallback;
        std::function<void(const std::string& error)> errorCallback;
        
        CustomSimulation()
            : name("Custom Simulation"), config(), parameters(), customSteps(),
              initialVariables(), events(), condition(nullptr), action(nullptr),
              completionCallback(nullptr), errorCallback(nullptr) {}
    };
    
    // Simulation execution API
    bool runCustomSimulation(const CustomSimulation& simulation);
    bool runSimulation(const SimulationConfig& config);
    bool runToStep(size_t targetStep);
    bool runUntil(double timepoint);
    bool runForDuration(double duration, double timestep = 0.01);
    bool runWithEvents(const std::vector<SimulationEvent>& events);
    bool runWithConditions(const std::vector<std::function<bool()>>& conditions);
    
    // Simulation control
    bool pauseSimulation();
    bool resumeSimulation();
    bool stopSimulation();
    bool stepSimulation();
    bool stepSimulation(double timestep);
    bool resetSimulation();
    
    // Simulation status
    bool isRunning() const;
    bool isPaused() const;
    bool isStopped() const;
    size_t getCurrentStep() const;
    double getCurrentTime() const;
    SimulationStep getCurrentSimulationStep() const;
    SimulationState getSimulationState() const;
    
    // Variable control
    bool setVariable(const std::string& name, float value);
    bool setVariable(SimulationVariable variable, float value);
    float getVariable(const std::string& name) const;
    float getVariable(SimulationVariable variable) const;
    bool addVariable(const SimulationVariable& variable);
    bool removeVariable(const std::string& name);
    std::vector<std::string> listVariables() const;
    
    // Event management
    bool addEvent(const SimulationEvent& event);
    bool removeEvent(size_t eventId);
    bool modifyEvent(size_t eventId, const SimulationEvent& newEvent);
    std::vector<SimulationEvent> listEvents() const;
    
    // Data collection
    bool startDataCollection(const std::string& dataFile);
    bool stopDataCollection();
    void collectDataPoint();
    bool isCollectingData() const;
    bool saveSimulationData(const std::string& filename);
    bool loadSimulationData(const std::string& filename);
    
    // Results analysis
    SimulationResult analyzeResults();
    SimulationResult analyzeResults(const std::vector<SimulationResult>& results);
    std::vector<SimulationResult> analyzeMultipleSimulations(const std::vector<SimulationConfig>& configs);
    
    // Simulation management
    bool createSimulationCheckpoint(const std::string& name);
    bool restoreSimulationCheckpoint(const std::string& name);
    std::vector<std::string> listSimulationCheckpoints() const;
    bool deleteSimulationCheckpoint(const std::string& name);
    
    // Simulation statistics
    struct SimulationStatistics {
        size_t totalSteps;
        double totalTime;
        double averageStepTime;
        double realTimeFactor;
        size_t activeNeurons;
        size_t firingNeurons;
        float averageFiringRate;
        float totalReward;
        float learningProgress;
        float stabilityIndex;
        size_t memoryUsage;
        double cpuUsage;
        
        SimulationStatistics()
            : totalSteps(0), totalTime(0.0), averageStepTime(0.0), realTimeFactor(1.0),
              activeNeurons(0), firingNeurons(0), averageFiringRate(0.0f),
              totalReward(0.0f), learningProgress(0.0f), stabilityIndex(1.0f),
              memoryUsage(0), cpuUsage(0.0) {}
    };
    
    SimulationStatistics getSimulationStatistics() const;
    
    // Configuration
    void loadSimulationConfiguration(const std::string& configFile);
    void saveSimulationConfiguration() const;
    void resetSimulationConfiguration();
    SimulationConfig getCurrentSimulationConfiguration() const;
    
    // Simulation monitoring
    void enableRealTimeMonitoring(bool enable);
    bool isRealTimeMonitoringEnabled() const;
    void setMonitoringCallback(std::function<void(const SimulationState&)> callback);
    void setProgressCallback(std::function<void(double progress)> callback);
    
    // Thread safety
    void lockSimulationRunner() const;
    void unlockSimulationRunner() const;
    bool tryLockSimulationRunner() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<BrainDebugger> debugger_;
    std::shared_ptr<Config> config_;
    
    mutable std::mutex runnerMutex_;
    std::atomic<bool> isRunning_;
    std::atomic<bool> isPaused_;
    std::atomic<bool> isStopped_;
    std::thread simulationThread_;
    
    // Simulation state
    size_t currentStep_;
    double currentTime_;
    double wallClockStart_;
    double lastStepTime_;
    size_t stepsCompleted_;
    
    // Variables and events
    std::unordered_map<std::string, float> simulationVariables_;
    std::unordered_map<size_t, SimulationEvent> simulationEvents_;
    size_t nextEventId_;
    
    // Data collection
    bool dataCollectionEnabled_;
    std::string dataCollectionFile_;
    std::ofstream dataFile_;
    
    // Callbacks
    std::function<void(const SimulationState&)> monitoringCallback_;
    std::function<void(double progress)> progressCallback_;
    
    // Statistics
    SimulationStatistics simulationStatistics_;
    
    // Internal helper methods
    void cleanup();
    void runSimulationLoop(const CustomSimulation& simulation);
    void executeSimulationStep(double timestep);
    void updateSimulationState();
    void processEvents();
    void executeEvent(const SimulationEvent& event);
    void evaluateConditions();
    SimulationState captureSimulationState() const;
    void restoreSimulationState(const SimulationState& state);
    void updateSimulationStatistics();
    void logSimulationStep();
    void exportSimulationData() const;
    SimulationResult analyzeSingleSimulation(const CustomSimulation& simulation) const;
    std::vector<SimulationResult> analyzeMultipleSimulationsInternal(const std::vector<SimulationConfig>& configs) const;
    void generateSimulationReports() const;
    void validateSimulationSetup() const;
    void setupSimulationEnvironment();
    void cleanupSimulationEnvironment();
    void handleSimulationError(const std::string& error);
    void syncWithDebugger();
};

} // namespace nlm