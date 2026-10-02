# NLM Advanced User Guide

This document provides comprehensive information about using NLM (Neural Learning Machine) at an advanced level, including expert configuration options, performance optimization, and advanced usage patterns.

## Table of Contents

1. [Introduction to Advanced Features](#introduction)
2. [Expert Configuration](#expert-configuration)
3. [Performance Optimization](#performance-optimization)
4. [Advanced Sensory Processing](#advanced-sensory-processing)
5. [Neuromodulation and Learning](#neuromodulation-and-learning)
6. [Script Automation](#script-automation)
7. [Troubleshooting and Debugging](#troubleshooting)
8. [Advanced Examples](#advanced-examples)

## Introduction <a name="introduction"></a>

NLM is an experimental artificial developmental brain project with Phase 6 (Final Integration) representing the most complete and sophisticated version available. This guide covers advanced usage patterns for experienced users who want to maximize the capabilities of the neural system.

## Expert Configuration <a name="expert-configuration"></a>

The expert configuration system provides advanced options for fine-tuning NLM performance and behavior. These options are not visible in the basic user interface and should be used with care.

### Expert Configuration Options

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `enableSIMD` | bool | false | Enable SIMD optimizations for neural computation |
| `enableParallel` | bool | false | Enable parallel processing across multiple threads |
| `threadCount` | int | 0 | Number of threads (0 = auto-detect) |
| `memoryPoolSize` | size_t | 0 | Memory pool size in bytes (0 = auto-allocate) |
| `enableMemoryTracking` | bool | false | Enable detailed memory tracking and reporting |
| `enableDebugLogging` | bool | false | Enable debug-level logging for troubleshooting |
| `enableMetrics` | bool | false | Enable performance metrics collection |
| `metricsOutputFile` | string | "" | File path for metrics output (empty = console) |

### Expert Configuration Commands

```bash
# Enable expert mode (enables debug logging and metrics)
./nlm --expert

# Configure specific expert options
./nlm --enable-simd --thread-count=8 --memory-pool-size=1024000000

# Load expert configuration from file
./nlm --config=expert.cfg

# Run with performance profiling
./nlm --expert --enable-metrics --metrics-output=performance.prof
```

### Configuration File Formats

#### JSON Configuration (Recommended for Expert Use)

```json
{
  "neuron_count": 2000,
  "region_count": 4,
  "connection_probability": 0.15,
  "expert_mode": true,
  "enable_simd": true,
  "thread_count": 8,
  "enable_debug_logging": true,
  "enable_metrics": true,
  "simulation_timestep": 0.0005,
  "enable_time_stepping": true,
  
  "expert_options": {
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "dopamine_baseline": 0.08,
    "synaptogenesis_rate": 0.0002f,
    "pruning_rate": 0.00002f,
    "working_memory_capacity": 200,
    "replay_interval": 50,
    "consolidation_interval": 500
  }
}
```

#### YAML Configuration (Alternative Format)

```yaml
# NLM Expert Configuration
neurons:
  count: 2000
  regions: 4
  connection_probability: 0.15

simulation:
  timestep: 0.0005
  enable_adaptive_time_stepping: true
  enable_time_stepping: true

expert:
  mode: true
  enable_simd: true
  thread_count: 8
  enable_debug_logging: true
  enable_metrics: true
  metrics_output: "performance.prof"

plasticity:
  stdp_ltp_weight: 0.02
  stdp_ltd_weight: 0.015
  dopamine_baseline: 0.08
  synaptogenesis_rate: 0.0002f
  pruning_rate: 0.00002f

memory:
  working_capacity: 200
  replay_interval: 50
  consolidation_interval: 500
```

#### TOML Configuration (Modern Format)

```toml
# NLM Expert Configuration
title = "NLM Expert Configuration"

[neurons]
count = 2000
regions = 4
connection_probability = 0.15

[simulation]
timestep = 0.0005
enable_adaptive_time_stepping = true
enable_time_stepping = true

[expert]
mode = true
enable_simd = true
thread_count = 8
enable_debug_logging = true
enable_metrics = true
metrics_output = "performance.prof"

[plasticity]
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
dopamine_baseline = 0.08
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002

[memory]
working_capacity = 200
replay_interval = 50
consolidation_interval = 500
```

## Performance Optimization <a name="performance-optimization"></a>

### SIMD Optimization

NLM supports Single Instruction Multiple Data (SIMD) optimizations for neural computation:

```bash
# Enable SIMD optimizations
./nlm --enable-simd

# Configure thread count for SIMD operations
./nlm --enable-simd --thread-count=8
```

SIMD optimizations provide significant speed improvements for:
- Neuron membrane potential updates
- Synaptic weight calculations
- Matrix operations in neural populations
- Vectorized memory operations

### Parallel Processing

Enable parallel processing for multi-core systems:

```bash
# Enable parallel processing
./nlm --enable-parallel --thread-count=8

# Configure memory pool for better memory management
./nlm --enable-parallel --thread-count=12 --memory-pool-size=2048000000
```

### Adaptive Time Stepping

Advanced users can enable adaptive time stepping for performance optimization:

```bash
# Enable adaptive time stepping
./nlm --expert --enable-time-stepping

# Configure with specific parameters
./nlm --expert --enable-time-stepping --simulation-timestep=0.0002
```

## Advanced Sensory Processing <a name="advanced-sensory-processing"></a>

### Vision Processing Configuration

Advanced vision processing allows fine-tuning of feature extraction:

```cpp
// Configure vision processor for expert use
VisionProcessor vision;
vision.configure(
    contrastThreshold: 0.7f,      // Higher contrast sensitivity
    edgeSensitivity: 1.5f,       // Enhanced edge detection
    motionDetection: 0.5f,       // Better motion tracking
    temporalSmoothing: 0.8f      // S smoother temporal integration
);
```

### Audio Processing Configuration

Advanced audio processing provides detailed spectral analysis:

```cpp
// Configure audio processor for expert use
AudioProcessor audio;
audio.configure(
    frequencyResolution: 0.005f,      // Finer frequency resolution
    temporalResolution: 0.0005f,       // Higher temporal resolution
    spectralCentroidWeight: 0.7f,       // Emphasize spectral centroid
    zeroCrossingRateWeight: 0.4f,       // Enhanced rhythm detection
    temporalSmoothing: 0.8f           // S smoother temporal integration
);
```

### Neural Integration

Advanced sensory processing can directly integrate with the brain's neural network:

```cpp
// Process sensory input with brain integration
brain->receiveSensoryInput(vision.getFeatures());
brain->receiveSensoryInput(audio.getFeatures());
brain->receiveSensoryInput(internal.getSignalVector());
```

## Neuromodulation and Learning <a name="neuromodulation-and-learning"></a>

### Advanced Dopamine Dynamics

The enhanced dopamine system now includes sophisticated reward prediction error calculations:

```cpp
// Configure advanced dopamine dynamics
Dopamine& dopamine = *brain->getDopamine();
dopamine.configure(
    baseline: 0.08f,        // Higher baseline tonic firing
    peak: 1.2f,             // Higher phasic burst amplitude
    decayRate: 0.08f,       // Slower decay for longer effects
    releaseRate: 1.5f       // Faster release for stronger signals
);

// Signal reward with advanced prediction error
dopamine.signalRewardPredictionError(0.15f);  // Better-than-expected reward
dopamine.signalRewardPredictionError(-0.05f); // Worse-than-expected reward
```

### Advanced Plasticity Rules

The enhanced plasticity system supports more sophisticated learning rules:

```cpp
// Configure advanced plasticity parameters
config->set("stdp_ltp_weight", 0.02f, ConfigSource::Runtime, "STDP LTP weight", "Expert: Controls long-term potentiation strength");
config->set("stdp_ltd_weight", 0.015f, ConfigSource::Runtime, "STDP LTD weight", "Expert: Controls long-term depression strength");
config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Runtime, "Synaptogenesis rate", "Expert: Controls new synapse formation rate");
config->set("pruning_rate", 0.00002f, ConfigSource::Runtime, "Pruning rate", "Expert: Controls synapse elimination rate");
```

## Script Automation <a name="script-automation"></a>

NLM supports expert scripting for automated configuration and batch processing:

### Simple Script Format

```python
# Expert configuration script
script = """
# Configuration settings
neuron_count = 3000
region_count = 6
connection_probability = 0.2
enable_simd = true
thread_count = 16

# Plasticity settings
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
dopamine_baseline = 0.1
synaptogenesis_rate = 0.0003
pruning_rate = 0.00003

# Memory settings
working_memory_capacity = 300
replay_interval = 25
consolidation_interval = 250

# Execution settings
enable_debug_logging = true
enable_metrics = true
metrics_output = "expert_run.prof"
"""
```

### Using Expert Script

```bash
# Create expert script file
./nlm --expert --execute=expert_script.txt

# Combine with configuration file
./nlm --config=base.cfg --execute=expert_script.txt
```

## Troubleshooting and Debugging <a name="troubleshooting"></a>

### Debug Logging

Enable comprehensive debug logging for troubleshooting:

```bash
# Enable debug logging
./nlm --expert --enable-debug-logging

# Redirect output to file
./nlm --expert --enable-debug-logging --log-file=debug.log
```

### Common Issues and Solutions

#### Issue: Simulation Instability

**Symptoms**: Neurons firing erratically, weights becoming unstable

**Solutions**:
- Reduce simulation timestep: `--simulation-timestep=0.0002`
- Decrease connection probability: `--connection-probability=0.05`
- Enable structural plasticity constraints: --enable-plasticity-constraints
- Increase synaptic weight bounds: `--weight-min=-0.5 --weight-max=0.5`

#### Issue: Memory Exhaustion

**Symptoms**: System running out of memory during simulation

**Solutions**:
- Configure memory pool: `--memory-pool-size=1073741824`
- Reduce neuron count: `--neuron-count=1000`
- Enable efficient memory management: `--enable-memory-pooling`
- Monitor memory usage: `--enable-memory-tracking`

#### Issue: Performance Degradation

**Symptoms**: Slow simulation performance

**Solutions**:
- Enable SIMD optimizations: `--enable-simd`
- Use parallel processing: `--enable-parallel --thread-count=8`
- Configure appropriate timestep: `--simulation-timestep=0.001`
- Disable unnecessary features: `--disable-metrics --disable-debug-logging`

### Debug Information Output

The expert configuration system provides detailed debug information:

```bash
# Get comprehensive system information
./nlm --expert --info

# Get performance metrics
./nlm --expert --metrics

# Get configuration validation
./nlm --expert --validate-config
```

## Advanced Examples <a name="advanced-examples"></a>

### Example 1: High-Performance Simulation

```cpp
// High-performance simulation configuration
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

int main() {
    auto config = std::make_shared<Config>();
    
    // Expert configuration for high-performance simulation
    config->set("neuron_count", 5000, ConfigSource::Runtime, 
                "Number of neurons for high-performance simulation",
                "Expert: Larger networks require more computational resources");
    
    config->set("region_count", 8, ConfigSource::Runtime,
                "Number of neural regions",
                "Expert: More regions improve parallelization");
    
    config->set("connection_probability", 0.08, ConfigSource::Runtime,
                "Connection probability between neurons",
                "Expert: Lower value for sparser, more efficient connectivity");
    
    // Expert options
    auto& expertOpts = config->getExpertOptions();
    expertOpts.enableSIMD = true;
    expertOpts.enableParallel = true;
    expertOpts.threadCount = 12;
    expertOpts.enableDebugLogging = false;  // Suppress for production
    expertOpts.enableMetrics = true;
    expertOpts.metricsOutputFile = "high_perf.prof";
    expertOpts.memoryPoolSize = 2048000000;  // 2GB memory pool
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (brain->initialize()) {
        std::cout << "High-performance brain initialized successfully" << std::endl;
        
        // Run simulation
        for (SimulationStep step = 0; step < 10000; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Save performance metrics
        brain->save("high_perf_checkpoint.nlm");
    }
    
    return 0;
}
```

### Example 2: Advanced Neuromodulation Learning

```cpp
// Advanced neuromodulation and learning example
#include "neuromodulation/Neuromodulator.hpp"
#include "plasticity/STDP.hpp"

void advancedLearningDemo() {
    // Create brain with expert configuration
    auto config = std::make_shared<Config>();
    config->set("enable_advanced_neuromodulation", true, ConfigSource::Runtime,
                "Enable advanced neuromodulation features",
                "Expert: Enables sophisticated dopamine and serotonin dynamics");
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Configure advanced dopamine dynamics
    auto dopamine = brain->getDopamine();
    dopamine->configure(
        baseline: 0.1f,           // Higher baseline for better motivation
        peak: 1.5f,              // Stronger phasic responses
        decayRate: 0.05f,        // Slower decay for longer learning effects
        releaseRate: 2.0f        // Faster release for quick adaptation
    );
    
    // Configure advanced STDP learning
    auto stdp = brain->getSTDP();
    stdp->configure(
        ltpWeight: 0.03f,       // Stronger LTP for learning
        ltdWeight: 0.025f,      // Balanced LTD for forgetting
        tau: 25.0f,             // Longer STDP time constant
        learningRate: 0.01f     // Enhanced learning rate
    );
    
    // Advanced reward-based learning scenario
    for (SimulationStep step = 0; step < 5000; ++step) {
        // Present challenge with variable difficulty
        float challengeDifficulty = 0.5f + 0.3f * std::sin(step * 0.01f);
        
        // Agent attempts task
        float reward = brain->processAction(challengeDifficulty);
        
        // Advanced reward prediction error
        float predictionError = reward - brain->getExpectedReward(challengeDifficulty);
        
        // Signal reward prediction error (dopamine burst/dip)
        dopamine->signalRewardPredictionError(predictionError);
        
        // Apply neuromodulation to plasticity
        float plasticityFactor = dopamine->getPlasticityFactor();
        
        // Apply advanced learning with neuromodulation
        if (plasticityFactor > 1.2f) {
            // High plasticity: aggressive learning
            stdp->updateWithAugmentation(step, plasticityFactor * 1.5f);
        } else if (plasticityFactor < 0.8f) {
            // Low plasticity: cautious learning
            stdp->updateWithAugmentation(step, plasticityFactor * 0.7f);
        } else {
            // Normal plasticity: standard learning
            stdp->update(step);
        }
        
        // Update development based on learning
        brain->getDevelopmentSystem()->updateLearningStage(step, predictionError);
    }
    
    // Save learned behavior
    brain->save("advanced_learning.nlm");
    
    std::cout << "Advanced learning demonstration completed" << std::endl;
    std::cout << "Dopamine level: " << dopamine->getLevel() << std::endl;
    std::cout << "Plasticity factor: " << dopamine->getPlasticityFactor() << std::endl;
}
```

### Example 3: Batch Processing with Expert Configuration

```bash
#!/bin/bash
# Advanced batch processing script for NLM

# Expert configuration
EXPERT_CONFIG="""
neuron_count = 3000
region_count = 6
connection_probability = 0.12
enable_simd = true
thread_count = 8
enable_debug_logging = false
enable_metrics = true
metrics_output = "batch_run.prof"

# Advanced plasticity
stdp_ltp_weight = 0.018
stdp_ltd_weight = 0.017
dopamine_baseline = 0.06
synaptogenesis_rate = 0.00015
pruning_rate = 0.000015

# Memory management
working_memory_capacity = 250
replay_interval = 100
consolidation_interval = 1000
"""

# Create expert configuration file
echo "$EXPERT_CONFIG" > expert_config.txt

# Run multiple simulation scenarios
for difficulty in {easy medium hard expert}; do
    echo "Running simulation with $difficulty difficulty..."
    
    # Modify configuration for different difficulties
    case $difficulty in
        easy)
            sed -i 's/connection_probability = 0.12/connection_probability = 0.05/' expert_config.txt
            sed -i 's/synaptogenesis_rate = 0.00015/synaptogenesis_rate = 0.0002/' expert_config.txt
            ;;
        medium)
            sed -i 's/connection_probability = 0.05/connection_probability = 0.12/' expert_config.txt
            sed -i 's/synaptogenesis_rate = 0.0002/synaptogenesis_rate = 0.00015/' expert_config.txt
            ;;
        hard)
            sed -i 's/connection_probability = 0.12/connection_probability = 0.2/' expert_config.txt
            sed -i 's/synaptogenesis_rate = 0.00015/synaptogenesis_rate = 0.0001/' expert_config.txt
            ;;
        expert)
            sed -i 's/connection_probability = 0.2/connection_probability = 0.08/' expert_config.txt
            sed -i 's/synaptogenesis_rate = 0.0001/synaptogenesis_rate = 0.00005/' expert_config.txt
            ;;
    esac
    
    # Run NLM simulation
    ./nlm --config=expert_config.txt --run-id=$difficulty --output-dir=results
    
    # Collect and analyze results
    echo "Collecting results for $difficulty scenario..."
    ./analyze_results.sh results/$difficulty
    
    # Cleanup
    rm -rf results/$difficulty
    
done

echo "Batch processing completed successfully"
```

## Conclusion

Advanced NLM usage requires careful configuration and understanding of the underlying neural mechanisms. The expert features provided in this guide enable users to:

1. **Maximize Performance**: Use SIMD, parallel processing, and memory optimization
2. **Fine-Tune Learning**: Configure advanced neuromodulation and plasticity rules
3. **Automate Processes**: Use expert scripting for batch processing and automation
4. **Debug Effectively**: Use comprehensive logging and troubleshooting tools
5. **Scale Systems**: Configure large-scale simulations with hundreds or thousands of neurons

The advanced features in NLM provide a powerful platform for research and experimentation with artificial developmental brains, enabling users to explore complex learning dynamics and neural behaviors at a level not possible with basic configuration options.

For the most advanced use cases, combining expert configuration with script automation can create highly sophisticated neural simulation systems capable of real-time learning and adaptation.

For support with advanced features, visit the NLM documentation or contact the development team with specific configuration requirements.