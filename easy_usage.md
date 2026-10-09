# NLM (Neural Learning Machine) - System Documentation

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain system that implements real spiking neural computation. Phase 2 of this project focuses on implementing real neural dynamics with:

- **Leaky Integrate-and-Fire (LIF) neurons** for realistic neuron dynamics
- **Event-driven spike propagation** with synaptic delays
- **STDP and Hebbian plasticity** for learning and adaptation
- **Structural plasticity** (synaptogenesis and pruning) for development
- **Integrated memory systems** for experience storage and recall

This documentation provides comprehensive information about the NLM system, including setup, configuration, usage, and advanced features.

## System Architecture

### Core Components

1. **Brain Class** - Central coordinator of the neural system
   - Manages neural regions and populations
   - Handles event-driven spike processing
   - Coordinates plasticity, development, and neuromodulation
   - Provides interface for external interactions

2. **NeuralRegion Class** - Container for neural populations
   - Manages multiple neuron populations
   - Handles intra-region connectivity
   - Provides access to neurons and synapses

3. **Neuron Class** - Implements LIF neuron dynamics
   - Membrane potential integration
   - Spike generation and refractory periods
   - Synaptic input processing
   - State management

4. **Synapse Class** - Represents neural connections
   - Weight management
   - Spike-timing dependent plasticity (STDP)
   - Delayed spike transmission
   - Plasticity flags and eligibility traces

5. **Integrated Systems** - Advanced brain functionalities
   - **Memory Systems**: Working memory, episodic memory, associative memory
   - **Prediction System**: Sensory prediction and error computation
   - **Cognition Systems**: Neural planning, concept formation, attention
   - **Development System**: Synaptogenesis, pruning, maturation stages
   - **Neuromodulation Systems**: Dopamine, curiosity, novelty detection

## Quick Start Guide

### Installation

```bash
# Clone the NLM repository
cd /path/to/nlm
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Basic Usage

Run the simulation with default settings:

```bash
./nlm
```

Run with custom neuron count:

```bash
./nlm --neuron-count 2000
```

Run with configuration file:

```bash
./nlm --config myconfig.json --verbose
```

Run benchmarks to measure performance:

```bash
./benchmark --neuron-count 1000 --steps 2000
```

### Configuration Files

NLM supports multiple configuration formats:

#### JSON Configuration

```json
{
  "neuron_count": 1000,
  "region_count": 1,
  "connection_probability": 0.1,
  "simulation_timestep": 0.001,
  "random_seed": 42,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012,
  "stdp_tau": 20.0,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001,
  "working_memory_capacity": 100,
  "episodic_memory_max_episodes": 1000,
  "development_stage": 2,
  "enable_plasticity": true,
  "enable_neuromodulation": true,
  "enable_memory": true,
  "checkpoint_dir": "./checkpoints"
}
```

#### Key-Value Configuration

```
neuron_count = 1000
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001
random_seed = 42
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
enable_plasticity = true
enable_neuromodulation = true
enable_memory = true
```

## Configuration Reference

### Core Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int64_t | 1000 | Total number of neurons in the network |
| `region_count` | int64_t | 1 | Number of brain regions |
| `connection_probability` | float | 0.1 | Probability of connection between neurons |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds |
| `random_seed` | int64_t | 42 | Seed for random number generator |

### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.01 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant in milliseconds |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse elimination |

### Memory System Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `working_memory_capacity` | int64_t | neuron_count/10 | Capacity of working memory |
| `episodic_memory_max_episodes` | int64_t | 1000 | Maximum number of stored episodes |

### Development Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development_stage` | int64_t | 2 | Current developmental stage (0=Initial, 1=CriticalPeriod, 2=Maturation, 3=Adult) |

### Advanced Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `enable_plasticity` | bool | true | Enable/disable synaptic plasticity |
| `enable_neuromodulation` | bool | true | Enable/disable neuromodulation systems |
| `enable_memory` | bool | true | Enable/disable memory systems |
| `checkpoint_dir` | string | "./checkpoints" | Directory for checkpoint files |

## Command Line Interface

### Basic Commands

```bash
# Show help information
./nlm --help

# Show version information
./nlm --version

# Run simulation with custom settings
./nlm --neuron-count 2000 --timestep 0.001 --verbose

# Run simulation with configuration file
./nlm --config config.json --validate --dry-run
```

### Advanced Options

```bash
# Set multiple configuration parameters
./nlm \
  --neuron-count 1000 \
  --region-count 2 \
  --connection-prob 0.15 \
  --timestep 0.0005 \
  --seed 12345 \
  --stdp-weights 0.02,0.018 \
  --plasticity true \
  --development-stage 2

# Run benchmarks with custom settings
./benchmark \
  --neuron-count 500 \
  --steps 2000 \
  --scenario active \
  --output benchmark_results

# Run specific experiments
./nlm experiment --type phase6 --steps 5000 --development
```

