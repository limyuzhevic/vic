# NLM Python Tests

This directory contains tests for the NLM Python bindings.

## Test Structure

The tests follow pytest conventions and include:
- Unit tests for individual Python bindings
- Integration tests for complex scenarios
- Performance benchmarks
- Documentation examples

## Running Tests

To run the tests, use:

```bash
pytest -v
```

Or with coverage:

```bash
pytest --cov=nlm --cov-report=html
```

## Test Categories

### 1. Core Functionality Tests
- Brain creation and initialization
- World simulation
- Agent behavior

### 2. Learning and Plasticity Tests
- STDP (Spike-Timing Dependent Plasticity)
- Hebbian learning
- Reward modulation

### 3. Neuromodulation Tests
- Dopamine signaling
- Curiosity mechanisms
- Novelty detection

### 4. Development Tests
- Brain maturation
- Critical period simulation
- Structural plasticity

### 5. Memory Tests
- Working memory
- Episodic memory
- Associative memory

### 6. Integration Tests
- Full brain-world-agent loop
- Complex scenarios
- Real-world simulations

## Test Files

### Basic Unit Tests
- `test_config.py` - Configuration testing
- `test_brain.py` - Basic brain functionality
- `test_world.py` - World simulation testing
- `test_agent.py` - Agent behavior testing

### Learning Tests
- `test_stdp_pybind.py` - STDP learning tests
- `test_hebbian_pybind.py` - Hebbian learning tests
- `test_reward_pybind.py` - Reward modulation tests
- `test_curiosity_pybind.py` - Curiosity tests

### Neuromodulation Tests
- `test_neuromodulation.py` - General neuromodulation tests
- `test_dopamine_pybind.py` - Dopamine-specific tests
- `test_novelty_pybind.py` - Novelty detection tests

### Development Tests
- `test_development_pybind.py` - Development system tests
- `test_maturation_pybind.py` - Brain maturation tests

### Memory Tests
- `test_memory_pybind.py` - Memory system tests
- `test_working_memory_pybind.py` - Working memory tests
- `test_episodic_memory_pybind.py` - Episodic memory tests

### Integration Tests
- `test_full_simulation.py` - Complete brain-world-agent simulation
- `test_learning_scenarios.py` - Complex learning scenarios
- `test_development_scenarios.py` - Development scenarios

### Performance Tests
- `test_performance.py` - Performance benchmarks
- `test_scalability.py` - Scalability tests

### Documentation Tests
- `test_examples.py` - Test all examples in documentation
- `test_api.py` - Test Python API consistency

## Test Design Principles

### 1. Isolation
Each test should be independent and not rely on other tests.

### 2. Reproducibility
All tests should use fixed random seeds for reproducible results.

### 3. Speed
Unit tests should run quickly (under 1 second).

### 4. Completeness
Test all major functionality and edge cases.

### 5. Clarity
Test names should clearly indicate what is being tested.

## Test Fixture Examples

### Basic Brain Test Fixture
```python
import pytest
import nlm

@pytest.fixture
def default_brain():
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    return brain

@pytest.fixture
def configured_brain():
    config = nlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    brain = nlm.createBrain(config)
    brain.initialize()
    return brain
```

### World Test Fixture
```python
@pytest.fixture
def default_world():
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    return world
```

### Agent Test Fixture
```python
@pytest.fixture
def default_agent():
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    world = nlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    return agent, brain, world
```

## Testing Strategies

### 1. Unit Testing
Test individual class methods and functions with controlled inputs.

### 2. Integration Testing
Test interactions between different components.

### 3. End-to-End Testing
Test complete scenarios from creation to completion.

### 4. Property Testing
Test that properties hold across different scenarios.

### 5. Stress Testing
Test with large numbers of neurons and complex scenarios.

## Test Data

### Synthetic Data
```python
import numpy as np

# Generate random neural data
np.random.seed(42)
neuron_types = np.random.choice(list(nlm.NeuronType), size=1000)
initial_weights = np.random.uniform(-1, 1, size=100)
sensory_inputs = np.random.uniform(0, 1, size=(8,))
```

