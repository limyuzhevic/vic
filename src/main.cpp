// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration CLI
//
// This is the main entry point for NLM Phase 6, providing a comprehensive
// command-line interface for interacting with the integrated artificial brain.
//
// Features:
// - Multiple execution modes (demo, experiment, interactive)
// - Flexible configuration management
// - Real-time monitoring and status display
// - Checkpoint save/load functionality
// - Performance profiling tools
// - Comprehensive logging system

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "performance/PerformanceMonitoring.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <thread>
#include <map>
#include <algorithm>
#include <csignal>

using namespace nlm;

// Global variables for signal handling
bool g_running = true;

void signalHandler(int signum) {
    g_running = false;
    NLM_LOG_INFO("Interrupt received, shutting down gracefully...");
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     All previous phases integrated into a complete brain loop. ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << R"(
NLM Phase 6: Final Integration CLI
Usage: nlm [options]

Execution Modes:
  --demo                    Run Phase 6 integration demo (default)
  --demo-performance        Run demo with comprehensive performance monitoring
  --experiment <name>       Run specific experiment (phase3, phase4, phase5, phase6)
  --interactive             Enter interactive mode for manual control

Configuration:
  --config <file>           Load configuration from file (default: configs/default.cfg)
  --set <key>=<value>       Set configuration value
  --list-configs           List all available configuration options

Simulation Control:
  --steps <count>           Number of simulation steps (default: 1000)
  --neurons <count>         Number of neurons (default: 1000)
  --regions <count>         Number of brain regions (default: 1)
  --time <seconds>          Total simulation time in seconds

Checkpointing:
  --checkpoint <file>       Save/load checkpoint
  --enable-checkpointing    Enable automatic checkpointing
  --checkpoint-dir <dir>    Directory for checkpoint files (default: ./checkpoints)

Display Options:
  --verbose                 Enable verbose logging
  --quiet                   Suppress informational messages
  --format <format>         Output format (text, json, csv)

Experimental Features:
  --enable-replay           Enable memory replay
  --enable-development      Enable developmental stages
  --enable-curiosity        Enable curiosity-driven exploration
  --enable-reward-mod       Enable reward-based modulation

Debug/Testing:
  --test-integration        Run integration verification only
  --test-memory             Test memory systems
  --test-neuromodulation    Test neuromodulation systems
  --test-checkpointing     Test checkpoint functionality
  --test-replay            Test replay system

Performance:
  --performance <mode>      Performance mode (fast, balanced, accurate)
  --profile                 Enable performance profiling
  --watch                  Monitor metrics in real-time

Examples:
  nlm --demo --steps 5000 --enable-replay
  nlm --demo-performance --neurons 500 --enable-replay
  nlm --experiment phase6 --checkpoint brain_state.bin
  nlm --interactive --watch
  nlm --test-integration --verbose
    )" << std::endl;
}

void printStatus(const Brain& brain, const Phase6Config& config, bool verbose = false) {
    std::cout << "\n=== NLM Brain Status ===" << std::endl;
    std::cout << "Region Count: " << brain.getRegionCount() << std::endl;
    std::cout << "Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "Firing Rate (this step): " << brain.getFiringNeuronCount() << "/" << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Average Firing Rate: " << brain.getAverageFiringRate() << std::endl;
    
    float eiRatio = brain.getExcitationInhibitionRatio();
    if (std::isinf(eiRatio)) {
        std::cout << "E/I Ratio: Infinity (all excitatory)" << std::endl;
    } else {
        std::cout << "E/I Ratio: " << eiRatio << std::endl;
    }
    
    auto configPtr = brain.getConfig();
    if (verbose && configPtr) {
        std::cout << "\nDetailed Configuration:" << std::endl;
        std::cout << "  Simulation Timestep: " << configPtr->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
        std::cout << "  Development Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
        
        if (brain.getWorkingMemory()) {
            std::cout << "  Working Memory: " << brain.getWorkingMemory()->getActiveTraces() << "/" << configPtr->getOr<size_t>("working_memory_capacity", 100) << " traces" << std::endl;
        }
        
        if (brain.getEpisodicMemory()) {
            std::cout << "  Episodic Memory: " << brain.getEpisodicMemory()->getEpisodeCount() << " episodes" << std::endl;
        }
    }
}

