// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// Modern CLI interface for Phase 6 integration test
// Provides commands to run experiments, configure settings, and monitor progress

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace nlm;

// Command line argument parser
class CLIParser {
public:
    struct Command {
        std::string name;
        std::string description;
        std::vector<std::string> args;
        std::function<void(std::shared_ptr<Config>, const std::vector<std::string>&)> handler;
    };
    
    CLIParser() {
        registerCommands();
    }
    
    void registerCommands() {
        // Help command
        addCommand("help", "Show help information", {},
            [](std::shared_ptr<Config>, const std::vector<std::string>&) {
                printHelp();
            });
        
        // Version command
        addCommand("version", "Show version information", {},
            [](std::shared_ptr<Config>, const std::vector<std::string>&) {
                printVersion();
            });
        
        // Build command
        addCommand("build", "Build the NLM brain with current config", {},
            [](std::shared_ptr<Config> config, const std::vector<std::string>&) {
                buildBrain(config);
            });
        
        // Run experiment commands
        addCommand("run-phase6", "Run Phase 6 integration experiment", {
            "--neurons <count>", "Number of neurons (default: 1000)",
            "--steps <steps>", "Number of steps (default: 10000)",
            "--verbose", "Enable verbose output",
            "--config <file>", "Load configuration from file",
            "--output <file>", "Save results to file"
        },
            [](std::shared_ptr<Config> config, const std::vector<std::string>& args) {
                runPhase6Experiment(config, args);
            });
        
        // Run demo command
        addCommand("demo", "Run interactive demo", {
            "--duration <seconds>", "Demo duration in seconds (default: 60)",
            "--world-size <size>", "World size (default: 20)",
            "--speed <multiplier>", "Simulation speed multiplier (default: 1.0)"
        },
            [](std::shared_ptr<Config> config, const std::vector<std::string>& args) {
                runDemo(config, args);
            });
        
        // Config commands
        addCommand("config", "Configuration management", {
            "load <file>", "Load configuration from file",
            "save <file>", "Save configuration to file",
            "set <key> <value>", "Set configuration value",
            "get <key>", "Get configuration value",
            "list", "List all configuration keys"
        },
            [](std::shared_ptr<Config> config, const std::vector<std::string>& args) {
                handleConfigCommands(config, args);
            });
        
        // Monitor command
        addCommand("monitor", "Monitor brain activity in real-time", {
            "--interval <seconds>", "Monitoring interval (default: 5.0)",
            "--max-steps <steps>", "Maximum steps to monitor (default: 1000)"
        },
            [](std::shared_ptr<Config> config, const std::vector<std::string>& args) {
                monitorBrain(config, args);
            });
        
        // Benchmark command
        addCommand("benchmark", "Run performance benchmark", {
            "--iterations <count>", "Number of iterations (default: 5)",
            "--neurons <count>", "Number of neurons (default: 1000)",
            "--detailed", "Enable detailed benchmark results"
        },
            [](std::shared_ptr<Config> config, const std::vector<std::string>& args) {
                runBenchmark(config, args);
            });
    }
    
    void addCommand(const std::string& name, const std::string& description,
                    const std::vector<std::string>& args, std::function<void(std::shared_ptr<Config>, const std::vector<std::string>&)> handler) {
        Command cmd;
        cmd.name = name;
        cmd.description = description;
        cmd.args = args;
        cmd.handler = std::move(handler);
        commands[name] = cmd;
    }
    
    bool execute(const std::string& commandName, std::shared_ptr<Config> config,
                 const std::vector<std::string>& args) {
        auto it = commands.find(commandName);
        if (it == commands.end()) {
            std::cerr << "ERROR: Unknown command: " << commandName << "\n";
            return false;
        }
        
        const Command& cmd = it->second;
        if (cmd.handler) {
            cmd.handler(config, args);
            return true;
        }
        
        return false;
    }
    
