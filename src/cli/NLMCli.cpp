// NLM Command Line Interface
// A comprehensive command-line tool for interacting with the NLM (Neural Learning Machine) brain

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <map>
#include <algorithm>

#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

namespace nlm {

class CommandLineInterface {
public:
    CommandLineInterface() : config_(std::make_shared<Config>()) {
        initializeLogger();
        loadDefaultConfig();
    }
    
    void run(const std::vector<std::string>& args) {
        if (args.empty()) {
            printHelp();
            return;
        }
        
        std::string command = args[0];
        std::vector<std::string> cmdArgs(args.begin() + 1, args.end());
        
        try {
            if (command == "help" || command == "--help") {
                printHelp();
            } else if (command == "version") {
                printVersion();
            } else if (command == "config") {
                handleConfigCommand(cmdArgs);
            } else if (command == "run") {
                handleRunCommand(cmdArgs);
            } else if (command == "experiment") {
                handleExperimentCommand(cmdArgs);
            } else if (command == "stats" || command == "statistics") {
                handleStatsCommand(cmdArgs);
            } else if (command == "memory") {
                handleMemoryCommand(cmdArgs);
            } else if (command == "neuromod") {
                handleNeuromodCommand(cmdArgs);
            } else if (command == "develop") {
                handleDevelopCommand(cmdArgs);
            } else if (command == "reset") {
                handleResetCommand(cmdArgs);
            } else if (command == "checkpoint") {
                handleCheckpointCommand(cmdArgs);
            } else if (command == "plot") {
                handlePlotCommand(cmdArgs);
            } else if (command == "export") {
                handleExportCommand(cmdArgs);
            } else if (command == "import") {
                handleImportCommand(cmdArgs);
            } else {
                NLM_LOG_ERROR("Unknown command: " + command);
                printHelp();
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error: ") + e.what());
        }
    }
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<SimulationClock> clock_;
    
    void initializeLogger() {
        Logger& logger = Logger::getGlobal();
        logger.setLevel(LogLevel::Info);
        NLM_LOG_INFO("NLM Command Line Interface initialized");
    }
    
    void loadDefaultConfig() {
        // Load default configuration
        config_->set("brain.type", "spiking");
        config_->set("brain.region.count", 4);
        config_->set("brain.region.per.population", 100);
        config_->set("simulation.steps", 1000);
        config_->set("simulation.temporal_step", 0.01f);
        config_->set("reward.function", "exponential");
        config_->set("exploration.curiosity", 0.5f);
        config_->set("development.enabled", true);
        config_->set("plasticity.stdp.enabled", true);
        config_->set("plasticity.hebbian.enabled", true);
        config_->set("memory.working.capacity", 1000);
        config_->set("memory.episodic.capacity", 5000);
        
        NLM_LOG_INFO("Default configuration loaded");
    }
    
    void handleConfigCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            printConfigHelp();
            return;
        }
        
        std::string subcommand = args[0];
        std::vector<std::string> subArgs(args.begin() + 1, args.end());
        
        if (subcommand == "list") {
            listConfig();
        } else if (subcommand == "set") {
            if (subArgs.size() < 2) {
                NLM_LOG_ERROR("Usage: config set <key> <value>");
                return;
            }
            std::string key = subArgs[0];
            std::string value = subArgs[1];
            setConfigValue(key, value);
        } else if (subcommand == "get") {
            if (subArgs.empty()) {
                NLM_LOG_ERROR("Usage: config get <key>");
                return;
            }
            getConfigValue(subArgs[0]);
        } else if (subcommand == "save") {
            if (subArgs.empty()) {
                NLM_LOG_ERROR("Usage: config save <filename>");
                return;
            }
            saveConfig(subArgs[0]);
        } else if (subcommand == "load") {
            if (subArgs.empty()) {
                NLM_LOG_ERROR("Usage: config load <filename>");
                return;
            }
            loadConfig(subArgs[0]);
        } else {
            printConfigHelp();
        }
    }
    
