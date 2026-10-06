// NLM Command Line Interface Implementation
// Provides a comprehensive CLI for NLM neural simulation

#include "CommandLineInterface.h"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"
#include "../src/experiments/Phase6Demo.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

CommandLineInterface::CommandLineInterface()
    : defaultConfigPath_("configs/default.cfg")
    , verboseMode_(false)
    , quietMode_(false) {
    
    // Register all commands
    registerCommand("run", "Run neural simulation experiment", 
                    "nlm run [config] [steps] [--checkpoint] [--verbose] [--quiet]",
                    std::bind(&CommandLineInterface::handleRun, this, std::placeholders::_1), false, false);
    
    registerCommand("create", "Create a new brain instance", 
                    "nlm create [neuron_count] [region_count]",
                    std::bind(&CommandLineInterface::handleCreate, this, std::placeholders::_1), false, false);
    
    registerCommand("save", "Save brain state to file", 
                    "nlm save <filepath> [--compressed] [--verify]",
                    std::bind(&CommandLineInterface::handleSave, this, std::placeholders::_1), true, false);
    
    registerCommand("load", "Load brain state from file", 
                    "nlm load <filepath>",
                    std::bind(&CommandLineInterface::handleLoad, this, std::placeholders::_1), false, false);
    
    registerCommand("status", "Display brain status and statistics", 
                    "nlm status [--detailed] [--memory] [--neuromodulation]",
                    std::bind(&CommandLineInterface::handleStatus, this, std::placeholders::_1), true, false);
    
    registerCommand("test", "Run integration tests", 
                    "nlm test [--quick] [--full] [--phase6]",
                    std::bind(&CommandLineInterface::handleTest, this, std::placeholders::_1), false, false);
    
    registerCommand("demo", "Run a demonstration of NLM functionality", 
                    "nlm demo [--duration] [--steps]",
                    std::bind(&CommandLineInterface::handleDemo, this, std::placeholders::_1), false, false);
    
    registerCommand("benchmark", "Run performance benchmark", 
                    "nlm benchmark [--neurons] [--steps] [--iterations]",
                    std::bind(&CommandLineInterface::handleBenchmark, this, std::placeholders::_1), false, true);
    
    registerCommand("replay", "Replay recorded episodes from memory", 
                    "nlm replay <episode_count> [--max_steps]",
                    std::bind(&CommandLineInterface::handleReplay, this, std::placeholders::_1), true, true);
    
    registerCommand("analyze", "Analyze brain state and patterns", 
                    "nlm analyze [--patterns] [--plasticity] [--development]",
                    std::bind(&CommandLineInterface::handleAnalyze, this, std::placeholders::_1), true, true);
    
    registerCommand("export", "Export data to various formats", 
                    "nlm export <format> <output_file>",
                    std::bind(&CommandLineInterface::handleExport, this, std::placeholders::_1), true, true);
    
    registerCommand("monitor", "Real-time monitoring of brain activity", 
                    "nlm monitor [--interval] [--output] [--history]",
                    std::bind(&CommandLineInterface::handleMonitor, this, std::placeholders::_1), true, true);
    
    registerCommand("check", "Check system health and integrity", 
                    "nlm check [--deep] [--all]",
                    std::bind(&CommandLineInterface::handleCheck, this, std::placeholders::_1), false, false);
    
    registerCommand("reset", "Reset brain state", 
                    "nlm reset [--confirm]",
                    std::bind(&CommandLineInterface::handleReset, this, std::placeholders::_1), true, false);
    
    registerCommand("info", "Display system information", 
                    "nlm info [--full] [--dependencies]",
                    std::bind(&CommandLineInterface::handleInfo, this, std::placeholders::_1), false, false);
}

CommandLineInterface::~CommandLineInterface() {
    // Clean up
    if (currentBrain_) {
        // Brain will be destroyed when shared_ptr goes out of scope
    }
}

int CommandLineInterface::run(int argc, char** argv) {
    if (argc <= 1) {
        displayHelp();
        return 0;
    }
    
    std::string command = argv[1];
    std::vector<std::string> args;
    
    // Parse remaining arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle flags with dashes
        if (arg.substr(0, 2) == "--") {
            args.push_back(arg);
        } else if (arg.substr(0, 1) == "-") {
            args.push_back(arg);
        } else {
            // Skip --help, --version flags as they're parsed before command execution
            if (arg == "--help" || arg == "--version") {
                continue;
            }
            args.push_back(arg);
        }
    }
    
    // Check for global flags before command execution
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--verbose") {
            verboseMode_ = true;
            args.erase(args.begin() + i);
            --i;
        } else if (args[i] == "--quiet") {
            quietMode_ = true;
            args.erase(args.begin() + i);
            --i;
        }
    }
    
    return executeCommand(command, args);
}

