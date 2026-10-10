# NLM Command System Design

## Overview

This document outlines the design for an improved command system for the NLM (Neural Learning Machine) brain simulation framework. The new system will address the current limitations in usability, configuration, and user experience while maintaining backward compatibility.

## Current System Issues

The existing command system in `src/main.cpp` has several limitations:

1. **Basic argument parsing**: Only supports `--config` file option
2. **No help system**: Users don't know available options
3. **Limited configuration**: No runtime configuration modification
4. **No interactive mode**: Cannot run multiple experiments or demos
5. **Poor error handling**: Basic error messages with no guidance
6. **No command discovery**: Users can't explore available features

## Design Principles

### 1. User-Centric Design
- **Beginners first**: Simple, intuitive interface for new users
- **Progressive disclosure**: Advanced features hidden until needed
- **Guided experience**: Help and examples built into the system

### 2. Extensible Architecture
- **Plugin system**: Easy to add new commands and features
- **Command registry**: Centralized command management
- **Configurable behavior**: Runtime configuration options

### 3. Robust Implementation
- **Error recovery**: Graceful handling of errors
- **Validation**: Input validation with helpful messages
- **Logging**: Comprehensive logging at multiple levels

## Command System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                 Command System Core                        │
├─────────────────────────────────────────────────────────────┤
│  Command Parser  |  Command Registry  |  Configuration     │
│                 |                  |  Manager          │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌──────────────┐  ┌───────────────────┐ │
│  │  Brain      │  │  Help System │  │  Config System   │ │
│  │  Commands   │  │              │  │                 │ │
│  └─────────────┘  └──────────────┘  └───────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
┌───────▼───────┐  ┌─────────▼─────────┐  ┌─────────▼─────────┐
│  Brain        │  │  World/Simulation │  │  Experiment/Metrics│
│  Operations   │  │  Commands         │  │  Commands         │
└───────────────┘  └───────────────────┘  └───────────────────┘
```

## Command Categories

### 1. Brain Management Commands
- `brain` - Brain initialization, configuration, and control
- `region` - Neural region management
- `neuron` - Individual neuron operations
- `synapse` - Synaptic connection management

### 2. Simulation Control Commands
- `run` - Execute simulations with various parameters
- `step` - Execute individual simulation steps
- `pause`/`resume` - Control simulation execution
- `reset` - Reset brain state

### 3. World Interaction Commands
- `world` - World creation and configuration
- `agent` - Agent brain interface management
- `sensory` - Sensory input processing
- `motor` - Motor command processing

### 4. Experiment Commands
- `experiment` - Run predefined experiments
- `compare` - Compare experiment results
- `benchmark` - Performance benchmarking
- `profile` - Performance profiling

### 5. Configuration Commands
- `config` - Configuration file management
- `set` - Runtime configuration modification
- `get` - Query configuration values
- `validate` - Configuration validation

### 6. Information Commands
- `info` - System information and status
- `help` - Command help and documentation
- `list` - Available commands and options
- `version` - Version and build information

### 7. Visualization Commands
- `plot` - Generate plots and visualizations
- `save` - Save simulation results
- `export` - Export data in various formats
- `view` - View saved results

## Command Structure

### Base Command Interface
```cpp
class Command {
public:
    virtual ~Command() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<std::string> getAliases() const { return {}; }
    virtual bool requiresBrain() const { return false; }
    virtual bool requiresWorld() const { return false; }
    virtual bool isInteractive() const { return false; }
    
    virtual bool execute(const std::vector<std::string>& args, 
                        NLMBrain* brain = nullptr,
                        World* world = nullptr) = 0;
};
```

### Command Parser
```cpp
class CommandParser {
public:
    CommandParser();
    void registerCommand(std::unique_ptr<Command> cmd);
    bool executeCommand(const std::string& input, 
                       NLMBrain* brain = nullptr,
                       World* world = nullptr);
    std::vector<std::string> getCompletions(const std::string& prefix);
    std::string getHelp(const std::string& commandName);
    
private:
    std::unordered_map<std::string, std::unique_ptr<Command>> commands;
    std::vector<std::unique_ptr<Command>> commandList;
};
```

### Configuration Manager
```cpp
class ConfigurationManager {
public:
    ConfigurationManager();
    bool loadFromFile(const std::string& filename);
    bool loadFromArgs(int argc, char* argv[]);
    bool setValue(const std::string& key, const std::string& value);
    std::string getValue(const std::string& key) const;
    bool validateConfig() const;
    void resetToDefaults();
    
private:
    Config config;
    std::unordered_map<std::string, std::function<void(const std::string&)>> validators;
};
```

## Command Implementation Examples

### Brain Initialization Command
```cpp
class BrainCommand : public Command {
public:
    std::string getName() const override { return "brain"; }
    std::string getDescription() const override { 
        return "Initialize and configure a brain for simulation"; 
    }
    
