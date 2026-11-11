#pragma once

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include <memory>
#include <string>
#include <chrono>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>

namespace nlm {

// Forward declarations for response types
struct BrainState;
struct SimulationMetrics;
struct ConfigurationOptions;

// Command types for unified interface
enum class CommandType {
    RUN, STEP, PAUSE, RESUME, SAVE, LOAD, CONFIG, STATUS, DEBUG, 
    VISUALIZE, SCRIPT, HELP, VERSION, QUIT, EXIT
};

// Simulation state tracking
enum class SimulationState {
    RUNNING, PAUSED, STOPPED
};

// Configuration options structure
struct ConfigurationOptions {
    // Brain parameters
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    
    // Simulation parameters
    double timestep;
    uint64_t maxSteps;
    uint64_t replayInterval;
    uint64_t consolidationInterval;
    
    // System integration
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    bool enableNovelty;
    bool enableCuriosity;
    bool enablePrediction;
    bool enableCognition;
    
    // Logging and output
    std::string logLevel;
    std::string checkpointPath;
    bool verboseLogging;
    
    // Performance
    size_t threadCount;
    bool enableSIMD;
    bool enableVectorization;
    
    // Integration with external systems
    bool enableScripting;
    bool enableVisualization;
    bool enableRemoteControl;
    
    ConfigurationOptions() : 
        neuronCount(1000),
        regionCount(1),
        connectionProbability(0.1f),
        timestep(0.001),
        maxSteps(10000),
        replayInterval(100),
        consolidationInterval(1000),
        enableCheckpointing(true),
        enableReplay(true),
        enableDevelopment(true),
        enableNovelty(true),
        enableCuriosity(true),
        enablePrediction(true),
        enableCognition(true),
        logLevel("INFO"),
        checkpointPath("./checkpoint.bin"),
        verboseLogging(false),
        threadCount(std::thread::hardware_concurrency()),
        enableSIMD(true),
        enableVectorization(true),
        enableScripting(false),
        enableVisualization(false),
        enableRemoteControl(false) {}
};

// Brain state snapshot for checkpointing
struct BrainState {
    uint64_t step;
    double time;
    float learnedPreference;
    std::vector<float> neuronStates;
    std::vector<float> synapticWeights;
    std::chrono::high_resolution_clock::time_point timestamp;
};

// Simulation metrics for monitoring
struct SimulationMetrics {
    uint64_t totalSteps;
    uint64_t totalSpikes;
    float averageFiringRate;
    float totalReward;
    uint64_t memoryEpisodesStored;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    double wallClockTime;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
    SimulationMetrics() : 
        totalSteps(0), totalSpikes(0), averageFiringRate(0.0f),
        totalReward(0.0f), memoryEpisodesStored(0.0f),
        noveltyLevel(0.0f), curiosityLevel(0.0f), dopamineLevel(0.0f),
        wallClockTime(0.0), startTime(), endTime() {}
};

// Main unified brain controller class
class CoreBrainController {
public:
    CoreBrainController(const std::shared_ptr<Config>& config, 
                       const ConfigurationOptions& options);
    ~CoreBrainController();
    
    // Main control methods
    void run();
    void step(uint64_t steps = 1);
    void pause();
    void resume();
    void stop();
    
    // State management
    bool saveState(const std::string& path);
    bool loadState(const std::string& path);
    void reset();
    
    // Configuration and information
    void printConfiguration() const;
    void printStatus() const;
    const ConfigurationOptions& getOptions() const { return options; }
    
    // Metrics and monitoring
    const SimulationMetrics& getMetrics() const { return metrics; }
    BrainState getCurrentBrainState() const;
    
    // Integration methods
    void enableAgentBrain(bool enable);
    void enableWorldSimulation(bool enable);
    void enableExperimentMode(bool enable);
    
    // Advanced operations
    void executeScript(const std::string& script);
    void startVisualization();
    void startDebugConsole();
    
    // Event handling
    void setStepCallback(std::function<void(uint64_t)> callback);
    void setCompletionCallback(std::function<void()> callback);
    void setErrorCallback(std::function<void(const std::string&)> callback);
    
private:
    // Core systems
    std::shared_ptr<Config> config;
    ConfigurationOptions options;
    
    // Main brain and integrated systems
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Phase6IntegratedExperiment> experiment;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    
    // Control and state
    SimulationState currentState;
    SimulationStep currentStep;
    Timestamp currentTime;
    bool running;
    
    // Metrics and statistics
    SimulationMetrics metrics;
    mutable std::mutex metricsMutex;
    
    // Callbacks and event handlers
    std::function<void(uint64_t)> stepCallback;
    std::function<void()> completionCallback;
    std::function<void(const std::string&)> errorCallback;
    
    // Performance monitoring
    std::chrono::high_resolution_clock::time_point simulationStartTime;
    uint64_t stepsCompleted;
    uint64_t spikesCompleted;
    
    // Internal methods
    void initializeSystems();
    void cleanupSystems();
    void updateMetrics();
    void processStep(uint64_t step, Timestamp time);
    bool validateConfiguration() const;
    std::string generateErrorMessage(const std::string& operation) const;
    
    // Integration helpers
    void setupIntegration();
    void setupMemorySystems();
    void setupNeuromodulation();
    void setupPredictionSystem();
    void setupCognitionSystems();
    
    // Performance optimizations
    void optimizeForOptions();
    void setupSIMD();
    void setupMultithreading();
};

} // namespace nlm