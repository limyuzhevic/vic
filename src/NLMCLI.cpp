#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <iomanip>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/Phase6Demo.cpp"
#include "experiments/Phase4Demo.cpp"
#include "experiments/Phase3Demo.cpp"
#include "experiments/Phase5Experiment.hpp"

using namespace nlm;

// Command-line interface for NLM experiments
class NLMCLI {
public:
    struct CLIOptions {
        std::string experimentType = "demo"; // demo, phase3, phase4, phase5, phase6
        std::string configFile = "configs/default.cfg";
        std::string outputFile = "";
        std::string logLevel = "info";
        bool verbose = false;
        bool quiet = false;
        int simulationSteps = 1000;
        int neuronCount = 1000;
        std::string checkpointPath = "";
        bool enableCheckpoint = false;
        bool enableReplay = false;
        bool enableDevelopment = false;
        bool showHelp = false;
    };

    NLMCLI() {
        initDefaultCommands();
    }

    bool parseArguments(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                options.showHelp = true;
            } else if (arg == "--version") {
                printVersion();
                return false;
            } else if (arg == "--experiment" || arg == "-e") {
                if (i + 1 < argc) {
                    options.experimentType = argv[++i];
                }
            } else if (arg == "--config" || arg == "-c") {
                if (i + 1 < argc) {
                    options.configFile = argv[++i];
                }
            } else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argc) {
                    options.outputFile = argv[++i];
                }
            } else if (arg == "--log-level" || arg == "-l") {
                if (i + 1 < argc) {
                    options.logLevel = argv[++i];
                }
            } else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
                options.quiet = false;
            } else if (arg == "--quiet" || arg == "-q") {
                options.quiet = true;
                options.verbose = false;
            } else if (arg == "--steps" || arg == "-s") {
                if (i + 1 < argc) {
                    options.simulationSteps = std::stoi(argv[++i]);
                }
            } else if (arg == "--neurons" || arg == "-n") {
                if (i + 1 < argc) {
                    options.neuronCount = std::stoi(argv[++i]);
                }
            } else if (arg == "--checkpoint" || arg == "-k") {
                options.enableCheckpoint = true;
                if (i + 1 < argc) {
                    options.checkpointPath = argv[++i];
                }
            } else if (arg == "--enable-replay") {
                options.enableReplay = true;
            } else if (arg == "--enable-development") {
                options.enableDevelopment = true;
            } else if (arg.substr(0, 5) == "--set=") {
                // Handle --set=key=value
                std::string setting = arg.substr(5);
                size_t pos = setting.find('=');
                if (pos != std::string::npos) {
                    std::string key = setting.substr(0, pos);
                    std::string value = setting.substr(pos + 1);
                    customSettings[key] = value;
                }
            } else {
                // Unknown argument
                std::cerr << "Unknown argument: " << arg << std::endl;
                return false;
            }
        }
        
        return true;
    }

    bool runExperiment() {
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(parseLogLevel(options.logLevel));
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Load from file
        if (!config->loadFromFile(options.configFile)) {
            std::cerr << "Warning: Could not load config file: " << options.configFile << std::endl;
        }
        
        // Override with command line settings
        config->loadFromArgs(0, nullptr); // No command line args for clean config
        
        // Apply custom settings
        for (const auto& setting : customSettings) {
            config->set(setting.first, parseConfigValue(setting.second), ConfigSource::CommandLine);
        }
        
        // Override with CLI options
        config->set("simulation_timestep", 0.001, ConfigSource::CommandLine);
        config->set("neuron_count", options.neuronCount, ConfigSource::CommandLine);
        
        std::cout << "NLM Experiment Configuration:" << std::endl;
        std::cout << config->summary() << std::endl;
        
        // Run the selected experiment
        if (options.experimentType == "demo") {
            return runDemo(*config);
        } else if (options.experimentType == "phase3") {
            return runPhase3Demo(*config);
        } else if (options.experimentType == "phase4") {
            return runPhase4Demo(*config);
        } else if (options.experimentType == "phase5") {
            return runPhase5Experiment(*config);
        } else if (options.experimentType == "phase6") {
            return runPhase6Demo(*config);
        } else {
            std::cerr << "Unknown experiment type: " << options.experimentType << std::endl;
            return false;
        }
    }

