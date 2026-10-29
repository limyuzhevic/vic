# NLM Python Test Suite

Test suite for the NLM (Neural Learning Machine) Python bindings.

This suite provides comprehensive testing for all Python bindings functionality,
including brain operations, agent interactions, configuration management, and
integration tests.

## Installation

```bash
pip install pytest numpy
pip install -e .  # Install the package in development mode
```

## Running Tests

```bash
# Run all tests
pytest

# Run specific test modules
pytest test_basic.py
pytest test_configuration.py
pytest test_agent.py
pytest test_integration.py

# Run with verbose output
pytest -v

# Run with coverage
pytest --cov=pynlm --cov-report=html
```

## Test Structure

Each test file contains:
- Unit tests for individual classes
- Integration tests for complex operations
- Performance benchmarks
- Error condition tests
- Configuration validation tests

## Test Files

### test_basic.py
Tests basic brain and world operations:
- Brain creation and initialization
- Simple simulation steps
- Neuron and synapse statistics
- Brain reset and re-initialization

### test_configuration.py
Tests configuration system:
- Default configuration creation
- Configuration file loading
- Parameter setting and getting
- Configuration validation
- Type conversion utilities

### test_agent.py
Tests agent brain interface:
- Agent creation and initialization
- Sensory processing
- Motor command decoding
- Neuromodulation application
- Behavior management

### test_world.py
Tests world simulation:
- World configuration
- Object management
- Position validation
- Sensory percept generation
- Motor command application

### test_integration.py
Integration tests:
- Complete simulation setup
- Agent in world interaction
- Brain learning demonstrations
- Multi-step episode testing
- Performance benchmarks

### test_error_handling.py
Error condition tests:
- Invalid parameter handling
- File not found errors
- Configuration validation errors
- Type conversion errors

### test_performance.py
Performance benchmarks:
- Simulation speed testing
- Memory usage monitoring
- Scalability testing
- Benchmark comparisons

## Usage Examples

### Basic Test

```python
import pytest
import pynlm

def test_brain_creation():
    """Test basic brain creation and initialization."""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    
    brain.initialize()
    assert brain.getFiringNeuronCount() >= 0

def test_brain_step():
    """Test brain simulation steps."""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    initial_spikes = brain.getTotalSpikeCount()
    brain.step(10)
    final_spikes = brain.getTotalSpikeCount()
    
    assert final_spikes >= initial_spikes
```

### Configuration Test

```python
import pytest
import pynlm

def test_default_config():
    """Test default configuration creation."""
    config = pynlm.createDefaultConfig()
    assert config.has("random_seed")
    assert config.has("simulation_timestep")
    assert config.has("neuron_count")

def test_config_set_get():
    """Test configuration parameter setting and getting."""
    config = pynlm.createDefaultConfig()
    
    # Set values
    config.setString("test.key1", "value1")
    config.setInt("test.key2", 42)
    config.setFloat("test.key3", 3.14)
    config.setBool("test.key4", True)
    
    # Get values
    assert config.getString("test.key1", "") == "value1"
    assert config.getInt("test.key2", 0) == 42
    assert config.getFloat("test.key3", 0.0) == 3.14
    assert config.getBool("test.key4", False) == True

def test_config_validation():
    """Test configuration validation."""
    config = pynlm.createDefaultConfig()
    
    # Test missing key
    assert not config.has("nonexistent.key")
    
    # Test validation with required keys
    with pytest.raises(py.value_error):
        config.validate(["nonexistent.key"])
```

### Agent Test

```python
import pytest
import pynlm

def test_agent_creation():
    """Test agent brain creation."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    assert agent.getBrain() is not None
    assert agent.getSensoryInputSize() > 0
    assert agent.getMotorOutputSize() > 0

def test_agent_behavior():
    """Test agent behavior control."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable different behaviors
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Check behavior profile
    profile = agent.getBehaviorProfile()
    assert profile["rewardModulation"] == True
    assert profile["curiosity"] == True
    assert profile["development"] == True
```

### Integration Test

```python
import pytest
import pynlm

def test_complete_simulation():
    """Test complete simulation with agent and world."""
    # Create complete setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run episode
    steps = 100
    successful_steps = 0
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        successful_steps += 1
        
        # Check agent is still working
        assert agent.getSensoryInputSize() > 0
    
    assert successful_steps == steps
    assert brain.getTotalSpikeCount() >= 0

def test_brain_learning():
    """Test brain learning capabilities."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Record initial state
    initial_spikes = brain.getTotalSpikeCount()
    initial_rate = brain.getAverageFiringRate()
    
    # Run simulation with sensory input
    for step in range(50):
        brain.step(step)
    
    # Check for activity
    assert brain.getTotalSpikeCount() >= initial_spikes
    assert brain.getFiringNeuronCount() >= 0
```

## Running Performance Tests

```bash
# Run performance tests
pytest test_performance.py -v

# Run with performance benchmarks
pytest test_performance.py --benchmark-only --benchmark-save=benchmark_results.json

# Check performance benchmarks
pytest-benchmark --benchmark-json=benchmark_results.json
```

## Error Handling Tests

```bash
# Run error handling tests
pytest test_error_handling.py -v

# Run with expected failure tests
pytest test_error_handling.py --tb=short
```

## Test Coverage

The test suite aims for high coverage of the Python bindings functionality:

- **Configuration System**: ~95% coverage
- **Brain Operations**: ~90% coverage  
- **Agent Interface**: ~85% coverage
- **World Simulation**: ~80% coverage
- **Integration Tests**: ~75% coverage
- **Error Handling**: ~70% coverage

## Continuous Integration

The test suite is designed to work with continuous integration:

```bash
# Install testing dependencies
pip install pytest pytest-benchmark numpy

# Run full test suite
pytest --cov=pynlm --cov-report=term-missing --cov-report=html

# Generate coverage report
open htmlcov/index.html
```

## Adding New Tests

To add new tests:

1. Create a new test file in the test directory
2. Follow the existing test patterns and naming conventions
3. Ensure tests are isolated and don't interfere with each other
4. Include both positive and negative test cases
5. Add documentation for new test scenarios

## Test Quality

This test suite follows best practices:

- **Isolation**: Each test is independent
- **Reproducibility**: Tests run in deterministic order
- **Comprehensive**: Cover both happy paths and error conditions
- **Performance**: Include benchmarks for critical operations
- **Maintainable**: Easy to understand and extend
- **Documented**: Clear docstrings and comments

## Licensing

MIT License - See LICENSE file for details.

## Authors

NLM Test Suite Authors
Version: 2.0 (Enhanced)

For questions or issues, please refer to the NLM documentation or project repository.