void CommandLineInterface::registerCommand(const std::string& name, 
                                         const std::string& description,
                                         const std::string& usage,
                                         std::function<int(const std::vector<std::string>&)> handler,
                                         bool requiresBrain,
                                         bool isAdvanced) {
    Command cmd;
    cmd.name = name;
    cmd.description = description;
    cmd.usage = usage;
    cmd.handler = handler;
    cmd.requiresBrain = requiresBrain;
    cmd.isAdvanced = isAdvanced;
    
    commands_.push_back(cmd);
    commandMap_[name] = cmd;
}

void CommandLineInterface::displayHelp() const {
    std::cout << "NLM (Neural Learning Machine) - Command Line Interface" << std::endl;
    std::cout << "=====================================================" << std::endl << std::endl;
    
    std::cout << "Basic Commands:" << std::endl;
    for (const auto& cmd : commands_) {
        if (!cmd.isAdvanced) {
            std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
            std::cout << "    Usage: " << cmd.usage << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Advanced Commands:" << std::endl;
    for (const auto& cmd : commands_) {
        if (cmd.isAdvanced) {
            std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
            std::cout << "    Usage: " << cmd.usage << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Global Options:" << std::endl;
    std::cout << "  --help                    Display this help message" << std::endl;
    std::cout << "  --version                 Display version information" << std::endl;
    std::cout << "  --verbose                 Enable verbose output" << std::endl;
    std::cout << "  --quiet                   Suppress non-essential output" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm run --steps 1000" << std::endl;
    std::cout << "  nlm status --detailed" << std::endl;
    std::cout << "  nlm demo --steps 500" << std::endl;
    std::cout << "  nlm benchmark --neurons 1000 --iterations 5" << std::endl;
}

void CommandLineInterface::displayVersion() const {
    std::cout << "NLM (Neural Learning Machine) v0.1.0" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Build with C++20, scikit-build-core, pybind11" << std::endl;
    std::cout << "Copyright (c) 2024 NLM Project" << std::endl;
}

bool CommandLineInterface::loadConfig(const std::string& configPath, std::shared_ptr<Config>& config) {
    config = std::make_shared<Config>();
    
    // Try to load from file
    if (!config->loadFromFile(configPath)) {
        NLM_LOG_WARN("Could not load config from " + configPath + ", using defaults");
        return false;
    }
    
    if (verboseMode_) {
        std::cout << "Loaded configuration from: " << configPath << std::endl;
    }
    
    return true;
}

void CommandLineInterface::applyConfigOverrides(const std::vector<std::string>& args, std::shared_ptr<Config>& config) {
    // Parse command line overrides for configuration
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i].substr(0, 8) == "--config=") {
            std::string configPath = args[i].substr(8);
            if (loadConfig(configPath, config)) {
                args.erase(args.begin() + i);
                --i;
            }
        } else if (args[i].find("=") != std::string::npos) {
            // Handle other key=value pairs
            size_t equalsPos = args[i].find("=");
            std::string key = args[i].substr(0, equalsPos);
            std::string value = args[i].substr(equalsPos + 1);
            
            // Convert string values to appropriate types
            try {
                if (key == "neuron_count") {
                    config->set(key, std::stoll(value), ConfigSource::CommandLine);
                } else if (key == "region_count") {
                    config->set(key, std::stoll(value), ConfigSource::CommandLine);
                } else if (key == "connection_probability") {
                    config->set(key, std::stof(value), ConfigSource::CommandLine);
                } else if (key == "steps" || key == "max_steps") {
                    config->set(key, std::stoll(value), ConfigSource::CommandLine);
                }
            } catch (...) {
                displayError("Invalid value for config key: " + key);
            }
        }
    }
}

int CommandLineInterface::executeCommand(const std::string& command, const std::vector<std::string>& args) {
    auto it = commandMap_.find(command);
    if (it == commandMap_.end()) {
        displayError("Unknown command: " + command);
        displayHelp();
        return 1;
    }
    
    const Command& cmd = it->second;
    
    // Check if brain is required
    if (cmd.requiresBrain && !currentBrain_) {
        displayError("No brain is currently loaded. Use 'nlm create' first or specify a saved brain.");
        return 1;
    }
    
    // Validate arguments
    if (!validateArguments(command, args)) {
        return 1;
    }
    
    try {
        // Create configuration if needed
        if (!currentConfig_) {
            currentConfig_ = std::make_shared<Config>();
            loadConfig(defaultConfigPath_, currentConfig_);
            applyConfigOverrides(args, currentConfig_);
        }
        
        // Execute command
        int result = cmd.handler(args);
        
        return result;
    } catch (const std::exception& e) {
        displayError(std::string("Command failed: ") + e.what());
        return 1;
    }
}

void CommandLineInterface::displayError(const std::string& message, bool isFatal) const {
    if (quietMode_) {
        return;
    }
    
    std::cerr << "ERROR: " << message << std::endl;
    
    if (isFatal) {
        std::cerr << "Fatal error, exiting." << std::endl;
    }
}

bool CommandLineInterface::validateArguments(const std::string& command, const std::vector<std::string>& args) const {
    // Basic validation for each command
    if (command == "run") {
        // Check for required arguments
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--config" && i + 1 < args.size()) {
                // Next argument should be a file path
                ++i;
            }
        }
    } else if (command == "save") {
        if (args.empty()) {
            displayError("Save command requires a file path");
            return false;
        }
    } else if (command == "load") {
        if (args.empty()) {
            displayError("Load command requires a file path");
            return false;
        }
    } else if (command == "replay") {
        if (args.empty()) {
            displayError("Replay command requires episode count");
            return false;
        }
    }
    
    return true;
}

