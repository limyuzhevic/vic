#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <optional>

namespace nlm {

// Core CLI types
using StringList = std::vector<std::string>;
using StringMap = std::map<std::string, std::string>;
using StringOption = std::pair<std::string, std::optional<std::string>>;
using CommandHandler = std::function<int(const StringList&)>;

// Option types for command-line arguments
enum class OptionType {
    Flag,
    Value,
    RequiredValue,
    OptionalValue,
    Range,
    Choice
};

// Option configuration structure
struct CLIOption {
    std::string name;
    std::string shortName;
    std::string description;
    OptionType type;
    bool required;
    std::string defaultValue;
    std::string validationPattern;
    std::vector<std::string> choices;
    std::string valueDescription;
    
    CLIOption(const std::string& name, const std::string& shortName, 
              const std::string& description, OptionType type = OptionType::Value,
              bool required = false, const std::string& defaultValue = "")
        : name(name), shortName(shortName), description(description), type(type),
          required(required), defaultValue(defaultValue) {}
};

// Command parameter structure
struct CLIParameter {
    std::string name;
    std::string description;
    bool required;
    std::string type;
    std::string defaultValue;
    
    CLIParameter(const std::string& name, const std::string& description,
                 bool required = false, const std::string& type = "string",
                 const std::string& defaultValue = "")
        : name(name), description(description), required(required),
          type(type), defaultValue(defaultValue) {}
};

// Command class for subcommand implementations
class CLICommand {
public:
    virtual ~CLICommand() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual std::string getHelp() const = 0;
    
    virtual int execute(const StringList& args) = 0;
    virtual bool validate(const StringList& args) const = 0;
    
    virtual const std::vector<CLIOption>& getOptions() const = 0;
    virtual const std::vector<CLIParameter>& getParameters() const = 0;
};

// Basic command implementation base class
class BasicCLICommand : public CLICommand {
protected:
    std::string name;
    std::string description;
    std::string usage;
    std::string help;
    std::vector<CLIOption> options;
    std::vector<CLIParameter> parameters;
    
public:
    BasicCLICommand(const std::string& name, const std::string& description,
                    const std::string& usage, const std::string& help = "")
        : name(name), description(description), usage(usage), help(help) {}
    
    std::string getName() const override { return name; }
    std::string getDescription() const override { return description; }
    std::string getUsage() const override { return usage; }
    std::string getHelp() const override { return help; }
    
    virtual int execute(const StringList& args) override = 0;
    virtual bool validate(const StringList& args) const override = 0;
    
    const std::vector<CLIOption>& getOptions() const override { return options; }
    const std::vector<CLIParameter>& getParameters() const override { return parameters; }
    
protected:
    // Helper methods for command execution
    std::string getOptionValue(const StringList& args, const std::string& option) const;
    bool hasOption(const StringList& args, const std::string& option) const;
    std::string getRequiredParameter(const StringList& args, size_t index) const;
    std::string getOptionalParameter(const StringList& args, size_t index) const;
};

// Command group for organizing related commands
class CLIGroup {
public:
    CLIGroup(const std::string& name, const std::string& description = "")
        : name(name), description(description) {}
    
    void addCommand(std::shared_ptr<CLICommand> command);
    std::shared_ptr<CLICommand> getCommand(const std::string& name) const;
    bool hasCommand(const std::string& name) const;
    
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    const std::vector<std::shared_ptr<CLICommand>>& getCommands() const { return commands; }
    
    void displayHelp(std::ostream& out = std::cout) const;
    
private:
    std::string name;
    std::string description;
    std::vector<std::shared_ptr<CLICommand>> commands;
};

// Main CLI engine with subcommand support
class CLIEngine {
public:
    CLIEngine();
    ~CLIEngine();
    
    // Register command groups
    void registerGroup(std::shared_ptr<CLIGroup> group);
    void registerCommand(std::shared_ptr<CLICommand> command);
    
    // Command execution
    bool parse(int argc, char** argv);
    int execute();
    
    // Help and documentation
    void showHelp(const std::string& command = "", const std::string& group = "") const;
    void showCommandHelp(const std::string& command) const;
    
    // Configuration and state
    void setProgramName(const std::string& name) { programName = name; }
    std::string getProgramName() const { return programName; }
    
    // Error handling
    void setExitOnError(bool exit) { exitOnError = exit; }
    bool shouldExitOnError() const { return exitOnError; }
    
private:
    std::string programName;
    bool exitOnError;
    std::vector<std::shared_ptr<CLIGroup>> groups;
    std::map<std::string, std::shared_ptr<CLICommand>> commands;
    
    // Internal parsing methods
    bool parseCommandLine(int argc, char** argv);
    bool parseOptions(StringList& args, const std::vector<CLIOption>& options) const;
    bool parseParameters(StringList& args, const std::vector<CLIParameter>& parameters) const;
    
    // Command lookup and execution
    std::shared_ptr<CLICommand> findCommand(const std::string& commandName) const;
    std::shared_ptr<CLIGroup> findGroup(const std::string& groupName) const;
    
    // Error reporting
    void printError(const std::string& message) const;
    void printUsage(const std::string& command) const;
};

// Utility functions for CLI applications
namespace CLIUtils {
    // String manipulation
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    bool startsWith(const std::string& str, const std::string& prefix);
    std::string capitalize(const std::string& str);
    
    // Output formatting
    void printColored(const std::string& text, const std::string& color = "");
    void printProgress(const std::string& message, double progress = -1.0);
    void printSeparator(char separator = '-', size_t width = 80);
    
    // Validation
    bool isValidFilePath(const std::string& path);
    bool isValidFileName(const std::string& name);
    std::string generateTimestamp();
    std::string formatDuration(double seconds);
}

} // namespace nlm
