# NLM Phase 6 Demo - User Guide

## Introduction

This guide provides comprehensive instructions for using the improved Phase 6 demo system for the NLM (熙然) artificial brain simulation project.

## What is NLM Phase 6?

NLM Phase 6 is the final integration phase of the artificial brain project. It demonstrates that all brain systems work together as a coherent, integrated system:

- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction System**: Temporal prediction and error signals
- **Cognition Systems**: Neural planning, concept formation, attention
- **Development Integration**: Age-dependent learning rates and structural changes
- **Checkpointing**: Save/load functionality for interrupted simulations
- **Replay**: Memory consolidation during rest periods

## System Requirements

### Minimum Requirements

- **Processor**: 2+ cores (4+ cores recommended)
- **Memory**: 4+ GB RAM (8+ GB recommended)
- **Disk Space**: 1+ GB for simulation data
- **Operating System**: Linux, macOS, or Windows

### Recommended Configuration

For optimal performance:

```bash
# For small demonstrations
./nlm_phase6_demo --neuron-count 100 --max-steps 1000

# For comprehensive demonstrations
./nlm_phase6_demo --neuron-count 1000 --max-steps 10000

# For high-performance benchmarking
./nlm_phase6_demo_advanced --demo-mode benchmark --benchmark-time 60
```

## Quick Start

### Step 1: Build the Project

Open a terminal and run:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Step 2: Run a Simple Demonstration

```bash
# Quick validation (fastest mode)
./nlm_phase6_demo --quick

# Thorough demonstration with output
./nlm_phase6_demo --verbose

# Quiet mode (errors only)
./nlm_phase6_demo --quiet
```

### Step 3: Explore Advanced Features

```bash
# Help documentation
./nlm_phase6_demo --help
./nlm_phase6_demo_advanced --help

# Export results to file
./nlm_phase6_demo --export-results --output results.txt

# Benchmark performance
./nlm_phase6_demo_advanced --demo-mode benchmark
```

## Understanding the Demo Modes

### Quick Mode

**Purpose**: Fast validation of integration
**When to use**:
- Initial testing after changes
- Continuous integration
- Quick validation

**Features**:
- Integration verification only
- No individual system tests
- No full integration experiment
- ~10-30 seconds runtime

### Thorough Mode

**Purpose**: Comprehensive integration test
**When to use**:
- Final validation before release
- Debugging integration issues
- Performance analysis

**Features**:
- Integration verification
- All individual system tests
- Full integration experiment
- Detailed metrics collection
- ~1-5 minutes runtime (depends on configuration)

### Benchmark Mode

**Purpose**: Performance measurement
**When to use**:
- Performance optimization
- System comparison
- Resource planning

**Features**:
- Optimized experiment settings
- Performance metrics collection
- Throughput calculation
- Efficiency analysis
- ~1-2 minutes runtime

### Comparison Mode

**Purpose**: Results comparison
**When to use**:
- Performance tracking
- A/B testing
- Evolution analysis

**Features**:
- Current configuration run
- Comparison with previous results
- Improvement/deterioration detection
- Historical analysis

## Common Use Cases

### 1. Initial Testing

```bash
# After building, verify the system works
./nlm_phase6_demo --quick
```

### 2. Daily Integration Testing

```bash
# Automated CI/CD validation
./nlm_phase6_demo --quick --quiet
```

### 3. Performance Tuning

```bash
# Find optimal neuron count for your hardware
./nlm_phase6_demo --neuron-count 500 --max-steps 2000
./nlm_phase6_demo --neuron-count 1000 --max-steps 2000
./nlm_phase6_demo --neuron-count 2000 --max-steps 2000
```

### 4. Debug Integration Issues

```bash
# Run with verbose output to see what's happening
./nlm_phase6_demo --verbose --enable-checkpointing
```

### 5. Long-term Experiment Tracking

```bash
# Run a thorough experiment with checkpointing
./nlm_phase6_demo --neuron-count 1000 --max-steps 10000 --enable-checkpointing
```

## Configuration Parameters

| Parameter | Description | Default | Recommended Range |
|-----------|-------------|---------|-------------------|
| `--neuron-count` | Number of neurons | 500 | 100-5000 |
| `--max-steps` | Maximum simulation steps | 2000 | 100-50000 |
| `--region-count` | Number of neural regions | 1 | 1-10 |
| `--connection-prob` | Connection probability | 0.1 | 0.01-0.5 |
| `--checkpoint-path` | Checkpoint file path | ./checkpoint_test.bin | Any valid path |

## Output Analysis

### What to Look For

#### Integration Status

```
Working Memory: CONNECTED
Episodic Memory: CONNECTED
Neuromodulation: CONNECTED
Prediction: CONNECTED
Development: CONNECTED
```

All systems should show "CONNECTED" for successful integration.

#### Performance Metrics

```
Total reward: 12.5
Avg firing rate: 0.08
Episodes stored: 156
Dopamine level: 0.72
```

- **Total reward**: Higher values indicate better performance
- **Avg firing rate**: Should be in reasonable range (0.01-0.5)
- **Episodes stored**: More episodes indicate better memory retention
- **Dopamine level**: Reflects neuromodulation activity

