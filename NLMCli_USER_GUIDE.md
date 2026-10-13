# NLM Advanced CLI Tool - User Guide

## Overview

The NLM (Neural Learning Machine) Advanced CLI Tool provides comprehensive command-line interfaces for power users to manage, debug, and analyze brain simulation states, memory systems, neuromodulation, prediction systems, and experimental configurations.

This tool builds upon the existing NLM Phase 2 neural computation framework and provides:
- **Brain State Management**: Checkpoint/save/load, snapshots, configuration export/import, performance profiling
- **Memory System Commands**: Working, episodic, and semantic memory inspection and control
- **Neuromodulation Commands**: Neuromodulator level adjustments and calibration
- **Prediction System Commands**: Accuracy inspection, error visualization, and tuning
- **Advanced Configuration**: Batch simulation runs and multi-experiment management
- **Debugging and Analysis**: Neural activity inspection, synapse state examination, plasticity statistics

## Installation

### Building from Source

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd nlm
   ```

2. Build with CMake:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

3. The tool will be available as:
   - `nlm-cli`: Advanced CLI tool with all power user commands
   - `nlm`: Basic neural simulation (existing)
   - `nlm_phase3_demo`: Phase 3 world interaction demo
   - `nlm_phase4_demo`: Phase 4 cognition demo

### Quick Start

```bash
# Install the shell wrapper script
chmod +x nlm-cli

# Help command
nlm-cli --help

