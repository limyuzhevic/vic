// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

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
#include "tests/TestFramework.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <unordered_map>
#include <algorithm>
#include <cctype>

using namespace nlm;

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
    ╚═════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << "NLM Test Runner - Available Options:" << std::endl;
    std::cout << "  --test <name>              Run specific test (use 'all' for all tests)" << std::endl;
    std::cout << "  --verbose                  Enable verbose output" << std::endl;
    std::cout << "  --help                     Show this help message" << std::endl;
    std::cout << "  --version                  Show version information" << std::endl;
    std::cout << std::endl;
    std::cout << "Available tests:" << std::endl;
    std::cout << "  basic-connectivity         Test basic neural signal propagation" << std::endl;
    std::cout << "  plasticity-learning        Test synaptic learning through experience" << std::endl;
    std::cout << "  stdp-verification          Test spike-timing dependent plasticity" << std::endl;
    std::cout << "  memory-integration         Test memory systems integration" << std::endl;
    std::cout << "  neuromodulation-integration  Test neuromodulation systems integration" << std::endl;
    std::cout << "  development-integration    Test developmental system integration" << std::endl;
    std::cout << "  all                        Run all tests" << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "An experimental artificial developmental brain." << std::endl;
    std::cout << "Built with C++20 and comprehensive brain simulation." << std::endl;
}

// Parse command line arguments
struct CommandLineArgs {
    std::string testName;
    bool verbose;
    bool help;
    bool version;
    std::string configFile;
    std::unordered_map<std::string, std::string> extraArgs;
};

CommandLineArgs parseCommandLine(int argc, char** argv) {
    CommandLineArgs args;
    args.verbose = false;
    args.help = false;
    args.version = false;
    args.testName = "all"; // Default: run all tests
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            args.help = true;
        } else if (arg == "--version") {
            args.version = true;
        } else if (arg == "--verbose") {
            args.verbose = true;
        } else if (arg == "--test" && i + 1 < argc) {
            args.testName = argv[++i];
        } else if (arg.find("--config=") == 0) {
            args.configFile = arg.substr(8);
        } else if (arg.find("--") == 0) {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                args.extraArgs[key] = value;
            }
        }
    }
    
    return args;
}

// Convert string to lowercase
std::string toLowercase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return str;
}

// Create and configure brain with given arguments
std::shared_ptr<Brain> createBrainFromArgs(const CommandLineArgs& args) {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    if (!args.configFile.empty()) {
        configFile = args.configFile;
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(0, nullptr); // We'll handle extraArgs differently
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Apply extra arguments from command line
    for (const auto& pair : args.extraArgs) {
        std::string key = pair.first;
        std::string value = pair.second;
        
        try {
            if (key == "neuron_count" || key == "random_seed") {
                config->set(key, static_cast<int64_t>(std::stoll(value)), ConfigSource::CommandLine);
            } else if (key == "connection_probability" || key == "stdp_ltp_weight" || 
                       key == "stdp_ltd_weight" || key == "stdp_tau" ||
                       key == "synaptogenesis_rate" || key == "pruning_rate") {
                config->set(key, static_cast<float>(std::stof(value)), ConfigSource::CommandLine);
            } else if (key == "simulation_timestep") {
                config->set(key, std::stod(value), ConfigSource::CommandLine);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to parse argument " + key + ": " + std::string(e.what()));
        }
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return nullptr;
    }
    
    brain->logStatus();
    return brain;
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Test Framework..." << std::endl << std::endl;
    
    // Parse command line arguments
    CommandLineArgs args = parseCommandLine(argc, argv);
    
    // Handle help/version flags
    if (args.help) {
        printHelp();
        return 0;
    }
    
    if (args.version) {
        printVersion();
        return 0;
    }
    
    std::cout << "Test: " << args.testName << std::endl;
    std::cout << "Verbose: " << (args.verbose ? "Yes" : "No") << std::endl;
    std::cout << "Config file: " << (args.configFile.empty() ? "default" : args.configFile) << std::endl << std::endl;
    
    // Create brain
    auto brain = createBrainFromArgs(args);
    if (!brain) {
        return 1;
    }
    
    // Create test runner
    NLMTestRunner testRunner(args.verbose);
    
    // Create tests based on requested test name
    std::string testKey = toLowercase(args.testName);
    
    if (testKey == "all" || testKey == "all tests") {
        // Add all available tests
        testRunner.addTest(std::make_unique<BasicConnectivityTest>());
        testRunner.addTest(std::make_unique<PlasticityLearningTest>());
        testRunner.addTest(std::make_unique<STDPVerificationTest>());
        testRunner.addTest(std::make_unique<MemorySystemIntegrationTest>());
        testRunner.addTest(std::make_unique<NeuromodulationIntegrationTest>());
        testRunner.addTest(std::make_unique<DevelopmentIntegrationTest>());
    } else if (testKey == "basic-connectivity" || testKey == "basic connectivity") {
        testRunner.addTest(std::make_unique<BasicConnectivityTest>());
    } else if (testKey == "plasticity-learning" || testKey == "plasticity learning") {
        testRunner.addTest(std::make_unique<PlasticityLearningTest>());
    } else if (testKey == "stdp-verification" || testKey == "stdp verification") {
        testRunner.addTest(std::make_unique<STDPVerificationTest>());
    } else if (testKey == "memory-integration" || testKey == "memory integration") {
        testRunner.addTest(std::make_unique<MemorySystemIntegrationTest>());
    } else if (testKey == "neuromodulation-integration" || testKey == "neuromodulation integration") {
        testRunner.addTest(std::make_unique<NeuromodulationIntegrationTest>());
    } else if (testKey == "development-integration" || testKey == "development integration") {
        testRunner.addTest(std::make_unique<DevelopmentIntegrationTest>());
    } else {
        std::cerr << "ERROR: Unknown test name: " << args.testName << std::endl;
        printHelp();
        return 1;
    }
    
    // Run tests
    testRunner.runAllTests(brain);
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("")
    
    return 0;
}