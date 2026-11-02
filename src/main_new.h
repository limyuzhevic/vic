#pragma once

// Enhanced NLM main executable with modular architecture
// Replaces the monolithic src/main.cpp with configurable, extensible design

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

// Forward declarations
class LearningExperiment;

// Command-line argument parser
class NLMCommandLine {
public:
    enum class Mode {
        BASIC,           // Basic connectivity test
        PLIABILITY,      // Plasticity learning experiment
        STDP,            // STDP verification
        INTEGRATION,     // Full integration demo
        SCRIPT,          // Script mode for batch processing
        HELP,            // Show help
        VERSION          // Show version info
    };
    
    struct Options {
        Mode mode = Mode::INTEGRATION;
        std::string configFile = "configs/default.cfg";
        uint64_t seed = 42;
        size_t neuronCount = 500;
        size_t steps = 1000;
        bool verbose = false;
        bool quiet = false;
        std::string outputFile;
        bool saveState = false;
        std::vector<std::string> customArgs;
    };
    
    static Options parseArgs(int argc, char** argv);
    static void printHelp();
    static void printVersion();
};

// Advanced brain operations for research and experimentation
class AdvancedBrainOperations {
public:
    static void runLearningExperiment(std::shared_ptr<Brain> brain, size_t steps);
    static void runPatternLearning(std::shared_ptr<Brain> brain,
                                 const std::vector<float>& pattern,
                                 size_t cycles);
    static void runMemoryTest(std::shared_ptr<Brain> brain);
    static void runDevelopmentSimulation(std::shared_ptr<Brain> brain,
                                       size_t lifespan);
    static void runNeuromodulationStudy(std::shared_ptr<Brain> brain);
};

// Convenience functions for common tasks
namespace Convenience {
    // Brain creation with presets
    std::shared_ptr<Brain> createMinimalBrain(size_t neuronCount = 100);
    std::shared_ptr<Brain> createLearningBrain(size_t neuronCount = 500);
    std::shared_ptr<Brain> createDevelopmentalBrain(size_t neuronCount = 1000);
    
    // Simulation helpers
    void runForSteps(std::shared_ptr<Brain> brain, size_t steps,
                    std::function<void(size_t)> callback = nullptr);
    
    // Analysis tools
    float calculateLearningScore(std::shared_ptr<Brain> brain);
    std::vector<float> getWeightDistribution(std::shared_ptr<Brain> brain);
    
    // Visualization helpers
    void printBrainStats(std::shared_ptr<Brain> brain, bool verbose = false);
}

// Enhanced main application class
class NLMMain {
public:
    NLMMain();
    ~NLMMain();
    
    bool initialize(const NLMCommandLine::Options& options);
    bool run();
    bool cleanup();
    
    // Configuration management
    void applyPhase2Defaults(std::shared_ptr<Config> config);
    void applyPhase6Defaults(std::shared_ptr<Config> config);
    void applyAdvancedOptions(std::shared_ptr<Config> config,
                             const NLMCommandLine::Options& options);
    
    // Execution modes
    bool runBasicMode(std::shared_ptr<Brain> brain);
    bool runPlasticityMode(std::shared_ptr<Brain> brain);
    bool runStdpMode(std::shared_ptr<Brain> brain);
    bool runIntegrationMode(std::shared_ptr<Brain> brain);
    bool runScriptMode(std::shared_ptr<Brain> brain);
    
    // Utility functions
    void setupLogging(bool verbose);
    void printBanner();
    std::string getModeString(NLMCommandLine::Mode mode);
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    NLMCommandLine::Options options_;
    bool initialized_ = false;
};

// Legacy compatibility functions (for backward compatibility)
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
void runPlasticityExperiment(std::shared_ptr<Brain> brain);
void runStdpVerification(std::shared_ptr<Brain> brain);

// Learning experiment struct (compatibility)
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s)
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState();
    void recordFinalState();
    void computeStatistics();
};

} // namespace nlm