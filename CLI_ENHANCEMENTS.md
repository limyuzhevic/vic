NLM Command-Line Interface Enhancements
=======================================

## Overview

This document details the comprehensive command-line interface (CLI) enhancements for the NLM (Neural Learning Machine) system, including improved argument parsing, help systems, interactive modes, and advanced command options.

## Changes Made

### 1. Enhanced CLI Framework

#### Core CLI Architecture
```cpp
class NLMCLI {
public:
    enum CommandType {
        RUN,
        CONFIG,
        HELP,
        VERSION,
        BUILD,
        TEST,
        PROFILE,
        EXPORT,
        IMPORT,
        VALIDATE,
        ANALYZE,
        VISUALIZE,
        BENCHMARK,
        MONITOR,
        DEBUG,
        INFO,
        SET,
        GET,
        RESET,
        CUSTOM
    };
    
    struct Command {
        std::string name;
        std::string description;
        CommandType type;
        std::vector<std::string> arguments;
        std::map<std::string, std::string> options;
        bool requires_config;
        bool requires_brain;
        bool is_dangerous;
        std::string help_text;
        std::function<bool(const Command&)> validation_function;
        std::function<void(const Command&)> execution_function;
    };
    
    // CLI interface
    static int run(int argc, char** argv);
    static int runInteractive();
    static int runScript(const std::string& script_file);
    static int runBatch(const std::string& batch_file);
    
    // Command management
    static bool registerCommand(const Command& command);
    static bool unregisterCommand(const std::string& name);
    static Command getCommand(const std::string& name) const;
    static std::vector<Command> getAllCommands() const;
    static std::vector<Command> getAvailableCommands(const std::string& context) const;
    
    // Command parsing
    static Command parseCommandLine(int argc, char** argv);
    static std::vector<Command> parseCommandFile(const std::string& filepath);
    static std::map<std::string, std::string> parseOptions(const std::vector<std::string>& args);
    static std::vector<std::string> parseArguments(const std::vector<std::string>& args);
    
    // Command execution
    static int executeCommand(const Command& command);
    static int executeCommand(const std::string& name, 
                            const std::map<std::string, std::string>& options = {});
    
    // Command completion
    static std::vector<std::string> completeCommand(const std::string& partial_command);
    static std::vector<std::string> completeOption(const std::string& command_name, 
                                                   const std::string& partial_option);
    
    // Configuration management
    static bool loadConfiguration(const std::string& config_file);
    static bool saveConfiguration(const std::string& config_file);
    static Config getCurrentConfiguration() const;
    static bool applyConfiguration(const Config& config);
    
    // Brain management
    static bool createBrain(const Config& config);
    static bool loadBrain(const std::string& brain_file);
    static bool saveBrain(const std::string& brain_file);
    static bool resetBrain();
    static bool isBrainLoaded() const;
    static std::shared_ptr<Brain> getCurrentBrain() const;
    
    // State management
    static bool saveState(const std::string& state_file);
    static bool loadState(const std::string& state_file);
    static bool resetState();
    static std::string getSessionInfo() const;
    
    // Help and documentation
    static void showHelp(const std::string& command_name = "");
    static void showCommandHelp(const std::string& command_name);
    static void showOptionHelp(const std::string& command_name, 
                              const std::string& option_name);
    static std::string getCommandUsage(const Command& command);
    static std::string getFullUsage() const;
    
    // Configuration
    static void configureFromFile(const std::string& config_file);
    static void configureFromArgs(int argc, char** argv);
    static void configureDefaultSettings();
    
    // Global CLI access
    static NLMCLI& getInstance() {
        static NLMCLI instance;
        return instance;
    }
    
    static void initialize(const std::string& config_file = "");
    static void shutdown();
    
    static NLMCLI& instance() { return getInstance(); }
};
```

