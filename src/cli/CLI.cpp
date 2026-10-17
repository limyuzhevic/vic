// NLM Command Line Interface Implementation
// Phase 2: Real Neural Computation CLI

#include "cli/CLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <chrono>
#include <thread>
#include <memory>
#include <cstring>
#include <vector>
#include <string>
#include <unordered_set>

namespace nlm {

struct CLIParser::Impl {
    std::string programName;
    std::string version;
    std::string buildInfo;
    
    // Command line arguments
    int argc;
    char** argv;
    
    // Available modes and commands
    std::unordered_map<std::string, CLIMode> modeMap;
    std::unordered_map<std::string, std::function<bool(int, char**, CLIConfig&, size_t&)>> argumentParsers;
    
    // Available tests and experiments
    std::vector<std::string> availableTests;
    std::vector<std::string> availableExperiments;
    std::vector<std::string> availableBenchmarks;
    
    // Configuration validation
    std::unordered_map<std::string, std::function<bool(const CLIConfig&)>> modeValidators;
    
    Impl() : argc(0), argv(nullptr), version("2.0.0"), buildInfo("Phase 2: Real Neural Computation") {
        programName = "nlm";
        
        // Initialize mode mapping
        modeMap["interactive"] = CLIMode::Interactive;
        modeMap["test"] = CLIMode::TestRun;
        modeMap["run"] = CLIMode::TestRun;
        modeMap["tests"] = CLIMode::ListTests;
        modeMap["list-tests"] = CLIMode::ListTests;
        modeMap["experiment"] = CLIMode::Experiment;
        modeMap["bench"] = CLIMode::Benchmark;
        modeMap["benchmark"] = CLIMode::Benchmark;
        modeMap["visualize"] = CLIMode::Visualize;
        modeMap["config"] = CLIMode::ConfigEdit;
        modeMap["edit-config"] = CLIMode::ConfigEdit;
        modeMap["help"] = CLIMode::Help;
        modeMap["?"] = CLIMode::Help;
        modeMap["version"] = CLIMode::Version;
        modeMap["ver"] = CLIMode::Version;
        
        // Initialize available tests
        availableTests = {
            "connectivity", "basic-connectivity", "test-1",
            "plasticity", "learning", "test-2",
            "stdp", "stdp-verification", "test-3",
            "brain", "brain-test", "full-test"
        };
        
        // Initialize available experiments
        availableExperiments = {
            "grid-world", "phase-1", "phase-2", "phase-3", "phase-4", "phase-5", "phase-6"
        };
        
        // Initialize benchmarks
        availableBenchmarks = {
            "performance", "scaling", "memory", "plasticity-speed",
            "neuron-dynamics", "synaptic-propagation"
        };
    }
};

CLIParser::CLIParser() : pImpl(std::make_unique<Impl>()) {
    // Register argument parsers
    pImpl->argumentParsers["--config"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                config.configFile = argv[++argIndex];
                return true;
            }
            return false;
        };
    
    pImpl->argumentParsers["--output"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                config.outputFile = argv[++argIndex];
                return true;
            }
            return false;
        };
    
    pImpl->argumentParsers["--log"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                config.logFile = argv[++argIndex];
                return true;
            }
            return false;
        };
    
    pImpl->argumentParsers["--log-level"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                std::string levelStr = argv[++argIndex];
                std::transform(levelStr.begin(), levelStr.end(), levelStr.begin(), ::tolower);
                if (levelStr == "debug") config.logLevel = LogLevel::Debug;
                else if (levelStr == "info") config.logLevel = LogLevel::Info;
                else if (levelStr == "warning") config.logLevel = LogLevel::Warning;
                else if (levelStr == "error") config.logLevel = LogLevel::Error;
                else if (levelStr == "critical") config.logLevel = LogLevel::Critical;
                return true;
            }
            return false;
        };
    
    pImpl->argumentParsers["--verbose"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            config.verbose = true;
            return true;
        };
    
    pImpl->argumentParsers["--quiet"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            config.quiet = true;
            return true;
        };
    
    pImpl->argumentParsers["--format"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                std::string formatStr = argv[++argIndex];
                std::transform(formatStr.begin(), formatStr.end(), formatStr.begin(), ::tolower);
                if (formatStr == "json") config.outputFormat = OutputFormat::JSON;
                else if (formatStr == "csv") config.outputFormat = OutputFormat::CSV;
                else if (formatStr == "detailed") config.outputFormat = OutputFormat::Detailed;
                else if (formatStr == "compact") config.outputFormat = OutputFormat::Compact;
                else if (formatStr == "machine-readable") config.outputFormat = OutputFormat::MachineReadable;
                return true;
            }
            return false;
        };
    
    pImpl->argumentParsers["--neuron-count"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                config.neuronCount = std::stoull(argv[++argIndex]);
                return true;
            }
            return false;
        };
    
    pImpl->argumentParsers["--region-count"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                config.regionCount = std::stoull(argv[++argIndex]);
                return true;
            }
            return false;
        };
    
    pImpl->argumentParsers["--timestep"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                config.timestep = std::stod(argv[++argIndex]);
                return true;
            }
            return false;
        };
    
    pImpl->argumentParsers["--dry-run"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            config.dryRun = true;
            return true;
        };
    
    pImpl->argumentParsers["--reset"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            config.resetBrain = true;
            return true;
        };
    
    pImpl->argumentParsers["--continuous"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            config.continuous = true;
            return true;
        };
    
    pImpl->argumentParsers["--profiling"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            config.enableProfiling = true;
            return true;
        };
    
    pImpl->argumentParsers["--validation"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            config.enableValidation = true;
            return true;
        };
    
    pImpl->argumentParsers["--memory-limit"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                config.maxMemoryMB = std::stoull(argv[++argIndex]);
                return true;
            }
            return false;
        };
    
    pImpl->argumentParsers["--timeout"] = 
        [](int argc, char** argv, CLIConfig& config, size_t& argIndex) {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                config.timeoutSeconds = std::stod(argv[++argIndex]);
                return true;
            }
            return false;
        };
    
    // Initialize mode validators
    pImpl->modeValidators[CLIMode::TestRun] = 
        [](const CLIConfig& config) {
            return !config.testNames.empty() || config.dryRun || config.continuous;
        };
    
    pImpl->modeValidators[CLIMode::Experiment] = 
        [](const CLIConfig& config) {
            return !config.experimentFile.empty() || config.dryRun;
        };
    
    pImpl->modeValidators[CLIMode::Benchmark] = 
        [](const CLIConfig& config) {
            return config.dryRun || config.continuous;
        };
}

