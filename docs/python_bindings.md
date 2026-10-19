# NLM Python Bindings Testing and Documentation

This directory contains testing and documentation for the NLM Python bindings.

## Files in this directory:

### test_python_bindings.py
Comprehensive Python bindings test suite.

### test_cpython_integration.py
C++/Python integration tests.

### test_advanced_bindings.py
Advanced Python bindings features test.

### docs/python_bindings.md
Documentation for Python bindings.

### examples/advanced/
Advanced example scripts.

## Usage

### Running Python Tests

```bash
# Run all Python tests
cd python
pytest tests/ -v

# Run specific test categories
cd python
pytest -m "performance" -v
cd python
pytest -m "integration" -v
cd python
pytest -m "memory" -v

# Run with coverage
cd python
pytest --cov=pynlm --cov-report=html

# Run with benchmarks
cd python
pytest -v -m benchmark --benchmark-save=benchmarks.json
```

### Testing Python Bindings

The NLM Python bindings test suite includes comprehensive testing of:

- **Core functionality**: Basic Python bindings for NLM components
- **Integration tests**: Cross-language integration between C++ and Python
- **Performance tests**: Performance benchmarking and optimization
- **Memory tests**: Memory usage and leak detection
- **Advanced features**: Advanced Python features and capabilities
- **Error handling**: Error handling and exception management

## Python Bindings Test Suite

### test_python_bindings.py

Comprehensive test suite for Python bindings:

```python
# test_python_bindings.py
import pytest
import numpy as np
from pynlm import Brain, Neuron, WorkingMemory, EpisodicMemory
from pynlm.neuromodulation import Reward, Curiosity
from pynlm.sensory import Vision, Audio

class TestBrainBindings:
    """Test Brain Python bindings."""

    def test_brain_creation(self):
        """Test basic brain creation."""
        brain = Brain()
        assert brain is not None
        assert brain.get_name() == "Brain"

    def test_neuron_creation(self):
        """Test neuron creation."""
        neuron = Neuron()
        assert neuron is not None
        assert neuron.get_type() == "Neuron"

    def test_add_neuron(self):
        """Test adding neurons to brain."""
        brain = Brain()
        neuron = Neuron()
        brain.add_neuron(neuron)
        assert len(brain.neurons) == 1

    def test_update(self):
        """Test brain update."""
        brain = Brain()
        neuron = Neuron()
        brain.add_neuron(neuron)

        spiked = brain.update(0.01)
        assert isinstance(spiked, list)

class TestMemoryBindings:
    """Test memory system Python bindings."""

    def test_working_memory(self):
        """Test working memory bindings."""
        memory = WorkingMemory({'size': 100})
        assert memory is not None
        assert not memory.is_active()

        pattern = np.random.rand(100)
        memory.activate_pattern(pattern, 0.5)
        assert memory.is_active()

    def test_episodic_memory(self):
        """Test episodic memory bindings."""
        memory = EpisodicMemory()
        assert memory is not None
        assert len(memory.get_recent_episodes(10)) == 0

class TestNeuromodulationBindings:
    """Test neuromodulation Python bindings."""

    def test_reward(self):
        """Test reward prediction error bindings."""
        reward = Reward()
        assert reward is not None
        assert reward.get_rpe() == 0.0
        assert reward.get_value_estimate() == 0.0

    def test_curiosity(self):
        """Test curiosity bindings."""
        curiosity = Curiosity()
        assert curiosity is not None
        assert curiosity.get_curiosity() == 0.0
        assert curiosity.get_intrinsic_reward() == 0.0

class TestSensoryBindings:
    """Test sensory system Python bindings."""

    def test_vision(self):
        """Test vision bindings."""
        vision = Vision()
        assert vision is not None
        assert vision.get_field_of_view() > 0
        assert vision.get_resolution()[0] > 0

    def test_audio(self):
        """Test audio bindings."""
        audio = Audio()
        assert audio is not None
        assert audio.get_frequency_range()[0] > 0

# Performance tests
@pytest.mark.performance
def test_performance():
    """Performance test for Python bindings."""
    import time

    start_time = time.time()
    brain = Brain()
    for i in range(100):
        neuron = Neuron()
        brain.add_neuron(neuron)

    end_time = time.time()
    assert (end_time - start_time) < 1.0  # Should complete within 1 second

# Memory tests
@pytest.mark.memory
def test_memory_usage():
    """Memory usage test for Python bindings."""
    import tracemalloc

    tracemalloc.start()

    # Create objects to test memory usage
    brain = Brain()
    for i in range(100):
        neuron = Neuron()
        brain.add_neuron(neuron)

    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()

    assert peak < 100 * 1024 * 1024  # Should use less than 100 MB
```

