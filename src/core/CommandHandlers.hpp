#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "CommandLineParser.hpp"

namespace nlm {

// Interface for command handlers
class CommandHandlers {
public:
    virtual ~CommandHandlers() = default;
    
    // Execute a command with given options
    virtual bool executeCommand(const std::string& commandName,
                               const std::map<std::string, std::string>& options,
                               std::shared_ptr<Config> config) = 0;
    
    // Get help for a specific command
    virtual std::string getCommandHelp(const std::string& commandName) const = 0;
    
    // Get list of all available commands
    virtual std::vector<std::string> getAvailableCommands() const = 0;
    
    // Get command category
    virtual CommandCategory getCommandCategory(const std::string& commandName) const = 0;
    
    // Check if command requires a brain instance
    virtual bool commandRequiresBrain(const std::string& commandName) const = 0;
    
    // Set brain reference for commands that need it
    virtual void setBrain(std::shared_ptr<Brain> brain) = 0;
};

// Configuration command handlers
class ConfigFileHandler {
public:
    static bool handleConfigFile(const std::string& filepath, std::shared_ptr<Config> config);
};

class SetConfigHandler {
public:
    static bool handleSetConfig(const std::string& key, const std::string& value,
                               std::shared_ptr<Config> config);
};

// Brain control command handlers
class SaveBrainHandler {
public:
    static bool handleSaveBrain(const std::string& filepath, std::shared_ptr<Brain> brain);
};

class LoadBrainHandler {
public:
    static bool handleLoadBrain(const std::string& filepath, std::shared_ptr<Brain> brain);
};

class ResetBrainHandler {
public:
    static bool handleResetBrain(std::shared_ptr<Brain> brain, bool preserveConfig);
};

class CheckpointBrainHandler {
public:
    static bool handleCheckpointBrain(const std::string& filepath,
                                     std::shared_ptr<Brain> brain);
};

// Simulation control command handlers
class RunSimulationHandler {
public:
    static bool handleRunSimulation(int steps, double timestep, double duration,
                                   std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
};

class SetStepsHandler {
public:
    static bool handleSetSteps(int steps, std::shared_ptr<Config> config);
};

class SetTimestepHandler {
public:
    static bool handleSetTimestep(double timestep, std::shared_ptr<Config> config);
};

class SetDurationHandler {
public:
    static bool handleSetDuration(double duration, std::shared_ptr<Config> config);
};

// Visualization command handlers
class PlotStatsHandler {
public:
    static bool handlePlotStats(const std::string& output, const std::string& format,
                               std::shared_ptr<Brain> brain);
};

class ExportStatsHandler {
public:
    static bool handleExportStats(const std::string& filepath, const std::string& format,
                                 std::shared_ptr<Config> config);
};

class DebugHandler {
public:
    static bool handleDebug(bool verbose, bool profile, const std::string& logFile);
};

// Advanced command handlers
class ProfileHandler {
public:
    static bool handleProfile(const std::string& output, const std::string& format,
                             bool stats, std::shared_ptr<Brain> brain);
};

class BenchmarkHandler {
public:
    static bool handleBenchmark(const std::string& output, double duration,
                               double warmup, std::shared_ptr<Brain> brain);
};

class ValidateHandler {
public:
    static bool handleValidate(const std::string& configFile, const std::string& brainFile,
                              bool strict);
};

class BatchHandler {
public:
    static bool handleBatch(const std::string& batchFile, int parallel,
                           const std::string& outputDir);
};

// Implementation of CommandHandlers interface
class DefaultCommandHandlers : public CommandHandlers {
private:
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    
public:
    DefaultCommandHandlers(std::shared_ptr<Config> cfg, std::shared_ptr<Brain> brn = nullptr)
        : config(cfg), brain(brn) {}
    
    bool executeCommand(const std::string& commandName,
                      const std::map<std::string, std::string>& options,
                      std::shared_ptr<Config> config) override {
        // Forward to the actual implementation
        // This will be implemented in CommandHandlers.cpp
        return false;
    }
    
    std::string getCommandHelp(const std::string& commandName) const override {
        // This would return help text for specific commands
        return "Help for command: " + commandName;
    }
    
    std::vector<std::string> getAvailableCommands() const override {
        return {
            "--config-file", "--set", "save", "load", "reset", "checkpoint",
            "run", "--steps", "--timestep", "--duration", "--plot",
            "--export-stats", "debug", "--profile", "--benchmark",
            "--validate", "batch"
        };
    }
    
    CommandCategory getCommandCategory(const std::string& commandName) const override {
        if (commandName.find("--") == 0) {
            return CommandCategory::Configuration;
        } else if (commandName == "save" || commandName == "load" ||
                   commandName == "reset" || commandName == "checkpoint") {
            return CommandCategory::BrainControl;
        } else if (commandName == "run" || commandName == "--steps" ||
                   commandName == "--timestep" || commandName == "--duration") {
            return CommandCategory::SimulationControl;
        } else if (commandName == "--plot" || commandName == "--export-stats" ||
                   commandName == "debug") {
            return CommandCategory::Visualization;
        } else {
            return CommandCategory::Advanced;
        }
    }
    
    bool commandRequiresBrain(const std::string& commandName) const override {
        return commandName == "save" || commandName == "load" ||
               commandName == "reset" || commandName == "checkpoint" ||
               commandName == "run" || commandName == "--plot" ||
               commandName == "--export-stats" || commandName == "debug" ||
               commandName == "--profile" || commandName == "--benchmark";
    }
    
    void setBrain(std::shared_ptr<Brain> brn) override {
        brain = brn;
    }
};

} // namespace nlm
