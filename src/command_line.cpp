#include "command_line.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/ConsoleLogger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>

namespace nlm {

CommandLine::CommandLine() : config_(std::make_shared<Config>()) {
    modeRunner_ = std::make_unique<ModeRunner>();
    initializeModes();
}

CommandLine::~CommandLine() = default;

bool CommandLine::parse(int argc, char** argv) {
    options_ = CommandLineOptions();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            options_.showHelp = true;
        }
        else if (arg == "--version") {
            options_.showVersion = true;
        }
        else if (arg == "--verbose" || arg == "-v") {
            options_.verbose = true;
        }
        else if (arg == "--quiet" || arg == "-q") {
            options_.quiet = true;
        }
        else if (arg == "--json") {
            options_.jsonOutput = true;
        }
        else if (arg == "--benchmark") {
            options_.benchmark = true;
        }
        else if (arg == "--validate-config") {
            options_.validateConfig = true;
        }
        else if (arg == "--set") {
            if (i + 1 < argc) {
                std::string setting = argv[++i];
                if (i + 1 < argc) {
                    std::string value = argv[++i];
                    options_.settings[setting] = value;
                }
            }
        }
        else if (arg == "--tests") {
            if (i + 1 < argc) {
                options_.testList = argv[++i];
            }
        }
        else if (arg == "--fast") {
            options_.fastMode = true;
        }
        else if (arg == "--thorough") {
            options_.thoroughMode = true;
        }
        else if (arg == "--plugins") {
            if (i + 1 < argc) {
                options_.pluginDir = argv[++i];
            }
        }
        else if (arg == "--experiment") {
            if (i + 1 < argc) {
                options_.experimentName = argv[++i];
            }
        }
        else if (arg == "--report") {
            if (i + 1 < argc) {
                options_.reportFile = argv[++i];
            }
        }
        else if (arg == "--metrics") {
            if (i + 1 < argc) {
                options_.metricsFile = argv[++i];
            }
        }
        else if (arg == "--save-state") {
            if (i + 1 < argc) {
                options_.saveStateFile = argv[++i];
            }
        }
        else if (arg == "--load-state") {
            if (i + 1 < argc) {
                options_.loadStateFile = argv[++i];
            }
        }
        else if (arg == "--log-level") {
            if (i + 1 < argc) {
                options_.logLevel = argv[++i];
            }
        }
        else if (arg == "--mode") {
            if (i + 1 < argc) {
                options_.mode = argv[++i];
            }
        }
        else if (arg == "--config") {
            if (i + 1 < argc) {
                options_.configFile = argv[++i];
            }
        }
        else if (arg == "--neurons") {
            if (i + 1 < argc) {
                options_.neuronCount = std::stoi(argv[++i]);
            }
        }
        else if (arg == "--episodes") {
            if (i + 1 < argc) {
                options_.episodeCount = std::stoi(argv[++i]);
            }
        }
        else if (arg == "--trials") {
            if (i + 1 < argc) {
                options_.trialCount = std::stoi(argv[++i]);
            }
        }
        else if (arg == "--timestep") {
            if (i + 1 < argc) {
                options_.timestep = std::stod(argv[++i]);
            }
        }
        else if (arg == "--neurons") {
            if (i + 1 < argc) {
                options_.neuronCount = std::stoi(argv[++i]);
            }
        }
        else {
            // Unknown argument
            std::cerr << "Unknown argument: " << arg << std::endl;
            showHelp();
            return false;
        }
    }
    
    return true;
}

void CommandLine::initializeModes() {
    // Register built-in modes
    addMode("basic", [this](const CommandLineOptions& opts, std::shared_ptr<Config> cfg) {
        return modeRunner_->runBasicMode(opts, cfg);
    });
    
    addMode("full", [this](const CommandLineOptions& opts, std::shared_ptr<Config> cfg) {
        return modeRunner_->runFullMode(opts, cfg);
    });
    
    addMode("phase3", [this](const CommandLineOptions& opts, std::shared_ptr<Config> cfg) {
        return modeRunner_->runPhase3Mode(opts, cfg);
    });
    
    addMode("phase4", [this](const CommandLineOptions& opts, std::shared_ptr<Config> cfg) {
        return modeRunner_->runPhase4Mode(opts, cfg);
    });
    
    addMode("phase6", [this](const CommandLineOptions& opts, std::shared_ptr<Config> cfg) {
        return modeRunner_->runPhase6Mode(opts, cfg);
    });
}

bool CommandLine::executeMode(const std::string& modeName) {
    auto it = modes_.find(modeName);
    if (it == modes_.end()) {
        std::cerr << "Unknown mode: " << modeName << std::endl;
        return false;
    }
    
    return it->second(options_, config_);
}

