// CommandLineParser.cpp
#include "core/CommandLineParser.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cctype>

namespace nlm {

CommandLineParser::CommandLineParser(int argc, char** argv) 
    : argc_(argc), argv_(argv), programName_(getProgramName(argv_[0])) {
}

bool CommandLineParser::parse() {
    parseArguments();
    normalizeOptions();
    return true;
}

void CommandLineParser::parseArguments() {
    for (size_t i = 1; i < static_cast<size_t>(argc_); ++i) {
        std::string arg = argv_[i];
        
        // Handle --help and -h
        if (arg == "--help" || arg == "-h") {
            options_.help = true;
            continue;
        }
        
        // Handle --version and -v
        if (arg == "--version" || arg == "-v") {
            options_.version = true;
            continue;
        }
        
        // Handle --verbose
        if (arg == "--verbose" || arg == "-V") {
            options_.verbose = "info";
            continue;
        }
        
        // Handle --quiet
        if (arg == "--quiet" || arg == "-q") {
            options_.quiet = "true";
            continue;
        }
        
        // Handle --debug
        if (arg == "--debug" || arg == "-D") {
            options_.debug = "detailed";
            continue;
        }
        
        // Handle --dry-run
        if (arg == "--dry-run" || arg == "-d") {
            options_.dryRun = true;
            continue;
        }
        
        // Handle --validate-only
        if (arg == "--validate-only") {
            options_.validateOnly = true;
            continue;
        }
        
        // Handle --gpu
        if (arg == "--gpu") {
            options_.gpuEnabled = true;
            continue;
        }
        
        // Handle -e EXPERIMENT
        if (arg == "-e" && i + 1 < static_cast<size_t>(argc_)) {
            options_.experiment = argv_[++i];
            continue;
        }
        
        // Handle -c CONFIG_FILE (short option)
        if (arg == "-c" && i + 1 < static_cast<size_t>(argc_)) {
            options_.configFile = argv_[++i];
            continue;
        }
        
        // Handle --config=FILE (backward compatibility)
        if (arg.find("--config=") == 0) {
            options_.configFile = arg.substr(9);
            continue;
        }
        
        // Handle --config file (backward compatibility)
        if (arg == "--config" && i + 1 < static_cast<size_t>(argc_)) {
            options_.configFile = argv_[++i];
            continue;
        }
        
        // Handle custom experiment scripts (--script option)
        if (arg.find("--script=") == 0) {
            options_.customScripts.push_back(arg.substr(8));
            continue;
        }
        
        // Handle custom output formats
        if (arg.find("--output-format=") == 0) {
            options_.outputFormat = arg.substr(16);
            continue;
        }
        
        // Handle log file
        if (arg.find("--log-file=") == 0) {
            options_.logFile = arg.substr(11);
            continue;
        }
        
        // Handle profiling options
        if (arg == "--profile" || arg == "--profiling") {
            options_.profilingEnabled = true;
            continue;
        }
        
        if (arg.find("--profile-granularity=") == 0) {
            options_.profilingGranularity = std::stoi(arg.substr(23));
            continue;
        }
        
        if (arg.find("--profile-output=") == 0) {
            options_.profilingOutput = arg.substr(19);
            continue;
        }
        
        if (arg == "--memory-tracking" || arg == "--track-memory") {
            options_.memoryTrackingEnabled = true;
            continue;
        }
    }
}

void CommandLineParser::normalizeOptions() {
    // Normalize verbose level
    if (!options_.verbose.empty()) {
        options_.verbose = "info";
    }
    
    // Normalize quiet level  
    if (!options_.quiet.empty()) {
        options_.quiet = "true";
    }
    
    // Normalize debug level
    if (!options_.debug.empty()) {
        options_.debug = "detailed";
    }
}

void CommandLineParser::showHelp(const std::string& programName) {
    std::cout << R"(
NLM — Neural Learning Machine
Usage: "" << programName << R"" [OPTIONS]

Core Options:
  -h, --help              Show this help message and exit
  -v, --version           Show version information and exit
  -e EXPERIMENT           Run specific experiment (e.g., learning, plasticity, stdp)
  -c CONFIG_FILE          Configuration file (short option)
      --config=FILE       Configuration file (long option, backward compatible)
      --dry-run           Setup configuration and exit without running
      --validate-only     Validate configuration without running
      --gpu               Enable GPU acceleration if available

Logging Options:
  --verbose, -V           Enable verbose logging (info level)
  --quiet, -q             Suppress output
      --debug, -D         Enable debug logging
      --log-file=FILE     Log output to file

Experiment Options:
      --script=FILE       Load custom experiment script
      --output-format=FORMAT  Output format (json, csv, txt, xml)

Performance Options:
      --profile            Enable performance profiling
      --memory-tracking    Enable memory usage tracking
      --profile-granularity=N  Profiling granularity (default: 1000)
      --profile-output=FILE  Profiling output file

Advanced Options:
      --disable-all-plasticity   Disable all plasticity mechanisms
      --enable-all-plasticity    Enable all plasticity mechanisms
      --checkpoint-interval=N     Save checkpoints every N steps
      --load-checkpoint=FILE      Load from checkpoint
      --save-checkpoint=FILE      Save checkpoint

Examples:
  "" << programName << R"" --help
  "" << programName << R"" --version
  "" << programName << R"" --config=my_config.cfg --dry-run
  "" << programName << R"" -e plasticity --verbose
  "" << programName << R"" --gpu --profile --output-format=json

For more information, visit: https://github.com/neural-learning-machine/NLM

Core Experiments Available:
  learning         LearningExperiment (default)
  plasticity       PlasticityLearningExperiment
  stdp            STDPVerificationExperiment
  basic           BasicConnectivityExperiment
  phase3          Phase3Demo (agent-based learning)
  phase4          Phase4IntegratedExperiment
  phase5          Phase5IntegratedExperiment
  phase6          Phase6IntegrationExperiment

Use --dry-run to preview configuration without running experiments.
Use --validate-only to check configuration files without running experiments.
    )" << std::endl;
}

void CommandLineParser::showVersion() {
    std::cout << "NLM (熙然) — Neural Learning Machine" << std::endl;
    std::cout << "Version: Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Build: Development" << std::endl;
    std::cout << "License: MIT" << std::endl;
    std::cout << "Repository: https://github.com/neural-learning-machine/NLM" << std::endl;
}

std::string CommandLineParser::getProgramName(const std::string& argv0) {
    size_t lastSlash = argv0.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return argv0.substr(lastSlash + 1);
    }
    return argv0;
}

bool CommandLineParser::hasOption(const std::string& option) const {
    if (option == "--help") return options_.help;
    if (option == "--version") return options_.version;
    if (option == "--verbose") return !options_.verbose.empty();
    if (option == "--quiet") return !options_.quiet.empty();
    if (option == "--debug") return !options_.debug.empty();
    if (option == "--dry-run") return options_.dryRun;
    if (option == "--validate-only") return options_.validateOnly;
    if (option == "--gpu") return options_.gpuEnabled;
    if (option == "--config") return !options_.configFile.empty();
    if (option == "--experiment") return !options_.experiment.empty();
    return false;
}

std::string CommandLineParser::getOptionValue(const std::string& option) const {
    if (option == "--config") return options_.configFile;
    if (option == "--experiment") return options_.experiment;
    if (option == "--verbose") return options_.verbose;
    if (option == "--quiet") return options_.quiet;
    if (option == "--debug") return options_.debug;
    return "";
}

} // namespace nlm