#### Enhanced Argument Parser
```cpp
class ArgumentParser {
public:
    struct Argument {
        std::string name;
        std::string short_name;
        std::string description;
        std::string type;
        std::string default_value;
        bool required;
        std::vector<std::string> valid_values;
        std::function<bool(const std::string&)> validation_function;
        bool has_value;
        std::string value;
        bool flag;
    };
    
    struct Option {
        std::string name;
        std::string short_name;
        std::string description;
        std::string type;
        std::string default_value;
        bool required;
        std::vector<std::string> valid_values;
        std::function<bool(const std::string&)> validation_function;
        bool has_value;
        std::string value;
        bool flag;
    };
    
    // Argument parsing interface
    static bool addArgument(const Argument& argument);
    static bool addOption(const Option& option);
    static bool parse(int argc, char** argv);
    static std::vector<std::string> getRemainingArguments() const;
    static std::string getArgument(const std::string& name) const;
    static bool hasArgument(const std::string& name) const;
    static std::string getOption(const std::string& name) const;
    static bool hasOption(const std::string& name) const;
    static std::map<std::string, std::string> getAllOptions() const;
    static std::vector<Argument> getAllArguments() const;
    static std::vector<Option> getAllOptionsList() const;
    
    // Argument completion
    static std::vector<std::string> completeArgument(const std::string& partial_name);
    static std::vector<std::string> completeOption(const std::string& partial_name);
    
    // Help and documentation
    static void showHelp() const;
    static std::string getUsage() const;
    
    // Configuration
    static void setShowHelpOnError(bool show);
    static void setHelpIndent(int indent);
    static void setShowVersion(bool show);
    static void setVersionString(const std::string& version);
    
    // Global argument parser access
    static ArgumentParser& getInstance() {
        static ArgumentParser instance;
        return instance;
    }
    
    static void initialize();
    static void shutdown();
    
    static ArgumentParser& instance() { return getInstance(); }
};
```

### 2. Enhanced Command Implementation

#### Basic Command Set
```cpp
class BasicCommands {
public:
    // Configuration commands
    static bool run(const std::map<std::string, std::string>& options);
    static bool config(const std::map<std::string, std::string>& options);
    static bool help(const std::map<std::string, std::string>& options);
    static bool version(const std::map<std::string, std::string>& options);
    static bool build(const std::map<std::string, std::string>& options);
    static bool test(const std::map<std::string, std::string>& options);
    
    // Brain management commands
    static bool create(const std::map<std::string, std::string>& options);
    static bool load(const std::map<std::string, std::string>& options);
    static bool save(const std::map<std::string, std::string>& options);
    static bool reset(const std::map<std::string, std::string>& options);
    
    // State management commands
    static bool state(const std::map<std::string, std::string>& options);
    static bool checkpoint(const std::map<std::string, std::string>& options);
    static bool restore(const std::map<std::string, std::string>& options);
    
    // Analysis commands
    static bool analyze(const std::map<std::string, std::string>& options);
    static bool profile(const std::map<std::string, std::string>& options);
    static bool benchmark(const std::map<std::string, std::string>& options);
    static bool monitor(const std::map<std::string, std::string>& options);
    
    // Export/import commands
    static bool export(const std::map<std::string, std::string>& options);
    static bool import(const std::map<std::string, std::string>& options);
    
    // Debug commands
    static bool debug(const std::map<std::string, std::string>& options);
    static bool info(const std::map<std::string, std::string>& options);
    static bool set(const std::map<std::string, std::string>& options);
    static bool get(const std::map<std::string, std::string>& options);
};
```

#### Advanced Command Set
```cpp
class AdvancedCommands {
public:
    // Experiment control commands
    static bool experiment(const std::map<std::string, std::string>& options);
    static bool simulation(const std::map<std::string, std::string>& options);
    static bool run_batch(const std::map<std::string, std::string>& options);
    static bool run_script(const std::map<std::string, std::string>& options);
    
    // Performance commands
    static bool performance(const std::map<std::string, std::string>& options);
    static bool benchmark_suite(const std::map<std::string, std::string>& options);
    static bool profile_session(const std::map<std::string, std::string>& options);
    static bool monitor_system(const std::map<std::string, std::string>& options);
    
    // Configuration management commands
    static bool config_validate(const std::map<std::string, std::string>& options);
    static bool config_apply(const std::map<std::string, std::string>& options);
    static bool config_reset(const std::map<std::string, std::string>& options);
    static bool config_export(const std::map<std::string, std::string>& options);
    static bool config_import(const std::map<std::string, std::string>& options);
    
    // Brain management commands
    static bool brain_init(const std::map<std::string, std::string>& options);
    static bool brain_step(const std::map<std::string, std::string>& options);
    static bool brain_reset(const std::map<std::string, std::string>& options);
    static bool brain_save(const std::map<std::string, std::string>& options);
    static bool brain_load(const std::map<std::string, std::string>& options);
    
    // World management commands
    static bool world_create(const std::map<std::string, std::string>& options);
    static bool world_update(const std::map<std::string, std::string>& options);
    static bool world_reset(const std::map<std::string, std::string>& options);
    static bool world_step(const std::map<std::string, std::string>& options);
    
    // Agent management commands
    static bool agent_create(const std::map<std::string, std::string>& options);
    static bool agent_init(const std::map<std::string, std::string>& options);
    static bool agent_step(const std::map<std::string, std::string>& options);
    static bool agent_reset(const std::map<std::string, std::string>& options);
};
```

