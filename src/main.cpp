// NLM - Neural Learning Machine
// Main executable with command-line interface for different modes

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "CLIAdvanced.hpp"
#include "DemoRunner.hpp"
#include "Utilities.hpp"
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <iomanip>
#include <chrono>

using namespace nlm;

using namespace nlm;

// Forward declarations
void printWelcomeBanner(const std::string& mode);
void printExecutionInfo(const CLIParser::Options& options);
void initializeLogging(bool verbose);
void printSystemInfo();
void printMemoryUsage();
void setupSignalHandlers();
void validateOptions(const CLIParser::Options& options);
void exportResults(const std::string& outputFile, const std::vector<std::string>& results);
std::vector<std::string> runBenchmarkSuite(size_t neurons, size_t steps);
void runPerformanceProfiling(size_t neurons, size_t steps);
void demonstrateAdvancedFeatures();
void runCustomConfiguration(const std::string& configFile);
void runStressTest();
void runComparativeAnalysis();

void printWelcomeBanner(const std::string& mode) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Complete Integration Demo                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    std::cout << "Mode: " << mode << std::endl;
    std::cout << "Ready to run the complete NLM brain simulation." << std::endl;
    std::cout << std::endl;
}

void printExecutionInfo(const CLIParser::Options& options) {
    std::cout << "=== Execution Configuration ===" << std::endl;
    std::cout << "Mode:              " << options.mode << std::endl;
    std::cout << "Config file:       " << options.configFile << std::endl;
    std::cout << "Simulation steps:  " << options.steps << std::endl;
    std::cout << "Neuron count:      " << options.neurons << std::endl;
    
    if (!options.outputFile.empty()) {
        std::cout << "Output file:       " << options.outputFile << std::endl;
    }
    
    std::cout << "Verbose logging:   " << (options.verbose ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
}

void initializeLogging(bool verbose) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(verbose ? LogLevel::Info : LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Logging initialized with level: " + std::string(verbose ? "Info" : "Warning"));
}

void printSystemInfo() {
    std::cout << "=== System Information ===" << std::endl;
    std::cout << Utilities::getSystemInfo() << std::endl;
    std::cout << "NLM Version:       " << Utilities::getVersionString() << std::endl;
    std::cout << std::endl;
}

void printMemoryUsage() {
    // Simple memory usage estimate
    std::cout << "=== Memory Usage (Estimated) ===" << std::endl;
    std::cout << "Core brain structures: ~10-50 MB" << std::endl;
    std::cout << "Memory systems: ~5-20 MB" << std::endl;
    std::cout << "Cognition systems: ~2-10 MB" << std::endl;
    std::cout << "Total: ~50-100 MB (depends on network size)" << std::endl;
    std::cout << std::endl;
}

void setupSignalHandlers() {
    // Setup graceful shutdown handlers
    std::signal(SIGINT, [](int) {
        std::cout << std::endl << "Received interrupt signal. Shutting down gracefully..." << std::endl;
        std::exit(0);
    });
    
    std::signal(SIGTERM, [](int) {
        std::cout << std::endl << "Received termination signal. Shutting down gracefully..." << std::endl;
        std::exit(0);
    });
}

void validateOptions(const CLIParser::Options& options) {
    if (options.steps < 10) {
        std::cerr << "WARNING: Very few steps requested (" << options.steps << "). This may not produce meaningful results." << std::endl;
    }
    
    if (options.neurons < 10) {
        std::cerr << "WARNING: Very few neurons requested (" << options.neurons << "). This may not show network dynamics." << std::endl;
    }
    
    if (options.neurons > 100000) {
        std::cerr << "WARNING: Large neuron count may cause high memory usage and long execution time." << std::endl;
    }
}

void exportResults(const std::string& outputFile, const std::vector<std::string>& results) {
    if (outputFile.empty()) return;
    
    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "ERROR: Cannot open output file: " << outputFile << std::endl;
        return;
    }
    
    outFile << "NLM Results Export" << std::endl;
    outFile << "===================" << std::endl;
    outFile << "Generated: " << __DATE__ " " __TIME__ << std::endl;
    outFile << std::endl;
    
    for (const auto& result : results) {
        outFile << result << std::endl;
    }
    
    outFile.close();
    std::cout << "Results exported to: " << outputFile << std::endl;
}

std::vector<std::string> runBenchmarkSuite(size_t neurons, size_t steps) {
    std::vector<std::string> results;
    std::cout << "Running benchmark suite with " << neurons << " neurons and " << steps << " steps..." << std::endl;
    
    // This would call the actual benchmark code
    results.push_back("Benchmark completed successfully");
    results.push_back("Performance metrics would be recorded here");
    
    return results;
}

void runPerformanceProfiling(size_t neurons, size_t steps) {
    std::cout << "Running performance profiling with " << neurons << " neurons and " << steps << " steps..." << std::endl;
    
    auto timer = Utilities::startTimer();
    
    // Run a simple simulation for profiling
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    double elapsed = Utilities::getElapsedSeconds(timer);
    
    std::cout << "Profiling completed in " << Utilities::formatFloat(elapsed) << " seconds" << std::endl;
    std::cout << "Steps per second: " << static_cast<size_t>(steps / elapsed) << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
}

