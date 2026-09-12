# Phase 6 Advanced User Guide

This document provides advanced usage information for the NLM Phase 6 integration demo.

## Overview

The Phase 6 integration demo has been enhanced with advanced features for experienced users, including:

- Comprehensive command-line argument parsing
- Configuration file support
- Advanced error handling and validation
- Detailed logging options
- Multiple execution modes

## Advanced Configuration

### Command-Line Arguments

The enhanced demo supports the following command-line arguments:

```bash
./nlm_phase6_demo [options]

Options:
  --config=FILE              Load configuration from file (default: configs/default_phase6.cfg)
  --neurons=N                Number of neurons (default: 1000)
  --steps=N                  Max simulation steps (default: 10000)
  --region-count=N            Number of brain regions (default: 1)
  --connection-prob=N        Connection probability (default: 0.1)
  --checkpoint-path=PATH      Checkpoint file path (default: ./checkpoint_test.bin)

  --no-checkpointing          Disable checkpoint saving
  --no-replay                 Disable replay system
  --no-development             Disable development system

  --verbose, -v              Enable debug logging (show detailed neural activity)
  --quiet, -q                Only show errors (minimal output)
  --help, -h                 Show this help message
```

### Configuration File Format

The demo supports configuration files in the following format (`configs/default_phase6.cfg`):

```ini
# NLM Phase 6 Configuration
# Example: 500 neurons, 2000 steps, with checkpointing
neuron_count = 500
max_steps = 2000
region_count = 2
connection_probability = 0.15
enable_checkpointing = true
enable_replay = true
enable_development = true
checkpoint_path = ./phase6_checkpoint.bin

# Advanced settings (if supported)
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

### Configuration File Fields

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `neuron_count` | int | 1000 | Total number of neurons |
| `max_steps` | int | 10000 | Maximum simulation steps |
| `region_count` | int | 1 | Number of brain regions |
| `connection_probability` | float | 0.1 | Probability of connection between neurons |
| `enable_checkpointing` | bool | true | Enable/disable checkpoint saving |
| `enable_replay` | bool | true | Enable/disable replay system |
| `enable_development` | bool | true | Enable/disable development system |
| `checkpoint_path` | string | "./checkpoint_test.bin" | Path for checkpoint files |

## Advanced Usage Examples

### 1. Running with Custom Configuration

```bash
# Run with specific parameters
./nlm_phase6_demo --neurons=2000 --steps=5000 --verbose

# Run with configuration file
./nlm_phase6_demo --config=my_config.cfg --no-replay --no-development

# Run in quiet mode for automated execution
./nlm_phase6_demo --checkpoint-path=/tmp/checkpoint.bin --quiet
```

### 2. Performance Testing

```bash
# Benchmark with different network sizes
./nlm_phase6_demo --neurons=500 --steps=1000 --verbose
./nlm_phase6_demo --neurons=1000 --steps=500 --verbose
./nlm_phase6_demo --neurons=2000 --steps=200 --verbose
```

### 3. Development Mode

```bash
# Run with development disabled (faster, less memory)
./nlm_phase6_demo --no-development --no-replay --no-checkpointing --quiet

# Run with checkpointing for later analysis
./nlm_phase6_demo --checkpoint-path=./my_experiment.bin --verbose
```

### 4. Integration Testing

For integration testing, use the `--verbose` flag to see detailed output:

```bash
./nlm_phase6_demo --neurons=500 --steps=2000 --verbose
```

## Logging Levels

The demo supports different logging levels:

### Info Level (Default)
Shows basic information about the experiment progress.

### Debug Level (`--verbose`)
Shows detailed neural activity, memory operations, and system interactions.

### Error Level (`--quiet`)
Shows only errors and failures, useful for automated testing.

## Expected Output

### Basic Run
```
=== NLM Phase 6 Integration Demo ===
Testing the integrated artificial brain...

--- Integration Verification ---
[INFO] === Phase 6 Integration Verification ===
[PASS] Working memory is integrated
[PASS] Episodic memory is integrated
[PASS] Dopamine system is integrated
[PASS] Curiosity system is integrated
[PASS] Prediction system is integrated
[PASS] Planner is integrated
[PASS] Concept formation is integrated
[PASS] Attention is integrated
[PASS] Development system is integrated

