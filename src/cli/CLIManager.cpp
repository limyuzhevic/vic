#include "cli/CLIManager.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace nlm::cli;

CLIManager::CLIManager()
    : configValidate(false)
    , neuronCount(500)
    , simulationTime(10.0)
    , timestep(0.001)
    , checkpointInterval(100)
    , noCheckpoint(false)
    , verbose(false)
    , debug(false)
    , trace(false)
    , interactive(false)
    , watch(false)
    , monitor(false)
    , config(nullptr)
    , brain(nullptr)
    , argc(0)
    , argv(nullptr) {
    
    // Register built-in commands
    registerCommand(std::make_unique<RunCommand>());
    registerCommand(std::make_unique<StatusCommand>());
    registerCommand(std::make_unique<ConfigCommand>());
    registerCommand(std::make_unique<MemoryCommand>());
    registerCommand(std::make_unique<NeuromodCommand>());
    registerCommand(std::make_unique<CognitionCommand>());
    registerCommand(std::make_unique<PredictionCommand>());
    registerCommand(std::make_unique<DevelopCommand>());
    registerCommand(std::make_unique<CheckpointCommand>());
    registerCommand(std::make_unique<ExperimentCommand>());
}

int CLIManager::run(int argc, char** argv) {
    this->argc = argc;
    this->argv = argv;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle flags and options
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg.substr(0, 12) == "--config-set") {
            if (arg.find('=') != std::string::npos) {
                configKeyValue = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configKeyValue = argv[++i];
            }
        } else if (arg == "--config-export" && i + 1 < argc) {
            configExportFile = argv[++i];
        } else if (arg == "--config-import" && i + 1 < argc) {
            configImportFile = argv[++i];
        } else if (arg == "--config-validate") {
            configValidate = true;
        } else if (arg == "--neuron-count" && i + 1 < argc) {
            neuronCount = std::stoll(argv[++i]);
        } else if (arg == "--simulation-time" && i + 1 < argc) {
            simulationTime = std::stod(argv[++i]);
        } else if (arg == "--timestep" && i + 1 < argc) {
            timestep = std::stod(argv[++i]);
        } else if (arg == "--checkpoint-interval" && i + 1 < argc) {
            checkpointInterval = std::stoll(argv[++i]);
        } else if (arg == "--no-checkpoint") {
            noCheckpoint = true;
        } else if (arg == "--log-level" && i + 1 < argc) {
            logLevel = argv[++i];
        } else if (arg == "--log-file" && i + 1 < argc) {
            logFile = argv[++i];
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--debug") {
            debug = true;
        } else if (arg == "--trace") {
            trace = true;
        } else if (arg == "--interactive") {
            interactive = true;
        } else if (arg == "--watch") {
            watch = true;
        } else if (arg == "--monitor") {
            monitor = true;
        } else if (command.empty()) {
            command = arg;
        } else if (subcommand.empty()) {
            subcommand = arg;
        }
    }
    
    // Parse config options
    std::vector<std::string> argStrings;
    for (int i = 1; i < argc; ++i) {
        argStrings.push_back(argv[i]);
    }
    
    parseConfigOptions(argStrings);
    parseControlOptions(argStrings);
    parseLoggingOptions(argStrings);
    parseInteractiveOptions(argStrings);
    
    // If config-related options are used, handle them
    if (!configExportFile.empty() || configValidate || !configImportFile.empty()) {
        return executeCommand();
    }
    
    // Setup configuration
    setupConfiguration();
    
    // Setup brain
    setupBrain();
    
    // Execute the requested command
    return executeCommand();
}

void CLIManager::registerCommand(std::unique_ptr<Command> command) {
    commands[command->getName()] = std::move(command);
}

Command* CLIManager::getCommand(const std::string& name) {
    auto it = commands.find(name);
    if (it != commands.end()) {
        return it->second.get();
    }
    return nullptr;
}

void CLIManager::parseConfigOptions(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                if (key == "config") {
                    configFile = arg.substr(pos + 1);
                } else if (key == "config-set") {
                    configKeyValue = arg.substr(pos + 1);
                } else if (key == "config-export") {
                    configExportFile = arg.substr(pos + 1);
                } else if (key == "config-import") {
                    configImportFile = arg.substr(pos + 1);
                } else if (key == "config-validate") {
                    configValidate = true;
                }
            }
        }
    }
}

void CLIManager::parseControlOptions(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                if (key == "neuron-count") {
                    neuronCount = std::stoll(arg.substr(pos + 1));
                } else if (key == "simulation-time") {
                    simulationTime = std::stod(arg.substr(pos + 1));
                } else if (key == "timestep") {
                    timestep = std::stod(arg.substr(pos + 1));
                } else if (key == "checkpoint-interval") {
                    checkpointInterval = std::stoll(arg.substr(pos + 1));
                } else if (key == "no-checkpoint") {
                    noCheckpoint = true;
                }
            }
        }
    }
}

void CLIManager::parseLoggingOptions(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                if (key == "log-level") {
                    logLevel = arg.substr(pos + 1);
                } else if (key == "log-file") {
                    logFile = arg.substr(pos + 1);
                }
            } else {
                if (arg == "--verbose") verbose = true;
                else if (arg == "--debug") debug = true;
                else if (arg == "--trace") trace = true;
            }
        }
    }
}

void CLIManager::parseInteractiveOptions(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        if (arg == "--interactive") interactive = true;
        else if (arg == "--watch") watch = true;
        else if (arg == "--monitor") monitor = true;
    }
}