    void printHelp() {
        std::cout << "NLM - Neural Learning Machine (Phase 6)\n";
        std::cout << "=========================================\n\n";
        
        std::cout << "Available commands:\n";
        for (const auto& pair : commands) {
            const Command& cmd = pair.second;
            std::cout << "  " << pair.first << " - " << cmd.description << "\n";
        }
        
        std::cout << "\nUse 'nlm <command> --help' for more information about a specific command.\n";
        std::cout << "\nExamples:\n";
        std::cout << "  nlm help                           # Show this help\n";
        std::cout << "  nlm version                        # Show version info\n";
        std::cout << "  nlm build                          # Build the brain\n";
        std::cout << "  nlm run-phase6 --neurons 500      # Run Phase 6 experiment\n";
        std::cout << "  nlm demo --world-size 30          # Run interactive demo\n";
        std::cout << "  nlm config load config.cfg         # Load configuration\n";
        std::cout << "  nlm monitor --interval 10          # Monitor brain activity\n";
    }
    
    void printVersion() {
        std::cout << "NLM - Neural Learning Machine (Phase 6)\n";
        std::cout << "Version: 0.1.0 (Phase 6.0)\n";
        std::cout << "Build: C++20 with SIMD optimizations\n";
        std::cout << "Integration: Complete Phase 6 - All brain systems integrated\n";
        std::cout << "Architecture: LIF neurons, STDP plasticity, neuromodulation\n";
        std::cout << "License: MIT\n";
    }
    
private:
    std::map<std::string, Command> commands;
};

// Configuration management commands
void handleConfigCommands(std::shared_ptr<Config> config, const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: config <subcommand> [options]\n";
        std::cout << "Subcommands: load, save, set, get, list\n";
        return;
    }
    
    std::string subcommand = args[0];
    
    if (subcommand == "load") {
        if (args.size() < 2) {
            std::cout << "ERROR: Missing file argument for 'load'\n";
            return;
        }
        
        if (config->loadFromFile(args[1])) {
            std::cout << "Configuration loaded successfully from: " << args[1] << "\n";
        } else {
            std::cout << "ERROR: Failed to load configuration from: " << args[1] << "\n";
        }
    }
    else if (subcommand == "save") {
        if (args.size() < 2) {
            std::cout << "ERROR: Missing file argument for 'save'\n";
            return;
        }
        
        if (config->saveToFile(args[1])) {
            std::cout << "Configuration saved successfully to: " << args[1] << "\n";
        } else {
            std::cout << "ERROR: Failed to save configuration to: " << args[1] << "\n";
        }
    }
    else if (subcommand == "set") {
        if (args.size() < 3) {
            std::cout << "ERROR: Missing key/value arguments for 'set'\n";
            return;
        }
        
        std::string key = args[1];
        std::string valueStr = args[2];
        
        // Try to parse value as different types
        try {
            if (valueStr == "true" || valueStr == "false") {
                bool value = (valueStr == "true");
                config->set(key, value);
                std::cout << "Set " << key << " = " << (value ? "true" : "false") << "\n";
            } else if (valueStr.find('.') != std::string::npos) {
                double value = std::stod(valueStr);
                config->set(key, value);
                std::cout << "Set " << key << " = " << value << "\n";
            } else {
                // Try to parse as integer
                size_t pos;
                long long intValue = std::stoll(valueStr, &pos);
                if (pos == valueStr.size()) {
                    config->set(key, intValue);
                    std::cout << "Set " << key << " = " << intValue << "\n";
                    return;
                }
                
                // Fall back to string
                config->set(key, valueStr);
                std::cout << "Set " << key << " = \"" << valueStr << "\"\n";
            }
        } catch (const std::exception& e) {
            std::cout << "ERROR: Failed to parse value: " << e.what() << "\n";
        }
    }
    else if (subcommand == "get") {
        if (args.size() < 2) {
            std::cout << "ERROR: Missing key argument for 'get'\n";
            return;
        }
        
        if (config->has(args[1])) {
            std::string key = args[1];
            auto keys = config->getKeys();
            for (const auto& k : keys) {
                if (k == key) {
                    std::cout << key << " = [";
                    
                    // Try to display the value
                    auto intVal = config->get<int>(key);
                    if (intVal) {
                        std::cout << *intVal;
                    } else {
                        auto doubleVal = config->get<double>(key);
                        if (doubleVal) {
                            std::cout << *doubleVal;
                        } else {
                            auto boolVal = config->get<bool>(key);
                            if (boolVal) {
                                std::cout << (*boolVal ? "true" : "false");
                            } else {
                                auto stringVal = config->get<std::string>(key);
                                if (stringVal) {
                                    std::cout << "\"" << *stringVal << "\"";
                                }
                            }
                        }
                    }
                    
                    std::cout << "] (source: " << static_cast<int>(config->getSource(key)) << ")\n";
                }
            }
        } else {
            std::cout << "Key not found: " << args[1] << "\n";
        }
    }
    else if (subcommand == "list") {
        std::cout << "Configuration keys:\n";
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            std::cout << "  " << key << "\n";
        }
        std::cout << "Total: " << keys.size() << " keys\n";
    }
    else {
        std::cout << "ERROR: Unknown config subcommand: " << subcommand << "\n";
    }
}

