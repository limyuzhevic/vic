// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation - Refactored Version
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "core/TestConstants.hpp"
#include "core/Phase2ConfigManager.hpp"
#include "core/Phase2TestFramework.hpp"
#include "core/Phase2CLI.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
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

using namespace nlm;

// Include Phase 2 test implementations
#include "Phase2Tests/ConnectivityTest.hpp"
#include "Phase2Tests/PlasticityTest.hpp"
#include "Phase2Tests/StdpTest.hpp"
#include "Phase2Tests/LearningExperiment.hpp"

// Main application class
class Phase2Application {
public:
    Phase2Application() = default;
    ~Phase2Application() = default;
    
    // Run the application
    int run(int argc, char** argv);
    
    // Initialize the application
    bool initialize();
    
    // Cleanup the application
    void cleanup();
    
    // Print banner
    void printBanner() const;
    
    // Print help information
    void printHelp() const;
    
    // Execute tests
    bool executeTests(std::shared_ptr<Config> config);
    
    // Execute benchmarks
    bool executeBenchmarks(std::shared_ptr<Config> config);
    
    // Save results
    bool saveResults(const std::string& filepath) const;
    
    // Load results
    bool loadResults(const std::string& filepath);
    
private:
    // Helper functions
    void setupLogging(const Phase2CLI& cli);
    std::shared_ptr<Config> createCoreConfig() const;
    void printSystemInfo() const;
    
    // State
    bool initialized_ = false;
    std::shared_ptr<Phase2CLI> cli_;
    std::shared_ptr<Phase2TestFramework> testFramework_;
    std::shared_ptr<Phase2Benchmark> benchmark_;
};

