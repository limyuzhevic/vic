# Phase 6: Advanced CLI Implementation

This document provides an overview of the advanced command-line interface implemented for the NLM (Neural Learning Machine) system in Phase 6.

## Overview

The NLM Advanced CLI system provides comprehensive command-line access to all brain management, simulation control, inspection, analysis, debugging, export/import, and advanced feature commands. It maintains full backward compatibility with the original CLI while adding 60+ advanced commands organized into 7 command groups.

## Command Structure

The advanced CLI is organized into 7 command groups:

### 1. Brain Management Commands
Commands for loading, saving, and managing brain states and configurations.

### 2. Simulation Control Commands
Commands for controlling simulation execution, timing, and performance.

### 3. Real-time Inspection Commands
Commands for inspecting brain components, memory systems, and neuromodulation.

### 4. Analysis Commands
Commands for analyzing neural activity, plasticity, memory usage, and performance.

### 5. Debugging Commands
Commands for debugging spike events, plasticity history, and system consistency.

### 6. Export/Import Commands
Commands for exporting and importing brain data in various formats.

### 7. Advanced Features
Commands for experiments, parameter analysis, and learning mode control.

## Implementation Details

### Core Infrastructure

The advanced CLI implementation consists of several key components:

#### 1. AdvancedCommandLineParser
- Enhanced argument parser that handles all 60+ commands
- Command group separation and validation
- Integration with existing configuration system

#### 2. CommandExecutionFramework
- Command execution engine with history and completion support
- Error handling and help system
- Command registration and discovery

#### 3. CommandOutputFormatter
- Rich output formatting for all command types
- Progress bars, progress indicators, and formatted data display
- Consistency across all command outputs

#### 4. Command-Specific Handlers
- Separate handler classes for each command group
- Modular design for easy extension and maintenance
- Placeholder implementations for all commands

### Command Groups

#### Brain Management Commands
```
--brain-config <file>          Load brain configuration
--brain-save <file>            Save brain state to file
--brain-load <file>            Load brain state from file
--brain-info                   Display detailed brain information
--brain-reset                  Reset brain to initial state
--brain-dump <file>            Dump brain state in binary format
--brain-diff <file1> <file2>   Compare two brain states
--brain-status                 Display current brain status
--brain-validate               Validate brain state consistency
```

#### Simulation Control Commands
```
--simulate <steps>             Run simulation for specified steps
--simulate-continue <steps>     Continue from current state
--simulate-pause                Pause simulation
--simulate-resume               Resume simulation
--simulate-step                 Execute single simulation step
--simulate-rewind <steps>       Rewind simulation
--simulate-fastforward <factor> Run simulation at accelerated speed
--simulate-loop <steps>         Run simulation in loop
--simulate-profile              Profile simulation performance
--simulate-benchmark            Run performance benchmark
```

#### Real-time Inspection Commands
```
--inspect-neurons <count>      Show neuron information (limited to count)
--inspect-neuron <id>           Show detailed neuron information
--inspect-synapses <count>      Show synapse information (limited to count)
--inspect-synapse <id>          Show detailed synapse information
--inspect-regions <count>       Show region information (limited to count)
--inspect-region <id>           Show detailed region information
--inspect-populations <count>   Show population information (limited to count)
--inspect-population <id>       Show detailed population information
--inspect-memory               Show memory system status
--inspect-episodic             Show episodic memory status
--inspect-associative          Show associative memory status
--inspect-neuromodulation       Show neuromodulation system status
--inspect-prediction            Show prediction system status
--inspect-planning              Show planning system status
--inspect-attention             Show attention system status
--inspect-concept              Show concept formation status
```

#### Analysis Commands
```
--analyze-neural-activity       Analyze neural activity patterns
--analyze-plasticity            Analyze plasticity patterns
--analyze-memory-usage          Analyze memory system usage
--analyze-performance           Analyze system performance
--analyze-stdp-timing          Analyze STDP timing patterns
--analyze-hebbian-coactivity   Analyze Hebbian co-activity patterns
--analyze-memory-consolidation  Analyze memory consolidation processes
--analyze-neuromodulation       Analyze neuromodulation effects
--analyze-prediction-errors    Analyze prediction error patterns
--analyze-exploration-exploitation Analyze exploration vs. exploitation
--analyze-developmental-stages  Analyze developmental stage effects
```

#### Debugging Commands
```
--debug-spike-events <count>    Show recent spike events (limited to count)
--debug-spike-history <neuron>  Show spike history for specific neuron
--debug-plasticity-history <synapse> Show plasticity history for specific synapse
--debug-memory-traces <count>   Show recent memory traces (limited to count)
--debug-plastic-change <synapse> Show recent plastic changes for synapse
--debug-neuromodulation         Show current neuromodulation levels
--debug-simulation-clock       Show simulation clock state
--debug-checkpoint-status       Show checkpoint system status
--debug-memory-consistency     Check memory system consistency
--debug-plasticity-consistency  Check plasticity system consistency
--debug-neural-consistency       Check neural system consistency
```

#### Export/Import Commands
```
--export-neural-graph <file>    Export neural connectivity graph
--export-synaptic-matrix <file> Export synaptic weight matrix
--export-memory-traces <file>   Export memory trace data
--export-plasticity-data <file> Export plasticity history data
--export-behavior-log <file>    Export behavioral log data
--import-neural-graph <file>     Import neural connectivity graph
--import-synaptic-matrix <file>  Import synaptic weight matrix
--import-behavior-log <file>     Import behavioral log data
```

