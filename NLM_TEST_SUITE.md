# Test Suite for NLM Python Bindings and AgentBrain Improvements

## Overview

This comprehensive test suite validates the NLM (Neural Learning Machine) Python bindings and AgentBrain improvements. It includes unit tests, integration tests, performance tests, Python API tests, and validation tests.

## Test Structure

```
tests/
├── conftest.py                    # pytest configuration and fixtures
├── unit_tests/                     # Unit tests for core components
│   ├── test_agentbrain_unit.py     # AgentBrain unit tests
│   ├── test_brain_unit.py          # Brain unit tests
│   └── test_neuromodulation_unit.py # Neuromodulation unit tests
├── integration_tests/              # Integration tests
│   ├── test_brain_world_agent.py   # Complete brain + world + agent integration
│   ├── test_simulation_loops.py   # Complete simulation loops
│   └── test_error_conditions.py   # Error conditions and edge cases
├── performance_tests/              # Performance validation
│   ├── test_large_neuron_counts.py # Tests with large numbers of neurons
│   └── test_simulation_efficiency.py # Simulation efficiency tests
├── python_api_tests/               # Python bindings API tests
│   ├── test_bindings_basics.py     # All public Python bindings
│   ├── test_error_handling.py      # Error handling in Python bindings
│   └── test_memory_management.py   # Memory management tests
├── validation_tests/               # Validation of improvements
│   ├── test_backward_compatibility.py # Backward compatibility
│   ├── test_convenience_methods.py   # New convenience methods
│   └── test_improvements.py        # Test that improvements work correctly
├── test_suite.py                   # Master test suite runner
└── requirements.txt                # Test dependencies
```

## Installation and Setup

### Prerequisites

1. Install NLM with Python bindings:
   ```bash
   # Clone the NLM repository
   git clone <repo-url>
   cd nlm

   # Build and install Python bindings
   pip install -e .
   ```

2. Install test dependencies:
   ```bash
   pip install pytest pytest-benchmark numpy
   ```

### Alternative: Build with CMake (for development)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
# Python bindings should be built and installed
```

## Running Tests

### Basic Test Execution

```bash
# Run all tests
pytest tests/

# Run specific test categories
pytest tests/unit_tests/           # Unit tests
pytest tests/integration_tests/    # Integration tests
pytest tests/python_api_tests/     # Python API tests
pytest tests/validation_tests/     # Validation tests
pytest tests/performance_tests/    # Performance tests

# Run tests with verbose output
pytest tests/ -v

# Run tests with coverage report
pytest tests/ --cov=pynlm --cov-report=html
```

### Performance Tests (Benchmarks)

```bash
# Run performance tests
pytest tests/performance_tests/ -v --benchmark-disable

# Run performance tests with benchmarks
pytest tests/performance_tests/ -v --benchmark
```

### Custom Test Configuration

```bash
# Run with specific Python version
python -m pytest tests/ --pyargs pynlm

# Run tests with custom timeout
pytest tests/ --timeout=300

# Run tests in parallel
pytest tests/ -n auto
```

## Test Categories

### 1. Unit Tests (test_agentbrain_unit.py)

Tests basic AgentBrain initialization and cleanup:

```python
def test_initialization_and_cleanup():
    """Test basic brain initialization and cleanup"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Brain should initialize successfully
    brain.initialize()
    
    # Should have neurons
    assert brain.getTotalNeuronCount() > 0
    
    # Cleanup
    brain.reset()
```

Tests sensory processing:

```python
def test_sensory_processing():
    """Test sensory input processing"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create sensory input
    vision = pynlm.Vision(11, 11)
    for i in range(121):
        vision.setData([0.5] * 121)
    
    # Process sensory input
    brain.processSensoryInput(vision)
    
    # Verify processing
    assert brain.getSensoryInputCount() > 0
```

Tests motor decoding:

```python
def test_motor_decoding():
    """Test motor command decoding"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Generate some neural activity
    for i in range(100):
        brain.step(i)
    
    # Decode motor command
    action = brain.decodeMotorCommand()
    
    # Verify action
    assert action is not None
    assert isinstance(action, pynlm.Action)
```

Tests reward modulation:

```python
def test_reward_modulation():
    """Test reward-based learning"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable reward modulation
    brain.enableRewardModulation(True)
    
    # Apply positive reward
    brain.applyRewardModulation(1.0, 0.8)
    
    # Apply negative reward
    brain.applyRewardModulation(-0.5, 0.3)
    
    # Verify reward effects
    assert brain.getDopamineLevel() > 0.0
