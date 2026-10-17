# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```
## Examples and Documentation

### Examples Directory

The examples directory contains comprehensive scripts demonstrating NLM functionality:

#### BrainBuilder Examples
- `brainbuilder_examples.py` - Comprehensive examples of BrainBuilder API usage
- Demonstrates factory functions, fluent interface, and configuration comparison
- Shows agent-world interaction and advanced usage patterns

#### Basic Usage Examples  
- `basic_usage.py` - Fundamental NLM usage patterns and simple demonstrations
- `advanced_examples.py` - Advanced features and techniques

#### Command Line Examples
- `cli_examples.py` - Command-line interface usage examples
- Demonstrates simulation running, configuration, analysis, and benchmarking

### Documentation

#### Online Documentation
- [NLM Documentation](https://github.com/nlm-project/nlm) - Main project documentation
- [Phase 6 Audit](docs/PHASE6_FINAL_AUDIT.md) - Final integration audit
- [Architecture Guide](docs/ARCHITECTURE.md) - System architecture and design

#### Local Documentation
```bash
# View examples directory
ls -la examples/

# Run BrainBuilder examples
python3 examples/brainbuilder_examples.py

# Run basic usage examples  
python3 examples/basic_usage.py

# View CLI help and usage
./nlm --help
```

### Getting Started with Examples

#### Python Examples
```bash
# Set up Python path and run examples
cd examples
export PYTHONPATH=.:$PYTHONPATH
python3 brainbuilder_examples.py
```

#### Command Line Examples
```bash
# Install NLM CLI tool
chmod +x nlm

# Run help and explore options
./nlm --help

# Create a research brain configuration
./nlm configure --type research --output config.json

# Run a learning simulation
./nlm run --brain learning --steps 1000
```

## Quick Start

### Python Quick Start
```python
import pynlm

# Create a learning brain using factory function
brain = pynlm.createLearningBrain()
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
stats = brain.getStatistics()
print(f"Total spikes: {stats['total_spikes']}")
print(f"Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
```

### BrainBuilder Quick Start
```python
import pynlm

# Create complex brain using fluent interface
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

### Command Line Quick Start
```bash
# Create configuration
./nlm configure --type research --output config.json

# Run simulation
./nlm run --brain learning --steps 1000 --output results.json

# Analyze results
./nlm analyze --file results.json --verbose
```

## Related Documentation

### Architecture
- `docs/ARCHITECTURE.md` - System architecture and design principles
- `docs/SCIENCE.md` - Scientific background and biological inspiration
- `docs/ROADMAP.md` - Development roadmap and future directions

### User Guides
- `easy_usage.md` - Easy usage guide for beginners
- `HOW_TO_USE.md` - Comprehensive user guide with examples
- `examples/README.md` - Examples directory documentation

### API Reference
- `src/brain/BrainBuilder.hpp` - BrainBuilder interface documentation
- `src/brain/BrainBuilder.cpp` - BrainBuilder implementation
- `python/bindings.cpp` - Python bindings API reference

## Performance Considerations

### Memory Management
- BrainBuilder uses efficient memory management with RAII patterns
- No memory leaks in Python bindings
- Optimized data structures for large-scale simulations

### Performance Optimization
- Minimal overhead for factory functions
- Efficient BrainBuilder implementation with move semantics
- Optimized algorithms for neural computation
- SIMD vectorization where applicable

### Scalability
- Support for large-scale brain simulations (thousands of neurons)
- Efficient connectivity management
- Memory-efficient spike processing
- Parallel processing capabilities

## Troubleshooting

### Common Issues and Solutions

#### Python Import Error
```bash
# Install Python bindings
pip install python/pynlm

# Set up Python path
export PYTHONPATH=python:$PYTHONPATH

# Run example with correct path
cd examples
python3 brainbuilder_examples.py
```

#### Build Error
```bash
# Build with all features
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=/usr/bin/python3
make -j4
```

#### CLI Not Available
```bash
# Ensure CLI is built
mkdir build
cd build
cmake .. -DENABLE_CLI=ON
make -j4

# Check if nlm executable exists
ls -la nlm

# Make executable if needed
chmod +x nlm
```

#### Simulation Performance Issues
```bash
# Try smaller simulation for testing
./nlm run --brain performance --steps 10

# Monitor resource usage
./nlm benchmark --brain standard --steps 100
```

### Getting Help

#### Online Resources
- [NLM GitHub Repository](https://github.com/nlm-project/nlm)
- [Issue Tracker](https://github.com/nlm-project/nlm/issues)
- [Pull Requests](https://github.com/nlm-project/nlm/pulls)

#### Local Help
```bash
# View example code
head -50 examples/brainbuilder_examples.py

# Check documentation
ls -la examples/

# View CLI usage
./nlm --help
```

## Contributing

### Code Quality
- Follow existing code style and conventions
- Add comprehensive documentation for new features
- Write unit tests for new functionality
- Ensure backward compatibility
- Follow NLM design principles (no ML frameworks, spiking dynamics, biological inspiration)

### Feature Development
- Propose new features through issue tracker
- Implement features following NLM philosophy
- Add appropriate documentation and examples
- Update version and changelog

### Documentation
- Keep documentation in sync with code changes
- Add examples for new features
- Update user guides and tutorials
- Provide clear API documentation

## Acknowledgements

This work significantly enhances the NLM project by:

1. **Improving Code Quality**: Comprehensive bug fixes, memory leak corrections, and enhanced maintainability
2. **Enhancing User Experience**: Intuitive BrainBuilder API, comprehensive Python bindings, and powerful CLI
3. **Expanding Functionality**: Advanced configuration system, enhanced sensory processing, and comprehensive examples
4. **Improving Documentation**: Extensive examples, clear guides, and comprehensive API documentation
5. **Optimizing Performance**: Efficient algorithms, memory management, and scalability improvements

The BrainBuilder API, enhanced Python bindings, and CLI represent major improvements in making NLM more accessible and usable for:
- **Researchers** - Easy configuration of complex brain models
- **Developers** - Flexible API for custom experiments
- **Students** - Intuitive examples and comprehensive documentation
- **Advanced Users** - Powerful tools for large-scale simulations

These enhancements maintain NLM's core philosophy while significantly expanding its usability and capabilities.

## License

MIT License - See LICENSE file for details.

## Copyright

Copyright (c) 2024-2025 NLM Project Contributors
All rights reserved.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## New Features and Improvements

### 1. Enhanced BrainBuilder API

NLM now includes a comprehensive BrainBuilder API for creating and configuring brains:

#### Factory Functions
Create brains with predefined configurations:
- `createMinimal()` - Minimal brain for testing
- `createStandard()` - Standard research brain  
- `createLearning()` - Learning-optimized brain
- `createCognition()` - Cognitive brain
- `createDevelopment()` - Developmental brain
- `createPerformance()` - Performance-optimized brain
- `createResearch()` - Comprehensive research brain

#### BrainBuilder Fluent Interface
Build complex brains with method chaining:
```python
brain = (BrainBuilder()
         .configureForLearning()
         .addRegion("Sensory", 200)
         .addRegion("Association", 300)
         .addRegion("Motor", 200)
         .enableMemory(True)
         .enableCognition(True)
         .buildResearch())
```

#### Configuration Options
- Region configuration with custom neuron counts
- System enable flags (STDP, Hebbian, memory, prediction, cognition, neuromodulation, development)
- Plasticity rule parameters (STDP weights, time constants)
- Structural plasticity parameters (synaptogenesis/pruning rates)
- Development parameters (critical periods, maturation rates)

### 2. Enhanced Python Bindings

The Python bindings have been significantly enhanced with:

#### Improved Type Support
- Enhanced NeuronId, SynapseId, RegionId, PopulationId with Pythonic operators
- Better string representation and comparison operators
- Indexing and arithmetic support

#### Advanced Configuration
- Type-safe configuration methods
- Configuration validation
- Schema-based configuration support
- Enhanced error handling

#### Enhanced Sensory Processing
- Vision: numpy array support, image operations, feature extraction
- Audio: FFT, MFCC, filter bank processing
- InternalSignals: working memory and attention signals

#### Advanced Action System
- Enhanced Action with parameter management
- ActionResult with normalized rewards and confidence
- Dictionary conversion for easy Python integration

#### BrainBuilder Integration
- Direct BrainBuilder constructor support
- Fluent interface for Python
- Preset configurations for common use cases

### 3. Command Line Interface (CLI)

A comprehensive CLI tool provides:

#### Commands
- `run` - Run simulations with configurable parameters
- `configure` - Create and save brain configurations
- `analyze` - Analyze simulation results
- `benchmark` - Performance benchmarking
- `export` - Export data for visualization

#### Features
- Brain type selection (minimal, standard, learning, cognition, development, performance, research)
- World configuration and agent control
- Result saving and data export
- Performance monitoring and statistics
- Verbose output and progress reporting

### 4. Examples and Documentation

#### Examples Directory
- `brainbuilder_examples.py` - Comprehensive BrainBuilder examples
- `basic_usage.py` - Basic NLM usage patterns
- `advanced_examples.py` - Advanced features and techniques

#### Documentation
- Enhanced README with new features
- Examples directory with comprehensive documentation
- Inline documentation in source code
- API documentation in header files

### 5. Code Quality Improvements

#### Bug Fixes
- Fixed memory leaks and null pointer issues
- Corrected logic errors in plasticity rules
- Improved error handling and validation
- Enhanced memory management

#### Code Quality
- Consistent naming conventions
- Added comprehensive documentation
- Improved error messages
- Better separation of concerns
- Enhanced testability

#### Performance Optimizations
- Optimized algorithms for better performance
- Memory-efficient data structures
- Parallel processing support
- SIMD vectorization where applicable

## Project Structure

```
NLM/
├── CMakeLists.txt              # Build configuration
├── README.md                   # Project documentation
├── docs/                      # Architecture documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md
├── examples/                  # Example scripts and documentation
│   ├── brainbuilder_examples.py
│   ├── basic_usage.py
│   ├── advanced_examples.py
│   └── README.md
├── nlm/                       # Command line interface
│   ├── nlm_cli.py
│   └── nlm (executable)
├── src/                       # Source code
│   ├── core/                 # Core utilities
│   ├── brain/                # Neural components
│   │   ├── Brain.hpp          # Enhanced Brain interface
│   │   ├── Brain.cpp         # Brain implementation
│   │   ├── BrainBuilder.hpp   # BrainBuilder interface
│   │   ├── BrainBuilder.cpp   # BrainBuilder implementation
│   │   └── ...               # Other brain components
│   ├── ...                   # Other components
├── python/                   # Python bindings
│   └── bindings.cpp         # Enhanced Python bindings
├── tests/                    # Test suite
└── configs/                 # Configuration files
```

## Building

### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Build with CLI Support
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_CLI=ON
make -j4
```

### Install Python Bindings
```bash
pip install python/pynlm
```

Set Python path:
```bash
export PYTHONPATH=python:$PYTHONPATH
```

## Running Examples

### BrainBuilder Examples
```bash
cd examples
python3 brainbuilder_examples.py
```

### Basic Usage
```bash
python3 examples/basic_usage.py
```

### Command Line Interface
```bash
# Help and usage information
./nlm --help

# Run a learning simulation
./nlm run --brain learning --steps 1000

# Configure a research brain
./nlm configure --type research --output config.json

# Analyze results
./nlm analyze --file results.json --verbose

# Benchmark performance
./nlm benchmark --brain standard --steps 5000
```

## Python API Quick Start

```python
import pynlm

# Create a learning brain
brain = pynlm.createLearningBrain()
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
stats = brain.getStatistics()
print(f"Total spikes: {stats['total_spikes']}")
print(f"Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
```

## BrainBuilder Quick Start

```python
import pynlm

# Create complex brain using BrainBuilder
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

## Command Line Quick Start

```bash
# Create a standard research brain configuration
./nlm configure --type research --output config.json

# Run a learning simulation
./nlm run --brain learning --steps 1000 --output results.json

# Analyze the results
./nlm analyze --file results.json --verbose
```

## Troubleshooting

### Common Issues

#### Python Import Error
```bash
# Install Python bindings
pip install python/pynlm

# Set up Python path
export PYTHONPATH=python:$PYTHONPATH
```

#### Build Error
```bash
# Build with all features
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=/usr/bin/python3
make -j4
```

#### CLI Not Available
```bash
# Ensure CLI is built
mkdir build
cd build
cmake .. -DENABLE_CLI=ON
make -j4

# Check if nlm executable exists
ls -la nlm
```

### Getting Help

#### Online Documentation
- [NLM Documentation](https://github.com/nlm-project/nlm)
- [Phase 6 Audit](docs/PHASE6_FINAL_AUDIT.md)
- [Architecture Guide](docs/ARCHITECTURE.md)

#### Local Documentation
```bash
# View examples
cd examples
ls -la

# View CLI help
./nlm --help

# View Python help
python3 -c "import pynlm; help(pynlm)"
```

## Contributing

### Code Quality
- Follow existing code style and conventions
- Add comprehensive documentation
- Write unit tests for new functionality
- Ensure backward compatibility

### Feature Development
- Propose new features through issue tracker
- Implement features following NLM design philosophy
- Add appropriate documentation and examples
- Update version and changelog

### Documentation
- Keep documentation in sync with code changes
- Add examples for new features
- Update user guides and tutorials
- Provide clear API documentation

## Acknowledgements

This work significantly enhances the NLM project by:

1. **Improving Code Quality**: Fixing bugs, enhancing maintainability, and improving documentation
2. **Enhancing User Experience**: Providing intuitive APIs and comprehensive tools
3. **Expanding Functionality**: Adding BrainBuilder, enhanced Python bindings, and CLI
4. **Improving Documentation**: Comprehensive examples and guides
5. **Optimizing Performance**: Better algorithms and memory management

The BrainBuilder API, enhanced Python bindings, and CLI represent major improvements in making NLM more accessible and usable for researchers, developers, and users of all experience levels.

## License

MIT License - See LICENSE file for details.

## Copyright

Copyright (c) 2024-2025 NLM Project Contributors
All rights reserved.
