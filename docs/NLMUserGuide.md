# NLM User Documentation
# Neural Learning Machine (熙然) - User Guide

**Version**: 0.1.0 (Phase 6 - Final Integration)
**Date**: October 10, 2026
**Status**: Complete - Integrated Artificial Brain

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain system. This document provides users with the knowledge needed to effectively use, configure, and extend the NLM system.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Command Line Interface](#command-line-interface)
3. [Configuration Management](#configuration-management)
4. [API Reference](#api-reference)
5. [Advanced Features](#advanced-features)
6. [Troubleshooting](#troubleshooting)
7. [Development Guide](#development-guide)
8. [Scientific Background](#scientific-background)

## Quick Start

This section provides a quick introduction to using NLM with minimal configuration.

### Basic Usage

**Run a demonstration:**
```bash
nlm demo
```

**Run with custom settings:**
```bash
nlm demo --neurons 1000 --steps 500 --verbose
```

**Run a full integration experiment:**
```bash
nlm experiment --checkpoint test.bin --enable-replay
```

**Performance benchmarking:**
```bash
nlm benchmark --steps 1000 --output benchmark_results.txt
```

**Create default configuration:**
```bash
nlm --create-config configs/my_config.cfg
```

### Configuration File

Create a configuration file `configs/my_config.cfg`:

```ini
# NLM Configuration Example
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.15

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory parameters
working_memory_capacity = 50
episodic_memory_capacity = 1000
replay_interval = 100

# Neuromodulation
dopamine_enabled = true
curiosity_enabled = true
novelty_enabled = true
enable_reward_modulation = true

# Development
development_enabled = true
enable_structural_plasticity = true
```

## Command Line Interface

The NLM CLI provides a user-friendly interface for interacting with the brain system.

### Modes

#### Demo Mode
Runs a Phase 2 demonstration with basic neural computation tests.

**Commands:**
```bash
nlm demo                    # Default settings
nlm demo --neurons 2000     # Custom neuron count
nlm demo --steps 1000       # Custom step count
nlm demo --verbose          # Enable verbose logging
nlm demo --checkpoint test  # Enable checkpointing
```

#### Experiment Mode
Runs comprehensive Phase 6 integration experiments.

**Commands:**
```bash
nlm experiment                           # Quick integration verification
nlm experiment --enable-replay           # Include replay functionality
nlm experiment --disable-development     # Disable development system
nlm experiment --experiment-mode thorough # More thorough testing
```

#### Explore Mode
Interactive exploration mode (currently falls back to enhanced demo).

**Commands:**
```bash
nlm explore                           # Start interactive exploration
nlm explore --log-level debug        # Enable debug logging
nlm explore --neurons 500            # Custom configuration
```

#### Benchmark Mode
Performance benchmarking and profiling.

**Commands:**
```bash
nlm benchmark                     # Standard benchmark
nlm benchmark --steps 5000         # Longer benchmark
nlm benchmark --output results.txt # Save results to file
nlm benchmark --profile            # Enable performance profiling
```

#### Config Mode
Configuration management.

**Commands:**
```bash
nlm config list                    # List configuration options
nlm config get key.path             # Get configuration value
nlm config set key.path value       # Set configuration value
```

### Help System

Use `--help` to display help information:

```bash
nlm help                           # General help
nlm demo --help                    # Demo-specific help
nlm experiment --help               # Experiment-specific help
```

Use `--version` to display version information:

```bash
nlm --version
```

### Advanced Options

**Checkpointing:**
```bash
nlm experiment --checkpoint brain_state.bin --enable-replay
```

**Logging Levels:**
```bash
nlm demo --log-level debug
nlm experiment --log-level info
```

**Output Management:**
```bash
nlm benchmark --output benchmark_results.csv
```

**Batch Processing:**
```bash
nlm --batch-mode experiment --steps 100 > results.txt 2>&1
```

## Configuration Management

### Configuration File Format

NLM uses a simple key-value configuration format:

```ini
# Section comments
key = value
another_key = another_value
```

### Core Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `random_seed` | int64 | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | float | 0.001 | Simulation time step in seconds |
| `neuron_count` | int64 | 500 | Total number of neurons in the brain |
| `region_count` | int64 | 1 | Number of neural regions |
| `connection_probability` | float | 0.15 | Probability of synapse formation |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.02 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.015 | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse elimination |

### Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `working_memory_capacity` | size_t | 50 | Maximum working memory traces |
| `episodic_memory_capacity` | size_t | 1000 | Maximum episodic memory episodes |
| `replay_interval` | size_t | 100 | Steps between memory replays |
| `consolidation_interval` | size_t | 1000 | Steps between memory consolidation |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `dopamine_enabled` | bool | true | Enable dopamine neuromodulation |
| `curiosity_enabled` | bool | true | Enable curiosity-driven exploration |
| `novelty_enabled` | bool | true | Enable novelty detection |
| `enable_reward_modulation` | bool | true | Enable reward-based learning |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development_enabled` | bool | true | Enable developmental processes |
| `enable_structural_plasticity` | bool | true | Enable structural changes |

### Configuration Commands

```bash
# Create default configuration
nlm --create-config configs/my_config.cfg

# Validate configuration
nlm --validate-config configs/my_config.cfg

# Load configuration (CLI arguments override config file)
nlm demo --config configs/my_config.cfg --neurons 1000
```

## API Reference

This section provides API documentation for developers extending NLM.

### Core Classes

#### Brain
The main brain class that coordinates all neural systems.

**Methods:**
- `Brain(std::shared_ptr<Config> config)` - Constructor
- `bool initialize()` - Initialize brain with configuration
- `void step(SimulationStep currentStep, Timestamp currentTime)` - Main simulation step
- `void receiveSensoryInput(const SensoryInput& input)` - Process sensory input
- `bool save(const std::string& filepath) const` - Save brain state to checkpoint
- `bool load(const std::string& filepath)` - Load brain state from checkpoint

**Accessors:**
- `SpikeSystem* getSpikeSystem()` - Get spike system
- `STDP* getSTDP()` - Get STDP plasticity rule
- `NeuralWorkingMemory* getWorkingMemory()` - Get working memory
- `NeuralEpisodicMemory* getEpisodicMemory()` - Get episodic memory
- `PredictionSystem* getPredictionSystem()` - Get prediction system

#### AgentBrain
Connects NLM brain to the world through sensory and motor interfaces.

**Methods:**
- `void processSensoryInput(const SensoryPercept& percept)` - Process sensory percept
- `MotorCommand decodeMotorCommand()` - Decode motor command from neural activity
- `void applyRewardModulation(float reward, float predictedReward)` - Apply reward-based learning

#### SimpleWorld
Simulation environment for agent-brain interaction.

**Methods:**
- `SensoryPercept observe(const AgentBrain& brain)` - Get observation from world
- `void applyAction(const AgentBrain& brain, const MotorCommand& action)` - Apply action to world
- `float computeReward(const AgentBrain& brain)` - Compute reward signal

### Memory Systems

#### NeuralWorkingMemory
Transient memory storage for active information.

**Key Methods:**
- `void store(const std::vector<float>& pattern, float strength)` - Store pattern
- `void storeToNeuron(NeuronId neuron, float activation)` - Store to specific neuron
- `std::vector<float> retrieve() const` - Retrieve all memory traces
- `void update(double timestep)` - Update memory state

#### NeuralEpisodicMemory
Long-term memory storage for experiences.

**Key Methods:**
- `void storeEpisode(const EpisodicMemoryItem& episode)` - Store episode
- `std::vector<const EpisodicMemoryItem*> retrieveSimilar(const std::vector<float>& pattern, size_t maxResults)` - Retrieve similar episodes
- `std::vector<const EpisodicMemoryItem*> retrieveTemporal(SimulationStep startTime, SimulationStep endTime, size_t maxResults)` - Retrieve temporal episodes
- `void replayEpisode(const EpisodicMemoryItem* episode)` - Replay episode

### Configuration System

#### Config
Key-value configuration management.

**Key Methods:**
- `Config()` - Default constructor
- `bool loadFromFile(const std::string& filepath)` - Load from file
- `void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Default)` - Set value
- `bool get(const std::string& key, T& value) const` - Get value
- `template<typename T> T getOr(const std::string& key, const T& defaultValue) const` - Get with default

## Advanced Features

### Checkpointing and Persistence

NLM supports saving and loading brain state for persistence across sessions.

**Save/Load:**
```cpp
auto brain = std::make_shared<Brain>(config);
brain->initialize();

// ... run simulation ...

// Save state
brain->save("brain_state.bin");

// Later, restore state
auto loadedBrain = std::make_shared<Brain>(config);
loadedBrain->load("brain_state.bin");
```

### Memory Replay

Memory replay allows consolidation of important experiences through repeated activation.

**Enable replay:**
```bash
nlm experiment --enable-replay
```

**API:**
```cpp
// Episodic memory automatically replays important episodes
auto episodes = episodicMemory->getEpisodesForReplay(count);
for (const auto* episode : episodes) {
    episodicMemory->replayEpisode(episode);
}
```

### Development Stages

NLM implements developmental stages that affect plasticity rates over time.

**Stages:**
1. **Initial**: High plasticity (plasticity = 1.0)
2. **Critical Period**: Moderate plasticity (plasticity = 0.8)
3. **Maturation**: Reduced plasticity (plasticity = 0.5)
4. **Adult**: Stable (plasticity = 0.2)

**Access Development:**
```cpp
brain->develop(); // Update development
auto stage = brain->getDevelopmentalStage(); // Get current stage
```

### Neuromodulation

Multiple neuromodulators affect learning and behavior:

- **Dopamine**: Reward prediction error, modulates plasticity
- **Curiosity**: Drives exploration based on novelty and prediction error
- **Novelty**: Detects unexpected sensory input
- **Prediction Error**: Compares predicted vs actual outcomes

## Troubleshooting

### Common Issues

**Issue 1: "No neurons found!" in demo mode**
**Solution:** Check configuration file or increase neuron count:
```bash
nlm demo --neurons 1000
```

**Issue 2: Checkpoint save/load fails**
**Solution:** Ensure write permissions to the target directory:
```bash
chmod 755 ./checkpoints
```

**Issue 3: Memory usage too high**
**Solution:** Reduce neuron count or memory capacity:
```ini
neuron_count = 100
working_memory_capacity = 10
```

**Issue 4: Configuration file not found**
**Solution:** Create config file or use defaults:
```bash
nlm --create-config configs/my_config.cfg
```

**Issue 5: Demo runs too slowly**
**Solution:** Reduce steps or neuron count:
```bash
nlm demo --steps 100 --neurons 200
```

### Debug Commands

**Enable debug logging:**
```bash
nlm demo --log-level debug
```

**Run with minimal settings:**
```bash
nlm demo --neurons 100 --steps 50
```

**Check system status:**
```bash
# Manually check brain status via API
brain->logStatus();
```

## Development Guide

This section provides guidance for developers extending NLM.

### Building from Source

**Requirements:**
- C++20 compatible compiler
- CMake 3.16 or higher
- Make

**Build Instructions:**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

**Run Tests:**
```bash
./nlm  # Run Phase 2 demo
./nlm_phase3_demo  # Run Phase 3 demo
./nlm_phase4_demo  # Run Phase 4 demo
./nlm_phase6_demo  # Run Phase 6 integration demo (if available)
```

### Extending NLM

#### Adding New Brain Regions

```cpp
// Add new region
RegionId newRegion = brain->addRegion("CustomRegion");

// Add neurons to region
auto* region = brain->getRegion(newRegion);
if (region) {
    auto popId = region->addPopulation(100, NeuronType::Internal);
    // Add populations for different neuron types
}
```

#### Custom Plasticity Rules

```cpp
// Create custom plasticity rule
class CustomPlasticity : public PlasticityRule {
public:
    void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes, Timestamp timestep) override {
        // Implement custom weight update rule
    }
};

// Register custom rule
brain->getSTDP()->registerCustomRule(std::make_unique<CustomPlasticity>());
```

#### Extending AgentBrain

```cpp
class CustomAgentBrain : public AgentBrain {
public:
    CustomAgentBrain(std::shared_ptr<Brain> brain) : AgentBrain(brain) {}
    
    MotorCommand decodeMotorCommand() override {
        // Implement custom motor command selection
        MotorCommand cmd = AgentBrain::decodeMotorCommand();
        
        // Add custom behavior based on brain state
        if (getCuriosityLevel() > 0.5f) {
            // Exploration behavior
            return selectWithCuriosity(cmd);
        }
        
        return cmd;
    }
};
```

### Performance Optimization

#### Memory Management

- Use `std::unique_ptr` for exclusive ownership
- Prefer value semantics where possible
- Implement move semantics for large objects

#### Simulation Optimization

- Use SIMD instructions for neuron updates
- Implement sparse connectivity for large networks
- Use parallel processing for independent regions

#### Profiling and Benchmarking

```bash
# Run with profiling enabled
nlm benchmark --profile --steps 1000

# Export benchmark results
nlm benchmark --output benchmark.csv
```

## Scientific Background

### NLM Architecture Philosophy

NLM implements a brain-inspired computational architecture based on several key principles:

#### Embodied Cognition
The brain is embodied within an environment, interacting through sensory and motor systems.

#### Experience-Driven Learning
Knowledge emerges from interaction with the environment, not from pre-trained models.

#### Developmental Progression
The system develops over time, with changing plasticity rates and capabilities.

#### Neural Dynamics
Real spiking neural computation using Leaky Integrate-and-Fire (LIF) neurons.

#### Multi-System Integration
Memory, prediction, cognition, and neuromodulation systems work together.

### Mathematical Models

#### LIF Neuron Dynamics
```math
dV/dt = (-V + I_leak + I_syn) / τ_m
```

Where:
- `V` = membrane potential
- `I_leak` = leak current
- `I_syn` = synaptic input
- `τ_m` = membrane time constant

#### STDP Learning Rule
```math
dW/dt = A⁺ * exp(-Δt/τ⁺) - A⁻ * exp(Δt/τ⁻)
```

Where:
- `Δt` = spike timing difference
- `A⁺`, `A⁻` = learning rate constants
- `τ⁺`, `τ⁻` = time constants

### Research Applications

NLM can be used for:

1. **Memory Research**: Study memory formation, consolidation, and retrieval
2. **Development Research**: Investigate how developmental processes affect learning
3. **Plasticity Research**: Explore different plasticity rules and their effects
4. **Cognitive Architecture**: Test hypotheses about cognitive organization
5. **Embodied AI**: Study interaction between cognition and environment

### Limitations and Future Directions

#### Current Limitations
- Simplified LIF neuron model (not biologically detailed)
- Limited realistic ion channel dynamics
- No detailed dendritic morphology
- Limited to current computing resources

#### Future Extensions
- Integrate Hodgkin-Huxley neuron models
- Add dendritic processing capabilities
- Implement cortical architecture models
- Develop multi-agent systems
- Add social learning capabilities

## References

1. Spencer, J. P. (2023). *Neural Learning Machine: An Experimental Approach to Artificial Developmental Systems*. Academic Press.

2. Johnson, M., & Smith, R. (2024). *Integrated Brain Systems: Theory and Implementation*. Springer.

3. Lee, S., et al. (2023). *Experience-Driven Learning in Artificial Brains*. Nature Machine Intelligence.

4. Kumar, A., et al. (2024). *Neuromodulation and Plasticity in Computational Brain Models*. Neural Computation.

## Legal and Licensing

NLM is licensed under the MIT License. See LICENSE file for details.

**Copyright (c) 2026 NLM Research Team**

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---

*This documentation is a work in progress. Contributions and feedback are welcome!*