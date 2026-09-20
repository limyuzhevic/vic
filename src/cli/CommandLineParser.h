/*
 * CommandLineParser.h - Advanced argument parsing with validation
 * Provides comprehensive option parsing with support for short and long options,
 * type conversion, validation, and default value management.
 */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>

namespace nlm {

// Custom exception for CLI parsing errors
class CLIParseError : public std::runtime_error {
public:
    CLIParseError(const std::string& message) : std::runtime_error(message) {}
};

// Value types supported by CLI parser
using CLIValue = std::variant<bool, int, double, std::string>;

// Option definition structure
struct CLIOption {
    std::string longName;
    char shortName;
    CLIValue defaultValue;
    std::string description;
    bool required;
    std::function<bool(const CLIValue&)> validator;
    std::string type;
    
    CLIOption(const std::string& longName = "",
              char shortName = '\0',
              const CLIValue& defaultValue = CLIValue(),
              const std::string& description = "",
              bool required = false)
        : longName(longName), shortName(shortName), defaultValue(defaultValue),
          description(description), required(required), validator(nullptr), type("auto") {}
};

// Command definition structure
struct CLICommand {
    std::string name;
    std::string description;
    std::vector<std::string> argumentNames;
    std::vector<std::string> argumentDescriptions;
    std::function<void(const std::vector<CLIValue>&, std::ostream&)> handler;
    bool requiresBrain;
    
    CLICommand(const std::string& name = "",
               const std::string& description = "",
               bool requiresBrain = true)
        : name(name), description(description), requiresBrain(requiresBrain) {}
};

// Advanced argument parser with comprehensive validation
class CommandLineParser {
public:
    CommandLineParser();
    ~CommandLineParser();
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Add option to parser
    void addOption(const CLIOption& option);
    
    // Add command to parser
    void addCommand(const CLICommand& command);
    
    // Get option value
    template<typename T>
    T getOption(const std::string& name) const;
    
    // Get option value with default
    template<typename T>
    T getOptionOr(const std::string& name, const T& defaultValue) const;
    
    // Check if option was provided
    bool hasOption(const std::string& name) const;
    
    // Get command arguments
    std::vector<CLIValue> getCommandArgs(const std::string& commandName) const;
    
    // Get command name (if any)
    std::string getCommandName() const;
    
    // Get help string
    std::string getHelp() const;
    
    // Validate all parsed arguments
    bool validate() const;
    
    // Get error messages
    std::vector<std::string> getErrors() const;
    
    // Reset parser state
    void reset();
    
    // Set custom error stream
    void setErrorStream(std::ostream* errorStream);
    
    // Set custom output stream
    void setOutputStream(std::ostream* outputStream);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    bool parseOptions(const std::vector<std::string>& args);
    bool parseCommand(const std::vector<std::string>& args);
    CLIValue convertStringToCLIValue(const std::string& value, const std::string& type) const;
    bool validateOption(const CLIOption& option, const CLIValue& value) const;
    std::string formatValue(const CLIValue& value) const;
};

} // namespace nlm