```

Tests development updates:

```python
def test_development_updates():
    """Test brain development"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable development
    brain.enableDevelopment(True)
    
    # Track development stage
    initial_stage = brain.getDevelopmentalStage()
    
    # Simulate development over time
    for i in range(100):
        brain.step(i)
    
    final_stage = brain.getDevelopmentalStage()
    
    # Development should progress
    assert final_stage > initial_stage
```

Tests novelty detection:

```python
def test_novelty_detection():
    """Test novelty detection"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable novelty detection
    brain.enableNoveltyDetection(True)
    
    # Create novel stimulus
    novel_vision = pynlm.Vision(11, 11)
    for i in range(121):
        novel_vision.setData([float(i % 10) / 10.0] * 121)
    
    # Process novel stimulus
    brain.processSensoryInput(novel_vision)
    
    # Check novelty signal
    novelty = brain.getNoveltySignal()
    assert novelty > 0.0
```

Tests curiosity exploration:

```python
def test_curiosity_exploration():
    """Test curiosity-driven exploration"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable curiosity
    brain.enableCuriosity(True)
    
    # Track curiosity-driven actions
    curiosity_actions = 0
    
    # Run simulation
    for i in range(200):
        brain.step(i)
        action = brain.decodeMotorCommand()
        
        # Check if action is curiosity-driven
        if brain.isCuriosityDrivenAction(action):
            curiosity_actions += 1
    
    # Should have some curiosity-driven actions
    assert curiosity_actions > 0
```

### 2. Integration Tests (test_brain_world_agent.py)

Tests brain + world + agent integration:

```python
def test_brain_world_agent_integration():
    """Test complete integration of brain, world, and agent"""
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run complete simulation loop
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Verify integration
    assert brain.getTotalSpikeCount() > 0
    assert world.getSimulationTime() > 0
```

Tests complete simulation loops:

```python
def test_complete_simulation_loops():
    """Test complete simulation from start to finish"""
    # Setup simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(30, 30, 11, 11)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    
    # Run complete simulation
    metrics = {}
    for step in range(500):
        # World update
        world.update(0.1)
        
        # Sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Action selection
        action = agent.decodeMotorCommand()
        
        # World interaction
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect metrics
        metrics['step'] = step
        metrics['spikes'] = brain.getTotalSpikeCount()
        metrics['firing'] = brain.getFiringNeuronCount()
        metrics['rate'] = brain.getAverageFiringRate()
    
    # Validate simulation completed successfully
    assert metrics['step'] == 499
    assert metrics['spikes'] > 0
    assert metrics['rate'] > 0
```

Tests error conditions and edge cases:

```python
def test_error_conditions():
    """Test error conditions and edge cases"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test 1: Invalid sensory input
    try:
        invalid_input = "not a sensory input"
        brain.processSensoryInput(invalid_input)
        assert False, "Should have thrown error"
    except Exception:
        pass  # Expected
    
    # Test 2: Out of range motor command
    try:
        invalid_action = pynlm.Action(999)  # Invalid action type
        brain.applyMotorCommand(invalid_action)
        assert False, "Should have thrown error"
    except Exception:
        pass  # Expected
    
    # Test 3: Step before initialization
    brain2 = pynlm.createBrain(config)
    try:
        brain2.step(0)  # Should work, brain initializes on first step
        assert True
    except Exception:
        assert False, "Brain should handle uninitialized step"
```

### 3. Performance Tests (test_large_neuron_counts.py)

Tests with large numbers of neurons:

```python
def test_large_neuron_counts():
    """Test brain performance with large neuron counts"""
    # Test with different neuron counts
    neuron_counts = [100, 1000, 5000]
    
    for neuron_count in neuron_counts:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Measure initialization time
        import time
        start_time = time.time()
        
        for i in range(50):
            brain.step(i)
        
        elapsed = time.time() - start_time
        
        # Performance should be reasonable
        max_time_per_step = 0.1  # 100ms per step
        assert elapsed < neuron_count * max_time_per_step, \
            f"Too slow for {neuron_count} neurons: {elapsed:.3f}s"
```

Tests simulation efficiency:

```python
def test_simulation_efficiency():
    """Test simulation efficiency and memory usage"""
    import time
    import tracemalloc
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test memory efficiency
    tracemalloc.start()
    
    for i in range(200):
        brain.step(i)
    
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    
    # Memory usage should be reasonable (less than 100MB for 200 steps)
    assert peak < 100 * 1024 * 1024, f"Memory usage too high: {peak / 1024 / 1024:.2f}MB"
    
    # Test time efficiency
    tracemalloc.start()
    start_time = time.time()
    
    for i in range(500):
        brain.step(i)
    
    elapsed = time.time() - start_time
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    
    # Should complete 500 steps efficiently
    time_per_step = elapsed / 500
    assert time_per_step < 0.05, f"Too slow: {time_per_step:.4f}s per step"
