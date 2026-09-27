# Basic Tutorial

## Welcome to NLM

This tutorial will guide you through your first experience with NLM (熙然), an experimental artificial developmental brain project. You'll learn how to set up, run, and analyze basic brain simulation experiments.

## Prerequisites

Before starting, ensure you have:

- C++ compiler with C++11 support
- CMake 3.10 or higher
- 4+ GB RAM (recommended)
- Build tools (make, cmake)

## Tutorial Overview

This tutorial covers:

1. **Setting up your development environment**
2. **Building NLM from source**
3. **Running your first experiment**
4. **Understanding the output**
5. **Exploring advanced features**

## Section 1: Development Environment Setup

### Step 1: Clone the Repository

First, get the NLM source code:

```bash
# Clone the repository
git clone https://github.com/your-username/nlm.git

# Navigate to the project directory
cd nlm
```

### Step 2: Build Instructions

Create a build directory and compile NLM:

```bash
# Create build directory (keep source separate from build files)
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project (use all available CPU cores)
make -j4
```

### Step 3: Verify Installation

Check if the build was successful:

```bash
# List the built executables
ls -la

# Run a simple test
./nlm --version
```

## Section 2: Running Your First Experiment

### The Phase 6 Demo

The Phase 6 demo is a comprehensive integration test that verifies all brain systems work together. It's perfect for beginners:

```bash
# Navigate to the build directory
# (assuming you're in the nlm project root)
cd build

# Run the Phase 6 demo (this may take a few minutes)
./nlm_phase6_demo
```

**What to Expect:**

- The demo will run for approximately 1000 simulation steps
- You'll see initialization messages for each brain system
- Final output will show system integration results
- Detailed statistics and performance metrics will be displayed

### Simple World Experiment

For a more focused experiment, try the Simple World environment:

```bash
# Using the nlm command directly with configuration
cd build
./nlm --config=../configs/simple_world.json --steps=100
```

## Section 3: Understanding the Output

### Key Outputs Explained

When NLM runs, you'll see several types of output:

#### 1. Initialization Messages

```
[INFO]  Initializing NLM brain...
[INFO]  Loading configuration...
[INFO]  Setting up memory systems...
[INFO]  Configuring plasticity rules...
[INFO]  Neuromodulation systems initialized...
```

#### 2. Simulation Progress

```
Step: 0/1000 | Time: 0.000s | Spikes: 42 | Memory: 15.2%
Step: 100/1000 | Time: 0.100s | Spikes: 387 | Memory: 17.8%
Step: 200/1000 | Time: 0.200s | Performance: 94.2%
```

#### 3. Final Statistics

At the end of an experiment, you'll see comprehensive results:

```
=== NLM Phase 6 Integration Results ===
Total Steps: 1000
Average Performance: 94.2%
Total Spikes: 2847
Memory Efficiency: 87.3%
Plasticity Events: 523
Neuromodulation Signals: 412

=== System Integration Status ===
✓ Neural Core: OPERATIONAL
✓ Memory Systems: OPERATIONAL  
✓ Prediction Systems: OPERATIONAL
✓ Cognitive Systems: OPERATIONAL
✓ Neuromodulation: OPERATIONAL
✓ Development System: OPERATIONAL

=== Performance Metrics ===
- Throughput: 3.2 MSps
- Memory Usage: 1.2 GB
- CPU Utilization: 45%
- Energy Efficiency: 87.5%
```

### Interpreting Results

#### Performance Metrics

- **Throughput**: Million spikes per second (MSps)
- **Memory Efficiency**: How effectively memory is used
- **Plasticity Events**: Number of synaptic modifications
- **Neuromodulation Signals**: Internal reward/error signals

#### System Health

Green checkmarks (✓) indicate operational systems. All systems should show as operational for a successful integration test.

## Section 4: Exploring Advanced Features

### Configuration Files

NLM uses JSON configuration files. Explore the configs directory:

```bash
cd nlm
ls configs/
```

Common configurations:

- `default.json` - Standard configuration
- `simple_world.json` - Basic environment setup
- `advanced.json` - Full-featured configuration

### Command Line Options

Learn about all available command-line options:

```bash
cd build
./nlm --help
```

Key options include:

- `--config [file]`: Load custom configuration
- `--steps [number]`: Run for specific number of steps
- `--output [file]`: Save results to file
- `--verbose`: Enable detailed logging

### Experiment Analysis

After running an experiment, you can analyze the results:

```bash
# Export results as CSV
cd build
./nlm --config=../configs/simple_world.json --export=csv

# Generate performance graphs
./nlm --config=../configs/simple_world.json --graph=performance

# Save brain state for later analysis
./nlm --config=../configs/simple_world.json --save=brain_state.json
```

## Section 5: Common Experiment Patterns

### Learning Patterns

#### 1. Single-Step Learning

Perfect for testing basic brain functionality:

```bash
./nlm --config=../configs/simple_world.json --steps=10
```

#### 2. Long-Term Development

For studying developmental changes:

```bash
./nlm --config=../configs/advanced.json --steps=10000 --save=developmental_path.json
```

#### 3. Comparative Analysis

Compare different configurations:

```bash
# Run experiment A
./nlm --config=config_a.json --steps=1000 --save=results_a.json

# Run experiment B
./nlm --config=config_b.json --steps=1000 --save=results_b.json

# Compare results
cd build
./compare_results.py results_a.json results_b.json
```

## Tutorial Summary

You now have:

✓ **NLM installed and built**
✓ **Running your first experiment**
✓ **Understanding basic output**
✓ **Exploring configuration options**
✓ **Starting to work with advanced features**

## Next Steps

1. **Try the Advanced Tutorial**: Explore more complex experiments
2. **Create Custom Configurations**: Design your own brain parameters
3. **Experiment with Different Environments**: Test various world setups
4. **Analyze Performance**: Study efficiency and optimization techniques
5. **Contribute to NLM**: Help improve the project documentation

## Resources

- **Documentation**: See the full user manual and API reference
- **Configuration Examples**: Browse the configs directory
- **Command Reference**: Use `--help` for detailed options
- **Community**: Join NLM discussion forums for questions

This tutorial provides a foundation for working with NLM. Remember that NLM is an experimental project - unexpected behaviors are normal and valuable for research!

Happy brain simulation!