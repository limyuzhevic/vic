# NLM - Neural Learning Machine
# Enhanced User Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. This guide covers the enhanced version with improved structure, comprehensive command-line interface, and advanced features.

## Quick Start

### Installation

1. Build the project:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. The main executable will be available in the build directory.

### Basic Usage

#### Run Phase 2 Demo (Basic Neural Computation)
```bash
./nlm --demo phase2
```

#### Run Phase 6 Demo (Full Integration)
```bash
./nlm --demo phase6 --neurons 1000 --steps 2000
```

#### Save/Load Brain State
```bash
# Save current brain state
./nlm --demo phase2 --save brain_state.bin

# Load brain state and continue
./nlm --demo phase2 --load brain_state.bin
```

#### Learning Experiment
```bash
./nlm --demo learning --neurons 500 --learning
```

#### Performance Benchmark
```bash
./nlm --demo benchmark --neurons 2000
```

## Command Line Interface

### Available Demo Modes

| Demo Mode | Description | Typical Use Case |
|-----------|-------------|------------------|
| `phase2` | Basic neural computation with LIF neurons | Research and testing |
| `phase3` | World interaction and agent behaviors | AI agent development |
| `phase4` | Cognition and memory systems | Cognitive architecture |
| `phase5` | Performance optimizations | High-performance computing |
| `phase6` | Full integration of all systems | Complete brain simulation |
| `benchmark` | Performance testing | System benchmarking |
| `learning` | Advanced learning experiments | Machine learning research |
| `comprehensive` | All tests in sequence | Full system validation |
| `quick` | Quick validation tests | Sanity checking |

### Command Line Options

#### Basic Options
- `--demo <mode>`: Select demo mode (required)
- `--help`: Show help information
- `--version`: Show version information

#### Configuration Options
- `--config <file>`: Load configuration from file
- `--preset <name>`: Use predefined preset (default, research, demo, production)
- `--neurons <count>`: Set neuron count (default: 500)
- `--steps <count>`: Set simulation steps (default: 1000)

#### Advanced Options
- `--save <file>`: Save brain state to file
- `--load <file>`: Load brain state from file
- `--verbose`: Enable verbose output
- `--quiet`: Suppress output
- `--learning`: Enable learning experiments
- `--no-development`: Disable development system
- `--no-checkpointing`: Disable checkpointing

#### Presets

| Preset | Description | Neuron Count | Connection Probability |
|--------|-------------|--------------|------------------------|
| `default` | Standard configuration for most users | 500 | 0.15 |
| `research` | Optimized for research experiments | 1000 | 0.05 |
| `demo` | Lightweight for demos and presentations | 100 | 0.20 |
| `production` | Production-ready configuration | 2000 | 0.10 |

## Advanced Features

### Configuration Presets

NLM includes several predefined configuration presets for different use cases:

#### Default Preset
```json
{
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    "random_seed": 42,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001
}
```

#### Research Preset
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.05,
    "random_seed": 12345,
    "stdp_ltp_weight": 0.05,
    "stdp_ltd_weight": 0.025,
    "synaptogenesis_rate": 0.0005,
    "pruning_rate": 0.00005
}
```

### Performance Monitoring

Enable performance monitoring with the `--benchmark` mode or by using custom configurations.

### Checkpointing

The system supports checkpointing for long-running simulations:

```bash
# Enable checkpointing (default)
./nlm --demo phase6 --enable-checkpointing

# Specify checkpoint file
./nlm --demo phase6 --save /path/to/checkpoint.bin
```

### Learning Experiments

Advanced learning experiments are available with enhanced plasticity controls:

```bash
# Run learning experiment with custom parameters
./nlm --demo learning --neurons 500 --learning
```

## Architecture Overview

### Core Components

1. **Neural Engine**: Spiking neural networks with LIF dynamics
2. **Memory Systems**: Working memory, episodic memory, associative memory
3. **Neuromodulation**: Dopamine, curiosity, novelty detection
4. **Prediction Systems**: Forward models and error computation
5. **Cognition Systems**: Planning, attention, concept formation
6. **Development System**: Plasticity modulation over time
7. **Motor System**: Action selection and execution
8. **Sensory System**: Perception and input processing

### Integration Architecture

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Development Guide

### Adding New Demos

1. Create a new demo function following the pattern of existing demos
2. Add the demo to the command-line parsing logic
3. Update the help text accordingly

### Configuration Customization

1. Create a new configuration preset in the configuration file
2. Use the `--preset` option to apply the configuration
3. Override individual parameters as needed

### Performance Optimization

1. Use the `--preset production` for maximum performance
2. Monitor system resources during long-running simulations
3. Use checkpointing for fault tolerance

## Troubleshooting

### Common Issues

#### "Failed to initialize brain"
- Check that all required dependencies are installed
- Ensure sufficient system memory
- Try with smaller neuron counts: `--neurons 100`

#### "No spikes generated"
- Check neuron parameters in configuration
- Verify current injection settings
- Try `--demo phase2` for basic functionality test

#### "Configuration file not found"
- Use absolute paths for configuration files
- Create default configuration: `configs/default.cfg`
- Use presets as an alternative: `--preset demo`

#### Performance issues
- Reduce neuron count for faster testing
- Use `--quiet` mode to reduce output overhead
- Consider using `--preset demo` for testing

## API Reference

### Python Bindings

The NLM framework provides Python bindings for easy integration:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create simple world
world = pynlm.createSimpleWorld()
world.configure(16, 16, 16, 16)

# Create agent brain
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

### C++ API

The C++ API provides full access to all NLM functionality:

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 500);
config->set("region_count", 1);

auto brain = std::make_shared<nlm::Brain>(config);
if (brain->initialize()) {
    // Run simulation
    for (size_t step = 0; step < 1000; ++step) {
        brain->step(step);
    }
}
```

## References

- [NLM Documentation](https://github.com/nlm-project/nlm)
- [Architecture Documentation](docs/ARCHITECTURE.md)
- [Science Documentation](docs/SCIENCE.md)
- [Roadmap](docs/ROADMAP.md)

## License

MIT License - See LICENSE file for details.

## Support

For support, please visit the NLM project repository or create an issue on GitHub.