# Show help for specific command
nlm-cli brain-snapshot --help
```

## Command Reference

### 1. BRAIN STATE MANAGEMENT

#### `checkpoint save [NAME]`
Save a checkpoint of the current brain state.

**Arguments:**
- `[NAME]`: Optional name for the checkpoint (defaults to timestamp-based)

**Examples:**
```bash
nlm-cli checkpoint save
nlm-cli checkpoint save training_checkpoint_1
```

#### `checkpoint list`
List all available checkpoints.

**Examples:**
```bash
nlm-cli checkpoint list
```

#### `checkpoint load NAME`
Load a checkpoint by name.

**Arguments:**
- `NAME`: Name of the checkpoint to load

**Examples:**
```bash
nlm-cli checkpoint load latest
nlm-cli checkpoint load training_checkpoint_1
```

#### `checkpoint delete NAME`
Delete a checkpoint.

**Arguments:**
- `NAME`: Name of the checkpoint to delete

**Examples:**
```bash
nlm-cli checkpoint delete old_checkpoint
```

#### `checkpoint info NAME`
Show detailed information about a checkpoint.

**Arguments:**
- `NAME`: Name of the checkpoint to inspect

**Output includes:**
- Checkpoint timestamp
- Number of neurons and synapses
- Simulation step and time
- File size
- Checksum

**Examples:**
```bash
nlm-cli checkpoint info training_checkpoint_1
```

#### `checkpoint restore NAME`
Restore a checkpoint (overwrites current brain state).

**Arguments:**
- `NAME`: Name of the checkpoint to restore

**Examples:**
```bash
nlm-cli checkpoint restore latest
```

#### `brain snapshot`
Take a snapshot of the current brain state.

**Examples:**
```bash
nlm-cli brain snapshot
```

#### `brain save-snapshot FILE`
Save a brain snapshot to a file.

**Arguments:**
- `FILE`: Output file path for the snapshot

**Examples:**
```bash
nlm-cli brain save-snapshot brain_state_20241013_120000.bin
nlm-cli brain save-snapshot brain_state.json
```

#### `brain load-snapshot FILE`
Load a brain snapshot from a file.

**Arguments:**
- `FILE`: Input file path for the snapshot

**Examples:**
```bash
nlm-cli brain load-snapshot brain_state_20241013_120000.bin
```

#### `config export FILE`
Export configuration to a file.

**Arguments:**
- `FILE`: Output file path for the configuration export

**Examples:**
```bash
nlm-cli config export brain_config.cfg
nlm-cli config export brain_config.json
```

#### `config import FILE`
Import configuration from a file.

**Arguments:**
- `FILE`: Input file path for the configuration import

**Examples:**
```bash
nlm-cli config import brain_config.cfg
nlm-cli config import brain_config.json
```

#### `config summary`
Show configuration summary with all current settings.

**Examples:**
```bash
nlm-cli config summary
```

#### `config set KEY VALUE`
Set a configuration value.

**Arguments:**
- `KEY`: Configuration key
- `VALUE`: Configuration value

**Examples:**
```bash
nlm-cli config set neuron_count 1000
nlm-cli config set simulation_timestep 0.001
nlm-cli config set random_seed 12345
```

#### `config get KEY`
Get a configuration value.

**Arguments:**
- `KEY`: Configuration key

**Examples:**
```bash
nlm-cli config get neuron_count
nlm-cli config get simulation_timestep
```

#### `profile start [FILE]`
Start performance profiling.

**Arguments:**
- `[FILE]`: Optional output file for profiling data (saves to memory if not specified)

**Examples:**
```bash
nlm-cli profile start
nlm-cli profile start profile_data.prof
```

#### `profile stop`
Stop performance profiling.

**Examples:**
```bash
nlm-cli profile stop
```

#### `profile report`
Generate a profiling report.

**Examples:**
```bash
nlm-cli profile report
```

### 2. MEMORY SYSTEM COMMANDS

#### `memory working [COUNT]`
Show working memory state.

**Arguments:**
- `[COUNT]`: Number of entries to display (default: 10)

**Examples:**
```bash
nlm-cli memory working
nlm-cli memory working 50
```

#### `memory episodic [COUNT]`
Show episodic memory state.

**Arguments:**
- `[COUNT]`: Number of entries to display (default: 10)

**Examples:**
```bash
nlm-cli memory episodic
nlm-cli memory episodic 25
```

#### `memory semantic [COUNT]`
Show semantic memory state.

**Arguments:**
- `[COUNT]`: Number of entries to display (default: 10)

**Examples:**
```bash
nlm-cli memory semantic
nlm-cli memory semantic 20
```

#### `memory consolidate`
Trigger memory consolidation.

**Description:** Forces the consolidation of working memory into episodic memory.

**Examples:**
```bash
nlm-cli memory consolidate
```

#### `memory replay START END`
Replay memories between simulation steps.

**Arguments:**
- `START`: Starting simulation step
- `END`: Ending simulation step

**Examples:**
```bash
nlm-cli memory replay 1000 2000
```

#### `memory stats`
Show memory statistics.

**Output includes:**
- Working memory capacity and usage
- Episodic memory storage
- Semantic memory statistics
- Consolidation effectiveness

**Examples:**
```bash
nlm-cli memory stats
```

#### `memory clear-working`
Clear working memory.

**Examples:**
```bash
nlm-cli memory clear-working
```

#### `memory clear-episodic`
Clear episodic memory.

**Examples:**
```bash
nlm-cli memory clear-episodic
```

#### `memory clear-semantic`
Clear semantic memory.

**Examples:**
```bash
nlm-cli memory clear-semantic
```

### 3. NEUROMODULATION COMMANDS

#### `neuromodulators`
Show all neuromodulator levels.

**Examples:**
```bash
nlm-cli neuromodulators
```

#### `neuromodulator set TYPE LEVEL`
Set neuromodulator level.

**Arguments:**
- `TYPE`: Neuromodulator type (dopamine, serotonin, acetylcholine, norepinephrine)
- `LEVEL`: Target level (0.0 - 1.0)

**Examples:**
```bash
nlm-cli neuromodulator set dopamine 0.5
nlm-cli neuromodulator set serotonin 0.8
```

#### `neuromodulator info TYPE`
Show neuromodulator information.

**Arguments:**
- `TYPE`: Neuromodulator type

**Examples:**
```bash
nlm-cli neuromodulator info dopamine
```

#### `neuromodulators calibrate`
Calibrate neuromodulator levels.

**Description:** Automatically calibrate neuromodulator levels based on current brain state.

**Examples:**
```bash
nlm-cli neuromodulators calibrate
```

#### `neuromodulators stats`
Show neuromodulator statistics.

**Output includes:**
- Average neuromodulator levels
- Variation across neurons
- Correlation with neural activity

**Examples:**
```bash
nlm-cli neuromodulators stats
```

#### `neuromodulators reset`
Reset neuromodulators to baseline.

**Examples:**
```bash
nlm-cli neuromodulators reset
```

#### `neuromodulators snapshot`
Take neuromodulator snapshot.

**Examples:**
```bash
nlm-cli neuromodulators snapshot
```

### 4. PREDICTION SYSTEM COMMANDS

#### `prediction accuracy`
Show prediction accuracy metrics.

**Output includes:**
- Current prediction accuracy
- Prediction improvement over time
- Error rates

**Examples:**
```bash
nlm-cli prediction accuracy
```

#### `prediction errors`
Show prediction error history.

**Examples:**
```bash
nlm-cli prediction errors
```

#### `prediction visualize`
Generate prediction error visualization.

**Description:** Creates visual representation of prediction errors.

**Examples:**
```bash
nlm-cli prediction visualize
```

#### `prediction tune PARAM VALUE`
Tune prediction system parameters.

**Arguments:**
- `PARAM`: Parameter name to tune
- `VALUE`: New value for the parameter

**Examples:**
```bash
nlm-cli prediction tune learning_rate 0.01
nlm-cli prediction tune prediction_horizon 10
```

#### `prediction reset`
Reset prediction system.

**Examples:**
```bash
nlm-cli prediction reset
```

#### `prediction info`
Show prediction system status.

**Examples:**
```bash
nlm-cli prediction info
```

#### `prediction snapshot`
Take prediction system snapshot.

**Examples:**
```bash
nlm-cli prediction snapshot
```

### 5. ADVANCED CONFIGURATION

#### `batch run CONFIG STEPS`
Run batch of simulations.

**Arguments:**
- `CONFIG`: Configuration file for the batch
- `STEPS`: Number of steps per simulation

**Examples:**
```bash
nlm-cli batch run my_batch.cfg 1000
```

#### `batch list`
List all batch configurations.

**Examples:**
```bash
nlm-cli batch list
```

#### `batch info NAME`
Show batch configuration details.

**Arguments:**
- `NAME`: Name of the batch configuration

**Examples:**
```bash
nlm-cli batch info my_batch
```

#### `batch delete NAME`
Delete batch configuration.

**Arguments:**
- `NAME`: Name of the batch configuration to delete

**Examples:**
```bash
nlm-cli batch delete old_batch
```

#### `batch run-all`
Run all batch configurations.

**Examples:**
```bash
nlm-cli batch run-all
```

#### `experiment list`
List available experiments.

**Examples:**
```bash
nlm-cli experiment list
```

#### `experiment run NAME [ARGS...]`
Run experiment with arguments.

**Arguments:**
- `NAME`: Name of the experiment
- `[ARGS...]`: Additional arguments for the experiment

**Examples:**
```bash
nlm-cli experiment run phase3 100
nlm-cli experiment run phase4
```

#### `experiment status NAME`
Show experiment status.

**Arguments:**
- `NAME`: Name of the experiment

**Examples:**
```bash
nlm-cli experiment status phase3
```

#### `sim-param set KEY VALUE`
Set simulation parameter.

**Arguments:**
- `KEY`: Parameter key
- `VALUE`: Parameter value

**Examples:**
```bash
nlm-cli sim-param set neuron_count 500
nlm-cli sim-param set connection_probability 0.2
```

#### `sim-param get KEY`
Get simulation parameter.

**Arguments:**
- `KEY`: Parameter key

**Examples:**
```bash
nlm-cli sim-param get neuron_count
```

#### `sim-param list`
List all simulation parameters.

**Examples:**
```bash
nlm-cli sim-param list
```

### 6. DEBUGGING AND ANALYSIS

#### `debug neural-activity`
Show current neural activity.

**Output includes:**
- Number of active neurons
- Average firing rate
- Neuron type distribution

**Examples:**
```bash
nlm-cli debug neural-activity
```

#### `debug synapses STATE`
Examine synapse states.

**Arguments:**
- `STATE`: Synapse state filter (healthy, weak, strong, plastic, etc.)

**Examples:**
```bash
nlm-cli debug synapses healthy
nlm-cli debug synapses plastic
```

#### `debug plasticity`
Show plasticity statistics.

**Output includes:**
- STDP modifications
- Hebbian modifications
- Structural changes
- Plasticity effectiveness

**Examples:**
```bash
nlm-cli debug plasticity
```

#### `debug events LOG_LEVEL`
Control event logging.

**Arguments:**
- `LOG_LEVEL`: Log level (debug, info, warning, error)

**Examples:**
```bash
nlm-cli debug events info
```

#### `debug logs`
Show recent log entries.

**Examples:**
```bash
nlm-cli debug logs
```

#### `debug memory-usage`
Show memory usage statistics.

**Output includes:**
- Memory allocation patterns
- Leak detection
- Fragmentation analysis

**Examples:**
```bash
nlm-cli debug memory-usage
```

#### `debug performance`
Show performance metrics.

**Output includes:**
- CPU usage
- Memory bandwidth
- Spike processing rate

**Examples:**
```bash
nlm-cli debug performance
```

#### `debug connectivity`
Show neural connectivity.

**Examples:**
```bash
nlm-cli debug connectivity
```

#### `neural inspect NEURON_ID`
Inspect specific neuron.

**Arguments:**
- `NEURON_ID`: Neuron identifier

**Examples:**
```bash
nlm-cli neural inspect 42
```

#### `synapse inspect SYNAPSE_ID`
Inspect specific synapse.

**Arguments:**
- `SYNAPSE_ID`: Synapse identifier

**Examples:**
```bash
nlm-cli synapse inspect 100
```

#### `plasticity stats`
Show plasticity system statistics.

**Examples:**
```bash
nlm-cli plasticity stats
```

#### `event-log enable LEVEL`
Enable event logging.

**Arguments:**
- `LEVEL`: Log level to enable

**Examples:**
```bash
nlm-cli event-log enable debug
```

#### `event-log disable LEVEL`
Disable event logging.

**Arguments:**
- `LEVEL`: Log level to disable

**Examples:**
```bash
nlm-cli event-log disable debug
```

## Configuration Files

### Configuration File Format

The NLM system uses a simple key=value format for configuration files:

```ini
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.15
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
log_level = INFO
```

### Checkpoint Files

Checkpoints are stored in binary format with the following structure:
- Header with metadata
- Neuron states (membrane potential, firing state, etc.)
- Synapse states (weights, plasticity, etc.)
- Spike history
- Plasticity state
- Developmental state
- Random generator state
- Simulation clock
- Environment state (optional)

### Snapshot Files

Brain snapshots are stored as JSON files for human readability:
```json
{
  "brain_state": {
    "neuron_count": 1000,
    "synapse_count": 50000,
    "regions": [...],
    "current_step": 1000,
    "simulation_time": 1.0
  },
  "config": {...},
  "timestamp": "2024-10-13T12:00:00"
}
```

## File Locations

- **Configuration files**: `configs/` directory
- **Checkpoint files**: `./checkpoints/` directory (configurable)
- **Snapshot files**: Current working directory
- **Log files**: `./logs/` directory (configurable)
- **Performance profiles**: `./profiles/` directory (configurable)
- **Batch configurations**: `./batches/` directory (configurable)

## Scripting Examples

### Save and Load Brain State

```bash
#!/bin/bash
# Save current brain state
nlm-cli brain save-snapshot brain_state_backup.bin