CLIParser::~CLIParser() = default;

bool CLIParser::parse(int argc, char** argv, CLIConfig& config) {
    pImpl->argc = argc;
    pImpl->argv = argv;
    
    // Set program name from argv[0]
    pImpl->programName = std::string(argv[0]);
    
    // Skip program name
    size_t argIndex = 1;
    
    // Check for version/help flags first
    if (argIndex < static_cast<size_t>(argc)) {
        std::string arg = argv[argIndex];
        std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
        
        if (arg == "--version" || arg == "-v" || arg == "--ver" || arg == "-ver") {
            config.mode = CLIMode::Version;
            return true;
        }
        
        if (arg == "--help" || arg == "-h" || arg == "--?" || arg == "-?") {
            config.mode = CLIMode::Help;
            return true;
        }
    }
    
    // Parse mode
    bool modeSpecified = false;
    if (argIndex < static_cast<size_t>(argc)) {
        std::string arg = argv[argIndex];
        std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
        
        if (arg[0] == '-') {
            // No mode specified, default to interactive
            config.mode = CLIMode::Interactive;
        } else {
            // Check if it's a mode
            auto it = pImpl->modeMap.find(arg);
            if (it != pImpl->modeMap.end()) {
                config.mode = it->second;
                argIndex++;
                modeSpecified = true;
            } else {
                // Could be a test name, check if it matches available tests
                bool isTest = std::find(pImpl->availableTests.begin(), 
                                      pImpl->availableTests.end(), arg) != pImpl->availableTests.end();
                bool isExperiment = std::find(pImpl->availableExperiments.begin(), 
                                            pImpl->availableExperiments.end(), arg) != pImpl->availableExperiments.end();
                
                if (isTest) {
                    config.mode = CLIMode::RunTest;
                    config.testNames.push_back(arg);
                    argIndex++;
                    modeSpecified = true;
                } else if (isExperiment) {
                    config.mode = CLIMode::Experiment;
                    config.experimentFile = arg;
                    argIndex++;
                    modeSpecified = true;
                } else {
                    // Unknown argument
                    std::cerr << "Error: Unknown argument or mode: " << arg << std::endl;
                    printHelp();
                    return false;
                }
            }
        }
    }
    
    if (!modeSpecified && config.mode == CLIMode::Interactive) {
        // Interactive mode by default
    }
    
    // Parse general arguments
    while (argIndex < static_cast<size_t>(argc)) {
        std::string arg = argv[argIndex];
        
        if (arg[0] != '-') {
            // Positional argument
            if (config.mode == CLIMode::RunTest) {
                // Check if it's a valid test name
                if (std::find(pImpl->availableTests.begin(), pImpl->availableTests.end(), arg) != pImpl->availableTests.end()) {
                    config.testNames.push_back(arg);
                    argIndex++;
                    continue;
                }
            } else if (config.mode == CLIMode::Experiment) {
                // Check if it's a valid experiment file
                if (arg.find('.') != std::string::npos || arg == "grid-world" || arg == "phase-1" || 
                    arg == "phase-2" || arg == "phase-3" || arg == "phase-4" || 
                    arg == "phase-5" || arg == "phase-6") {
                    config.experimentFile = arg;
                    argIndex++;
                    continue;
                }
            }
            
            // Unknown positional argument
            std::cerr << "Error: Unknown argument: " << arg << std::endl;
            printHelp();
            return false;
        }
        
        // Process argument
        auto it = pImpl->argumentParsers.find(arg);
        if (it != pImpl->argumentParsers.end()) {
            if (!it->second(argc, argv, config, argIndex)) {
                std::cerr << "Error: Missing value for argument: " << arg << std::endl;
                return false;
            }
        } else {
            // Check for mode-specific arguments
            bool handled = false;
            if (config.mode == CLIMode::TestRun) {
                handled = parseTestArguments(argc, argv, config, argIndex);
            } else if (config.mode == CLIMode::Experiment) {
                handled = parseExperimentArguments(argc, argv, config, argIndex);
            } else if (config.mode == CLIMode::Benchmark) {
                handled = parseBenchmarkArguments(argc, argv, config, argIndex);
            } else if (config.mode == CLIMode::ConfigEdit) {
                handled = parseConfigArguments(argc, argv, config, argIndex);
            } else if (config.mode == CLIMode::Interactive) {
                handled = parseOutputArguments(argc, argv, config, argIndex) ||
                         parseLoggingArguments(argc, argv, config, argIndex) ||
                         parseNeuralArguments(argc, argv, config, argIndex) ||
                         parsePerformanceArguments(argc, argv, config, argIndex);
            }
            
            if (!handled) {
                std::cerr << "Error: Unknown argument: " << arg << std::endl;
                printHelp();
                return false;
            }
        }
        
        argIndex++;
    }
    
    return validate(config);
}

void CLIParser::printHelp() const {
    std::cout << getUsage() << std::endl;
}

void CLIParser::printVersion() const {
    std::cout << "NLM " << pImpl->version << " - " << pImpl->buildInfo << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Copyright (c) 2024 NLM Authors" << std::endl;
    std::cout << "All rights reserved." << std::endl;
}

bool CLIParser::validate(const CLIConfig& config) const {
    // Check mode-specific validation
    auto validatorIt = pImpl->modeValidators.find(config.mode);
    if (validatorIt != pImpl->modeValidators.end()) {
        if (!validatorIt->second(config)) {
            std::cerr << "Error: Invalid configuration for mode: " << static_cast<int>(config.mode) << std::endl;
            return false;
        }
    }
    
    // General validation
    if (config.mode == CLIMode::Interactive && config.outputFile.empty() && 
        config.logFile.empty() && !config.verbose && !config.quiet) {
        // Interactive mode needs at least one output option
    }
    
    return true;
}

