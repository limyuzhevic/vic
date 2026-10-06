# Test Configuration Examples

This directory contains configuration files specifically for testing NLM functionality.

## Test Configuration Overview

These configuration files are optimized for fast, reliable testing and cover different testing scenarios:

- **Minimal Configuration**: Fast unit tests with reduced scope
- **Integration Testing**: Comprehensive tests for system integration
- **Performance Testing**: Tests optimized for measuring performance
- **Stress Testing**: Tests that push system limits

## Example 1: Unit Test Configuration

**File: `tests/configs/unit_test.cfg`**

```
# Unit Test Configuration
# Optimized for fast, isolated testing

# Core parameters - minimal scope
random_seed = 42
simulation_timestep = 0.001
neuron_count = 50          # Smaller for faster testing
region_count = 1
connection_probability = 0.2

# Strict parameters for predictable behavior
neuron_threshold = -55.0
neuron_resting_potential = -70.0
neuron_reset_potential = -65.0
refractory_period = 3

# Minimal plasticity for unit testing
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.008
stdp_tau = 15.0
synaptogenesis_rate = 0.00001
pruning_rate = 0.000001

# Development disabled for unit tests
enable_development = false

# Sparse connectivity for faster setup
enable_sparse_connectivity = true
connection_threshold = 0.1

# Memory optimization
working_memory_capacity = 10
episodic_memory_capacity = 50

# Minimal logging for cleaner test output
log_level = WARNING
log_file = unit_test.log

# Fast checkpointing for tests
enable_checkpointing = true
checkpoint_interval = 100
```

## Example 2: Integration Test Configuration

**File: `tests/configs/integration_test.cfg`**

```
# Integration Test Configuration
# Tests for system integration and compatibility

# Core parameters - sufficient for integration testing
random_seed = 123456
simulation_timestep = 0.001
neuron_count = 200          # Medium scale
region_count = 2
connection_probability = 0.15

# Realistic neuron parameters
neuron_threshold = -55.0
neuron_resting_potential = -70.0
neuron_reset_potential = -65.0
refractory_period = 5

# Balanced plasticity for integration testing
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# All plasticity types enabled for integration
enable_hebbian_plasticity = true
enable_reward_plasticity = true
enable_structural_plasticity = true

# Full neuromodulation suite for integration
enable_dopamine = true
dopamine_sensitivity = 1.0
enable_curiosity = true
curiosity_sensitivity = 0.8
enable_novelty = true
novelty_threshold = 0.1
novelty_decay = 0.95

# Complete memory systems
enable_working_memory = true
working_memory_capacity = 30
enable_episodic_memory = true
episodic_memory_capacity = 200
enable_concept_formation = true
concept_retention = 0.9

# Advanced cognitive features
enable_prediction = true
prediction_horizon = 10
prediction_confidence = 0.8
enable_attention = true
attention_span = 5
enable_planning = true
planning_horizon = 20
planning_confidence = 0.7

# Development enabled for integration testing
enable_development = true
development_duration = 60.0
development_plasticity_factor = 0.5

# Replay and consolidation enabled
enable_replay = true
replay_frequency = 0.1
replay_strength = 0.8
enable_consolidation = true
consolidation_threshold = 0.7

# Performance features
enable_checkpointing = true
checkpoint_interval = 500
enable_sparse_connectivity = true
connection_threshold = 0.05
enable_simd = true
enable_parallel = true

# Standard logging for integration tests
log_level = INFO
log_file = integration_test.log
metrics_output = integration_metrics.csv
performance_monitoring = true
```

## Example 3: Performance Test Configuration

**File: `tests/configs/performance_test.cfg`**

```
# Performance Test Configuration
# Optimized for measuring performance and scalability

# Core parameters - larger scale for performance measurement
random_seed = 999999
simulation_timestep = 0.001
neuron_count = 1000          # Large scale for performance
region_count = 4
connection_probability = 0.1

# Optimized neuron parameters for performance
neuron_threshold = -50.0      # Higher threshold for stability
neuron_resting_potential = -70.0
neuron_reset_potential = -65.0
refractory_period = 10       # Longer refractory for performance

# Balanced plasticity for performance testing
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.012
stdp_tau = 25.0
synaptogenesis_rate = 0.00005
pruning_rate = 0.000005

# All features enabled for comprehensive performance testing
enable_hebbian_plasticity = true
enable_reward_plasticity = true
enable_structural_plasticity = true

# Full neuromodulation
enable_dopamine = true
dopamine_sensitivity = 1.2    # Higher sensitivity for performance
enable_curiosity = true
curiosity_sensitivity = 1.0
enable_novelty = true
novelty_threshold = 0.08
novelty_decay = 0.93

# Advanced memory systems
enable_working_memory = true
working_memory_capacity = 100
enable_episodic_memory = true
episodic_memory_capacity = 1000
enable_concept_formation = true
concept_retention = 0.95      # High retention for performance

# Advanced cognition
enable_prediction = true
prediction_horizon = 20
prediction_confidence = 0.9
enable_attention = true
attention_span = 10
enable_planning = true
planning_horizon = 50
planning_confidence = 0.8

# Development enabled
enable_development = true
development_duration = 120.0
 development_plasticity_factor = 0.3

# Performance features enabled
enable_checkpointing = true
checkpoint_interval = 2000
enable_sparse_connectivity = true
connection_threshold = 0.08
enable_simd = true
enable_parallel = true

# Performance monitoring enabled
log_level = INFO
log_file = performance_test.log
metrics_output = performance_metrics.csv
performance_monitoring = true
```

