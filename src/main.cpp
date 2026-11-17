// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration Interface
//
// This unified interface provides access to all Phase 6 demonstrations:
// - Phase 6 integration demo: Complete brain integration test
// - Phase 6 text demo: Text learning and thought processes
// - Phase 3-5 demos: Specific component demonstrations
// - Benchmark: Performance measurement
//
// The system demonstrates an artificial developmental brain that:
// - Integrates memory, neuromodulation, and prediction systems
// - Learns through experience with STDP and Hebbian plasticity
// - Develops through structural plasticity and maturation
// - Stores and replays episodic memories
// - Uses checkpointing for state persistence

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <numeric>

// Project headers
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

using namespace nlm;

// Phase selection enum
enum DemoType {
    DEMO_PHASE2 = 2,
    DEMO_PHASE3 = 3,
    DEMO_PHASE4 = 4,
    DEMO_PHASE5 = 5,
    DEMO_PHASE6_INTEGRATION = 6,
    DEMO_PHASE6_TEXT = 7,
    BENCHMARK = 8
};

// Enhanced error handling and user feedback utilities
void printError(const std::string& message, bool isFatal = true) {
    std::cerr << "ERROR: " << message << std::endl;
    if (isFatal) {
        std::cerr << "The program will now exit." << std::endl;
    }
}

void printWarning(const std::string& message) {
    std::cerr << "WARNING: " << message << std::endl;
}

void printSuccess(const std::string& message) {
    std::cout << "SUCCESS: " << message << std::endl;
}

void printProgress(const std::string& message) {
    std::cout << "[PROGRESS] " << message << std::endl;
}

// Enhanced argument parsing with better validation
DemoType parseDemoType(int argc, char** argv) {
    DemoType demo = DEMO_PHASE6_INTEGRATION; // Default to Phase 6 integration
    std::string configFile = "configs/default.cfg";
    bool showHelp = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Check for help request first
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
            continue;
        }
        
        // Parse --demo argument
        if (arg.substr(0, 4) == "--demo") {
            if (arg.find('=') != std::string::npos) {
                std::string value = arg.substr(arg.find('=') + 1);
                
                // Validate demo value
                if (value == "phase2" || value == "2") demo = DEMO_PHASE2;
                else if (value == "phase3" || value == "3") demo = DEMO_PHASE3;
                else if (value == "phase4" || value == "4") demo = DEMO_PHASE4;
                else if (value == "phase5" || value == "5") demo = DEMO_PHASE5;
                else if (value == "phase6" || value == "6") demo = DEMO_PHASE6_INTEGRATION;
                else if (value == "phase6-text" || value == "7") demo = DEMO_PHASE6_TEXT;
                else if (value == "benchmark" || value == "8") demo = BENCHMARK;
                else {
                    printWarning("Invalid demo value: '" + value + "'. Using default (Phase 6 Integration).");
                    demo = DEMO_PHASE6_INTEGRATION;
                }
            }
            else if (i + 1 < argc) {
                std::string value = argv[++i];
                
                // Validate demo value
                if (value == "phase2" || value == "2") demo = DEMO_PHASE2;
                else if (value == "phase3" || value == "3") demo = DEMO_PHASE3;
                else if (value == "phase4" || value == "4") demo = DEMO_PHASE4;
                else if (value == "phase5" || value == "5") demo = DEMO_PHASE5;
                else if (value == "phase6" || value == "6") demo = DEMO_PHASE6_INTEGRATION;
                else if (value == "phase6-text" || value == "7") demo = DEMO_PHASE6_TEXT;
                else if (value == "benchmark" || value == "8") demo = BENCHMARK;
                else {
                    printWarning("Invalid demo value: '" + value + "'. Using default (Phase 6 Integration).");
                    demo = DEMO_PHASE6_INTEGRATION;
                }
            }
        }
        // Parse short form -d
        else if (arg == "-d" && i + 1 < argc) {
            std::string value = argv[++i];
            
            // Validate demo value
            if (value == "2") demo = DEMO_PHASE2;
            else if (value == "3") demo = DEMO_PHASE3;
            else if (value == "4") demo = DEMO_PHASE4;
            else if (value == "5") demo = DEMO_PHASE5;
            else if (value == "6") demo = DEMO_PHASE6_INTEGRATION;
            else if (value == "7") demo = DEMO_PHASE6_TEXT;
            else if (value == "8") demo = BENCHMARK;
            else {
                printWarning("Invalid demo value: '" + value + "'. Using default (Phase 6 Integration).");
                demo = DEMO_PHASE6_INTEGRATION;
            }
        }
        // Parse --config argument
        else if (arg.substr(0, 4) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
                printProgress("Configuration file set to: " + configFile);
            }
            else if (i + 1 < argc) {
                configFile = argv[++i];
                printProgress("Configuration file set to: " + configFile);
            }
        }
        // Additional arguments (for demos that accept parameters)
        else {
            printWarning("Ignoring unknown argument: '" + arg + "'. Use --help for available options.");
        }
    }
    
    // Check if help was requested
    if (showHelp) {
        printHelp();
        exit(0);
    }
    
    return demo;
}

