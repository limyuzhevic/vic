#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../experiments/Phase6IntegratedExperiment.hpp"

namespace nlm {

class AdvancedCLInterface {
public:
    AdvancedCLInterface() {
        setupCommands();
    }
    
    void run() {
        std::cout << "=== NLM Advanced CLI Interface ===" << std::endl;
        std::cout << "Type 'help' for available commands or 'exit' to quit." << std::endl;
        
        std::string line;
        while (true) {
            std::cout << ">>> ";
            std::getline(std::cin, line);
            
            if (line.empty()) continue;
            
            // Check for exit commands
            if (line == "exit" || line == "quit") {
                std::cout << "Exiting NLM CLI. Goodbye!" << std::endl;
                break;
            }
            
            // Check for help
            if (line == "help") {
                showHelp();
                continue;
            }
            
            // Process command
            processCommand(line);
        }
    }
    
private:
    struct Command {
        std::string name;
        std::string description;
        std::string usage;
        std::function<void(const std::vector<std::string>&)> handler;
    };
    
    std::vector<Command> commands_;
    std::map<std::string, std::string> commandDescriptions_;
    std::shared_ptr<Config> currentConfig_;
    std::shared_ptr<Brain> currentBrain_;
    std::shared_ptr<SimpleWorld> currentWorld_;
    std::shared_ptr<AgentBrain> currentAgent_;
    Phase6IntegratedExperiment currentExperiment_;
    
    void setupCommands() {
        // Core system commands
        commands_.push_back({
            "create_brain",
            "Create a new brain with specified configuration",
            "create_brain <neuron_count> [region_count] [connection_probability]",
            [this](const std::vector<std::string>& args) { handleCreateBrain(args); }
        });
        
        commands_.push_back({
            "run_experiment",
            "Run a specific integration experiment",
            "run_experiment <experiment_type> [parameters]",
            [this](const std::vector<std::string>& args) { handleRunExperiment(args); }
        });
        
        commands_.push_back({
            "save_brain",
            "Save current brain state to checkpoint file",
            "save_brain <filepath>",
            [this](const std::vector<std::string>& args) { handleSaveBrain(args); }
        });
        
        commands_.push_back({
            "load_brain",
            "Load brain state from checkpoint file",
            "load_brain <filepath>",
            [this](const std::vector<std::string>& args) { handleLoadBrain(args); }
        });
        
        // World simulation commands
        commands_.push_back({
            "create_world",
            "Create a new simulation world",
            "create_world <width> <height>",
            [this](const std::vector<std::string>& args) { handleCreateWorld(args); }
        });
        
        commands_.push_back({
            "set_world_goals",
            "Set goals for world simulation",
            "set_world_goals <goal1> <goal2> ...",
            [this](const std::vector<std::string>& args) { handleSetWorldGoals(args); }
        });
        
        // Agent control commands
        commands_.push_back({
            "enable_feature",
            "Enable a specific feature",
            "enable_feature <feature_name>",
            [this](const std::vector<std::string>& args) { handleEnableFeature(args); }
        });
        
        commands_.push_back({
            "disable_feature",
            "Disable a specific feature",
            "disable_feature <feature_name>",
            [this](const std::vector<std::string>& args) { handleDisableFeature(args); }
        });
        
        // Configuration commands
        commands_.push_back({
            "set_config",
            "Set configuration value",
            "set_config <key> <value>",
            [this](const std::vector<std::string>& args) { handleSetConfig(args); }
        });
        
        commands_.push_back({
            "get_config",
            "Get configuration value",
            "get_config <key>",
            [this](const std::vector<std::string>& args) { handleGetConfig(args); }
        });
        
        // Experiment commands
        commands_.push_back({
            "run_integration_test",
            "Run complete integration test",
            "run_integration_test [steps] [neuron_count]",
            [this](const std::vector<std::string>& args) { handleRunIntegrationTest(args); }
        });
        
        commands_.push_back({
            "verify_system",
            "Verify system integration and health",
            "verify_system",
            [this](const std::vector<std::string>& args) { handleVerifySystem(args); }
        });
        
        // Performance commands
        commands_.push_back({
            "benchmark",
            "Run performance benchmark",
            "benchmark <duration> [mode]",
            [this](const std::vector<std::string>& args) { handleBenchmark(args); }
        });
        
        commands_.push_back({
            "profile",
            "Profile system performance",
            "profile <mode>",
            [this](const std::vector<std::string>& args) { handleProfile(args); }
        });
        
        // Advanced exploration commands
        commands_.push_back({
            "analyze_network",
            "Analyze network connectivity and structure",
            "analyze_network",
            [this](const std::vector<std::string>& args) { handleAnalyzeNetwork(args); }
        });
        
        commands_.push_back({
            "simulate_scenario",
            "Simulate a specific scenario",
            "simulate_scenario <scenario_file>",
            [this](const std::vector<std::string>& args) { handleSimulateScenario(args); }
        });
        
        // Utility commands
        commands_.push_back({
            "show_status",
            "Show current system status",
            "show_status",
            [this](const std::vector<std::string>& args) { handleShowStatus(args); }
        });
        
        commands_.push_back({
            "show_config",
            "Show current configuration",
            "show_config",
            [this](const std::vector<std::string>& args) { handleShowConfig(args); }
        });
        
        commands_.push_back({
            "export_state",
            "Export brain state for analysis",
            "export_state <filepath>",
            [this](const std::vector<std::string>& args) { handleExportState(args); }
        });
        
        // Batch processing
        commands_.push_back({
            "run_batch",
            "Run a batch of commands from file",
            "run_batch <batch_file>",
            [this](const std::vector<std::string>& args) { handleRunBatch(args); }
        });
        
        // Debug and development
        commands_.push_back({
            "debug",
            "Enable debug mode",
            "debug",
            [this](const std::vector<std::string>& args) { handleDebug(args); }
        });
        
        commands_.push_back({
            "trace",
            "Start execution trace",
            "trace",
            [this](const std::vector<std::string>& args) { handleTrace(args); }
        });
    }
    
