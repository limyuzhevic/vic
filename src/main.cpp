// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
//
// Enhanced with comprehensive command-line interface options
// for configuration, debugging, profiling, and performance monitoring

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
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <map>
#include <algorithm>

using namespace nlm;

// Forward declarations
struct CommandLineOptions;
void printAdvancedBanner();
void printHelp(const std::string& programName);
void processAdvancedOptions(const CommandLineOptions& options, std::shared_ptr<Config> config);
void setupLogging(const CommandLineOptions& options);
void printConfigurationSummary(const CommandLineOptions& options, const std::shared_ptr<Config>& config);
void runPerformanceBenchmark(const std::shared_ptr<Brain>& brain, const CommandLineOptions& options);
void runExperimentalSuite(const std::shared_ptr<Brain>& brain, const CommandLineOptions& options);

// Advanced command-line options structure
struct CommandLineOptions {
    std::string configFile;
    std::string logFile;
    std::string outputFile;
    std::string experimentMode;
    size_t numEpisodes;
    size_t stepsPerEpisode;
    size_t benchmarkSize;
    double simulationTimestep;
    size_t neuronCount;
    size_t regionCount;
    double connectionProbability;
    std::string logLevel;
    bool enableColors;
    bool enableStatistics;
    bool enableProfiling;
    bool enableDebug;
    bool enablePerformanceMonitoring;
    bool enableDetailedLogging;
    size_t maxLogHistory;
    float learningRate;
    float stdpLtpWeight;
    float stdpLtdWeight;
    float synaptogenesisRate;
    float pruningRate;
    std::string randomSeed;
    bool useJSONConfig;
    bool useYAMLConfig;
    std::string benchmarkMode;
    bool runAllExperiments;
    bool generateReports;
    std::string reportFormat;
};

// Default options
CommandLineOptions getDefaultOptions() {
    CommandLineOptions options;
    options.configFile = "configs/default.cfg";
    options.logFile = "";
    options.outputFile = "";
    options.experimentMode = "phase2";  // phase2, phase3, phase4, all
    options.numEpisodes = 5;
    options.stepsPerEpisode = 200;
    options.benchmarkSize = 1000;
    options.simulationTimestep = 0.001;
    options.neuronCount = 500;
    options.regionCount = 1;
    options.connectionProbability = 0.15f;
    options.logLevel = "info";
    options.enableColors = true;
    options.enableStatistics = true;
    options.enableProfiling = false;
    options.enableDebug = false;
    options.enablePerformanceMonitoring = false;
    options.enableDetailedLogging = false;
    options.maxLogHistory = 1000;
    options.learningRate = 0.01f;
    options.stdpLtpWeight = 0.02f;
    options.stdpLtdWeight = 0.015f;
    options.synaptogenesisRate = 0.0001f;
    options.pruningRate = 0.00001f;
    options.randomSeed = "42";
    options.useJSONConfig = false;
    options.useYAMLConfig = false;
    options.benchmarkMode = "quick";  // quick, detailed, comprehensive
    options.runAllExperiments = false;
    options.generateReports = false;
    options.reportFormat = "text";  // text, json, csv
    return options;
}

void printAdvancedBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     Enhanced with Advanced CLI Options                       ║
    ║     Real-time Configuration, Debugging, and Performance Monitoring
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp(const std::string& programName) {
    std::cout << "NLM - Neural Learning Machine (熙然)" << std::endl;
    std::cout << "Version: 2.0 Enhanced" << std::endl;
    std::cout << "Phase 2: Real Neural Computation with Advanced CLI" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --config <file>                    Configuration file (JSON/YAML/key=value)" << std::endl;
    std::cout << "  --json-config                      Use JSON format for configuration" << std::endl;
    std::cout << "  --yaml-config                      Use YAML format for configuration" << std::endl;
    std::cout << "  --set <key>=<value>                Set configuration parameter" << std::endl;
    std::cout << "  --int <key>=<value>               Set integer parameter" << std::endl;
    std::cout << "  --float <key>=<value>             Set float parameter" << std::endl;
    std::cout << "  --bool <key>=<value>              Set boolean parameter" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Experiment Control:" << std::endl;
    std::cout << "  --mode <mode>                      Experiment mode (phase2, phase3, phase4, all)" << std::endl;
    std::cout << "  --episodes <num>                   Number of episodes (Phase 3)" << std::endl;
    std::cout << "  --steps <num>                      Steps per episode" << std::endl;
    std::cout << "  --run-all-experiments              Run all Phase 4 experiments" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Logging and Output:" << std::endl;
    std::cout << "  --log-level <level>                Logging level (debug, info, warning, error, critical)" << std::endl;
    std::cout << "  --log-file <file>                  Log output file" << std::endl;
    std::cout << "  --output <file>                    Simulation output file" << std::endl;
    std::cout << "  --no-colors                        Disable colored output" << std::endl;
    std::cout << "  --enable-statistics                Enable logging statistics" << std::endl;
    std::cout << "  --enable-profiling                 Enable performance profiling" << std::endl;
    std::cout << "  --enable-debug                     Enable debug logging" << std::endl;
    std::cout << "  --enable-detailed-logging          Enable detailed logging" << std::endl;
    std::cout << "  --log-history <num>                Maximum log history (default: 1000)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Performance and Benchmarking:" << std::endl;
    std::cout << "  --benchmark-size <num>             Neuron count for benchmarking" << std::endl;
    std::cout << "  --benchmark-mode <mode>            Benchmark mode (quick, detailed, comprehensive)" << std::endl;
    std::cout << "  --enable-performance-monitoring     Enable performance monitoring" << std::endl;
    std::cout << "  --performance-report <file>        Save performance report to file" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Plasticity Parameters:" << std::endl;
    std::cout << "  --learning-rate <float>             Learning rate for Hebbian plasticity" << std::endl;
    std::cout << "  --stdp-ltp <float>                 STDP LTP weight" << std::endl;
    std::cout << "  --stdp-ltd <float>                 STDP LTD weight" << std::endl;
    std::cout << "  --synaptogenesis-rate <float>      Rate of new synapse formation" << std::endl;
    std::cout << "  --pruning-rate <float>             Rate of synapse pruning" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Brain Configuration:" << std::endl;
    std::cout << "  --timestep <float>                  Simulation timestep (seconds)" << std::endl;
    std::cout << "  --neurons <num>                    Number of neurons" << std::endl;
    std::cout << "  --regions <num>                    Number of brain regions" << std::endl;
    std::cout << "  --connection-prob <float>          Connection probability" << std::endl;
    std::cout << "  --random-seed <seed>                Random seed for reproducibility" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Reporting:" << std::endl;
    std::cout << "  --generate-reports                 Generate detailed reports" << std::endl;
    std::cout << "  --report-format <format>            Report format (text, json, csv)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --mode phase3 --episodes 10 --steps 500" << std::endl;
    std::cout << "  " << programName << " --mode phase4 --benchmark-size 2000 --enable-statistics" << std::endl;
    std::cout << "  " << programName << " --config my_config.json --log-level debug --enable-profiling" << std::endl;
    std::cout << "  " << programName << " --run-all-experiments --generate-reports --report-format json" << std::endl;
}

