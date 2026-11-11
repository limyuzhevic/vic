#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <memory>
#include <functional>
#include <atomic>

namespace nlm {

// Command types for unified NLM interface
enum class CommandType {
    RUN, STEP, PAUSE, RESUME, SAVE, LOAD, CONFIG, STATUS, DEBUG,
    VISUALIZE, SCRIPT, HELP, VERSION, QUIT, EXIT
};

// Command line option structure
struct CommandLineOption {
    std::string flag;
    std::string longFlag;
    std::string description;
    std::string defaultValue;
    bool hasArgument;
    std::string argumentName;
    bool required;
};

// Command line argument parsing result
struct ParsedArguments {
    CommandType command;
    std::string commandArg;
    std::unordered_map<std::string, std::string> options;
    std::vector<std::string> positionalArgs;
    bool helpRequested;
    bool versionRequested;
    std::string errorMessage;
    bool isValid;
};

class CommandLineParser {
public:
    CommandLineParser();
    ~CommandLineParser();
    
    // Parse command line arguments
    ParsedArguments parse(int argc, char** argv);
    
    // Get configuration from parsed arguments
    std::unordered_map<std::string, std::string> getOptions() const { return currentArgs.options; }
    std::vector<std::string> getPositionalArgs() const { return currentArgs.positionalArgs; }
    CommandType getCommand() const { return currentArgs.command; }
    std::string getCommandArg() const { return currentArgs.commandArg; }
    bool isValid() const { return currentArgs.isValid; }
    std::string getErrorMessage() const { return currentArgs.errorMessage; }
    
    // Help and version printing
    void printHelp() const;
    void printVersion() const;
    
    // Command validation
    bool isCommandValid(const std::string& cmd) const;
    bool isOptionValid(const std::string& option) const;
    bool isFlagValid(const std::string& flag) const;
    
private:
    ParsedArguments currentArgs;
    std::unordered_map<std::string, CommandLineOption> options;
    
    // Command and option definitions
    std::vector<std::string> validCommands;
    std::unordered_map<std::string, CommandType> commandMap;
    
    // Internal parsing methods
    void initializeOptions();
    void initializeCommands();
    
    ParsedArguments parseArguments(int argc, char** argv);
    CommandType parseCommand(const std::string& cmd) const;
    
    std::string normalizeString(const std::string& str) const;
    bool stringStartsWith(const std::string& str, const std::string& prefix) const;
    std::string extractOptionName(const std::string& arg) const;
    std::string extractOptionValue(const std::string& arg) const;
    
    std::string getTypeDescription(CommandType type) const;
    void printOptionDetails(const CommandLineOption& option) const;
    void printCommandUsage(const std::string& cmd) const;
    void printAvailableCommands() const;
    void printAvailableOptions() const;
};

} // namespace nlm