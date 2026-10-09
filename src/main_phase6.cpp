// NLM (熙然) - Neural Learning Machine
// Phase 6: Advanced Command-Line Interface with Full Integration
//
// This file provides advanced command-line options for power users to:
// - Test Phase 6 integration with --integration-test
// - Control all brain systems (memory, neuromodulation, development, etc.)
// - Configure neural parameters for experiments
// - Monitor performance and save checkpoints
// - Visualize and analyze results
// 
// The existing main.cpp (Phase 2) is preserved as a fallback for basic demos.

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
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "development/DevelopmentSystem.hpp"
#include "performance/CheckpointSystem.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <memory>
#include <fstream>
#include <algorithm>
#include <chrono>

using namespace nlm;

// Print advanced banner for Phase 6
void printAdvancedBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════════════════════╗
    ║                                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                                               ║
    ║     Phase 6: Advanced Command-Line Interface                    ║
    ║                                                                               ║
    ║     Full integration of all brain systems with advanced controls.             ║
    ║     Supports memory, neuromodulation, development, plasticity, replay,          ║
    ║     checkpointing, and complete system integration.                           ║
    ║                                                                               ║
    ╚═══════════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Advanced help text for Phase 6 CLI
void printAdvancedHelp() {
    printAdvancedBanner();
    std::cout << R"(
=== PHASE 6 ADVANCED COMMAND-LINE OPTIONS ===

Phase 6 Integration Testing:
  --integration-test          Run Phase6IntegratedExperiment
  --verify-integration        Test integration only (no full run)
  --phase6-config=<file>      Specify Phase 6 config file (default: ./phase6_default.cfg)

Memory System Controls:
  --enable-memory             Enable working memory systems
  --disable-memory            Disable working memory systems
  --enable-episodic-memory    Enable episodic memory
  --disable-episodic-memory   Disable episodic memory
  --memory-capacity=<int>     Set working memory capacity (frames)

Neuromodulation Controls:
  --enable-neuromodulation    Enable neuromodulation
  --disable-neuromodulation   Disable neuromodulation
  --dopamine-level=<float>    Set baseline dopamine level (0.0-1.0)
  --curiosity-level=<float>   Set curiosity level (0.0-1.0)

Development Controls:
  --development-stage=<stage> Set developmental stage (0=juvenile, 1=adult, 2=senescent)
  --enable-development        Enable developmental processes
  --disable-development       Disable developmental processes

Checkpointing Controls:
  --checkpoint-file=<path>    Specify checkpoint file path
  --save-interval=<int>       Set checkpoint save interval (steps)

Performance Controls:
  --neuron-count=<int>        Override neuron count for simulation
  --timestep=<float>           Set simulation timestep (default: 0.001)
  --max-steps=<int>            Limit simulation steps
  --enable-sparse-conn        Enable sparse connectivity optimization

Output and Visualization:
  --quiet                     Minimal output (errors only)
  --verbose                   Full verbose logging
  --output-stats=<file>       Write detailed statistics to file
  --progress                  Show progress updates

Advanced Neural Controls:
  --plasticity-enable         Enable synaptic plasticity
  --plasticity-disable        Disable synaptic plasticity
  --plasticity-rate=<float>   Set plasticity rate (0.0-1.0)
  --excitatory-inhibition-ratio=<float> Set E/I balance (0.0-1.0)

Replay and Sleep Controls:
  --enable-replay             Enable memory replay
  --disable-replay            Disable memory replay
  --replay-interval=<int>     Set replay frequency (steps between replays)
  --enable-sleep              Enable sleep/wake cycles
  --disable-sleep             Disable sleep/wake cycles

Development Controls:
  --enable-synaptogenesis     Enable structural synaptogenesis
  --disable-synaptogenesis    Disable structural synaptogenesis
  --enable-pruning            Enable structural pruning
  --disable-pruning           Disable structural pruning

Example Usage:
  ./nlm_phase6 --integration-test --neuron-count=2000 --enable-memory
  ./nlm_phase6 --verify-integration --development-stage=1 --dopamine-level=0.7
  ./nlm_phase6 --max-steps=5000 --checkpoint-file=./brain.bin --save-interval=1000

=== END ADVANCED OPTIONS ===

For basic usage, run without --integration-test to see Phase 2 demo.
)" << std::endl;
}

