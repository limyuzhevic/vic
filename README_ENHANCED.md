# NLM (Neural Learning Machine) - Enhanced Version

## Welcome to NLM Phase 6 Enhanced Edition!

This is the enhanced version of the NLM neural simulation framework with comprehensive improvements in code structure, user interface, debugging capabilities, visualization tools, performance benchmarking, and error handling.

## Key Improvements

### 🚀 **Enhanced Code Structure**
- **Modular Architecture**: System coordinators and helper classes for better separation of concerns
- **Reduced Complexity**: Monolithic Brain.cpp broken into focused, testable components
- **Improved Maintainability**: Clear interfaces and better code organization

### 🎮 **Advanced CLI Interface**
- **Multiple Experiment Modes**: Demo, benchmark, full experiment, debug
- **Preset Configurations**: Tiny, small, medium, large, research presets
- **Comprehensive Options**: Configuration, monitoring, visualization, and debugging options
- **Help System**: Detailed help with categorized options

### 🔍 **Enhanced Debugging and Monitoring**
- **Real-time System State**: Continuous monitoring of all brain systems
- **Performance Metrics**: Detailed performance tracking and analysis
- **Health Monitoring**: System health checks and automatic diagnostics
- **Structured Logging**: Multiple log levels and output formats

### 📊 **Visualization and Output Tools**
- **Real-time Visualization**: Neural activity and system state monitoring
- **Data Export**: JSON/CSV export for external analysis
- **Multiple Output Formats**: Console, file, and structured output
- **Interactive Monitoring**: Real-time system status display

### ⚡ **Performance Benchmarking**
- **Execution Time Measurement**: Precise timing of simulation steps
- **Throughput Analysis**: Steps per second and spikes per second metrics
- **Memory Usage Tracking**: Memory consumption monitoring
- **Performance Profiling**: Detailed performance analysis

### 🛡️ **Robust Error Handling**
- **Input Validation**: Comprehensive parameter validation
- **Graceful Degradation**: System continues operating with degraded functionality
- **Exception Handling**: Robust exception catching and recovery
- **System Health Checks**: Automatic system state validation

## Quick Start Guide

### **Installation and Building**
```bash
# Build the enhanced version
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### **Basic Usage**
```bash
# Run demo with enhanced features
./nlm --demo

# Run with custom configuration
./nlm --neuron-count 2000 --timestep 0.0005 --regions 3

# Run benchmark
./nlm --benchmark --neuron-count 5000

# Run full experiment
./nlm --experiment --preset research

# Run debug mode
./nlm --debug --log-level DEBUG --monitor
```

### **Python API**
```python
import pynlm

# Create brain with configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.15)

brain = pynlm.createBrain(config)
brain.initialize()

# Enhanced monitoring
state = brain.getSystemState()
print(f"Neurons: {state.neural.totalNeurons}")
print(f"Synapses: {state.neural.totalSynapses}")
print(f"Working memory: {state.memory.workingMemoryTraces}")
print(f"Dopamine level: {state.neuromodulation.dopamineLevel}")

# Run simulation
for step in range(100):
    brain.step(step)
```

## Experiment Modes

### **1. Demo Mode** (`--demo`)
- **Purpose**: Quick demonstration of enhanced features
- **Features**: System monitoring, real-time logging, educational output
- **Usage**: `./nlm --demo`

### **2. Benchmark Mode** (`--benchmark`)
- **Purpose**: Performance benchmarking and optimization
- **Features**: Detailed performance metrics, memory tracking, throughput analysis
- **Usage**: `./nlm --benchmark --neuron-count 5000`

### **3. Full Experiment Mode** (`--experiment`)
- **Purpose**: Comprehensive experiment suite
- **Features**: Multiple experiment phases, detailed analysis, system validation
- **Usage**: `./nlm --experiment --preset research`

### **4. Debug Mode** (`--debug`)
- **Purpose**: Enhanced debugging and development
- **Features**: Detailed logging, system health checks, anomaly detection
- **Usage**: `./nlm --debug --log-level DEBUG --monitor`

## Configuration Options

### **Basic Configuration**
```bash
# Set basic parameters
./nlm --neuron-count 2000 --timestep 0.0005 --regions 3

# Enable specific systems
./nlm --plasticity --neuromodulation --development

