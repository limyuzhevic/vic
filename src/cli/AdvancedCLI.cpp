// Advanced CLI implementation
// 
// This file implements the AdvancedCLI class with full command-line parsing,
// configuration management, and output generation capabilities.

#include "cli/AdvancedCLI.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <filesystem>

namespace nlm {

void AdvancedCLI::initializeOptions() {
    // Initialize with default values
    setupOptionHandlers();
    
    // Set default values for all options
    options_.brainType = "phase6";
    options_.logLevel = "INFO";
    options_.reportFormat = "text";
    options_.visualizationType = "minimal";
    
    // Parse environment variables
    parseFromEnvironment();
}

void AdvancedCLI::parseArguments() {
    // This would typically be called from main() with argc, argv
    // For now, we'll simulate parsing from a command line
    std::vector<std::string> args = {"nlm"}; // argv[0]
    
    for (size_t i = 1; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg == "-h" || arg == "--help") {
            options_.helpRequested = true;
        } else if (arg == "-v" || arg == "--version") {
            options_.versionRequested = true;
        } else if (arg == "-c" || arg == "--config") {
            if (i + 1 < args.size()) {
                options_.configFile = args[++i];
            }
        } else if (arg == "--output") {
            if (i + 1 < args.size()) {
                options_.outputFile = args[++i];
            }
        } else if (arg == "--log-level") {
            if (i + 1 < args.size()) {
                options_.logLevel = args[++i];
            }
        } else if (arg == "--neuron-count") {
            if (i + 1 < args.size()) {
                options_.neuronCount = std::stoul(args[++i]);
            }
        } else if (arg == "--timestep") {
            if (i + 1 < args.size()) {
                options_.timestep = std::stod(args[++i]);
            }
        } else if (arg == "--seed") {
            if (i + 1 < args.size()) {
                options_.randomSeed = std::stoull(args[++i]);
            }
        } else if (arg == "--brain-type") {
            if (i + 1 < args.size()) {
                options_.brainType = args[++i];
            }
        } else if (arg == "--experiment") {
            if (i + 1 < args.size()) {
                std::string experiment = args[++i];
                if (experiment == "basic") {
                    options_.runBasicTests = true;
                    options_.runPlasticityTests = false;
                    options_.runStdpVerification = false;
                } else if (experiment == "plasticity") {
                    options_.runBasicTests = false;
                    options_.runPlasticityTests = true;
                    options_.runStdpVerification = false;
                } else if (experiment == "stdp") {
                    options_.runBasicTests = false;
                    options_.runPlasticityTests = false;
                    options_.runStdpVerification = true;
                } else if (experiment == "integration") {
                    options_.runIntegrationTest = true;
                    options_.runBasicTests = false;
                    options_.runPlasticityTests = false;
                    options_.runStdpVerification = false;
                } else if (experiment == "full") {
                    options_.runBasicTests = true;
                    options_.runPlasticityTests = true;
                    options_.runStdpVerification = true;
                    options_.runIntegrationTest = true;
                }
            }
        } else if (arg == "--enable-stdp") {
            options_.enableSTDP = true;
        } else if (arg == "--disable-stdp") {
            options_.enableSTDP = false;
        } else if (arg == "--enable-hebbian") {
            options_.enableHebbian = true;
        } else if (arg == "--disable-hebbian") {
            options_.enableHebbian = false;
        } else if (arg == "--enable-curiosity") {
            options_.enableCuriosity = true;
        } else if (arg == "--disable-curiosity") {
            options_.enableCuriosity = false;
        } else if (arg == "--enable-checkpointing") {
            options_.enableCheckpointing = true;
        } else if (arg == "--disable-checkpointing") {
            options_.enableCheckpointing = false;
        } else if (arg == "--verbose") {
            options_.verboseOutput = true;
            options_.logLevel = "DEBUG";
        } else if (arg == "--quiet") {
            options_.quietMode = true;
            options_.logLevel = "ERROR";
        } else if (arg == "--report-format") {
            if (i + 1 < args.size()) {
                options_.reportFormat = args[++i];
            }
        } else if (arg == "--help-all") {
            // Show all available options
        } else {
            // Custom parameter
            size_t equalsPos = arg.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = arg.substr(0, equalsPos);
                std::string value = arg.substr(equalsPos + 1);
                options_.customParams[key] = value;
            } else {
                // Unknown argument
            }
        }
    }
}

