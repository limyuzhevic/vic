#pragma once

/**
 * CLI Framework for NLM (熙然)
 * Comprehensive command-line interface for simulation and experiment management
 * 
 * This framework provides:
 * - Advanced argument parsing with subcommands and options
 * - Real-time progress reporting and logging
 * - Configuration file management
 * - Experiment result saving and analysis
 * - Performance metrics and statistics
 * - Debugging and profiling tools
 * - Scriptable batch processing
 * - Support for multiple simulation modes
 * - Integration with all new Phase 2 features
 * - Cross-platform compatibility
 * - Clean, consistent interface with helpful help text
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>
#include <sstream>
#include <iomanip>
#include <fstream>

namespace nlm {

/**
 * CLI Option type for argument parsing
 */
enum class OptionType {
    Flag,      // --flag
    Value,     // --key=value or --key value
    Positional // positional argument
};

/**
 * CLI Command structure for subcommand handling
 */
struct CLICommand {
    std::string name;
    std::string description;
    std::string help;
    std::function<int(const std::map<std::string, std::string>&, std::ostream&)> handler;
    std::map<std::string, OptionType> options;
    std::vector<std::string> requiredPositionalArgs;
    int minArgs;
    int maxArgs;
    bool requiresBrain;
    bool requiresConfig;
    bool experimental;
};

/**
 * Progress reporter for real-time status updates
 */
class CLIProgressReporter {
public:
    CLIProgressReporter(std::ostream& output = std::cout, bool quiet = false)
        : output(output), quiet(quiet), started(false), completed(false) {}
    
    void start(const std::string& title, int totalSteps, bool showProgress = true) {
        this->title = title;
        this->totalSteps = totalSteps;
        this->showProgress = showProgress && !quiet;
        currentStep = 0;
        startTime = std::chrono::high_resolution_clock::now();
        lastLogTime = startTime;
        started = true;
        completed = false;
        
        if (showProgress && !quiet) {
            output << "\n=== " << title << " ===" << std::endl;
            output << "Total steps: " << totalSteps << std::endl;
            output << std::string(50, '=') << std::endl;
        }
    }
    
    void update(int step, const std::string& message = "") {
        if (!started || !showProgress || quiet) return;
        
        currentStep = step;
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count() / 1000.0;
        
        // Log at most every second
        auto timeSinceLastLog = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLogTime).count();
        if (timeSinceLastLog >= 1000 || step == totalSteps) {
            double progress = (totalSteps > 0) ? (step * 100.0 / totalSteps) : 0.0;
            output << "\rStep " << step << "/" << totalSteps << " (" << std::fixed << std::setprecision(1) 
                   << progress << "%) - " << message;
            if (elapsed > 0) {
                double speed = step / elapsed;
                output << " - Speed: " << std::fixed << std::setprecision(2) << speed << " steps/s";
            }
            output << std::flush;
            lastLogTime = now;
        }
    }
    
    void complete(const std::string& message = "Completed successfully") {
        if (!started || completed) return;
        
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count() / 1000.0;
        
        if (showProgress && !quiet) {
            output << "\n" << std::string(50, '=') << std::endl;
            output << message << std::endl;
            output << "Total time: " << std::fixed << std::setprecision(3) << elapsed << "s";
            if (currentStep > 0) {
                double speed = currentStep / elapsed;
                output << " - Average speed: " << std::fixed << std::setprecision(2) << speed << " steps/s";
            }
            output << std::endl;
        }
        
        completed = true;
    }
    
    void log(const std::string& message) {
        if (quiet) return;
        output << "[INFO] " << message << std::endl;
    }
    
    void error(const std::string& message) {
        output << "[ERROR] " << message << std::endl;
    }
    
    void warning(const std::string& message) {
        output << "[WARN] " << message << std::endl;
    }
    
    bool isQuiet() const { return quiet; }
    
private:
    std::ostream& output;
    bool quiet;
    bool started;
    bool completed;
    std::string title;
    int totalSteps;
    int currentStep;
    bool showProgress;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point lastLogTime;
};

/**
 * Configuration manager for CLI command-line and file-based configuration
 */
class CLIConfigManager {
public:
    CLIConfigManager() = default;
    
    // Load configuration from file
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;
        
        std::string line;
        while (std::getline(file, line)) {
            // Remove comments
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            
            // Trim whitespace
            size_t start = line.find_first_not_of(" \t\r\n");
            size_t end = line.find_last_not_of(" \t\r\n");
            if (start == std::string::npos || end == std::string::npos) continue;
            
            line = line.substr(start, end - start + 1);
            
            // Parse key=value
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos + 1);
                
