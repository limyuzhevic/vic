# Advanced User Guide - NLM Phase 6

## Introduction

This guide covers advanced usage of the NLM (熙然) Phase 6 artificial brain system, including enhanced CLI features, configuration management, export/import capabilities, and advanced usage patterns for researchers and developers.

## Table of Contents

1. [Enhanced Command-Line Interface](#enhanced-command-line-interface)
   - [Phase 2 Demo](#phase-2-demo)
   - [Phase 6 Integration Demo](#phase-6-integration-demo)
   - [Performance Benchmark Suite](#performance-benchmark-suite)

2. [Advanced Configuration Management](#advanced-configuration-management)
   - [Configuration Presets](#configuration-presets)
   - [Configuration Validation](#configuration-validation)
   - [Export/Import Configuration](#exportimport-configuration)

3. [Brain State Management](#brain-state-management)
   - [Exporting Brain States](#exporting-brain-states)
   - [Importing Brain States](#importing-brain-states)
   - [Checkpointing and Recovery](#checkpointing-and-recovery)

4. [Experiment Automation](#experiment-automation)
   - [Automated Experiment Runs](#automated-experiment-runs)
   - [Batch Processing](#batch-processing)
   - [Experiment Comparison](#experiment-comparison)

5. [Advanced Usage Patterns](#advanced-usage-patterns)
   - [Custom Brain Architectures](#custom-brain-architectures)
   - [Performance Optimization](#performance-optimization)
   - [Integration with External Systems](#integration-with-external-systems)

6. [Troubleshooting and Best Practices](#troubleshooting-and-best-practices)
   - [Common Issues](#common-issues)
   - [Performance Tuning](#performance-tuning)
   - [Debugging Techniques](#debugging-techniques)

## Enhanced Command-Line Interface

### Phase 2 Demo

The Phase 2 demo has been enhanced with comprehensive CLI options for detailed control over neural experiments:

#### Usage Examples

```bash
# Run full demo with custom settings
nlm --neurons 2000 --steps 500 --seed 12345 --verbose

# Run only plasticity experiments
nlm --mode plasticity --no-stdp

# Export results to file
nlm --output phase2_results.json --brain-state brain_state.bin

# Quick check mode
nlm --mode basic --quiet
```

#### Key Options

- `--neurons <n>`: Set neuron count (default: 500)
- `--steps <n>`: Total simulation steps (default: 1000)
- `--seed <n>`: Random seed for reproducibility
- `--mode <mode>`: Run mode (basic|plasticity|stdp|full)
- `--output <file>`: Save results in JSON format
- `--brain-state <file>`: Export brain state for later analysis
- `--verbose/-v`: Enable detailed logging
- `--quiet/-q`: Suppress informational messages

### Phase 6 Integration Demo

The Phase 6 integration demo provides comprehensive testing of all brain systems:

#### Usage Examples

```bash
# Quick integration verification
nlm_phase6_demo --quick

# Test specific subsystems
nlm_phase6_demo --memory-only --neuromod-only

# Full integration with custom settings
nlm_phase6_demo --neurons 1000 --steps 3000 --output phase6_results.csv

# Subsystems test only
nlm_phase6_demo --checkpoint-only --replay-only --no-development
```

#### Key Options

- `--quick/-q`: Run only integration verification
- `--full/-f`: Run full integration test
- `--neurons <n>`: Neuron count (default: 500)
- `--steps <n>`: Simulation steps (default: 2000)
- `--output <file>`: Save results (JSON/CSV formats)
- `--format <fmt>`: Output format (csv, json, tabular)
- `--memory-only`: Test only memory integration
- `--neuromod-only`: Test only neuromodulation integration

### Performance Benchmark Suite

The benchmark suite provides detailed performance analysis with real-time monitoring:

#### Usage Examples

```bash
# Quick performance check
nlm_benchmark --quick

# Thorough benchmark suite
nlm_benchmark --thorough --output perf_results.json

# Custom benchmark with specific parameters
nlm_benchmark --custom 500 2000 5000 --format csv

# Production performance testing
nlm_benchmark --min-neurons 1000 --max-neurons 5000 --steps 2000
```

#### Key Options

- `--quick/-q`: Quick benchmark (4 configurations)
- `--thorough/-t`: Comprehensive benchmark suite
- `--custom/-c`: Custom benchmark with specific parameters
- `--min-neurons <n>`: Minimum neuron count
- `--max-neurons <n>`: Maximum neuron count
- `--steps <n>`: Steps per configuration
- `--output <file>`: Save results
- `--format <fmt>`: Output format (csv, json, tabular)
- `--no-memory`: Skip memory usage calculation
- `--no-cpu`: Skip CPU usage calculation

## Advanced Configuration Management

### Configuration Presets

NLM provides several configuration presets for different use cases:

#### 1. Default Configuration
```cpp
// Create default configuration for general research
Config config = Config::createDefaultConfig();
```

**Characteristics:**
- 500 neurons, 1 region
- Moderate plasticity settings
- Balanced performance and realism
- Suitable for general research and learning

#### 2. Quick Start Configuration
```cpp
// Create quick-start configuration for rapid prototyping
Config config = Config::createQuickStartConfig();
```

**Characteristics:**
- 200 neurons, 1 region
- Faster simulation setup
- Essential features only
- Ideal for debugging and rapid iteration

#### 3. Research Configuration
```cpp
// Create research configuration for comprehensive studies
Config config = Config::createResearchConfig();
```

**Characteristics:**
- 5000 neurons, 4 regions
- High performance settings
- All features enabled
- Optimized for large-scale research

### Configuration Validation

The enhanced configuration system includes validation for all key parameters:

```cpp
// Register a custom validation for specific keys
ConfigValidation validation({
    .key = "custom_parameter",
    .type = "float",
    .description = "A custom configuration parameter",
    .min = 0.0,
    .max = 100.0,
    .required = true
});

Config::registerDefaultValidation("custom_parameter", validation);
```

**Validation Types Supported:**
- `int`: Integer values with min/max bounds
- `float/double`: Floating-point values with min/max bounds
- `bool`: Boolean values
- `string`: String values with optional pattern matching
- `list`: Array/list values

### Export/Import Configuration

Configuration can be exported to JSON format for sharing and analysis:

```bash
# Export configuration from command line
nlm --config myconfig.json

# Save configuration programmatically
config.saveToFile("config.json");

# Export results with validation
config.toJSON(); // Returns JSON string
```

Import supports both JSON and simple key=value formats:

```bash
# Import configuration
nlm --config myconfig.json

# Import and override with command line
nlm --config myconfig.json --neurons 1000 --seed 42
```

## Brain State Management

### Exporting Brain States

Brain states can be exported for analysis, sharing, or checkpoint recovery:

```bash
# Export brain state from Phase 2 demo
nlm --brain-state brain_state_42.bin --output results.json

# Export Phase 6 brain state
nlm_phase6_demo --brain-state phase6_brain.bin --output integration_results.json
```

**Exported Data Includes:**
- Neural connectivity patterns
- Synaptic weights and eligibility traces
- Memory system states
- Neuromodulator levels
- Developmental stage information
- Performance metrics

### Importing Brain States

Brain states can be imported to continue experiments or create new starting points:

```cpp
// Import brain state programmatically
bool success = config.importBrainState("brain_state.bin");
```

**Import Process:**
1. Validate brain state integrity
2. Restore neural architecture
3. Reconstruct synaptic connections
4. Restore memory systems
5. Reinitialize neuromodulators
6. Set developmental stage

### Checkpointing and Recovery

Checkpointing provides incremental backup and recovery capabilities:

```cpp
// Enable checkpointing
config.set("enable_checkpointing", true);
config.set("checkpoint_interval", 1000);
config.set("checkpoint_path", "./checkpoints/");

// Automatically recover from checkpoint
if (config.hasCheckpoint()) {
    config.recoverFromCheckpoint();
}
```

**Checkpoint Features:**
- Periodic state saving during long experiments
- Incremental backups for efficient recovery
- Compression for storage efficiency
- Version control for different brain states

## Experiment Automation

### Automated Experiment Runs

Run multiple experiments with different configurations automatically:

```bash
#!/bin/bash
# Run batch of experiments with different parameters
for neurons in 200 500 1000 2000; do
    for seed in 42 123 456; do
        echo "Running experiment: neurons=$neurons, seed=$seed"
        nlm --neurons $neurons --seed $seed --output "exp_${neurons}_${seed}.json"
        echo "Experiment completed"
    done
done
```

### Batch Processing

Process large numbers of experiments efficiently:

```python
# Python script for batch processing
import subprocess
import json
import os

def run_experiment_batch(configs):
    results = []
    for config in configs:
        print(f"Running experiment: {config}")
        process = subprocess.run([
            'nlm', '--neurons', str(config['neurons']),
            '--seed', str(config['seed']), '--verbose'
        ], capture_output=True, text=True)
        
        results.append({
            'config': config,
            'success': process.returncode == 0,
            'output': process.stdout,
            'error': process.stderr
        })
    
    return results
```

### Experiment Comparison

Compare results from different experimental runs:

```cpp
// Compare brain states from different experiments
bool compareExperiments(const std::string& exp1_file, const std::string& exp2_file) {
    BrainState state1 = BrainState::loadFromFile(exp1_file);
    BrainState state2 = BrainState::loadFromFile(exp2_file);
    
    // Compare neural architecture
    if (state1.getNeuronCount() != state2.getNeuronCount()) {
        std::cout << "Error: Different neuron counts" << std::endl;
        return false;
    }
    
    // Compare synaptic connectivity
    double similarity = state1.calculateSimilarity(state2);
    std::cout << "Brain state similarity: " << similarity << std::endl;
    
    // Compare performance metrics
    auto metrics1 = state1.getPerformanceMetrics();
    auto metrics2 = state2.getPerformanceMetrics();
    
    std::cout << "Experiment 1: " << metrics1.toString() << std::endl;
    std::cout << "Experiment 2: " << metrics2.toString() << std::endl;
    
    return true;
}
```

## Advanced Usage Patterns

### Custom Brain Architectures

Create custom brain architectures by combining different components:

```cpp
// Custom brain with multiple regions
class CustomBrain {
private:
    std::vector<Region> regions;
    std::vector<Connection> connections;
    
public:
    void createMultiRegionArchitecture() {
        // Sensory regions
        regions.push_back(Region("sensory_vision", NeuronType::Sensory, 100));
        regions.push_back(Region("sensory_auditory", NeuronType::Sensory, 50));
        
        // Processing regions
        regions.push_back(Region("association", NeuronType::Internal, 200));
        regions.push_back(Region("planning", NeuronType::Internal, 50));
        
        // Motor regions
        regions.push_back(Region("motor", NeuronType::Motor, 150));
        
        // Create custom connections
        createCustomConnections();
        
        // Initialize with custom configuration
        initCustomConfiguration();
    }
    
private:
    void createCustomConnections() {
        // Implement custom connectivity patterns
        for (size_t i = 0; i < regions.size(); ++i) {
            for (size_t j = 0; j < regions.size(); ++j) {
                if (i != j) {
                    double probability = calculateConnectionProbability(i, j);
                    if (std::rand() / (RAND_MAX + 1.0) < probability) {
                        connections.push_back(Connection(i, j));
                    }
                }
            }
        }
    }
};
```

### Performance Optimization

Optimize performance for specific use cases:

```cpp
// Performance optimization strategies
class PerformanceOptimizer {
public:
    void optimizeForSpeed() {
        config.set("neuron_count", 1000);  // Smaller network
        config.set("connection_probability", 0.05);  // Fewer connections
        config.set("simulation_timestep", 0.001);  // Fixed timestep
        config.set("enable_checkpointing", false);  // Disable checkpointing
    }
    
    void optimizeForAccuracy() {
        config.set("neuron_count", 5000);  // Larger network
        config.set("connection_probability", 0.15);  // More connections
        config.set("simulation_timestep", 0.0001);  // Smaller timestep
        config.set("enable_checkpointing", true);  // Enable checkpointing
        config.set("plasticity_enabled", true);  // Enable plasticity
    }
    
    void optimizeForMemory() {
        config.set("working_memory_size", 50);  // Smaller memory
        config.set("episodic_memory_capacity", 500);  // Limited capacity
        config.set("enable_replay", false);  // Disable replay
        config.set("enable_development", false);  // Disable development
    }
};
```

### Integration with External Systems

Integrate NLM with external systems and data sources:

```cpp
// Integration with external data sources
class ExternalIntegration {
public:
    void loadExternalPatterns(const std::string& filename) {
        std::ifstream file(filename);
        json j;
        file >> j;
        
        // Load sensory patterns
        if (j.contains("vision_patterns")) {
            auto& patterns = j["vision_patterns"];
            for (const auto& pattern : patterns) {
                loadVisionPattern(pattern.get<std::vector<float>>());
            }
        }
        
        // Load reward signals
        if (j.contains("reward_signals")) {
            auto& rewards = j["reward_signals"];
            for (const auto& reward : rewards) {
                loadRewardSignal(reward.get<double>());
            }
        }
    }
    
    void exportResults(const std::string& filename) {
        json j;
        
        // Export neural architecture
        j["neuron_count"] = brain->getTotalNeuronCount();
        j["synapse_count"] = brain->getTotalSynapseCount();
        
        // Export performance metrics
        j["total_reward"] = getTotalReward();
        j["firing_rate"] = getAverageFiringRate();
        
        // Export memory states
        j["working_memory"] = getWorkingMemoryState();
        j["episodic_memory"] = getEpisodicMemoryState();
        
        // Save to file
        std::ofstream file(filename);
        file << j.dump(2);
    }
};
```

## Troubleshooting and Best Practices

### Common Issues

**Issue 1: No activity in the brain**

*Cause*: Insufficient sensory input or incorrect neuron parameters

*Solution*: Check neuron threshold and synaptic weights, ensure proper sensory input:

```cpp
// Debug neuron parameters
for (auto* neuron : brain->getAllNeurons()) {
    std::cout << "Neuron " << neuron->getId() 
              << " V=" << neuron->getMembranePotential()
              << " thresh=" << neuron->getThreshold()
              << " type=" << static_cast<int>(neuron->getType())
              << std::endl;
}
```

**Issue 2: Simulation too slow**

*Cause*: Too many neurons or inefficient configuration

*Solution*: Reduce network size or optimize configuration:

```bash
# Use quick-start configuration for testing
nlm --neurons 200 --steps 500

# For production, use research configuration
nlm --neurons 2000 --steps 1000
```

**Issue 3: Learning not occurring**

*Cause*: Plasticity disabled or incorrect parameters

*Solution*: Check plasticity settings and experiment parameters:

```cpp
// Verify plasticity settings
bool plasticityEnabled = config.getOr<bool>("enable_plasticity", true);
if (!plasticityEnabled) {
    std::cout << "WARNING: Plasticity is disabled!" << std::endl;
    // Enable plasticity
    config.set("enable_plasticity", true);
    config.set("stdp_enabled", true);
    config.set("hebbian_enabled", true);
}
```

### Performance Tuning

**1. Neural Network Size**

```bash
# Small network (fast testing)
nlm --neurons 100 --steps 500

# Medium network (balanced)
nlm --neurons 500 --steps 1000

# Large network (research)
nlm --neurons 2000 --steps 2000
```

**2. Simulation Parameters**

```bash
# Fast simulation (larger timestep)
nlm --config myconfig.cfg

# High precision (smaller timestep)
nlm --config myconfig.cfg --steps 5000
```

**3. Memory Management**

```cpp
// Optimize memory usage
config.set("working_memory_size", 100);  // Limit working memory
config.set("episodic_memory_capacity", 1000);  // Limit episodic memory
config.set("enable_replay", false);  // Disable replay if memory limited
```

### Debugging Techniques

**1. Enable Verbose Logging**

```bash
# Run with verbose logging
nlm --verbose --output debug.log
```

**2. Use Checkpoints for Debugging**

```cpp
// Create checkpoints during debugging
config.set("enable_checkpointing", true);
config.set("checkpoint_interval", 100);

// Verify checkpoint functionality
if (brain->saveCheckpoint("debug_checkpoint.bin")) {
    std::cout << "Checkpoint created successfully" << std::endl;
}
```

**3. Monitor System Resources**

```bash
# Monitor system performance during benchmark
nlm_benchmark --thorough --output perf_data.csv
```

## References

1. Original NLM Documentation: See `docs/` directory for comprehensive documentation
2. Configuration System: Enhanced `core/Config/` with validation and export/import
3. Performance Monitoring: Real-time metrics in benchmark suite
4. Integration Examples: See demo executables for advanced usage patterns

## Quick Start Guide

For rapid experimentation with NLM Phase 6:

1. **Run basic demo**:
   ```bash
   nlm
   ```

2. **Run integration test**:
   ```bash
   nlm_phase6_demo --quick
   ```

3. **Run performance benchmark**:
   ```bash
   nlm_benchmark --quick
   ```

4. **Export brain state**:
   ```bash
   nlm --brain-state my_brain.bin --output results.json
   ```

5. **Import custom configuration**:
   ```bash
   nlm --config myconfig.json
   ```

This guide provides comprehensive coverage of advanced NLM usage patterns, including CLI options, configuration management, brain state operations, and troubleshooting techniques for researchers and developers working with the NLM artificial brain system.