    bool execute(const std::vector<std::string>& args, 
                NLMBrain* brain = nullptr,
                World* world = nullptr) override {
        if (!brain) {
            std::cerr << "Error: No brain available. Use 'brain init' first." << std::endl;
            return false;
        }
        
        if (args.empty()) {
            printBrainStatus(brain);
            return true;
        }
        
        if (args[0] == "init") {
            if (args.size() < 2) {
                std::cerr << "Usage: brain init <config_file>" << std::endl;
                return false;
            }
            return brain->initialize(args[1]);
        } else if (args[0] == "config") {
            // Handle brain configuration
        }
        
        std::cerr << "Unknown brain subcommand: " << args[0] << std::endl;
        return false;
    }
};
```

### Run Simulation Command
```cpp
class RunCommand : public Command {
public:
    std::string getName() const override { return "run"; }
    std::string getDescription() const override { 
        return "Run a simulation with specified parameters"; 
    }
    bool requiresBrain() const override { return true; }
    bool requiresWorld() const override { return true; }
    
    bool execute(const std::vector<std::string>& args, 
                NLMBrain* brain = nullptr,
                World* world = nullptr) override {
        SimulationParams params = parseSimulationArgs(args);
        
        if (!validateSimulationParams(params)) {
            return false;
        }
        
        std::cout << "Starting simulation with " << params.steps << " steps..." << std::endl;
        
        SimulationResult result;
        if (!runSimulation(brain, world, params, result)) {
            std::cerr << "Simulation failed!" << std::endl;
            return false;
        }
        
        printSimulationResults(result);
        saveResults(result);
        
        return true;
    }
};
```

## Interactive Mode

### REPL (Read-Eval-Print Loop)
```cpp
class InteractiveMode {
public:
    InteractiveMode() : parser(), configManager() {
        registerAllCommands();
    }
    
    void start() {
        std::cout << "NLM Interactive Mode" << std::endl;
        std::cout << "Type 'help' for commands, 'exit' to quit" << std::endl;
        
        std::string input;
        while (std.tcout() >> input) {
            if (input == "exit" || input == "quit") {
                break;
            }
            
            handleCommand(input);
        }
    }
    
private:
    void handleCommand(const std::string& input) {
        auto tokens = splitInput(input);
        if (tokens.empty()) {
            return;
        }
        
        std::string commandName = tokens[0];
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());
        
        parser.executeCommand(commandName, args, currentBrain, currentWorld);
    }
};
```

### Tab Completion
```cpp
class TabCompleter {
public:
    std::vector<std::string> getCompletions(const std::string& input) {
        auto tokens = splitInput(input);
        
        if (tokens.empty()) {
            return getCommandCompletions();
        }
        
        std::string prefix = tokens.back();
        
        if (tokens.size() == 1) {
            return getCommandCompletions(prefix);
        } else {
            return getArgumentCompletions(tokens, prefix);
        }
    }
    
private:
    std::vector<std::string> getCommandCompletions(const std::string& prefix = "");
    std::vector<std::string> getArgumentCompletions(const std::vector<std::string>& tokens, const std::string& prefix);
};
```

## Configuration System

### Configuration File Format
```ini
# NLM Configuration File

[brain]
neuron_count = 1000
region_count = 2
connection_probability = 0.1
random_seed = 42
simulation_timestep = 0.001

[plasticity]
enable_stdp = true
enable_hebbian = true
enable_structural = true

[neuromodulation]
enable_dopamine = true
enable_curiosity = true
enable_novelty = true

[simulation]
steps_per_episode = 1000
num_episodes = 5
enable_checkpointing = true
enable_replay = false

[world]
width = 20
height = 20
vision_width = 8
vision_height = 8
```

### Runtime Configuration Commands
```cpp
class ConfigCommand : public Command {
public:
    std::string getName() const override { return "config"; }
    std::string getDescription() const override { 
        return "Configuration file and runtime settings management"; 
    }
    
    bool execute(const std::vector<std::string>& args, 
                NLMBrain* brain = nullptr,
                World* world = nullptr) override {
        if (args.empty()) {
            printConfigHelp();
            return true;
        }
        
        if (args[0] == "load") {
            return configManager.loadFromFile(args[1]);
        } else if (args[0] == "set") {
            if (args.size() < 3) {
                std::cerr << "Usage: config set <key> <value>" << std::endl;
                return false;
            }
            return configManager.setValue(args[1], args[2]);
        } else if (args[0] == "get") {
            if (args.size() < 2) {
                std::cerr << "Usage: config get <key>" << std::endl;
                return false;
            }
            std::string value = configManager.getValue(args[1]);
            if (value.empty()) {
                std::cout << "Key not found: " << args[1] << std::endl;
            } else {
                std::cout << args[1] << " = " << value << std::endl;
            }
            return true;
        }
        
        std::cerr << "Unknown config command: " << args[0] << std::endl;
        return false;
    }
};
```

## Help System

### Context-Sensitive Help
```cpp
class HelpSystem {
public:
    void registerHelp(const std::string& command, 
                     const std::string& description,
                     const std::string& usage,
                     const std::vector<std::string>& examples = {});
    