## Example 4: Stress Test Configuration

**File: `tests/configs/stress_test.cfg`**

```
# Stress Test Configuration
# Tests system limits and robustness

# Core parameters - maximum scale for stress testing
random_seed = 888888
simulation_timestep = 0.0005     # Smaller timestep for stability
neuron_count = 5000              # High scale for stress
region_count = 10                 # Many regions for stress
connection_probability = 0.08

# Extreme but valid neuron parameters
neuron_threshold = -45.0         # Higher threshold
neuron_resting_potential = -75.0 # More hyperpolarized
neuron_reset_potential = -70.0
refractory_period = 20           # Long refractory

# Maximum plasticity settings
stdp_ltp_weight = 0.025          # High LTP
stdp_ltd_weight = 0.02           # High LTD
stdp_tau = 40.0                  # Long STDP window
synaptogenesis_rate = 0.00002   # High synaptogenesis
pruning_rate = 0.000002         # High pruning

# All features enabled at maximum
enable_hebbian_plasticity = true
enable_reward_plasticity = true
enable_structural_plasticity = true

# Extreme neuromodulation
enable_dopamine = true
dopamine_sensitivity = 2.0       # Maximum sensitivity
enable_curiosity = true
curiosity_sensitivity = 1.5      # High sensitivity
enable_novelty = true
novelty_threshold = 0.05         # Low threshold
novelty_decay = 0.98             # Slow decay

# Massive memory systems
enable_working_memory = true
working_memory_capacity = 500
enable_episodic_memory = true
episodic_memory_capacity = 5000
enable_concept_formation = true
concept_retention = 0.99          # Very high retention

# Advanced cognition
enable_prediction = true
prediction_horizon = 50
prediction_confidence = 0.95
enable_attention = true
attention_span = 15
enable_planning = true
planning_horizon = 100
planning_confidence = 0.9

# Extended development
enable_development = true
development_duration = 300.0
development_plasticity_factor = 0.1

# Advanced features enabled
enable_checkpointing = true
checkpoint_interval = 5000
enable_sparse_connectivity = true
connection_threshold = 0.12     # Lower threshold for more connections
enable_simd = true
enable_parallel = true

# Comprehensive monitoring
log_level = WARNING               # Reduce log noise
log_file = stress_test.log
metrics_output = stress_metrics.csv
performance_monitoring = true
```

## Configuration File Usage

### Command Line Integration

Test configurations can be used with the NLM CLI:

```bash
# Run unit tests
./nlm test --config tests/configs/unit_test.cfg

# Run integration tests
./nlm test --config tests/configs/integration_test.cfg

# Run performance tests
./nlm test --config tests/configs/performance_test.cfg

# Run stress tests
./nlm test --config tests/configs/stress_test.cfg
```

### Test Command Options

Available test options:
- `--verbose` : Enable verbose output
- `--output file` : Save test results to file
- `--iterations N` : Number of test iterations
- `--parallel` : Run tests in parallel
- `--continue-on-error` : Continue testing even if some tests fail

### Performance Monitoring

Test configurations include performance monitoring settings:
- `metrics_output` : File for performance metrics
- `performance_monitoring` : Enable/disable performance monitoring
- `checkpoint_interval` : Frequency of checkpoint saves

## Test Configuration Validation

Each test configuration includes validation scripts:

```bash
# Validate test configuration
./nlm validate --config tests/configs/unit_test.cfg

# Compare configurations
./nlm config-compare tests/configs/unit_test.cfg tests/configs/integration_test.cfg

# Generate configuration report
./nlm config-report tests/configs/performance_test.cfg
```

## Test Configuration Templates

For quick setup, use the templates:

```bash
# Create a new test config
cp tests/configs/unit_test.cfg my_custom_test.cfg

# Modify parameters in the template
./nlm config-template --create my_test.cfg --neurons 100 --regions 2
```

## Configuration Testing Best Practices

1. **Isolate Changes**: Use minimal configurations for unit tests
2. **Realistic Parameters**: Use realistic values for integration tests
3. **Performance Focused**: Optimize for timing measurements in performance tests
4. **Boundary Testing**: Use extreme values for stress testing
5. **Resource Management**: Monitor and limit resource usage in all tests