### test_cpython_integration.py

C++/Python integration tests:

```python
# test_cpython_integration.py
import pytest
import numpy as np
from pynlm import Brain, Neuron
from pynlm.core import c_integration

class TestCPythonIntegration:
    """Test C++/Python integration."""

    def test_cpython_functionality(self):
        """Test C++ functionality from Python."""
        brain = Brain()
        assert brain is not None

    def test_numpy_interoperability(self):
        """Test NumPy interoperability."""
        arr = np.array([1.0, 2.0, 3.0])
        assert arr.sum() == 6.0

    def test_memory_layout(self):
        """Test memory layout compatibility."""
        from pynlm.core import get_memory_info
        info = get_memory_info()
        assert 'allocated' in info
        assert 'limit' in info

    def test_performance_consistency(self):
        """Test performance consistency between C++ and Python."""
        import time

        # C++ implementation
        start_time = time.time()
        brain = Brain()
        for i in range(100):
            neuron = Neuron()
            brain.add_neuron(neuron)
        cpp_time = time.time() - start_time

        # Python implementation (if any)
        # Note: This depends on available Python implementations

        # Compare performance
        assert cpp_time < 1.0  # Should be fast
```

### test_advanced_bindings.py

Advanced Python bindings features test:

```python
# test_advanced_bindings.py
import pytest
import numpy as np
from pynlm import Brain, Neuron
from pynlm.core import advanced_features

class TestAdvancedBindings:
    """Test advanced Python bindings features."""

    def test_context_managers(self):
        """Test context manager bindings."""
        from pynlm.core import ManagedResource

        with ManagedResource() as resource:
            assert resource is not None

    def test_factory_functions(self):
        """Test factory function bindings."""
        from pynlm import create_brain, create_neuron, create_memory_system

        brain = create_brain({'neuron_count': 10})
        neuron = create_neuron({'threshold': -50})
        memory = create_memory_system('working')

        assert brain is not None
        assert neuron is not None
        assert memory is not None

    def test_type_safety(self):
        """Test type safety features."""
        from pynlm.core import TypeSafeValue

        # Test type-safe value binding
        value = TypeSafeValue('int', 42)
        assert value.get() == 42
        assert value.get_type() == 'int'

    def test_parameter_validation(self):
        """Test parameter validation."""
        from pynlm.core import validate_parameters

        params = {'threshold': -50, 'rest_potential': -65}
        assert validate_parameters(params)

        invalid_params = {'invalid_param': 123}
        assert not validate_parameters(invalid_params)
```

## Python Bindings Documentation

### docs/python_bindings.md

Comprehensive documentation for Python bindings:

```markdown
# NLM Python Bindings

## Overview

The NLM Python bindings provide access to the NLM (Neural Learning Machine) C++ library from Python, enabling:

- **Brain simulation**: Create and simulate neural networks
- **Memory systems**: Access working and episodic memory
- **Neuromodulation**: Use reward and curiosity signals
- **Sensory processing**: Process visual and auditory inputs
- **Motor control**: Control agent actions
- **Development**: Simulate neural development

## Installation

```bash
# Install from PyPI
pip install pynlm

# Install from source
pip install -e .
```

## Quick Start

```python
from pynlm import Brain, Neuron, create_brain

# Create a simple brain
brain = create_brain({'neuron_count': 10})

# Add neurons
for i in range(10):
    neuron = Neuron()
    brain.add_neuron(neuron)

# Run simulation
spiked = brain.update(0.01)
print(f"Spiked neurons: {len(spiked)}")
```

## Core Components

### Brain

```python
brain = Brain(config)
neuron = Neuron(config)

# Brain methods
brain.add_neuron(neuron)
brain.update(dt)
brain.get_state()
brain.set_state(state)
```

### Memory Systems

```python
working_memory = WorkingMemory(config)
episodic_memory = EpisodicMemory(config)

# Working memory methods
working_memory.activate_pattern(pattern, strength)
working_memory.update(dt)
working_memory.get_activations()
working_memory.is_active()
```

### Neuromodulation

```python
reward = Reward(config)
curiosity = Curiosity(config)

# Neuromodulation methods
reward.update(reward_signal, value_estimate)
reward.get_rpe()
reward.get_value_estimate()
```

### Sensory Processing

```python
vision = Vision(config)
audio = Audio(config)

# Sensory methods
vision.process_input(visual_input)
vision.get_input()
```

## Advanced Features

### Context Managers

```python
from pynlm.core import ManagedResource

with ManagedResource() as resource:
    # Resource is automatically managed
    resource.do_something()
```

### Factory Functions

```python
brain = create_brain(config)
neuron = create_neuron(config)
memory = create_memory_system(type, config)
```

### Type Safety

```python
from pynlm.core import TypeSafeValue