    void showHelp() {
        std::cout << "=== Available Commands ===" << std::endl;
        std::cout << std::endl;
        
        for (const auto& cmd : commands_) {
            std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
            std::cout << "    Usage: " << cmd.usage << std::endl;
            std::cout << std::endl;
        }
        
        std::cout << "=== System Commands ===" << std::endl;
        std::cout << "  create_brain    - Initialize NLM brain with custom configuration" << std::endl;
        std::cout << "  run_experiment  - Execute specific experiment type" << std::endl;
        std::cout << "  save_brain      - Save brain state to checkpoint" << std::endl;
        std::cout << "  load_brain      - Load brain state from checkpoint" << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== World Commands ===" << std::endl;
        std::cout << "  create_world    - Create simulation world with dimensions" << std::endl;
        std::cout << "  set_world_goals  - Set world simulation goals" << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== Agent Commands ===" << std::endl;
        std::cout << "  enable_feature  - Enable specific agent feature" << std::endl;
        std::cout << "  disable_feature - Disable specific agent feature" << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== Configuration Commands ===" << std::endl;
        std::cout << "  set_config      - Set configuration value" << std::endl;
        std::cout << "  get_config      - Get configuration value" << std::endl;
        std::cout << "  show_config     - Display current configuration" << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== Experiment Commands ===" << std::endl;
        std::cout << "  run_integration_test - Run complete integration test" << std::endl;
        std::cout << "  verify_system   - Verify system integration and health" << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== Performance Commands ===" << std::endl;
        std::cout << "  benchmark       - Run performance benchmark" << std::endl;
        std::cout << "  profile         - Profile system performance" << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== Utility Commands ===" << std::endl;
        std::cout << "  show_status     - Display current system status" << std::endl;
        std::cout << "  export_state    - Export brain state for analysis" << std::endl;
        std::cout << "  run_batch      - Execute batch of commands" << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== Help ===" << std::endl;
        std::cout << "  help           - Show this help message" << std::endl;
        std::cout << "  exit/quit      - Exit the CLI" << std::endl;
    }
    
