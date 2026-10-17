# NLM Testing Framework

This directory contains the testing infrastructure for the NLM Python bindings.

## Overview

The testing framework provides comprehensive unit tests, integration tests, and performance benchmarks
for the NLM Python bindings. It ensures the bindings work correctly with the C++ core library
and maintains compatibility across different Python versions.

## Test Structure

### Unit Tests (`tests/unit/`)
- **Core Functionality**: Basic brain operations, configuration management
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, reward processing
- **Plasticity**: STDP, Hebbian learning, structural plasticity
- **Error Handling**: Invalid inputs, edge cases, error recovery

### Integration Tests (`tests/integration/`)
- **Phase 6 Integration**: Complete brain system integration
- **Cross-System Communication**: Memory-neuromodulation interactions
- **Persistence**: Save/load checkpoint functionality
- **Real-world Scenarios**: Complex simulation scenarios

### Performance Tests (`tests/performance/`)
- **Large Scale**: Testing with 10,000+ neurons
- **Real-time Performance**: Monitoring and control systems
- **Memory Usage**: Memory management and optimization
- **Parallel Processing**: Batch operations and concurrency

### Functional Tests (`tests/functional/`)
- **API Coverage**: Comprehensive API testing
- **User Scenarios**: Common user workflows and use cases
- **Configuration Testing**: Various configuration options
- **Integration Testing**: Integration with other tools and frameworks

## Running Tests

### Prerequisites

```bash
pip install -r requirements.txt
python -m pytest --version
```

### Running All Tests

```bash
pytest
```

### Running Specific Test Suites

```bash
# Unit tests
pytest tests/unit/

# Integration tests
pytest tests/integration/

# Performance tests
pytest tests/performance/

# Functional tests
pytest tests/functional/

# Run tests for a specific module
pytest tests/unit/test_brain.py
```

### Running Tests with Coverage

```bash
pytest --cov=pynlm --cov-report=html
```

### Running Performance Benchmarks

```bash
python -m pytest tests/performance/ --benchmark-only --benchmark-save=results.json
```

## Test Configuration

### Environment Variables

```bash
# Set test environment variables
export NLM_TEST_BENCHMARK_MODE=1
export NLM_TEST_DATA_PATH=/path/to/test/data
export NLM_TEST_PYTHON_VERSION=3.8
```

### Configuration File

Create `tests/config.json` for test configuration:

```json
{
    "neuron_count": 100,
    "region_count": 1,
    "connection_probability": 0.15,
    "max_steps": 1000,
    "benchmark_mode": false,
    "verbose_output": true
}
```

## Test Fixtures

### Brain Fixture

Provides a standardized brain for testing:

```python
@pytest.fixture
def test_brain():
    """Create a standardized test brain."""
    config = {
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.15,
    }
    brain = Brain(**config)
    brain.initialize()
    return brain
```

### Memory Fixture

Provides memory systems for testing:

```python
@pytest.fixture
def test_memory():
    """Create test memory systems."""
    from pynlm.memory import WorkingMemory, EpisodicMemory
    
    wm = WorkingMemory()
    em = EpisodicMemory()
    
    # Set up test data
    wm.add_trace("test_trace_1", {"value": 1.0})
    em.store_episode({"step": 0, "activity": "test"})
    
    return wm, em
```

### Neuromodulation Fixture

Provides neuromodulation systems for testing:

```python
@pytest.fixture
def test_neuromodulation():
    """Create test neuromodulation systems."""
    from pynlm.neuromodulation import DopamineSystem, CuriositySystem
    
    dopamine = DopamineSystem()
    curiosity = CuriositySystem()
    
    return dopamine, curiosity
```

## Test Categories

### 1. Core Functionality Tests

**Files**: `tests/unit/test_core.py`

**Tests**:
- Brain initialization and configuration
- Basic simulation steps
- Neural activity tracking
- System status and diagnostics

**Examples**:
```python
def test_brain_initialization():
    """Test brain initialization."""
    brain = Brain(neuron_count=100)
    assert brain.initialize()
    assert brain.getTotalNeuronCount() == 100

def test_simulation_step():
    """Test basic simulation step."""
    brain = Brain(neuron_count=50)
    brain.initialize()
    
    for i in range(10):
        brain.step(i, i * 0.001)
    
    assert brain.getTotalSpikeCount() > 0
```

### 2. Memory System Tests

**Files**: `tests/unit/test_memory.py`

