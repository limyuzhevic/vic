# NLM Test Framework (Phase 2 Tests)

## Overview

The NLM Test Framework provides comprehensive testing of NLM's core neural computation capabilities (Phase 2). It offers a modular, extensible testing architecture for validating LIF neuron dynamics, spike propagation, plasticity mechanisms, and basic brain functionality.

## Quick Start

### Build the Test Framework

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make nlm_tests
```

### Run Tests

```bash
# Run all available tests
./nlm_tests

# Run specific test suite
./nlm_tests --test all
./nlm_tests --test basic-connectivity
./nlm_tests --test plasticity-learning
./nlm_tests --test stdp-verification

# Run with verbose output
./nlm_tests --test all --verbose

# Run custom test with specific parameters
./nlm_tests --test plasticity-learning --neuron_count=2000 --connection_probability=0.05
```

## Available Test Categories

### 1. Basic Connectivity Test
**Purpose**: Verifies neurons can propagate spikes through the network
**Tests**: Fundamental neural signal transmission and spike propagation
**Use Case**: Validates basic neural computation functionality

### 2. Plasticity Learning Test
**Purpose**: Demonstrates measurable synaptic changes through repeated experience
**Tests**: Long-term memory formation, Hebbian learning, and synaptic strengthening
**Use Case**: Validates learning capabilities and experience-dependent adaptation

### 3. STDP Verification Test
**Purpose**: Validates spike-timing dependent plasticity (STDP) mechanism
**Tests**: Pre-before-post activity produces potentiation (LTP)
**Use Case**: Confirms precise biological learning mechanisms

### 4. Memory System Integration Test
**Purpose**: Tests integration between working, episodic, and associative memory
**Tests**: Cross-system communication and coordination
**Use Case**: Validates memory architecture integration

### 5. Neuromodulation Integration Test
**Purpose**: Tests integration between dopamine, curiosity, novelty detection, and prediction error
**Tests**: Coordinated neuromodulator action on neural processing
**Use Case**: Validates motivation and learning signal integration

### 6. Development Integration Test
**Purpose**: Tests developmental system integration and plasticity modulation
**Tests**: Age-dependent changes in neural dynamics
**Use Case**: Validates developmental trajectory modeling

## Configuration and Command Line Options

### Test Selection

| Option | Description | Example |
|--------|-------------|---------|
| `--test all` | Run all available tests | `./nlm_tests --test all` |
| `--test <name>` | Run specific test | `./nlm_tests --test basic-connectivity` |
| `--verbose` | Enable detailed output | `./nlm_tests --test all --verbose` |

### Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `--neuron_count` | int | 500 | Number of neurons in test brain |
| `--connection_probability` | float | 0.15 | Probability of synaptic connections |
| `--simulation_timestep` | float | 0.001 | Simulation time step (s) |
| `--random_seed` | int | 42 | Random seed for reproducible tests |
| `--config=<file>` | string | configs/default.cfg | Load configuration from file |

### Custom Parameter Examples

```bash
# Custom network size for performance testing
./nlm_tests --test all --neuron_count=2000

# Higher connection probability for dense networks
./nlm_tests --test all --connection_probability=0.3

# Slower simulation for detailed observation
./nlm_tests --test all --simulation_timestep=0.0005

# Reproducible testing with custom seed
./nlm_tests --test all --random_seed=12345
```

## Configuration Files

### Default Configuration

Create or modify configuration files in the `configs/` directory:

**configs/default.cfg (example):**
```
# NLM Test Framework Configuration
neuron_count = 500
region_count = 1
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
random_seed = 42
```

**Usage:**
```bash
./nlm_tests --test all --config=configs/default.cfg
```

### Custom Configuration Examples

**configs/memory_test.cfg:**
```
# Memory system intensive testing
neuron_count = 1000
region_count = 2
connection_probability = 0.1
synaptogenesis_rate = 0.0005
pruning_rate = 0.00005
random_seed = 11111
```

**configs/plasticity_test.cfg:**
```
# Plasticity mechanism intensive testing
neuron_count = 800
region_count = 1
connection_probability = 0.2
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.025
random_seed = 22222
```

## Test Results and Output

### Standard Output

Tests provide detailed feedback on execution:

```
============================================
NLM TEST RUNNER - Phase 2
============================================
Running 6 test(s)...

--- Running: Basic Connectivity Test ---
Tests if neurons can propagate spikes through the network
✓ PASSED
Execution time: 0.15 seconds
Details: Spikes generated: 8 ✓ Spikes propagate through network

--- Running: Plasticity Learning Test ---
Tests measurable synaptic changes through repeated experience
✗ FAILED
Execution time: 3.45 seconds
Details: ✗ No learning: Weights did not change significantly
```

### Verbose Output Mode

Use `--verbose` for detailed metrics:

```bash
./nlm_tests --test all --verbose
```

Verbose output includes:
- Detailed execution parameters
- Step-by-step test progress
- Internal metrics and statistics
- Resource usage information

## Integration and Compatibility

### With Existing NLM Build System

The test framework integrates seamlessly with the existing NLM build system:

```bash
# Build all targets
make -j4

# Build specific targets
make nlm_tests
make nlm
make nlm_phase3_demo
```

### With Development Workflow

1. **Development Testing**: Run specific tests during implementation
2. **Integration Testing**: Validate cross-system connectivity
3. **Performance Benchmarking**: Measure execution efficiency
4. **Regression Testing**: Ensure backward compatibility

### Continuous Integration

The test framework is designed for CI/CD integration:

```yaml
# Example GitHub Actions configuration
- name: Run NLM Tests
  run: |
    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make nlm_tests
    ./nlm_tests --test all --verbose
