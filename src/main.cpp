// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "core/ConfigurationManager/ConfigurationManager.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "core/Constants.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <cstdlib>

using namespace nlm;

// Forward declaration of helper functions
void printBanner();
void printHelp(const std::string& programName);
void printVersion();

// Main entry point
int main(int argc, char** argv) {
    printBanner();
    
    try {
        // Initialize logging system
        initializeLogging();
        
        // Create and configure configuration manager
        auto configManager = std::make_shared<ConfigurationManager>();
        
        // Parse command line arguments
        if (!parseCommandLine(argc, argv, *configManager)) {
            return 1; // Help requested or error
        }
        
        // Load configuration from file and validate
        if (!loadConfiguration(*configManager)) {
            NLM_LOG_ERROR("Failed to load or validate configuration");
            return 1;
        }
        
        // Print configuration summary
        configManager->printSummary();
        
        // Initialize simulation components
        if (!initializeSimulation(*configManager)) {
            NLM_LOG_ERROR("Failed to initialize simulation components");
            return 1;
        }
        
        // Create and configure experiment runner
        auto experimentRunner = std::make_shared<ExperimentRunner>();
        
        // Run experiments in sequence
        if (!runAllExperiments(*configManager, *experimentRunner)) {
            NLM_LOG_ERROR("Experiments failed to complete successfully");
            return 1;
        }
        
        // Print final results
        printFinalResults(*configManager, *experimentRunner);
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Phase 2 Complete ===");
        NLM_LOG_INFO("");

        NLM_LOG_INFO("Phase 2 Objectives Completed:");
        NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
        NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
        NLM_LOG_INFO("  ✓ STDP plasticity rule");
        NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
        NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
        NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
        NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
        NLM_LOG_INFO("");

        NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
        NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
        NLM_LOG_INFO("");

        
        return 0;
        
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL("Fatal error in main execution: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_CRITICAL("Unknown fatal error occurred");
        return 1;
    }
}

// Helper functions implementation

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp(const std::string& programName) {
    std::cout << "NLM (熙然) - Neural Learning Machine Phase 2" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version, -v           Show version information" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --seed <num>            Set random seed (default: " 
              << constants::DEFAULT_RANDOM_SEED << ")" << std::endl;
    std::cout << "  --neurons <num>         Set neuron count (default: " 
              << constants::DEFAULT_NEURON_COUNT << ")" << std::endl;
    std::cout << "  --regions <num>         Set region count (default: " 
              << constants::DEFAULT_REGION_COUNT << ")" << std::endl;
    std::cout << "  --verbose               Enable verbose logging" << std::endl;
    std::cout << "  --timestep <float>       Set simulation timestep (default: " 
              << constants::DEFAULT_SIMULATION_TIMESTEP << ")" << std::endl;
    std::cout << "  --plasticity-enable     Enable plasticity (default: true)" << std::endl;
    std::cout << "  --stdp-enable           Enable STDP (default: true)" << std::endl;
    std::cout << "  --structural-plasticity-enable  Enable structural plasticity (default: true)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration File:" << std::endl;
    std::cout << "  Can specify custom config file with --config" << std::endl;
    std::cout << "  Default: configs/default.cfg" << std::endl;
    std::cout << std::endl;
    
    ConfigurationManager::printHelp();
}

void printVersion() {
    std::cout << "NLM Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Version: 2.0.0" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Phase 2 implements real spiking neural computation with LIF neurons, "
              << "STDP plasticity, and structural development." << std::endl;
}

bool parseCommandLine(int argc, char** argv, ConfigurationManager& configManager) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp(argv[0]);
            return false;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return false;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                std::string configFile = argv[++i];
                configManager.loadFromFile(configFile);
            }
        } else if (arg.find("--seed=") == 0) {
            try {
                int64_t seed = std::stoll(arg.substr(6));
                configManager.getConfig()->set("random_seed", seed, ConfigSource::CommandLine);
            } catch (...) {
                NLM_LOG_WARNING("Invalid seed value: " + arg.substr(6));
            }
        } else if (arg.find("--neurons=") == 0) {
            try {
                int64_t count = std::stoll(arg.substr(9));
                configManager.getConfig()->set("neuron_count", count, ConfigSource::CommandLine);
            } catch (...) {
                NLM_LOG_WARNING("Invalid neuron count: " + arg.substr(9));
            }
        } else if (arg.find("--regions=") == 0) {
            try {
                int64_t count = std::stoll(arg.substr(9));
                configManager.getConfig()->set("region_count", count, ConfigSource::CommandLine);
            } catch (...) {
                NLM_LOG_WARNING("Invalid region count: " + arg.substr(9));
            }
        } else if (arg.find("--timestep=") == 0) {
            try {
                double timestep = std::stod(arg.substr(11));
                configManager.getConfig()->set("simulation_timestep", timestep, ConfigSource::CommandLine);
            } catch (...) {
                NLM_LOG_WARNING("Invalid timestep: " + arg.substr(11));
            }
        } else if (arg == "--verbose") {
            configManager.getConfig()->set("verbose", true, ConfigSource::CommandLine);
        } else if (arg == "--plasticity-enable") {
            configManager.getConfig()->set("plasticity_learning_rate", 0.01f, ConfigSource::CommandLine);
        } else if (arg == "--stdp-enable") {
            configManager.getConfig()->set("stdp_ltp_weight", constants::DEFAULT_STDP_LTP_WEIGHT, ConfigSource::CommandLine);
            configManager.getConfig()->set("stdp_ltd_weight", constants::DEFAULT_STDP_LTD_WEIGHT, ConfigSource::CommandLine);
        } else if (arg == "--structural-plasticity-enable") {
            configManager.getConfig()->set("synaptogenesis_rate", constants::DEFAULT_SYNAPTONGENESIS_RATE, ConfigSource::CommandLine);
            configManager.getConfig()->set("pruning_rate", constants::DEFAULT_PRUNING_RATE, ConfigSource::CommandLine);
        }
    }
    
    return true;
}