### Real-world Data
- Save/load brain states from files
- Use pre-recorded simulation data
- Import experimental data

## Performance Testing

### Metrics to Measure
- Simulation speed (spikes/second)
- Memory usage
- Learning efficiency
- Convergence time

### Benchmarks
```python
import time

def benchmark_brain_simulation(brain, steps):
    start_time = time.time()
    for i in range(steps):
        brain.step(i)
    end_time = time.time()
    return (end_time - start_time) / steps  # time per step

def benchmark_learning(brain, world, agent, steps):
    start_time = time.time()
    for i in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    end_time = time.time()
    return (end_time - start_time) / steps  # time per step
```

## Documentation Examples

Test all examples from the documentation:
```python
def test_examples():
    import subprocess
    import sys
    
    # Test basic usage example
    result = subprocess.run([sys.executable, "examples/basic_usage.py"], 
                          capture_output=True, text=True)
    assert result.returncode == 0
    assert "All examples completed successfully!" in result.stdout
```

## Continuous Integration

### GitHub Actions
```yaml
name: NLM Python Tests

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        python-version: [3.8, 3.9, "3.10", "3.11"]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v3
      with:
        python-version: ${{ matrix.python-version }}
    
    - name: Install dependencies
      run: |
        pip install -e .[dev]
        pip install pytest-cov
    
    - name: Run tests
      run: |
        pytest -v --cov=nlm --cov-report=xml
    
    - name: Upload coverage
      uses: codecov/codecov-action@v3
      with:
        file: ./coverage.xml
        fail_ci_if_no_tests: false
```

### Local Testing
```bash
# Install test dependencies
pip install -e .[dev]

# Run all tests
pytest -v

# Run specific test category
pytest tests/test_learning.py -v

# Run tests with coverage
pytest --cov=nlm --cov-report=html

# Run tests with verbose output
pytest -v --tb=short
```

## Test Coverage Guidelines

### Required Coverage
- Core classes (Brain, Config, SimpleWorld, AgentBrain): > 90%
- Learning mechanisms (STDP, Hebbian, Reward): > 85%
- Neuromodulation: > 80%
- Development: > 75%
- Integration tests: > 70%

### Coverage Reports
- Generate HTML coverage reports
- Include coverage in CI/CD pipelines
- Track coverage over time
- Identify and fix uncovered code

## Best Practices

### 1. Test Naming
```python
def test_brain_creation_with_custom_config():
    """Test that brain can be created with custom configuration."""
    
def test_world_simulation_with_sensors():
    """Test that world simulation provides correct sensory input."""
```

### 2. Test Setup
```python
def setup_module(module):
    """Module-level setup."""
    # Set up shared resources
    
def teardown_module(module):
    """Module-level teardown."""
    # Clean up shared resources
    
def setup_function(function):
    """Function-level setup."""
    # Set up test-specific resources
    
def teardown_function(function):
    """Function-level teardown."""
    # Clean up test-specific resources
```

### 3. Assertions
```python
def test_brain_stats():
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    
    # Use assert for simple checks
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    
    # Use pytest.approx for floating point comparisons
    assert brain.getAverageFiringRate() >= 0.0
    assert abs(brain.getAverageFiringRate() - expected_rate) < 0.1
    
    # Use pytest.raises for exception testing
    with pytest.raises(ValueError):
        invalid_brain.initialize()
```

### 4. Mocking
```python
def test_agent_with_mock_brain(mock_brain):
    """Test agent behavior with mocked brain."""
    # Mock the brain object
    mock_brain.getTotalNeuronCount.return_value = 100
    
    # Test agent behavior
    agent = nlm.createAgentBrain(mock_brain)
    # ... rest of test
```

## Next Steps for Tests

1. **Implement Core Tests**: Create tests for basic functionality
2. **Add Learning Tests**: Test plasticity mechanisms
3. **Create Integration Tests**: Test complete scenarios
4. **Add Performance Tests**: Benchmark simulation performance
5. **Update Documentation**: Add test examples to API docs
6. **Set up CI/CD**: Configure automated testing

This test suite will ensure the NLM Python bindings are reliable, maintainable, and perform well across different scenarios.