# Run some operations...
sleep 10

# Load brain state back
nlm-cli brain load-snapshot brain_state_backup.bin
```

### Memory Analysis Script

```bash
#!/bin/bash
# Analyze memory statistics
echo "=== Memory Statistics ==="
nlm-cli memory stats

echo "=== Working Memory (Top 20) ==="
nlm-cli memory working 20

echo "=== Triggering Consolidation ==="
nlm-cli memory consolidate

echo "=== Memory Statistics After Consolidation ==="
nlm-cli memory stats
```

### Performance Monitoring Script

```bash
#!/bin/bash
# Start performance profiling
nlm-cli profile start profile_data.prof

# Run simulation
./nlm --config training.cfg

# Stop profiling
nlm-cli profile stop

# Generate report
nlm-cli profile report
```

### Batch Processing Script

```bash
#!/bin/bash
# Create batch configuration
cat > batch_config.cfg << EOF
random_seed = 123
neuron_count = 500
region_count = 2
connection_probability = 0.1
EOF

# Run batch
nlm-cli batch run batch_config.cfg 1000

# List completed batches
nlm-cli batch list
```

## Troubleshooting

### Common Issues

1. **Brain not initialized**
   ```bash
   nlm-cli brain snapshot
   ```

2. **Checkpoint not found**
   ```bash
   nlm-cli checkpoint list
   ```

3. **Invalid configuration**
   ```bash
   nlm-cli config summary
   ```

4. **Memory corruption**
   ```bash
   nlm-cli memory clear-working
   nlm-cli memory clear-episodic
   nlm-cli memory clear-semantic
   ```

5. **Performance issues**
   ```bash
   nlm-cli debug performance
   ```

### Getting More Help

- **General help**: `nlm-cli --help`
- **Command help**: `nlm-cli <command> --help`
- **Checkpoints**: `nlm-cli checkpoint list`
- **Memory stats**: `nlm-cli memory stats`
- **Performance**: `nlm-cli debug performance`

## Advanced Usage

### Custom Script Integration

The NLM CLI tool can be integrated into custom workflows:

```bash
#!/bin/bash
# Automated experiment with checkpointing and analysis