bool loadConfiguration(ConfigurationManager& configManager) {
    if (!configManager.getConfig()->loadFromFile("configs/default.cfg")) {
        NLM_LOG_INFO("Using default configuration (config file not found)");
    }
    
    return configManager.validate();
}

bool initializeLogging() {
    try {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
        NLM_LOG_INFO("Initializing logging system...");
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize logging: " << e.what() << std::endl;
        return false;
    }
}

bool initializeSimulation(ConfigurationManager& configManager) {
    NLM_LOG_INFO("");;
    NLM_LOG_INFO("Initializing simulation components...");
    
    // Initialize random number generator
    uint64_t seed = configManager.getOr<int64_t>("random_seed", constants::DEFAULT_RANDOM_SEED);
    Random::setSeed(seed);
    NLM_LOG_INFO("Random seed set to: " + std::to_string(seed));
    
    // Initialize simulation clock
    double timestep = configManager.getOr<double>("simulation_timestep", constants::DEFAULT_SIMULATION_TIMESTEP);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");;
    NLM_LOG_INFO("Initializing NLM Brain...");
    
    try {
        auto brain = std::make_shared<Brain>(configManager.getConfig());
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return false;
        }
        
        brain->logStatus();
        
        // Store brain as global static for experiment runner (temporary solution)
        // In a real implementation, we would pass brain by reference or use dependency injection
        setBrainForExperiments(brain);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during brain initialization: " + std::string(e.what()));
        return false;
    }
    
    return true;
}

bool runAllExperiments(ConfigurationManager& configManager, ExperimentRunner& experimentRunner) {
    NLM_LOG_INFO("");;
    NLM_LOG_INFO("Starting experiments...");
    
    // Get brain from static variable (temporary solution)
    auto brain = getBrainForExperiments();
    if (!brain) {
        NLM_LOG_ERROR("Brain not available for experiments");
        return false;
    }
    
    bool allExperimentsSuccessful = true;
    
    // Experiment 1: Basic Connectivity Test
    NLM_LOG_INFO("=" + std::string(60, '='));
    NLM_LOG_INFO("Running Experiment 1: Basic Neural Connectivity");
    NLM_LOG_INFO("=" + std::string(60, '='));
    
    if (!experimentRunner.runBasicConnectivityTest(brain, *configManager.getConfig())) {
        NLM_LOG_WARNING("Basic connectivity test failed");
        allExperimentsSuccessful = false;
    }
    
    // Reset brain for next experiment
    experimentRunner.resetBrain(brain);
    
    // Experiment 2: Plasticity Learning Experiment
    NLM_LOG_INFO("=" + std::string(60, '='));
    NLM_LOG_INFO("Running Experiment 2: Plasticity Learning Experiment");
    NLM_LOG_INFO("=" + std::string(60, '='));
    
    if (!experimentRunner.runPlasticityExperiment(brain, *configManager.getConfig())) {
        NLM_LOG_WARNING("Plasticity experiment failed");
        allExperimentsSuccessful = false;
    }
    
    // Reset brain for next experiment
    experimentRunner.resetBrain(brain);
    
    // Experiment 3: STDP Verification
    NLM_LOG_INFO("=" + std::string(60, '='));
    NLM_LOG_INFO("Running Experiment 3: STDP Verification");
    NLM_LOG_INFO("=" + std::string(60, '='));
    
    if (!experimentRunner.runStdpVerification(brain, *configManager.getConfig())) {
        NLM_LOG_WARNING("STDP verification experiment failed");
        allExperimentsSuccessful = false;
    }
    
    return allExperimentsSuccessful;
}

void printFinalResults(ConfigurationManager& configManager, ExperimentRunner& experimentRunner) {
    NLM_LOG_INFO("");;
    NLM_LOG_INFO("=== Final Brain Status ===");
    
    auto brain = getBrainForExperiments();
    if (brain) {
        brain->logStatus();
    }
    
    NLM_LOG_INFO("");;
    NLM_LOG_INFO("Configuration used for experiments:");
    
    double timestep = configManager.getOr<double>("simulation_timestep", constants::DEFAULT_SIMULATION_TIMESTEP);
    NLM_LOG_INFO("  Simulation timestep: " + std::to_string(timestep) + "s");
    
    int64_t neuronCount = configManager.getOr<int64_t>("neuron_count", constants::DEFAULT_NEURON_COUNT);
    NLM_LOG_INFO("  Neuron count: " + std::to_string(neuronCount));
    
    int64_t regionCount = configManager.getOr<int64_t>("region_count", constants::DEFAULT_REGION_COUNT);
    NLM_LOG_INFO("  Region count: " + std::to_string(regionCount));
    
    int64_t randomSeed = configManager.getOr<int64_t>("random_seed", constants::DEFAULT_RANDOM_SEED);
    NLM_LOG_INFO("  Random seed: " + std::to_string(randomSeed));
}

// Static variables for temporary brain storage (should be replaced with proper dependency injection)
std::shared_ptr<Brain> gBrain;

void setBrainForExperiments(std::shared_ptr<Brain> brain) {
    gBrain = brain;
}

std::shared_ptr<Brain> getBrainForExperiments() {
    return gBrain;
}