    void processCommand(const std::string& line) {
        std::vector<std::string> args = parseCommandLine(line);
        if (args.empty()) return;
        
        std::string commandName = args[0];
        auto it = std::find_if(commands_.begin(), commands_.end(),
            [&commandName](const Command& cmd) { return cmd.name == commandName; });n
        if (it != commands_.end()) {
            try {
                it->handler(args);
            } catch (const std::exception& e) {
                std::cerr << "Error executing command: " << e.what() << std::endl;
            }
        } else {
            std::cout << "Unknown command: " << commandName << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }
    
    std::vector<std::string> parseCommandLine(const std::string& line) {
        std::vector<std::string> args;
        std::istringstream iss(line);
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }
        return args;
    }
    
    // Command handlers
    void handleCreateBrain(const std::vector<std::string>& args) {
        if (args.size() < 1) {
            std::cout << "Error: neuron_count required for create_brain command" << std::endl;
            return;
        }
        
        size_t neuronCount = std::stoull(args[0]);
        size_t regionCount = (args.size() > 1) ? std::stoull(args[1]) : 1;
        float connectionProb = (args.size() > 2) ? std::stof(args[2]) : 0.1f;
        
        currentConfig_ = std::make_shared<Config>();
        currentConfig_->set("neuron_count", static_cast<int64_t>(neuronCount));
        currentConfig_->set("region_count", static_cast<int64_t>(regionCount));
        currentConfig_->set("connection_probability", connectionProb);
        
        currentBrain_ = std::make_shared<Brain>(currentConfig_);
        if (currentBrain_->initialize()) {
            std::cout << "Brain created successfully:" << std::endl;
            std::cout << "  Neurons: " << currentBrain_->getTotalNeuronCount() << std::endl;
            std::cout << "  Regions: " << currentBrain_->getRegionCount() << std::endl;
            std::cout << "  Synapses: " << currentBrain_->getTotalSynapseCount() << std::endl;
        } else {
            std::cout << "Error: Failed to initialize brain" << std::endl;
        }
    }
    
    void handleCreateWorld(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "Error: width and height required for create_world command" << std::endl;
            return;
        }
        
        int width = std::stoi(args[0]);
        int height = std::stoi(args[1]);
        
        currentWorld_ = std::make_shared<SimpleWorld>();
        currentWorld_->initialize(width, height);
        
        // Initialize agent if brain exists
        if (currentBrain_) {
            currentAgent_ = std::make_shared<AgentBrain>(currentBrain_);
            currentAgent_->initialize(*currentWorld_);
        }
        
        std::cout << "World created successfully: " << width << "x" << height << std::endl;
    }
    
    void handleEnableFeature(const std::vector<std::string>& args) {
        if (args.size() < 1) {
            std::cout << "Error: feature_name required for enable_feature command" << std::endl;
            return;
        }
        
        std::string feature = args[0];
        if (currentAgent_) {
            if (feature == "reward_modulation") {
                currentAgent_->enableRewardModulation(true);
            } else if (feature == "structural_plasticity") {
                currentAgent_->enableStructuralPlasticity(true);
            } else if (feature == "development") {
                currentAgent_->enableDevelopment(true);
            } else if (feature == "curiosity") {
                currentAgent_->enableCuriosity(true);
            }
            std::cout << "Feature '" << feature << "' enabled successfully" << std::endl;
        } else {
            std::cout << "Error: No agent available. Create brain and world first." << std::endl;
        }
    }
    
    void handleSetConfig(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "Error: key and value required for set_config command" << std::endl;
            return;
        }
        
