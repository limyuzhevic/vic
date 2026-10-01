#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <variant>
#include "core/Config/Config.hpp"

namespace nlm {

// Forward declarations
class Command;
class CommandHandler;
class Config;

// Command execution status
enum class CommandStatus {
    Success,
    Failed,
    InvalidArguments,
    NotImplemented
};

// Command line option types
enum class OptionType {
    Flag,
    Value,
    String,
    Integer,
    Double,
    Boolean
};

// Command line option structure
struct CommandLineOption {
    std::string name;
    char shortName;
    OptionType type;
    std::string description;
    std::string defaultValue;
    bool required;
    bool hasValue;
    std::vector<std::string> allowedValues;
    std::function<bool(const std::string&)> validator;
};

// Command hierarchy types
enum class CommandCategory {
    Configuration,
    BrainControl,
    SimulationControl,
    Visualization,
    Advanced
};

// Base Command class
class Command {
public:
    virtual ~Command() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual CommandCategory getCategory() const = 0;
    virtual bool execute(const std::map<std::string, std::string>& options, 
                       std::shared_ptr<Config> config) = 0;
    virtual std::vector<CommandLineOption> getOptions() const = 0;
    virtual bool requiresBrain() const = 0;
};

// Configuration Command base class
class ConfigurationCommand : public Command {
public:
    virtual CommandCategory getCategory() const override { return CommandCategory::Configuration; }
    virtual bool requiresBrain() const override { return false; }
};

// Brain Control Command base class
class BrainControlCommand : public Command {
public:
    virtual CommandCategory getCategory() const override { return CommandCategory::BrainControl; }
    virtual bool requiresBrain() const override { return true; }
};

// Simulation Control Command base class
class SimulationControlCommand : public Command {
public:
    virtual CommandCategory getCategory() const override { return CommandCategory::SimulationControl; }
    virtual bool requiresBrain() const override { return true; }
};

// Visualization Command base class
class VisualizationCommand : public Command {
public:
    virtual CommandCategory getCategory() const override { return CommandCategory::Visualization; }
    virtual bool requiresBrain() const override { return true; }
};

// Advanced Command base class
class AdvancedCommand : public Command {
public:
    virtual CommandCategory getCategory() const override { return CommandCategory::Advanced; }
    virtual bool requiresBrain() const override { return false; }
};

// Command execution result
struct CommandResult {
    CommandStatus status;
    std::string message;
    std::shared_ptr<Config> config;
    
    CommandResult(CommandStatus s, const std::string& msg, std::shared_ptr<Config> cfg = nullptr)
        : status(s), message(msg), config(cfg) {}
};

// Command Handler interface
class CommandHandler {
public:
    virtual ~CommandHandler() = default;
    virtual CommandResult execute(const std::string& commandName,
                                const std::map<std::string, std::string>& options,
                                std::shared_ptr<Config> config) = 0;
    virtual std::vector<std::shared_ptr<Command>> getAvailableCommands() const = 0;
    virtual void setBrain(std::shared_ptr<Brain> brain) = 0;
};

// Main CommandLineParser class
class CommandLineParser {
public:
    CommandLineParser();
    ~CommandLineParser();
    
    // Disable copying, enable moving
    CommandLineParser(const CommandLineParser&) = delete;
    CommandLineParser& operator=(const CommandLineParser&) = delete;
    CommandLineParser(CommandLineParser&&) noexcept;
    CommandLineParser& operator=(CommandLineParser&&) noexcept;
    
    // Set command handler
    void setCommandHandler(std::shared_ptr<CommandHandler> handler);
    
    // Parse and execute command line arguments
    CommandResult parseAndExecute(int argc, char** argv);
    
    // Get help text
    std::string getHelp() const;
    
    // Get version information
    static std::string getVersion();
    
    // Get usage examples
    static std::string getUsageExamples();
    
    // Check if arguments contain help or version flags
    static bool needsHelp(int argc, char** argv);
    static bool needsVersion(int argc, char** argv);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Parse arguments into option map
    static std::map<std::string, std::string> parseArguments(int argc, char** argv);
    
    // Validate parsed options against command requirements
    CommandResult validateOptions(const std::string& commandName,
                                 const std::map<std::string, std::string>& options,
                                 const std::vector<CommandLineOption>& commandOptions);
};

} // namespace nlm
