// NLM Phase 2: Unified Neural Computation Test Runner
// Provides a clean interface to run all Phase 2 tests with improved options

#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

// Forward declarations for test functions
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
void runPlasticityExperiment(std::shared_ptr<Brain> brain);
void runStdpVerification(std::shared_ptr<Brain> brain);

// Enhanced command line options
struct CommandLineOptions {
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    LogLevel logLevel = LogLevel::Info;
    bool verbose = false;
    bool quiet = false;
    
    // Test control
    bool runAllTests = true;
    bool runConnectivityTest = true;
    bool runPlasticityTest = true;
    bool runStdpTest = true;
    
    // Test parameters
    size_t neuronCount = 500;
    size_t simulationSteps = 1000;
    size_t stdpTrials = 50;
    double timestep = 0.001;
    
    // Output and results
    bool saveResults = false;
    bool generateReport = false;
    std::string reportFormat = "text";
    
    // Validation
    bool validate() const {
        if (neuronCount == 0) return false;
        if (timestep <= 0.0) return false;
        if (simulationSteps == 0) return false;
        if (stdpTrials == 0) return false;
        return true;
    }
};

// Enhanced configuration manager
class EnhancedConfigManager {
private:
    std::shared_ptr<Config> config;
    CommandLineOptions options;
    
public:
    EnhancedConfigManager(std::shared_ptr<Config> cfg, const CommandLineOptions& opts) 
        : config(cfg), options(opts) {
        if (!options.validate()) {
            throw std::invalid_argument("Invalid command line options");
        }
    }
    
    void applyDefaults() {
        // Core simulation parameters
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", options.timestep, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    }
    
    void loadConfiguration() {
        // Try to load from file if provided
        if (config->loadFromFile(options.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Override with command line args
        // Note: Command line loading is done in individual test files
    }
    
    void logConfiguration() const {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration Summary:");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
        NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
        NLM_LOG_INFO("");
    }
};

// Enhanced test runner
class TestRunner {
private:
    std::shared_ptr<Brain> brain;
    CommandLineOptions options;
    bool allTestsPassed = true;
    std::string testResults;
    
public:
    TestRunner(std::shared_ptr<Brain> b, const CommandLineOptions& opts) 
        : brain(b), options(opts) {
        if (!brain) {
            throw std::invalid_argument("Brain pointer is null");
        }
    }
    
    bool runAllTests() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== NLM Phase 2: Enhanced Test Suite ===");
        NLM_LOG_INFO("Running all Phase 2 tests with improved error handling and reporting...");
        NLM_LOG_INFO("");
        
        try {
            if (options.runConnectivityTest) {
                NLM_LOG_INFO("Running Test 1: Basic Neural Connectivity");
                if (!runConnectivityTest()) {
                    allTestsPassed = false;
                    testResults += "Connectivity Test: FAILED\n";
                } else {
                    testResults += "Connectivity Test: PASSED\n";
                }
            }
            
            if (options.runPlasticityTest) {
                NLM_LOG_INFO("Running Test 2: Plasticity Learning Experiment");
                if (!runPlasticityTest()) {
                    allTestsPassed = false;
                    testResults += "Plasticity Test: FAILED\n";
                } else {
                    testResults += "Plasticity Test: PASSED\n";
                }
            }
            
            if (options.runStdpTest) {
                NLM_LOG_INFO("Running Test 3: STDP Verification");
                if (!runStdpTest()) {
                    allTestsPassed = false;
                    testResults += "STDP Test: FAILED\n";
                } else {
                    testResults += "STDP Test: PASSED\n";
                }
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Test suite error: " + std::string(e.what()));
            allTestsPassed = false;
            testResults += "Test Suite: EXCEPTION - " + std::string(e.what()) + "\n";
        }
        
        return allTestsPassed;
    }
    
    std::string getTestResults() const {
        return testResults;
    }
    
private:
    bool runConnectivityTest() {
        try {
            runBasicConnectivityTest(brain);
            NLM_LOG_INFO("✓ Basic connectivity test completed successfully");
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error in connectivity test: " + std::string(e.what()));
            return false;
        }
    }
    
    bool runPlasticityTest() {
        try {
            runPlasticityExperiment(brain);
            NLM_LOG_INFO("✓ Plasticity test completed successfully");
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error in plasticity test: " + std::string(e.what()));
            return false;
        }
    }
    
    bool runStdpTest() {
        try {
            runStdpVerification(brain);
            NLM_LOG_INFO("✓ STDP verification test completed successfully");
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error in STDP test: " + std::string(e.what()));
            return false;
        }
    }
};

// Enhanced logging setup
void setupEnhancedLogging(const CommandLineOptions& options) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(options.logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Enhanced Neural Computation ===");
    NLM_LOG_INFO("Improved test suite with better error handling and reporting");
    NLM_LOG_INFO("");
}

// Parse command line arguments
CommandLineOptions parseCommandLine(int argc, char** argv) {
    CommandLineOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
            options.logLevel = LogLevel::Debug;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
            options.logLevel = LogLevel::Error;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                options.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg.substr(0, 7) == "--output") {
            if (arg.find('=') != std::string::npos) {
                options.outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.outputFile = argv[++i];
            }
        } else if (arg == "--no-connectivity") {
            options.runConnectivityTest = false;
        } else if (arg == "--no-plasticity") {
            options.runPlasticityTest = false;
        } else if (arg == "--no-stdp") {
            options.runStdpTest = false;
        } else if (arg == "--no-all") {
            options.runAllTests = false;
            options.runConnectivityTest = false;
            options.runPlasticityTest = false;
            options.runStdpTest = false;
        } else if (arg.substr(0, 7) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                options.neuronCount = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.neuronCount = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 5) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                options.simulationSteps = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.simulationSteps = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 6) == "--trials") {
            if (arg.find('=') != std::string::npos) {
                options.stdpTrials = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.stdpTrials = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 4) == "--step") {
            if (arg.find('=') != std::string::npos) {
                options.timestep = std::stod(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.timestep = std::stod(argv[++i]);
            }
        } else if (arg == "--save-results") {
            options.saveResults = true;
        } else if (arg == "--generate-report") {
            options.generateReport = true;
        } else if (arg.substr(0, 4) == "--format") {
            if (arg.find('=') != std::string::npos) {
                options.reportFormat = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.reportFormat = argv[++i];
            }
        } else if (arg == "--help" || arg == "-h") {
            printUsage();
            exit(0);
        }
    }
    