int CommandLineInterface::handleCreate(const std::vector<std::string>& args) {
    // Parse arguments for brain creation
    size_t neuronCount = 500;  // Default
    size_t regionCount = 1;   // Default
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--neurons" || args[i] == "-n") {
            if (i + 1 < args.size()) {
                neuronCount = std::stoll(args[++i]);
            }
        } else if (args[i] == "--regions" || args[i] == "-r") {
            if (i + 1 < args.size()) {
                regionCount = std::stoll(args[++i]);
            }
        } else if (args[i].find("--neurons=") == 0) {
            neuronCount = std::stoll(args[i].substr(9));
        } else if (args[i].find("--regions=") == 0) {
            regionCount = std::stoll(args[i].substr(9));
        }
    }
    
    // Create configuration
    currentConfig_ = std::make_shared<Config>();
    currentConfig_->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::CommandLine);
    currentConfig_->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::CommandLine);
    currentConfig_->set("connection_probability", 0.15f, ConfigSource::Default);
    currentConfig_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    
    // Create brain
    currentBrain_ = std::make_shared<Brain>(currentConfig_);
    if (!currentBrain_->initialize()) {
        displayError("Failed to initialize brain");
        return 1;
    }
    
    if (!quietMode_) {
        std::cout << "Brain created successfully:" << std::endl;
        std::cout << "  Neurons: " << neuronCount << std::endl;
        std::cout << "  Regions: " << regionCount << std::endl;
        std::cout << "  Configuration: " << defaultConfigPath_ << std::endl;
    }
    
    return 0;
}