```

## Advanced Features

### Test Composition

Run multiple specific tests:

```bash
# Individual tests
./nlm_tests --test basic-connectivity
./nlm_tests --test stdp-verification

# Test combinations
./nlm_tests --test plasticity-learning --test memory-integration
```

### Performance Benchmarking

Use custom parameters for performance testing:

```bash
# Large-scale network testing
./nlm_tests --test all --neuron_count=5000

# Dense connectivity testing
./nlm_tests --test all --connection_probability=0.5

# Slow-motion simulation for analysis
./nlm_tests --test all --simulation_timestep=0.01
```

### Custom Test Development

The test framework is extensible:

1. **Add new test types**: Create new classes inheriting from `NLMTest`
2. **Custom test suites**: Combine existing tests for specific scenarios
3. **Parameter sweeps**: Systematically test parameter ranges
4. **Integration tests**: Test cross-system compatibility

## Troubleshooting

### Common Issues

| Issue | Solution |
|-------|----------|
| Test fails to compile | Ensure dependencies installed, rebuild after changes |
| Configuration file not found | Use default config or create custom config |
| Test execution hangs | Check resource limits, use timeout, reduce parameters |
| Permission denied | Check file permissions, use `chmod +x` |
| No output | Enable verbose mode with `--verbose` |

### Error Messages

- **Configuration Error**: Check file format and syntax
- **Resource Error**: Increase system resources or reduce test scale
- **Integration Error**: Verify all dependencies are properly linked

## Testing Best Practices

### Before Running Tests

1. **Check system requirements**
   ```bash
   # Verify CMake version
   cmake --version
   # Check available memory
   free -h
   ```

2. **Validate configuration**
   ```bash
   # Check configuration file syntax
   cat configs/default.cfg
   ```

3. **Set up build environment**
   ```bash
   mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

### During Testing

1. **Monitor resource usage**
   ```bash
   # Monitor test progress
   timeout 60 ./nlm_tests --test all
   ```

2. **Check for failures**
   ```bash
   # Run with verbose output for details
   ./nlm_tests --test all --verbose
   ```

3. **Validate results**
   ```bash
   # Check specific test output
   ./nlm_tests --test stdp-verification
   ```

### After Testing

1. **Clean up temporary files**
   ```bash
   # Remove checkpoint files
   rm -f /tmp/nlm_checkpoint_test.bin
   ```

2. **Review test results**
   - Check output for failed tests
   - Verify performance metrics
   - Document any issues or unexpected results

## Example Workflows

### Quick Validation Workflow

```bash
# 1. Validate basic functionality
./nlm_tests --test basic-connectivity

# 2. Test learning capabilities
./nlm_tests --test plasticity-learning

# 3. Verify plasticity mechanisms
./nlm_tests --test stdp-verification

# 4. Comprehensive integration test
./nlm_tests --test all --verbose
```

### Performance Optimization Workflow

```bash
# 1. Baseline performance test
./nlm_tests --test all --neuron_count=500

# 2. Scale up for larger networks
./nlm_tests --test all --neuron_count=2000

# 3. Optimize for specific connectivity patterns
./nlm_tests --test all --connection_probability=0.05
```

### Research and Development Workflow

```bash
# 1. Test new configuration parameters
./nlm_tests --test all --config=configs/research_config.cfg

# 2. Validate specific mechanisms
./nlm_tests --test stdp-verification

# 3. Test memory system integration
./nlm_tests --test memory-integration

# 4. Verify neuromodulation effects
./nlm_tests --test neuromodulation-integration
```

## Support and Documentation

### Getting Help

- **Project Documentation**: Check `docs/` directory for detailed documentation
- **Configuration Examples**: Review `configs/` directory for examples
- **Issue Reporting**: Report bugs in project issue tracker
- **Development Support**: Join project community for development discussions

### Community Resources

- **Documentation**: Comprehensive guides and API references
- **Examples**: Sample configurations and test scripts
- **Tutorials**: Step-by-step guides for common tasks
- **Support**: Community forums and issue tracking

## Related Projects

The test framework integrates with:

- **NLM Core Library**: `nlm_core` - Fundamental neural computation
- **Phase 3 Systems**: `nlm_agent`, `nlm_world` - World interaction
- **Phase 4 Systems**: `nlm_phase4` - Complex cognition
- **Phase 6 Integration**: `nlm_phase6_demo` - Full system integration

## Future Enhancements

### Planned Improvements

1. **Automated Test Generation**: Generate test cases from specifications
2. **Parallel Test Execution**: Run tests concurrently for faster execution
3. **Visual Test Results**: Graphical visualization of test outcomes
4. **Integration with CI/CD**: Native support for continuous integration
5. **Performance Profiling**: Detailed performance analysis tools

### Extensions

- **Custom Test Suites**: User-defined test combinations
- **Scenario Testing**: Real-world scenario validation
- **Stress Testing**: Extreme parameter validation
- **Regression Testing**: Automated consistency checks

## Acknowledgments

This test framework builds upon the foundation of NLM's evolutionary development:

- **Phase 2**: Core neural computation and plasticity
- **Phase 3**: World interaction and agent behavior
- **Phase 4**: Complex cognition and planning
- **Phase 5**: Performance optimizations and scaling
- **Phase 6**: Full integration and deployment

The Phase 2 Test Framework provides comprehensive validation of NLM's foundational capabilities while supporting the development and deployment of advanced neural simulation systems.