# NLM - Enhanced Python Bindings v2.0.0

**An Experimental Artificial Developmental Brain with Advanced Python API**

## Overview

NLM (熙然, meaning "serene flow") is an enhanced version of the original NLM neural simulation framework, now featuring a comprehensive Python API with advanced features for both beginners and power users.

## Key Improvements

### 🚀 Enhanced API Design

#### Pythonic Interface
- **Type-safe** with comprehensive type hints
- **Intuitive** method names following Python conventions
- **Flexible** configuration system with dot notation support
- **Comprehensive** documentation and examples

#### Advanced Features
- Performance monitoring and profiling
- Batch processing capabilities
- Memory management utilities
- Multi-environment simulation support
- Advanced neural configuration options

### 📊 Performance Monitoring

#### Built-in Profiling
```python
# Create profiler
profiler = brain.get_performance()

# Get real-time metrics
stats = profiler.get_current_metrics()
print(f"Average step time: {stats.step_time_avg * 1000:.2f} ms")
print(f"Firing rate: {brain.get_average_firing_rate():.2f} Hz")

# Performance summary
summary = profiler.get_summary()
print(f"Steps per second: {summary['steps_per_second']:.1f}")
```

#### Memory Management
```python
# Monitor memory usage
from pynlm import PerformanceProfiler

profiler = PerformanceProfiler()
# Use profiler during simulation
# Automatic memory cleanup
```

### 🔧 Enhanced Configuration System

#### Advanced Configuration
```python
import pynlm

# Create advanced brain with optimization
config = pynlm.create_default_config()

# Configure with dot notation
config.set("brain.neuron_count", 5000)
config.set("brain.region_count", 5)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("development.synaptogenesis_rate", 0.0002)

# Create brain
brain = pynlm.create_brain(config)
```

#### Configuration Files
```python
# Save configuration
config.save("my_brain_config.json")

# Load configuration
config = pynlm.NLMConfig.load("my_brain_config.json")
```

### 🧠 Advanced Brain Features

#### Enhanced Plasticity Control
```python
# Enable specific plasticity mechanisms
brain.enable_plasticity(hebbian=True, stdp=True, reward_modulated=False)

# Enable developmental changes
brain.enable_developmental_changes(True)
brain.enable_curiosity(True)
```

#### State Management
```python
# Save and load brain states
brain.save("checkpoint1.bin")
# ... run simulation ...
brain.load("checkpoint1.bin")  # Resume from checkpoint
```

### 🌍 Multi-Environment Simulation

#### World Management
```python
import pynlm

# Create world
world = pynlm.SimpleWorld(width=30, height=30, 
                         vision_width=12, vision_height=12)

# Run complete agent simulation
brain = pynlm.create_brain()
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Enable advanced subsystems
agent.enable_subsystems(reward_modulation=True, 
                       structural_plasticity=True,
                       development=True,
                       curiosity=True)

# Run simulation
results = pynlm.run_simulation(brain, world, agent, 
                              num_steps=2000, verbose=True)
```

### ⚡ Performance Optimization

#### Batch Processing
```python
# Run multiple simulations efficiently
from pynlm import run_multiple_simulations

def brain_factory():
    return pynlm.create_advanced_brain(neuron_count=1000, regions=3)

results = run_multiple_simulations(
    num_simulations=10,
    brain_factory=brain_factory,
    steps_per_simulation=500,
    parallel=False  # Set to True for parallel execution
)

# Analyze results
for i, result in enumerate(results):
    print(f"Simulation {i}: {result['final_spikes']} spikes")
```

#### Advanced Brain Creation
```python
# For rapid prototyping
brain = pynlm.create_reduced_brain(neuron_count=50, regions=1)

# For high-performance simulation
brain = pynlm.create_advanced_brain(
    neuron_count=5000, 
    regions=5, 
    connectivity=0.15
)
```

### 📚 Comprehensive Documentation

#### Getting Started
```python
import pynlm

# Quick start with advanced features
config = pynlm.create_default_config()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

brain = pynlm.create_brain(config)
for step in range(100):
    brain.step(step)

print(f"Simulation complete! Firing neurons: {brain.get_firing_neurons()}")
```

