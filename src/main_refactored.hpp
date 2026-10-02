pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

class TestConstants {
public:
    // Neuron configuration constants
    static constexpr size_t DEFAULT_NEURON_COUNT = 500;
    static constexpr size_t SMALL_NEURON_COUNT = 100;
    static constexpr size_t TEST_NEURON_COUNT = 50;
    
    // Simulation constants
    static constexpr double DEFAULT_TIMESTEP = 0.001;
    static constexpr double FAST_TIMESTEP = 0.01;
    static constexpr SimulationStep DEFAULT_MAX_STEPS = 1000;
    static constexpr SimulationStep PLAPACITY_EXPERIMENT_STEPS = 1000;
    static constexpr SimulationStep STD_CONNECTED_ACTIVITY_STEPS = 50;
    
    // Current injection constants
    static constexpr float DEFAULT_EXCITATORY_CURRENT = 30.0f;
    static constexpr float CONNECTIVITY_TEST_CURRENT = 50.0f;
    static constexpr float STD_ACTIVITY_CURRENT = 60.0f;
    
    // Plasticity constants
    static constexpr float DEFAULT_LTP_WEIGHT = 0.02f;
    static constexpr float DEFAULT_LTD_WEIGHT = 0.015f;
    static constexpr float DEFAULT_STDP_TAU = 20.0f;
    
    // Learning constants
    static constexpr float WEIGHT_CHANGE_THRESHOLD = 0.01f;
    static constexpr float LEARNING_THRESHOLD = 0.001f;
    
    // Structural plasticity constants
    static constexpr float DEFAULT_SYNAPSTOGENESIS_RATE = 0.0001f;
    static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Random seed
    static constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
};

class TestResult {
public:
    std::string testName;
    bool passed = false;
    std::string message;
    double executionTime = 0.0;
    std::map<std::string, std::string> metrics;
};

class NeuralTestRunner {
public:
    NeuralTestRunner(std::shared_ptr<Config> config) : config_(config) {}
    
    TestResult runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    TestResult runPlasticityExperiment(std::shared_ptr<Brain> brain);
    TestResult runStdpVerification(std::shared_ptr<Brain> brain);
    TestResult runLearningExperiment(std::shared_ptr<Brain> brain);
    
    std::vector<TestResult> runAllTests(std::shared_ptr<Brain> brain);
    
private:
    std::shared_ptr<Config> config_;
    
    void resetBrainForTest(std::shared_ptr<Brain> brain);
    void configureForTest(std::shared_ptr<Brain> brain, bool enablePlasticity = false);
};

class NLMCommandLineInterface {
public:
    struct CommandLineOptions {
        std::string configFile = "configs/default.cfg";
        std::string outputFile = "";
        std::string logFile = "";
        std::string testMode = "all"; // "all", "connectivity", "plasticity", "stdp", "learning"
        double timestep = TestConstants::DEFAULT_TIMESTEP;
        size_t neuronCount = TestConstants::DEFAULT_NEURON_COUNT;
        SimulationStep maxSteps = TestConstants::DEFAULT_MAX_STEPS;
        bool verbose = false;
        bool help = false;
        bool version = false;
    };
    
    static CommandLineOptions parseCommandLine(int argc, char** argv);
    static void printHelp(const std::string& programName);
    static void printVersion();
    
    static int runApplication(CommandLineOptions& options);
    static int runTests(CommandLineOptions& options);
};

} // namespace nlm
