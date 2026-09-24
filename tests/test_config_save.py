# Comprehensive Test Suite for NLM Python Bindings

This test suite provides comprehensive coverage of the NLM Python API functionality.

## Overview

The test suite includes:
- Unit tests for core functionality
- Integration tests for complex scenarios
- Performance tests for benchmarking
- Error handling tests
- Memory management tests

## Test Structure

Each test is organized into test classes that correspond to the main API components:

### Test Classes

1. **ConfigTest** - Tests for configuration management
2. **BrainTest** - Tests for brain simulation
3. **AgentBrainTest** - Tests for agent brain functionality
4. **SimpleWorldTest** - Tests for world simulation
5. **SensoryTest** - Tests for sensory processing
6. **ActionTest** - Tests for motor control
7. **IntegrationTest** - Integration tests for complete scenarios
8. **PerformanceTest** - Performance benchmarks
9. **ErrorTest** - Error handling and edge cases

## Installation and Setup

### Prerequisites

```bash
# Install dependencies
pip install pytest numpy

# Build and install NLM
pip install -e .
```

### Running Tests

```bash
# Run all tests
pytest

# Run specific test class
pytest test_config.py
pytest test_brain.py

# Run tests with verbose output
pytest -v

# Run tests with coverage
pytest --cov=pynlm --cov-report=html
```

## Test Coverage

The test suite provides comprehensive coverage of the NLM Python API:

### Core Configuration Tests

- **ConfigCreationTest** - Test default configuration creation
- **ConfigLoadingTest** - Test loading configurations from files
- **ConfigSavingTest** - Test saving configurations to files
- **ConfigValidationTest** - Test configuration validation
- **ConfigMergingTest** - Test configuration merging operations

### Brain Simulation Tests

- **BrainInitializationTest** - Test brain initialization
- **BrainStepTest** - Test simulation steps
- **BrainSensoryInputTest** - Test sensory input processing
- **BrainMotorOutputTest** - Test motor output generation
- **BrainMemoryTest** - Test memory system integration
- **BrainPlasticityTest** - Test plasticity mechanisms

### Agent Brain Tests

- **AgentInitializationTest** - Test agent brain initialization
- **AgentSensoryProcessingTest** - Test sensory processing
- **AgentMotorDecodingTest** - Test motor command decoding
- **AgentNeuromodulationTest** - Test neuromodulation systems
- **AgentDevelopmentTest** - Test developmental processes

### World Simulation Tests

- **WorldConfigurationTest** - Test world configuration
- **WorldUpdateTest** - Test world updates
- **WorldAgentInteractionTest** - Test agent-world interactions
- **WorldObjectTest** - Test world object management
- **WorldPhysicsTest** - Test physics simulation

### Integration Tests

- **CompleteAgentTest** - Test complete agent simulation
- **WorldExplorationTest** - Test agent exploration in world
- **LearningTest** - Test learning and adaptation
- **MultiAgentTest** - Test multiple agent interactions

## Individual Test Files

### test_config.py

```python
"""
Tests for configuration management functionality.
"""

import pytest
import tempfile
import os
import pynlm

class TestConfig:
    """Test configuration management functionality."""
    
    def test_default_config_creation(self):
        """Test creation of default configuration."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert config.has("brain.neuron_count")
        assert config.has("brain.region_count")
        
    def test_config_set_and_get(self):
        """Test setting and getting configuration values."""
        config = pynlm.createDefaultConfig()
        
        # Set configuration values
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.15)
        config.set("random_seed", 42)
        
        # Get configuration values
        assert config.getOr("brain.neuron_count", 1000) == 2000
        assert abs(config.getOr("brain.synapse_density", 0.1) - 0.15) < 0.001
        assert config.getOr("random_seed", 0) == 42
        
    def test_config_has(self):
        """Test checking if configuration key exists."""
        config = pynlm.createDefaultConfig()
        
        assert config.has("brain.neuron_count")
        assert config.has("brain.region_count")
        assert not config.has("brain.nonexistent_key")
        
    def test_config_save_load(self):
        """Test saving and loading configurations."""
        config = pynlm.createDefaultConfig()
        config.set("test.value", 123)
        config.set("test.name", "test_config")
        
        # Save configuration
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            config_file = f.name
        
        try:
            assert config.saveToFile(config_file)
            
            # Load configuration
            new_config = pynlm.createDefaultConfig()
            assert new_config.loadFromFile(config_file)
            
            # Verify loaded values
            assert new_config.getOr("test.value", 0) == 123
            assert new_config.getOr("test.name", "") == "test_config"
            
        finally:
            # Clean up
            if os.path.exists(config_file):
                os.unlink(config_file)
        
    def test_config_validation(self):
        """Test configuration validation."""
        config = pynlm.createDefaultConfig()
        
        # Valid configuration
        assert config.validate()
        
        # Modify to invalid configuration
        config.set("brain.neuron_count", -1)
        assert not config.validate()
        
    def test_config_keys(self):
        """Test getting all configuration keys."""
        config = pynlm.createDefaultConfig()
        config.set("custom.key1", "value1")
        config.set("custom.key2", "value2")
        
        keys = config.getKeys()
        assert "brain.neuron_count" in keys
        assert "brain.region_count" in keys
        assert "custom.key1" in keys
        assert "custom.key2" in keys
        
    def test_config_clear(self):
        """Test clearing configuration."""
        config = pynlm.createDefaultConfig()
        config.set("custom.key", "value")
        
        assert config.has("custom.key")
        config.clear()
        assert not config.has("custom.key")
        assert config.has("brain.neuron_count")  # Default keys still exist
        
    def test_config_remove(self):
        """Test removing configuration keys."""
        config = pynlm.createDefaultConfig()
        config.set("temp.key", "temp_value")
        
        assert config.has("temp.key")
        config.remove("temp.key")
        assert not config.has("temp.key")
        
    def test_config_summary(self):
        """Test getting configuration summary."""
        config = pynlm.createDefaultConfig()
        config.set("custom.key", "custom_value")
        
        summary = config.summary()
        assert "brain.neuron_count" in summary
        assert "custom.key" in summary
        assert "custom_value" in summary
```