    void showHelp(const std::string& command = "");
    void showUsage(const std::string& command);
    
private:
    struct CommandHelp {
        std::string description;
        std::string usage;
        std::vector<std::string> examples;
    };
    
    std::unordered_map<std::string, CommandHelp> helpEntries;
};
```

### Help Command
```cpp
class HelpCommand : public Command {
public:
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { 
        return "Show help information for commands and topics"; 
    }
    
    bool execute(const std::vector<std::string>& args, 
                NLMBrain* brain = nullptr,
                World* world = nullptr) override {
        if (args.empty()) {
            showGeneralHelp();
            return true;
        }
        
        std::string command = args[0];
        if (command == "brain") {
            showBrainHelp();
        } else if (command == "run") {
            showRunHelp();
        } else {
            return helpSystem.showHelp(command);
        }
        
        return true;
    }
};
```

## Error Handling and Validation

### Error Handling Class
```cpp
class ErrorHandler {
public:
    enum class ErrorType {
        INVALID_COMMAND,
        MISSING_BRAIN,
        MISSING_WORLD,
        INVALID_ARGUMENT,
        FILE_NOT_FOUND,
        CONFIG_VALIDATION,
        SIMULATION_ERROR
    };
    
    static void handleError(ErrorType type, const std::string& message);
    static std::string getErrorMessage(ErrorType type);
    static bool isRecoverable(ErrorType type);
};
```

## Integration with Existing Code

### Backward Compatibility
The new command system must maintain backward compatibility with the existing `main.cpp` interface:

1. **Command-line arguments**: `--config file.cfg` still works
2. **Basic execution**: Running `./nlm` should default to interactive mode
3. **Experiment execution**: Existing experiment executables should still work
4. **Configuration loading**: Config file loading should work as before

### Migration Path
1. **Phase 1**: Add command system to existing `main.cpp`
2. **Phase 2**: Add interactive mode and basic commands
3. **Phase 3**: Add experiment and simulation commands
4. **Phase 4**: Add configuration and brain management commands
5. **Phase 5**: Add help system and error handling
6. **Phase 6**: Add advanced features and documentation

## Implementation Timeline

### Week 1-2: Core Infrastructure
- Design command system architecture
- Implement base command classes
- Create command parser and registry
- Add basic error handling

### Week 3-4: Command Implementation
- Implement brain management commands
- Add simulation control commands
- Create world interaction commands
- Implement basic help system

### Week 5-6: Configuration System
- Implement configuration manager
- Add configuration commands
- Create configuration file parser
- Add runtime configuration

### Week 7-8: Interactive Mode
- Implement REPL (Read-Eval-Print Loop)
- Add tab completion
- Create command history
- Add context-sensitive help

### Week 9-10: Integration
- Integrate with existing code
- Add backward compatibility
- Fix main.cpp to use new command system
- Add Phase 5 demo executable

### Week 11-12: Testing and Refinement
- Comprehensive testing
- Performance optimization
- Documentation updates
- User interface improvements

## Success Criteria

### Technical Requirements
1. **Backward Compatibility**: Existing code and executables must continue to work
2. **Performance**: Command execution should not significantly impact simulation performance
3. **Reliability**: Error handling should be robust and informative
4. **Extensibility**: New commands should be easy to add
5. **Validation**: All user inputs should be validated

### User Experience Requirements
1. **Usability**: System should be intuitive for beginners
2. **Documentation**: Help system should be comprehensive
3. **Feedback**: Users should receive clear feedback for all actions
4. **Discovery**: Users should be able discover available features
5. **Helpfulness**: Error messages should guide users to solutions

### Code Quality Requirements
1. **Maintainability**: Code should be well-organized and documented
2. **Testability**: Components should be easily testable
3. **Modularity**: Components should be loosely coupled
4. **Performance**: Should not introduce significant overhead
5. **Compatibility**: Should work across different platforms and compilers

## Conclusion

The improved command system will transform the NLM brain simulation from a command-line tool with limited functionality to a comprehensive, user-friendly application that supports users of all skill levels. By implementing this design, the NLM project will become more accessible, maintainable, and extensible, ensuring its continued success as a research and educational tool.

The key to success will be maintaining backward compatibility while introducing new features, ensuring that existing users can continue using the system while new users benefit from the improved interface. The implementation must be careful, tested, and documented to ensure a smooth transition.