int CommandLine::execute() {
    if (options_.showHelp) {
        showHelp();
        return 0;
    }
    
    if (options_.showVersion) {
        std::cout << "NLM Phase 6 (熙然) - Neural Learning Machine" << std::endl;
        std::cout << "Version: 0.6.0 (Phase 6 Final Integration)" << std::endl;
        return 0;
    }
    
    if (options_.validateConfig) {
        if (config_->loadFromFile(options_.configFile)) {
            std::cout << "Configuration validation successful" << std::endl;
            return 0;
        } else {
            std::cerr << "Configuration validation failed" << std::endl;
            return 1;
        }
    }
    
    if (options_.mode.empty()) {
        options_.mode = "basic"; // Default mode
    }
    
    // Load configuration if specified
    if (!options_.configFile.empty()) {
        if (!config_->loadFromFile(options_.configFile)) {
            std::cerr << "Failed to load configuration from: " << options_.configFile << std::endl;
            return 1;
        }
    }
    
    // Apply settings from command line
    for (const auto& setting : options_.settings) {
        config_->set(setting.first, setting.second);
    }
    
    // Apply specific parameters
    if (options_.neuronCount > 0) {
        config_->set("brain.neuron_count", std::to_string(options_.neuronCount));
    }
    
    if (options_.episodeCount > 0) {
        config_->set("brain.episode_count", std::to_string(options_.episodeCount));
    }
    
    if (options_.trialCount > 0) {
        config_->set("brain.trial_count", std::to_string(options_.trialCount));
    }
    
    if (options_.timestep > 0.0) {
        config_->set("brain.timestep", std::to_string(options_.timestep));
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(stringToLogLevel(options_.logLevel));
    consoleLogger->setUseColors(true);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Execute the selected mode
    if (options_.benchmark) {
        auto startTime = std::chrono::high_resolution_clock::now();
        bool result = executeMode(options_.mode);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        if (options_.jsonOutput) {
            std::cout << "{\"benchmark\":{\"mode\":\"" << options_.mode 
                     << "\",\"duration_ms\":" << duration.count()
                     << ",\"success\":" << (result ? "true" : "false") << "}}" << std::endl;
        } else {
            std::cout << "Benchmark results:" << std::endl;
            std::cout << "  Mode: " << options_.mode << std::endl;
            std::cout << "  Duration: " << duration.count() << " ms" << std::endl;
            std::cout << "  Success: " << (result ? "true" : "false") << std::endl;
        }
        
        return result ? 0 : 1;
    }
    
    // Normal execution
    bool result = executeMode(options_.mode);
    
    // Save state if requested
    if (!options_.saveStateFile.empty()) {
        config_->saveToFile(options_.saveStateFile + "_config.json");
    }
    
    return result ? 0 : 1;
}

void CommandLine::showHelp() const {
    std::cout << "NLM (熙然) - Neural Learning Machine Command Line Interface" << std::endl;
    std::cout << std::endl;
    std::cout << "USAGE:" << std::endl;
    std::cout << "  nlm --mode <MODE> [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "MODES:" << std::endl;
    std::cout << "  --mode basic          Run basic connectivity and learning tests" << std::endl;
    std::cout << "  --mode full           Run complete integration demo" << std::endl;
    std::cout << "  --mode phase3         Run Phase 3 world interaction demo" << std::endl;
    std::cout << "  --mode phase4         Run Phase 4 cognition experiments" << std::endl;
    std::cout << "  --mode phase6          Run Phase 6 integration test" << std::endl;
    std::cout << "  --mode help            Show help and available modes" << std::endl;
    std::cout << std::endl;
    std::cout << "CONFIGURATION OPTIONS:" << std::endl;
    std::cout << "  --config <file>       Load configuration from JSON/YAML file" << std::endl;
    std::cout << "  --set <key>=<value>   Set individual configuration values" << std::endl;
    std::cout << "  --neurons <count>     Set neuron count" << std::endl;
    std::cout << "  --episodes <count>    Set number of episodes" << std::endl;
    std::cout << "  --trials <count>       Set number of trials" << std::endl;
    std::cout << "  --timestep <value>     Set simulation timestep" << std::endl;
    std::cout << std::endl;
    std::cout << "ADVANCED FEATURES:" << std::endl;
    std::cout << "  --verbose -v          Enable verbose logging" << std::endl;
    std::cout << "  --log-level <level>   Set log level (DEBUG, INFO, WARNING, ERROR)" << std::endl;
    std::cout << "  --save-state <file>   Save brain state to file" << std::endl;
    std::cout << "  --load-state <file>   Load brain state from file" << std::endl;
    std::cout << "  --benchmark            Run performance benchmark" << std::endl;
    std::cout << "  --version              Show version information" << std::endl;
    std::cout << std::endl;
    std::cout << "FLEXIBLE TEST SELECTION:" << std::endl;
    std::cout << "  --tests <test1>,<test2>,...  Specify which tests to run (basic/plasticity/stdp)" << std::endl;
    std::cout << "  --fast                 Run reduced test suite for quick verification" << std::endl;
    std::cout << "  --thorough             Run extended test suite for comprehensive validation" << std::endl;
    std::cout << std::endl;
    std::cout << "OUTPUT AND REPORTING:" << std::endl;
    std::cout << "  --quiet -q            Minimal output" << std::endl;
    std::cout << "  --json                 Output results in JSON format" << std::endl;
    std::cout << "  --report <file>        Save detailed report to file" << std::endl;
    std::cout << "  --metrics <file>       Save metrics to file" << std::endl;
    std::cout << std::endl;
    std::cout << "PLUGIN AND EXTENSION SUPPORT:" << std::endl;
    std::cout << "  --plugins <dir>        Load plugins from directory" << std::endl;
    std::cout << "  --experiment <name>    Run specific experiment" << std::endl;
    std::cout << "  --validate-config       Validate configuration" << std::endl;
    std::cout << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nlm --mode basic --verbose" << std::endl;
    std::cout << "  nlm --mode phase3 --config examples/config.json" << std::endl;
    std::cout << "  nlm --mode phase6 --benchmark --json" << std::endl;
    std::cout << "  nlm --mode full --thorough --report results.txt" << std::endl;
}

void CommandLine::addMode(const std::string& name, std::function<bool(const CommandLineOptions&, std::shared_ptr<Config>)> modeFunc) {
    modes_[name] = modeFunc;
}

} // namespace nlm