# NLM (熙然) - Neural Learning Machine

## Overview

NLM (熙然) is a comprehensive framework for artificial neural computation and brain simulation. This repository implements a full-featured spiking neural network system with real neuron dynamics, plasticity mechanisms, and learning capabilities.

## Key Features

### Core Neural Computation
- **LIF Neuron Dynamics**: Leaky Integrate-and-Fire neurons with realistic membrane potential dynamics
- **Event-Driven Spike Propagation**: Realistic spike timing with synaptic delays
- **Multi-Region Architecture**: Hierarchical brain organization with multiple neural regions
- **Full Plasticity Suite**: STDP, Hebbian learning, and structural plasticity

### Learning and Adaptation
- **Experience-Dependent Learning**: Synaptic weights change through activity
- **Neuromodulation**: Dopamine, reward, and prediction error systems
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Cognitive Functions**: Planning, concept formation, and prediction

### Environment and Interaction
- **Sensory Systems**: Vision, audio, and internal signal processing
- **Motor Control**: Action generation and execution
- **World Simulation**: Interactive environments with objects
- **Visualization**: Brain state visualization and analysis

## CLI Interface

The NLM project includes a comprehensive command-line interface (CLI) that provides access to all simulation and analysis capabilities:

### Available Commands

1. **`nlm run`** - Execute brain simulations with various experiment types
2. **`nlm inspect`** - Inspect brain state, configuration, and statistics
3. **`nlm config`** - Manage configuration files and parameters
4. **`nlm benchmark`** - Run performance benchmarks on different network sizes
5. **`nlm replay`** - Replay saved brain states and simulation episodes
6. **`nlm checkpoint`** - Manage simulation checkpoints
7. **`nlm debug`** - Debugging tools and verbose output
8. **`nlm simulate`** - Run specific simulation scenarios
9. **`nlm explore`** - Explore brain structure and connections

### CLI Features

- **Command-line argument parsing** with long and short options
- **Configuration file loading** (JSON/YAML/txt formats)
- **Help and usage documentation** for each subcommand
- **Logging level control** (Warning, Info, Debug)
- **Statistics collection** and detailed reporting
- **Error handling** and validation
- **Export capabilities** for brain state and statistics
- **Scripting support** with configuration files
- **Plugin architecture** for extending functionality
- **Command history** and session management

## Usage Examples

### Basic Simulation
```bash
# Run a basic connectivity simulation
nlm run --experiment basic --steps 1000 --neuron-count 1000

# Run with custom configuration
nlm run -c my_config.cfg --experiment plasticity
```

### Inspection
```bash
# Inspect current brain state
nlm inspect --detailed

# Export brain state to JSON
nlm inspect --export json --export-file brain_state.json
```

### Configuration Management
```bash
# List current configuration
nlm config list

# Set a configuration value
nlm config set neuron_count int:2000

# Save configuration
nlm config save my_config.cfg

# Load configuration
nlm config load my_config.cfg
```

### Benchmarking
```bash
# Run performance benchmarks
nlm benchmark --neuron-counts 100,500,1000,2000

# Save results to file
nlm benchmark --output benchmark_results.csv --format csv
```

### Exploration
```bash
# Explore brain structure
nlm explore --region 1 --neuron 5 --synapse 10

# Show connection statistics
nlm explore --connections

# Export brain structure to Graphviz format
nlm explore --export dot --export-file brain.dot
```

## Project Structure

### Source Code
- `src/` - Core NLM implementation
  - `brain/` - Neural computation (neurons, synapses, regions)
  - `core/` - Core systems (config, logging, random, simulation)
  - `environment/` - Simulation environment
  - `experiments/` - Various experiment implementations
  - `memory/` - Memory systems
  - `motor/` - Motor control
  - `neuromodulation/` - Neuromodulation systems
  - `plasticity/` - Plasticity mechanisms
  - `prediction/` - Prediction systems
  - `sensory/` - Sensory processing
  - `visualization/` - Visualization interface

### CLI Interface
- `cli/` - Command-line interface
  - `nlm_cli.cpp` - Main CLI entry point
  - Individual command files for each subcommand

### Configuration
- `configs/` - Default configuration files
- `docs/` - Documentation and examples

## Building and Installation

### Prerequisites
- C++20 compatible compiler
- CMake 3.16 or higher

### Build Instructions
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Installation
```bash
cmake --install . --prefix /usr/local
```

## Running Tests

```bash
cd build
ctest --output-on-failure
```

## Advanced Features

### Scripting Support
The CLI supports JSON and YAML configuration files for scripting:

```json
{
  "neuron_count": 1000,
  "region_count": 2,
  "connection_probability": 0.1,
  "experiment": "plasticity",
  "steps": 2000
}
```

### Plugin Architecture
The CLI supports plugins for extending functionality:

- Plugin directory: `plugins/`
- Plugin format: Shared libraries (.so/.dll)
- Plugin interface: Standard command registration

### Session Management
- Command history: `.nlm_history` file
- Session save/load: `--save-session` and `--load-session` options
- Configuration persistence: Automatic saving

## Performance Optimization

The NLM framework includes several performance optimization features:

- **Event-driven simulation**: Only processes neurons that spike
- **Parallel processing**: Multiple regions can simulate concurrently
- **Memory-efficient data structures**: Compact neuron and synapse representations
- **Optimized synaptic propagation**: Fast spike transmission with delays
- **Efficient plasticity updates**: Localized weight updates

## Research Applications

The NLM framework is suitable for various research applications:

- **Neural circuit modeling**: Study of brain connectivity and function
- **Machine learning**: Alternative to traditional neural networks
- **Cognitive modeling**: Implementation of cognitive processes
- **Neuromorphic computing**: Energy-efficient neural computation
- **Synthetic biology**: Modeling of biological neural systems

## License

MIT License - See LICENSE file for details.

## Contributing

Contributions are welcome! Please follow the project's coding standards and submit pull requests.

## Contact

For questions, issues, or collaboration opportunities, please refer to the project's documentation and issue tracker.
