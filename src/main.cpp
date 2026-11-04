// NLM (熙然) - Neural Learning Machine
// Phase 6: Unified Demo System
//
// This phase implements a comprehensive unified demo system that provides:
// - Consistent interfaces for running brain demonstrations
// - Multiple demo types (basic, plasticity, integration, advanced)
// - Advanced configuration and user interface
// - Better documentation and examples

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
#include "unified_demo.cpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>

using namespace nlm;

// Unified command line argument parser
struct UnifiedDemoArgs {
    std::string demoType = "basic";
    uint64_t steps = 1000;
    size_t neurons = 500;
    size_t regions = 1;
    float connectionProb = 0.15f;
    bool enableCheckpointing = false;
    bool enableDevelopment = false;
    bool enableCuriosity = false;
    bool enableReward = false;
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    std::string checkpointPath = "./checkpoint.bin";
    std::string demoMode = "auto";
    bool verbose = true;
    bool quiet = false;
    std::vector<std::string> customArgs;
};

UnifiedDemoArgs parseUnifiedArgs(int argc, char** argv) {
    UnifiedDemoArgs args;
    
    std::map<std::string, std::string> argMap;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t eqPos = arg.find('=');
            if (eqPos != std::string::npos) {
                std::string key = arg.substr(2, eqPos - 2);
                std::string value = arg.substr(eqPos + 1);
                argMap[key] = value;
            } else {
                argMap[arg.substr(2)] = "true";
            }
        } else if (arg.substr(0, 1) == "-") {
            std::string key = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                argMap[key] = argv[++i];
            } else {
                argMap[key] = "true";
            }
        } else {
            args.customArgs.push_back(arg);
        }
    }
    
    // Process mapped arguments
    if (argMap.find("demo") != argMap.end()) args.demoType = argMap["demo"];
    if (argMap.find("steps") != argMap.end()) args.steps = std::stoul(argMap["steps"]);
    if (argMap.find("neurons") != argMap.end()) args.neurons = static_cast<size_t>(std::stoul(argMap["neurons"]));
    if (argMap.find("regions") != argMap.end()) args.regions = static_cast<size_t>(std::stoul(argMap["regions"]));
    if (argMap.find("connection_prob") != argMap.end()) args.connectionProb = std::stof(argMap["connection_prob"]);
    if (argMap.find("checkpoint") != argMap.end() || argMap.find("checkpointing") != argMap.end()) args.enableCheckpointing = true;
    if (argMap.find("development") != argMap.end()) args.enableDevelopment = true;
    if (argMap.find("curiosity") != argMap.end()) args.enableCuriosity = true;
    if (argMap.find("reward") != argMap.end()) args.enableReward = true;
    if (argMap.find("verbose") != argMap.end() && argMap["verbose"] != "false" && argMap["verbose"] != "0") args.verbose = true;
    if (argMap.find("quiet") != argMap.end() || argMap.find("silent") != argMap.end()) args.quiet = true;
    if (argMap.find("config") != argMap.end()) args.configFile = argMap["config"];
    if (argMap.find("output") != argMap.end()) args.outputFile = argMap["output"];
    if (argMap.find("checkpoint") != argMap.end()) args.checkpointPath = argMap["checkpoint"];
    if (argMap.find("mode") != argMap.end()) args.demoMode = argMap["mode"];
    
    return args;
}

void printDemoBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Unified Demo System                              ║
    ║                                                               ║
    ║     Advanced artificial developmental brain demonstrations.   ║
    ║     Features: Memory, Prediction, Cognition, Neuromodulation. ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printDemoHelp() {
    std::cout << R"(
=== NLM Unified Demo System ===

Demo Types:
  basic      - Simple brain functionality (Phase 2)
  plasticity - Learning and synaptic changes (STDP/Hebbian)
  integration - Full system integration (Phase 6)
  advanced   - All features enabled

Usage:
  ./nlm [options]

Options:
  --demo=<type>        Demo type (basic, plasticity, integration, advanced)
  --steps=<n>          Number of simulation steps (default: 1000)
  --neurons=<n>        Number of neurons (default: 500)
  --regions=<n>        Number of brain regions (default: 1)
  --connection_prob=<f> Connection probability (default: 0.15)
  --checkpoint         Enable checkpointing
  --development        Enable brain development
  --curiosity          Enable curiosity-driven exploration
  --reward             Enable reward-based learning
  --verbose            Verbose output (default)
  --quiet, --silent    Quiet mode
  --config=<file>      Configuration file (default: configs/default.cfg)
  --checkpoint=<path>  Checkpoint file path (default: ./checkpoint.bin)
  --mode=<mode>        Demo mode: auto, batch, interactive

Examples:
  ./nlm --demo=basic --steps=500
  ./nlm --demo=plasticity --neurons=200 --development
  ./nlm --demo=integration --checkpoint --curiosity
  ./nlm --demo=advanced --steps=2000 --reward --development

For more information, see:
  - docs/UNIFIED_DEMO.md
  - docs/DEMOS.md
  - easy_usage.md
)" << std::endl;
}

int main(int argc, char** argv) {
    printDemoBanner();
    
    std::cout << "Initializing NLM Unified Demo System...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line arguments
    UnifiedDemoArgs args = parseUnifiedArgs(argc, argv);
    
    // Handle help
    if (argc == 1 || 
        (args.customArgs.size() == 1 && (args.customArgs[0] == "--help" || args.customArgs[0] == "-h" || args.customArgs[0] == "help"))) {
        printDemoHelp();
        return 0;
    }
    
    // Handle quiet mode
    if (args.quiet) {
        consoleLogger->setLevel(LogLevel::Error);
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    if (config->loadFromFile(args.configFile)) {
        if (args.verbose) {
            std::cout << "Loaded configuration from: " << args.configFile << std::endl;
        }
    } else {
        if (args.verbose) {
            std::cout << "Using default configuration (file not found: " << args.configFile << ")" << std::endl;
        }
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set demo-specific parameters
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(args.neurons), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(args.regions), ConfigSource::Default);
    config->set("connection_probability", args.connectionProb, ConfigSource::Default);
    
    // Enable plasticity rules for all demos
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Enable structural plasticity
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    if (args.verbose) {
        std::cout << "\nConfiguration:" << std::endl;
        std::cout << "  random_seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
        std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
        std::cout << "  neuron_count: " << config->getOr<int64_t>"("neuron_count", 500) << std::endl;
        std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
        std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.15f) << std::endl;
        std::cout << "  demo_type: " << args.demoType << std::endl;
        std::cout << std::endl;
    }
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    
    if (args.verbose) {
        std::cout << "Simulation clock initialized with timestep: " << timestep << "s" << std::endl;
    }
    
    // Create unified demo configuration
    nlm::DemoConfig demoConfig;
    demoConfig.demoType = args.demoType;
    demoConfig.maxSteps = args.steps;
    demoConfig.neuronCount = args.neurons;
    demoConfig.regionCount = args.regions;
    demoConfig.connectionProbability = args.connectionProb;
    demoConfig.enableCheckpointing = args.enableCheckpointing;
    demoConfig.enableDevelopment = args.enableDevelopment;
    demoConfig.enableCuriosity = args.enableCuriosity;
    demoConfig.enableReward = args.enableReward;
    demoConfig.checkpointPath = args.checkpointPath;
    demoConfig.outputPrefix = args.outputFile;
    demoConfig.verbose = args.verbose && !args.quiet;
    
    // Run unified demo
    NLM_LOG_INFO("=== NLM Unified Demo System ===");
    NLM_LOG_INFO("Demo: " + args.demoType);
    NLM_LOG_INFO("Steps: " + std::to_string(args.steps));
    NLM_LOG_INFO("Neurons: " + std::to_string(args.neurons));
    
    UnifiedDemo unifiedDemo;
    bool success = unifiedDemo.run(demoConfig);
    
    if (args.verbose) {
        std::cout << "\n=== Demo Complete ===" << std::endl;
    }
    
    if (success) {
        NLM_LOG_INFO("Demo completed successfully!");
        return 0;
    } else {
        NLM_LOG_ERROR("Demo failed!");
        return 1;
    }
}
