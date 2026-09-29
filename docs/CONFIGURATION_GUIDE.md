# NLM Configuration Guide

## Overview

The NLM (熙然) configuration system provides comprehensive control over brain parameters, performance settings, and experiment configuration. This guide covers all available configuration options, how to create and manage configurations, and best practices for configuring NLM for different use cases.

## Configuration System

### Configuration Structure

NLM uses a hierarchical configuration system with the following levels:

1. **Global Configuration** - System-wide settings
2. **Brain Configuration** - Brain-specific parameters
3. **Experiment Configuration** - Experiment parameters
4. **Performance Configuration** - Performance optimization settings

### Configuration Formats

#### YAML Configuration
```yaml
name: "working_memory_experiment"
seed: 123456789
neuron_count: 5000
synapse_density: 0.1
plasticity_rate: 0.1
working_memory_capacity: 100
prediction_horizon: 3
development_enabled: true
reward_modulation_enabled: true
structural_plasticity_enabled: false
curiosity_enabled: true
thread_count: 4
vectorization_enabled: true
sparse_connectivity_enabled: true
event_queue_size: 65536
```

#### JSON Configuration
```json
{
  "name": "working_memory_experiment",
  "seed": 123456789,
  "neuron_count": 5000,
  "synapse_density": 0.1,
  "plasticity_rate": 0.1,
  "working_memory_capacity": 100,
  "prediction_horizon": 3,
  "development_enabled": true,
  "reward_modulation_enabled": true,
  "structural_plasticity_enabled": false,
  "curiosity_enabled": true,
  "thread_count": 4,
  "vectorization_enabled": true,
  "sparse_connectivity_enabled": true,
  "event_queue_size": 65536
}
```

### Configuration File Locations

