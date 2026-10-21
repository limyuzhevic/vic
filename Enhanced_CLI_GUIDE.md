# NLM - Neural Learning Machine: Enhanced CLI Features

## Quick Start with Enhanced CLI

NLM (熙然) is a **brain-inspired spiking neural network simulator** featuring **Phase 6: FINAL INTEGRATION** with **ADVANCED CLI FEATURES**. This represents the culmination of years of development, combining neural dynamics, memory systems, cognitive capabilities, and developmental processes into a coherent artificial brain.

### What's New in Phase 6?

- **✅ All Systems Integrated**: Memory, prediction, cognition, neuromodulation connected to neural processing
- **✅ Neuromodulation Coordination**: Dopamine, curiosity, novelty, prediction error integrated
- **✅ Advanced CLI Features**: Comprehensive configuration, benchmarking, profiling, multi-mode execution
- **✅ Error Recovery**: Graceful shutdown with recovery strategies
- **✅ Scripted Operations**: Batch mode, parameter sweep, random search, multi-run
- **✅ Real-time Monitoring**: System metrics and learning statistics

## Installation (Updated for Enhanced CLI)

### Prerequisites

- C++20 compatible compiler (GCC 11+, Clang 14+, or MSVC 2022+)
- CMake 3.20 or higher
- Python 3.8+ (for Python bindings)
- pybind11 2.11+ (for Python bindings)

### Build Instructions

```bash
# Standard build with advanced CLI support
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_ADVANCED_CLI=ON
make -j$(nproc)

# Build with additional optimization flags
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DENABLE_ADVANCED_CLI=ON \
         -DENABLE_PROFILING=ON \
         -DENABLE_MEMORY_POOL=ON
make -j$(nproc)

# Install
cmake --install .
```

### Running the Enhanced CLI

```bash
# Phase 6: Complete integration demo
./nlm --mode phase6

# Integration with monitoring
./nlm --mode integration --enable-system-monitoring

# Run verification
./nlm --mode verification

# Run experiment
./nlm --mode experiment --neurons 1000 --steps 5000

# Detailed benchmarking
./nlm --mode phase6 --detailed-benchmark
```

## Basic Examples with Enhanced CLI

### Example 1: Simple Brain Simulation

```bash
./nlm --mode phase2 --neurons 500 --steps 1000
```

**Output:** Basic neural computation demonstration showing spike generation and synaptic dynamics.

### Example 2: Environment Interaction

```bash
./nlm --mode phase3 --neurons 1000 --steps 2000 \
     --regions 4 --connection-prob 0.15
```

**Output:** Agent learns through interaction with environment using reward-modulated plasticity.

### Example 3: Cognitive Development

```bash
./nlm --mode phase4 --neurons 2000 --steps 5000 \
     --development --regions 8
```

**Output:** Neural system develops cognitive abilities including planning and concept formation.

### Example 4: Complete Integration

```bash
./nlm --mode phase6 --neurons 2000 --steps 10000 \
     --enable-checkpointing --enable-replay \
     --enable-system-monitoring --verbose
```

**Output:** Comprehensive integration test verifying all Phase 6 systems work together.

## Advanced Configuration Examples

### Using Configuration File

Create `experiment_config.json`:

```json
{
  "neuron_count": 1000,
  "steps": 5000,
  "regions": 4,
  "connection_probability": 0.1,
  "plasticity": {
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012
  },
  "development": {
    "plasticity_enabled": true,
    "maturation_rate": 0.01
  },
  "neuromodulation": {
    "dopamine_scale": 1.0,
    "curiosity_enabled": true
  },
  "memory": {
    "working_memory_capacity": 1000,
    "episodic_memory_capacity": 5000
  }
}
```

Run with configuration:

```bash
./nlm --mode phase6 --config-file experiment_config.json
```

### Performance Benchmarking Setup

```bash
# Comprehensive benchmark suite
./nlm --mode phase6 \
     --detailed-benchmark \
     --benchmark \
     --benchmark-compare

# Compare different configurations
./nlm --mode phase6 --benchmark \
     --benchmark-neurons "500 1000 2000" \
     --benchmark-steps "1000 5000 10000"
```

### Scripted Operations

```bash
# Batch mode execution
./nlm --mode phase6 --enable-scripted-ops --batch-mode --batch-delay 100

# Parameter sweep
./nlm --mode phase6 --enable-scripted-ops --parameter-sweep

# Random search optimization
./nlm --mode phase6 --enable-scripted-ops --random-search --random-seeds 10

# Multi-run experiment
./nlm --mode phase6 --enable-scripted-ops --multi-run-mode --multi-run-count 50
```

## Enhanced CLI Reference

### Command Line Syntax

```
./nlm [options] [arguments]
```