void AdvancedCLI::parseFromEnvironment() {
    // Parse environment variables
    const char* envBrainType = std::getenv("NLM_BRAIN_TYPE");
    if (envBrainType) options_.brainType = envBrainType;
    
    const char* envNeuronCount = std::getenv("NLM_NEURON_COUNT");
    if (envNeuronCount) options_.neuronCount = std::stoul(envNeuronCount);
    
    const char* envTimestep = std::getenv("NLM_TIMESTEP");
    if (envTimestep) options_.timestep = std::stod(envTimestep);
    
    const char* envSeed = std::getenv("NLM_SEED");
    if (envSeed) options_.randomSeed = std::stoull(envSeed);
    
    const char* envLogLevel = std::getenv("NLM_LOG_LEVEL");
    if (envLogLevel) options_.logLevel = envLogLevel;
    
    const char* envQuiet = std::getenv("NLM_QUIET");
    if (envQuiet && std::string(envQuiet) == "1") {
        options_.quietMode = true;
        options_.logLevel = "ERROR";
    }
}

void AdvancedCLI::setupOptionHandlers() {
    commandHandlers_ = {
        {"help", std::bind(&AdvancedCLI::handleHelpCommand, this, std::placeholders::_1)},
        {"version", std::bind(&AdvancedCLI::handleVersionCommand, this, std::placeholders::_1)},
        {"config", std::bind(&AdvancedCLI::handleConfigCommand, this, std::placeholders::_1)},
        {"experiment", std::bind(&AdvancedCLI::handleExperimentCommand, this, std::placeholders::_1)},
        {"output", std::bind(&AdvancedCLI::handleOutputCommand, this, std::placeholders::_1)},
        {"log", std::bind(&AdvancedCLI::handleLogCommand, this, std::placeholders::_1)},
        {"memory", std::bind(&AdvancedCLI::handleMemoryCommand, this, std::placeholders::_1)},
        {"neuromod", std::bind(&AdvancedCLI::handleNeuromodCommand, this, std::placeholders::_1)},
        {"checkpoint", std::bind(&AdvancedCLI::handleCheckpointCommand, this, std::placeholders::_1)},
        {"visualization", std::bind(&AdvancedCLI::handleVisualizationCommand, this, std::placeholders::_1)},
    };
    
    optionSetters_ = {
        {"neuron_count", [this](const std::string& value) { options_.neuronCount = std::stoul(value); }},
        {"timestep", [this](const std::string& value) { options_.timestep = std::stod(value); }},
        {"seed", [this](const std::string& value) { options_.randomSeed = std::stoull(value); }},
        {"log_level", [this](const std::string& value) { options_.logLevel = value; }},
        {"brain_type", [this](const std::string& value) { options_.brainType = value; }},
        {"output_file", [this](const std::string& value) { options_.outputFile = value; }},
        {"log_file", [this](const std::string& value) { options_.logFile = value; }},
        {"checkpoint_file", [this](const std::string& value) { options_.checkpointFile = value; }},
        {"enable_stdp", [this](const std::string& value) { options_.enableSTDP = true; }},
        {"disable_stdp", [this](const std::string& value) { options_.enableSTDP = false; }},
        {"enable_hebbian", [this](const std::string& value) { options_.enableHebbian = true; }},
        {"disable_hebbian", [this](const std::string& value) { options_.enableHebbian = false; }},
    };
}

