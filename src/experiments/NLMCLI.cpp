// NLM CLI (Neural Learning Machine Command Line Interface)
// Unified interface for NLM simulation and management

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <chrono>

namespace nlm {

class NLMCLI {
public:
    NLMCLI() : running_(false), currentBrain_(nullptr) {
        logger_ = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger_->addLogger(consoleLogger);
        Logger::setGlobal(logger_);
    }
    
    ~NLMCLI() {}
    
    void run() {
        printBanner();
        running_ = true;
        
        std::cout << "NLM CLI - Neural Learning Machine Command Line Interface" << std::endl;
        std::cout << "Type 'help' for available commands" << std::endl;
        std::cout << std::endl;
        
        while (running_) {
            std::cout << "nlm-cli> ";
            std::string command;
            std::getline(std::cin, command);
            
            if (command.empty()) continue;
            
            handleCommand(command);
        }
    }
    
    void handleCommand(const std::string& command) {
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        
        auto it = commands_.find(cmd);
        if (it != commands_.end()) {
            it->second();
        } else {
            std::cout << "Unknown command: " << cmd << std::endl;
            printHelp();
        }
    }
    
    void registerCommands() {
        commands_["create-brain"] = [this]() { cmdCreateBrain(); };
        commands_["reset"] = [this]() { cmdReset(); };
        commands_["step"] = [this]() { cmdStep(); };
        commands_["status"] = [this]() { cmdStatus(); };
        commands_["save"] = [this]() { cmdSave(); };
        commands_["load"] = [this]() { cmdLoad(); };
        commands_["enable-subsystem"] = [this]() { cmdEnableSubsystem(); };
        commands_["disable-subsystem"] = [this]() { cmdDisableSubsystem(); };
        commands_["list-subsystems"] = [this]() { cmdListSubsystems(); };
        commands_["run"] = [this]() { cmdRun(); };
        commands_["pause"] = [this]() { cmdPause(); };
        commands_["stop"] = [this]() { cmdStop(); };
        commands_["monitor"] = [this]() { cmdMonitor(); };
        commands_["benchmark"] = [this]() { cmdBenchmark(); };
        commands_["stats"] = [this]() { cmdStats(); };
        commands_["config"] = [this]() { cmdConfig(); };
        commands_["help"] = [this]() { printHelp(); };
        commands_["exit"] = [this]() { cmdExit(); };
        commands_["quit"] = [this]() { cmdExit(); };
    }
    
    void cmdCreateBrain() {
        std::cout << "Creating brain with configuration..." << std::endl;
        auto config = std::make_shared<Config>();
        
        config->set("neuron_count", 1000);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 42u);
        
        currentBrain_ = std::make_shared<Brain>(config);
        currentBrain_->initialize();
        
        std::cout << "Brain created successfully!" << std::endl;
        std::cout << "  Neurons: " << currentBrain_->getTotalNeuronCount() << std::endl;
        std::cout << "  Synapses: " << currentBrain_->getTotalSynapseCount() << std::endl;
    }
    
    void cmdReset() {
        if (!currentBrain_) {
            std::cout << "No brain created yet. Use 'create-brain' first." << std::endl;
            return;
        }
        
        currentBrain_->reset();
        currentBrain_->initialize();
        std::cout << "Brain reset and reinitialized." << std::endl;
    }
    
    void cmdStep() {
        if (!currentBrain_) {
            std::cout << "No brain created yet. Use 'create-brain' first." << std::endl;
            return;
        }
        
        static SimulationStep step = 0;
        currentBrain_->step(step++, step * 0.001);
        std::cout << "Completed step " << step-1 << std::endl;
        
        std::cout << "  Firing neurons: " << currentBrain_->getFiringNeuronCount() << std::endl;
        std::cout << "  Total spikes: " << currentBrain_->getTotalSpikeCount() << std::endl;
        std::cout << "  Average firing rate: " << currentBrain_->getAverageFiringRate() << std::endl;
    }
    
    void cmdStatus() {
        if (!currentBrain_) {
            std::cout << "No brain created yet. Use 'create-brain' first." << std::endl;
            return;
        }
        
        std::cout << "=== Brain Status ===" << std::endl;
        std::cout << "Neurons: " << currentBrain_->getTotalNeuronCount() << std::endl;
        std::cout << "Synapses: " << currentBrain_->getTotalSynapseCount() << std::endl;
        std::cout << "Active neurons: " << currentBrain_->getActiveNeuronCount() << std::endl;
        std::cout << "Firing neurons: " << currentBrain_->getFiringNeuronCount() << std::endl;
        std::cout << "Total spikes: " << currentBrain_->getTotalSpikeCount() << std::endl;
        std::cout << "Average firing rate: " << currentBrain_->getAverageFiringRate() << std::endl;
        std::cout << "E/I ratio: " << currentBrain_->getExcitationInhibitionRatio() << std::endl;
        std::cout << "Developmental stage: " << static_cast<int>(currentBrain_->getDevelopmentalStage()) << std::endl;
    }
    