# Run with checkpoint
./nlm --checkpoint my_checkpoint.bin
```

### **Advanced Configuration**
```bash
# Use custom configuration file
./nlm --config my_config.json

# Enable monitoring and profiling
./nlm --monitor --profile --export-data results.json

# Run script-based experiment
./nlm --script custom_experiment.py
```

### **Preset Configurations**
```bash
# Tiny configuration (50 neurons)
./nlm --preset tiny

# Research configuration (50,000 neurons, all features)
./nlm --preset research
```

## System Monitoring

### **Real-time System State**
The enhanced NLM provides comprehensive system state monitoring:

```python
# Get current system state
state = brain.getSystemState()

# Neural system metrics
print(f"Total Neurons: {state.neural.totalNeurons}")
print(f"Total Synapses: {state.neural.totalSynapses}")
print(f"Active Neurons: {state.neural.activeNeurons}")
print(f"Firing Neurons: {state.neural.firingNeurons}")
print(f"Average Firing Rate: {state.neural.averageFiringRate}")
print(f"E/I Ratio: {state.neural.excitationInhibitionRatio}")

# Memory system metrics
print(f"Working Memory Traces: {state.memory.workingMemoryTraces}")
print(f"Episodic Memory Episodes: {state.memory.episodicMemoryEpisodes}")

# Neuromodulation metrics
print(f"Dopamine Level: {state.neuromodulation.dopamineLevel}")
print(f"Curiosity Level: {state.neuromodulation.curiosityLevel}")
print(f"Novelty Level: {state.neuromodulation.noveltyLevel}")

# Development system metrics
print(f"Developmental Stage: {state.development.stage}")
print(f"Plasticity Modulation: {state.development.plasticityModulation}")
```

## Performance Features

### **Benchmark Results**
```
=== BENCHMARK RESULTS ===
Total simulation time: 12.5 seconds
Steps per second: 80.0
Spikes per second: 2500.0
Total spikes generated: 50000
Average firing rate: 40.0 Hz
E/I ratio: 1.2
Memory traces: 500
Memory episodes: 100
Dopamine level: 0.5
Curiosity level: 0.3
Novelty level: 0.8
Developmental stage: 3 (Adult)
```

### **Monitoring Options**
```bash
# Monitor neural system
./nlm --monitor-neural --neuron-count 1000

# Monitor memory system
./nlm --monitor-memory --neuron-count 2000

# Monitor cognition system
./nlm --monitor-cognition --neuron-count 3000

# Monitor development system
./nlm --monitor-development --neuron-count 1500

# Monitor plasticity
./nlm --monitor-plasticity --neuron-count 2500

# Monitor neuromodulation
./nlm --monitor-neuromodulation --neuron-count 1000
```

## Visualization Features

### **Available Visualizations**
- **Neural Activity Plotting**: Real-time neural activity over time
- **Memory System Visualization**: Working memory and episodic memory graphs
- **Plasticity Visualization**: Synaptic weight change tracking
- **Neuromodulation Visualization**: Dopamine, curiosity, and novelty level plots

### **Data Export**
```bash
# Export neural data
./nlm --export-data neural_data.json --plot-neural

# Export memory data
./nlm --export-data memory_data.csv --plot-memory

# Export plasticity data
./nlm --export-data plasticity_data.json --plot-plasticity

# Export neuromodulation data
./nlm --export-data neuromodulation_data.json --plot-neuromodulation
```

## Debugging Features

### **Debug Mode**
```bash
# Run with debug level
./nlm --debug --log-level DEBUG

# Run with enhanced monitoring
./nlm --debug --monitor --profile

# Run with system health checks
./nlm --debug --check-health --log-level INFO
```

### **Error Handling**
```python
# Enhanced error handling in Python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

if not brain.initialize():
    print("Error: Failed to initialize brain")
    # System will attempt recovery or graceful degradation
    # Continue with degraded functionality
    print("System continuing with degraded functionality...")

# System health checks
if brain.getTotalNeuronCount() == 0:
    print("CRITICAL: Brain has no neurons!")
    # Attempt recovery or graceful shutdown
