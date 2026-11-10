# NLM (Neural Learning Machine) - Improved Python API

This directory contains the improved Python bindings and API for NLM, featuring:

## Key Improvements

### 1. **Enhanced Package Structure**
- Modular Python package with organized submodules
- Comprehensive documentation and examples
- Improved error handling and type safety

### 2. **Advanced Configuration Management**
- Schema-based configuration validation
- Preset configurations for different use cases
- Runtime configuration modification

### 3. **Performance Benchmarking**
- Comprehensive benchmark suite for performance testing
- Memory usage analysis and optimization
- Scalability testing across different brain sizes

### 4. **Advanced Features**
- Performance monitoring and metrics
- Error handling and validation
- Example scenarios and tutorials

### 5. **Developer Tools**
- Command-line interface for NLM operations
- Performance analysis tools
- Experimental example suite

## Installation and Setup

### Installation

Install the NLM Python package:

```bash
pip install pynlm
```

Or install in development mode:

```bash
pip install -e .
```

### Build from Source

Build the Python bindings from source:

```bash
# Clone the repository
cd /path/to/nlm

# Create build directory
mkdir build
cd build

# Configure with CMake (for C++ core)
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON

# Build Python bindings
pip install .
```

## Usage Examples

### Basic Python Usage

Create and run a simple NLM brain:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Agent Simulation

Create a complete agent simulation:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent
agent.initialize(world)
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Advanced Configuration

Use predefined configuration presets:

```python
import pynlm

# Create configuration with preset
config = pynlm.createDefaultConfig()

# Load research preset (larger brain, advanced features)
config.load_preset("research")

# Modify specific parameters
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.learning_rate", 0.002)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

## Key Modules

### 1. Core Module (`pynlm.core`)

Basic types and fundamental classes:

```python
from pynlm.core import (
    Config, ConfigSource, ConfigSchema,
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, MotorCommand,
    Brain, Logger, SimulationStep, Timestamp
)
```

### 2. Brain Module (`pynlm.brain`)

Neural system components:

```python
from pynlm.brain import (
    Brain, NeuralRegion, Neuron, Synapse,
    SpikeSystem, PlasticityRule, STDP, Hebbian
)
```

### 3. Config Module (`pynlm.config`)

Advanced configuration management:

```python
from pynlm.config import (
    ConfigManager, ConfigSchema, ConfigValidationError,
    ConfigPresets, create_default_config,
    validateConfig, load_preset, mergeConfigs
)
```

### 4. Benchmark Module (`pynlm.benchmark`)

Performance testing and benchmarking:

```python
from pynlm.benchmark import (
    NLMBenchmark, BenchmarkResult, BenchmarkStage,
    PerformanceMetrics, benchmark_all
)
```

### 5. CLI Module (`pynlm.cli`)

Command-line interface:

```python
from pynlm.cli import main
```

### 6. Examples Module (`pynlm.examples`)

Example scenarios and tutorials:

```python
from pynlm.examples import (
    run_all_examples, get_example_list, run_example,
    ExamplesDirectory
)
```

## Command-Line Interface

### Basic Operations

```bash
# Create and initialize a brain
nlm-python create-brain --neurons 1000 --steps 100

# Run agent simulation
nlm-python agent --steps 500 --world-size 20 --enable-curiosity

# Run benchmarks
nlm-python benchmark --type all --output benchmarks.json

# Create and configure a world
nlm-python create-world --width 30 --height 30 --vision-size 8

# Run development simulation
nlm-python develop --steps 200 --enable-curiosity --enable-development
```

### Configuration Management

```bash
# Create configuration from preset
nlm-python utils create-config --filename myconfig.json --preset research

# Run example
nlm-python example basic

# Run all examples
nlm-python example all
```

## Performance Benchmarking

### Basic Benchmarking

```python
import pynlm

benchmark = pynlm.NLMBenchmarkSuite()

# Run all benchmarks
results = benchmark.run_all_benchmarks()

# Print summary
benchmark.print_summary()

# Save results
benchmark.save_results("benchmark_results.json")
```

### Specific Benchmarking

```python
import pynlm

benchmark = pynlm.NLMBenchmarkSuite()

# Run creation benchmarks
creation_results = benchmark.run_brain_creation_benchmark()

# Run simulation benchmarks
simulation_results = benchmark.benchmark_simulation_speed()

# Run plasticity benchmarks
plasticity_results = benchmark.benchmark_plasticity_performance()

# Run memory benchmarks
memory_results = benchmark.run_memory_benchmark()
```

## Examples and Tutorials

### Running Examples

```python
import pynlm

# Run all examples
results = pynlm.run_all_examples()

