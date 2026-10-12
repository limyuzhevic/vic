# NLM - Neural Learning Machine
# Documentation for Version 0.1.0

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Quick Start

### 1. Build the Project

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### 2. Run Basic Tests

```bash
# Run all tests
./nlm test --all

# Run specific test
./nlm test --test connectivity
./nlm test --test plasticity
./nlm test --test stdp

# Run benchmark
./nlm benchmark --steps 1000

# Interactive mode
./nlm interactive

# Run experiment
./nlm experiment --name default --steps 1000
```

### 3. Python API

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run for 100 steps
for i in range(100):
    brain.step(i)

print("Total spikes:", brain.getTotalSpikeCount())

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run agent in world
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Command Line Interface

### Global Options

- `-h, --help`          Show help message
- `-v, --version`       Show version information

### Commands

#### `nlm test [OPTIONS]`

Run neural network tests.

**Options:**
- `-t, --test <name>`    Specific test to run (default: "all")
- `-a, --all`           Run all tests

**Available tests:**
- `connectivity`       Test basic neural connectivity and spike propagation
- `plasticity`         Test STDP and Hebbian plasticity with learning experiment
- `stdp`               Test spike-timing dependent plasticity (STDP) rule

#### `nlm benchmark [OPTIONS]`

Run performance benchmark.

**Options:**
- `-s, --steps <n>`     Number of simulation steps to run (default: 1000)
- `-c, --config <file>` Configuration file (default: "configs/default.cfg")

**Example:**
```bash
./nlm benchmark --steps 5000
./nlm benchmark --config custom.cfg
```

#### `nlm interactive [OPTIONS]`

Start interactive mode for exploring brain state.

**Options:**
- `-c, --config <file>` Configuration file (default: "configs/default.cfg")

**Interactive Commands:**
- `step <steps>`        Run simulation steps
- `status`              Print brain status
- `config`              Show current configuration
- `help` or `h`         Show help message
- `exit` or `q`         Exit interactive mode

#### `nlm experiment [OPTIONS]`

Run experiment simulation.

**Options:**
- `-n, --name <name>`   Experiment name (default: "default")
- `-s, --steps <n>`     Number of steps to run (default: 1000)
- `-c, --config <file>` Configuration file (default: "configs/default.cfg")

**Example:**
```bash
./nlm experiment --name learning --steps 2000
```

## Configuration File Format

NLM uses a simple key=value format for configuration files. Example:

```
# NLM Configuration File
# Basic simulation settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

# Plasticity settings
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
```

### Configuration Options

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `random_seed` | int64 | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds |
| `neuron_count` | int64 | 500 | Number of neurons in the brain |
| `region_count` | int64 | 1 | Number of brain regions |
| `connection_probability` | double | 0.15 | Connection probability for random connectivity |
| `stdp_ltp_weight` | double | 0.02 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | double | 0.015 | STDP long-term depression weight |
| `stdp_time_constant` | double | 20.0 | STDP time constant |
| `log_level` | string | "INFO" | Logging level (DEBUG, INFO, WARN, ERROR) |
| `log_to_file` | bool | false | Whether to log to file |
| `log_filename` | string | "nlm.log" | Log file name |
| `max_simulation_steps` | int64 | 10000 | Maximum simulation steps |
| `simulation_time_limit` | double | 0.0 | Time limit in seconds (0 = unlimited) |

## Testing

### Running Tests

The test framework provides comprehensive testing for NLM components:

```bash
./nlm test --all
```

### Test Descriptions

1. **Connectivity Test**: Verifies basic neural connectivity and spike propagation through the network.

2. **Plasticity Test**: Tests STDP and Hebbian plasticity rules with learning experiments to verify measurable synaptic changes.

3. **STDP Test**: Specifically validates spike-timing dependent plasticity rule with correlated pre-post neural activity.

### Test Output

Each test produces detailed logging showing:
- Initial brain state
- Test execution progress
- Results and statistics
- Success/failure status

## Benchmarking

### Running Benchmarks

Performance benchmarking measures the time required for NLM to process neural simulations:

```bash
./nlm benchmark --steps 1000
./nlm benchmark --steps 5000 --config perf.cfg
```

