# NLM - Quick Reference

**Date:** 2026-10-13
**Audience:** All users (quick access to essential information)
**Level:** Quick reference

---

## Table of Contents

1. [Quick Start Guide](#quick-start-guide)
2. [Core Classes and Methods](#core-classes-and-methods)
3. [Configuration Options](#configuration-options)
4. [Performance Metrics](#performance-metrics)
5. [Troubleshooting Quick Fix](#troubleshooting-quick-fix)
6. [Command Line Tools](#command-line-tools)
7. [Import Statements](#import-statements)

---

## Quick Start Guide

### Essential Commands

```python
# Quick start - create and run a simple brain
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get results
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.3f} Hz")
```

### Complete Agent Loop (One-liner)

```python
# Create complete agent system
agent, brain, world = None, None, None

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Core Classes and Methods

### Brain Class (Main Interface)

| Method | Description | Example Usage |
|--------|-------------|---------------|
| `createBrain(config)` | Create brain instance | `brain = pynlm.createBrain(config)` |
| `initialize()` | Initialize brain | `brain.initialize()` |
| `step(step)` | Run one simulation step | `brain.step(step)` |
| `reset()` | Reset brain to initial state | `brain.reset()` |
| `getTotalNeuronCount()` | Get total neuron count | `brain.getTotalNeuronCount()` |
| `getTotalSynapseCount()` | Get total synapse count | `brain.getTotalSynapseCount()` |
| `getFiringNeuronCount()` | Get currently firing neurons | `brain.getFiringNeuronCount()` |
| `getAverageFiringRate()` | Get average firing rate | `brain.getAverageFiringRate()` |
| `getTotalSpikeCount()` | Get total spike count | `brain.getTotalSpikeCount()` |
| `getExcitationInhibitionRatio()` | Get E/I balance | `brain.getExcitationInhibitionRatio()` |
| `save(filepath)` | Save brain state | `brain.save("brain.bin")` |
| `load(filepath)` | Load brain state | `brain.load("brain.bin")` |

### AgentBrain Class (Interface Bridge)

| Method | Description | Example Usage |
|--------|-------------|---------------|
| `createAgentBrain(brain)` | Create agent interface | `agent = pynlm.createAgentBrain(brain)` |
| `initialize(world)` | Initialize with world | `agent.initialize(world)` |
| `processSensoryInput(percept)` | Process sensory input | `agent.processSensoryInput(percept)` |
| `decodeMotorCommand()` | Get motor command | `action = agent.decodeMotorCommand()` |
| `applyRewardModulation(reward, predicted)` | Apply reward learning | `agent.applyRewardModulation(reward, predicted)` |
| `updateDevelopment(timestep)` | Update development | `agent.updateDevelopment(timestep)` |
| `enableRewardModulation(bool)` | Enable/disable reward learning | `agent.enableRewardModulation(True)` |
| `enableCuriosity(bool)` | Enable/disable curiosity | `agent.enableCuriosity(True)` |
| `enableStructuralPlasticity(bool)` | Enable/disable structural plasticity | `agent.enableStructuralPlasticity(True)` |
| `enableDevelopment(bool)` | Enable/disable development | `agent.enableDevelopment(True)` |
| `getCuriosityLevel()` | Get curiosity level | `curiosity = agent.getCuriosityLevel()` |
| `getNoveltyLevel()` | Get novelty level | `novelty = agent.getNoveltyLevel()` |
| `getPredictionError()` | Get prediction error | `error = agent.getPredictionError()` |

### SimpleWorld Class (Environment)

| Method | Description | Example Usage |
|--------|-------------|---------------|
| `createSimpleWorld()` | Create simple world | `world = pynlm.createSimpleWorld()` |
| `configure(width, height, visionWidth, visionHeight)` | Configure world | `world.configure(20, 20, 8, 8)` |
| `reset()` | Reset world to initial state | `world.reset()` |
| `update(timestep)` | Update world | `world.update(0.1)` |
| `getSensoryPercept()` | Get sensory percept | `percept = world.getSensoryPercept()` |
| `applyMotorCommand(action, time)` | Apply motor command | `world.applyMotorCommand(action, time)` |
| `getSimulationTime()` | Get simulation time | `time = world.getSimulationTime()` |

### Configuration Class

| Method | Description | Example Usage |
|--------|-------------|---------------|
| `createDefaultConfig()` | Create default config | `config = pynlm.createDefaultConfig()` |
| `set(key, value)` | Set configuration value | `config.set("brain.neuron_count", 1000)` |
| `get(key, default)` | Get configuration value | `value = config.get("brain.neuron_count", 1000)` |
| `has(key)` | Check if key exists | `if config.has("brain.neuron_count")` |
| `loadFromFile(filepath)` | Load config from file | `config.loadFromFile("config.json")` |
| `saveToFile(filepath)` | Save config to file | `config.saveToFile("config.json")` |

## Configuration Options

### Brain Configuration

| Key | Type | Default | Description |
|----|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Std dev of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential |
| `brain.v_rest` | float | -70.0 | Resting potential |
| `brain.v_reset` | float | -75.0 | Reset potential after spike |
| `brain.tau_mem` | float | 20.0 | Membrane time constant |
| `brain.tau_ref` | float | 2.0 | Refractory period |

### Plasticity Configuration

| Key | Type | Default | Description |
|----|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant plus |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant minus |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Key | Type | Default | Description |
|----|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.prediction_error.enable` | bool | true | Enable prediction error |

### Development Configuration

| Key | Type | Default | Description |
|----|------|---------|-------------|
| `development.enabled` | bool | true | Enable development system |
| `development.critical_period.enabled` | bool | true | Enable critical period |
| `development.maturation.rate` | float | 0.01 | Maturation rate |
| `development.plasticity_modulation` | float | 1.0 | Plasticity modulation factor |

## Performance Metrics

### Simulation Performance

| Metric | Formula | Typical Range | Description |
|--------|---------|---------------|-------------|
| Steps/sec | Steps / elapsed_time | 1,000-10,000 | Simulation speed |
| Memory usage | RSS / (1024*1024) | 10-1000 MB | Memory consumption |
| CPU usage | CPU% | 10-100% | Processor utilization |
| Memory efficiency | (useful_memory / total_memory) | 0.1-0.9 | Memory usage efficiency |

### Neural Performance

| Metric | Formula | Typical Range | Description |
|--------|---------|---------------|-------------|
| Firing rate | Total_spikes / (neuron_count * time) | 0.1-10.0 Hz | Average firing rate |
| Firing efficiency | Active_neurons / total_neurons | 0.01-0.1 | Neural population activity |
| E/I balance | Excitation / Inhibition | 0.5-2.0 | Excitation/inhibition ratio |
| Network stability | 1 / std_dev | 0.1-10.0 | Network stability measure |

### Memory System Performance

| Metric | Formula | Typical Range | Description |
|--------|---------|---------------|-------------|
| Working memory load | Active_memory / capacity | 0.0-1.0 | Working memory utilization |
| Episodic retention | Memory_strength / time | 0.0-1.0 | Episodic memory retention |
| Consolidation efficiency | New_connections / total_connections | 0.0-1.0 | Memory consolidation efficiency |
| Memory decay rate | Memory_strength / time_step | 0.0-0.1 | Memory decay rate |

## Troubleshooting Quick Fix

### Issue | Solution
|-------|----------
| "Brain not responding" | ✅ Call `brain.initialize()` before simulation
| "Zero activity" | ✅ Run more steps: `for step in range(1000)`
| "Agent not moving" | ✅ Call `world.update(timestep)` before processing
| "Everything is 0" | ✅ Wait for brain "warm-up" (50-100 steps)
| "Memory too high" | ✅ Enable sparse connectivity, reduce neuron count
| "Performance too slow" | ✅ Profile with `profiler.start_cpu_profiling()`
| "Import error" | ✅ `pip install -e .` to install package
| "Segmentation fault" | ✅ Check for null pointer dereference

### Quick Command Line Fixes

```bash
# Install package
pip install -e .

# Run simulation
./build/nlm --benchmark --neurons=1000 --steps=1000

# Profile performance
python -m cProfile -o profile.stats your_script.py

# Check memory usage
ps aux | grep nlm
```

## Command Line Tools

### Build and Installation Commands

```bash
# Install Python package (quick)
pip install .

# Install in development mode
pip install -e .

# Build C++ from source
cd /workspace/app
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### Running NLM

```bash
# Run Phase 6 demo (integration test)
./build/nlm_phase6_demo

# Run with specific parameters
./build/nlm_phase6_demo --neurons=1000 --steps=1000 --dt=0.1

# Run Python examples
python examples_comprehensive.py

# Run tests
cd build
./nlm_test --gtest_filter="*Basic*"
ctest --output-on-failure
```

### Development Tools

```bash
# Build with debugging
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_ADVANCED_API=ON
make -j$(nproc)

# Profile with valgrind
valgrind --tool=massif ./nlm_phase6_demo

# Profile Python with cProfile
python -m cProfile -o profile.stats your_script.py
```

## Import Statements

### Basic Imports

```python
import pynlm
```

### Advanced API Imports

```python
# Import entire Advanced API
from pynlm.advanced_api import (
    AdvancedAPI,
    SimpleAgent,
    SimulationRunner,
    BrainProfiler,
    createBrainProfiler,
    createNeuronActivityMonitor,
    createNetworkConnectivityAnalyzer,
    createMemorySystemProfiler,
    createNeuromodulationAnalyzer,
    createDevelopmentStageAnalyzer
)

# Import high-level convenience functions
from pynlm.advanced_api import (
    run_simulation,
    create_brain_from_config,
    create_brain_from_json,
    save_brain_state,
    analyze_brain_activity,
    compute_statistics,
    visualize_brain_state,
    save_experiment_results,
    load_experiment_results
)
```

### Legacy API Imports

```python
# Original NLM API (still available)
import pynlm
from pynlm import createBrain, createAgentBrain, createSimpleWorld, createDefaultConfig
```

### Combined Import

```python
# All imports in one statement
import pynlm
from pynlm import createBrain, createAgentBrain, createSimpleWorld, createDefaultConfig
from pynlm.advanced_api import AdvancedAPI, SimulationRunner, SimpleAgent
```

## Summary

This quick reference provides **essential NLM information** in **compact format**:

### ✅ What You'll Find
1. **Quick start** - 5-line essential commands
2. **Core API** - Main classes and methods
3. **Configuration** - All important settings
4. **Performance** - Key metrics and targets
5. **Troubleshooting** - Quick fixes for common issues
6. **Commands** - Build and run commands
7. **Imports** - All necessary import statements

### ✅ Quick Access
- **For beginners**: Start with "Quick Start Guide"
- **For developers**: Use "Core Classes and Methods"
- **For configuration**: Refer to "Configuration Options"
- **For troubleshooting**: Check "Troubleshooting Quick Fix"

### ✅ Navigation
- **Section 1**: Quick start with essential commands
- **Section 2**: Detailed API reference
- **Section 3**: Configuration settings
- **Section 4**: Performance metrics
- **Section 5**: Quick troubleshooting
- **Section 6**: Command line tools
- **Section 7**: Import statements

This quick reference is designed for **fast access** to NLM information, perfect for when you need **quick answers** or **essential reminders**.

---

**Last Updated:** 2026-10-13
**Status:** Production Ready
**Next:** Use this as your primary reference for daily NLM work!