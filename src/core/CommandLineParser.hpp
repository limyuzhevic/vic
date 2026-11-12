// Command-line argument parsing utilities for NLM
// This file provides comprehensive command-line argument parsing utilities
// that can be used across all NLM executables

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <functional>
#include <algorithm>
#include <cstring>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/ErrorHandling.hpp"

namespace nlm {

// Command-line argument types
enum class ArgumentType {
    Flag,
    Value,
    OptionalValue,
    String,
    Number,
    Boolean,
    FilePath
};

// Command-line argument definition
struct CommandLineArg {
    std::string name;
    ArgumentType type;
    std::string description;
    std::string defaultValue;
    bool required;
    std::function<bool(const std::string&)> validator;
    
    CommandLineArg(const std::string& n, ArgumentType t, const std::string& desc = "", 
                   const std::string& def = "", bool req = false)
        : name(n), type(t), description(desc), defaultValue(def), required(req), validator(nullptr) {}
    
    CommandLineArg(const std::string& n, ArgumentType t, const std::string& desc,
                   const std::string& def, bool req,
                   std::function<bool(const std::string&)> val)
        : name(n), type(t), description(desc), defaultValue(def), required(req), validator(val) {}
};

// Parsed command-line arguments
struct ParsedArguments {
    std::map<std::string, std::string> values;
    std::map<std::string, bool> flags;
    std::vector<std::string> positionalArgs;
    std::map<std::string, std::string> parsedValues;
    
    std::string get(const std::string& key, const std::string& defaultValue = "") const;
    bool getFlag(const std::string& key, bool defaultValue = false) const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    int64_t getInt64(const std::string& key, int64_t defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    float getFloat(const std::string& key, float defaultValue = 0.0f) const;
    
    std::string toString() const;
};

// Command-line parser class
class CommandLineParser {
public:
    CommandLineParser() = default;
    ~CommandLineParser() = default;
    
    // Add argument definition
    void addArgument(const std::string& name, ArgumentType type, 
                     const std::string& description = "",
                     const std::string& defaultValue = "",
                     bool required = false,
                     std::function<bool(const std::string&)> validator = nullptr);
    
    // Add positional argument
    void addPositionalArgument(const std::string& name,
                               ArgumentType type,
                               const std::string& description = "",
                               const std::string& defaultValue = "");
    
    // Parse command line arguments
    ParsedArguments parse(int argc, char* argv[]);
    
    // Show help information
    void printHelp() const;
    
    // Get all argument definitions
    const std::vector<CommandLineArg>& getArguments() const { return arguments_; }
    
    // Get all positional argument definitions
    const std::vector<CommandLineArg>& getPositionalArguments() const { return positionalArguments_; }
    
    // Validate all required arguments
    bool validateRequired() const;
    
    // Validate value against validator
    bool validateValue(const CommandLineArg& arg, const std::string& value) const;
    
    // Get argument count
    size_t getArgumentCount() const { return arguments_.size(); }
    size_t getPositionalArgumentCount() const { return positionalArguments_.size(); }
    
private:
    std::vector<CommandLineArg> arguments_;
    std::vector<CommandLineArg> positionalArguments_;
    
    // Parse single argument
    bool parseArgument(const std::string& arg, std::map<std::string, std::string>& values,
                      std::map<std::string, bool>& flags,
                      std::vector<std::string>& positionalArgs);
    
    // Extract key-value pair from argument
    std::pair<std::string, std::string> extractKeyValue(const std::string& arg) const;
    
    // Convert argument to lowercase
    static std::string toLower(const std::string& str);
    
    // Trim whitespace from string
    static std::string trim(const std::string& str);
};

// Built-in argument definitions for common NLM options
struct BuiltInArgs {
    static CommandLineArg getConfigFile;
    static CommandLineArg getHelp;
    static CommandLineArg getVersion;
    static CommandLineArg getLogLevel;
    static CommandLineArg getNeuronCount;
    static CommandLineArg getRegionCount;
    static CommandLineArg getConnectionProbability;
    static CommandLineArg getRandomSeed;
    static CommandLineArg getSimulationTimestep;
    static CommandLineArg getMaxSimulationSteps;
    static CommandLineArg getEnableVisualization;
    static CommandLineArg getCheckpointFile;
    static CommandLineArg getLoadCheckpoint;
    static CommandLineArg getSaveCheckpoint;
    