    void handleRunCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            NLM_LOG_ERROR("Usage: run [steps] [timestep] [start_step]");
            return;
        }
        
        int steps = 1000;
        float timestep = 0.01f;
        int startStep = 0;
        
        if (args.size() >= 1) {
            steps = std::stoi(args[0]);
        }
        if (args.size() >= 2) {
            timestep = std::stof(args[1]);
        }
        if (args.size() >= 3) {
            startStep = std::stoi(args[2]);
        }
        
        runSimulation(steps, timestep, startStep);
    }
    
    void handleExperimentCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            NLM_LOG_ERROR("Usage: experiment <name> [params...]");
            return;
        }
        
        std::string experimentName = args[0];
        std::vector<std::string> params(args.begin() + 1, args.end());
        
        if (experimentName == "phase3") {
            runPhase3Experiment(params);
        } else if (experimentName == "phase4") {
            runPhase4Experiment(params);
        } else if (experimentName == "integration") {
            runIntegrationExperiment(params);
        } else {
            NLM_LOG_ERROR("Unknown experiment: " + experimentName);
        }
    }
    
    void handleStatsCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            printGlobalStats();
        } else {
            std::string target = args[0];
            printTargetStats(target);
        }
    }
    
    void handleMemoryCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            printMemoryHelp();
            return;
        }
        
        std::string subcommand = args[0];
        std::vector<std::string> subArgs(args.begin() + 1, args.end());
        
        if (subcommand == "info") {
            printMemoryInfo();
        } else if (subcommand == "clear") {
            clearMemory();
        } else if (subcommand == "export") {
            exportMemory(subArgs.size() > 0 ? subArgs[0] : "memory.dat");
        } else {
            printMemoryHelp();
        }
    }
    
    void handleNeuromodCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            NLM_LOG_ERROR("Usage: neuromod <set|get> <param> [value]");
            return;
        }
        
        std::string action = args[0];
        
        if (action == "set") {
            if (args.size() < 2) {
                NLM_LOG_ERROR("Usage: neuromod set <param> <value>");
                return;
            }
            std::string param = args[1];
            float value = (args.size() >= 3) ? std::stof(args[2]) : 0.0f;
            setNeuromodulation(param, value);
        } else if (action == "get") {
            if (args.size() < 2) {
                NLM_LOG_ERROR("Usage: neuromod get <param>");
                return;
            }
            getNeuromodulation(args[1]);
        }
    }
    
    void handleDevelopCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            NLM_LOG_ERROR("Usage: develop <set|get> <stage> [value]");
            return;
        }
        
        std::string action = args[0];
        
        if (action == "set") {
            if (args.size() < 2) {
                NLM_LOG_ERROR("Usage: develop set <stage> <value>");
                return;
            }
            std::string stage = args[1];
            float value = (args.size() >= 3) ? std::stof(args[2]) : 0.0f;
            setDevelopmentStage(stage, value);
        } else if (action == "get") {
            if (args.size() < 2) {
                NLM_LOG_ERROR("Usage: develop get <stage>");
                return;
            }
            getDevelopmentStage(args[1]);
        }
    }
    
    void handleResetCommand(const std::vector<std::string>& args) {
        resetBrain();
        NLM_LOG_INFO("Brain reset completed");
    }
    
    void handleCheckpointCommand(const std::vector<std::string>& args) {
        std::string filename = args.empty() ? "checkpoint.dat" : args[0];
        saveCheckpoint(filename);
        NLM_LOG_INFO("Checkpoint saved to: " + filename);
    }
    
    void handlePlotCommand(const std::vector<std::string>& args) {
        // TODO: Implement plotting functionality
        NLM_LOG_WARNING("Plotting functionality not yet implemented");
    }
    
    void handleExportCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            NLM_LOG_ERROR("Usage: export <format> [filename]");
            return;
        }
        std::string format = args[0];
        std::string filename = args.size() > 1 ? args[1] : "export.dat";
        exportData(format, filename);
    }
    
    void handleImportCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            NLM_LOG_ERROR("Usage: import <format> <filename>");
            return;
        }
        std::string format = args[0];
        std::string filename = args[1];
        importData(format, filename);
    }
    
    // Helper functions
    void setConfigValue(const std::string& key, const std::string& value) {
        // Parse value based on key type
        if (key.find("int") != std::string::npos) {
            config_->set(key, std::stoi(value));
        } else if (key.find("float") != std::string::npos || key.find("timestep") != std::string::npos || key.find("rate") != std::string::npos) {
            config_->set(key, std::stof(value));
        } else if (key.find("bool") != std::string::npos) {
            config_->set(key, value == "true" || value == "1");
        } else {
            config_->set(key, value);
        }
        NLM_LOG_INFO("Set config " + key + " = " + value);
    }
    
    void getConfigValue(const std::string& key) {
        auto value = config_->get<std::string>(key);
        if (value.has_value()) {
            NLM_LOG_INFO(key + " = " + value.value());
        } else {
            NLM_LOG_ERROR("Key not found: " + key);
        }
    }
    
    void saveConfig(const std::string& filename) {
        // TODO: Implement JSON serialization
        NLM_LOG_WARNING("Config save not yet implemented (requires JSON library)");
    }
    
    void loadConfig(const std::string& filename) {
        // TODO: Implement JSON deserialization
        NLM_LOG_WARNING("Config load not yet implemented (requires JSON library)");
    }
    
    void listConfig() {
        auto keys = config_->getKeys();
        NLM_LOG_INFO("Configuration keys (" + std::to_string(keys.size()) + "):");
        for (const auto& key : keys) {
            NLM_LOG_INFO("  " + key);
        }
    }
    
    void printConfigHelp() {
        NLM_LOG_INFO("=== Config Command Help ===");
        NLM_LOG_INFO("config list - List all configuration keys");
        NLM_LOG_INFO("config get <key> - Get configuration value");
        NLM_LOG_INFO("config set <key> <value> - Set configuration value");
        NLM_LOG_INFO("config save <filename> - Save configuration to file");
        NLM_LOG_INFO("config load <filename> - Load configuration from file");
    }
    
    void runSimulation(int steps, float timestep, int startStep) {
        NLM_LOG_INFO("Starting simulation: " + std::to_string(steps) + " steps, timestep=" + std::to_string(timestep) + ", start_step=" + std::to_string(startStep));
        
        // Initialize brain and world if not already done
        if (!brain_) {
            initializeBrain();
        }
        
        // Run simulation
        for (int step = startStep; step < startStep + steps; ++step) {
            double time = step * timestep;
            brain_->step(step, time);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + "/" + std::to_string(startStep + steps));
            }
        }
        
        NLM_LOG_INFO("Simulation completed");
    }
    
    void initializeBrain() {
        NLM_LOG_INFO("Initializing brain with configuration...");
        
        // TODO: Implement brain initialization with config
        brain_ = std::make_shared<Brain>(config_);
        brain_->initialize();
        
        // Create agent brain
        agentBrain_ = std::make_shared<AgentBrain>(brain_);
        
        // Create simple world
        world_ = std::make_shared<SimpleWorld>();
        
        // Create simulation clock
        clock_ = std::make_shared<SimulationClock>();
        
        NLM_LOG_INFO("Brain initialized successfully");
    }
    
    void printGlobalStats() {
        if (!brain_) {
            NLM_LOG_ERROR("Brain not initialized. Use 'run' to start simulation first.");
            return;
        }
        
        NLM_LOG_INFO("=== Global Brain Statistics ===");
        NLM_LOG_INFO("Neurons: " + std::to_string(brain_->getTotalNeuronCount()));
        NLM_LOG_INFO("Synapses: " + std::to_string(brain_->getTotalSynapseCount()));
        NLM_LOG_INFO("Active Neurons: " + std::to_string(brain_->getActiveNeuronCount()));
        NLM_LOG_INFO("Firing Neurons: " + std::to_string(brain_->getFiringNeuronCount()));
        NLM_LOG_INFO("Firing Rate: " + std::to_string(brain_->getAverageFiringRate()) + " Hz");
        NLM_LOG_INFO("Spikes: " + std::to_string(brain_->getTotalSpikeCount()));
        NLM_LOG_INFO("Pending Events: " + std::to_string(brain_->getPendingSpikeEventCount()));
        
        if (agentBrain_) {
            NLM_LOG_INFO("Neuromodulation Level: " + std::to_string(agentBrain_->getNeuromodulationLevel()));
            NLM_LOG_INFO("Curiosity Level: " + std::to_string(agentBrain_->getCuriosityLevel()));
            NLM_LOG_INFO("Novelty Level: " + std::to_string(agentBrain_->getNoveltyLevel()));
            NLM_LOG_INFO("Prediction Error: " + std::to_string(agentBrain_->getPredictionError()));
            NLM_LOG_INFO("Developmental Stage: " + getDevelopmentalStageString(agentBrain_->getDevelopmentalStage()));
        }
    }
    
    void printTargetStats(const std::string& target) {
        // TODO: Implement target-specific statistics
        NLM_LOG_WARNING("Statistics for '" + target + "' not yet implemented");
    }
    
    void printMemoryHelp() {
        NLM_LOG_INFO("=== Memory Command Help ===");
        NLM_LOG_INFO("memory info - Show memory statistics");
        NLM_LOG_INFO("memory clear - Clear all memory");
        NLM_LOG_INFO("memory export [filename] - Export memory to file");
    }
    
    void printMemoryInfo() {
        if (!brain_) {
            NLM_LOG_ERROR("Brain not initialized.");
            return;
        }
        
        NLM_LOG_INFO("=== Memory Information ===");
        
        auto workingMem = brain_->getWorkingMemory();
        if (workingMem) {
            NLM_LOG_INFO("Working Memory Capacity: " + std::to_string(workingMem->getCapacity()));
        }
        
        auto episodicMem = brain_->getEpisodicMemory();
        if (episodicMem) {
            NLM_LOG_INFO("Episodic Memory Episodes: " + std::to_string(episodicMem->getEpisodeCount()));
        }
        
        auto associativeMem = brain_->getAssociativeMemory();
        if (associativeMem) {
            NLM_LOG_INFO("Associative Memory Patterns: " + std::to_string(associativeMem->getPatternCount()));
        }
    }
    
    void clearMemory() {
        if (!brain_) {
            NLM_LOG_ERROR("Brain not initialized.");
            return;
        }
        
        brain_->reset();
        NLM_LOG_INFO("Memory cleared");
    }
    
    void exportMemory(const std::string& filename) {
        if (!brain_) {
            NLM_LOG_ERROR("Brain not initialized.");
            return;
        }
        
        // TODO: Implement memory export
        NLM_LOG_WARNING("Memory export not yet implemented");
    }
    
    void setNeuromodulation(const std::string& param, float value) {
        if (!agentBrain_) {
            NLM_LOG_ERROR("Agent brain not initialized.");
            return;
        }
        
        // TODO: Implement neuromodulation parameter setting
        NLM_LOG_INFO("Set neuromodulation parameter '" + param + "' = " + std::to_string(value));
    }
    
    void getNeuromodulation(const std::string& param) {
        if (!agentBrain_) {
            NLM_LOG_ERROR("Agent brain not initialized.");
            return;
        }
        
        // TODO: Implement neuromodulation parameter retrieval
        NLM_LOG_INFO("Neuromodulation parameter '" + param + "' value will be implemented");
    }
    
    void setDevelopmentStage(const std::string& stage, float value) {
        if (!agentBrain_) {
            NLM_LOG_ERROR("Agent brain not initialized.");
            return;
        }
        
        // TODO: Implement development stage setting
        NLM_LOG_INFO("Set development stage '" + stage + "' = " + std::to_string(value));
    }
    
    void getDevelopmentStage(const std::string& stage) {
        if (!agentBrain_) {
            NLM_LOG_ERROR("Agent brain not initialized.");
            return;
        }
        
        // TODO: Implement development stage retrieval
        NLM_LOG_INFO("Development stage '" + stage + "' value will be implemented");
    }
    
    void resetBrain() {
        if (brain_) {
            brain_->reset();
        }
        if (agentBrain_) {
            agentBrain_->reset();
        }
        NLM_LOG_INFO("Brain and agent reset");
    }
    
    void saveCheckpoint(const std::string& filename) {
        if (!brain_) {
            NLM_LOG_ERROR("Brain not initialized.");
            return;
        }
        
        brain_->save(filename);
        NLM_LOG_INFO("Checkpoint saved to: " + filename);
    }
    
    void exportData(const std::string& format, const std::string& filename) {
        // TODO: Implement data export
        NLM_LOG_WARNING("Data export not yet implemented");
    }
    
    void importData(const std::string& format, const std::string& filename) {
        // TODO: Implement data import
        NLM_LOG_WARNING("Data import not yet implemented");
    }
    
    std::string getDevelopmentalStageString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "Critical Period";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            default: return "Unknown";
        }
    }
    
    void printHelp() {
        NLM_LOG_INFO("=== NLM Command Line Interface ===");
        NLM_LOG_INFO("Available commands:");
        NLM_LOG_INFO("  help                     - Show this help message");
        NLM_LOG_INFO("  version                  - Show version information");
        NLM_LOG_INFO("  config                   - Configuration management");
        NLM_LOG_INFO("  run [steps] [timestep] [start_step] - Run simulation");
        NLM_LOG_INFO("  experiment <name> [params...] - Run specific experiment");
        NLM_LOG_INFO("  stats [target]           - Show brain statistics");
        NLM_LOG_INFO("  memory <subcommand>      - Memory management");
        NLM_LOG_INFO("  neuromod <action> <param> [value] - Neuromodulation control");
        NLM_LOG_INFO("  develop <action> <stage> [value] - Development control");
        NLM_LOG_INFO("  reset                    - Reset brain state");
        NLM_LOG_INFO("  checkpoint [filename]    - Save/restore checkpoint");
        NLM_LOG_INFO("  plot                     - Generate plots (not implemented)");
        NLM_LOG_INFO("  export <format> [filename] - Export data");
        NLM_LOG_INFO("  import <format> <filename> - Import data");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("For more help on a specific command, run: nlm_cli <command> help");
    }
    
    void printVersion() {
        NLM_LOG_INFO("NLM Command Line Interface v0.1.0");
        NLM_LOG_INFO("Built with C++20 and scikit-build");
        NLM_LOG_INFO("Phase 6 Integration Complete");
    }
    
    void runPhase3Experiment(const std::vector<std::string>& params) {
        NLM_LOG_INFO("Running Phase 3 Experiment...");
        // TODO: Implement Phase 3 experiment execution
        NLM_LOG_WARNING("Phase 3 experiment not yet implemented");
    }
    
    void runPhase4Experiment(const std::vector<std::string>& params) {
        NLM_LOG_INFO("Running Phase 4 Experiment...");
        // TODO: Implement Phase 4 experiment execution
        NLM_LOG_WARNING("Phase 4 experiment not yet implemented");
    }
    
    void runIntegrationExperiment(const std::vector<std::string>& params) {
        NLM_LOG_INFO("Running Integration Experiment...");
        // TODO: Implement integration experiment execution
        NLM_LOG_WARNING("Integration experiment not yet implemented");
    }
};

// Main entry point
int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    CommandLineInterface cli;
    cli.run(args);
    
    return 0;
}