int CommandLineInterface::handleRun(const std::vector<std::string>& args) {
    if (!currentBrain_) {
        displayError("No brain is currently loaded. Use 'nlm create' first.");
        return 1;
    }
    
    // Parse run arguments
    uint64_t steps = 1000;  // Default
    bool checkpoint = false;
    std::string checkpointPath;
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--steps" || args[i] == "-s") {
            if (i + 1 < args.size()) {
                steps = std::stoll(args[++i]);
            }
        } else if (args[i] == "--checkpoint" || args[i] == "-c") {
            checkpoint = true;
            checkpointPath = "/tmp/nlm_checkpoint_" + std::to_string(std::time(nullptr)) + ".bin";
            if (i + 1 < args.size() && args[i + 1].find("--") != 0) {
                checkpointPath = args[++i];
            }
        } else if (args[i].find("--steps=") == 0) {
            steps = std::stoll(args[i].substr(8));
        } else if (args[i].find("--checkpoint=") == 0) {
            checkpointPath = args[i].substr(13);
            checkpoint = true;
        }
    }
    
    // Create world and agent
    auto world = std::make_shared<SimpleWorld>();
    world->initialize(16, 16);
    
    auto agent = std::make_shared<AgentBrain>(currentBrain_);
    agent->initialize(*world);
    
    // Run simulation
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    size_t firingCount = 0;
    
    if (!quietMode_) {
        std::cout << "Running NLM simulation for " << steps << " steps..." << std::endl;
    }
    
    for (uint64_t step = 0; step < steps; ++step) {
        // Get observation
        SensoryPercept percept = world->observe(currentBrain_->getRegions()[0].get());
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain step
        currentBrain_->step(step, step * 0.001);
        
        // Get motor command
        MotorCommand cmd = agent->decodeMotorCommand();
        
        // Apply action to world
        world->applyAction(currentBrain_->getRegions()[0].get(), cmd);
        
        // Compute reward
        float reward = world->computeReward(currentBrain_->getRegions()[0].get());
        totalReward += reward;
        
        // Apply reward modulation
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.001);
        
        // Collect metrics
        totalFiringRate += currentBrain_->getAverageFiringRate();
        if (currentBrain_->getFiringNeuronCount() > 0) firingCount++;
        
        // Log progress
        if (!quietMode_ && step % 100 == 0) {
            std::cout << "Step " << step << "/" << steps << 
                      " | Reward: " << (totalReward / (step + 1)) <<
                      " | Firing: " << currentBrain_->getAverageFiringRate() <<
                      " | WorkingMem: " << (currentBrain_->getWorkingMemory() ? 
                          currentBrain_->getWorkingMemory()->getActiveTraces() : 0) << std::endl;
        }
        
        // Save checkpoint if requested
        if (checkpoint && step % 100 == 0) {
            if (currentBrain_->save(checkpointPath + "_step" + std::to_string(step) + ".bin")) {
                if (!quietMode_) {
                    std::cout << "Checkpoint saved at step " << step << std::endl;
                }
            }
        }
    }
    
    // Print final results
    if (!quietMode_) {
        std::cout << std::endl;
        std::cout << "=== Simulation Results ===" << std::endl;
        std::cout << "Total Reward: " << (totalReward / steps) << std::endl;
        std::cout << "Average Firing Rate: " << (totalFiringRate / steps) << std::endl;
        std::cout << "Steps with Activity: " << firingCount << "/" << steps << std::endl;
        std::cout << "Simulation completed successfully!" << std::endl;
    }
    
    return 0;
}

int CommandLineInterface::handleStatus(const std::vector<std::string>& args) {
    if (!currentBrain_) {
        displayError("No brain is currently loaded.");
        return 1;
    }
    
    bool detailed = false;
    bool showMemory = false;
    bool showNeuromodulation = false;
    
    // Parse flags
    for (const auto& arg : args) {
        if (arg == "--detailed" || arg == "-d") {
            detailed = true;
        } else if (arg == "--memory" || arg == "-m") {
            showMemory = true;
        } else if (arg == "--neuromodulation" || arg == "-n") {
            showNeuromodulation = true;
        }
    }
    
    // Display brain status
    currentBrain_->logStatus();
    
    if (detailed) {
        std::cout << std::endl;
        std::cout << "=== Detailed Brain Statistics ===" << std::endl;
        std::cout << "Total Spikes: " << currentBrain_->getTotalSpikeCount() << std::endl;
        std::cout << "Pending Spike Events: " << currentBrain_->getPendingSpikeEventCount() << std::endl;
        std::cout << "Excitatory/Inhibitory Ratio: " << currentBrain_->getExcitationInhibitionRatio() << std::endl;
        
        if (showMemory && currentBrain_->getWorkingMemory()) {
            std::cout << std::endl;
            std::cout << "=== Memory System ===" << std::endl;
            std::cout << "Working Memory Active Traces: " << currentBrain_->getWorkingMemory()->getActiveTraces() << std::endl;
            std::cout << "Working Memory Capacity: " << currentBrain_->getWorkingMemory()->getCapacity() << std::endl;
            std::cout << "Working Memory Activity Level: " << currentBrain_->getWorkingMemory()->getMemoryActivity() << std::endl;
            
            if (currentBrain_->getEpisodicMemory()) {
                std::cout << "Episodic Memory Episodes: " << currentBrain_->getEpisodicMemory()->getEpisodeCount() << std::endl;
            }
        }
        
        if (showNeuromodulation) {
            std::cout << std::endl;
            std::cout << "=== Neuromodulation System ===" << std::endl;
            
            if (currentBrain_->getDopamine()) {
                std::cout << "Dopamine Level: " << currentBrain_->getDopamine()->getLevel() << std::endl;
                std::cout << "Dopamine Plasticity Factor: " << currentBrain_->getDopamine()->getPlasticityFactor() << std::endl;
            }
            
            if (currentBrain_->getCuriosity()) {
                std::cout << "Curiosity Level: " << currentBrain_->getCuriosity()->getLevel() << std::endl;
            }
            
            if (currentBrain_->getNovelty()) {
                std::cout << "Novelty Level: " << currentBrain_->getNovelty()->getLevel() << std::endl;
            }
            
            if (currentBrain_->getPredictionErrorSignal()) {
                std::cout << "Prediction Error: " << currentBrain_->getPredictionErrorSignal()->getError() << std::endl;
            }
        }
        
        // Show region details
        std::cout << std::endl;
        std::cout << "=== Regional Details ===" << std::endl;
        for (const auto& region : currentBrain_->getRegions()) {
            std::cout << "Region " << region->getId().index() << " (" << region->getName() << "):" << std::endl;
            std::cout << "  Neurons: " << region->getTotalNeuronCount() << std::endl;
            std::cout << "  Populations: " << region->getPopulationCount() << std::endl;
            std::cout << "  Synapses: " << region->getSynapseCount() << std::endl;
            std::cout << "  Average Synaptic Weight: " << region->getAverageSynapticWeight() << std::endl;
        }
    }
    
    return 0;
}

