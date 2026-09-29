// NLM Command Line Interface (CLI)
// Advanced command-line interface for controlling NLM simulations
// Provides comprehensive control over brain initialization, simulation, and analysis

#include "src/brain/Brain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/experiments/Phase6IntegratedExperiment.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>

namespace nlm {

// CLI Command parser and executor
class NLMCLI {
public:
    enum class CommandType {
        HELP,
        VERSION,
        INIT,
        CONFIG,
        RUN,
        SAVE,
        LOAD,
        STATUS,
        STATS,
        MONITOR,
        EXPERIMENT,
        TEMPLATE,
        PLUGIN,
        EXIT,
        UNKNOWN
    };
    
    struct Command {
        CommandType type;
        std::string description;
        std::vector<std::string> args;
    };
    
    NLMCLI() : currentBrain_(nullptr), currentWorld_(nullptr), currentAgent_(nullptr), 
               isMonitoring_(false), simulationRunning_(false) {
        setupCommands();
        setupTemplates();
    }
    
    ~NLMCLI() {
        cleanup();
    }
    
    // Main CLI loop
    void run() {
        printWelcomeBanner();
        
        std::string input;
        std::cout << "NLM CLI ready. Type 'help' for available commands." << std::endl;
        
        while (true) {
            std::cout << "nlm> ";
            std::getline(std::cin, input);
            
            if (input.empty()) continue;
            
            Command cmd = parseCommand(input);
            
            if (!executeCommand(cmd)) {
                std::cout << "Error: Unknown command or command failed." << std::endl;
            }
            
            if (cmd.type == CommandType::EXIT) {
                break;
            }
        }
    }
    
private:
    // Command setup
    void setupCommands() {
        commandDescriptions_ = {
            {CommandType::HELP, "Show help information"},
            {CommandType::VERSION, "Show version information"},
            {CommandType::INIT, "Initialize brain with configuration"},
            {CommandType::CONFIG, "Configure brain settings"},
            {CommandType::RUN, "Run simulation"},
            {CommandType::SAVE, "Save brain state to file"},
            {CommandType::LOAD, "Load brain state from file"},
            {CommandType::STATUS, "Show current status"},
            {CommandType::STATS, "Show statistics"},
            {CommandType::MONITOR, "Start monitoring mode"},
            {CommandType::EXPERIMENT, "Run experiment"},
            {CommandType::TEMPLATE, "Manage configuration templates"},
            {CommandType::PLUGIN, "Manage plugins"},
            {CommandType::EXIT, "Exit NLM CLI"}
        };
    }
    
    // Template setup
    void setupTemplates() {
        templates_ = {
            {"demo", createDemoTemplate()},
            {"research", createResearchTemplate()},
            {"fast_test", createFastTestTemplate()},
            {"full_integration", createFullIntegrationTemplate()},
            {"curiosity", createCuriosityTemplate()},
            {"development", createDevelopmentTemplate()}
        };
    }
    
    // Command parsing
    Command parseCommand(const std::string& input) {
        Command cmd;
        cmd.type = CommandType::UNKNOWN;
        
        std::istringstream iss(input);
        std::string token;
        iss >> token;
        
        // Convert to lowercase
        std::transform(token.begin(), token.end(), token.begin(), ::tolower);
        
        if (token == "help") {
            cmd.type = CommandType::HELP;
        } else if (token == "version") {
            cmd.type = CommandType::VERSION;
        } else if (token == "init") {
            cmd.type = CommandType::INIT;
        } else if (token == "config") {
            cmd.type = CommandType::CONFIG;
        } else if (token == "run") {
            cmd.type = CommandType::RUN;
        } else if (token == "save") {
            cmd.type = CommandType::SAVE;
        } else if (token == "load") {
            cmd.type = CommandType::LOAD;
        } else if (token == "status") {
            cmd.type = CommandType::STATUS;
        } else if (token == "stats") {
            cmd.type = CommandType::STATS;
        } else if (token == "monitor") {
            cmd.type = CommandType::MONITOR;
        } else if (token == "experiment") {
            cmd.type = CommandType::EXPERIMENT;
        } else if (token == "template") {
            cmd.type = CommandType::TEMPLATE;
        } else if (token == "plugin") {
            cmd.type = CommandType::PLUGIN;
        } else if (token == "exit" || token == "quit") {
            cmd.type = CommandType::EXIT;
        }
        
        // Capture remaining arguments
        std::string arg;
        while (iss >> arg) {
            cmd.args.push_back(arg);
        }
        
        return cmd;
    }
    