```

### 4. Python API Tests (test_bindings_basics.py)

Tests all public Python bindings:

```python
def test_bindings_basics():
    """Test all basic Python bindings"""
    # Test Config class
    config = pynlm.createDefaultConfig()
    config.set_simulation_steps(1000)
    assert config.get_simulation_steps() == 1000
    
    # Test Brain class
    brain = pynlm.createBrain(config)
    brain.initialize()
    assert brain.getTotalNeuronCount() > 0
    
    # Test AgentBrain class
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    assert agent is not None
    
    # Test Vision class
    vision = pynlm.Vision(11, 11)
    assert vision.getWidth() == 11
    assert vision.getHeight() == 11
    
    # Test Action class
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    assert action.getType() == pynlm.ActionType.MoveForward
    
    # Test SimpleWorld class
    assert world.getWidth() == 10
    assert world.getHeight() == 10
    
    # Test NeuronId, SynapseId, RegionId
    neuron_id = pynlm.NeuronId(42)
    assert neuron_id.value == 42
    
    synapse_id = pynlm.SynapseId(100)
    assert synapse_id.value == 100
    
    region_id = pynlm.RegionId(200)
    assert region_id.value == 200
```

Tests error handling in Python bindings:

```python
def test_error_handling():
    """Test error handling in Python bindings"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test invalid config access
    try:
        config.get_string("nonexistent_key")
        assert False, "Should have thrown error"
    except RuntimeError:
        pass  # Expected
    
    # Test invalid action type
    try:
        action = pynlm.Action(99999)  # Invalid action type
        assert False, "Should have thrown error"
    except Exception:
        pass  # Expected
    
    # Test invalid vision dimensions
    try:
        vision = pynlm.Vision(0, 0)  # Invalid dimensions
        assert False, "Should have thrown error"
    except Exception:
        pass  # Expected
```

Tests memory management:

```python
def test_memory_management():
    """Test memory management in Python bindings"""
    import gc
    
    # Create and destroy many objects
    initial_objects = len(gc.get_objects())
    
    for i in range(100):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Use objects
        for j in range(10):
            brain.step(j)
        
        # Explicit cleanup
        agent = None
        world = None
        brain = None
        config = None
    
    # Force garbage collection
    gc.collect()
    
    final_objects = len(gc.get_objects())
    
    # Memory usage should be reasonable (not growing indefinitely)
    object_growth = final_objects - initial_objects
    assert object_growth < 1000, f"Too many objects created: {object_growth}"
```

### 5. Validation Tests (test_backward_compatibility.py)

Tests backward compatibility:

```python
def test_backward_compatibility():
    """Test backward compatibility with old API"""
    # Test old Config API
    config = pynlm.Config()
    config.set("simulation_steps", 500)
    assert config.has("simulation_steps")
    assert config.get_int("simulation_steps") == 500
    
    # Test old Brain API
    brain = pynlm.Brain(config)
    brain.initialize()
    brain.step(0)
    assert brain.getTotalSpikeCount() > 0
    
    # Test old ActionType enum
    assert pynlm.ActionType.MoveForward == 0
    assert pynlm.ActionType.MoveBackward == 1
    assert pynlm.ActionType.TurnLeft == 4
    assert pynlm.ActionType.TurnRight == 5
    
    # Test old MotorCommand enum
    assert pynlm.MotorCommand.MoveForward == 0
    assert pynlm.MotorCommand.TurnLeft == 3
```

Tests new convenience methods:

```python
def test_convenience_methods():
    """Test new convenience methods"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test config convenience methods
    assert config.get_simulation_steps() == 1000  # Default
    config.set_simulation_steps(2000)
    assert config.get_simulation_steps() == 2000
    
    width, height = config.get_vision_size()
    assert width == 11 and height == 11
    
    config.set_vision_size(16, 16)
    width, height = config.get_vision_size()
    assert width == 16 and height == 16
    
    # Test brain convenience methods
    brain.step(0)
    assert brain.getFiringNeuronCount() >= 0
    assert brain.getAverageFiringRate() >= 0.0
    assert brain.getDevelopmentalStage() >= pynlm.DevelopmentalStage.Initial
    
    # Test agent convenience methods
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    
    # Verify enabled
    assert agent.isRewardModulationEnabled()
    assert agent.isStructuralPlasticityEnabled()
    assert agent.isDevelopmentEnabled()
    assert agent.isCuriosityEnabled()
    assert agent.isNoveltyDetectionEnabled()
