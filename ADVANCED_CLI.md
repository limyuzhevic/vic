# NLM Advanced Command-Line Interface

## Overview
This module implements advanced command-line interface features for NLM (Neural Learning Machine) power users. It provides comprehensive command-line tools for configuration management, debugging, monitoring, and simulation control.

## Features

### 1. Command-Line Argument Parsing
- Advanced argument parsing with validation
- Command registration system
- Help system with auto-generated documentation
- Tab completion support
- Command history

### 2. Configuration Management Commands
- Validate configuration files
- List configuration entries
- Set configuration values
- Save configuration to file
- Load configuration from file
- Configuration diff

### 3. Debug and Monitoring Commands
- Step-by-step execution
- Breakpoint management
- Variable inspection
- Performance profiling
- Memory monitoring
- Simulation control (pause/resume/step)

### 4. Experiment Management Commands
- Run batch simulations
- Parameter sweeping
- Result aggregation
- Experiment tracking
- Report generation

## Command-Line Interface Structure

### Core Components
```
cli/
├── nlm_cli.cpp               # Main CLI entry point
├── command_manager.cpp       # Command management system
├── argument_parser.cpp        # Argument parsing
├── help_system.cpp            # Help system
├── shell.cpp                 # Interactive shell
├── readline.cpp              # Readline integration
│
├── commands/                 # Command implementations
│   ├── config_commands.cpp    # Configuration commands
│   ├── debug_commands.cpp     # Debug commands
│   ├── monitor_commands.cpp    # Monitoring commands
│   ├── experiment_commands.cpp # Experiment commands
│   ├── visualization_commands.cpp # Visualization commands
│   └── system_commands.cpp     # System commands
│
├── parsers/                  # Argument parsers
│   ├── basic_parser.cpp       # Basic argument parsing
│   ├── advanced_parser.cpp    # Advanced parsing
│   └── validation_parser.cpp  # Argument validation
│
└── utils/                    # Utility functions
    ├── command_utils.cpp      # Command utilities
    ├── help_utils.cpp         # Help utilities
    ├── shell_utils.cpp        # Shell utilities
    └── completion_utils.cpp   # Tab completion utilities
```

## Command-Line Interface Implementation

### 1. Command Management System

The command management system provides:

#### Command Registration
```cpp
class CommandManager {
public:
    // Register a new command
    void registerCommand(std::unique_ptr<Command> cmd);
    
    // Get a command by name
    Command* getCommand(const std::string& name);
    
    // Get all commands
    std::vector<Command*> getAllCommands() const;
    
    // Execute a command
    bool executeCommand(const std::vector<std::string>& args);
    
    // Get command help
    std::string getCommandHelp(const std::string& name) const;
    
    // Get all command help
    std::string getAllCommandsHelp() const;
};
```

#### Command Interface
```cpp
class Command {
public:
    virtual ~Command() = default;
    
    // Get command name
    virtual std::string getName() const = 0;
    
    // Get command description
    virtual std::string getDescription() const = 0;
    
    // Get command usage
    virtual std::string getUsage() const = 0;
    
    // Get command help (detailed)
    virtual std::string getHelp() const = 0;
    
    // Execute the command
    virtual bool execute(const std::vector<std::string>& args) = 0;
    
    // Validate arguments
    virtual bool validateArguments(const std::vector<std::string>& args) const = 0;
};
```

### 2. Configuration Commands

#### Config Validate Command
```cpp
class ConfigValidateCommand : public Command {
public:
    std::string getName() const override { return "validate"; }
    std::string getDescription() const override { 
        return "Validate configuration file or current configuration"; 
    }
    std::string getUsage() const override { 
        return "nlm validate [--file <config_file>] [--strict]"; 
    }
    std::string getHelp() const override {
        return "Validates configuration file or current configuration.\n"
               "Options:\n"
               "  --file <file>    Configuration file to validate\n"
               "  --strict          Enable strict validation\n"
               "  --verbose         Enable verbose output";
    }
    
    bool execute(const std::vector<std::string>& args) override {
        // Parse arguments
        std::string configFile;
        bool strict = false;
        bool verbose = false;
        
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--file" && i + 1 < args.size()) {
                configFile = args[++i];
            } else if (args[i] == "--strict") {
                strict = true;
            } else if (args[i] == "--verbose") {
                verbose = true;
            }
        }
        
        // Validate configuration
        if (!configFile.empty()) {
            return validateFile(configFile, strict, verbose);
        } else {
            return validateCurrent(strict, verbose);
        }
    }
    
private:
    bool validateFile(const std::string& filepath, bool strict, bool verbose);
    bool validateCurrent(bool strict, bool verbose);
};
```

