# NLM Examples

This directory contains example scripts demonstrating NLM (Neural Learning Machine)
functionality and new features.

## Contents

### BrainBuilder Examples
- `brainbuilder_examples.py` - Comprehensive examples of BrainBuilder usage

### CLI Examples
- `cli_examples.py` - Command-line interface usage examples

### Basic Usage
- `basic_usage.py` - Simple NLM usage patterns

### Advanced Examples
- `advanced_examples.py` - Advanced NLM features and techniques

## Installation

1. Build NLM with Python bindings:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. Install Python bindings:
   ```bash
   pip install python/pynlm
   ```

3. Set up Python path:
   ```bash
   export PYTHONPATH=$PROJECT_ROOT/python:$PYTHONPATH
   ```

## Running Examples

### BrainBuilder Examples

```bash
# Run all BrainBuilder examples
cd /path/to/nlm/examples
python3 brainbuilder_examples.py
```

### Basic Usage Examples

```bash
# Run basic usage examples
python3 examples/basic_usage.py
```

### CLI Examples

```bash
# Install the NLM CLI tool
chmod +x nlm

# Run CLI help
./nlm --help

# Run a simple simulation
./nlm run --brain learning --steps 100

# Configure a brain
./nlm configure --type research --output config.json

# Analyze results
./nlm analyze --file results.json --verbose

# Run benchmarks
./nlm benchmark --brain standard --steps 1000
```

## BrainBuilder API Overview

The BrainBuilder provides a fluent interface for creating and configuring NLM brains:

### Factory Functions
- `createMinimal()` - Create minimal brain for testing
- `createStandard()` - Create standard research brain
- `createLearning()` - Create learning-optimized brain
- `createCognition()` - Create cognitive brain
- `createDevelopment()` - Create developmental brain
- `createPerformance()` - Create performance-optimized brain
- `createResearch()` - Create comprehensive research brain

### BrainBuilder Methods
- `configureForLearning()` - Configure for rapid adaptation
- `configureForCognition()` - Configure for cognitive tasks
- `configureForPerformance()` - Configure for speed and efficiency
- `configureForDevelopment()` - Configure for development studies
- `configureForStandardResearch()` - Configure for general research
- `configureForMinimal()` - Configure for testing

- `addRegion(name, neuronCount)` - Add a neural region
- `setNeuronCount(count)` - Set total neuron count
- `setRegionCount(count)` - Set number of regions
- `setConnectionProbability(prob)` - Set connection probability
- `enableSTDP(enable)` - Enable/disable STDP
- `enableHebbian(enable)` - Enable/disable Hebbian learning
- `enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `enableMemory(enable)` - Enable/disable memory systems
- `enablePrediction(enable)` - Enable/disable prediction system
- `enableCognition(enable)` - Enable/disable cognition systems
- `enableNeuromodulation(enable)` - Enable/disable neuromodulation
- `enableDevelopment(enable)` - Enable/disable development system

### Build Methods
- `buildMinimal()` - Build minimal brain
- `buildStandard()` - Build standard brain
- `buildLearning()` - Build learning brain
- `buildCognition()` - Build cognitive brain
- `buildDevelopment()` - Build developmental brain
- `buildPerformance()` - Build performance brain
- `buildResearch()` - Build research brain

## Command Line Interface (CLI)

The NLM CLI provides a comprehensive command-line interface for:

### Commands

#### `run`
Run a simulation with specified parameters:

```bash
nlm run [OPTIONS]
```

Options:
- `--brain TYPE` - Brain type (minimal, standard, learning, cognition, development, performance, research)
- `--steps N` - Number of simulation steps
- `--output FILE` - Save simulation results
- `--export FORMAT` - Export data in specified format
- `--enable-learning`/`--disable-learning` - Enable/disable learning
- World configuration options

#### `configure`
Configure and save a brain configuration:

```bash
nlm configure [OPTIONS]
```

Options:
- `--type TYPE` - Brain type
- `--output FILE` - Output configuration file
- Neuron and system enable flags

#### `analyze`
Analyze simulation results:

```bash
nlm analyze [OPTIONS]
```

Options:
- `--file FILE` - Input results file
- `--report FILE` - Output statistics report
- `--verbose` - Verbose output

#### `benchmark`
Run performance benchmarks:

```bash
nlm benchmark [OPTIONS]
```

Options:
- `--brain TYPE` - Brain type
- `--steps N` - Number of benchmark steps
- `--output FILE` - Output benchmark results

#### `export`
Export simulation data:

```bash
nlm export [OPTIONS]
```

Options:
- `--format FORMAT` - Export format (csv, json, mat, hdf5)
- `--input FILE` - Input simulation file
- `--output FILE` - Output exported data file

### Global Options

- `--version` - Show version information
- `--verbose` - Enable verbose output
- `--quiet` - Suppress non-error output

## Python API Enhancements

### Enhanced Type Support

The NLM Python bindings now include:

#### Enhanced Identifiers
- `NeuronId` with Pythonic arithmetic and string representation
- `SynapseId` with comparison operators
- `RegionId` with indexing support
- `PopulationId` with hash support

#### Enumeration Types
- `NeuronType` - Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
- `SynapseType` - Excitatory, Inhibitory, Modulatory, Electrical, GapJunction
- `DevelopmentalStage` - Initial, CriticalPeriod, Maturation, Adult, Aging
- `FiringState` - Resting, Active, Refractory, Inhibited
- `ActionType` - Movement, perception, interaction, maintenance actions

### Enhanced Configuration

The Config class now includes:
- Type-safe configuration methods
- Configuration validation
- Configuration merging
- Enhanced get/set methods
- Validation and error handling

### Enhanced Sensory Input

#### Vision
- Support for numpy arrays
- Image operations (blur, resize)
- Data normalization
- Feature extraction support

#### Audio
- FFT computation
- MFCC computation
- Filter bank support
- Signal processing

#### InternalSignals
- Signal management
- Working memory support
- Attention signals
- Excitability signals

### Enhanced Action System

#### Action
- Parameter management
- Dictionary conversion
- Cloning support
- Serialization support

#### ActionResult
- Normalized rewards
- Confidence calculation
- Success/failure status
- Dictionary conversion

### Enhanced Brain Interface

#### BrainBuilder Integration
- Direct BrainBuilder constructor support
- Fluent interface for configuration
- Preset configuration methods

#### Statistics
- Comprehensive statistics methods
- Dictionary conversion
- Statistics report generation
- Performance metrics

#### Enhanced AgentBrain
- Learning system management
- Neuromodulation state tracking
- Episode-based training
- Development system integration

## Advanced Features

### Memory and Performance Monitoring
- Memory management tools
- Performance monitoring
- Profiling capabilities
- Resource usage tracking

### Visualization Support
- Data export for visualization
- Plot generation support
- Data formatting for common tools

### Advanced Analysis Tools
- Pattern analysis
- Behavior analysis
- Statistical analysis
- Machine learning integration

## Examples and Tutorials

### Quick Start
```python
import pynlm