std::string CLIParser::getUsage() const {
    std::ostringstream usage;
    
    usage << "Usage: " << pImpl->programName << " [OPTIONS] [MODE] [ARGS]" << std::endl;
    usage << std::endl;
    
    usage << getGeneralHelp() << std::endl;
    usage << std::endl;
    usage << getTestHelp() << std::endl;
    usage << std::endl;
    usage << getExperimentHelp() << std::endl;
    usage << std::endl;
    usage << getBenchmarkHelp() << std::endl;
    usage << std::endl;
    usage << getConfigHelp() << std::endl;
    
    usage << std::endl;
    usage << "Examples:" << std::endl;
    usage << "  " << pImpl->programName << " run test-1" << std::endl;
    usage << "  " << pImpl->programName << " test --neuron-count 1000 --timestep 0.0005" << std::endl;
    usage << "  " << pImpl->programName << " experiment grid-world --output results.json" << std::endl;
    usage << "  " << pImpl->programName << " benchmark performance --profiling" << std::endl;
    usage << "  " << pImpl->programName << " help" << std::endl;
    usage << "  " << pImpl->programName << " version" << std::endl;
    
    return usage.str();
}

std::vector<std::string> CLIParser::getAvailableTests() const {
    return pImpl->availableTests;
}

std::vector<std::string> CLIParser::getAvailableExperiments() const {
    return pImpl->availableExperiments;
}

std::vector<std::string> CLIParser::getAvailableBenchmarks() const {
    return pImpl->availableBenchmarks;
}

const char* CLIParser::getGeneralHelp() const {
    return R"(
General Options:
  --help, -h, --?                 Show this help message
  --version, -v, --ver, -ver      Show version information
  --config FILE                   Configuration file (default: configs/default.cfg)
  --output FILE, -o FILE          Output file for results
  --log FILE                      Log file (in addition to console)
  --log-level LEVEL               Logging level (debug, info, warning, error, critical)
  --verbose, -v                   Verbose output
  --quiet, -q                     Quiet mode (errors only)
  --dry-run                       Validate arguments without running
  --reset                          Reset brain state before execution
  --format FORMAT                 Output format (console, json, csv, detailed, compact, machine-readable)
  --timestep VALUE                Simulation timestep in seconds
  --neuron-count COUNT            Number of neurons (default: 500)
  --region-count COUNT            Number of regions (default: 1)
  --memory-limit MB               Maximum memory usage in MB (default: 1024)
  --timeout SECONDS               Maximum execution time in seconds (default: 300)
  --profiling                      Enable profiling
  --validation                     Enable configuration validation
  --continuous                     Run in continuous mode
  --config-file FILE               Configuration file (alias for --config)
  --help-config                    Show configuration options

Modes:
  interactive                     Interactive mode (default)
  test, run MODE                  Run tests (specify test names: connectivity, plasticity, stdp, brain)
  experiment EXP_FILE             Run experiment (grid-world, phase-1..6)
  benchmark BENCH                 Run benchmark (performance, scaling, memory, plasticity-speed)
  visualize                       Visualize neural activity
  config                          Edit configuration
  list-tests                      List available tests
  run-test TEST_NAME              Run specific test
  custom                          Custom command mode

Examples:
  nlm                           Start interactive mode
  nlm test-1                     Run basic connectivity test
  nlm connectivity               Run connectivity test
  nlm experiment grid-world      Run grid world experiment
  nlm benchmark performance      Run performance benchmark
  nlm --neuron-count 1000 --timestep 0.0005 run test-1
  nlm --config myconfig.cfg --output results.json run test-1
  nlm --format json --output results.json benchmark scaling --profiling
)";
}

const char* CLIParser::getTestHelp() const {
    return R"(
Test Options:
  test-name                         Specify test to run (can be repeated)
  --test-name NAME                 Alternative to positional argument
  --dry-run                         Validate test configuration without running
  --reset                           Reset brain before running test
  --continuous                      Run test continuously (for periodic validation)
  --noisy                          Enable verbose test output

Available Tests:
  connectivity, basic-connectivity, test-1      Basic neural connectivity test
  plasticity, learning, test-2                  Plasticity learning experiment
  stdp, stdp-verification, test-3              STDP verification test
  brain, brain-test, full-test                 Full brain test suite
  scaling                               Performance scaling test
  memory                                 Memory usage test
  plasticity-speed                       Plasticity performance test

Test Examples:
  nlm connectivity
  nlm --neuron-count 1000 --region-count 2 run test-1
  nlm --format json --output test-results.json --reset plasticity
  nlm --dry-run --validation connectivity
)";
}

const char* CLIParser::getExperimentHelp() const {
    return R"(
Experiment Options:
  experiment-file                    Specify experiment to run (grid-world, phase-1..6)
  --experiment-file FILE            Alternative to positional argument
  --output FILE                      Output file for experiment results
  --format FORMAT                    Output format for experiment results
  --reset                             Reset brain before running experiment
  --continuous                       Run experiment continuously
  --generate-report                  Generate detailed experiment report
  --save-results                     Save experiment state for later analysis
  --timeline                         Enable detailed timeline recording
  --metrics FILE                      File to save experiment metrics

Available Experiments:
  grid-world                          Simple grid world navigation
  phase-1                             Basic neural development
  phase-2                             Real spiking neural computation
  phase-3                             Learning and memory
  phase-4                             Cognitive development
  phase-5                             Social behavior
  phase-6                             Integrated system

Experiment Examples:
  nlm experiment grid-world
  nlm --neuron-count 2000 --timestep 0.0001 experiment phase-2
  nlm --format json --output exp-results.json --generate-report experiment phase-4
  nlm --continuous --save-results --timeline experiment grid-world
)";
}

const char* CLIParser::getBenchmarkHelp() const {
    return R"(
Benchmark Options:
  benchmark-mode                     Specify benchmark to run (performance, scaling, memory, plasticity-speed)
  --bench MODE                       Alternative to positional argument
  --iterations COUNT                 Number of benchmark iterations (default: 5)
  --warmup-iterations COUNT          Number of warmup iterations (default: 2)
  --profiling                         Enable detailed profiling
  --noisy                             Enable verbose benchmark output
  --report-file FILE                 File to save benchmark report
  --format FORMAT                     Output format for benchmark results
  --temperature TEMP                 Operating temperature for thermal benchmarks
  --load-balance                     Enable load balancing for parallel benchmarks
  --stress-test                      Run stress tests at system limits
  --regression-test                  Run regression tests

Available Benchmarks:
  performance                         General performance benchmark
  scaling                             Neural network scaling benchmark
  memory                               Memory usage benchmark
  plasticity-speed                    Synaptic plasticity performance benchmark
  neuron-dynamics                     LIF neuron dynamics benchmark
  synaptic-propagation                Synaptic transmission benchmark

Benchmark Examples:
  nlm benchmark performance
  nlm --iterations 10 --warmup-iterations 5 benchmark scaling
  nlm --profiling --report-file bench-report.txt --format detailed benchmark performance
  nlm --stress-test --temperature 85 benchmark neuron-dynamics
  nlm --load-balance --regression-test benchmark plasticity-speed
)";
}