#### Config List Command
```cpp
class ConfigListCommand : public Command {
public:
    std::string getName() const override { return "list"; }
    std::string getDescription() const override { 
        return "List configuration entries"; 
    }
    std::string getUsage() const override { 
        return "nlm list [--type <type>] [--source <source>] [--all]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        // Parse arguments
        std::string type;
        std::string source;
        bool all = false;
        
        // Parse arguments (similar to validate command)
        
        // List configuration entries
        return listEntries(type, source, all);
    }
    
private:
    bool listEntries(const std::string& type, const std::string& source, bool all);
};
```

#### Config Set Command
```cpp
class ConfigSetCommand : public Command {
public:
    std::string getName() const override { return "set"; }
    std::string getDescription() const override { 
        return "Set configuration value"; 
    }
    std::string getUsage() const override { 
        return "nlm set <key> <value> [--source <source>]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        if (args.size() < 2) {
            return false;
        }
        
        std::string key = args[0];
        std::string value = args[1];
        std::string source = "commandline";
        
        // Parse additional arguments
        for (size_t i = 2; i < args.size(); ++i) {
            if (args[i] == "--source" && i + 1 < args.size()) {
                source = args[++i];
            }
        }
        
        // Set configuration value
        return setValue(key, value, source);
    }
    
private:
    bool setValue(const std::string& key, const std::string& value, const std::string& source);
};
```

### 3. Debug Commands

#### Debug Step Command
```cpp
class DebugStepCommand : public Command {
public:
    std::string getName() const override { return "step"; }
    std::string getDescription() const override { 
        return "Execute one simulation step"; 
    }
    std::string getUsage() const override { 
        return "nlm debug step [--brain <brain_name>] [--step <step_number>]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        // Parse arguments
        std::string brainName;
        int stepNumber = 0;
        
        // Parse arguments
        
        // Execute one step
        return executeStep(brainName, stepNumber);
    }
    
private:
    bool executeStep(const std::string& brainName, int stepNumber);
};
```

#### Debug Breakpoint Command
```cpp
class DebugBreakpointCommand : public Command {
public:
    std::string getName() const override { return "breakpoint"; }
    std::string getDescription() const override { 
        return "Set or remove breakpoint"; 
    }
    std::string getUsage() const override { 
        return "nlm debug breakpoint <command> [args]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            return false;
        }
        
        std::string command = args[0];
        
        if (command == "set") {
            return setBreakpoint(args);
        } else if (command == "remove") {
            return removeBreakpoint(args);
        } else if (command == "list") {
            return listBreakpoints(args);
        } else {
            return false;
        }
    }
    
private:
    bool setBreakpoint(const std::vector<std::string>& args);
    bool removeBreakpoint(const std::vector<std::string>& args);
    bool listBreakpoints(const std::vector<std::string>& args);
};
```

#### Debug Watch Command
```cpp
class DebugWatchCommand : public Command {
public:
    std::string getName() const override { return "watch"; }
    std::string getDescription() const override { 
        return "Watch variable values"; 
    }
    std::string getUsage() const override { 
        return "nlm debug watch <variable> [--update] [--condition <condition>]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            return false;
        }
        
        std::string variable = args[0];
        bool update = false;
        std::string condition;
        
        // Parse additional arguments
        
        // Start watching variable
        return startWatching(variable, update, condition);
    }
    
private:
    bool startWatching(const std::string& variable, bool update, const std::string& condition);
};
```

### 4. Monitoring Commands