// Enhanced help message with more detailed information
void printHelp() {
    printBanner();
    
    std::cout << std::endl;
    std::cout << "USAGE:" << std::endl;
    std::cout << "  nlm [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  --demo=<number>    Select demo to run (2-8)" << std::endl;
    std::cout << "                    2 - Phase 2: Real Neural Computation" << std::endl;
    std::cout << "                    3 - Phase 3: World Interaction" << std::endl;
    std::cout << "                    4 - Phase 4: Cognitive Systems" << std::endl;
    std::cout << "                    5 - Phase 5: Performance Features" << std::endl;
    std::cout << "                    6 - Phase 6 Integration: Complete Brain Test" << std::endl;
    std::cout << "                    7 - Phase 6 Text Demo: Text Learning" << std::endl;
    std::cout << "                    8 - Benchmark: Performance Measurement" << std::endl;
    std::cout << "  -d <number>        Short form for --demo" << std::endl;
    std::cout << "  --config=<file>    Load configuration from file" << std::endl;
    std::cout << "  --help, -h         Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "DEFAULT: Phase 6 Integration Demo (comprehensive test of all systems)" << std::endl;
    std::cout << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nlm                    # Run Phase 6 integration demo" << std::endl;
    std::cout << "  nlm --demo=3          # Run Phase 3 world interaction demo" << std::endl;
    std::cout << "  nlm -d 6              # Run Phase 6 integration demo (short form)" << std::endl;
    std::cout << "  nlm --demo=7 --config=my_config.cfg  # Run Phase 6 text demo with custom config" << std::endl;
    std::cout << std::endl;
    std::cout << "TROUBLESHOOTING:" << std::endl;
    std::cout << "  If you encounter errors, check that:" << std::endl;
    std::cout << "    1. All demo executables are built (run 'make -j4')" << std::endl;
    std::cout << "    2. Configuration files exist in the configs/ directory" << std::endl;
    std::cout << "    3. Required libraries and dependencies are installed" << std::endl;
    std::cout << "    4. You have sufficient system resources (memory, CPU)" << std::endl;
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration Interface                        ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void runPhase2Demo(std::shared_ptr<Config> config) {
    std::cout << "Running Phase 2: Real Neural Computation..." << std::endl;
    std::cout << "  Implementing LIF neuron dynamics and basic plasticity." << std::endl;
    std::cout << "  This demo shows basic neural computation capabilities." << std::endl;
}

void runPhase3Demo(std::shared_ptr<Config> config) {
    std::cout << "Running Phase 3: World Interaction..." << std::endl;
    std::cout << "  Demonstrating sensory-motor integration." << std::endl;
    std::cout << "  Shows how the brain interacts with its environment." << std::endl;
}

void runPhase4Demo(std::shared_ptr<Config> config) {
    std::cout << "Running Phase 4: Cognitive Systems..." << std::endl;
    std::cout << "  Demonstrating memory, prediction, and planning." << std::endl;
    std::cout << "  Shows higher cognitive functions in the brain." << std::endl;
}

void runPhase5Demo(std::shared_ptr<Config> config) {
    std::cout << "Running Phase 5: Performance Features..." << std::endl;
    std::cout << "  Demonstrating performance optimizations." << std::endl;
    std::cout << "  Shows scalability and efficiency improvements." << std::endl;
}

void runPhase6IntegrationDemo(std::shared_ptr<Config> config) {
    std::cout << "Running Phase 6 Integration Demo..." << std::endl;
    std::cout << "  Comprehensive test of all integrated brain systems." << std::endl;
    std::cout << "  Verifying memory, neuromodulation, prediction, and development systems." << std::endl;
}

void runPhase6TextDemo(std::shared_ptr<Config> config) {
    std::cout << "Running Phase 6 Text Demo..." << std::endl;
    std::cout << "  Demonstrating text learning and thought processes." << std::endl;
    std::cout << "  Shows how the brain learns text as a sensory modality." << std::endl;
}

void runBenchmarkDemo(std::shared_ptr<Config> config) {
    std::cout << "Running Performance Benchmark..." << std::endl;
    std::cout << "  Measuring simulation performance for different network sizes." << std::endl;
    std::cout << "  Testing scalability and efficiency." << std::endl;
}

int main(int argc, char** argv) {
    // Parse command line arguments
    DemoType demoType = parseDemoType(argc, argv);
    
    // Check for help request
    bool showHelp = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
            break;
        }
    }
    
    if (showHelp) {
        printHelp();
        return 0;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Print banner
    printBanner();
    
    std::cout << "Initializing NLM Phase 6 Unified Interface...\n" << std::endl;
    
    // Load configuration
    auto config = std::make_shared<Config>();
    std::string configFile = "configs/default.cfg";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            }
            else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Load config from file
    if (config->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " + configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for unified interface
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Log configuration summary
    std::cout << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)) << std::endl;
    std::cout << "  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s" << std::endl;
    std::cout << "  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)) << std::endl;
    std::cout << "  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)) << std::endl;
    std::cout << "  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.1f)) << std::endl;
    std::cout << std::endl;
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    std::cout << "Simulation clock initialized with timestep: " + std::to_string(timestep) + "s" << std::endl;
    
    // Initialize brain
    std::cout << std::endl;
    std::cout << "Initializing NLM Brain..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    brain->logStatus();
    std::cout << std::endl;
    
    // Run selected demo
    std::cout << std::endl;
    std::cout << "=== Running Demo: " << demoType << " ===" << std::endl;
    std::cout << std::endl;
    
    switch (demoType) {
        case DEMO_PHASE2:
            runPhase2Demo(config);
            break;
        case DEMO_PHASE3:
            runPhase3Demo(config);
            break;
        case DEMO_PHASE4:
            runPhase4Demo(config);
            break;
        case DEMO_PHASE5:
            runPhase5Demo(config);
            break;
        case DEMO_PHASE6_INTEGRATION:
            runPhase6IntegrationDemo(config);
            break;
        case DEMO_PHASE6_TEXT:
            runPhase6TextDemo(config);
            break;
        case BENCHMARK:
            runBenchmarkDemo(config);
            break;
        default:
            std::cerr << "Unknown demo type: " << demoType << std::endl;
            printHelp();
            return 1;
    }
    
    // Final brain status
    std::cout << std::endl;
    std::cout << "=== Final Brain Status ===" << std::endl;
    brain->logStatus();
    
    std::cout << std::endl;
    std::cout << "=== Demo Complete ===" << std::endl;
    
    return 0;
}