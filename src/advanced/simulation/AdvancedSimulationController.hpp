#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Advanced simulation controller with comprehensive parameter management
class AdvancedSimulationController {
public:
    AdvancedSimulationController(std::shared_ptr<Brain> brain,
                                 std::shared_ptr<Environment> environment,
                                 std::shared_ptr<Logger> logger);
    ~AdvancedSimulationController();
    
    // Simulation lifecycle management
    void startSimulation();
    void pauseSimulation();
    void resumeSimulation();
    void stopSimulation();
    bool isSimulationRunning() const;
    bool isSimulationPaused() const;
    bool isSimulationStopped() const;
    
    // Step control and execution
    void stepForward();
    void stepBackward();
    void stepTo(SimulationStep step);
    void setStepCount(size_t steps);
    size_t getCurrentStep() const;
    size_t getTotalSteps() const;
    SimulationStep getMaxStep() const;
    
    // Speed and timing control
    void setSpeed(float speed);
    float getSpeed() const;
    void accelerate(float factor);
    void decelerate(float factor);
    double getCurrentStepDuration() const;
    double getAverageStepDuration() const;
    void setStepDuration(double duration);
    
    // Parameter management
    void setSimulationParameter(const std::string& name, double value);
    double getSimulationParameter(const std::string& name) const;
    bool hasSimulationParameter(const std::string& name) const;
    std::vector<std::string> getAvailableParameters() const;
    std::map<std::string, double> getAllParameters() const;
    
    // Parameter sets and presets
    void saveParameterSet(const std::string& name);
    void loadParameterSet(const std::string& name);
    void deleteParameterSet(const std::string& name);
    bool hasParameterSet(const std::string& name) const;
    std::vector<std::string> getParameterSetNames() const;
    
    // Environment control
    void resetEnvironment();
    void modifyEnvironment(const std::string& parameter, double value);
    std::string getEnvironmentState() const;
    void applyEnvironmentalChange(const std::string& changeName,
                                 const std::map<std::string, double>& parameters);
    
    // Brain control
    void resetBrain();
    void modifyBrainParameters(const std::map<std::string, double>& parameters);
    std::string getBrainState() const;
    void applyBrainModification(const std::string& modificationName,
                               const std::map<std::string, double>& parameters);
    
    // Recording and playback
    bool startRecording(const std::string& filepath);
    bool stopRecording();
    bool startPlayback(const std::string& filepath);
    bool stopPlayback();
    bool isPlaying() const;
    float getPlaybackPosition() const;
    size_t getPlaybackSpeed() const;
    void setPlaybackSpeed(size_t speed);
    
    // Event recording and replay
    struct SimulationEvent {
        double timestamp;
        std::string eventType;
        std::map<std::string, std::string> parameters;
        std::string source;
    };
    
    void recordEvent(const std::string& eventName,
                    const std::map<std::string, std::string>& parameters);
    std::vector<SimulationEvent> getRecordedEvents() const;
    void clearRecordedEvents();
    bool replayEvents(const std::vector<SimulationEvent>& events);
    
    // Batch execution
    void startBatch(size_t batchSize);
    void endBatch();
    bool isInBatchMode() const;
    std::vector<std::string> getBatchResults() const;
    void executeBatch(const std::vector<std::function<void()>>& operations);
    
    // Statistics and metrics
    std::string getSimulationStatistics() const;
    std::map<std::string, double> getDetailedStatistics() const;
    double calculateEfficiency() const;
    double calculateAccuracy() const;
    double calculateThroughput() const;
    std::map<std::string, double> getPerformanceMetrics() const;
    
    // Export/import simulation data
    bool exportSimulation(const std::string& filepath) const;
    bool importSimulation(const std::string& filepath);
    bool exportResults(const std::string& filepath) const;
    bool importResults(const std::string& filepath);
    
    // Scenario management
    void loadScenario(const std::string& scenarioName);
    void saveScenario(const std::string& scenarioName);
    bool hasScenario(const std::string& scenarioName) const;
    void deleteScenario(const std::string& scenarioName);
    std::vector<std::string> getScenarioNames() const;
    
    // Control interfaces
    void connectControlInterface(const std::string& interfaceName);
    void disconnectControlInterface();
    bool isControlInterfaceConnected() const;
    
    // Emergency controls
    void emergencyPause();
    void emergencyStop();
    bool isEmergencyMode() const;
    void executeEmergencyRecovery();
    
    // Real-time control
    void setTargetFPS(int fps);
    int getTargetFPS() const;
    void enableFrameLimiting(bool enable);
    bool isFrameLimitingEnabled() const;
    
    // Calibration and validation
    void calibrateTiming();
    bool validateConfiguration() const;
    std::vector<std::string> getValidationErrors() const;
    std::vector<std::string> getValidationWarnings() const;
    
    // Monitoring and diagnostics
    void enableMonitoring(bool enable);
    bool isMonitoringEnabled() const;
    std::map<std::string, double> getRealTimeDiagnostics() const;
    std::map<std::string, std::vector<double>> getTimeSeriesData(const std::string& metric,
                                                                 size_t samples = 100) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
