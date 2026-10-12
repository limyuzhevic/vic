#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <chrono>
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"
#include "SensoryPercept.hpp"

namespace nlm {
namespace CLI {

class AgentBrainCLI {
public:
    static void printHelp() {
        std::cout << "=== NLM AgentBrain Command Line Interface ===\n\n";
        std::cout << "USAGE:\n";
        std::cout << "  nlm-agent [options] [command] [arguments]\n\n";
        std::cout << "COMMANDS:\n";
        std::cout << "  run              Run agent in simulation mode\n";
        std::cout << "  test <name>      Run a specific test\n";
        std::cout << "  config <file>    Load configuration from file\n";
        std::cout << "  stats <file>     Export statistics to file\n";
        std::cout << "  benchmark N       Run benchmark for N iterations\n";
        std::cout << "  interactive       Start interactive mode\n\n";
        std::cout << "OPTIONS:\n";
        std::cout << "  --verbose, -v    Enable verbose output\n";
        std::cout << "  --debug, -d       Enable debug mode\n";
        std::cout << "  --help, -h        Show this help message\n";
        std::cout << "  --config <file>   Load configuration from file\n";
        std::cout << "  --output <file>   Set output file\n";
        std::cout << "  --world <size>    Set world dimensions (e.g., --world 20,20)\n";
        std::cout << "  --time <time>     Set simulation time step (default: 0.1)\n";
        std::cout << "  --steps <N>       Set number of simulation steps (default: 1000)\n\n";
        std::cout << "ADVANCED OPTIONS:\n";
        std::cout << "  --curiosity <level>      Set curiosity threshold (0-1)\n";
        std::cout << "  --plasticity <level>     Set plasticity modifier (0-2)\n";
        std::cout << "  --reward <factor>        Set reward factor (0-10)\n";
        std::cout << "  --no-curiosity           Disable curiosity system\n";
        std::cout << "  --no-development         Disable development system\n";
        std::cout << "  --no-plasticity          Disable structural plasticity\n";
        std::cout << "  --no-reward-modulation   Disable reward modulation\n";
        std::cout << "  --action-space <actions>  Set action space (comma-separated)\n";
        std::cout << "  --sensory-scales <scales>  Set sensory scale factors (v,t,i,p)\n";
        std::cout << "  --novelty-decay <decay>    Set novelty decay factor (0-1)\n\n";
        std::cout << "TUTORIALS:\n";
        std::cout << "  beginner     Run beginner tutorial\n";
        std::cout << "  intermediate Run intermediate tutorial\n";
        std::cout << "  advanced     Run advanced tutorial\n\n";
        std::cout << "EXAMPLES:\n";
        std::cout << "  nlm-agent run --world 30,30 --steps 500 --curiosity 0.5\n";
        std::cout << "  nlm-agent test learning --benchmark\n";
        std::cout << "  nlm-agent config my_config.json stats output.txt\n";
    }

    static bool parseArguments(int argc, char** argv, AgentBrainConfig& config, std::string& command, std::map<std::string, std::string>& args) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return false;
            }
            
            else if (arg == "run" || arg == "test" || arg == "config" || arg == "stats" || 
                     arg == "benchmark" || arg == "interactive" || arg == "beginner" || 
                     arg == "intermediate" || arg == "advanced") {
                command = arg;
            }
            
            else if (arg.find("--") == 0) {
                size_t pos = arg.find("=");
                if (pos != std::string::npos) {
                    std::string key = arg.substr(0, pos);
                    std::string value = arg.substr(pos + 1);
                    args[key] = value;
                } else {
                    args[arg] = "true";
                }
            }
            
            else if (command.empty() && i > 1) {
                command = arg;
            }
        }
        
        // Apply parsed arguments to config
        applyArgumentsToConfig(args, config);
        