        std::string key = args[0];
        std::string value = args[1];
        
        if (!currentConfig_) {
            currentConfig_ = std::make_shared<Config>();
        }
        
        // Try to parse as number first
        try {
            if (value.find('.') != std::string::npos) {
                float floatValue = std::stof(value);
                currentConfig_->set(key, floatValue);
            } else {
                int intValue = std::stoi(value);
                currentConfig_->set(key, intValue);
            }
        } catch (...) {
            // If not a number, treat as string
            currentConfig_->set(key, value);
        }
        
        std::cout << "Configuration set: " << key << " = " << value << std::endl;
    }
    
    void handleGetConfig(const std::vector<std::string>& args) {
        if (args.size() < 1) {
            std::cout << "Error: key required for get_config command" << std::endl;
            return;
        }
        
        std::string key = args[0];
        
        if (!currentConfig_) {
            std::cout << "No configuration available" << std::endl;
            return;
        }
        
        auto value = currentConfig_->get<std::string>(key);
        if (value) {
            std::cout << key << " = " << *value << std::endl;
        } else {
            std::cout << "Configuration key not found: " << key << std::endl;
        }
    }
    
    void handleRunIntegrationTest(const std::vector<std::string>& args) {
        std::cout << "Running NLM Phase 6 Integration Test..." << std::endl;
        
        Phase6Config config;
        config.neuronCount = (args.size() > 0) ? std::stoull(args[0]) : 500;
        config.maxSteps = (args.size() > 1) ? std::stoull(args[1]) : 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        auto result = currentExperiment_.run(config);
        
        std::cout << "=== Integration Test Results ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Wall time: " << result.totalWallClockTime << "s" << std::endl;
        std::cout << "Memory systems integrated: " << (result.memoryWorkingMemoryIntegrated ? "Yes" : "No") << std::endl;
        std::cout << "Neuromodulation integrated: " << (result.neuromodulationIntegrated ? "Yes" : "No") << std::endl;
    }
    
    void handleShowStatus() {
        std::cout << "=== NLM System Status ===" << std::endl;
        
        if (currentBrain_) {
            std::cout << "Brain Status:" << std::endl;
            std::cout << "  Neurons: " << currentBrain_->getTotalNeuronCount() << std::endl;
            std::cout << "  Regions: " << currentBrain_->getRegionCount() << std::endl;
            std::cout << "  Synapses: " << currentBrain_->getTotalSynapseCount() << std::endl;
            std::cout << "  Active neurons: " << currentBrain_->getActiveNeuronCount() << std::endl;
            std::cout << "  Firing neurons: " << currentBrain_->getFiringNeuronCount() << std::endl;
            std::cout << "  Total spikes: " << currentBrain_->getTotalSpikeCount() << std::endl;
        } else {
            std::cout << "Brain: Not initialized" << std::endl;
        }
        
        if (currentWorld_) {
            std::cout << "World: " << currentWorld_->getWidth() << "x" << currentWorld_->getHeight() << std::endl;
        } else {
            std::cout << "World: Not initialized" << std::endl;
        }
        
        if (currentAgent_) {
            std::cout << "Agent: Active" << std::endl;
            std::cout << "  Reward modulation: " << (currentAgent_->isRewardModulationEnabled() ? "Enabled" : "Disabled") << std::endl;
            std::cout << "  Development: " << (currentAgent_->isDevelopmentEnabled() ? "Enabled" : "Disabled") << std::endl;
            std::cout << "  Curiosity: " << (currentAgent_->isCuriosityEnabled() ? "Enabled" : "Disabled") << std::endl;
        } else {
            std::cout << "Agent: Not active" << std::endl;
        }
    }
    
    void handleShowConfig() {
        if (currentConfig_) {
            std::cout << "Current Configuration:" << std::endl;
            std::cout << currentConfig_->summary() << std::endl;
        } else {
            std::cout << "No configuration available" << std::endl;
        }
    }
    
    void handleRunBatch(const std::vector<std::string>& args) {
        if (args.size() < 1) {
            std::cout << "Error: batch_file required for run_batch command" << std::endl;
            return;
        }
        
        std::cout << "Running batch commands from: " << args[0] << std::endl;
        std::cout << "(Batch processing functionality is a placeholder - implement actual batch processing)" << std::endl;
    }
    
    void handleDebug(const std::vector<std::string>& args) {
        std::cout << "Debug mode enabled. Additional debugging information would be displayed here." << std::endl;
        std::cout << "Current state:" << std::endl;
        handleShowStatus();
    }
    
    void handleTrace(const std::vector<std::string>& args) {
        std::cout << "Execution trace started. Tracing is a placeholder implementation." << std::endl;
    }
    
    // Placeholder implementations for other commands
    void handleSaveBrain(const std::vector<std::string>& args) {
        if (args.size() < 1) {
            std::cout << "Error: filepath required for save_brain command" << std::endl;
            return;
        }
        
        if (currentBrain_) {
            if (currentBrain_->save(args[0])) {
                std::cout << "Brain saved successfully to: " << args[0] << std::endl;
            } else {
                std::cout << "Error: Failed to save brain" << std::endl;
            }
        } else {
            std::cout << "Error: No brain to save" << std::endl;
        }
    }
    
    void handleLoadBrain(const std::vector<std::string>& args) {
        if (args.size() < 1) {
            std::cout << "Error: filepath required for load_brain command" << std::endl;
            return;
        }
        
        if (!currentConfig_) {
            currentConfig_ = std::make_shared<Config>();
        }
        
        currentBrain_ = std::make_shared<Brain>(currentConfig_);
        if (currentBrain_->initialize() && currentBrain_->load(args[0])) {
            std::cout << "Brain loaded successfully from: " << args[0] << std::endl;
        } else {
            std::cout << "Error: Failed to load brain" << std::endl;
        }
    }
    
    void handleRunExperiment(const std::vector<std::string>& args) {
        std::cout << "Running experiment... (placeholder implementation)" << std::endl;
    }
    
    void handleSetWorldGoals(const std::vector<std::string>& args) {
        std::cout << "World goals set... (placeholder implementation)" << std::endl;
    }
    
    void handleDisableFeature(const std::vector<std::string>& args) {
        std::cout << "Feature disabled... (placeholder implementation)" << std::endl;
    }
    
    void handleVerifySystem(const std::vector<std::string>& args) {
        std::cout << "System verification... (placeholder implementation)" << std::endl;
        if (currentExperiment_.verifyIntegration()) {
            std::cout << "✓ System integration verified" << std::endl;
        } else {
            std::cout << "✗ System integration verification failed" << std::endl;
        }
    }
    
    void handleBenchmark(const std::vector<std::string>& args) {
        std::cout << "Running benchmark... (placeholder implementation)" << std::endl;
    }
    
    void handleProfile(const std::vector<std::string>& args) {
        std::cout << "Profiling... (placeholder implementation)" << std::endl;
    }
    
    void handleAnalyzeNetwork(const std::vector<std::string>& args) {
        std::cout << "Network analysis... (placeholder implementation)" << std::endl;
    }
    
    void handleSimulateScenario(const std::vector<std::string>& args) {
        std::cout << "Scenario simulation... (placeholder implementation)" << std::endl;
    }
    
    void handleExportState(const std::vector<std::string>& args) {
        if (args.size() < 1) {
            std::cout << "Error: filepath required for export_state command" << std::endl;
            return;
        }
        
        std::cout << "Exporting state to: " << args[0] << "... (placeholder implementation)" << std::endl;
    }
};  // namespace nlm

// Entry point for advanced CLI interface
int main(int argc, char** argv) {
    AdvancedCLInterface cli;
    cli.run();
    return 0;
}
