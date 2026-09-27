// NLM Command-Line Tool Implementation
// Advanced command-line interface for NLM (熙然) neural brain simulation

#include "tools/nlm_cmd.h"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <thread>

namespace nlm {
namespace cmd {

// Static helper functions
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

static std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Constructor
NLMCommandTool::NLMCommandTool(int argc, char** argv) {
    parseArguments(argc, argv);
}

// Parse command-line arguments
void NLMCommandTool::parseArguments(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                parseOption(key, value);
            }
        } else if (arg[0] == '-' && arg.size() > 1) {
            std::string key = arg.substr(1);
            // Handle flags (boolean options)
            if (key == "debug") options.enableDebug = true;
            else if (key == "quiet") options.quiet = true;
            else if (key == "verbose") options.verbose = true;
            else if (key == "no-plasticity") options.enablePlasticity = false;
            else if (key == "no-development") options.enableDevelopment = false;
            else if (key == "no-curiosity") options.enableCuriosity = false;
            else if (key == "no-reward") options.enableRewardModulation = false;
        }
    }
}

// Parse individual option
void NLMCommandTool::parseOption(const std::string& key, const std::string& value) {
    if (key == "config") {
        options.configFile = value;
    } else if (key == "output") {
        options.outputFile = value;
    } else if (key == "checkpoint") {
        options.checkpointFile = value;
    } else if (key == "log") {
        options.logFile = value;
    } else if (key == "experiment") {
        options.experimentType = value;
    } else if (key == "mode") {
        options.mode = value;
    } else if (key == "steps") {
        try {
            options.simulationSteps = std::stoi(value);
        } catch (...) {
            std::cerr << "Warning: Invalid steps value, using default" << std::endl;
        }
    } else if (key == "timestep") {
        try {
            options.timeStep = std::stod(value);
        } catch (...) {
            std::cerr << "Warning: Invalid timestep value, using default" << std::endl;
        }
    } else if (key == "neurons") {
        try {
            options.neuronCount = std::stoi(value);
        } catch (...) {
            std::cerr << "Warning: Invalid neuron count, using default" << std::endl;
        }
    } else if (key == "regions") {
        try {
            options.regionCount = std::stoi(value);
        } catch (...) {
            std::cerr << "Warning: Invalid region count, using default" << std::endl;
        }
    }
}

// Print help information
void NLMCommandTool::printHelp() const {
    std::cout << "NLM Command-Line Tool" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm_cmd [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  run      Run a simulation (default)" << std::endl;
    std::cout << "  monitor  Continuous monitoring session" << std::endl;
    std::cout << "  validate Run validation suite" << std::endl;
    std::cout << std::endl;
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version               Show version information" << std::endl;
    std::cout << "  --config <file>         Configuration file" << std::endl;
    std::cout << "  --experiment <type>     Experiment type (basic, learning, stdp)" << std::endl;
    std::cout << "  --steps <n>             Number of simulation steps" << std::endl;
    std::cout << "  --timestep <t>          Simulation time step" << std::endl;
    std::cout << "  --neurons <n>           Neuron count" << std::endl;
    std::cout << "  --regions <n>           Region count" << std::endl;
    std::cout << std::endl;
    std::cout << "Output Options:" << std::endl;
    std::cout << "  --output <file>         Output file for results" << std::endl;
    std::cout << "  --checkpoint <file>     Checkpoint file" << std::endl;
    std::cout << "  --log <file>            Log file" << std::endl;
    std::cout << "  --json                  Output results in JSON format" << std::endl;
    std::cout << "  --metrics-only          Output only metrics" << std::endl;
    std::cout << std::endl;
    std::cout << "Feature Flags:" << std::endl;
    std::cout << "  --debug                 Enable debug logging" << std::endl;
    std::cout << "  --quiet                 Quiet mode (minimal output)" << std::endl;
    std::cout << "  --no-plasticity         Disable plasticity" << std::endl;
    std::cout << "  --no-development        Disable development" << std::endl;
    std::cout << "  --no-curiosity          Disable curiosity" << std::endl;
    std::cout << "  --no-reward             Disable reward modulation" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_cmd --experiment learning --steps 5000" << std::endl;
    std::cout << "  nlm_cmd --mode monitor --interval 1.0" << std::endl;
    std::cout << "  nlm_cmd --experiment stdp --no-plasticity" << std::endl;
    std::cout << "  nlm_cmd --output results.json --json" << std::endl;
}

// Print version information
void NLMCommandTool::printVersion() const {
    std::cout << "NLM Command-Line Tool v0.1.0" << std::endl;
    std::cout << "Built for NLM (熙然) Phase 2: Real Neural Computation" << std::endl;
    std::cout << "C++20 Implementation with JSON Configuration Support" << std::endl;
}

// Load configuration
bool NLMCommandTool::loadConfiguration() {
    config = std::make_shared<Config>();
    
    if (!config->loadFromFile(options.configFile)) {
        std::cerr << "Warning: Could not load config file " << options.configFile << std::endl;
        std::cerr << "Using default configuration" << std::endl;
    }
    
    return true;
}

// Save configuration
bool NLMCommandTool::saveConfiguration() const {
    if (!options.outputFile.empty()) {
        return config->saveToFile(options.outputFile);
    }
    return false;
}

// Apply options to configuration
void NLMCommandTool::applyOptionsToConfig() {
    if (!config) return;
    
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::CommandLine);
    config->set("simulation_timestep", options.timeStep, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::CommandLine);
    config->set("connection_probability", 0.15f, ConfigSource::CommandLine);
    
    if (options.experimentType == "learning") {
        config->set("plasticity_enabled", options.enablePlasticity, ConfigSource::CommandLine);
        config->set("development_enabled", options.enableDevelopment, ConfigSource::CommandLine);
        config->set("curiosity_enabled", options.enableCuriosity, ConfigSource::CommandLine);
        config->set("reward_enabled", options.enableRewardModulation, ConfigSource::CommandLine);
    }
}

