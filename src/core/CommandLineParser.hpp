// Command-line parser for NLM with advanced features
// Supports subcommands, config validation, logging levels, profiling, etc.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace nlm {

// Subcommand handler type
using SubcommandHandler = std::function<bool(const std::map<std::string, std::string>& args, std::ostream& output)>;

// Command-line option definition
struct CLIOption {
    std::string name;
    std::string description;
    char shortForm = 0;
    bool required = false;
    std::string defaultValue;
    std::string type; // "string", "int", "float", "bool"
    bool isFlag = false;
};

class CommandLineParser {
public:
    CommandLineParser();
    ~CommandLineParser();
    
    // Add a subcommand
    void addSubcommand(const std::string& name, const std::string& description, SubcommandHandler handler);
    
    // Add a global option
    void addOption(const std::string& name, const std::string& description, char shortForm = 0, 
                   const std::string& defaultValue = "", const std::string& type = "string", 
                   bool isFlag = false, bool required = false);
    
    // Parse command line arguments
    bool parse(int argc, char** argv, std::ostream& output = std::cout);
    
    // Get parsed values
    std::string getValue(const std::string& key, const std::string& defaultValue = "") const;
    bool getFlag(const std::string& key) const;
    bool hasSubcommand(const std::string& name) const;
    const std::map<std::string, std::string>& getArgs(const std::string& subcommand) const;
    
    // Show help
    void printHelp(std::ostream& output = std::cout) const;
    
    // Get subcommand arguments
    const std::string& getCurrentSubcommand() const { return currentSubcommand_; }
    
    // Configuration file handling
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
    // Validation
    bool validate() const;
    std::string getValidationErrors() const;
    
    // Backward compatibility
    bool hasBackwardCompatArgs(int argc, char** argv) const;
    std::string parseBackwardCompatArgs(int argc, char** argv, std::ostream& output = std::cout);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string currentSubcommand_;
    std::map<std::string, std::map<std::string, std::string>> subcommandArgs_;
    std::map<std::string, CLIOption> options_;
    std::map<std::string, std::pair<std::string, SubcommandHandler>> subcommands_;
    std::map<std::string, std::string> parsedValues_;
    std::map<std::string, bool> parsedFlags_;
    
    // Help text generation
    void generateHelpText(std::ostream& output) const;
    void generateSubcommandHelp(const std::string& subcommand, std::ostream& output) const;
    
    // Argument parsing helpers
    bool parseArgs(int argc, char** argv, std::ostream& output);
    bool parseSubcommand(std::vector<std::string>& tokens, std::ostream& output);
    bool parseOption(std::vector<std::string>& tokens, std::ostream& output);
    
    // Backward compatibility parsing
    bool parseBackwardCompatOption(const std::string& arg, std::ostream& output);
};

} // namespace nlm