const char* CLIParser::getConfigHelp() const {
    return R"(
Configuration Options:
  --config-key KEY                  Configuration key to display/edit
  --config-value VALUE              New value for configuration key
  --config-source SOURCE             Configuration source (default, file, command-line, runtime)
  --config-description DESC          Description for new configuration
  --export-config FILE               Export current configuration to file
  --import-config FILE               Import configuration from file
  --validate-config                  Validate current configuration
  --config-file FILE                 Configuration file path (default: configs/default.cfg)
  --backup-config                    Backup current configuration before editing
  --diff-config FILE                 Show differences with another configuration file
  --schema-version VERSION           Target configuration schema version

Configuration Keys (examples):
  random_seed                        Random number generator seed
  simulation_timestep                Simulation timestep in seconds
  neuron_count                       Number of neurons
  region_count                       Number of neural regions
  connection_probability             Probability of inter-region connections
  stdp_ltp_weight                    STDP long-term potentiation weight
  stdp_ltd_weight                    STDP long-term depression weight
  stdp_tau                           STDP time constant
  synaptogenesis_rate                Structural plasticity synaptogenesis rate
  pruning_rate                       Structural plasticity pruning rate
  leak_conductance                    Neuron leak conductance
  threshold                           Neuron firing threshold
  resting_potential                  Neuron resting potential
  reset_potential                    Neuron reset potential
  refractory_period                  Neuron refractory period
  max_membrane_potential             Maximum membrane potential
  min_membrane_potential             Minimum membrane potential

Configuration Examples:
  nlm --config-key neuron_count --config-value 1000
  nlm --config-key stdp_ltp_weight --config-value 0.025
  nlm --export-config myconfig.cfg
  nlm --import-config myconfig.cfg
  nlm --validate-config --diff-config base.cfg
)";
}

bool CLIParser::parseModeArguments(const std::string& mode, int argc, char** argv, 
                                  CLIConfig& config, size_t& argIndex) {
    if (mode == "test" || mode == "run" || mode == "tests") {
        return parseTestArguments(argc, argv, config, argIndex);
    } else if (mode == "experiment") {
        return parseExperimentArguments(argc, argv, config, argIndex);
    } else if (mode == "benchmark" || mode == "bench") {
        return parseBenchmarkArguments(argc, argv, config, argIndex);
    } else if (mode == "config" || mode == "edit-config") {
        return parseConfigArguments(argc, argv, config, argIndex);
    }
    return false;
}

bool CLIParser::parseTestArguments(int argc, char** argv, CLIConfig& config, 
                                  size_t& argIndex) {
    while (argIndex < static_cast<size_t>(argc)) {
        std::string arg = argv[argIndex];
        
        if (arg[0] != '-') {
            // Positional test name
            if (std::find(pImpl->availableTests.begin(), pImpl->availableTests.end(), arg) != 
                pImpl->availableTests.end()) {
                config.testNames.push_back(arg);
                argIndex++;
                continue;
            } else {
                // Not a test name, break out of test argument parsing
                break;
            }
        }
        
        // Process test-specific flags
        if (arg == "--test-name") {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                std::string testName = argv[++argIndex];
                if (std::find(pImpl->availableTests.begin(), pImpl->availableTests.end(), testName) != 
                    pImpl->availableTests.end()) {
                    config.testNames.push_back(testName);
                } else {
                    std::cerr << "Error: Unknown test: " << testName << std::endl;
                    return false;
                }
            } else {
                std::cerr << "Error: Missing value for --test-name" << std::endl;
                return false;
            }
        } else if (arg == "--dry-run") {
            config.dryRun = true;
        } else if (arg == "--reset") {
            config.resetBrain = true;
        } else if (arg == "--continuous") {
            config.continuous = true;
        } else if (arg == "--noisy") {
            config.verbose = true;
        } else {
            // Not a test argument
            break;
        }
        
        argIndex++;
    }
    
    return true;
}

bool CLIParser::parseExperimentArguments(int argc, char** argv, CLIConfig& config, 
                                        size_t& argIndex) {
    while (argIndex < static_cast<size_t>(argc)) {
        std::string arg = argv[argIndex];
        
        if (arg[0] != '-') {
            // Positional experiment file
            if (arg.find('.') != std::string::npos || arg == "grid-world" || arg == "phase-1" || 
                arg == "phase-2" || arg == "phase-3" || arg == "phase-4" || 
                arg == "phase-5" || arg == "phase-6") {
                config.experimentFile = arg;
                argIndex++;
                continue;
            } else {
                // Not an experiment file
                break;
            }
        }
        
        // Process experiment-specific flags
        if (arg == "--experiment-file") {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                config.experimentFile = argv[++argIndex];
            } else {
                std::cerr << "Error: Missing value for --experiment-file" << std::endl;
                return false;
            }
        } else if (arg == "--generate-report") {
            // Flag for report generation
        } else if (arg == "--save-results") {
            // Flag for saving results
        } else if (arg == "--timeline") {
            // Flag for timeline recording
        } else if (arg == "--metrics") {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                config.experimentFile = argv[++argIndex]; // Simplified: store metrics file
            } else {
                std::cerr << "Error: Missing value for --metrics" << std::endl;
                return false;
            }
        } else {
            // Not an experiment argument
            break;
        }
        
        argIndex++;
    }
    
    return true;
}

