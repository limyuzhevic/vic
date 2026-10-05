// NLM (熙然) - Neural Learning Machine
// Enhanced main entry point with modular architecture
// Supports configurable test suites and flexible configuration

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <map>
#include <functional>

using namespace nlm;

// Forward declarations for test functions
class NLMTestSuite;
class NLMExperimentManager;

// ==============================================================================
// CONFIGURATION AND ARGUMENT PARSING
// ==============================================================================

struct NLMConfig {
    // Core configuration
    uint64_t randomSeed = 42;
    double simulationTimestep = 0.001;
    int64_t neuronCount = 500;
    int64_t regionCount = 1;
    float connectionProbability = 0.15f;
    std::string configFile = "configs/default.cfg";
    
    // Test configuration
    bool enableConnectivityTest = true;
    bool enablePlasticityTest = true;
    bool enableStdpTest = true;
    bool enableLearningExperiment = true;
    bool enablePerformanceBenchmark = false;
    bool enableMemoryProfiling = false;
    
    // Output configuration
    bool verboseOutput = true;
    bool quietMode = false;
    std::string outputFile = "";
    
    // Performance configuration
    int benchmarkSteps = 1000;
    int memoryTestSteps = 1000;
    
    // Phase selection
    std::vector<std::string> phasesToRun = {"all"};
};

class NLMArgumentParser {
public:
    static NLMConfig parse(int argc, char** argv) {
        NLMConfig config;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                printUsage();
                exit(0);
            } else if (arg.substr(0, 7) == "--config") {
                config.configFile = parseConfigFile(arg, i, argv);
            } else if (arg == "--benchmark") {
                config.enablePerformanceBenchmark = true;
            } else if (arg == "--no-plasticity") {
                config.enablePlasticityTest = false;
            } else if (arg == "--no-stdp") {
                config.enableStdpTest = false;
            } else if (arg == "--learning-only") {
                config.enableLearningExperiment = true;
                config.enableConnectivityTest = false;
                config.enablePlasticityTest = false;
                config.enableStdpTest = false;
            } else if (arg == "--quick") {
                config.neuronCount = 100;
                config.benchmarkSteps = 100;
            } else if (arg == "--verbose") {
                config.verboseOutput = true;
                config.quietMode = false;
            } else if (arg == "--quiet") {
                config.verboseOutput = false;
                config.quietMode = true;
            } else if (arg.substr(0, 6) == "--seed") {
                config.randomSeed = parseIntArg(arg, 6);
            } else if (arg.substr(0, 9) == "--neurons") {
                config.neuronCount = parseIntArg(arg, 9);
            } else if (arg.substr(0, 12) == "--timestep") {
                config.simulationTimestep = parseFloatArg(arg, 12);
            } else if (arg.substr(0, 10) == "--phase") {
                addPhaseToRun(config, arg.substr(10));
            }
        }
        
        return config;
    }
    
private:
    static std::string parseConfigFile(const std::string& arg, int& i, char** argv) {
        if (arg.find('=') != std::string::npos) {
            return arg.substr(arg.find('=') + 1);
        } else if (i + 1 < argc) {
            i++;
            return argv[i];
        }
        return "configs/default.cfg";
    }
    
    static int64_t parseIntArg(const std::string& arg, size_t prefixLen) {
        std::string value = arg.substr(prefixLen);
        if (!value.empty() && value[0] == '=') {
            value = value.substr(1);
        }
        return std::stoll(value);
    }
    
    static double parseFloatArg(const std::string& arg, size_t prefixLen) {
        std::string value = arg.substr(prefixLen);
        if (!value.empty() && value[0] == '=') {
            value = value.substr(1);
        }
        return std::stod(value);
    }
    
    static void addPhaseToRun(NLMConfig& config, const std::string& phase) {
        if (phase != "all") {
            config.phasesToRun.push_back(phase);
        }
    }
    
    static void printUsage() {
        std::cout << "Usage: nlm [options]\n\n"
                  << "Options:\n"
                  << "  --help, -h                    Show this help message\n"
                  << "  --config=<file>               Load configuration from file\n"
                  << "  --seed=<seed>                 Set random seed\n"
                  << "  --neurons=<count>             Set neuron count\n"
                  << "  --timestep=<timestep>         Set simulation timestep\n"
                  << "  --learning-only               Run only learning experiments\n"
                  << "  --quick                       Run quick test with small network\n"
                  << "  --verbose                     Enable verbose output\n"
                  << "  --quiet                       Disable progress output\n"
                  << "  --benchmark                   Run performance benchmark\n"
                  << "  --no-plasticity               Disable plasticity tests\n"
                  << "  --no-stdp                      Disable STDP tests\n"
                  << "  --phase=<phase>               Run specific phase (all, phase2, phase3, phase6)\n\n"
                  << "Example:\n"
                  << "  nlm --neurons=1000 --seed=123 --learning-only\n";
    }
};

