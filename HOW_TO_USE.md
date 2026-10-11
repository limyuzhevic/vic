# NLM - How To Use

# Overview

NLM (熙然, "serene flow") is an experimental artificial developmental brain project. This document covers how to build, run, and extend NLM.

# Building

## Prerequisites

- CMake 3.16 or higher
- C++20 compiler (g++ 11+, clang 14+, MSVC 2022)
- Make or Ninja build system

## Standard Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Debug Build

```bash
mkdir debug_build
cd debug_build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_BUILD_TYPE=Debug
make -j4
```

## Install (Optional)

```bash
cd build
sudo make install
```

# Running Examples

## Phase 2: Basic Neural Computation

```bash
cd build/./nlm
```

This runs the Phase 2 demo showing real LIF neuron dynamics, synaptic plasticity, and learning experiments.

**Key Features Demonstrated:**
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays  
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Learning experiment with measurable synaptic changes

## Phase 3: World Interaction

```bash
cd build/./nlm_phase3_demo
```

This demo shows an agent learning to interact with a simple world through sensory input and motor output.

## Phase 4: Cognitive Development

```bash
cd build/./nlm_phase4_demo
```

This demonstrates cognitive abilities including planning, concept formation, and social learning.

## Phase 6: Full Integration

```bash
cd build/./nlm_phase6_demo
```

**Most Comprehensive Test:**
- Integration verification across all brain systems
- Memory systems (working + episodic) integration
- Neuromodulation (dopamine, curiosity, novelty) integration
- Prediction system integration
- Cognitive systems (planner, concept formation, attention) integration
- Development system integration
- Checkpoint save/load functionality
- Replay and consolidation capabilities

# Configuration

## File-Based Configuration

Use `configs/default.cfg` or create custom config files:

```
# NLM Configuration File
# neuron_count: Number of neurons (default: 500)
neuron_count = 1000

# region_count: Number of brain regions (default: 1)
region_count = 4

# connection_probability: Probability of synaptic connections (default: 0.15)
connection_probability = 0.2

# stdp_ltp_weight: STDP long-term potentiation weight (default: 0.02)
stdp_ltp_weight = 0.015

# stdp_ltd_weight: STDP long-term depression weight (default: 0.015)
stdp_ltd_weight = 0.018

# simulation_timestep: Simulation timestep in seconds (default: 0.001)
simulation_timestep = 0.0005

# random_seed: Random number generator seed (default: 42)
random_seed = 12345
```

## Command Line Override

```bash
./nlm --neuron_count=2000 --region_count=8 --connection_probability=0.25 --random_seed=99999
```

**Command Line Arguments:**
- `--config=file.cfg` - Load specific config file
- `--key=value` - Override specific config value
- `-k value` - Alternative short format

# Python Bindings

NLM includes Python bindings for programmatic control:

```python
import nlm_py

# Create brain configuration
config = nlm_py.Config()
config.set("neuron_count", 1000)
config.set("region_count", 4)

# Create and initialize brain
brain = nlm_py.Brain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)

# Get statistics
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate()}")
```

# Development and Extension

## Adding New Brain Regions

1. Create new header file in `src/core/Regions/`
2. Implement in corresponding `src/core/Regions/*.cpp`
3. Register in `src/brain/Brain.hpp`/`Brain.cpp`
4. Update CMakeLists.txt if needed

## Adding New Plasticity Rules

1. Create rule class inheriting from `PlasticityRule`
2. Implement `update()` method
3. Register in `PlasticityRule.hpp` namespace
4. Configure in brain initialization

## Adding New Modulators

1. Create new header in `src/neuromodulation/`
2. Implement integration with brain system
3. Update `Neuromodulator.hpp` factory pattern

## Running Custom Experiments

1. Create new experiment class inheriting from `Experiment`
2. Implement required virtual methods
3. Register in `ExperimentRunner.hpp`
4. Run via `Phase6IntegratedExperiment` or custom main

# Testing

## Running Unit Tests

```bash
cd tests
./test_runner
```

Individual test binaries:
- `./test_brain` - Brain system tests
- `./test_neuron` - Neuron dynamics tests
- `./test_synapse` - Synapse behavior tests
- `./test_stdp` - Spike-timing dependent plasticity
- `./test_clock` - Simulation timing
- `./test_config` - Configuration loading
- `./test_random` - Random number generation
- `./test_types` - Type system tests

## Running Integration Tests

```bash
cd build
./nlm_phase6_demo
```

This runs comprehensive integration tests verifying all systems work together.

# Troubleshooting

## Build Failures

**Missing CMake dependencies:**
```bash
sudo apt-get install build-essential cmake
# or on macOS:
brew install cmake
```

**Compiler errors:**
- Ensure C++20 support: `-std=c++20` or equivalent
- Check compiler version (g++ 11+, clang 14+)

## Runtime Errors

**"Failed to initialize brain!"**
- Check configuration values
- Verify system resources (memory, file descriptors)
- Ensure all required .cpp files are compiled

**Python bindings not found:**
- Ensure pybind11 is installed: `pip install pybind11`
- Build Python extensions: `python3 setup.py build_ext --inplace`

# Performance Optimization

## Large Networks

For networks >10,000 neurons:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_OPTIMIZATIONS=ON
```

## Debugging

Use debug build with logging:
```bash
cd debug_build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
cd ../build
./nlm --verbose
```

# Advanced Topics

## Custom Configuration Parsers

Replace the simple INI parser in `Config.cpp` with:
- JSON parser (nlohmann/json)
- YAML parser (yaml-cpp) 
- TOML parser (toml11)

## Memory Management

NLM uses custom memory pools:
- `MemoryPool.hpp` - General purpose allocation
- `EventQueue.hpp` - High-performance event handling
- SIMD vectorization for neural computations

## Parallel Processing

Enable for multi-core systems:
```bash
cmake .. -DNLM_PARALLEL=ON -DNLM_THREADS=8
```

# License

MIT License. See LICENSE file for details.

# Authors

Research project - See docs/ for scientific background and implementation details.