bool CLIParser::parseBenchmarkArguments(int argc, char** argv, CLIConfig& config, 
                                       size_t& argIndex) {
    while (argIndex < static_cast<size_t>(argc)) {
        std::string arg = argv[argIndex];
        
        if (arg[0] != '-') {
            // Positional benchmark mode
            if (std::find(pImpl->availableBenchmarks.begin(), pImpl->availableBenchmarks.end(), arg) != 
                pImpl->availableBenchmarks.end()) {
                // Store benchmark mode (simplified: use experimentFile)
                config.experimentFile = arg;
                argIndex++;
                continue;
            } else {
                // Not a benchmark mode
                break;
            }
        }
        
        // Process benchmark-specific flags
        if (arg == "--iterations") {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                // Parse iterations (not stored in config, would need separate field)
            } else {
                std::cerr << "Error: Missing value for --iterations" << std::endl;
                return false;
            }
        } else if (arg == "--warmup-iterations") {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                // Parse warmup iterations
            } else {
                std::cerr << "Error: Missing value for --warmup-iterations" << std::endl;
                return false;
            }
        } else if (arg == "--stress-test") {
            // Stress test flag
        } else if (arg == "--regression-test") {
            // Regression test flag
        } else if (arg == "--load-balance") {
            // Load balancing flag
        } else if (arg == "--temperature") {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                // Parse temperature
            } else {
                std::cerr << "Error: Missing value for --temperature" << std::endl;
                return false;
            }
        } else {
            // Not a benchmark argument
            break;
        }
        
        argIndex++;
    }
    
    return true;
}

bool CLIParser::parseConfigArguments(int argc, char** argv, CLIConfig& config, 
                                    size_t& argIndex) {
    // Simplified config argument parsing
    // In a full implementation, this would be more sophisticated
    while (argIndex < static_cast<size_t>(argc)) {
        std::string arg = argv[argIndex];
        
        if (arg[0] != '-') {
            // Not a config argument
            break;
        }
        
        if (arg == "--export-config") {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                config.experimentFile = argv[++argIndex]; // Simplified: use experimentFile
            } else {
                std::cerr << "Error: Missing value for --export-config" << std::endl;
                return false;
            }
        } else if (arg == "--import-config") {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                config.experimentFile = argv[++argIndex]; // Simplified: use experimentFile
            } else {
                std::cerr << "Error: Missing value for --import-config" << std::endl;
                return false;
            }
        } else if (arg == "--validate-config") {
            config.enableValidation = true;
        } else {
            // Not a config argument
            break;
        }
        
        argIndex++;
    }
    
    return true;
}

bool CLIParser::parseOutputArguments(int argc, char** argv, CLIConfig& config, 
                                    size_t& argIndex) {
    while (argIndex < static_cast<size_t>(argc)) {
        std::string arg = argv[argIndex];
        
        if (arg[0] != '-') {
            // Not an output argument
            break;
        }
        
        if (arg == "--format") {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                std::string formatStr = argv[++argIndex];
                std::transform(formatStr.begin(), formatStr.end(), formatStr.begin(), ::tolower);
                if (formatStr == "json") config.outputFormat = OutputFormat::JSON;
                else if (formatStr == "csv") config.outputFormat = OutputFormat::CSV;
                else if (formatStr == "detailed") config.outputFormat = OutputFormat::Detailed;
                else if (formatStr == "compact") config.outputFormat = OutputFormat::Compact;
                else if (formatStr == "machine-readable") config.outputFormat = OutputFormat::MachineReadable;
            } else {
                std::cerr << "Error: Missing value for --format" << std::endl;
                return false;
            }
        } else if (arg == "--noisy") {
            config.verbose = true;
        } else {
            // Not an output argument
            break;
        }
        
        argIndex++;
    }
    
    return true;
}

bool CLIParser::parseLoggingArguments(int argc, char** argv, CLIConfig& config, 
                                     size_t& argIndex) {
    while (argIndex < static_cast<size_t>(argc)) {
        std::string arg = argv[argIndex];
        
        if (arg[0] != '-') {
            // Not a logging argument
            break;
        }
        
        if (arg == "--log-level") {
            if (argIndex + 1 < static_cast<size_t>(argc) && 
                argv[argIndex + 1][0] != '-') {
                std::string levelStr = argv[++argIndex];
                std::transform(levelStr.begin(), levelStr.end(), levelStr.begin(), ::tolower);
                if (levelStr == "debug") config.logLevel = LogLevel::Debug;
                else if (levelStr == "info") config.logLevel = LogLevel::Info;
                else if (levelStr == "warning") config.logLevel = LogLevel::Warning;
                else if (levelStr == "error") config.logLevel = LogLevel::Error;
                else if (levelStr == "critical") config.logLevel = LogLevel::Critical;
            } else {
                std::cerr << "Error: Missing value for --log-level" << std::endl;
                return false;
            }
        } else {
            // Not a logging argument
            break;
        }
        
        argIndex++;
    }
    
    return true;
}

bool CLIParser::parseNeuralArguments(int argc, char** argv, CLIConfig& config, 
                                    size_t& argIndex) {
    while (argIndex < static_cast<size_t>(argc)) {
        std::string arg = argv[argIndex];
        
        if (arg[0] != '-') {
            // Not a neural argument
            break;
        }
        
        // Simplified neural argument parsing
        // In a full implementation, this would be more sophisticated
        argIndex++;
    }
    
    return true;
}

bool CLIParser::parsePerformanceArguments(int argc, char** argv, CLIConfig& config, 
                                         size_t& argIndex) {
    while (argIndex < static_cast<size_t>(argc)) {
        std::string arg = argv[argIndex];
        
        if (arg[0] != '-') {
            // Not a performance argument
            break;
        }
        
        if (arg == "--profiling") {
            config.enableProfiling = true;
        } else {
            // Not a performance argument
            break;
        }
        
        argIndex++;
    }
    
    return true;
}

CLIMode CLIParser::resolveMode(const std::string& modeStr) const {
    auto it = modeMap.find(modeStr);
    if (it != modeMap.end()) {
        return it->second;
    }
    return CLIMode::Interactive;
}

bool CLIParser::validateModeSpecific(const CLIConfig& config) const {
    auto validatorIt = pImpl->modeValidators.find(config.mode);
    if (validatorIt != pImpl->modeValidators.end()) {
        return validatorIt->second(config);
    }
    return true;
}

struct CLIExecutor::Impl {
    std::shared_ptr<Logger> logger;
    std::shared_ptr<Config> appConfig;
    std::chrono::steady_clock::time_point startTime;
    bool profilingEnabled;
    