// Setup logging
void NLMCommandTool::setupLogging() const {
    logger = std::make_shared<Logger>();
    
    LogLevel logLevel = options.enableDebug ? LogLevel::Debug : 
                       options.verbose ? LogLevel::Info : LogLevel::Warning;
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    
    if (!options.logFile.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(options.logFile);
        logger->addLogger(fileLogger);
    }
    
    Logger::setGlobal(logger);
}

// Print banner
void NLMCommandTool::printBanner() const {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║     NLM Command-Line Tool v0.1.0                            ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║     Advanced NLM Simulation Control                            ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

// Print configuration
void NLMCommandTool::printConfiguration() const {
    if (!options.quiet) {
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Config file: " << options.configFile << std::endl;
        std::cout << "  Experiment: " << options.experimentType << std::endl;
        std::cout << "  Mode: " << options.mode << std::endl;
        std::cout << "  Steps: " << options.simulationSteps << std::endl;
        std::cout << "  Time step: " << options.timeStep << "s" << std::endl;
        std::cout << "  Neurons: " << options.neuronCount << std::endl;
        std::cout << "  Regions: " << options.regionCount << std::endl;
        std::cout << "  Plasticity: " << (options.enablePlasticity ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Development: " << (options.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Curiosity: " << (options.enableCuriosity ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Reward modulation: " << (options.enableRewardModulation ? "Enabled" : "Disabled") << std::endl;
    }
}

// Generate metrics report
void NLMCommandTool::generateMetricsReport() const {
    if (brain && !options.quiet) {
        std::cout << std::endl;
        std::cout << "=== Simulation Metrics ===" << std::endl;
        std::cout << "Timestamp: " << getTimestamp() << std::endl;
        std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        if (options.jsonOutput) {
            std::cout << "Formatted metrics for JSON export:" << std::endl;
            std::cout << "{" << std::endl;
            std::cout << "  \"timestamp\": \"" << getTimestamp() << "\"," << std::endl;
            std::cout << "  \"total_neurons\": " << brain->getTotalNeuronCount() << "," << std::endl;
            std::cout << "  \"total_synapses\": " << brain->getTotalSynapseCount() << "," << std::endl;
            std::cout << "  \"active_neurons\": " << brain->getActiveNeuronCount() << "," << std::endl;
            std::cout << "  \"firing_neurons\": " << brain->getFiringNeuronCount() << "," << std::endl;
            std::cout << "  \"total_spikes\": " << brain->getTotalSpikeCount() << "," << std::endl;
            std::cout << "  \"average_firing_rate\": " << brain->getAverageFiringRate() << "," << std::endl;
            std::cout << "  \"excitation_inhibition_ratio\": " << brain->getExcitationInhibitionRatio() << std::endl;
            std::cout << "}" << std::endl;
        }
    }
}

// Initialize brain
bool NLMCommandTool::initializeBrain() {
    setupLogging();
    printBanner();
    printConfiguration();
    
    brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Error: Brain initialization failed" << std::endl;
        return false;
    }
    
    if (!options.quiet) {
        std::cout << std::endl;
        std::cout << "Brain initialized successfully:" << std::endl;
        brain->logStatus();
    }
    
    return true;
}

// Run experiment
void NLMCommandTool::runExperiment() {
    std::cout << std::endl;
    std::cout << "=== Running " << options.experimentType << " Experiment ===" << std::endl;
    
    if (options.experimentType == "basic") {
        std::cout << "Running basic connectivity test..." << std::endl;
        // Implement basic experiment logic
        runBasicConnectivityTest(brain);
    } else if (options.experimentType == "learning") {
        std::cout << "Running plasticity learning experiment..." << std::endl;
        runPlasticityExperiment(brain);
    } else if (options.experimentType == "stdp") {
        std::cout << "Running STDP verification..." << std::endl;
        runStdpVerification(brain);
    } else {
        std::cout << "Running default experiment..." << std::endl;
        // Run all tests
        brain->reset();
        brain->initialize();
        runBasicConnectivityTest(brain);
        
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    }
}

// Run monitoring session
void NLMCommandTool::runMonitoringSession() {
    std::cout << std::endl;
    std::cout << "=== Starting Monitoring Session ===" << std::endl;
    
    auto startTime = std::chrono::steady_clock::now();
    
    for (int step = 0; step < options.simulationSteps; ++step) {
        brain->step(step, step * options.timeStep);
        
        if (step % 100 == 0 || options.verbose) {
            std::cout << "Step " << step << ": "
                     << brain->getFiringNeuronCount() << " firing, "
                     << brain->getTotalSpikeCount() << " total spikes" << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << std::endl;
    std::cout << "Monitoring session completed in " << duration.count() << " seconds" << std::endl;
}

// Run validation suite
void NLMCommandTool::runValidationSuite() {
    std::cout << std::endl;
    std::cout << "=== Running Validation Suite ===" << std::endl;
    
    // Test basic brain functionality
    if (brain->initialize()) {
        std::cout << "✓ Brain initialization test passed" << std::endl;
    } else {
        std::cout << "✗ Brain initialization test failed" << std::endl;
        return;
    }
    
    // Test neuron functionality
    brain->reset();
    if (brain->getTotalNeuronCount() > 0) {
        std::cout << "✓ Neuron count test passed" << std::endl;
    } else {
        std::cout << "✗ Neuron count test failed" << std::endl;
    }
    
    // Test synapse functionality
    if (brain->getTotalSynapseCount() >= 0) {
        std::cout << "✓ Synapse count test passed" << std::endl;
    } else {
        std::cout << "✗ Synapse count test failed" << std::endl;
    }
    
    std::cout << "Validation suite completed" << std::endl;
}

// Generate debug output
void NLMCommandTool::generateDebugOutput() const {
    if (brain) {
        std::cout << std::endl;
        std::cout << "=== Debug Output ===" << std::endl;
        
        // Log brain statistics
        brain->logStatus();
        
        // Generate debug configuration dump
        std::cout << std::endl;
        std::cout << "Debug Configuration Dump:" << std::endl;
        std::cout << config->summary() << std::endl;
    }
}

// Write checkpoint
void NLMCommandTool::writeCheckpoint() const {
    if (brain && !options.checkpointFile.empty()) {
        std::cout << std::endl;
        std::cout << "Writing checkpoint to: " << options.checkpointFile << std::endl;
        
        if (brain->save(options.checkpointFile)) {
            std::cout << "✓ Checkpoint saved successfully" << std::endl;
        } else {
            std::cerr << "✗ Failed to save checkpoint" << std::endl;
        }
    }
}

// Load checkpoint
bool NLMCommandTool::loadCheckpoint() {
    if (!options.checkpointFile.empty()) {
        std::cout << "Loading checkpoint from: " << options.checkpointFile << std::endl;
        
        if (brain->load(options.checkpointFile)) {
            std::cout << "✓ Checkpoint loaded successfully" << std::endl;
            return true;
        } else {
            std::cerr << "✗ Failed to load checkpoint" << std::endl;
            return false;
        }
    }
    return false;
}

// Cleanup brain
void NLMCommandTool::cleanupBrain() {
    if (brain) {
        brain.reset();
    }
    if (logger) {
        logger.reset();
    }
    if (config) {
        config.reset();
    }
}

} // namespace cmd
} // namespace nlm
