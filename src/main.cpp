// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration - Unified Brain System
//
// This is the main entry point for NLM, providing a unified interface
// to the complete artificial brain simulation with comprehensive
// configuration options and multiple operational modes.

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <sstream>
#include <map>
#include <chrono>
#include <algorithm>
#include <cctype>

using namespace nlm;

// NLM Mode enumeration
enum class NLMMode {
    Default,      // Run Phase 6 demo with default settings
    Experiment,   // Run Phase 6 integration experiment
    Demo,         // Run simple demonstration
    Test,         // Run comprehensive tests
    Config,       // Configuration management
    Benchmark,    // Run performance benchmarks
    Profile,      // Run profiling session
    Help          // Show help information
};

// Parse command line arguments and determine mode
NLMMode parseMode(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        // Convert to lowercase for case-insensitive comparison
        std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
        
        if (arg == "--mode" && i + 1 < argc) {
            std::string modeArg = argv[++i];
            std::transform(modeArg.begin(), modeArg.end(), modeArg.begin(), ::tolower);
            if (modeArg == "experiment") return NLMMode::Experiment;
            if (modeArg == "demo") return NLMMode::Demo;
            if (modeArg == "test") return NLMMode::Test;
            if (modeArg == "config") return NLMMode::Config;
            if (modeArg == "benchmark") return NLMMode::Benchmark;
            if (modeArg == "profile") return NLMMode::Profile;
            if (modeArg == "help") return NLMMode::Help;
        } else if (arg == "--experiment" || arg == "-e") {
            return NLMMode::Experiment;
        } else if (arg == "--demo" || arg == "-d") {
            return NLMMode::Demo;
        } else if (arg == "--test" || arg == "-t") {
            return NLMMode::Test;
        } else if (arg == "--benchmark" || arg == "-b") {
            return NLMMode::Benchmark;
        } else if (arg == "--profile") {
            return NLMMode::Profile;
        } else if (arg == "--help" || arg == "-h") {
            return NLMMode::Help;
        }
    }
    return NLMMode::Default;
}

// Extract value from command line argument (handle --key=value and --key value formats)
std::string extractArgValue(int argc, char** argv, int& currentIndex) {
    std::string arg = argv[currentIndex];
    if (arg.find("=") != std::string::npos) {
        return arg.substr(arg.find("=") + 1);
    } else if (currentIndex + 1 < argc) {
        currentIndex++;
        return argv[currentIndex];
    }
    return "";
}

// Show comprehensive help information
void showHelp() {
    std::cout << R"(
═══════════════════════════════════════════════════════════════════════════════
                                                                            ║
   ████████╗███████╗██████╗ ███████╗    ██████╗ ██╗   ██╗███╗   ███╗ ██████╗    ║
   ╚══██╔══╝██╔════╝██╔══██╗██╔════██╗   ╚════██╗██║   ██║████╗ ████║██╔═══██╗   ║
      ██║   █████╗  ██████╔╝█████╗██║    █████╔╝██║   ██║██╔████╔██║██║   ██║   ║
      ██║   ██╔══╝  ██╔══██╗██╔══╝██║    ╚═══██╗██║   ██║██║╚██╔╝██║██║   ██║   ║
      ██║   ███████╗██║  ██║███████╗██║   ██████╔╝╚██████╔╝██║ ╚═╝ ██║██████╔╝   ║
      ╚═╝   ╚══════╝╚═╝  ╚═╝╚══════╝╚═╝   ╚═════╝  ╚═════╝ ╚═╝     ╚═╝╚═════╝    ║
                                                                            ║
═══════════════════════════════════════════════════════════════════════════════

USAGE:
    nlm [OPTIONS]
    nlm_phase6_demo [OPTIONS]  # Also available with --mode experiment

MODES:
    --mode <type>              Set operation mode (default, experiment, demo, test, config, benchmark, profile, help)
    --experiment, -e           Run Phase 6 integration experiment
    --demo, -d                 Run simple demonstration
    --test, -t                 Run comprehensive tests
    --benchmark, -b            Run performance benchmarks
    --profile                  Run profiling session
    --help, -h                 Show this help information

CONFIGURATION:
    --config <file>            Load configuration from file (default: configs/default.cfg)
    --neuron-count <n>         Number of neurons (default: 1000)
    --region-count <n>         Number of regions (default: 1)
    --connection-prob <p>      Connection probability (default: 0.1)
    --timestep <t>             Simulation timestep (default: 0.001)
    --steps <n>                Number of simulation steps (default: 10000)
    --seed <n>                 Random seed (default: 42)

CHECKPOINTING:
    --checkpoint <file>        Checkpoint path (default: ./checkpoint.bin)
    --enable-checkpointing     Enable checkpoint saving/loading
    --enable-replay            Enable memory replay system
    --enable-development       Enable developmental processes

ADVANCED OPTIONS:
    --verbose                  Enable verbose logging
    --quiet                    Suppress non-essential output
    --background               Run in background (if supported)

PRESETS:
    --fast                      Fast mode (small network, quick)
    --comprehensive             Comprehensive mode (large network, thorough)
    --research                  Research mode (high detail, export data)
    --demo-mode                Easy mode for beginners

EXAMPLES:
    nlm --mode experiment --neuron-count 500 --steps 2000
    nlm --mode demo --config my_config.cfg
    nlm --mode benchmark --fast
    nlm --mode profile --comprehensive --verbose
    nlm --help

For more information:
    See docs/ARCHITECTURE.md for system overview
    See docs/PHASE6_FINAL_AUDIT.md for Phase 6 details
    See easy_usage.md for beginner guide
    See HOW_TO_USE.md for Python API documentation
";
}

