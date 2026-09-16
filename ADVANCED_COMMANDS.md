# Advanced Command System for NLM Brain

## Overview

The NLM (Neural Learning Machine) now features a comprehensive advanced command system designed for power users and researchers. This system builds upon the existing checkpointing infrastructure and provides powerful tools for batch processing, checkpoint management, performance monitoring, configuration management, and plugin extensions.

## Key Features

### 1. Batch Processing Commands
- **`run-batch <steps> [output_file] [save_interval]`**: Run multiple simulation steps efficiently with progress tracking
- **`run-until <condition> [max_steps] [checkpoint_on_condition]`**: Run simulation until a condition is met (spikes, neurons, stable learning)
- **`parallel-run <threads> <steps_per_thread> [sync_mode]`**: Multi-threaded execution with configurable thread count

### 2. Advanced Checkpoint Commands
- **`save-checkpoint <name> [description] [priority] [--compress] [--metadata key=value]...`**: Save brain state with rich metadata
- **`load-checkpoint <name> [--validate] [--reset] [--skip-validation]`**: Load checkpoint with validation options
- **`list-checkpoints [--detailed] [--filter <pattern>] [--show-meta]`**: Show available checkpoints with filtering
- **`delete-checkpoint <name> [--force] [--skip-backup]`**: Remove checkpoints with safety checks
- **`checkpoint-info <name> [--include-metadata] [--show-compression]`**: Show detailed checkpoint information

### 3. Scripting Support
- **`script <script_file> [--interactive] [--replay] [--history]`**: Execute script files or enter interactive mode
- **`replay <count> [--all]`**: Replay last N commands from command history
- **`history [--clear] [--max <size>] [--show]`**: Manage command history
- Built-in scripting language with:
  - Conditional execution (`if condition command`)
  - Loops (`while condition command`, `for var start end step command`)
  - Variable assignment (`set var value`)
  - Command execution (`run command`, `monitor command`, `checkpoint command`)

### 4. Performance Monitoring
- **`monitor-start [--frequency <interval>] [--metrics <list>] [--output <file>]`**: Start performance monitoring
- **`monitor-stop [--save-stats] [--output <file>]`**: Stop monitoring and optionally save statistics
- **`performance-stats [--detailed] [--format json|csv|table] [--output <file>]`**: Show current performance metrics

### 5. Configuration Management
- **`save-config <name> [description] [--force]`**: Save current configuration to file
- **`load-config <name> [--validate] [--override]`**: Load saved configuration

### 6. Plugin/System Extensions
- **`register-plugin <name> <path> [--enable] [--priority N]`**: Load external plugins
- **`list-plugins [--enabled-only] [--detail]`**: Show available and registered plugins

## Usage Examples

### Basic Batch Processing

```bash
# Run 10,000 simulation steps with output to CSV
nlm --advanced-mode run-batch 10000 batch_output.csv 100

# Run until learning stabilizes (max 50,000 steps)
nlm --advanced-mode run-until stable 50000

# Parallel execution with 4 threads
nlm --advanced-mode parallel-run 4 1000 independent
```

### Checkpoint Management

```bash
# Save current brain state with metadata
nlm --advanced-mode save-checkpoint experiment1 "Training run #1" 8 --compress --metadata date=2025-09-16 --metadata version=1.0

# List all checkpoints
nlm --advanced-mode list-checkpoints --detailed

# Load a specific checkpoint
nlm --advanced-mode load-checkpoint experiment1 --validate --reset

# Get checkpoint information
nlm --advanced-mode checkpoint-info experiment1 --include-metadata --show-compression
```

### Performance Monitoring

```bash
# Start monitoring with 500ms intervals
nlm --advanced-mode monitor-start --frequency 500 --metrics spikes,neurons,firing,memory perf_output.csv

# Check current performance
nlm --advanced-mode performance-stats --detailed --format table

# Stop monitoring and save statistics
nlm --advanced-mode monitor-stop --save-stats --output performance_stats.json
```

### Configuration Management

```bash
# Save current configuration
nlm --advanced-mode save-config training_config "Training configuration for experiment" --force

# Load a saved configuration
nlm --advanced-mode load-config training_config --validate --override
```

### Plugin Management

```bash
# Register a new plugin
nlm --advanced-mode register-plugin memory_optimizer ./plugins/libmemory_opt.so --enable --priority 7

# List all plugins
nlm --advanced-mode list-plugins --detail
```

### Scripting

#### Script File

