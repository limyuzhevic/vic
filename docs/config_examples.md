# NLM Configuration Examples and Optimization Scenarios

## Overview

This guide provides comprehensive examples for configuring NLM for various use cases, performance optimization scenarios, and best practices for achieving the desired neural network behavior. It covers configuration schemas, optimization techniques, and practical examples for research and production environments.

## Configuration Overview

### Configuration Schema

NLM uses a hierarchical key-value configuration system with the following structure:

```
Configuration Keys:
├── brain                    # Brain configuration
│   ├── neuron_count         # Number of neurons
│   ├── synapse_density      # Synapse connectivity
│   ├── v_thresh            # Neuron threshold
│   ├── v_rest              # Resting potential
│   └── development         # Development settings
├── plasticity               # Plasticity rules
│   ├── stdp                # Spike-timing dependent plasticity
│   ├── hebbian            # Hebbian learning
│   └── structural         # Structural plasticity
├── neuromodulation           # Neuromodulation systems
│   ├── dopamine           # Reward signaling
│   ├── curiosity          # Exploration motivation
│   └── novelty            # Novelty detection
├── memory                   # Memory systems
│   ├── working             # Working memory
│   ├── episodic           # Episodic memory
│   └── associative        # Associative memory
├── prediction                # Prediction systems
│   └── neural              # Neural prediction
├── environment               # Environment interface
│   ├── world              # World simulation
│   └── sensory            # Sensory processing
├── performance               # Performance optimization
│   ├── simd                # SIMD vectorization
│   └── memory_pools        # Memory management
└── development              # Development processes
    ├── synaptogenesis      # New synapse formation
    ├── pruning            # Synapse elimination
    └── maturation         # Neural maturation
```

## Configuration Examples

### Example 1: Basic Learning Agent

```cpp
// Basic configuration for a learning agent
#include "Config.hpp"

void createBasicLearningAgentConfig(Config& config) {
    // Brain configuration
    config.set<int>("brain.neuron_count", 1000);
    config.set<double>("brain.synapse_density", 0.1);
    config.set<double>("brain.v_thresh", -50.0);
    config.set<double>("brain.v_rest", -70.0);
    config.set<double>("brain.v_reset", -75.0);
    config.set<double>("brain.tau_mem", 20.0);
    config.set<int>("brain.tau_ref", 5);
    
    // Plasticity configuration
    config.set<bool>("plasticity.stdp.enable", true);
    config.set<double>("plasticity.stdp.learning_rate", 0.01);
    config.set<double>("plasticity.stdp.tau_plus", 20.0);
    config.set<double>("plasticity.stdp.tau_minus", 20.0);
    
    config.set<bool>("plasticity.hebbian.enable", true);
    config.set<double>("plasticity.hebbian.learning_rate", 0.001);
    
    config.set<bool>("plasticity.structural.enable", false); // Disabled for basic agent
    
    // Neuromodulation configuration
    config.set<bool>("neuromod.dopamine.scale", true);
    config.set<double>("neuromod.dopamine.scale_factor", 1.0);
    
    config.set<bool>("neuromod.curiosity.enable", true);
    config.set<double>("neuromod.curiosity.exploration_rate", 0.1);
    
    config.set<bool>("neuromod.novelty.enable", true);
    config.set<double>("neuromod.novelty.sensitivity", 0.5);
    
    // Development configuration
    config.set<bool>("development.enable", true);
    config.set<double>("development.critical_period.end", 100.0);
    config.set<double>("development.maturation.rate", 0.01);
    
    // Environment configuration
    config.set<double>("environment.world.width", 20.0);
    config.set<double>("environment.world.height", 20.0);
    config.set<int>("environment.world.resolution", 4);
    
    // Performance configuration
    config.set<bool>("performance.optimization.enable", true);
    config.set<string>("performance.optimization.level", "simd");
    
    std::cout << "Basic learning agent configuration created.\n";
    std::cout << "  Neurons: " << config.get<int>("brain.neuron_count") << "\n";
    std::cout << "  Synapses: " << config.get<int>("brain.synapse_density") * 1000 << "\n";
    std::cout << "  STDP enabled: " << (config.get<bool>("plasticity.stdp.enable") ? "Yes" : "No") << "\n";
    std::cout << "  Neuromodulation enabled: " << (config.get<bool>("neuromod.curiosity.enable") ? "Yes" : "No") << "\n";
}
```

