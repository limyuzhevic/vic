# NLM Project Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Project Structure

The NLM project is organized into several key modules:

```
NLM/
├── src/
│   ├── core/           # Core utilities and infrastructure
│   │   ├── Types/      # Data types and neural IDs
│   │   ├── Config/      # Configuration management
│   │   ├── Random/      # Random number generation
│   │   ├── Logger/      # Logging infrastructure
│   │   └── SimulationClock/  # Time management
│   ├── agent/          # Agent brain integration
│   │   ├── AgentBrain    # Core agent-brain interface
│   │   ├── SensoryPercept # Sensory processing
│   │   └── AgentBody     # Agent body implementation
│   ├── brain/          # Neural brain implementation
│   │   ├── NeuralRegion # Brain regions
│   │   ├── NeuralPopulation # Neuron populations
│   │   ├── Neuron        # LIF neuron model
│   │   └── Synapse       # Synaptic connections
│   ├── dynamics/       # Neural dynamics (LIF, spikes)
│   ├── plasticity/     # Learning mechanisms
│   │   ├── STDP        # Spike-timing dependent plasticity
│   │   ├── Hebbian     # Hebbian learning
│   │   └── StructuralPlasticity # Synaptogenesis and pruning
│   ├── memory/         # Memory systems
│   │   ├── NeuralWorkingMemory # Short-term memory
│   │   ├── NeuralEpisodicMemory # Long-term memory
│   │   └── NeuralAssociativeMemory # Pattern associations
│   ├── prediction/     # Prediction systems
│   │   ├── NeuralPrediction     # Forward models
│   │   └── PredictionSystem    # Prediction errors and learning
│   ├── cognition/      # Cognitive mechanisms
│   │   ├── NeuralPlanner # Action planning
│   │   ├── ConceptFormation # Pattern discovery
│   │   └── AttentionalSelection # Focus selection
│   ├── neuromodulation/# Neuromodulation systems
│   │   ├── Dopamine     # Reward signaling
│   │   ├── Curiosity    # Exploration drive
│   │   ├── Novelty      # Novelty detection
│   │   └── PredictionError # Learning signals
│   ├── development/    # Developmental processes
│   │   └── DevelopmentSystem # Age-based changes
│   ├── sensory/        # Sensory processing
│   │   ├── Vision      # Visual processing
│   │   ├── Audio       # Auditory processing
│   │   └── SensoryInput # Input interface
│   ├── motor/          # Motor system
│   │   ├── MotorCommand # Actions
│   │   └── MotorSystem  # Motor control
│   ├── environment/    # Environment interface
│   ├── experiments/    # Experiment framework
│   │   ├── Phase3Demo   # Basic demonstrations
│   │   ├── Phase4Demo   # Cognitive functions
│   │   ├── Phase5Demo   # Performance optimizations
│   │   └── Phase6Demo   # Integration test
│   └── visualization/  # Visualization tools
│       ├── VisualizationInterface # Display systems
│       └── NeuralVisualizer      # Brain visualization
├── tests/              # Unit and integration tests
├── docs/               # Documentation
│   ├── ARCHITECTURE.md   # System architecture
│   ├── SCIENCE.md        # Scientific background
│   ├── ROADMAP.md        # Development roadmap
│   ├── EXPERIMENTS.md    # Experiments and results
│   ├── LIMITATIONS.md    # Project limitations
│   └── PHASE6_FINAL_AUDIT.md # Phase 6 integration audit
└── python/             # Python bindings (if any)
```

## Key Components

### 1. Core Neural Engine

**LIF Neuron Model**: Leaky Integrate-and-Fire neurons with:
- Membrane potential dynamics
- Refractory periods
- Spike generation
- Synaptic input handling

**Synaptic Connections**: Event-driven spike propagation with:
- Delay-based transmission
- Synaptic plasticity
- Eligibility traces for neuromodulation

### 2. Memory Systems

**Working Memory**: Transient active information storage with:
- Competitive maintenance
- Activity-based encoding
- Limited capacity

**Episodic Memory**: Experience storage with:
- Episode consolidation
- Replay mechanisms
- Sleep/rest integration

**Associative Memory**: Pattern associations with:
- Hebbian learning
- Pattern completion
- Recall mechanisms