int CommandLineInterface::handleSave(const std::vector<std::string>& args) {
    if (args.empty()) {
        displayError("Save command requires a file path.");
        return 1;
    }
    
    std::string filepath = args[0];
    bool compressed = false;
    bool verify = false;
    
    // Parse additional flags
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--compressed" || args[i] == "-c") {
            compressed = true;
        } else if (args[i] == "--verify" || args[i] == "-v") {
            verify = true;
        }
    }
    
    if (!currentBrain_->save(filepath)) {
        displayError("Failed to save brain state to " + filepath);
        return 1;
    }
    
    if (!quietMode_) {
        std::cout << "Brain state saved successfully to: " << filepath << std::endl;
        
        if (verify) {
            // Try to load it back
            auto tempBrain = std::make_shared<Brain>(currentConfig_);
            tempBrain->initialize();
            
            if (tempBrain->load(filepath)) {
                std::cout << "Verification successful - brain state can be loaded." << std::endl;
            } else {
                std::cout << "WARNING: Verification failed - brain state could not be loaded." << std::endl;
            }
        }
    }
    
    return 0;
}

int CommandLineInterface::handleLoad(const std::vector<std::string>& args) {
    if (args.empty()) {
        displayError("Load command requires a file path.");
        return 1;
    }
    
    std::string filepath = args[0];
    
    // Create new brain
    currentBrain_ = std::make_shared<Brain>(currentConfig_);
    
    if (!currentBrain_->initialize()) {
        displayError("Failed to initialize brain for loading.");
        return 1;
    }
    
    if (!currentBrain_->load(filepath)) {
        displayError("Failed to load brain state from " + filepath);
        return 1;
    }
    
    if (!quietMode_) {
        std::cout << "Brain state loaded successfully from: " << filepath << std::endl;
    }
    
    return 0;
}

int CommandLineInterface::handleDemo(const std::vector<std::string>& args) {
    // Use the existing Phase6Demo for demonstration
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Running comprehensive integration demonstration..." << std::endl << std::endl;
    
    // Run the demo using the existing Phase6Demo
    auto experiment = std::make_unique<Phase6IntegratedExperiment>();
    
    // Parse demo arguments
    uint64_t steps = 2000;
    uint64_t neurons = 500;
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--steps" || args[i] == "-s") {
            if (i + 1 < args.size()) {
                steps = std::stoll(args[++i]);
            }
        } else if (args[i] == "--neurons" || args[i] == "-n") {
            if (i + 1 < args.size()) {
                neurons = std::stoll(args[++i]);
            }
        } else if (args[i].find("--steps=") == 0) {
            steps = std::stoll(args[i].substr(8));
        } else if (args[i].find("--neurons=") == 0) {
            neurons = std::stoll(args[i].substr(10));
        }
    }
    
    Phase6Config config;
    config.neuronCount = neurons;
    config.maxSteps = steps;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment->run(config);
    
    std::cout << std::endl;
    std::cout << "=== DEMO RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    
    return 0;
}