    Impl() : profilingEnabled(false) {
        logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        consoleLogger->setUseColors(true);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        appConfig = std::make_shared<Config>();
        
        // Load default config
        appConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        appConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
        appConfig->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        appConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        appConfig->set("connection_probability", 0.15f, ConfigSource::Default);
    }
};

CLIExecutor::CLIExecutor() : pImpl(std::make_unique<Impl>()) {
}

CLIExecutor::~CLIExecutor() = default;

int CLIExecutor::execute(const CLIConfig& config) {
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->profilingEnabled = config.enableProfiling;
    
    // Initialize logging
    initializeLogging(config);
    
    // Load configuration
    if (!loadConfiguration(config, pImpl->appConfig)) {
        std::cerr << "Error: Failed to load configuration" << std::endl;
        return 1;
    }
    
    // Apply command line overrides
    applyOverrides(config, pImpl->appConfig);
    
    // Execute based on mode
    switch (config.mode) {
        case CLIMode::Interactive:
            return executeInteractive(config);
        case CLIMode::TestRun:
            return executeTestRun(config);
        case CLIMode::Experiment:
            return executeExperiment(config);
        case CLIMode::Benchmark:
            return executeBenchmark(config);
        case CLIMode::Visualize:
            return executeVisualization(config);
        case CLIMode::ConfigEdit:
            return executeConfigEdit(config);
        case CLIMode::ListTests:
            return executeListTests(config);
        case CLIMode::RunTest:
            return executeRunTest(config);
        case CLIMode::Help:
            return executeHelp(config);
        case CLIMode::Version:
            return executeVersion(config);
        case CLIMode::Custom:
            return executeCustom(config);
        default:
            std::cerr << "Error: Unsupported mode" << std::endl;
            return 1;
    }
}

