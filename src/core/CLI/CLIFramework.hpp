// NLM CLI Framework - Enhanced Command Line Interface
// Provides comprehensive argument parsing, help system, and command management

#ifndef NLM_CLI_FRAMEWORK_H
#define NLM_CLI_FRAMEWORK_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <cctype>

namespace nlm {

class CLIArgument {
public:
    std::string name;
    char shortOpt;
    std::string longOpt;
    std::string description;
    std::string defaultValue;
    bool required;
    bool hasValue;
    std::string value;
    
    CLIArgument(const std::string& name, char shortOpt, const std::string& longOpt, 
                const std::string& description, const std::string& defaultValue = "",
                bool required = false, bool hasValue = false)
        : name(name), shortOpt(shortOpt), longOpt(longOpt), description(description),
          defaultValue(defaultValue), required(required), hasValue(hasValue) {}
};

class CLICommand {
public:
    std::string name;
    std::string description;
    std::string usage;
    std::function<void(const std::map<std::string, std::string>&)> handler;
    
    CLICommand(const std::string& name, const std::string& description, 
               const std::string& usage, std::function<void(const std::map<std::string, std::string>&)> handler)
        : name(name), description(description), usage(usage), handler(handler) {}
};

class CLIParser {
public:
    std::vector<CLIArgument> arguments;
    std::vector<CLICommand> commands;
    std::map<std::string, std::string> parsedArgs;
    std::string helpText;
    bool verbose;
    bool quiet;
    
    CLIParser() : verbose(false), quiet(false) {
        addArgument("help", 'h', "help", "Show help message", "", false, false);
        addArgument("version", 'v', "version", "Show version information", "", false, false);
        addArgument("verbose", 'V', "verbose", "Enable verbose output", "false", false, false);
        addArgument("quiet", 'q', "quiet", "Suppress output", "false", false, false);
        addArgument("config", 'c', "config", "Configuration file path", "configs/default.cfg", false, true);
        addArgument("log-level", 'L', "log-level", "Log level (DEBUG, INFO, WARN, ERROR)", "INFO", false, true);
        addArgument("output", 'o', "output", "Output file path", "", false, true);
        addArgument("input", 'i', "input", "Input file path", "", false, true);
        
        helpText = "NLM (熙然) - Neural Learning Machine\n\n";
        helpText += "Usage: nlm [OPTIONS] [COMMAND] [ARGS...]\n\n";
        helpText += "Commands:\n";
        helpText += "  brain-config    Configure brain parameters\n";
        helpText += "  state-manager   Save/load/checkpoint brain state\n";
        helpText += "  performance     Performance monitoring and profiling\n";
        helpText += "  diagnostics     Diagnostic and debugging tools\n";
        helpText += "  export-import   Export/import brain configurations\n";
        helpText += "  batch           Batch processing for multiple simulations\n";
        helpText += "  monitor         Real-time monitoring interfaces\n";
        helpText += "  experiment      Automated experiment running\n";
        helpText += "  optimize        Parameter optimization tools\n";
        helpText += "  visualize       Visualization interfaces\n";
        helpText += "  analyze         Statistical analysis tools\n\n";
        helpText += "Options:\n";
    }
    
    void addArgument(const std::string& name, char shortOpt, const std::string& longOpt, 
                     const std::string& description, const std::string& defaultValue = "",
                     bool required = false, bool hasValue = false) {
        arguments.push_back(CLIArgument(name, shortOpt, longOpt, description, defaultValue, required, hasValue));
    }
    
    void addCommand(const std::string& name, const std::string& description, 
                    const std::string& usage, std::function<void(const std::map<std::string, std::string>&)> handler) {
        commands.push_back(CLICommand(name, description, usage, handler));
    }
    
    bool parse(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                showHelp();
                return false;
            }
            
            if (arg == "--version") {
                showVersion();
                return false;
            }
            
            if (arg == "--verbose") {
                verbose = true;
                continue;
            }
            
            if (arg == "--quiet") {
                quiet = true;
                continue;
            }
            
