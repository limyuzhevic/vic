#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include "../src/core/Config/Config.hpp"

namespace nlm {
namespace cli {

// CLI result structure
struct CLIResult {
    bool success;
    std::string message;
    std::string error;
    std::map<std::string, std::string> metadata;
    
    CLIResult() : success(false) {}
    CLIResult(bool s, const std::string& msg = "", const std::string& err = "")
        : success(s), message(msg), error(err) {}
};

// Verbosity levels
enum class VerbosityLevel {
    Quiet = 0,
    Error = 1,
    Warning = 2,
    Info = 3,
    Debug = 4
};

// Base subcommand interface
class Subcommand {
public:
    virtual ~Subcommand() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual void addOptions() = 0;
    virtual CLIResult execute(const std::shared_ptr<Config>& config) = 0;
    virtual void showHelp() const = 0;
};

// Main CLI class
class CLI {
private:
    VerbosityLevel verbosity;
    std::shared_ptr<Config> config;
    std::map<std::string, std::unique_ptr<Subcommand>> subcommands;
    
    // Common options
    std::string configFile;
    bool helpRequested;
    bool versionRequested;
    std::string outputFile;
    
    // Help and usage
    std::string programName;
    std::string version;
    
public:
    CLI(int argc, char** argv);
    ~CLI() = default;
    
    // Public interface
    bool parseArguments(int argc, char** argv);
    bool execute() const;
    void showHelp() const;
    void showVersion() const;
    
    // Configuration management
    bool loadConfigFromFile(const std::string& filepath);
    void applyCommandLineConfig(int argc, char** argv);
    
    // Utility methods
    VerbosityLevel getVerbosity() const { return verbosity; }
    const std::shared_ptr<Config>& getConfig() const { return config; }
    const std::string& getProgramName() const { return programName; }
    
    // Verbosity-based logging
    void log(const std::string& message, VerbosityLevel level = VerbosityLevel::Info) const;
    void error(const std::string& message) const;
    void warning(const std::string& message) const;
    void info(const std::string& message) const;
    void debug(const std::string& message) const;
    
    // Register subcommands
    void registerSubcommand(std::unique_ptr<Subcommand> subcommand);
    
private:
    bool isOption(const std::string& arg) const;
    std::string extractOptionValue(const std::string& arg) const;
    bool handleCommonOptions(std::map<std::string, std::string>& options);
    void setupDefaultConfig();
};

} // namespace cli
} // namespace nlm