```

Tests that improvements work correctly:

```python
def test_improvements():
    """Test that improvements work correctly"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 9, 9)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test improved error handling
    try:
        # This should fail gracefully with clear error message
        config.set("invalid.key.path", "value")
        assert False, "Should have validated key path"
    except Exception as e:
        assert "key" in str(e).lower() or "invalid" in str(e).lower()
    
    # Test improved performance with large simulations
    import time
    start_time = time.time()
    
    for i in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    elapsed = time.time() - start_time
    assert elapsed < 5.0, f"Simulation too slow: {elapsed:.2f}s"
    
    # Test improved memory efficiency
    import tracemalloc
    tracemalloc.start()
    
    for i in range(100):
        brain.step(i)
    
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    
    # Should use less than 50MB for 100 steps
    assert peak < 50 * 1024 * 1024, f"Memory usage too high: {peak / 1024 / 1024:.2f}MB"
```

## Test Configuration

### pytest.ini

Create a `pytest.ini` file in the tests directory:

```ini
[pytest]
testpaths = [tests]
python_files = [test_*.py]
python_classes = [Test*]
python_functions = [test_*]
addopts = -v --tb=short
filterwarnings =
    ignore::DeprecationWarning
    ignore::FutureWarning
    ignore::PendingDeprecationWarning
markers =
    unit: Unit tests
    integration: Integration tests
    performance: Performance tests
    api: Python API tests
    validation: Validation tests
    slow: Slow tests that require more time
    benchmark: Benchmark tests
```

### requirements.txt

```txt
pytest>=7.0.0
pytest-benchmark>=4.0.0
pytest-timeout>=2.0.0
numpy>=1.20.0
memory-profiler>=0.5.0
psutil>=5.0.0
```

### conftest.py

```python
import pytest
import pynlm
import gc
import time
import tracemalloc
def pytest_configure(config):
    """Configure pytest for NLM tests"""
    config.addinivalue_line("markers", "unit: Unit tests")
    config.addinivalue_line("markers", "integration: Integration tests")
    config.addinivalue_line("markers", "performance: Performance tests")
    config.addinivalue_line("markers", "api: Python API tests")
    config.addinivalue_line("markers", "validation: Validation tests")
    config.addinivalue_line("markers", "slow: Tests that require more time")
    config.addinivalue_line("markers", "benchmark: Benchmark tests")
def pytest_sessionstart(session):
    """Session start hook"""
    print("\n=== NLM Test Suite ===")
    print(f"Python bindings version: {pynlm.__version__ if hasattr(pynlm, '__version__') else 'unknown'}")
    print(f"Pytest version: {pytest.__version__}")
    print("=" * 40)
@pytest.fixture
def default_config():
    """Fixture providing default configuration"""
    return pynlm.createDefaultConfig()
@pytest.fixture
def brain_with_config(config):
    """Fixture providing initialized brain"""
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain
@pytest.fixture
def world_with_agent(brain_with_config):
    """Fixture providing world with agent"""
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain_with_config)
    agent.initialize(world)
    
    return {
        'brain': brain_with_config,
        'world': world,
        'agent': agent
    }
@pytest.fixture(autouse=True)
def memory_cleanup():
    """Fixture to ensure memory cleanup between tests"""
    gc.collect()
    yield
    gc.collect()
def pytest_runtest_setup(item):
    """Setup hook for each test"""
    # Start memory tracking for performance tests
    if "performance" in item.keywords:
        tracemalloc.start()
def pytest_runtest_teardown(item, nextitem):
    """Teardown hook for each test"""
    # Stop memory tracking for performance tests
    if "performance" in item.keywords:
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        # Log memory usage
        print(f"\nMemory usage: {peak / 1024 / 1024:.2f} MB (peak)")
        
        # Assert reasonable memory usage
        if peak > 100 * 1024 * 1024:  # 100MB
            print(f"WARNING: High memory usage: {peak / 1024 / 1024:.2f} MB")
@pytest.hookimpl(tryfirst=True)
def pytest_exception_interact(excinfo, call):
    """Handle exceptions"""
    if excinfo.errisinstance(Exception):
        print(f"\nTest failed with exception: {excinfo.type.__name__}: {excinfo.value}")
```

## Advanced Test Features

### Using pytest-benchmark

```python
def test_brain_performance(benchmark):
    """Test brain performance with benchmark"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run benchmark
    result = benchmark(lambda: [brain.step(i) for i in range(100)])
    
    assert len(result) == 100
    assert brain.getTotalSpikeCount() > 0
