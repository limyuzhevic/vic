// NLM CLI - Command Line Interface for Neural Learning Machine
// Provides comprehensive control over NLM simulations and experiments

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase5Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

namespace nlm {
namespace cli {

class NLMCLI {
public:
    NLMCLI() {
        initializeLogger();
    }
    
    int run(int argc, char** argv) {
        if (argc < 2) {
            printUsage();
            return 1;
        }
        
        std::string command = argv[1];
        
        try {
            if (command == "run") {
                return handleRunCommand(argc, argv);
            } else if (command == "config") {
                return handleConfigCommand(argc, argv);
            } else if (command == "experiment") {
                return handleExperimentCommand(argc, argv);
            } else if (command == "monitor") {
                return handleMonitorCommand(argc, argv);
            } else if (command == "benchmark") {
                return handleBenchmarkCommand(argc, argv);
            } else if (command == "save") {
                return handleSaveCommand(argc, argv);
            } else if (command == "load") {
                return handleLoadCommand(argc, argv);
            } else if (command == "profile") {
                return handleProfileCommand(argc, argv);
            } else if (command == "help") {
                printHelp(command);
                return 0;
            } else if (command == "version") {
                printVersion();
                return 0;
            } else {
                std::cerr << "Unknown command: " << command << std::endl;
                printUsage();
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }
    
private:
    void initializeLogger() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    int handleRunCommand(int argc, char** argv) {
        std::string configFile = "configs/default.cfg";
        std::string experimentType = "basic";
        SimulationStep steps = 1000;
        bool interactive = false;
        
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            } else if (arg.substr(0, 15) == "--experiment") {
                if (arg.find('=') != std::string::npos) {
                    experimentType = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    experimentType = argv[++i];
                }
            } else if (arg.substr(0, 8) == "--steps") {
                if (arg.find('=') != std::string::npos) {
                    steps = std::stoull(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    steps = std::stoull(argv[++i]);
                }
            } else if (arg == "--interactive") {
                interactive = true;
            }
        }
        
        NLM_LOG_INFO("Starting NLM simulation...")
        NLM_LOG_INFO("Config file: " + configFile)
        NLM_LOG_INFO("Experiment type: " + experimentType)
        NLM_LOG_INFO("Steps: " + std::to_string(steps))
        
        auto config = std::make_shared<Config>();
        
        if (configFile.empty() || !config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        NLM_LOG_INFO("Brain initialized successfully (" + 
                    std::to_string(brain->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(brain->getTotalSynapseCount()) + " synapses)");
        
        if (interactive) {
            runInteractiveMode(brain, steps);
        } else {
            runNonInteractiveMode(brain, steps, experimentType);
        }
        
        brain->logStatus();
        return 0;
    }
    
    int handleConfigCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing subcommand for config command" << std::endl;
            printConfigHelp();
            return 1;
        }
        
        std::string subcommand = argv[2];
        
        if (subcommand == "show") {
            return handleConfigShowCommand(argc, argv);
        } else if (subcommand == "set") {
            return handleConfigSetCommand(argc, argv);
        } else if (subcommand == "load") {
            return handleConfigLoadCommand(argc, argv);
        } else if (subcommand == "save") {
            return handleConfigSaveCommand(argc, argv);
        } else if (subcommand == "validate") {
            return handleConfigValidateCommand(argc, argv);
        } else {
            std::cerr << "Unknown config subcommand: " << subcommand << std::endl;
            printConfigHelp();
            return 1;
        }
    }
    
    int handleConfigShowCommand(int argc, char** argv) {
        std::string configFile = "configs/default.cfg";
        
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            }
        }
        
        auto config = std::make_shared<Config>();
        
        if (configFile.empty() || !config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        NLM_LOG_INFO("Configuration:");
        NLM_LOG_INFO(config->summary());
        return 0;
    }
    
