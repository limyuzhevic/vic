# Advanced NLM User Guide

## Overview

This guide provides comprehensive documentation for using the advanced Neural Learning Machine (NLM) simulation framework. The system has been enhanced with advanced features for improved usability, performance, and flexibility.

## Key Features

### 1. Advanced Command-Line Interface

The NLM system now includes a comprehensive command-line interface with extensive configuration options:

#### Basic Commands
- `--demo`: Run Phase 2 demonstration (default)
- `--phase3`: Run Phase 3 world interaction demo
- `--phase6`: Run Phase 6 integration test
- `--benchmark`: Run performance benchmark
- `--interactive`: Start interactive mode with real-time monitoring

#### Configuration Options
- `--config FILE`: Load configuration from JSON file
- `--seed VALUE`: Set random seed (default: 42)
- `--neurons COUNT`: Set number of neurons (default: 1000)
- `--regions COUNT`: Set number of brain regions (default: 2)
- `--steps COUNT`: Number of simulation steps (default: 1000)
- `--timestep VALUE`: Simulation timestep in seconds (default: 0.001)

#### Advanced Options
- `--synaptogenesis RATE`: Set synaptogenesis rate
- `--pruning RATE`: Set pruning rate
- `--stdp-ltp WEIGHT`: STDP LTP weight
- `--stdp-ltd WEIGHT`: STDP LTD weight
- `--stdp-tau VALUE`: STDP time constant
- `--plasticity-mod VALUE`: Plasticity modifier

### 2. Enhanced Configuration System

#### Configuration File Format
The configuration system supports JSON format with comprehensive options:

```json
{
  "neural": {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "initial_excitatory_weight": 0.2,
    "initial_inhibitory_weight": -0.1
  },
  "plasticity": {
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "stdp_trace_decay": 0.95,
    "hebbian_learning_rate": 0.001,
    "reward_modulation_strength": 0.1
  },
  "memory": {
    "working_memory_capacity": 100,
    "working_memory_decay_rate": 0.95,
    "episodic_memory_max_episodes": 1000,
    "episodic_memory_decay_rate": 0.99
  }
}
```

#### Configuration Templates
The system provides configuration templates:

```bash
# Create a template configuration
./nlm_advanced --help  # Shows all configuration options
./nlm_advanced --config config.json --neurons 2000 --steps 5000
```

### 3. Performance Monitoring and Profiling

#### Real-Time Monitoring
Enable real-time monitoring with:
```bash
./nlm_advanced --demo --monitor
./nlm_advanced --benchmark --profile
```

#### Performance Metrics
The system tracks and reports:
- Step execution times
- Memory usage
- Spike generation rates
- Plasticity computation times
- Energy consumption
- Bottleneck identification

#### Profiling Data Export
Export profiling data in JSON format:
```bash
./nlm_advanced --demo --export-stats metrics.json
```

### 4. Advanced Neural Simulation Features

#### Enhanced Plasticity
The system supports multiple plasticity mechanisms:
- **STDP**: Spike-Timing-Dependent Plasticity
- **Hebbian Learning**: Activity-dependent strengthening
- **Reward Modulation**: Reinforcement learning integration
- **Structural Plasticity**: Synapse creation and elimination

#### Memory Systems
Advanced memory integration:
- **Working Memory**: Short-term active storage
- **Episodic Memory**: Experience recording
- **Associative Memory**: Pattern associations

#### Neuromodulation
Biological neuromodulation systems:
- **Dopamine**: Reward prediction error
- **Curiosity**: Exploration motivation
- **Novelty**: Surprise detection
- **Prediction Error**: Learning signal

#### Cognitive Systems
Advanced cognitive architecture:
- **Attention**: Competitive selection
- **Planning**: Action sequence evaluation
- **Concept Formation**: Pattern abstraction
- **Self-Model**: Self-awareness

## Usage Examples

### Example 1: Basic Advanced Demo
```bash
./nlm_advanced --demo --config myconfig.json --neurons 2000 --steps 5000
```

### Example 2: Comprehensive Benchmark
```bash
./nlm_advanced --benchmark --profile --export-stats benchmark.json
```

### Example 3: Load and Continue
```bash
./nlm_advanced --demo --load-checkpoint brain.state --steps 2000
```

### Example 4: Interactive Exploration
```bash
./nlm_advanced --interactive --monitor --neural-view
```

### Example 5: Custom Experiment
```bash
./nlm_advanced --experiment STDP --verbose 3
```

