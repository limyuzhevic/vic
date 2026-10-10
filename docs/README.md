# Project configuration for NLM

project_name = "NLM"
version = "0.1.0"
description = "Neural Learning Machine - Advanced Brain Simulation Framework"

# Python package metadata
[project]
name = "pynlm"
version = "0.1.0"
description = "Python bindings for NLM - Neural Learning Machine"
readme = "README.md"
requires-python = ">=3.8"
license = {text = "MIT"}
authors = [
    {name = "NLM Team"}
]
classifiers = [
    "Development Status :: 4 - Beta",
    "Intended Audience :: Science/Research",
    "Topic :: Scientific/Engineering :: Artificial Intelligence",
    "Topic :: Scientific/Engineering :: Neuroscience",
    "License :: OSI Approved :: MIT License",
    "Programming Language :: Python :: 3",
    "Programming Language :: C++"
]

[project.optional-dependencies]
dev = [
    "pytest",
    "pytest-cov",
    "memory_profiler",
    "pytest-benchmark",
    "ruff",
    "black",
    "isort"
]

# Build system requirements
build-system = [
    "setuptools>=42.0.0",
    "wheel>=0.37.0",
    "pybind11[global]>=2.10.0",
    "cmake>=3.16.0"
]

# Package data and inclusion
[options]
package-dir = {"" = "src"}
packages = find:
include-package-data = true
zip-safe = false

# Build extensions
[options.packages.find]
where = ["."]

# Wheel configuration
[bdist_wheel]
universal = false

# Test configuration
[tool.pytest.ini_options]
minversion = "6.0"
addopts = "-v --tb=short"
python_files = "test_*.py"
python_classes = "Test*"
python_functions = "test_*"

# Linting configuration
[tool.ruff]
line-length = 88
target-version = "py38"
select = [
    "E",  # pycodestyle errors
    "W",  # pycodestyle warnings
    "F",  # pyflakes
    "I",  # isort
    "B",  # flake8-bugbear
    "C4", # flake8-comprehensions
    "UP", # pyupgrade
    "N",  # flake8-naming
    "PL", # pylint
    "SIM", # flake8-simplify
    "ARG", # flake8-unused-arguments
    "PTH", # flake8-use-pathlib
    "T10", # flake8-debugger
    "Q",   # flake8-quotes
    "SLF", # flake8-self
]
ignore = [
    "E501",  # line too long, handled by line-length
    "PLR0913",  # too many arguments in function definition
    "PLR0915",  # too many statements
]

[tool.ruff.isort]
known-first-party = ["pynlm"]

[tool.black]
line-length = 88
target-version = ['py38']
include = '\.pyi?$'
extend-exclude = '''
/(
    \.eggs
  | \.git
  | \.hg
  | \.mypy_cache
  | \.tox
  | \.venv
  | _build
  | buck-out
  | build
  | dist
)/
'''

# Documentation configuration
[build-system.readme]
README.md = "
# NLM Python Bindings

This package provides Python bindings for the Neural Learning Machine (NLM) framework.

## Installation

```bash
pip install pynlm
```

## Usage Examples

### Basic Brain Simulation
```python
import pynlm

# Create a basic neural system
system = pynlm.BasicSystem()
neuron = system.create_neuron()
synapse = system.create_synapse(neuron)

# Run simulation
result = system.run(1000)
print(f"Simulation completed: {result}")
```

### Neuromodulation
```python
import pynlm

# Enable neuromodulation
system = pynlm.NeuromodulationSystem()
reward = system.create_reward_module()
prediction_error = system.create_prediction_error_module()

# Run with neuromodulation
result = system.run(1000)
print(f"Neuromodulation simulation: {result}")
```

### Memory Systems
```python
import pynlm

# Working memory
system = pynlm.MemorySystem()
working_mem = system.create_working_memory()

# Episodic memory
episodic_mem = system.create_episodic_memory()

# Run memory simulation
result = system.run(1000)
print(f"Memory simulation: {result}")
```

