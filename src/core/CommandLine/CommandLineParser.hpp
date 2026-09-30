// Enhanced command-line argument parser for NLM
// Supports advanced argument formats with validation and error handling

#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <functional>
#include <memory>
#include <variant>
#include <algorithm>
#include <iostream>

// Forward declaration to avoid circular dependency
namespace nlm {
class Config;

// Command-line parsing error
class CommandLineError : public std::runtime_error {
public:
    CommandLineError(const std::string& message) 
        : std::runtime_error("Command-line error: " + message) {}
};

// Command-line argument representation
struct CommandLineArg {
    std::string name;
    std::string value;
    bool present;
    std::string description;
    bool required;
    bool hasDefault;
    std::string defaultValue;
    std::string type;
    std::string category;
    
    CommandLineArg(const std::string& n, const std::string& d = "", 
                  bool req = false, bool def = false, const std::string& defVal = "")
        : name(n), value(), present(false), description(d), required(req), 
          hasDefault(def), defaultValue(defVal), type("string"), category("general") {}
};

// Advanced command-line parser with validation
class CommandLineParser {
public:
    CommandLineParser() = default;
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get argument value by name
    std::optional<std::string> get(const std::string& name) const;
    
    // Get argument value with default
    std::string getOr(const std::string& name, const std::string& defaultValue) const;
    
    // Check if argument is present
    bool has(const std::string& name) const;
    
    // Get all parsed arguments
    const std::vector<CommandLineArg>& getAllArgs() const { return args; }
    
    // Get parsing errors
    const std::vector<std::string>& getErrors() const { return errors; }
    
    // Add argument definition
    void addArgument(const CommandLineArg& arg);
    
    // Set validation function
    void setValidation(const std::function<bool(const std::string&, const std::string&)>& validator) {
        validationFunc = validator;
    }
    
    // Set help function
    void setHelpFunction(const std::function<void()>& helpFunc) {
        helpFunction = helpFunc;
    }
    
    // Check if validation was successful
    bool isValid() const { return errors.empty(); }
    
    // Validate all arguments
    bool validate();
    
    // Display help information
    void displayHelp() const;
    
    // Load arguments into Config
    void applyToConfig(Config& config) const;
    
private:
    std::vector<CommandLineArg> args;
    std::vector<std::string> errors;
    std::function<bool(const std::string&, const std::string&)> validationFunc;
    std::function<void()> helpFunction;
    
    // Parse --key=value format
    bool parseLongFormat(const std::string& arg);
    
    // Parse -key value format
    bool parseShortFormat(const std::string& arg, const std::string& value);
    
    // Parse boolean flags
    bool parseFlag(const std::string& arg);
    
    // Convert string value to specific type
    template<typename T>
    T convertValue(const std::string& value) const;
    
    // Find argument by name
    CommandLineArg* findArg(const std::string& name);
    
    // Find argument by name (const version)
    const CommandLineArg* findArg(const std::string& name) const;
};

// Global instance for convenience
CommandLineParser& getCommandLineParser();

} // namespace nlm