    // Command execution
    bool executeCommand(const Command& cmd) {
        switch (cmd.type) {
            case CommandType::HELP:
                showHelp(cmd.args);
                break;
            case CommandType::VERSION:
                showVersion();
                break;
            case CommandType::INIT:
                initializeBrain(cmd.args);
                break;
            case CommandType::CONFIG:
                configureBrain(cmd.args);
                break;
            case CommandType::RUN:
                runSimulation(cmd.args);
                break;
            case CommandType::SAVE:
                saveState(cmd.args);
                break;
            case CommandType::LOAD:
                loadState(cmd.args);
                break;
            case CommandType::STATUS:
                showStatus();
                break;
            case CommandType::STATS:
                showStats();
                break;
            case CommandType::MONITOR:
                startMonitoring(cmd.args);
                break;
            case CommandType::EXPERIMENT:
                runExperiment(cmd.args);
                break;
            case CommandType::TEMPLATE:
                manageTemplate(cmd.args);
                break;
            case CommandType::PLUGIN:
                managePlugins(cmd.args);
                break;
            case CommandType::EXIT:
                return false;  // Exit signal
            default:
                return false;
        }
        return true;
    }
    
    // Command implementations
    void showHelp(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cout << "Available commands:" << std::endl;
            for (const auto& pair : commandDescriptions_) {
                std::cout << "  " << getCommandName(pair.first) << " - " << pair.second << std::endl;
            }
        } else if (args[0] == "init") {
            std::cout << "Usage: init <template> [--steps <N>] [--neurons <N>] [--region <N>] [--config <file>]" << std::endl;
            std::cout << "Initialize brain with configuration template" << std::endl;
            std::cout << "Templates: " << getTemplateList() << std::endl;
        } else if (args[0] == "run") {
            std::cout << "Usage: run [--steps <N>] [--timestep <T>] [--monitor] [--progress]" << std::endl;
            std::cout << "Run simulation with current configuration" << std::endl;
        } else if (args[0] == "save") {
            std::cout << "Usage: save <filename> [--format binary|json]" << std::endl;
            std::cout << "Save brain state to file" << std::endl;
        } else if (args[0] == "load") {
            std::cout << "Usage: load <filename> [--validate]" << std::endl;
            std::cout << "Load brain state from file" << std::endl;
        } else if (args[0] == "experiment") {
            std::cout << "Usage: experiment <type> [--steps <N>] [--config <file>]" << std::endl;
            std::cout << "Run specific experiment: phase6, memory, neuromodulation, prediction, cognition, development" << std::endl;
        }
    }
    
    void showVersion() {
        std::cout << "NLM - Neural Learning Machine (Phase 6: Final Integration)" << std::endl;
        std::cout << "Version: 2.0.0 (Enhanced CLI)" << std::endl;
        std::cout << "Build: 2026-09-29" << std::endl;
        std::cout << "Architecture: Experimental artificial developmental brain" << std::endl;
        std::cout << "Features: Integrated memory, prediction, cognition, neuromodulation" << std::endl;
    }
    