### Development Systems
```python
import pynlm

# Development system
system = pynlm.DevelopmentSystem()
synaptogenesis = system.create_synaptogenesis()
pruning = system.create_pruning()

# Run development simulation
result = system.run(1000)
print(f"Development simulation: {result}")
```

### Advanced Usage
```python
import pynlm
import numpy as np

# Create complex neural network
config = pynlm.Configuration({
    "brain_size": 1000,
    "plasticity_enabled": True,
    "neuromodulation_enabled": True,
    "memory_enabled": True
})

system = pynlm.AdvancedSystem(config)

# Initialize and run
result = system.initialize()
output = system.run(1000)

print(f"Advanced simulation results:")
print(f"Initialization: {result}")
print(f"Final output: {output}")
```

## API Reference

### Core Components

#### BasicSystem
Main neural system interface with basic functionality.

#### AdvancedSystem
Full-featured system with all modules enabled.

#### Configuration
System configuration manager for parameter tuning.

### Brain Modules

#### Neuron
Individual neural units with activation dynamics.

#### Synapse
Connections between neurons with plasticity rules.

#### NeuralPopulation
Groups of neurons with shared properties.

#### NeuralRegion
Anatomical organization of neural structures.

#### Brain
Complete brain architecture with all components.

### Neuromodulation Modules

#### Neuromodulator
Chemical messengers regulating neural activity.

#### Reward
Reward prediction and processing module.

#### PredictionError
Prediction error signaling for learning.

#### Novelty
Novelty detection and processing.

#### Curiosity
Intrinsic motivation system.

#### Acetylcholine
Cholinergic neuromodulation.

#### Norepinephrine
Noradrenergic neuromodulation.

#### Serotonin
Serotonergic neuromodulation.

### Memory Modules

#### WorkingMemory
Short-term working memory system.

#### EpisodicMemory
Event-based memory storage.

#### SemanticMemory
Knowledge-based memory system.

#### ProceduralMemory
Skill-based memory system.

#### AssociativeMemory
Connection-based memory associations.

#### NeuralWorkingMemory
Neural implementation of working memory.

#### NeuralEpisodicMemory
Neural implementation of episodic memory.

### Cognition Modules

#### ConceptFormation
Abstract concept creation and management.

#### NeuralPlanner
Goal-directed planning and decision making.

### Sensory Modules

#### SensoryInput
General sensory input processing.

#### Vision
Visual processing and perception.

#### Audio
Auditory processing and perception.

#### InternalSignals
Internal state monitoring.

### Prediction Modules

#### PredictionSystem
Predictive modeling and inference.

#### NeuralPrediction
Neural implementation of prediction.

### Motor Modules

#### MotorSystem
Action generation and execution.

### Environment Modules

#### Environment
Simulation environment management.

#### Observation
Perception of environmental state.

#### Action
Generation and execution of actions.

### Development Modules

#### DevelopmentSystem
System development and maturation.

#### Synaptogenesis
Formation of neural connections.

#### Pruning
Optimization of neural connectivity.

#### Maturation
Neural system maturation and refinement.

### Experiment Modules

#### Experiment
Experimental protocol management.

#### ExperimentRunner
Execution of experimental protocols.

#### Metrics
Performance measurement and analysis.

### Visualization Modules

#### VisualizationInterface
Visual output and rendering.

### Agent Modules

#### AgentBrain
Cognitive processing for agents.

#### SensoryPercept
Sensory processing for agents.

#### AgentBody
Physical body model for agents.

### World Modules

#### SimpleWorld
Basic world simulation.

## Advanced Topics

### Custom Modules
```python
# Create custom neural modules
class CustomModule(pynlm.BaseModule):
    def __init__(self, system):
        super().__init__(system)
        self.name = "CustomModule"
    
    def initialize(self):
        # Custom initialization
        return True
    
    def update(self, dt):
        # Custom update logic
        return self.state
    
    def run(self, steps):
        # Custom run method
        for i in range(steps):
            self.update(0.001)
        return self.state

# Add to system
system.add_module(CustomModule(system))
```