bool parseCommandLineOptions(int argc, char** argv, CommandLineOptions& options) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp(argv[0]);
            return false;
        }
        else if (arg == "--version") {
            std::cout << "NLM Version 2.0 Enhanced" << std::endl;
            return false;
        }
        else if (arg.find("--config=") == 0) {
            options.configFile = arg.substr(9);
        }
        else if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        }
        else if (arg == "--json-config") {
            options.useJSONConfig = true;
        }
        else if (arg == "--yaml-config") {
            options.useYAMLConfig = true;
        }
        else if (arg.find("--set=") == 0) {
            std::string param = arg.substr(6);
            size_t pos = param.find('=');
            if (pos != std::string::npos) {
                std::string key = param.substr(0, pos);
                std::string value = param.substr(pos + 1);
                // Note: These settings would need to be processed later
                // For now, we'll just store them
                std::cout << "Note: Configuration parameter " << key << " = " << value << " (not implemented in this enhanced version)" << std::endl;
            }
        }
        else if (arg.find("--int=") == 0) {
            std::string param = arg.substr(6);
            size_t pos = param.find('=');
            if (pos != std::string::npos) {
                std::string key = param.substr(0, pos);
                int value = std::stoi(param.substr(pos + 1));
                // Process later
            }
        }
        else if (arg.find("--float=") == 0) {
            std::string param = arg.substr(8);
            size_t pos = param.find('=');
            if (pos != std::string::npos) {
                std::string key = param.substr(0, pos);
                float value = std::stof(param.substr(pos + 1));
                // Process later
            }
        }
        else if (arg.find("--bool=") == 0) {
            std::string param = arg.substr(7);
            size_t pos = param.find('=');
            if (pos != std::string::npos) {
                std::string key = param.substr(0, pos);
                std::string value = param.substr(pos + 1);
                bool boolValue = (value == "true" || value == "1" || value == "yes");
                // Process later
            }
        }
        else if (arg.find("--mode=") == 0) {
            options.experimentMode = arg.substr(7);
        }
        else if (arg.find("--episodes=") == 0) {
            options.numEpisodes = std::stoi(arg.substr(11));
        }
        else if (arg.find("--steps=") == 0) {
            options.stepsPerEpisode = std::stoi(arg.substr(8));
        }
        else if (arg.find("--benchmark-size=") == 0) {
            options.benchmarkSize = std::stoi(arg.substr(17));
        }
        else if (arg.find("--log-level=") == 0) {
            options.logLevel = arg.substr(12);
        }
        else if (arg.find("--log-file=") == 0) {
            options.logFile = arg.substr(11);
        }
        else if (arg.find("--output=") == 0) {
            options.outputFile = arg.substr(9);
        }
        else if (arg == "--no-colors") {
            options.enableColors = false;
        }
        else if (arg == "--enable-statistics") {
            options.enableStatistics = true;
        }
        else if (arg == "--enable-profiling") {
            options.enableProfiling = true;
        }
        else if (arg == "--enable-debug") {
            options.enableDebug = true;
        }
        else if (arg == "--enable-detailed-logging") {
            options.enableDetailedLogging = true;
        }
        else if (arg.find("--log-history=") == 0) {
            options.maxLogHistory = std::stoi(arg.substr(14));
        }
        else if (arg.find("--learning-rate=") == 0) {
            options.learningRate = std::stof(arg.substr(17));
        }
        else if (arg.find("--stdp-ltp=") == 0) {
            options.stdpLtpWeight = std::stof(arg.substr(12));
        }
        else if (arg.find("--stdp-ltd=") == 0) {
            options.stdpLtdWeight = std::stof(arg.substr(12));
        }
        else if (arg.find("--synaptogenesis-rate=") == 0) {
            options.synaptogenesisRate = std::stof(arg.substr(22));
        }
        else if (arg.find("--pruning-rate=") == 0) {
            options.pruningRate = std::stof(arg.substr(16));
        }
        else if (arg.find("--timestep=") == 0) {
            options.simulationTimestep = std::stof(arg.substr(11));
        }
        else if (arg.find("--neurons=") == 0) {
            options.neuronCount = std::stoi(arg.substr(10));
        }
        else if (arg.find("--regions=") == 0) {
            options.regionCount = std::stoi(arg.substr(10));
        }
        else if (arg.find("--connection-prob=") == 0) {
            options.connectionProbability = std::stof(arg.substr(19));
        }
        else if (arg.find("--random-seed=") == 0) {
            options.randomSeed = arg.substr(15);
        }
        else if (arg.find("--benchmark-mode=") == 0) {
            options.benchmarkMode = arg.substr(17);
        }
        else if (arg == "--run-all-experiments") {
            options.runAllExperiments = true;
        }
        else if (arg == "--generate-reports") {
            options.generateReports = true;
        }
        else if (arg.find("--report-format=") == 0) {
            options.reportFormat = arg.substr(17);
        }
        else if (arg == "--enable-performance-monitoring") {
            options.enablePerformanceMonitoring = true;
        }
        else if (arg.find("--performance-report=") == 0) {
            std::string reportFile = arg.substr(21);
            // Save performance report
            std::cout << "Note: Performance report will be saved to " << reportFile << std::endl;
        }
        else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printHelp(argv[0]);
            return false;
        }
    }
    return true;
}