### 3. Enhanced Command-Line Interface

#### Interactive Mode
```cpp
class InteractiveCLI {
public:
    enum InteractionMode {
        STANDARD,
        ADVANCED,
        SCRIPT,
        BATCH,
        DEBUG
    };
    
    struct CommandHistory {
        std::vector<std::string> commands;
        std::chrono::system_clock::time_point timestamp;
        std::string user;
        std::string session_id;
    };
    
    // Interactive interface
    static int startInteractive(InteractionMode mode = InteractionMode::STANDARD);
    static void stopInteractive();
    static bool isInteractive() const;
    static void setInteractionMode(InteractionMode mode);
    static InteractionMode getInteractionMode() const;
    
    // Command history
    static void addToHistory(const std::string& command);
    static std::vector<CommandHistory> getCommandHistory() const;
    static void clearCommandHistory();
    static void saveCommandHistory(const std::string& filepath);
    static void loadCommandHistory(const std::string& filepath);
    
    // Command completion
    static std::vector<std::string> completeCommand(const std::string& partial_command);
    static std::vector<std::string> getRecentCommands(size_t count = 10) const;
    
    // Context-aware help
    static std::vector<std::string> getContextAwareCommands(const std::string& context) const;
    static std::string getContextualHelp(const std::string& command_name) const;
    
    // Configuration
    static void configurePrompt(const std::string& prompt_format);
    static void setAutoComplete(bool enabled);
    static bool isAutoCompleteEnabled() const;
    static void setCommandHistorySize(size_t size);
    static size_t getCommandHistorySize() const;
    
    // Global interactive CLI access
    static InteractiveCLI& getInstance() {
        static InteractiveCLI instance;
        return instance;
    }
    
    static void initialize();
    static void shutdown();
    
    static InteractiveCLI& instance() { return getInstance(); }
};
```

#### Script Mode
```cpp
class ScriptCLI {
public:
    struct Script {
        std::string name;
        std::string description;
        std::vector<Command> commands;
        std::map<std::string, std::string> variables;
        bool requires_brain;
        bool requires_world;
        bool requires_agent;
    };
    
    // Script management
    static std::string createScript(const Script& script);
    static bool saveScript(const Script& script);
    static Script getScript(const std::string& name) const;
    static bool deleteScript(const std::string& name);
    static std::vector<Script> getAllScripts() const;
    
    // Script execution
    static bool runScript(const std::string& name,
                         const std::map<std::string, std::string>& variables = {});
    static bool runScriptFile(const std::string& filepath);
    static bool runScriptString(const std::string& script_string);
    static bool runBatchScript(const std::string& batch_file);
    
    // Script variables
    static bool setScriptVariable(const std::string& script_name,
                                 const std::string& variable_name,
                                 const std::string& value);
    static std::string getScriptVariable(const std::string& script_name,
                                        const std::string& variable_name) const;
    static std::map<std::string, std::string> getAllScriptVariables(
        const std::string& script_name) const;
    
    // Script completion
    static std::vector<std::string> completeScript(const std::string& partial_script);
    static std::vector<std::string> getRecentScripts(size_t count = 10) const;
    
    // Configuration
    static void setScriptDirectory(const std::string& directory);
    static std::string getScriptDirectory() const;
    static void setAutoLoadScripts(bool enabled);
    static bool isAutoLoadScriptsEnabled() const;
    
    // Global script CLI access
    static ScriptCLI& getInstance() {
        static ScriptCLI instance;
        return instance;
    }
    
    static void initialize();
    static void shutdown();
    
    static ScriptCLI& instance() { return getInstance(); }
};
```

### 4. Enhanced Command-Line Options

#### Common Command Options
```bash
# Common command options
--help, -h                    Show help for command
--version, -v                Show version information
--config, -c FILE            Configuration file
--brain FILE                 Load brain from file
--world FILE                 Load world from file
--agent FILE                 Load agent from file
--output, -o FILE            Output file
--input, -i FILE             Input file
--verbose, -v                 Verbose output
--quiet, -q                  Quiet output
--force, -f                  Force operation
--dry-run, --check            Check only, do not execute
--interactive, -I            Interactive mode
--batch, -b FILE             Batch mode
--script, -s FILE            Script mode
--daemon, --background        Run in background
--foreground                 Run in foreground
--log, --logging FILE         Enable logging
--log-level LEVEL            Set log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
--profile                    Enable profiling
--benchmark                  Run benchmark
--validate                  Validate configuration
--analyze                    Analyze results
--export                     Export results
--import                     Import data
--help                        Show help
--version                     Show version
```

