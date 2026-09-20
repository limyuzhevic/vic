#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <chrono>
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Forward declarations
class PerformanceProfiler;
class NetworkVisualizer;
class AdvancedDebugger;
class ConfigurationTemplateManager;
class PowerUserAPI;

// Custom stimulus patterns
enum class StimulusPattern {
    RandomPoisson,
    RegularSpiking,
    Bursting,
    Theta,
    Gamma,
    Custom
};

// Replay modes for checkpoint system
enum class ReplayMode {
    Fast,
    Normal,
    Slow,
    VariableSpeed
};

// Custom stimulus injection configuration
struct CustomStimulusConfig {
    std::string name;
    StimulusPattern pattern;
    float frequency;           // Hz for regular patterns
    float meanRate;            // spikes per second for Poisson
    float burstFrequency;      // Hz for bursts
    float burstDuration;       // ms
    float burstIntensity;      // multiplier for normal rate
    std::vector<float> customTimes;    // for custom pattern
    std::vector<float> customAmplitudes;
    float targetNeuronId;      // -1 for all neurons
    NeuronType neuronType;     // All, Sensory, Internal, Motor
    float startTime;           // ms
    float duration;            // ms
    bool enabled;
};

// Synchronization control configuration
struct SyncControlConfig {
    bool enableTimeSync;               // Synchronize with real-time clock
    bool enableFixedSteps;             // Fixed number of simulation steps
    bool enableAdaptiveTimestep;       // Adjust timestep based on load
    float targetRealTimeStep;          // ms per simulation step
    float speedupFactor;               // 1.0 = real-time, >1.0 = faster
    float slowDownFactor;              // 0.0-1.0 for slow motion
    SimulationStep maxSteps;           // Hard limit on steps
    float softLimitRatio;              // Soft limit as ratio of estimated max
    bool pauseWhenBehind;              // Stop if simulation falls behind real-time
    float catchUpSpeed;                // Speed when catching up
};

// Batch operation configuration
struct BatchOperationConfig {
    std::string name;
    bool runInParallel;                // Use threading for batch
    size_t threadCount;                // Number of worker threads
    std::function<void(std::shared_ptr<Brain>)> operation; // Operation to run
    std::vector<float> parameterVariations; // Parameters to vary
    bool randomizeParameters;          // Randomize parameter ranges
    float successThreshold;            // Minimum success rate
    float timeoutPerBrain;             // seconds per brain
    std::string outputDirectory;       // Where to save results
    bool saveCheckpoints;              // Save intermediate states
    bool saveMetrics;                  // Save performance metrics
};

// Replay system configuration
struct ReplayConfig {
    std::string sourceFile;            // Checkpoint file to replay
    ReplayMode replayMode;              // Speed control mode
    SimulationStep startStep;          // Step to start replay at
    SimulationStep endStep;            // Step to stop replay at (-1 = end)
    bool skipSynchronization;          // Skip time synchronization
    bool updateSystemState;             // Update system state from checkpoint
    std::string outputFile;             // Save replay results
    float speed;                        // Playback speed factor
    bool loop;                          // Loop replay
    size_t loopCount;                   // Number of times to loop
};

// AdvancedSimulationController: Enhanced brain control for power users
class AdvancedSimulationController {
public:
    AdvancedSimulationController(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<Config> config,
        std::shared_ptr<PerformanceProfiler> profiler = nullptr,
        std::shared_ptr<NetworkVisualizer> visualizer = nullptr,
        std::shared_ptr<AdvancedDebugger> debugger = nullptr);
    
    ~AdvancedSimulationController();
    
    // Custom stimulus injection
    bool injectCustomStimulus(const CustomStimulusConfig& config);
    bool injectCustomStimulusPattern(
        const std::string& patternName,
        const std::vector<CustomStimulusConfig>& stimuli);
    void stopAllStimuli();
    void pauseStimuli(bool pause);
    bool isStimulusPaused() const;
    
    // Synchronization and timing controls
    void configureSyncControl(const SyncControlConfig& config);
    void pauseSync(bool pause);
    bool isSyncPaused() const;
    double getRealTimeSpeed() const;
    double getSimulationSpeed() const;
    
    // Batch operations
    std::vector<std::shared_ptr<Brain>> runBatchOperation(
        const BatchOperationConfig& config,
        size_t brainCount);
    bool runSingleBrainOperation(
        const BatchOperationConfig& config,
        std::shared_ptr<Brain> brain);
    
    // Checkpoint and replay systems
    bool saveCheckpoint(const std::string& filepath, 
                       const std::string& description = "");
    bool loadCheckpoint(const std::string& filepath);
    bool replayCheckpoint(const ReplayConfig& config);
    
    // Advanced simulation controls
    void setTimestepDuration(float dt);
    void setMaxSynapsesPerNeuron(size_t maxSynapses);
    void enableSparseConnectivity(bool enable);
    void setParallelProcessingLevel(size_t level);
    
    // Power user controls
    void setPlasticityRule(PlasticityType type, float weight);
    void enableEventDrivenMode(bool enable);
    void setEventThreshold(float threshold);
    
    // Monitoring and statistics
    double getSimulationTime() const;
    size_t getStepCount() const;
    size_t getTotalSpikesThisStep() const;
    float getAverageFiringRate() const;
    size_t getActiveNeuronCount() const;
    float getMemoryUsage() const;
    
    // Configuration management
    void updateConfig(const std::string& key, const ConfigValue& value);
    bool validateConfig() const;
    std::string getSimulationStatus() const;
    
    // Integration with external systems
    void connectProfiler(std::shared_ptr<PerformanceProfiler> profiler);
    void connectVisualizer(std::shared_ptr<NetworkVisualizer> visualizer);
    void connectDebugger(std::shared_ptr<AdvancedDebugger> debugger);
    void connectTemplateManager(std::shared_ptr<ConfigurationTemplateManager> tm);
    
    // Analysis and reporting
    std::string generateSimulationReport() const;
    std::string generatePerformanceReport() const;
    std::string generateConnectivityReport() const;
    
    // Cleanup
    void cleanup() noexcept;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