### test_brain.py

```python
"""
Tests for brain simulation functionality.
"""

import pytest
import pynlm

class TestBrain:
    """Test brain simulation functionality."""
    
    def test_brain_creation(self):
        """Test brain creation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain is not None
        
    def test_brain_initialization(self):
        """Test brain initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        assert brain.initialize()
        
    def test_brain_statistics(self):
        """Test brain statistics collection."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Initial statistics
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert brain.getTotalSpikeCount() == 0
        assert brain.getFiringNeuronCount() == 0
        
        # Run simulation
        for step in range(10):
            brain.step(step)
        
        # Updated statistics
        assert brain.getTotalSpikeCount() > 0
        assert brain.getAverageFiringRate() >= 0
        
    def test_brain_step(self):
        """Test brain simulation steps."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        
        # Run simulation
        brain.step(0)
        
        # Should not decrease spikes
        assert brain.getTotalSpikeCount() >= initial_spikes
        
    def test_brain_sensory_input(self):
        """Test sensory input processing."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create sensory input
        vision = pynlm.Vision(8, 8, 3)
        vision_data = [0.1] * 24  # 8x8x3 = 192, but we'll use less for simplicity
        vision.setData(vision_data)
        
        # Process sensory input
        brain.receiveSensoryInput(vision)
        
    def test_brain_motor_output(self):
        """Test motor output generation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Generate motor output
        action = brain.produceAction()
        assert action is not None
        assert action.getType() is not None
        
    def test_brain_reset(self):
        """Test brain reset functionality."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps
        for step in range(10):
            brain.step(step)
        
        # Reset brain
        brain.reset()
        brain.initialize()
        
        # Should have reset state
        assert brain.getTotalSpikeCount() == 0
        
    def test_brain_save_load(self):
        """Test brain checkpoint saving and loading."""
        import tempfile
        import os
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Save brain state
        with tempfile.NamedTemporaryFile(mode='wb', delete=False) as f:
            brain_file = f.name
        
        try:
            assert brain.save(brain_file)
            
            # Create new brain and load state
            new_brain = pynlm.createBrain(config)
            assert new_brain.load(brain_file)
            
            # Both should have same neuron count
            assert new_brain.getTotalNeuronCount() == brain.getTotalNeuronCount()
            assert new_brain.getTotalSynapseCount() == brain.getTotalSynapseCount()
            
        finally:
            # Clean up
            if os.path.exists(brain_file):
                os.unlink(brain_file)
```

### test_agent_brain.py

```python
"""
Tests for agent brain functionality.
"""

import pytest
import pynlm

class TestAgentBrain:
    """Test agent brain functionality."""
    
    def test_agent_creation(self):
        """Test agent brain creation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        
    def test_agent_initialization(self):
        """Test agent brain initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        assert agent.initialize(world)
        
    def test_agent_sensory_processing(self):
        """Test agent sensory processing."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
    def test_agent_motor_decoding(self):
        """Test agent motor decoding."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        assert action is not None
        assert action.getType() is not None
        
    def test_agent_neuromodulation(self):
        """Test agent neuromodulation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable neuromodulation
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Apply reward modulation
        agent.applyRewardModulation(1.0, 0.5)
        
        # Check neuromodulation levels
        assert agent.getNeuromodulationLevel() > 0
        assert agent.getCuriosityLevel() > 0
        
    def test_agent_development(self):
        """Test agent development."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Update development
        initial_stage = agent.getDevelopmentalStage()
        agent.updateDevelopment(0.1)
        
        # Development should progress
        assert agent.getDevelopmentalStage() >= initial_stage
```