#### Brain Management Options
```bash
# Brain management options
--brain-create NAME           Create new brain
--brain-load FILE             Load brain from file
--brain-save FILE             Save brain to file
--brain-reset                  Reset brain
--brain-init CONFIG           Initialize brain with configuration
--brain-step STEPS            Step brain N steps
--brain-sim TIME              Simulate brain for TIME seconds
--brain-stats                 Print brain statistics
--brain-info                  Print brain information
--brain-check                 Check brain integrity
--brain-validate              Validate brain
```

#### World Management Options
```bash
# World management options
--world-create CONFIG         Create new world
--world-load FILE             Load world from file

--world-save FILE             Save world to file
--world-reset                  Reset world
--world-update TIME            Update world for TIME seconds
--world-step STEPS            Step world N steps
--world-info                  Print world information
--world-check                 Check world integrity
--world-validate              Validate world
--world-set-AGENT POSITION     Set agent starting position
--world-set-ENVIRONMENT        Set world environment
--world-set-OBJECTS           Set world objects
```

#### Agent Management Options
```bash
# Agent management options
--agent-create NAME            Create new agent
--agent-load FILE              Load agent from file
--agent-init WORLD             Initialize agent with world
--agent-step                    Step agent
--agent-reset                   Reset agent
--agent-info                    Print agent information
--agent-check                  Check agent integrity
--agent-validate                Validate agent
--agent-set                    Set agent parameters
--agent-get                    Get agent parameters
--agent-action ACTION           Perform agent action
```

#### Configuration Options
```bash
# Configuration options
--config-create NAME            Create new configuration
--config-load FILE             Load configuration from file
--config-save FILE             Save configuration to file
--config-reset                  Reset configuration to defaults
--config-validate              Validate configuration
--config-apply FILE             Apply configuration
--config-merge FILE            Merge configuration with current
--config-diff                  Show configuration differences
--config-info                  Print configuration information
--config-check                 Check configuration integrity
```

#### Analysis Options
```bash
# Analysis options
--analyze-performance          Analyze performance
--analyze-behavior            Analyze behavior
--analyze-neuromodulation       Analyze neuromodulation
--analyze-plasticity         Analyze plasticity
--analyze-development         Analyze development
--analyze-memory              Analyze memory
--analyze-plans                Analyze planning
--profile-start NAME           Start profiling session
--profile-stop NAME            Stop profiling session
--profile-list                List profiling sessions
--profile-export NAME          Export profiling data
--profile-import NAME          Import profiling data
--benchmark-start NAME         Start benchmark suite
--benchmark-stop NAME          Stop benchmark suite
--benchmark-list              List benchmark suites
--benchmark-export NAME       Export benchmark results
--benchmark-import NAME        Import benchmark results
--monitor-start NAME          Start monitoring session
--monitor-stop NAME           Stop monitoring session
--monitor-list                List monitoring sessions
--monitor-export NAME          Export monitoring data
--monitor-import NAME          Import monitoring data
```

### 5. Command-Line Interface Examples

#### Basic Usage Examples
```bash
# Basic usage
nlm --help                    # Show help
nlm --version                  # Show version
nlm --config config.json       # Run with configuration
nlm --brain brain.bin         # Load brain from file
nlm --world world.bin         # Load world from file
nlm --agent agent.bin         # Load agent from file
nlm run                       # Run simulation
nlm step 100                  # Run 100 simulation steps
nlm help run                   # Show help for run command
nlm help config                # Show help for config command
nlm help brain                 # Show help for brain command
```

#### Advanced Usage Examples
```bash
# Advanced usage
nlm run --config config.json --brain brain.bin --world world.bin --agent agent.bin
nlm run --timestep 0.001 --steps 1000 --checkpoint-interval 100
nlm run --enable-curiosity --enable-novelty --enable-reward-modulation
nlm run --plot --output performance.png
nlm run --export --format json --output results.json
nlm run --analyze --benchmark --profile
nlm run --validate --export --format xml
nlm interactive                        # Enter interactive mode
nlm script myscript.ncl         # Run script
nlm batch experiments/            # Run batch of experiments
nlm help run                      # Show help for run command
nlm help config                    # Show help for config command
nlm set brain.neuron_count 2000   # Set configuration value
nlm get brain.neuron_count        # Get configuration value
nlm reset brain                   # Reset brain
nlm save brain.bin               # Save brain
nlm load brain.bin               # Load brain
```