// Helper function to parse CLI arguments
std::vector<std::string> parseCLIArgs(int argc, char* argv[]) {
    std::vector<std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                // Extract both key and value
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                args.push_back("--" + key);
                args.push_back(value);
            } else {
                args.push_back(arg);
            }
        } else if (arg.substr(0, 1) == "-") {
            args.push_back(arg);
        } else {
            // Positional argument
            args.push_back(arg);
        }
    }
    
    return args;
}

void buildBrain(std::shared_ptr<Config> config) {
    std::cout << "=== NLM Brain Builder ===\n";
    
    // Get configuration values
    size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config->getOr<size_t>("region_count", 1);
    float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    
    std::cout << "Configuration:\n";
    std::cout << "  Neurons: " << neuronCount << "\n";
    std::cout << "  Regions: " << regionCount << "\n";
    std::cout << "  Connection probability: " << connectionProbability << "\n";
    
    // Create configuration for brain
    auto brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", static_cast<int64_t>(neuronCount));
    brainConfig->set("region_count", static_cast<int64_t>(regionCount));
    brainConfig->set("connection_probability", connectionProbability);
    brainConfig->set("random_seed", static_cast<int64_t>(42));  // Fixed seed for reproducibility
    
    std::cout << "\nBuilding NLM brain...\n";
    
    try {
        // Create and initialize brain
        auto brain = std::make_shared<Brain>(brainConfig);
        if (brain->initialize()) {
            std::cout << "✓ Brain built successfully!\n";
            std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << "\n";
            std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << "\n";
            std::cout << "  Brain status: ACTIVE\n";
        } else {
            std::cout << "ERROR: Failed to initialize brain\n";
        }
    } catch (const std::exception& e) {
        std::cout << "ERROR: Failed to build brain: " << e.what() << "\n";
    }
}

void runPhase6Experiment(std::shared_ptr<Config> config, const std::vector<std::string>& args) {
    std::cout << "=== NLM Phase 6 Integration Experiment ===\n";
    
    // Parse command line arguments
    size_t neuronCount = 1000;
    size_t maxSteps = 10000;
    bool verbose = false;
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--neurons") {
            if (i + 1 < args.size()) {
                neuronCount = std::stoul(args[i + 1]);
                i++;
            }
        } else if (args[i] == "--steps") {
            if (i + 1 < args.size()) {
                maxSteps = std::stoul(args[i + 1]);
                i++;
            }
        } else if (args[i] == "--verbose") {
            verbose = true;
        } else if (args[i] == "--config") {
            if (i + 1 < args.size()) {
                configFile = args[i + 1];
                i++;
            }
        } else if (args[i] == "--output") {
            if (i + 1 < args.size()) {
                outputFile = args[i + 1];
                i++;
            }
        }
    }
    
    // Create configuration
    Phase6Config phase6Config;
    phase6Config.neuronCount = neuronCount;
    phase6Config.maxSteps = maxSteps;
    phase6Config.enableCheckpointing = !outputFile.empty();
    phase6Config.checkpointPath = outputFile.empty() ? "checkpoint_test.bin" : outputFile;
    
    std::cout << "Experiment Configuration:\n";
    std::cout << "  Neurons: " << phase6Config.neuronCount << "\n";
    std::cout << "  Max steps: " << phase6Config.maxSteps << "\n";
    std::cout << "  Checkpointing: " << (phase6Config.enableCheckpointing ? "Yes" : "No") << "\n";
    if (!outputFile.empty()) {
        std::cout << "  Output file: " << outputFile << "\n";
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    std::cout << "\nStarting Phase 6 integration experiment...\n";
    
    // Run experiment
    auto result = experiment.run(phase6Config);
    
    std::cout << "\n=== Experiment Results ===\n";
    std::cout << "Total reward: " << result.totalReward << "\n";
    std::cout << "Avg firing rate: " << result.avgFiringRate << "\n";
    std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << "\n";
    std::cout << "Dopamine level: " << result.dopamineLevel << "\n";
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s\n";
    
    std::cout << "\n=== Integration Status ===\n";
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n";
    
    // Save results if requested
    if (!outputFile.empty()) {
        std::ofstream resultFile(outputFile);
        if (resultFile.is_open()) {
            resultFile << "NLM Phase 6 Experiment Results\n";
            resultFile << "==============================\n\n";
            resultFile << "Total reward: " << result.totalReward << "\n";
            resultFile << "Avg firing rate: " << result.avgFiringRate << "\n";
            resultFile << "Memory episodes stored: " << result.memoryEpisodesStored << "\n";
            resultFile << "Integration status:\n";
            resultFile << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
            resultFile << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
            resultFile << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
            resultFile << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
            resultFile << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
            resultFile << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n";
            resultFile.close();
            std::cout << "\nResults saved to: " << outputFile << "\n";
        }
    }
}