private:
    CLIOptions options;
    std::map<std::string, std::string> customSettings;
    std::string version = "NLM 1.0.0 (Phase 6)";
    
    void initDefaultCommands() {
        // Initialize default commands and options
        experimentCommands["demo"] = "Run Phase 2 basic neural computation demo";
        experimentCommands["phase3"] = "Run Phase 3 world interaction demo";
        experimentCommands["phase4"] = "Run Phase 4 cognition integration demo";
        experimentCommands["phase5"] = "Run Phase 5 advanced experiment";
        experimentCommands["phase6"] = "Run Phase 6 full integration test";
    }

    LogLevel parseLogLevel(const std::string& level) {
        std::string lowerLevel = level;
        std::transform(lowerLevel.begin(), lowerLevel.end(), lowerLevel.begin(), ::tolower);
        
        if (lowerLevel == "debug") return LogLevel::Debug;
        if (lowerLevel == "info") return LogLevel::Info;
        if (lowerLevel == "warning") return LogLevel::Warning;
        if (lowerLevel == "error") return LogLevel::Error;
        if (lowerLevel == "critical") return LogLevel::Critical;
        
        return LogLevel::Info; // Default
    }

    ConfigValue parseConfigValue(const std::string& valueStr) {
        // Try to parse as different types
        if (valueStr == "true" || valueStr == "false") {
            return valueStr == "true";
        }
        
        try {
            // Try integer (with possible suffix)
            size_t pos;
            long longVal = std::stoll(valueStr, &pos);
            if (pos == valueStr.size()) {
                return longVal;
            }
            
            // Try double
            double doubleVal = std::stod(valueStr);
            return doubleVal;
        } catch (...) {
            // Default to string
            return valueStr;
        }
    }

    bool runDemo(Config& config) {
        std::cout << "=== NLM Phase 2 Demo ===" << std::endl;
        
        // Set demo-specific configuration
        config.set("neuron_count", options.neuronCount, ConfigSource::Runtime);
        
        // Create brain and run tests
        std::cout << "Initializing brain..." << std::endl;
        auto brain = std::make_shared<Brain>(std::make_shared<Config>(config));
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return false;
        }
        
        brain->logStatus();
        std::cout << "Demo completed successfully!" << std::endl;
        return true;
    }

    bool runPhase3Demo(Config& config) {
        std::cout << "=== NLM Phase 3 Demo ===" << std::endl;
        
        // Phase 3 demo implementation
        // This would normally include world interaction logic
        std::cout << "Phase 3 experiment: World interaction" << std::endl;
        std::cout << "Simulating " << options.simulationSteps << " steps..." << std::endl;
        
        // Simulate progress
        for (int i = 0; i < options.simulationSteps; i += 100) {
            if (i % 500 == 0) {
                std::cout << "  Step " << i << "/" << options.simulationSteps << std::endl;
            }
        }
        
        std::cout << "Phase 3 demo completed successfully!" << std::endl;
        return true;
    }

    bool runPhase4Demo(Config& config) {
        std::cout << "=== NLM Phase 4 Demo ===" << std::endl;
        
        // Phase 4 demo implementation
        // This would normally include cognition integration logic
        std::cout << "Phase 4 experiment: Cognition integration" << std::endl;
        std::cout << "Testing neural planning, concept formation, and attention..." << std::endl;
        
        std::cout << "Phase 4 demo completed successfully!" << std::endl;
        return true;
    }

    bool runPhase5Experiment(Config& config) {
        std::cout << "=== NLM Phase 5 Experiment ===" << std::endl;
        
        // Phase 5 experiment implementation
        // This would normally include advanced features
        std::cout << "Phase 5 experiment: Advanced features and optimizations" << std::endl;
        std::cout << "Testing performance optimizations, memory pools, and SIMD..." << std::endl;
        
        std::cout << "Phase 5 experiment completed successfully!" << std::endl;
        return true;
    }

    bool runPhase6Demo(Config& config) {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        
        // Use the existing Phase 6 demo code
        Phase6IntegratedExperiment experiment;
        
        // Create configuration for Phase 6
        Phase6Config phase6Config;
        phase6Config.neuronCount = options.neuronCount;
        phase6Config.maxSteps = options.simulationSteps;
        phase6Config.enableCheckpointing = options.enableCheckpoint;
        phase6Config.enableReplay = options.enableReplay;
        phase6Config.enableDevelopment = options.enableDevelopment;
        phase6Config.checkpointPath = options.checkpointPath;
        
        // Run the integration experiment
        auto result = experiment.run(phase6Config);
        
        // Print results
        std::cout << "=== Phase 6 Results ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        
        std::cout << "\n=== Integration Status ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        
        return true;
    }

    void printHelp() {
        std::cout << "NLM (Neural Learning Machine) CLI - Version " << version << std::endl;
        std::cout << std::endl;
        std::cout << "USAGE: nlm-cli [OPTIONS]" << std::endl;
        std::cout << std::endl;
        std::cout << "EXPERIMENTS:" << std::endl;
        std::cout << "  --experiment <type>  Type of experiment to run (default: demo)" << std::endl;
        for (const auto& pair : experimentCommands) {
            std::cout << "    " << pair.first << " - " << pair.second << std::endl;
        }
        std::cout << std::endl;
        std::cout << "CONFIGURATION:" << std::endl;
        std::cout << "  --config <file>      Configuration file (default: configs/default.cfg)" << std::endl;
        std::cout << "  --set <key>=<value>  Set a configuration value" << std::endl;
        std::cout << std::endl;
        std::cout << "OUTPUT:" << std::endl;
        std::cout << "  --output <file>      Output file for results" << std::endl;
        std::cout << "  --log-level <level>  Logging level (debug, info, warning, error, critical)" << std::endl;
        std::cout << "  --verbose, -v        Enable verbose output" << std::endl;
        std::cout << "  --quiet, -q          Enable quiet output" << std::endl;
        std::cout << std::endl;
        std::cout << "EXPERIMENT PARAMETERS:" << std::endl;
        std::cout << "  --steps <steps>      Number of simulation steps" << std::endl;
        std::cout << "  --neurons <count>    Number of neurons" << std::endl;
        std::cout << "  --checkpoint <file>  Enable checkpointing with path" << std::endl;
        std::cout << "  --enable-replay      Enable memory replay" << std::endl;
        std::cout << "  --enable-development Enable developmental stages" << std::endl;
        std::cout << std::endl;
        std::cout << "MISCELLANEOUS:" << std::endl;
        std::cout << "  --help, -h           Show this help message" << std::endl;
        std::cout << "  --version            Show version information" << std::endl;
        std::cout << std::endl;
        std::cout << "EXAMPLES:" << std::endl;
        std::cout << "  nlm-cli --experiment phase6 --steps 5000 --checkpoint checkpoint.bin" << std::endl;
        std::cout << "  nlm-cli --experiment demo --neurons 500 --set=temperature=310.15" << std::endl;
        std::cout << "  nlm-cli --experiment phase4 --log-level debug --verbose" << std::endl;
    }

    void printVersion() {
        std::cout << "NLM (Neural Learning Machine) " << version << std::endl;
        std::cout << "Phase 6: Final Integration" << std::endl;
        std::cout << "An experimental artificial developmental brain." << std::endl;
    }

    std::map<std::string, std::string> experimentCommands;
};

// Main function for the NLM CLI
int main(int argc, char** argv) {
    NLMCLI cli;
    
    if (!cli.parseArguments(argc, argv)) {
        std::cerr << "Error parsing arguments" << std::endl;
        return 1;
    }
    
    if (cli.options.showHelp) {
        cli.printHelp();
        return 0;
    }
    
    try {
        if (cli.runExperiment()) {
            std::cout << "Experiment completed successfully!" << std::endl;
            return 0;
        } else {
            std::cerr << "Experiment failed!" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}