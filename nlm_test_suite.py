# NLM Python Test Suite

This directory contains the NLM (Neural Learning Machine) Python testing framework and benchmarks.

## Overview

The test suite provides comprehensive testing and benchmarking for the NLM Python API, including:
- Unit tests for core functionality
- Integration tests for system components
- Performance benchmarks for common operations
- Detailed reporting and result analysis
- JSON result export for further analysis

## Installation

The NLM Python package must be installed first. You can install it from source using:

```bash
pip install -e .
```

Or from the NLM project directory:

```bash
pip install pynlm
```

## Running Tests

### Basic Usage

Run the test suite using the main script:

```bash
python nlm_test_suite.py
```

This will run all unit tests and benchmarks with default settings.

### Command Line Options

The test suite supports several command-line options:

```bash
python nlm_test_suite.py [options]

Options:
  --help                    Show this help message
  --quick                   Run quick tests (fewer iterations)
  --thorough                 Run thorough tests (more iterations, default)
  --config file.json         Load configuration from JSON file
  --output-dir dir           Set output directory (default: "results")
  --verbose                 Enable verbose output
```

### Example Commands

```bash
# Run basic tests (fast)
python nlm_test_suite.py --quick

# Run thorough tests (comprehensive)
python nlm_test_suite.py --thorough

# Run with custom output directory
python nlm_test_suite.py --output-dir my_results

# Run with verbose output
python nlm_test_suite.py --verbose
```

## Test Categories

The test suite is organized into several categories:

### Creation Tests
- `test_basic_creation`: Test basic brain creation and initialization
- `test_agent_creation`: Test agent brain creation
- `test_world_creation`: Test world creation and configuration
- `test_sensory_percept`: Test sensory percept creation and manipulation

### Execution Tests
- `test_brain_step`: Test brain stepping functionality
- `test_agent_processing`: Test agent sensory processing
- `test_plasticity_enable_disable`: Test plasticity system enable/disable
- `test_neuromodulation_levels`: Test neuromodulation level access
- `test_developmental_stage`: Test developmental stage access

### Integration Tests
- `test_config_operations`: Test configuration management
- `test_memory_system_stats`: Test memory system integration
- `test_world_interaction`: Test brain-world interaction

### Memory Tests
- `test_working_memory`: Test working memory system
- `test_episodic_memory`: Test episodic memory system
- `test_prediction_system`: Test prediction system integration

## Benchmarks

The test suite includes performance benchmarks for common operations:

### Creation Benchmarks
- `benchmark_brain_creation`: Benchmark brain creation and initialization
- `benchmark_agent_creation`: Benchmark agent brain creation

### Execution Benchmarks
- `benchmark_brain_step`: Benchmark brain stepping performance
- `benchmark_agent_processing`: Benchmark agent sensory processing
- `benchmark_config_operations`: Benchmark configuration management

### Memory Benchmarks
- `benchmark_memory_access`: Benchmark memory system access patterns

## Test Results

The test suite saves detailed results to the `results/` directory:

### JSON Files
- `test_results.json`: Complete unit test results
- `benchmark_results.json`: Complete benchmark results
- Individual test files for specific categories

### Summary Reports
- Console output with detailed pass/fail statistics
- Category-wise breakdown of test results
- Performance metrics and throughput calculations
- Error reports and stack traces for failed tests

## Configuration

### Configuration File
You can create a `test_config.json` file to customize test behavior:

```json
{
  "tests": {
    "quick": {
      "brain_step_iterations": 10,
      "agent_processing_iterations": 5,
      "config_ops_iterations": 20,
      "memory_access_iterations": 10
    },
    "thorough": {
      "brain_step_iterations": 100,
      "agent_processing_iterations": 50,
      "config_ops_iterations": 200,
      "memory_access_iterations": 100
    }
  },
  "output": {
    "directory": "results",
    "prefix": "nlm_test",
    "compress": true
  },
  "logging": {
    "level": "INFO",
    "file": "test_suite.log"
  }
}
```

### Environment Variables
- `NLM_TEST_VERBOSE`: Set to "1" for verbose output
- `NLM_TEST_OUTPUT_DIR`: Custom output directory
- `NLM_TEST_ITERATIONS`: Number of iterations for benchmarks

## Python API Testing

The test suite validates the NLM Python API comprehensively:

