# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

**NEW IN THIS VERSION:**
- Enhanced Python bindings with improved error handling and Pythonic interfaces
- Comprehensive research utilities for advanced analysis and visualization
- Python CLI tools for common operations
- Comprehensive API documentation with examples
- Advanced build system with enhanced development dependencies
- Integration testing and validation suite

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

## Python Bindings

The NLM Python bindings provide comprehensive access to the NLM C++ neural simulation framework with enhanced features:

### Key Features

#### Enhanced API with Pythonic Interfaces
- **Error Handling**: Comprehensive error handling with descriptive exceptions
- **Context Managers**: `SimulationContext` for controlled execution
- **Property Access**: Pythonic property-based access to brain state
- **Validation**: Configuration and state validation
- **Type Safety**: Enhanced type checking and validation

#### Core Classes
- **Brain**: Central neural simulation with integrated memory, prediction, and cognition
- **Config**: Configuration management with validation and safety checks
- **SimpleWorld**: 2D world simulation for agent-environment interaction
- **AgentBrain**: Agent interface connecting brain to world
- **SensoryInput**: Base class for vision, audio, and internal signals
- **Action**: Motor output representation
- **WorldObject**: World object representation

#### Advanced Features
- **Research Utilities**: `research_utils.py` with advanced analysis and visualization
- **Simulation Managers**: Enhanced simulation control with statistics
- **Property Access**: Dynamic property access to brain state
- **Debugging Tools**: Enhanced logging and debugging utilities

### Installation

```bash
# Install with basic dependencies
pip install pynlm

# Install with research features
pip install pynlm[research]

# Install with development tools
pip install pynlm[dev]

# Install all features
pip install pynlm[all-dev]
```

### Basic Usage

```python
import pynlm

# Create and initialize brain
brain = pynlm.create_brain()
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(brain.get_stats())
```

### Advanced Usage

```python
import pynlm
from pynlm import create_simulation_context

# Use context manager for controlled execution
with create_simulation_context() as brain:
    # Brain is automatically initialized
    for step in range(100):
        brain.step(step)
    
    # Access brain state
    print(f"Total neurons: {brain.get_total_neuron_count()}")
```

## Research Utilities

The `research_utils.py` module provides advanced tools for NLM research:

### Key Features

#### Experiment Management
- **Experiment Runner**: Comprehensive experiment management with monitoring
- **Batch Processing**: Run multiple experiments in batch mode
- **Result Analysis**: Automatic result analysis and visualization

#### Data Analysis
- **Brain Dynamics Analysis**: Deep analysis of brain network dynamics
- **Performance Profiling**: Detailed performance profiling and optimization
- **Network Topology**: Analysis of neural network structure and properties

#### Export/Import
- **JSON Export**: Export brain models to JSON format
- **Text Export**: Export brain models to human-readable text format
- **Import Support**: Import brain models from various formats

#### Visualization
- **Plot Generation**: Automatic generation of experiment plots
- **Interim Visualization**: Real-time visualization during experiments
- **Result Dashboard**: Comprehensive result dashboards

### Usage Examples

```python
from research_utils import (
    create_research_environment,
    run_research_simulation,
    analyze_brain_network,
    profile_brain_performance
)

# Create research environment
brain, world, agent = create_research_environment(
    neuron_count=5000,
    enable_structural_plasticity=True,
    enable_development=True
)

# Run research experiment
results = run_research_simulation(
    steps=1000,
    save_interval=100,
    enable_monitoring=True,
    export_data=True,
    save_plots=True
)

# Analyze brain dynamics
analysis = analyze_brain_network(brain, steps=500)
print(f"Network has {analysis['network_topology']['total_neurons']} neurons")

# Profile performance
profile = profile_brain_performance(brain, steps=100)
print(f"Average step time: {profile['performance_summary']['avg_step_time']:.4f}s")
```

## Python CLI Tools

The NLM project includes comprehensive Python CLI tools:

### NLM CLI (`nlm_cli.py`)

A command-line interface for common NLM operations:

```bash
# Create brain
nlm-cli create-brain --neuron-count 1000 --synapse-density 0.1

# Run simulation
nlm-cli run-simulation 100 --config-file config.json --checkpoint checkpoint.bin
```

### Brain CLI (`scripts/brain_cli.py`)

Simplified brain operations:

```bash
# Create brain
scripts/brain_cli.py create-brain --neuron-count 1000

# Run simulation  
scripts/brain_cli.py run-simulation 100
```

### Research CLI (`scripts/research_cli.py`)

Research-specific operations:

```bash
# Run experiment
scripts/research_cli.py run-experiment my_experiment --steps 1000

# Analyze results
scripts/research_cli.py analyze-results my_experiment --plot
```

## Testing

The NLM project includes comprehensive testing suites:

### Python Integration Tests (`tests/test_python_integration.py`)

Tests for enhanced Python bindings and functionality:

```bash
# Run Python integration tests
pytest tests/test_python_integration.py -v

# Run with coverage
pytest tests/ --cov=pynlm --cov-report=html
```

### C++ Tests

The existing C++ tests continue to validate the core NLM functionality:

```bash
# Run C++ tests
make test
```

### Test Coverage

- **Python Bindings**: Comprehensive testing of all Python classes and methods
- **Error Handling**: Testing of exception handling and error cases
- **Integration**: Testing of Python-C++ integration
- **Performance**: Performance and stress testing

