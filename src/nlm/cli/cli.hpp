// nlm/cli/cli.hpp - Unified command-line interface for NLM
#pragma once

#include "brain/BrainFactory.hpp"
#include "nlm/api.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace nlm {
namespace cli {

// Command-line option types
class CLICommand {
public:
    std::string name;
    std::string description;
    std::string usage;
    std::string helpText;
    bool requiresArgument;
    bool isActive;
    
    CLICommand(const std::string& cmdName, const std::string& cmdDescription,
               const std::string& cmdUsage, const std::string& cmdHelp,
               bool requiresArg = false)
        : name(cmdName), description(cmdDescription), usage(cmdUsage),
          helpText(cmdHelp), requiresArgument(requiresArg), isActive(false) {}
};

// Command-line argument parser
class CLIArguments {
public:
    std::map<std::string, std::string> options;
    std::vector<std::string> positionalArgs;
    std::string command;
    bool helpRequested;
    bool versionRequested;
    
    CLIArguments(int argc, char** argv);
    bool hasOption(const std::string& option) const;
    std::string getOption(const std::string& option, const std::string& defaultValue = "") const;
    int getOptionAsInt(const std::string& option, int defaultValue = 0) const;
    float getOptionAsFloat(const std::string& option, float defaultValue = 0.0f) const;
    double getOptionAsDouble(const std::string& option, double defaultValue = 0.0) const;
    bool getOptionAsBool(const std::string& option, bool defaultValue = false) const;
    bool hasPositionalArg(size_t index) const;
    std::string getPositionalArg(size_t index) const;
    size_t getPositionalArgCount() const;
    void printUsage() const;
};

// Command handler interface
class CLICommandHandler {
public:
    virtual ~CLICommandHandler() = default;
    virtual bool handleCommand(const CLIArguments& args) = 0;
    virtual std::string getCommandName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
};

// Demo command handler
class DemoCommandHandler : public CLICommandHandler {
public:
    bool handleCommand(const CLIArguments& args) override;
    std::string getCommandName() const override { return "demo"; }
    std::string getDescription() const override { return "Run NLM demos (Phase 2, 3, 4, 5, 6)"; }
    std::string getUsage() const override { return "nlm demo [phase] [--episodes N] [--steps N] [--config file]"; }
};

// Run command handler
class RunCommandHandler : public CLICommandHandler {
public:
    bool handleCommand(const CLIArguments& args) override;
    std::string getCommandName() const override { return "run"; }
    std::string getDescription() const override { return "Run brain experiments and simulations"; }
    std::string getUsage() const override { return "nlm run [--config file] [--steps N] [--controller config]"; }
};

// Benchmark command handler
class BenchmarkCommandHandler : public CLICommandHandler {
public:
    bool handleCommand(const CLIArguments& args) override;
    std::string getCommandName() const override { return "benchmark"; }
    std::string getDescription() const override { return "Run performance benchmarks"; }
    std::string getUsage() const override { return "nlm benchmark [--neurons N] [--steps N] [--repeats N]"; }
};

// Profile command handler
class ProfileCommandHandler : public CLICommandHandler {
public:
    bool handleCommand(const CLIArguments& args) override;
    std::string getCommandName() const override { return "profile"; }
    std::string getDescription() const override { return "Profile brain performance and memory usage"; }
    std::string getUsage() const override { return "nlm profile [--output file] [--duration seconds] [--metrics file]"; }
};

// Plugin command handler
class PluginCommandHandler : public CLICommandHandler {
public:
    bool handleCommand(const CLIArguments& args) override;
    std::string getCommandName() const override { return "plugin"; }
    std::string getDescription() const override { return "Manage integration plugins"; }
    std::string getUsage() const override { return "nlm plugin list|load|unload|info [plugin]"; }
};

// Config command handler
class ConfigCommandHandler : public CLICommandHandler {
public:
    bool handleCommand(const CLIArguments& args) override;
    std::string getCommandName() const override { return "config"; }
    std::string getDescription() const override { return "Manage configuration files and settings"; }
    std::string getUsage() const override { return "nlm config save|load|show [--file file] [--key key]"; }
};

// Main CLI application
class CLIApplication {
public:
    CLIApplication();
    ~CLIApplication();
    
    // Application setup
    void addCommandHandler(std::unique_ptr<CLICommandHandler> handler);
    void setDefaultCommand(const std::string& commandName);
    void setHelpText(const std::string& helpText);
    void setVersion(const std::string& version);
    
    // Command execution
    bool run(int argc, char** argv);
    bool run(const std::vector<std::string>& args);
    
    // Command discovery
    std::vector<std::string> getAvailableCommands() const;
    CLICommandHandler* getCommandHandler(const std::string& commandName);
    
    // Help and documentation
    void printHelp() const;
    void printCommandHelp(const std::string& commandName) const;
    void printVersion() const;
    void printWelcome() const;
    
    // Configuration
    void setWorkingDirectory(const std::string& directory);
    std::string getWorkingDirectory() const;
    
    // Plugin management
    void loadPluginsFromDirectory(const std::string& directory);
    void unloadPlugin(const std::string& name);
    bool isPluginLoaded(const std::string& name) const;
    
    // Statistics and monitoring
    size_t getCommandCount() const;
    double getTotalExecutionTime() const;
    void printStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Command-line tool entry point
class CLITool {
public:
    static int main(int argc, char** argv);
    static int run(CLIApplication& app, int argc, char** argv);
    static int run(CLIApplication& app, const std::vector<std::string>& args);
    static bool initialize();
    static bool shutdown();
    static bool isInitialized();
    static std::string getVersion();
    static void printBanner();
    
    // Global application access
    static CLIApplication& getApplication();
    static void setApplication(CLIApplication& app);
    
    // Plugin registration
    static bool registerCommandHandler(std::unique_ptr<CLICommandHandler> handler);
    static bool unregisterCommandHandler(const std::string& commandName);
    static std::vector<std::string> getAvailableCommands();
};}