```

### Using pytest-timeout

```python
@pytest.mark.timeout(30)
def test_long_simulation():
    """Test long simulation with timeout"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for i in range(1000):
        brain.step(i)
    
    assert brain.getTotalSpikeCount() > 0
```

### Parameterized Tests

```python
import pytest
@pytest.mark.parametrize("neuron_count", [100, 500, 1000, 5000])
def test_different_neuron_counts(neuron_count):
    """Test with different neuron counts"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for i in range(50):
        brain.step(i)
    
    assert brain.getTotalNeuronCount() == neuron_count
@pytest.mark.parametrize("vision_size", [(8, 8), (11, 11), (16, 16)])
def test_different_vision_sizes(vision_size):
    """Test with different vision sizes"""
    width, height = vision_size
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, width, height)
    world.reset()
    
    percept = world.getSensoryPercept()
    assert percept.getVisionWidth() == width
    assert percept.getVisionHeight() == height
```

## Running the Test Suite

### Full Test Suite Execution

```bash
# Run all tests
pytest tests/ -v

# Run all tests with coverage
pytest tests/ --cov=pynlm --cov-report=html --cov-report=term

# Run all tests with benchmarks
pytest tests/ --benchmark

# Run all tests in parallel
pytest tests/ -n auto
```

### Specific Test Categories

```bash
# Unit tests
pytest tests/unit_tests/ -v

# Integration tests
pytest tests/integration_tests/ -v

# Python API tests
pytest tests/python_api_tests/ -v

# Validation tests
pytest tests/validation_tests/ -v

# Performance tests
pytest tests/performance_tests/ -v

# Specific test markers
pytest tests/ -m "unit or integration"
pytest tests/ -m "not slow"  # Skip slow tests
```

### Test Report Generation

```bash
# Generate HTML report
pytest tests/ --html=report.html --self-contained-html

# Generate JSON report
pytest tests/ --json-report --json-report-file=report.json

# Generate JUnit XML report
pytest tests/ --junitxml=test-results.xml

# Run with custom output
pytest tests/ --tb=short --no-header -rN
```

## Expected Results and Validation Criteria

### Unit Tests Criteria

1. **Initialization Tests**: Brain initializes successfully and has neurons
2. **Sensory Processing**: Sensory input is processed correctly
3. **Motor Decoding**: Motor commands are decoded from neural activity
4. **Reward Modulation**: Reward signals affect neural learning
5. **Development Updates**: Brain development progresses over time
6. **Novelty Detection**: Novel stimuli trigger novelty signals
7. **Curiosity Exploration**: Curiosity drives exploration behavior

### Integration Tests Criteria

1. **Brain + World + Agent**: All three components work together
2. **Complete Simulation Loops**: Full simulation from sensory to motor output
3. **Error Conditions**: Errors are handled gracefully

### Performance Tests Criteria

1. **Neuron Scale**: Performance scales reasonably with neuron count
2. **Memory Efficiency**: Memory usage is reasonable and stable
3. **Time Efficiency**: Simulation time per step is reasonable
4. **Resource Management**: Resources are properly cleaned up

### Python API Tests Criteria

1. **All Bindings**: All public Python bindings work correctly
2. **Error Handling**: Errors are handled with clear messages
3. **Memory Management**: Memory is properly managed

### Validation Tests Criteria

1. **Backward Compatibility**: Old API still works
2. **Convenience Methods**: New convenience methods work
3. **Improvements**: All improvements work correctly
4. **Performance**: Performance improvements are effective

## Troubleshooting

### Common Issues

1. **Module Not Found**: Install NLM with Python bindings
   ```bash
   pip install -e .
   ```

2. **Build Failures**: Ensure dependencies are installed
   ```bash
   apt-get update && apt-get install -y cmake build-essential
   pip install scikit-build-core pybind11
   ```

3. **Memory Issues**: Use smaller test configurations
   ```python
   config = pynlm.createDefaultConfig()
   config.set("brain.neuron_count", 100)  # Smaller brain
   ```

4. **Timeouts**: Skip slow tests or increase timeout
   ```bash
   pytest tests/ -m "not slow"
   pytest tests/ --timeout=300
   ```

### Debugging Tips

1. **Print Test Information**:
   ```bash
   pytest tests/ -s -v
   ```

2. **Enable Debug Mode**:
   ```python
   import pynlm
   pynlm.enableDebug(True)
   ```

3. **Check Version**:
   ```python
   import pynlm
   print(f"NLM version: {pynlm.__version__}")
   ```

## License

This test suite is part of the NLM project. All tests are licensed under the MIT License.

## Authors

Test suite developed by the NLM team for validating Python bindings and AgentBrain improvements.