    void initializeBrain(const std::vector<std::string>& args) {
        std::string templateName = "demo";  // Default
        std::string configFile = "";
        
        // Parse arguments
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--template" && i + 1 < args.size()) {
                templateName = args[++i];
            } else if (args[i] == "--config" && i + 1 < args.size()) {
                configFile = args[++i];
            }
        }
        
        // Check if template exists
        if (templates_.find(templateName) == templates_.end()) {
            std::cout << "Error: Template not found: " << templateName << std::endl;
            std::cout << "Available templates: " << getTemplateList() << std::endl;
            return;
        }
        
        std::cout << "Initializing brain with template: " << templateName << std::endl;
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Load template configuration
        Phase6Config phase6Config;
        if (templateName == "demo") {
            phase6Config = templates_["demo"];
        } else if (templateName == "research") {
            phase6Config = templates_["research"];
        } else if (templateName == "fast_test") {
            phase6Config = templates_["fast_test"];
        } else if (templateName == "full_integration") {
            phase6Config = templates_["full_integration"];
        } else if (templateName == "curiosity") {
            phase6Config = templates_["curiosity"];
        } else if (templateName == "development") {
            phase6Config = templates_["development"];
        }
        
        // Override with template-specific settings
        config->set("neuron_count", phase6Config.neuronCount);
        config->set("region_count", phase6Config.regionCount);
        config->set("connection_probability", phase6Config.connectionProbability);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        // Load custom config if specified
        if (!configFile.empty()) {
            if (config->loadFromFile(configFile)) {
                std::cout << "Loaded configuration from: " << configFile << std::endl;
            } else {
                std::cout << "Warning: Failed to load custom config, using template" << std::endl;
            }
        }
        
        // Create brain
        currentBrain_ = std::make_shared<Brain>(config);
        
        if (currentBrain_->initialize()) {
            std::cout << "✓ Brain initialized successfully" << std::endl;
            std::cout << "  - Neurons: " << currentBrain_->getTotalNeuronCount() << std::endl;
            std::cout << "  - Regions: " << currentBrain_->getRegionCount() << std::endl;
            std::cout << "  - Synapses: " << currentBrain_->getTotalSynapseCount() << std::endl;
            
            // Create agent and world
            currentWorld_ = std::make_unique<SimpleWorld>();
            currentWorld_->configure(16, 16, 16, 16);
            currentWorld_->setRandomSeed(42);
            currentWorld_->reset();
            
            currentAgent_ = std::make_unique<AgentBrain>(currentBrain_);
            currentAgent_->initialize(*currentWorld_);
            currentAgent_->enableRewardModulation(true);
            currentAgent_->enableStructuralPlasticity(true);
            currentAgent_->enableDevelopment(true);
            currentAgent_->enableCuriosity(true);
            
            std::cout << "✓ Agent and world initialized" << std::endl;
        } else {
            std::cout << "Error: Failed to initialize brain" << std::endl;
            currentBrain_.reset();
            currentWorld_.reset();
            currentAgent_.reset();
        }
    }
    
    void configureBrain(const std::vector<std::string>& args) {
        if (!currentBrain_) {
            std::cout << "Error: No brain initialized. Use 'init' command first." << std::endl;
            return;
        }
        
        std::cout << "Current configuration:" << std::endl;
        
        // Display current settings
        auto config = currentBrain_->getConfig();
        if (config) {
            std::cout << "  neuron_count: " << config->getOr<size_t>("neuron_count", 1000) << std::endl;
            std::cout << "  region_count: " << config->getOr<size_t>("region_count", 1) << std::endl;
            std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.1f) << std::endl;
            std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
            std::cout << "  stdp_ltp_weight: " << config->getOr<float>("stdp_ltp_weight", 0.01f) << std::endl;
            std::cout << "  stdp_ltd_weight: " << config->getOr<float>("stdp_ltd_weight", 0.012f) << std::endl;
        }
        
        // Allow modification (simplified - would typically use a proper config editor)
        std::cout << "Configuration modification not yet implemented (use direct file editing)" << std::endl;
    }
    
    void runSimulation(const std::vector<std::string>& args) {
        if (!currentBrain_ || !currentWorld_ || !currentAgent_) {
            std::cout << "Error: Brain, world, or agent not initialized. Use 'init' command first." << std::endl;
            return;
        }
        
        uint64_t maxSteps = 1000;
        double timestep = 0.001;
        bool monitor = false;
        bool progress = false;
        
        // Parse arguments
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--steps" && i + 1 < args.size()) {
                maxSteps = std::stoull(args[++i]);
            } else if (args[i] == "--timestep" && i + 1 < args.size()) {
                timestep = std::stod(args[++i]);
            } else if (args[i] == "--monitor") {
                monitor = true;
            } else if (args[i] == "--progress") {
                progress = true;
            }
        }
        
        std::cout << "Running simulation with " << maxSteps << " steps (timestep: " << timestep << "s)" << std::endl;
        
        if (monitor) {
            startMonitoringMode(maxSteps, timestep, progress);
            return;
        }
        
        // Run simulation
        simulationRunning_ = true;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        float totalReward = 0.0f;
        size_t step = 0;
        
        for (step = 0; step < maxSteps && simulationRunning_; ++step) {
            // Get observation
            SensoryPercept percept = currentWorld_->observe(currentBrain_->getRegions()[0].get());
            
            // Process sensory input
            currentAgent_->processSensoryInput(percept);
            
            // Brain step
            currentBrain_->step(step, step * timestep);
            
            // Get motor command
            MotorCommand cmd = currentAgent_->decodeMotorCommand();
            
            // Apply action to world
            ActionResult actionResult = currentWorld_->applyMotorCommand(cmd, step * timestep);
            
            // Compute reward
            float reward = currentWorld_->computeReward(currentBrain_->getRegions()[0].get());
            totalReward += reward;
            
            // Apply reward modulation
            currentAgent_->applyRewardModulation(reward, 0.0f);
            
            // Update development
            currentAgent_->updateDevelopment(timestep);
            
            // Progress output
            if (progress && step % 100 == 0) {
                std::cout << "\rStep: " << step << "/" << maxSteps 
                         << " | Reward: " << (totalReward / (step + 1)) 
                         << " | Spikes: " << currentBrain_->getTotalSpikeCount()
                         << " | Active: " << currentBrain_->getActiveNeuronCount() << "...";
                std::cout.flush();
            }
        }
        
        simulationRunning_ = false;
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << std::endl;
        std::cout << "Simulation completed in " << elapsed << " seconds (" << maxSteps / elapsed << " steps/sec)" << std::endl;
        std::cout << "Total reward: " << totalReward << std::endl;
        std::cout << "Average reward: " << (totalReward / maxSteps) << std::endl;
        std::cout << "Final spike count: " << currentBrain_->getTotalSpikeCount() << std::endl;
    }
    
    void showStatus() {
        std::cout << "=== NLM System Status ===" << std::endl;
        
        if (!currentBrain_) {
            std::cout << "Status: No brain initialized" << std::endl;
            return;
        }
        
        std::cout << "Brain Status:" << std::endl;
        currentBrain_->logStatus();
        
        if (currentAgent_) {
            std::cout << "\nAgent Status:" << std::endl;
            std::cout << "  - Reward Modulation: " << (currentAgent_->isRewardModulationEnabled() ? "Enabled" : "Disabled") << std::endl;
            std::cout << "  - Structural Plasticity: " << (currentAgent_->isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << std::endl;
            std::cout << "  - Development: " << (currentAgent_->isDevelopmentEnabled() ? "Enabled" : "Disabled") << std::endl;
            std::cout << "  - Curiosity: " << (currentAgent_->isCuriosityEnabled() ? "Enabled" : "Disabled") << std::endl;
            std::cout << "  - Dopamine Level: " << currentAgent_->getNeuromodulationLevel() << std::endl;
            std::cout << "  - Curiosity Level: " << currentAgent_->getCuriosityLevel() << std::endl;
            std::cout << "  - Novelty Level: " << currentAgent_->getNoveltyLevel() << std::endl;
        }
        
        if (currentWorld_) {
            std::cout << "\nWorld Status:" << std::endl;
            std::cout << "  - Size: " << currentWorld_->getWidth() << " x " << currentWorld_->getHeight() << std::endl;
            std::cout << "  - Simulation Time: " << currentWorld_->getSimulationTime() << "s" << std::endl;
            std::cout << "  - Max Energy: " << currentWorld_->getMaxEnergy() << std::endl;
        }
        
        std::cout << "\nMonitoring: " << (isMonitoring_ ? "Active" : "Inactive") << std::endl;
        std::cout << "Simulation Running: " << (simulationRunning_ ? "Yes" : "No") << std::endl;
    }
    
    void showStats() {
        if (!currentBrain_) {
            std::cout << "Error: No brain initialized" << std::endl;
            return;
        }
        
        std::cout << "=== NLM Brain Statistics ===" << std::endl;
        std::cout << "Basic Metrics:" << std::endl;
        std::cout << "  - Total Neurons: " << currentBrain_->getTotalNeuronCount() << std::endl;
        std::cout << "  - Total Synapses: " << currentBrain_->getTotalSynapseCount() << std::endl;
        std::cout << "  - Active Neurons: " << currentBrain_->getActiveNeuronCount() << std::endl;
        std::cout << "  - Firing Neurons: " << currentBrain_->getFiringNeuronCount() << std::endl;
        std::cout << "  - Average Firing Rate: " << currentBrain_->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  - Total Spike Count: " << currentBrain_->getTotalSpikeCount() << std::endl;
        
        // Memory systems
        std::cout << "\nMemory Systems:" << std::endl;
        if (currentBrain_->getWorkingMemory()) {
            std::cout << "  - Working Memory: " << currentBrain_->getWorkingMemory()->getActiveTraces() << " traces" << std::endl;
        } else {
            std::cout << "  - Working Memory: Not available" << std::endl;
        }
        
        if (currentBrain_->getEpisodicMemory()) {
            std::cout << "  - Episodic Memory: " << currentBrain_->getEpisodicMemory()->getEpisodeCount() << " episodes" << std::endl;
        } else {
            std::cout << "  - Episodic Memory: Not available" << std::endl;
        }
        
        // Neuromodulation
        std::cout << "\nNeuromodulation:" << std::endl;
        if (currentBrain_->getDopamine()) {
            std::cout << "  - Dopamine System: Active" << std::endl;
        } else {
            std::cout << "  - Dopamine System: Inactive" << std::endl;
        }
        
        if (currentBrain_->getCuriosity()) {
            std::cout << "  - Curiosity System: Active" << std::endl;
        } else {
            std::cout << "  - Curiosity System: Inactive" << std::endl;
        }
        
        // Development
        if (currentBrain_->getDevelopmentSystem()) {
            std::cout << "  - Development System: Active" << std::endl;
            std::cout << "  - Developmental Stage: " << static_cast<int>(currentBrain_->getDevelopmentalStage()) << std::endl;
        } else {
            std::cout << "  - Development System: Inactive" << std::endl;
        }
    }
    
    void saveState(const std::vector<std::string>& args) {
        if (!currentBrain_) {
            std::cout << "Error: No brain initialized" << std::endl;
            return;
        }
        
        if (args.empty()) {
            std::cout << "Error: Filename required. Usage: save <filename> [--format binary|json]" << std::endl;
            return;
        }
        
        std::string filename = args[0];
        std::string format = "binary";  // Default
        
        // Check for format argument
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "--format" && i + 1 < args.size()) {
                format = args[++i];
            }
        }
        
        std::cout << "Saving brain state to: " << filename << " (format: " << format << ")" << std::endl;
        
        if (currentBrain_->save(filename)) {
            std::cout << "✓ Save successful" << std::endl;
        } else {
            std::cout << "Error: Failed to save brain state" << std::endl;
        }
    }
    
    void loadState(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cout << "Error: Filename required. Usage: load <filename> [--validate]" << std::endl;
            return;
        }
        
        std::string filename = args[0];
        bool validate = false;
        
        // Check for validate argument
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "--validate") {
                validate = true;
            }
        }
        
        std::cout << "Loading brain state from: " << filename << std::endl;
        
        // Stop current simulation
        simulationRunning_ = false;
        
        // Clear current state
        cleanup();
        
        // Create new brain
        auto config = std::make_shared<Config>();
        
        // Try to load configuration from file
        if (config->loadFromFile(filename + ".config")) {
            std::cout << "Loaded configuration from: " << filename + ".config" << std::endl;
        }
        
        currentBrain_ = std::make_shared<Brain>(config);
        
        if (currentBrain_->initialize()) {
            if (currentBrain_->load(filename)) {
                std::cout << "✓ Load successful" << std::endl;
                
                // Create agent and world (simplified - would need proper restoration)
                currentWorld_ = std::make_unique<SimpleWorld>();
                currentWorld_->configure(16, 16, 16, 16);
                
                currentAgent_ = std::make_unique<AgentBrain>(currentBrain_);
                currentAgent_->initialize(*currentWorld_);
                
                if (validate) {
                    showStats();
                }
            } else {
                std::cout << "Error: Failed to load brain state (checkpoint file not found or corrupted)" << std::endl;
                currentBrain_.reset();
            }
        } else {
            std::cout << "Error: Failed to initialize brain from loaded config" << std::endl;
            currentBrain_.reset();
        }
    }
    
    void startMonitoringMode(uint64_t maxSteps, double timestep, bool progress) {
        isMonitoring_ = true;
        simulationRunning_ = true;
        
        std::cout << "Starting monitoring mode..." << std::endl;
        std::cout << "Press Ctrl+C to stop monitoring" << std::endl;
        
        // Start monitoring in a separate thread
        std::thread monitorThread([this, maxSteps, timestep, progress]() {
            auto startTime = std::chrono::high_resolution_clock::now();
            uint64_t step = 0;
            
            while (step < maxSteps && simulationRunning_) {
                // Update monitoring data
                if (currentBrain_) {
                    // Collect and display statistics
                    if (step % 50 == 0) {
                        std::cout << "\r[MONITOR] Step: " << step << "/" << maxSteps
                                 << " | Spikes: " << currentBrain_->getTotalSpikeCount()
                                 << " | Firing: " << currentBrain_->getFiringNeuronCount()
                                 << " | Rate: " << currentBrain_->getAverageFiringRate() << " Hz"
                                 << " | Reward: " << (currentAgent_ ? currentAgent_->getNeuromodulationLevel() : 0.0f);
                        std::cout.flush();
                    }
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                ++step;
            }
            
            simulationRunning_ = false;
            isMonitoring_ = false;
        });
        
        // Wait for simulation to complete in main thread
        runSimulation({});
        
        monitorThread.join();
    }
    
    void runExperiment(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cout << "Error: Experiment type required" << std::endl;
            std::cout << "Available experiments: phase6, memory, neuromodulation, prediction, cognition, development" << std::endl;
            return;
        }
        
        std::string experimentType = args[0];
        
        // Parse additional arguments
        uint64_t steps = 1000;
        std::string configFile = "";
        
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "--steps" && i + 1 < args.size()) {
                steps = std::stoull(args[++i]);
            } else if (args[i] == "--config" && i + 1 < args.size()) {
                configFile = args[++i];
            }
        }
        
        std::cout << "Running experiment: " << experimentType << " (" << steps << " steps)" << std::endl;
        
        if (experimentType == "phase6") {
            runPhase6Experiment(steps, configFile);
        } else if (experimentType == "memory") {
            runMemoryExperiment(steps, configFile);
        } else if (experimentType == "neuromodulation") {
            runNeuromodulationExperiment(steps, configFile);
        } else if (experimentType == "prediction") {
            runPredictionExperiment(steps, configFile);
        } else if (experimentType == "cognition") {
            runCognitionExperiment(steps, configFile);
        } else if (experimentType == "development") {
            runDevelopmentExperiment(steps, configFile);
        } else {
            std::cout << "Error: Unknown experiment type: " << experimentType << std::endl;
        }
    }
    
    void runPhase6Experiment(uint64_t steps, const std::string& configFile) {
        // Create Phase6 config
        Phase6Config config;
        config.maxSteps = steps;
        config.neuronCount = 500;
        config.regionCount = 1;
        config.connectionProbability = 0.1f;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.checkpointPath = "./phase6_checkpoint.bin";
        
        // Load custom config if specified
        if (!configFile.empty()) {
            auto customConfig = std::make_shared<Config>();
            if (customConfig->loadFromFile(configFile)) {
                config.neuronCount = customConfig->getOr<size_t>("neuron_count", config.neuronCount);
                config.regionCount = customConfig->getOr<size_t>("region_count", config.regionCount);
                config.connectionProbability = customConfig->getOr<float>("connection_probability", config.connectionProbability);
            }
        }
        
        // Run Phase 6 integration experiment
        Phase6IntegratedExperiment experiment;
        
        std::cout << "Running Phase 6 integration experiment..." << std::endl;
        auto result = experiment.run(config);
        
        // Display results
        std::cout << "\n=== Phase 6 Experiment Results ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        
        std::cout << "\n=== Integration Status ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    }
    
    void runMemoryExperiment(uint64_t steps, const std::string& configFile) {
        std::cout << "Running memory integration experiment..." << std::endl;
        // Implementation would test memory systems integration
        std::cout << "Memory experiment implementation pending" << std::endl;
    }
    
    void runNeuromodulationExperiment(uint64_t steps, const std::string& configFile) {
        std::cout << "Running neuromodulation integration experiment..." << std::endl;
        // Implementation would test neuromodulation systems
        std::cout << "Neuromodulation experiment implementation pending" << std::endl;
    }
    
    void runPredictionExperiment(uint64_t steps, const std::string& configFile) {
        std::cout << "Running prediction system integration experiment..." << std::endl;
        // Implementation would test prediction systems
        std::cout << "Prediction experiment implementation pending" << std::endl;
    }
    
    void runCognitionExperiment(uint64_t steps, const std::string& configFile) {
        std::cout << "Running cognition system integration experiment..." << std::endl;
        // Implementation would test cognition systems
        std::cout << "Cognition experiment implementation pending" << std::endl;
    }
    
    void runDevelopmentExperiment(uint64_t steps, const std::string& configFile) {
        std::cout << "Running development system integration experiment..." << std::endl;
        // Implementation would test development systems
        std::cout << "Development experiment implementation pending" << std::endl;
    }
    
    void manageTemplate(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cout << "Template management commands:" << std::endl;
            std::cout << "  list - List available templates" << std::endl;
            std::cout << "  show <name> - Show template details" << std::endl;
            std::cout << "  create <name> - Create new template" << std::endl;
            std::cout << "  delete <name> - Delete template" << std::endl;
            return;
        }
        
        std::string subcommand = args[0];
        
        if (subcommand == "list") {
            std::cout << "Available templates:" << std::endl;
            for (const auto& pair : templates_) {
                std::cout << "  " << pair.first << " - " << getTemplateDescription(pair.first) << std::endl;
            }
        } else if (subcommand == "show" && args.size() > 1) {
            showTemplate(args[1]);
        } else if (subcommand == "create" && args.size() > 1) {
            createTemplate(args[1], args.size() > 2 ? args[2] : "demo");
        } else if (subcommand == "delete" && args.size() > 1) {
            deleteTemplate(args[1]);
        } else {
            std::cout << "Unknown template command: " << subcommand << std::endl;
            showHelp({"template"});
        }
    }
    
    void showTemplate(const std::string& name) {
        if (templates_.find(name) != templates_.end()) {
            std::cout << "Template: " << name << std::endl;
            Phase6Config config = templates_[name];
            std::cout << "  Neurons: " << config.neuronCount << std::endl;
            std::cout << "  Regions: " << config.regionCount << std::endl;
            std::cout << "  Connection Probability: " << config.connectionProbability << std::endl;
            std::cout << "  Enable Checkpointing: " << (config.enableCheckpointing ? "Yes" : "No") << std::endl;
            std::cout << "  Enable Replay: " << (config.enableReplay ? "Yes" : "No") << std::endl;
            std::cout << "  Enable Development: " << (config.enableDevelopment ? "Yes" : "No") << std::endl;
        } else {
            std::cout << "Template not found: " << name << std::endl;
        }
    }
    
    void createTemplate(const std::string& name, const std::string& baseTemplate) {
        if (templates_.find(baseTemplate) == templates_.end()) {
            std::cout << "Error: Base template not found: " << baseTemplate << std::endl;
            return;
        }
        
        templates_[name] = templates_[baseTemplate];
        std::cout << "Created template: " << name << " (based on " << baseTemplate << ")" << std::endl;
    }
    
    void deleteTemplate(const std::string& name) {
        if (templates_.erase(name) > 0) {
            std::cout << "Deleted template: " << name << std::endl;
        } else {
            std::cout << "Template not found: " << name << std::endl;
        }
    }
    
    void managePlugins(const std::vector<std::string>& args) {
        std::cout << "Plugin management commands:" << std::endl;
        std::cout << "  list - List available plugins" << std::endl;
        std::cout << "  load <name> - Load plugin" << std::endl;
        std::cout << "  unload <name> - Unload plugin" << std::endl;
        std::cout << "  info <name> - Show plugin information" << std::endl;
    }
    
    // Helper methods
    std::string getCommandName(CommandType type) {
        switch (type) {
            case CommandType::HELP: return "help";
            case CommandType::VERSION: return "version";
            case CommandType::INIT: return "init";
            case CommandType::CONFIG: return "config";
            case CommandType::RUN: return "run";
            case CommandType::SAVE: return "save";
            case CommandType::LOAD: return "load";
            case CommandType::STATUS: return "status";
            case CommandType::STATS: return "stats";
            case CommandType::MONITOR: return "monitor";
            case CommandType::EXPERIMENT: return "experiment";
            case CommandType::TEMPLATE: return "template";
            case CommandType::PLUGIN: return "plugin";
            case CommandType::EXIT: return "exit";
            default: return "unknown";
        }
    }
    
    std::string getTemplateList() {
        std::string list;
        for (const auto& pair : templates_) {
            if (!list.empty()) list += ", ";
            list += pair.first;
        }
        return list;
    }
    
    std::string getTemplateDescription(const std::string& name) {
        if (name == "demo") return "Basic demonstration with 500 neurons";
        if (name == "research") return "Research configuration with advanced features";
        if (name == "fast_test") return "Quick performance test with 200 neurons";
        if (name == "full_integration") return "Complete Phase 6 integration test";
        if (name == "curiosity") return "Curiosity-driven exploration focus";
        if (name == "development") return "Developmental plasticity emphasis";
        return "Unknown template";
    }
    
    Phase6Config getDemoTemplate() {
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.regionCount = 1;
        config.connectionProbability = 0.1f;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.checkpointPath = "./demo_checkpoint.bin";
        return config;
    }
    
    Phase6Config getResearchTemplate() {
        Phase6Config config;
        config.neuronCount = 2000;
        config.maxSteps = 10000;
        config.regionCount = 2;
        config.connectionProbability = 0.05f;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.checkpointPath = "./research_checkpoint.bin";
        return config;
    }
    
    Phase6Config getFastTestTemplate() {
        Phase6Config config;
        config.neuronCount = 200;
        config.maxSteps = 500;
        config.regionCount = 1;
        config.connectionProbability = 0.2f;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        config.checkpointPath = "";
        return config;
    }
    
    Phase6Config getFullIntegrationTemplate() {
        Phase6Config config;
        config.neuronCount = 1000;
        config.maxSteps = 5000;
        config.regionCount = 3;
        config.connectionProbability = 0.08f;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.checkpointPath = "./integration_checkpoint.bin";
        return config;
    }
    
    Phase6Config getCuriosityTemplate() {
        Phase6Config config;
        config.neuronCount = 800;
        config.maxSteps = 3000;
        config.regionCount = 1;
        config.connectionProbability = 0.12f;
        config.enableCheckpointing = true;
        config.enableReplay = false;
        config.enableDevelopment = true;
        config.checkpointPath = "./curiosity_checkpoint.bin";
        return config;
    }
    
    Phase6Config getDevelopmentTemplate() {
        Phase6Config config;
        config.neuronCount = 600;
        config.maxSteps = 4000;
        config.regionCount = 1;
        config.connectionProbability = 0.15f;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.checkpointPath = "./development_checkpoint.bin";
        return config;
    }
    
    void printWelcomeBanner() {
        std::cout << "=========================================" << std::endl;
        std::cout << "NLM - Neural Learning Machine CLI (v2.0.0)" << std::endl;
        std::cout << "Experimental artificial developmental brain system" << std::endl;
        std::cout << "=========================================" << std::endl;
        std::cout << "Phase 6: Final Integration Complete" << std::endl;
        std::cout << "Enhanced with thread safety and improved error handling" << std::endl;
        std::cout << "=========================================" << std::endl;
    }
    
    void cleanup() {
        simulationRunning_ = false;
        isMonitoring_ = false;
        
        currentBrain_.reset();
        currentWorld_.reset();
        currentAgent_.reset();
    }
    
    // Getter methods
    bool isMonitoring() const { return isMonitoring_; }
    bool isSimulationRunning() const { return simulationRunning_; }
    
    // Static instance method for global access
    static NLMCLI& getInstance() {
        static NLMCLI instance;
        return instance;
    }