bool AdvancedCLI::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        NLM_LOG_ERROR("Cannot open config file: " + filename);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        size_t equalsPos = line.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = trim(line.substr(0, equalsPos));
            std::string value = trim(line.substr(equalsPos + 1));
            
            if (optionSetters_.find(key) != optionSetters_.end()) {
                optionSetters_[key](value);
            } else {
                options_.customParams[key] = value;
            }
        }
    }
    
    return true;
}

bool AdvancedCLI::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        NLM_LOG_ERROR("Cannot write config file: " + filename);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by Advanced CLI\n\n";
    
    file << "# Basic Options\n";
    file << "neuron_count=" << options_.neuronCount << "\n";
    file << "timestep=" << options_.timestep << "\n";
    file << "random_seed=" << options_.randomSeed << "\n";
    file << "log_level=" << options_.logLevel << "\n";
    file << "quiet_mode=" << (options_.quietMode ? "1" : "0") << "\n";
    file << "verbose_output=" << (options_.verboseOutput ? "1" : "0") << "\n\n";
    
    file << "# Brain Configuration\n";
    file << "brain_type=" << options_.brainType << "\n";
    file << "enable_stdp=" << (options_.enableSTDP ? "1" : "0") << "\n";
    file << "enable_hebbian=" << (options_.enableHebbian ? "1" : "0") << "\n";
    file << "enable_structural_plasticity=" << (options_.enableStructuralPlasticity ? "1" : "0") << "\n";
    file << "enable_development=" << (options_.enableDevelopment ? "1" : "0") << "\n";
    file << "enable_curiosity=" << (options_.enableCuriosity ? "1" : "0") << "\n";
    file << "enable_novelty=" << (options_.enableNovelty ? "1" : "0") << "\n\n";
    
    file << "# Neuromodulation\n";
    file << "enable_dopamine=" << (options_.enableDopamine ? "1" : "0") << "\n";
    file << "enable_prediction_error=" << (options_.enablePredictionError ? "1" : "0") << "\n\n";
    
    file << "# Memory Systems\n";
    file << "enable_working_memory=" << (options_.enableWorkingMemory ? "1" : "0") << "\n";
    file << "enable_episodic_memory=" << (options_.enableEpisodicMemory ? "1" : "0") << "\n";
    file << "max_working_memory_capacity=" << options_.maxWorkingMemoryCapacity << "\n";
    file << "max_episodic_memory_episodes=" << options_.maxEpisodicMemoryEpisodes << "\n\n";
    
    file << "# Experiments\n";
    file << "run_basic_tests=" << (options_.runBasicTests ? "1" : "0") << "\n";
    file << "run_plasticity_tests=" << (options_.runPlasticityTests ? "1" : "0") << "\n";
    file << "run_stdp_verification=" << (options_.runStdpVerification ? "1" : "0") << "\n";
    file << "run_integration_test=" << (options_.runIntegrationTest ? "1" : "0") << "\n";
    file << "test_steps=" << options_.testSteps << "\n\n";
    
    file << "# Output\n";
    file << "output_file=" << options_.outputFile << "\n";
    file << "log_file=" << options_.logFile << "\n";
    file << "report_format=" << options_.reportFormat << "\n";
    file << "generate_report=" << (options_.generateReport ? "1" : "0") << "\n\n";
    
    file << "# Custom Parameters\n";
    for (const auto& param : options_.customParams) {
        file << param.first << "=" << param.second << "\n";
    }
    
    return true;
}

bool AdvancedCLI::processCommand(const std::string& command) {
    if (commandHandlers_.find(command) != commandHandlers_.end()) {
        std::vector<std::string> args;
        return commandHandlers_[command](args);
    }
    return false;
}

bool AdvancedCLI::executeCommand(const std::string& command, 
                                const std::vector<std::string>& args) {
    if (commandHandlers_.find(command) != commandHandlers_.end()) {
        return commandHandlers_[command](args);
    }
    return false;
}