```

## Python API Enhancements

### **Enhanced Python Bindings**
```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.15)
config.set("brain.plasticity_enabled", True)
config.set("brain.neuromodulation_enabled", True)
config.set("brain.development_enabled", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Enhanced monitoring
state = brain.getSystemState()
print(f"=== NLM Enhanced System Status ===")
print(f"Neural System: {state.neural.totalNeurons} neurons, {state.neural.totalSynapses} synapses")
print(f"Memory System: {state.memory.workingMemoryTraces} traces, {state.memory.episodicMemoryEpisodes} episodes")
print(f"Neuromodulation: Dopamine={state.neuromodulation.dopamineLevel:.2f}, " +
          f"Curiosity={state.neuromodulation.curiosityLevel:.2f}, " +
          f"Novelty={state.neuromodulation.noveltyLevel:.2f}")
print(f"Development: Stage={state.development.stage}, Plasticity={state.development.plasticityModulation:.2f}")

# Run simulation with monitoring
for step in range(100):
    brain.step(step)
    
    # Get system state for monitoring
    state = brain.getSystemState()
    if step % 10 == 0:
        print(f"Step {step}: {state.neural.firingNeurons} firing neurons, " +
              f"Dopamine={state.neuromodulation.dopamineLevel:.2f}")
```

## System Architecture

### **Modular Design**
```
NLM Core
├── Core Systems
│   ├── Config Management
│   ├── Random Generation
│   ├── Logging System
│   ├── Simulation Clock
│   └── Types and Constants
│
├── Brain Systems
│   ├── Neural Computation
│   │   ├── NeuralComputationSystem
│   │   ├── SpikeProcessor
│   │   └── NeuronIterationHelpers
│   ├── Memory Systems
│   │   ├── NeuralWorkingMemory
│   │   ├── NeuralEpisodicMemory
│   │   └── NeuralAssociativeMemory
│   ├── Neuromodulation Systems
│   │   ├── Dopamine
│   │   ├── Curiosity
│   │   ├── Novelty
│   │   └── PredictionError
│   ├── Cognition Systems
│   │   ├── NeuralPlanner
│   │   ├── ConceptFormation
│   │   └── AttentionalSelection
│   ├── Development System
│   └── Plasticity Systems
│       ├── STDP
│       ├── Hebbian
│       └── StructuralPlasticity
│
├── Integration
│   ├── BrainInitializer
│   ├── SystemStateTracker
│   ├── BrainSystemCoordinators
│   └── Enhanced Brain Class
│
└── Enhanced Main
    ├── Command Line Interface
    ├── Configuration Parser
    ├── Experiment Manager
    ├── Debugging Tools
    └── Visualization Support
```

## Build Instructions

### **Build with Enhanced Features**
```bash
# Standard build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Debug build (for development)
mkdir debug-build
cd debug-build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Install
make install
```

### **Run Tests**
```bash
# Run unit tests
ctest --output-on-failure

# Run integration tests
./nlm --experiment

# Run benchmarks
./nlm --benchmark
```

## License

MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

## About NLM

### **Project Information**
- **Name**: NLM (Neural Learning Machine)
- **Version**: 0.1.0 (Phase 6 Enhanced Edition)
- **Phase**: Phase 6 - Final Integration
- **Language**: C++20
- **Architecture**: Brain-inspired spiking neural network simulator

### **Features**
- **Real Neural Computation**: Leaky Integrate-and-Fire (LIF) neurons
- **Event-driven Processing**: Spike propagation with synaptic delays
- **Advanced Plasticity**: STDP and Hebbian learning rules
- **Structural Plasticity**: Synaptogenesis and pruning
- **Integrated Memory**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
- **Cognition**: Neural planning, concept formation, attention
- **Development**: Developmental stages and plasticity modulation
- **Monitoring**: Real-time system state tracking and performance metrics
- **Visualization**: Multiple visualization options and data export

### **Applications**
- **Research**: Neural network research and experimentation
- **Education**: Teaching neural network concepts
- **Simulation**: Complex system simulation and modeling
- **Development**: Development of advanced AI systems

### **Acknowledgments**
The NLM project builds on decades of research in computational neuroscience,
neural networks, and artificial intelligence. Special thanks to the open-source
community and researchers who contributed to the foundation of this work.

---

**For more information, visit:** https://github.com/nlm-project/nlm

**Documentation:** https://nlm.readthedocs.io

**Issues:** https://github.com/nlm-project/nlm/issues

**Discussions:** https://github.com/nlm-project/nlm/discussions