// Parse command line arguments for Phase 6
bool parsePhase6Arguments(int argc, char** argv, Phase6Config& phase6Config, Config& config) {
    bool integrationTest = false;
    bool verifyIntegration = false;
    std::string configFile = "./phase6_default.cfg";
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--integration-test") {
            integrationTest = true;
        } else if (arg == "--verify-integration") {
            verifyIntegration = true;
        } else if (arg.substr(0, 15) == "--phase6-config=") {
            configFile = arg.substr(15);
        } else if (arg.substr(0, 13) == "--neuron-count=") {
            phase6Config.neuronCount = std::stoul(arg.substr(13));
        } else if (arg.substr(0, 12) == "--timestep=") {
            // Note: timestep is in config, not Phase6Config, but we can handle it
            double timestep = std::stod(arg.substr(12));
            config.set("simulation_timestep", timestep, ConfigSource::CommandLine);
        } else if (arg.substr(0, 12) == "--max-steps=") {
            phase6Config.maxSteps = std::stoull(arg.substr(12));
        } else if (arg == "--enable-memory") {
            config.set("enable_working_memory", true, ConfigSource::CommandLine);
        } else if (arg == "--disable-memory") {
            config.set("enable_working_memory", false, ConfigSource::CommandLine);
        } else if (arg == "--enable-episodic-memory") {
            config.set("enable_episodic_memory", true, ConfigSource::CommandLine);
        } else if (arg == "--disable-episodic-memory") {
            config.set("enable_episodic_memory", false, ConfigSource::CommandLine);
        } else if (arg == "--enable-neuromodulation") {
            config.set("enable_neuromodulation", true, ConfigSource::CommandLine);
        } else if (arg == "--disable-neuromodulation") {
            config.set("enable_neuromodulation", false, ConfigSource::CommandLine);
        } else if (arg.substr(0, 20) == "--dopamine-level=") {
            float dopamine = std::stof(arg.substr(20));
            config.set("dopamine_level", dopamine, ConfigSource::CommandLine);
        } else if (arg.substr(0, 19) == "--curiosity-level=") {
            float curiosity = std::stof(arg.substr(19));
            config.set("curiosity_level", curiosity, ConfigSource::CommandLine);
        } else if (arg.substr(0, 22) == "--development-stage=") {
            DevelopmentalStage stage = static_cast<DevelopmentalStage>(std::stoi(arg.substr(22)));
            config.set("developmental_stage", static_cast<int>(stage), ConfigSource::CommandLine);
        } else if (arg == "--enable-development") {
            config.set("enable_development", true, ConfigSource::CommandLine);
        } else if (arg == "--disable-development") {
            config.set("enable_development", false, ConfigSource::CommandLine);
        } else if (arg.substr(0, 17) == "--checkpoint-file=") {
            phase6Config.checkpointPath = arg.substr(17);
        } else if (arg.substr(0, 17) == "--save-interval=") {
            config.set("checkpoint_interval", std::stoi(arg.substr(17)), ConfigSource::CommandLine);
        } else if (arg == "--quiet") {
            config.set("log_level", 2, ConfigSource::CommandLine); // Error only
        } else if (arg == "--verbose") {
            config.set("log_level", 0, ConfigSource::CommandLine); // Debug
        } else if (arg.substr(0, 16) == "--output-stats=") {
            config.set("stats_output_file", arg.substr(16), ConfigSource::CommandLine);
        } else if (arg == "--progress") {
            config.set("show_progress", true, ConfigSource::CommandLine);
        } else if (arg == "--plasticity-enable") {
            config.set("enable_plasticity", true, ConfigSource::CommandLine);
        } else if (arg == "--plasticity-disable") {
            config.set("enable_plasticity", false, ConfigSource::CommandLine);
        } else if (arg.substr(0, 18) == "--plasticity-rate=") {
            float rate = std::stof(arg.substr(18));
            config.set("plasticity_rate", rate, ConfigSource::CommandLine);
        } else if (arg.substr(0, 37) == "--excitatory-inhibition-ratio=") {
            float ratio = std::stof(arg.substr(37));
            config.set("e_i_ratio", ratio, ConfigSource::CommandLine);
        } else if (arg == "--enable-replay") {
            phase6Config.enableReplay = true;
        } else if (arg == "--disable-replay") {
            phase6Config.enableReplay = false;
        } else if (arg.substr(0, 18) == "--replay-interval=") {
            config.set("replay_interval", std::stoi(arg.substr(18)), ConfigSource::CommandLine);
        } else if (arg == "--enable-sleep") {
            config.set("enable_sleep", true, ConfigSource::CommandLine);
        } else if (arg == "--disable-sleep") {
            config.set("enable_sleep", false, ConfigSource::CommandLine);
        } else if (arg == "--enable-synaptogenesis") {
            config.set("synaptogenesis_enabled", true, ConfigSource::CommandLine);
        } else if (arg == "--disable-synaptogenesis") {
            config.set("synaptogenesis_enabled", false, ConfigSource::CommandLine);
        } else if (arg == "--enable-pruning") {
            config.set("pruning_enabled", true, ConfigSource::CommandLine);
        } else if (arg == "--disable-pruning") {
            config.set("pruning_enabled", false, ConfigSource::CommandLine);
        } else if (arg == "--help" || arg == "-h") {
            printAdvancedHelp();
            return false;
        } else if (arg.substr(0, 9) == "--config=") {
            // Standard config file
            configFile = arg.substr(9);
        } else if (arg[0] == '-') {
            // Unknown option
            std::cerr << "Unknown option: " << arg << std::endl;
            printAdvancedHelp();
            return false;
        }
    }
    
    // Load Phase 6 config file if specified
    if (!configFile.empty()) {
        std::ifstream file(configFile);
        if (file.good()) {
            std::cout << "Loaded Phase 6 config from: " << configFile << std::endl;
            // Parse simple key=value config
            std::string line;
            while (std::getline(file, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#') {
                    continue;
                }
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    // Try to parse based on key name
                    if (key == "max_steps") phase6Config.maxSteps = std::stoull(value);
                    else if (key == "neuron_count") phase6Config.neuronCount = std::stoul(value);
                    else if (key == "region_count") phase6Config.regionCount = std::stoul(value);
                    else if (key == "connection_probability") phase6Config.connectionProbability = std::stof(value);
                    else if (key == "enable_checkpointing") phase6Config.enableCheckpointing = (value == "true" || value == "1");
                    else if (key == "enable_replay") phase6Config.enableReplay = (value == "true" || value == "1");
                    else if (key == "enable_development") phase6Config.enableDevelopment = (value == "true" || value == "1");
                    else if (key == "checkpoint_path") phase6Config.checkpointPath = value;
                }
            }
        }
    }
    
    return true;
}