void AdvancedCLI::showHelp() const {
    std::cout << "=== NLM - Advanced Command Line Interface ===\n\n";
    std::cout << "Usage: nlm [OPTIONS]\n\n";
    
    std::cout << "=== Basic Options ===\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -v, --version           Show version information\n";
    std::cout << "  -c, --config FILE       Load configuration from file\n";
    std::cout << "  --output FILE           Specify output file\n";
    std::cout << "  --log-level LEVEL       Set log level (DEBUG, INFO, WARN, ERROR)\n";
    std::cout << "  --help-all              Show all available options\n\n";
    
    std::cout << "=== Simulation Options ===\n";
    std::cout << "  --neuron-count N        Number of neurons (default: 500)\n";
    std::cout << "  --timestep T            Simulation timestep in seconds (default: 0.001)\n";
    std::cout << "  --seed S                Random seed (default: 42)\n";
    std::cout << "  --brain-type TYPE       Brain phase (phase2, phase3, phase4, phase5, phase6)\n";
    std::cout << "  --experiment EXP        Run specific experiment (basic, plasticity, stdp, integration, full)\n\n";
    
    std::cout << "=== Brain Configuration ===\n";
    std::cout << "  --enable-stdp           Enable STDP plasticity (default)\n";
    std::cout << "  --disable-stdp          Disable STDP plasticity\n";
    std::cout << "  --enable-hebbian        Enable Hebbian plasticity (default)\n";
    std::cout << "  --disable-hebbian       Disable Hebbian plasticity\n";
    std::cout << "  --enable-curiosity      Enable curiosity-driven exploration (default)\n";
    std::cout << "  --disable-curiosity     Disable curiosity-driven exploration\n\n";
    
    std::cout << "=== Memory Systems ===\n";
    std::cout << "  --enable-working-memory Enable working memory (default)\n";
    std::cout << "  --disable-working-memory Disable working memory\n";
    std::cout << "  --enable-episodic-memory Enable episodic memory (default)\n";
    std::cout << "  --disable-episodic-memory Disable episodic memory\n\n";
    
    std::cout << "=== Neuromodulation ===\n";
    std::cout << "  --enable-dopamine       Enable dopamine system (default)\n";
    std::cout << "  --disable-dopamine      Disable dopamine system\n";
    std::cout << "  --enable-prediction-error Enable prediction error (default)\n";
    std::cout << "  --disable-prediction-error Disable prediction error\n\n";
    
    std::cout << "=== Output and Logging ===\n";
    std::cout << "  --verbose               Enable verbose output (DEBUG logging)\n";
    std::cout << "  --quiet                 Quiet mode (ERROR logging only)\n";
    std::cout << "  --report-format FMT     Report format: text, json, csv\n";
    std::cout << "  --log-file FILE         Log output to file\n";
    std::cout << "  --no-report             Disable report generation\n\n";
    
    std::cout << "=== Performance ===\n";
    std::cout << "  --enable-checkpointing   Enable checkpointing (default)\n";
    std::cout << "  --disable-checkpointing  Disable checkpointing\n";
    std::cout << "  --no-progress           Disable progress display\n\n";
    
    std::cout << "=== Examples ===\n";
    std::cout << "  nlm --neuron-count 1000 --timestep 0.0005 --experiment basic\n";
    std::cout << "  nlm --brain-type phase6 --enable-stdp --enable-hebbian --verbose\n";
    std::cout << "  nlm --experiment integration --output results.json --log-level INFO\n\n";
}

void AdvancedCLI::showVersion() const {
    std::cout << "NLM v0.1.0\n";
    std::cout << "Phase 6: Final Integration\n";
    std::cout << "Advanced Artificial Brain Simulator\n\n";
    std::cout << "Built with C++20 and pybind11\n";
    std::cout << "Features: Real LIF neurons, STDP, Hebbian plasticity, memory systems,\n";
    std::cout << "         neuromodulation, prediction, cognition, development.\n\n";
}

