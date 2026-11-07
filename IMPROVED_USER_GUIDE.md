# NLM - Improved User Guide and Documentation
# Phase 6: Enhanced Artificial Brain System

## Overview

This document describes the improvements made to the NLM (Neural Learning Machine) system, a brain-inspired spiking neural network simulator. The improvements focus on:

1. **Enhanced Configuration Management** - Advanced config parser with JSON/YAML support
2. **Improved Logging System** - Configurable logging with multiple destinations
3. **Advanced Command Line Interface** - Rich CLI for running demos and configuring parameters
4. **Robust Error Handling** - Better validation and error reporting
5. **Enhanced Synapse Management** - Improved STDP and spike history tracking
6. **Comprehensive Demo Suite** - Multiple demo modes for different use cases
7. **Integration and Development** - Better brain initialization and development processes

## 1. Enhanced Configuration System

### Configuration File Formats

NLM now supports multiple configuration file formats:

#### Simple Format (key=value)
```
neuron_count = 1000
region_count = 2
connection_probability = 0.15
simulation_timestep = 0.001
log_level = INFO
```

#### JSON Format
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "log_level": "INFO",
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.02
        }
    }
}
```

#### YAML Format
```yaml
neuron_count: 1000
region_count: 2
connection_probability: 0.15
simulation_timestep: 0.001
log_level: INFO
plasticity:
  stdp:
    enable: true
    learning_rate: 0.02