// Run Phase 6 integration test
Phase6IntegrationResult runPhase6IntegrationTest(std::shared_ptr<Brain> brain, const Phase6Config& config, const std::string& statsOutputFile = "") {
    std::cout << std::endl;
    std::cout << "=== RUNNING PHASE 6 INTEGRATED EXPERIMENT ===" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Max steps: " << config.maxSteps << std::endl;
    std::cout << "  Neuron count: " << config.neuronCount << std::endl;
    std::cout << "  Region count: " << config.regionCount << std::endl;
    std::cout << "  Connection probability: " << config.connectionProbability << std::endl;
    std::cout << "  Enable checkpointing: " << (config.enableCheckpointing ? "yes" : "no") << std::endl;
    std::cout << "  Enable replay: " << (config.enableReplay ? "yes" : "no") << std::endl;
    std::cout << "  Enable development: " << (config.enableDevelopment ? "yes" : "no") << std::endl;
    std::cout << "  Checkpoint path: " << config.checkpointPath << std::endl;
    std::cout << std::endl;
    
    // Create and run Phase 6 integrated experiment
    auto experiment = std::make_shared<Phase6IntegratedExperiment>();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    Phase6IntegrationResult result = experiment->run(config);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    result.startTime = std::chrono::system_clock::to_time_t(std::chrono::high_resolution_clock::now());
    result.endTime = std::chrono::system_clock::to_time_t(endTime);
    result.totalWallClockTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Write stats to file if requested
    if (!statsOutputFile.empty()) {
        std::cout << "Writing stats to: " << statsOutputFile << std::endl;
        std::ofstream statsFile(statsOutputFile);
        if (statsFile.is_open()) {
            statsFile << "=== Phase 6 Integration Results ===\n";
            statsFile << "Total reward: " << result.totalReward << "\n";
            statsFile << "Average firing rate: " << result.avgFiringRate << "\n";
            statsFile << "Average synaptic weight: " << result.avgSynapticWeight << "\n";
            statsFile << "Memory episodes stored: " << result.memoryEpisodesStored << "\n";
            statsFile << "Novelty level: " << result.noveltyLevel << "\n";
            statsFile << "Curiosity level: " << result.curiosityLevel << "\n";
            statsFile << "Dopamine level: " << result.dopamineLevel << "\n";
            statsFile << "Integration status:\n";
            statsFile << "  Memory working memory: " << (result.memoryWorkingMemoryIntegrated ? "Integrated" : "Failed") << "\n";
            statsFile << "  Memory episodic: " << (result.memoryEpisodicMemoryIntegrated ? "Integrated" : "Failed") << "\n";
            statsFile << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "Integrated" : "Failed") << "\n";
            statsFile << "  Prediction: " << (result.predictionIntegrated ? "Integrated" : "Failed") << "\n";
            statsFile << "  Development: " << (result.developmentIntegrated ? "Integrated" : "Failed") << "\n";
            statsFile << "  Checkpointing: " << (result.checkpointingWorks ? "Working" : "Failed") << "\n";
            statsFile << "  Replay: " << (result.replayWorks ? "Working" : "Failed") << "\n";
            statsFile << "\nTotal wall clock time: " << result.totalWallClockTime << " seconds\n";
            statsFile.close();
        }
    }
    
    // Print results
    std::cout << std::endl;
    std::cout << "=== PHASE 6 INTEGRATION RESULTS ===" << std::endl;
    std::cout << "System Integration Status:" << std::endl;
    std::cout << "  Memory working memory: " << (result.memoryWorkingMemoryIntegrated ? "✓ Integrated" : "✗ Failed") << std::endl;
    std::cout << "  Memory episodic: " << (result.memoryEpisodicMemoryIntegrated ? "✓ Integrated" : "✗ Failed") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ Integrated" : "✗ Failed") << std::endl;
    std::cout << "  Prediction: " << (result.predictionIntegrated ? "✓ Integrated" : "✗ Failed") << std::endl;
    std::cout << "  Development: " << (result.developmentIntegrated ? "✓ Integrated" : "✗ Failed") << std::endl;
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "✓ Working" : "✗ Failed") << std::endl;
    std::cout << "  Replay: " << (result.replayWorks ? "✓ Working" : "✗ Failed") << std::endl;
    
    std::cout << std::endl;
    std::cout << "Performance Metrics:" << std::endl;
    std::cout << "  Total reward: " << result.totalReward << std::endl;
    std::cout << "  Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "  Average synaptic weight: " << result.avgSynapticWeight << std::endl;
    std::cout << "  Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "  Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "  Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "  Dopamine level: " << result.dopamineLevel << std::endl;
    
    std::cout << std::endl;
    std::cout << "Temporal Metrics:" << std::endl;
    std::cout << "  Start time: " << std::ctime(&result.startTime) << std::endl;
    std::cout << "  End time: " << std::ctime(&result.endTime) << std::endl;
    std::cout << "  Total wall clock time: " << result.totalWallClockTime << " seconds" << std::endl;
    
    return result;
}