    void cmdSave() {
        if (!currentBrain_) {
            std::cout << "No brain created yet. Use 'create-brain' first." << std::endl;
            return;
        }
        
        std::cout << "Enter filename to save brain state: ";
        std::string filename;
        std::getline(std::cin >> std::ws, filename);
        
        if (filename.empty()) {
            std::cout << "Filename cannot be empty." << std::endl;
            return;
        }
        
        if (currentBrain_->save(filename)) {
            std::cout << "Brain saved to " << filename << std::endl;
        } else {
            std::cout << "Failed to save brain to " << filename << std::endl;
        }
    }
    
    void cmdLoad() {
        std::cout << "Enter filename to load brain state: ";
        std::string filename;
        std::getline(std::cin >> std::ws, filename);
        
        if (filename.empty()) {
            std::cout << "Filename cannot be empty." << std::endl;
            return;
        }
        
        if (!currentBrain_) {
            auto config = std::make_shared<Config>();
            currentBrain_ = std::make_shared<Brain>(config);
        }
        
        if (currentBrain_->load(filename)) {
            std::cout << "Brain loaded from " << filename << std::endl;
        } else {
            std::cout << "Failed to load brain from " << filename << std::endl;
        }
    }
    
    void cmdEnableSubsystem() {
        std::cout << "Enter subsystem name to enable: ";
        std::string subsystem;
        std::getline(std::cin >> std::ws, subsystem);
        
        if (currentAgent_) {
            if (subsystem == "reward_modulation") {
                currentAgent_->enableRewardModulation(true);
            } else if (subsystem == "structural_plasticity") {
                currentAgent_->enableStructuralPlasticity(true);
            } else if (subsystem == "development") {
                currentAgent_->enableDevelopment(true);
            } else if (subsystem == "curiosity") {
                currentAgent_->enableCuriosity(true);
            } else {
                std::cout << "Unknown subsystem: " << subsystem << std::endl;
            }
        }
    }
    
    void cmdDisableSubsystem() {
        std::cout << "Enter subsystem name to disable: ";
        std::string subsystem;
        std::getline(std::cin >> std::ws, subsystem);
        
        if (currentAgent_) {
            if (subsystem == "reward_modulation") {
                currentAgent_->enableRewardModulation(false);
            } else if (subsystem == "structural_plasticity") {
                currentAgent_->enableStructuralPlasticity(false);
            } else if (subsystem == "development") {
                currentAgent_->enableDevelopment(false);
            } else if (subsystem == "curiosity") {
                currentAgent_->enableCuriosity(false);
            } else {
                std::cout << "Unknown subsystem: " << subsystem << std::endl;
            }
        }
    }
    
