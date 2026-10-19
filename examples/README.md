# NLM CLI Examples Directory

This directory contains example scripts demonstrating the enhanced NLM command-line interface with all available modes and configurations.

## Overview

The NLM enhanced CLI provides six main execution modes:

1. **demo** - Phase 6 integration tests (basic connectivity, plasticity, STDP)
2. **learning** - Plasticity learning experiments with different input patterns
3. **benchmark** - Performance testing and timing measurements
4. **explore** - Curiosity-driven exploration with adaptive input
5. **compare** - Parameter comparison and sensitivity analysis
6. **test** - Basic functionality tests

Each example script demonstrates different aspects of using these modes, from basic usage to advanced configurations.

## How to Use

Make all scripts executable:

```bash
chmod +x examples/*.sh
```

Run the quick reference guide:

```bash
# Provides examples for all modes
examples/quick_reference.sh
```

Run specific examples:

```bash
# Basic demo example
examples/basic_demo.sh

# Learning experiment example
examples/learning_example.sh

# Benchmark example
examples/benchmark_example.sh

# Explore example
examples/explore_example.sh

# Compare example
examples/compare_example.sh
```

## Example Scripts

### 1. basic_demo.sh
Demonstrates basic demo mode usage:
- Default demo execution
- Demo with verbose output
- Demo with custom configuration
- Demo with validation

**Typical use:** Initial setup verification, educational demonstrations

### 2. learning_example.sh
Demonstrates learning experiments:
- Learning with different input patterns (random, sinusoidal, correlated, pulse)
- Batch learning tests
- Output to files

**Typical use:** Research experiments, parameter tuning, studying learning dynamics

### 3. benchmark_example.sh
Demonstrates performance benchmarking:
- Standard benchmark suite
- Custom benchmark with specific iterations
- Output to files
- Configuration file usage
- Validation

**Typical use:** Performance optimization, capacity planning, system monitoring

### 4. explore_example.sh
Demonstrates curiosity-driven exploration:
- Random exploration with different time limits
- Sinusoidal exploration
- Timeout handling
- Batch exploration
- Verbose output

**Typical use:** Behavioral research, curiosity studies, discovery experiments

### 5. compare_example.sh
Demonstrates parameter comparison:
- Basic parameter comparison
- Comparison with output files
- Sensitivity analysis
- Validation
- Batch parameter comparison

**Typical use:** Parameter optimization, sensitivity analysis, experimental design

### 6. quick_reference.sh
Comprehensive quick reference guide:
- Examples for all modes
- Help commands
- Configuration file operations
- Common patterns and use cases

**Typical use:** Quick start guide, reference documentation

## Configuration Files

The examples directory includes configuration files:

### configs/default.cfg
Default configuration file with basic settings

### configs/example.cfg
Example configuration for learning experiments

### configs/benchmark.cfg
Example configuration for benchmark tests

These configuration files can be used with the `--config` option:

```bash
./nlm --mode learning --config examples/configs/learning.cfg
```

## Testing and Validation

Each example script can be combined with validation:

```bash
# Validate configuration before running
./nlm --mode learning --pattern random --iterations 500 --validate

# Run example with validation
examples/learning_example.sh --validate
```

## Integration with Scripts

The example scripts can be integrated into larger workflows:

### Shell Script Integration

```bash
#!/bin/bash
# run_all_experiments.sh

echo "Running all NLM examples..."
examples/quick_reference.sh

# Run specific examples
examples/basic_demo.sh
echo "Demo completed"

eamples/learning_example.sh
echo "Learning completed"

eamples/benchmark_example.sh
echo "Benchmark completed"

echo "All examples completed successfully!"
```

### Python Integration

```python
#!/usr/bin/env python3
import subprocess
import os

def run_nlm_example(example_path):
    """Run an NLM example script"""
    result = subprocess.run(["bash", example_path], 
                          capture_output=True, text=True)
    return result.returncode, result.stdout, result.stderr

# Run basic demo
code, stdout, stderr = run_nlm_example("examples/basic_demo.sh")
if code == 0:
    print("Basic demo completed successfully")
    print(stdout)
else:
    print("Basic demo failed:")
    print(stderr)
```

## Common Patterns

### Pattern 1: Batch Processing

```bash
#!/bin/bash
# batch_processing.sh

# Run multiple experiments in parallel
for pattern in random sinusoidal correlated pulse; do
    ./nlm --mode learning --pattern $pattern --iterations 100 &
done

# Wait for all background jobs to complete
wait
```

### Pattern 2: Parameter Sweeps

```bash
#!/bin/bash
# parameter_sweep.sh

# Sweep through different parameter values
for neurons in 500 1000 2000; do
    for connections in 0.05 0.1 0.15; do
        ./nlm --mode learning \
            --neuron_count $neurons \
            --connection_probability $connections \
            --output "n${neurons}_c${connections}.txt"
    done
done
```

### Pattern 3: Output Processing

```bash
#!/bin/bash
# process_results.sh

# Run experiment and parse results
./nlm --mode benchmark --output raw_results.txt

# Extract specific metrics
python3 parse_benchmark_results.py raw_results.txt > summary.txt

# Generate plots
python3 plot_performance.py raw_results.txt performance_plot.png
```

## Troubleshooting

### Common Issues

**"Permission denied"**
```bash
# Make scripts executable
chmod +x examples/*.sh
```

**"Command not found: nlm"**
```bash
# Build NLM first
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

**"Unknown option"**
```bash
# Check available options
./nlm --help-options
```

### Debugging

```bash
# Run examples with verbose output
examples/quick_reference.sh --verbose

# Check logs
./nlm --mode learning --pattern random --iterations 500 --log-file learning.log

# Validate configuration
./nlm --validate --config examples/configs/learning.cfg
```

## Best Practices

1. **Always validate configurations** before running experiments
2. **Use output files** for saving results instead of console output
3. **Run benchmarks** regularly to monitor performance
4. **Test with different parameter sets** to understand system behavior
5. **Use batch processing** for large-scale experiments
6. **Monitor resources** during long-running experiments
7. **Document experiment parameters** for reproducibility

## Further Reading

- **easy_usage.md** - Beginner-friendly usage guide with examples
- **HOW_TO_USE.md** - Comprehensive documentation
- **TEST_FRAMEWORK.md** - Testing framework documentation
- **docs/ARCHITECTURE.md** - System architecture details

## Version Information

These examples are designed for NLM version 2.0 (enhanced CLI). They may require updates for future versions.

## License

These example scripts are provided as-is for demonstration purposes. They can be modified and extended as needed for specific use cases.