**Tests**:
- Working memory trace management
- Episodic memory storage and retrieval
- Associative memory pattern matching
- Memory consolidation

**Examples**:
```python
def test_working_memory():
    """Test working memory functionality."""
    wm = WorkingMemory()
    
    # Add traces
    wm.add_trace("test1", {"value": 1.0})
    wm.add_trace("test2", {"value": 2.0})
    
    # Retrieve traces
    traces = wm.get_active_traces()
    assert len(traces) == 2
    assert traces["test1"]["value"] == 1.0
```

### 3. Neuromodulation Tests

**Files**: `tests/unit/test_neuromodulation.py`

**Tests**:
- Dopamine system dynamics
- Curiosity system exploration
- Reward prediction error
- Novelty detection

**Examples**:
```python
def test_dopamine_system():
    """Test dopamine system."""
    dopamine = DopamineSystem()
    
    # Process reward
    dopamine.process_reward(1.0, 0.5)
    
    # Check modulation
    assert dopamine.get_modulation_level() > 0.0
```

### 4. Plasticity Tests

**Files**: `tests/unit/test_plasticity.py`

**Tests**:
- STDP learning rule
- Hebbian plasticity
- Structural plasticity
- Synaptogenesis and pruning

**Examples**:
```python
def test_stdp():
    """Test STDP plasticity."""
    synapse = Synapse()
    
    # Record initial weight
    initial_weight = synapse.get_weight()
    
    # Apply STDP
    synapse.apply_stdp(pre_spike=True, post_spike=True)
    
    # Check weight change
    new_weight = synapse.get_weight()
    assert new_weight != initial_weight
```

### 5. Development Tests

**Files**: `tests/unit/test_development.py`

**Tests**:
- Synaptogenesis
- Pruning
- Maturation
- Age-dependent plasticity

**Examples**:
```python
def test_synaptogenesis():
    """Test developmental synaptogenesis."""
    dev = DevelopmentSystem()
    
    initial_synapses = dev.get_synaptic_count()
    dev.update(0.001)
    
    # Check synapse formation
    new_synapses = dev.get_synaptic_count()
    assert new_synapses >= initial_synapses
```

## Integration Tests

### Phase 6 Integration Tests

**Files**: `tests/integration/test_phase6_integration.py`

**Tests**:
- Complete system integration
- Memory-neuromodulation coupling
- Prediction system integration
- Development and plasticity interaction

**Examples**:
```python
def test_complete_integration():
    """Test complete Phase 6 integration."""
    from pynlm import Phase6Integration
    
    integration = Phase6Integration()
    result = integration.run(neuron_count=500, max_steps=1000)
    
    # Check integration status
    assert result.memory_working_memory_integrated
    assert result.neuromodulation_integrated
    assert result.prediction_integrated
```

### Cross-System Communication Tests

**Files**: `tests/integration/test_cross_system.py`

**Tests**:
- Memory and neuromodulation interaction
- Prediction and planning coupling
- Development and learning integration
- Sensory and motor system coordination

## Performance Tests

### Large Scale Tests

**Files**: `tests/performance/test_large_scale.py`

**Tests**:
- Performance with 10,000 neurons
- Memory usage analysis
- Simulation speed
- Parallel processing capabilities

**Example**:
```python
def test_large_scale_performance(benchmark):
    """Test performance with large brain."""
    brain = Brain(neuron_count=10000)
    brain.initialize()
    
    # Run simulation
    benchmark(brain.step, 1000)
```

### Real-time Tests

**Files**: `tests/performance/test_realtime.py`

**Tests**:
- Real-time processing capabilities
- Monitoring system performance
- Event-driven processing
- Latency measurements

## Functional Tests

### User Workflow Tests

**Files**: `tests/functional/test_user_workflows.py`

**Tests**:
- Typical user scenarios
- Configuration and setup
- Experiment execution
- Result analysis

**Examples**:
```python
def test_learning_workflow():
    """Test typical learning workflow."""
    # Create brain
    brain = Brain(neuron_count=500)
    brain.initialize()
    
    # Run learning experiment
    experiment = Experiment(brain)
    result = experiment.run(steps=1000)
    
    # Analyze results
    assert result.learning_occurred
    assert result.final_performance > 0.5
```

### Configuration Tests

**Files**: `tests/functional/test_configuration.py`

**Tests**:
- Different configuration formats
- Configuration validation
- Default configuration
- Custom configuration

## Advanced Testing

### Property-Based Testing

Use Hypothesis for property-based testing:

```python
from hypothesis import given, strategies as st

@given(neuron_count=st.integers(10, 1000))
def test_brain_initialization(neuron_count):
    """Test brain initialization with various neuron counts."""
    brain = Brain(neuron_count=neuron_count)
    assert brain.initialize()
    assert brain.getTotalNeuronCount() == neuron_count
```

### Fuzz Testing

For testing robustness with invalid inputs:

```python
def test_invalid_inputs():
    """Test robustness with invalid inputs."""
    import pytest
    
    # Test with negative neuron count
    with pytest.raises(ValueError):
        Brain(neuron_count=-1)
    
    # Test with zero steps
    brain = Brain(neuron_count=10)
    brain.initialize()
    
    with pytest.raises(ValueError):
        for step in range(0):
            brain.step(step, 0.001)
```

## Test Results

### Expected Test Results

- **Unit Tests**: >95% coverage, <5% failure rate
- **Integration Tests**: >90% success rate
- **Performance Tests**: Within 20% of performance benchmarks
- **Functional Tests**: >98% user scenario success

### Test Reporting

Generated reports include:

1. **HTML Coverage Report**: Interactive coverage visualization
2. **JSON Results**: Machine-readable test results
3. **PDF Report**: Detailed test documentation
4. **Dashboard**: Real-time test monitoring

### CI/CD Integration

**GitHub Actions Workflow**:

```yaml
name: NLM Tests

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Set up Python
      uses: actions/setup-python@v2
      with:
        python-version: 3.8
    
    - name: Install dependencies
      run: |
        pip install -r python/requirements.txt
        pip install -r tests/requirements.txt
    
    - name: Run unit tests
      run: pytest tests/unit/ -v
    
    - name: Run integration tests
      run: pytest tests/integration/ -v
    
    - name: Run functional tests
      run: pytest tests/functional/ -v
    
    - name: Generate coverage report
      run: pytest --cov=pynlm --cov-report=xml
    
    - name: Upload coverage
      uses: codecov/codecov-action@v2
```

## Test Data

### Test Data Directory

`tests/data/` contains:

- **Simulation Data**: Pre-recorded simulation runs
- **Configuration Files**: Various configuration examples
- **Expected Results**: Known good results for validation
- **Benchmark Data**: Performance baseline data

### Data Generation

Automatic test data generation:

```python
def generate_test_data():
    """Generate test data for machine learning."""
    from pynlm import Brain, Experiment
    
    # Generate training data
    brains = []
    results = []
    
    for i in range(100):
        brain = Brain(neuron_count=100)
        brain.initialize()
        
        experiment = Experiment(brain)
        result = experiment.run(steps=500)
        
        brains.append(brain)
        results.append(result)
    
    # Save generated data
    save_test_data(brains, results)
```

## Maintenance

### Adding New Tests

1. **Identify Test Category**: Core, Integration, Performance, or Functional
2. **Create Test File**: `tests/category/test_name.py`
3. **Add Fixtures**: Define required test fixtures
4. **Write Tests**: Implement test cases
5. **Add Documentation**: Document test purpose and expected behavior

### Updating Tests

1. **Update Dependencies**: Keep dependencies current
2. **Add New Test Cases**: Cover new functionality
3. **Update Test Data**: Refresh test data if changed
4. **Update Documentation**: Update test documentation

### Troubleshooting

**Common Issues**:

1. **Test Failures**: Check test configuration and dependencies
2. **Performance Issues**: Optimize test code and remove unnecessary operations
3. **Memory Issues**: Use memory-efficient data structures
4. **Python Version Issues**: Test with multiple Python versions

**Solutions**:

1. Use `pytest -v` for verbose output
2. Use `pytest --tb=short` for concise traceback
3. Use `pytest --lf` to run only failed tests
4. Use `pytest --ff` to run previously failed tests

## Future Enhancements

### Machine Learning Integration

- **Model Training**: Train ML models on simulation data
- **Automated Experiment Design**: Use ML to optimize experiment parameters
- **Transfer Learning**: Apply learned models to new tasks

### Advanced Testing

- **Differential Testing**: Compare implementations across platforms
- **Property Testing**: Test system properties with mathematical proofs
- **Chaos Testing**: Test robustness under extreme conditions

### Continuous Testing

- **Integration Testing**: Test against staging environments
- **Performance Monitoring**: Monitor test performance over time
- **Load Testing**: Test with large-scale simulations

---
*This testing framework ensures the NLM Python bindings are robust, reliable, and ready for production use.*