#### Monitor Performance Command
```cpp
class MonitorPerformanceCommand : public Command {
public:
    std::string getName() const override { return "monitor"; }
    std::string getDescription() const override { 
        return "Monitor system performance"; 
    }
    std::string getUsage() const override { 
        return "nlm monitor <type> [--duration <duration>] [--output <output_file>]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            return false;
        }
        
        std::string type = args[0];
        int duration = 0;
        std::string outputFile;
        
        // Parse additional arguments
        
        // Monitor performance
        return monitorPerformance(type, duration, outputFile);
    }
    
private:
    bool monitorPerformance(const std::string& type, int duration, const std::string& outputFile);
};
```

#### Monitor Memory Command
```cpp
class MonitorMemoryCommand : public Command {
public:
    std::string getName() const override { return "memory"; }
    std::string getDescription() const override { 
        return "Monitor memory usage"; 
    }
    std::string getUsage() const override { 
        return "nlm memory [--process <pid>] [--detailed] [--output <output_file>]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        // Parse arguments
        int processId = -1;
        bool detailed = false;
        std::string outputFile;
        
        // Parse additional arguments
        
        // Monitor memory
        return monitorMemory(processId, detailed, outputFile);
    }
    
private:
    bool monitorMemory(int processId, bool detailed, const std::string& outputFile);
};
```

### 5. Experiment Commands

#### Experiment Run Command
```cpp
class ExperimentRunCommand : public Command {
public:
    std::string getName() const override { return "experiment"; }
    std::string getDescription() const override { 
        return "Run experiment with specified configuration"; 
    }
    std::string getUsage() const override { 
        return "nlm experiment <experiment_config> [--output <output_dir>] [--resume]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            return false;
        }
        
        std::string experimentConfig = args[0];
        std::string outputDir;
        bool resume = false;
        
        // Parse additional arguments
        
        // Run experiment
        return runExperiment(experimentConfig, outputDir, resume);
    }
    
private:
    bool runExperiment(const std::string& config, const std::string& outputDir, bool resume);
};
```

#### Experiment Sweep Command
```cpp
class ExperimentSweepCommand : public Command {
public:
    std::string getName() const override { return "sweep"; }
    std::string getDescription() const override { 
        return "Run parameter sweep experiment"; 
    }
    std::string getUsage() const override { 
        return "nlm sweep <param_file> [--output <output_dir>] [--parallel] [--grid]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            return false;
        }
        
        std::string paramFile = args[0];
        std::string outputDir;
        bool parallel = false;
        bool grid = false;
        
        // Parse additional arguments
        
        // Run parameter sweep
        return runParameterSweep(paramFile, outputDir, parallel, grid);
    }
    
private:
    bool runParameterSweep(const std::string& paramFile, const std::string& outputDir, 
                          bool parallel, bool grid);
};
```

### 6. System Commands

#### System Info Command
```cpp
class SystemInfoCommand : public Command {
public:
    std::string getName() const override { return "system"; }
    std::string getDescription() const override { 
        return "Display system information"; 
    }
    std::string getUsage() const override { 
        return "nlm system info [--detailed] [--json]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        bool detailed = false;
        bool json = false;
        
        // Parse additional arguments
        
        // Display system information
        return displaySystemInfo(detailed, json);
    }
    
private:
    bool displaySystemInfo(bool detailed, bool json);
};
```

### 7. Help System

The help system provides:

#### Command Help
```cpp
class HelpCommand : public Command {
public:
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { 
        return "Display help information"; 
    }
    std::string getUsage() const override { 
        return "nlm help [command] [--detailed] [--examples]"; 
    }
    
    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            // Display all commands help
            return displayAllCommandsHelp();
        } else {
            // Display specific command help
            std::string commandName = args[0];
            bool detailed = false;
            bool examples = false;
            
            // Parse additional arguments
            
            return displayCommandHelp(commandName, detailed, examples);
        }
    }
    
private:
    bool displayAllCommandsHelp();
    bool displayCommandHelp(const std::string& commandName, bool detailed, bool examples);
};
```

### 8. Interactive Shell

The interactive shell provides:

#### Shell Interface
```cpp
class InteractiveShell {
public:
    InteractiveShell();
    ~InteractiveShell();
    
    // Start the interactive shell
    void start();
    
    // Stop the interactive shell
    void stop();
    
    // Execute a command
    bool executeCommand(const std::string& commandLine);
    
    // Display prompt
    void displayPrompt();
    
    // Get command history
    std::vector<std::string> getCommandHistory() const;
    
    // Clear command history
    void clearCommandHistory();
    
    // Set command completion
    void setCommandCompletion(std::function<bool(const std::string&)> completer);
    
    // Get command completion
    std::vector<std::string> getCommandCompletion(const std::string& partialCommand);
};
```

## Command-Line Interface Integration

### 1. Main CLI Entry Point
```cpp
// nlm_cli.cpp
#include "cli/nlm_cli.h"
#include "command_manager.h"
#include "help_command.h"
#include "config_commands.h"
#include "debug_commands.h"
#include "monitor_commands.h"
#include "experiment_commands.h"
#include "system_commands.h"

int main(int argc, char* argv[]) {
    try {
        // Initialize CLI system
        CLIApplication app;
        
        // Register all commands
        registerCommands(app.getCommandManager());
        
        // Parse command line arguments
        std::vector<std::string> args = parseArguments(argc, argv);
        
        // Execute command
        if (args.empty()) {
            // Display help
            app.displayHelp();
            return 0;
        }
        
        return app.executeCommand(args) ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

### 2. Command Registration
```cpp
// Command registration
def registerCommands(CommandManager* commandManager) {
    // Register help command
    commandManager->registerCommand(std::make_unique<HelpCommand>());
    
    // Register configuration commands
    commandManager->registerCommand(std::make_unique<ConfigValidateCommand>());
    commandManager->registerCommand(std::make_unique<ConfigListCommand>());
    commandManager->registerCommand(std::make_unique<ConfigSetCommand>());
    
    // Register debug commands
    commandManager->registerCommand(std::make_unique<DebugStepCommand>());
    commandManager->registerCommand(std::make_unique<DebugBreakpointCommand>());
    commandManager->registerCommand(std::make_unique<DebugWatchCommand>());
    
    // Register monitoring commands
    commandManager->registerCommand(std::make_unique<MonitorPerformanceCommand>());
    commandManager->registerCommand(std::make_unique<MonitorMemoryCommand>());
    
    // Register experiment commands
    commandManager->registerCommand(std::make_unique<ExperimentRunCommand>());
    commandManager->registerCommand(std::make_unique<ExperimentSweepCommand>());
    
    // Register system commands
    commandManager->registerCommand(std::make_unique<SystemInfoCommand>());
}
```

### 3. Argument Parsing
```cpp
// Argument parsing
def parseArguments(int argc, char* argv[]) {
    std::vector<std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            // Show help and exit
            // (Help will be shown by command manager)
            return {"help"};
        } else if (arg == "--version" || arg == "-v") {
            // Show version and exit
            return {"version"};
        } else if (arg == "--verbose" || arg == "-V") {
            // Set verbose flag
            // This would be handled by the argument parser
        } else if (arg.substr(0, 2) == "--") {
            // Long option
            args.push_back(arg);
        } else if (arg.substr(0, 1) == "-") {
            // Short option
            args.push_back(arg);
        } else {
            // Positional argument
            args.push_back(arg);
        }
    }
    
    return args;
}
```

## Command-Line Interface Features

### 1. Tab Completion
- Intelligent tab completion for commands
- Command argument completion
- File path completion
- Configuration key completion

### 2. Command History
- Command history for easy re-execution
- History search with Ctrl+R
- History navigation with up/down arrows

### 3. Help System
- Auto-generated command documentation
- Detailed help with examples
- Command usage examples
- Related command suggestions

### 4. Error Handling
- Detailed error messages
- Error recovery
- Help on error
- Command validation

### 5. Configuration Management
- Configuration validation
- Configuration backup
- Configuration comparison
- Configuration diff

### 6. Debugging Support
- Breakpoint management
- Step-by-step execution
- Variable inspection
- Stack trace

### 7. Performance Monitoring
- Real-time performance monitoring
- Resource usage tracking
- Performance profiling
- Bottleneck detection

### 8. Experiment Management
- Experiment tracking
- Result aggregation
- Parameter sweeping
- Batch execution

## Command-Line Interface Configuration

### Configuration File
The command-line interface can be configured using a configuration file:

```
# CLI Configuration
verbose = true
history_file = ~/.nlm_history
tab_completion = true
auto_save_config = true
backup_before_save = true

