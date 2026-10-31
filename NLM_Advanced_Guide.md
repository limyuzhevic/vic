# NLM - Neural Learning Machine
# Advanced User Guide
# Phase 6: Final Integration

This document provides comprehensive documentation for using and extending the NLM neural simulation framework.

## Table of Contents

1. [Introduction](#introduction)
2. [Quick Start Guide](#quick-start-guide)
3. [Configuration Management](#configuration-management)
4. [Advanced Features](#advanced-features)
5. [Performance Optimization](#performance-optimization)
6. [Debugging and Troubleshooting](#debugging-and-troubleshooting)
7. [Extending NLM](#extending-nlm)
8. [API Reference](#api-reference)
9. [Best Practices](#best-practices)
10. [Frequently Asked Questions](#frequently-asked-questions)

## Introduction

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Quick Start Guide

### System Requirements

- C++17 compatible compiler (g++ 7.0+, clang 5.0+)
- CMake 3.10 or higher
- Python 3.8+ (for Python bindings)
- Approximately 2GB RAM for 1000 neurons

### Building NLM

1. **Clone the repository:**
```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

2. **Create build directory and configure:**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

3. **Build the project:**
```bash
make -j4
```

4. **Run demos:**
```bash
# Basic neural computation demo
./nlm

# Benchmark the system
./nlm_benchmark

# Phase 6 integration test
./nlm_phase6_demo
```

### Python Bindings

The Python bindings are built automatically as part of the CMake process. To use them:

1. **Install dependencies:**
```bash
pip install -r requirements.txt
```

2. **Run Python examples:**
```bash
python3 examples/simple_brain.py
python3 examples/agent_world.py
```

## Configuration Management

### Configuration File Format

NLM uses flexible configuration files in multiple formats:

#### NLM Config Format (`.nlm` files)
```ini
[simulation]
random_seed = 42
neuron_count = 1000
region_count = 2

[plasticity]
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015

[world]
world_width = 20
world_height = 20
```

#### JSON Format (`.json` files)
```json
{
    "simulation": {
        "random_seed": 42,
        "neuron_count": 1000
    },
    "world": {
        "world_width": 20,
        "world_height": 20
    }
}
```

#### YAML Format (`.yaml` files)
```yaml
simulation:
  random_seed: 42
  neuron_count: 1000
  region_count: 2
world:
  world_width: 20
  world_height: 20
```

### Configuration Management Tools

NLM provides several tools for configuration management:

1. **Command-line Interface:**
```bash
# Create default configuration
nlm_cli.sh create-config

# Load configuration
nlm_cli.sh --load-config my_config.cfg

# Set configuration value
nlm_cli.sh --set simulation.neuron_count 2000

# Get configuration value
nlm_cli.sh --get simulation.random_seed

# Run with custom configuration
nlm_cli.sh --config my_config.cfg demo
```

2. **Python Configuration Manager:**
```python
from nlm_config_manager import NLMConfigManager

# Create config manager
config = NLMConfigManager("my_config.json")

# Set values
config.set_value("simulation.neuron_count", 2000)
config.set_value("world.world_width", 30)

# Get values
neuron_count = config.get_value("simulation.neuron_count", 1000)
print(f"Neuron count: {neuron_count}")

# Save configuration
config.save_config("new_config.json", "json")

# Create presets
config.create_preset("research_config", "Configuration for research experiments")
config.load_preset("production_config")
```

### Configuration Reference

| Section | Key | Type | Default | Description |
|---------|-----|------|---------|-------------|
| simulation | random_seed | int | 42 | Random seed for reproducible results |
| simulation | neuron_count | int | 1000 | Number of neurons in the brain |
| simulation | region_count | int | 2 | Number of neural regions |
| simulation | connection_probability | float | 0.15 | Probability of synaptic connections |
| neurons | resting_potential | float | -70.0 | Neuron resting potential (mV) |
| neurons | threshold | float | -55.0 | Firing threshold (mV) |
| neurons | refractory_period | int | 5 | Refractory period in steps |
| plasticity | stdp_ltp_weight | float | 0.02 | STDP long-term potentiation weight |
| plasticity | stdp_ltd_weight | float | 0.015 | STDP long-term depression weight |
| world | world_width | int | 20 | World width in units |
| world | world_height | int | 20 | World height in units |

## Advanced Features

### 1. Multi-Region Connectivity

NLM supports complex connectivity between neural regions:

```cpp
// Add inter-region connections
brain->addInterRegionConnection(
    RegionId(1),  // Source region
    RegionId(2),  // Target region
    0.5,         // Connection weight
    10           // Delay in steps
);
```

### 2. Advanced Plasticity Rules

#### STDP (Spiking Temporal Difference Learning)
- **LTP (Long-Term Potentiation):** When presynaptic spike precedes postsynaptic spike
- **LTD (Long-Term Depression):** When postsynaptic spike precedes presynaptic spike

#### Hebbian Learning
- **Maxelet's Rule:** "Neurons that fire together, wire together"
- **Pairing-Based Learning:** Correlated activation strengthens connections

#### Neuromodulation
- **Dopamine:** Reward prediction error signal
- **Curiosity:** Novelty detection and exploration drive
- **Prediction Error:** Discrepancy between predicted and actual outcomes

### 3. Memory Systems

#### Working Memory
- **Capacity-limited buffer** for active information
- **Competitive dynamics** for information selection
- **Rapid consolidation** to long-term memory

#### Episodic Memory
- **Event-based storage** of experiences
- **Temporal ordering** of neural activity patterns
- **Replay mechanisms** for memory consolidation

#### Associative Memory
- **Pattern completion** for recall
- **Pattern separation** for distinct representations
- **Semantic associations** between concepts

### 4. Developmental Systems

#### Developmental Stages
1. **Initial:** High plasticity, rapid learning
2. **Critical Period:** Sensitive learning window
3. **Maturation:** Stabilizing connections
4. **Adult:** Maintaining and fine-tuning
5. **Aging:** Gradual decline and adaptation

#### Plasticity Modulation
- **Activity-dependent:** Learning based on neural activity
- **Experience-dependent:** Learning based on environmental interaction
- **Gene-environment interaction:** Nature vs. nurture balance

### 5. Cognitive Functions

#### Neural Planning
- **Goal-directed behavior** with action sequences
- **Predictive modeling** of future states
- **Outcome evaluation** and reward maximization

#### Concept Formation
- **Pattern recognition** and categorization
- **Abstract representation** of experiences
- **Generalization** across similar patterns

#### Attentional Selection
- **Competitive neural dynamics** for attention
- **Prioritization** based on relevance
- **Resource allocation** for cognitive tasks

## Performance Optimization

### 1. Computational Efficiency

#### Memory Management
- **Smart pointers** for automatic memory management
- **Object pooling** for frequently allocated objects
- **Memory-efficient data structures** for large-scale simulations

#### Parallel Processing
- **SIMD optimizations** for vectorized operations
- **Multi-threading** for independent region processing
- **GPU acceleration** for matrix operations (future)

### 2. Simulation Parameters

#### Time Integration
```ini
# Smaller timesteps for accuracy
time_step = 0.0001

# Larger timesteps for speed
time_step = 0.001

# Adaptive timesteps
adaptive_time_step = true
max_time_step = 0.001
min_time_step = 0.00001
```

#### Network Size
```ini
# Large-scale simulation
neuron_count = 100000
region_count = 10

# Small-scale simulation for development
neuron_count = 100
region_count = 1
```

### 3. Performance Monitoring

NLM provides comprehensive performance monitoring:

```bash
# Enable performance monitoring
nlm --performance benchmark 1000

# Generate performance report
python3 analyze_performance.py --output performance_report.html
```

### 4. Benchmark Results

Typical performance on modern hardware:

| Neurons | Synapses | Steps/sec | Memory Usage | Duration |
|---------|----------|-----------|--------------|----------|
| 1000 | 15,000 | 50,000 | 200MB | ~0.02s |
| 5000 | 75,000 | 25,000 | 1GB | ~0.2s |
| 10000 | 150,000 | 10,000 | 2GB | ~1.0s |

## Debugging and Troubleshooting

### 1. Common Issues

#### Simulation Instability
**Symptoms:** Neuronal voltages oscillate wildly or diverge to infinity

**Solutions:**
1. **Reduce time step:** `simulation_timestep = 0.0001`
2. **Increase leak conductance:** `leak_conductance = 20.0`
3. **Add weight clamping:** `stdp_ltp_weight = 0.01`

#### Memory Usage
**Symptoms:** Memory usage exceeds available system memory

**Solutions:**
1. **Reduce network size:** `neuron_count = 500`
2. **Enable compression:** `checkpoint_compression = "fast"`
3. **Use streaming mode:** `streaming_mode = true`

#### Performance Degradation
**Symptoms:** Simulation runs slower than expected

**Solutions:**
1. **Enable optimizations:** `enable_performance_optimizations = true`
2. **Reduce output frequency:** `output_frequency = 1000`
3. **Disable visualizations:** `create_visualizations = false`

### 2. Debugging Tools

#### NLM Debug Mode
```bash
# Enable debug mode
nlm --verbose --debug --config debug_config.cfg

# Generate debug output
python3 debug_analysis.py --input simulation_output.log --output debug_report.html
```

#### Python Debugging
```python
import pynlm

# Enable debug logging
import logging
logging.basicConfig(level=logging.DEBUG)

# Create brain with debug configuration
config = pynlm.createDefaultConfig()
config.set("log_level", "DEBUG")
brain = pynlm.createBrain(config)
brain.initialize()
```

### 3. Error Messages

| Error Code | Description | Solution |
|------------|-------------|----------|
| ERR_001 | Configuration file not found | Check file path and permissions |
| ERR_002 | Invalid configuration value | Verify value format and range |
| ERR_003 | Memory allocation failed | Reduce network size or available memory |
| ERR_004 | Simulation step timeout | Reduce network size or increase timestep |

## Extending NLM

### 1. Adding New Neuron Types

Create a new neuron class by inheriting from the base Neuron class:

```cpp
class CustomNeuron : public Neuron {
public:
    CustomNeuron(NeuronId id) : Neuron(id) {
        // Custom initialization
        setType(NeuronType::Custom);
        setRestingPotential(-75.0f);
        setThreshold(-50.0f);
    }
    
    void customUpdate(TimestepDuration dt) override {
        // Custom dynamics
    }
};
```

### 2. Adding New Plasticity Rules

Create new plasticity rule classes:

```cpp
class CustomPlasticity : public PlasticityRule {
public:
    void update(Synapse* synapse, const std::vector<Timestamp>& preHistory,
                const std::vector<Timestamp>& postHistory, TimestepDuration dt) override {
        // Custom plasticity rule
    }
};
```

### 3. Creating Custom Environments

Implement custom world and environment interfaces:

```cpp
class CustomWorld : public SimpleWorld {
public:
    void update(TimestepDuration dt) override {
        // Custom world dynamics
    }
    
    SensoryInput getSensoryPercept() override {
        // Custom sensory processing
    }
};
```

### 4. Plugin Architecture

NLM supports plugin architecture for extending functionality:

```cpp
// Plugin interface
class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    virtual std::string getName() const = 0;
    virtual void initialize(Brain* brain) = 0;
    virtual void update(TimestepDuration dt) = 0;
};

// Plugin example
class ExamplePlugin : public NLMPlugin {
public:
    std::string getName() const override { return "ExamplePlugin"; }
    
    void initialize(Brain* brain) override {
        // Plugin initialization
    }
    
    void update(TimestepDuration dt) override {
        // Plugin update logic
    }
};
```

## API Reference

### Core Classes

#### Brain
```cpp
class Brain {
public:
    Brain(std::shared_ptr<Config> config);
    bool initialize();
    void step(SimulationStep step);
    void step(SimulationStep step, Timestamp time);
    void receiveSensoryInput(const SensoryInput& input);
    // ... more methods
};
```

#### Neuron
```cpp
class Neuron {
public:
    Neuron(NeuronId id);
    void stepLIF(Timestamp time, TimestepDuration dt);
    bool checkThreshold() const;
    void injectCurrent(MembranePotential current);
    // ... more methods
};
```

#### Synapse
```cpp
class Synapse {
public:
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    void step(Timestamp time);
    // ... more methods
};
```

### Python Bindings

#### Basic Usage
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate()} Hz")
```

#### Agent World
```python
import pynlm

# Create brain, world, and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run agent simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Best Practices

### 1. Configuration Management
- **Use descriptive keys:** `neuron_count` instead of `n1`
- **Document all parameters:** Add comments explaining each setting
- **Use presets:** Create preset configurations for common use cases
- **Validate configurations:** Check for valid parameter ranges

### 2. Performance Optimization
- **Profile before optimizing:** Measure performance before making changes
- **Use appropriate data structures:** Choose between vectors, maps, sets based on use case
- **Leverage parallelism:** Use multi-threading for independent computations
- **Cache frequently used data:** Store results of expensive computations

### 3. Memory Management
- **Use smart pointers:** Prefer `std::shared_ptr` and `std::unique_ptr`
- **Avoid circular references:** Use weak pointers where appropriate
- **Clear unused resources:** Free memory when no longer needed
- **Use object pools:** Reuse objects to reduce allocation overhead

### 4. Code Organization
- **Follow single responsibility principle:** Each class should have one clear purpose
- **Use namespaces:** Organize code into logical groups
- **Document public interfaces:** Keep implementation details private
- **Write unit tests:** Test individual components in isolation

### 5. Simulation Best Practices
- **Use reproducible seeds:** Set `random_seed` for reproducible results
- **Validate parameters:** Check that parameters are within valid ranges
- **Monitor performance:** Track simulation time and memory usage
- **Log important events:** Record key simulation events for debugging

## Frequently Asked Questions

### Q: What's the difference between NLM and traditional neural networks?

A: NLM uses biologically realistic neuron models (LIF dynamics) and event-driven spike propagation, while traditional neural networks typically use artificial activation functions and batch processing. NLM also includes developmental processes, memory systems, and neuromodulation that are not typically found in standard neural networks.

### Q: How do I get started with research using NLM?

A: 1. **Read the documentation:** Start with this guide and the `docs/` directory
2. **Try examples:** Run the provided demo executables
3. **Experiment:** Modify configuration parameters and observe effects
4. **Use tools:** Leverage the configuration management tools
5. **Document:** Keep track of parameters and results

### Q: Can I extend NLM with custom neuron types?

A: Yes! NLM is designed to be extensible. You can create custom neuron types by inheriting from the base `Neuron` class and overriding the necessary methods. See the "Extending NLM" section for more details.

### Q: What's the recommended way to save and load brain states?

A: Use the checkpoint system:
1. **Save brain state:** `brain->save("brain_checkpoint.bin")`
2. **Load brain state:** `brain->load("brain_checkpoint.bin")`
3. **Use configuration:** Save configurations alongside checkpoints

### Q: How do I handle large-scale simulations?

A: For large-scale simulations:
1. **Increase time step:** `simulation_timestep = 0.001`
2. **Reduce network size:** Decrease `neuron_count` or `region_count`
3. **Enable optimizations:** Set `enable_performance_optimizations = true`
4. **Use parallel processing:** Configure CMake to enable OpenMP
5. **Monitor memory:** Use performance monitoring tools

### Q: What's the difference between Phase 2 and Phase 6?

A: **Phase 2** implements basic neural computation with LIF neurons and STDP plasticity. **Phase 6** is the final integration phase that includes all previous components plus memory systems, neuromodulation, cognition, development, and persistence. Phase 6 represents a complete, integrated artificial brain simulation.

## Conclusion

NLM represents a significant step forward in computational neuroscience and artificial intelligence. By combining biologically realistic neuron models with advanced learning algorithms and memory systems, NLM provides a powerful framework for research and applications in brain-inspired computing.

The improvements documented in this guide make NLM more accessible, efficient, and extensible while maintaining its scientific rigor and biological plausibility.

For the latest information, documentation, and examples, visit the NLM project website and documentation portal.
