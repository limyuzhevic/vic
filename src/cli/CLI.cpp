#include "CLI.hpp"
#include "version.cpp"
#include "config.cpp"
#include "benchmark.cpp"
#include "experiment.cpp"
#include "analyze.cpp"
#include "../../src/core/Logger/Logger.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <fstream>

namespace nlm {
namespace cli {

CLI::CLI(int argc, char** argv) : verbosity(VerbosityLevel::Info), helpRequested(false), versionRequested(false) {
    programName = "nlm";
    version = "0.1.0 (Phase 6)";
    config = std::make_shared<Config>();
    
    if (argc > 0) {
        programName = std::string(argv[0]);
    }

    // Register available subcommands
    registerSubcommand(std::make_unique<VersionSubcommand>());
    registerSubcommand(std::make_unique<ConfigSubcommand>());
    registerSubcommand(std::make_unique<BenchmarkSubcommand>());
    registerSubcommand(std::make_unique<ExperimentSubcommand>());
    registerSubcommand(std::make_unique<AnalyzeSubcommand>());
}

bool CLI::parseArguments(int argc, char** argv) {
    std::map<std::string, std::string> options;
    std::string currentCommand;
    bool inCommand = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            helpRequested = true;
            continue;
        }
        
        if (arg == "--version") {
            versionRequested = true;
            continue;
        }
        
        if (arg == "--quiet" || arg == "-q") {
            verbosity = VerbosityLevel::Quiet;
            continue;
        }
        
        if (arg == "--verbose" || arg == "-v") {
            verbosity = VerbosityLevel::Debug;
            continue;
        }
        
        if (arg == "--verbosity") {
            if (i + 1 < argc) {
                std::string level = argv[++i];
                if (level == "quiet") verbosity = VerbosityLevel::Quiet;
                else if (level == "error") verbosity = VerbosityLevel::Error;
                else if (level == "warning") verbosity = VerbosityLevel::Warning;
                else if (level == "info") verbosity = VerbosityLevel::Info;
                else if (level == "debug") verbosity = VerbosityLevel::Debug;
            }
            continue;
        }
        
        if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
                options["--config"] = configFile;
            }
            continue;
        }
        
        if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
                options["--output"] = outputFile;
            }
            continue;
        }
        
        // Check if this is a subcommand
        auto it = subcommands.find(arg);
        if (it != subcommands.end()) {
            currentCommand = arg;
            inCommand = true;
            continue;
        }
        
        // If we're in a command, pass this to it
        if (inCommand) {
            // Check if this is a command-specific option
            if (arg.substr(0, 2) == "--") {
                std::string optionName = arg;
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    options[optionName] = argv[++i];
                } else {
                    options[optionName] = "true";
                }
            } else {
                options[currentCommand] = arg;
            }
        } else {
            // Unknown argument
            error("Unknown argument: " + arg);
            return false;
        }
    }
    
    return handleCommonOptions(options);
}

bool CLI::handleCommonOptions(std::map<std::string, std::string>& options) {
    // Load configuration from file if specified
    if (options.find("--config") != options.end()) {
        if (!loadConfigFromFile(options["--config"])) {
            return false;
        }
    }
    
    // Apply command line overrides
    applyCommandLineConfig(options.size() > 0 ? nullptr : nullptr, nullptr);
    
    return true;
}

void CLI::showHelp() const {
    std::cout << "Usage: " << programName << " [OPTIONS] COMMAND [ARGS]..." << std::endl << std::endl;
    
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -v, --verbose           Increase verbosity (can be repeated)" << std::endl;
    std::cout << "  -q, --quiet             Decrease verbosity" << std::endl;
    std::cout << "      --verbosity <level>  Set verbosity level (quiet, error, warning, info, debug)" << std::endl;
    std::cout << "  -o, --output <file>     Output file for results" << std::endl;
    std::cout << "      --config <file>      Configuration file" << std::endl << std::endl;
    
    std::cout << "COMMANDS:" << std::endl;
    
    for (const auto& pair : subcommands) {
        const auto& cmd = pair.second;
        std::cout << "  " << cmd->getName() << std::endl;
        std::cout << "      " << cmd->getDescription() << std::endl;
    }
    
    std::cout << std::endl << "Use '" << programName << " <command> --help' for more information about a command." << std::endl;
}

void CLI::showVersion() const {
    std::cout << programName << " version " << version << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
}

bool CLI::loadConfigFromFile(const std::string& filepath) {
    if (config->loadFromFile(filepath)) {
        log("Loaded configuration from: " + filepath);
        return true;
    } else {
        warning("Could not load configuration from: " + filepath + " (using defaults)");
        return true; // Continue with defaults
    }
}

void CLI::applyCommandLineConfig(int argc, char** argv) {
    // Apply command line arguments as config overrides
    config->loadFromArgs(argc, argv);
}

void CLI::setupDefaultConfig() {
    // Set sensible defaults for NLM
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
}

void CLI::log(const std::string& message, VerbosityLevel level) const {
    if (verbosity >= level) {
        std::cout << message << std::endl;
    }
}

void CLI::error(const std::string& message) const {
    std::cerr << "ERROR: " << message << std::endl;
}

void CLI::warning(const std::string& message) const {
    std::cout << "WARNING: " << message << std::endl;
}

void CLI::info(const std::string& message) const {
    log(message, VerbosityLevel::Info);
}

void CLI::debug(const std::string& message) const {
    log(message, VerbosityLevel::Debug);
}

void CLI::registerSubcommand(std::unique_ptr<Subcommand> subcommand) {
    subcommands[subcommand->getName()] = std::move(subcommand);
}

bool CLI::execute() const {
    if (helpRequested) {
        showHelp();
        return true;
    }
    
    if (versionRequested) {
        showVersion();
        return true;
    }
    
    if (subcommands.empty()) {
        error("No subcommands registered");
        return false;
    }
    
    // For now, show all commands if no specific command is provided
    // This can be enhanced to require a specific command
    showHelp();
    return true;
}

} // namespace cli
} // namespace nlm