                // Trim key and value
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                
                fileConfig[key] = value;
            }
        }
        
        return true;
    }
    
    // Parse command-line arguments
    void parseArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg.substr(0, 2) == "--") {
                // Option argument
                size_t equalsPos = arg.find('=');
                if (equalsPos != std::string::npos) {
                    // --key=value
                    std::string key = arg.substr(2, equalsPos - 2);
                    std::string value = arg.substr(equalsPos + 1);
                    cmdLineConfig[key] = value;
                } else {
                    // --flag (store as true)
                    std::string key = arg.substr(2);
                    cmdLineConfig[key] = "true";
                }
            } else if (arg.substr(0, 1) == "-") {
                // Short option (not implemented yet)
                cmdLineConfig[arg.substr(1)] = "true";
            } else {
                // Positional argument
                positionalArgs.push_back(arg);
            }
        }
    }
    
    // Get configuration value
    std::string getConfig(const std::string& key, const std::string& defaultValue = "") const {
        // Check command line overrides first
        auto it = cmdLineConfig.find(key);
        if (it != cmdLineConfig.end()) {
            return it->second;
        }
        
        // Check file config
        it = fileConfig.find(key);
        if (it != fileConfig.end()) {
            return it->second;
        }
        
        return defaultValue;
    }
    
    // Get boolean configuration value
    bool getBool(const std::string& key, bool defaultValue = false) const {
        std::string value = getConfig(key);
        if (value.empty()) return defaultValue;
        
        // Convert common true values
        std::string lowerValue = value;
        for (char& c : lowerValue) c = std::tolower(c);
        
        return (lowerValue == "true" || lowerValue == "yes" || lowerValue == "1" ||
                lowerValue == "on" || lowerValue == "enabled");
    }
    
    // Get integer configuration value
    int getInt(const std::string& key, int defaultValue = 0) const {
        std::string value = getConfig(key);
        if (value.empty()) return defaultValue;
        
        try {
            return std::stoi(value);
        } catch (...) {
            return defaultValue;
        }
    }
    
    // Get float configuration value
    float getFloat(const std::string& key, float defaultValue = 0.0f) const {
        std::string value = getConfig(key);
        if (value.empty()) return defaultValue;
        
        try {
            return std::stof(value);
        } catch (...) {
            return defaultValue;
        }
    }
    
    // Get positional arguments
    const std::vector<std::string>& getPositionalArgs() const { return positionalArgs; }
    
    // Check if option is set
    bool hasOption(const std::string& key) const {
        return cmdLineConfig.find(key) != cmdLineConfig.end();
    }
    
    // Get help text for a command
    std::string getHelp(const std::string& command) const {
        auto it = helpTexts.find(command);
        if (it != helpTexts.end()) return it->second;
        return "";
    }
    
    // Add help text
    void addHelp(const std::string& command, const std::string& help) {
        helpTexts[command] = help;
    }
    
    // Save configuration to file
    bool saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) return false;
        
        file << "# NLM Configuration generated by CLI" << std::endl;
        for (const auto& pair : fileConfig) {
            file << pair.first << " = " << pair.second << std::endl;
        }
        
        return true;
    }
    
    // Export configuration for experiment results
    std::map<std::string, std::string> exportConfig() const {
        std::map<std::string, std::string> config;
        
        // Merge all config sources
        for (const auto& pair : fileConfig) {
            config[pair.first] = pair.second;
        }
        for (const auto& pair : cmdLineConfig) {
            config[pair.first] = pair.second; // Command line takes precedence
        }
        
        return config;
    }
    
    // Clear all configuration
    void clear() {
        fileConfig.clear();
        cmdLineConfig.clear();
        positionalArgs.clear();
    }
    
private:
    std::map<std::string, std::string> fileConfig;      // From config file
    std::map<std::string, std::string> cmdLineConfig;   // From command line
    std::vector<std::string> positionalArgs;           // Positional arguments
    std::map<std::string, std::string> helpTexts;       // Command help texts
};

/**
 * NLM Command Line Interface (CLI) framework
 * Main entry point for all NLM CLI tools
 */
class NLMCLI {
public:
    NLMCLI(int argc, char** argv, const std::string& programName = "nlm")
        : argc(argc), argv(argv), programName(programName), configManager() {
        
        // Parse command line arguments
        configManager.parseArgs(argc, argv);
        
        // Parse subcommands
        parseSubcommands();
        
        // Parse global options
        parseGlobalOptions();
        
        // Get subcommand
        std::string subcommand = configManager.getConfig("subcommand", "help");
        currentCommand = commands.find(subcommand);
        
        if (currentCommand == commands.end()) {
            std::cerr << "Unknown subcommand: " << subcommand << std::endl;
            printUsage();
            exitCode = 1;
        }
    }
    