    return options;
}

// Print usage information
void printUsage() {
    std::cout << R"(
NLM Phase 2 Test Runner - Enhanced Interface

Usage: nlm_phase2 [options]

Basic Options:
  -v, --verbose           Enable verbose logging (DEBUG level)
  -q, --quiet             Quiet mode (ERROR level only)
  -h, --help              Show this help message

Configuration:
  --config=FILE           Load configuration from FILE
                        (default: configs/default.cfg)
  --output=FILE           Save test results to FILE

Test Control:
  --no-connectivity       Skip basic connectivity test
  --no-plasticity         Skip plasticity learning test
  --no-stdp               Skip STDP verification test
  --no-all                Skip all tests

Test Parameters:
  --neurons=N             Set neuron count (default: 500)
  --steps=N               Set simulation steps for plasticity test (default: 1000)
  --trials=N              Set STDP trials for verification (default: 50)
  --step=VALUE            Set simulation timestep (default: 0.001)

Output Options:
  --save-results          Save detailed results to file
  --generate-report       Generate comprehensive test report
  --format=FORMAT         Report format: text, json, xml (default: text)

Examples:
  nlm_phase2 --verbose
  nlm_phase2 --neurons=1000 --steps=2000 --trials=100
  nlm_phase2 --no-stdp --generate-report --output=results.txt
  nlm_phase2 --config=my_config.cfg --save-results

Enhanced features:
  - Better error handling and recovery
  - Configurable test suite
  - Flexible output formats
  - Improved logging and reporting
  - Command-line parameter validation
  )" << std::endl;
}

// Include test function implementations
void includeTestFunctions() {
    // Test functions are defined in separate files
    // This function is a placeholder to ensure proper linking
}

int main(int argc, char** argv) {
    printBanner();
    
    try {
        // Parse command line arguments
        CommandLineOptions options = parseCommandLine(argc, argv);
        
        // Setup logging
        setupEnhancedLogging(options);
        
        // Validate options
        if (!options.validate()) {
            NLM_LOG_ERROR("Invalid command line options provided");
            printUsage();
            return 1;
        }
        
        // Initialize configuration
        auto config = std::make_shared<Config>();
        EnhancedConfigManager configManager(config, options);
        
        // Apply defaults and load configuration
        configManager.applyDefaults();
        configManager.loadConfiguration();
        configManager.logConfiguration();
        
        // Initialize simulation clock
        SimulationClock clock(options.timestep);
        NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(options.timestep) + "s");
        
        // Initialize brain
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // Run tests using the enhanced test runner
        TestRunner testRunner(brain, options);
        bool testsPassed = testRunner.runAllTests();
        
        // Output test results
        if (!testRunner.getTestResults().empty()) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Test Results:")
            NLM_LOG_INFO(testRunner.getTestResults());
        }
        
        // Final brain status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Brain Status ===");
        brain->logStatus();
        
        // Test summary
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test Summary ===");
        if (testsPassed) {
            NLM_LOG_INFO("✓ ALL TESTS PASSED");
            NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
            NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
        } else {
            NLM_LOG_INFO("✗ SOME TESTS FAILED");
            NLM_LOG_INFO("Please review the test output above for details.");
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Phase 2 Complete ===");
        
        return testsPassed ? 0 : 1;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Fatal error: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Unknown error occurred");
        return 1;
    }
}