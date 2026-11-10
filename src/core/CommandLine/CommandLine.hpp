#ifndef NLM_COMMAND_LINE_H
#define NLM_COMMAND_LINE_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace nlm {

// Command line argument parsing and configuration
struct CommandLineArgs {
    std::string programName;
    std::map<std::string, std::string> options;
    std::vector<std::string> positionalArgs;
    bool helpRequested = false;
    bool versionRequested = false;
    bool listConfigKeys = false;
    bool validateOnly = false;
    std::string configFile;
    std::string outputFile;
    std::string logLevel;
    bool enableProfiling = false;
    size_t profilingInterval = 0;
    std::vector<std::string> customKeys;
};

class CommandLineProcessor {
public:
    static CommandLineProcessor& getInstance() {
        static CommandLineProcessor instance;
        return instance;
    }
    
    // Parse command line arguments
    CommandLineArgs parse(int argc, char** argv);
    
    // Get parsed arguments
    const CommandLineArgs& getArgs() const { return args; }
    
    // Generate help message
    std::string generateHelp(const std::string& programName) const;
    
    // Generate usage information
    std::string generateUsage() const;
    
    // Validate arguments
    bool validateArguments(const CommandLineArgs& args) const;
    
    // Process common NLM arguments
    void processCommonArgs(CommandLineArgs& args);
    
    // Process NLM-specific arguments
    void processNLMArgs(CommandLineArgs& args);
    
    // Get help for specific keys
    std::string getHelpForKey(const std::string& key) const;
    
    // List all configuration keys
    std::vector<std::string> listAvailableKeys() const;
    
private:
    CommandLineArgs args;
    
    // Common argument definitions
    struct ArgumentDefinition {
        std::string name;
        std::string shortForm;
        std::string longForm;
        std::string description;
        std::string type;
        std::string defaultValue;
        bool hasArgument;
    };
};

// Global command line processor instance
inline CommandLineProcessor& cmdLine = CommandLineProcessor::getInstance();

// Convenience functions for common operations
inline bool cmdLineParse(int argc, char** argv) {
    CommandLineArgs args = cmdLine.parse(argc, argv);
    return !args.helpRequested;
}

inline std::string cmdLineGetOption(const std::string& key, const std::string& default = "") {
    auto& args = cmdLine.getArgs();
    if (args.options.find(key) != args.options.end()) {
        return args.options[key];
    }
    return default;
}

inline void cmdLineSetOption(const std::string& key, const std::string& value) {
    cmdLine.getArgs().options[key] = value;
}

// Advanced command line features
struct BatchMode {
    std::string inputFile;
    std::string outputFile;
    std::map<std::string, std::string> overrides;
    bool validateOnly = false;
    bool generateProfile = false;
};

struct InteractiveMode {
    bool enable = false;
    std::string promptFile;
    std::string outputFile;
    bool autoSave = true;
    size_t autoSaveInterval = 0;
};

struct DevelopmentMode {
    bool enable = false;
    std::string debugLogFile;
    std::string performanceProfileFile;
    bool verboseLogging = false;
    bool enableMemoryTracking = false;
    bool enableStepByStep = false;
};

// Factory functions for different modes
inline BatchMode createBatchMode(int argc, char** argv) {
    BatchMode mode;
    auto& args = cmdLine.getArgs();
    
    if (args.options.find("input") != args.options.end()) {
        mode.inputFile = args.options["input"];
    }
    if (args.options.find("output") != args.options.end()) {
        mode.outputFile = args.options["output"];
    }
    mode.validateOnly = args.validateOnly;
    mode.generateProfile = args.enableProfiling;
    
    return mode;
}

inline InteractiveMode createInteractiveMode(int argc, char** argv) {
    InteractiveMode mode;
    auto& args = cmdLine.getArgs();
    
    mode.enable = args.options.find("interactive") != args.options.end();
    if (args.options.find("prompt") != args.options.end()) {
        mode.promptFile = args.options["prompt"];
    }
    if (args.options.find("output") != args.options.end()) {
        mode.outputFile = args.options["output"];
    }
    mode.autoSave = args.options.find("auto_save") != args.options.end();
    if (args.options.find("save_interval") != args.options.end()) {
        mode.autoSaveInterval = std::stoi(args.options["save_interval"]);
    }
    
    return mode;
}

inline DevelopmentMode createDevelopmentMode(int argc, char** argv) {
    DevelopmentMode mode;
    auto& args = cmdLine.getArgs();
    
    mode.enable = args.options.find("dev") != args.options.end() || 
                  args.options.find("debug") != args.options.end();
    if (args.options.find("debug_log") != args.options.end()) {
        mode.debugLogFile = args.options["debug_log"];
    }
    if (args.options.find("profile_file") != args.options.end()) {
        mode.performanceProfileFile = args.options["profile_file"];
    }
    mode.verboseLogging = args.options.find("verbose") != args.options.end();
    mode.enableMemoryTracking = args.options.find("memory_tracking") != args.options.end();
    mode.enableStepByStep = args.options.find("step_by_step") != args.options.end();
    
    return mode;
}

// Configuration file operations
struct ConfigFileManager {
    static bool backupConfig(const std::string& filepath);
    static bool validateConfig(const std::string& filepath);
    static bool mergeConfigs(const std::string& baseFile, const std::string& overrideFile);
    static bool exportConfig(const std::string& filepath, const std::map<std::string, std::string>& config);
    static std::map<std::string, std::string> importConfig(const std::string& filepath);
    static bool listConfigKeys(const std::string& filepath, bool showDescriptions = true);
};

// Configuration migration utilities
struct ConfigMigration {
    static bool migrateFromLegacy(const std::string& oldFilepath);
    static bool migrateToModern(const std::string& filepath, const std::string& outputFile = "");
    static bool validateModernConfig(const std::string& filepath);
};

// Utility functions for command line processing
inline std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

inline std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

inline bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

inline bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Logging utilities for command line
inline void logCommandLineError(const std::string& message) {
    NLM_LOG_ERROR("Command line error: " + message);
}

inline void logCommandLineWarning(const std::string& message) {
    NLM_LOG_WARNING("Command line warning: " + message);
}

inline void logCommandLineInfo(const std::string& message) {
    NLM_LOG_INFO("Command line info: " + message);
}

} // namespace nlm

#endif // NLM_COMMAND_LINE_H