### Core Options

| Option | Short | Description | Default |
|--------|--------|-------------|---------|
| `--mode` | `-m` | Execution mode | `phase6` |
| `--neurons` | `-n` | Number of neurons | `1000` |
| `--steps` | `-s` | Simulation steps | `5000` |
| `--regions` | `-r` | Brain regions count | `4` |
| `--help` | `?` | Show help information | |
| `--version` | `V` | Show version information | |

### Advanced Mode Options

#### Mode Selection

| Mode | Description |
|------|-------------|
| `phase2` | Basic neural computation |
| `phase3` | World interaction and learning |
| `phase4` | Full cognitive development |
| `phase5` | Advanced learning and memory |
| `phase6` | Complete integration demo |
| `integration` | Integration simulation with monitoring |
| `experiment` | Controlled experiment mode |
| `verification` | Integration verification |

#### Feature Enable/Disable

| Option | Description |
|--------|-------------|
| `--enable-checkpointing` | Save/restore brain state |
| `--enable-replay` | Replay experiences for consolidation |
| `--enable-development` | Enable developmental processes |
| `--enable-system-monitoring` | Real-time system metrics |
| `--enable-statistics-export` | Export learning statistics |
| `--enable-scripted-ops` | Enable batch/parameter/random search |

#### Configuration Options

| Option | Description |
|--------|-------------|
| `--config-file` | Load configuration from file |
| `--export-config` | Export current configuration to file |
| `--validate-configs` | Validate configuration parameters |
| `--auto-recover` | Attempt recovery on error |
| `--error-recovery-mode` | Recovery mode: graceful/aggressive/strict |

#### Performance Options

| Option | Description |
|--------|-------------|
| `--detailed-benchmark` | Run detailed benchmarking |
| `--benchmark` | Run performance benchmark |
| `--benchmark-compare` | Compare multiple benchmarks |
| `--benchmark-neurons` | Neuron counts for benchmark |
| `--benchmark-steps` | Step counts for benchmark |
| `--enable-profiling` | Enable performance profiling |

#### Monitoring Options

| Option | Description |
|--------|-------------|
| `--enable-system-monitoring` | Collect system metrics |
| `--export-metrics` | Export metrics to CSV file |
| `--stats-output` | Output file for statistics |
| `--verbose` | Verbose output |
| `--quiet` | Quiet mode |
| `--debug` | Debug mode |

#### Scripted Operations Options

| Option | Description |
|--------|-------------|
| `--batch-mode` | Enable batch mode |
| `--batch-delay` | Delay between batches (ms) |
| `--parameter-sweep` | Enable parameter sweep |
| `--random-search` | Enable random search |
| `--random-seeds` | Number of random seeds |
| `--multi-run-mode` | Enable multi-run mode |
| `--multi-run-count` | Number of multi-run iterations |

#### Learning Parameters

| Option | Description |
|--------|-------------|
| `--learning-rate` | STDP learning rate | `0.01` |
| `--connection-prob` | Synaptic connection probability | `0.1` |
| `--development` | Enable developmental plasticity | `false` |

## Error Handling and Recovery

### Error Types

| Type | Severity | Description |
|------|----------|-------------|
| ConfigurationError | 1-3 | Configuration file errors |
| SystemError | 1-2 | System-level errors |
| MemoryError | 2 | Memory allocation errors |
| PerformanceError | 3 | Performance degradation |
| IntegrationError | 2 | System integration failures |
| CheckpointError | 2 | Save/load errors |
| ProfilingError | 3 | Profiling failures |
| StatisticsError | 3 | Statistics collection errors |

### Recovery Modes

1. **Graceful**: Safe recovery with checkpoints
2. **Aggressive**: Fast recovery with restarts
3. **Strict**: Comprehensive error recovery

### Error Handling Examples

```bash
# Graceful error recovery (default)
./nlm --mode phase6 --auto-recover

# Aggressive recovery for critical systems
./nlm --mode integration --error-recovery-mode aggressive

# Strict recovery for research
./nlm --mode phase6 --error-recovery-mode strict
```

## Configuration File Format

### JSON Configuration Schema