#### Advanced Examples
```python
import pynlm

# Complete agent simulation with all features
def run_advanced_simulation():
    # Setup
    brain = pynlm.create_advanced_brain(neuron_count=3000, regions=4)
    world = pynlm.SimpleWorld(width=25, height=25)
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enable_subsystems(reward_modulation=True,
                           structural_plasticity=True,
                           development=True,
                           curiosity=True)
    
    # Run simulation
    results = pynlm.run_simulation(brain, world, agent, 
                                  num_steps=1000, verbose=True)
    
    return results

# Execute
results = run_advanced_simulation()
print(f"Simulation completed in {results['total_time']:.2f} seconds")
```

## 🚀 Command Line Interface

### Available Commands
```bash
# Get information about the installation
pynlm-info

# Run a simulation
pynlm-run --steps 1000 --config config.json

# Run benchmark tests
pynlm-benchmark --neurons 1000 --iterations 10

# Train neural networks
pynlm-train --data dataset.csv --epochs 100

# Explore and analyze
pynlm-explore --mode "interactive" --brain brain.bin
```

### Command Line Examples
```bash
# Run simple brain simulation
pynlm-run --steps 500 --verbose

# Run advanced agent simulation
pynlm-run --world-size 30 --agent-mode --steps 1000

# Benchmark performance
pynlm-benchmark --neurons 2000 --regions 5 --timesteps 100

# Train with custom configuration
pynlm-train --config custom_config.json --output trained_brain.bin
```

## 📖 API Reference

### Core Classes

#### `NLMConfig`
- **Purpose**: Advanced configuration management
- **Methods**: `get()`, `set()`, `has()`, `save()`, `load()`, `to_dict()`
- **Features**: Dot notation support, type-safe accessors, JSON serialization

#### `NLMBrain`
- **Purpose**: Main neural brain simulation engine
- **Methods**: `step()`, `reset()`, `get_stats()`, `save()`, `load()`
- **Features**: Performance monitoring, advanced plasticity control, checkpointing

#### `SimpleWorld`
- **Purpose**: Environment simulation for agent interaction
- **Methods**: `update()`, `get_percept()`, `apply_action()`, `reset()`
- **Features**: Multi-dimensional sensing, action application

#### `AgentBrain`
- **Purpose**: Agent brain interface for sensory-motor processing
- **Methods**: `process_sensory_input()`, `decode_motor_command()`, `apply_reward_modulation()`
- **Features**: Neuromodulation support, development tracking

### Utility Functions

#### `create_brain(config=None)`
- Creates a new brain with optional configuration
- Returns: `NLMBrain` instance

#### `create_default_config()`
- Creates default configuration with optimized values
- Returns: `NLMConfig` instance

#### `run_simulation(brain, world=None, agent=None, num_steps=1000, verbose=False)`
- Runs complete simulation with optional agent in world
- Returns: Dictionary with simulation results and statistics

#### `create_advanced_brain(neuron_count=5000, regions=5, connectivity=0.15)`
- Creates advanced brain with optimized configuration
- Returns: `NLMBrain` instance

#### `create_reduced_brain(neuron_count=100, regions=1)`
- Creates reduced brain for rapid testing and prototyping
- Returns: `NLMBrain` instance

## 📊 Performance Monitoring

### Real-time Metrics
```python
# Create performance profiler
from pynlm import PerformanceProfiler

profiler = PerformanceProfiler()

# Monitor during simulation
for step in range(1000):
    profiler.start_step()
    brain.step(step)
    profiler.end_step()

# Get performance summary
summary = profiler.get_summary()
print(f"Total simulation time: {summary['total_simulation_time']:.2f}s")
print(f"Average step time: {summary['average_step_time_ms']:.2f}ms")
print(f"Steps per second: {summary['steps_per_second']:.1f}")
```

### Memory Management
```python
from pynlm import cleanup_memory

# Cleanup memory after batch processing
cleanup_memory()
```

## 🔧 Development Tools