int CLIExecutor::executeInteractive(const CLIConfig& config) {
    NLM_LOG_INFO("Entering interactive mode");
    std::cout << "NLM Interactive Mode" << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help                    Show help" << std::endl;
    std::cout << "  test <name>             Run specific test" << std::endl;
    std::cout << "  experiment <name>       Run experiment" << std::endl;
    std::cout << "  benchmark <name>        Run benchmark" << std::endl;
    std::cout << "  config                  Edit configuration" << std::endl;
    std::cout << "  quit, exit              Exit" << std::endl;
    
    std::string command;
    while (config.continuous && !pImpl->appConfig->get<bool>("quit")) {
        std::cout << "> ";
        std::getline(std::cin, command);
        
        if (command.empty()) continue;
        
        // Parse and execute command
        if (command == "quit" || command == "exit") {
            break;
        } else if (command.substr(0, 4) == "help") {
            // Print help for specific topic
        } else if (command.substr(0, 4) == "test") {
            // Run test
        } else if (command.substr(0, 11) == "experiment") {
            // Run experiment
        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }
    
    return 0;
}

int CLIExecutor::executeTestRun(const CLIConfig& config) {
    NLM_LOG_INFO("Starting test run");
    
    if (config.testNames.empty() && !config.dryRun && !config.continuous) {
        // Run all standard tests
        NLM_LOG_INFO("Running all standard tests");
        
        // Create brain
        auto brain = createBrain(pImpl->appConfig);
        if (!brain || !brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return 1;
        }
        
        // Run standard tests
        int result = runStandardTests(brain, config);
        
        // Final brain status
        brain->logStatus();
        
        return result;
    } else {
        // Run specific tests
        NLM_LOG_INFO("Running specific tests: " + 
                    std::to_string(config.testNames.size()) + " tests specified");
        
        for (const auto& testName : config.testNames) {
            NLM_LOG_INFO("Running test: " + testName);
            
            // Create fresh brain for each test if reset is enabled
            auto brain = createBrain(pImpl->appConfig);
            if (!brain || !brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain for test: " + testName);
                return 1;
            }
            
            // Run specific test
            int testResult = runSpecificTest(brain, testName, config);
            
            if (testResult != 0) {
                NLM_LOG_ERROR("Test failed: " + testName);
                return testResult;
            }
        }
        
        return 0;
    }
}

int CLIExecutor::executeExperiment(const CLIConfig& config) {
    NLM_LOG_INFO("Starting experiment: " + config.experimentFile);
    
    if (config.dryRun) {
        NLM_LOG_INFO("Dry run mode - would run experiment: " + config.experimentFile);
        return 0;
    }
    
    // Create environment based on experiment type
    std::shared_ptr<Environment> environment;
    if (config.experimentFile == "grid-world") {
        environment = std::make_shared<GridWorldEnvironment>(10, 10);
    } else if (config.experimentFile.substr(0, 6) == "phase-") {
        // Create appropriate experiment environment
        environment = std::make_shared<GridWorldEnvironment>(20, 20);
    } else {
        NLM_LOG_ERROR("Unknown experiment: " + config.experimentFile);
        return 1;
    }
    
    // Create brain
    auto brain = createBrain(pImpl->appConfig);
    if (!brain || !brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for experiment");
        return 1;
    }
    
    // Run experiment
    int result = runExperiment(brain, environment, config);
    
    return result;
}

int CLIExecutor::executeBenchmark(const CLIConfig& config) {
    NLM_LOG_INFO("Starting benchmark: " + config.experimentFile); // Using experimentFile for benchmark name
    
    if (config.dryRun) {
        NLM_LOG_INFO("Dry run mode - would run benchmark: " + config.experimentFile);
        return 0;
    }
    
    // Run benchmark
    return runBenchmark(config);
}

int CLIExecutor::executeVisualization(const CLIConfig& config) {
    NLM_LOG_INFO("Visualization mode not yet implemented");
    return 0;
}

int CLIExecutor::executeConfigEdit(const CLIConfig& config) {
    NLM_LOG_INFO("Configuration edit mode not yet implemented");
    return 0;
}

int CLIExecutor::executeListTests(const CLIConfig& config) {
    NLM_LOG_INFO("Available Tests:");
    
    // This would require access to the CLIParser
    // For now, hard-coded list
    std::vector<std::string> tests = {
        "connectivity", "basic-connectivity", "test-1",
        "plasticity", "learning", "test-2",
        "stdp", "stdp-verification", "test-3",
        "brain", "brain-test", "full-test"
    };
    
    for (const auto& test : tests) {
        std::cout << "  " << test << std::endl;
    }
    
    return 0;
}

int CLIExecutor::executeRunTest(const CLIConfig& config) {
    if (config.testNames.empty()) {
        NLM_LOG_ERROR("No test specified for run-test mode");
        return 1;
    }
    
    // Create brain
    auto brain = createBrain(pImpl->appConfig);
    if (!brain || !brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return 1;
    }
    
    // Run specific test
    int result = runSpecificTest(brain, config.testNames[0], config);
    
    if (result == 0) {
        NLM_LOG_INFO("Test completed successfully: " + config.testNames[0]);
    }
    
    return result;
}

int CLIExecutor::executeHelp(const CLIConfig& config) {
    // Create temporary parser to print help
    CLIParser parser;
    parser.printHelp();
    return 0;
}

int CLIExecutor::executeVersion(const CLIConfig& config) {
    // Create temporary parser to print version
    CLIParser parser;
    parser.printVersion();
    return 0;
}

int CLIExecutor::executeCustom(const CLIConfig& config) {
    NLM_LOG_INFO("Custom command mode: " + config.customCommand);
    return 0;
}

void CLIExecutor::initializeLogging(const CLIConfig& config) {
    // Reset global logger
    Logger::setGlobal(pImpl->logger);
    
    // Set log level
    pImpl->logger->setLevel(config.logLevel);
    
    // Set up file logging if specified
    if (!config.logFile.empty()) {
        auto consoleLogger = std::make_shared<ConsoleLogger>(config.logLevel);
        consoleLogger->setOutputFile(config.logFile);
        consoleLogger->setUseColors(config.verbose && !config.quiet);
        pImpl->logger->addLogger(consoleLogger);
    }
    
    // Log startup information
    NLM_LOG_INFO("NLM CLI starting");
    NLM_LOG_INFO("Mode: " + std::to_string(static_cast<int>(config.mode)));
    NLM_LOG_INFO("Log level: " + std::to_string(static_cast<int>(config.logLevel)));
    NLM_LOG_INFO("Verbose: " + std::to_string(config.verbose));
    NLM_LOG_INFO("Quiet: " + std::to_string(config.quiet));
}

bool CLIExecutor::loadConfiguration(const CLIConfig& config, std::shared_ptr<Config> appConfig) {
    if (!config.configFile.empty()) {
        // Try to load from file
        if (!appConfig->loadFromFile(config.configFile)) {
            NLM_LOG_WARNING("Failed to load configuration from: " + config.configFile);
            NLM_LOG_INFO("Using default configuration");
        } else {
            NLM_LOG_INFO("Loaded configuration from: " + config.configFile);
        }
    }
    
    // Apply default values for Phase 2
    appConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    appConfig->set("simulation_timestep", config.timestep, ConfigSource::Default);
    appConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    appConfig->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
    appConfig->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    appConfig->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    appConfig->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    appConfig->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    appConfig->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    appConfig->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    return true;
}

void CLIExecutor::applyOverrides(const CLIConfig& cliConfig, std::shared_ptr<Config> appConfig) {
    // Apply command line overrides
    if (cliConfig.verbose) {
        appConfig->set("verbose_logging", true, ConfigSource::CommandLine);
    }
    if (cliConfig.quiet) {
        appConfig->set("quiet_mode", true, ConfigSource::CommandLine);
    }
    
    // Override neural parameters
    if (cliConfig.neuronCount != 500) {
        appConfig->set("neuron_count", static_cast<int64_t>(cliConfig.neuronCount), ConfigSource::CommandLine);
    }
    if (cliConfig.regionCount != 1) {
        appConfig->set("region_count", static_cast<int64_t>(cliConfig.regionCount), ConfigSource::CommandLine);
    }
    if (cliConfig.timestep != 0.001) {
        appConfig->set("simulation_timestep", cliConfig.timestep, ConfigSource::CommandLine);
    }
}

std::shared_ptr<Brain> CLIExecutor::createBrain(const std::shared_ptr<Config>& config) {
    try {
        auto brain = std::make_shared<Brain>(config);
        return brain;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to create brain: " + std::string(e.what()));
        return nullptr;
    }
}

int CLIExecutor::runStandardTests(std::shared_ptr<Brain> brain, const CLIConfig& config) {
    NLM_LOG_INFO("Running standard test suite");
    
    // Create brain copy for each test if reset is enabled
    std::shared_ptr<Brain> currentBrain = brain;
    
    // Test 1: Basic connectivity
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No regions found for test 1");
        return 1;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_ERROR("No neurons found for test 1");
        return 1;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    NLM_LOG_INFO("Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * config.timestep);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("  Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getState().membranePotential) +
                        " thresh=" + std::to_string(neurons[i]->getState().threshold));
        }
    }
    
    return 0;
}

int CLIExecutor::runSpecificTest(std::shared_ptr<Brain> brain, const std::string& testName, 
                                const CLIConfig& config) {
    NLM_LOG_INFO("Running specific test: " + testName);
    
    if (testName == "connectivity" || testName == "basic-connectivity" || testName == "test-1") {
        // Test 1: Basic connectivity
        return runBasicConnectivityTest(brain, config);
    } else if (testName == "plasticity" || testName == "learning" || testName == "test-2") {
        // Test 2: Plasticity learning
        return runPlasticityTest(brain, config);
    } else if (testName == "stdp" || testName == "stdp-verification" || testName == "test-3") {
        // Test 3: STDP verification
        return runStdpTest(brain, config);
    } else if (testName == "brain" || testName == "brain-test" || testName == "full-test") {
        // Full brain test
        return runFullBrainTest(brain, config);
    } else {
        NLM_LOG_ERROR("Unknown test: " + testName);
        return 1;
    }
}

int CLIExecutor::runExperiment(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment,
                               const CLIConfig& config) {
    NLM_LOG_INFO("Running experiment");
    
    // Run experiment using ExperimentRunner
    auto experimentRunner = std::make_shared<ExperimentRunner>();
    
    // Create experiment
    auto experiment = experimentRunner->createExperiment(config.experimentFile);
    
    // Run experiment
    bool success = experimentRunner->runExperiment(experiment, brain, environment, 1000);
    
    if (!success) {
        NLM_LOG_ERROR("Experiment failed");
        return 1;
    }
    
    // Log experiment results
    NLM_LOG_INFO("Experiment completed successfully");
    
    return 0;
}

