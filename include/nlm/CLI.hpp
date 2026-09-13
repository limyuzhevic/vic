#pragma once

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <memory>
#include <functional>
#include <sstream>

namespace nlm {

/**
 * Professional CLI framework for NLM applications
 * Provides argument parsing, help system, and command management
 */
class CLI {
public:
    // Command line options structure
    struct Options {
        std::string configFile = "configs/default.cfg";
        std::string logLevel = "info";
        std::string logFile = "";
        bool verbose = false;
        bool quiet = false;
        bool help = false;
        bool version = false;
        std::string experimentType = "all";
        uint64_t randomSeed = 42;
        size_t neuronCount = 500;
        size_t regionCount = 1;
        float connectionProbability = 0.15f;
        float inputCurrent = 30.0f;
        size_t steps = 1000;
        size_t simulationRate = 10;
        bool enableLearning = true;
        bool enableDevelopment = false;
        bool enableNeuromodulation = true;
        bool enableMemory = true;
        std::string outputDir = ".";
        std::string checkpointPath = "./checkpoint.bin";
        bool enableCheckpointing = false;
        bool enableReplay = false;
        std::vector<std::string> experimentList;
        std::string experimentParams;
    };

    // Version information
    struct VersionInfo {
        int major = 0;
        int minor = 1;
        int patch = 0;
        std::string name = "NLM (熙然)";
        std::string description = "Neural Learning Machine";
    };

    // Log level enumeration
    enum class LogLevel {
        Trace = 0,
        Debug = 1,
        Info = 2,
        Warning = 3,
        Error = 4,
        Critical = 5,
        Off = 6
    };

    // Exception for CLI errors
    class CLIException : public std::runtime_error {
    public:
        CLIException(const std::string& message) : std::runtime_error(message) {}
    };

    // Command definition for CLI commands
    struct Command {
        std::string name;
        std::string description;
        std::function<bool(const Options&)> handler;
    };

    CLI(const std::string& programName = "nlm", const VersionInfo& version = VersionInfo());
    ~CLI();

    // Parse command line arguments
    bool parse(int argc, char** argv);

    // Get parsed options
    const Options& getOptions() const { return options; }

    // Print help information
    void printHelp() const;

    // Print version information
    void printVersion() const;

    // Add a new command to the CLI
    void addCommand(const std::string& name, const std::string& description, std::function<bool(const Options&)>
    handler);

    // Register default commands
    void registerDefaultCommands();

    // Execute the parsed command
    bool execute();

    // Get current log level from string
    static LogLevel getLogLevelFromString(const std::string& level);

    // Convert log level to string
    static std::string logLevelToString(LogLevel level);

    // Validate options
    bool validateOptions() const;

    // Get program name
    const std::string& getProgramName() const { return programName; }

private:
    std::string programName;
    VersionInfo version;
    Options options;
    std::vector<Command> commands;
    bool helpRequested;
    bool versionRequested;

    // Parse argument string into key-value pair
    void parseArgument(const std::string& arg, std::string& key, std::string& value);

    // Process command line arguments
    void processArguments(int argc, char** argv);

    // Process option arguments (--key value or --key=value)
    void processOptionArgument(const std::string& key, const std::string& value);

    // Process boolean flags
    void processBooleanFlag(const std::string& flag);

    // Print error message
    void printError(const std::string& message) const;

    // Convert string to lowercase
    static std::string toLowercase(const std::string& str);
};

} // namespace nlm