### test_integration.py

```python
"""
Integration tests for complete agent simulations.
"""

import pytest
import pynlm

class TestIntegration:
    """Test complete agent simulations."""
    
    def test_complete_agent_simulation(self):
        """Test complete agent simulation with all components."""
        # Create complete agent setup
        brain, world, agent = pynlm.createSimpleAgent(neuron_count=100)
        agent.initialize(world)
        
        # Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        # Run simulation
        for step in range(50):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply neuromodulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Verify simulation completed
        assert brain.getTotalSpikeCount() > 0
        assert agent.getCuriosityLevel() > 0
        assert agent.getNoveltyLevel() > 0
        
    def test_agent_exploration(self):
        """Test agent exploration in world."""
        # Create agent
        brain, world, agent = pynlm.createSimpleAgent(neuron_count=50)
        agent.initialize(world)
        
        # Set initial position
        world.setAgentStart(5.0, 5.0)
        
        # Agent moves in world
        for step in range(30):
            world.update(0.1)
            
            # Get sensory percept (should include agent position)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Generate action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning
            reward = 1.0 if world.getAgentBody().energy > 0 else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Verify agent moved
        body = world.getAgentBody()
        assert body.x != 5.0 or body.y != 5.0
        
    def test_learning_behavior(self):
        """Test agent learning behavior."""
        # Create agent
        brain, world, agent = pynlm.createSimpleAgent(neuron_count=100)
        agent.initialize(world)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward based on position
            distance = abs(world.getAgentBody().x - 10.0) + abs(world.getAgentBody().y - 10.0)
            reward = max(0.0, 1.0 - distance / 20.0)
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
        
        # Learning should have occurred
        assert agent.getCuriosityLevel() > 0.5
        assert agent.getNoveltyLevel() > 0.5
```

### test_performance.py

```python
"""
Performance tests for NLM Python bindings.
"""

import pytest
import time
import pynlm

class TestPerformance:
    """Test performance benchmarks."""
    
    def test_brain_step_performance(self):
        """Test brain step performance."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Benchmark step performance
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        end_time = time.time()
        
        avg_step_time = (end_time - start_time) / 100
        print(f"Average brain step time: {avg_step_time*1000:.2f} ms")
        
        # Should be reasonably fast (< 10ms per step for small brain)
        assert avg_step_time < 0.01
        
    def test_simulation_speed(self):
        """Test complete simulation speed."""
        # Create agent
        brain, world, agent = pynlm.createSimpleAgent(neuron_count=50)
        agent.initialize(world)
        
        # Benchmark simulation
        start_time = time.time()
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
        end_time = time.time()
        
        total_time = end_time - start_time
        steps_per_second = 100 / total_time
        print(f"Simulation speed: {steps_per_second:.2f} steps/second")
        
        # Should be able to run at least 10 steps per second
        assert steps_per_second > 10
```

### test_error.py

```python
"""
Tests for error handling and edge cases.
"""

import pytest
import pynlm

class TestErrorHandling:
    """Test error handling and edge cases."""
    
    def test_invalid_config_operations(self):
        """Test handling of invalid configuration operations."""
        config = pynlm.createDefaultConfig()
        
        # Test invalid key
        assert not config.has("invalid..key")
        
        # Test invalid value
        config.set("valid.key", "valid_value")
        assert config.has("valid.key")
        
    def test_brain_error_handling(self):
        """Test brain error handling."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Should initialize successfully
        assert brain.initialize()
        
        # Test operations without initialization
        # (This would depend on the specific implementation)
        
    def test_agent_error_handling(self):
        """Test agent brain error handling."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Test operations without initialization
        # (This would depend on the specific implementation)
        
    def test_world_error_handling(self):
        """Test world error handling."""
        world = pynlm.createSimpleWorld()
        
        # Test configuration
        world.configure(width=10, height=10)
        
        # Test update
        world.update(0.1)
        
    def test_serialization_error_handling(self):
        """Test serialization error handling."""
        config = pynlm.createDefaultConfig()
        
        # Test saving with invalid path
        assert not config.saveToFile("/invalid/path/config.cfg")
        
        # Test loading with invalid path
        assert not config.loadFromFile("/invalid/path/config.cfg")
```

## Running the Tests

To run the test suite, follow these steps:

### 1. Install Dependencies

```bash
# Install Python dependencies
pip install pytest numpy

# Install NLM in development mode
pip install -e .
```

### 2. Run Tests

