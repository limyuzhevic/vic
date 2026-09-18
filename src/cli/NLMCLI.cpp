#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryInput.hpp"
#include "core/Config/Config.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace nlm;

class CLI {
public:
    CLI() : verbose_(false), quiet_(false) {}
    
    int run(int argc, char* argv[]) {
        if (argc < 2) {
            printUsage();
            return 1;
        }
        
        std::string command = argv[1];
        
        if (command == "run") {
            return handleRunCommand(argc, argv);
        } else if (command == "demo") {
            return handleDemoCommand(argc, argv);
        } else if (command == "experiment") {
            return handleExperimentCommand(argc, argv);
        } else if (command == "configure") {
            return handleConfigureCommand(argc, argv);
        } else if (command == "info") {
            return handleInfoCommand(argc, argv);
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
    }
    
private:
    bool verbose_;
    bool quiet_;
    std::shared_ptr<Config> config_;
    
    void printUsage() {
        std::cout << "NLM (Neural Learning Machine) - Command Line Interface\n\n";
        std::cout << "Usage: nlm <command> [options]\n\n";
        std::cout << "Commands:\n";
        std::cout << "  run <file.cfg>               Run brain simulation with config file\n";
        std::cout << "  demo <type> [args]           Run demonstration (phase3, phase4, phase6)\n";
        std::cout << "  experiment <steps>           Run integration experiment\n";
        std::cout << "  configure <file> [key=value] Set or get configuration values\n";
        std::cout << "  info [section]               Display system information\n";
        std::cout << "  help <command>               Show help for command\n";
        std::cout << "  version                       Show version information\n\n";
        std::cout << "Examples:\n";
        std::cout << "  nlm run myconfig.cfg --steps 1000\n";
        std::cout << "  nlm demo phase3 --episodes 10 --steps 500\n";
        std::cout << "  nlm experiment 5000\n";
        std::cout << "  nlm configure myconfig.cfg --neuron-count 2000\n";
        std::cout << "  nlm info brain\n";
    }
    
    void printHelp(const std::string& command) {
        std::cout << "Help for command: " << command << "\n\n";
        
        if (command == "run") {
            std::cout << "Run a brain simulation.\n\n";
            std::cout << "Usage: nlm run <file.cfg> [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --steps <N>                 Number of simulation steps (default: 1000)\n";
            std::cout << "  --config <file>             Configuration file (default: configs/default.cfg)\n";
            std::cout << "  --brain <file>              Save brain checkpoint after simulation\n";
            std::cout << "  --verbose                    Enable verbose output\n";
            std::cout << "  --quiet                      Suppress non-essential output\n\n";
            std::cout << "Examples:\n";
            std::cout << "  nlm run myconfig.cfg --steps 5000 --brain checkpoint.bin\n";
            std::cout << "  nlm run configs/large.cfg --verbose\n";
        } else if (command == "demo") {
            std::cout << "Run a demonstration of NLM phases.\n\n";
            std::cout << "Usage: nlm demo <phase> [options]\n\n";
            std::cout << "Phases:\n";
            std::cout << "  phase3   Phase 3 demonstration (world interaction)\n";
            std::cout << "  phase4   Phase 4 demonstration (cognition and prediction)\n";
            std::cout << "  phase6   Phase 6 demonstration (full integration)\n\n";
            std::cout << "Options:\n";
            std::cout << "  --episodes <N>             Number of episodes (default: 5)\n";
            std::cout << "  --steps <N>                Steps per episode (default: 200)\n";
            std::cout << "  --neuron-count <N>         Number of neurons (default: 500)\n";
            std::cout << "  --quiet                     Suppress non-essential output\n\n";
            std::cout << "Examples:\n";
            std::cout << "  nlm demo phase3 --episodes 10 --steps 500\n";
            std::cout << "  nlm demo phase6 --neuron-count 2000\n";
        } else if (command == "experiment") {
            std::cout << "Run a comprehensive integration experiment.\n\n";
            std::cout << "Usage: nlm experiment <steps> [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --checkpoints               Enable checkpointing\n";
            std::cout << "  --replay                    Enable memory replay\n";
            std::cout << "  --development              Enable developmental stages\n";
            std::cout << "  --output <file>             Output results to JSON file\n";
            std::cout << "  --verbose                   Enable verbose output\n\n";
            std::cout << "Examples:\n";
            std::cout << "  nlm experiment 10000 --checkpoints --output results.json\n";
            std::cout << "  nlm experiment 5000 --checkpoints --replay --development\n";
        } else if (command == "configure") {
            std::cout << "Set or get configuration values.\n\n";
            std::cout << "Usage: nlm configure <file> [key=value]\n\n";
            std::cout << "Options:\n";
            std::cout << "  -g, --get <key>             Get configuration value\n";
            std::cout << "  -s, --set <key> <value>     Set configuration value\n";
            std::cout << "  --list                      List all configuration keys\n";
            std::cout << "  --reset                     Reset to defaults\n\n";
            std::cout << "Examples:\n";
            std::cout << "  nlm configure myconfig.cfg --list\n";
            std::cout << "  nlm configure myconfig.cfg --get brain.neuron_count\n";
            std::cout << "  nlm configure myconfig.cfg --set brain.neuron_count 2000\n";
        } else if (command == "info") {
            std::cout << "Display system information.\n\n";
            std::cout << "Usage: nlm info [section]\n\n";
            std::cout << "Sections:\n";
            std::cout << "  brain      Brain system information\n";
            std::cout << "  memory     Memory systems information\n";
            std::cout << "  config     Configuration values\n";
            std::cout << "  performance Performance statistics\n";
            std::cout << "  all        All information (default)\n\n";
            std::cout << "Examples:\n";
            std::cout << "  nlm info brain\n";
            std::cout << "  nlm info config --format json\n";
        }
    }
    
    void printVersion() {
        std::cout << "NLM (Neural Learning Machine) v0.1.0\n";
        std::cout << "Phase 6: Full Integration Complete\n";
        std::cout << "C++20 with event-driven spiking neural dynamics\n";
        std::cout << "MIT License\n";
        std::cout << "\nWebsite: https://nlm.readthedocs.io\n";
        std::cout << "Repository: https://github.com/nlm-project/nlm\n";
    }
    
    int handleRunCommand(int argc, char* argv[]) {
        if (argc < 3) {
            std::cerr << "Error: Missing config file\n";
            return 1;
        }
        
        std::string configFile = "configs/default.cfg";
        std::string brainFile;
        int steps = 1000;
        
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--steps" && i + 1 < argc) {
                steps = std::stoi(argv[++i]);
            } else if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            } else if (arg == "--brain" && i + 1 < argc) {
                brainFile = argv[++i];
            } else if (arg == "--verbose") {
                verbose_ = true;
            } else if (arg == "--quiet") {
                quiet_ = true;
            } else if (arg == "--help") {
                printHelp("run");
                return 0;
            }
        }
        
        return runSimulation(configFile, steps, brainFile);
    }
    
    int handleDemoCommand(int argc, char* argv[]) {
        if (argc < 3) {
            std::cerr << "Error: Missing demo type\n";
            printHelp("demo");
            return 1;
        }
        
        std::string demoType = argv[2];
        int episodes = 5;
        int stepsPerEpisode = 200;
        int neuronCount = 500;
        
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--episodes" && i + 1 < argc) {
                episodes = std::stoi(argv[++i]);
            } else if (arg == "--steps" && i + 1 < argc) {
                stepsPerEpisode = std::stoi(argv[++i]);
            } else if (arg == "--neuron-count" && i + 1 < argc) {
                neuronCount = std::stoi(argv[++i]);
            } else if (arg == "--quiet") {
                quiet_ = true;
            } else if (arg == "--help") {
                printHelp("demo");
                return 0;
            }
        }
        
        if (demoType == "phase3") {
            return runPhase3Demo(episodes, stepsPerEpisode, neuronCount);
        } else if (demoType == "phase4") {
            return runPhase4Demo(episodes, stepsPerEpisode, neuronCount);
        } else if (demoType == "phase6") {
            return runPhase6Demo(episodes, stepsPerEpisode, neuronCount);
        } else {
            std::cerr << "Unknown demo type: " << demoType << std::endl;
            printHelp("demo");
            return 1;
        }
    }
    
    int handleExperimentCommand(int argc, char* argv[]) {
        if (argc < 3) {
            std::cerr << "Error: Missing number of steps\n";
            printHelp("experiment");
            return 1;
        }
        
        int steps = std::stoi(argv[2]);
        bool enableCheckpoints = false;
        bool enableReplay = false;
        bool enableDevelopment = false;
        std::string outputFile;
        
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--checkpoints") {
                enableCheckpoints = true;
            } else if (arg == "--replay") {
                enableReplay = true;
            } else if (arg == "--development") {
                enableDevelopment = true;
            } else if (arg == "--output" && i + 1 < argc) {
                outputFile = argv[++i];
            } else if (arg == "--verbose") {
                verbose_ = true;
            } else if (arg == "--help") {
                printHelp("experiment");
                return 0;
            }
        }
        
        return runIntegrationExperiment(steps, enableCheckpoints, enableReplay, enableDevelopment, outputFile);
    }
    
    int handleConfigureCommand(int argc, char* argv[]) {
        if (argc < 3) {
            std::cerr << "Error: Missing config file\n";
            printHelp("configure");
            return 1;
        }
        
        std::string configFile = argv[2];
        std::string getKey;
        std::string setKey;
        std::string setValue;
        bool listConfig = false;
        bool resetConfig = false;
        
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--get" && i + 1 < argc) {
                getKey = argv[++i];
            } else if (arg == "--set" && i + 1 < argc) {
                setKey = argv[++i];
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    setValue = argv[++i];
                } else {
                    std::cerr << "Error: Missing value for set command\n";
                    return 1;
                }
            } else if (arg == "--list") {
                listConfig = true;
            } else if (arg == "--reset") {
                resetConfig = true;
            } else if (arg == "--help") {
                printHelp("configure");
                return 0;
            }
        }
        
        return configureSettings(configFile, getKey, setKey, setValue, listConfig, resetConfig);
    }
    
    int handleInfoCommand(int argc, char* argv[]) {
        std::string section = "all";
        std::string format = "text";
        
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--format" && i + 1 < argc) {
                format = argv[++i];
            } else if (arg == "--help") {
                printHelp("info");
                return 0;
            }
        }
        
        if (argc >= 3) {
            section = argv[2];
        }
        
        return displayInfo(section, format);
    }
    
    int runSimulation(const std::string& configFile, int steps, const std::string& brainFile) {
        try {
            // Initialize logger
            auto logger = std::make_shared<Logger>();
            auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
            logger->addLogger(consoleLogger);
            Logger::setGlobal(logger);
            
            NLM_LOG_INFO("=== NLM Brain Simulation ===");
            NLM_LOG_INFO("Running " + std::to_string(steps) + " simulation steps");
            
            // Load configuration
            auto config = std::make_shared<Config>();
            if (!config->loadFromFile(configFile)) {
                NLM_LOG_WARN("Could not load config from " + configFile + ", using defaults");
            }
            
            // Initialize brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain");
                return 1;
            }
            
            // Run simulation
            NLM_LOG_INFO("Starting simulation...");
            for (SimulationStep step = 0; step < steps; ++step) {
                brain->step(step, static_cast<Timestamp>(step) * 0.001);
                
                if (step % 1000 == 0 && !quiet_) {
                    NLM_LOG_INFO("Step " + std::to_string(step) + ": " +
                                std::to_string(brain->getTotalSpikeCount()) + " total spikes");
                }
            }
            
            // Save checkpoint if requested
            if (!brainFile.empty()) {
                NLM_LOG_INFO("Saving checkpoint to " + brainFile);
                if (!brain->save(brainFile)) {
                    NLM_LOG_ERROR("Failed to save checkpoint");
                    return 1;
                }
            }
            
            // Final statistics
            NLM_LOG_INFO("=== Simulation Complete ===");
            NLM_LOG_INFO("Total steps: " + std::to_string(steps));
            NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
            NLM_LOG_INFO("Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
            NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
            
            return 0;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error: ") + e.what());
            return 1;
        }
    }
    
    int runPhase3Demo(int episodes, int stepsPerEpisode, int neuronCount) {
        try {
            // Initialize logger
            auto logger = std::make_shared<Logger>();
            auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
            logger->addLogger(consoleLogger);
            Logger::setGlobal(logger);
            
            NLM_LOG_INFO("=== NLM Phase 3 Demo ===");
            NLM_LOG_INFO("Running " + std::to_string(episodes) + " episodes of " + std::to_string(stepsPerEpisode) + " steps each");
            
            // Create configuration
            auto config = std::make_shared<Config>();
            config->set("neuron_count", neuronCount);
            config->set("region_count", 2);
            config->set("connection_probability", 0.1f);
            config->set("random_seed", 42u);
            config->set("simulation_timestep", 0.001);
            
            // Create world
            SimpleWorld world;
            world.configure(20.0f, 20.0f, 16, 16);
            world.setRandomSeed(42);
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Create agent brain interface
            AgentBrain agentBrain(brain);
            agentBrain.initialize(world);
            agentBrain.enableRewardModulation(true);
            agentBrain.enableStructuralPlasticity(true);
            agentBrain.enableDevelopment(true);
            agentBrain.enableCuriosity(true);
            
            // Run episodes
            float totalReward = 0.0f;
            for (int episode = 0; episode < episodes; ++episode) {
                NLM_LOG_INFO("--- Episode " + std::to_string(episode) + " ---");
                
                // Reset for new episode
                world.reset();
                agentBrain.reset();
                brain->reset();
                brain->initialize();
                
                float episodeReward = 0.0f;
                for (int step = 0; step < stepsPerEpisode; ++step) {
                    // Get sensory input
                    const SensoryPercept& percept = world.getSensoryPercept();
                    
                    // Process in brain
                    agentBrain.processSensoryInput(percept);
                    
                    // Simulate brain
                    for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                        brain->step(neuralStep);
                    }
                    
                    // Get action
                    MotorCommand cmd = agentBrain.decodeMotorCommand();
                    
                    // Apply action
                    ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                    episodeReward += result.reward;
                    
                    // Apply reward
                    agentBrain.applyRewardModulation(result.reward, 0.0f);
                    
                    // Update development
                    agentBrain.updateDevelopment(0.01);
                    
                    // Update world
                    world.update(0.01);
                    
                    if (verbose_) {
                        NLM_LOG_INFO("  Step " + std::to_string(step) +
                                    ", action=" + motorCommandToString(cmd) +
                                    ", reward=" + std::to_string(result.reward));
                    }
                }
                
                totalReward += episodeReward;
                NLM_LOG_INFO("Episode " + std::to_string(episode) + " reward: " + std::to_string(episodeReward));
            }
            
            // Final summary
            NLM_LOG_INFO("=== Phase 3 Demo Complete ===");
            NLM_LOG_INFO("Total reward: " + std::to_string(totalReward));
            NLM_LOG_INFO("Average reward per episode: " + std::to_string(totalReward / episodes));
            
            return 0;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error: ") + e.what());
            return 1;
        }
    }
    
    int runPhase4Demo(int episodes, int stepsPerEpisode, int neuronCount) {
        try {
            NLM_LOG_INFO("Phase 4 demo not yet implemented - requires implementation of Phase 4 features");
            return 0;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error: ") + e.what());
            return 1;
        }
    }
    
    int runPhase6Demo(int episodes, int stepsPerEpisode, int neuronCount) {
        try {
            NLM_LOG_INFO("=== NLM Phase 6 Demo ===");
            
            // Create experiment
            Phase6IntegratedExperiment experiment;
            
            // Run quick verification first
            NLM_LOG_INFO("--- Integration Verification ---");
            bool integrationOK = experiment.verifyIntegration();
            
            if (!integrationOK) {
                NLM_LOG_ERROR("Integration verification failed");
                return 1;
            }
            
            // Configure for demo
            Phase6Config config;
            config.neuronCount = neuronCount;
            config.maxSteps = stepsPerEpisode * episodes;
            config.enableCheckpointing = true;
            config.enableReplay = true;
            config.enableDevelopment = true;
            
            // Run full experiment
            NLM_LOG_INFO("--- Full Integration Experiment ---");
            auto result = experiment.run(config);
            
            // Display results
            NLM_LOG_INFO("=== FINAL RESULTS ===");
            NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
            NLM_LOG_INFO("Average firing rate: " + std::to_string(result.avgFiringRate));
            NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
            NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
            NLM_LOG_INFO("Integration status:");
            NLM_LOG_INFO("  Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
            NLM_LOG_INFO("  Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
            NLM_LOG_INFO("  Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"));
            NLM_LOG_INFO("  Prediction: " + std::string(result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"));
            NLM_LOG_INFO("  Development: " + std::string(result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"));
            
            return 0;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error: ") + e.what());
            return 1;
        }
    }
    
    int runIntegrationExperiment(int steps, bool enableCheckpoints, bool enableReplay, bool enableDevelopment, const std::string& outputFile) {
        try {
            NLM_LOG_INFO("=== NLM Integration Experiment ===");
            NLM_LOG_INFO("Running " + std::to_string(steps) + " steps of integration experiment");
            
            // Create experiment
            Phase6IntegratedExperiment experiment;
            
            // Configure experiment
            Phase6Config config;
            config.neuronCount = 1000;
            config.maxSteps = steps;
            config.enableCheckpointing = enableCheckpoints;
            config.enableReplay = enableReplay;
            config.enableDevelopment = enableDevelopment;
            
            // Run experiment
            auto result = experiment.run(config);
            
            // Save results if requested
            if (!outputFile.empty()) {
                NLM_LOG_INFO("Saving results to " + outputFile);
                std::ofstream file(outputFile);
                if (file.is_open()) {
                    file << "Total reward: " << result.totalReward << "\n";
                    file << "Average firing rate: " << result.avgFiringRate << "\n";
                    file << "Memory episodes stored: " << result.memoryEpisodesStored << "\n";
                    file << "Dopamine level: " << result.dopamineLevel << "\n";
                    file << "Wall clock time: " << result.totalWallClockTime << "s\n";
                    file.close();
                } else {
                    NLM_LOG_ERROR("Failed to open output file: " + outputFile);
                    return 1;
                }
            }
            
            // Display summary
            NLM_LOG_INFO("=== Experiment Complete ===");
            NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
            NLM_LOG_INFO("Average firing rate: " + std::to_string(result.avgFiringRate));
            NLM_LOG_INFO("Time elapsed: " + std::to_string(result.totalWallClockTime) + "s");
            
            return 0;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error: ") + e.what());
            return 1;
        }
    }
    
    int configureSettings(const std::string& configFile, const std::string& getKey, 
                         const std::string& setKey, const std::string& setValue,
                         bool listConfig, bool resetConfig) {
        try {
            auto config = std::make_shared<Config>();
            
            if (resetConfig) {
                NLM_LOG_INFO("Resetting configuration to defaults");
                // Reset is handled by creating new config
            }
            
            if (listConfig) {
                NLM_LOG_INFO("Configuration keys:");
                auto keys = config->getKeys();
                for (const auto& key : keys) {
                    NLM_LOG_INFO("  " + key);
                }
                return 0;
            }
            
            if (!getKey.empty()) {
                if (config->has(getKey)) {
                    if (config->isBool(getKey)) {
                        bool val = config->get<bool>(getKey);
                        NLM_LOG_INFO(getKey + ": " + (val ? "true" : "false"));
                    } else if (config->isInt64(getKey)) {
                        int64_t val = config->get<int64_t>(getKey);
                        NLM_LOG_INFO(getKey + ": " + std::to_string(val));
                    } else if (config->isFloat(getKey)) {
                        float val = config->get<float>(getKey);
                        NLM_LOG_INFO(getKey + ": " + std::to_string(val));
                    } else if (config->isString(getKey)) {
                        std::string val = config->get<std::string>(getKey);
                        NLM_LOG_INFO(getKey + ": " + val);
                    }
                } else {
                    NLM_LOG_WARN("Key not found: " + getKey);
                }
                return 0;
            }
            
            if (!setKey.empty() && !setValue.empty()) {
                // Parse value based on key pattern
                if (setKey.find('.') != std::string::npos) {
                    std::string type = "unknown";
                    if (setKey == "brain.neuron_count" || setKey == "plasticity.stdp.learning_rate") {
                        float val = std::stof(setValue);
                        config->set(setKey, val, ConfigSource::CommandLine);
                        NLM_LOG_INFO("Set " + setKey + " to " + setValue);
                    } else if (setKey == "random_seed") {
                        uint64_t val = std::stoull(setValue);
                        config->set(setKey, val, ConfigSource::CommandLine);
                        NLM_LOG_INFO("Set " + setKey + " to " + setValue);
                    } else {
                        // Try to parse as float first, then string
                        try {
                            float val = std::stof(setValue);
                            config->set(setKey, val, ConfigSource::CommandLine);
                            NLM_LOG_INFO("Set " + setKey + " to " + setValue);
                        } catch (...) {
                            config->set(setKey, setValue, ConfigSource::CommandLine);
                            NLM_LOG_INFO("Set " + setKey + " to " + setValue);
                        }
                    }
                }
            }
            
            // Save configuration
            if (!configFile.empty()) {
                if (!config->saveToFile(configFile)) {
                    NLM_LOG_ERROR("Failed to save configuration to " + configFile);
                    return 1;
                }
                NLM_LOG_INFO("Configuration saved to " + configFile);
            }
            
            return 0;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error: ") + e.what());
            return 1;
        }
    }
    
    int displayInfo(const std::string& section, const std::string& format) {
        try {
            NLM_LOG_INFO("=== System Information ===");
            
            if (section == "brain" || section == "all") {
                NLM_LOG_INFO("Brain System:");
                NLM_LOG_INFO("  Architecture: Neural Integration (Phase 6)");
                NLM_LOG_INFO("  Neuron model: Leaky Integrate-and-Fire (LIF)");
                NLM_LOG_INFO("  Plasticity: STDP, Hebbian, Structural");
                NLM_LOG_INFO("  Memory systems: Working, Episodic, Associative");
                NLM_LOG_INFO("  Neuromodulation: Dopamine, Curiosity, Novelty");
                NLM_LOG_INFO("  Development: Multi-stage maturation");
            }
            
            if (section == "memory" || section == "all") {
                NLM_LOG_INFO("Memory Systems:");
                NLM_LOG_INFO("  Working Memory: Transient active patterns (capacity: configurable)");
                NLM_LOG_INFO("  Episodic Memory: Experience storage with replay mechanism");
                NLM_LOG_INFO("  Associative Memory: Pattern completion through Hebbian learning");
                NLM_LOG_INFO("  Integration: Direct access to neural activity patterns");
            }
            
            if (section == "config" || section == "all") {
                NLM_LOG_INFO("Configuration System:");
                NLM_LOG_INFO("  Format: Hierarchical key-value pairs");
                NLM_LOG_INFO("  Sources: File, Command Line, Program Defaults");
                NLM_LOG_INFO("  Types: Boolean, Integer, Float, String");
                NLM_LOG_INFO("  Validation: Type-safe access with defaults");
            }
            
            if (section == "performance" || section == "all") {
                NLM_LOG_INFO("Performance Features:");
                NLM_LOG_INFO("  Memory pools for efficient allocation");
                NLM_LOG_INFO("  Event queue system for spike propagation");
                NLM_LOG_INFO("  SIMD vectorization support");
                NLM_LOG_INFO("  Parallel processing architecture");
                NLM_LOG_INFO("  Checkpoint system for persistence");
            }
            
            return 0;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error: ") + e.what());
            return 1;
        }
    }
    
    std::string motorCommandToString(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }
};

int main(int argc, char* argv[]) {
    CLI cli;
    return cli.run(argc, argv);
}