            if (arg.find("--") == 0) {
                std::string opt = arg.substr(2);
                size_t pos = opt.find('=');
                std::string key = opt.substr(0, pos);
                std::string value = (pos != std::string::npos) ? opt.substr(pos + 1) : "true";
                
                setArgument(key, value);
            } else if (arg.find('-') == 0 && arg.length() > 1) {
                std::string opt = arg.substr(1);
                if (opt.length() == 1) {
                    // Single character option (e.g., -c)
                    char optChar = opt[0];
                    auto it = std::find_if(arguments.begin(), arguments.end(), 
                                          [optChar](const CLIArgument& arg) { return arg.shortOpt == optChar; });
                    if (it != arguments.end()) {
                        if (it->hasValue && i + 1 < argc && argv[i + 1][0] != '-') {
                            setArgument(it->name, argv[++i]);
                        } else {
                            setArgument(it->name, "true");
                        }
                    }
                } else {
                    // Combined short options (e.g., -cv)
                    for (char c : opt) {
                        auto it = std::find_if(arguments.begin(), arguments.end(), 
                                              [c](const CLIArgument& arg) { return arg.shortOpt == c; });
                        if (it != arguments.end()) {
                            if (it->hasValue && i + 1 < argc && argv[i + 1][0] != '-') {
                                setArgument(it->name, argv[++i]);
                            } else {
                                setArgument(it->name, "true");
                            }
                        }
                    }
                }
            } else {
                // Command
                auto it = std::find_if(commands.begin(), commands.end(), 
                                      [arg](const CLICommand& cmd) { return cmd.name == arg; });
                if (it != commands.end()) {
                    // Parse command arguments (simplified)
                    std::map<std::string, std::string> cmdArgs;
                    i++; // Skip to next argument
                    while (i < argc && argv[i][0] != '-') {
                        if (i + 1 < argc && argv[i + 1][0] != '-') {
                            cmdArgs[argv[i]] = argv[i + 1];
                            i += 2;
                        } else {
                            cmdArgs[argv[i]] = "true";
                            i++;
                        }
                    }
                    it->handler(cmdArgs);
                    return false; // Command executed, stop parsing
                }
            }
        }
        
        // Set default values for required arguments not provided
        for (auto& arg : arguments) {
            if (arg.required && parsedArgs.find(arg.name) == parsedArgs.end()) {
                std::cerr << "Error: Required argument --" << arg.longOpt << " is missing!" << std::endl;
                return false;
            }
            
            if (parsedArgs.find(arg.name) == parsedArgs.end()) {
                parsedArgs[arg.name] = arg.defaultValue;
            }
        }
        
        return true;
    }
    
    bool setArgument(const std::string& name, const std::string& value) {
        for (auto& arg : arguments) {
            if (arg.name == name) {
                parsedArgs[name] = value;
                return true;
            }
        }
        return false;
    }
    
    std::string getArgument(const std::string& name, const std::string& defaultValue = "") {
        auto it = parsedArgs.find(name);
        return it != parsedArgs.end() ? it->second : defaultValue;
    }
    
    bool hasArgument(const std::string& name) {
        return parsedArgs.find(name) != parsedArgs.end();
    }
    
    void showHelp() {
        std::cout << helpText << std::endl;
        
        std::cout << "\nArguments:\n";
        for (const auto& arg : arguments) {
            if (arg.name == "help" || arg.name == "version" || arg.name == "verbose" || arg.name == "quiet") {
                continue; // Skip already displayed options
            }
            std::cout << "  -" << arg.shortOpt << ", --" << arg.longOpt;
            if (!arg.defaultValue.empty()) {
                std::cout << "=" << arg.defaultValue;
            }
            std::cout << "\n    " << arg.description << "\n";
        }
        
        std::cout << "\n";
    }
    
    void showVersion() {
        std::cout << "NLM (熙然) Neural Learning Machine v0.1.0\n";
        std::cout << "Phase 2: Real Neural Computation\n";
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << "\n";
    }
    
    void log(const std::string& message) {
        if (!quiet) {
            if (verbose) {
                std::cout << "[VERBOSE] " << message << std::endl;
            } else {
                std::cout << message << std::endl;
            }
        }
    }
};

} // namespace nlm

#endif // NLM_CLI_FRAMEWORK_H