    int handleConfigSetCommand(int argc, char** argv) {
        if (argc < 5) {
            std::cerr << "Error: Missing key or value for config set command" << std::endl;
            return 1;
        }
        
        std::string key = argv[3];
        std::string value = argv[4];
        
        auto config = std::make_shared<Config>();
        config->set(key, value, ConfigSource::CommandLine);
        
        NLM_LOG_INFO("Set configuration key: " + key + " = " + value);
        return 0;
    }
    
    int handleConfigLoadCommand(int argc, char** argv) {
        if (argc < 4) {
            std::cerr << "Error: Missing file path for config load command" << std::endl;
            return 1;
        }
        
        std::string configFile = argv[3];
        
        auto config = std::make_shared<Config>();
        
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Configuration loaded from: " + configFile);
            return 0;
        } else {
            NLM_LOG_ERROR("Failed to load configuration from: " + configFile);
            return 1;
        }
    }
    
    int handleConfigSaveCommand(int argc, char** argv) {
        if (argc < 4) {
            std::cerr << "Error: Missing file path for config save command" << std::endl;
            return 1;
        }
        
        std::string configFile = argv[3];
        
        auto config = std::make_shared<Config>();
        
        if (config->saveToFile(configFile)) {
            NLM_LOG_INFO("Configuration saved to: " + configFile);
            return 0;
        } else {
            NLM_LOG_ERROR("Failed to save configuration to: " + configFile);
            return 1;
        }
    }
    
    int handleConfigValidateCommand(int argc, char** argv) {
        // Placeholder for configuration validation
        NLM_LOG_INFO("Configuration validation not yet implemented.");
        return 0;
    }
    
    int handleExperimentCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing subcommand for experiment command" << std::endl;
            printExperimentHelp();
            return 1;
        }
        
        std::string subcommand = argv[2];
        