int CommandLineInterface::handleTest(const std::vector<std::string>& args) {
    std::cout << "=== NLM Integration Tests ===" << std::endl;
    
    bool quick = false;
    bool full = false;
    bool phase6 = false;
    
    for (const auto& arg : args) {
        if (arg == "--quick" || arg == "-q") {
            quick = true;
        } else if (arg == "--full" || arg == "-f") {
            full = true;
        } else if (arg == "--phase6" || arg == "-p6") {
            phase6 = true;
        }
    }
    
    auto experiment = std::make_unique<Phase6IntegratedExperiment>();
    
    if (quick) {
        std::cout << "Running quick integration verification..." << std::endl;
        bool success = experiment->verifyIntegration();
        if (success) {
            std::cout << "✓ Integration verification PASSED" << std::endl;
        } else {
            std::cout << "✗ Integration verification FAILED" << std::endl;
        }
        return success ? 0 : 1;
    } else if (phase6) {
        std::cout << "Running Phase 6 integration experiment..." << std::endl;
        Phase6Config config;
        config.neuronCount = 100;
        config.maxSteps = 500;
        
        auto result = experiment->run(config);
        
        std::cout << std::endl;
        std::cout << "=== Test Results ===" << std::endl;
        std::cout << "Memory Working Memory Integrated: " << (result.memoryWorkingMemoryIntegrated ? "YES" : "NO") << std::endl;
        std::cout << "Memory Episodic Memory Integrated: " << (result.memoryEpisodicMemoryIntegrated ? "YES" : "NO") << std::endl;
        std::cout << "Neuromodulation Integrated: " << (result.neuromodulationIntegrated ? "YES" : "NO") << std::endl;
        return 0;
    } else {
        std::cout << "Running comprehensive integration tests..." << std::endl;
        
        // Test 1: Memory integration
        bool memoryOK = experiment->testMemoryIntegration();
        
        // Test 2: Neuromodulation integration
        bool neuromodOK = experiment->testNeuromodulationIntegration();
        
        // Test 3: Checkpointing
        bool checkpointOK = experiment->testCheckpointing();
        
        // Test 4: Replay
        bool replayOK = experiment->testReplay();
        
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Memory Integration: " << (memoryOK ? "PASS" : "FAIL") << std::endl;
        std::cout << "Neuromodulation Integration: " << (neuromodOK ? "PASS" : "FAIL") << std::endl;
        std::cout << "Checkpointing: " << (checkpointOK ? "PASS" : "FAIL") << std::endl;
        std::cout << "Replay: " << (replayOK ? "PASS" : "FAIL") << std::endl;
        
        return (memoryOK && neuromodOK && checkpointOK && replayOK) ? 0 : 1;
    }
}

