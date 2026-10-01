#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "CommandLineParser.hpp"

namespace nlm {

// Forward declarations
class Command;
class CommandHandler;

// Configuration command classes
class ConfigFileCommand : public ConfigurationCommand {
public:
    std::string getName() const override { return "--config-file"; }
    std::string getDescription() const override { return "Load configuration from file"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

class SetConfigCommand : public ConfigurationCommand {
public:
    std::string getName() const override { return "--set"; }
    std::string getDescription() const override { return "Set configuration key-value pair"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

// Brain control command classes
class SaveBrainCommand : public BrainControlCommand {
public:
    std::string getName() const override { return "save"; }
    std::string getDescription() const override { return "Save current brain state to file"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

class LoadBrainCommand : public BrainControlCommand {
public:
    std::string getName() const override { return "load"; }
    std::string getDescription() const override { return "Load brain state from file"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

class ResetBrainCommand : public BrainControlCommand {
public:
    std::string getName() const override { return "reset"; }
    std::string getDescription() const override { return "Reset brain to initial state"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

class CheckpointBrainCommand : public BrainControlCommand {
public:
    std::string getName() const override { return "checkpoint"; }
    std::string getDescription() const override { return "Create a checkpoint of current brain state"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

// Simulation control command classes
class RunSimulationCommand : public SimulationControlCommand {
public:
    std::string getName() const override { return "run"; }
    std::string getDescription() const override { return "Run simulation with current configuration"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

class SetStepsCommand : public SimulationControlCommand {
public:
    std::string getName() const override { return "--steps"; }
    std::string getDescription() const override { return "Set number of simulation steps"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

class SetTimestepCommand : public SimulationControlCommand {
public:
    std::string getName() const override { return "--timestep"; }
    std::string getDescription() const override { return "Set simulation time step"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

class SetDurationCommand : public SimulationControlCommand {
public:
    std::string getName() const override { return "--duration"; }
    std::string getDescription() const override { return "Set simulation duration in seconds"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

// Visualization command classes
class PlotStatsCommand : public VisualizationCommand {
public:
    std::string getName() const override { return "--plot"; }
    std::string getDescription() const override { return "Enable plotting of statistics"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

class ExportStatsCommand : public VisualizationCommand {
public:
    std::string getName() const override { return "--export-stats"; }
    std::string getDescription() const override { return "Export simulation statistics to file"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

class DebugCommand : public VisualizationCommand {
public:
    std::string getName() const override { return "debug"; }
    std::string getDescription() const override { return "Enable debug output and diagnostics"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

// Advanced command classes
class ProfileCommand : public AdvancedCommand {
public:
    std::string getName() const override { return "--profile"; }
    std::string getDescription() const override { return "Enable profiling mode"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

class BenchmarkCommand : public AdvancedCommand {
public:
    std::string getName() const override { return "--benchmark"; }
    std::string getDescription() const override { return "Run performance benchmark"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

class ValidateCommand : public AdvancedCommand {
public:
    std::string getName() const override { return "--validate"; }
    std::string getDescription() const override { return "Validate configuration and brain state"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

class BatchCommand : public AdvancedCommand {
public:
    std::string getName() const override { return "batch"; }
    std::string getDescription() const override { return "Process multiple simulations from file"; }
    std::vector<CommandLineOption> getOptions() const override;
    bool execute(const std::map<std::string, std::string>& options,
                 std::shared_ptr<Config> config) override;
};

// Command Factory interface
class CLIFactory {
public:
    static std::vector<std::shared_ptr<Command>> createDefaultCommands();
    static std::shared_ptr<CommandHandler> createCommandHandler(std::shared_ptr<Config> config,
                                                              std::shared_ptr<Brain> brain = nullptr);
    
private:
    static std::vector<std::shared_ptr<Command>> createConfigurationCommands();
    static std::vector<std::shared_ptr<Command>> createBrainControlCommands();
    static std::vector<std::shared_ptr<Command>> createSimulationControlCommands();
    static std::vector<std::shared_ptr<Command>> createVisualizationCommands();
    static std::vector<std::shared_ptr<Command>> createAdvancedCommands();
};

} // namespace nlm