### Building from Source
```bash
# Install dependencies
pip install scikit-build-core pybind11 numpy typing-extensions

# Build and install
pip install .

# Build in development mode
pip install -e .

# Build with extended features
pip install .[extended]

# Build with development tools
pip install .[dev]
```

### Running Tests
```bash
# Run all tests
pytest

# Run specific test modules
pytest tests/test_brain.py
pytest tests/test_config.py
pytest benchmarks/

# Run with coverage
pytest --cov=pynlm --cov-report=html

# Run benchmarks
pytest benchmarks/ --benchmark-enable
```

### Building Custom Builds
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DENABLE_PERFORMANCE_PROFILING=ON \
         -DENABLE_ADVANCED_FEATURES=ON

# Build
make -j$(nproc)

# Install
make install
```

## 🎯 Use Cases

### 1. Research and Experimentation
```python
# Run multiple experiments with different configurations
experiments = []
for neuron_count in [100, 500, 1000, 2000]:
    config = pynlm.create_default_config()
    config.set("brain.neuron_count", neuron_count)
    
    brain = pynlm.create_brain(config)
    for step in range(200):
        brain.step(step)
    
    experiments.append({
        'neurons': neuron_count,
        'spikes': brain.get_total_spikes(),
        'firing_rate': brain.get_average_firing_rate()
    })
```

### 2. Agent Training
```python
# Train agent with learning
brain = pynlm.create_advanced_brain()
world = pynlm.SimpleWorld()
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)
agent.enable_subsystems(reward_modulation=True, curiosity=True)

for episode in range(100):
    # Reset world
    world.reset()
    
    # Run episode
    for step in range(100):
        world.update(0.1)
        percept = world.get_percept()
        agent.process_sensory_input(percept)
        brain.step(step)
        
        action = agent.decode_motor_command()
        world.apply_action(action, world.get_simulation_time())
        
        # Get reward and learn
        reward = calculate_reward(world)
        agent.apply_reward_modulation(reward, 0.0)
    
    # Update development
    agent.update_development(0.1)
```

### 3. Performance Analysis
```python
# Compare different brain configurations
from pynlm import run_multiple_simulations

def fast_brain_factory():
    return pynlm.create_reduced_brain(100, 1)

def slow_brain_factory():
    return pynlm.create_advanced_brain(5000, 5, 0.15)

# Run comparison
fast_results = run_multiple_simulations(
    num_simulations=5,
    brain_factory=fast_brain_factory,
    steps_per_simulation=200
)

slow_results = run_multiple_simulations(
    num_simulations=5,
    brain_factory=slow_brain_factory,
    steps_per_simulation=200
)

# Analyze and compare performance
```

## 📈 Version History

### v2.0.0 (Current)
- Complete rewrite with enhanced Python API
- Performance monitoring and profiling
- Advanced configuration system
- Batch processing capabilities
- Comprehensive documentation
- Development tools and CLI interface

### v1.0.0 (Original)
- Basic Python bindings
- Limited API design
- Basic configuration support
- Minimal documentation

## 🔌 Compatibility

### Python Versions
- **Python 3.8+** (fully supported)
- **Python 3.9+** (recommended)
- **Python 3.10+** (optimal performance)
- **Python 3.11+** (latest features)

### Dependencies
- **Required**: numpy>=1.20, typing-extensions>=4.0
- **Optional Dev**: pytest>=7.0, pytest-benchmark>=4.0
- **Optional Extended**: scipy>=1.7.0, matplotlib>=3.5.0, pandas>=1.3.0

### Operating Systems
- **Linux**: Fully supported
- **macOS**: Supported with caveats
- **Windows**: Supported (requires CMake)

## 🛠️ Technical Implementation

### Architecture
```
Python API (pynlm)
├── Configuration System
│   ├── NLMConfig (type-safe configuration)
│   └── Configuration file I/O
├── Core Simulation
│   ├── NLMBrain (neural simulation engine)
│   ├── SimpleWorld (environment simulation)
│   └── AgentBrain (agent interface)
├── Performance Monitoring
│   ├── PerformanceProfiler (real-time metrics)
│   └── Memory management
├── Batch Processing
│   ├── run_multiple_simulations()
│   └── parallel execution support
└── Development Tools
    ├── CLI interface
    ├── testing frameworks
    └── benchmarking tools