### Core Functionality
- **Brain Class**: Creation, initialization, stepping, statistics
- **AgentBrain Class**: Sensory processing, motor decoding, neuromodulation
- **SimpleWorld Class**: World management, object interaction, perception
- **Config Class**: Configuration management, serialization

### System Integration
- **Memory Systems**: Working memory, episodic memory, prediction
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
- **Development**: Developmental stages, plasticity modulation
- **Plasticity**: STDP, Hebbian learning, structural plasticity

### Advanced Features
- **Checkpointing**: Brain state save/load functionality
- **Development**: Multi-stage developmental processes
- **Novelty Detection**: Change detection and curiosity modulation
- **Reward Processing**: Reinforcement learning integration

## Performance Analysis

The benchmark suite provides insights into performance characteristics:

### Throughput Measurements
- Operations per second (ops/sec)
- Average execution time with standard deviation
- Minimum/maximum execution time

### Scalability Testing
- Memory access patterns under different loads
- Configuration operations performance
- Concurrent agent processing capability

### Resource Usage
- Memory footprint analysis
- CPU utilization tracking
- I/O performance measurements

## Extending the Test Suite

### Adding New Tests

To add a new test, create a function that follows the test pattern:

```python
def test_new_feature():
    """Test new NLM feature"""
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test logic
    assert brain.getTotalNeuronCount() > 0
    
    # Cleanup
    brain.reset()
    
    return True
```

Add it to the `test_functions` dictionary in the test runner.

### Adding New Benchmarks

To add a new benchmark:

```python
def benchmark_new_feature(iterations):
    """Benchmark new NLM feature"""
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Benchmark logic
    def benchmark_operation():
        brain.step(0)
        return brain.getTotalSpikeCount()
    
    return [benchmark_operation() for _ in range(iterations)]
```

Add it to the `benchmark_functions` dictionary in the test runner.

## Common Errors and Solutions

### "ImportError: No module named 'pynlm'"

**Solution**: Ensure the NLM Python package is installed:

```bash
pip install pynlm
```

Or install from source:

```bash
pip install -e .
```

### "AttributeError: 'Brain' object has no attribute 'method'"

**Solution**: Check that the NLM Python API is up-to-date. The method may have been renamed or moved.

### "AssertionError: test failed"

**Solution**: Check the test output for specific assertion errors. The test failure message should indicate which assertion failed.

### "TimeoutError: test took too long"

**Solution**: Increase the number of benchmark iterations or check the benchmark function for performance issues.

## Integration with CI/CD

The test suite can be integrated into continuous integration pipelines:

### GitHub Actions Example

Create a `.github/workflows/test.yml` file:

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
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Set up Python
      uses: actions/setup-python@v2
      with:
        python-version: '3.8'
    
    - name: Install dependencies
      run: |
        pip install pybind11 scikit-build-core pytest numpy
        pip install -e .
    
    - name: Run tests
      run: |
        python nlm_test_suite.py --quick
    
    - name: Upload results
      uses: actions/upload-artifact@v2
      with:
        name: test-results
        path: results/
```

## Contributing

### Reporting Issues

If you encounter issues with the test suite:

1. Check the test output for error messages
2. Verify that the NLM Python package is installed correctly
3. Report bugs with detailed reproduction steps
4. Include test output and configuration files

### Improving Tests

To improve the test suite:

1. Add coverage for edge cases
2. Improve performance of existing tests
3. Add more comprehensive error handling
4. Include integration tests for real-world scenarios

## Future Enhancements

The test suite can be extended with:

### New Test Categories
- **Stress Testing**: Test system behavior under extreme conditions
- **Concurrent Testing**: Test multiple agents/systems simultaneously
- **Long-running Tests**: Test extended simulation sessions
- **Cross-language Testing**: Test interoperability with other languages

### Advanced Benchmarking
- **Memory Profiling**: Detailed memory usage analysis
- **GPU Acceleration Testing**: If GPU support is added
- **Distributed Testing**: Test performance across multiple machines

### AI/ML Integration
- **Neural Architecture Search**: Test different brain configurations
- **Hyperparameter Optimization**: Benchmark different parameter sets
- **Model Compression**: Test performance with compressed models

## License

This test suite is part of the NLM (Neural Learning Machine) project and is licensed under the MIT License.

For more information about the NLM project, visit: https://github.com/nlm-project/nlm

---

*Last updated: $(date +'%Y-%m-%d %H:%M:%S')*
*For questions and support, visit: https://github.com/nlm-project/nlm/issues*