### Example 2: High-Performance Simulation

```cpp
void createHighPerformanceConfig(Config& config) {
    // Brain configuration for high performance
    config.set<int>("brain.neuron_count", 5000);
    config.set<double>("brain.synapse_density", 0.05); // Lower density for performance
    config.set<double>("brain.v_thresh", -45.0);
    config.set<double>("brain.v_rest", -65.0);
    config.set<double>("brain.v_reset", -70.0);
    config.set<double>("brain.tau_mem", 15.0); // Faster dynamics
    config.set<int>("brain.tau_ref", 3);
    
    // Enable performance optimizations
    config.set<bool>("performance.optimization.enable", true);
    config.set<string>("performance.optimization.level", "simd_avx512");
    config.set<bool>("performance.memory_pools.enable", true);
    config.set<size_t>("performance.memory_pools.neuron_capacity", 10000);
    config.set<size_t>("performance.memory_pools.synapse_capacity", 50000);
    
    // Plasticity configuration for performance
    config.set<bool>("plasticity.stdp.enable", true);
    config.set<double>("plasticity.stdp.learning_rate", 0.005); // Lower rate for stability
    config.set<double>("plasticity.stdp.tau_plus", 15.0);
    config.set<double>("plasticity.stdp.tau_minus", 15.0);
    
    config.set<bool>("plasticity.hebbian.enable", true);
    config.set<double>("plasticity.hebbian.learning_rate", 0.0005);
    
    config.set<bool>("plasticity.structural.enable", true);
    config.set<double>("plasticity.structural.synaptogenesis_rate", 0.01);
    config.set<double>("plasticity.structural.pruning_threshold", 0.01);
    
    // Memory system configuration
    config.set<bool>("memory.working.enable", true);
    config.set<double>("memory.working.capacity", 1000.0);
    config.set<double>("memory.working.decay_rate", 0.001);
    
    config.set<bool>("memory.episodic.enable", true);
    config.set<double>("memory.episodic.capacity", 500);
    config.set<double>("memory.episodic.consolidation_rate", 0.01);
    
    config.set<bool>("memory.associative.enable", true);
    config.set<double>("memory.associative.pattern_capacity", 200);
    
    // Development configuration
    config.set<bool>("development.enable", true);
    config.set<double>("development.critical_period.end", 200.0);
    config.set<double>("development.maturation.rate", 0.005);
    config.set<double>("development.pruning.threshold", 0.005);
    
    // Performance tuning
    config.set<int>("performance.thread_pool.size", 4);
    config.set<bool>("performance.concurrent.enabled", true);
    config.set<double>("performance.simulation.timestep", 0.5);
    config.set<bool>("performance.batching.enabled", true);
    config.set<size_t>("performance.batching.size", 100);
    
    std::cout << "High-performance configuration created.\n";
    std::cout << "  Neurons: " << config.get<int>("brain.neuron_count") << "\n";
    std::cout << "  Synapses: " << config.get<int>("brain.synapse_density") * 5000 << "\n";
    std::cout << "  Performance level: SIMD AVX-512\n";
    std::cout << "  Memory pools: Enabled\n";
    std::cout << "  Concurrent processing: Enabled\n";
}
```

### Example 3: Curiosity-Driven Exploration

