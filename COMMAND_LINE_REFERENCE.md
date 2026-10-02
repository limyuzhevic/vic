# NLM Command Line Interface Reference

**Complete Guide to NLM Command Line Options**

## Overview

The NLM framework provides an enhanced command-line interface that supports both basic and advanced usage scenarios. This guide documents all available options for users ranging from beginners to advanced researchers.

## Basic Usage

### Help and Version

```bash
# Show general help
./nlm --help

# Show version information
./nlm --version
```

**Example Output:**
```
=== NLM Phase 2 - Neural Learning Machine ===
Version: 0.1.0
Phase: Real Neural Computation
Description: Experimental artificial developmental brain with real spiking neural computation.
Features: LIF neurons, event-driven spike propagation, STDP and Hebbian plasticity, structural plasticity.
Build: Debug (for development)
```

### Quick Start Examples

```bash
# Basic neural computation test (default configuration)
./nlm

# Run with custom neuron count
./nlm --neurons 1000

# Run with reduced steps for quick testing
./nlm --steps 20

# Run with all plasticity disabled
./nlm --no-plasticity --no-stdp --no-hebbian --no-structural

# Run with enhanced logging
./nlm --log-level debug --log-file nlm_debug.log

# Quick test with specific random seed
./nlm --random-seed 12345

# Run with custom configuration file
./nlm --config my_config.cfg
```

## Experimental Control Options

### Plasticity Control

```bash
# Enable all plasticity (default)
./nlm --plasticity --stdp --hebbian --structural

# Enable only specific plasticity types
./nlm --plasticity --stdp

# Disable all plasticity
./nlm --no-plasticity --no-stdp --no-hebbian --no-structural

# Enable plasticity with specific weights
./nlm --plasticity --stdp-ltp 0.03 --stdp-ltd 0.025
```

### Configuration File Management

```bash
# Load custom configuration
./nlm --config path/to/config.json

# Load and override configuration values
./nlm --config base.cfg --neurons 2000 --timestep 0.01

# View configuration file format
# Check docs/CONFIG_FORMAT.md for details
```

### Logging and Output Control

```bash
# Different log levels
./nlm --log-level debug      # All debug information
./nlm --log-level info       # Normal operation info
./nlm --log-level warning    # Only warnings and errors
./nlm --log-level error      # Only errors
./nlm --log-level critical   # Only critical errors

# Log to file
./nlm --log-file simulation.log --log-level info

# Suppress informational messages (quiet mode)
./nlm --quiet

# Combined logging options
./nlm --log-level debug --log-file debug.log --quiet
```

### Performance and Optimization

```bash
# Set simulation timestep
./nlm --timestep 0.001        # 1ms timestep (default)
./nlm --timestep 0.01         # 10ms timestep (faster, less accurate)

# Enable parallel processing (if available)
./nlm --parallel

# Enable SIMD vectorization
./nlm --vectorize

# Performance monitoring
./nlm --enable-performance-monitoring
```

### Random Seed and Reproducibility

```bash
# Set random seed for reproducible experiments
./nlm --random-seed 42

# Generate seed for random testing
./nlm --generate-random-seed

# Use time-based seed (different each run)
./nlm --random-seed-time
```

## Advanced Options

### Configuration Override

```bash
# Override individual configuration parameters
./nlm \
  --config base.cfg \
  --neurons 5000 \
  --connection-prob 0.05 \
  --stdp-ltp 0.015 \
  --stdp-ltd 0.01 \
  --stdp-tau 30.0
```

### Experimental Scenarios

#### Scenario 1: Fast Testing
```bash
./nlm --neurons 100 --steps 10 --quiet \
  --no-plasticity --no-structural
```

#### Scenario 2: High Plasticity Research
```bash
./nlm --neurons 2000 --steps 1000 \
  --plasticity --stdp --hebbian --structural \
  --stdp-ltp 0.05 --stdp-ltd 0.04 --stdp-tau 50.0 \
  --timestep 0.0001
```

#### Scenario 3: Memory System Focus
```bash
./nlm --neurons 1500 --steps 2000 \
  --plasticity --stdp --hebbian \
  --enable-memory-integration --memory-capacity 10000
```

#### Scenario 4: Neuromodulation Study
```bash
./nlm --neurons 1000 --steps 500 \
  --plasticity --stdp --hebbian \
  --enable-neuromodulation \
  --dopamine-baseline 0.2 \
  --curiosity-level 0.1
```

### Batch Processing

```bash
# Process multiple configurations
for seed in 1 2 3 4 5; do
    ./nlm --random-seed $seed \
          --neurons 1000 \
          --steps 100 \
          --output-dir results/seed_$seed \
          --log-file results/seed_$seed/log.txt
    echo "Completed seed $seed"
done
```