```

### Build Process
```
Python Package (pyproject.toml)
├── C++ Extension (_pynlm)
│   ├── Brain simulation (core neural dynamics)
│   ├── World simulation (environment)
│   ├── Agent interface (sensory-motor processing)
│   └── Configuration system (parameter management)
├── Python Bindings (pybind11)
│   ├── Type-safe wrappers
│   ├── Performance monitoring
│   └── Advanced API design
└── Development Tools
    ├── Documentation (Sphinx)
    ├── Testing (pytest)
    └── Benchmarking (pytest-benchmark)
```

## 🎓 Learning Resources

### Tutorial
1. **Installation**: `pip install pynlm[extended]`
2. **Basic Usage**: `pynlm-run --steps 100`
3. **Advanced Features**: Read documentation and examples
4. **Performance Optimization**: Use profiling and batch processing

### Documentation
- **Main Documentation**: https://nlm.readthedocs.io
- **API Reference**: Auto-generated from type hints
- **Examples**: `examples/` directory
- **Tutorials**: `docs/` directory
- **API Reference**: `docs/api/` directory

### Community
- **GitHub**: https://github.com/nlm-project/nlm
- **Discussions**: GitHub Discussions
- **Issues**: GitHub Issues
- **Pull Requests**: Welcome!

## 🏆 Quality Assurance

### Testing
- **Unit Tests**: Comprehensive test coverage
- **Integration Tests**: System-level testing
- **Performance Tests**: Benchmark testing
- **Regression Tests**: Continuous integration

### Code Quality
- **Type Hints**: Full type annotations
- **Documentation**: Comprehensive docstrings
- **Linting**: Black, isort, mypy
- **Testing**: pytest with coverage

### Performance
- **Profiling**: Built-in performance monitoring
- **Optimization**: Efficient C++ implementation
- **Memory Management**: Automatic cleanup
- **Scalability**: Supports large-scale simulations

## 🎯 Future Enhancements

### Planned Features
1. **Distributed Computing**: Multi-machine simulation support
2. **GPU Acceleration**: CUDA integration for faster computation
3. **Advanced Learning**: Reinforcement learning integration
4. **Visualization**: Real-time brain activity visualization
5. **Export/Import**: Standardized brain format exchange
6. **Web Interface**: Web-based simulation tools

### Research Integration
- **Neuroscientific Models**: Integrate with brain research
- **Machine Learning**: Combine with ML algorithms
- **Robotics**: Apply to robotics control systems
- **Education**: Interactive learning tools

## 📝 Acknowledgements

### Contributors
- **Original NLM Team**: Core neural simulation framework
- **Pybind11 Team**: Python bindings library
- **Scientific Community**: Open-source neuroscience research
- **Users**: Feedback and feature requests

### Funding
- **Research Grants**: Support for neural simulation research
- **Open Source**: Community-driven development
- **Academic**: University research collaborations

## 🚀 Getting Started

### Quick Start (5 minutes)
```bash
# Install with all features
pip install pynlm[extended]

# Run quick test
pynlm-run --steps 10 --verbose

# View documentation
pynlm-info
```

### Next Steps
1. **Read Documentation**: https://nlm.readthedocs.io
2. **Try Examples**: `examples/` directory
3. **Join Community**: GitHub Discussions
4. **Contribute**: Submit issues and pull requests
5. **Research**: Apply to neuroscience and AI research

## ✨ Conclusion

NLM v2.0.0 represents a significant advancement in neural simulation frameworks, offering:

- **Advanced Python API** with comprehensive features
- **Performance Monitoring** with real-time metrics
- **Batch Processing** for efficient experimentation
- **Development Tools** for research and production
- **Comprehensive Documentation** for users and developers

The enhanced Python bindings make it easier than ever to explore, experiment with, and advance neural simulation research. Whether you're a student, researcher, or developer, NLM provides the tools you need to create sophisticated neural systems.

**Start your neural simulation journey today with NLM v2.0.0!** 🎯