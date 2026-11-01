# NLM (熙然) Enhanced System - Quick Start Guide

## Overview

This guide provides a quick introduction to the enhanced NLM (Neural Learning Machine) system, covering the main features and how to get started with the most common tasks.

## System Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. The enhanced version provides a unified interface for running demonstrations, tests, and experiments with advanced features including:

- Unified demo framework with command-line interface
- Enhanced configuration management
- Comprehensive logging and progress monitoring
- Performance profiling capabilities
- Checkpoint and recovery management
- Brain state visualization

## Getting Started

### Installation

The enhanced NLM system is pre-built and ready to use. The executables are available in the `build/` directory or can be run directly from the source:

```bash
# Change to the NLM directory
# Build and run demos:
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Then run demos:
./nlm_demo          # Enhanced unified demo system
./nlm_enhanced      # Enhanced main demo
./nlm_benchmark     # Performance benchmark
./nlm_visualization  # Brain state visualization
./nlm_checkpoint    # Checkpoint management
```

### First Demo Run

Let's run a simple demo to get started:

```bash
# Run basic connectivity demo
./nlm_demo --demo=basic --steps=1000 --verbose

# Run plasticity learning demo
./nlm_demo --demo=plasticity --config=configs/default.cfg

# Run all demos sequentially
./nlm_demo --demo=all --progress --output=results.txt
```

## Core Commands

### Demo Commands

| Command | Description | Example |
|---------|-------------|---------|
| `--demo=basic` | Test basic connectivity and spike propagation | `./nlm_demo --demo=basic --steps=1000` |
| `--demo=plasticity` | Demonstrate synaptic plasticity and learning | `./nlm_demo --demo=plasticity --verbose` |
| `--demo=stdp` | Verify spike-timing dependent plasticity | `./nlm_demo --demo=stdp --steps=500` |
| `--demo=phase6` | Run Phase 6 integration test | `./nlm_demo --demo=phase6` |
| `--demo=text` | Text teaching and concept formation demo | `./nlm_demo --demo=text` |
| `--demo=benchmark` | Run performance benchmark | `./nlm_demo --demo=benchmark` |
| `--demo=all` | Run all available demos | `./nlm_demo --demo=all --progress` |

### Configuration Commands

```bash
# List available config files
./nlm_demo --list-configs

# Run demo with specific config
./nlm_demo --demo=plasticity --config=my_custom_config.cfg

# Show help
./nlm_demo --help
```

### Output and Results

The enhanced system generates comprehensive output including:

- Console output with real-time progress
- Result files in `demo_results_YYYYMMDD_HHMMSS.txt`
- Performance profiles in `benchmark_profile_YYYYMMDD_HHMMSS.json`
- Progress logs in `demo_progress_YYYYMMDD_HHMMSS.log`

## Advanced Features

### Performance Benchmarking

Run advanced performance benchmarks with detailed metrics:

```bash
# Standard benchmark
./nlm_benchmark

# Enhanced benchmark with profiling
./nlm_benchmark_enhanced --bench=100:1000,500:1000,1000:500

# Custom benchmark configuration
./nlm_benchmark_enhanced --output=perf_results.txt --format=json
```

### Checkpoint Management

Manage brain state checkpoints:

```bash
# Create checkpoint
./nlm_checkpoint --create --brain-id=my_brain

# Load checkpoint
./nlm_checkpoint --load --checkpoint-path=./checkpoint.bin

# List checkpoints
./nlm_checkpoint --list

# Cleanup old checkpoints
./nlm_checkpoint --cleanup
```

### Brain State Visualization

Monitor brain state in real-time:

```bash
# Start visualization
./nlm_visualization --start

# Generate visualization report
./nlm_visualization --output=visualization.json --format=json
```

## Configuration Management

### Configuration File Format

NLM uses INI-style configuration files:

```ini
# Example configuration file
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
log_level = INFO
```

### Configuration Override

Command-line arguments override configuration file settings:

```bash
# Configuration file sets neuron_count = 1000
# Command-line overrides it to 2000
./nlm_demo --demo=basic --config=config.cfg --neuron-count=2000
```

## Troubleshooting

### Common Issues

**Issue**: Demo fails to start
**Solution**: Check configuration files for syntax errors and verify required files exist.

**Issue**: Performance is slow
**Solution**: Increase simulation timestep, reduce network size, or disable visualization.

**Issue**: Checkpoint fails to save
**Solution**: Check disk space and file permissions.

**Issue**: Visualization doesn't display
**Solution**: Check terminal capabilities and disable colors if needed.

### Error Messages

- `Error: Configuration validation failed` - Fix configuration file syntax
- `Error: Brain initialization failed` - Check system resources
- `Error: Checkpoint save failed` - Verify disk space and permissions
- `Error: Visualization failed to start` - Check terminal settings

## Frequently Asked Questions

### Q: What are the differences between the demo systems?
A: 
- `nlm_demo`: Unified demo framework with advanced features
- `nlm`: Original Phase 2 demo (legacy)
- `nlm_enhanced`: Enhanced main demo with improved architecture
- `nlm_benchmark`: Basic performance benchmark
- `nlm_benchmark_enhanced`: Advanced benchmark with profiling
- `nlm_visualization`: Brain state visualization
- `nlm_checkpoint`: Enhanced checkpoint management

### Q: How do I run multiple demos?
A: Use `--demo=all` to run all demos sequentially, or run demos individually in sequence.

### Q: How do I customize the demos?
A: Create custom configuration files and use the `--config` option, or use command-line overrides like `--steps=<n>` and `--neuron-count=<n>`.

### Q: How do I monitor performance?
A: Use `./nlm_benchmark` for basic benchmarking, or `./nlm_benchmark_enhanced` for advanced performance profiling.

### Q: How do I save and restore experiments?
A: Use `./nlm_checkpoint` to create and load checkpoints, or use the `--checkpoint` option in demo commands.

## Example Workflow

Here's a complete example workflow:

```bash
#!/bin/bash
# Run comprehensive test suite
./nlm_demo --demo=all --progress --output=all_results.txt

# Extract specific metrics
./nlm_demo --demo=plasticity --output=plasticity_metrics.txt

# Run performance benchmark
./nlm_benchmark_enhanced --bench=100:1000,500:1000,1000:500

# Monitor system resources during demo
./nlm_visualization --start &
viz_pid=$!
./nlm_demo --demo=basic
sleep 10
kill $viz_pid

# Cleanup old checkpoints
./nlm_checkpoint --cleanup

# Generate final report
./nlm_demo --demo=phase6 --output=final_report.txt
```

## Next Steps

1. **Run the basic demo** to understand the system
2. **Explore configuration files** to customize experiments
3. **Run benchmarks** to measure performance
4. **Set up visualization** for real-time monitoring
5. **Implement checkpointing** for long-running experiments
6. **Document your findings** and share results

## Additional Resources

- **Documentation**: See `docs/ENHANCED_NLM_DOCUMENTATION.md` for comprehensive guide
- **Examples**: Check `examples/` directory for sample configurations and scripts
- **Community**: Join NLM forums for support and discussions
- **Bug Reports**: Submit issues through the issue tracker
- **Updates**: Check for system updates and enhancements

## Conclusion

The enhanced NLM system provides a powerful, flexible framework for neural learning research and experimentation. With its unified interface, advanced features, and comprehensive documentation, it supports a wide range of use cases from simple connectivity tests to complex cognitive learning experiments.

Start exploring the system today and discover the capabilities of artificial developmental brains!

---

*For more information, visit the NLM project documentation or contact the development team.*