```

### Configuration Parser Improvements

#### Automatic Format Detection
The configuration parser automatically detects the file format:
- **JSON**: Files starting with `{` or `[`
- **YAML**: Files containing `:` (key: value pairs)
- **Simple**: Fallback for basic key=value format

#### Enhanced Value Parsing
The parser now supports:
- **Type Detection**: Automatically detects bool, int, double, and string values
- **Array Support**: Simple array parsing (e.g., `[1, 2, 3]`)
- **Nested Structures**: Basic JSON/YAML object support
- **Error Handling**: Detailed error messages for malformed configurations

#### Configuration Validation
```
// Validate configuration ranges
if (neuron_count < 1 || neuron_count > 100000) {
    log_error("Invalid neuron_count");
    return false;
}

if (connection_probability < 0.0 || connection_probability > 1.0) {
    log_error("Invalid connection_probability");
    return false;
}
```

## 2. Improved Logging System

### Log Levels
- **DEBUG**: Detailed debug information
- **INFO**: General information messages
- **WARNING**: Warning conditions that may indicate problems
- **ERROR**: Error conditions
- **CRITICAL**: Critical errors requiring immediate attention

### Enhanced Logging Features

#### File Logging
```bash
nlm --log-to-file --log-file=nlm.log phase6
```

#### Console Colors
- Automatic color coding based on log level
- Optional color disabling

#### Log Format
Enhanced log format includes:
```
[INFO] 2024-01-01 12:00:00 [filename:line] Message with context
[DEBUG] [neuron_id] Spike recorded at time 0.123s
```

#### Log Filtering
```
// Enable only INFO and above
nlm --log-level=INFO phase2

// Suppress warnings
nlm --log-level=ERROR learning
```

## 3. Advanced Command Line Interface

### Basic Usage
```bash
nlm [OPTIONS] [DEMO]

Available Demos:
  phase2      - Phase 2: Basic neural computation
  phase6      - Phase 6: Final integration
  learning    - Learning in interactive world
  plasticity  - Synaptic plasticity demonstration
  memory     - Memory integration demo
  prediction  - Prediction and planning demo
  development - Development process demo
  stdp        - STDP verification demo
  hebbian     - Hebbian learning demo
  comprehensive - Comprehensive demo suite
```

### Common Options
```bash
nlm --neuron-count=3000 --region-count=4 learning
nlm --config=myconfig.json --log-to-file phase6
nlm --enable-plasticities --enable-curiosity plasticity
nlm --log-level=DEBUG --log-file=nlm.log phase2
nlm --list-demos
nlm --help
```

### Advanced Options
```bash
// Override configuration file
nlm --config=advanced.cfg --neuron-count=5000 phase6

// Configure specific parameters
nlm --stdp_ltp_weight=0.025 --stdp_ltd_weight=0.015 phase2

// Demo with special features
nlm --enable-curiosity --enable-novelty --enable-reward-modulation learning

// Batch processing
nlm --log-to-file --checkpoint-dir=./checkpoints phase6
```

## 4. Enhanced Synapse and STDP Management

### Improved Spike History
The Synapse class now provides:

#### Spike Count Methods
```cpp
// Get spike counts since specific time
size_t preSpikeCount = synapse.getPreSpikeCount(100.0f);  // Spikes in last 100ms
size_t postSpikeCount = synapse.getPostSpikeCount(100.0f);

// Get spike frequency
float frequency = synapse.getSpikeFrequency(100.0f);  // Hz
```

#### Enhanced STDP
```cpp
// Predict STDP weight change
float predictedChange = synapse.calculateSTDPWeightChange(
    preTime, postTime, 0.02f, 0.015f
);

// Apply STDP with learning
 synapse.updateForSTDP(preTime, postTime, 0.02f);
```

#### Short-Term Plasticity
```cpp
// Access STP state
float facilitation = synapse.getShortTermFacilitation();
float depression = synapse.getShortTermDepression();
```

## 5. Comprehensive Demo Suite

### Demo Options

#### Phase 2 Demo
```bash
nlm phase2
nlm --neuron-count=500 --region-count=2 phase2
```

#### Phase 6 Demo
```bash
nlm phase6
nlm --enable-curiosity --enable-novelty phase6
```

#### Learning World Demo
```bash
nlm learning
nlm --neuron-count=1500 --region-count=3 learning
```

#### Plasticity Demo
```bash
nlm plasticity
nlm --enable-plasticities --enable-curiosity plasticity
```

#### Memory Demo
```bash
nlm memory
nlm --neuron-count=1000 memory
```

#### Development Demo
```bash
nlm development
nlm --enable-development development
```

#### STDP Demo
```bash
nlm stdp
```

#### Hebbian Demo
```bash
nlm hebbian
```

### Demo Outputs

Each demo provides detailed logging and statistics:
```
Phase 2 Demo Results:
  - Neurons: 500
  - Synapses: 7,500
  - Firing rate: 12.5 Hz
  - STDP LTP: 0.024
  - STDP LTD: 0.018
  - Development Stage: CriticalPeriod

Learning World Demo Results:
  - Final curiosity: 0.734
  - Final novelty: 0.891
  - Neuromodulation: 0.456
  - Memory traces: 128
  - Episodic episodes: 42
```

## 6. Configuration Examples

### Basic Configuration
```bash
nlm --neuron-count=2000 --region-count=3 --random-seed=42 phase6
```

### Advanced Configuration
```bash
nlm --config=advanced.json \
     --enable-curiosity \
     --enable-novelty \
     --enable-reward-modulation \
     --enable-development \
     --log-to-file \
     --log-file=advanced.log \
     --checkpoint-dir=./advanced_checkpoints \
     phase6
```

### Configuration File Examples

#### config.json (JSON format)
```json
{
    "neuron_count": 2000,
    "region_count": 3,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "log_level": "INFO",
    "log_to_file": true,
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.02,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.01
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.002,
            "pruning_rate": 0.001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    },
    "development": {
        "enable": true,
        "synaptogenesis_rate": 0.002,
        "pruning_rate": 0.001
    }
}
```

#### config.yaml (YAML format)
```yaml
neuron_count: 2000
region_count: 3
connection_probability: 2

simulation_timestep: 0.001
random_seed: 42
log_level: INFO
log_to_file: true

plasticity:
  stdp:
    enable: true
    learning_rate: 0.02
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true
    learning_rate: 0.01
  structural:
    enable: true
    synaptogenesis_rate: 0.002
    pruning_rate: 0.001

neuromod:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
  novelty:
    enable: true

development:
  enable: true
  synaptogenesis_rate: 0.002
  pruning_rate: 0.001
```

## 7. Performance and Optimization

### Performance Monitoring
```bash
nlm benchmark
nlm --neuron-count=1000 --region-count=2 --steps=5000 benchmark
```

### Checkpointing
```bash
nlm --checkpoint-dir=./checkpoints --checkpoint-interval=1000 phase6
```

### Memory Management
```bash
nlm --memory-limit=2048 --gc-interval=500 phase6
```

## 8. Troubleshooting

### Common Issues

#### Configuration File Errors
```bash
# Fix malformed JSON
# Fix invalid key=value format
# Check for duplicate keys in default.cfg
```

#### Log File Permissions
```bash
# Ensure log directory exists and is writable
mkdir -p logs
chmod 755 logs
```

#### Memory Issues
```bash
# Increase memory limits
nlm --memory-limit=4096 --neuron-count=3000 phase6
```

#### Performance Issues
```bash
# Use fewer regions for better performance
nlm --region-count=2 --neuron-count=2000 phase6

# Disable less important features
nlm --disable-plasticities --disable-curiosity phase2
```

## 9. API Documentation

### Core Classes

#### Brain Class
```cpp
// Create and initialize brain
auto brain = std::make_shared<Brain>(config);
brain->initialize();

// Run simulation
brain->step(step, time);

// Access systems
auto memory = brain->getWorkingMemory();
auto prediction = brain->getPredictionSystem();
```

#### Synapse Class
```cpp
// Enhanced synapse operations
synapse.recordPreSpike(timestamp);
synapse.recordPostSpike(timestamp);

float weightChange = synapse.calculateSTDPWeightChange(preTime, postTime);
synapse.updateForSTDP(preTime, postTime, learningRate);
```

#### Config Class
```cpp
// Load configuration
config->loadFromFile("config.json");
config->loadFromArgs(argc, argv);

// Get configuration values
int neuronCount = config->getOr<int>("neuron_count", 1000);
double timestep = config->getOr<double>("simulation_timestep", 0.001);
```

## 10. Future Enhancements

### Planned Features
1. **Full JSON/YAML Parser** - Complete implementation with library support
2. **REST API** - HTTP API for remote brain control
3. **Web Interface** - Web-based visualization and control
4. **Distributed Computing** - Support for multi-GPU and multi-machine deployments
5. **Advanced Learning** - Reinforcement learning with reward prediction
6. **Neuroevolution** - Genetic algorithms for brain structure optimization

### Performance Targets
- **Scalability**: Support for 10,000+ neurons
- **Real-time**: Simulation at 1kHz or higher
- **Memory Efficiency**: < 1MB per neuron
- **Speed**: > 10,000 synaptic updates per second

## 11. Legal and License Information

This project is licensed under the MIT License. See LICENSE file for details.

## 12. References

- [NLM Documentation](https://nlm.readthedocs.io)
- [Architecture Documentation](docs/ARCHITECTURE.md)
- [Scientific Background](docs/SCIENCE.md)
- [Phase 6 Audit](docs/PHASE6_FINAL_AUDIT.md)

---

**Last Updated**: Phase 6 Implementation Complete**
**Version**: 0.1.0 (Phase 6)
**Status**: Production Ready