void AdvancedCLI::showExperimentInfo(const std::string& experiment) const {
    std::cout << "=== " << experiment << " Experiment ===\n\n";
    
    if (experiment == "basic") {
        std::cout << "Tests basic neural connectivity and spike propagation.\n";
        std::cout << "Injects current into sensory neurons and verifies spike transmission.\n\n";
        std::cout << "Key metrics:\n";
        std::cout << "  - Spike generation and propagation\n";
        std::cout << "  - Neural excitability\n";
        std::cout << "  - Initial synaptic weights\n";
    } else if (experiment == "plasticity") {
        std::cout << "Tests plasticity learning mechanisms.\n";
        std::cout << "Enables STDP and Hebbian plasticity and measures weight changes.\n\n";
        std::cout << "Key metrics:\n";
        std::cout << "  - Synaptic weight changes\n";
        std::cout << "  - Learning rate effectiveness\n";
        std::cout << "  - Activity-dependent plasticity\n";
    } else if (experiment == "stdp") {
        std::cout << "Specifically tests STDP (Spike-Timing-Dependent Plasticity).\n";
        std::cout << "Creates pre-post spike timing correlations to verify LTP/LTD.\n\n";
        std::cout << "Key metrics:\n";
        std::cout << "  - Pre-before-post potentiation (LTP)\n";
        std::cout << "  - Post-before-pre depression (LTD)\n";
        std::cout << "  - STDP time window effects\n";
    } else if (experiment == "integration") {
        std::cout << "Runs full Phase 6 integration test.\n";
        std::cout << "Tests all brain systems working together coherently.\n\n";
        std::cout << "Key metrics:\n";
        std::cout << "  - Memory system integration\n";
        std::cout << "  - Neuromodulation coordination\n";
        std::cout << "  - Prediction system accuracy\n";
        std::cout << "  - Development effects\n";
        std::cout << "  - Checkpoint functionality\n";
    } else if (experiment == "full") {
        std::cout << "Runs complete simulation with all features enabled.\n";
        std::cout << "Full experiment with curiosity, development, and replay.\n\n";
        std::cout << "Key metrics:\n";
        std::cout << "  - Total reward accumulation\n";
        std::cout << "  - Average firing rate\n";
        std::cout << "  - Memory episodes stored\n";
        std::cout << "  - Neuromodulation levels\n";
        std::cout << "  - Development stage progression\n";
    }
}

