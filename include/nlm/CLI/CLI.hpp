// NLM (熙然) - Neural Learning Machine
// CLI Argument Parser Framework
//
// This framework provides a modern, consistent command-line interface
// for all NLM executables with support for help, logging, configuration,
// and parameter overrides.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <set>

namespace nlm {

class CLIArgumentParser {
public:
    struct ArgumentInfo {
        std::string name;
        std::string shortName;
        std::string description;
        std::string defaultValue;
        std::function<bool(std::string)> validator;
        std::function<void(std::string)> callback;
        bool required;
        std::string valueType;
    };

    CLIArgumentParser(const std::string& programName, const std::string& description);
    ~CLIArgumentParser() = default;

    // Add argument
    void addArgument(const std::string& name, const std::string& shortName = "",
                     const std::string& description = "", 
                     const std::string& defaultValue = "",
                     const std::function<bool(std::string)>& validator = nullptr,
                     const std::function<void(std::string)>& callback = nullptr,
                     bool required = false,
                     const std::string& valueType = "string");

    // Add flag (boolean argument)
    void addFlag(const std::string& name, const std::string& description, 
                 const std::function<void(bool)>& callback = nullptr);

    // Parse arguments
    bool parse(int argc, char** argv);

    // Get argument value
    template<typename T>
    std::optional<T> getArgument(const std::string& name) const;

    // Get flag value
    bool getFlag(const std::string& name) const;

    // Check if argument was provided
    bool hasArgument(const std::string& name) const;

    // Check if flag is set
    bool hasFlag(const std::string& name) const;

    // Print help
    void printHelp(std::ostream& out = std::cout) const;

    // Print version
    void printVersion(std::ostream& out = std::cout) const;

    // Get program name
    const std::string& getProgramName() const { return programName; }

    // Get description
    const std::string& getDescription() const { return description; }

    // Get all argument names
    std::vector<std::string> getArgumentNames() const;

    // Get all flag names
    std::vector<std::string> getFlagNames() const;

    // Set version string
    void setVersion(const std::string& version) { this->version = version; }

    // Set help text
    void setHelpText(const std::string& help) { this->helpText = help; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    std::string programName;
    std::string description;
    std::string version;
    std::string helpText;

    // Internal parsing helpers
    bool parseArgument(const std::string& arg, std::string& currentKey);
    bool validateAndSet(const std::string& key, const std::string& value);
    std::string trim(const std::string& str) const;
    std::string toLower(const std::string& str) const;
};

// Convenience functions for common argument types
namespace CLI {
    // Create standard argument parsers for common use cases
    bool parseInt(const std::string& value, int& output);
    bool parseDouble(const std::string& value, double& output);
    bool parseBool(const std::string& value, bool& output);
    bool parseString(const std::string& value, std::string& output);

    // Standard validators
    bool validatePositiveInt(const std::string& value);
    bool validatePositiveDouble(const std::string& value);
    bool validateNonEmptyString(const std::string& value);
    bool validateFileExists(const std::string& value);

    // Standard callbacks
    void setLogLevel(const std::string& level);
    void setLogFile(const std::string& filename);
    void enableBenchmark(const std::string& name);
    void setCheckpointFile(const std::string& filename);
}

// Global CLI parser instance helper
class GlobalCLI {
public:
    static CLIArgumentParser& getParser(const std::string& programName, 
                                       const std::string& description);
    static void registerStandardArguments(CLIArgumentParser& parser);
};

// Standardized argument definitions for NLM executables
namespace StandardArgs {
    // Help and version
    const std::string HELP = "--help";
    const std::string VERSION = "--version";
    const std::string SHORT_HELP = "-h";
    const std::string SHORT_VERSION = "-v";

    // Logging
    const std::string VERBOSE = "--verbose";
    const std::string QUIET = "--quiet";
    const std::string LOG_LEVEL = "--log-level";
    const std::string LOG_FILE = "--log-file";

    // Configuration
    const std::string CONFIG = "--config";
    const std::string SET = "--set";

    // Testing and control
    const std::string TEST = "--test";
    const std::string STEPS = "--steps";
    const std::string DURATION = "--duration";

    // Output and performance
    const std::string OUTPUT = "--output";
    const std::string BENCHMARK = "--benchmark";
    const std::string CHECKPOINT = "--checkpoint";

    // Mode selection
    const std::string MODE = "--mode";
    const std::string VERBOSE_MODE = "--verbose-mode";
}

} // namespace nlm