### Integration with Analysis Tools

```bash
# Run experiment and pipe output to analysis tool
./nlm --neurons 1000 --steps 1000 \
  | python3 analysis_tool.py --plot-firing-rates

# Run with checkpointing for later analysis
./nlm --neurons 2000 --steps 2000 \
  --enable-checkpointing \
  --checkpoint-dir checkpoints/

# Resume from checkpoint
./nlm --resume-checkpoint latest_checkpoint.bin
```

## Option Reference

### Basic Options

| Option | Short Form | Description | Default |
|--------|-------------|-------------|---------|
| `--help` | -h | Show help information | - |
| `--version` | -v | Show version information | - |

### Experiment Options

| Option | Description | Default |
|--------|-------------|---------|
| `--neurons <N>` | Number of neurons | 500 |
| `--steps <N>` | Number of simulation steps | 50 |
| `--plasticity` | Enable plasticity experiments | Enabled |
| `--no-plasticity` | Disable plasticity experiments | - |
| `--stdp` | Enable STDP experiments | Enabled |
| `--no-stdp` | Disable STDP experiments | - |
| `--hebbian` | Enable Hebbian experiments | Enabled |
| `--no-hebbian` | Disable Hebbian experiments | - |
| `--structural` | Enable structural plasticity | Enabled |
| `--no-structural` | Disable structural plasticity | - |

### Configuration Options

| Option | Description | Default |
|--------|-------------|---------|
| `--config <file>` | Load configuration from file | configs/default.cfg |
| `--random-seed <S>` | Random seed for reproducibility | 42 |
| `--connection-prob <P>` | Connection probability (0.0-1.0) | 0.15 |
| `--timestep <T>` | Simulation timestep in milliseconds | 0.001 |

### Plasticity Parameters

| Option | Description | Default |
|--------|-------------|---------|
| `--stdp-ltp <W>` | STDP LTP weight | 0.02 |
| `--stdp-ltd <W>` | STDP LTD weight | 0.015 |
| `--stdp-tau <T>` | STDP time constant | 20.0 |
| `--hebbian-rate <R>` | Hebbian learning rate | 0.01 |

### Logging Options

| Option | Description | Default |
|--------|-------------|---------|
| `--log-level <level>` | Log level (debug, info, warning, error, critical) | info |
| `--log-file <file>` | Log output to file | - |
| `--quiet` | Suppress informational messages | Disabled |

### Performance Options

| Option | Description | Default |
|--------|-------------|---------|
| `--parallel` | Enable parallel processing | Disabled |
| `--vectorize` | Enable SIMD vectorization | Disabled |
| `--enable-performance-monitoring` | Enable performance profiling | Disabled |

### Debug and Testing Options

| Option | Description | Default |
|--------|-------------|---------|
| `--debug` | Enable debug mode | Disabled |
| `--validate-config` | Validate configuration only | Disabled |
| `--generate-random-seed` | Generate and display random seed | Disabled |
| `--test-plasticity-rules` | Test only plasticity rules | Disabled |

## Configuration File Format

### Basic Configuration (JSON)

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.15,
        "plasticity": {
            "stdp": {
                "enabled": true,
                "ltp_weight": 0.02,
                "ltd_weight": 0.015,
                "time_constant": 20.0
            },
            "hebbian": {
                "enabled": true,
                "learning_rate": 0.01
            },
            "structural": {
                "enabled": true,
                "synaptogenesis_rate": 0.0001,
                "pruning_rate": 0.00001
            }
        }
    },
    "memory": {
        "working_memory": {
            "capacity": 500,
            "decay_rate": 0.95
        }
    },
    "neuromodulation": {
        "dopamine": {
            "baseline": 0.1,
            "phasic_response": true
        }
    }
}
```

### Command Line Override Syntax

```bash
# Override configuration values from command line
./nlm --config config.json \
  --neurons 2000 \
  --connection-prob 0.05 \
  --stdp-ltp 0.015 \
  --stdp-ltd 0.01
```

## Output Formats

### Default Output

```
=== NLM Phase 2 - Neural Learning Machine ===
Real neural computation with LIF dynamics and plasticity.

===========================================
=== NLM Phase 2: Real Neural Computation ===
===========================================
Initializing NLM Phase 2 Real Neural Computation...
... (simulation output) ...

===========================================
=== Final Brain Status ===
===========================================
... (final statistics) ...