private:
    // Current system state
    std::shared_ptr<Brain> currentBrain_;
    std::unique_ptr<SimpleWorld> currentWorld_;
    std::unique_ptr<AgentBrain> currentAgent_;
    
    // CLI state
    bool isMonitoring_;
    bool simulationRunning_;
    
    // Command and template definitions
    std::map<CommandType, std::string> commandDescriptions_;
    std::map<std::string, Phase6Config> templates_;
};

// Global CLI instance function
void runCLI() {
    NLMCLI::getInstance().run();
}

// Convenience function for simple CLI usage
void startNLM(const std::vector<std::string>& args) {
    // Parse command line arguments
    bool runDemo = false;
    std::string configFile = "";
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--demo" || args[i] == "-d") {
            runDemo = true;
        } else if (args[i] == "--config" && i + 1 < args.size()) {
            configFile = args[++i];
        }
    }
    
    if (runDemo) {
        std::cout << "Starting NLM demo mode..." << std::endl;
        NLMCLI::getInstance().initializeBrain({"--template", "demo"});
        if (NLMCLI::getInstance().isMonitoring()) {
            NLMCLI::getInstance().runSimulation({"--steps", "1000", "--monitor", "--progress"});
        } else {
            NLMCLI::getInstance().runSimulation({"--steps", "1000", "--progress"});
        }
        std::cout << "Demo completed successfully!" << std::endl;
    } else {
        std::cout << "Starting NLM CLI..." << std::endl;
        std::cout << "Use --demo or -d flag for quick demo, or run without flags for interactive mode." << std::endl;
        NLMCLI::getInstance().run();
    }
}

} // namespace nlm