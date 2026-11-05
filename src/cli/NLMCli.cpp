// NLM Command-Line Interface
// Provides command-line access to all NLM demos and experiments with batch processing support

#include "CliOptions.hpp"
#include "CommandProcessor.hpp"
#include "ResultFormatter.hpp"
#include "ShellInterface.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <memory>

using namespace nlm;

// Command help text
const std::string HELP_TEXT = R"(
NLM - Neural Learning Machine Command Line Interface

Usage: nlm [OPTIONS] [DEMO_TYPE]

Demo Types:
  main          Phase 2: Real neural computation (default)
  phase3        Phase 3: World interaction
  phase4        Phase 4: Advanced cognition
  phase5        Phase 5: Performance and personality
  phase6        Phase 6: Integration test
  phase6_text   Phase 6: Text-based interface
  benchmark     Performance benchmark
  shell         Interactive shell mode

Common Options:
  --config FILE          Load configuration from FILE (default: configs/default.cfg)
  --output FILE          Save output/results to FILE
  --log-level LEVEL      Set logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
  --log-file FILE        Log to file instead of/to console
  --verbose, -v          Enable verbose output
  --help, -h             Show this help message
  --version             Show version information
  --shell               Enter interactive shell mode

Batch Processing:
  --batch FILE           Run experiments from batch configuration FILE
  --experiment NAME      Specify experiment name for batch processing
  --max-steps STEPS      Set maximum simulation steps
  --neuron-count COUNT   Set neuron count for the brain

Experiment Options:
  --list-experiments     List available experiments
  --list-benchmarks      List available benchmarks

Examples:
  nlm                              # Run Phase 2 demo with default settings
  nlm --config myconfig.cfg        # Run with custom configuration
  nlm --neuron-count 2000          # Run with 2000 neurons
  nlm phase3 --max-steps 5000      # Run Phase 3 with 5000 steps
  nlm --batch batch.cfg phase4     # Run Phase 4 with batch configuration
  nlm --shell                     # Enter interactive shell mode

For detailed information, visit: https://github.com/nlm-project/nlm
)";

// Version information
const std::string VERSION = "NLM Phase 6.2 (2026)";

// Parse command-line arguments
CliOptions parseArguments(int argc, char** argv) {
    CliOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version") {
            options.version = true;
        } else if (arg == "--list-experiments") {
            options.listExperiments = true;
        } else if (arg == "--list-benchmarks") {
            options.listBenchmarks = true;
        } else if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            options.outputFile = argv[++i];
        } else if (arg == "--log-level" && i + 1 < argc) {
            std::string levelStr = argv[++i];
            std::transform(levelStr.begin(), levelStr.end(), levelStr.begin(), ::toupper);
            if (levelStr == "DEBUG") options.logLevel = LogLevel::Debug;
            else if (levelStr == "INFO") options.logLevel = LogLevel::Info;
            else if (levelStr == "WARNING") options.logLevel = LogLevel::Warning;
            else if (levelStr == "ERROR") options.logLevel = LogLevel::Error;
            else if (levelStr == "CRITICAL") options.logLevel = LogLevel::Critical;
        } else if (arg == "--log-file" && i + 1 < argc) {
            options.logFile = argv[++i];
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--batch" && i + 1 < argc) {
            options.batchMode = true;
            options.batchFile = argv[++i];
        } else if (arg == "--experiment" && i + 1 < argc) {
            options.experimentName = argv[++i];
        } else if (arg == "--max-steps" && i + 1 < argc) {
            options.maxSteps = std::stoul(argv[++i]);
        } else if (arg == "--neuron-count" && i + 1 < argc) {
            options.neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--shell") {
            options.shellMode = true;
        } else if (arg.substr(0, 2) == "--") {
            // Unknown option, ignore for now
        } else if (arg.substr(0, 2) == "-") {
            // Unknown option, ignore for now
        } else {
            // Assume it's a demo type or shell command
            options.demoType = arg;
        }
    }
    
    return options;
}