### Benchmark Output

Benchmark results include:
- Operation timing
- Average, minimum, and maximum execution times
- Total wall time
- Brain statistics (neurons, synapses, spikes, firing rate)

## Advanced Usage

### Configuration Override

Command line options override configuration file values:

```bash
./nlm benchmark --steps 5000 --log_level DEBUG --neuron_count 1000
```

### Custom Configuration

Create custom configuration files:

```bash
cp configs/default.cfg custom.cfg
# Edit custom.cfg with desired settings
./nlm benchmark --config custom.cfg
```

## Performance Tips

1. **Large simulations**: Use `--steps` with larger values for performance testing
2. **Debugging**: Set `log_level = DEBUG` for detailed output
3. **Memory efficiency**: Monitor memory usage with `--log_to_file true`
4. **Development**: Enable development features with `enableDevelopment(true)`

## Troubleshooting

### Common Issues

1. **"My brain isn't doing anything"**
   - Did you call `brain.initialize()`?
   - Try increasing the number of steps
   - Check configuration settings

2. **"The agent isn't moving"**
   - Did you call `world.applyMotorCommand()`?
   - Check that `world.update()` is being called
   - Verify agent's motor neuron connectivity

3. **"Everything is 0"**
   - Brains need time to "warm up" - try more steps
   - Some neurons need input to fire - make sure sensory input is connected

### Error Messages

- **Configuration validation failed**: Check for invalid values in config file
- **Test failed**: Check logs for specific error details
- **Benchmark failed**: Ensure system resources are available

## Architecture

NLM follows a modular architecture:

```
CORE:         Config, Types, Logger, Random, Clock
             ↓
NEURAL:       Neuron, Synapse, Brain, NeuralRegion
             ↓
DYNAMICS:     SpikeSystem, NeuralDynamics
             ↓
PLASTICITY:   STDP, Hebbian, StructuralPlasticity
             ↓
DEVELOPMENT:  DevelopmentSystem, Synaptogenesis, Pruning, Maturation
             ↓
NEUROMODULATION: Dopamine, Curiosity, Novelty, PredictionError
             ↓
MEMORY:      WorkingMemory, EpisodicMemory, AssociativeMemory
             ↓
PREDICTION:   PredictionSystem, NeuralPrediction
             ↓
COGNITION:    NeuralPlanner, ConceptFormation, AttentionalSelection
             ↓
SENSORY:     Vision, Audio, InternalSignals, SensoryInput
             ↓
MOTOR:       MotorSystem, Action
             ↓
ENVIRONMENT: Environment, SimpleWorld, Observation
             ↓
AGENT:       AgentBrain, AgentBody, SensoryPercept
             ↓
EXPERIMENTS:  ExperimentRunner, Phase3Experiment, Phase4Experiment,
             Phase5Experiment, Phase6IntegratedExperiment
```

## Plugins and Extensions

### Plugin System

NLM supports plugin extensions for:
- Custom sensory modalities
- Novel motor control schemes
- Specialized learning algorithms
- Domain-specific experiments

### Plugin Development

To create a plugin, implement the required interfaces:

```cpp
class CustomPlugin {
public:
    virtual void initialize() = 0;
    virtual void update(double timestep) = 0;
    virtual std::string getName() const = 0;
};
```

## Future Enhancements

### Planned Features

1. **Plugin Architecture**: Modular plugin system for extensions
2. **Advanced Visualization**: Real-time brain state visualization
3. **Distributed Computing**: Multi-process neural simulation
4. **Machine Learning Integration**: Hybrid neuro-symbolic approaches
5. **Hardware Acceleration**: GPU/FPGA support for large-scale simulations
6. **Web Interface**: Browser-based brain monitoring and control
7. **Cloud Deployment**: Serverless NLM simulation services

### Research Areas

- Neural developmental trajectories
- Memory consolidation mechanisms
- Neuromodulatory learning systems
- Predictive coding in biological brains
- Emergent cognitive architectures

## License

MIT License

## Copyright

Copyright (c) 2024 NLM Project

---

*Documentation generated automatically by NLM build system*