===========================================
=== Phase 2 Complete ===
===========================================
Phase 2 Objectives Completed:
  ✓ Real LIF neuron dynamics implemented
  ✓ Event-driven spike propagation with delays
  ✓ STDP plasticity rule
  ✓ Hebbian plasticity rule
  ✓ Structural plasticity (synaptogenesis/pruning)
  ...
```

### Log File Format

```
[2024-01-01 12:00:00.000] INFO: === NLM Phase 2: Real Neural Computation ===
[2024-01-01 12:00:00.001] INFO: Implementing:
[2024-01-01 12:00:00.002] INFO:   - Leaky Integrate-and-Fire (LIF) neuron dynamics
[2024-01-01 12:00:00.003] INFO:   - Event-driven spike propagation with delays
[2024-01-01 12:00:00.004] INFO:   - STDP and Hebbian plasticity rules
[2024-01-01 12:00:00.005] INFO:   - Structural plasticity (synaptogenesis/pruning)
```

## Exit Codes

| Code | Description |
|------|-------------|
| 0 | Success |
| 1 | General error |
| 2 | Configuration error |
| 3 | Memory allocation error |
| 4 | Invalid command line arguments |
| 5 | Plasticity system error |

## Troubleshooting

### Common Issues

#### Issue: Command line options not recognized
**Solution**: Ensure you're using the correct option format:
```bash
# Incorrect
./nlm --neurons=1000

# Correct
./nlm --neurons 1000
```

#### Issue: Too many neurons causing memory issues
**Solution**: Reduce neuron count or enable memory pooling:
```bash
./nlm --neurons 500 --enable-memory-pooling
```

#### Issue: Configuration file not found
**Solution**: Check file path or use absolute path:
```bash
./nlm --config /absolute/path/to/config.cfg
```

#### Issue: Simulation running too slowly
**Solution**: Adjust timestep or enable optimizations:
```bash
./nlm --timestep 0.01 --parallel --vectorize
```

### Debug Scripts

#### Command Line Option Tester

```bash
#!/bin/bash
# test_cli_options.sh

# Test all command line options
./nlm --help > help_output.txt 2>&1
echo "Help test: $(grep -c "Usage:" help_output.txt) lines of help"

# Test version
./nlm --version > version_output.txt 2>&1
echo "Version test: $(cat version_output.txt)"

# Test invalid option
./nlm --invalid-option 2>&1 | grep -q "Unknown argument" && echo "Invalid option test: PASSED"
```

#### Performance Comparison

```bash
#!/bin/bash
# compare_performance.sh

echo "Performance comparison for different configurations:"
echo "========================================="

# Configuration 1: Small/fast
./nlm --neurons 100 --steps 10 --quiet \
  --no-plasticity --no-structural \
  --timestep 0.01 2>&1 | grep -E "(Time:|Steps:|Neurons:)" | head -3

# Configuration 2: Medium
./nlm --neurons 500 --steps 50 --quiet \
  --plasticity --stdp \
  --timestep 0.001 2>&1 | grep -E "(Time:|Steps:|Neurons:)" | head -3

# Configuration 3: Large
./nlm --neurons 2000 --steps 200 --quiet \
  --plasticity --stdp --hebbian --structural \
  --timestep 0.001 2>&1 | grep -E "(Time:|Steps:|Neurons:)" | head -3
```

## Integration with External Tools

### Python Analysis

```bash
# Run NLM simulation and pipe results to Python script
./nlm --neurons 1000 --steps 1000 --log-file simulation.log \
  | python3 analyze_results.py --plot --format pdf

# Run with checkpoint and resume
./nlm --neurons 2000 --steps 5000 --enable-checkpointing \
  --checkpoint-dir checkpoints/ \
  | tee simulation_output.log

# Resume from latest checkpoint
./nlm --resume-checkpoint checkpoints/latest.bin \
  --neurons 2000  # Override if needed
```

### MATLAB Analysis

```bash
# Export results in MATLAB format
./nlm --neurons 1000 --steps 1000 --output-format matlab > results.mat

# Use with MATLAB script
matlab -batch "load('results.mat'); analyze_neural_data;"
```

### R Statistical Analysis

```bash
# Export for R analysis
./nlm --neurons 1000 --steps 1000 --output-format r > r_data.csv

# Use with R script
Rscript analyze_r_data.R r_data.csv --plot
```

## License and Usage Terms

This command line interface is part of the NLM framework under the MIT License. Users are encouraged to:

1. Use command line options for flexible configuration
2. Create configuration files for complex experiments
3. Use batch processing for systematic parameter sweeps
4. Share custom configurations with the community

For more information, visit: https://github.com/nlm-project/nlm

---

**Last Updated**: $(date)  
**Version**: 0.1.0  
**Documentation**: NLM Command Line Interface Reference