// Print application banner
void Phase2Application::printBanner() const {
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

// Print help information
void Phase2Application::printHelp() const {
    std::cout << "=== NLM Phase 2 Help ===" << std::endl;
    std::cout << "Phase 2 Neural Learning Machine Tests" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
    std::cout << "  --config <file>           Load configuration from file" << std::endl;
    std::cout << "  --output <file>           Save results to file" << std::endl;
    std::cout << "  --checkpoint <file>       Checkpoint file path" << std::endl;
    std::cout << "  --test <name>[,<name>...]  Run specific tests" << std::endl;
    std::cout << "  --benchmark               Run performance benchmarks" << std::endl;
    std::cout << "  --verbose                 Enable verbose output" << std::endl;
    std::cout << "  --neuron-count <count>    Number of neurons" << std::endl;
    std::cout << "  --plasticity-steps <steps> Number of plasticity steps" << std::endl;
    std::cout << "  --seed <seed>             Random seed" << std::endl;
    std::cout << std::endl;
    std::cout << "Available tests:" << std::endl;
    auto& framework = Phase2TestFramework::getInstance();
    for (const auto& test : framework.getAllTests()) {
        std::cout << "  " << test->getName() << ": " << test->getDescription() << std::endl;
    }
    std::cout << std::endl;
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
}

// Initialize the application
bool Phase2Application::initialize() {
    try {
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Initialize test framework
        testFramework_ = std::make_shared<Phase2TestFramework>();
        testFramework_->registerDefaultTests();
        
        // Initialize benchmark
        benchmark_ = std::make_shared<Phase2Benchmark>();
        
        initialized_ = true;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error initializing application: " << e.what() << std::endl;
        return false;
    }
}

// Cleanup the application
void Phase2Application::cleanup() {
    if (initialized_) {
        testFramework_.reset();
        benchmark_.reset();
        Logger::setGlobal(nullptr);
        initialized_ = false;
    }
}

// Setup logging
void Phase2Application::setupLogging(const Phase2CLI& cli) {
    auto& logger = Logger::getGlobal();
    
    // Set log level based on verbosity
    if (cli.getOptions().enableVerbose) {
        logger->setLevel(LogLevel::Debug);
    } else {
        logger->setLevel(LogLevel::Info);
    }
    
    // Configure file logging if specified
    if (!cli.getOptions().logFile.empty()) {
        // Add file logger (simplified implementation)
    }
}

// Create core configuration
std::shared_ptr<Config> Phase2Application::createCoreConfig() const {
    auto config = std::make_shared<Config>();
    
    // Set default values
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", PHASE2_SIMULATION_TIMESTEP, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(PHASE2_NEURON_COUNT), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(PHASE2_REGION_COUNT), ConfigSource::Default);
    config->set("connection_probability", PHASE2_CONNECTION_PROBABILITY, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", PHASE2_STDP_LTP_WEIGHT, ConfigSource::Default);
    config->set("stdp_ltd_weight", PHASE2_STDP_LTD_WEIGHT, ConfigSource::Default);
    config->set("stdp_tau", PHASE2_STDP_TAU, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", PHASE2_SYNAPTOGENESIS_RATE, ConfigSource::Default);
    config->set("pruning_rate", PHASE2_PRUNING_RATE, ConfigSource::Default);
    
    return config;
}

// Print system information
void Phase2Application::printSystemInfo() const {
    std::cout << "=== NLM Phase 2 System Information ===" << std::endl;
    std::cout << "Project: NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Phase: 2 - Real Neural Computation" << std::endl;
    std::cout << "Compiler: " << __VERSION__ << std::endl;
    std::cout << "Build Type: Release" << std::endl;
    std::cout << "=========================================" << std::endl;
}

// Execute tests
bool Phase2Application::executeTests(std::shared_ptr<Config> config) {
    if (!testFramework_) {
        std::cerr << "Error: Test framework not initialized" << std::endl;
        return false;
    }
    
    try {
        auto results = testFramework_->runAllTests(config);
        
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        
        int passed = 0;
        int failed = 0;
        
        for (const auto& result : results) {
            std::cout << result.testName << ": ";
            if (result.passed) {
                std::cout << "PASSED" << std::endl;
                passed++;
            } else {
                std::cout << "FAILED" << std::endl;
                failed++;
                std::cout << "  Message: " << result.message << std::endl;
            }
            
            if (!result.details.empty()) {
                std::cout << "  Details: " << result.details.size() << " items" << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "Total Tests: " << results.size() << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        
        return failed == 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error executing tests: " << e.what() << std::endl;
        return false;
    }
}

// Execute benchmarks
bool Phase2Application::executeBenchmarks(std::shared_ptr<Config> config) {
    if (!benchmark_) {
        std::cerr << "Error: Benchmark not initialized" << std::endl;
        return false;
    }
    
    try {
        // Run benchmarks for key operations
        benchmark_->setMaxIterations(10);
        benchmark_->setMinTimeThreshold(0.1);
        
        // Neuron initialization benchmark
        auto neuronBenchmark = benchmark_->runBenchmarkIterations(
            "Neuron Initialization",
            [config]() {
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
            },
            10
        );
        
        // Synapse formation benchmark
        auto synapseBenchmark = benchmark_->runBenchmarkIterations(
            "Synapse Formation",
            [config]() {
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                brain->step(0, 0.001);
            },
            10
        );
        
        benchmark_->printBenchmarkSummary();
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error executing benchmarks: " << e.what() << std::endl;
        return false;
    }
}

// Save results
bool Phase2Application::saveResults(const std::string& filepath) const {
    // Implementation for saving results to file
    std::cout << "Saving results to: " << filepath << std::endl;
    // TODO: Implement actual file saving
    return true;
}

// Load results
bool Phase2Application::loadResults(const std::string& filepath) {
    // Implementation for loading results from file
    std::cout << "Loading results from: " << filepath << std::endl;
    // TODO: Implement actual file loading
    return true;
}

// Main application entry point
int Phase2Application::run(int argc, char** argv) {
    // Initialize application
    if (!initialize()) {
        return 1;
    }
    
    try {
        // Parse command line arguments
        cli_ = std::make_shared<Phase2CLI>();
        if (!cli_->parseArgs(argc, argv)) {
            std::cerr << "Error parsing command line arguments: " << cli_->getErrorMessage() << std::endl;
            printHelp();
            return 1;
        }
        
        // Setup logging
        setupLogging(*cli_);
        
        // Print banner
        printBanner();
        
        // Print system information
        printSystemInfo();
        
        // Print help if requested
        if (cli_->getOptions().help) {
            printHelp();
            return 0;
        }
        
        // Print version if requested
        if (cli_->getOptions().version) {
            std::cout << "NLM Phase 2 Version 0.1.0" << std::endl;
            return 0;
        }
        
        // Print available tests if requested
        if (cli_->getOptions().listTests) {
            cli_->printAvailableTests();
            return 0;
        }
        
        // Create core configuration
        auto config = createCoreConfig();
        
        // Print configuration summary
        auto configManager = Phase2TestConfigManager::getInstance();
        configManager.printConfigSummary();
        
        // Execute tests
        bool testsPassed = executeTests(config);
        
        // Execute benchmarks if requested
        if (cli_->getOptions().enableBenchmark) {
            std::cout << std::endl;
            std::cout << "=== Performance Benchmarks ===" << std::endl;
            bool benchmarksPassed = executeBenchmarks(config);
            if (!benchmarksPassed) {
                std::cerr << "Benchmarks failed" << std::endl;
                return 1;
            }
        }
        
        // Save results if requested
        if (!cli_->getOptions().outputFile.empty()) {
            if (!saveResults(cli_->getOptions().outputFile)) {
                std::cerr << "Error saving results" << std::endl;
                return 1;
            }
        }
        
        // Return appropriate exit code
        return testsPassed ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
        
    } finally {
        cleanup();
    }
}

// Main function
int main(int argc, char** argv) {
    Phase2Application app;
    return app.run(argc, argv);
}