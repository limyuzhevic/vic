// Command Line Interface Framework
// Core CLI parsing and command infrastructure for NLM

#ifndef NLM_CLI_H
#define NLM_CLI_H

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <iomanip>
#include <variant>
#include <optional>

namespace nlm {
namespace cli {

// Forward declarations
class Command;
class OptionParser;
class HelpSystem;

// Command categories for organization
enum class CommandCategory {
    SIMULATION,
    CONFIGURATION,
    DEBUGGING,
    VALIDATION,
    PLUGIN,
    PERFORMANCE,
    ANALYSIS,
    UTILITIES
};

// Command base class - all commands inherit from this
class Command {
public:
    virtual ~Command() = default;
    
    // Command identification
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual CommandCategory getCategory() const = 0;
    
    // Command execution
    virtual bool execute(const std::vector<std::string>& args) = 0;
    virtual void showHelp() const = 0;
    
    // Advanced features
    virtual bool validateArguments(const std::vector<std::string>& args) const { return true; }
    virtual std::string getUsage() const { return ""; }
    virtual std::vector<std::string> getExamples() const { return {}; }
};

// Option types for flexible parameter handling
enum class OptionType {
    FLAG,
    STRING,
    INTEGER,
    DOUBLE,
    FLOAT,
    BOOLEAN
};

// Command line option definition
struct Option {
    std::string name;
    char shortName;
    OptionType type;
    std::string description;
    std::string defaultValue;
    bool required;
    std::vector<std::string> validValues;
    
    Option(std::string n, char s, OptionType t, std::string desc, 
           std::string def = "", bool req = false)
        : name(n), shortName(s), type(t), description(desc), 
          defaultValue(def), required(req) {}
};

// Global options that apply to all commands
struct GlobalOptions {
    bool verbose = false;
    bool quiet = false;
    std::string logLevel = "info";
    std::string configFile = "configs/default.cfg";
    bool enableProfiling = false;
    bool enableBenchmarking = false;
    int maxThreads = 1;
    bool helpRequested = false;
    bool versionRequested = false;
    std::string outputFormat = "text";
    std::string outputFile = "";
};

// Advanced options for complex simulations
struct AdvancedOptions {
    // Simulation options
    int maxSteps = 1000;
    double timestep = 0.001;
    int neuronCount = 500;
    std::string neuronModel = "LIF";
    std::string plasticityRule = "STDP";
    
    // Output options
    std::string outputFile;
    std::string outputFormat = "text";
    bool quietMode = false;
    bool verboseMode = false;
    bool enableProgress = true;
    
    // Performance options
    bool enableProfiling = false;
    bool enableBenchmarking = false;
    int parallelThreads = 1;
    int memoryLimit = 0; // MB, 0 = unlimited
    
    // Debugging options
    bool enableDebug = false;
    std::string debugLevel = "info";
    bool enableValidation = false;
    bool enableLogging = false;
    std::string debugFile = "";
    
    // Plugin options
    std::vector<std::string> pluginPaths;
    bool loadPlugins = true;
    bool enablePluginHotSwapping = false;
    
    // Analysis options
    bool enableAnalysis = false;
    std::string analysisType = "performance";
    std::string analysisOutput = "";
};

// Command Line Interface - main CLI framework
class CommandLine {
private:
    std::vector<std::unique_ptr<Command>> commands;
    GlobalOptions globalOptions;
    std::unique_ptr<HelpSystem> helpSystem;
    bool interactiveMode = false;
    
public:
    CommandLine();
    ~CommandLine() = default;
    
    // Command management
    void addCommand(std::unique_ptr<Command> cmd);
    Command* getCommand(const std::string& name) const;
    
    // Option parsing and execution
    bool execute(int argc, char** argv);
    bool parseGlobalOptions(int argc, char** argv);
    bool parseCommandOptions(const std::string& cmdName, 
                            const std::vector<std::string>& args);
    
    // Help and interactive features
    void showHelp(const std::string& commandName = "") const;
    void showVersion() const;
    void setInteractive(bool enable) { interactiveMode = enable; }
    bool isInteractive() const { return interactiveMode; }
    