value = TypeSafeValue('int', 42)
assert value.get() == 42
```

## Error Handling

### Custom Exceptions

```python
from pynlm import NLMError, ConfigurationError, MemoryError

try:
    brain = Brain(config)
except ConfigurationError as e:
    print(f"Configuration error: {e}")
except MemoryError as e:
    print(f"Memory error: {e}")
except NLMError as e:
    print(f"NLM error: {e}")
```

## Performance Optimization

### NumPy Interoperability

```python
import numpy as np

# Use NumPy arrays for efficiency
pattern = np.random.rand(100)
memory.activate_pattern(pattern)

# Use NumPy for calculations
result = np.sum(neuron_weights)
```

### Memory Management

```python
import gc
from pynlm import ManagedResource

# Explicit memory management
resource = ManagedResource()
del resource
gc.collect()
```

## Configuration

### Brain Configuration

```python
brain_config = {
    'neuron_count': 100,
    'threshold': -50,
    'rest_potential': -65,
    'reset_potential': -65,
    'learning_rate': 0.01,
    'plasticity_strength': 1.0,
    'membrane_time_constant': 10.0,
    'refractory_period': 0.5
}
```

### Memory Configuration

```python
memory_config = {
    'size': 100,
    'decay_rate': 0.1,
    'persistence_threshold': 0.01,
    'replay_rate': 0.1
}
```

### Neuromodulation Configuration

```python
neuromodulation_config = {
    'learning_rate': 0.01,
    'discount_factor': 0.9,
    'exploration_rate': 0.1,
    'novelty_threshold': 0.5
}
```

## Examples

### Simple Brain Simulation

```python
from pynlm import Brain, Neuron

brain = Brain()

for i in range(50):
    neuron = Neuron()
    brain.add_neuron(neuron)

for step in range(100):
    spiked = brain.update(0.01)
    print(f"Step {step}: {len(spiked)} neurons spiked")
```

### Agent-Environment Interaction

```python
from pynlm import Brain, WorkingMemory, Environment, Reward, Curiosity

class Agent:
    def __init__(self):
        self.brain = Brain()
        self.memory = WorkingMemory()
        self.reward = Reward()
        self.curiosity = Curiosity()

    def perceive(self, state):
        self.memory.activate_pattern(state)

    def act(self):
        spiked = self.brain.update(0.01)
        return len(spiked)

    def learn(self, reward_signal, next_state):
        self.reward.update(reward_signal, 0.0)
        self.curiosity.update(next_state)
```

### Memory System Analysis

```python
from pynlm import WorkingMemory, EpisodicMemory
import numpy as np

# Working memory
wm = WorkingMemory({'size': 1000})
pattern = np.random.rand(1000)
wm.activate_pattern(pattern, 0.5)

# Episodic memory
em = EpisodicMemory()
for i in range(10):
    em.encode_experience({'step': i, 'reward': np.random.rand()})

em.store_episode()
```

## Testing and Validation

### Running Tests

```bash
# Install test dependencies
pip install -e .[dev]

# Run all tests
pytest -v

# Run specific test categories
pytest -m performance -v
pytest -m integration -v
pytest -m memory -v

# Run with coverage
coverage run -m pytest
coverage report
```

### Performance Testing

```python
import time
from pynlm import Brain

# Performance test
start_time = time.time()
brain = Brain()
for i in range(1000):
    neuron = Neuron()
    brain.add_neuron(neuron)
end_time = time.time()

print(f"Created {1000} neurons in {end_time - start_time:.3f} seconds")
```

## API Reference

### Classes

- `Brain`: Neural network simulation
- `Neuron`: Individual neuron model
- `WorkingMemory`: Persistent activity memory
- `EpisodicMemory`: Experience-based memory
- `Reward`: Reward prediction error
- `Curiosity`: Curiosity-driven exploration
- `Vision`: Visual sensory processing
- `Audio`: Auditory sensory processing

### Functions

- `create_brain()`: Factory function for brain creation
- `create_neuron()`: Factory function for neuron creation

### Constants

- `__version__`: Package version
- `__author__`: Package author

## Troubleshooting

### Common Issues

#### Memory issues
- Use `del` to explicitly delete objects
- Use `gc.collect()` for garbage collection
- Monitor memory usage with profiling tools

#### Performance issues
- Use NumPy arrays for large data
- Profile performance with timing tools
- Optimize code for better performance

#### Type errors
- Check parameter types
- Use type annotations
- Validate inputs before use

### Getting Help

- **Documentation**: https://nlm.readthedocs.io/python-bindings
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Slack/Discord**: Contact project maintainers for support
```

## License

MIT License. See project root for details.
```

# docs/python_bindings.md
```