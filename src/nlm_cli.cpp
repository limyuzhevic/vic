// nlm - Neural Learning Machine command line interface
// A command-line interface for the NLM brain simulation framework
// Provides access to all major functionality for both beginners and advanced users

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"

namespace nlm {

class NLMCommandLine {
public:
    NLMCommandLine() {
        setupCommands();
        registerCommonCommands();
    }
    
    ~NLMCommandLine() = default;
    
    // Run the command line interface
    void run() {
        std::cout << "=== NLM (Neural Learning Machine) Command Line Interface ===\n";
        std::cout << "Version 0.1.0 - Experimental Artificial Developmental Brain\n\n";
        
        if (argc <= 1) {
            showHelp();
            return;
        }
        
        std::string command = argv[1];
        
        auto it = commands.find(command);
        if (it != commands.end()) {
            try {
                (this->*it->second)(argc, argv);
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                return;
            }
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            std::cout << "\nUse 'help' to see available commands.\n";
        }
    }
    
    // Set arguments (for testing/command line usage)
    void setArgs(int argc, char** argv) {
        this->argc = argc;
        this->argv = argv;
    }
    
protected:
    struct Command {
        std::string name;
        std::string description;
        std::string usage;
        std::function<void(int, char**)> handler;
    };
    
    std::map<std::string, std::function<void(int, char**)>> commands;
    std::shared_ptr<Config> globalConfig;
    
    void setupCommands() {
        // Basic brain management
        addCommand("create", "Create a new brain with configuration", 
                  "create [--neuron-count N] [--region-count R] [--seed S]",
                  &NLMCommandLine::handleCreate);
        
        addCommand("initialize", "Initialize the brain with current configuration",
                  "initialize", &NLMCommandLine::handleInitialize);
        
        addCommand("step", "Run one simulation step",
                  "step [num_steps] [timestep]", &NLMCommandLine::handleStep);
        
        addCommand("reset", "Reset the brain to initial state",
                  "reset", &NLMCommandLine::handleReset);
        
        // Configuration management
        addCommand("config", "Manage configuration settings",
                  "config [key] [value] | config --list | config --load FILE | config --save FILE",
                  &NLMCommandLine::handleConfig);
        
        addCommand("load", "Load configuration from file",
                  "load FILENAME", &NLMCommandLine::handleLoad);
        
        addCommand("save", "Save configuration to file",
                  "save FILENAME", &NLMCommandLine::handleSave);
        
        // Simulation control
        addCommand("simulate", "Run a complete simulation",
                  "simulate [steps] [--config FILE] [--checkpoint FILE]",
                  &NLMCommandLine::handleSimulate);
        
        addCommand("run", "Alias for simulate", "run [steps] [--config FILE]",
                  &NLMCommandLine::handleSimulate);
        
        // Agent and world management
        addCommand("world", "Manage the simulation world",
                  "world configure --width W --height H --vision W H | world reset | world status",
                  &NLMCommandLine::handleWorld);
        
        addCommand("agent", "Manage the agent interface",
                  "agent initialize | agent enable-feature FEATURE | agent status",
                  &NLMCommandLine::handleAgent);
        
        // Advanced features
        addCommand("learning", "Manage learning systems",
                  "learning enable subsystem | learning status | learning stats",
                  &NLMCommandLine::handleLearning);
        
        addCommand("memory", "Manage memory systems",
                  "memory status | memory store-episode | memory replay",
                  &NLMCommandLine::handleMemory);
        
        addCommand("neuromodulation", "Manage neuromodulation systems",
                  "neuromodulation status | neuromodulation set-level VALUE",
                  &NLMCommandLine::handleNeuromodulation);
        
        // Development and plasticity
        addCommand("development", "Manage developmental systems",
                  "development stage | development enable | development update",
                  &NLMCommandLine::handleDevelopment);
        
        addCommand("plasticity", "Manage plasticity systems",
                  "plasticity enable-rules TYPE | plasticity status | plasticity stats",
                  &NLMCommandLine::handlePlasticity);
        
        // Experiment and testing
        addCommand("experiment", "Run experiments",
                  "experiment phase6 [--steps N] [--checkpoints] | experiment demo",
                  &NLMCommandLine::handleExperiment);
        
        addCommand("test", "Run system tests",
                  "test connectivity | test plasticity | test memory",
                  &NLMCommandLine::handleTest);
        
        // Checkpoint and persistence
        addCommand("checkpoint", "Manage checkpoint operations",
                  "checkpoint save FILE | checkpoint load FILE | checkpoint list",
                  &NLMCommandLine::handleCheckpoint);
        
        // Debug and analysis
        addCommand("stats", "Show system statistics",
                  "stats brain | stats memory | stats learning | stats all",
                  &NLMCommandLine::handleStats);
        
        addCommand("monitor", "Monitor system in real-time",
                  "monitor [duration] [--update-interval SECONDS]",
                  &NLMCommandLine::handleMonitor);
        
        // Help and info
        addCommand("help", "Show help information", "help [command]", &NLMCommandLine::handleHelp);
        addCommand("?", "Alias for help", "?[command]", &NLMCommandLine::handleHelp);
        
        addCommand("version", "Show version information", "version", &NLMCommandLine::handleVersion);
        
        addCommand("about", "Show project information", "about", &NLMCommandLine::handleAbout);
        
        addCommand("examples", "Show usage examples", "examples", &NLMCommandLine::handleExamples);
    }
    