void CLIManager::setupConfiguration() {
    config = std::make_shared<Config>();
    
    // Load from file if specified
    if (!configFile.empty()) {
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line arguments
    config->loadFromArgs(argc, argv);
    
    // Set default values
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", timestep, ConfigSource::Default);
    config->set("neuron_count", neuronCount, ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("simulation_duration", simulationTime, ConfigSource::Default);
    config->set("checkpoint_interval", checkpointInterval, ConfigSource::Default);
    config->set("enable_checkpointing", !noCheckpoint, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("  simulation_duration: " + std::to_string(config->getOr<double>("simulation_duration", 10.0)) + "s");
}

void CLIManager::setupBrain() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        throw std::runtime_error("Brain initialization failed");
    }
    
    brain->logStatus();
}

void CLIManager::printHelp(const std::string& version, 
                           const std::string& command, 
                           const std::string& subcommand) const {
    if (command.empty()) {
        std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
        std::cout << "Phase 2: Real Neural Computation" << std::endl;
        std::cout << std::endl;
        std::cout << "USAGE: nlm [OPTIONS] <COMMAND> [SUBCOMMAND]" << std::endl;
        std::cout << std::endl;
        std::cout << "COMMANDS:" << std::endl;
        std::cout << "  run         Run the NLM brain simulation" << std::endl;
        std::cout << "  status      Display brain system status and statistics" << std::endl;
        std::cout << "  config      Configuration management (show, set, export, import)" << std::endl;
        std::cout << "  memory      Memory system commands (show, clear, info)" << std::endl;
        std::cout << "  neuromod     Neuromodulation system commands (show, set)" << std::endl;
        std::cout << "  cognition   Cognition system commands (show, plan)" << std::endl;
        std::cout << "  prediction   Prediction system commands (show, predict)" << std::endl;
        std::cout << "  develop     Development system commands (show, stage)" << std::endl;
        std::cout << "  checkpoint  Checkpoint management commands (save, load, list)" << std::endl;
        std::cout << "  experiment  Run specific experiments" << std::endl;
        std::cout << "  help        Display command help and usage" << std::endl;
        std::cout << std::endl;
        std::cout << "GLOBAL OPTIONS:" << std::endl;
        std::cout << "  --help, -h              Show this help message" << std::endl;
        std::cout << "  --version               Show version information" << std::endl;
        std::cout << "  --config <file>         Specify configuration file" << std::endl;
        std::cout << "  --config-set <key=value> Set configuration value" << std::endl;
        std::cout << "  --config-export <file>  Export configuration to file" << std::endl;
        std::cout << "  --config-import <file>  Import configuration from file" << std::endl;
        std::cout << "  --config-validate       Validate configuration" << std::endl;
        std::cout << "  --neuron-count <count>  Set number of neurons" << std::endl;
        std::cout << "  --simulation-time <time> Set simulation duration" << std::endl;
        std::cout << "  --timestep <time>       Set simulation timestep" << std::endl;
        std::cout << "  --checkpoint-interval <steps> Set checkpoint interval" << std::endl;
        std::cout << "  --no-checkpoint         Disable checkpointing" << std::endl;
        std::cout << "  --log-level <level>     Set logging level" << std::endl;
        std::cout << "  --log-file <file>       Set log file" << std::endl;
        std::cout << "  --verbose               Enable verbose logging" << std::endl;
        std::cout << "  --debug                 Enable debug mode" << std::endl;
        std::cout << "  --trace                 Enable trace logging" << std::endl;
        std::cout << "  --interactive           Enable interactive command-line mode" << std::endl;
        std::cout << "  --watch                 Watch configuration files for changes" << std::endl;
        std::cout << "  --monitor               Monitor system performance" << std::endl;
        std::cout << std::endl;
        std::cout << "Use 'nlm help <COMMAND>' for more information about a specific command." << std::endl;
    } else {
        Command* cmd = getCommand(command);
        if (cmd) {
            std::cout << "Command: " << command << std::endl;
            std::cout << cmd->getDescription() << std::endl;
            std::cout << std::endl;
            std::cout << "Usage: nlm " << command << " " << cmd->getUsage() << std::endl;
            std::cout << std::endl;
            std::cout << "Description:" << std::endl;
            std::cout << "  " << cmd->getDescription() << std::endl;
            std::cout << std::endl;
        }
    }
}

void CLIManager::printVersion() const {
    std::cout << "NLM Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Version: 2.0" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
}

int CLIManager::executeCommand() {
    // Handle config-related operations first
    if (!configExportFile.empty()) {
        if (!config->saveToFile(configExportFile)) {
            std::cerr << "Failed to export configuration to: " << configExportFile << std::endl;
            return 1;
        }
        NLM_LOG_INFO("Configuration exported to: " + configExportFile);
        return 0;
    }
    
    if (configValidate) {
        bool valid = config->validateSchema();
        std::cout << "Configuration validation: " << (valid ? "PASS" : "FAIL") << std::endl;
        return valid ? 0 : 1;
    }
    
    // Import configuration if requested
    if (!configImportFile.empty()) {
        if (!config->loadFromFile(configImportFile)) {
            std::cerr << "Failed to import configuration from: " << configImportFile << std::endl;
            return 1;
        }
        NLM_LOG_INFO("Configuration imported from: " + configImportFile);
    }
    
    // Check if brain is already initialized
    if (!brain) {
        setupBrain();
    }
    
    // Execute the requested command
    Command* cmd = getCommand(command);
    if (!cmd) {
        NLM_LOG_ERROR("Unknown command: " + command);
        printHelp();
        return 1;
    }
    
    std::vector<std::string> cmdArgs;
    if (!subcommand.empty()) {
        cmdArgs.push_back(subcommand);
    }
    
    return cmd->execute(cmdArgs);
}