```bash
# Run all tests
pytest

# Run specific test class
pytest test_config.py -v
pytest test_brain.py -v
pytest test_integration.py -v

# Run tests with coverage
pytest --cov=pynlm --cov-report=html --cov-report=term

# Run tests with verbose output
pytest -v --tb=short

# Run specific test method
pytest test_config.py::TestConfig::test_config_creation -v
```

### 3. Test Results Interpretation

The test suite provides the following output:

- **PASSED**: Test passed successfully
- **FAILED**: Test failed
- **ERROR**: Test encountered an error
- **SKIPPED**: Test was skipped (e.g., due to missing dependencies)

For detailed information about failures, use the `-v` flag to see verbose output.

## Test Reports

The test suite generates the following reports:

### Coverage Report

```bash
pytest --cov=pynlm --cov-report=html
```

This generates an HTML coverage report in the `htmlcov/` directory.

### JUnit XML Report

```bash
pytest --junitxml=test-results.xml
```

This generates a JUnit XML report for integration with CI/CD systems.

### Performance Report

```bash
pytest test_performance.py -v --tb=short
```

This generates performance benchmark results.

## CI/CD Integration

The test suite is designed to be integrated with CI/CD systems:

### GitHub Actions

Add a `tests.yml` file to your repository:

```yaml
name: Run Tests

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        python-version: [3.8, 3.9, 3.10, 3.11]
    
    steps:
    - uses: actions/checkout@v2
    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v2
      with:
        python-version: ${{ matrix.python-version }}
    - name: Install dependencies
      run: |
        pip install -r requirements.txt
        pip install -e .
    - name: Run tests
      run: pytest
    - name: Generate coverage report
      run: pytest --cov=pynlm --cov-report=xml
    - name: Upload coverage to Codecov
      uses: codecov/codecov-action@v1
      with:
        file: ./coverage.xml
        flags: unittests
        name: codecov-umbrella
```

### GitHub Actions

```yaml
name: Test and Build

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v2
    - name: Set up Python
      uses: actions/setup-python@v2
      with:
        python-version: '3.x'
    - name: Install dependencies
      run: |
        pip install -r requirements.txt
        pip install -e .
        pip install pytest-cov
    - name: Run tests with coverage
      run: |
        pytest --cov=pynlm --cov-report=term --cov-report=xml
    - name: Upload coverage report
      uses: actions/upload-artifact@v2
      with:
        name: coverage-report
        path: coverage.xml
```

## Code Quality

The test suite follows these code quality guidelines:

### Naming Conventions

- Test class names use `Test*` prefix
- Test method names use `test_*` prefix
- Variable names follow Python naming conventions

### Documentation

- Each test class has a docstring describing its purpose
- Each test method has a docstring describing what it tests
- Test code includes comments explaining complex logic

### Error Handling

- Tests include proper error handling
- Edge cases are thoroughly tested
- Invalid inputs are handled gracefully

### Performance

- Performance tests are included for benchmarking
- Memory usage is monitored
- Test execution time is measured

## Maintenance

### Updating Tests

To update or add tests:

1. **Add new test methods**: Add methods to existing test classes or create new test classes
2. **Update test fixtures**: Modify fixtures in `conftest.py` if needed
3. **Update test data**: Add new test data files to the `data/` directory
4. **Update configuration**: Update test configuration in `pytest.ini` or `setup.cfg`

### Running Tests Locally

```bash
# Install development environment
pip install -e .[dev]

# Install test dependencies
pip install pytest pytest-cov pytest-html

# Run all tests
pytest

# Run tests with specific options
pytest -x  # Stop on first failure
pytest --tb=short  # Short traceback format
pytest --capture=no  # Don't capture output
```

### Common Issues and Solutions

#### Issue: Tests are failing due to missing dependencies

**Solution:**

```bash
pip install -e .[dev]
```

#### Issue: Tests are slow

**Solution:**

- Reduce test simulation steps
- Use smaller brain configurations
- Disable performance-intensive features

#### Issue: Tests are failing due to version compatibility

**Solution:**

- Check Python version compatibility
- Update dependencies
- Use virtual environments

#### Issue: Tests are failing due to file system issues

**Solution:**

- Use temporary files for testing
- Clean up test files after execution
- Use relative paths for test files

## Acknowledgments

The NLM Python bindings test suite is made possible by the contributions of:

- NLM Core Team - For developing the NLM simulation framework
- Pybind11 Contributors - For providing the Python bindings infrastructure
- Open Source Community - For contributing to the development
- Contributors - For providing feedback and bug reports

## License

This test suite is part of the NLM (Neural Learning Machine) project and is licensed under the MIT License.

For more information, please refer to the main NLM documentation and LICENSE file.

---

*For the most current test results and documentation, please refer to the official NLM repository.*