### Subcommands

#### Run Command

```bash
./nlm run --neuron-count 1000 --timestep 0.001
```

#### Benchmark Command

```bash
./nlm benchmark --sizes 100,500,1000,2000 --output results.csv
```

#### Configure Command

```bash
./nlm configure --set random_seed=12345 --set neuron_count=2000
```

#### Debug Command

```bash
./nlm debug --brain-file checkpoint.bin --load
```

#### Experiment Command

```bash
./nlm experiment --type phase6 --steps 5000 --development
```

#### Checkpoint Command

```bash
./nlm checkpoint --save ./checkpoints/brain_state.bin
./nlm checkpoint --load ./checkpoints/brain_state.bin
```

## Using the NLM API

### Basic Usage Example

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
        
        // Inject sensory input
        // brain->receiveSensoryInput(sensoryInput);
    }
    
    // Get statistics
    std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    
    return 0;
}
```

### Working with Memory Systems

```cpp
// Access working memory
auto* workingMemory = brain->getWorkingMemory();
if (workingMemory) {
    size_t activeTraces = workingMemory->getActiveTraces();
    std::cout << "Active working memory traces: " << activeTraces << std::endl;
}

// Access episodic memory
auto* episodicMemory = brain->getEpisodicMemory();
if (ephemeralMemory) {
    size_t episodeCount = ephemeralMemory->getEpisodeCount();
    std::cout << "Stored episodes: " << episodeCount << std::endl;
}

// Access associative memory
auto* associativeMemory = brain->getAssociativeMemory();
if (associativeMemory) {
    // Use associative memory for pattern recall
}
```

### Working with Plasticity

```cpp
// Access plasticity systems
auto* stdp = brain->getSTDP();
auto* hebbian = brain->getHebbian();
auto* structuralPlasticity = brain->getStructuralPlasticity();

if (stdp) {
    // Configure STDP parameters
    stdp->configure(0.02f, 0.018f, 20.0f);
    
    // Get current weights
    float ltpWeight = stdp->getLTPWeight();
    float ltdWeight = stdp->getLTDWeight();
    float timeConstant = stdp->getTimeConstant();
}

if (structuralPlasticity) {
    // Configure structural plasticity
    structuralPlasticity->setSynaptogenesisRate(0.0001);
    structuralPlasticity->setPruningRate(0.00001);
}
```

### Working with Neuromodulation

```cpp
// Access neuromodulation systems
auto* dopamine = brain->getDopamine();
auto* curiosity = brain->getCuriosity();
auto* novelty = brain->getNovelty();
auto* predictionError = brain->getPredictionErrorSignal();

if (dopamine) {
    // Get dopamine level
    float dopamineLevel = dopamine->getLevel();
    std::cout << "Dopamine level: " << dopamineLevel << std::endl;
    
    // Apply reward modulation
    dopamine->applyRewardModulation(0.5f);
}

if (curiosity) {
    // Get curiosity level
    float curiosityLevel = curiosity->getLevel();
    std::cout << "Curiosity level: " << curiosityLevel << std::endl;
}
```

## Performance Tuning

### Optimizing for Different Scenarios

#### High-Performance Configuration

```bash
./nlm \
  --neuron-count 2000 \
  --region-count 2 \
  --connection-prob 0.2 \
  --timestep 0.0005 \
  --steps 2000 \
  --plasticity true \
  --enable-neuromodulation true
```

#### Memory-Constrained Configuration

```bash
./nlm \
  --neuron-count 500 \
  --region-count 1 \
  --connection-prob 0.05 \
  --timestep 0.001 \
  --enable-memory true \
  --working-memory-capacity 50
```

#### Learning-Focused Configuration

```bash
./nlm \
  --neuron-count 1000 \
  --region-count 1 \
  --connection-prob 0.1 \
  --timestep 0.001 \
  --plasticity true \
  --development-stage 2 \
  --stdp-weights 0.02,0.018
```

### Performance Monitoring

Use benchmarking to measure and optimize performance:

```bash
# Run comprehensive benchmark
./benchmark --neuron-count 1000 --steps 2000 --output bench_results

# Compare different configurations
./benchmark \
  --neuron-count 500,1000,2000 \
  --steps 1000,2000,3000 \
  --scenario balanced,active,quiet \
  --output comparison_results.csv
