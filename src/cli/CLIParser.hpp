// CLI Parser for NLM - Modern C++ Argument Parsing
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <variant>
#include <any>

namespace nlm {
namespace cli {

// Command line variant types
using CLIValue = std::variant<
    std::string,
    int,
    int64_t,
    double,
    bool,
    std::vector<std::string>
>;

// Command result for execution
struct CommandResult {
    int exitCode;
    std::string output;
    bool success;
    
    CommandResult(int code = 0, const std::string& out = "", bool ok = true)
        : exitCode(code), output(out), success(ok) {}
};

// Exception for CLI errors
class CLIError : public std::runtime_error {
public:
    CLIError(const std::string& message) : std::runtime_error(message) {}
};

// Argument definition
struct Argument {
    std::string name;
    std::string description;
    std::string type;
    std::string defaultValue;
    bool required;
    std::vector<std::string> options;
    
    Argument(const std::string& n, const std::string& desc, const std::string& type = "string",
             const std::string& defaultVal = "", bool req = false)
        : name(n), description(desc), type(type), defaultValue(defaultVal), required(req) {}
    
    Argument(const std::string& n, const std::string& desc, const std::string& type,
             const std::vector<std::string>& opts)
        : name(n), description(desc), type(type), defaultValue(""), required(false), options(opts) {}
};

// Subcommand base class
class Subcommand {
protected:
    std::string name;
    std::string description;
    std::vector<Argument> arguments;
    std::map<std::string, CLIValue> parsedArgs;
    
public:
    Subcommand(const std::string& n, const std::string& desc) : name(n), description(desc) {}
    
    virtual ~Subcommand() = default;
    
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    
    virtual void addArguments() = 0;
    
    void addArgument(const Argument& arg) {
        arguments.push_back(arg);
    }
    
    virtual CommandResult execute() = 0;
    
    virtual std::string getHelp() const {
        std::string help = "  " + name + " - " + description + "\n";
        if (arguments.empty()) return help;
        
        help += "    Arguments:\n";
        for (const auto& arg : arguments) {
            std::string required = arg.required ? "(required)" : "(optional)";
            help += "      --" + arg.name + " <" + arg.type + "> " + arg.description + " " + required + "\n";
        }
        return help;
    }
    
    virtual bool parseArguments(const std::vector<std::string>& args) {
        return true; // Base implementation does nothing
    }
};

// Main parser class
class CLIParser {
private:
    std::string programName;
    std::string version;
    std::string description;
    std::vector<Subcommand*> subcommands;
    std::map<std::string, std::string> globalOptions;
    bool helpRequested;
    bool versionRequested;
    
public:
    CLIParser(const std::string& name, const std::string& ver, const std::string& desc)
        : programName(name), version(ver), description(desc), helpRequested(false), versionRequested(false) {}
    
    static std::unique_ptr<CLIParser> create(int argc, char** argv) {
        return std::unique_ptr<CLIParser>(new CLIParser("nlm", "1.0.0", "NLM - Neural Learning Machine"));
    }
    
    void addSubcommand(Subcommand* cmd) {
        subcommands.push_back(cmd);
        cmd->addArguments();
    }
    
    void setGlobalOption(const std::string& key, const std::string& value) {
        globalOptions[key] = value;
    }
    
    std::unique_ptr<Subcommand> parse(int argc, char** argv) {
        std::vector<std::string> args;
        for (int i = 0; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        if (args.size() <= 1) {
            // No subcommand provided, show help
            showHelp();
            return nullptr;
        }
        
        std::string subcommandName = args[1];
        
        // Check for global options
        helpRequested = false;
        versionRequested = false;
        
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "--help" || args[i] == "-h") {
                helpRequested = true;
            } else if (args[i] == "--version") {
                versionRequested = true;
            }
        }
        
        if (helpRequested || versionRequested) {
            if (helpRequested) showHelp();
            if (versionRequested) showVersion();
            return nullptr;
        }
        
        // Find the requested subcommand
        for (auto* cmd : subcommands) {
            if (cmd->getName() == subcommandName) {
                // For now, simple parsing - in real implementation would parse all arguments
                if (cmd->parseArguments(args)) {
                    return std::unique_ptr<Subcommand>(cmd);
                }
            }
        }
        
        throw CLIError("Unknown subcommand: " + subcommandName);
    }
    
    void showHelp() const {
        std::cout << "Usage: " << programName << " <subcommand> [options]" << std::endl << std::endl;
        std::cout << description << std::endl << std::endl;
        
        std::cout << "Global Options:" << std::endl;
        std::cout << "  --help, -h          Show this help message" << std::endl;
        std::cout << "  --version           Show version information" << std::endl;
        std::cout << "  --config <file>     Configuration file path" << std::endl;
        std::cout << "  --log-level <level> Logging level (DEBUG, INFO, WARN, ERROR)" << std::endl;
        std::cout << "  --verbose           Enable verbose output" << std::endl;
        std::cout << "  --quiet             Suppress output" << std::endl;
        std::cout << "  --batch             Non-interactive mode" << std::endl;
        std::cout << "  --profile           Enable performance profiling" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Available Subcommands:" << std::endl;
        for (const auto* cmd : subcommands) {
            std::cout << "  " << cmd->getHelp();
        }
    }
    
    void showVersion() const {
        std::cout << programName << " " << version << std::endl;
    }
    
    // Get global option value
    std::string getGlobalOption(const std::string& key) const {
        auto it = globalOptions.find(key);
        return it != globalOptions.end() ? it->second : "";
    }
};

// Command processor interface
class CommandProcessor {
public:
    virtual ~CommandProcessor() = default;
    virtual CommandResult execute(Subcommand& command) = 0;
};

} // namespace cli
} // namespace nlm