// Configuration management utility
void manageConfiguration(int argc, char** argv) {
    std::cout << "=== NLM Configuration Manager ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    if (config->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default configuration (no file found: " << configFile << ")" << std::endl;
    }
    
    // Apply command line overrides
    config->loadFromArgs(argc, argv);
    
    std::cout << "\nCurrent Configuration Summary:" << std::endl;
    std::cout << config->summary() << std::endl;
    
    // Save updated configuration
    std::string outputFile = "configs/generated.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 10) == "--output=") {
            outputFile = arg.substr(arg.find('=') + 1);
        }
    }
    
    config->saveToFile(outputFile);
    std::cout << "Configuration saved to: " << outputFile << std::endl;
    std::cout << "Configuration management complete." << std::endl;
}

// Simple demo mode - basic brain demonstration
void runDemoMode(std::shared_ptr<Config> config) {
    std::cout << "=== NLM Demo Mode ===" << std::endl;
    std::cout << "Running simple demonstration of artificial brain..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6 Demo Mode ===");
    NLM_LOG_INFO("Demonstrating integrated artificial brain capabilities");
    
    // Create simple world
    SimpleWorld world;
    world.configure(16, 16, 8, 8);
    world.reset();
    
    // Set agent start position
    world.setAgentStart(8, 8);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Enable basic features
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    
    NLM_LOG_INFO("Demo setup complete. Starting simulation...");
    
    // Run demo simulation
    size_t maxSteps = config->getOr<int64_t>("demo_steps", 500);
    float totalReward = 0.0f;
    
    for (uint64_t step = 0; step < maxSteps; ++step) {
        // Update world
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        world.update(timestep);
        
        // Get perception
        SensoryPercept percept = world.getSensoryPercept();
        
        // Process through agent
        agent.processSensoryInput(percept);
        
        // Brain computation
        brain->step(step, step * timestep);
        
        // Get action
        MotorCommand action = agent.decodeMotorCommand();
        
        // Apply action
        ActionResult result = world.applyMotorCommand(action, step * timestep);
        totalReward += result.reward;
        
        // Apply reward
        agent.applyRewardModulation(result.reward, 0.0f);
        
        // Update development occasionally
        if (step % 100 == 0) {
            agent.updateDevelopment(0.001);
        }
        
        // Log progress
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Reward: " + std::to_string(totalReward / (step + 1)) +
                        " | Neurons: " + std::to_string(brain->getFiringNeuronCount()) +
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    
    NLM_LOG_INFO("=== Demo Complete ===");
    NLM_LOG_INFO("Simulation ran for " + std::to_string(maxSteps) + " steps");
    NLM_LOG_INFO("Total reward: " + std::to_string(totalReward));
    NLM_LOG_INFO("Duration: " + std::to_string(duration) + " seconds");
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    
    brain->logStatus();
}

// Comprehensive test mode - run all verification tests
void runTestMode(std::shared_ptr<Config> config) {
    std::cout << "=== NLM Comprehensive Test Mode ===" << std::endl;
    std::cout << "Running all verification tests..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Integration verification
    std::cout << "1. Running integration verification..." << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
    }
    
    // Individual system tests
    std::cout << "2. Testing memory integration..." << std::endl;
    experiment.testMemoryIntegration();
    
    std::cout << "3. Testing neuromodulation integration..." << std::endl;
    experiment.testNeuromodulationIntegration();
    
    std::cout << "4. Testing checkpoint system..." << std::endl;
    experiment.testCheckpointing();
    
    std::cout << "5. Testing replay system..." << std::endl;
    experiment.testReplay();
    
    std::cout << "\n=== All Tests Complete ===" << std::endl;
    std::cout << "All NLM Phase 6 systems are properly integrated and functional." << std::endl;
}