    // Global options access
    const GlobalOptions& getGlobalOptions() const { return globalOptions; }
    GlobalOptions& getGlobalOptions() { return globalOptions; }
};

// Basic option parser - handles command line options
class OptionParser {
private:
    std::vector<Option> options;
    std::unordered_map<std::string, std::string> parsedOptions;
    std::vector<std::string> positionalArgs;
    
public:
    OptionParser() = default;
    
    // Option registration
    void addOption(const Option& opt);
    void addGlobalOptions();
    
    // Parsing
    bool parse(const std::vector<std::string>& args);
    bool parse(int argc, char** argv);
    
    // Query results
    std::string getOption(const std::string& name) const;
    bool hasOption(const std::string& name) const;
    std::vector<std::string> getPositionalArgs() const { return positionalArgs; }
    
    // Validation
    bool validateRequiredOptions() const;
    std::vector<std::string> getMissingRequiredOptions() const;
};

// Comprehensive help system with examples
class HelpSystem {
private:
    std::unordered_map<std::string, std::string> commandDescriptions;
    std::unordered_map<std::string, std::string> commandUsage;
    std::unordered_map<std::string, std::vector<std::string>> commandExamples;
    bool formattedOutput = true;
    
public:
    HelpSystem() = default;
    
    // Command help registration
    void registerCommand(const std::string& name, const std::string& description, 
                        const std::string& usage = "",
                        const std::vector<std::string>& examples = {});
    
    // Help display
    void showHelp(const std::string& commandName = "") const;
    void showCommandHelp(const std::string& commandName) const;
    void showAllCommands() const;
    void showCategoryHelp(CommandCategory category) const;
    
    // Interactive help
    void startInteractiveHelp() const;
    void setFormattedOutput(bool enable) { formattedOutput = enable; }
    
    // Examples management
    void addExample(const std::string& commandName, const std::string& example);
};

// Configuration file integration
class ConfigManager {
private:
    std::string configFile;
    std::unordered_map<std::string, std::variant<std::string, int, double, bool>> configValues;
    
public:
    ConfigManager(const std::string& file = "configs/default.cfg");
    
    // File operations
    bool loadFromFile(const std::string& file);
    bool saveToFile(const std::string& file) const;
    
    // Configuration access
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue) const;
    
    void setValue(const std::string& key, const std::variant<std::string, int, double, bool>& value);
    
    // Command line merging
    void mergeWithCommandLine(const std::vector<std::string>& args);
    
    // Environment variable support
    void loadFromEnvironment();
    static std::string getEnvVar(const std::string& name);
};

// Progress indicator for long-running operations
class ProgressIndicator {
private:
    bool enabled = true;
    std::string message;
    int totalSteps;
    int currentStep;
    bool indeterminate = false;
    
public:
    ProgressIndicator(bool enabled = true) : enabled(enabled) {}
    
    void start(const std::string& msg, int total, bool indeterminate = false);
    void update(int step);
    void update();
    void complete();
    void setMessage(const std::string& msg) { message = msg; }
    void disable() { enabled = false; }
    bool isEnabled() const { return enabled; }
};

// Status reporter for command execution
class StatusReporter {
public:
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);
    static void success(const std::string& message);
    static void progress(const std::string& message);
    
    static void setGlobalOptions(const GlobalOptions& opts) { globalOpts = opts; }
    static void enableColoredOutput(bool enable) { coloredOutput = enable; }
    
private:
    static GlobalOptions globalOpts;
    static bool coloredOutput;
};

// Environment variable helper
class Environment {
public:
    static std::string getVar(const std::string& name, const std::string& defaultValue = "");
    static int getVar(const std::string& name, int defaultValue);
    static double getVar(const std::string& name, double defaultValue);
    static bool getVar(const std::string& name, bool defaultValue);
    static std::vector<std::string> getVarList(const std::string& name, char separator = ',');
    
    static bool setVar(const std::string& name, const std::string& value);
    static bool isSet(const std::string& name);
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_H