```json
{
  "general": {
    "neuron_count": 1000,
    "steps": 5000,
    "regions": 4,
    "simulation_timestep": 0.001,
    "random_seed": 42
  },
  "connectivity": {
    "connection_probability": 0.1,
    "initial_weight_mean": 0.5,
    "initial_weight_std": 0.1
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "enable": true,
      "learning_rate": 0.01
    },
    "structural": {
      "enable": true,
      "synaptogenesis_rate": 0.0001,
      "pruning_rate": 0.00001
    }
  },
  "development": {
    "enable": true,
    "plasticity_modifier": 0.8,
    "maturation_rate": 0.01,
    "stages": [
      "Initial",
      "CriticalPeriod",
      "Maturation",
      "Adult",
      "Aging"
    ]
  },
  "neuromodulation": {
    "dopamine": {
      "enable": true,
      "initial": 0.1,
      "scale": 1.0
    },
    "curiosity": {
      "enable": true,
      "initial": 0.1,
      "weight": 0.5
    },
    "novelty": {
      "enable": true,
      "threshold": 0.3
    },
    "prediction_error": {
      "enable": true,
      "weight": 0.5
    }
  },
  "memory": {
    "working_memory": {
      "capacity": 1000,
      "decay_rate": 0.01
    },
    "episodic_memory": {
      "capacity": 5000,
      "replay_interval": 100
    },
    "semantic_memory": {
      "decay_rate": 0.001
    }
  },
  "prediction": {
    "enable": true,
    "window_size": 100,
    "error_weight": 0.5
  },
  "cognitive": {
    "attention": {
      "enable": true,
      "focus_strength": 0.6
    },
    "planning": {
      "enable": true,
      "horizon": 10
    }
  },
  "performance": {
    "memory_pool": true,
    "event_queue": true,
    "simd_optimization": true
  }
}
```

### Configuration Validation

```bash
# Validate configuration file
./nlm --validate-configs --config-file config.json

# Export configuration
./nlm --export-config --config-file config.json

# Load and validate
./nlm --mode phase6 --config-file config.json --validate-configs
```

## Performance Benchmarking Guide

### Running Benchmarks

```bash
# Standard benchmark
./nlm --mode phase6 --benchmark

# Detailed benchmarking with metrics
./nlm --mode phase6 --detailed-benchmark

# Compare multiple configurations
./nlm --mode phase6 --benchmark --benchmark-compare \
     --benchmark-neurons "500 1000 2000" \
     --benchmark-steps "1000 5000 10000"
```

### Benchmark Metrics Collected

| Metric | Description |
|--------|-------------|
| Steps/sec | Simulation throughput |
| Spikes/step | Average spike generation |
| Memory usage | RAM consumption |
| CPU usage | Processor utilization |
| Throughput | Operations per second |

### Performance Optimization

```bash
# Build with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DENABLE_MEMORY_POOL=ON \
         -DENABLE_SIMD=ON \
         -DENABLE_EVENT_QUEUE=ON

# Run with profiling
./nlm --mode phase6 --enable-profiling

# Monitor system resources
./nlm --mode integration --enable-system-monitoring
```

## Scripted Operations Guide

### Batch Mode

```bash
./nlm --mode phase6 --enable-scripted-ops --batch-mode --batch-delay 500
```

**Features:**
- Execute multiple experiments sequentially
- Configurable delays between batches
- Progress tracking

### Parameter Sweep

```bash
./nlm --mode phase6 --enable-scripted-ops --parameter-sweep
```

**Sweeps:**
- Neuron counts: 100, 500, 1000
- Connection probabilities: 0.05, 0.1, 0.2
- Learning rates: 0.005, 0.01, 0.02

### Random Search

```bash
./nlm --mode phase6 --enable-scripted-ops --random-search --random-seeds 20
```

**Features:**
- Random configuration generation
- Performance scoring
- Best configuration tracking

### Multi-run Mode

```bash
./nlm --mode phase6 --enable-scripted-ops --multi-run-mode --multi-run-count 100
```

**Features:**
- Statistical analysis of results
- Mean, standard deviation calculation
- Min/max result tracking

## Monitoring and Statistics

### System Monitoring

```bash
./nlm --mode integration --enable-system-monitoring --export-metrics
```

**Collected Metrics:**
- CPU usage percentage
- Memory consumption (MB)
- Available memory
- Disk usage
- Network bytes transferred
- Process count
- Load average

### Learning Statistics

```bash
./nlm --mode integration --enable-system-monitoring --stats-output metrics.csv
```

**Learning Metrics:**
- Total reward accumulation
- Average firing rate
- Total spike count
- Active neuron count
- Novelty level
- Curiosity level
- Dopamine level
- Prediction error
- Developmental stage

## Extension Guide

### Adding New Modes

```cpp
// Add new mode to CLIParser::AdvancedOptions
// Add handling in main function
// Register mode with CLIParser::parse()
```

### Custom Recovery Strategies

```cpp
// Extend AdvancedErrorHandler::RecoveryStrategy
// Implement custom canRecover() and recover() functions
```

### Configuration File Extensions

```json
{
  "custom_settings": {
    "parameter_name": "value",
    "nested_configuration": {
      "sub_parameter": "value"
    }
  }
}
```

### Building with Custom Features