```cpp
void createCuriosityDrivenConfig(Config& config) {
    // Brain configuration optimized for exploration
    config.set<int>("brain.neuron_count", 2000);
    config.set<double>("brain.synapse_density", 0.15); // Higher density for exploration
    config.set<double>("brain.v_thresh", -55.0);
    config.set<double>("brain.v_rest", -68.0);
    
    // Neuromodulation configuration for curiosity
    config.set<bool>("neuromod.dopamine.scale", true);
    config.set<double>("neuromod.dopamine.scale_factor", 2.0); // Enhanced reward sensitivity
    
    config.set<bool>("neuromod.curiosity.enable", true);
    config.set<double>("neuromod.curiosity.exploration_rate", 0.3); // High exploration
    config.set<double>("neuromod.curiosity.sampling_rate", 0.1); // Frequent exploration
    config.set<double>("neuromod.curiosity.threshold", 0.5); // Lower curiosity threshold
    
    config.set<bool>("neuromod.novelty.enable", true);
    config.set<double>("neuromod.novelty.sensitivity", 1.0); // High novelty detection
    config.set<double>("neuromod.novelty.memory_size", 1000); // Large novelty memory
    
    // Plasticity configuration
    config.set<bool>("plasticity.stdp.enable", true);
    config.set<double>("plasticity.stdp.learning_rate", 0.02); // Faster learning
    config.set<double>("plasticity.stdp.tau_plus", 25.0);
    config.set<double>("plasticity.stdp.tau_minus", 25.0);
    
    config.set<bool>("plasticity.hebbian.enable", true);
    config.set<double>("plasticity.hebbian.learning_rate", 0.002);
    
    // Structural plasticity for exploration
    config.set<bool>("plasticity.structural.enable", true);
    config.set<double>("plasticity.structural.synaptogenesis_rate", 0.05); // High synaptogenesis
    config.set<double>("plasticity.structural.pruning_rate", 0.01);
    
    // Development configuration
    config.set<bool>("development.enable", true);
    config.set<double>("development.critical_period.end", 150.0);
    config.set<double>("development.maturation.rate", 0.008);
    
    // Environment configuration
    config.set<double>("environment.world.width", 30.0);
    config.set<double>("environment.world.height", 30.0);
    config.set<int>("environment.world.resolution", 6); // Higher resolution for exploration
    
    // Performance configuration
    config.set<bool>("performance.optimization.enable", true);
    config.set<string>("performance.optimization.level", "simd");
    config.set<bool>("performance.memory_pools.enable", true);
    
    std::cout << "Curiosity-driven exploration configuration created.\n";
    std::cout << "  Neurons: " << config.get<int>("brain.neuron_count") << "\n";
    std::cout << "  Curiosity rate: " << config.get<double>("neuromod.curiosity.exploration_rate") << "\n";
    std::cout << "  Novelty sensitivity: " << config.get<double>("neuromod.novelty.sensitivity") << "\n";
    std::cout << "  Exploration enabled: " << (config.get<bool>("neuromod.curiosity.enable") ? "Yes" : "No") << "\n";
}
```

### Example 4: Predictive Control System

```cpp
void createPredictiveControlConfig(Config& config) {
    // Brain configuration for predictive control
    config.set<int>("brain.neuron_count", 3000);
    config.set<double>("brain.synapse_density", 0.12);
    config.set<double>("brain.v_thresh", -52.0);
    config.set<double>("brain.v_rest", -67.0);
    
    // Prediction system configuration
    config.set<bool>("prediction.neural.enable", true);
    config.set<double>("prediction.neural.prediction_horizon", 10.0); // Predict 10 steps ahead
    config.set<double>("prediction.neural.prediction_accuracy", 0.9);
    config.set<double>("prediction.neural.confidence_threshold", 0.7);
    
    // Action consequence prediction
    config.set<bool>("prediction.action.enable", true);
    config.set<double>("prediction.action.prediction_window", 5.0);
    config.set<double>("prediction.action.outcome_sampling", 0.3);
    
    // Neuromodulation for predictive control
    config.set<bool>("neuromod.dopamine.scale", true);
    config.set<double>("neuromod.dopamine.scale_factor", 1.5);
    
    config.set<bool>("neuromod.prediction_error.enable", true);
    config.set<double>("neuromod.prediction_error.sensitivity", 0.8);
    
    // Plasticity for predictive learning
    config.set<bool>("plasticity.stdp.enable", true);
    config.set<double>("plasticity.stdp.learning_rate", 0.015);
    config.set<double>("plasticity.stdp.tau_plus", 20.0);
    config.set<double>("plasticity.stdp.tau_minus", 20.0);
    
    // Memory system for prediction
    config.set<bool>"memory.working.enable", true);
    config.set<double>"memory.working.capacity", 2000.0;
    config.set<double>"memory.working.decay_rate", 0.002;
    
    config.set<bool>"memory.associative.enable", true;
    config.set<double>"memory.associative.pattern_capacity", 500;
    config.set<double>"memory.associative.similarity_threshold", 0.8;
    
    // Development for predictive capabilities
    config.set<bool>"development.enable", true;
    config.set<double>"development.critical_period.end", 120.0;
    config.set<double>"development.maturation.rate", 0.006;
    
    // Performance optimization
    config.set<bool>"performance.optimization.enable", true;
    config.set<string>"performance.optimization.level", "simd_avx2";
    config.set<bool>"performance.batching.enabled", true;
    config.set<size_t>"performance.batching.size", 50;
    
    std.out << "Predictive control configuration created.\n";
    std.out << "  Neurons: " << config.get<int>"brain.neuron_count" << "\n";
    std.out << "  Prediction horizon: " << config.get<double>"prediction.neural.prediction_horizon" << " steps\n";
    std.out << "  Action prediction enabled: " << (config.get<bool>"prediction.action.enable" ? "Yes" : "No") << "\n";
    std.out << "  Prediction error sensitivity: " << config.get<double>"neuromod.prediction_error.sensitivity" << "\n";
}
```