Integration verification passed!

--- Memory Integration Test ---
[INFO] === Testing Memory Integration ===
[PASS] Working memory has active traces: 5
[PASS] Episodic memory has episodes: 3

--- Neuromodulation Integration Test ---
[INFO] === Testing Neuromodulation Integration ===
[PASS] Neuromodulation systems are functional

--- Checkpoint Test ---
[INFO] === Testing Checkpoint Save/Load ===
[PASS] Checkpoint save/load completed

--- Replay Test ---
[INFO] === Testing Replay System ===
[PASS] Episodes available for replay: 5
[PASS] Replay system can retrieve episodes

--- Full Integration Experiment ---
[INFO] Running with 500 neurons for 2000 steps...
[INFO] Step 1000 | Reward: 0.123 | Firing: 0.456 | WorkingMem: 2

=== FINAL RESULTS ===
Total reward: 0.123
Average firing rate: 0.456
Memory episodes stored: 3.000
Dopamine level: 0.123
Average synaptic weight: 0.512
Novelty level: 0.789
Curiosity level: 0.456

=== INTEGRATION STATUS ===
Working Memory: ✓ CONNECTED
Episodic Memory: ✓ CONNECTED
Neuromodulation: ✓ CONNECTED
Prediction System: ✓ CONNECTED
Development System: ✓ CONNECTED
Checkpointing: ✓ WORKING
Replay System: ✓ WORKING

=== TIMING ===
Wall clock time: 12.34 seconds
Average time per step: 0.00617 seconds
```

## Troubleshooting

### Common Issues and Solutions

**Issue: "Error: neuron_count must be at least 10"**
```bash
# Solution: Use a larger value
./nlm_phase6_demo --neurons=100
```

**Issue: "Error: max_steps must be at least 100"**
```bash
# Solution: Increase the number of steps
./nlm_phase6_demo --steps=500
```

**Issue: Configuration file not found**
```bash
# Solution: Create the config file or specify full path
./nlm_phase6_demo --config=./configs/my_config.cfg
```

**Issue: No output in quiet mode**
```bash
# Solution: Use verbose mode for debugging
./nlm_phase6_demo --verbose
```

### Advanced Debugging

For detailed debugging, use the `--verbose` flag:

```bash
./nlm_phase6_demo --neurons=200 --steps=1000 --verbose
```

This will show:
- Neural activity details
- Memory system operations
- Integration verification results
- System parameter values

## Performance Considerations

### Memory Usage
- **Small configuration**: 500 neurons, 2000 steps (~100MB RAM)
- **Medium configuration**: 1000 neurons, 5000 steps (~250MB RAM)
- **Large configuration**: 2000 neurons, 2000 steps (~400MB RAM)

### Execution Time
- **Small configuration**: ~10-15 seconds
- **Medium configuration**: ~30-45 seconds
- **Large configuration**: ~60-90 seconds

### Optimization Tips

1. **For faster execution**: Use `--no-development --no-replay --no-checkpointing`
2. **For better results**: Use larger neuron counts and step counts
3. **For debugging**: Use `--verbose` flag for detailed output
4. **For automation**: Use `--quiet` flag for minimal output

## Extending the Demo

### Adding Custom Configuration Options

To add custom configuration options, modify the `Phase6Config` struct in `Phase6IntegratedExperiment.hpp` and update the argument parsing in `Phase6Demo.cpp`.

### Adding Custom Tests

To add custom integration tests, extend the `Phase6IntegratedExperiment` class in `Phase6IntegratedExperiment.cpp` and add new test methods.

### Creating Custom Demos

For custom demos, create a new demo file that includes the `Phase6IntegratedExperiment` class and custom configuration.

## References

- [Phase 6 Integrated Experiment Documentation](Phase6IntegratedExperiment.hpp)
- [NLM Documentation](../README.md)
- [Advanced NLM Usage](HOW_TO_USE.md)

## Version Information

- **Version**: 2.0 (Enhanced)
- **Last Updated**: 2024
- **Requires**: NLM Phase 6.0 or later

For more advanced features and customization options, refer to the full NLM documentation.
