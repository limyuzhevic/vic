#pragma once

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>

namespace nlm {

// Command callback function type
using CommandCallback = std::function<void(const std::vector<std::string>&)>;

// Advanced NLM Command Line Interface
class CommandLineInterface {
public:
    CommandLineInterface() {
        registerCommands();
    }
    
    // Parse and execute command
    bool execute(const std::string& commandLine);
    
    // Set configuration
    void setConfig(std::shared_ptr<Config> config) { m_config = config; }
    
    // Set brain instance
    void setBrain(std::shared_ptr<Brain> brain) { m_brain = brain; }
    
    // Set experiment runner
    void setExperimentRunner(std::shared_ptr<ExperimentRunner> runner) { m_experimentRunner = runner; }
    
    // Print available commands
    void printHelp() const;
    
    // Print brain status
    void printBrainStatus() const;
    
    // Print configuration
    void printConfig() const;
    
    // Save brain state
    bool saveBrain(const std::string& filepath) const;
    
    // Load brain state
    bool loadBrain(const std::string& filepath);
    
    // Run experiment
    bool runExperiment(const std::string& experimentName, int steps);
    
    // Get command history
    const std::vector<std::string>& getHistory() const { return m_commandHistory; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::shared_ptr<Config> m_config;
    std::shared_ptr<Brain> m_brain;
    std::shared_ptr<ExperimentRunner> m_experimentRunner;
    
    // Available commands
    std::map<std::string, CommandCallback> m_commands;
    
    // Command history
    std::vector<std::string> m_commandHistory;
    
    // Register all available commands
    void registerCommands();
    
    // Parse command line into tokens
    static std::vector<std::string> tokenize(const std::string& commandLine);
    
    // Get current simulation step
    static uint64_t getCurrentStep() { return 0; } // TODO: Integrate with clock
    
    // Get current simulation time
    static double getCurrentTime() { return 0.0; } // TODO: Integrate with clock
    
    // Log command execution
    void logCommand(const std::string& command) const;
    
    // Check if command exists
    bool hasCommand(const std::string& command) const;
    
    // Execute help command
    void cmdHelp(const std::vector<std::string>& args);
    
    // Execute brain command
    void cmdBrain(const std::vector<std::string>& args);
    
    // Execute config command
    void cmdConfig(const std::vector<std::string>& args);
    
    // Execute experiment command
    void cmdExperiment(const std::vector<std::string>& args);
    
    // Execute save command
    void cmdSave(const std::vector<std::string>& args);
    
    // Execute load command
    void cmdLoad(const std::vector<std::string>& args);
    
    // Execute exit command
    void cmdExit(const std::vector<std::string>& args);
    
    // Execute run command
    void cmdRun(const std::vector<std::string>& args);
    
    // Execute status command
    void cmdStatus(const std::vector<std::string>& args);
    
    // Execute stats command
    void cmdStats(const std::vector<std::string>& args);
    
    // Execute plot command
    void cmdPlot(const std::vector<std::string>& args);
    
    // Execute benchmark command
    void cmdBenchmark(const std::vector<std::string>& args);
};

} // namespace nlm