#### Scripting Examples
```bash
# Script file (nlm.ncl)
script_name: "Multi-Region Brain Experiment"
script_type: "batch"
script_description: "Run multi-region brain experiment"

# Commands
commands:
  - name: "create_brain"
    type: "brain"
    args:
      - "--config"
      - "brain_config.json"
    options:
      --brain-name: "region_brain"
      --neuron-count: 1000
      --region-count: 2
  
  - name: "load_brain"
    type: "brain"
    args:
      - "--load"
      - "region_brain.bin"
  
  - name: "run_simulation"
    type: "simulation"
    args:
      - "--steps"
      - "1000"
      - "--timestep"
      - "0.001"
    options:
      --enable-curiosity: "true"
      --enable-novelty: "true"
      --enable-reward-modulation: "true"
  
  - name: "save_results"
    type: "brain"
    args:
      - "--save"
      - "final_brain.bin"
  
  - name: "analyze_results"
    type: "analysis"
    args:
      - "--benchmark"
    options:
      --output-format: "json"
      --output-file: "benchmark_results.json"
```

```bash
# Batch file execution
#!/bin/bash
# batch_experiments.sh

# Run multiple experiments
for brain_type in "small" "medium" "large"; do
    for plasticity in "low" "medium" "high"; do
        for neuromodulation in "none" "basic" "advanced"; do
            echo "Running experiment: $brain_type $plasticity $neuromodulation"
            
            # Create configuration
            python3 create_config.py \
                --brain-type "$brain_type" \
                --plasticity "$plasticity" \
                --neuromodulation "$neuromodulation" \
                --output "config_${brain_type}_${plasticity}_${neuromodulation}.json"
            
            # Run experiment
            nlm run \
                --config "config_${brain_type}_${plasticity}_${neuromodulation}.json" \
                --benchmark \
                --profile \
                --analyze
            
            # Collect results
            mv results.json "results_${brain_type}_${plasticity}_${neuromodulation}.json"
            
            echo "Completed experiment: $brain_type $plasticity $neuromodulation"
        done
    done
    done
```

### 6. Command-Line Interface Configuration