void runDemoMode(const Phase6Config& config) {
    NLM_LOG_INFO("=== NLM Phase 6 Demo Mode ===");
    NLM_LOG_INFO("Running integration demonstration with " + std::to_string(config.neuronCount) + " neurons for " + 
                 std::to_string(config.maxSteps) + " steps.");
    
    Phase6IntegratedExperiment experiment;
    Phase6IntegrationResult result = experiment.run(config);
    
    std::cout << "\n=== Demo Results ===" << std::endl;
    std::cout << "Total Reward: " << result.totalReward << std::endl;
    std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
    std::cout << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
    
    std::cout << "\nIntegration Status:" << std::endl;
    std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Prediction: " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Development: " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << "  Replay: " << (result.replayWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
}

void runDemoModeWithPerformanceMonitoring(const Phase6Config& config) {
    NLM_LOG_INFO("=== NLM Phase 6 Demo Mode with Performance Monitoring ===");
    NLM_LOG_INFO("Running integration demonstration with " + std::to_string(config.neuronCount) + " neurons for " + 
                 std::to_string(config.maxSteps) + " steps.");
    
    // Start global performance monitoring
    nlm::startGlobalMonitoring();
    
    // Create performance counters for key operations
    auto& brain_init = nlm::getPerfCounter("brain_initialization");
    auto& simulation_step = nlm::getPerfCounter("simulation_step");
    auto& memory_update = nlm::getPerfCounter("memory_update");
    auto& neuromodulation = nlm::getPerfCounter("neuromodulation_update");
    
    Phase6IntegratedExperiment experiment;
    Phase6IntegrationResult result = experiment.run(config);
    
    // Stop performance monitoring
    nlm::stopGlobalMonitoring();
    
    // Generate and display performance report
    std::cout << "\n=== Performance Analysis ===" << std::endl;
    std::cout << nlm::generatePerformanceReport() << std::endl;
    
    std::cout << "\n=== Demo Results ===" << std::endl;
    std::cout << "Total Reward: " << result.totalReward << std::endl;
    std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
    std::cout << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
    
    std::cout << "\nPerformance Summary:" << std::endl;
    std::cout << "  Brain Initialization: " << brain_init.getAverage() << " ms (" << brain_init.getSamples() << " samples)" << std::endl;
    std::cout << "  Average Simulation Step: " << simulation_step.getAverage() << " ms (" << simulation_step.getSamples() << " samples)" << std::endl;
    std::cout << "  Average Memory Update: " << memory_update.getAverage() << " ms" << std::endl;
    std::cout << "  Average Neuromodulation: " << neuromodulation.getAverage() << " ms" << std::endl;
    
    // Export performance report
    nlm::exportPerformanceReport("./nlm_performance_report.txt");
    NLM_LOG_INFO("Performance report exported to ./nlm_performance_report.txt");
}

void runExperimentMode(const std::string& experimentName, const Phase6Config& config) {
    NLM_LOG_INFO("=== Running NLM " + experimentName + " Experiment ===");
    
    if (experimentName == "phase3") {
        NLM_LOG_INFO("Phase 3 experiment: World interaction and learning");
        // TODO: Implement Phase 3 experiment
    } else if (experimentName == "phase4") {
        NLM_LOG_INFO("Phase 4 experiment: Cognitive development");
        // TODO: Implement Phase 4 experiment
    } else if (experimentName == "phase5") {
        NLM_LOG_INFO("Phase 5 experiment: Performance optimization");
        // TODO: Implement Phase 5 experiment
    } else if (experimentName == "phase6") {
        runDemoMode(config);
    } else {
        NLM_LOG_ERROR("Unknown experiment: " + experimentName);
        std::cout << "Available experiments: phase3, phase4, phase5, phase6" << std::endl;
    }
}

void runInteractiveMode(std::shared_ptr<Brain> brain) {
    std::cout << "\n=== Interactive Mode ===" << std::endl;
    std::cout << "Enter commands to control the brain simulation and monitor performance:" << std::endl;
    std::cout << "  step    <n>           - Advance simulation by n steps" << std::endl;
    std::cout << "  inject  <type> <amount> - Inject current into neuron type" << std::endl;
    std::cout << "  status                 - Display brain status" << std::endl;
    std::cout << "  save    <file>        - Save current brain state" << std::endl;
    std::cout << "  load    <file>        - Load brain state from file" << std::endl;
    std::cout << "  reset                 - Reset brain to initial state" << std::endl;
    std::cout << "  monitor                - Update and display system metrics" << std::endl;
    std::cout << "  perf                   - Display detailed performance metrics" << std::endl;
    std::cout << "  export    <file>      - Export performance report to file" << std::endl;
    std::cout << "  resetperf             - Reset performance counters" << std::endl;
    std::cout << "  watch                 - Enable real-time monitoring" << std::endl;
    std::cout << "  help                  - Show this help" << std::endl;
    std::cout << "  exit/quit             - Exit interactive mode" << std::endl;
    
    std::string line;
    while (g_running) {
        std::cout << "\nnlm> ";
        std::getline(std::cin, line);
        
        if (line.empty()) continue;
        
        size_t pos = line.find(' ');
        std::string command = line.substr(0, pos);
        std::string args = (pos != std::string::npos) ? line.substr(pos + 1) : "";
        
        if (command == "help") {
            std::cout << "Available commands: step, inject, status, save, load, reset, monitor, perf, export, resetperf, watch, help, exit" << std::endl;
        } else if (command == "exit" || command == "quit") {
            break;
        } else if (command == "step") {
            try {
                size_t steps = std::stoul(args);
                std::cout << "Advancing " << steps << " steps..." << std::endl;
                for (size_t i = 0; i < steps; ++i) {
                    brain->step(i, i * brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
                    if (i % 100 == 0) {
                        std::cout << "  Step " << i << ". " << brain->getFiringNeuronCount() << " neurons firing" << std::endl;
                    }
                }
            } catch (...) {
                std::cout << "Invalid step count. Usage: step <n>" << std::endl;
            }
        } else if (command == "inject") {
            try {
                size_t delim1 = args.find(' ');
                if (delim1 == std::string::npos) {
                    std::cout << "Invalid inject format. Usage: inject <type> <amount>" << std::endl;
                    continue;
                }
                std::string typeStr = args.substr(0, delim1);
                std::string amountStr = args.substr(delim1 + 1);
                
                NeuronType type;
                if (typeStr == "sensory") type = NeuronType::Sensory;
                else if (typeStr == "internal") type = NeuronType::Internal;
                else if (typeStr == "motor") type = NeuronType::Motor;
                else {
                    std::cout << "Invalid neuron type. Use: sensory, internal, motor" << std::endl;
                    continue;
                }
                
                float amount = std::stof(amountStr);
                brain->injectCurrentToNeurons(type, amount);
                std::cout << "Injected " << amount << " current into " << typeStr << " neurons" << std::endl;
            } catch (...) {
                std::cout << "Invalid inject arguments. Usage: inject <type> <amount>" << std::endl;
            }
        } else if (command == "status") {
            printStatus(*brain, Phase6Config(), true);
        } else if (command == "save") {
            if (args.empty()) {
                std::cout << "Usage: save <filepath>" << std::endl;
            } else {
                std::cout << "Saving brain state to: " << args << std::endl;
                if (brain->save(args)) {
                    std::cout << "Save successful!" << std::endl;
                } else {
                    std::cout << "Save failed!" << std::endl;
                }
            }
        } else if (command == "load") {
            if (args.empty()) {
                std::cout << "Usage: load <filepath>" << std::endl;
            } else {
                std::cout << "Loading brain state from: " << args << std::endl;
                if (brain->load(args)) {
                    std::cout << "Load successful!" << std::endl;
                    printStatus(*brain, Phase6Config(), true);
                } else {
                    std::cout << "Load failed!" << std::endl;
                }
            }
        } else if (command == "reset") {
            std::cout << "Resetting brain to initial state..." << std::endl;
            brain->reset();
            printStatus(*brain, Phase6Config(), true);
        } else if (command == "monitor") {
            NLM_LOG_INFO("Updating system metrics...");
            nlm::updateSystemMetrics();
            
            auto& sys_monitor = nlm::global_system_monitor;
            std::cout << "\n=== System Metrics ===" << std::endl;
            std::cout << "CPU Usage: " << std::fixed << std::setprecision(2) << sys_monitor.getCpuUsage() << "%" << std::endl;
            std::cout << "Memory Usage: " << std::fixed << std::setprecision(2) << sys_monitor.getMemoryUsage() << " MB" << std::endl;
            std::cout << "Memory Available: " << std::fixed << std::setprecision(2) << sys_monitor.getMemoryAvailable() << " MB" << std::endl;
            std::cout << "Memory Usage: " << std::fixed << std::setprecision(2) << sys_monitor.getMemoryUsagePercent() << "%" << std::endl;
        } else if (command == "perf") {
            std::cout << "\n=== Performance Metrics ===" << std::endl;
            std::cout << nlm::global_perf_monitor.generateReport() << std::endl;
            std::cout << nlm::global_step_profiler.generateStepReport() << std::endl;
            std::cout << nlm::global_system_monitor.generateSystemReport() << std::endl;
        } else if (command == "export") {
            if (args.empty()) {
                args = "performance_export.txt";
            }
            std::cout << "Exporting performance report to: " << args << std::endl;
            if (nlm::exportPerformanceReport(args)) {
                std::cout << "Export successful!" << std::endl;
            } else {
                std::cout << "Export failed!" << std::endl;
            }
        } else if (command == "resetperf") {
            std::cout << "Resetting performance counters..." << std::endl;
            nlm::global_perf_monitor.reset();
            nlm::global_step_profiler.reset();
            std::cout << "Performance counters reset!" << std::endl;
        } else {
            std::cout << "Unknown command: " << command << ". Type 'help' for available commands." << std::endl;
        }
    }
}

void runTestSuite(std::shared_ptr<Brain> brain, const std::vector<std::string>& tests) {
    NLM_LOG_INFO("=== NLM Test Suite ===");
    
    Phase6IntegratedExperiment experiment;
    
    for (const auto& test : tests) {
        NLM_LOG_INFO("Running test: " + test);
        
        if (test == "integration") {
            if (experiment.verifyIntegration()) {
                NLM_LOG_INFO("[PASS] Integration verification successful");
            } else {
                NLM_LOG_ERROR("[FAIL] Integration verification failed");
            }
        } else if (test == "memory") {
            if (experiment.testMemoryIntegration()) {
                NLM_LOG_INFO("[PASS] Memory integration test passed");
            } else {
                NLM_LOG_ERROR("[FAIL] Memory integration test failed");
            }
        } else if (test == "neuromodulation") {
            if (experiment.testNeuromodulationIntegration()) {
                NLM_LOG_INFO("[PASS] Neuromodulation integration test passed");
            } else {
                NLM_LOG_ERROR("[FAIL] Neuromodulation integration test failed");
            }
        } else if (test == "checkpointing") {
            if (experiment.testCheckpointing()) {
                NLM_LOG_INFO("[PASS] Checkpointing test passed");
            } else {
                NLM_LOG_ERROR("[FAIL] Checkpointing test failed");
            }
        } else if (test == "replay") {
            if (experiment.testReplay()) {
                NLM_LOG_INFO("[PASS] Replay test passed");
            } else {
                NLM_LOG_ERROR("[FAIL] Replay test failed");
            }
        } else {
            NLM_LOG_WARNING("Unknown test: " + test);
        }
    }
}

int main(int argc, char** argv) {
    // Setup signal handling
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    printBanner();
    
    // Parse command line arguments
    std::string mode = "demo";  // Default mode
    std::string experimentName = "";
    std::string configFile = "configs/default.cfg";
    std::string checkpointFile = "";
    std::vector<std::string> tests;
    bool verbose = false;
    bool quiet = false;
    std::string outputFormat = "text";
    bool enableReplay = false;
    bool enableDevelopment = false;
    bool enableCuriosity = false;
    bool enableRewardMod = false;
    bool enableCheckpointing = false;
    std::string checkpointDir = "./checkpoints";
    size_t steps = 1000;
    size_t neurons = 1000;
    size_t regions = 1;
    double totalTime = 0.0;
    std::string performanceMode = "balanced";
    bool profile = false;
    bool watch = false;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--demo-performance") {
            mode = "demo-performance";
        } else if (arg == "--demo") {
            mode = "demo";
        } else if (arg == "--interactive") {
            mode = "interactive";
        } else if (arg.substr(0, 14) == "--experiment=") {
            mode = "experiment";
            experimentName = arg.substr(14);
        } else if (arg.substr(0, 13) == "--experiment ") {
            mode = "experiment";
            experimentName = arg.substr(13);
        } else if (arg.substr(0, 8) == "--config=") {
            configFile = arg.substr(8);
        } else if (arg.substr(0, 8) == "--config ") {
            configFile = arg.substr(8);
        } else if (arg.substr(0, 5) == "--set=") {
            // TODO: Parse key=value and set in config
        } else if (arg.substr(0, 5) == "--set ") {
            // TODO: Parse key=value and set in config
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--quiet") {
            quiet = true;
        } else if (arg.substr(0, 7) == "--steps=") {
            steps = std::stoul(arg.substr(7));
        } else if (arg.substr(0, 8) == "--steps ") {
            steps = std::stoul(arg.substr(8));
        } else if (arg.substr(0, 9) == "--neurons=") {
            neurons = std::stoul(arg.substr(9));
        } else if (arg.substr(0, 10) == "--neurons ") {
            neurons = std::stoul(arg.substr(10));
        } else if (arg.substr(0, 9) == "--regions=") {
            regions = std::stoul(arg.substr(9));
        } else if (arg.substr(0, 10) == "--regions ") {
            regions = std::stoul(arg.substr(10));
        } else if (arg.substr(0, 7) == "--time=") {
            totalTime = std::stod(arg.substr(7));
        } else if (arg.substr(0, 8) == "--time ") {
            totalTime = std::stod(arg.substr(8));
        } else if (arg.substr(0, 13) == "--checkpoint=") {
            checkpointFile = arg.substr(13);
        } else if (arg.substr(0, 14) == "--checkpoint ") {
            checkpointFile = arg.substr(14);
        } else if (arg == "--enable-checkpointing") {
            enableCheckpointing = true;
        } else if (arg.substr(0, 16) == "--checkpoint-dir=") {
            checkpointDir = arg.substr(16);
        } else if (arg.substr(0, 17) == "--checkpoint-dir ") {
            checkpointDir = arg.substr(17);
        } else if (arg == "--enable-replay") {
            enableReplay = true;
        } else if (arg == "--enable-development") {
            enableDevelopment = true;
        } else if (arg == "--enable-curiosity") {
            enableCuriosity = true;
        } else if (arg == "--enable-reward-mod") {
            enableRewardMod = true;
        } else if (arg == "--test-integration") {
            tests.push_back("integration");
        } else if (arg == "--test-memory") {
            tests.push_back("memory");
        } else if (arg == "--test-neuromodulation") {
            tests.push_back("neuromodulation");
        } else if (arg == "--test-checkpointing") {
            tests.push_back("checkpointing");
        } else if (arg == "--test-replay") {
            tests.push_back("replay");
        } else if (arg.substr(0, 12) == "--format=") {
            outputFormat = arg.substr(12);
        } else if (arg.substr(0, 13) == "--format ") {
            outputFormat = arg.substr(13);
        } else if (arg.substr(0, 16) == "--performance=") {
            performanceMode = arg.substr(16);
        } else if (arg.substr(0, 17) == "--performance ") {
            performanceMode = arg.substr(17);
        } else if (arg == "--profile") {
            profile = true;
        } else if (arg == "--watch") {
            watch = true;
        } else {
            NLM_LOG_WARNING("Unknown argument: " + arg + ". Use --help for usage information.");
        }
    }
    
    // Setup logging based on verbosity
    auto logger = std::make_shared<Logger>();
    if (quiet) {
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Error);
        logger->addLogger(consoleLogger);
    } else if (verbose) {
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
    } else {
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
        logger->addLogger(consoleLogger);
    }
    Logger::setGlobal(logger);
    
    // Create Phase 6 configuration
    Phase6Config config;
    config.maxSteps = steps;
    config.neuronCount = neurons;
    config.regionCount = regions;
    config.connectionProbability = 0.1f;
    config.enableCheckpointing = enableCheckpointing;
    config.enableReplay = enableReplay;
    config.enableDevelopment = enableDevelopment;
    config.checkpointPath = checkpointFile.empty() ? "./checkpoint_test.bin" : checkpointFile;
    
    // Create brain
    NLM_LOG_INFO("=== Initializing NLM Phase 6 ===");
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", neurons);
    cfg->set("region_count", regions);
    cfg->set("connection_probability", 0.1f);
    cfg->set("random_seed", static_cast<int64_t>(42));
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    // Run mode-specific operations
    if (mode == "demo") {
        runDemoMode(config);
    } else if (mode == "demo-performance") {
        runDemoModeWithPerformanceMonitoring(config);
    } else if (mode == "experiment") {
        if (experimentName.empty()) {
            NLM_LOG_ERROR("Experiment name required for --experiment mode");
            printHelp();
            return 1;
        }
        runExperimentMode(experimentName, config);
    } else if (mode == "interactive") {
        runInteractiveMode(brain);
    } else {
        NLM_LOG_ERROR("Unknown mode: " + mode);
        printHelp();
        return 1;
    }
    
    // Run test suite if tests were specified
    if (!tests.empty()) {
        runTestSuite(brain, tests);
    }
    
    // Display final status
    printStatus(*brain, config, verbose);
    
    NLM_LOG_INFO("=== NLM Phase 6 Complete ===");
    
    return 0;
}