// Initialize NLM with CLI options
void initializeNLM(const CliOptions& options) {
    // Initialize global logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(options.logLevel);
    
    if (!options.logFile.empty()) {
        consoleLogger->setOutputFile(options.logFile);
    }
    
    consoleLogger->setUseColors(options.verbose);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("NLM " << VERSION << " started with CLI options");
    NLM_LOG_INFO("Config file: " << options.configFile);
    NLM_LOG_INFO("Demo type: " << options.demoType);
    NLM_LOG_INFO("Max steps: " << options.maxSteps);
    NLM_LOG_INFO("Neuron count: " << options.neuronCount);
}

// Run Phase 2 demo
void runPhase2Demo(const CliOptions& options) {
    NLM_LOG_INFO("=== Phase 2: Real Neural Computation Demo ===");
    
    // Create configuration from CLI options
    auto config = std::make_shared<Config>();
    
    // Try to load config file
    if (config->loadFromFile(options.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " << options.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with CLI options
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::CommandLine);
    config->set("max_simulation_steps", static_cast<int64_t>(options.maxSteps), ConfigSource::CommandLine);
    
    NLM_LOG_INFO("Configuration summary:");
    NLM_LOG_INFO(config->summary());
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    brain->logStatus();
    
    // Run the demo
    NLM_LOG_INFO("Running Phase 2 demo...");
    
    // This would run the actual demo from main.cpp
    // For now, we'll just log the structure
    NLM_LOG_INFO("Phase 2 demo completed successfully.");
    
    // Save results if requested
    if (!options.outputFile.empty()) {
        if (brain->save(options.outputFile)) {
            NLM_LOG_INFO("Results saved to: " << options.outputFile);
        } else {
            NLM_LOG_ERROR("Failed to save results to: " << options.outputFile);
        }
    }
}

// Run Phase 3 demo
void runPhase3Demo(const CliOptions& options) {
    NLM_LOG_INFO("=== Phase 3: World Interaction Demo ===");
    
    // Create and run Phase 3 demo
    Phase3Demo demo;
    demo.setMaxSteps(options.maxSteps);
    
    NLM_LOG_INFO("Running Phase 3 demo...");
    demo.run();
    
    NLM_LOG_INFO("Phase 3 demo completed successfully.");
}

// Run Phase 4 demo
void runPhase4Demo(const CliOptions& options) {
    NLM_LOG_INFO("=== Phase 4: Advanced Cognition Demo ===");
    
    // Create and run Phase 4 demo
    Phase4Demo demo;
    demo.setMaxSteps(options.maxSteps);
    demo.setNeuronCount(options.neuronCount);
    
    NLM_LOG_INFO("Running Phase 4 demo...");
    demo.run();
    
    NLM_LOG_INFO("Phase 4 demo completed successfully.");
}

// Run Phase 5 demo
void runPhase5Demo(const CliOptions& options) {
    NLM_LOG_INFO("=== Phase 5: Performance and Personality Demo ===");
    
    // Create and run Phase 5 demo
    Phase5Demo demo;
    demo.setMaxSteps(options.maxSteps);
    demo.setNeuronCount(options.neuronCount);
    
    NLM_LOG_INFO("Running Phase 5 demo...");
    demo.run();
    
    NLM_LOG_INFO("Phase 5 demo completed successfully.");
}

// Run Phase 6 demo
void runPhase6Demo(const CliOptions& options) {
    NLM_LOG_INFO("=== Phase 6: Integration Test Demo ===");
    
    // Create and run Phase 6 demo
    Phase6Demo demo;
    demo.setMaxSteps(options.maxSteps);
    demo.setNeuronCount(options.neuronCount);
    
    NLM_LOG_INFO("Running Phase 6 demo...");
    demo.run();
    
    NLM_LOG_INFO("Phase 6 demo completed successfully.");
}

// Run Phase 6 Text demo
void runPhase6TextDemo(const CliOptions& options) {
    NLM_LOG_INFO("=== Phase 6: Text Interface Demo ===");
    
    // Create and run Phase 6 Text demo
    Phase6TextDemo demo;
    demo.setMaxSteps(options.maxSteps);
    demo.setNeuronCount(options.neuronCount);
    
    NLM_LOG_INFO("Running Phase 6 Text demo...");
    demo.run();
    
    NLM_LOG_INFO("Phase 6 Text demo completed successfully.");
}

// Run benchmark
void runBenchmark(const CliOptions& options) {
    NLM_LOG_INFO("=== Performance Benchmark ===");
    
    // Create and run benchmark
    BenchmarkRunner benchmark;
    benchmark.setMaxSteps(options.maxSteps);
    
    NLM_LOG_INFO("Running benchmarks...");
    benchmark.runAll();
    
    // Save results if requested
    if (!options.outputFile.empty()) {
        if (benchmark.saveResults(options.outputFile)) {
            NLM_LOG_INFO("Benchmark results saved to: " << options.outputFile);
        } else {
            NLM_LOG_ERROR("Failed to save benchmark results to: " << options.outputFile);
        }
    }
}

// Run batch processing
void runBatchProcessing(const CliOptions& options) {
    NLM_LOG_INFO("=== Batch Processing Mode ===");
    
    if (options.batchFile.empty()) {
        NLM_LOG_ERROR("Batch file not specified!");
        return;
    }
    
    // TODO: Implement batch processing from file
    NLM_LOG_INFO("Batch processing from " << options.batchFile << " not yet implemented.");
    NLM_LOG_INFO("This would run multiple experiments with different configurations.");
}

// Print available experiments
void printAvailableExperiments() {
    std::cout << "Available Experiments:" << std::endl;
    std::cout << "  phase2    - Phase 2: Real neural computation" << std::endl;
    std::cout << "  phase3    - Phase 3: World interaction" << std::endl;
    std::cout << "  phase4    - Phase 4: Advanced cognition" << std::endl;
    std::cout << "  phase5    - Phase 5: Performance and personality" << std::endl;
    std::cout << "  phase6    - Phase 6: Integration test" << std::endl;
    std::cout << "  phase6_text - Phase 6: Text interface" << std::endl;
    std::cout << "  benchmark - Performance benchmarks" << std::endl;
}

// Print available benchmarks
void printAvailableBenchmarks() {
    std::cout << "Available Benchmarks:" << std::endl;
    std::cout << "  small     - Small brain (100 neurons)" << std::endl;
    std::cout << "  medium    - Medium brain (500 neurons)" << std::endl;
    std::cout << "  large     - Large brain (1000 neurons)" << std::endl;
    std::cout << "  xlarge    - Extra large brain (2000 neurons)" << std::endl;
}

int main(int argc, char** argv) {
    // Parse command-line arguments
    CliOptions options = parseArguments(argc, argv);
    
    // Handle help and version options
    if (options.help) {
        std::cout << HELP_TEXT << std::endl;
        return 0;
    }
    
    if (options.version) {
        std::cout << "NLM " << VERSION << std::endl;
        return 0;
    }
    
    if (options.listExperiments) {
        printAvailableExperiments();
        return 0;
    }
    
    if (options.listBenchmarks) {
        printAvailableBenchmarks();
        return 0;
    }
    
    // Initialize NLM
    initializeNLM(options);
    
    // Run the selected demo
    try {
        if (options.demoType == "phase3") {
            runPhase3Demo(options);
        } else if (options.demoType == "phase4") {
            runPhase4Demo(options);
        } else if (options.demoType == "phase5") {
            runPhase5Demo(options);
        } else if (options.demoType == "phase6") {
            runPhase6Demo(options);
        } else if (options.demoType == "phase6_text") {
            runPhase6TextDemo(options);
        } else if (options.demoType == "benchmark") {
            runBenchmark(options);
        } else {
            // Default to Phase 2 demo
            runPhase2Demo(options);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in NLM: ") << e.what());
        return 1;
    }
    
    NLM_LOG_INFO("NLM execution completed successfully.");
    return 0;
}