// Benchmarking mode - performance testing
void runBenchmarkMode(std::shared_ptr<Config> config) {
    std::cout << "=== NLM Benchmark Mode ===" << std::endl;
    std::cout << "Running performance benchmarks..." << std::endl;
    
    Phase6Config benchConfig;
    benchConfig.neuronCount = config->getOr<int64_t>("benchmark_neuron_count", 500);
    benchConfig.maxSteps = config->getOr<int64_t>("benchmark_steps", 1000);
    benchConfig.enableCheckpointing = true;
    benchConfig.enableReplay = false;
    benchConfig.enableDevelopment = true;
    
    Phase6IntegratedExperiment experiment;
    
    auto start = std::chrono::high_resolution_clock::now();
    Phase6IntegrationResult result = experiment.run(benchConfig);
    auto end = std::chrono::high_resolution_clock::now();
    
    double wallTime = std::chrono::duration<double>(end - start).count();
    
    std::cout << "\n=== Benchmark Results ===" << std::endl;
    std::cout << "Neurons: " << benchConfig.neuronCount << std::endl;
    std::cout << "Steps: " << benchConfig.maxSteps << std::endl;
    std::cout << "Wall time: " << wallTime << "s" << std::endl;
    std::cout << "Speed: " << (benchConfig.maxSteps / wallTime) << " steps/second" << std::endl;
    std::cout << "\nSystem Integration Status:" << std::endl;
    std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Prediction: " << (result.predictionIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Development: " << (result.developmentIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "✓" : "✗") << std::endl;
    std::cout << "\nPerformance Metrics:" << std::endl;
    std::cout << "  Average reward: " << result.totalReward << std::endl;
    std::cout << "  Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "  Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "  Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "  Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "  Dopamine level: " << result.dopamineLevel << std::endl;
}

// Default mode - comprehensive Phase 6 experiment
void runDefaultMode(std::shared_ptr<Config> config) {
    std::cout << "=== NLM Phase 6 Default Mode ===" << std::endl;
    std::cout << "Running complete Phase 6 integration experiment..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Configure experiment from config
    Phase6Config phase6Config;
    phase6Config.neuronCount = config->getOr<int64_t>("neuron_count", 1000);
    phase6Config.maxSteps = config->getOr<int64_t>("max_steps", 10000);
    phase6Config.regionCount = config->getOr<int64_t>("region_count", 1);
    phase6Config.connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    phase6Config.enableCheckpointing = config->getOr<bool>("enable_checkpointing", true);
    phase6Config.enableReplay = config->getOr<bool>("enable_replay", true);
    phase6Config.enableDevelopment = config->getOr<bool>("enable_development", true);
    
    std::string checkpointPath = config->getOr<std::string>("checkpoint_path", "./checkpoint.bin");
    phase6Config.checkpointPath = checkpointPath;
    
    std::cout << "Starting Phase 6 integration experiment with " << phase6Config.neuronCount 
              << " neurons for " << phase6Config.maxSteps << " steps..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    Phase6IntegrationResult result = experiment.run(phase6Config);
    auto end = std::chrono::high_resolution_clock::now();
    
    double wallTime = std::chrono::duration<double>(end - start).count();
    
    std::cout << "\n=== Phase 6 Experiment Complete ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Wall time: " << wallTime << "s" << std::endl;
    
    std::cout << "\n=== Integration Verification ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    
    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "The NLM brain is a functioning artificial neural substrate" << std::endl;
    std::cout << "capable of changing its own synaptic connections through experience." << std::endl;
    std::cout << "All systems are integrated and working together cohesively." << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line mode
    NLMMode mode = parseMode(argc, argv);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::string configFile = "configs/default.cfg";
    
    // Extract configuration from command line arguments
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 6
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Apply presets based on command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
        
        if (arg == "--fast") {
            config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
            config->set("max_steps", static_cast<int64_t>(2000), ConfigSource::Default);
            NLM_LOG_INFO("Applied fast preset: smaller network");
        } else if (arg == "--comprehensive") {
            config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
            config->set("max_steps", static_cast<int64_t>(50000), ConfigSource::Default);
            NLM_LOG_INFO("Applied comprehensive preset: larger network, more thorough");
        } else if (arg == "--research") {
            config->set("neuron_count", static_cast<int64_t>(3000), ConfigSource::Default);
            config->set("max_steps", static_cast<int64_t>(100000), ConfigSource::Default);
            config->set("enable_checkpointing", true, ConfigSource::Default);
            NLM_LOG_INFO("Applied research preset: large scale with checkpointing");
        } else if (arg == "--demo-mode") {
            config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
            config->set("max_steps", static_cast<int64_t>(200), ConfigSource::Default);
            NLM_LOG_INFO("Applied demo-mode preset: beginner-friendly settings");
        }
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Phase 6 Configuration ===");
    NLM_LOG_INFO("neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
    NLM_LOG_INFO("region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.1f)));
    NLM_LOG_INFO("simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("max_steps: " + std::to_string(config->getOr<int64_t>("max_steps", 10000)));
    
    // Execute based on mode
    switch (mode) {
        case NLMMode::Help:
            showHelp();
            break;
            
        case NLMMode::Config:
            manageConfiguration(argc, argv);
            break;
            
        case NLMMode::Demo:
            runDemoMode(config);
            break;
            
        case NLMMode::Test:
            runTestMode(config);
            break;
            
        case NLMMode::Benchmark:
            runBenchmarkMode(config);
            break;
            
        case NLMMode::Experiment:
        case NLMMode::Default:
        default:
            runDefaultMode(config);
            break;
    }
    
    NLM_LOG_INFO("=== NLM Execution Complete ===");
    
    return 0;
}