#### CLI Configuration Schema
```json
{
    "cli": {
        "global": {
            "default_config_file": "nlm_config.json",
            "default_brain_file": "brain.bin",
            "default_world_file": "world.bin",
            "default_agent_file": "agent.bin",
            "default_output_directory": "./outputs",
            "default_log_directory": "./logs",
            "default_script_directory": "./scripts",
            "default_batch_directory": "./batches",
            "show_help_on_error": true,
            "show_version": true,
            "version_string": "1.0.0",
            "help_indent": 2,
            "help_max_width": 80,
            "command_history_size": 1000,
            "auto_complete_enabled": true,
            "auto_load_scripts": true,
            "interactive_mode_default": "standard",
            "script_mode_default": "standard",
            "batch_mode_default": "standard",
            "daemon_mode_default": false,
            "foreground_mode_default": true
        },
        
        "commands": {
            "run": {
                "description": "Run simulation",
                "help_text": "Run NLM simulation with specified configuration",
                "requires_config": true,
                "requires_brain": false,
                "requires_world": false,
                "requires_agent": false,
                "is_dangerous": false,
                "options": [
                    "--timestep",
                    "--steps",
                    "--duration",
                    "--enable-curiosity",
                    "--enable-novelty",
                    "--enable-reward-modulation",
                    "--enable-development",
                    "--enable-plasticity",
                    "--enable-neuromodulation",
                    "--plot",
                    "--analyze",
                    "--benchmark",
                    "--profile",
                    "--export",
                    "--format"
                ]
            },
            "config": {
                "description": "Manage configuration",
                "help_text": "Manage NLM configuration",
                "requires_config": false,
                "requires_brain": false,
                "requires_world": false,
                "requires_agent": false,
                "is_dangerous": false,
                "options": [
                    "--create",
                    "--load",
                    "--save",
                    "--reset",
                    "--validate",
                    "--apply",
                    "--merge",
                    "--diff",
                    "--info",
                    "--check"
                ]
            },
            "brain": {
                "description": "Manage brain",
                "help_text": "Manage NLM brain",
                "requires_config": false,
                "requires_brain": false,
                "requires_world": false,
                "requires_agent": false,
                "is_dangerous": false,
                "options": [
                    "--create",
                    "--load",
                    "--save",
                    "--reset",
                    "--init",
                    "--step",
                    "--sim",
                    "--stats",
                    "--info",
                    "--check",
                    "--validate"
                ]
            },
            "world": {
                "description": "Manage world",
                "help_text": "Manage NLM world",
                "requires_config": false,
                "requires_brain": false,
                "requires_world": false,
                "requires_agent": false,
                "is_dangerous": false,
                "options": [
                    "--create",
                    "--load",
                    "--save",
                    "--reset",
                    "--update",
                    "--step",
                    "--info",
                    "--check",
                    "--validate",
                    "--set-agent",
                    "--set-environment",
                    "--set-objects"
                ]
            },
            "agent": {
                "description": "Manage agent",
                "help_text": "Manage NLM agent",
                "requires_config": false,
                "requires_brain": false,
                "requires_world": false,
                "requires_agent": false,
                "is_dangerous": false,
                "options": [
                    "--create",
                    "--load",
                    "--init",
                    "--step",
                    "--reset",
                    "--info",
                    "--check",
                    "--validate",
                    "--set",
                    "--get",
                    "--action"
                ]
            },
            "analyze": {
                "description": "Analyze results",
                "help_text": "Analyze NLM simulation results",
                "requires_config": true,
                "requires_brain": true,
                "requires_world": false,
                "requires_agent": false,
                "is_dangerous": false,
                "options": [
                    "--benchmark",
                    "--profile",
                    "--plot",
                    "--export",
                    "--format",
                    "--output"
                ]
            },
            "profile": {
                "description": "Profile performance",
                "help_text": "Profile NLM performance",
                "requires_config": true,
                "requires_brain": true,
                "requires_world": false,
                "requires_agent": false,
                "is_dangerous": false,
                "options": [
                    "--start",
                    "--stop",
                    "--list",
                    "--export",
                    "--import",
                    "--analyze"
                ]
            },
            "benchmark": {
                "description": "Run benchmark",
                "help_text": "Run NLM benchmark",
                "requires_config": true,
                "requires_brain": true,
                "requires_world": false,
                "requires_agent": false,
                "is_dangerous": false,
                "options": [
                    "--start",
                    "--stop",
                    "--list",
                    "--export",
                    "--import",
                    "--analyze"
                ]
            },
            "monitor": {
                "description": "Monitor performance",
                "help_text": "Monitor NLM performance",
                "requires_config": true,
                "requires_brain": true,
                "requires_world": false,
                "requires_agent": false,
                "is_dangerous": false,
                "options": [
                    "--start",
                    "--stop",
                    "--list",
                    "--export",
                    "--import"
                ]
            },
            "interactive": {
                "description": "Enter interactive mode",
                "help_text": "Enter NLM interactive mode",
                "requires_config": false,
                "requires_brain": false,
                "requires_world": false,
                "requires_agent": false,
                "is_dangerous": false,
                "options": [
                    "--mode",
                    "--help"
                ]
            },
            "script": {
                "description": "Run script",
                "help_text": "Run NLM script",
                "requires_config": false,
                "requires_brain": false,
                "requires_world": false,
                "requires_agent": false,
                "is_dangerous": false,
                "options": [
                    "--mode",
                    "--variables"
                ]
            },
            "batch": {
                "description": "Run batch",
                "help_text": "Run NLM batch",
                "requires_config": false,
                "requires_brain": false,
                "requires_world": false,
                "requires_agent": false,
                "is_dangerous": false,
                "options": [
                    "--mode",
                    "--variables"
                ]
            }
        }
    }
}
```

### 7. Command-Line Interface Integration

#### Integration with Core Systems
```cpp
// CLI integration with core systems
class CLIIntegration {
private:
    std::shared_ptr<NLMCLI> cli_;
    std::shared_ptr<ArgumentParser> parser_;
    std::shared_ptr<InteractiveCLI> interactive_cli_;
    std::shared_ptr<ScriptCLI> script_cli_;
    std::shared_ptr<BasicCommands> basic_commands_;
    std::shared_ptr<AdvancedCommands> advanced_commands_;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    
    bool initialized_;
    
public:
    CLIIntegration();
    ~CLIIntegration();
    
    // Integration interface
    bool initialize(const std::string& config_file = "");
    bool shutdown();
    
    // Core system integration
    bool integrateWithBrain(std::shared_ptr<Brain> brain);
    bool integrateWithWorld(std::shared_ptr<SimpleWorld> world);
    bool integrateWithAgent(std::shared_ptr<AgentBrain> agent);
    
    // CLI integration
    bool integrateWithCLI(std::shared_ptr<NLMCLI> cli);
    bool integrateWithParser(std::shared_ptr<ArgumentParser> parser);
    bool integrateWithInteractiveCLI(std::shared_ptr<InteractiveCLI> interactive_cli);
    bool integrateWithScriptCLI(std::shared_ptr<ScriptCLI> script_cli);
    
    // Command integration
    bool registerCommands();
    bool registerBasicCommands(std::shared_ptr<BasicCommands> basic_commands);
    bool registerAdvancedCommands(std::shared_ptr<AdvancedCommands> advanced_commands);
    
    // CLI execution
    int executeCommandLine(int argc, char** argv);
    int executeInteractiveMode();
    int executeScriptMode(const std::string& script_file);
    int executeBatchMode(const std::string& batch_file);
    
    // State management
    bool saveState(const std::string& state_file);
    bool loadState(const std::string& state_file);
    bool resetState();
    
    // Configuration management
    bool configureFromConfig(const Config& config);
    bool configureFromCommandLine(int argc, char** argv);
    
    // Global CLI integration access
    static CLIIntegration& getInstance() {
        static CLIIntegration instance;
        return instance;
    }
    
    static void initialize(const std::string& config_file = "");
    static void shutdown();
    
    static CLIIntegration& instance() { return getInstance(); }
};
```