void runDemo(std::shared_ptr<Config> config, const std::vector<std::string>& args) {
    std::cout << "=== NLM Interactive Demo ===\n";
    std::cout << "Starting interactive demonstration of Phase 6 integration...\n\n";
    
    // Parse command line arguments
    size_t duration = 60;  // seconds
    size_t worldSize = 20;
    float speed = 1.0f;
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--duration") {
            if (i + 1 < args.size()) {
                duration = std::stoul(args[i + 1]);
                i++;
            }
        } else if (args[i] == "--world-size") {
            if (i + 1 < args.size()) {
                worldSize = std::stoul(args[i + 1]);
                i++;
            }
        } else if (args[i] == "--speed") {
            if (i + 1 < args.size()) {
                speed = std::stof(args[i + 1]);
                i++;
            }
        }
    }
    
    std::cout << "Demo Configuration:\n";
    std::cout << "  Duration: " << duration << " seconds\n";
    std::cout << "  World size: " << worldSize << "x" << worldSize << "\n";
    std::cout << "  Speed: " << speed << "x\n";
    
    // Create configuration
    auto demoConfig = std::make_shared<Config>();
    demoConfig->set("neuron_count", static_cast<int64_t>(worldSize * 5));
    demoConfig->set("region_count", static_cast<int64_t>(2));
    demoConfig->set("connection_probability", 0.1f);
    demoConfig->set("random_seed", static_cast<int64_t>(42));
    
    std::cout << "\nBuilding brain for demo...\n";
    
    try {
        // Create and initialize brain
        auto brain = std::make_shared<Brain>(demoConfig);
        if (!brain->initialize()) {
            std::cout << "ERROR: Failed to initialize brain for demo\n";
            return;
        }
        
        // Create simple world
        SimpleWorld world;
        world.initialize(worldSize, worldSize);
        
        // Create agent
        AgentBrain agent(brain);
        agent.initialize(world);
        
        // Enable subsystems for demo
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(true);
        agent.enableDevelopment(true);
        agent.enableCuriosity(true);
        
        std::cout << "Demo started! Press Ctrl+C to stop.\n\n";
        
        // Run demo for specified duration
        auto startTime = std::chrono::steady_clock::now();
        auto endTime = startTime + std::chrono::seconds(duration);
        
        size_t step = 0;
        while (std::chrono::steady_clock::now() < endTime) {
            // Update world
            world.update(speed * 0.1);  // Speed-adjusted timestep
            
            // Get observation
            SensoryPercept percept = world.observe(brain.get());
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent.decodeMotorCommand();
            
            // Apply action to world
            world.applyAction(brain.get(), cmd);
            
            // Log progress periodically
            if (step % 100 == 0) {
                std::cout << "[Step " << step << "] "
                         << "Firing: " << brain->getFiringNeuronCount() << ", "
                         << "Reward: " << world.computeReward(brain.get()) << ", "
                         << "Energy: " << world.getAgentBody().energy << "\r";
                std::cout.flush();
            }
            
            step++;
        }
        
        // Clear progress line
        std::cout << "\n\nDemo completed!\n";
        std::cout << "Total steps: " << step << "\n";
        std::cout << "Final brain status:\n";
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << "\n";
        std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << "\n";
        std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << "\n";
        
    } catch (const std::exception& e) {
        std::cout << "ERROR: Demo failed: " << e.what() << "\n";
    }
}