### Example 5: Energy-Constrained Embedded System

```cpp
void createEnergyConstrainedConfig(Config& config) {
    // Low-power brain configuration
    config.set<int>"brain.neuron_count", 500; // Small network for energy efficiency
    config.set<double>"brain.synapse_density", 0.08;
    config.set<double>"brain.v_thresh", -48.0;
    config.set<double>"brain.v_rest", -65.0;
    config.set<double>"brain.tau_mem", 25.0; // Slower dynamics for energy savings
    config.set<int>"brain.tau_ref", 8);
    
    // Disable computationally expensive features
    config.set<bool>"plasticity.stdp.enable", true; // Enable but with low rate
    config.set<double>"plasticity.stdp.learning_rate", 0.001;
    config.set<double>"plasticity.stdp.tau_plus", 30.0;
    config.set<double>"plasticity.stdp.tau_minus", 30.0);
    
    config.set<bool>"plasticity.hebbian.enable", false); // Disable for energy savings
    config.set<bool>"plasticity.structural.enable", false); // Disable for energy savings
    
    // Minimal neuromodulation
    config.set<bool>"neuromod.dopamine.scale", true;
    config.set<double>"neuromod.dopamine.scale_factor", 0.5); // Low sensitivity
    
    config.set<bool>"neuromod.curiosity.enable", false); // Disable for energy savings
    config.set<bool>"neuromod.novelty.enable", false); // Disable for energy savings
    
    // Limited memory systems
    config.set<bool>"memory.working.enable", true;
    config.set<double>"memory.working.capacity", 100.0); // Small capacity
    config.set<double>"memory.working.decay_rate", 0.01); // Fast decay
    
    config.set<bool>"memory.episodic.enable", false); // Disable for energy savings
    config.set<bool>"memory.associative.enable", false); // Disable for energy savings
    
    // Aggressive performance optimization
    config.set<bool>"performance.optimization.enable", true;
    config.set<string>"performance.optimization.level", "scalar"); // Disable SIMD for compatibility
    config.set<bool>"performance.memory_pools.enable", true;
    config.set<bool>"performance.concurrent.enabled", false); // Disable multi-threading
    config.set<bool>"performance.batching.enabled", true;
    
    // Slow development
    config.set<bool>"development.enable", true;
    config.set<double>"development.critical_period.end", 300.0); // Extended critical period
    config.set<double>"development.maturation.rate", 0.001); // Very slow maturation
    
    // Limited environment
    config.set<double>"environment.world.width", 10.0);
    config.set<double>"environment.world.height", 10.0);
    config.set<int>"environment.world.resolution", 2);
    
    std.out << "Energy-constrained configuration created.\n";
    std.out << "  Neurons: " << config.get<int>"brain.neuron_count" << "\n";
    std.out << "  Plasticity: Enabled (low rate)\n";
    std.out << "  Neuromodulation: Minimal\n";
    std.out << "  Memory: Limited\n";
    std.out << "  Development: Slow\n";
}
```

## Optimization Scenarios

### Scenario 1: Fast-Response System