### Performance Optimization
```python
import pynlm

# Create optimized system
system = pynlm.AdvancedSystem(config)

# Memory optimization
system.enable_memory_optimization()

# Parallel processing
system.enable_parallel_processing()

# SIMD acceleration
system.enable_simd_acceleration()

# Run optimized simulation
result = system.run(1000)
```

### Memory Management
```python
import pynlm

# Create system with memory management
system = pynlm.MemoryManagedSystem()

# Monitor memory usage
usage = system.get_memory_usage()
print(f"Memory usage: {usage} MB")

# Enable memory leak detection
system.enable_memory_leak_detection()

# Run with memory monitoring
result = system.run(1000)
print(f"Simulation completed with memory monitoring")
```

### Multi-threading
```python
import pynlm
import threading

# Create system
system = pynlm.ThreadSafeSystem()

# Run multiple simulations in parallel
def run_simulation(id):
    system = pynlm.AdvancedSystem()
    result = system.run(1000)
    return (id, result)

threads = []
results = []

for i in range(4):
    thread = threading.Thread(target=lambda i=i: results.append(run_simulation(i)))
    threads.append(thread)
    thread.start()

for thread in threads:
    thread.join()

print(f"Parallel simulation results: {results}")
```

## Testing

Run tests with pytest:
```bash
pytest
```

Run specific test suites:
```bash
pytest tests/test_core.py
pytest tests/test_neuromodulation.py
pytest tests/test_memory.py
```

Run benchmarks:
```bash
pytest --benchmark
```

## Contributing

### Development Setup
```bash
# Clone repository
git clone https://github.com/your-repo/nlm.git
cd nlm

# Create and activate virtual environment
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# Install development dependencies
pip install -e .[dev]

# Build C++ extensions
mkdir build
cd build
cmake ..
make
```

### Code Style

This project follows PEP 8 style guide with black formatting. Run:
```bash
black src/
tests/
```

And:
```bash
ruff check src/ tests/
```

### Testing

Add new tests in the tests directory. Use pytest with memory profiling:
```python
import pytest
from pynlm import AdvancedSystem

def test_basic_functionality():
    system = AdvancedSystem()
    result = system.run(100)
    assert result is not None
    assert isinstance(result, dict)

@pytest.mark.benchmark
def test_performance():
    system = AdvancedSystem()
    import time
    start = time.time()
    result = system.run(1000)
    end = time.time()
    assert (end - start) < 10.0  # Should complete within 10 seconds
```

### Adding New Modules

To add new neural modules:

1. Create a new module directory in src/
2. Implement the module in C++ with corresponding Python bindings
3. Add the module to the main CMakeLists.txt if needed
4. Update Python bindings CMakeLists.txt
5. Add tests in the appropriate test directory
6. Update documentation

## Troubleshooting

### Common Issues

**Q: Why is the build failing?**
A: Ensure you have all dependencies installed:
- CMake >= 3.16
- Python >= 3.8
- pybind11
- A C++ compiler with C++20 support

**Q: Why are Python bindings not found?**
A: Ensure the package is installed in development mode:
```bash
cd build
pip install -e .
```

**Q: Why is memory usage high?**
A: Enable memory optimization in the system configuration:
```python
system = pynlm.AdvancedSystem(config)
system.enable_memory_optimization()
```

**Q: Why are tests failing?**
A: Check for memory leaks and ensure all modules are properly initialized:
```python
import pynlm
system = pynlm.AdvancedSystem()
system.initialize()
result = system.run(100)
```

## Conclusion

The NLM Python bindings provide a comprehensive interface to the Neural Learning Machine framework, enabling advanced brain simulation capabilities in Python. With support for modular architecture, performance optimization, and extensive testing, pynlm is suitable for both research and production environments.

For more information and examples, visit the documentation and examples directories.
```