int CommandLineInterface::handleBenchmark(const std::vector<std::string>& args) {
    std::cout << "=== NLM Performance Benchmark ===" << std::endl;
    
    // Parse benchmark arguments
    size_t neurons = 500;
    uint64_t steps = 1000;
    size_t iterations = 3;
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--neurons" || args[i] == "-n") {
            if (i + 1 < args.size()) {
                neurons = std::stoll(args[++i]);
            }
        } else if (args[i] == "--steps" || args[i] == "-s") {
            if (i + 1 < args.size()) {
                steps = std::stoll(args[++i]);
            }
        } else if (args[i] == "--iterations" || args[i] == "-i") {
            if (i + 1 < args.size()) {
                iterations = std::stoll(args[++i]);
            }
        } else if (args[i].find("--neurons=") == 0) {
            neurons = std::stoll(args[i].substr(9));
        } else if (args[i].find("--steps=") == 0) {
            steps = std::stoll(args[i].substr(7));
        } else if (args[i].find("--iterations=") == 0) {
            iterations = std::stoll(args[i].substr(12));
        }
    }
    
    std::vector<double> times;
    std::vector<float> rewards;
    
    for (size_t iter = 0; iter < iterations; ++iter) {
        if (!quietMode_) {
            std::cout << "Iteration " << (iter + 1) << "/" << iterations << "..." << std::endl;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create brain
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create world and agent
        auto world = std::make_shared<SimpleWorld>();
        world->initialize(16, 16);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Run simulation
        float totalReward = 0.0f;
        
        for (uint64_t step = 0; step < steps; ++step) {
            // Get observation
            SensoryPercept percept = world->observe(brain->getRegions()[0].get());
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyAction(brain->getRegions()[0].get(), cmd);
            
            // Compute reward
            float reward = world->computeReward(brain->getRegions()[0].get());
            totalReward += reward;
            
            // Apply reward modulation
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        times.push_back(duration);
        rewards.push_back(totalReward / steps);
        
        if (!quietMode_) {
            std::cout << "  Time: " << duration << "s, Reward: " << (totalReward / steps) << std::endl;
        }
        
        // Clean up
        brain.reset();
        world.reset();
        agent.reset();
    }
    
    // Calculate statistics
    double totalTime = 0.0;
    float totalReward = 0.0f;
    
    for (size_t i = 0; i < times.size(); ++i) {
        totalTime += times[i];
        totalReward += rewards[i];
    }
    
    double avgTime = totalTime / times.size();
    float avgReward = totalReward / times.size();
    
    std::cout << std::endl;
    std::cout << "=== Benchmark Results ===" << std::endl;
    std::cout << "Iterations: " << iterations << std::endl;
    std::cout << "Neurons per brain: " << neurons << std::endl;
    std::cout << "Steps per iteration: " << steps << std::endl;
    std::cout << std::endl;
    std::cout << "Performance:" << std::endl;
    std::cout << "  Average time per iteration: " << avgTime << "s" << std::endl;
    std::cout << "  Average reward per step: " << avgReward << std::endl;
    std::cout << "  Steps per second: " << (steps / avgTime) << std::endl;
    std::cout << std::endl;
    std::cout << "Individual iterations:" << std::endl;
    for (size_t i = 0; i < times.size(); ++i) {
        std::cout << "  Iter " << (i + 1) << ": " << times[i] << "s, reward: " << rewards[i] << std::endl;
    }
    
    return 0;
}

int CommandLineInterface::handleReset(const std::vector<std::string>& args) {
    if (!currentBrain_) {
        displayError("No brain is currently loaded.");
        return 1;
    }
    
    bool confirm = false;
    for (const auto& arg : args) {
        if (arg == "--confirm" || arg == "-c") {
            confirm = true;
        }
    }
    
    if (!confirm) {
        std::cout << "Warning: Reset will clear all brain state including memory, synapses, and development. "
                  << "This cannot be undone." << std::endl;
        std::cout << "Type 'reset --confirm' to proceed: ";
        std::string input;
        std::getline(std::cin, input);
        if (input != "reset --confirm") {
            std::cout << "Reset cancelled." << std::endl;
            return 0;
        }
    }
    
    currentBrain_->reset();
    
    if (!quietMode_) {
        std::cout << "Brain reset successfully." << std::endl;
    }
    
    return 0;
}

int CommandLineInterface::handleInfo(const std::vector<std::string>& args) {
    std::cout << "=== NLM System Information ===" << std::endl;
    
    bool full = false;
    bool dependencies = false;
    
    for (const auto& arg : args) {
        if (arg == "--full" || arg == "-f") {
            full = true;
        } else if (arg == "--dependencies" || arg == "-d") {
            dependencies = true;
        }
    }
    
    std::cout << "Version: NLM v0.1.0" << std::endl;
    std::cout << "Phase: Real Neural Computation" << std::endl;
    std::cout << "Build: C++20, scikit-build-core, pybind11" << std::endl;
    std::cout << "License: MIT" << std::endl;
    
    if (full) {
        std::cout << std::endl;
        std::cout << "=== Build Configuration ===" << std::endl;
        std::cout << "C++ Standard: C++20" << std::endl;
        std::cout << "Compiler: " << __VERSION__ << std::endl;
        std::cout << "Platform: Linux" << std::endl;
        
        if (dependencies) {
            std::cout << std::endl;
            std::cout << "=== Dependencies ===" << std::endl;
            std::cout << "Core libraries: nlm_core, nlm_agent, nlm_world" << std::endl;
            std::cout << "Experiment libraries: nlm_phase3, nlm_phase4" << std::endl;
            std::cout << "Python bindings: pynlm (scikit-build-core)" << std::endl;
            std::cout << "Optional dependencies: pytest, numpy (dev)" << std::endl;
        }
    }
    
    return 0;
}

int CommandLineInterface::handleCheck(const std::vector<std::string>& args) {
    std::cout << "=== NLM System Integrity Check ===" << std::endl;
    
    bool deep = false;
    bool all = false;
    
    for (const auto& arg : args) {
        if (arg == "--deep" || arg == "-d") {
            deep = true;
        } else if (arg == "--all" || arg == "-a") {
            all = true;
        }
    }
    
    // Check 1: Configuration
    std::cout << "1. Checking configuration..." << std::endl;
    if (!currentConfig_) {
        std::cout << "   ✗ No configuration loaded" << std::endl;
        if (all) {
            // Try to create default config
            currentConfig_ = std::make_shared<Config>();
            std::cout << "   - Created default configuration" << std::endl;
        } else {
            return 1;
        }
    } else {
        std::cout << "   ✓ Configuration loaded" << std::endl;
        
        if (deep) {
            // Check config validity
            auto neuronCount = currentConfig_->getOr<int64_t>("neuron_count", -1);
            auto regionCount = currentConfig_->getOr<int64_t>("region_count", -1);
            auto connectionProb = currentConfig_->getOr<float>("connection_probability", -1.0f);
            
            if (neuronCount > 0 && regionCount > 0 && connectionProb >= 0.0f) {
                std::cout << "   ✓ Configuration values are valid" << std::endl;
            } else {
                std::cout << "   ✗ Invalid configuration values" << std::endl;
            }
        }
    }
    
    // Check 2: Brain instance
    std::cout << "2. Checking brain instance..." << std::endl;
    if (!currentBrain_) {
        std::cout << "   ✗ No brain instance loaded" << std::endl;
        if (all) {
            std::cout << "   - Note: Brain can be created with 'nlm create'" << std::endl;
        } else {
            return 1;
        }
    } else {
        std::cout << "   ✓ Brain instance loaded" << std::endl;
        
        if (deep) {
            // Check brain statistics
            auto neuronCount = currentBrain_->getTotalNeuronCount();
            auto synapseCount = currentBrain_->getTotalSynapseCount();
            auto spikeCount = currentBrain_->getTotalSpikeCount();
            
            std::cout << "   - Neuron count: " << neuronCount << std::endl;
            std::cout << "   - Synapse count: " << synapseCount << std::endl;
            std::cout << "   - Total spikes: " << spikeCount << std::endl;
            
            if (neuronCount > 0 && synapseCount >= 0) {
                std::cout << "   ✓ Brain state appears valid" << std::endl;
            } else {
                std::cout << "   ✗ Invalid brain state" << std::endl;
            }
        }
    }
    
    // Check 3: Memory systems
    std::cout << "3. Checking memory systems..." << std::endl;
    if (currentBrain_) {
        auto* workingMem = currentBrain_->getWorkingMemory();
        auto* episodicMem = currentBrain_->getEpisodicMemory();
        auto* associativeMem = currentBrain_->getAssociativeMemory();
        auto* predictionSys = currentBrain_->getPredictionSystem();
        
        if (workingMem) {
            std::cout << "   ✓ Working memory: Active traces = " << workingMem->getActiveTraces() << std::endl;
        } else {
            std::cout << "   ✗ Working memory not available" << std::endl;
        }
        
        if (episodicMem) {
            std::cout << "   ✓ Episodic memory: Episodes = " << episodicMem->getEpisodeCount() << std::endl;
        } else {
            std::cout << "   ✗ Episodic memory not available" << std::endl;
        }
        
        if (associativeMem) {
            std::cout << "   ✓ Associative memory: Active patterns = " << associativeMem->getActivePatternCount() << std::endl;
        } else {
            std::cout << "   ✗ Associative memory not available" << std::endl;
        }
        
        if (predictionSys) {
            std::cout << "   ✓ Prediction system: Error = " << predictionSys->getPredictionError() << std::endl;
        } else {
            std::cout << "   ✗ Prediction system not available" << std::endl;
        }
    } else {
        std::cout << "   ⚠ No brain to check memory systems" << std::endl;
    }
    
    // Check 4: Neuromodulation
    std::cout << "4. Checking neuromodulation systems..." << std::endl;
    if (currentBrain_) {
        auto* dopamine = currentBrain_->getDopamine();
        auto* curiosity = currentBrain_->getCuriosity();
        auto* novelty = currentBrain_->getNovelty();
        
        if (dopamine) {
            std::cout << "   ✓ Dopamine system: Level = " << dopamine->getLevel() << std::endl;
        } else {
            std::cout << "   ✗ Dopamine system not available" << std::endl;
        }
        
        if (curiosity) {
            std::cout << "   ✓ Curiosity system: Level = " << curiosity->getLevel() << std::endl;
        } else {
            std::cout << "   ✗ Curiosity system not available" << std::endl;
        }
        
        if (novelty) {
            std::cout << "   ✓ Novelty system: Level = " << novelty->getLevel() << std::endl;
        } else {
            std::cout << "   ✗ Novelty system not available" << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "=== Integrity Check Complete ===" << std::endl;
    
    return 0;
}

} // namespace nlm