```cpp
void optimizeForFastResponse(Config& config) {
    // Optimize configuration for fast response time
    
    // Brain configuration
    config.set<int>"brain.neuron_count", 1000;
    config.set<double>"brain.synapse_density", 0.2); // High connectivity for fast response
    config.set<double>"brain.v_thresh", -45.0); // Lower threshold for faster firing
    config.set<double>"brain.tau_mem", 10.0); // Fast membrane time constant
    config.set<int>"brain.tau_ref", 2); // Short refractory period
    
    // Performance optimization
    config.set<bool>"performance.optimization.enable", true;
    config.set<string>"performance.optimization.level", "simd");
    config.set<bool>"performance.batching.enabled", true;
    config.set<size_t>"performance.batching.size", 200);
    config.set<bool>"performance.concurrent.enabled", true;
    config.set<int>"performance.thread_pool.size", 8);
    
    // Minimize plasticity delays
    config.set<bool>"plasticity.stdp.enable", true;
    config.set<double>"plasticity.stdp.learning_rate", 0.005);
    config.set<double>"plasticity.stdp.tau_plus", 10.0);
    config.set<double>"plasticity.stdp.tau_minus", 10.0);
    
    config.set<bool>"plasticity.hebbian.enable", true;
    config.set<double>"plasticity.hebbian.learning_rate", 0.001);
    
    // Disable slow features
    config.set<bool>"plasticity.structural.enable", false);
    config.set<bool>"neuromod.curiosity.enable", false);
    config.set<bool>"neuromod.novelty.enable", false);
    
    // Lightweight development
    config.set<bool>"development.enable", true;
    config.set<double>"development.maturation.rate", 0.02); // Fast development
    
    std.out << "Fast-response configuration optimized.\n";
    std.out << "  Focus: Speed over accuracy\n";
    std.out << "  Response time: < 1ms per step\n";
    std.out << "  Trade-off: Reduced memory capacity\n";
}
```

### Scenario 2: High-Precision System

```cpp
void optimizeForHighPrecision(Config& config) {
    // Optimize configuration for high-precision control
    
    // Brain configuration
    config.set<int>"brain.neuron_count", 3000;
    config.set<double>"brain.synapse_density", 0.08); // Sparse connectivity for precision
    config.set<double>"brain.v_thresh", -52.0); // Precise threshold
    config.set<double>"brain.v_rest", -67.0);
    config.set<double>"brain.tau_mem", 30.0); // Stable dynamics for precision
    config.set<int>"brain.tau_ref", 10);
    
    // High-precision plasticity
    config.set<bool>"plasticity.stdp.enable", true;
    config.set<double>"plasticity.stdp.learning_rate", 0.0001); // Very low rate for precision
    config.set<double>"plasticity.stdp.tau_plus", 50.0); // Long time constants
    config.set<double>"plasticity.stdp.tau_minus", 50.0);
    
    config.set<bool>"plasticity.hebbian.enable", true;
    config.set<double>"plasticity.hebbian.learning_rate", 0.00005);
    
    // Enable all plasticity for learning
    config.set<bool>"plasticity.structural.enable", true;
    config.set<double>"plasticity.structural.synaptogenesis_rate", 0.005);
    config.set<double>"plasticity.structural.pruning_threshold", 0.001);
    
    // Enhanced neuromodulation
    config.set<bool>"neuromod.dopamine.scale", true;
    config.set<double>"neuromod.dopamine.scale_factor", 1.0);
    
    config.set<bool>"neuromod.curiosity.enable", true;
    config.set<double>"neuromod.curiosity.exploration_rate", 0.05); // Balanced exploration
    
    config.set<bool>"neuromod.novelty.enable", true;
    config.set<double>"neuromod.novelty.sensitivity", 0.5);
    
    // Comprehensive memory systems
    config.set<bool>"memory.working.enable", true;
    config.set<double>"memory.working.capacity", 5000.0);
    config.set<double>"memory.working.decay_rate", 0.0001); // Very slow decay
    
    config.set<bool>"memory.episodic.enable", true;
    config.set<double>"memory.episodic.capacity", 2000);
    config.set<double>"memory.episodic.consolidation_rate", 0.005);
    
    config.set<bool>"memory.associative.enable", true;
    config.set<double>"memory.associative.pattern_capacity", 1000);
    config.set<double>"memory.associative.similarity_threshold", 0.9); // High similarity
    
    // Precise development
    config.set<bool>"development.enable", true;
    config.set<double>"development.critical_period.end", 100.0);
    config.set<double>"development.maturation.rate", 0.001); // Very slow maturation
    
    // Maximum performance optimization
    config.set<bool>"performance.optimization.enable", true;
    config.set<string>"performance.optimization.level", "simd_avx512");
    config.set<bool>"performance.memory_pools.enable", true;
    config.set<bool>"performance.concurrent.enabled", true;
    config.set<int>"performance.thread_pool.size", 16);
    config.set<bool>"performance.batching.enabled", true;
    config.set<size_t>"performance.batching.size", 500);
    
    std.out << "High-precision configuration optimized.\n";
    std.out << "  Focus: Accuracy over speed\n";
    std.out << "  Memory: Extensive\n";
    std.out << "  Learning: Comprehensive\n";
}
```