    void cmdListSubsystems() {
        std::cout << "=== Enabled Subsystems ===" << std::endl;
        if (currentAgent_) {
            std::cout << "Reward Modulation: " << (currentAgent_->isRewardModulationEnabled() ? "Enabled" : "Disabled") << std::endl;
            std::cout << "Structural Plasticity: " << (currentAgent_->isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << std::endl;
            std::cout << "Development: " << (currentAgent_->isDevelopmentEnabled() ? "Enabled" : "Disabled") << std::endl;
            std::cout << "Curiosity: " << (currentAgent_->isCuriosityEnabled() ? "Enabled" : "Disabled") << std::endl;
        }
    }
    
    void cmdRun() {
        std::cout << "Starting simulation..." << std::endl;
        std::cout << "Simulation started. Press Ctrl+C to stop." << std::endl;
    }
    
    void cmdPause() {
        std::cout << "Pause not yet implemented." << std::endl;
    }
    
    void cmdStop() {
        running_ = false;
        std::cout << "Stopping simulation..." << std::endl;
    }
    
    void cmdMonitor() {
        std::cout << "Starting monitor (not yet implemented)..." << std::endl;
    }
    
    void cmdBenchmark() {
        std::cout << "Running benchmark (not yet implemented)..." << std::endl;
    }
    
    void cmdStats() {
        std::cout << "Statistics (not yet implemented)..." << std::endl;
    }
    
    void cmdConfig() {
        std::cout << "=== Configuration Options ===" << std::endl;
        std::cout << "1. List all configuration keys" << std::endl;
        std::cout << "2. Set configuration value" << std::endl;
        std::cout << "3. Load configuration from file" << std::endl;
        std::cout << "4. Save configuration to file" << std::endl;
        
        std::cout << "Enter option (1-4): ";
        std::string choice;
        std::getline(std::cin >> std::ws, choice);
        
        if (choice == "1") {
            if (currentBrain_) {
                auto config = currentBrain_->getConfig();
                std::vector<std::string> keys = config->getKeys();
                std::cout << "Configuration keys:" << std::endl;
                for (const auto& key : keys) {
                    std::cout << "  " << key << std::endl;
                }
            }
        } else if (choice == "2") {
            std::cout << "Enter key: ";
            std::string key;
            std::getline(std::cin >> std::ws, key);
            std::cout << "Enter value: ";
            std::string value;
            std::getline(std::cin >> std::ws, value);
            
            if (currentBrain_) {
                auto config = currentBrain_->getConfig();
                config->set(key, value);
                std::cout << "Configuration updated." << std::endl;
            }
        } else if (choice == "3") {
            std::cout << "Enter configuration file path: ";
            std::string filepath;
            std::getline(std::cin >> std::ws, filepath);
            
            if (currentBrain_) {
                auto config = currentBrain_->getConfig();
                if (config->loadFromFile(filepath)) {
                    std::cout << "Configuration loaded from " << filepath << std::endl;
                } else {
                    std::cout << "Failed to load configuration from " << filepath << std::endl;
                }
            }
        } else if (choice == "4") {
            std::cout << "Enter configuration file path: ";
            std::string filepath;
            std::getline(std::cin >> std::ws, filepath);
            
            if (currentBrain_) {
                auto config = currentBrain_->getConfig();
                if (config->saveToFile(filepath)) {
                    std::cout << "Configuration saved to " << filepath << std::endl;
                } else {
                    std::cout << "Failed to save configuration to " << filepath << std::endl;
                }
            }
        }
    }
    
    void cmdExit() {
        running_ = false;
        std::cout << "Exiting NLM CLI..." << std::endl;
    }
    
    void printHelp() {
        std::cout << "=== NLM CLI Help ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Brain Management:" << std::endl;
        std::cout << "  create-brain    - Create a new brain with default configuration" << std::endl;
        std::cout << "  reset           - Reset and reinitialize the current brain" << std::endl;
        std::cout << "  step [count]    - Run one or more simulation steps" << std::endl;
        std::cout << "  status          - Display current brain status" << std::endl;
        std::cout << "  save [file]     - Save brain state to file" << std::endl;
        std::cout << "  load [file]     - Load brain state from file" << std::endl;
        std::cout << std::endl;
        std::cout << "Subsystem Management:" << std::endl;
        std::cout << "  enable-subsystem <name> - Enable a subsystem (reward_modulation, structural_plasticity, development, curiosity)" << std::endl;
        std::cout << "  disable-subsystem <name> - Disable a subsystem" << std::endl;
        std::cout << "  list-subsystems  - List enabled subsystems" << std::endl;
        std::cout << std::endl;
        std::cout << "Simulation Control:" << std::endl;
        std::cout << "  run             - Run simulation loop" << std::endl;
        std::cout << "  pause           - Pause simulation" << std::endl;
        std::cout << "  stop            - Stop simulation" << std::endl;
        std::cout << std::endl;
        std::cout << "Monitoring and Diagnostics:" << std::endl;
        std::cout << "  monitor         - Start real-time monitoring" << std::endl;
        std::cout << "  benchmark       - Run performance benchmarks" << std::endl;
        std::cout << "  stats           - Display current statistics" << std::endl;
        std::cout << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  config          - Configuration management menu" << std::endl;
        std::cout << std::endl;
        std::cout << "General:" << std::endl;
        std::cout << "  help            - Show this help message" << std::endl;
        std::cout << "  exit|quit       - Exit NLM CLI" << std::endl;
    }
    
private:
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM CLI — Neural Learning Machine Command Line Interface   ║
    ║                                                               ║
    ║     Unified interface for NLM simulation and management.      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    std::map<std::string, std::function<void()>> commands_;
    std::shared_ptr<Logger> logger_;
    bool running_;
    std::shared_ptr<Brain> currentBrain_;
    std::shared_ptr<SimpleWorld> currentWorld_;
    std::shared_ptr<AgentBrain> currentAgent_;
};

} // namespace nlm

// Main entry point for NLM CLI
int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::string mode = "interactive";
    std::string configFile = "configs/default.cfg";
    int steps = 0;
    bool benchmarkMode = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            std::cout << "NLM CLI - Neural Learning Machine Command Line Interface" << std::endl;
            std::cout << "Usage: nlm-cli [OPTIONS]" << std::endl;
            std::cout << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --mode <mode>      CLI mode: interactive, batch, benchmark (default: interactive)" << std::endl;
            std::cout << "  --config <file>    Configuration file (default: configs/default.cfg)" << std::endl;
            std::cout << "  --steps <count>    Number of steps to run in batch mode" << std::endl;
            std::cout << "  --benchmark        Run benchmark test" << std::endl;
            std::cout << "  --help, -h         Show this help message" << std::endl;
            return 0;
        } else if (arg == "--mode") {
            if (i + 1 < argc) {
                mode = argv[++i];
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                steps = std::atoi(argv[++i]);
            }
        } else if (arg == "--benchmark") {
            benchmarkMode = true;
        }
    }
    
    nlm::NLMCLI cli;
    cli.registerCommands();
    
    if (mode == "batch") {
        std::cout << "Batch mode not yet implemented." << std::endl;
    } else if (mode == "benchmark") {
        std::cout << "Benchmark mode not yet implemented." << std::endl;
    } else {
        // Interactive mode
        cli.run();
    }
    
    return 0;
}
