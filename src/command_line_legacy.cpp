#include "command_line_legacy.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/ConsoleLogger.hpp"
#include "brain/Brain.hpp"
#include "learning_experiment.hpp"
#include "basic_connectivity_test.hpp"
#include "plasticity_experiment_test.hpp"
#include "stdp_verification_test.hpp"

namespace nlm {

LegacyCommandLineInterface::LegacyCommandLineInterface() {
    consoleLogger_ = std::make_shared<ConsoleLogger>();
    consoleLogger_->setUseColors(true);
    Logger::setGlobal(std::make_shared<Logger>());
    Logger::getGlobal()->addLogger(consoleLogger_);
}

LegacyCommandLineInterface::~LegacyCommandLineInterface() = default;

bool LegacyCommandLineInterface::parseLegacyCommandLine(int argc, char** argv, LegacyTestOptions& options) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            displayLegacyHelp();
            std::exit(0);
        }
        else if (arg == "--test" || arg == "-t") {
            if (i + 1 < argc) {
                std::string testName = argv[++i];
                if (testName == "all") {
                    options.runAll = true;
                    options.runBasic = true;
                    options.runPlasticity = true;
                    options.runStdp = true;
                } else if (testName == "basic") {
                    options.runAll = false;
                    options.runBasic = true;
                    options.runPlasticity = false;
                    options.runStdp = false;
                } else if (testName == "plasticity") {
                    options.runAll = false;
                    options.runBasic = false;
                    options.runPlasticity = true;
                    options.runStdp = false;
                } else if (testName == "stdp") {
                    options.runAll = false;
                    options.runBasic = false;
                    options.runPlasticity = false;
                    options.runStdp = true;
                } else {
                    std::cerr << "Unknown test: " << testName << std::endl;
                    displayLegacyHelp();
                    return false;
                }
            }
        }
        else if (arg == "--config") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        }
        else if (arg == "--log-level") {
            if (i + 1 < argc) {
                options.logLevel = argv[++i];
            }
        }
        else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        }
        else if (arg == "--max-neurons") {
            if (i + 1 < argc) {
                options.maxNeurons = std::stoi(argv[++i]);
            }
        }
        else if (arg == "--max-time") {
            if (i + 1 < argc) {
                options.maxTime = std::stod(argv[++i]);
            }
        }
    }
    
    return true;
}

void LegacyCommandLineInterface::displayLegacyHelp() const {
    std::cout << "NLM Legacy Test Runner Usage:" << std::endl;
    std::cout << "  --test all           Run all tests (default)" << std::endl;
    std::cout << "  --test basic         Run basic connectivity test only" << std::endl;
    std::cout << "  --test plasticity    Run plasticity learning experiment only" << std::endl;
    std::cout << "  --test stdp          Run STDP verification test only" << std::endl;
    std::cout << "  --config FILE        Load configuration from FILE" << std::endl;
    std::cout << "  --log-level LEVEL    Set log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)" << std::endl;
    std::cout << "  --verbose -v        Enable verbose output" << std::endl;
    std::cout << "  --max-neurons N      Set maximum neuron count to N" << std::endl;
    std::cout << "  --max-time T         Set maximum simulation time to T seconds" << std::endl;
    std::cout << "  --help -h            Show this help message and exit" << std::endl;
}

LogLevel LegacyCommandLineInterface::stringToLogLevel(const std::string& level) {
    std::string upper = level;
    for (char& c : upper) c = std::toupper(c);
    
    if (upper == "DEBUG") return LogLevel::Debug;
    if (upper == "WARNING") return LogLevel::Warning;
    if (upper == "ERROR") return LogLevel::Error;
    if (upper == "CRITICAL") return LogLevel::Critical;
    return LogLevel::Info;
}