### Scenario 3: Memory-Efficient System

```cpp
void optimizeForMemoryEfficiency(Config& config) {
    // Optimize configuration for minimal memory usage
    
    // Compact brain configuration
    config.set<int>"brain.neuron_count", 500;
    config.set<double>"brain.synapse_density", 0.05); // Very sparse
    config.set<double>"brain.v_thresh", -50.0);
    config.set<double>"brain.v_rest", -70.0);
    
    // Minimal plasticity (memory-hungry)
    config.set<bool>"plasticity.stdp.enable", true;
    config.set<double>"plasticity.stdp.learning_rate", 0.001);
    config.set<double>"plasticity.stdp.tau_plus", 20.0);
    config.set<double>"plasticity.stdp.tau_minus", 20.0);
    
    config.set<bool>"plasticity.hebbian.enable", false); // Disabled
    config.set<bool>"plasticity.structural.enable", false); // Disabled
    
    // Basic neuromodulation
    config.set<bool>"neuromod.dopamine.scale", false); // Disabled
    config.set<bool>"neuromod.curiosity.enable", false); // Disabled
    config.set<bool>"neuromod.novelty.enable", false); // Disabled
    
    // Minimal memory systems
    config.set<bool>"memory.working.enable", true;
    config.set<double>"memory.working.capacity", 200.0); // Very small
    config.set<double>"memory.working.decay_rate", 0.01); // Fast decay
    
    config.set<bool>"memory.episodic.enable", false); // Disabled
    config.set<bool>"memory.associative.enable", false); // Disabled
    
    // Basic development
    config.set<bool>"development.enable", false); // Disabled for memory savings
    
    // Conservative performance
    config.set<bool>"performance.optimization.enable", true;
    config.set<string>"performance.optimization.level", "scalar"); // No SIMD
    config.set<bool>"performance.memory_pools.enable", true;
    config.set<bool>"performance.concurrent.enabled", false); // No threading
    config.set<bool>"performance.batching.enabled", false); // No batching
    
    // Small environment
    config.set<double>"environment.world.width", 5.0);
    config.set<double>"environment.world.height", 5.0);
    config.set<int>"environment.world.resolution", 1);
    
    std.out << "Memory-efficient configuration optimized.\n";
    std.out << "  Focus: Minimal memory footprint\n";
    std.out << "  Memory: < 10MB\n";
    std.out << "  Trade-off: Reduced functionality\n";
}
```

### Scenario 4: Learning-Heavy System

```cpp
void optimizeForHeavyLearning(Config& config) {
    // Optimize configuration for maximum learning capability
    
    // Large brain configuration
    config.set<int>"brain.neuron_count", 4000;
    config.set<double>"brain.synapse_density", 0.12);
    config.set<double>"brain.v_thresh", -48.0);
    config.set<double>"brain.v_rest", -65.0);
    
    // Maximum plasticity
    config.set<bool>"plasticity.stdp.enable", true;
    config.set<double>"plasticity.stdp.learning_rate", 0.03); // High learning rate
    config.set<double>"plasticity.stdp.tau_plus", 30.0);
    config.set<double>"plasticity.stdp.tau_minus", 30.0);
    
    config.set<bool>"plasticity.hebbian.enable", true;
    config.set<double>"plasticity.hebbian.learning_rate", 0.005);
    
    // Enable all structural plasticity
    config.set<bool>"plasticity.structural.enable", true;
    config.set<double>"plasticity.structural.synaptogenesis_rate", 0.08);
    config.set<double>"plasticity.structural.pruning_rate", 0.02);
    
    // Full neuromodulation
    config.set<bool>"neuromod.dopamine.scale", true;
    config.set<double>"neuromod.dopamine.scale_factor", 2.0);
    
    config.set<bool>"neuromod.curiosity.enable", true;
    config.set<double>"neuromod.curiosity.exploration_rate", 0.4);
    
    config.set<bool>"neuromod.novelty.enable", true;
    config.set<double>"neuromod.novelty.sensitivity", 1.0);
    
    // Comprehensive memory systems
    config.set<bool>"memory.working.enable", true;
    config.set<double>"memory.working.capacity", 10000.0);
    config.set<double>"memory.working.decay_rate", 0.0005);
    
    config.set<bool>"memory.episodic.enable", true;
    config.set<double>"memory.episodic.capacity", 5000);
    config.set<double>"memory.episodic.consolidation_rate", 0.02);
    
    config.set<bool>"memory.associative.enable", true;
    config.set<double>"memory.associative.pattern_capacity", 3000);
    config.set<double>"memory.associative.similarity_threshold", 0.8);
    
    // Extended development
    config.set<bool>"development.enable", true;
    config.set<double>"development.critical_period.end", 200.0);
    config.set<double>"development.maturation.rate", 0.01);
    
    // Maximum performance
    config.set<bool>"performance.optimization.enable", true;
    config.set<string>"performance.optimization.level", "simd_avx512");
    config.set<bool>"performance.memory_pools.enable", true;
    config.set<bool>"performance.concurrent.enabled", true);
    config.set<int>"performance.thread_pool.size", 32);
    config.set<bool>"performance.batching.enabled", true;
    config.set<size_t>"performance.batching.size", 1000);
    
    std.out << "Learning-heavy configuration optimized.\n";
    std.out << "  Focus: Maximum learning capability\n";
    std.out << "  Neurons: 4000\n";
    std.out << "  Synapses: 960,000\n";
    std.out << "  Plasticity: Full spectrum\n";
}
```