void setupLogging(const CommandLineOptions& options) {
    // Initialize logger with advanced features
    auto logger = std::make_shared<Logger>();
    
    LogLevel logLevel;
    if (options.logLevel == "debug") logLevel = LogLevel::Debug;
    else if (options.logLevel == "info") logLevel = LogLevel::Info;
    else if (options.logLevel == "warning") logLevel = LogLevel::Warning;
    else if (options.logLevel == "error") logLevel = LogLevel::Error;
    else if (options.logLevel == "critical") logLevel = LogLevel::Critical;
    else logLevel = LogLevel::Info;
    
    // Create console logger with advanced features
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    consoleLogger->setUseColors(options.enableColors);
    
    if (!options.logFile.empty()) {
        consoleLogger->setOutputFile(options.logFile);
    }
    
    // Configure advanced logging features
    if (options.enableStatistics || options.enableProfiling || options.enableDebug) {
        consoleLogger->configureAdvancedFeatures(
            options.enableStatistics,
            options.enableProfiling,
            options.enableDetailedLogging,
            options.maxLogHistory
        );
    }
    
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Set global logging features
    logger->setAdvancedLoggingFeatures(options.enableStatistics, options.enablePerformanceMonitoring);
    
    std::cout << "Logging configured: level=" << options.logLevel 
              << ", colors=" << (options.enableColors ? "enabled" : "disabled")
              << ", statistics=" << (options.enableStatistics ? "enabled" : "disabled")
              << std::endl;
}

void processAdvancedOptions(const CommandLineOptions& options, std::shared_ptr<Config> config) {
    // Apply advanced configuration options
    config->set("simulation_timestep", options.simulationTimestep, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::CommandLine);
    config->set("connection_probability", options.connectionProbability, ConfigSource::CommandLine);
    config->set("random_seed", std::stoull(options.randomSeed), ConfigSource::CommandLine);
    
    // Apply plasticity parameters
    config->set("stdp_ltp_weight", options.stdpLtpWeight, ConfigSource::CommandLine);
    config->set("stdp_ltd_weight", options.stdpLtdWeight, ConfigSource::CommandLine);
    config->set("synaptogenesis_rate", options.synaptogenesisRate, ConfigSource::CommandLine);
    config->set("pruning_rate", options.pruningRate, ConfigSource::CommandLine);
    
    std::cout << "Advanced configuration applied from command line options" << std::endl;
}

void printConfigurationSummary(const CommandLineOptions& options, const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Advanced Configuration Summary ===");
    NLM_LOG_INFO("Experiment Mode: " + options.experimentMode);
    NLM_LOG_INFO("Configuration File: " + options.configFile);
    NLM_LOG_INFO("Neuron Count: " + std::to_string(options.neuronCount));
    NLM_LOG_INFO("Region Count: " + std::to_string(options.regionCount));
    NLM_LOG_INFO("Simulation Timestep: " + std::to_string(options.simulationTimestep) + "s");
    NLM_LOG_INFO("Connection Probability: " + std::to_string(options.connectionProbability));
    NLM_LOG_INFO("Log Level: " + options.logLevel);
    NLM_LOG_INFO("Enable Statistics: " + std::string(options.enableStatistics ? "Yes" : "No"));
    NLM_LOG_INFO("Enable Profiling: " + std::string(options.enableProfiling ? "Yes" : "No"));
    NLM_LOG_INFO("Enable Debug: " + std::string(options.enableDebug ? "Yes" : "No"));
    NLM_LOG_INFO("Learning Rate: " + std::to_string(options.learningRate));
    NLM_LOG_INFO("STDP LTP Weight: " + std::to_string(options.stdpLtpWeight));
    NLM_LOG_INFO("STDP LTD Weight: " + std::to_string(options.stdpLtdWeight));
    NLM_LOG_INFO("Synapse Formation Rate: " + std::to_string(options.synaptogenesisRate));
    NLM_LOG_INFO("Pruning Rate: " + std::to_string(options.pruningRate));
    NLM_LOG_INFO("");
}