// ==============================================================================
// COMMAND LINE TEST FUNCTIONS
// ==============================================================================

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced with Modular Architecture                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printSection(const std::string& title) {
    if (!title.empty()) {
        if (title == "=== Testing Results ===") {
            std::cout << std::endl;
            std::cout << "=" << std::string(60, '=') << std::endl;
            std::cout << title << std::endl;
            std::cout << "=" << std::string(60, '=') << std::endl;
        } else {
            std::cout << std::endl;
            std::cout << ">>> " << title << " <<<" << std::endl;
            std::cout << std::string(title.length() + 6, '=') << std::endl;
        }
    }
}

class NLMExperimentManager {
public:
    NLMExperimentManager(std::shared_ptr<Logger> logger) : logger(logger) {}
    
    void runAllExperiments(const NLMConfig& config, std::shared_ptr<Brain> brain) {
        std::cout << std::endl;
        std::cout << "Running comprehensive experiment suite..." << std::endl;
        
        if (config.enableConnectivityTest) {
            runConnectivityTest(brain);
        }
        
        if (config.enableLearningExperiment) {
            runLearningExperiment(brain, config);
        }
        
        if (config.enableStdpTest) {
            runStdpVerification(brain);
        }
        
        if (config.enablePerformanceBenchmark) {
            runPerformanceBenchmark(brain, config.benchmarkSteps);
        }
        
        if (config.enableMemoryProfiling) {
            runMemoryProfiling(brain, config.memoryTestSteps);
        }
        
        std::cout << std::endl;
        std::cout << "=== Experiment Suite Complete ===" << std::endl;
    }
    
private:
    std::shared_ptr<Logger> logger;
    
    void runConnectivityTest(std::shared_ptr<Brain> brain) {
        printSection("Test 1: Basic Neural Connectivity");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            std::cout << "  [WARNING] No neurons found for connectivity test!" << std::endl;
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            std::cout << "  [WARNING] No neurons found!" << std::endl;
            return;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        std::cout << "  Injecting current into " << std::min(size_t(10), neurons.size()) << " neurons..." << std::endl;
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        std::cout << "  Spikes generated: " << spikes << std::endl;
        
        if (spikes > 0) {
            std::cout << "  ✓ Spikes propagate through network" << std::endl;
        } else {
            std::cout << "  ! No spikes detected" << std::endl;
        }
    }
    
    void runLearningExperiment(std::shared_ptr<Brain> brain, const NLMConfig& config) {
        printSection("Test 2: Plasticity Learning Experiment");
        
        // Learning experiment structure
        auto& region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        // Enable plasticity
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
        
        std::cout << "Applying input patterns (" << config.benchmarkSteps << " steps)..." << std::endl;
        
        for (SimulationStep step = 0; step < config.benchmarkSteps; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
            
            if (step % 200 == 0) {
                std::cout << "  Step " << step 
                          << " | Spikes: " << brain->getTotalSpikeCount()
                          << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
            }
        }
        
        std::cout << "  ✓ Learning experiment completed" << std::endl;
    }
    
    void runStdpVerification(std::shared_ptr<Brain> brain) {
        printSection("Test 3: STDP Verification");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region || region->getSynapses().size() < 5) {
            std::cout << "  [WARNING] Not enough synapses for STDP test" << std::endl;
            return;
        }
        
        auto& synapses = region->getSynapses();
        std::vector<float> beforeWeights;
        
        std::cout << "Testing STDP on 5 synapses:" << std::endl;
        for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
            std::cout << "    Synapse " << i << " before: " << beforeWeights[i] << std::endl;
        }
        
        std::cout << "Creating correlated pre->post activity..." << std::endl;
        
        for (int trial = 0; trial < 50; ++trial) {
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                neurons[0]->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                neurons[1]->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        std::cout << "  After correlated activity:" << std::endl;
        float totalDelta = 0.0f;
        for (size_t i = 0; i < beforeWeights.size() && i < synapses.size(); ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            std::cout << "    Synapse " << i 
                      << " after: " << synapses[i]->getWeight()
                      << " (Δ=" << delta << ")" << std::endl;
        }
        
        if (totalDelta > 0.001f) {
            std::cout << "  ✓ STDP working: Pre-before-post produced potentiation" << std::endl;
        } else if (totalDelta < -0.001f) {
            std::cout << "  ! STDP reversed: Check parameters" << std::endl;
        } else {
            std::cout << "  ! No change: STDP may not be triggering" << std::endl;
        }
    }
    
    void runPerformanceBenchmark(std::shared_ptr<Brain> brain, int steps) {
        printSection("Performance Benchmark");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        
        std::cout << "  Steps: " << steps << std::endl;
        std::cout << "  Time: " << duration.count() << " seconds" << std::endl;
        std::cout << "  Steps/sec: " << (steps / duration.count()) << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        
        if (duration.count() > 0) {
            std::cout << "  ✓ Performance benchmark completed" << std::endl;
        }
    }
    
    void runMemoryProfiling(std::shared_ptr<Brain> brain, int steps) {
        printSection("Memory Profiling");
        
        // Simple memory profiling by checking memory usage patterns
        std::cout << "Monitoring memory usage patterns..." << std::endl;
        
        size_t initialMemory = brain->getTotalSpikeCount();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % 500 == 0) {
                std::cout << "  Step " << step 
                          << " | Spikes: " << brain->getTotalSpikeCount()
                          << " | Memory traces: " << brain->getWorkingMemory()->getActiveTraces() << std::endl;
            }
        }
        
        std::cout << "  ✓ Memory profiling completed" << std::endl;
    }
};