    // Register a command
    void registerCommand(const CLICommand& cmd) {
        commands[cmd.name] = cmd;
        
        // Add to config help
        std::stringstream helpStream;
        helpStream << "  " << cmd.name << ": " << cmd.description;
        if (!cmd.help.empty()) {
            helpStream << "\n    " << cmd.help;
        }
        configManager.addHelp(cmd.name, helpStream.str());
    }
    
    // Run the current command
    int run() {
        if (exitCode != 0) {
            return exitCode;
        }
        
        std::cout << "NLM CLI " << programName << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        if (currentCommand == commands.end()) {
            std::cerr << "Error: No command specified" << std::endl;
            printUsage();
            return 1;
        }
        
        try {
            // Check if help is requested
            if (configManager.hasOption("--help") || configManager.hasOption("-h")) {
                printCommandHelp(currentCommand->first);
                return 0;
            }
            
            // Prepare arguments for command
            std::map<std::string, std::string> cmdArgs;
            for (const auto& pair : configManager.getPositionalArgs()) {
                cmdArgs["arg_" + std::to_string(cmdArgs.size())] = pair;
            }
            
            // Add options as well
            for (const auto& pair : configManager.getPositionalArgs()) {
                // Parse options from positional args (simplified)
                if (pair.substr(0, 2) == "--") {
                    size_t equalsPos = pair.find('=');
                    if (equalsPos != std::string::npos) {
                        std::string key = pair.substr(2, equalsPos - 2);
                        std::string value = pair.substr(equalsPos + 1);
                        cmdArgs[key] = value;
                    }
                }
            }
            
            // Execute command
            std::stringstream output;
            int result = currentCommand->second.handler(cmdArgs, output);
            
            if (output.str().empty() && result == 0) {
                std::cout << "Command completed successfully." << std::endl;
            } else {
                std::cout << output.str();
            }
            
            return result;
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }
    
    // Print usage information
    void printUsage() {
        std::cout << programName << " - Neural Learning Machine CLI" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "Usage: " << programName << " [OPTIONS] <COMMAND> [ARGS]..." << std::endl;
        std::cout << std::endl;
        
        // Global options
        std::cout << "Global Options:" << std::endl;
        std::cout << "  -h, --help\t\tShow this help message" << std::endl;
        std::cout << "  --version\t\tShow version information" << std::endl;
        std::cout << "  --quiet, -q\t\tSuppress non-essential output" << std::endl;
        std::cout << "  --verbose, -v\t\tIncrease verbosity" << std::endl;
        std::cout << "  --config <file>\t\tLoad configuration from file" << std::endl;
        std::cout << "  --dry-run\t\tDry run (don't actually run experiments)" << std::endl;
        std::cout << std::endl;
        
        // Available commands
        std::cout << "Available Commands:" << std::endl;
        for (const auto& pair : commands) {
            std::cout << "  " << pair.first << "\t\t" << pair.second.description << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "Use '" << programName << " <COMMAND> --help' for more information about a command." << std::endl;
    }
    
    // Print help for a specific command
    void printCommandHelp(const std::string& commandName) {
        auto it = commands.find(commandName);
        if (it == commands.end()) {
            std::cerr << "Unknown command: " << commandName << std::endl;
            return;
        }
        
        const CLICommand& cmd = it->second;
        
        std::cout << programName << " " << commandName << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << cmd.description << std::endl;
        std::cout << std::endl;
        
        if (!cmd.help.empty()) {
            std::cout << "Description:" << std::endl;
            std::cout << cmd.help << std::endl;
            std::cout << std::endl;
        }
        
        // Options
        if (!cmd.options.empty()) {
            std::cout << "Options:" << std::endl;
            for (const auto& pair : cmd.options) {
                std::cout << "  --" << pair.first << std::endl;
            }
            std::cout << std::endl;
        }
        
        // Arguments
        if (!cmd.requiredPositionalArgs.empty()) {
            std::cout << "Arguments:" << std::endl;
            for (const auto& arg : cmd.requiredPositionalArgs) {
                std::cout << "  " << arg << std::endl;
            }
            std::cout << std::endl;
        }
    }
    
    // Get configuration manager
    CLIConfigManager& getConfigManager() { return configManager; }
    
    // Check if experimental features are enabled
    bool isExperimentalEnabled() const { return experimentalEnabled; }
    
    // Enable experimental features
    void enableExperimental(bool enable) { experimentalEnabled = enable; }
    
protected:
    // Parse subcommands from command line
    void parseSubcommands() {
        // Standard subcommands
        CLICommand helpCmd;
        helpCmd.name = "help";
        helpCmd.description = "Show help information";
        helpCmd.help = "Display help information for all commands or a specific command.";
        helpCmd.handler = [this](const std::map<std::string, std::string>&, std::ostream&) {
            printUsage();
            return 0;
        };
        registerCommand(helpCmd);
        
        CLICommand versionCmd;
        versionCmd.name = "version";
        versionCmd.description = "Show version information";
        versionCmd.help = "Display NLM version and build information.";
        versionCmd.handler = [this](const std::map<std::string, std::string>&, std::ostream& out) {
            out << "NLM (熙然) - Neural Learning Machine" << std::endl;
            out << "Version: 0.1.0" << std::endl;
            out << "Phase: Phase 2" << std::endl;
            out << "Build: Release" << std::endl;
            return 0;
        };
        registerCommand(versionCmd);
        
        CLICommand configCmd;
        configCmd.name = "config";
        configCmd.description = "Advanced configuration management";
        configCmd.help = "Manage NLM configuration files. Includes save, load, list, and edit operations.";
        configCmd.handler = [this](const std::map<std::string, std::string>& args, std::ostream& out) {
            return handleConfigCommand(args, out);
        };
        configCmd.options["help"] = OptionType::Flag;
        registerCommand(configCmd);
        
        CLICommand checkpointCmd;
        checkpointCmd.name = "checkpoint";
        checkpointCmd.description = "Checkpoint save/load management";
        checkpointCmd.help = "Manage brain checkpoints. Save brain state for later restoration and analysis.";
        checkpointCmd.handler = [this](const std::map<std::string, std::string>& args, std::ostream& out) {
            return handleCheckpointCommand(args, out);
        };
        checkpointCmd.options["help"] = OptionType::Flag;
        registerCommand(checkpointCmd);
        
        CLICommand experimentCmd;
        experimentCmd.name = "experiment";
        experimentCmd.description = "Run experiments";
        experimentCmd.help = "Run NLM experiments. Supports batch processing and result analysis.";
        experimentCmd.handler = [this](const std::map<std::string, std::string>& args, std::ostream& out) {
            return handleExperimentCommand(args, out);
        };
        experimentCmd.options["help"] = OptionType::Flag;
        registerCommand(experimentCmd);
        
        CLICommand phase6DemoCmd;
        phase6DemoCmd.name = "phase6";
        phase6DemoCmd.description = "Phase 6 integration tests";
        phase6DemoCmd.help = "Run Phase 6 integration tests. Verifies all brain systems are properly connected.";
        phase6DemoCmd.handler = [this](const std::map<std::string, std::string>& args, std::ostream& out) {
            return handlePhase6DemoCommand(args, out);
        };
        phase6DemoCmd.options["help"] = OptionType::Flag;
        registerCommand(phase6DemoCmd);
        
        CLICommand visualizationCmd;
        visualizationCmd.name = "visualize";
        visualizationCmd.description = "Visualization and analysis";
        visualizationCmd.help = "Generate visualizations and analyses. Support various output formats.";
        visualizationCmd.handler = [this](const std::map<std::string, std::string>& args, std::ostream& out) {
            return handleVisualizationCommand(args, out);
        };
        visualizationCmd.options["help"] = OptionType::Flag;
        registerCommand(visualizationCmd);
        
        CLICommand simCmd;
        simCmd.name = "simulate";
        simCmd.description = "Run simulation";
        simCmd.help = "Run NLM simulation. Supports various simulation modes and parameters.";
        simCmd.handler = [this](const std::map<std::string, std::string>& args, std::ostream& out) {
            return handleSimulationCommand(args, out);
        };
        simCmd.options["help"] = OptionType::Flag;
        registerCommand(simCmd);
        
        CLICommand debugCmd;
        debugCmd.name = "debug";
        debugCmd.description = "Debugging and profiling tools";
        debugCmd.help = "Debug NLM execution. Includes profiling, memory analysis, and performance monitoring.";
        debugCmd.handler = [this](const std::map<std::string, std::string>& args, std::ostream& out) {
            return handleDebugCommand(args, out);
        };
        debugCmd.options["help"] = OptionType::Flag;
        registerCommand(debugCmd);
        
        // Set the subcommand option
        configManager.addHelp("subcommand", "The command to execute");
        configManager.addHelp("help", "Show help for a command");
        configManager.addHelp("version", "Show NLM version");
    }
    
    // Parse global options
    void parseGlobalOptions() {
        experimentalEnabled = configManager.getBool("experimental", false);
    }
    
    // Command handlers (placeholders for now)
    int handleConfigCommand(const std::map<std::string, std::string>& args, std::ostream& out) {
        if (configManager.hasOption("--help") || args.find("help") != args.end()) {
            out << "Configuration CLI Commands:" << std::endl;
            out << "  show    Show current configuration" << std::endl;
            out << "  load    Load configuration from file" << std::endl;
            out << "  save    Save configuration to file" << std::endl;
            out << "  set     Set configuration value" << std::endl;
            out << "  unset   Remove configuration value" << std::endl;
            out << "  list    List all configuration keys" << std::endl;
            return 0;
        }
        
        out << "Configuration CLI not yet implemented" << std::endl;
        return 1;
    }
    
    int handleCheckpointCommand(const std::map<std::string, std::string>& args, std::ostream& out) {
        if (configManager.hasOption("--help") || args.find("help") != args.end()) {
            out << "Checkpoint CLI Commands:" << std::endl;
            out << "  save    Save brain checkpoint" << std::endl;
            out << "  load    Load brain checkpoint" << std::endl;
            out << "  list    List available checkpoints" << std::endl;
            out << "  delete  Delete checkpoint" << std::endl;
            return 0;
        }
        
        out << "Checkpoint CLI not yet implemented" << std::endl;
        return 1;
    }
    
    int handleExperimentCommand(const std::map<std::string, std::string>& args, std::ostream& out) {
        if (configManager.hasOption("--help") || args.find("help") != args.end()) {
            out << "Experiment CLI Commands:" << std::endl;
            out << "  run      Run experiment(s)" << std::endl;
            out << "  list     List available experiments" << std::endl;
            out << "  batch    Run experiments in batch mode" << std::endl;
            out << "  analyze  Analyze experiment results" << std::endl;
            return 0;
        }
        
        out << "Experiment CLI not yet implemented" << std::endl;
        return 1;
    }
    
    int handlePhase6DemoCommand(const std::map<std::string, std::string>& args, std::ostream& out) {
        if (configManager.hasOption("--help") || args.find("help") != args.end()) {
            out << "Phase 6 Demo CLI Commands:" << std::endl;
            out << "  run      Run Phase 6 integration demo" << std::endl;
            out << "  verify   Run integration verification only" << std::endl;
            out << "  test     Run individual system tests" << std::endl;
            return 0;
        }
        
        out << "Phase 6 Demo CLI not yet implemented" << std::endl;
        return 1;
    }
    
    int handleVisualizationCommand(const std::map<std::string, std::string>& args, std::ostream& out) {
        if (configManager.hasOption("--help") || args.find("help") != args.end()) {
            out << "Visualization CLI Commands:" << std::endl;
            out << "  generate  Generate visualizations" << std::endl;
            out << "  analyze   Analyze simulation data" << std::endl;
            out << "  export    Export data to various formats" << std::endl;
            return 0;
        }
        
        out << "Visualization CLI not yet implemented" << std::endl;
        return 1;
    }
    
    int handleSimulationCommand(const std::map<std::string, std::string>& args, std::ostream& out) {
        if (configManager.hasOption("--help") || args.find("help") != args.end()) {
            out << "Simulation CLI Commands:" << std::endl;
            out << "  run      Run simulation" << std::endl;
            out << "  profile  Run with profiling enabled" << std::endl;
            out << "  benchmark Run benchmark" << std::endl;
            return 0;
        }
        
        out << "Simulation CLI not yet implemented" << std::endl;
        return 1;
    }
    
    int handleDebugCommand(const std::map<std::string, std::string>& args, std::ostream& out) {
        if (configManager.hasOption("--help") || args.find("help") != args.end()) {
            out << "Debug CLI Commands:" << std::endl;
            out << "  profile  Run profiler" << std::endl;
            out << "  memory   Check memory usage" << std::endl;
            out << "  trace    Enable execution tracing" << std::endl;
            out << "  stats    Show system statistics" << std::endl;
            return 0;
        }
        
        out << "Debug CLI not yet implemented" << std::endl;
        return 1;
    }
    
private:
    int argc;
    char** argv;
    std::string programName;
    CLIConfigManager configManager;
    std::map<std::string, CLICommand> commands;
    std::map<std::string, CLICommand>::iterator currentCommand;
    bool experimentalEnabled;
    int exitCode;
};

} // namespace nlm