### 8. Command-Line Interface Testing

#### CLI Testing Framework
```cpp
class CLITestFramework {
public:
    struct TestCase {
        std::string name;
        std::string description;
        std::vector<std::string> command_line_args;
        std::map<std::string, std::string> expected_options;
        std::vector<std::string> expected_arguments;
        int expected_exit_code;
        std::function<bool(const std::string& output)> validation_function;
    };
    
    // Test management
    static void addTestCase(const TestCase& test_case);
    static void removeTestCase(const std::string& name);
    static void clearTestCases();
    static std::vector<TestCase> getTestCases() const;
    static TestCase getTestCase(const std::string& name) const;
    
    // Test execution
    static bool runTestCase(const TestCase& test_case);
    static bool runTestCase(const std::string& name);
    static std::map<std::string, bool> runAllTestCases();
    static std::map<std::string, std::string> runAllTestCasesWithResults();
    
    // Test configuration
    static void setTestEnvironment(const std::string& config_file);
    static void setTestBrain(const std::string& brain_file);
    static void setTestWorld(const std::string& world_file);
    static void setTestAgent(const std::string& agent_file);
    
    // Test results
    static std::vector<std::string> getPassedTestCases() const;
    static std::vector<std::string> getFailedTestCases() const;
    static std::vector<std::string> getSkippedTestCases() const;
    static std::map<std::string, std::string> getTestResults() const;
    static std::string getTestStatistics() const;
    
    // Test reporting
    static void generateTestReport(const std::string& filepath);
    static void generateTestSummaryReport(const std::string& filepath);
    static std::string getTestSummary() const;
    
    // CLI testing
    static bool testCommandLineParsing();
    static bool testCommandLineExecution();
    static bool testInteractiveMode();
    static bool testScriptMode();
    static bool testBatchMode();
    static bool testHelpSystem();
    static bool testErrorHandling();
    
    // Global CLI test framework access
    static CLITestFramework& getInstance() {
        static CLITestFramework instance;
        return instance;
    }
    
    static void initialize();
    static void shutdown();
    
    static CLITestFramework& instance() { return getInstance(); }
};
```

### 9. Command-Line Interface Usage Examples

