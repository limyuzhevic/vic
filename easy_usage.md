# Quick Start - NLM Command Line Interface

## Getting Started with NLM

NLM (Neural Learning Machine) is an experimental brain simulation framework. This guide shows you how to use the new Command Line Interface (CLI) to run experiments and explore the system.

## Installation

If you haven't built NLM yet, follow these steps:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Quick Commands

### 1. Try the Basic Test
```bash
./nlm_cli basic
```
This runs a simple connectivity test to verify NLM is working.

### 2. Run Learning Experiments
```bash
./nlm_cli plasticity
```
This demonstrates how neural networks learn through experience.

### 3. Test Plasticity Mechanisms
```bash
./nlm_cli stdp
```
This verifies spike-timing dependent plasticity (LTP/LTD).

### 4. Performance Benchmark
```bash
./nlm_cli benchmark
```
This measures how fast NLM can simulate.

### 5. Integration Test
```bash
./nlm_cli integration --enable-replay --enable-development
```
This runs Phase 6 integration test with all brain systems.

## Advanced Usage

### Custom Configuration
Create a config file `my_config.cfg`:
```
neuron_count = 1000
connection_probability = 0.2
stdp_ltp_weight = 0.03
```

Run with custom config:
```bash
./nlm_cli basic --config my_config.cfg
```

### Multiple Experiments
```bash
# Run all experiments in sequence
./nlm_cli auto

# Run specific experiment sizes
./nlm_cli basic --neurons 2000 --steps 10000
./nlm_cli plasticity --neurons 1500 --steps 5000
```

### Help and Information
```bash
# List all commands
./nlm_cli --list-commands

# Get help for a specific command
./nlm_cli basic --help

# Show all options
./nlm_cli --show-advanced-help
```

## Example Experiment Script

Create a script file `quick_test.nml`:
```bash
#!/bin/bash

# Quick test script for NLM

echo "=== NLM Quick Test ==="

# Test basic connectivity
./nlm_cli basic --neurons 100 --steps 500

# Test learning
./nlm_cli plasticity --neurons 200 --steps 1000

# Test plasticity
./nlm_cli stdp --neurons 150 --steps 300

echo "=== All tests completed ==="
```

Make it executable:
```bash
chmod +x quick_test.nml

# Run it
./quick_test.nml
```

## Understanding the Results

### Basic Mode Output
Shows spike propagation through the neural network. Look for:
- "Spikes generated: N" - number of spikes
- "✓ Connectivity test passed" - successful propagation

### Plasticity Mode Output
Shows learning through synaptic weight changes. Look for:
- "Learning detected: Synaptic weights changed through experience"
- Weight changes showing strengthening/weakening

### STDP Mode Output
Shows spike-timing dependent plasticity. Look for:
- "STDP working: Pre-before-post produced potentiation"
- Weight deltas (Δ) showing changes

### Benchmark Mode Output
Shows performance metrics. Look for:
- "Average steps per second: X"
- Performance assessment (Excellent/Good/Moderate/Low)

## Troubleshooting

### Command not found
Make sure you're running from the build directory:
```bash
cd build
./nlm_cli basic
```

### Configuration errors
Check your config file syntax and ensure it exists:
```bash
ls configs/
cat configs/default.cfg
```

### Memory issues
Reduce neuron count for smaller systems:
```bash
./nlm_cli basic --neurons 100 --steps 100
```

### Help not showing
Ensure you have all dependencies installed:
```bash
make -j4  # Reinstall if needed
```

## Next Steps

1. **Experiment with different parameters**
   - Try different neuron counts
   - Experiment with plasticity weights
   - Test with different step counts

2. **Create custom configurations**
   - Modify the configs/default.cfg file
   - Create specialized configs for different experiments

3. **Learn the systems**
   - Run integration tests to understand all brain systems
   - Study how memory, neuromodulation, and cognition work together

4. **Performance optimization**
   - Use benchmark mode to find optimal settings
   - Experiment with parallel processing options

## Quick Reference

| Command | Description | Typical Use |
|---------|-------------|-------------|
| `nlm_cli basic` | Basic connectivity test | System verification |
| `nlm_cli plasticity` | Learning experiments | Studying plasticity |
| `nlm_cli stdp` | STDP verification | Testing LTP/LTD |
| `nlm_cli benchmark` | Performance measurement | Optimization |
| `nlm_cli integration` | Full integration test | System validation |
| `nlm_cli auto` | All experiments | Complete testing |

That's it! You're ready to start exploring the NLM neural simulation system. Have fun experimenting with artificial brain dynamics! 🎯