        return true;
    }

    static void applyArgumentsToConfig(const std::map<std::string, std::string>& args, AgentBrainConfig& config) {
        for (const auto& pair : args) {
            const std::string& key = pair.first;
            const std::string& value = pair.second;
            
            if (key == "--verbose" || key == "-v") {
                config.cliOptions.verbose = true;
            } else if (key == "--debug" || key == "-d") {
                config.cliOptions.debug = true;
            } else if (key == "--world") {
                size_t pos = value.find(",");
                if (pos != std::string::npos) {
                    int width = std::stoi(value.substr(0, pos));
                    int height = std::stoi(value.substr(pos + 1));
                    config.worldWidth = width;
                    config.worldHeight = height;
                }
            } else if (key == "--time") {
                config.timeStep = std::stod(value);
            } else if (key == "--steps") {
                config.simulationSteps = std::stoi(value);
            } else if (key == "--output") {
                config.cliOptions.outputFile = value;
            } else if (key == "--config") {
                config.configFile = value;
            } else if (key == "--curiosity") {
                float curiosity = std::stof(value);
                config.curiosityLevel = std::clamp(curiosity, 0.0f, 1.0f);
            } else if (key == "--plasticity") {
                float plasticity = std::stof(value);
                config.plasticityModifier = std::clamp(plasticity, 0.0f, 2.0f);
            } else if (key == "--reward") {
                config.rewardFactor = std::stof(value);
            } else if (key == "--no-curiosity") {
                config.enableCuriosity = false;
            } else if (key == "--no-development") {
                config.enableDevelopment = false;
            } else if (key == "--no-plasticity") {
                config.enableStructuralPlasticity = false;
            } else if (key == "--no-reward-modulation") {
                config.enableRewardModulation = false;
            } else if (key == "--action-space") {
                std::vector<MotorCommand> actions;
                parseActionSpace(value, actions);
                config.actionSpace = actions;
            } else if (key == "--sensory-scales") {
                parseSensoryScales(value, config);
            } else if (key == "--novelty-decay") {
                config.noveltyDecay = std::stof(value);
            }
        }
    }

    static void parseActionSpace(const std::string& actionsStr, std::vector<MotorCommand>& actions) {
        size_t pos = 0;
        size_t comma;
        std::string token;
        
        while ((comma = actionsStr.find(',', pos)) != std::string::npos) {
            token = actionsStr.substr(pos, comma - pos);
            MotorCommand action = parseMotorCommand(token);
            actions.push_back(action);
            pos = comma + 1;
        }
        
        token = actionsStr.substr(pos);
        MotorCommand action = parseMotorCommand(token);
        actions.push_back(action);
    }

    static MotorCommand parseMotorCommand(const std::string& commandStr) {
        std::string cmd = commandStr;
        // Convert to uppercase for case-insensitive comparison
        for (char& c : cmd) {
            c = std::toupper(c);
        }
        
        if (cmd == "MOVEFORWARD") return MotorCommand::MoveForward;
        if (cmd == "MOVEBACKWARD") return MotorCommand::MoveBackward;
        if (cmd == "TURNLEFT") return MotorCommand::TurnLeft;
        if (cmd == "TURNRIGHT") return MotorCommand::TurnRight;
        if (cmd == "LOOKLEFT") return MotorCommand::LookLeft;
        if (cmd == "LOOKRIGHT") return MotorCommand::LookRight;
        if (cmd == "INTERACT") return MotorCommand::Interact;
        if (cmd == "WAIT") return MotorCommand::Wait;
        
        return MotorCommand::Wait; // Default
    }

    static void parseSensoryScales(const std::string& scalesStr, AgentBrainConfig& config) {
        size_t pos = 0;
        size_t comma;
        std::string token;
        
        while ((comma = scalesStr.find(',', pos)) != std::string::npos) {
            token = scalesStr.substr(pos, comma - pos);
            config.sensoryScales.push_back(std::stof(token));
            pos = comma + 1;
        }
        
        token = scalesStr.substr(pos);
        config.sensoryScales.push_back(std::stof(token));
    }

    static void printWelcomeMessage() {
        std::cout << "=== NLM AgentBrain Command Line Interface ===\n\n";
        std::cout << "Welcome to the NLM (Neural Learning Machine) AgentBrain CLI!\n";
        std::cout << "This interface provides advanced control over your artificial developmental brain.\n\n";
        std::cout << "Features available:\n";
        std::cout << "  • Configurable agent behavior\n";
        std::cout << "  • Advanced command line interface\n";
        std::cout << "  • Simulation control and monitoring\n";
        std::cout << "  • Configuration management\n";
        std::cout << "  • Statistical analysis\n";
        std::cout << "  • Benchmarking\n\n";
    }

    static void printAgentStatus(const AgentBrain& agent) {
        std::cout << "=== Agent Status ===\n";
        std::cout << "Developmental Stage: " << static_cast<int>(agent.getDevelopmentalStage()) << "\n";
        std::cout << "Neuromodulation Level: " << agent.getNeuromodulationLevel() << "\n";
        std::cout << "Curiosity Level: " << agent.getCuriosityLevel() << "\n";
        std::cout << "Novelty Level: " << agent.getNoveltyLevel() << "\n";
        std::cout << "Prediction Error: " << agent.getPredictionError() << "\n";
        std::cout << "Developmental Age: " << agent.getDevelopmentalAge() << "\n";
        std::cout << "Plasticity Modifier: " << agent.getPlasticityModifier() << "\n";
        std::cout << "Reward Modulation: " << (agent.isRewardModulationEnabled() ? "Enabled" : "Disabled") << "\n";
        std::cout << "Development: " << (agent.isDevelopmentEnabled() ? "Enabled" : "Disabled") << "\n";
        std::cout << "Structural Plasticity: " << (agent.isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << "\n";
        std::cout << "Curiosity: " << (agent.isCuriosityEnabled() ? "Enabled" : "Disabled") << "\n\n";
    }

    static bool runSimulation(AgentBrainConfig& config) {
        std::cout << "=== Running NLM Agent Simulation ===\n\n";
        
        // Create brain
        auto brainConfig = AgentBrainConfig();
        auto brain = std::make_shared<Brain>(brainConfig);
        brain->initialize();
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(config.worldWidth, config.worldHeight, 8, 8);
        world->reset();
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain, config);
        agent->initialize(*world);
        
        // Configure agent with parsed options
        agent->setCuriosityParameters(config.curiosityLevel, config.curiosityLevel * 0.3f);
        agent->setSensoryScaleFactors(config.sensoryScales[0], config.sensoryScales[1], 
                                     config.sensoryScales[2], config.sensoryScales[3]);
        
        if (!config.actionSpace.empty()) {
            agent->setActionSpace(config.actionSpace);
        }
        
        // Run simulation
        std::cout << "Starting simulation with " << config.simulationSteps << " steps...\n";
        
        for (int step = 0; step < config.simulationSteps; ++step) {
            // Update world
            world->update(config.timeStep);
            
            // Get sensory percept
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * config.timeStep);
            
            // Decode motor command
            MotorCommand action = agent->decodeMotorCommand();
            
            // Apply motor command
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            agent->applyRewardModulation(1.0f, 0.5f); // Simple reward system
            
            // Update development
            agent->updateDevelopment(config.timeStep);
            
            // Print progress every 100 steps
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() << " neurons firing\n";
            }
        }
        
        std::cout << "\n=== Simulation Complete ===\n";
        return true;
    }

    static bool runTest(AgentBrainConfig& config, const std::string& testName) {
        std::cout << "=== Running Test: " << testName << " ===\n\n";
        
        // Create brain
        auto brainConfig = AgentBrainConfig();
        auto brain = std::make_shared<Brain>(brainConfig);
        brain->initialize();
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain, config);
        agent->initialize(*world);
        
        if (testName == "learning") {
            return runLearningTest(agent, world, config);
        } else if (testName == "connectivity") {
            return runConnectivityTest(agent, world, config);
        } else if (testName == "plasticity") {
            return runPlasticityTest(agent, world, config);
        } else {
            std::cout << "Unknown test: " << testName << "\n";
            return false;
        }
    }

    static bool runLearningTest(const std::shared_ptr<AgentBrain>& agent, 
                               const std::shared_ptr<SimpleWorld>& world,
                               const AgentBrainConfig& config) {
        std::cout << "Running learning test...\n";
        
        // Enable learning features
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        for (int step = 0; step < 500; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory percept
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            agent->getBrain()->step(step, step * 0.1);
            
            // Decode motor command
            MotorCommand action = agent->decodeMotorCommand();
            
            // Apply motor command
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            agent->applyRewardModulation(1.0f, 0.5f);
            
            // Update development
            agent->updateDevelopment(0.1);
        }
        
        std::cout << "Learning test completed.\n";
        return true;
    }

    static bool runConnectivityTest(const std::shared_ptr<AgentBrain>& agent,
                                    const std::shared_ptr<SimpleWorld>& world,
                                    const AgentBrainConfig& config) {
        std::cout << "Running connectivity test...\n";
        
        // Test basic neural connectivity
        auto brain = agent->getBrain();
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into neurons
        auto neurons = brain->getTotalNeuronCount() / 4;
        for (size_t i = 0; i < std::min(neurons, static_cast<size_t>(10)); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        // Run a few steps
        for (size_t step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        std::cout << "Connectivity test: " << spikes << " spikes generated\n";
        
        if (spikes > 0) {
            std::cout << "✓ Connectivity test passed!\n";
        } else {
            std::cout << "! No spikes detected - checking neuron parameters...\n";
        }
        
        return true;
    }

    static bool runPlasticityTest(const std::shared_ptr<AgentBrain>& agent,
                                 const std::shared_ptr<SimpleWorld>& world,
                                 const AgentBrainConfig& config) {
        std::cout << "Running plasticity test...\n";
        
        // Enable plasticity
        agent->enableStructuralPlasticity(true);
        
        auto brain = agent->getBrain();
        
        // Record initial state
        size_t initialSynapses = brain->getTotalSynapseCount();
        std::cout << "Initial synapse count: " << initialSynapses << "\n";
        
        // Apply repeated input pattern to stimulate learning
        for (size_t step = 0; step < 200; ++step) {
            // Create input pattern
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            
            // Step brain
            brain->step(step, step * 0.01);
            
            // Print progress every 50 steps
            if (step % 50 == 0) {
                std::cout << "Step " << step << ": " << brain->getTotalSpikeCount() << " total spikes\n";
            }
        }
        
        // Record final state
        size_t finalSynapses = brain->getTotalSynapseCount();
        std::cout << "Final synapse count: " << finalSynapses << "\n";
        
        // Check if plasticity occurred
        if (finalSynapses != initialSynapses) {
            std::cout << "✓ Plasticity test passed! Synapses changed.\n";
        } else {
            std::cout << "! No synaptic change detected.\n";
        }
        
        return true;
    }

    static bool exportStats(AgentBrainConfig& config, const std::string& outputFile) {
        std::cout << "=== Exporting Statistics ===\n\n";
        
        // Create brain
        auto brainConfig = AgentBrainConfig();
        auto brain = std::make_shared<Brain>(brainConfig);
        brain->initialize();
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain, config);
        agent->initialize(*world);
        
        // Run a quick simulation
        for (int step = 0; step < 100; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step, step * 0.1);
            MotorCommand action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            agent->applyRewardModulation(1.0f, 0.5f);
            agent->updateDevelopment(0.1);
        }
        
        // Create statistics
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open output file: " << outputFile << "\n";
            return false;
        }
        
        file << "=== NLM AgentBrain Statistics ===\n\n";
        file << "Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        file << "Total Synapses: " << brain->getTotalSynapseCount() << "\n";
        file << "Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        file << "Total Spikes: " << brain->getTotalSpikeCount() << "\n";
        file << "Average Firing Rate: " << brain->getAverageFiringRate() << "\n";
        file << "Development Stage: " << static_cast<int>(agent->getDevelopmentalStage()) << "\n";
        file << "Neuromodulation Level: " << agent->getNeuromodulationLevel() << "\n";
        file << "Curiosity Level: " << agent->getCuriosityLevel() << "\n";
        file << "Novelty Level: " << agent->getNoveltyLevel() << "\n";
        file << "Prediction Error: " << agent->getPredictionError() << "\n";
        file << "Developmental Age: " << agent->getDevelopmentalAge() << "\n";
        file << "Plasticity Modifier: " << agent->getPlasticityModifier() << "\n";
        
        file.close();
        
        std::cout << "Statistics exported to: " << outputFile << "\n";
        return true;
    }

    static bool importConfig(AgentBrainConfig& config, const std::string& configFile) {
        std::cout << "=== Loading Configuration ===\n\n";
        
        std::ifstream file(configFile);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open config file: " << configFile << "\n";
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                config.configPairs[key] = value;
            }
        }
        
        file.close();
        std::cout << "Configuration loaded from: " << configFile << "\n";
        return true;
    }

    static void benchmarkAgent(AgentBrainConfig& config, int iterations) {
        std::cout << "=== Benchmarking NLM AgentBrain ===\n\n";
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
            // Create brain
            auto brainConfig = AgentBrainConfig();
            auto brain = std::make_shared<Brain>(brainConfig);
            brain->initialize();
            
            // Create world
            auto world = std::make_shared<SimpleWorld>();
            world->configure(10, 10, 8, 8);
            world->reset();
            
            // Create agent
            auto agent = std::make_shared<AgentBrain>(brain, config);
            agent->initialize(*world);
            
            // Run quick simulation
            for (int step = 0; step < 50; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step, step * 0.1);
                MotorCommand action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "=== Benchmark Results ===\n";
        std::cout << "Iterations: " << iterations << "\n";
        std::cout << "Total Time: " << duration.count() << " ms\n";
        std::cout << "Average Time per Iteration: " << (duration.count() / iterations) << " ms\n";
        std::cout << "Iterations per Second: " << (iterations * 1000 / duration.count()) << "\n";
    }

    static void runInteractiveMode(AgentBrainConfig& config) {
        std::cout << "=== Interactive Mode ===\n\n";
        
        // Create brain
        auto brainConfig = AgentBrainConfig();
        auto brain = std::make_shared<Brain>(brainConfig);
        brain->initialize();
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(30, 30, 8, 8);
        world->reset();
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain, config);
        agent->initialize(*world);
        
        std::string command;
        std::cout << "Type 'help' for available commands, 'quit' to exit\n\n";
        
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, command);
            
            if (command == "help" || command == "?") {
                printHelp();
            } else if (command == "quit" || command == "exit") {
                break;
            } else if (command == "status") {
                printAgentStatus(*agent);
            } else if (command == "reset") {
                agent->reset();
                std::cout << "Agent reset.\n";
            } else if (command == "step") {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(0, 0.1);
                MotorCommand action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                std::cout << "Step completed.\n";
            } else if (command.substr(0, 5) == "set ") {
                processSetCommand(command.substr(5), *agent);
            } else if (command.substr(0, 4) == "run ") {
                runInteractiveSimulation(command.substr(4), *agent, *world);
            } else {
                std::cout << "Unknown command: " << command << "\n";
                std::cout << "Type 'help' for available commands.\n";
            }
        }
        
        std::cout << "Interactive mode exited.\n";
    }

    static void processSetCommand(const std::string& setCommand, AgentBrain& agent) {
        size_t pos = setCommand.find(" ");
        if (pos == std::string::npos) {
            std::cout << "Invalid set command. Usage: set <property> <value>\n";
            return;
        }
        
        std::string property = setCommand.substr(0, pos);
        std::string value = setCommand.substr(pos + 1);
        
        if (property == "curiosity") {
            float curiosity = std::stof(value);
            agent.setCuriosityParameters(curiosity, curiosity * 0.3f);
            std::cout << "Curiosity threshold set to " << curiosity << "\n";
        } else if (property == "plasticity") {
            float plasticity = std::stof(value);
            agent.enableDevelopment(true);
            std::cout << "Plasticity modifier set to " << plasticity << "\n";
        } else if (property == "reward") {
            float reward = std::stof(value);
            std::cout << "Reward factor set to " << reward << "\n";
        } else if (property == "curiosity-enabled") {
            bool enabled = (value == "true" || value == "1" || value == "yes");
            agent.enableCuriosity(enabled);
            std::cout << "Curiosity " << (enabled ? "enabled" : "disabled") << "\n";
        } else {
            std::cout << "Unknown property: " << property << "\n";
        }
    }

    static void runInteractiveSimulation(const std::string& command, AgentBrain& agent, SimpleWorld& world) {
        std::cout << "=== Interactive Simulation ===\n\n";
        
        int steps = 100;
        size_t pos = command.find(" ");
        if (pos != std::string::npos) {
            steps = std::stoi(command.substr(pos + 1));
        }
        
        for (int step = 0; step < steps; ++step) {
            world.update(0.1);
            auto percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            agent.getBrain()->step(step, step * 0.1);
            MotorCommand action = agent.decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
            agent.applyRewardModulation(1.0f, 0.5f);
            agent.updateDevelopment(0.1);
            
            if (step % 20 == 0) {
                std::cout << "Step " << step << ": " << agent.getBrain()->getFiringNeuronCount() << " neurons firing\n";
            }
        }
        
        std::cout << "Simulation completed.\n";
    }
};

} // namespace CLI
} // namespace nlm