## Advanced Configuration Options

### Neural Network Configuration
| Option | Default | Description |
|--------|---------|-------------|
| random_seed | 42 | Random seed for reproducibility |
| neuron_count | 1000 | Number of neurons in the brain |
| region_count | 2 | Number of brain regions |
| connection_probability | 0.1 | Probability of synaptic connections |
| simulation_timestep | 0.001 | Time step for simulation (seconds) |

### Plasticity Configuration
| Option | Default | Description |
|--------|---------|-------------|
| stdp_ltp_weight | 0.02 | STDP long-term potentiation weight |
| stdp_ltd_weight | 0.015 | STDP long-term depression weight |
| stdp_tau | 20.0 | STDP time constant |
| hebbian_learning_rate | 0.001 | Hebbian learning rate |
| reward_modulation_strength | 0.1 | Reward modulation strength |

### Memory Configuration
| Option | Default | Description |
|--------|---------|-------------|
| working_memory_capacity | 100 | Capacity of working memory |
| episodic_memory_max_episodes | 1000 | Maximum episodic memory entries |
| associative_memory_pattern_count | 500 | Number of associative patterns |

### Feature Flags
| Option | Default | Description |
|--------|---------|-------------|
| enable_curiosity | true | Enable curiosity-driven exploration |
| enable_development | true | Enable developmental stages |
| enable_reward_modulation | true | Enable reward-based learning |
| enable_structural_plasticity | true | Enable structural plasticity |

## Performance Optimization

### Configuration for High Performance
```bash
./nlm_advanced --demo \
  --neurons 5000 \
  --regions 4 \
  --steps 10000 \
  --timestep 0.0005 \
  --synaptogenesis 0.00005 \
  --pruning 0.000005
```

### Memory-Efficient Configuration
```bash
./nlm_advanced --demo \
  --neurons 500 \
  --regions 1 \
  --working-memory-size 50 \
  --episodic-memory-size 200
```

## Troubleshooting

### Common Issues

#### Configuration File Not Found
```bash
# Ensure the configuration file exists
./nlm_advanced --config configs/myconfig.json
```

#### Too Many Neurons
```bash
# Reduce neuron count for memory constraints
./nlm_advanced --demo --neurons 500
```

#### Simulation Time Too Long
```bash
# Reduce number of steps
./nlm_advanced --demo --steps 1000
```

### Error Messages
- **"Configuration validation failed"**: Check configuration file format
- **"Memory allocation failed"**: Reduce memory usage or increase system resources
- **"Plasticity computation failed"**: Check plasticity parameters

## Advanced Features

### Neural Visualization
```bash
./nlm_advanced --demo --neural-view
```

### Custom Experiments
Create custom experiments by extending the experiment framework:
```cpp
// Custom experiment example
class CustomExperiment {
public:
    void run() {
        // Custom experiment logic
    }
};
```

### Plugin Architecture
The system supports plugin architecture for custom extensions:
```cpp
// Plugin example
class PluginInterface {
public:
    virtual void initialize() = 0;
    virtual void update() = 0;
    virtual void shutdown() = 0;
};
```

## Best Practices

### 1. Use Configuration Files
- Store frequently used configurations in JSON files
- Use the `--config` option for consistency

### 2. Monitor Performance
- Use `--monitor` and `--profile` options
- Regularly check performance metrics

### 3. Balance Configuration
- Balance between simulation accuracy and performance
- Adjust plasticity parameters for desired learning behavior

### 4. Use Appropriate Neuron Counts
- Start with smaller networks for development
- Scale up for production and testing

### 5. Save and Load States
- Use `--checkpoint` to save brain states
- Use `--load-checkpoint` to continue from saved states

## Technical Reference

### Configuration Sources
Configuration sources are applied in order:
1. Default values
2. Configuration file (`--config`)
3. Command-line arguments
4. Runtime modifications

### Memory Management
- Working memory: Active neuron storage
- Episodic memory: Experience recording
- Associative memory: Pattern associations

### Plasticity Rules
- STDP: Spike-timing dependent plasticity
- Hebbian: Activity-dependent strengthening
- Reward-modulated: Reinforcement learning
- Structural: Synapse creation/elimination

## Conclusion

The advanced NLM system provides a comprehensive suite of tools for neural network simulation. With enhanced configuration options, performance monitoring, and advanced neural features, it supports a wide range of research and educational applications.

For more information, refer to the specific documentation for each component or contact the NLM support team.