## Build System

The NLM project uses `scikit-build-core` for Python package building with enhanced features:

### Building

```bash
# Install build dependencies
pip install scikit-build-core pybind11

# Build from source
python -m build

# Install in development mode
pip install -e .

# Build specific configuration
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PYTHON_CLI=ON ..
make -j4
```

### Build Options

The build system supports various options:

```bash
# Build with Python CLI support
cmake .. -DNLM_ENABLE_PYTHON_CLI=ON

# Build with Phase 6 experiments
cmake .. -DNLM_BUILD_PHASE6_EXPERIMENTS=ON

# Build with examples
cmake .. -DNLM_BUILD_EXAMPLES=ON

# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/nlm
make install
```

### Enhanced pyproject.toml

The `pyproject.toml` includes:
- Optional dependencies for development, research, and documentation
- Enhanced test configuration with coverage reporting
- Black formatting and Flake8 linting configuration
- Mypy type checking configuration
- Custom build options and commands

## Documentation

The NLM project includes comprehensive documentation:

### Generated Documentation

- **API Reference**: `python/API_REFERENCE.md` - Comprehensive Python API documentation
- **Research Documentation**: `research_utils.py` with detailed examples and usage patterns
- **Development Guide**: Enhanced documentation for developers and researchers

### Documentation Structure

```
docs/
├── ARCHITECTURE.md          # System architecture overview
├── SCIENCE.md              # Scientific background
├── ROADMAP.md              # Development roadmap
├── EXPERIMENTS.md          # Experiment descriptions
├── LIMITATIONS.md          # Scientific limitations
├── PHASE6_FINAL_AUDIT.md   # Phase 6 integration audit
```

## Advanced Usage Examples

### 1. Multi-Agent Simulation

```python
import pynlm
from research_utils import create_research_environment

# Create multiple brains
brains = []
for i in range(3):
    brain, world, agent = create_research_environment(
        neuron_count=2000,
        enable_curiosity=True,
        enable_reward_modulation=True
    )
    brains.append((brain, world, agent))

# Run competition simulation
for step in range(500):
    for brain, world, agent in brains:
        world.update(0.1)
        agent.process_sensory_input(world.get_sensory_percept())
        brain.step(step)
        world.apply_motor_command(agent.decode_motor_command(), world.get_simulation_time())
```

### 2. Development Studies

```python
import pynlm
from research_utils import create_research_environment

# Study developmental effects
brain, world, agent = create_research_environment(
    enable_development=True,
    enable_structural_plasticity=True
)

# Track developmental changes over time
development_stages = []
firing_rates = []

for step in range(2000):
    brain.step(step)
    
    development_stages.append(brain.get_developmental_stage())
    firing_rates.append(brain.getAverageFiringRate())
    
    if step % 500 == 0:
        print(f"Step {step}: Stage {brain.get_developmental_stage()}, "
              f"Firing rate: {brain.getAverageFiringRate():.3f}")

# Analyze developmental trajectory
print(f"Development completed: {max(development_stages)} reached")
```

### 3. Checkpoint and Resume

```python
import pynlm
from research_utils import create_research_environment

# Create brain and run initial simulation
brain, world, agent = create_research_environment()

# Run some steps
for step in range(100):
    brain.step(step)

# Save checkpoint
brain.save("initial_checkpoint.bin")
print("Checkpoint saved")

# Later, resume from checkpoint
brain2 = pynlm.create_brain()
brain2.load("initial_checkpoint.bin")
print("Brain resumed from checkpoint")
print(f"Remaining spikes: {brain2.getTotalSpikeCount()}")
```

### 4. Custom Configuration

```python
import pynlm
from pynlm import create_default_config

# Create custom configuration
config = create_default_config()

# Modify configuration parameters
config.set("brain.neuron_count", 3000)
config.set("brain.synapse_density", 0.12)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.curiosity.enable", True)
config.set("development.enable", True)

# Validate configuration
if hasattr(config, 'validate'):
    errors = config.validate()
    if errors:
        print(f"Configuration errors: {errors}")
    else:
        print("Configuration is valid")

# Create brain with custom configuration
brain = pynlm.create_brain(config)
brain.initialize()
print(f"Created brain with {brain.get_total_neuron_count()} neurons")
```

## License

MIT

## Authors

Research project — See docs for scientific background.

## Contribution Guidelines

### For Developers

1. **Code Quality**: Follow existing coding standards and best practices
2. **Documentation**: Update documentation when adding new features
3. **Testing**: Add comprehensive tests for new functionality
4. **Error Handling**: Implement proper error handling and validation
5. **Performance**: Optimize performance for research use cases

### For Researchers

1. **Use Research Utilities**: Leverage `research_utils.py` for advanced analysis
2. **Contribute Examples**: Add example notebooks and scripts to documentation
3. **Report Issues**: Report bugs and feature requests to the issue tracker
4. **Share Findings**: Share research findings and best practices with the community

## Support

For questions, issues, or support, please:

1. Check the [GitHub Issues](https://github.com/nlm-project/nlm/issues)
2. Read the [Documentation](https://nlm.readthedocs.io)
3. Join the [Discussions](https://github.com/nlm-project/nlm/discussions)
4. File bug reports with detailed reproduction steps

The NLM Python API is actively maintained and improved. Your feedback helps make it better for everyone!