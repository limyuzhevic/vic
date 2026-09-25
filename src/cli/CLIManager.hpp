#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

namespace nlm {
namespace cli {

class Command {
public:
    virtual ~Command() = default;
    virtual int execute(const std::vector<std::string>& args) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
};

class CLIManager {
public:
    CLIManager();
    ~CLIManager() = default;
    
    int run(int argc, char** argv);
    void registerCommand(std::unique_ptr<Command> command);
    Command* getCommand(const std::string& name);
    
    // Configuration management
    void parseConfigOptions(const std::vector<std::string>& args);
    void parseControlOptions(const std::vector<std::string>& args);
    void parseLoggingOptions(const std::vector<std::string>& args);
    void parseInteractiveOptions(const std::vector<std::string>& args);
    
    // Option parsing helpers
    std::string getConfigFile() const { return configFile; }
    std::string getConfigKeyValue() const { return configKeyValue; }
    std::string getConfigExportFile() const { return configExportFile; }
    std::string getConfigImportFile() const { return configImportFile; }
    bool getConfigValidate() const { return configValidate; }
    int64_t getNeuronCount() const { return neuronCount; }
    double getSimulationTime() const { return simulationTime; }
    double getTimestep() const { return timestep; }
    int64_t getCheckpointInterval() const { return checkpointInterval; }
    bool getNoCheckpoint() const { return noCheckpoint; }
    std::string getLogLevel() const { return logLevel; }
    std::string getLogFile() const { return logFile; }
    bool getVerbose() const { return verbose; }
    bool getDebug() const { return debug; }
    bool getTrace() const { return trace; }
    bool getInteractive() const { return interactive; }
    bool getWatch() const { return watch; }
    bool getMonitor() const { return monitor; }
    
    std::string getCommand() const { return command; }
    std::string getSubcommand() const { return subcommand; }
    
    // Configuration access
    std::shared_ptr<Config> getConfig() const { return config; }
    std::shared_ptr<Brain> getBrain() const { return brain; }
    
    // Configuration setup
    void setupConfiguration();
    void setupBrain();
    
    // Help and usage
    void printHelp(const std::string& version = "NLM Phase 2", 
                   const std::string& command = "", 
                   const std::string& subcommand = "") const;
    void printVersion() const;
    
    // Command execution
    int executeCommand();
    
private:
    // Registered commands
    std::unordered_map<std::string, std::unique_ptr<Command>> commands;
    
    // Current arguments
    std::vector<std::string> args;
    std::string command;
    std::string subcommand;
    
    // Configuration options
    std::string configFile;
    std::string configKeyValue;
    std::string configExportFile;
    std::string configImportFile;
    bool configValidate;
    
    // Control options
    int64_t neuronCount;
    double simulationTime;
    double timestep;
    int64_t checkpointInterval;
    bool noCheckpoint;
    
    // Logging options
    std::string logLevel;
    std::string logFile;
    bool verbose;
    bool debug;
    bool trace;
    
    // Interactive options
    bool interactive;
    bool watch;
    bool monitor;
    
    // Core objects
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    
    // Parsed from command line
    int argc;
    char** argv;
};

} // namespace cli
} // namespace nlm