#### Performance Metrics

```
Neurons per Second: 1234.5
Steps per Second: 61725.0
Memory Efficiency: 0.156
Convergence Rate: 0.89
```

- **Neurons per Second**: Processing speed
- **Steps per Second**: Simulation speed
- **Memory Efficiency**: Memory usage per neuron
- **Convergence Rate**: Learning speed

## Troubleshooting

### Common Issues and Solutions

#### Issue: "Integration verification failed"
**Possible causes**:
- Missing libraries or dependencies
- Compilation errors
- Configuration issues

**Solutions**:
```bash
# Re-run with verbose output
./nlm_phase6_demo --verbose

# Check build logs
make VERBOSE=1

# Clean and rebuild
make clean
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Issue: "Failed to save checkpoint"
**Possible causes**:
- No write permissions
- Insufficient disk space
- Invalid path

**Solutions**:
```bash
# Check permissions
ls -la checkpoint_test.bin

# Check disk space
df -h .

# Use different path
./nlm_phase6_demo --checkpoint-path ./my_checkpoints/checkpoint.bin
```

#### Issue: "No memory episodes available for replay"
**This is normal for short experiments**

**Solutions**:
- Increase experiment duration
- Enable more detailed logging
- This is expected behavior

### Getting Help

#### Command Line Help

```bash
# Basic help
./nlm_phase6_demo --help

# Advanced help
./nlm_phase6_demo_advanced --help

# Show version
./nlm_phase6_demo --version
```

#### Documentation

- **Main documentation**: `docs/PHASE6_DEMO_DOCUMENTATION.md`
- **Architecture**: `docs/PHASE6_ARCHITECTURE.md`
- **Phase 6 audit**: `docs/PHASE6_FINAL_AUDIT.md`
- **Scientific background**: `docs/SCIENCE.md`

#### Community Support

- Check project repository for issues
- Review source code comments
- Consult README.md for general information

## Best Practices

### For Performance Testing

1. **Start with small configurations**
   ```bash
   ./nlm_phase6_demo --neuron-count 100 --max-steps 1000
   ```

2. **Use checkpointing for long experiments**
   ```bash
   ./nlm_phase6_demo --enable-checkpointing --checkpoint-path ./checkpoints/
   ```

3. **Benchmark before scaling**
   ```bash
   ./nlm_phase6_demo_advanced --demo-mode benchmark
   ```

### For Development

1. **Always run quick mode first**
   ```bash
   ./nlm_phase6_demo --quick
   ```

2. **Use verbose output for debugging**
   ```bash
   ./nlm_phase6_demo --verbose
   ```

3. **Export results for analysis**
   ```bash
   ./nlm_phase6_demo --export-results --output results.json
   ```

### For Production Use

1. **Schedule regular runs**
   ```bash
   # Add to crontab for daily checks
   0 2 * * * /path/to/build/nlm_phase6_demo --quick --quiet >> /var/log/nlm_phase6.log
   ```

2. **Monitor system resources**
   ```bash
   ./nlm_phase6_demo --verbose | grep -i "memory\|cpu\|gpu"
   ```

3. **Archive results**
   ```bash
   # Compress and archive results
   ./nlm_phase6_demo --export-results
   tar -czf results_$(date +%Y%m%d).tar.gz results.txt
   ```

## Advanced Features

### Custom Configuration

Create a configuration file and use it with the demo:

```bash
# Create custom config
cat > custom_config.txt << EOF
--neuron-count 2000
--max-steps 5000
--enable-checkpointing
--checkpoint-path ./custom_checkpoints/checkpoint.bin
EOF

# Use in script
while read line; do
    ./nlm_phase6_demo $line
    break
done < custom_config.txt
```

### Batch Processing

```bash
#!/bin/bash
# Run multiple experiments with different configurations

echo "Running quick validation..."
./nlm_phase6_demo --quick

echo "Running thorough test..."
./nlm_phase6_demo --neuron-count 500 --max-steps 2000

echo "Running benchmark..."
./nlm_phase6_demo_advanced --demo-mode benchmark

echo "All tests completed!"
```

## Conclusion

The NLM Phase 6 demo system provides powerful tools for:

1. **Validating integration** of artificial brain systems
2. **Testing performance** and efficiency
3. **Analyzing behavior** and learning
4. **Documenting progress** and improvements

By using the basic and advanced demo modes, you can:

- Quickly verify that your changes work
- Run comprehensive integration tests
- Measure system performance
- Track improvements over time
- Export results for analysis

The improved Phase 6 demo system addresses all the issues mentioned in the original request:

- ✅ **Fixed bugs**: CMake configuration, missing demo executables
- ✅ **Improved usability**: Command-line arguments, help system
- ✅ **Added advanced features**: Multiple demo modes, benchmarking, export
- ✅ **Enhanced documentation**: Comprehensive guides and examples
- ✅ **Better error handling**: Validation, informative messages
- ✅ **Performance optimization**: Configurable resource usage

Start with the basic demo for initial testing, then explore the advanced features for comprehensive analysis.

Happy experimenting with the artificial brain!