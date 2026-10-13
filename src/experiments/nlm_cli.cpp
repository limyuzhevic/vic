// File: /workspace/app/src/experiments/nlm_cli.cpp
// NLM Command Line Interface
// Provides a unified interface for controlling NLM brains via command line

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <map>
#include <functional>

namespace nlm {

class CommandLineInterface {
public:
    CommandLineInterface() {
        registerCommands();
    }
    
    ~CommandLineInterface() = default;
    
    bool run(int argc, char** argv);
    
private:
    // Command registry
    struct Command {
        std::string name;
        std::string description;
        std::string usage;
        std::function<bool(int, char**)> handler;
        std::string helpText;
    };
    
    std::vector<Command> commands_;
    std::shared_ptr<Config> currentConfig_;
    bool quietMode_ = false;
    
    void registerCommands() {
        // Core brain commands
        registerCommand("create", "Create a new brain with configuration", 
                       "create [--config file] [--neurons N] [--regions R]",
                       [this](int argc, char** argv) { return handleCreateBrain(argc, argv); },
                       "Create a new brain instance");
        
        registerCommand("run", "Run brain simulation", 
                       "run [--steps N] [--config file] [--output file]",
                       [this](int argc, char** argv) { return handleRunSimulation(argc, argv); },
                       "Run a brain simulation for specified steps");
        
        registerCommand("config", "Manage configuration", 
                       "config <get|set|load|save> [key] [value]",
                       [this](int argc, char** argv) { return handleConfigCommand(argc, argv); },
                       "Get/set configuration values");
        
        // Advanced commands
        registerCommand("experiment", "Run predefined experiment", 
                       "experiment <phase3|phase4|phase6> [args...]",
                       [this](int argc, char** argv) { return handleExperiment(argc, argv); },
                       "Run a specific integration experiment");
        
        registerCommand("agent", "Run agent in world", 
                       "agent [--world-size X,Y] [--vision W,H] [--episodes N]",
                       [this](int argc, char** argv) { return handleAgent(argc, argv); },
                       "Run agent brain in world environment");
        
        registerCommand("validate", "Validate configuration", 
                       "validate [--config file] [--strict]",
                       [this](int argc, char** argv) { return handleValidateConfig(argc, argv); },
                       "Validate configuration file or current config");
        
        registerCommand("benchmark", "Benchmark brain performance", 
                       "benchmark [--config file] [--duration SECONDS]",
                       [this](int argc, char** argv) { return handleBenchmark(argc, argv); },
                       "Benchmark brain performance metrics");
        
        registerCommand("status", "Display brain status", 
                       "status [--brain <name>] [--format json|text]",
                       [this](int argc, char** argv) { return handleStatus(argc, argv); },
                       "Display current brain or system status");
        
        registerCommand("checkpoint", "Save/load brain state", 
                       "checkpoint <save|load> [--filepath file] [--brain <name>]",
                       [this](int argc, char** argv) { return handleCheckpoint(argc, argv); },
                       "Save or load brain checkpoint");
        
        // Utility commands
        registerCommand("help", "Show help for commands", 
                       "help [command]",
                       [this](int argc, char** argv) { return handleHelp(argc, argv); },
                       "Show help information");
        
        registerCommand("version", "Show version information", 
                       "version",
                       [this](int argc, char** argv) { return handleVersion(argc, argv); },
                       "Show NLM version information");
        
        registerCommand("list", "List available commands", 
                       "list",
                       [this](int argc, char** argv) { return handleListCommands(argc, argv); },
                       "List all available commands");
    }
    
    void registerCommand(const std::string& name, const std::string& description, 
                        const std::string& usage, std::function<bool(int, char**)> handler,
                        const std::string& helpText) {
        Command cmd;
        cmd.name = name;
        cmd.description = description;
        cmd.usage = usage;
        cmd.handler = handler;
        cmd.helpText = helpText;
        commands_.push_back(cmd);
    }
    