        if (subcommand == "run") {
            return handleExperimentRunCommand(argc, argv);
        } else if (subcommand == "list") {
            return handleExperimentListCommand(argc, argv);
        } else if (subcommand == "info") {
            return handleExperimentInfoCommand(argc, argv);
        } else {
            std::cerr << "Unknown experiment subcommand: " << subcommand << std::endl;
            printExperimentHelp();
            return 1;
        }
    }
    
    int handleExperimentRunCommand(int argc, char** argv) {
        std::string experimentType = "phase3";
        SimulationStep steps = 10000;
        std::string outputFile = "experiment_results.json";
        
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 15) == "--experiment") {
                if (arg.find('=') != std::string::npos) {
                    experimentType = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    experimentType = argv[++i];
                }
            } else if (arg.substr(0, 8) == "--steps") {
                if (arg.find('=') != std::string::npos) {
                    steps = std::stoull(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    steps = std::stoull(argv[++i]);
                }
            } else if (arg.substr(0, 13) == "--output-file") {
                if (arg.find('=') != std::string::npos) {
                    outputFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    outputFile = argv[++i];
                }
            }
        }
        
        NLM_LOG_INFO("Running experiment: " + experimentType);
        NLM_LOG_INFO("Steps: " + std::to_string(steps));
        NLM_LOG_INFO("Output file: " + outputFile);
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        if (experimentType == "phase3") {
            return runPhase3Experiment(brain, steps, outputFile);
        } else if (experimentType == "phase4") {
            return runPhase4Experiment(brain, steps, outputFile);
        } else if (experimentType == "phase5") {
            return runPhase5Experiment(brain, steps, outputFile);
        } else if (experimentType == "phase6") {
            return runPhase6Experiment(brain, steps, outputFile);
        } else {
            NLM_LOG_ERROR("Unknown experiment type: " + experimentType);
            return 1;
        }
    }
    
    int runPhase3Experiment(std::shared_ptr<Brain> brain, SimulationStep steps, 
                           const std::string& outputFile) {
        auto experiment = std::make_shared<Phase3Experiment>("Phase3_" + 
                             std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
        
        auto runner = std::make_shared<ExperimentRunner>();
        
        if (runner->runExperiment(experiment, brain, nullptr, steps)) {
            NLM_LOG_INFO("Phase 3 experiment completed successfully");
            
            if (!outputFile.empty()) {
                if (runner->saveResults(outputFile)) {
                    NLM_LOG_INFO("Experiment results saved to: " + outputFile);
                } else {
                    NLM_LOG_WARNING("Failed to save experiment results");
                }
            }
            
            return 0;
        } else {
            NLM_LOG_ERROR("Phase 3 experiment failed");
            return 1;
        }
    }
    
    int runPhase4Experiment(std::shared_ptr<Brain> brain, SimulationStep steps,
                           const std::string& outputFile) {
        auto experiment = std::make_shared<Phase4Experiment>("Phase4_" +
                             std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
        
        auto runner = std::make_shared<ExperimentRunner>();
        
        if (runner->runExperiment(experiment, brain, nullptr, steps)) {
            NLM_LOG_INFO("Phase 4 experiment completed successfully");
            
            if (!outputFile.empty()) {
                if (runner->saveResults(outputFile)) {
                    NLM_LOG_INFO("Experiment results saved to: " + outputFile);
                } else {
                    NLM_LOG_WARNING("Failed to save experiment results");
                }
            }
            
            return 0;
        } else {
            NLM_LOG_ERROR("Phase 4 experiment failed");
            return 1;
        }
    }
    
    int runPhase5Experiment(std::shared_ptr<Brain> brain, SimulationStep steps,
                           const std::string& outputFile) {
        auto experiment = std::make_shared<Phase5Experiment>("Phase5_" +
                             std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
        
        auto runner = std::make_shared<ExperimentRunner>();
        
        if (runner->runExperiment(experiment, brain, nullptr, steps)) {
            NLM_LOG_INFO("Phase 5 experiment completed successfully");
            
            if (!outputFile.empty()) {
                if (runner->saveResults(outputFile)) {
                    NLM_LOG_INFO("Experiment results saved to: " + outputFile);
                } else {
                    NLM_LOG_WARNING("Failed to save experiment results");
                }
            }
            
            return 0;
        } else {
            NLM_LOG_ERROR("Phase 5 experiment failed");
            return 1;
        }
    }
    
    int runPhase6Experiment(std::shared_ptr<Brain> brain, SimulationStep steps,
                           const std::string& outputFile) {
        auto experiment = std::make_shared<Phase6IntegratedExperiment>("Phase6_" +
                             std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
        
        auto runner = std::make_shared<ExperimentRunner>();
        
        if (runner->runExperiment(experiment, brain, nullptr, steps)) {
            NLM_LOG_INFO("Phase 6 experiment completed successfully");
            
            if (!outputFile.empty()) {
                if (runner->saveResults(outputFile)) {
                    NLM_LOG_INFO("Experiment results saved to: " + outputFile);
                } else {
                    NLM_LOG_WARNING("Failed to save experiment results");
                }
            }
            
            return 0;
        } else {
            NLM_LOG_ERROR("Phase 6 experiment failed");
            return 1;
        }
    }
    
    int handleExperimentListCommand(int argc, char** argv) {
        auto runner = std::make_shared<ExperimentRunner>();
        
        NLM_LOG_INFO("Registered experiments:");
        for (const auto& exp : runner->getExperiments()) {
            NLM_LOG_INFO("  - " + exp->getName());
        }
        
        return 0;
    }
    
    int handleExperimentInfoCommand(int argc, char** argv) {
        if (argc < 4) {
            std::cerr << "Error: Missing experiment name for experiment info command" << std::endl;
            return 1;
        }
        
        std::string experimentName = argv[3];
        
        auto runner = std::make_shared<ExperimentRunner>();
        auto experiment = runner->getExperiment(experimentName);
        
        if (experiment) {
            NLM_LOG_INFO("Experiment: " + experiment->getName());
            NLM_LOG_INFO("Start step: " + std::to_string(experiment->getStartStep()));
            NLM_LOG_INFO("End step: " + std::to_string(experiment->getEndStep()));
            NLM_LOG_INFO("Metrics count: " + std::to_string(experiment->getMetrics().size()));
        } else {
            NLM_LOG_ERROR("Experiment not found: " + experimentName);
            return 1;
        }
        
        return 0;
    }
    
    int handleMonitorCommand(int argc, char** argv) {
        std::string outputFile = "monitoring.log";
        double interval = 0.1;
        size_t duration = 100;
        
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 13) == "--output-file") {
                if (arg.find('=') != std::string::npos) {
                    outputFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    outputFile = argv[++i];
                }
            } else if (arg.substr(0, 9) == "--interval") {
                if (arg.find('=') != std::string::npos) {
                    interval = std::stod(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    interval = std::stod(argv[++i]);
                }
            } else if (arg.substr(0, 10) == "--duration") {
                if (arg.find('=') != std::string::npos) {
                    duration = static_cast<size_t>(std::stoull(arg.substr(arg.find('=') + 1)));
                } else if (i + 1 < argc) {
                    duration = static_cast<size_t>(std::stoull(argv[++i]));
                }
            }
        }
        
        NLM_LOG_INFO("Starting monitoring...");
        NLM_LOG_INFO("Output file: " + outputFile);
        NLM_LOG_INFO("Interval: " + std::to_string(interval) + "s");
        NLM_LOG_INFO("Duration: " + std::to_string(duration) + "s");
        
        // Placeholder for monitoring implementation
        NLM_LOG_INFO("Monitoring not yet implemented.");
        
        return 0;
    }
    
    int handleBenchmarkCommand(int argc, char** argv) {
        std::string benchmarkType = "performance";
        size_t steps = 1000;
        
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 15) == "--benchmark-type") {
                if (arg.find('=') != std::string::npos) {
                    benchmarkType = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    benchmarkType = argv[++i];
                }
            } else if (arg.substr(0, 8) == "--steps") {
                if (arg.find('=') != std::string::npos) {
                    steps = std::stoull(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    steps = std::stoull(argv[++i]);
                }
            }
        }
        
        NLM_LOG_INFO("Running benchmark: " + benchmarkType);
        NLM_LOG_INFO("Steps: " + std::to_string(steps));
        
        // Placeholder for benchmark implementation
        NLM_LOG_INFO("Benchmarking not yet implemented.");
        
        return 0;
    }
    
    int handleSaveCommand(int argc, char** argv) {
        if (argc < 4) {
            std::cerr << "Error: Missing file path for save command" << std::endl;
            return 1;
        }
        
        std::string brainFile = argv[3];
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        if (brain->save(brainFile)) {
            NLM_LOG_INFO("Brain state saved to: " + brainFile);
            return 0;
        } else {
            NLM_LOG_ERROR("Failed to save brain state to: " + brainFile);
            return 1;
        }
    }
    
    int handleLoadCommand(int argc, char** argv) {
        if (argc < 4) {
            std::cerr << "Error: Missing file path for load command" << std::endl;
            return 1;
        }
        
        std::string brainFile = argv[3];
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        if (brain->load(brainFile)) {
            NLM_LOG_INFO("Brain state loaded from: " + brainFile);
            brain->logStatus();
            return 0;
        } else {
            NLM_LOG_ERROR("Failed to load brain state from: " + brainFile);
            return 1;
        }
    }
    
    int handleProfileCommand(int argc, char** argv) {
        std::string profileType = "performance";
        
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 15) == "--profile-type") {
                if (arg.find('=') != std::string::npos) {
                    profileType = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    profileType = argv[++i];
                }
            }
        }
        
        NLM_LOG_INFO("Starting profiling: " + profileType);
        
        // Placeholder for profiling implementation
        NLM_LOG_INFO("Profiling not yet implemented.");
        
        return 0;
    }
    
    void runNonInteractiveMode(std::shared_ptr<Brain> brain, SimulationStep steps, 
                               const std::string& experimentType) {
        NLM_LOG_INFO("Running in non-interactive mode...");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step: " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        NLM_LOG_INFO("Simulation completed in " + std::to_string(duration.count()) + "ms");
        NLM_LOG_INFO("Average steps per second: " + 
                    std::to_string(static_cast<double>(steps) / (duration.count() / 1000.0)));
    }
    
    void runInteractiveMode(std::shared_ptr<Brain> brain, SimulationStep steps) {
        NLM_LOG_INFO("Running in interactive mode...")
        NLM_LOG_INFO("Available commands:")
        NLM_LOG_INFO("  step - Run one simulation step")
        NLM_LOG_INFO("  status - Show current brain status")
        NLM_LOG_INFO("  quit - Exit interactive mode")
        
        std::string command;
        SimulationStep currentStep = 0;
        
        while (currentStep < steps) {
            std::cout << "\nStep " << currentStep << "> ";
            std::getline(std::cin, command);
            
            if (command == "step") {
                brain->step(currentStep, currentStep * 0.001);
                NLM_LOG_INFO("Step completed. Spikes: " + std::to_string(brain->getTotalSpikeCount()));
                currentStep++;
            } else if (command == "status") {
                brain->logStatus();
            } else if (command == "quit") {
                break;
            } else {
                NLM_LOG_WARNING("Unknown command: " + command);
            }
        }
    }
    
    void printUsage() {
        printHelp("nlm");
    }
    
    void printHelp(const std::string& command) {
        if (command == "nlm") {
            std::cout << "Usage: nlm <command> [options]" << std::endl;
            std::cout << "\nCommands:" << std::endl;
            std::cout << "  run           Run a simulation" << std::endl;
            std::cout << "  config        Configuration management" << std::endl;
            std::cout << "  experiment    Experiment management" << std::endl;
            std::cout << "  monitor       Real-time monitoring" << std::endl;
            std::cout << "  benchmark     Performance benchmarking" << std::endl;
            std::cout << "  save <file>   Save brain state to file" << std::endl;
            std::cout << "  load <file>   Load brain state from file" << std::endl;
            std::cout << "  profile       Performance profiling" << std::endl;
            std::cout << "  help          Show help information" << std::endl;
            std::cout << "  version       Show version information" << std::endl;
        } else if (command == "config") {
            printConfigHelp();
        } else if (command == "experiment") {
            printExperimentHelp();
        }
    }
    
    void printConfigHelp() {
        std::cout << "Usage: nlm config <subcommand> [options]" << std::endl;
        std::cout << "\nSubcommands:" << std::endl;
        std::cout << "  show          Show configuration" << std::endl;
        std::cout << "  set <key> <value>  Set configuration value" << std::endl;
        std::cout << "  load <file>   Load configuration from file" << std::endl;
        std::cout << "  save <file>   Save configuration to file" << std::endl;
        std::cout << "  validate      Validate configuration" << std::endl;
    }
    
    void printExperimentHelp() {
        std::cout << "Usage: nlm experiment <subcommand> [options]" << std::endl;
        std::cout << "\nSubcommands:" << std::endl;
        std::cout << "  run           Run an experiment" << std::endl;
        std::cout << "  list          List registered experiments" << std::endl;
        std::cout << "  info <name>   Show experiment information" << std::endl;
    }
    
    void printVersion() {
        std::cout << "NLM (熙然) Neural Learning Machine v0.1.0" << std::endl;
        std::cout << "Phase 2: Real Neural Computation" << std::endl;
    }
};

int main(int argc, char** argv) {
    NLMCLI cli;
    return cli.run(argc, argv);
}

} // namespace cli
} // namespace nlm