# Get example list
examples = pynlm.get_example_list()

# Run specific example
result = pynlm.run_example('basic_brain')
```

### Example Categories

- **Getting Started**: Basic concepts and simple usage
- **Interaction**: Agent-world interaction and communication
- **Learning**: Plasticity and learning mechanisms
- **Memory**: Memory systems and consolidation
- **Modulation**: Neuromodulation and motivation
- **Cognition**: Cognitive processes and prediction
- **Performance**: Benchmarking and optimization
- **Persistence**: Checkpointing and state management
- **Multi-Agent**: Social learning and coordination

## Development and Testing

### Running Tests

```bash
# Run unit tests
pytest tests/

# Run integration tests
pytest tests/integration/

# Run performance tests
pytest tests/performance/

# Run examples
python -m pynlm.examples
```

### Development Setup

```bash
# Install development dependencies
git clone https://github.com/nlm-project/nlm.git
cd nlm
pip install -r requirements-dev.txt

# Run examples
python -m pynlm.examples

# Run benchmarks
python -m pynlm.benchmark
```

## Advanced Features

### Performance Monitoring

```python
import pynlm

# Create performance monitor
monitor = pynlm.PerformanceMonitor()

# Start timing operation
start_time = monitor.start_measurement("simulation")

# Perform operation
brain.step(step)

# End timing and record
monitor.end_measurement("simulation", start_time)

# Get performance summary
summary = monitor.get_summary("simulation")
```

### Error Handling and Validation

```python
import pynlm

try:
    # Create and run simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
        
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
```

### Results Export and Import

```python
import pynlm

# Create results dictionary
results = {
    "simulation": {
        "steps_completed": 100,
        "total_spikes": 500,
        "average_firing_rate": 5.0
    },
    "performance": {
        "mean_time": 0.125,
        "std_time": 0.01
    }
}

# Export to file
pynlm.export_results(results, "results.json")

# Import from file
imported_results = pynlm.import_results("results.json")
```

## Configuration Presets

### Available Presets

```python
import pynlm

# List available presets
presets = pynlm.ConfigPresets.list_presets()
print(f"Available presets: {presets}")

# Load specific preset
config = pynlm.createDefaultConfig()
config.load_preset("research")
```

### Preset Definitions

- **minimal**: Small brain, no advanced features
- **standard**: Medium-sized brain with basic features
- **research**: Large brain with advanced features
- **simulation**: Optimized for simulation performance

## Troubleshooting

### Common Issues and Solutions

1. **"Brain not doing anything"**
   - Ensure you call `brain.initialize()` before `brain.step()`
   - Check that all required features are enabled
   - Use more simulation steps

2. **"Everything is 0"**
   - Brains need time to "warm up" - try more steps
   - Ensure sensory input is connected
   - Check configuration parameters

3. **"Performance issues"**
   - Use benchmarks to identify bottlenecks
   - Optimize brain size for your use case
   - Consider using presets for optimal configurations

### Getting Help

1. **Documentation**: Check README.md and example files
2. **Community**: Join NLM discussion forums
3. **Issues**: Report bugs on GitHub
4. **Examples**: Run examples to understand functionality

## Advanced Usage

### Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 3)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.learning_rate", 0.01)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Use custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### Performance Optimization

```python
import pynlm

# Create benchmark for optimization
benchmark = pynlm.NLMBenchmarkSuite()

# Run benchmarks
results = benchmark.run_all_benchmarks()

# Analyze and optimize
for stage in ["creation", "simulation", "plasticity"]:
    stage_results = [r for r in results if r.stage.value == stage]
    if stage_results:
        # Analyze performance
        avg_time = sum(r.metrics.wall_time for r in stage_results) / len(stage_results)
        print(f"{stage} average time: {avg_time:.3f}s")
```

## Conclusion

The improved NLM Python API provides:

1. **Enhanced Usability**: Better package structure and documentation
2. **Advanced Features**: Performance monitoring, benchmarking, and examples
3. **Development Support**: CLI tools, testing frameworks, and examples
4. **Research Capabilities**: Comprehensive benchmarking and validation
5. **Performance Optimization**: Tools to measure and improve performance

The NLM Python API is now more powerful, easier to use, and better suited for research, development, and production environments.

## Quick Start

```bash
# Install
pip install pynlm

# Run examples
python -m pynlm.examples

# Run benchmarks
python -m pynlm.benchmark

# Check documentation
python -m pynlm.cli --help
```

## Support and Community

- **Documentation**: https://nlm.readthedocs.io
- **Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Repository**: https://github.com/nlm-project/nlm

The NLM project welcomes contributions, suggestions, and feedback from the community!