    bool handleCreateBrain(int argc, char** argv) {
        printInfo("Creating new brain...");
        
        currentConfig_ = std::make_shared<Config>();
        
        // Parse command line arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--config" && i + 1 < argc) {
                std::string configFile = argv[++i];
                if (!currentConfig_->loadFromFile(configFile)) {
                    printError("Failed to load config from: " + configFile);
                    return false;
                }
                printInfo("Loaded config from: " + configFile);
            } else if (arg == "--neurons" && i + 1 < argc) {
                int neurons = std::atoi(argv[++i]);
                currentConfig_->set("neuron_count", neurons);
                printInfo("Set neuron count to: " + std::to_string(neurons));
            } else if (arg == "--regions" && i + 1 < argc) {
                int regions = std::atoi(argv[++i]);
                currentConfig_->set("region_count", regions);
                printInfo("Set region count to: " + std::to_string(regions));
            }
        }
        
        printInfo("Brain created successfully!");
        return true;
    }
    
    bool handleRunSimulation(int argc, char** argv) {
        if (!currentConfig_) {
            printError("No brain created. Use 'create' command first.");
            return false;
        }
        
        printInfo("Running simulation...");
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(currentConfig_);
        if (!brain->initialize()) {
            printError("Failed to initialize brain");
            return false;
        }
        
        int steps = 100; // Default
        std::string outputFile = "";
        
        // Parse command line arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--steps" && i + 1 < argc) {
                steps = std::atoi(argv[++i]);
            } else if (arg == "--output" && i + 1 < argc) {
                outputFile = argv[++i];
            }
        }
        
        // Run simulation
        brain->logStatus();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001f);
            
            if (!quietMode_ && step % 100 == 0) {
                std::cout << "Step " << step << ": "
                         << brain->getFiringNeuronCount() << " firing neurons, "
                         << brain->getTotalSpikeCount() << " total spikes\n";
            }
        }
        
        brain->logStatus();
        
        if (!outputFile.empty()) {
            if (brain->save(outputFile)) {
                printInfo("Simulation saved to: " + outputFile);
            } else {
                printError("Failed to save simulation to: " + outputFile);
            }
        }
        
        printInfo("Simulation completed!");
        return true;
    }
    
    bool handleConfigCommand(int argc, char** argv) {
        if (argc < 3) {
            printError("Config command requires subcommand (get, set, load, save)");
            return false;
        }
        
        std::string subcommand = argv[2];
        
        if (subcommand == "get" && argc >= 4) {
            std::string key = argv[3];
            if (currentConfig_ && currentConfig_->has(key)) {
                auto val = currentConfig_->get<std::string>(key);
                if (val.has_value()) {
                    printInfo(key + " = " + val.value());
                } else {
                    printError("Failed to get value for key: " + key);
                }
            } else {
                printError("Key not found: " + key);
            }
        } else if (subcommand == "set" && argc >= 5) {
            std::string key = argv[3];
            std::string value = argv[4];
            currentConfig_->set(key, value);
            printInfo("Set " + key + " = " + value);
        } else if (subcommand == "load" && argc >= 4) {
            std::string filepath = argv[3];
            if (currentConfig_->loadFromFile(filepath)) {
                printInfo("Config loaded from: " + filepath);
            } else {
                printError("Failed to load config from: " + filepath);
            }
        } else if (subcommand == "save" && argc >= 4) {
            std::string filepath = argv[3];
            if (currentConfig_->saveToFile(filepath)) {
                printInfo("Config saved to: " + filepath);
            } else {
                printError("Failed to save config to: " + filepath);
            }
        } else {
            printError("Invalid config command usage");
            printInfo("Usage: config <get|set|load|save> [key] [value]");
            return false;
        }
        
        return true;
    }
    
    bool handleExperiment(int argc, char** argv) {
        if (argc < 3) {
            printError("Experiment command requires experiment name (phase3, phase4, phase6)");
            return false;
        }
        
        std::string experiment = argv[2];
        
        if (experiment == "phase3") {
            printInfo("Running Phase 3 experiment...");
            // Create Phase 3 demo
            auto config = std::make_shared<Config>();
            Phase3Experiment experiment(config);
            
            int episodes = 5;
            int steps = 200;
            
            // Parse additional arguments
            for (int i = 3; i < argc; ++i) {
                std::string arg(argv[i]);
                if (arg == "--episodes" && i + 1 < argc) {
                    episodes = std::atoi(argv[++i]);
                } else if (arg == "--steps" && i + 1 < argc) {
                    steps = std::atoi(argv[++i]);
                }
            }
            
            experiment.run(episodes, steps);
        } else if (experiment == "phase4") {
            printInfo("Running Phase 4 experiment...");
            auto config = std::make_shared<Config>();
            Phase4Experiment experiment(config);
            
            int episodes = 5;
            for (int i = 3; i < argc; ++i) {
                std::string arg(argv[i]);
                if (arg == "--episodes" && i + 1 < argc) {
                    episodes = std::atoi(argv[++i]);
                }
            }
            
            experiment.run(episodes);
        } else if (experiment == "phase6") {
            printInfo("Running Phase 6 integration experiment...");
            Phase6Demo demo;
            demo.run(1, 500); // One episode, 500 steps for demo
        } else {
            printError("Unknown experiment: " + experiment);
            printInfo("Available experiments: phase3, phase4, phase6");
            return false;
        }
        
        return true;
    }
    
    bool handleAgent(int argc, char** argv) {
        printInfo("Running agent in world...");
        
        auto config = std::make_shared<Config>();
        
        // Parse arguments
        float worldWidth = 20.0f;
        float worldHeight = 20.0f;
        size_t visionWidth = 16;
        size_t visionHeight = 16;
        int episodes = 5;
        
        for (int i = 2; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--world-size" && i + 1 < argc) {
                std::string sizeStr = argv[++i];
                size_t commaPos = sizeStr.find(',');
                if (commaPos != std::string::npos) {
                    worldWidth = std::stof(sizeStr.substr(0, commaPos));
                    worldHeight = std::stof(sizeStr.substr(commaPos + 1));
                }
            } else if (arg == "--vision" && i + 1 < argc) {
                std::string visionStr = argv[++i];
                size_t commaPos = visionStr.find(',');
                if (commaPos != std::string::npos) {
                    visionWidth = static_cast<size_t>(std::stoi(visionStr.substr(0, commaPos)));
                    visionHeight = static_cast<size_t>(std::stoi(visionStr.substr(commaPos + 1)));
                }
            } else if (arg == "--episodes" && i + 1 < argc) {
                episodes = std::atoi(argv[++i]);
            }
        }
        
        // Create world
        SimpleWorld world;
        world.configure(worldWidth, worldHeight, visionWidth, visionHeight);
        world.setRandomSeed(42);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            printError("Failed to initialize brain");
            return false;
        }
        
        // Create agent
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Enable learning features
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        // Run episodes
        for (int episode = 0; episode < episodes; ++episode) {
            printInfo("Episode " + std::to_string(episode) + ": Starting");
            
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            for (int step = 0; step < 200; ++step) {
                // Get percept
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // Process input
                agentBrain.processSensoryInput(percept);
                
                // Brain step
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain->step(step * 10 + neuralStep);
                }
                
                // Get action
                MotorCommand action = agentBrain.decodeMotorCommand();
                
                // Apply to world
                ActionResult result = world.applyMotorCommand(action, world.getSimulationTime());
                
                // Apply reward
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                
                // Update world
                world.update(0.01);
                
                // Update development
                agentBrain.updateDevelopment(0.01);
                
                if (!quietMode_ && step % 50 == 0) {
                    std::cout << "  Step " << step << ": reward=" << result.reward
                             << ", energy=" << world.getAgentBody().energy
                             << ", novelty=" << agentBrain.getNoveltyLevel()
                             << ", curiosity=" << agentBrain.getCuriosityLevel()
                             << "\n";
                }
            }
            
            printInfo("Episode " + std::to_string(episode) + ": Completed. "
                     "Neuromodulation: " + std::to_string(agentBrain.getNeuromodulationLevel()) +
                     ", Novelty: " + std::to_string(agentBrain.getNoveltyLevel()) +
                     ", Curiosity: " + std::to_string(agentBrain.getCuriosityLevel()) +
                     ", Energy: " + std::to_string(world.getAgentBody().energy));
        }
        
        printInfo("Agent simulation completed!");
        return true;
    }
    
    bool handleValidateConfig(int argc, char** argv) {
        printInfo("Validating configuration...");
        
        std::string configFile = "";
        bool strict = false;
        
        // Parse arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            } else if (arg == "--strict") {
                strict = true;
            }
        }
        
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            if (!config->loadFromFile(configFile)) {
                printError("Failed to load config file: " + configFile);
                return false;
            }
        }
        
        // Basic validation
        bool valid = true;
        
        if (config->has("neuron_count")) {
            auto neurons = config->getOr<int64_t>("neuron_count", 0);
            if (neurons <= 0) {
                printError("Invalid neuron_count: " + std::to_string(neurons));
                valid = false;
            }
        }
        
        if (config->has("region_count")) {
            auto regions = config->getOr<int64_t>("region_count", 0);
            if (regions <= 0) {
                printError("Invalid region_count: " + std::to_string(regions));
                valid = false;
            }
        }
        
        if (strict) {
            // Additional strict validation
            if (config->has("simulation_timestep")) {
                auto timestep = config->getOr<double>("simulation_timestep", 0.0);
                if (timestep <= 0.0 || timestep > 1.0) {
                    printError("Invalid simulation_timestep (must be > 0 and <= 1.0): " + std::to_string(timestep));
                    valid = false;
                }
            }
            
            if (config->has("connection_probability")) {
                auto prob = config->getOr<float>("connection_probability", 1.0f);
                if (prob < 0.0f || prob > 1.0f) {
                    printError("Invalid connection_probability (must be 0.0-1.0): " + std::to_string(prob));
                    valid = false;
                }
            }
        }
        
        if (valid) {
            printInfo("Configuration validation PASSED");
        } else {
            printError("Configuration validation FAILED");
        }
        
        return valid;
    }
    
    bool handleBenchmark(int argc, char** argv) {
        printInfo("Running benchmark...");
        
        auto config = std::make_shared<Config>();
        double duration = 10.0; // Default 10 seconds
        
        // Parse arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--config" && i + 1 < argc) {
                std::string configFile = argv[++i];
                if (!config->loadFromFile(configFile)) {
                    printError("Failed to load config from: " + configFile);
                    return false;
                }
            } else if (arg == "--duration" && i + 1 < argc) {
                duration = std::stod(argv[++i]);
            }
        }
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            printError("Failed to initialize brain");
            return false;
        }
        
        // Benchmark simulation
        auto start = std::chrono::high_resolution_clock::now();
        double elapsed = 0.0;
        size_t totalSteps = 0;
        
        printInfo("Benchmark started. Running for " + std::to_string(duration) + " seconds...");
        
        while (elapsed < duration) {
            brain->step(totalSteps, totalSteps * 0.001f);
            totalSteps++;
            
            // Check elapsed time every 1000 steps
            if (totalSteps % 1000 == 0) {
                auto now = std::chrono::high_resolution_clock::now();
                elapsed = std::chrono::duration<double>(now - start).count();
                
                if (!quietMode_) {
                    std::cout << "Steps: " << totalSteps 
                             << ", Time: " << elapsed << "s, "
                             << "Rate: " << (totalSteps / elapsed) << " steps/s\n";
                }
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(end - start).count();
        
        printInfo("Benchmark completed:");
        printInfo("  Total steps: " + std::to_string(totalSteps));
        printInfo("  Total time: " + std::to_string(totalTime) + "s");
        printInfo("  Steps per second: " + std::to_string(totalSteps / totalTime));
        printInfo("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
        printInfo("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        
        return true;
    }
    
    bool handleStatus(int argc, char** argv) {
        printInfo("System status:");
        
        if (currentConfig_) {
            printInfo("Current config has " + std::to_string(currentConfig_->getKeys().size()) + " entries");
            std::cout << "Config keys:";
            for (const auto& key : currentConfig_->getKeys()) {
                std::cout << " " << key;
            }
            std::cout << "\n";
        }
        
        printInfo("Quiet mode: " + std::string(quietMode_ ? "enabled" : "disabled"));
        return true;
    }
    
    bool handleCheckpoint(int argc, char** argv) {
        if (argc < 3) {
            printError("Checkpoint command requires action (save|load)");
            return false;
        }
        
        std::string action = argv[2];
        std::string filepath = "brain_checkpoint.bin";
        
        // Parse arguments
        for (int i = 3; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--filepath" && i + 1 < argc) {
                filepath = argv[++i];
            }
        }
        
        if (action == "save") {
            if (!currentConfig_) {
                printError("No brain to save");
                return false;
            }
            
            auto brain = std::make_shared<Brain>(currentConfig_);
            if (!brain->initialize()) {
                printError("Failed to initialize brain for save");
                return false;
            }
            
            if (brain->save(filepath)) {
                printInfo("Brain saved to: " + filepath);
            } else {
                printError("Failed to save brain to: " + filepath);
            }
        } else if (action == "load") {
            if (!currentConfig_) {
                printError("No config to load with");
                return false;
            }
            
            auto brain = std::make_shared<Brain>(currentConfig_);
            if (brain->load(filepath)) {
                printInfo("Brain loaded from: " + filepath);
            } else {
                printError("Failed to load brain from: " + filepath);
            }
        } else {
            printError("Unknown checkpoint action: " + action);
            printInfo("Available actions: save, load");
            return false;
        }
        
        return true;
    }
    
    bool handleHelp(int argc, char** argv) {
        if (argc < 3) {
            printInfo("=== NLM Command Line Interface ===");
            printInfo("Usage: nlm-cli <command> [arguments]");
            printInfo("\nAvailable commands:");
            
            for (const auto& cmd : commands_) {
                std::cout << "  " << cmd.name << " - " << cmd.description << "\n";
            }
            
            printInfo("\nFor detailed help on a specific command, use: help <command>");
            return true;
        }
        
        std::string commandName = argv[2];
        for (const auto& cmd : commands_) {
            if (cmd.name == commandName) {
                printInfo("=== " + cmd.name + " ===");
                printInfo(cmd.description);
                printInfo("Usage: nlm-cli " + cmd.usage);
                printInfo("Help: " + cmd.helpText);
                return true;
            }
        }
        
        printError("Unknown command: " + commandName);
        return false;
    }
    
    bool handleVersion(int argc, char** argv) {
        printInfo("NLM (Neural Learning Machine) v0.1.0");
        printInfo("Phase 2: Real Neural Computation");
        printInfo("Phase 3: World Interaction");
        printInfo("Phase 4: Cognitive Integration");
        printInfo("Phase 6: Final Integration (Complete)");
        printInfo("\nPython bindings: Available as 'pynlm' package");
        printInfo("Documentation: Available in docs/ and README.md");
        return true;
    }
    
    bool handleListCommands(int argc, char** argv) {
        printInfo("Available NLM commands:");
        
        for (size_t i = 0; i < commands_.size(); ++i) {
            const auto& cmd = commands_[i];
            std::cout << (i + 1) << ". " << cmd.name << " - " << cmd.description << "\n";
        }
        
        return true;
    }
    
    void printInfo(const std::string& message) {
        if (!quietMode_) {
            std::cout << "[INFO] " << message << "\n";
        }
    }
    
    void printError(const std::string& message) {
        std::cerr << "[ERROR] " << message << "\n";
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
};

bool CommandLineInterface::run(int argc, char** argv) {
    if (argc < 2) {
        printInfo("=== NLM Command Line Interface ===");
        printInfo("Usage: nlm-cli <command> [arguments]");
        printInfo("\nAvailable commands:");
        
        for (size_t i = 0; i < commands_.size(); ++i) {
            const auto& cmd = commands_[i];
            std::cout << (i + 1) << ". " << cmd.name << " - " << cmd.description << "\n";
        }
        
        printInfo("\nFor detailed help on a specific command, use: nlm-cli help <command>");
        return true;
    }
    
    std::string command = argv[1];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    // Check for global options
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
        
        if (arg == "--quiet" || arg == "-q") {
            // We'll need a reference to set quietMode_, so let's create a helper
            // For now, just parse and continue
            continue;
        } else if (arg == "--help" || arg == "-h") {
            printInfo("=== NLM Command Line Interface ===");
            printInfo("Usage: nlm-cli <command> [arguments]");
            printInfo("\nAvailable commands:");
            
            for (size_t i = 0; i < commands_.size(); ++i) {
                const auto& cmd = commands_[i];
                std::cout << (i + 1) << ". " << cmd.name << " - " << cmd.description << "\n";
            }
            
            printInfo("\nFor detailed help on a specific command, use: nlm-cli help <command>");
            return true;
        }
    }
    
    // Find and execute command
    for (const auto& cmd : commands_) {
        if (cmd.name == command) {
            return cmd.handler(argc, argv);
        }
    }
    
    printError("Unknown command: " + command);
    printInfo("Use 'nlm-cli help' for list of available commands");
    return false;
}

// Main entry point for nlm-cli
int main(int argc, char* argv[]) {
    CommandLineInterface cli;
    bool success = cli.run(argc, argv);
    return success ? 0 : 1;
}

} // namespace nlm