```bash
cmake .. -DENABLE_FEATURE_NAME=ON \
         -DCUSTOM_FEATURE_PATH=/path/to/feature
```

## Developer Documentation

### Code Structure

```
/src/
├── core/           # Core utilities (Config, Random, Logger, SimulationClock)
├── brain/          # Neural components (Neuron, Synapse, NeuralPopulation, NeuralRegion, Brain)
├── dynamics/       # Neural dynamics (NeuralDynamics, SpikeSystem)
├── plasticity/     # Plasticity rules (PlasticityRule, Hebbian, STDP, StructuralPlasticity)
├── development/    # Developmental system (DevelopmentSystem, Synaptogenesis, Pruning, Maturation)
├── neuromodulation/# Neuromodulators (Neuromodulator, Dopamine, Novelty, Curiosity, PredictionError)
├── memory/        # Memory systems (WorkingMemory, EpisodicMemory, SemanticMemory, ProceduralMemory)
├── prediction/     # Prediction systems (NeuralPrediction, ActionConsequencePredictor)
├── cognition/     # Cognitive mechanisms (Attention, Planning, SelfModel, SocialLearning)
├── sensory/       # Sensory processing (SensoryInput, VisionProcessor, TouchProcessor)
├── motor/         # Motor system (MotorCommand, ActionResult, MovementController)
├── environment/   # Environment interface (SimpleWorld, WorldObject)
├── experiments/   # Experiment framework (ExperimentRunner, Phase6IntegratedExperiment)
└── visualization/ # Visualization (GLRenderer, Visualizer)
```

### Building from Source

```bash
# Clone repository
git clone <repository-url>
cd <repository-name>

# Create build directory
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure

# Install
make install
```

### Python Bindings

```python
import pynlm

# Create brain with enhanced configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enhanced agent brain with all features
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

### API Reference

#### Core Classes

| Class | Header | Description |
|-------|--------|-------------|
| Config | core/Config/Config.hpp | Configuration management |
| Random | core/Random/Random.hpp | Deterministic random numbers |
| Logger | core/Logger/Logger.hpp | Structured logging |
| SimulationClock | core/SimulationClock/SimulationClock.hpp | Time management |

#### Neural Classes

| Class | Header | Description |
|-------|--------|-------------|
| Neuron | brain/Neuron.hpp | Individual neuron (LIF dynamics) |
| Synapse | brain/Synapse.hpp | Synaptic connection |
| Brain | brain/Brain.hpp | Central coordinator |

#### Integration Classes

| Class | Header | Description |
|-------|--------|-------------|
| AgentBrain | agent/AgentBrain.hpp | Sensorimotor integration |
| SimpleWorld | environment/Environment.hpp | Virtual environment |

### Testing

```bash
# Unit tests
./nlm_test

# Integration tests
./nlm_phase6_demo

# Performance tests
./nlm --mode phase6 --benchmark
```

### Troubleshooting

#### Common Issues

**Issue:** Application fails to start
```bash
# Check permissions and dependencies
ls -la ./nlm
cat /proc/version
dpkg -l | grep cmake
```

**Issue:** Configuration file not loaded
```bash
# Check file path and permissions
ls -la config.json
./nlm --config-file config.json --validate-configs
```

**Issue:** Memory allocation failures
```bash
# Check system resources
free -h
./nlm --mode phase6 --neurons 100 --steps 1000
```

**Issue:** Performance degradation
```bash
# Profile application
./nlm --mode phase6 --enable-profiling

# Check system resources
./nlm --mode integration --enable-system-monitoring
```

## Release Notes

### Phase 6 Enhancements

- All Phase 6 integration requirements satisfied
- Advanced CLI features added for comprehensive configuration
- Error recovery strategies implemented
- Performance monitoring and benchmarking added
- Scripted operations for automated testing

### Future Directions

- [ ] Machine learning integration
- [ ] Multi-agent systems
- [ ] Real-time adaptation
- [ ] Cloud deployment capabilities

## Support and Community

### Documentation
- This documentation: `docs/ENHANCED_CLI_GUIDE.md`
- Architecture: `docs/PHASE6_ARCHITECTURE.md`
- Science: `docs/SCIENCE.md`

### Resources
- GitHub repository: [NLM GitHub](https://github.com/NeuralLearningMachine/NLM)
- Issues: [GitHub Issues](https://github.com/NeuralLearningMachine/NLM/issues)
- Discussions: [GitHub Discussions](https://github.com/NeuralLearningMachine/NLM/discussions)
- Wiki: [NLM Wiki](https://github.com/NeuralLearningMachine/NLM/wiki)

## License

MIT License

---

*Documentation generated automatically from enhanced main.cpp implementation*
*Last updated: $(date +%Y-%m-%d)*
*Version: Phase 6 with Advanced CLI Features*