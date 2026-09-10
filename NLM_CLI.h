// CLI Implementation Header - NLM Command Line Interface
// Advanced CLI for controlling NLM Brain with comprehensive commands

#ifndef NLM_CLI_H
#define NLM_CLI_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm_cli {

// Forward declarations
class Command;
class CommandHandler;
class ConfigManager;
class BrainController;
class MemoryManager;
class PredictionManager;
class NeuromodulationManager;
class DevelopmentManager;
class CheckpointManager;
class PerformanceMonitor;
class Visualizer;

// Main CLI Application Class
class NLMCLI {
private:
    std::unique_ptr<CommandHandler> commandHandler;
    std::unique_ptr<ConfigManager> configManager;
    std::unique_ptr<BrainController> brainController;
    std::unique_ptr<MemoryManager> memoryManager;
    std::unique_ptr<PredictionManager> predictionManager;
    std::unique_ptr<NeuromodulationManager> neuromodulationManager;
    std::unique_ptr<DevelopmentManager> developmentManager;
    std::unique_ptr<CheckpointManager> checkpointManager;
    std::unique_ptr<PerformanceMonitor> performanceMonitor;
    std::unique_ptr<Visualizer> visualizer;
    
    bool running;
    std::string currentConfig;
    std::shared_ptr<class nlm::Brain> currentBrain;
    
public:
    NLMCLI();
    ~NLMCLI();
    
    void run();
    void stop();
    
    // Component access
    void setBrain(std::shared_ptr<class nlm::Brain> brain) { currentBrain = brain; }
    std::shared_ptr<class nlm::Brain> getBrain() const { return currentBrain; }
    
private:
    void initializeComponents();
    void setupCommands();
    void printBanner() const;
    void processInput(const std::string& input);
    void showHelp() const;
    void showVersion() const;
};

// Base Command Class
class Command {
public:
    virtual ~Command() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual std::string getHelp() const = 0;
    virtual bool execute(const std::vector<std::string>& args, NLMCLI& cli) = 0;
    virtual bool validateArgs(const std::vector<std::string>& args) const = 0;
    virtual bool isSubcommand() const { return false; }
};

// Command Handler Class
class CommandHandler {
private:
    NLMCLI& cli;
    std::map<std::string, std::unique_ptr<Command>> commands;
    std::map<std::string, std::map<std::string, std::unique_ptr<Command>>> subcommands;
    
public:
    CommandHandler(NLMCLI& cliRef);
    
    void registerCommand(std::unique_ptr<Command> cmd);
    void registerSubcommand(const std::string& parent, std::unique_ptr<Command> subcmd);
    
    void handleCommand(const std::string& cmd, const std::vector<std::string>& args);
    bool executeCommand(const std::string& cmd, const std::vector<std::string>& args, NLMCLI& cli);
    
    void showHelp(const std::string& cmd = "") const;
    void showCommandHelp(const std::string& cmd, const std::string& subcmd = "") const;
    
private:
    void parseAndExecute(const std::string& cmdLine, NLMCLI& cli);
};

// Forward declaration for nlm namespace
namespace nlm {
    class Brain;
    class Config;
}