## Configuration Validation

### Validate Configuration

```cpp
void validateConfiguration(const Config& config) {
    std::vector<string> errors;
    std::vector<string> warnings;
    
    // Validate brain configuration
    validateBrainConfig(config, errors, warnings);
    
    // Validate plasticity configuration
    validatePlasticityConfig(config, errors, warnings);
    
    // Validate neuromodulation configuration
    validateNeuromodulationConfig(config, errors, warnings);
    
    // Validate memory configuration
    validateMemoryConfig(config, errors, warnings);
    
    // Validate performance configuration
    validatePerformanceConfig(config, errors, warnings);
    
    // Report results
    reportValidationResults(errors, warnings);
}
```

### Brain Configuration Validation

```cpp
void validateBrainConfig(const Config& config, std::vector<string>& errors,
                         std::vector<string>& warnings) {
    // Check neuron count bounds
    int neuronCount = config.get<int>"brain.neuron_count";
    if (neuronCount < 10 || neuronCount > 100000) {
        errors.push_back("brain.neuron_count must be between 10 and 100,000");
    }
    
    // Check synapse density
    double synapseDensity = config.get<double>"brain.synapse_density";
    if (synapseDensity < 0.0 || synapseDensity > 1.0) {
        errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
    }
    
    // Check potential values
    double vThresh = config.get<double>"brain.v_thresh";
    double vRest = config.get<double>"brain.v_rest";
    if (vThresh <= vRest) {
        errors.push_back("brain.v_thresh must be greater than brain.v_rest");
    }
    
    // Warn about large networks
    if (neuronCount > 10000) {
        warnings.push_back("Large neural network may require significant memory and computation");
    }
}
```

### Plasticity Configuration Validation

```cpp
void validatePlasticityConfig(const Config& config, std::vector<string>& errors,
                              std::vector<string>& warnings) {
    // Validate STDP configuration
    if (config.get<bool>"plasticity.stdp.enable") {
        double stdpLearningRate = config.get<double>"plasticity.stdp.learning_rate";
        if (stdpLearningRate <= 0.0 || stdpLearningRate > 1.0) {
            errors.push_back("plasticity.stdp.learning_rate must be between 0 and 1");
        }
        
        double stdpTau = config.get<double>"plasticity.stdp.tau_plus";
        if (stdpTau <= 0.0) {
            errors.push_back("plasticity.stdp.tau_plus must be positive");
        }
    }
    
    // Validate Hebbian configuration
    if (config.get<bool>"plasticity.hebbian.enable") {
        double hebbianLearningRate = config.get<double>"plasticity.hebbian.learning_rate";
        if (hebbianLearningRate <= 0.0) {
            errors.push_back("plasticity.hebbian.learning_rate must be positive");
        }
    }
    
    // Warn about conflicting plasticity
    bool stdpEnabled = config.get<bool>"plasticity.stdp.enable";
    bool hebbianEnabled = config.get<bool>"plasticity.hebbian.enable";
    bool structuralEnabled = config.get<bool>"plasticity.structural.enable";
    
    if (stdpEnabled && hebbianEnabled && structuralEnabled) {
        warnings.push_back("All plasticity mechanisms enabled. Consider performance impact.");
    }
}
```