# Command-specific settings
[command validate]
strict_mode = false
verbose_output = true

[command debug]
step_count = 1
breakpoint_file = ~/.nlm_breakpoints

[command monitor]
default_duration = 60
output_format = json
```

### Environment Variables
- `NLM_VERBOSE`: Enable verbose output
- `NLM_CONFIG_FILE`: Configuration file path
- `NLM_HISTORY_FILE`: Command history file path
- `NLM_BREAKPOINT_FILE`: Breakpoint file path
- `NLM_LOG_FILE`: Log file path

## Command-Line Interface Implementation

### 1. Build System Integration
```cmake
# CMakeLists.txt additions
add_executable(nlm_cli
    cli/nlm_cli.cpp
    command_manager.cpp
    help_system.cpp
    shell.cpp
    readline.cpp
    
    commands/config_commands.cpp
    commands/debug_commands.cpp
    commands/monitor_commands.cpp
    commands/experiment_commands.cpp
    commands/visualization_commands.cpp
    commands/system_commands.cpp
    
    parsers/basic_parser.cpp
    parsers/advanced_parser.cpp
    parsers/validation_parser.cpp
    
    utils/command_utils.cpp
    utils/help_utils.cpp
    utils/shell_utils.cpp
    utils/completion_utils.cpp
)

target_link_libraries(nlm_cli PRIVATE nlm_core)
```

### 2. Installation
```bash
# Install command-line interface
make install

# Install to custom location
make install PREFIX=/usr/local

# Create symbolic link
sudo ln -s /usr/local/bin/nlm_cli /usr/local/bin/nlm
```

### 3. Usage Examples

#### Basic Usage
```bash
# Show help
nlm --help
nlm help

# Validate configuration
nlm validate
nlm validate --file config.txt
nlm validate --strict

# List configuration
nlm list
nlm list --type int
nlm list --source file

# Set configuration value
nlm set key value
nlm set key value --source commandline

# Run simulation step
nlm debug step
nlm debug step --step 100

# Monitor performance
nlm monitor cpu --duration 60
nlm monitor memory

# Run experiment
nlm experiment my_experiment
nlm experiment my_experiment --output results

# Run parameter sweep
nlm sweep param_file.txt
nlm sweep param_file.txt --parallel --grid

# System information
nlm system info
nlm system info --detailed
nlm system info --json
```

#### Interactive Shell
```bash
# Start interactive shell
nlm

# Execute commands
validate --strict
list --type string

# Tab completion
nlm val<tab>  # Completes to "validate"
nlm set<tab>  # Completes to "set"

# Command history
# Up arrow: previous command
# Down arrow: next command
# Ctrl+R: search history
```

## Command-Line Interface Development

### 1. Adding New Commands
To add a new command:

1. Create a new command class inheriting from `Command`
2. Implement the required virtual methods
3. Register the command with the command manager
4. Implement command-specific functionality
5. Add command to the help system
6. Test the command with various inputs

### 2. Command Development Checklist
- [ ] Implement command interface
- [ ] Add command validation
- [ ] Implement command execution logic
- [ ] Add error handling
- [ ] Update help system
- [ ] Add unit tests
- [ ] Integrate with CLI
- [ ] Test with real data

### 3. Testing Command-Line Interface
- **Unit tests**: Test individual commands
- **Integration tests**: Test command combinations
- **Functional tests**: Test complete workflows
- **Performance tests**: Test performance with large datasets
- **Error tests**: Test error conditions and recovery

## Conclusion

The advanced command-line interface provides power users with comprehensive tools for:

1. **Configuration management**: Validate, list, and modify configuration
2. **Debugging**: Step-by-step execution, breakpoints, and variable inspection
3. **Monitoring**: Real-time performance and resource monitoring
4. **Experiment management**: Run and analyze experiments
5. **Automation**: Batch processing and parameter sweeping

This command-line interface significantly enhances the NLM framework for power users, providing advanced tools for simulation control, debugging, monitoring, and automation while maintaining backward compatibility.