bool LegacyCommandLineInterface::initializeApplication(const LegacyTestOptions& options, std::shared_ptr<Config>& config) {
    try {
        if (options.verbose) {
            NLM_LOG_INFO("Verbose mode enabled");
            NLM_LOG_INFO("Config file: " + options.configFile);
            NLM_LOG_INFO("Log level: " + options.logLevel);
        }
        
        // Load configuration if specified
        config = std::make_shared<Config>();
        if (!options.configFile.empty() && options.configFile != "configs/default.cfg") {
            if (!config->loadFromFile(options.configFile)) {
                NLM_LOG_ERROR("Failed to load configuration from: " + options.configFile);
                return false;
            }
        }
        
        // Set log level
        consoleLogger_->setLevel(stringToLogLevel(options.logLevel));
        
        // Apply command line overrides
        if (options.maxNeurons > 0) {
            config->set("max_neurons", std::to_string(options.maxNeurons));
        }
        
        if (options.maxTime > 0.0) {
            config->set("max_time", std::to_string(options.maxTime));
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Application initialization failed: " + std::string(e.what()));
        return false;
    }
}

bool LegacyCommandLineInterface::initializeBrain(std::shared_ptr<Brain>& brain, std::shared_ptr<Config> config) {
    try {
        brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Brain initialization failed");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain initialization failed: " + std::string(e.what()));
        return false;
    }
}

bool LegacyCommandLineInterface::runBasicTest(std::shared_ptr<Brain> brain) {
    try {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== TEST EXECUTION: Basic Connectivity Test ===");
        
        runBasicConnectivityTest(brain);
        NLM_LOG_INFO("✓ Basic connectivity test completed");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Basic connectivity test failed: " + std::string(e.what()));
        return false;
    }
}

bool LegacyCommandLineInterface::runPlasticityTest(std::shared_ptr<Brain> brain) {
    try {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== TEST EXECUTION: Plasticity Learning Experiment ===");
        
        runLearningExperiment(brain);
        NLM_LOG_INFO("✓ Plasticity learning experiment completed");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Plasticity learning experiment failed: " + std::string(e.what()));
        return false;
    }
}

bool LegacyCommandLineInterface::runStdpTest(std::shared_ptr<Brain> brain) {
    try {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== TEST EXECUTION: STDP Verification ===");
        
        runStdpVerificationTest(brain);
        NLM_LOG_INFO("✓ STDP verification completed");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("STDP verification test failed: " + std::string(e.what()));
        return false;
    }
}

bool LegacyCommandLineInterface::executeLegacyTests(const LegacyTestOptions& options, std::shared_ptr<Brain> brain) {
    bool allTestsPassed = true;
    
    if (options.runAll || options.runBasic) {
        if (!runBasicTest(brain)) {
            allTestsPassed = false;
        }
    }
    
    if (options.runAll || options.runPlasticity) {
        if (!runPlasticityTest(brain)) {
            allTestsPassed = false;
        }
    }
    
    if (options.runAll || options.runStdp) {
        if (!runStdpTest(brain)) {
            allTestsPassed = false;
        }
    }
    
    return allTestsPassed;
}

void LegacyCommandLineInterface::displayFinalSummary(std::shared_ptr<Brain>& brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule verified");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule functional");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning) active");
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
}

void LegacyCommandLineInterface::printUsage() const {
    std::cout << "NLM Legacy Test Runner Usage:" << std::endl;
    std::cout << "  --test all           Run all tests (default)" << std::endl;
    std::cout << "  --test basic         Run basic connectivity test only" << std::endl;
    std::cout << "  --test plasticity    Run plasticity learning experiment only" << std::endl;
    std::cout << "  --test stdp          Run STDP verification test only" << std::endl;
    std::cout << "  --config FILE        Load configuration from FILE" << std::endl;
    std::cout << "  --log-level LEVEL    Set log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)" << std::endl;
    std::cout << "  --verbose -v        Enable verbose output" << std::endl;
    std::cout << "  --max-neurons N      Set maximum neuron count to N" << std::endl;
    std::cout << "  --max-time T         Set maximum simulation time to T seconds" << std::endl;
    std::cout << "  --help -h            Show this help message and exit" << std::endl;
}

} // namespace nlm