# Create a learning brain
brain = pynlm.createLearningBrain()
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
```

### Agent-World Interaction
```python
import pynlm

# Create components
brain = pynlm.createLearningBrain()
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run interaction
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### BrainBuilder Fluent Interface
```python
import pynlm

brain = (pynlm.BrainBuilder()
         .configureForLearning()
         .addRegion("Sensory", 200)
         .addRegion("Association", 300)
         .addRegion("Motor", 200)
         .enableMemory(True)
         .enableCognition(True)
         .buildResearch())

brain.initialize()
```

## Performance Considerations

### Memory Management
- BrainBuilder uses proper memory management
- No memory leaks in Python bindings
- Efficient configuration system

### Performance Optimization
- Minimal overhead for factory functions
- Efficient BrainBuilder implementation
- Optimized data structures

### Scalability
- Support for large-scale simulations
- Parallel processing capabilities
- Memory-efficient data handling

## Troubleshooting

### Common Issues

#### Python Import Errors
```bash
# Install NLM Python bindings
pip install python/pynlm

# Set up Python path
export PYTHONPATH=/path/to/nlm/python:$PYTHONPATH
```

#### Build Errors
```bash
# Build with all features
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=/usr/bin/python3
make -j4
```

#### Runtime Errors
- Check that all required dependencies are installed
- Ensure proper memory allocation
- Verify configuration parameters

### Debugging Tips

#### Brain Builder Issues
- Use `--verbose` flag with CLI
- Check brain configuration
- Verify initialization

#### Simulation Issues
- Use smaller step counts for debugging
- Enable verbose output
- Check sensor input
- Verify world configuration

## Related Documentation

### Architecture
- `docs/ARCHITECTURE.md` - System architecture and design
- `docs/SCIENCE.md` - Scientific background
- `docs/ROADMAP.md` - Development roadmap

### User Guides
- `easy_usage.md` - Easy usage guide for beginners
- `HOW_TO_USE.md` - Comprehensive user guide

### API Reference
- `src/brain/BrainBuilder.hpp` - BrainBuilder header
- `src/brain/BrainBuilder.cpp` - BrainBuilder implementation
- `python/bindings.cpp` - Python bindings source

## Acknowledgements

This work builds on the existing NLM framework and adds significant enhancements to:

- Code quality and maintainability
- User experience and accessibility
- Performance and scalability
- Extensibility and customization
- Documentation and examples

The BrainBuilder and enhanced Python bindings represent a major step forward in making NLM more accessible and usable for researchers, developers, and users of all experience levels.

## License

MIT License - See LICENSE file for details.

## Copyright

Copyright (c) 2024-2025 NLM Project Contributors
All rights reserved.