void monitorBrain(std::shared_ptr<Config> config, const std::vector<std::string>& args) {
    std::cout << "=== NLM Brain Monitor ===\n";
    std::cout << "Monitoring brain activity in real-time...\n\n";
    
    // Parse command line arguments
    float interval = 5.0f;  // seconds
    size_t maxSteps = 1000;
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--interval") {
            if (i + 1 < args.size()) {
                interval = std::stof(args[i + 1]);
                i++;
            }
        } else if (args[i] == "--max-steps") {
            if (i + 1 < args.size()) {
                maxSteps = std::stoul(args[i + 1]);
                i++;
            }
        }
    }
    
    std::cout << "Monitoring Configuration:\n";
    std::cout << "  Interval: " << interval << " seconds\n";
    std::cout << "  Max steps: " << maxSteps << "\n";
    
    // Create configuration
    auto monitorConfig = std::make_shared<Config>();
    monitorConfig->set("neuron_count", static_cast<int64_t>(500));
    monitorConfig->set("region_count", static_cast<int64_t>(1));
    monitorConfig->set("connection_probability", 0.1f);
    monitorConfig->set("random_seed", static_cast<int64_t>(42));
    
    std::cout << "\nBuilding brain for monitoring...\n";
    
    try {
        // Create and initialize brain
        auto brain = std::make_shared<Brain>(monitorConfig);
        if (!brain->initialize()) {
            std::cout << "ERROR: Failed to initialize brain for monitoring\n";
            return;
        }
        
        std::cout << "Monitoring started! Press Ctrl+C to stop.\n\n";
        std::cout << std::string(60, '=') << "\n";
        
        size_t step = 0;
        auto lastPrintTime = std::chrono::steady_clock::now();
        
        while (step < maxSteps) {
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastPrintTime);
            
            if (elapsed.count() >= interval) {
                // Print monitoring information
                std::cout << "[Step " << step << "] "
                         << "Time: " << (step * 0.001) << "s, "
                         << "Firing: " << brain->getFiringNeuronCount() << ", "
                         << "Total Spikes: " << brain->getTotalSpikeCount() << ", "
                         << "Avg Firing: " << brain->getAverageFiringRate() << "\n";
                
                lastPrintTime = currentTime;
            }
            
            // Run brain step
            brain->step(step, step * 0.001);
            step++;
        }
        
        std::cout << std::string(60, '=') << "\n";
        std::cout << "Monitoring completed!\n";
        std::cout << "Total steps: " << step << "\n";
        
    } catch (const std::exception& e) {
        std::cout << "ERROR: Monitoring failed: " << e.what() << "\n";
    }
}

