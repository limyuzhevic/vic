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

### Standard Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Enhanced Build with Python Bindings

```bash
# For Python bindings with advanced features
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_PYTHON_BINDINGS=ON \
    -DENABLE_ADVANCED_FEATURES=ON \
    -DENABLE_CONVENIENCE_MODULE=ON
make -j4
```

### Building Python Package

```bash
# Build Python package using scikit-build
cd build
pip install -e .  # Install in development mode
# or
cd build
pip install .     # Install standalone
```

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Examples

```bash
# Run Python examples
cd examples
python demo.py
python learning_example.py
python batch_example.py
```

## Python Package Installation

### Quick Installation

```bash
# Install the enhanced Python package
pip install pynlm

# For development, install in editable mode
pip install -e .
```

### Installation from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build and install
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_PYTHON_BINDINGS=ON
make -j4
pip install .
```

## Python Package Features

### Enhanced Python Bindings

NLM now includes enhanced Python bindings with:

#### Convenience Module (`pynlm.conv`)

Access advanced convenience functions:

```python
import pynlm

# Get configuration templates
demo_config = pynlm.conv.create_demo_config()
prod_config = pynlm.conv.create_production_config()
research_config = pynlm.conv.create_research_config()

# Create brain with template
brain = pynlm.createBrain(demo_config)
brain.initialize()

# Use batch operations
brain.runSteps(1000)
```

#### Advanced Learning Systems

Enable learning systems with simple commands:

```python
import pynlm

# Create agent with world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation with learning
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Configuration Templates

Quick access to different configuration presets:

```python
import pynlm

# Demo configuration (quick testing)
demo_config = pynlm.conv.create_demo_config()
print(f"Demo config: {demo_config.getOr<int64_t>('brain.neuron_count', 0)} neurons")

# Production configuration (balanced)
prod_config = pynlm.conv.create_production_config()
print(f"Production config: {prod_config.getOr<int64_t>('brain.neuron_count', 0)} neurons")

# Research configuration (maximum)
research_config = pynlm.conv.create_research_config()
print(f"Research config: {research_config.getOr<int64_t>('brain.neuron_count', 0)} neurons")
```

#### Statistics and Analysis

Comprehensive brain analysis:

```python
import pynlm

brain = pynlm.createBrain(pynlm.conv.create_production_config())
brain.initialize()

# Run simulation
brain.runSteps(1000)

# Get comprehensive statistics
stats = brain.compute_neural_statistics()
print("=== Neural Network Analysis ===")
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Network density: {stats['network_density']:.4f}")
print(f"Firing rate per second: {stats['spike_rate_per_second']:.1f}")
print(f"Firing percentage: {stats['firing_percentage']:.1f}%")
print(f"E/I ratio: {stats['excitation_inhibition_ratio']:.2f}")
```

#### Batch Processing

Efficient batch operations:

```python
import pynlm

brain = pynlm.createBrain(pynlm.conv.create_research_config())
brain.initialize()

# Create multiple neurons
neuron_ids = pynlm.conv.create_neuron_batch(list(range(10)))
currents = [5.0] * 10

# Inject currents to all neurons at once
brain.batch_neurons(neuron_ids, currents)

# Run batch simulation
results = brain.runSteps(500)
```

#### Error Handling and Validation

Robust error handling:

```python
import pynlm

brain = pynlm.createBrain(pynlm.conv.create_demo_config())
brain.initialize()

# Validate brain state
validation = pynlm.conv.validate_brain_state(brain)
if not validation['is_valid']:
    print(f"Issues found: {validation['issues']}")
    # Fix issues
    fixed = pynlm.conv.validate_and_fix_brain(brain)
    print(f"Fixes applied: {fixed['fixes_applied']}")

# Safe step with error handling
result = pynlm.conv.safe_brain_step(brain, 100, 0.1)
if result['success']:
    print("Step completed successfully")
else:
    print(f"Step failed: {result['error']}")
```

#### Educational Examples

Pre-built examples for learning:

```python
import pynlm

# Create educational examples
basic_example = pynlm.conv.create_educational_example("basic")
learning_example = pynlm.conv.create_educational_example("learning")

# Run basic example
brain = basic_example['brain']
brain.initialize()
brain.runSteps(100)

# Run learning example
brain = learning_example['brain']
agent = learning_example['agent']
world = pynlm.createSimpleWorld()
world.reset()

agent.initialize(world)
# Run simulation...
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Enhanced build system
├── pyproject.toml                    # Python package configuration
├── README.md                          # This file
├── python/                           # Python bindings
│   ├── bindings.cpp                  # Enhanced C++ bindings
│   ├── NLM_CONVENIENCE.md             # Convenience module documentation
├── python/examples/                   # Example scripts
│   ├── demo.py                       # Basic demo
│   ├── learning_example.py            # Learning example
│   ├── batch_example.py              # Batch processing example
│   └── visualization_example.py      # Visualization example
├── examples/                         # C++ examples
│   ├── main.cpp                      # Main C++ example
│   ├── phase6_demo.cpp                # Phase 6 demo
├── src/                              # C++ source code
│   ├── core/                         # Core utilities
│   ├── brain/                        # Neural components
│   ├── dynamics/                     # Neural dynamics
│   ├── plasticity/                   # Plasticity systems
│   ├── development/                  # Development system
│   ├── neuromodulation/               # Neuromodulators
│   ├── memory/                       # Memory systems
│   ├── cognition/                    # Cognitive mechanisms
│   ├── prediction/                   # Prediction systems
│   ├── sensory/                      # Sensory processing
│   ├── motor/                        # Motor system
│   ├── environment/                  # Environment interface
│   ├── experiments/                  # Experiment framework
│   └── visualization/                # Visualization
├── docs/                             # Documentation
│   ├── ARCHITECTURE.md               # System architecture
│   ├── SCIENCE.md                    # Scientific background
│   ├── ROADMAP.md                    # Development roadmap
│   ├── EXPERIMENTS.md                 # Experiment descriptions
│   ├── LIMITATIONS.md                 # Known limitations
│   └── PHASE6_FINAL_AUDIT.md          # Phase 6 audit
├── configs/                          # Configuration files
│   ├── default.cfg                   # Default configuration
│   └── template.cfg                  # Template configuration
├── tests/                            # Test suite
│   ├── unit/                         # Unit tests
│   ├── integration/                  # Integration tests
│   └── python/                       # Python tests
├── IMPROVEMENTS_SUMMARY.md           # Summary of improvements
├── easy_usage.md                     # Beginner-friendly guide
└── HOW_TO_USE.md                     # Detailed user guide
```

## Key Improvements

### 1. Enhanced Python Bindings

- Comprehensive convenience methods
- Advanced error handling and validation
- Batch processing capabilities
- Educational examples
- Configuration templates

### 2. Convenience Module

New `pynlm.conv` module with:
- Configuration templates (demo, production, research)
- Simulation utilities
- Data conversion tools
- Validation and error handling
- Educational examples
- Batch processing utilities

### 3. Advanced Features

- Enhanced learning systems (reward, curiosity, development)
- Comprehensive statistics analysis
- Performance monitoring
- Debugging and diagnostic tools
- Visualization support
- Experiment management

### 4. Build System Improvements

- Enhanced CMake configuration
- Python bindings support
- Installation targets for Python module
- Documentation generation
- Testing infrastructure
- Configuration management

### 5. Documentation Improvements

- Comprehensive README with examples
- Beginner-friendly easy_usage.md
- Advanced HOW_TO_USE.md
- Convenience module documentation
- Improvements summary
- API reference documentation

### 6. Configuration Management

- Multiple configuration templates
- Configuration validation
- Error recovery
- Advanced configuration options
- Template-based configuration

## Usage Examples

### Basic Usage

```python
import pynlm

# Quick start with demo configuration
config = pynlm.conv.create_demo_config()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### Advanced Usage

```python
import pynlm

# Research-grade configuration
config = pynlm.conv.create_research_config()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced learning
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Create world with enriched environment
world = pynlm.create_enriched_world(50, 50, 16, 16, True, True)

# Run training
results = agent.run_episode(world, maxSteps=10000, verbose=True)

# Analyze results
stats = brain.compute_neural_statistics()
print(f"Training completed in {results['steps_completed']} steps")
print(f"Average reward: {results['average_reward_per_step']:.3f}")
```

### Batch Processing

```python
import pynlm