```

## Troubleshooting

### Common Issues and Solutions

#### Brain Initialization Fails

**Symptoms:**
- "Failed to initialize brain!" error
- Brain log shows initialization errors

**Solutions:**
1. Check configuration parameters:
   ```bash
   ./nlm --help
   ```

2. Validate configuration:
   ```bash
   ./nlm --validate
   ```

3. Check system resources:
   - Ensure sufficient RAM for specified neuron count
   - Verify disk space for checkpoint operations

#### Simulation Runs Out of Memory

**Symptoms:**
- High memory usage
- Simulation crashes or hangs

**Solutions:**
1. Reduce neuron count:
   ```bash
   ./nlm --neuron-count 500
   ```

2. Disable memory systems:
   ```bash
   ./nlm --enable-memory false
   ```

3. Increase system resources or use more efficient machine learning algorithms

#### Plasticity Not Working

**Symptoms:**
- Synaptic weights not changing
- Learning experiments show no adaptation

**Solutions:**
1. Enable plasticity explicitly:
   ```bash
   ./nlm --plasticity true
   ```

2. Check STDP parameters:
   ```bash
   ./nlm --stdp-weights 0.01,0.012
   ```

3. Verify synapse connectivity:
   - Ensure neurons have incoming and outgoing synapses
   - Check connection probability is not too low

#### Neuromodulation Not Responding

**Symptoms:**
- Dopamine/curiosity levels remain zero
- Reward signals not affecting neural activity

**Solutions:**
1. Enable neuromodulation:
   ```bash
   ./nlm --enable-neuromodulation true
   ```

2. Check reward input sources:
   - Verify environment provides reward signals
   - Check action output is being processed

### Debugging Tools

#### Debug Mode

```bash
./nlm debug --brain-file checkpoint.bin --load --verbose
```

This mode provides detailed logging and allows loading saved brain states.

#### Configuration Validation

```bash
./nlm --validate --config custom_config.json
```

Validates configuration files and reports any issues.

### Getting More Help

#### Online Resources

- **Documentation:** https://github.com/nml-project/documentation
- **API Reference:** https://github.com/nml-project/api-docs
- **Examples:** https://github.com/nml-project/examples

#### Community Support

- **GitHub Issues:** https://github.com/nml-project/nlm/issues
- **Discussion Forum:** https://github.com/nml-project/discussions
- **Slack Channel:** https://nml-slack.herokuapp.com

## Advanced Features

### 1. Multi-Region Brain Architecture

NLM supports multiple brain regions with inter-region connectivity:

```cpp
// Add regions
RegionId region1 = brain->addRegion("Cortex");
RegionId region2 = brain->addRegion("Thalamus");

// Connect regions
brain->addInterRegionConnection(region1, region2, 0.5f, 5);
```

### 2. Development Stages

The brain goes through different developmental stages:

- **Initial:** High plasticity, rapid synaptogenesis
- **Critical Period:** Optimized for learning
- **Maturation:** Stabilization of connections
- **Adult:** Low plasticity, maintenance

```cpp
// Set development stage
brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
```

### 3. Checkpoint and Replay

Save and load brain states for reproducibility:

```cpp
// Save checkpoint
brain->save("my_checkpoint.bin");

// Load checkpoint
brain->load("my_checkpoint.bin");
```

### 4. Memory Replay

Replay important memories during rest periods:

```cpp
// Enable replay
brain->enableReplay(true, 100); // Replay every 100 steps
```

### 5. Concept Formation

Automatic discovery of patterns and concepts:

```cpp
auto* conceptFormation = brain->getConceptFormation();
if (conceptFormation) {
    conceptFormation->processCurrentActivity();
}
```

## Performance Guidelines

### Recommended Hardware

#### Minimum Requirements
- CPU: 2 cores
- RAM: 4 GB
- Disk Space: 1 GB

#### Recommended for Production
- CPU: 8+ cores
- RAM: 16+ GB
- Disk Space: 10+ GB
- SSD Storage

### Scaling Guidelines

#### Small Networks (100-500 neurons)
- Use for prototyping and testing
- Good for detailed analysis
- Fast simulation times

#### Medium Networks (500-2000 neurons)
- Good balance of performance and complexity
- Suitable for most applications
- Reasonable simulation times

#### Large Networks (2000+ neurons)
- Requires significant computational resources
- Use for research and production systems
- Consider parallel processing

### Best Practices

1. **Configuration Validation:** Always validate configuration before running
2. **Memory Management:** Monitor memory usage during long simulations
3. **Logging:** Use appropriate log levels for different scenarios
4. **Checkpointing:** Regularly save checkpoints for reproducibility
5. **Benchmarking:** Use benchmarks to measure and optimize performance
6. **Testing:** Use unit tests to verify functionality

## Conclusion

The NLM system provides a comprehensive framework for implementing and studying artificial neural networks. With its advanced features including plasticity, development, memory, and neuromodulation, it offers a powerful platform for research and experimentation in computational neuroscience.

This documentation provides the foundation for using, configuring, and extending the NLM system. For more advanced usage and API reference, consult the source code and online documentation.

---

*Last updated: October 2026*
*NLM Version 2.0.0*
*Phase 2: Real Neural Computation*