EXPERIMENT_NAME="experiment_$(date +%Y%m%d_%H%M%S)"

# Initialize experiment
nlm-cli config set random_seed $RANDOM
nlm-cli config set neuron_count 1000

# Create checkpoint directory
mkdir -p checkpoints/$EXPERIMENT_NAME

# Run simulation with checkpointing
./nlm --config experiment.cfg > logs/$EXPERIMENT_NAME.log 2>&1

# Create final checkpoint
nlm-cli checkpoint save $EXPERIMENT_NAME

# Analyze results
nlm-cli memory stats
nlm-cli debug neural-activity

# Generate summary report
cat > reports/$EXPERIMENT_NAME.summary << EOF
Experiment: $EXPERIMENT_NAME
Date: $(date)
Checkpoints: $(ls checkpoints/$EXPERIMENT_NAME/ | wc -l)
Memory Stats: $(nlm-cli memory stats | grep -c "Working Memory")
EOF

echo "Experiment $EXPERIMENT_NAME completed successfully!"
```

### Parallel Processing

Run multiple simulations in parallel:

```bash
#!/bin/bash
# Run 5 parallel simulations
for i in {1..5}; do
    (nlm-cli config set random_seed $i && ./nlm --config parallel.cfg) &
done

# Wait for all to complete
wait