```bash
# Save a script to file
script cat > experiment_script.txt << EOF
# Run batch simulation
run-batch 5000 results.csv 500

# Save checkpoint after completion
save-checkpoint final_state "Completed simulation" 7

# Generate performance report
performance-stats --detailed
EOF

# Execute the script
nlm --advanced-mode script experiment_script.txt
```

#### Interactive Mode

```bash
# Start interactive scripting mode
nlm --advanced-mode script --interactive
```

Interactive commands:
- `run-batch 1000 test.csv` - Run batch simulation
- `save-checkpoint test_run "Test simulation" 5` - Save checkpoint
- `if spikes>100 run-batch 500 test2.csv` - Conditional execution
- `history` - Show command history
- `replay 5` - Replay last 5 commands
- `exit` - Exit interactive mode
```

### Performance Statistics

Performance statistics are collected during monitoring and include:
- Total steps executed
- Total spikes generated
- Active neurons count
- Total neuron count
- Firing rate
- Memory usage
- Prediction error
- Reward values
- Elapsed time

## Technical Implementation

### Command System Architecture

The advanced command system is organized into several layers:

1. **Base Command Interface (`ICommand`)**: Abstract base class defining command interface
2. **Command Manager**: Central command dispatcher with support for global and brain-dependent commands
3. **Command Categories**: Each feature group has its own header file:
   - `BatchProcessingCommands.hpp`: Batch execution commands
   - `CheckpointCommands.hpp`: Checkpoint management commands
   - `PerformanceCommands.hpp`: Performance monitoring commands
   - `ScriptingCommands.hpp`: Scripting and command history
   - `AdvancedCommandManager.hpp`: Advanced command manager
4. **Integration with Main Application**: Commands can be invoked via command-line arguments with `--advanced-mode` or `--power-user` flags

### Key Design Principles

1. **Backward Compatibility**: All new commands are optional and don't interfere with normal NLM operation
2. **Extensibility**: Plugin system allows for easy addition of new commands
3. **Error Handling**: Comprehensive error handling with informative messages
4. **Performance**: Efficient execution with minimal overhead for normal operation
5. **User Experience**: Intuitive command syntax with helpful error messages and completion

## Files Added/Modified

### New Files
- `src/command/CommandSystem.hpp` - Base command interface and manager
- `src/command/CommandSystem.cpp` - Command system implementation
- `src/command/BatchProcessingCommands.hpp` - Batch processing commands
- `src/command/CheckpointCommands.hpp` - Checkpoint management commands
- `src/command/PerformanceCommands.hpp` - Performance monitoring commands
- `src/command/ScriptingCommands.hpp` - Scripting and command history
- `src/command/AdvancedCommandManager.hpp` - Advanced command manager
- `src/command/AdvancedCommandManager.cpp` - Advanced command manager implementation

### Modified Files
- `CMakeLists.txt` - Added new command system libraries
- `src/main.cpp` - Added advanced command support and initialization
- `src/performance/CheckpointManager.cpp` - Updated checkpoint manager implementation

## Build Instructions

The advanced command system requires no additional build dependencies beyond the existing NLM codebase. Simply rebuild the project to include the new functionality:

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Usage with NLM

### Normal Operation (Unchanged)

The advanced commands are opt-in and don't affect normal NLM operation:

```bash
# Normal NLM execution (unchanged)
./nlm --config configs/my_config.cfg
```

### Advanced Mode

To use advanced commands, add `--advanced-mode` or `--power-user` flag:

```bash
# With advanced mode flag
./nlm --advanced-mode

# Or use specific advanced commands directly
./nlm --advanced-mode run-batch 1000 output.csv
```

### Help and Documentation

View available advanced commands:

```bash
./nlm --advanced-mode help
```

For command-specific help:

```bash
./nlm --advanced-mode help <command_name>
```

## Future Enhancements

The advanced command system is designed to be extensible:

1. **New Commands**: Easy to add new commands by implementing `ICommand` and registering them
2. **Plugin Architecture**: Plugin system allows for external extensions
3. **Scripting Language**: Can be expanded with more advanced scripting features
4. **GUI Integration**: Foundation for potential GUI command interfaces
5. **Distributed Computing**: Infrastructure for parallel and distributed execution

## Conclusion

The advanced command system brings powerful new capabilities to the NLM brain system, making it more accessible and powerful for advanced users while maintaining full backward compatibility. The system provides comprehensive tools for simulation control, state management, performance monitoring, and scripting, enabling researchers and developers to work with the NLM more efficiently and effectively.