#### Complete CLI Usage Example
```bash
#!/bin/bash
# comprehensive_cli_example.sh

# Set error handling
set -e

# Configuration
CONFIG_FILE="config.json"
BRAIN_FILE="brain.bin"
WORLD_FILE="world.bin"
AGENT_FILE="agent.bin"
OUTPUT_DIR="./outputs"
LOG_DIR="./logs"

# Create output and log directories
mkdir -p "$OUTPUT_DIR"
mkdir -p "$LOG_DIR"

# Step 1: Create configuration
echo "Step 1: Creating configuration..."
python3 create_config.py \
    --brain-type "advanced" \
    --neuron-count 2000 \
    --region-count 3 \
    --plasticity "high" \
    --neuromodulation "advanced" \
    --development "enabled" \
    --output "$CONFIG_FILE"

# Step 2: Create brain
echo "Step 2: Creating brain..."
nlm brain --create \
    --config "$CONFIG_FILE" \
    --brain-name "advanced_brain" \
    --neuron-count 2000 \
    --region-count 3

# Step 3: Initialize brain
echo "Step 3: Initializing brain..."
nlm brain --init \
    --brain "advanced_brain.bin"

# Step 4: Create world
echo "Step 4: Creating world..."
nlm world --create \
    --width 20 \
    --height 20 \
    --vision-width 8 \
    --vision-height 8 \
    --world-name "simulation_world"

# Step 5: Initialize world
echo "Step 5: Initializing world..."
nlm world --init \
    --world "simulation_world.bin"

# Step 6: Create agent
echo "Step 6: Creating agent..."
nlm agent --create \
    --brain "advanced_brain.bin" \
    --world "simulation_world.bin" \
    --agent-name "learning_agent"

# Step 7: Initialize agent
echo "Step 7: Initializing agent..."
nlm agent --init \
    --agent "learning_agent.bin" \
    --enable-curiosity \
    --enable-novelty \
    --enable-reward-modulation \
    --enable-development

# Step 8: Run simulation with profiling
echo "Step 8: Running simulation with profiling..."
nlm run \
    --config "$CONFIG_FILE" \
    --brain "advanced_brain.bin" \
    --world "simulation_world.bin" \
    --agent "learning_agent.bin" \
    --steps 1000 \
    --timestep 0.001 \
    --enable-curiosity \
    --enable-novelty \
    --enable-reward-modulation \
    --enable-development \
    --profile \
    --benchmark \
    --analyze \
    --plot \
    --export \
    --format "json" \
    --output "$OUTPUT_DIR/results.json"

# Step 9: Analyze results
echo "Step 9: Analyzing results..."
nlm analyze \
    --benchmark \
    --profile \
    --plot \
    --export \
    --format "html" \
    --output "$OUTPUT_DIR/analysis.html" \
    --input "$OUTPUT_DIR/results.json"

# Step 10: Export results
echo "Step 10: Exporting results..."
nlm export \
    --format "xml" \
    --output "$OUTPUT_DIR/results.xml" \
    --input "$OUTPUT_DIR/results.json"

# Step 11: Create script
echo "Step 11: Creating script..."
cat > experiment_script.ncl << EOF
script_name: "Multi-Region Learning Experiment"
script_type: "batch"
script_description: "Advanced multi-region learning experiment"

commands:
  - name: "create_brain"
    type: "brain"
    args:
      - "--config"
      - "config.json"
    options:
      --brain-name: "region_brain"
      --neuron-count: 1000
      --region-count: 2
  
  - name: "run_simulation"
    type: "simulation"
    args:
      - "--steps"
      - "1000"
      - "--timestep"
      - "0.001"
    options:
      --enable-curiosity: "true"
      --enable-novelty: "true"
      --enable-reward-modulation: "true"
  
  - name: "save_results"
    type: "brain"
    args:
      - "--save"
      - "final_brain.bin"
  
  - name: "analyze_results"
    type: "analysis"
    args:
      - "--benchmark"
    options:
      --output-format: "json"
      --output-file: "benchmark_results.json"
EOF

# Step 12: Run script
echo "Step 12: Running script..."
nlm script experiment_script.ncl

# Step 13: Interactive analysis
echo "Step 13: Interactive analysis..."
nlm interactive --mode "advanced"

# Step 14: Batch processing
echo "Step 14: Batch processing..."
nlm batch experiments/

# Step 15: Generate final report
echo "Step 15: Generating final report..."
nlm analyze \
    --benchmark \
    --profile \
    --plot \
    --export \
    --format "html" \
    --output "$OUTPUT_DIR/final_report.html" \
    --input "$OUTPUT_DIR/results.json"

echo "Experiment completed successfully!"
```

## Summary

This document provides comprehensive command-line interface (CLI) enhancements for the NLM system, including:

1. **Enhanced CLI Framework**: Modern CLI architecture with command registration, parsing, and execution
2. **Argument Parser**: Sophisticated argument parser with validation, completion, and help systems
3. **Command Implementation**: Comprehensive command set including basic and advanced commands
4. **Interactive Mode**: Rich interactive CLI with command history, completion, and contextual help
5. **Script Mode**: Flexible script execution with variable substitution and command execution
6. **Batch Mode**: Batch processing capabilities for running multiple experiments
7. **Enhanced Command-Line Options**: Extensive command-line options for all major components
8. **Integration with Core Systems**: Seamless integration with brain, world, agent, and configuration systems
9. **Testing Framework**: Comprehensive testing framework for CLI functionality
10. **Usage Examples**: Complete usage examples and documentation

These CLI enhancements provide:

- **Intuitive Command Line Interface**: User-friendly CLI with comprehensive help and completion
- **Flexible Command Execution**: Support for interactive, script, and batch execution modes
- **Advanced Command Options**: Extensive command options for all major NLM components
- **Integration with Core Systems**: Seamless integration with NLM's core systems
- **Comprehensive Testing**: Robust testing framework for CLI functionality
- **Performance Monitoring**: Performance monitoring and profiling capabilities
- **Error Handling**: Comprehensive error handling and recovery mechanisms
- **Documentation**: Extensive documentation and usage examples

The enhanced CLI system provides a powerful, flexible, and user-friendly interface for NLM users, supporting both basic and advanced usage scenarios while maintaining ease of use and accessibility.

This comprehensive CLI enhancement significantly improves the NLM system's usability, making it more accessible to both beginners and advanced users while maintaining powerful functionality for complex scenarios.
