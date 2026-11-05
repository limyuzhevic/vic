# NLM Command-Line Interface Enhancement Analysis

## 1. Feature List

### 1.1 Configuration Management Commands

**nlm config list**
- Purpose: List all available configuration options with descriptions
- Usage: `nlm config list [--detailed]`
- Description: Displays all configurable keys, their current values, and descriptions

**nlm config get <key>**
- Purpose: Get specific configuration value
- Usage: `nlm config get random_seed`
- Description: Retrieves current value of specified configuration key

**nlm config set <key> <value>**
- Purpose: Set configuration value
- Usage: `nlm config set neuron_count 800`
- Description: Sets configuration key with specified value

**nlm config validate**
- Purpose: Validate current configuration
- Usage: `nlm config validate`
- Description: Checks all configuration values for validity

**nlm config export <format>**
- Purpose: Export configuration in specified format
- Usage: `nlm config export json > config.json`
- Description: Exports current configuration to file (JSON, YAML, etc.)

### 1.2 Brain Management Commands

**nlm brain info**
- Purpose: Display brain statistics and metadata
- Usage: `nlm brain info [--detailed]`
- Description: Shows neuron/synapse counts, regions, developmental stage

**nlm brain reset**
- Purpose: Reset brain to initial state
- Usage: `nlm brain reset [--preserve-config]`
- Description: Resets neural state while preserving configuration

**nlm brain save <path>**
- Purpose: Save brain state to checkpoint
- Usage: `nlm brain save brain_state.json`
- Description: Creates persistent brain checkpoint

**nlm brain load <path>**
- Purpose: Load brain state from checkpoint
- Usage: `nml brain load brain_state.json`
- Description: Restores brain from saved checkpoint

**nlm brain compare <path1> <path2>**
- Purpose: Compare two brain states
- Usage: `nlm brain compare state1.json state2.json`
- Description: Compares metrics between two brain checkpoints

### 1. Measuring Experiment Capabilities

**nlm experiment list**
- Purpose: List available experiments
- Usage: `nlm experiment list [--all]`
- Description: Shows all configured experiments with metadata

**nlm experiment run <name> [config]**
- Purpose: Run specific experiment
- Usage: `nlm experiment run exploration --neuron_count 1000`
- Description: Executes specified experiment with optional config overrides

**nlm experiment metrics <path>**
- Purpose: Analyze experiment metrics
- Usage: `nlm experiment metrics results.json --statistical`
- Description: Analyzes experiment data and generates statistics

**nlm experiment visualize <path>**
- Purpose: Generate visualizations
- Usage: `nlm experiment visualize results.json --output plot.png`
- Description: Creates visual representations of experiment data

### 1.3 System Control Commands

**nlm system status**
- Purpose: Display system status and resource usage
- Usage: `nlm system status [--verbose]`
- Description: Shows CPU, memory, neural network status

**nlm system monitor <duration>**
- Purpose: Monitor system performance
- Usage: `nlm system monitor 60 --interval 5`
- Description: Monitors system metrics over specified duration

**nlm system logs <level>**
- Purpose: Display system logs
- Usage: `nlm system logs info --tail 100`
- Description: Shows log messages at specified level

**nlm system profiling <mode>**
- Purpose: Manage profiling mode
- Usage: `nlm system profiling start --mode timeline`
- Description: Controls profiling data collection

### 1.4 Debug and Analysis Commands

**nlm debug neurons <region> [count]**
- Purpose: Analyze neuron distribution
- Usage: `nlm debug neurons all --statistics --top 10`
- Description: Analyzes neuron properties and distribution patterns

**nlm debug synapses <count>**
- Purpose: Analyze synaptic connections
- Usage: `nlm debug synapses 10000 --types all --plasticity enabled`
- Description: Examines synaptic properties and connectivity

**nlm debug plasticity <steps>**
- Purpose: Analyze plasticity changes
- Usage: `nlm debug plasticity 1000 --rule stdp`
- Description: Analyzes learning dynamics and weight changes

**nlm debug memory <type>**
- Purpose: Analyze memory system status
- Usage: `nlm debug memory working --traces 50`
- Description: Analyzes memory system contents and behavior

### 1. Advanced Commands for Experiment Controllers

**nlm advanced replay <path> <duration>**
- Purpose: Replay recorded brain activity
- Usage: `nlm advanced replay recorded_activity.brain 10.0`
- Description: Replays saved neural activity patterns

