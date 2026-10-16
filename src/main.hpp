#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <string>
#include <memory>
#include <map>
#include <chrono>

namespace nlm {

class CommandLineOptions {
public:
    enum class Mode {
        SIMULATION,
        CONNECTIVITY_TEST,
        PLASTICITY_TEST,
        STDP_TEST,
        BENCHMARK,
        CONFIG_EXPORT,
        HELP
    };
    
    struct Options {
        Mode mode = Mode::SIMULATION;
        std::string configFile = "configs/default.cfg";
        std::string outputFile = "brain_checkpoint.bin";
        std::string testMode = "all";
        size_t neuronCount = 1000;
        size_t duration = 1000;
        size_t stepsPerSecond = 1000;
        double timestep = 0.001;
        bool verbose = false;
        bool headless = false;
        bool saveOnExit = false;
        std::map<std::string, std::string> customArgs;
    };
    
    static Options parse(int argc, char** argv);
    static void printHelp(Mode context = Mode::HELP);
    static bool validateOptions(const Options& opts);
};

class NLMApplication {
public:
    NLMApplication(const CommandLineOptions::Options& options);
    ~NLMApplication() = default;
    
    bool initialize();
    void run();
    void cleanup();
    
    // Mode handlers
    void runSimulationMode();
    void runConnectivityTest();
    void runPlasticityTest();
    void runSTDPTest();
    void runBenchmark();
    void exportConfiguration();
    
private:
    CommandLineOptions::Options options_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<class SimulationClock> clock_;
    
    void setupLogging();
    void setupConfiguration();
    void setupBrain();
    void runStandardTests();
    void printPerformanceStats();
    
    // Statistics
    struct PerfStats {
        size_t totalSteps = 0;
        size_t totalSpikes = 0;
        double startTime = 0.0;
        double endTime = 0.0;
    } stats_;
};

// Learning Experiment structure (extracted from main.cpp)
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

// Test functions (extracted from main.cpp)
void printBanner();
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
void runPlasticityExperiment(std::shared_ptr<Brain> brain);
void runStdpVerification(std::shared_ptr<Brain> brain);

} // namespace nlm