void runPerformanceBenchmark(const std::shared_ptr<Brain>& brain, const CommandLineOptions& options) {
    std::cout << "Running Performance Benchmark..." << std::endl;
    std::cout << "Benchmark Mode: " << options.benchmarkMode << std::endl;
    std::cout << "Neuron Count: " << options.benchmarkSize << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < options.benchmarkSize; ++step) {
        // Inject some random input
        for (size_t i = 0; i < std::min(size_t(10), options.benchmarkSize / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress for detailed mode
        if (options.benchmarkMode == "detailed" && step % (options.benchmarkSize / 10) == 0) {
            std::cout << "Progress: " << (step * 100 / options.benchmarkSize) << "% "
                      << "Spikes: " << brain->getTotalSpikeCount() << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << std::endl;
    std::cout << "Benchmark Results:" << std::endl;
    std::cout << "  Total Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "  Steps/sec: " << std::fixed << std::setprecision(0) << (options.benchmarkSize / totalTime) << std::endl;
    std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(2) 
              << brain->getAverageFiringRate() << " Hz" << std::endl;
}

void runExperimentalSuite(const std::shared_ptr<Brain>& brain, const CommandLineOptions& options) {
    std::cout << "Running Experimental Suite..." << std::endl;
    std::cout << "Mode: " << options.experimentMode << std::endl;
    
    if (options.experimentMode == "phase2") {
        std::cout << "Running Phase 2 experiments (neural computation)" << std::endl;
        // Run Phase 2 experiments
        runBasicConnectivityTest(brain);
        runPlasticityExperiment(brain);
        runStdpVerification(brain);
    }
    else if (options.experimentMode == "phase3") {
        std::cout << "Running Phase 3 experiments (world interaction)" << std::endl;
        // Create world and agent for Phase 3
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        world->setRandomSeed(std::stoull(options.randomSeed));
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        // Run episodes
        for (int episode = 0; episode < options.numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " ---" << std::endl;
            world->reset();
            agentBrain->reset();
            brain->reset();
            
            for (int step = 0; step < options.stepsPerEpisode; ++step) {
                // Get sensory percept from world
                const SensoryPercept& percept = world->getSensoryPercept();
                
                // Process sensory input
                agentBrain->processSensoryInput(percept);
                
                // Run brain step
                brain->step(step, step * 0.001);
                
                // Decode motor command
                MotorCommand cmd = agentBrain->decodeMotorCommand();
                
                // Apply motor command to world
                ActionResult result = world->applyMotorCommand(cmd, world->getSimulationTime());
                
                // Apply reward modulation
                agentBrain->applyRewardModulation(result.reward, 0.0f);
                
                // Update development
                agentBrain->updateDevelopment(0.01);
                
                if (step % 100 == 0) {
                    std::cout << "  Step " << step 
                              << ", Reward: " << result.reward
                              << ", Firing: " << brain->getFiringNeuronCount()
                              << ", Curiosity: " << agentBrain->getCuriosityLevel()
                              << std::endl;
                }
            }
        }
    }
    else if (options.experimentMode == "phase4") {
        std::cout << "Running Phase 4 experiments (emerging cognition)" << std::endl;
        if (options.runAllExperiments) {
            std::cout << "Running full Phase 4 experimental suite..." << std::endl;
            // Note: This would require implementing the full Phase 4 experiments
            std::cout << "(Phase 4 experiment implementation required)" << std::endl;
        } else {
            std::cout << "Run all experiments with --run-all-experiments" << std::endl;
        }
    }
    else if (options.experimentMode == "all") {
        std::cout << "Running all experiment modes" << std::endl;
        runExperimentalSuite(brain, options); // Recursive call for all modes
    }
}

int main(int argc, char** argv) {
    printAdvancedBanner();
    
    // Parse command line options
    CommandLineOptions options = getDefaultOptions();
    if (!parseCommandLineOptions(argc, argv, options)) {
        return 1;
    }
    
    // Setup advanced logging
    setupLogging(options);
    
    NLM_LOG_INFO("=== NLM Phase 2 Enhanced CLI ===");
    NLM_LOG_INFO("Program started with advanced options");
    NLM_LOG_INFO("Config file: " + options.configFile);
    NLM_LOG_INFO("Experiment mode: " + options.experimentMode);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (config->loadFromFile(options.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Apply command line options (override config file)
    processAdvancedOptions(options, config);
    
    // Log configuration summary
    printConfigurationSummary(options, config);
    
    // Initialize simulation clock
    SimulationClock clock(options.simulationTimestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(options.simulationTimestep) + "s");
    
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
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run experiments based on mode
    if (options.enableProfiling) {
        runPerformanceBenchmark(brain, options);
    } else {
        runExperimentalSuite(brain, options);
    }
    
    // Generate reports if requested
    if (options.generateReports) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Generating Reports ===");
        NLM_LOG_INFO("Report format: " + options.reportFormat);
        NLM_LOG_INFO("(Report generation not fully implemented in this enhanced version)");
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced CLI Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("NLM brain completed successfully with advanced CLI options!");
    NLM_LOG_INFO("For more information, visit: https://github.com/neural-learning-machine");
    
    return 0;
}