### 3. Neuromodulation

**Dopamine**: Reward prediction error signaling with:
- Prediction error computation
- Plasticity modulation
- Exploration-exploitation balance

**Curiosity**: Exploration motivation with:
- Novelty detection
- Information seeking
- Behavioral flexibility

### 4. Cognitive Systems

**Neural Planning**: Action sequence evaluation with:
- Hierarchical planning
- Reward prediction
- Uncertainty handling

**Concept Formation**: Pattern discovery from:
- Experience patterns
- Statistical regularities
- Abstraction mechanisms

### 5. Development

**Developmental Stages**: Age-based changes with:
- Initial high plasticity
- Critical periods
- Maturation to adult stability

## Usage Examples

### Basic Usage

```cpp
#include "nlm/Brain.hpp"
#include "nlm/Config.hpp"
#include "nlm/agent/AgentBrain.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create agent
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
    return 0;
}
```

### Python Bindings

```python
import pynlm as nlm

# Create neural system
brain = nlm.Brain()
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
stats = brain.log_status()
print(f"Neurons: {stats['total_neurons']}")
print(f"Spikes: {stats['total_spikes']}")
```

## Configuration

### Configuration File Format

NLM supports multiple configuration file formats:

**JSON Format**:
```json
{
    "neuron_count": 1000,
    "region_count": 1,
    "simulation_timestep": 0.001,
    "connection_probability": 0.1,
    "random_seed": 42,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "replay_interval": 100,
    "consolidation_interval": 1000,
    "checkpoint_dir": "./checkpoints",
    "enable_reward_modulation": true,
    "enable_structural_plasticity": true,
    "enable_development": true,
    "enable_curiosity": true
}
```

**YAML Format**:
```yaml
neuron_count: 1000
region_count: 1
simulation_timestep: 0.001
connection_probability: 0.1
random_seed: 42
stdp_ltp_weight: 0.01
stdp_ltd_weight: 0.012
synaptogenesis_rate: 0.0001
pruning_rate: 0.00001
replay_interval: 100
consolidation_interval: 1000
checkpoint_dir: ./checkpoints
enable_reward_modulation: true
enable_structural_plasticity: true
enable_development: true
enable_curiosity: true
```

**Simple Key=Value Format**:
```
# Configuration for NLM neural simulation
neuron_count = 1000
region_count = 1
simulation_timestep = 0.001
connection_probability = 0.1
random_seed = 42
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
replay_interval = 100
consolidation_interval = 1000
checkpoint_dir = ./checkpoints
enable_reward_modulation = true
enable_structural_plasticity = true
enable_development = true
enable_curiosity = true
```

### Configuration Options

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `neuron_count` | int | 1000 | Total number of neurons |
| `region_count` | int | 1 | Number of brain regions |
| `simulation_timestep` | double | 0.001 | Time step for simulation |
| `connection_probability` | float | 0.1 | Probability of synaptic connections |
| `random_seed` | int | 42 | Random seed for reproducibility |
| `stdp_ltp_weight` | float | 0.01 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse elimination |
| `replay_interval` | int | 100 | Steps between memory replay |
| `consolidation_interval` | int | 1000 | Steps between memory consolidation |
| `checkpoint_dir` | string | "./checkpoints" | Directory for checkpoint files |
| `enable_reward_modulation` | bool | true | Enable reward-based learning |
| `enable_structural_plasticity` | bool | true | Enable structural changes |
| `enable_development` | bool | true | Enable developmental processes |
| `enable_curiosity` | bool | true | Enable curiosity-driven exploration |

## Building

### Dependencies

- C++17 compatible compiler
- CMake 3.16 or higher
- Python 3.8 or higher (for bindings)

### Building with CMake

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Bindings

The project includes Python bindings via pybind11:

```bash
# Build Python bindings
cd build
make pynlm

# Install Python package
pip install -e .
```

## Running

### Phase 6 Integration Demo

Run the comprehensive integration test:

```bash
./nlm_phase6_demo
```

This runs a complete test of all Phase 6 integration features.

### Command Line Interface

The project includes a comprehensive command-line interface:

```bash
# Show help
nlm --help

# Start neural agent
nlm agent start --brain-size 1000 --regions 1

# Run Phase 6 demo
nlm demo phase6

# Get system statistics
nlm stats status

# Load configuration
nlm config load my_config.json

# Run experiment
nlm experiment run learning --duration 60
```

### Performance Monitoring

Enable performance monitoring:

```bash
nlm --monitor --export performance_data.json --report
```

### Python API

```python
import pynlm as nlm
import json

# Create brain system
brain = nlm.Brain()
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get performance metrics
metrics = brain.get_performance_metrics()
print(json.dumps(metrics, indent=2, default=str))

# Export data
brain.export_data("simulation_results.json")
```

## Configuration Management

### Command Line Configuration

Set configuration via command line:

```bash
./nlm --neuron_count 2000 --region_count 2 --simulation_timestep 0.0005
```

### Runtime Configuration

Modify configuration at runtime:

```cpp
brain->getConfig()->set("neuron_count", 2000);
brain->getConfig()->set("enable_curiosity", false);
```

### Save/Load Configuration

Save and load configuration files:

```bash
nlm config save my_config.json
nlm config load my_config.json
```

## Experiments and Demos

### Phase 3: Basic Functions

Tests basic neural dynamics:
- LIF neuron spiking
- STDP plasticity
- Basic sensory processing

### Phase 4: Cognitive Functions

Implements cognitive abilities:
- Working memory
- Episodic memory
- Concept formation
- Neural planning

### Phase 5: Performance Optimizations

Performance improvements:
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6: Integration Test

Final integration test verifying:
- All systems connected
- Memory systems work with neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional

## Performance Monitoring

### System Metrics

Monitor system performance:

```bash
nlm --monitor --alerts
```

Monitored metrics:
- CPU usage
- Memory usage
- Simulation speed
- Neuron firing rates
- Spike processing times

### Profiling

Profile specific operations:

```cpp
// Start profiling
auto profile = brain->getProfiler()->start_profile("plasticity_update");

// Run operation
brain->updatePlasticity();

// Stop profiling and get results
auto results = brain->getProfiler()->stop_profile(profile);
```

## Debugging

### Debug Session

Start debugging session:

```bash
nlm --monitor --export debug_data.json
```

Debugging features:
- Performance monitoring
- Memory tracking
- Event timeline
- Variable watching
- Breakpoints

### Debug Output

Enable detailed debug output:

```cpp
nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Debug);
brain->logStatus();
```

## Research Features

### Advanced Neural Mechanisms

Research features include:

1. **Structural Plasticity**: Dynamic synapse formation and elimination
2. **Neuromodulatory Integration**: Coordinated dopamine, curiosity, and prediction error
3. **Developmental Programming**: Age-based changes in plasticity and behavior
4. **Memory Replay**: Sleep-like consolidation during rest
5. **Prediction-Error Learning**: Forward models for expectation-driven behavior

### Research API

Access research features via:

```cpp
// Access structural plasticity
auto* sp = brain->getStructuralPlasticity();
sp->setSynaptogenesisRate(0.0002);

// Access neuromodulation
auto* dopamine = brain->getDopamine();
dopamine->setLevel(0.5);

// Access memory systems
auto* working_memory = brain->getWorkingMemory();
working_memory->setCapacity(500);
```

## Scientific Limitations

### Model Limitations

NLM is a research project investigating computational brain-like systems. Current limitations include:

- **Simplified LIF neuron model** (not Hodgkin-Huxley)
- **No realistic ion channel dynamics**
- **No detailed dendritic morphology**
- **No detailed cortical architecture**
- **No claim of consciousness, intelligence, or human-like cognition**
- **Limited to what can be simulated with available computing resources**

### Research Focus

Phase 6 investigates:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## License

MIT License. See LICENSE file for details.

## Authors

Research project — See docs for scientific background.

## Support and Contact

For issues and support, please refer to the project repository.

## Further Reading

- [NLM Scientific Papers](https://github.com/nlm-project/nlm/papers)
- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM Research Blog](https://nlm-project.org/blog)
- [NLM Discord Community](https://discord.gg/nlm)

## Versioning

- **Version**: 0.1.0 (Alpha)
- **Status**: Phase 6 - Final Integration
- **Release Date**: October 2026
- **Next Version**: 0.2.0 (Phase 7 - Advanced Research Features)