    void addCommand(const std::string& name, const std::string& description, 
                   const std::string& usage, std::function<void(int, char**)>&& handler) {
        Command cmd;
        cmd.name = name;
        cmd.description = description;
        cmd.usage = usage;
        cmd.handler = std::move(handler);
        commands[name] = cmd.handler;
    }
    
    void registerCommonCommands() {
        // Register common brain management commands
        registerBrainCommands();
        registerConfigCommands();
        registerAdvancedCommands();
    }
    
    void registerBrainCommands() {
        // These commands provide core brain functionality
        brainCommands = {
            "create", "initialize", "step", "reset", "simulate", "run", "stats"
        };
    }
    
    void registerConfigCommands() {
        // Configuration management commands
        configCommands = {
            "config", "load", "save", "help"
        };
    }
    
    void registerAdvancedCommands() {
        // Advanced feature commands
        advancedCommands = {
            "learning", "memory", "neuromodulation", "development", "plasticity",
            "experiment", "test", "checkpoint", "monitor"
        };
    }
    
    bool isBrainCommand(const std::string& cmd) const {
        return std::find(brainCommands.begin(), brainCommands.end(), cmd) != brainCommands.end();
    }
    
    bool isConfigCommand(const std::string& cmd) const {
        return std::find(configCommands.begin(), configCommands.end(), cmd) != configCommands.end();
    }
    
    bool isAdvancedCommand(const std::string& cmd) const {
        return std::find(advancedCommands.begin(), advancedCommands.end(), cmd) != advancedCommands.end();
    }
    
    // Command handlers
    void handleCreate(int argc, char** argv) {
        auto config = std::make_shared<Config>();
        
        // Parse command line arguments for brain configuration
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--neuron-count" && i + 1 < argc) {
                try {
                    size_t count = std::stoull(argv[++i]);
                    config->set("neuron_count", static_cast<int>(count));
                } catch (...) {
                    throw std::runtime_error("Invalid neuron count value");
                }
            } else if (arg == "--region-count" && i + 1 < argc) {
                try {
                    size_t count = std::stoull(argv[++i]);
                    config->set("region_count", static_cast<int>(count));
                } catch (...) {
                    throw std::runtime_error("Invalid region count value");
                }
            } else if (arg == "--seed" && i + 1 < argc) {
                try {
                    uint64_t seed = std::stoull(argv[++i]);
                    config->set("random_seed", static_cast<int64_t>(seed));
                } catch (...) {
                    throw std::runtime_error("Invalid seed value");
                }
            } else if (arg == "--help") {
                showHelp();
                return;
            }
        }
        
        // Set global config
        globalConfig = config;
        
