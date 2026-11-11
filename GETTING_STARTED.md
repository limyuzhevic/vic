# NLM (Neural Learning Machine) - Getting Started Guide

## Overview

NLM is an experimental computational brain project designed to create neural systems that learn from experience through interaction with an environment. This guide provides an introduction to using NLM, from basic setup to advanced usage.

## Installation and Setup

### Prerequisites

- C++ compiler with C++20 support
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)

### Building the Project

1. **Clone the repository**
```bash
git clone <repository-url>
cd nlm-project
```

2. **Build the C++ core**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

3. **Build Python bindings (optional)**
```bash
cd ..
pip install -e .
```

## Basic Usage

### Running Phase 2 Demo (Basic Neural Computation)

The Phase 2 demo demonstrates real spiking neural computation with LIF neurons, plasticity, and learning:

```bash
./nlm
```

This will run a comprehensive test showing:
- Basic neural connectivity
- Plasticity learning experiments  
- STDP verification
- Real-time neural dynamics

### Running Integration Experiments (Phase 6)

The Phase 6 demo shows a fully integrated artificial brain with all systems:

```bash
./nlm_phase6_demo
```

### Running Performance Benchmarks

```bash
./nlm_benchmark
```

## Configuration

### Configuration File Formats

NLM supports two configuration file formats:

#### Simple Format (.cfg)
```
# Configuration comment
neuron_count = 1000
region_count = 4
connection_probability = 0.1
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
simulation_timestep = 0.001
```

#### JSON Format (.json)
```json
{
    "neuron_count": 1000,
    "region_count": 4,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "simulation_timestep": 0.001
}
```

### Command Line Arguments

```bash
./nlm --config my_config.cfg
./nlm --neuron_count=2000 --region_count=8
```

### Runtime Configuration API

```cpp
#include "core/Config/Config.hpp"

auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
config->set("connection_probability", 0.15f);

int neurons = config->getOr<int>("neuron_count", 500);
double timestep = config->getOr<double>("simulation_timestep", 0.001);
```

## Python Bindings

For programmatic access to NLM, use the Python bindings:

```python
import nlm

# Create a brain with custom configuration
config = nlm.Config()
config.set("neuron_count", 500)
config.set("region_count", 2)

brain = nlm.Brain(config)
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}, Firing rate: {brain.getAverageFiringRate():.2f} Hz")

# Save brain state
brain.save("/tmp/my_brain.bin")

# Load brain state into new brain
brain2 = nlm.Brain(config)
brain2.initialize()
brain2.load("/tmp/my_brain.bin")
```

## Advanced Features

### Working with Memories

NLM includes integrated memory systems:

```cpp
// Access working memory
auto* workingMemory = brain->getWorkingMemory();
if (workingMemory) {
    float activation = workingMemory->getActivation(NeuronId(42));
    workingMemory->storeToNeuron(NeuronId(42), 1.0f);
}

// Access episodic memory
auto* episodicMemory = brain->getEpisodicMemory();
if (episodicMemory) {
    auto episodes = episodicMemory->getEpisodes();
    std::cout << "Stored " << episodes.size() << " episodes" << std::endl;
}
```

### Neuromodulation

Access neuromodulation systems:

```cpp
// Dopamine system
auto* dopamine = brain->getDopamine();
if (dopamine) {
    float level = dopamine->getLevel();
    dopamine->signalReward(1.0f);
    dopamine->signalRewardPredictionError(0.1f);
}

// Curiosity system
auto* curiosity = brain->getCuriosity();
if (curiosity) {
    float level = curiosity->getLevel();
}
```

### Development and Plasticity

Control developmental processes:

```cpp
// Development system
auto* development = brain->getDevelopmentSystem();
if (development) {
    development->update(10000.0f); // Update with 10 seconds of developmental time
}

// Structural plasticity
auto* plasticity = brain->getStructuralPlasticity();
if (plasticity) {
    plasticity->update(brain.get(), rng); // Add new synapses or prune weak ones
}
```

### Experiment Framework

Use the built-in experiment framework for systematic testing:

```cpp
#include "experiments/ExperimentRunner.hpp"

ExperimentRunner runner;

auto experiment = runner.createExperiment("learning_test");
experiment->setStartStep(0);

// Run experiment
bool success = runner.runExperiment(experiment, brain, environment, 1000);

if (success) {
    std::cout << "Experiment completed successfully" << std::endl;
}
```

## Performance Optimization

### Profiling and Monitoring

The system includes built-in profiling capabilities:

```cpp
// Monitor brain performance
brain->logStatus();  // Log detailed brain statistics
brain->getAverageFiringRate();  // Get current firing rate
brain->getTotalSpikeCount();    // Get total spike count
brain->getExcitaitonInhibitionRatio(); // Get E/I balance
```

### Performance Tips

1. **Use larger timestep**: Increase simulation timestep for faster simulations
2. **Reduce neuron count**: Use fewer neurons for prototyping
3. **Disable plasticity**: Turn off plasticity for faster training
4. **Use checkpoints**: Save and load brain states to resume simulations

```cpp
config->set("simulation_timestep", 0.01);
config->set("neuron_count", 100);
config->set("synaptogenesis_rate", 0.0f);
config->set("pruning_rate", 0.0f);
```

## Troubleshooting

### Common Issues

**Issue 1: Building fails**
```bash
cmake --version
# Ensure CMake >= 3.16
```

**Issue 2: Runtime errors**
```bash
./nlm --help
# Use help to see all available options
```

**Issue 3: Memory usage too high**
```bash
# Reduce neuron count or region count
./nlm --neuron_count=500 --region_count=2
```

### Error Codes

- **Exit code 1**: Configuration error
- **Exit code 2**: Brain initialization failed
- **Exit code 3**: File I/O error
- **Exit code 4**: Simulation error

## Contributing

### Development Setup

```bash
# Clone with submodules
cd nlm-project
# Run tests
make test
```

### Code Quality Standards

- Use C++20 features
- Follow existing coding conventions
- Add comprehensive inline documentation
- Include error handling for all I/O operations
- Write unit tests for new functionality

## Further Reading

### Documentation

- [Architecture Documentation](docs/ARCHITECTURE.md)
- [Science Documentation](docs/SCIENCE.md)
- [Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)

### Research Papers

- "Neural Learning Machine: An Artificial Developmental Brain" (2024)
- "Phase 6 Integration: A Complete Artificial Brain Architecture" (2024)

## Support

For issues and support, visit the GitHub repository or contact the development team.

---

*This guide is continuously updated as NLM evolves. Check the documentation for the latest features and improvements.*