**nlm advanced pattern <input>**
- Purpose: Test pattern recognition
- Usage: `nlm advanced pattern "visual_pattern" --neurons 500`
- Description: Tests brain's ability to recognize patterns

**nlm advanced learning <iterations>**
- Purpose: Test learning capabilities
- Usage: `nlm advanced learning 100 --type stdp`
- Description: Tests learning dynamics over specified iterations

**nlm advanced transfer <target>**
- Purpose: Test transfer learning
- Usage: `nlm advanced transfer "motor_control" --from exploration`
- Description: Tests ability to transfer learning to new tasks

### 1 Batch Processing Commands

**nlm batch run <experiment_list> <config>**
- Purpose: Run multiple experiments
- Usage: `nlm batch run experiments.txt config.json --parallel 4`
- Description: Executes multiple experiments in sequence or parallel

**nlm batch parallel <count>**
- Purpose: Run experiments in parallel
- Usage: `nlm batch parallel 8`
- Description: Sets up parallel execution environment

**nlm batch results <directory>**
- Purpose: Collect and aggregate results
- Usage: `nlm batch results ./experiments/2026-11-05 --format json`
- Description: Aggregates results from multiple experiment runs

## 2. Command Architecture Design

### 2.1 File Structure

```
/src
  /cli                    # Core CLI framework
    /commands            # Command implementations
      /config            # Configuration management
      /brain             # Brain operations
      /experiment        # Experiment management
      /system            # System controls
      /debug             # Debug and analysis
      /advanced          # Advanced operations
      /batch             # Batch processing
    /framework            # Common infrastructure
    /parser              # Argument parsing
    /help                # Help and completion
  /main                 # Entry point (nlm executable)
  /legacy                # Backwards compatibility (existing demos)
```

### 2.2 Command Architecture

#### 2.2.1 Main Executable
- **Entry point**: `src/main.cpp` -> `nlm`
- **Architecture**: Subcommand-based with unified interface
- **Integration**: Maintains backward compatibility with existing demos

#### 2.2.2 Command Framework
```
Command Base Classes:
├── Command (abstract base)
├── ConfigCommand (for config operations)
├── BrainCommand (for brain operations)
├── SystemCommand (for system operations)
└── AnalysisCommand (for debug/analysis)

Command Structure:
├── CommandFactory (creates command instances)
├── CommandRegistry (manages available commands)
├── CommandParser (parses command line arguments)
└── CommandExecutor (executes commands)
```

#### 2.2.3 Integration Points

**2.2.3.1 Configuration System Integration**
- All commands use shared Config instance
- Commands can read/modify configuration
- Configuration validation and persistence

**2.2.3.2 Brain System Integration**
- Commands operate on running brain instances
- State management (save/load/reset)
- Real-time monitoring and control

**2.2.3.3 Experiment System Integration**
- Experiment discovery and execution
- Results collection and analysis
- Experiment lifecycle management

**2.2.3.4 Logging System Integration**
- Unified logging across all commands
- Progress reporting
- Error handling and reporting

## 3. Implementation Plan

### 3.1 Phase 1: Foundation (Week 1-2)

**3.1.1 Core Infrastructure**
- Create unified CLI framework in `src/cli/framework`
- Implement command base classes and factory
- Set up argument parsing and help system
- Create command registry and execution engine

**3.1.2 Configuration Command Framework**
- Implement config list, get, set, validate, export commands
- Integrate with existing Config class
- Add configuration validation rules

**3.1.3 Brain Command Framework**
- Implement brain info, reset, save, load, compare commands
- Integrate with Brain class methods
- Add brain state monitoring capabilities

### 3.2 Phase 2: Core Commands (Week 3-4)

**3.2.1 Experiment Management**
- Implement experiment list, run, metrics, visualize commands
- Integrate with experiment system
- Add experiment configuration management

**3.2.2 System Control**
- Implement system status, monitor, logs, profiling commands
- Add system resource monitoring
- Integrate logging system

**3.2.3 Debug and Analysis**
- Implement debug neurons, synapses, plasticity, memory commands
- Add analysis capabilities for brain state
- Integrate with brain statistics systems

### 3.3 Phase 3: Advanced Features (Week 5-6)

**3.3.1 Advanced Operations**
- Implement advanced replay, pattern, learning, transfer commands
- Add sophisticated testing and validation capabilities
- Integrate with brain's advanced features

**3.3.2 Batch Processing**
- Implement batch run, parallel, results commands
- Add experiment batch management
- Integrate with experiment runner