#### Advanced Features
```
--experiment-create <name>     Create new experiment
--experiment-run <name>        Run experiment
--experiment-list               List all experiments
--experiment-status <name>      Show experiment status
--experiment-results <name>     Show experiment results
--experiment-export <name>      Export experiment data
--experiment-import <name>      Import experiment data

--parameter-sweep <file>        Run parameter sweep experiment
--monte-carlo <runs>            Run Monte Carlo simulation
--sensitivity-analysis <param>   Analyze parameter sensitivity

--online-learning               Enable online learning mode
--batch-learning                Enable batch learning mode
--curriculum-learning           Enable curriculum learning mode
```

## Usage Examples

### Basic Usage

#### Running a Simulation
```bash
./nlm_cli --advanced --simulate 1000
```

#### Inspecting the Brain
```bash
./nlm_cli --advanced --inspect-neurons 10
./nlm_cli --advanced --inspect-region 1
./nlm_cli --advanced --inspect-memory
```

#### Analyzing Performance
```bash
./nlm_cli --advanced --analyze-performance
./nlm_cli --advanced --analyze-plasticity
```

#### Saving and Loading Brain States
```bash
./nlm_cli --advanced --brain-save brain_state.nml
./nlm_cli --advanced --brain-load brain_state.nml
```

#### Running Experiments
```bash
./nlm_cli --advanced --experiment-create learning_experiment
./nlm_cli --advanced --experiment-run learning_experiment
./nlm_cli --advanced --experiment-status learning_experiment
./nlm_cli --advanced --experiment-results learning_experiment
```

### Advanced Usage

#### Parameter Analysis
```bash
./nlm_cli --advanced --parameter-sweep params.cfg
./nlm_cli --advanced --monte-carlo 1000
./nlm_cli --advanced --sensitivity-analysis learning_rate
```

#### Debugging
```bash
./nlm_cli --advanced --debug-spike-events 100
./nlm_cli --advanced --debug-plasticity-history 42
./nlm_cli --advanced --debug-memory-consistency
```

#### Export and Import
```bash
./nlm_cli --advanced --export-neural-graph neural_graph.json
./nlm_cli --advanced --export-synaptic-matrix synaptic_weights.csv
./nlm_cli --advanced --import-behavior-log behavior_log.json
```

## Command Help

### General Help
```bash
./nlm_cli --advanced --help
./nlm_cli --help
```

### Command Group Help
```bash
./nlm_cli --advanced brain --help
./nlm_cli --advanced simulation --help
./nlm_cli --advanced inspection --help
./nlm_cli --advanced analysis --help
./nlm_cli --advanced debugging --help
./nlm_cli --advanced export --help
./nlm_cli --advanced advanced --help
```

### Specific Command Help
```bash
./nlm_cli --advanced --brain-info --help
./nlm_cli --advanced --simulate --help
./nlm_cli --advanced --inspect-neuron --help
```

## Integration with Original CLI

The advanced CLI maintains full backward compatibility with the original NLM CLI:

### Original CLI Usage
```bash
./nlm_cli --config configs/default.cfg
```

### Advanced CLI Usage
```bash
./nlm_cli --advanced --config configs/default.cfg --brain-info
./nlm_cli --advanced --simulate 100
```

### Automatic Detection

The advanced CLI automatically detects whether to use the original or advanced interface:
- Without `--advanced` flag: Uses original CLI
- With `--advanced` flag: Uses advanced CLI

## Testing

### Unit Tests
All command implementations include unit tests:

```bash
./nlm_cli --test test_cli_advanced
./nlm_cli --test test_cli_commands
./nlm_cli --test test_cli_integration
```

### Integration Tests
Integration tests verify that all commands work correctly with the brain system:

```bash
./nlm_cli --advanced --test integration
```

## Configuration

The advanced CLI uses the same configuration system as the original NLM:

### Configuration File
```
configs/default.cfg
configs/advanced.cfg
configs/debug.cfg
```

### Command Line Overrides
```bash
./nlm_cli --advanced --brain-config configs/my_config.cfg --brain-info
```

## File Formats

### Brain State Format
- Binary format: `.nml` (NLM Brain State)
- Text format: `.json` (NLM Brain State)

### Export Formats
- Neural Graph: JSON
- Synaptic Matrix: CSV
- Memory Traces: JSON
- Plasticity Data: JSON
- Behavior Log: JSON

## Performance Considerations

The advanced CLI is designed for:
- **Efficiency**: Optimized command parsing and execution
- **Scalability**: Handles large brain systems with thousands of neurons
- **Memory Usage**: Minimal memory overhead for command processing
- **Thread Safety**: Safe concurrent command execution
- **Extensibility**: Easy addition of new commands and command groups

## Development and Extension

### Adding New Commands
New commands can be added by:
1. Creating a new command handler class in `src/cli/`
2. Implementing the command parsing logic in `AdvancedCommandLineParser`
3. Adding command registration in `AdvancedCLI::registerCommands()`
4. Adding output formatting methods in `CommandOutputFormatter`

### Command Group Structure
Existing command groups can be extended by:
1. Adding new command structures in `AdvancedCommandLineParser`
2. Implementing command handlers in the respective command group classes
3. Updating help and documentation

### Testing Commands
Commands can be tested by:
1. Adding unit tests in `tests/test_cli_advanced.cpp`
2. Adding integration tests in `tests/test_cli_integration.cpp`
3. Running the test suite with `./nlm_cli --test all`

## Conclusion

The NLM Advanced CLI system provides a comprehensive command-line interface for the NLM brain system. It maintains full backward compatibility while adding 60+ advanced commands organized into logical groups. The implementation follows modular design principles, making it easy to maintain, extend, and test. The advanced CLI transforms the NLM project from a basic research prototype into a powerful production tool with extensive command-line capabilities for advanced users.