# Create research-grade brain with many neurons
brain = pynlm.createBrain(pynlm.conv.create_research_config())
brain.initialize()

# Create batch of neurons
neuron_ids = pynlm.conv.create_neuron_batch(list(range(100)))
currents = [5.0 + (i % 10) * 2.0 for i in range(100)]

# Inject currents to all neurons
print(f"Injecting currents to {len(neuron_ids)} neurons...")
brain.batch_neurons(neuron_ids, currents)

# Run batch simulation
print("Running batch simulation...")
results = brain.runSteps(1000)
print(f"Batch simulation complete: {results['total_spikes']} spikes")
```

## Installation

### System Dependencies

```bash
# Required dependencies
sudo apt-get install build-essential cmake python3-dev python3-pip

# Optional dependencies for enhanced features
sudo apt-get install python3-numpy python3-matplotlib python3-scipy
```

### Python Package Installation

```bash
# Install using pip
pip install pynlm

# Install in development mode
pip install -e .
```

### Building from Source

```bash
# Clone repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build with Python bindings
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_PYTHON_BINDINGS=ON
make -j4

# Install Python package
pip install .
```

## Testing

### Running Tests

```bash
# Run C++ tests
cd build
ctest --output-on-failure

# Run Python tests
pytest tests/

# Run all tests (including Python)
./runtests  # Custom target from CMake
```

### Python Test Examples

```python
import pynlm
import pynlm.conv as conv

# Test convenience module
config = conv.create_demo_config()
assert config.getOr<int64_t>("brain.neuron_count", 0) > 0

# Test brain creation
brain = pynlm.createBrain(config)
assert brain is not None
brain.initialize()

# Test batch operations
brain.runSteps(10)

# Test statistics
stats = brain.compute_neural_statistics()
assert "total_neurons" in stats

print("All tests passed!")
```

## Troubleshooting

### Common Issues

**Python Bindings Not Found**

```bash
# Check if Python bindings are built
ls -la build/python/

# Reinstall Python package
pip install -e .
```

**Import Error**

```python
import pynlm
# If this fails, check:
# 1. Python bindings are installed
# 2. Python path includes the module
# 3. Dependencies are installed
```

**Configuration Errors**

```python
import pynlm
config = pynlm.createDefaultConfig()
# Check for errors
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.getOr<int64_t>('brain.neuron_count', 0)}")
```

**Performance Issues**

```python
import pynlm

# Use batch operations for better performance
brain = pynlm.createBrain(pynlm.conv.create_demo_config())
brain.initialize()

# Run batch instead of single steps
brain.runSteps(1000)

# Use larger timesteps for faster simulation
# (if supported by configuration)
```

## Getting Help

### Documentation

- **Main Documentation**: `README.md` (this file)
- **Beginner Guide**: `easy_usage.md`
- **Advanced Guide**: `HOW_TO_USE.md`
- **Convenience Module**: `python/NLM_CONVENIENCE.md`
- **Improvements Summary**: `IMPROVEMENTS_SUMMARY.md`

### Examples

- **Python Examples**: `python/examples/`
- **C++ Examples**: `examples/`
- **Documentation**: `docs/`

### Support

- **GitHub Issues**: [Open an issue](https://github.com/nlm-project/nlm/issues)
- **Discussions**: [GitHub Discussions](https://github.com/nlm-project/nlm/discussions)
- **Email**: nlm-support@nlm-project.org

## Contributing

### Code Contributions

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests and documentation
5. Submit a pull request

### Documentation Contributions

1. Fork the repository
2. Create a feature branch
3. Update documentation
4. Submit a pull request

### Bug Reports

1. Open an issue on GitHub
2. Provide detailed reproduction steps
3. Include configuration information
4. Include error messages and stack traces

## License

MIT License

Copyright (c) 2024 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Acknowledgments

- Inspired by biological brain research
- Built on insights from neuroscience
- Thanks to the open-source community
- Special thanks to contributors and users

## Version

Version: 0.1.0 (Alpha)

This is an experimental project with ongoing improvements. Feedback and contributions are welcome!