// Verify integration without full run
bool verifyPhase6Integration(std::shared_ptr<Brain> brain) {
    std::cout << std::endl;
    std::cout << "=== VERIFYING PHASE 6 INTEGRATION ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    bool success = experiment.verifyIntegration();
    
    std::cout << std::endl;
    if (success) {
        std::cout << "✓ Phase 6 integration verification PASSED" << std::endl;
    } else {
        std::cout << "✗ Phase 6 integration verification FAILED" << std::endl;
    }
    
    return success;
}

int mainPhase6(int argc, char** argv) {
    // Check if we should run Phase 6 or fall back to Phase 2
    bool runPhase6 = false;
    bool runIntegrationTest = false;
    bool verifyOnly = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--integration-test") {
            runPhase6 = true;
            runIntegrationTest = true;
        } else if (arg == "--verify-integration") {
            runPhase6 = true;
            verifyOnly = true;
        } else if (arg == "--help" || arg == "-h") {
            printAdvancedHelp();
            return 0;
        } else if (arg == "--phase6" || arg == "--phase6-demo") {
            runPhase6 = true;
        }
    }
    
    // Run Phase 6 with advanced options
    if (runPhase6) {
        printAdvancedBanner();
        
        std::cout << "Initializing NLM Phase 6 Advanced Interface..." << std::endl;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("=== NLM Phase 6: Advanced Interface ===");
        NLM_LOG_INFO("Running advanced command-line interface with full system integration.");
        
        // Initialize Phase 6 configuration
        auto phase6Config = Phase6Config();
        Config config;
        
        // Parse Phase 6 specific arguments
        bool parseSuccess = parsePhase6Arguments(argc, argv, phase6Config, config);
        if (!parseSuccess) {
            return 1;
        }
        
        // Set default values for Phase 6
        config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config.set("simulation_timestep", 0.001, ConfigSource::Default);
        config.set("neuron_count", static_cast<int64_t>(phase6Config.neuronCount), ConfigSource::Default);
        config.set("region_count", static_cast<int64_t>(phase6Config.regionCount), ConfigSource::Default);
        config.set("connection_probability", phase6Config.connectionProbability, ConfigSource::Default);
        config.set("enable_working_memory", true, ConfigSource::Default);
        config.set("enable_episodic_memory", true, ConfigSource::Default);
        config.set("enable_neuromodulation", true, ConfigSource::Default);
        config.set("enable_development", phase6Config.enableDevelopment, ConfigSource::Default);
        config.set("enable_plasticity", true, ConfigSource::Default);
        config.set("plasticity_rate", 0.1f, ConfigSource::Default);
        config.set("e_i_ratio", 0.8f, ConfigSource::Default);
        config.set("enable_checkpointing", phase6Config.enableCheckpointing, ConfigSource::Default);
        config.set("checkpoint_interval", 100, ConfigSource::Default);
        config.set("enable_replay", phase6Config.enableReplay, ConfigSource::Default);
        config.set("replay_interval", 50, ConfigSource::Default);
        config.set("enable_sleep", true, ConfigSource::Default);
        config.set("synaptogenesis_enabled", true, ConfigSource::Default);
        config.set("pruning_enabled", true, ConfigSource::Default);
        
        // Get log level from config
        int logLevel = config.getOr<int>("log_level", 1); // Default to Info
        
        // Update console logger level
        switch (logLevel) {
            case 0: consoleLogger->setLevel(LogLevel::Debug); break;
            case 1: consoleLogger->setLevel(LogLevel::Info); break;
            case 2: consoleLogger->setLevel(LogLevel::Warning); break;
            case 3: consoleLogger->setLevel(LogLevel::Error); break;
        }
        
        // Log configuration summary
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Phase 6 Configuration Summary:");
        NLM_LOG_INFO("  Random seed: " + std::to_string(config.getOr<int64_t>("random_seed", 42)));
        NLM_LOG_INFO("  Simulation timestep: " + std::to_string(config.getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("  Neuron count: " + std::to_string(config.getOr<int64_t>("neuron_count", phase6Config.neuronCount)));
        NLM_LOG_INFO("  Region count: " + std::to_string(config.getOr<int64_t>("region_count", phase6Config.regionCount)));
        NLM_LOG_INFO("  Connection probability: " + std::to_string(config.getOr<float>("connection_probability", phase6Config.connectionProbability)));
        NLM_LOG_INFO("  Working memory enabled: " + std::string(config.getOr<bool>("enable_working_memory", true) ? "yes" : "no"));
        NLM_LOG_INFO("  Episodic memory enabled: " + std::string(config.getOr<bool>("enable_episodic_memory", true) ? "yes" : "no"));
        NLM_LOG_INFO("  Neuromodulation enabled: " + std::string(config.getOr<bool>("enable_neuromodulation", true) ? "yes" : "no"));
        NLM_LOG_INFO("  Development enabled: " + std::string(config.getOr<bool>("enable_development", phase6Config.enableDevelopment) ? "yes" : "no"));
        NLM_LOG_INFO("  Plasticity enabled: " + std::string(config.getOr<bool>("enable_plasticity", true) ? "yes" : "no"));
        NLM_LOG_INFO("");
        
        // Initialize simulation clock
        double timestep = config.getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);
        NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
        
        // Initialize brain with Phase 6 configuration
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Initializing Phase 6 NLM Brain with advanced configuration...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // If verification only, just test integration
        if (verifyOnly) {
            return verifyPhase6Integration(brain) ? 0 : 1;
        }
        
        // Otherwise run full integration test
        Phase6IntegrationResult result = runPhase6IntegrationTest(brain, phase6Config, config.getOr<std::string>("stats_output_file", ""));
        
        // Final status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== PHASE 6 COMPLETE ===");
        
        if (result.memoryWorkingMemoryIntegrated && 
            result.memoryEpisodicMemoryIntegrated &&
            result.neuromodulationIntegrated &&
            result.predictionIntegrated &&
            result.developmentIntegrated &&
            result.checkpointingWorks &&
            result.replayWorks) {
            NLM_LOG_INFO("✓ ALL SYSTEMS INTEGRATED SUCCESSFULLY");
            NLM_LOG_INFO("The Phase 6 brain is fully functional with:");
            NLM_LOG_INFO("  ✓ Working memory system operational");
            NLM_LOG_INFO("  ✓ Episodic memory system functional");
            NLM_LOG_INFO("  ✓ Neuromodulation system active");
            NLM_LOG_INFO("  ✓ Prediction system integrated");
            NLM_LOG_INFO("  ✓ Development system operational");
            NLM_LOG_INFO("  ✓ Checkpointing system working");
            NLM_LOG_INFO("  ✓ Replay system functional");
        } else {
            NLM_LOG_ERROR("⚠ SOME SYSTEMS FAILED INTEGRATION");
        }
        
        return result.memoryWorkingMemoryIntegrated && 
               result.memoryEpisodicMemoryIntegrated &&
               result.neuromodulationIntegrated &&
               result.predictionIntegrated &&
               result.developmentIntegrated &&
               result.checkpointingWorks &&
               result.replayWorks ? 0 : 1;
    }
    
    // Fall back to Phase 2 demo for backward compatibility
    std::cout << "Running Phase 2 demo (use --integration-test or --verify-integration for Phase 6)..." << std::endl;
    
    // Check if Phase 2 demo is requested via --demo phase2
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--demo" && i + 1 < argc && std::string(argv[i + 1]) == "phase2") {
            std::cout << "Phase 2 demo would run here. Use ./nlm for the original Phase 2 implementation." << std::endl;
            return 0;
        }
    }
    
    // Show help if no Phase 6 options provided
    std::cout << "No Phase 6 options detected. Use --help for usage information." << std::endl;
    return 0;
}

// Helper function for string trimming (used in config parsing)
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper function for string case conversion (used in config parsing)
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}