// Brain Control Commands
class InitializeCommand : public Command {
public:
    std::string getName() const override { return "initialize"; }
    std::string getDescription() const override { return "Initialize NLM brain with configuration"; }
    std::string getUsage() const override { return "initialize [--config file] [--steps count]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class StepCommand : public Command {
public:
    std::string getName() const override { return "step"; }
    std::string getDescription() const override { return "Run a single brain simulation step"; }
    std::string getUsage() const override { return "step [--step number] [--verbose]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class RunCommand : public Command {
public:
    std::string getName() const override { return "run"; }
    std::string getDescription() const override { return "Run simulation for specified number of steps"; }
    std::string getUsage() const override { return "run [steps] [--config file] [--output file] [--json]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class ResetCommand : public Command {
public:
    std::string getName() const override { return "reset"; }
    std::string getDescription() const override { return "Reset brain state and reinitialize"; }
    std::string getUsage() const override { return "reset [--confirm]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class StatusCommand : public Command {
public:
    std::string getName() const override { return "status"; }
    std::string getDescription() const override { return "Show brain status and statistics"; }
    std::string getUsage() const override { return "status [--json] [--verbose]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

// Memory System Commands
class WorkingMemoryCommand : public Command {
public:
    std::string getName() const override { return "workingmemory"; }
    std::string getDescription() const override { return "Working memory system commands"; }
    std::string getUsage() const override { return "workingmemory <subcommand> [args]"; }
    std::string getHelp() const override;
    
    bool isSubcommand() const override { return true; }
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class EpisodicMemoryCommand : public Command {
public:
    std::string getName() const override { return "episodicmemory"; }
    std::string getDescription() const override { return "Episodic memory system commands"; }
    std::string getUsage() const override { return "episodicmemory <subcommand> [args]"; }
    std::string getHelp() const override;
    
    bool isSubcommand() const override { return true; }
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class AssociativeMemoryCommand : public Command {
public:
    std::string getName() const override { return "associativememory"; }
    std::string getDescription() const override { return "Associative memory system commands"; }
    std::string getUsage() const override { return "associativememory <subcommand> [args]"; }
    std::string getHelp() const override;
    
    bool isSubcommand() const override { return true; }
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

// Prediction System Commands
class PredictionCommand : public Command {
public:
    std::string getName() const override { return "prediction"; }
    std::string getDescription() const override { return "Prediction system commands"; }
    std::string getUsage() const override { return "prediction <subcommand> [args]"; }
    std::string getHelp() const override;
    
    bool isSubcommand() const override { return true; }
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class ErrorCommand : public Command {
public:
    std::string getName() const override { return "error"; }
    std::string getDescription() const override { return "Prediction error system commands"; }
    std::string getUsage() const override { return "error <subcommand> [args]"; }
    std::string getHelp() const override;
    
    bool isSubcommand() const override { return true; }
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

// Neuromodulation Commands
class NeuromodulationCommand : public Command {
public:
    std::string getName() const override { return "neuromodulation"; }
    std::string getDescription() const override { return "Neuromodulation system commands"; }
    std::string getUsage() const override { return "neuromodulation <subcommand> [args]"; }
    std::string getHelp() const override;
    
    bool isSubcommand() const override { return true; }
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class DopamineCommand : public Command {
public:
    std::string getName() const override { return "dopamine"; }
    std::string getDescription() const override { return "Dopamine system commands"; }
    std::string getUsage() const override { return "dopamine <subcommand> [args]"; }
    std::string getHelp() const override;
    
    bool isSubcommand() const override { return true; }
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class CuriosityCommand : public Command {
public:
    std::string getName() const override { return "curiosity"; }
    std::string getDescription() const override { return "Curiosity system commands"; }
    std::string getUsage() const override { return "curiosity <subcommand> [args]"; }
    std::string getHelp() const override;
    
    bool isSubcommand() const override { return true; }
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

// Development System Commands
class DevelopmentCommand : public Command {
public:
    std::string getName() const override { return "development"; }
    std::string getDescription() const override { return "Development system commands"; }
    std::string getUsage() const override { return "development <subcommand> [args]"; }
    std::string getHelp() const override;
    
    bool isSubcommand() const override { return true; }
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class StageCommand : public Command {
public:
    std::string getName() const override { return "stage"; }
    std::string getDescription() const override { return "Set developmental stage"; }
    std::string getUsage() const override { return "stage <stage_name> [--age age] [--duration duration]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

// Checkpoint Commands
class SaveCommand : public Command {
public:
    std::string getName() const override { return "save"; }
    std::string getDescription() const override { return "Save brain state to checkpoint file"; }
    std::string getUsage() const override { return "save [--file file] [--format format] [--compress]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class LoadCommand : public Command {
public:
    std::string getName() const override { return "load"; }
    std::string getDescription() const override { return "Load brain state from checkpoint file"; }
    std::string getUsage() const override { return "load --file file [--steps steps] [--verify]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class CheckpointCommand : public Command {
public:
    std::string getName() const override { return "checkpoint"; }
    std::string getDescription() const override { return "Checkpoint management commands"; }
    std::string getUsage() const override { return "checkpoint <subcommand> [args]"; }
    std::string getHelp() const override;
    
    bool isSubcommand() const override { return true; }
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

// Performance Commands
class MonitorCommand : public Command {
public:
    std::string getName() const override { return "monitor"; }
    std::string getDescription() const override { return "Monitor system performance and resources"; }
    std::string getUsage() const override { return "monitor [target] [--interval interval] [--json] [--verbose]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class BenchmarkCommand : public Command {
public:
    std::string getName() const override { return "benchmark"; }
    std::string getDescription() const override { return "Run performance benchmarks"; }
    std::string getUsage() const override { return "benchmark <test> [--iterations count] [--output file] [--json]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class ProfileCommand : public Command {
public:
    std::string getName() const override { return "profile"; }
    std::string getDescription() const override { return "Profile brain performance and bottlenecks"; }
    std::string getUsage() const override { return "profile <mode> [--steps count] [--output file] [--json]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

// Configuration Commands
class ConfigCommand : public Command {
public:
    std::string getName() const override { return "config"; }
    std::string getDescription() const override { return "Configuration management commands"; }
    std::string getUsage() const override { return "config <subcommand> [args]"; }
    std::string getHelp() const override;
    
    bool isSubcommand() const override { return true; }
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class SetCommand : public Command {
public:
    std::string getName() const override { return "set"; }
    std::string getDescription() const override { return "Set configuration value"; }
    std::string getUsage() const override { return "set <key> <value> [--source source] [--type type]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class GetCommand : public Command {
public:
    std::string getName() const override { return "get"; }
    std::string getDescription() const override { return "Get configuration value"; }
    std::string getUsage() const override { return "get <key> [--source source] [--format format]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class ResetCommand : public Command {
public:
    std::string getName() const override { return "reset"; }
    std::string getDescription() const override { return "Reset configuration to defaults"; }
    std::string getUsage() const override { return "reset <key> [--source source] [--confirm]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

// Advanced Commands
class OptimizeCommand : public Command {
public:
    std::string getName() const override { return "optimize"; }
    std::string getDescription() const override { return "Optimize brain performance and parameters"; }
    std::string getUsage() const override { return "optimize --mode <mode> --target <target> [--output file] [--json]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class AnalyzeCommand : public Command {
public:
    std::string getName() const override { return "analyze"; }
    std::string getDescription() const override { return "Analyze brain state and performance metrics"; }
    std::string getUsage() const override { return "analyze <type> [--output file] [--json] [--format format]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class DebugCommand : public Command {
public:
    std::string getName() const override { return "debug"; }
    std::string getDescription() const override { return "Debug tools and diagnostic commands"; }
    std::string getUsage() const override { return "debug <type> [--output file] [--verbose] [--json]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class HelpCommand : public Command {
public:
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { return "Show help information"; }
    std::string getUsage() const override { return "help [command] [subcommand]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

class ExitCommand : public Command {
public:
    std::string getName() const override { return "exit"; }
    std::string getDescription() const override { return "Exit NLM CLI"; }
    std::string getUsage() const override { return "exit [--message message]"; }
    std::string getHelp() const override;
    
    bool execute(const std::vector<std::string>& args, NLMCLI& cli) override;
    bool validateArgs(const std::vector<std::string>& args) const override;
};

// Configuration Manager
class ConfigManager {
private:
    std::map<std::string, std::string> currentConfig;
    std::map<std::string, std::string> defaultConfig;
    std::string currentSource;
    
public:
    ConfigManager();
    
    bool loadConfig(const std::string& configFile);
    bool saveConfig(const std::string& configFile);
    
    bool setConfig(const std::string& key, const std::string& value, const std::string& source = "command");
    std::string getConfig(const std::string& key, const std::string& source = "all") const;
    bool resetConfig(const std::string& key, const std::string& source = "default");
    
    void listConfigs() const;
    bool hasConfig(const std::string& key, const std::string& source = "all") const;
    
    std::string getConfigSource(const std::string& key) const;
    void setCurrentSource(const std::string& source) { currentSource = source; }
    std::string getCurrentSource() const { return currentSource; }
};

// Brain Controller
class BrainController {
private:
    std::shared_ptr<nlm::Brain> brain;
    bool brainInitialized;
    
public:
    BrainController();
    
    bool initialize(std::shared_ptr<class nlm::Config> config, int steps = 100);
    void reset();
    bool step(int step = 0);
    bool run(int steps);
    
    bool isInitialized() const { return brainInitialized; }
    std::shared_ptr<class nlm::Brain> getBrain() const { return brain; }
    
    void showStatus(bool verbose = false, bool json = false) const;
    json getStatus(bool verbose = false) const;
};

// Memory Manager
class MemoryManager {
private:
    std::shared_ptr<class nlm::Brain> brain;
    
public:
    MemoryManager();
    
    void setBrain(std::shared_ptr<class nlm::Brain> brain) { this->brain = brain; }
    
    void showWorkingMemory(bool verbose = false) const;
    json getWorkingMemoryInfo() const;
    
    void showEpisodicMemory(bool verbose = false) const;
    json getEpisodicMemoryInfo() const;
    
    void showAssociativeMemory(bool verbose = false) const;
    json getAssociativeMemoryInfo() const;
};

// Prediction Manager
class PredictionManager {
private:
    std::shared_ptr<class nlm::Brain> brain;
    
public:
    PredictionManager();
    
    void setBrain(std::shared_ptr<class nlm::Brain> brain) { this->brain = brain; }
    
    void showPrediction(bool verbose = false) const;
    json getPredictionInfo() const;
    
    void showError(bool verbose = false) const;
    json getErrorInfo() const;
};

// Neuromodulation Manager
class NeuromodulationManager {
private:
    std::shared_ptr<class nlm::Brain> brain;
    
public:
    NeuromodulationManager();
    
    void setBrain(std::shared_ptr<class nlm::Brain> brain) { this->brain = brain; }
    
    void showNeuromodulation(bool verbose = false) const;
    json getNeuromodulationInfo() const;
    
    void showDopamine(bool verbose = false) const;
    json getDopamineInfo() const;
    
    void showCuriosity(bool verbose = false) const;
    json getCuriosityInfo() const;
};

// Development Manager
class DevelopmentManager {
private:
    std::shared_ptr<class nlm::Brain> brain;
    
public:
    DevelopmentManager();
    
    void setBrain(std::shared_ptr<class nlm::Brain> brain) { this->brain = brain; }
    
    void showDevelopment(bool verbose = false) const;
    json getDevelopmentInfo() const;
    
    void showStage() const;
    json getStageInfo() const;
};

// Checkpoint Manager
class CheckpointManager {
private:
    std::string defaultCheckpointPath;
    
public:
    CheckpointManager();
    
    bool saveBrain(const std::string& filepath, std::shared_ptr<class nlm::Brain> brain);
    bool loadBrain(const std::string& filepath, std::shared_ptr<class nlm::Brain> brain, int steps = 0);
    
    void listCheckpoints(const std::string& directory = ".");
    void showCheckpointInfo(const std::string& filepath) const;
};

// Performance Monitor
class PerformanceMonitor {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point lastStepTime;
    
public:
    PerformanceMonitor();
    
    void startMonitoring();
    void stopMonitoring();
    double getElapsedTime() const;
    double getStepTime() const;
    
    bool runBenchmark(const std::string& test, int iterations, const std::string& outputFile);
    bool profilePerformance(const std::string& mode, int steps, const std::string& outputFile);
    
    void showSystemResources() const;
    void showBrainPerformance() const;
};

// Visualizer
class Visualizer {
public:
    Visualizer();
    
    bool createVisualization(const std::string& type, const std::string& outputFile, int steps = 100);
    bool createNeuralNetworkVisualization(const std::string& outputFile, int steps = 100);
    bool createMemoryVisualization(const std::string& outputFile);
    bool createPerformanceVisualization(const std::string& outputFile);
    
    void showVisualizationOptions() const;
};

} // namespace nlm_cli

#endif // NLM_CLI_H