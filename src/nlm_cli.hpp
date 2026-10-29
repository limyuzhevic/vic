#pragma once

#include "experiments/Experiment.hpp"
#include "experiments/Metrics.hpp"
#include <memory>
#include <string>
#include <vector>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;
class Brain;
class AgentBrain;
class SimpleWorld;
class SensoryPercept;
class MotorCommand;

/**
 * Phase 6 integrated experiment runner
 */
class Phase6Demo {
public:
    Phase6Demo();
    ~Phase6Demo();
    
    /**
     * Run the complete integration demonstration
     */
    void run(size_t numEpisodes = 5, size_t stepsPerEpisode = 2000);
    
    /**
     * Show help information
     */
    static void showHelp();
    
    /**
     * Show version information
     */
    static void showVersion();
    
private:
    void setupConfiguration(std::shared_ptr<Config> config);
    void setupExperiment(std::shared_ptr<Config> config);
    void runIntegrationVerification(std::shared_ptr<Brain> brain);
    void testMemoryIntegration(std::shared_ptr<Brain> brain);
    void testNeuromodulationIntegration(std::shared_ptr<Brain> brain);
    void testCheckpointing(std::shared_ptr<Brain> brain);
    void testReplay(std::shared_ptr<Brain> brain);
    void runFullExperiment(std::shared_ptr<Brain> brain, size_t maxSteps);
    void collectResults(const Phase6IntegrationResult& result);
    
    std::shared_ptr<Config> config_;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime_;
    
    // Configuration for demo
    struct DemoConfig {
        size_t neuronCount = 500;
        size_t regionCount = 1;
        float connectionProbability = 0.1f;
        uint64_t seed = 42;
        double timestep = 0.001;
        bool enableCheckpointing = true;
        bool enableReplay = true;
        bool enableDevelopment = true;
        std::string checkpointPath = "./checkpoint_test.bin";
    };
    
    DemoConfig demoConfig_;
};

/**
 * Unified CLI interface
 */
class NLMCommandLineInterface {
public:
    NLMCommandLineInterface();
    ~NLMCommandLineInterface();
    
    /**
     * Initialize the CLI interface
     */
    bool initialize(int argc, char** argv);
    
    /**
     * Run the main application loop
     */
    void run();
    
    /**
     * Get current command
     */
    const std::string& getCommand() const { return command_; }
    
    /**
     * Get current options
     */
    const std::unordered_map<std::string, std::string>& getOptions() const { return options_; }
    
    /**
     * Get current arguments
     */
    const std::vector<std::string>& getCurrentArgs() const { return currentArgs_; }
    
private:
    // Command line parsing
    bool parseArguments(int argc, char** argv);
    bool loadConfiguration();
    bool initializeLogging();
    bool configureSimulation();
    
    // Configuration presets
    void applyPresetConfiguration(const std::string& presetName);
    
    // Mode execution
    void runDemoMode();
    void runExperimentMode();
    void runVisualizationMode();
    void runBenchmarkMode();
    
    // Experiment execution
    void runPhase3Experiment();
    void runPhase4Experiment();
    void runPhase6Experiment();
    void runCustomExperiment();
    
    // Feature application
    void applyFeatureToggles(AgentBrain& agent);
    
    // Help and information
    void showHelp();
    void showVersion();
    
    // State
    std::string command_;
    std::unordered_map<std::string, std::string> options_;
    std::vector<std::string> currentArgs_;
    bool verboseMode_;
    bool quietMode_;
    bool profilingEnabled_;
    
    enum class ErrorHandlingMode {
        Halt,
        Continue,
        Retry,
        Graceful
    };
    
    ErrorHandlingMode errorHandling_;
    size_t numEpisodes_;
    size_t stepsPerEpisode_;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
    std::shared_ptr<Config> config_;
    SimulationClock simulationClock_;
};

} // namespace nlm