# Create combined analysis
nlm-cli memory stats
nlm-cli debug performance
```

### Integration with Existing Tools

The NLM CLI can be used with existing NLM tools:

```bash
# Run Phase 3 demo with advanced CLI
./nlm_phase3_demo 10 200

# Analyze results
nlm-cli debug neural-activity
nlm-cli memory stats

# Save brain state
nlm-cli brain save-snapshot phase3_results.bin
```

## Known Limitations

1. **Checkpoint file size**: Large checkpoints may exceed memory limits
2. **Memory usage**: Memory inspection may not show all memory usage
3. **Real-time monitoring**: Some debugging commands may not support real-time updates
4. **Configuration validation**: Configuration validation is basic

## Future Enhancements

The NLM CLI tool will be extended with:

1. **Real-time monitoring**: Live dashboard with real-time metrics
2. **Advanced visualization**: Graphical representations of brain state
3. **Machine learning integration**: Automated hyperparameter optimization
4. **Cloud integration**: Distributed computing support
5. **API endpoints**: RESTful API for programmatic access
6. **Extended debugging**: Advanced debugging tools and profilers
7. **Batch automation**: Automated batch experiment management
8. **Integration plugins**: Plugin system for extending functionality

## License

This tool is part of the NLM (Neural Learning Machine) project. See LICENSE file for details.

## Support

For support with the NLM CLI tool, please:
1. Check the documentation
2. Review the examples
3. Report issues with detailed error messages
4. Provide system information and logs
5. Check for known limitations and workarounds