void runBenchmark(std::shared_ptr<Config> config, const std::vector<std::string>& args) {
    std::cout << "=== NLM Performance Benchmark ===\n";
    std::cout << "Running performance benchmark...\n\n";
    
    // Parse command line arguments
    size_t iterations = 5;
    size_t neuronCount = 1000;
    bool detailed = false;
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--iterations") {
            if (i + 1 < args.size()) {
                iterations = std::stoul(args[i + 1]);
                i++;
            }
        } else if (args[i] == "--neurons") {
            if (i + 1 < args.size()) {
                neuronCount = std::stoul(args[i + 1]);
                i++;
            }
        } else if (args[i] == "--detailed") {
            detailed = true;
        }
    }
    
    std::cout << "Benchmark Configuration:\n";
    std::cout << "  Iterations: " << iterations << "\n";
    std::cout << "  Neuron count: " << neuronCount << "\n";
    std::cout << "  Detailed output: " << (detailed ? "Yes" : "No") << "\n";
    
    std::vector<double> buildTimes;
    std::vector<double> stepTimes;
    std::vector<double> spikeRates;
    
    std::cout << "\nRunning benchmark iterations...\n";
    
    for (size_t iteration = 0; iteration < iterations; ++iteration) {
        std::cout << "Iteration " << (iteration + 1) << "/" << iterations << ": ";
        
        // Create configuration for this iteration
        auto benchConfig = std::make_shared<Config>();
        benchConfig->set("neuron_count", static_cast<int64_t>(neuronCount));
        benchConfig->set("region_count", static_cast<int64_t>(1));
        benchConfig->set("connection_probability", 0.1f);
        benchConfig->set("random_seed", static_cast<int64_t>(iteration + 42));  // Different seed per iteration
        
        // Measure build time
        auto buildStart = std::chrono::high_resolution_clock::now();
        
        auto brain = std::make_shared<Brain>(benchConfig);
        bool initSuccess = brain->initialize();
        
        auto buildEnd = std::chrono::high_resolution_clock::now();
        double buildTime = std::chrono::duration<double>(buildEnd - buildStart).count();
        
        // Measure step time
        auto stepStart = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto stepEnd = std::chrono::high_resolution_clock::now();
        double stepTime = std::chrono::duration<double>(stepEnd - stepStart).count();
        double avgFiringRate = brain->getAverageFiringRate();
        
        buildTimes.push_back(buildTime);
        stepTimes.push_back(stepTime);
        spikeRates.push_back(avgFiringRate);
        
        std::cout << "Build: " << buildTime << "s, Step: " << stepTime << "s, Firing: " << avgFiringRate << "\n";
    }
    
    // Calculate statistics
    double avgBuildTime = 0.0, avgStepTime = 0.0, avgFiringRate = 0.0;
    double minBuildTime = std::numeric_limits<double>::max(), minStepTime = std::numeric_limits<double>::max();
    double maxBuildTime = 0.0, maxStepTime = 0.0;
    
    for (size_t i = 0; i < iterations; ++i) {
        avgBuildTime += buildTimes[i];
        avgStepTime += stepTimes[i];
        avgFiringRate += spikeRates[i];
        minBuildTime = std::min(minBuildTime, buildTimes[i]);
        minStepTime = std::min(minStepTime, stepTimes[i]);
        maxBuildTime = std::max(maxBuildTime, buildTimes[i]);
        maxStepTime = std::max(maxStepTime, stepTimes[i]);
    }
    
    avgBuildTime /= iterations;
    avgStepTime /= iterations;
    avgFiringRate /= iterations;
    
    std::cout << "\n=== Benchmark Results ===\n";
    std::cout << "Build Time:\n";
    std::cout << "  Average: " << avgBuildTime << "s\n";
    std::cout << "  Min: " << minBuildTime << "s\n";
    std::cout << "  Max: " << maxBuildTime << "s\n";
    std::cout << "\nStep Time:\n";
    std::cout << "  Average: " << avgStepTime << "s\n";
    std::cout << "  Min: " << minStepTime << "s\n";
    std::cout << "  Max: " << maxStepTime << "s\n";
    std::cout << "\nFiring Rate:\n";
    std::cout << "  Average: " << avgFiringRate << " Hz\n";
    
    if (detailed) {
        std::cout << "\nDetailed Results:\n";
        for (size_t i = 0; i < iterations; ++i) {
            std::cout << "  Iteration " << (i + 1) << ": "
                     << "Build: " << buildTimes[i] << "s, "
                     << "Step: " << stepTimes[i] << "s, "
                     << "Firing: " << spikeRates[i] << " Hz\n";
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize global logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Parse command line arguments
    std::vector<std::string> args = parseCLIArgs(argc, argv);
    
    // Create CLI parser
    CLIParser cli;
    
    if (args.empty()) {
        // Default behavior: run help
        cli.printHelp();
        return 0;
    }
    
    // Execute command
    std::string command = args[0];
    std::vector<std::string> commandArgs(args.begin() + 1, args.end());
    
    if (cli.execute(command, config, commandArgs)) {
        return 0;
    } else {
        std::cerr << "ERROR: Failed to execute command: " << command << "\n";
        return 1;
    }
}