    // Performance and profiling options
    static CommandLineArg getProfile;
    static CommandLineArg getProfileFrequency;
    static CommandLineArg getPerformanceMetrics;
    
    // Debug and testing options
    static CommandLineArg getDebug;
    static CommandLineArg getTestMode;
    static CommandLineArg getBenchmark;
    
    // Export and import options
    static CommandLineArg getExportBrain;
    static CommandLineArg getImportBrain;
    
    // Environment variable expansion
    static CommandLineArg getEnvironment;
};

// Advanced command-line parsing utilities
namespace Advanced {
    
    // Command-line argument with callback
    struct CallbackArg {
        std::string name;
        ArgumentType type;
        std::string description;
        std::function<void(const std::string&, ParsedArguments&)> callback;
        
        CallbackArg(const std::string& n, ArgumentType t, 
                   const std::string& desc,
                   std::function<void(const std::string&, ParsedArguments&)> cb)
            : name(n), type(t), description(desc), callback(cb) {}
    };
    
    // Command-line parser with callbacks
    class CallbackParser : public CommandLineParser {
    public:
        void addCallbackArgument(const std::string& name, ArgumentType type,
                                const std::string& description,
                                std::function<void(const std::string&, ParsedArguments&)> callback);
        
        ParsedArguments parse(int argc, char* argv[]);
        
    private:
        std::vector<CallbackArg> callbacks_;
    };
    
    // Command-line completion utility
    class CompletionUtility {
    public:
        static std::vector<std::string> generateCompletions(const std::string& partialCommand,
                                                            const CommandLineParser& parser);
        
        static bool completeArgument(const std::string& partialCommand,
                                    const CommandLineParser& parser);
    };
    
    // Command-line validation utility
    class ValidationUtility {
    public:
        static bool validateEnvironmentVariables(const std::string& value);
        static bool validateFilePath(const std::string& path);
        static bool validateNumberRange(const std::string& value, double min, double max);
        static bool validateBooleanValue(const std::string& value);
        static bool validateLogLevel(const std::string& value);
    };
    
    // Command-line help generator
    class HelpGenerator {
    public:
        static std::string generateHelp(const CommandLineParser& parser,
                                       const std::string& programName = "nlm");
        static std::string generateUsage(const CommandLineParser& parser,
                                        const std::string& programName = "nlm");
        static std::string generateExamples(const CommandLineParser& parser);
    };
    
    // Command-line argument aliases and shortcuts
    class AliasManager {
    public:
        void addAlias(const std::string& alias, const std::string& target);
        std::string resolveAlias(const std::string& alias) const;
        std::vector<std::string> getAliases(const std::string& target) const;
        
    private:
        std::map<std::string, std::string> aliases_;
        std::map<std::string, std::vector<std::string>> reverseAliases_;
    };
    
} // namespace Advanced

// Utility functions for common command-line patterns
namespace Utils {
    
    // Parse common simulation parameters
    ParsedArguments parseSimulationArgs(int argc, char* argv[]);
    
    // Parse common brain configuration parameters
    ParsedArguments parseBrainArgs(int argc, char* argv[]);
    
    // Parse common logging and debugging parameters
    ParsedArguments parseLoggingArgs(int argc, char* argv[]);
    
    // Parse common performance and profiling parameters
    ParsedArguments parsePerformanceArgs(int argc, char* argv[]);
    
    // Merge multiple argument parsers
    ParsedArguments mergeArguments(const std::vector<ParsedArguments>& argsList);
    
    // Convert ParsedArguments to Config
    std::shared_ptr<Config> parsedArgumentsToConfig(const ParsedArguments& parsedArgs);
    
    // Print parsed arguments summary
    void printArgumentsSummary(const ParsedArguments& parsedArgs);
    
    // Check if help or version flag is present
    bool hasHelpOrVersion(int argc, char* argv[]);
    
    // Get help or version message
    std::string getHelpOrVersionMessage();
    
} // namespace Utils

// Convenience parser instances
extern CommandLineParser& getDefaultParser();

} // namespace nlm