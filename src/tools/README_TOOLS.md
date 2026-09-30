# NLM Brain Command-Line Tools

This directory contains command-line tools for inspecting and controlling the NLM (熙然) brain.
These tools are designed for advanced users and researchers working with the NLM brain system.

## Overview

The NLM brain is an experimental artificial developmental brain project. These command-line tools
provide interfaces to:

1. **nlm_info** - Display brain status and statistics
2. **nlm_save** - Save brain state to checkpoint
3. **nlm_load** - Load brain state from checkpoint
4. **nlm_status** - Show current system state
5. **nlm_metrics** - Display performance metrics

## Dependencies

All tools require the NLM library to be built. They depend on:

- Core NLM libraries (nlm_core)
- Brain components
- Configuration system
- Logger system

## Building Tools

These tools should be built as part of the main NLM project using CMake:

```bash
mkdir build
cd build
cmake ..
make -j4
```

The tools will be built as executables and available in the build directory.

## Usage

### General Usage

All tools support the following common options:

- `--help, -h` - Show help message
- `--config <file>` - Specify configuration file (default: configs/default.cfg)
- `--verbose, -v` - Enable verbose output

### nlm_info

Display brain status and statistics:

```bash
# Basic usage
./nlm_info

# With verbose output
./nlm_info --verbose

# With custom configuration
./nlm_info --config my_config.cfg

# With JSON output (if supported)
./nlm_info --format json
```

**Output includes:**
- Brain statistics (neuron count, synapse count, firing rate, etc.)
- Configuration details
- System information
- Optional verbose information

### nlm_save

Save brain state to checkpoint:

```bash
# Save current brain state
./nlm_save brain_checkpoint.chk

# Save with verbose output
./nlm_save my_brain.chk --verbose

# Save with custom configuration
./nlm_save checkpoint.chk --config custom.cfg
```

**Creates:** Checkpoint files (.chk extension) containing:
- Neuron states
- Synapse connections
- Brain configuration
- System statistics

### nlm_load

Load brain state from checkpoint:

```bash
# Load brain state
./nlm_load brain_checkpoint.chk

# Load with verbose output
./nlm_load saved_brain.chk --verbose

# Load with custom configuration
./nlm_load checkpoint.chk --config custom.cfg
```

**Restores:** Complete brain state including:
- Neural connectivity
- Synaptic weights
- Brain configuration
- System statistics

### nlm_status

Show current system state:

```bash
# Basic status
./nlm_status

# With detailed information
./nlm_status --detailed

# With verbose output
./nlm_status --verbose

# With JSON output
./nlm_status --format json
```

**Output includes:**
- Core system information
- Brain status
- Memory system status
- Neuromodulation status
- Development stage

### nlm_metrics

Display performance metrics:

```bash
# Single metric snapshot
./nlm_metrics

# Continuous monitoring every 30 seconds
./nlm_metrics --interval 30

# Monitor for 5 minutes
./nlm_metrics --duration 300

# Output in CSV format
./nlm_metrics --format csv

# Output in JSON format
./nlm_metrics --format json
```

**Metrics include:**
- Core performance (neurons, synapses, firing rates)
- Memory system utilization
- Neuromodulation levels
- Development status
- System health indicators

## File Formats

### Text Output
Human-readable format with detailed information.

### JSON Output
Structured format for programmatic consumption.

### CSV Output
Comma-separated values for spreadsheet analysis.

## Configuration

All tools use configuration files to control brain behavior:

- **Default configuration:** `configs/default.cfg`
- **Custom configuration:** Specify with `--config <file>`

Configuration options include:
- Random seed
- Simulation timestep
- Neuron count
- Region count
- Connection probability
- Plasticity parameters
- Neuromodulation settings

## Integration

### With Main NLM Demo
These tools can be used alongside the main NLM demo executables:

```bash
# Run main demo first
./nlm_phase6_demo

# Then use tools to inspect or save brain state
./nlm_info
./nlm_save saved_state.chk
./nlm_load saved_state.chk
```

### With Research Workflows
The tools are designed for research workflows:

1. **Experiment setup:** Configure and initialize brain
2. **Baseline metrics:** Run `nlm_metrics` to establish baseline
3. **Experiment execution:** Run main experiments
4. **Post-experiment analysis:** Use `nlm_info` and `nlm_metrics` to analyze results
5. **State preservation:** Use `nlm_save` to save experimental results
6. **State restoration:** Use `nlm_load` to resume experiments

## Scripting

These tools can be used in scripts for automated workflows:

```bash
#!/bin/bash

# Save current state
./nlm_save checkpoint_$(date +%Y%m%d_%H%M%S).chk

# Run analysis
./nlm_info > analysis_$(date +%Y%m%d_%H%M%S).txt

# Monitor performance in background
./nlm_metrics --interval 60 &
METRICS_PID=$!

# Run experiments...

# Kill monitoring
kill $METRICS_PID
```

## Troubleshooting

### Common Issues

1. **Configuration file not found**
   - Ensure the configuration file exists
   - Use absolute path or relative to current directory

2. **Brain initialization fails**
   - Check system resources
   - Verify configuration parameters
   - Check logs for error messages

3. **Checkpoint file corruption**
   - Try saving again
   - Check file permissions
   - Use a different file name

4. **Output format not supported**
   - Current supported formats: text, json, csv
   - Specify a supported format with `--format`

### Getting Help

- For tool-specific help: `./<tool_name> --help`
- For general usage: `./<tool_name> --help`
- For examples: Check this documentation file

## Future Enhancements

The tools can be extended with additional features:

1. **Advanced configuration options**
2. **Export/import utilities**
3. **Brain comparison tools**
4. **Visualization integration**
5. **Multi-tool workflows**
6. **Plugin architecture for custom tools**

## License

These tools are part of the NLM project and are licensed under the same terms as the main project.

## Contact

For questions or issues, refer to the main NLM documentation or project repository.
