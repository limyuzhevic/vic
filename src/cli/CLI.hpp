#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <iostream>
#include <variant>
#include "../core/Config/Config.hpp"

namespace nlm {

// Output format types
enum class OutputFormat {
    HumanReadable,
    JSON,
    CSV,
    Quiet
};

// Command groups
enum class CommandGroup {
    Config,
    Experiment,
    Inspection,
    Output,
    Batch,
    System
};

// Base command interface
class Command {
public:
    virtual ~Command() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual CommandGroup getGroup() const = 0;
    virtual void execute(std::shared_ptr<Config> config, const std::vector<std::string>& args) = 0;
    virtual void printHelp(std::ostream& out) const = 0;
    virtual std::string getUsage() const = 0;
};

// CLI configuration and state
struct CLIConfig {
    OutputFormat outputFormat = OutputFormat::HumanReadable;
    bool verbose = false;
    bool quiet = false;
    std::string outputFile;
    std::string configFile = "configs/default.cfg";
    std::string logFile;
    std::string batchFile;
    std::string scenarioFile;
    std::string profileFile;
    bool validateParams = false;
    bool showVersion = false;
    bool showHelp = false;
    std::string version = "1.0.0";
    std::string buildInfo = "Release Build";
    std::string features = "Real Neural Computation";
};

// Enhanced CLI class
class CLI {
public:
    CLI();
    ~CLI() = default;
    
    // Main entry point
    int run(int argc, char** argv);
    
    // Command management
    void registerCommand(std::unique_ptr<Command> cmd);
    
    // Execution helpers
    void setConfig(std::shared_ptr<Config> config) { m_config = config; }
    std::shared_ptr<Config> getConfig() { return m_config; }
    
    // Output helpers
    void printOutput(const std::string& message, OutputFormat format = OutputFormat::HumanReadable);
    void printError(const std::string& error);
    void printHelp(CommandGroup group) const;
    void printVersion() const;
    void printBanner() const;
    
    // Config helpers
    void loadConfigFromFile(const std::string& filepath);
    void saveConfigToFile(const std::string& filepath);
    void validateConfig() const;
    void printConfigSummary() const;
    void printConfigStats() const;
    
    // Parameter helpers
    void setParameter(const std::string& key, const std::string& value);
    void getParameter(const std::string& key);
    void listParameters() const;
    void exportParameters(const std::string& format);
    
    // Brain inspection helpers
    void inspectBrain() const;
    void checkNeuralIntegrity() const;
    void analyzePlasticity() const;
    
    // Experiment helpers
    void runExperiment(const std::string& name = "default");
    void resetBrain() const;
    void getExperimentStatus() const;
    
    // Batch processing helpers
    void batchRun(const std::string& profile);
    void scenarioRun(const std::string& scenario);
    
    // Parsing helpers
    static std::pair<std::string, std::vector<std::string>> parseCommandLine(int argc, char** argv);
    static std::string trim(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    
private:
    std::shared_ptr<Config> m_config;
    CLIConfig m_cliConfig;
    std::map<std::string, std::unique_ptr<Command>> m_commands;
    std::string m_currentCommand;
};

} // namespace nlm