- **Project configs/**: `configs/` - Project-specific configurations
- **User configs/**: `~/.config/nlm/` - User preferences
- **Experiment configs/**: `experiments/configs/` - Experiment templates
- **Temp configs/**: `/tmp/nlm/` - Temporary configurations

## Configuration Categories

### Core Brain Configuration

#### neuron_count
- **Type**: `size_t`
- **Default**: `1000`
- **Range**: `[100, 1000000]`
- **Description**: Total number of neurons in the brain
- **Impact**: Memory usage, computational complexity, learning capacity
- **Performance**: Linear scaling with neurons

#### synapse_density
- **Type**: `float`
- **Default**: `0.1f`
- **Range**: `[0.0f, 1.0f]`
- **Description**: Probability of synapse formation between neurons
- **Impact**: Network connectivity, information flow
- **Performance**: O(N²) potential, sparse connectivity recommended

#### plasticity_rate
- **Type**: `float`
- **Default**: `0.1f`
- **Range**: `[0.0f, 1.0f]`
- **Description**: Base learning rate for plasticity rules
- **Impact**: Learning speed, stability of learned behaviors
- **Performance**: Moderate impact

#### working_memory_capacity
- **Type**: `size_t`
- **Default**: `100`
- **Range**: `[10, 10000]`
- **Description**: Maximum number of items in working memory
- **Impact**: Ability to maintain and manipulate information
- **Performance**: Linear memory usage

#### prediction_horizon
- **Type**: `size_t`
- **Default**: `3`
- **Range**: `[1, 100]`
- **Description**: Number of future steps predicted by prediction system
- **Impact**: Planning depth, temporal reasoning capability
- **Performance**: Quadratic growth with horizon

### Development Configuration

#### development_enabled
- **Type**: `bool`
- **Default**: `true`
- **Description**: Enable developmental processes
- **Impact**: Age-dependent plasticity, critical periods
- **Performance**: Significant impact on learning dynamics

#### developmental_stage
- **Type**: `DevelopmentalStage`
- **Default**: `DEVELOPMENTAL_STAGE_ADULT`
- **Values**: `INITIAL`, `CRITICAL`, `MATURATION`, `ADULT`
- **Description**: Current developmental stage
- **Impact**: Plasticity rate, learning capabilities
- **Performance**: Stage-dependent performance

#### critical_period_duration
- **Type**: `double`
- **Default**: `100.0`
- **Range**: `[0.0, 1000.0]`
- **Description**: Length of critical learning period
- **Impact**: Learning window, behavioral flexibility
- **Performance**: Duration of high plasticity

#### maturation_threshold
- **Type**: `double`
- **Default**: `200.0`
- **Range**: `[0.0, 1000.0]`
- **Description**: Age at which brain reaches adult maturity
- **Impact**: Stabilization of learned behaviors
- **Performance**: Long-term memory consolidation

### Neuromodulation Configuration

#### reward_modulation_enabled
- **Type**: `bool`
- **Default**: `true`
- **Description**: Enable reward-based learning
- **Impact**: Reinforcement learning, goal-directed behavior
- **Performance**: Moderate impact

#### curiosity_enabled
- **Type**: `bool`
- **Default**: `true`
- **Description**: Enable curiosity-driven exploration
- **Impact**: Exploration-exploitation balance
- **Performance**: Significant impact on behavior diversity

#### novelty_threshold
- **Type**: `float`
- **Default**: `0.1f`
- **Range**: `[0.0f, 1.0f]`
- **Description**: Threshold for detecting novel stimuli
- **Impact**: Exploration triggers, learning novelty
- **Performance**: Affects exploration frequency

#### dopamine_strength
- **Type**: `float`
- **Default**: `1.0f`
- **Range**: `[0.0f, 10.0f]`
- **Description**: Magnitude of reward prediction error signals
- **Impact**: Learning signal strength
- **Performance**: Affects convergence rate

#### prediction_error_decay
- **Type**: `float`
- **Default**: `0.99f`
- **Range**: `[0.0f, 1.0f]`
- **Description**: Decay rate of prediction error signals
- **Impact**: Credit assignment span
- **Performance**: Temporal credit assignment accuracy

### Performance Configuration

#### thread_count
- **Type**: `size_t`
- **Default**: `std::thread::hardware_concurrency()`
- **Range**: `[1, 64]`
- **Description**: Number of threads for parallel processing
- **Impact**: CPU utilization, simulation speed
- **Performance**: Near-linear scaling up to ~8 cores

#### vectorization_enabled
- **Type**: `bool`
- **Default**: `true`
- **Description**: Enable SIMD vectorization
- **Impact**: CPU instruction-level parallelism
- **Performance**: Significant speedup for neural computations

#### sparse_connectivity_enabled
- **Type**: `bool`
- **Default**: `true`
- **Description**: Use sparse connectivity representation
- **Impact**: Memory efficiency, scalable to large networks
- **Performance**: Critical for scaling beyond 10k neurons

#### event_queue_size
- **Type**: `size_t`
- **Default**: `16384`
- **Range**: `[1024, 262144]`
- **Description**: Size of spike event queues
- **Impact**: Spike processing capacity, event loss
- **Performance**: Queue-full events affect performance

#### memory_pool_size
- **Type**: `size_t`
- **Default**: `size_t(1.5 * neuron_count)`
- **Description**: Total memory pool size
- **Impact**: Allocation efficiency, memory fragmentation
- **Performance**: O(1) allocation for neurons/synapses

### Random Configuration

#### random_seed
- **Type**: `uint64_t`
- **Default**: `std::chrono::system_clock::now().time_since_epoch().count()`
- **Description**: Seed for random number generator
- **Impact**: Reproducibility of experiments
- **Performance**: No impact on performance

#### deterministic_mode
- **Type**: `bool`
- **Default**: `false`
- **Description**: Use deterministic random generation
- **Impact**: Reproducible experiments
- **Performance**: Same as random mode

### Experiment Configuration

#### experiment_name
- **Type**: `std::string`
- **Default**: `""`
- **Description**: Unique identifier for experiment
- **Impact**: Results organization, file naming
- **Performance**: No impact

#### max_steps
- **Type**: `size_t`
- **Default**: `1000`
- **Range**: `[1, 1000000]`
- **Description**: Maximum simulation steps
- **Impact**: Experiment duration, data collection
- **Performance**: Linear scaling

#### checkpoint_interval
- **Type**: `size_t`
- **Default**: `100`
- **Range**: `[1, 10000]`
- **Description**: Interval between checkpoints
- **Impact**: Recovery capability, memory usage
- **Performance**: Checkpoint overhead

#### measurement_interval
- **Type**: `size_t`
- **Default**: `10`
- **Range**: `[1, 100]`
- **Description**: Interval between measurements
- **Impact**: Data granularity, memory usage
- **Performance**: Measurement overhead

## Creating Configuration Files

### Using Command Line

Create a configuration file from command line:

```bash
# Create basic configuration
cat > config.yaml << EOF
name: "my_experiment"
seed: 123456789
neuron_count: 5000
plasticity_rate: 0.1
EOF

# Create comprehensive configuration
cat > experiment_config.yaml << EOF
name: "working_memory_capacity_test"
seed: 123456789
neuron_count: 10000
synapse_density: 0.05
plasticity_rate: 0.05
working_memory_capacity: 500
prediction_horizon: 10
development_enabled: true
reward_modulation_enabled: true
structural_plasticity_enabled: true
curiosity_enabled: true
thread_count: 8
vectorization_enabled: true
sparse_connectivity_enabled: true
event_queue_size: 262144
EOF
```

### Using API

```cpp
#include "core/Config/Config.hpp"

// Create empty configuration
auto config = std::make_shared<Config>();

// Set individual parameters
config->set("neuron_count", 10000);
config->set("plasticity_rate", 0.05f);

// Load from YAML file
config = Config::loadFromFile("config.yaml");

// Save to YAML file
config->saveToFile("experiment_config.yaml");

// Copy configuration
auto experiment_config = config->copy();

// Merge configurations
auto base_config = Config::loadFromFile("base.yaml");
base_config->merge(experiment_config);
```

### Configuration Validation

Validate configuration before use:

```cpp
// Check if configuration is valid
if (!config->validate()) {
    std::cout << "Configuration errors:" << std::endl;
    for (const auto& error : config->getErrors()) {
        std::cout << "  - " << error << std::endl;
    }
    return 1;
}
```

### Configuration Templates

Create reusable configuration templates:

#### Working Memory Test Template
```yaml
name: "working_memory_template"
seed: 123456789
neuron_count: 5000
synapse_density: 0.1
plasticity_rate: 0.1
working_memory_capacity: 100
prediction_horizon: 3
development_enabled: true
reward_modulation_enabled: true
structural_plasticity_enabled: false
curiosity_enabled: true
thread_count: 4
vectorization_enabled: true
sparse_connectivity_enabled: true
event_queue_size: 65536
```

#### Large Scale Test Template
```yaml
name: "large_scale_template"
seed: 123456789
neuron_count: 100000
synapse_density: 0.05
plasticity_rate: 0.05
working_memory_capacity: 500
prediction_horizon: 10
development_enabled: false
reward_modulation_enabled: true
structural_plasticity_enabled: true
curiosity_enabled: true
thread_count: 8
vectorization_enabled: true
sparse_connectivity_enabled: true
event_queue_size: 262144
memory_pool_size: size_t(1.5 * neuron_count)
```

## Configuration Best Practices

### For Scientific Experiments

1. **Reproducibility**: Always specify `seed` for reproducibility
2. **Validation**: Validate configuration before running experiments
3. **Documentation**: Document configuration in experiment reports
4. **Variation**: Test different parameter combinations systematically

### For Performance Testing

1. **Scaling Tests**: Use linear scaling laws for neuron count
2. **Memory Monitoring**: Monitor memory usage at target scale
3. **Hardware Awareness**: Set `thread_count` appropriately for hardware
4. **Baseline Testing**: Establish baseline with conservative settings

### For Production Use

1. **Error Handling**: Check return values from configuration operations
2. **Logging**: Enable logging for debugging
3. **Monitoring**: Set up performance monitoring
4. **Recovery**: Configure appropriate checkpoint intervals

### Common Configuration Patterns

#### Small Scale Research
```yaml
name: "small_scale_research"
seed: 123456789
neuron_count: 1000
synapse_density: 0.2
plasticity_rate: 0.2
working_memory_capacity: 50
prediction_horizon: 2
development_enabled: true
reward_modulation_enabled: true
curiosity_enabled: false
```

#### Medium Scale Experiment
```yaml
name: "medium_scale_experiment"
seed: 123456789
neuron_count: 10000
synapse_density: 0.1
plasticity_rate: 0.1
working_memory_capacity: 200
prediction_horizon: 5
development_enabled: true
reward_modulation_enabled: true
structural_plasticity_enabled: true
curiosity_enabled: true
thread_count: 4
```

#### Large Scale Production
```yaml
name: "large_scale_production"
seed: 123456789
neuron_count: 100000
synapse_density: 0.05
plasticity_rate: 0.05
working_memory_capacity: 1000
prediction_horizon: 20
development_enabled: true
reward_modulation_enabled: true
structural_plasticity_enabled: true
curiosity_enabled: true
thread_count: 8
vectorization_enabled: true
sparse_connectivity_enabled: true
event_queue_size: 262144
memory_pool_size: size_t(2.0 * neuron_count)
```

## Configuration CLI Tools

### Configuration Utility

Build and use configuration utilities:

```bash
# Build configuration tools
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Use configuration utility
./nlm_config --help

# Generate configuration from template
./nlm_config generate working_memory_template config.yaml

# Validate configuration
./nlm_config validate config.yaml

# Merge configurations
./nlm_config merge base.yaml experiment.yaml merged.yaml
```

### Configuration Commands

```bash
# List available templates
./nlm_config templates

# Show template details
./nlm_config show working_memory_template

# Create new configuration from template
./nlm_config create my_experiment working_memory_template

# Export configuration to different formats
./nlm_config export config.yaml --format json
./nlm_config export config.yaml --format cpp

# Compare configurations
./nlm_config compare config1.yaml config2.yaml
```

## Advanced Configuration

### Runtime Configuration Changes

Change configuration during runtime:

```cpp
// Change configuration after brain creation
brain->getConfig()->set("working_memory_capacity", 200);

// Re-initialize brain with new configuration
brain->reset();
brain->initialize();
```

### Configuration Inheritance

Create configuration inheritance hierarchy:

```cpp
// Base configuration
auto base_config = Config::loadFromFile("base.yaml");

// Experiment-specific configuration
auto exp_config = Config::loadFromFile("experiment.yaml");

// Merge with experiment taking precedence
base_config->merge(exp_config);
```

### Configuration Versioning

Track configuration changes:

```cpp
// Version configuration
config->setVersion("1.0.0");
config->addComment("Experiment parameter tuning");

// Compare versions
auto old_config = Config::loadFromFile("v1.0.0.yaml");
auto new_config = Config::loadFromFile("v1.1.0.yaml");
auto diff = old_config->diff(new_config);
```

## Configuration Monitoring

### Runtime Metrics

Monitor configuration effectiveness:

```cpp
// Get configuration metrics
auto metrics = config->getMetrics();
std::cout << "Configuration metrics:" << std::endl;
std::cout << "  Memory usage: " << metrics.memoryUsage << std::endl;
std::cout << "  Performance score: " << metrics.performanceScore << std::endl;
std::cout << "  Stability: " << metrics.stability << std::endl;
```

### Configuration Validation

Validate configuration continuously:

```cpp
// Validate configuration periodically
void validateConfiguration(std::shared_ptr<Config> config) {
    if (!config->validate()) {
        std::cerr << "Configuration invalid!" << std::endl;
        // Log errors
        for (const auto& error : config->getErrors()) {
            std::cerr << "  - " << error << std::endl;
        }
        // Take corrective action
        config->autoCorrect();
    }
}
```

## Configuration Migration

### Migrate Between Versions

Migrate configuration between versions:

```cpp
// Migrate old configuration format
auto old_config = Config::loadFromFile("old_format.yaml");
auto new_config = old_config->migrateToFormat("new_format.yaml");

// Apply migration rules
new_config->set("working_memory_capacity", old_config->get<size_t>("working_memory_capacity") * 2);
```

## Configuration Documentation

### Document Configuration Effects

Document configuration parameter effects:

```cpp
// Add documentation comments
config->set("neuron_count", 5000,
    "Number of neurons in brain",
    "Memory: ~4 bytes * neuron_count",
    "Performance: O(neuron_count) scaling",
    "Default: 1000",
    "Range: [100, 1000000]");
```

### Generate Configuration Documentation

Generate configuration documentation:

```bash
# Generate configuration documentation
./nlm_config --generate-docs docs/configuration_documentation.md

# Generate example configurations
./nlm_config --generate-examples examples/configs/
```

## Configuration Files

### Included Configuration Files

The project includes several configuration files:

- `configs/experiment_template.yaml` - Basic experiment template
- `configs/performance_optimized.yaml` - Performance-optimized configuration
- `configs/research_template.yaml` - Research-oriented configuration
- `configs/production_template.yaml` - Production configuration

### Example Configuration Files

Check the `examples/configs/` directory for configuration examples and templates.

## Configuration Best Practices Summary

1. **Always specify seeds** for reproducible experiments
2. **Validate configurations** before use
3. **Document configuration effects** in experiment reports
4. **Monitor configuration performance** during long runs
5. **Use appropriate scales** for different use cases
6. **Back up important configurations**
7. **Use templates** for common configurations
8. **Test configurations** before deployment
9. **Monitor configuration drift** over time
10. **Version important configurations** for reproducibility

## Conclusion

Configuration is a critical aspect of NLM that determines both experimental outcomes and performance. By understanding and properly using the configuration system, users can:

- Run reproducible scientific experiments
- Optimize performance for different scales
- Create flexible experimental designs
- Monitor and debug configuration issues
- Maintain configuration consistency across runs

This guide provides comprehensive coverage of NLM configuration options and best practices for successful experimentation.