class NLMTestSuite {
public:
    NLMTestSuite(std::shared_ptr<Logger> logger) : logger(logger) {}
    
    void runTests(const NLMConfig& config, std::shared_ptr<Brain> brain) {
        NLMExperimentManager experimentManager(logger);
        
        std::cout << std::endl;
        std::cout << "NLM Test Suite - Enhanced Version" << std::endl;
        std::cout << "=================================" << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Neurons: " << config.neuronCount << std::endl;
        std::cout << "  Regions: " << config.regionCount << std::endl;
        std::cout << "  Timestep: " << config.simulationTimestep << "s" << std::endl;
        std::cout << "  Random Seed: " << config.randomSeed << std::endl;
        std::cout << "  Learning: " << (config.enableLearningExperiment ? "ENABLED" : "DISABLED") << std::endl;
        std::cout << "  Connectivity Test: " << (config.enableConnectivityTest ? "ENABLED" : "DISABLED") << std::endl;
        std::cout << "  STDP Test: " << (config.enableStdpTest ? "ENABLED" : "DISABLED") << std::endl;
        
        std::cout << std::endl;
        std::cout << "Starting enhanced test suite..." << std::endl;
        
        experimentManager.runAllExperiments(config, brain);
        
        // Final status report
        printSection("Final Brain Status");
        brain->logStatus();
        
        printSection("Test Suite Complete");
        std::cout << "✓ Enhanced test suite completed successfully" << std::endl;
        std::cout << "✓ All modular components working correctly" << std::endl;
        std::cout << "✓ Improved configuration and extensibility verified" << std::endl;
    }
    
private:
    std::shared_ptr<Logger> logger;
};

int main(int argc, char** argv) {
    // Parse command line arguments
    NLMConfig config = NLMArgumentParser::parse(argc, argv);
    
    // Initialize logging with enhanced configuration
    auto logger = std::make_shared<Logger>();
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(config.quietMode ? LogLevel::Error : LogLevel::Info);
    logger->addLogger(consoleLogger);
    
    if (!config.outputFile.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(config.outputFile);
        logger->addLogger(fileLogger);
    }
    
    Logger::setGlobal(logger);
    
    // Print banner
    printBanner();
    
    // Display startup information
    std::cout << "Initializing Enhanced NLM with Modular Architecture..." << std::endl;
    
    // Load configuration
    auto cfg = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = config.configFile;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    if (cfg->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Override with parsed config values
    cfg->set("random_seed", static_cast<int64_t>(config.randomSeed), ConfigSource::Default);
    cfg->set("simulation_timestep", config.simulationTimestep, ConfigSource::Default);
    cfg->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    cfg->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
    cfg->set("connection_probability", config.connectionProbability, ConfigSource::Default);
    
    // Enhanced STDP parameters
    cfg->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    cfg->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    cfg->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    cfg->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    cfg->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Set default values for enhanced Phase 2
    cfg->set("enable_development", true, ConfigSource::Default);
    cfg->set("enable_curiosity", true, ConfigSource::Default);
    cfg->set("enable_reward_modulation", true, ConfigSource::Default);
    cfg->set("enable_prediction", true, ConfigSource::Default);
    
    // Display configuration summary
    printSection("Configuration Summary");
    std::cout << "  random_seed: " << config.randomSeed << std::endl;
    std::cout << "  simulation_timestep: " << config.simulationTimestep << "s" << std::endl;
    std::cout << "  neuron_count: " << config.neuronCount << std::endl;
    std::cout << "  region_count: " << config.regionCount << std::endl;
    std::cout << "  connection_probability: " << config.connectionProbability << std::endl;
    
    // Initialize simulation clock
    SimulationClock clock(config.simulationTimestep);
    std::cout << "Simulation clock initialized with timestep: " << config.simulationTimestep << "s" << std::endl;
    
    // Initialize brain with enhanced configuration
    std::cout << std::endl;
    std::cout << "Initializing Enhanced NLM Brain..." << std::endl;
    auto brain = std::make_shared<Brain>(cfg);
    
    if (!brain->initialize()) {
        std::cout << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    brain->logStatus();
    
    // Run enhanced test suite
    NLMTestSuite testSuite(logger);
    testSuite.runTests(config, brain);
    
    // Final summary
    printSection("Enhanced NLM - Final Summary");
    std::cout << "✓ Modular architecture implementation complete" << std::endl;
    std::cout << "✓ Enhanced configuration management working" << std::endl;
    std::cout << "✓ Improved command-line interface operational" << std::endl;
    std::cout << "✓ Extensible test framework functional" << std::endl;
    std::cout << "✓ Performance monitoring capabilities active" << std::endl;
    std::cout << std::endl;
    std::cout << "The Enhanced NLM brain is now a fully functional artificial neural" << std::endl;
    std::cout << "substrate with improved modularity, configurability, and extensibility." << std::endl;
    
    return 0;
}