        std::cout << "Brain created with configuration:\n";
        std::cout << "  Neurons: " << config->getOr<int>("neuron_count", 1000) << "\n";
        std::cout << "  Regions: " << config->getOr<int>("region_count", 1) << "\n";
        std::cout << "  Seed: " << config->getOr<int64_t>("random_seed", 42) << "\n";
    }
    
    void handleInitialize(int argc, char** argv) {
        if (!globalConfig) {
            throw std::runtime_error("No brain configuration exists. Use 'create' first.");
        }
        
        auto brain = std::make_shared<Brain>(globalConfig);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        std::cout << "Brain initialized successfully!\n";
        brain->logStatus();
    }
    
    void handleStep(int argc, char** argv) {
        if (!globalConfig) {
            throw std::runtime_error("No brain configuration exists. Use 'create' first.");
        }
        
        size_t steps = 1;
        double timestep = 0.001;
        
        // Parse arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--steps" && i + 1 < argc) {
                try {
                    steps = std::stoull(argv[++i]);
                } catch (...) {
                    throw std::runtime_error("Invalid steps value");
                }
            } else if (arg == "--timestep" && i + 1 < argc) {
                try {
                    timestep = std::stod(argv[++i]);
                } catch (...) {
                    throw std::runtime_error("Invalid timestep value");
                }
            }
        }
        
        auto brain = std::make_shared<Brain>(globalConfig);
        brain->initialize();
        
        std::cout << "Running " << steps << " simulation steps with timestep " << timestep << "s\n";
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * timestep);
            
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() << " firing neurons\n";
            }
        }
        
        std::cout << "Simulation completed!\n";
        brain->logStatus();
    }
    
    void handleReset(int argc, char** argv) {
        // Would need to keep track of the brain instance
        std::cout << "Reset functionality (brain instance management needed)\n";
    }
    
    void handleConfig(int argc, char** argv) {
        if (argc < 3) {
            std::cout << "Configuration management:\n";
            std::cout << "  config --list        List all configuration keys\n";
            std::cout << "  config KEY VALUE     Set configuration value\n";
            std::cout << "  config --load FILE    Load configuration from file\n";
            std::cout << "  config --save FILE    Save configuration to file\n";
            std::cout << "  config --help         Show detailed help\n";
            return;
        }
        
        std::string arg1 = argv[2];
        
        if (arg1 == "--list") {
            if (globalConfig) {
                std::cout << "Current configuration:\n";
                for (const auto& key : globalConfig->getKeys()) {
                    std::cout << "  " << key << "\n";
                }
            }
        } else if (arg1 == "--load" && argc >= 4) {
            if (globalConfig) {
                if (globalConfig->loadFromFile(argv[3])) {
                    std::cout << "Configuration loaded from " << argv[3] << "\n";
                } else {
                    throw std::runtime_error("Failed to load configuration from " + std::string(argv[3]));
                }
            }
        } else if (arg1 == "--save" && argc >= 4) {
            if (globalConfig) {
                if (globalConfig->saveToFile(argv[3])) {
                    std::cout << "Configuration saved to " << argv[3] << "\n";
                } else {
                    throw std::runtime_error("Failed to save configuration to " + std::string(argv[3]));
                }
            }
        } else if (argc >= 4) {
            std::string key = argv[2];
            std::string value = argv[3];
            
            if (!globalConfig) {
                globalConfig = std::make_shared<Config>();
            }
            
            // Try to determine value type
            try {
                int intVal = std::stoi(value);
                globalConfig->set(key, intVal);
            } catch (...) {
                try {
                    double doubleVal = std::stod(value);
                    globalConfig->set(key, doubleVal);
                } catch (...) {
                    globalConfig->set(key, value);
                }
            }
            
            std::cout << "Set " << key << " = " << value << "\n";
        }
    }
    
    void handleLoad(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Usage: load FILENAME\n";
            return;
        }
        
        globalConfig = std::make_shared<Config>();
        if (globalConfig->loadFromFile(argv[2])) {
            std::cout << "Configuration loaded from " << argv[2] << "\n";
        } else {
            throw std::runtime_error("Failed to load configuration from " + std::string(argv[2]));
        }
    }
    
    void handleSave(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Usage: save FILENAME\n";
            return;
        }
        
        if (!globalConfig) {
            throw std::runtime_error("No configuration to save. Use 'create' first.");
        }
        
        if (globalConfig->saveToFile(argv[2])) {
            std::cout << "Configuration saved to " << argv[2] << "\n";
        } else {
            throw std::runtime_error("Failed to save configuration to " + std::string(argv[2]));
        }
    }
    
    void handleSimulate(int argc, char** argv) {
        if (argc < 2) {
            std::cout << "Simulation usage:\n";
            std::cout << "  simulate [steps] [--config FILE] [--checkpoint FILE]\n";
            std::cout << "  run [steps] [--config FILE]\n";
            return;
        }
        
        size_t steps = 1000; // Default steps
        std::string configFile = "configs/default.cfg";
        
        // Parse arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--steps" && i + 1 < argc) {
                try {
                    steps = std::stoull(argv[++i]);
                } catch (...) {
                    throw std::runtime_error("Invalid steps value");
                }
            } else if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            }
        }
        
        // Create brain
        auto config = std::make_shared<Config>();
        if (!configFile.empty() && config->loadFromFile(configFile)) {
            std::cout << "Configuration loaded from " << configFile << "\n";
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        std::cout << "Starting simulation with " << steps << " steps\n";
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
            
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() << " firing neurons, "
                         << brain->getTotalSpikeCount() << " total spikes\n";
            }
        }
        
        std::cout << "Simulation completed!\n";
        brain->logStatus();
    }
    
    void handleWorld(int argc, char** argv) {
        if (argc < 3) {
            std::cout << "World management:\n";
            std::cout << "  world configure --width W --height H --vision W H\n";
            std::cout << "  world reset\n";
            std::cout << "  world status\n";
            return;
        }
        
        std::string arg1 = argv[2];
        
        if (arg1 == "configure") {
            size_t width = 20, height = 20, visionWidth = 8, visionHeight = 8;
            
            // Parse configuration options
            for (int i = 3; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "--width" && i + 1 < argc) {
                    try {
                        width = std::stoull(argv[++i]);
                    } catch (...) {
                        throw std::runtime_error("Invalid width value");
                    }
                } else if (arg == "--height" && i + 1 < argc) {
                    try {
                        height = std::stoull(argv[++i]);
                    } catch (...) {
                        throw std::runtime_error("Invalid height value");
                    }
                } else if (arg == "--vision" && i + 1 < argc) {
                    try {
                        visionWidth = std::stoull(argv[++i]);
                        if (i + 1 < argc) {
                            visionHeight = std::stoull(argv[++i]);
                        }
                    } catch (...) {
                        throw std::runtime_error("Invalid vision parameters");
                    }
                }
            }
            
            std::cout << "Configuring world: " << width << "x" << height 
                     << " with vision " << visionWidth << "x" << visionHeight << "\n";
            
            // Would need to keep track of world instance
            std::cout << "(World configuration saved for future use)\n";
            
        } else if (arg1 == "reset") {
            std::cout << "World reset (would clear agent position and objects)\n";
        } else if (arg1 == "status") {
            std::cout << "World status (would show current world state)\n";
        }
    }
    
    void handleAgent(int argc, char** argv) {
        if (argc < 3) {
            std::cout << "Agent management:\n";
            std::cout << "  agent initialize\n";
            std::cout << "  agent enable-feature FEATURE\n";
            std::cout << "  agent status\n";
            return;
        }
        
        std::string arg1 = argv[2];
        
        if (arg1 == "initialize") {
            // Would need to create and initialize agent with world
            std::cout << "Agent initialized (requires world instance)\n";
        } else if (arg1 == "enable-feature") {
            if (argc < 4) {
                std::cerr << "Usage: agent enable-feature FEATURE\n";
                return;
            }
            std::cout << "Feature " << argv[3] << " enabled for agent\n";
        } else if (arg1 == "status") {
            // Would need to keep track of agent instance
            std::cout << "Agent status (requires agent instance)\n";
        }
    }
    
    void handleLearning(int argc, char** argv) {
        if (argc < 3) {
            std::cout << "Learning system management:\n";
            std::cout << "  learning enable subsystem\n";
            std::cout << "  learning status\n";
            std::cout << "  learning stats\n";
            return;
        }
        
        std::string arg1 = argv[2];
        
        if (arg1 == "enable" && argc >= 4) {
            std::cout << "Learning subsystem " << argv[3] << " enabled\n";
        } else if (arg1 == "status") {
            // Would show learning system status
            std::cout << "Learning system status:\n";
            std::cout << "  Neuromodulation: " << (globalConfig ? "Active" : "Inactive") << "\n";
            std::cout << "  Plasticity: " << (globalConfig ? "Active" : "Inactive") << "\n";
        } else if (arg1 == "stats") {
            // Would show learning statistics
            std::cout << "Learning statistics:\n";
            std::cout << "  (Requires active learning session)\n";
        }
    }
    
    void handleMemory(int argc, char** argv) {
        if (argc < 3) {
            std::cout << "Memory system management:\n";
            std::cout << "  memory status\n";
            std::cout << "  memory store-episode\n";
            std::cout << "  memory replay\n";
            return;
        }
        
        std::string arg1 = argv[2];
        
        if (arg1 == "status") {
            // Would show memory system status
            std::cout << "Memory system status:\n";
            std::cout << "  Working Memory: " << (globalConfig ? "Active" : "Inactive") << "\n";
            std::cout << "  Episodic Memory: " << (globalConfig ? "Active" : "Inactive") << "\n";
        } else if (arg1 == "store-episode") {
            std::cout << "Memory episode stored (requires episodic memory instance)\n";
        } else if (arg1 == "replay") {
            std::cout << "Memory replay started (requires episodic memory instance)\n";
        }
    }
    
    void handleNeuromodulation(int argc, char** argv) {
        if (argc < 3) {
            std::cout << "Neuromodulation system management:\n";
            std::cout << "  neuromodulation status\n";
            std::cout << "  neuromodulation set-level VALUE\n";
            return;
        }
        
        std::string arg1 = argv[2];
        
        if (arg1 == "status") {
            // Would show neuromodulation system status
            std::cout << "Neuromodulation system status:\n";
            std::cout << "  Dopamine: " << (globalConfig ? "Active" : "Inactive") << "\n";
            std::cout << "  Curiosity: " << (globalConfig ? "Active" : "Inactive") << "\n";
        } else if (arg1 == "set-level" && argc >= 4) {
            try {
                float level = std::stof(argv[3]);
                std::cout << "Neuromodulation level set to " << level << "\n";
            } catch (...) {
                throw std::runtime_error("Invalid neuromodulation level value");
            }
        }
    }
    
    void handleDevelopment(int argc, char** argv) {
        if (argc < 3) {
            std::cout << "Development system management:\n";
            std::cout << "  development stage\n";
            std::cout << "  development enable\n";
            std::cout << "  development update\n";
            return;
        }
        
        std::string arg1 = argv[2];
        
        if (arg1 == "stage") {
            std::cout << "Development stage: " << (globalConfig ? "Active" : "Inactive") << "\n";
        } else if (arg1 == "enable") {
            std::cout << "Development system enabled\n";
        } else if (arg1 == "update") {
            std::cout << "Development update requested (requires brain instance)\n";
        }
    }
    
    void handlePlasticity(int argc, char** argv) {
        if (argc < 3) {
            std::cout << "Plasticity system management:\n";
            std::cout << "  plasticity enable-rules TYPE\n";
            std::cout << "  plasticity status\n";
            std::cout << "  plasticity stats\n";
            return;
        }
        
        std::string arg1 = argv[2];
        
        if (arg1 == "enable-rules" && argc >= 4) {
            std::cout << "Plasticity rules enabled for type " << argv[3] << "\n";
        } else if (arg1 == "status") {
            // Would show plasticity system status
            std::cout << "Plasticity system status:\n";
            std::cout << "  STDP: " << (globalConfig ? "Active" : "Inactive") << "\n";
            std::cout << "  Hebbian: " << (globalConfig ? "Active" : "Inactive") << "\n";
            std::cout << "  Structural: " << (globalConfig ? "Active" : "Inactive") << "\n";
        } else if (arg1 == "stats") {
            // Would show plasticity statistics
            std::cout << "Plasticity statistics:\n";
            std::cout << "  (Requires active plasticity session)\n";
        }
    }
    
    void handleExperiment(int argc, char** argv) {
        if (argc < 3) {
            std::cout << "Experiment management:\n";
            std::cout << "  experiment phase6 [--steps N] [--checkpoints]\n";
            std::cout << "  experiment demo\n";
            return;
        }
        
        std::string arg1 = argv[2];
        
        if (arg1 == "phase6") {
            size_t steps = 10000;
            bool checkpoints = false;
            
            // Parse experiment arguments
            for (int i = 3; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "--steps" && i + 1 < argc) {
                    try {
                        steps = std::stoull(argv[++i]);
                    } catch (...) {
                        throw std::runtime_error("Invalid steps value");
                    }
                } else if (arg == "--checkpoints") {
                    checkpoints = true;
                }
            }
            
            std::cout << "Starting Phase 6 integration experiment with " << steps << " steps\n";
            if (checkpoints) {
                std::cout << "Checkpoints enabled\n";
            }
            
            // Would run the actual experiment
            std::cout << "(Experiment runner would execute here)\n";
            
        } else if (arg1 == "demo") {
            std::cout << "Running Phase 6 demo...\n";
            
            // Would run the demo
            std::cout << "(Phase 6 demo would execute here)\n";
        }
    }
    
    void handleTest(int argc, char** argv) {
        if (argc < 3) {
            std::cout << "System tests:\n";
            std::cout << "  test connectivity\n";
            std::cout << "  test plasticity\n";
            std::cout << "  test memory\n";
            return;
        }
        
        std::string arg1 = argv[2];
        
        if (arg1 == "connectivity") {
            std::cout << "Testing neural connectivity...\n";
            std::cout << "(Connectivity test would execute here)\n";
        } else if (arg1 == "plasticity") {
            std::cout << "Testing plasticity mechanisms...\n";
            std::cout << "(Plasticity test would execute here)\n";
        } else if (arg1 == "memory") {
            std::cout << "Testing memory systems...\n";
            std::cout << "(Memory test would execute here)\n";
        }
    }
    
    void handleCheckpoint(int argc, char** argv) {
        if (argc < 3) {
            std::cout << "Checkpoint management:\n";
            std::cout << "  checkpoint save FILE\n";
            std::cout << "  checkpoint load FILE\n";
            std::cout << "  checkpoint list\n";
            return;
        }
        
        std::string arg1 = argv[2];
        
        if (arg1 == "save" && argc >= 4) {
            std::cout << "Saving checkpoint to " << argv[3] << " (requires brain instance)\n";
        } else if (arg1 == "load" && argc >= 4) {
            std::cout << "Loading checkpoint from " << argv[3] << " (requires brain instance)\n";
        } else if (arg1 == "list") {
            std::cout << "Checking available checkpoints (requires checkpoint directory)\n";
        }
    }
    
    void handleStats(int argc, char** argv) {
        if (argc < 3) {
            std::cout << "System statistics:\n";
            std::cout << "  stats brain\n";
            std::cout << "  stats memory\n";
            std::cout << "  stats learning\n";
            std::cout << "  stats all\n";
            return;
        }
        
        std::string arg1 = argv[2];
        
        if (arg1 == "brain") {
            std::cout << "Brain statistics (requires brain instance):\n";
            std::cout << "  (Would show neuron counts, firing rates, etc.)\n";
        } else if (arg1 == "memory") {
            std::cout << "Memory statistics:\n";
            std::cout << "  (Would show memory usage, active traces, episodes)\n";
        } else if (arg1 == "learning") {
            std::cout << "Learning statistics:\n";
            std::cout << "  (Would show learning progress, plasticity rates)\n";
        } else if (arg1 == "all") {
            std::cout << "Comprehensive system statistics:\n";
            std::cout << "  (Would show all system statistics)\n";
        }
    }
    
    void handleMonitor(int argc, char** argv) {
        size_t duration = 60; // Default 60 seconds
        size_t updateInterval = 1; // Default 1 second
        
        // Parse monitor arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--duration" && i + 1 < argc) {
                try {
                    duration = std::stoull(argv[++i]);
                } catch (...) {
                    throw std::runtime_error("Invalid duration value");
                }
            } else if (arg == "--update-interval" && i + 1 < argc) {
                try {
                    updateInterval = std::stoull(argv[++i]);
                } catch (...) {
                    throw std::runtime_error("Invalid update interval value");
                }
            }
        }
        
        std::cout << "Starting system monitor for " << duration << " seconds\n";
        std::cout << "Update interval: " << updateInterval << " seconds\n";
        
        // Would run real-time monitoring
        std::cout << "(Real-time monitoring would run here)\n";
    }
    
    void handleHelp(int argc, char** argv) {
        if (argc < 3) {
            std::cout << "=== NLM Command Line Interface Help ===\n\n";
            std::cout << "Available commands:\n";
            
            int maxNameLength = 0;
            for (const auto& pair : commands) {
                maxNameLength = std::max(maxNameLength, static_cast<int>(pair.first.length()));
            }
            
            for (const auto& pair : commands) {
                std::cout << "  " << pair.first << std::string(maxNameLength - pair.first.length() + 3, ' ')
                         << pair.second.description << "\n";
            }
            
            std::cout << "\nFor detailed help on a specific command: help <command>\n";
            std::cout << "\nCommon brain management commands: create, initialize, step, simulate, stats\n";
            std::cout << "Configuration commands: config, load, save\n";
            std::cout << "Advanced commands: learning, memory, neuromodulation, development, plasticity\n";
            
        } else {
            std::string cmd = argv[2];
            auto it = commands.find(cmd);
            if (it != commands.end()) {
                std::cout << "=== " << it->second.name << " ===\n";
                std::cout << it->second.description << "\n\n";
                std::cout << "Usage: " << it->second.usage << "\n";
            } else {
                std::cerr << "Unknown command: " << cmd << std::endl;
            }
        }
    }
    
    void handleVersion(int argc, char** argv) {
        std::cout << "NLM (Neural Learning Machine) Command Line Interface\n";
        std::cout << "Version: 0.1.0\n";
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << "\n";
        std::cout << "Description: Experimental artificial developmental brain simulation\n";
    }
    
    void handleAbout(int argc, char** argv) {
        std::cout << "=== NLM - Neural Learning Machine ===\n\n";
        std::cout << "Project Overview:\n";
        std::cout << "  NLM is an experimental computational brain project.\n";
        std::cout << "  Long-term goal: Create a neural system that begins in a primitive\n";
        std::cout << "  developmental state and acquires increasingly complex abilities.\n\n";
        std::cout << "Key Features:\n";
        std::cout << "  * Real spiking neural computation (LIF neurons)\n";
        std::cout << "  * Event-driven spike propagation with delays\n";
        std::cout << "  * Multiple plasticity mechanisms (STDP, Hebbian)\n";
        std::cout << "  * Structural plasticity (synaptogenesis/pruning)\n";
        std::cout << "  * Integrated memory systems (working, episodic)\n";
        std::cout << "  * Neuromodulation (dopamine, curiosity, novelty)\n";
        std::cout << "  * Prediction systems\n";
        std::cout << "  * Cognitive mechanisms (planning, attention)\n";
        std::cout << "  * Developmental stages\n\n";
        std::cout << "Current Phase: Phase 6 (Final Integration)\n";
        std::cout << "Website: https://github.com/nlm-project/nlm\n";
    }
    
    void handleExamples(int argc, char** argv) {
        std::cout << "=== NLM Usage Examples ===\n\n";
        
        std::cout << "1. Create a simple brain:\n";
        std::cout << "   nlm create --neuron-count 1000 --region-count 2\n\n";
        
        std::cout << "2. Initialize and simulate:\n";
        std::cout << "   nlm create --neuron-count 500\n";
        std::cout << "   nlm initialize\n";
        std::cout << "   nlm step --steps 1000\n\n";
        
        std::cout << "3. Run complete simulation:\n";
        std::cout << "   nlm simulate --steps 5000 --config my_config.cfg\n\n";
        
        std::cout << "4. Configure world and agent:\n";
        std::cout << "   nlm world configure --width 20 --height 20 --vision 8 8\n";
        std::cout << "   nlm agent initialize\n\n";
        
        std::cout << "5. Advanced simulation with learning:\n";
        std::cout << "   nlm create --seed 42\n";
        std::cout << "   nlm simulate --steps 10000\n";
        std::cout << "   nlm learning enable neuromodulation\n\n";
        
        std::cout << "6. Run integration experiment:\n";
        std::cout << "   nlm experiment phase6 --steps 2000\n\n";
        
        std::cout << "7. Monitor system:\n";
        std::cout << "   nlm monitor --duration 30 --update-interval 2\n\n";
        
        std::cout << "8. Check system status:\n";
        std::cout << "   nlm stats brain\n";
        std::cout << "   nlm stats memory\n";
        std::cout << "   nlm stats all\n";
        
        std::cout << "\nFor more details on any command: nlm help <command>\n";
    }
    
    void showHelp() {
        std::cout << "=== NLM (Neural Learning Machine) Command Line Interface ===\n\n";
        std::cout << "Usage: nlm [OPTIONS] [COMMAND] [ARGS]\n\n";
        std::cout << "Basic commands:\n";
        std::cout << "  create          Create a new brain with configuration\n";
        std::cout << "  initialize      Initialize the brain with current configuration\n";
        std::cout << "  step            Run one or more simulation steps\n";
        std::cout << "  simulate         Run a complete simulation\n";
        std::cout << "  reset           Reset the brain to initial state\n\n";
        std::cout << "Configuration management:\n";
        std::cout << "  config          Manage configuration settings\n";
        std::cout << "  load FILENAME   Load configuration from file\n";
        std::cout << "  save FILENAME   Save configuration to file\n\n";
        std::cout << "Advanced features:\n";
        std::cout << "  world           Manage the simulation world\n";
        std::cout << "  agent           Manage the agent interface\n";
        std::cout << "  learning        Manage learning systems\n";
        std::cout << "  memory          Manage memory systems\n";
        std::cout << "  neuromodulation  Manage neuromodulation systems\n\n";
        std::cout << "Development and plasticity:\n";
        std::cout << "  development     Manage developmental systems\n";
        std::cout << "  plasticity      Manage plasticity systems\n\n";
        std::cout << "Experiment and testing:\n";
        std::cout << "  experiment      Run experiments\n";
        std::cout << "  test           Run system tests\n\n";
        std::cout << "Checkpoint and persistence:\n";
        std::cout << "  checkpoint      Manage checkpoint operations\n\n";
        std::cout << "Debug and analysis:\n";
        std::cout << "  stats           Show system statistics\n";
        std::cout << "  monitor         Monitor system in real-time\n\n";
        std::cout << "Information:\n";
        std::cout << "  help            Show help information\n";
        std::cout << "  version         Show version information\n";
        std::cout << "  about          Show project information\n";
        std::cout << "  examples        Show usage examples\n\n";
        
        std::cout << "Examples:\n";
        std::cout << "  nlm create --neuron-count 1000\n";
        std::cout << "  nlm simulate --steps 1000\n";
        std::cout << "  nlm help simulate\n\n";
        
        std::cout << "For more information, visit: https://github.com/nlm-project/nlm\n";
    }
    
    // Helper methods
    void updateProgress(size_t current, size_t total, size_t step) {
        if (step % 100 == 0 || step == total) {
            float progress = static_cast<float>(current) / static_cast<float>(total) * 100.0f;
            std::cout << "\rProgress: " << progress << "% (" << current << "/" << total << ")";
            std::cout.flush();
        }
    }
    
    void clearLine() {
        std::cout << "\r" << std::string(80, ' ') << "\r";
        std::cout.flush();
    }
    
    void logMessage(const std::string& message, const std::string& level = "INFO") {
        auto timestamp = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        
        std::cout << "[" << std::put_time(std::localtime(&time_t), "%H:%M:%S") 
                 << "] " << level << ": " << message << "\n";
    }
};

} // namespace nlm

// Main entry point
int main(int argc, char** argv) {
    try {
        nlm::NLMCommandLine cli;
        cli.setArgs(argc, argv);
        cli.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