### 3.4 Phase 4: Integration and Refinement (Week 7-8)

**3.4.1 Final Integration**
- Integrate all commands with existing systems
- Add command line completion support
- Implement comprehensive help system

**3.4.2 Testing and Validation**
- Test all commands with existing demos
- Validate command integration with current systems
- Performance optimization for large-scale operations

**3.4.3 Documentation and Examples**
- Create comprehensive command documentation
- Add usage examples for all commands
- Create reference guides and cheat sheets

## 4. Sample Command Line Implementations

### 4.1 Configuration Management

```bash
# List all configuration options
nlm config list

# Get specific value
nlm config get neuron_count

# Set configuration value
nlm config set neuron_count 1000

# Validate configuration
nlm config validate

# Export configuration to JSON
nlm config export json > brain_config.json

# Export configuration to YAML
nlm config export yaml > brain_config.yaml
```

### 4.2 Brain Operations

```bash
# Display brain information
nlm brain info

# Display detailed brain status
nlm brain info --detailed

# Reset brain while preserving config
nlm brain reset --preserve-config

# Save brain state to file
nlm brain save checkpoint_2025_11_05.json

# Load brain state from file
nlm brain load checkpoint_2025_11_05.json

# Compare two brain states
nlm brain compare brain1.json brain2.json
```

### 4.93 Experiment Management

```bash
# List available experiments
nlm experiment list

# List all experiments including internal ones
nlm experiment list --all

# Run exploration experiment with custom config
nlm experiment run exploration --neuron_count 800 --plasticity_learning_rate 0.02

# Run learning experiment with predefined config
nlm experiment run learning --config experiments/learning_config.json

# Analyze experiment metrics
nlm experiment metrics experiment_results.json --statistical --format summary

# Generate visualization from experiment data
nlm experiment visualize experiment_results.json --output performance_plot.png --type heatmap
```

### 4.4 System Control

```bash
# Get system status
nlm system status

# Get verbose system status
nlm system status --verbose

# Monitor system for 60 seconds
nlm system monitor 60 --interval 5

# Show recent logs
nlm system logs info --tail 100

# Start timeline profiling
nlm system profiling start --mode timeline

# Stop profiling
nlm system profiling stop
```

### 4.5 Debug and Analysis

```bash
# Analyze neuron distribution
nlm debug neurons all --statistics --top 10

# Analyze synaptic connections (top 1000 by strength)
nlm debug synapses 1000 --types excitatory, inhibitory --plasticity enabled

# Analyze STDP plasticity changes
nlm debug plasticity 1000 --rule stdp --window 10

# Analyze working memory contents
nlm debug memory working --traces 50 --format json

# Analyze episodic memory entries
nlm debug memory episodic --count 100 --age "> 1000"
```

### 4.6 Advanced Operations

```bash
# Replay recorded brain activity
nlm advanced replay recorded_activity.brain 10.0

# Test pattern recognition with visual pattern
nlm advanced pattern "visual_pattern" --neurons 500 --similarity_threshold 0.8

# Test STDP learning capabilities
nlm advanced learning 100 --type stdp --monitor_convergence

# Test transfer learning from exploration to motor_control
nlm advanced transfer "motor_control" --from "exploration" --adaptation_rate 0.1
```

### 4.7 Batch Processing

```bash
# Create experiment list file (experiments.txt)
cat > experiments.txt << EOF
exploration --config exp1.json
learning --config exp2.json
transfer --config exp3.json
EOF

# Run all experiments sequentially
nlm batch run experiments.txt --config default.json

# Run experiments in parallel (up to 8)
nlm batch parallel 8

# Collect results from multiple experiment directories
nlm batch results ./experiments/2025-11-* --format json --aggregate performance_metrics
```

## 5. Integration Strategy

### 5.1 Configuration System Integration

**5.1.1 Current Configuration System**
- `src/core/Config/Config.cpp/hpp`: Key=value parsing, file loading
- `--config=filename`, `--key=value`, `-key value` formats
- Simple file-based configuration (currently placeholder for JSON/YAML)

**5.1.2 Integration Approach**
- All new commands share same Config instance
- Commands can read/write configuration values
- Configuration validation and persistence maintained
- Backward compatibility with existing argument parsing

**5.1.3 Implementation Details**
```cpp
// Config command integration
class ConfigCommand : public Command {
public:
    void execute(const CommandContext& ctx) override {
        if (subcommand == "list") {
            // Display all config entries
            auto keys = config->getKeys();
            for (const auto& key : keys) {
                auto value = config->getValue(key);
                std::cout << key << " = " << value << std::endl;
            }
        }
        // ... other subcommands
    }
};
```