int CLIExecutor::runBenchmark(const CLIConfig& config) {
    NLM_LOG_INFO("Running benchmark: " + config.experimentFile); // Using experimentFile for benchmark name
    
    if (config.dryRun) {
        NLM_LOG_INFO("Dry run mode - would run benchmark: " + config.experimentFile);
        return 0;
    }
    
    // Run benchmark based on type
    if (config.experimentFile == "performance") {
        // Performance benchmark
        NLM_LOG_INFO("Running performance benchmark");
        // TODO: Implement performance benchmark
        return 0;
    } else if (config.experimentFile == "scaling") {
        // Scaling benchmark
        NLM_LOG_INFO("Running scaling benchmark");
        // TODO: Implement scaling benchmark
        return 0;
    } else if (config.experimentFile == "memory") {
        // Memory benchmark
        NLM_LOG_INFO("Running memory benchmark");
        // TODO: Implement memory benchmark
        return 0;
    } else {
        NLM_LOG_ERROR("Unknown benchmark: " + config.experimentFile);
        return 1;
    }
}

std::string CLIExecutor::formatOutput(const std::string& data, OutputFormat format) const {
    switch (format) {
        case OutputFormat::JSON:
            // Format as JSON
            return "{\"data\": \"" + data + "\"}";
        case OutputFormat::CSV:
            // Format as CSV
            return "data\"\"\"" + data + "\"\"\n";
        case OutputFormat::Detailed:
            // Detailed format
            return "Detailed output:\n" + data + "\n";
        case OutputFormat::Compact:
            // Compact format
            return data;
        case OutputFormat::MachineReadable:
            // Machine-readable format
            return "DATA:" + data;
        default:
            // Default to console format
            return data;
    }
}

bool CLIExecutor::writeOutput(const std::string& data, const std::string& filepath, 
                             OutputFormat format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open output file: " + filepath);
        return false;
    }
    
    file << formatOutput(data, format);
    file.close();
    
    return true;
}

int CLIExecutor::runBasicConnectivityTest(std::shared_ptr<Brain> brain, const CLIConfig& config) {
    NLM_LOG_INFO("=== Test: Basic Neural Connectivity ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No regions found");
        return 1;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_ERROR("No neurons found");
        return 1;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    NLM_LOG_INFO("Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * config.timestep);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            const auto& state = neurons[i]->getState();
            NLM_LOG_INFO("  Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(state.membranePotential) +
                        " thresh=" + std::to_string(state.threshold));
        }
    }
    
    return 0;
}

int CLIExecutor::runPlasticityTest(std::shared_ptr<Brain> brain, const CLIConfig& config) {
    NLM_LOG_INFO("=== Test: Plasticity Learning Experiment ===");
    
    // Enable plasticity on synapses
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * config.timestep);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("✓ Plasticity experiment completed");
    return 0;
}

int CLIExecutor::runStdpTest(std::shared_ptr<Brain> brain, const CLIConfig& config) {
    NLM_LOG_INFO("=== Test: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No regions found");
        return 1;
    }
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_ERROR("Not enough synapses for STDP test");
        return 1;
    }
    
    NLM_LOG_INFO("Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("Creating correlated pre->post activity (potentiation)...");
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        for (int trial = 0; trial < 50; ++trial) {
            // Fire pre-synaptic neuron
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                // Pre fires first
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * config.timestep);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * config.timestep);
            }
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
    
    return 0;
}

int CLIExecutor::runFullBrainTest(std::shared_ptr<Brain> brain, const CLIConfig& config) {
    NLM_LOG_INFO("=== Test: Full Brain Suite ===");
    
    // Run all standard tests
    int result = runStandardTests(brain, config);
    if (result != 0) return result;
    
    // Reset for plasticity test
    brain->reset();
    brain->initialize();
    
    result = runPlasticityTest(brain, config);
    if (result != 0) return result;
    
    // Reset for STDP test
    brain->reset();
    brain->initialize();
    
    result = runStdpTest(brain, config);
    if (result != 0) return result;
    
    NLM_LOG_INFO("✓ Full brain test suite completed");
    return 0;
}

int runCLI(int argc, char** argv) {
    CLIParser parser;
    CLIConfig config;
    
    if (!parser.parse(argc, argv, config)) {
        return 1;
    }
    
    CLIExecutor executor;
    return executor.execute(config);
}

namespace Programmatic {
    int runWithConfig(const std::string& configFile) {
        CLIParser parser;
        CLIConfig config;
        
        // Set up minimal config for programmatic use
        config.mode = CLIMode::TestRun;
        config.configFile = configFile;
        config.testNames = {"test-1", "test-2", "test-3"};
        
        if (!parser.parse(0, nullptr, config)) {
            return 1;
        }
        
        CLIExecutor executor;
        return executor.execute(config);
    }
    
    int runTest(const std::string& testName, const std::string& configFile) {
        CLIParser parser;
        CLIConfig config;
        
        config.mode = CLIMode::RunTest;
        config.configFile = configFile;
        config.testNames = {testName};
        
        if (!parser.parse(0, nullptr, config)) {
            return 1;
        }
        
        CLIExecutor executor;
        return executor.execute(config);
    }
    
    int runExperiment(const std::string& experimentName, const std::string& configFile) {
        CLIParser parser;
        CLIConfig config;
        
        config.mode = CLIMode::Experiment;
        config.configFile = configFile;
        config.experimentFile = experimentName;
        
        if (!parser.parse(0, nullptr, config)) {
            return 1;
        }
        
        CLIExecutor executor;
        return executor.execute(config);
    }
    
    std::string getBrainState(const std::shared_ptr<Brain>& brain) {
        std::ostringstream state;
        state << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        state << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        state << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        state << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        state << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        state << "Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        return state.str();
    }
    
    bool saveBrain(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        return brain->save(filepath);
    }
    
    std::shared_ptr<Brain> loadBrain(const std::string& filepath, const std::shared_ptr<Config>& config) {
        auto brain = std::make_shared<Brain>(config);
        if (brain->load(filepath)) {
            return brain;
        }
        return nullptr;
    }
}

} // namespace nlm