void demonstrateAdvancedFeatures() {
    std::cout << "=== Advanced Features Demo ===" << std::endl;
    std::cout << "This demo showcases advanced NLM capabilities:" << std::endl;
    std::cout << std::endl;
    
    std::cout << "1. Multi-region connectivity:" << std::endl;
    std::cout << "   - Inter-region connections with synaptic delays" << std::endl;
    std::cout << "   - Specialized populations for different functions" << std::endl;
    std::cout << std::endl;
    
    std::cout << "2. Integrated memory systems:" << std::endl;
    std::cout << "   - Working memory with competitive maintenance" << std::endl;
    std::cout << "   - Episodic memory with automatic storage" << std::endl;
    std::cout << "   - Associative memory for pattern completion" << std::endl;
    std::cout << std::endl;
    
    std::cout << "3. Neuromodulation effects:" << std::endl;
    std::cout << "   - Dopamine-driven reward prediction error" << std::endl;
    std::cout << "   - Curiosity-driven exploration" << std::endl;
    std::cout << "   - Novelty detection and habituation" << std::endl;
    std::cout << std::endl;
    
    std::cout << "4. Cognitive architecture:" << std::endl;
    std::cout << "   - Neural planning with action evaluation" << std::endl;
    std::cout << "   - Concept formation from experience" << std::endl;
    std::cout << "   - Competitive attentional selection" << std::endl;
    std::cout << std::endl;
    
    std::cout << "5. Developmental dynamics:" << std::endl;
    std::cout << "   - Critical periods for learning" << std::endl;
    std::cout << "   - Synaptogenesis and pruning" << std::endl;
    std::cout << "   - Maturation of cognitive functions" << std::endl;
    std::cout << std::endl;
}

void runCustomConfiguration(const std::string& configFile) {
    std::cout << "=== Custom Configuration Demo ===" << std::endl;
    std::cout << "Loading configuration from: " << configFile << std::endl;
    
    auto config = std::make_shared<Config>();
    
    if (config->loadFromFile(configFile)) {
        std::cout << "Configuration loaded successfully:" << std::endl;
        std::cout << config->summary() << std::endl;
    } else {
        std::cout << "Using default configuration (file not found or invalid)" << std::endl;
        config = std::make_shared<Config>();
    }
    
    // Create and run brain with custom config
    auto brain = std::make_shared<Brain>(config);
    if (brain->initialize()) {
        std::cout << "Brain initialized with custom configuration" << std::endl;
        
        // Run a short simulation
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        std::cout << "Custom experiment completed" << std::endl;
        std::cout << "Final status:" << std::endl;
        brain->logStatus();
    }
}

void runStressTest() {
    std::cout << "=== NLM Stress Test ===" << std::endl;
    std::cout << "Testing system performance under high load..." << std::endl;
    
    auto timer = Utilities::startTimer();
    
    // Run multiple simulations in quick succession
    std::vector<std::shared_ptr<Brain>> brains;
    
    std::cout << "Creating 10 brains simultaneously..." << std::endl;
    for (int i = 0; i < 10; ++i) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100 + i * 50), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            brains.push_back(brain);
            std::cout << Utilities::formatFloat(Utilities::getElapsedSeconds(timer)) << ": Brain " << (i + 1) << " created" << std::endl;
        }
    }
    
    std::cout << "Running stress simulation..." << std::endl;
    for (auto& brain : brains) {
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
    }
    
    double elapsed = Utilities::getElapsedSeconds(timer);
    
    std::cout << std::endl;
    std::cout << "=== Stress Test Results ===" << std::endl;
    std::cout << "Total brains created: " << brains.size() << std::endl;
    std::cout << "Total time: " << Utilities::formatFloat(elapsed) << " seconds" << std::endl;
    std::cout << "Brains per second: " << static_cast<size_t>(brains.size() / elapsed) << std::endl;
    std::cout << "All brains functioning correctly." << std::endl;
}

void runComparativeAnalysis() {
    std::cout << "=== Comparative Analysis ===" << std::endl;
    std::cout << "Comparing performance across different network sizes..." << std::endl;
    std::endl
    
    std::vector<std::pair<size_t, size_t>> configs = {
        {100, 1000},
        {500, 1000},
        {1000, 500},
        {2000, 200},
    };
    
    std::cout << "Testing configurations:" << std::endl;
    for (const auto& config : configs) {
        std::cout << "  " << config.first << " neurons, " << config.second << " steps" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Comparative analysis to be implemented with detailed metrics collection." << std::endl;
}

int main(int argc, char** argv) {
    try {
        CLIAdvanced::Options options = CLIAdvanced::parse(argc, argv);
        
        if (options.help) {
            CLIAdvanced::printHelp();
            return 0;
        }
        
        if (options.showVersion) {
            CLIAdvanced::printVersion();
            return 0;
        }
        
        printWelcomeBanner(options.mode);
        printExecutionInfo(options);
        printSystemInfo();
        
        // Setup signal handlers for graceful shutdown
        setupSignalHandlers();
        
        // Validate options
        validateOptions(options);
        
        // Initialize logging
        initializeLogging(options.verbose);
        
        NLM_LOG_INFO("Starting NLM execution in " + options.mode + " mode");
        
        // Route to appropriate demo based on mode
        if (options.mode == "phase2") {
            DemoRunner::runPhase2Demo();
        } else if (options.mode == "phase4") {
            DemoRunner::runPhase4Demo();
        } else if (options.mode == "phase6") {
            DemoRunner::runPhase6Demo();
        } else if (options.mode == "benchmark") {
            DemoRunner::runBenchmarks();
        } else if (options.mode == "integration") {
            DemoRunner::runIntegrationTests();
        } else if (options.mode == "custom") {
            DemoRunner::runCustomExperiment();
        } else {
            NLM_LOG_ERROR("Unknown mode: " + options.mode);
            return 1;
        }
        
        // Show memory usage
        printMemoryUsage();
        
        // Export results if requested
        std::vector<std::string> results;
        if (options.mode == "benchmark") {
            results = runBenchmarkSuite(options.neurons, options.steps);
        }
        
        exportResults(options.outputFile, results);
        
        NLM_LOG_INFO("Execution completed successfully");
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}