### 5.2 Brain System Integration

**5.2.1 Current Brain System**
- `src/brain/Brain.hpp/cpp`: Core brain implementation
- `Brain::initialize()`, `Brain::step()`, `Brain::reset()`
- `Brain::save()`, `Brain::load()`: Checkpoint functionality
- Multiple component systems (memory, neuromodulation, etc.)

**5.2.2 Integration Approach**
- Commands operate on running brain instances
- State management through existing methods
- Real-time monitoring and control
- Integration with existing demo executables

**5.2.3 Implementation Details**
```cpp
// Brain command integration
class BrainCommand : public Command {
public:
    void execute(const CommandContext& ctx) override {
        if (subcommand == "info") {
            brain->logStatus();
            // Display additional metrics
            std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
            std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
        }
        // ... other subcommands
    }
};
```

### 5.3 Experiment System Integration

**5.3.1 Current Experiment System**
- `src/experiments/Phase3Demo.cpp`: Phase 3 world interaction demo
- `src/experiments/Phase4Demo.cpp`: Phase 4 cognition demo
- `src/experiments/Phase6Demo.cpp`: Phase 6 integration test
- Multiple experiment libraries (nlm_phase3, nlm_phase4, nlm_phase6)

**5.3.2 Integration Approach**
- Unified experiment management interface
- Integration with existing experiment runners
- Results collection and analysis
- Experiment lifecycle management

**5.3.3 Implementation Details**
```cpp
// Experiment command integration
class ExperimentCommand : public Command {
public:
    void execute(const CommandContext& ctx) override {
        if (subcommand == "list") {
            // Discover available experiments
            auto experiments = discoverExperiments();
            for (const auto& exp : experiments) {
                std::cout << exp.name << ": " << exp.description << std::endl;
            }
        }
        // ... other subcommands
    }
};
```

### 5.4 Logging System Integration

**5.4.1 Current Logging System**
- `src/core/Logger/Logger.hpp/cpp`: Core logging infrastructure
- Multiple log levels (Trace, Debug, Info, Warning, Error)
- Console and file logging

**5.4.2 Integration Approach**
- Unified logging across all commands
- Progress reporting for long-running operations
- Error handling and reporting
- Command-specific log levels

**5.4.3 Implementation Details**
```cpp
// Logging integration
class CommandWithLogging : public Command {
protected:
    void logProgress(const std::string& message) {
        NLM_LOG_INFO("[" + commandName + "] " + message);
    }
    
    void logError(const std::string& message) {
        NLM_LOG_ERROR("[" + commandName + "] " + message);
    }
};
```

### 5.5 Backward Compatibility

**5.5.1 Existing Demo Executables**
- `./nlm` (Phase 2): Main demo with basic neural computation
- `./nlm_phase3_demo`: Phase 3 world interaction demo
- `./nlm_phase4_demo`: Phase 4 cognition demo
- `./nlm_phase6_demo`: Phase 6 integration test
- `./nlm_benchmark`: Performance benchmark

**5.5.2 Compatibility Strategy**
- Maintain existing executables unchanged
- Unified CLI as primary interface
- Existing demos can be accessed through CLI wrapper
- Command-line arguments are passed to underlying demos

**5.5.5.3 Implementation Details**
```cpp
// Main CLI entry point
int main(int argc, char* argv[]) {
    // Parse arguments
    auto result = commandParser.parse(argc, argv);
    
    if (result.isLegacyDemo()) {
        // Launch legacy demo executable
        return launchLegacyDemo(result.demoName, result.args);
    }
    
    // Execute new command
    return commandExecutor.execute(result.command);
}
```

## 6. Summary

This analysis provides a comprehensive plan for implementing advanced CLI commands for the NLM project. The unified CLI architecture will:

1. **Enhance Usability**: Provide intuitive command structure with comprehensive help
2. **Improve Control**: Enable fine-grained control over brain, experiments, and system
3. **Enable Analysis**: Provide powerful debugging and analysis capabilities
4. **Support Advanced Usage**: Offer advanced features for research and experimentation
5. **Maintain Compatibility**: Preserve existing demo executables and functionality

The implementation will integrate seamlessly with existing configuration, brain, and experiment systems while providing a user-friendly interface for controlling and analyzing the NLM artificial brain system.