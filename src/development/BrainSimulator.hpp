#pragma once

#include "Brain.hpp"
#include "BrainDebugger.hpp"
#include "../core/Config/Config.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../core/Random/Random.hpp"
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include <unordered_map>

namespace nlm {

class BrainSimulator {
public:
    BrainSimulator(std::shared_ptr<Brain> brain, std::shared_ptr<BrainDebugger> debugger);
    ~BrainSimulator();
    
    // Disable copying, enable moving
    BrainSimulator(const BrainSimulator&) = delete;
    BrainSimulator& operator=(const BrainSimulator&) = delete;
    BrainSimulator(BrainSimulator&&) noexcept = default;
    BrainSimulator& operator=(BrainSimulator&&) noexcept = default;
    
    // Simulation control
    bool initialize();
    void shutdown();
    
    // Main simulation loop
    void run(double duration, double timestep = 0.01);
    void runToStep(size_t targetStep);
    void runUntil(double timepoint);
    void pause();
    void resume();
    void stop();
    
    // Single step simulation
    void step();
    void step(double timestep);
    void step(SimulationStep step, Timestamp time);
    
    // Simulation control
    bool isRunning() const;
    bool isPaused() const;
    size_t getCurrentStep() const;
    double getCurrentTime() const;
    SimulationStep getCurrentSimulationStep() const;
    
    // Custom simulation configuration
    struct CustomSimulationConfig {
        size_t maxSteps;
        double timestep;
        double duration;
        bool enablePlasticity;
        bool enableDevelopment;
        bool enableNeuromodulation;
        bool enableMemorySystems;
        bool enablePrediction;
        bool enableCognition;
        std::string configName;
        
        // Custom initialization
        std::function<void(BrainSimulator&)> initializationCallback;
        std::function<void(BrainSimulator&, SimulationStep, double)> stepCallback;
        std::function<void(BrainSimulator&, const std::string&)> completionCallback;
        
        // Injection and modification
        std::vector<std::pair<NeuronId, MembranePotential>> inputCurrents;
        std::vector<std::pair<float, float>> parameterModifications;
        std::vector<std::pair<SynapseId, float>> weightAdjustments;
        
        CustomSimulationConfig()
            : maxSteps(10000), timestep(0.01), duration(100.0), 
              enablePlasticity(true), enableDevelopment(true), enableNeuromodulation(true),
              enableMemorySystems(true), enablePrediction(true), enableCognition(true),
              configName("Custom Simulation") {}
    };
    
    // Run custom simulation
    void runCustomSimulation(const CustomSimulationConfig& config);
    
    // Simulation state management
    struct SimulationState {
        size_t step;
        double time;
        double wallClockTime;
        bool isRunning;
        bool isPaused;
        float learningProgress;
        float stabilityIndex;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        float averageFiringRate;
        float totalReward;
        std::vector<float> performanceMetrics;
        std::unordered_map<std::string, float> customMetrics;
        
        SimulationState()
            : step(0), time(0.0), wallClockTime(0.0), isRunning(false), isPaused(false),
              learningProgress(0.0f), stabilityIndex(1.0f), activeNeuronCount(0),
              firingNeuronCount(0), averageFiringRate(0.0f), totalReward(0.0f) {}
    };
    
    SimulationState getSimulationState() const;
    void restoreSimulationState(const SimulationState& state);
    
    // Step callbacks and hooks
    void setStepCallback(std::function<void(BrainSimulator&, SimulationStep, double)> callback);
    void setCompletionCallback(std::function<void(BrainSimulator&, const std::string&)> callback);
    void setErrorCallback(std::function<void(BrainSimulator&, const std::string&)> callback);
    void setMetricsCallback(std::function<void(BrainSimulator&, const SimulationState&)> callback);
    
    // Performance and timing
    double getAverageStepTime() const;
    double getRealTimeFactor() const;
    size_t getStepsPerSecond() const;
    double getSimulatedTimePerRealTimeSecond() const;
    
    // Input and modification utilities
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToType(NeuronType type, MembranePotential current);
    void injectCurrentToRegion(RegionId region, MembranePotential current);
    void injectSensoryInput(const class SensoryInput& input);
    
    // Parameter modification during simulation
    void adjustWeight(SynapseId synapse, float weightDelta);
    void setWeight(SynapseId synapse, float weight);
    void adjustThreshold(NeuronId neuron, float thresholdDelta);
    void setThreshold(NeuronId neuron, float threshold);
    void tunePlasticity(PlasticityId plasticityId, const PlasticityParameters& params);
    void modifyNeuromodulator(NeuromodulatorId neuromodulator, float level);
    void setDevelopmentStage(DevelopmentalStage stage);
    
    // State exploration
    void snapshot(const std::string& name = "auto");
    bool restoreSnapshot(const std::string& name);
    std::vector<std::string> listSnapshots() const;
    bool deleteSnapshot(const std::string& name);
    
    // Debugging and monitoring
    std::shared_ptr<BrainDebugger> getDebugger() const { return debugger_; }
    bool attachDebugger(std::shared_ptr<BrainDebugger> debugger);
    
    // Configuration
    void setRealTimeFactor(double factor) { realTimeFactor_ = factor; }
    double getRealTimeFactor() const { return realTimeFactor_; }
    void enableLogging(bool enable) { loggingEnabled_ = enable; }
    bool isLoggingEnabled() const { return loggingEnabled_; }
    
    // Thread safety
    void lockSimulation() const;
    void unlockSimulation() const;
    bool tryLockSimulation() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<BrainDebugger> debugger_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<SimulationClock> clock_;
    std::shared_ptr<RandomGenerator> random_;
    
    mutable std::mutex simulationMutex_;
    std::atomic<bool> isRunning_;
    std::atomic<bool> isPaused_;
    std::thread simulationThread_;
    
    // Simulation state
    size_t currentStep_;
    double currentTime_;
    double wallClockStart_;
    double lastStepTime_;
    size_t stepsCompleted_;
    size_t totalStepsExecuted_;
    
    // Performance metrics
    double realTimeFactor_;
    double avgStepTime_;
    size_t stepsPerSecond_;
    bool loggingEnabled_;
    
    // Callbacks
    std::function<void(BrainSimulator&, SimulationStep, double)> stepCallback_;
    std::function<void(BrainSimulator&, const std::string&)> completionCallback_;
    std::function<void(BrainSimulator&, const std::string&)> errorCallback_;
    std::function<void(BrainSimulator&, const SimulationState&)> metricsCallback_;
    
    // Snapshot management
    std::unordered_map<std::string, SimulationState> snapshots_;
    std::deque<std::string> snapshotHistory_;
    
    // Simulation lifecycle
    void runSimulationLoop(const CustomSimulationConfig& config);
    void executeStep(double timestep);
    void cleanup();
    SimulationState captureState() const;
    void restoreState(const SimulationState& state);
    void logStep(const SimulationState& state);
    double calculateRealTimeFactor() const;
    void updatePerformanceMetrics();
};

} // namespace nlm