## Configuration Management

### Configuration Templates

```cpp
namespace ConfigTemplates {
    
    // Standard template for basic research
    Config getBasicResearchTemplate() {
        Config config;
        
        // Standard brain configuration
        config.set<int>"brain.neuron_count", 1000);
        config.set<double>"brain.synapse_density", 0.1);
        config.set<double>"brain.v_thresh", -50.0);
        config.set<double>"brain.v_rest", -70.0);
        
        // Basic plasticity
        config.set<bool>"plasticity.stdp.enable", true);
        config.set<double>"plasticity.stdp.learning_rate", 0.01);
        
        config.set<bool>"plasticity.hebbian.enable", true);
        config.set<double>"plasticity.hebbian.learning_rate", 0.001);
        
        // Essential neuromodulation
        config.set<bool>"neuromod.dopamine.scale", true);
        config.set<bool>"neuromod.curiosity.enable", true);
        
        // Basic development
        config.set<bool>"development.enable", true);
        
        return config;
    }
    
    // Production template
    Config getProductionTemplate() {
        Config config;
        
        // Optimized brain configuration
        config.set<int>"brain.neuron_count", 2000);
        config.set<double>"brain.synapse_density", 0.08);
        
        // Performance optimizations
        config.set<bool>"performance.optimization.enable", true);
        config.set<string>"performance.optimization.level", "simd");
        config.set<bool>"performance.memory_pools.enable", true);
        
        // Essential plasticity
        config.set<bool>"plasticity.stdp.enable", true);
        config.set<double>"plasticity.stdp.learning_rate", 0.005);
        
        return config;
    }
    
    // Research template (maximum flexibility)
    Config getResearchTemplate() {
        Config config;
        
        // Large brain for research
        config.set<int>"brain.neuron_count", 5000);
        config.set<double>"brain.synapse_density", 0.15);
        
        // Full plasticity spectrum
        config.set<bool>"plasticity.stdp.enable", true);
        config.set<double>"plasticity.stdp.learning_rate", 0.02);
        
        config.set<bool>"plasticity.hebbian.enable", true);
        config.set<double>"plasticity.hebbian.learning_rate", 0.002);
        
        config.set<bool>"plasticity.structural.enable", true);
        config.set<double>"plasticity.structural.synaptogenesis_rate", 0.05);
        
        // Full neuromodulation
        config.set<bool>"neuromod.dopamine.scale", true);
        config.set<bool>"neuromod.curiosity.enable", true);
        config.set<bool>"neuromod.novelty.enable", true);
        
        // Full memory systems
        config.set<bool>"memory.working.enable", true);
        config.set<bool>"memory.episodic.enable", true);
        config.set<bool>"memory.associative.enable", true);
        
        // Full development
        config.set<bool>"development.enable", true);
        
        return config;
    }
}
```

## Configuration Examples Summary

| Use Case | Neurons | Synapses | Focus | Trade-offs |
|----------|---------|----------|-------|------------|
| Basic Learning | 1,000 | ~100 | Balance | Limited scalability |
| High Performance | 5,000 | ~400 | Speed | High memory usage |
| Curiosity-Driven | 2,000 | ~240 | Exploration | Slower response |
| Predictive Control | 3,000 | ~360 | Accuracy | Complex computation |
| Energy-Constrained | 500 | ~40 | Energy efficiency | Limited functionality |
| Fast-Response | 1,000 | ~200 | Speed | Reduced memory |
| High-Precision | 3,000 | ~240 | Accuracy | Slower processing |
| Memory-Efficient | 500 | ~40 | Minimal memory | Limited capabilities |
| Learning-Heavy | 4,000 | ~960 | Maximum learning | High resource usage |

## Conclusion

NLM configuration provides extensive flexibility to tailor neural network behavior to specific application requirements. The examples and optimization scenarios demonstrate how to:

1. **Balance System Resources**: Choose appropriate neuron and synapse counts
2. **Optimize Performance**: Configure plasticity, neuromodulation, and memory systems
3. **Focus on Specific Behaviors**: Tailor configuration for learning, exploration, prediction, or control
4. **Manage Resource Constraints**: Balance functionality with memory and computational limits
5. **Validate Configuration**: Ensure settings are consistent and within valid ranges

The configuration system enables researchers and developers to create customized neural networks that meet their specific requirements while maintaining the biological plausibility and learning capabilities of the NLM framework.