void AdvancedCLI::showConfigurationSummary() const {
    std::cout << "=== NLM Configuration Summary ===\n\n";
    
    std::cout << "Brain Configuration:\n";
    std::cout << "  Type: " << options_.brainType << "\n";
    std::cout << "  Neurons: " << options_.neuronCount << "\n";
    std::cout << "  Regions: " << options_.regionCount << "\n";
    std::cout << "  Connection probability: " << options_.connectionProbability << "\n\n";
    
    std::cout << "Plasticity:\n";
    std::cout << "  STDP: " << (options_.enableSTDP ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Hebbian: " << (options_.enableHebbian ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Structural: " << (options_.enableStructuralPlasticity ? "Enabled" : "Disabled") << "\n\n";
    
    std::cout << "Neuromodulation:\n";
    std::cout << "  Dopamine: " << (options_.enableDopamine ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Prediction Error: " << (options_.enablePredictionError ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Curiosity: " << (options_.enableCuriosity ? "Enabled" : "Disabled") << "\n\n";
    
    std::cout << "Memory Systems:\n";
    std::cout << "  Working Memory: " << (options_.enableWorkingMemory ? "Enabled" : "Disabled") << "\n";
    stdcout << "  Episodic Memory: " << (options_.enableEpisodicMemory ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Maximum Working Memory Capacity: " << options_.maxWorkingMemoryCapacity << "\n";
    std::cout << "  Maximum Episodic Memory Episodes: " << options_.maxEpisodicMemoryEpisodes << "\n\n";
    
    std::cout << "Experiments:\n";
    std::cout << "  Basic Tests: " << (options_.runBasicTests ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Plasticity Tests: " << (options_.runPlasticityTests ? "Enabled" : "Disabled") << "\n";
    std::cout << "  STDP Verification: " << (options_.runStdpVerification ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Integration Test: " << (options_.runIntegrationTest ? "Enabled" : "Disabled") << "\n\n";
    
    std::cout << "Output:\n";
    std::cout << "  Report Format: " << options_.reportFormat << "\n";
    std::cout << "  Log Level: " << options_.logLevel << "\n";
    std::cout << "  Verbose: " << (options_.verboseOutput ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Quiet Mode: " << (options_.quietMode ? "Enabled" : "Disabled") << "\n\n";
}

void AdvancedCLI::setOption(const std::string& option, const std::string& value) {
    std::string key = toLower(option);
    
    if (optionSetters_.find(key) != optionSetters_.end()) {
        optionSetters_[key](value);
    } else {
        options_.customParams[key] = value;
    }
}

std::string AdvancedCLI::getOption(const std::string& option) const {
    std::string key = toLower(option);
    
    if (key == "neuron_count") {
        return std::to_string(options_.neuronCount);
    } else if (key == "timestep") {
        return std::to_string(options_.timestep);
    } else if (key == "random_seed") {
        return std::to_string(options_.randomSeed);
    } else if (key == "log_level") {
        return options_.logLevel;
    } else if (key == "brain_type") {
        return options_.brainType;
    } else if (key == "output_file") {
        return options_.outputFile;
    } else if (key == "log_file") {
        return options_.logFile;
    } else if (key == "checkpoint_file") {
        return options_.checkpointFile;
    } else if (options_.customParams.find(key) != options_.customParams.end()) {
        return options_.customParams.at(key);
    }
    
    return "";
}

bool AdvancedCLI::hasOption(const std::string& option) const {
    std::string key = toLower(option);
    
    if (key == "neuron_count" || key == "timestep" || key == "random_seed" ||
        key == "log_level" || key == "brain_type" || key == "output_file" ||
        key == "log_file" || key == "checkpoint_file") {
        return true;
    }
    
    return options_.customParams.find(key) != options_.customParams.end();
}

bool AdvancedCLI::validateOptions() const {
    validationErrors_.clear();
    
    // Validate brain type
    if (!validateBrainType(options_.brainType)) {
        validationErrors_.push_back("Invalid brain type: " + options_.brainType);
    }
    
    // Validate log level
    if (!validateLogLevel(options_.logLevel)) {
        validationErrors_.push_back("Invalid log level: " + options_.logLevel);
    }
    
    // Validate report format
    if (!validateReportFormat(options_.reportFormat)) {
        validationErrors_.push_back("Invalid report format: " + options_.reportFormat);
    }
    
    // Validate timestep
    if (options_.timestep <= 0.0) {
        validationErrors_.push_back("Timestep must be positive: " + std::to_string(options_.timestep));
    }
    
    // Validate neuron count
    if (options_.neuronCount == 0 || options_.neuronCount > 1000000) {
        validationErrors_.push_back("Invalid neuron count: " + std::to_string(options_.neuronCount));
    }
    
    // Validate random seed
    if (options_.randomSeed > std::numeric_limits<uint64_t>::max() / 2) {
        validationErrors_.push_back("Random seed too large: " + std::to_string(options_.randomSeed));
    }
    
    return validationErrors_.empty();
}

std::vector<std::string> AdvancedCLI::getValidationErrors() const {
    return validationErrors_;
}

void AdvancedCLI::logStatistics(const std::chrono::high_resolution_clock::duration& duration) const {
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
    
    std::cout << "=== Performance Statistics ===\n";
    std::cout << "Total simulation time: " << seconds << "." << std::setfill('0') << std::setw(3) << milliseconds << " seconds\n";
    
    if (seconds > 0) {
        std::cout << "Simulation rate: " << options_.testSteps / seconds << " steps/second\n";
    }
}

std::string AdvancedCLI::generateReport(const std::string& format) const {
    std::stringstream ss;
    
    if (format == "json") {
        ss << "{\n";
        ss << "  \"brain_type\": \"" << options_.brainType << "\",\n";
        ss << "  \"neuron_count\": " << options_.neuronCount << ",\n";
        ss << "  \"timestep\": " << options_.timestep << ",\n";
        ss << "  \"random_seed\": " << options_.randomSeed << ",\n";
        ss << "  \"enable_stdp\": " << (options_.enableSTDP ? "true" : "false") << ",\n";
        ss << "  \"enable_hebbian\": " << (options_.enableHebbian ? "true" : "false") << ",\n";
        ss << "  \"enable_curiosity\": " << (options_.enableCuriosity ? "true" : "false") << ",\n";
        ss << "}\n";
    } else if (format == "csv") {
        ss << "brain_type,neuron_count,timestep,random_seed,enable_stdp,enable_hebbian,enable_curiosity\n";
        ss << options_.brainType << "," << options_.neuronCount << "," 
           << options_.timestep << "," << options_.randomSeed << "," 
           << (options_.enableSTDP ? "true" : "false") << "," 
           << (options_.enableHebbian ? "true" : "false") << "," 
           << (options_.enableCuriosity ? "true" : "false") << "\n";
    } else {
        // Default text format
        showConfigurationSummary();
        ss << "Report generated successfully.\n";
    }
    
    return ss.str();
}

std::string AdvancedCLI::getExperimentCommand(const std::string& experiment) const {
    std::stringstream ss;
    ss << "nlm --experiment " << experiment;
    
    if (options_.neuronCount != 500) {
        ss << " --neuron-count " << options_.neuronCount;
    }
    
    if (options_.timestep != 0.001) {
        ss << " --timestep " << options_.timestep;
    }
    
    if (options_.enableSTDP) {
        ss << " --enable-stdp";
    }
    
    if (options_.enableHebbian) {
        ss << " --enable-hebbian";
    }
    
    return ss.str();
}

std::string AdvancedCLI::getBrainCommand(const std::string& brainType) const {
    std::stringstream ss;
    ss << "nlm --brain-type " << brainType;
    
    if (options_.neuronCount != 500) {
        ss << " --neuron-count " << options_.neuronCount;
    }
    
    return ss.str();
}

// Command handlers implementation

bool AdvancedCLI::handleHelpCommand(const std::vector<std::string>& args) {
    showHelp();
    return true;
}

bool AdvancedCLI::handleVersionCommand(const std::vector<std::string>& args) {
    showVersion();
    return true;
}

bool AdvancedCLI::handleConfigCommand(const std::vector<std::string>& args) {
    if (args.size() > 0) {
        return loadFromFile(args[0]);
    } else {
        std::cout << "Usage: --config <filename>\n";
        return false;
    }
}

bool AdvancedCLI::handleExperimentCommand(const std::vector<std::string>& args) {
    if (args.size() > 0) {
        showExperimentInfo(args[0]);
        return true;
    } else {
        std::cout << "Usage: --experiment <basic|plasticity|stdp|integration|full>\n";
        return false;
    }
}

bool AdvancedCLI::handleOutputCommand(const std::vector<std::string>& args) {
    std::cout << "Output options:\n";
    std::cout << "  --output-file <file>      Specify output file\n";
    std::cout << "  --report-format <fmt>      Specify report format\n";
    std::cout << "  --no-report                Disable report generation\n";
    return true;
}

bool AdvancedCLI::handleLogCommand(const std::vector<std::string>& args) {
    std::cout << "Logging options:\n";
    std::cout << "  --log-level <level>        Set log level\n";
    std::cout << "  --verbose                  Enable verbose output\n";
    std::cout << "  --quiet                    Quiet mode\n";
    std::cout << "  --log-file <file>          Log to file\n";
    return true;
}

bool AdvancedCLI::handleMemoryCommand(const std::vector<std::string>& args) {
    std::cout << "Memory system options:\n";
    std::cout << "  --enable-working-memory    Enable working memory\n";
    std::cout << "  --disable-working-memory   Disable working memory\n";
    std::cout << "  --enable-episodic-memory   Enable episodic memory\n";
    std::cout << "  --disable-episodic-memory  Disable episodic memory\n";
    std::cout << "  --max-working-memory-capacity <N> Set working memory capacity\n";
    std::cout << "  --max-episodic-memory-episodes <N> Set episodic memory capacity\n";
    return true;
}

bool AdvancedCLI::handleNeuromodCommand(const std::vector<std::string>& args) {
    std::cout << "Neuromodulation options:\n";
    std::cout << "  --enable-dopamine          Enable dopamine system\n";
    std::cout << "  --disable-dopamine         Disable dopamine system\n";
    std::cout << "  --enable-prediction-error  Enable prediction error\n";
    std::cout << "  --disable-prediction-error Disable prediction error\n";
    std::cout << "  --enable-curiosity         Enable curiosity\n";
    std::cout << "  --disable-curiosity        Disable curiosity\n";
    std::cout << "  --enable-novelty           Enable novelty detection\n";
    std::cout << "  --disable-novelty          Disable novelty detection\n";
    return true;
}

bool AdvancedCLI::handleCheckpointCommand(const std::vector<std::string>& args) {
    std::cout << "Checkpoint options:\n";
    std::cout << "  --enable-checkpointing      Enable checkpointing\n";
    std::cout << "  --disable-checkpointing     Disable checkpointing\n";
    std::cout << "  --checkpoint-interval <N>  Set checkpoint interval\n";
    std::cout << "  --max-checkpoints <N>       Set maximum checkpoints\n";
    return true;
}

bool AdvancedCLI::handleVisualizationCommand(const std::vector<std::string>& args) {
    std::cout << "Visualization options:\n";
    std::cout << "  --enable-visualization      Enable visualization\n";
    std::cout << "  --disable-visualization     Disable visualization\n";
    std::cout << "  --visualization-type <type> Set visualization type\n";
    std::cout << "  --save-visualizations       Save visualizations to files\n";
    return true;
}

// String utility functions

std::string AdvancedCLI::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool AdvancedCLI::stringStartsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool AdvancedCLI::stringContains(const std::string& str, const std::string& substring) {
    return str.find(substring) != std::string::npos;
}

std::string AdvancedCLI::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos) return str;
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> AdvancedCLI::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(str);
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

bool AdvancedCLI::isValidFile(const std::string& filename) {
    return std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename);
}

bool AdvancedCLI::isValidParameter(const std::string& param) {
    return !param.empty() && param.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-.") == std::string::npos;
}

// Validation functions

bool AdvancedCLI::validateLogLevel(const std::string& level) const {
    return level == "DEBUG" || level == "INFO" || level == "WARN" || level == "ERROR";
}

bool AdvancedCLI::validateBrainType(const std::string& type) const {
    return type == "phase2" || type == "phase3" || type == "phase4" ||
           type == "phase5" || type == "phase6";
}

bool AdvancedCLI::validateReportFormat(const std::string& format) const {
    return format == "text" || format == "json" || format == "csv";
}

bool AdvancedCLI::validateExperiment(const std::string& experiment) const {
    return experiment == "basic" || experiment == "plasticity" || experiment == "stdp" ||
           experiment == "integration" || experiment == "full";
}

} // namespace nlm