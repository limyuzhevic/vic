# NLM Best Practices and Performance Tuning Guide

## Overview

This guide provides comprehensive best practices and performance tuning recommendations for the NLM (Neural Learning Machine) system, with specific emphasis on Phase 6 integration capabilities. It covers configuration optimization, memory management, simulation tuning, and debugging strategies to help users get the most out of their NLM brain simulations.

## Table of Contents

1. [Introduction and Scope](#introduction-and-scope)
2. [Configuration Best Practices](#configuration-best-practices)
3. [Memory Management Best Practices](#memory-management-best-practices)
4. [Performance Optimization Strategies](#performance-optimization-strategies)
5. [Phase 6 Integration Best Practices](#phase-6-integration-best-practices)
6. [Debugging and Monitoring](#debugging-and-monitoring)
7. [Simulation Management](#simulation-management)
8. [Advanced Tuning](#advanced-tuning)
9. [Troubleshooting Guide](#troubleshooting-guide)
10. [Performance Benchmarking](#performance-benchmarking)

---

## Introduction and Scope

### What This Guide Covers

This guide provides practical, actionable advice for optimizing NLM performance across three dimensions:

1. **Configuration Management**: Optimal settings for different use cases
2. **Memory Efficiency**: Managing Phase 6 memory systems for performance
3. **Computational Performance**: Simulation speed and resource utilization
4. **Integration Optimization**: Getting the most from Phase 6 connected systems

### What This Guide Doesn't Cover

- Detailed mathematical derivations of neural algorithms
- Step-by-step code examples for basic functionality
- Installation and setup instructions
- Theory of neural computation (see `SCIENCE.md` for this)

### Audience

This guide is intended for:
- **Researchers** running complex Phase 6 simulations
- **Engineers** optimizing performance for specific applications
- **Developers** integrating NLM into larger systems
- **Users** seeking to maximize Phase 6 capabilities while managing computational costs

---

## Configuration Best Practices

### Configuration Management Principles

#### **1. Use Phase 6 Optimizations by Default**

For Phase 6 simulations, use the following baseline configuration:

```python
import pynlm

# Phase 6 optimized configuration
config = pynlm.createDefaultConfig()

# Memory system optimizations
config.set("replay_interval", 100)        # Replay every 100 steps
config.set("consolidation_interval", 1000) # Consolidate every 1000 steps
config.set("working_memory_capacity", 100)  # Working memory capacity

# Plasticity optimizations
config.set("synaptogenesis_rate", 0.0001)  # Controlled structural growth
config.set("pruning_rate", 0.00001)       # Balanced pruning
config.set("plasticity.stdp.learning_rate", 0.001)  # Standard STDP

# Neuromodulation optimizations
config.set("neuromod.dopamine.scale", 1.0) # Full dopamine effect
config.set("neuromod.curiosity.enable", True) # Curiosity enabled

# Simulation optimizations
config.set("simulation_timestep", 0.001)  # Fine time resolution
config.set("random_seed", 42)            # Reproducible results
```

#### **2. Task-Specific Configurations**

**For Learning Research Tasks**:
```python
config.set("memory.replay_interval", 50)           # Frequent replay for learning
config.set("memory.consolidation_interval", 500)   # Frequent consolidation
config.set("prediction.learning_rate", 0.01)      # Fast prediction learning
config.set("cognition.concept_learning_rate", 0.001) # Slower concept formation
config.set("neuromod.curiosity.enable", True)       # Enable exploration
```

**For Behavior Control Tasks**:
```python
config.set("memory.replay_interval", 200)          # Less frequent replay
config.set("prediction.learning_rate", 0.001)     # Slower prediction
config.set("cognition.planning_horizon", 2.0)      # Short-term planning
config.set("neuromod.curiosity.enable", False)     # Focus on exploitation
```

**For Memory Retention Studies**:
```python
config.set("memory.replay_interval", 1000)         # Infrequent replay
config.set("memory.consolidation_interval", 5000)  # Long consolidation
config.set("working_memory.capacity", 500)        # Large working memory
config.set("episodic_memory.window", 100000)       # Long memory window
```

### Configuration File Management

#### **Best Practices for Configuration Files**:

1. **Use JSON for Human-Readable Configuration**
```json
{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.15
    },
    "memory": {
        "replay_interval": 100,
        "consolidation_interval": 1000,
        "working_memory_capacity": 150
    },
    "plasticity": {
        "stdp": {
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        }
    }
}
```

2. **Environment Variables for Experiment Control**
```bash
# Set experiment parameters via environment
export NLM_REPLAY_INTERVAL=100
export NLM_CONSOLIDATION_INTERVAL=1000
export NLM_WORKING_MEMORY_CAPACITY=200
export NLM_PREDICTION_LEARNING_RATE=0.01

# Then in your Python code
import os
replay_interval = int(os.getenv('NLM_REPLAY_INTERVAL', '100'))
consolidation_interval = int(os.getenv('NLM_CONSOLIDATION_INTERVAL', '1000'))
```

3. **Configuration Versioning**
```python
# Create configuration with versioning
class ConfigurationManager:
    def __init__(self, base_config_path):
        self.base_config_path = base_config_path
        self.config_version = "Phase6.1"
        self.experiment_id = f"exp_{int(time.time())}"
    
    def create_experiment_config(self, experiment_name):
        config = pynlm.createDefaultConfig()
        
        # Load base configuration
        config.loadFromFile(f"{self.base_config_path}/base.json")
        
        # Apply experiment-specific modifications
        if experiment_name == "memory_research":
            self.apply_memory_research_config(config)
        elif experiment_name == "planning_research":
            self.apply_planning_research_config(config)
        elif experiment_name == "curiosity_study":
            self.apply_curiosity_study_config(config)
        
        # Add experiment metadata
        config.set("experiment.name", experiment_name)
        config.set("experiment.id", self.experiment_id)
        config.set("experiment.version", self.config_version)
        config.set("experiment.timestamp", time.time())
        
        return config
    
    def apply_memory_research_config(self, config):
        config.set("memory.replay_interval", 50)
        config.set("memory.consolidation_interval", 500)
        config.set("working_memory.capacity", 200)
        config.set("prediction.learning_rate", 0.02)
```

### Configuration Validation

#### **Validate Configuration Before Use**:

```python
def validate_phase6_config(config):
    """Validate that configuration meets Phase 6 requirements."""
    issues = []
    
    # Check memory system configuration
    replay_interval = config.getOr("memory.replay_interval", 100)
    if replay_interval < 10 or replay_interval > 1000:
        issues.append(f"replay_interval ({replay_interval}) should be between 10 and 1000")
    
    consolidation_interval = config.getOr("memory.consolidation_interval", 1000)
    if consolidation_interval < 100 or consolidation_interval > 10000:
        issues.append(f"consolidation_interval ({consolidation_interval}) should be between 100 and 10000")
    
    # Check neural system configuration
    neuron_count = config.getOr("brain.neuron_count", 1000)
    if neuron_count < 100 or neuron_count > 100000:
        issues.append(f"neuron_count ({neuron_count}) should be between 100 and 100000")
    
    # Check plasticity configuration
    stdp_rate = config.getOr("plasticity.stdp.learning_rate", 0.001)
    if stdp_rate <= 0 or stdp_rate > 0.1:
        issues.append(f"stdp.learning_rate ({stddp_rate}) should be between 0 and 0.1")
    
    if issues:
        print("Configuration validation issues:")
        for issue in issues:
            print(f"  - {issue}")
        return False
    else:
        print("Configuration validation passed!")
        return True

# Usage example
if __name__ == "__main__":
    config = pynlm.createDefaultConfig()
    config.set("memory.replay_interval", 100)
    config.set("brain.neuron_count", 2000)
    
    if validate_phase6_config(config):
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("Phase 6 simulation initialized successfully!")
    else:
        print("Please fix configuration issues before running simulation.")
```

---

## Memory Management Best Practices

### Working Memory Management

#### **1. Capacity Planning**

**Working Memory Capacity Guidelines**:
```python
# Memory capacity based on neuron count and task complexity
# Formula: capacity = max(50, min(1000, neuron_count * 0.1))

def calculate_working_memory_capacity(neuron_count, task_complexity="standard"):
    base_capacity = max(50, min(1000, neuron_count * 0.1))
    
    # Adjust for task complexity
    if task_complexity == "high":
        capacity = int(base_capacity * 1.5)
    elif task_complexity == "low":
        capacity = int(base_capacity * 0.7)
    else:
        capacity = base_capacity
    
    # Ensure reasonable limits
    capacity = max(10, min(2000, capacity))
    return capacity

# Usage examples
working_memory_capacity = calculate_working_memory_capacity(1000, "standard")  # 100
working_memory_capacity = calculate_working_memory_capacity(5000, "high")      # 750
working_memory_capacity = calculate_working_memory_capacity(1000, "low")       # 70
```

#### **2. Decay Rate Optimization**
```python
# Working memory decay based on task requirements
# Formula: decay_rate = base_decay * complexity_factor

def calculate_working_memory_decay(task_complexity="standard", simulation_timestep=0.001):
    base_decay_rate = 0.1  # 10% decay per timestep
    
    # Adjust for task complexity
    if task_complexity == "high":
        decay_factor = 0.8  # Slower decay for complex tasks
    elif task_complexity == "low":
        decay_factor = 1.5  # Faster decay for simple tasks
    else:
        decay_factor = 1.0  # Standard decay
    
    decay_rate = base_decay_rate * decay_factor * simulation_timestep
    return min(0.5, max(0.01, decay_rate))  # Ensure reasonable limits

# Configure working memory decay in simulation
working_memory_decay = calculate_working_memory_decay("high")
brain.getWorkingMemory().setDecayRate(working_memory_decay)
```

#### **3. Memory Utilization Monitoring**
```python
# Monitor and log working memory utilization

def monitor_working_memory(brain, step_interval=100):
    working_mem = brain.getWorkingMemory()
    if working_mem:
        capacity = working_mem.getCapacity()
        current_size = working_mem.getActiveTraces()
        utilization = current_size / capacity if capacity > 0 else 0.0
        activity_level = working_mem.getMemoryActivity()
        
        if step_interval > 0 and step % step_interval == 0:
            print(f"Working Memory: {current_size}/{capacity} traces "
                  f"({utilization:.1%} utilized, activity: {activity_level:.3f})")
        
        return {
            'capacity': capacity,
            'current_size': current_size,
            'utilization': utilization,
            'activity_level': activity_level
        }
    
    return None

# Usage in simulation loop
for step in range(num_steps):
    brain.step(step)
    
    # Monitor working memory periodically
    if step % 100 == 0:
        mem_stats = monitor_working_memory(brain)
        if mem_stats:
            # Adjust parameters based on utilization
            if mem_stats['utilization'] > 0.9:
                print("Warning: Working memory near capacity, consider reducing capacity or increasing decay rate")
            elif mem_stats['utilization'] < 0.1:
                print("Info: Working memory underutilized, could increase capacity for better performance")
```

### Episodic Memory Management

#### **1. Memory Window Management**
```python
# Configure episodic memory retention window

def configure_episodic_memory_window(brain, window_size="standard"):
    episodic_mem = brain.getEpisodicMemory()
    if episodic_mem:
        if window_size == "short":
            # Short-term memory focus
            episodic_mem.setRetentionWindow(1000)  # 1000 steps
            episodic_mem.setConsolidationThreshold(0.8)  # Quick consolidation
        elif window_size == "long":
            # Long-term memory focus
            episodic_mem.setRetentionWindow(100000)  # 100000 steps
            episodic_mem.setConsolidationThreshold(0.3)  # Slower consolidation
        else:  # standard
            # Balanced approach
            episodic_mem.setRetentionWindow(10000)  # 10000 steps
            episodic_mem.setConsolidationThreshold(0.5)  # Moderate consolidation
        
        print(f"Episodic memory window configured: {window_size} ({episodic_mem.getRetentionWindow()} steps)")

# Usage
configure_episodic_memory_window(brain, "long")
```

#### **2. Replay Strategy Optimization**
```python
# Optimize replay strategy based on task requirements

def optimize_replay_strategy(brain, task_type="learning"):
    episodic_mem = brain.getEpisodicMemory()
    if not episodic_mem:
        return
    
    config = brain.getConfig()
    replay_interval = config.getOr("replay_interval", 100)
    
    if task_type == "learning":
        # Frequent replay for rapid learning
        episodic_mem.setReplayStrength(1.0)
        episodic_mem.setReplayFrequency(replay_interval // 2)  # Replay more frequently
        print("Replay strategy optimized for learning: high frequency, strong replay")
    
    elif task_type == "recall":
        # Infrequent replay for memory retention
        episodic_mem.setReplayStrength(0.5)
        episodic_mem.setReplayFrequency(replay_interval * 2)  # Replay less frequently
        print("Replay strategy optimized for recall: low frequency, weak replay")
    
    elif task_type == "adaptation":
        # Adaptive replay based on performance
        episodic_mem.setReplayStrength(0.8)
        episodic_mem.setReplayFrequency(replay_interval)  # Standard replay
        print("Replay strategy optimized for adaptation: balanced approach")

# Apply replay optimization
optimize_replay_strategy(brain, "learning")
```

### Associative Memory Management

#### **1. Pattern Association Configuration**
```python
# Configure associative memory based on task requirements

def configure_associative_memory(brain, association_strength="standard"):
    associative_mem = brain.getAssociativeMemory()
    if associative_mem:
        if association_strength == "strong":
            associative_mem.setMinAssociationStrength(0.3)  # Lower threshold
            associative_mem.setMaxAssociationCount(1000)    # More associations
            print("Associative memory configured for strong pattern associations")
        
        elif association_strength == "weak":
            associative_mem.setMinAssociationStrength(0.7)  # Higher threshold
            associative_mem.setMaxAssociationCount(100)     # Fewer associations
            print("Associative memory configured for selective associations")
        
        else:  # standard
            associative_mem.setMinAssociationStrength(0.5)  # Standard threshold
            associative_mem.setMaxAssociationCount(500)    # Standard count
            print("Associative memory configured for standard associations")

# Usage
configure_associative_memory(brain, "strong")
```

---

## Performance Optimization Strategies

### Neural Computation Optimization

#### **1. Neuron Population Optimization**
```python
# Optimize neuron population configuration for performance

def optimize_neuron_populations(brain, target_performance="balanced"):
    regions = brain.getRegions()
    for region in regions:
        populations = region.getPopulations()
        for population in populations:
            neuron_count = population.getSize()
            neuron_type = population.getType()
            
            # Adjust population size based on performance target
            if target_performance == "speed":
                # Smaller populations for faster computation
                if neuron_count > 500:
                    # Consider reducing population size or increasing efficiency
                    print(f"Population {population.getId()} large ({neuron_count} neurons) for speed optimization")
            
            elif target_performance == "accuracy":
                # Larger populations for better accuracy
                if neuron_count < 100:
                    print(f"Population {population.getId()} small ({neuron_count} neurons) consider increasing for accuracy")
            
            elif target_performance == "balanced":
                # Keep current size for balanced performance
                pass
```

#### **2. Synaptic Connection Optimization**
```python
# Optimize synaptic connections for Phase 6 performance

def optimize_synaptic_connections(brain):
    config = brain.getConfig()
    
    # Adjust synaptic density based on configuration
    base_neuron_count = config.getOr("brain.neuron_count", 1000)
    target_synaptic_density = 0.1  # 10% connectivity
    
    # Calculate expected synapse count
    expected_synapses = int(base_neuron_count * base_neuron_count * target_synaptic_density / 2)
    
    # Monitor actual vs. expected
    actual_synapses = brain.getTotalSynapseCount()
    efficiency = actual_synapses / expected_synapses if expected_synapses > 0 else 1.0
    
    print(f"Synaptic efficiency: {efficiency:.2f} ({actual_synapses} actual, {expected_synapses} expected)")
    
    if efficiency < 0.5:
        print("Warning: Low synaptic efficiency, consider adjusting connection probability")
    elif efficiency > 2.0:
        print("Warning: High synaptic efficiency, consider pruning unnecessary connections")

# Apply synaptic optimization
optimize_synaptic_connections(brain)
```

### Plasticity Optimization

#### **1. STDP Parameter Tuning**
```python
# Optimize STDP parameters for Phase 6 performance

def optimize_stdp_parameters(brain, learning_phase="early"):
    stdp = brain.getSTDP()
    if not stdp:
        return
    
    config = brain.getConfig()
    base_learning_rate = config.getOr("plasticity.stdp.learning_rate", 0.001)
    
    if learning_phase == "early":
        # High learning rate for rapid initial learning
        stdp.setLTPWeight(base_learning_rate * 2.0)
        stdp.setLTDWeight(base_learning_rate * 2.5)
        print("STDP parameters optimized for early learning: high learning rates")
    
    elif learning_phase == "stable":
        # Moderate learning rate for stable performance
        stdp.setLTPWeight(base_learning_rate)
        stdp.setLTDWeight(base_learning_rate)
        print("STDP parameters optimized for stable performance: standard learning rates")
    
    elif learning_phase == "fine_tuning":
        # Low learning rate for fine-tuning
        stdp.setLTPWeight(base_learning_rate * 0.5)
        stdp.setLTDWeight(base_learning_rate * 0.5)
        print("STDP parameters optimized for fine-tuning: low learning rates")

# Apply STDP optimization
optimize_stdp_parameters(brain, "stable")
```

#### **2. Structural Plasticity Management**
```python
# Manage structural plasticity for performance and memory

def optimize_structural_plasticity(brain, growth_strategy="controlled"):
    structural_plasticity = brain.getStructuralPlasticity()
    if not structural_plasticity:
        return
    
    if growth_strategy == "controlled":
        # Controlled growth with pruning
        structural_plasticity.setSynaptogenesisRate(0.0001)
        structural_plasticity.setPruningRate(0.00001)
        print("Structural plasticity: controlled growth with balanced pruning")
    
    elif growth_strategy == "aggressive":
        # Aggressive growth for complex tasks
        structural_plasticity.setSynaptogenesisRate(0.001)
        structural_plasticity.setPruningRate(0.0001)
        print("Structural plasticity: aggressive growth with selective pruning")
    
    elif growth_strategy == "conservative":
        # Conservative growth for stable performance
        structural_plasticity.setSynaptogenesisRate(0.00001)
        structural_plasticity.setPruningRate(0.000001)
        print("Structural plasticity: conservative growth with minimal pruning")

# Apply structural plasticity optimization
optimize_structural_plasticity(brain, "controlled")
```

### Neuromodulation Optimization

#### **1. Dopamine System Tuning**
```python
# Optimize dopamine system for learning efficiency

def optimize_dopamine_system(brain, learning_objective="speed"):
    dopamine = brain.getDopamine()
    if not dopamine:
        return
    
    if learning_objective == "speed":
        # High dopamine for rapid learning
        dopamine.setLearningRateModulation(2.0)
        dopamine.setAdaptationRate(0.1)
        print("Dopamine system optimized for speed: high learning modulation")
    
    elif learning_objective == "stability":
        # Moderate dopamine for stable learning
        dopamine.setLearningRateModulation(1.0)
        dopamine.setAdaptationRate(0.5)
        print("Dopamine system optimized for stability: moderate learning modulation")
    
    elif learning_objective == "exploration":
        # Low dopamine for exploration
        dopamine.setLearningRateModulation(0.5)
        dopamine.setAdaptationRate(1.0)
        print("Dopamine system optimized for exploration: low learning modulation")

# Apply dopamine optimization
optimize_dopamine_system(brain, "balanced")
```

#### **2. Curiosity System Tuning**
```python
# Optimize curiosity system for exploration vs. exploitation balance

def optimize_curiosity_system(brain, exploration_strategy="balanced"):
    # Note: This is a conceptual example since curiosity is controlled through AgentBrain
    # In practice, you'd configure this in the agent initialization
    
    if exploration_strategy == "balanced":
        print("Curiosity system: balanced exploration-exploitation")
        print("  - Moderate curiosity levels for adaptive exploration")
        print("  - Confidence-based exploration policy")
    
    elif exploration_strategy == "exploration":
        print("Curiosity system: exploration-focused")
        print("  - High curiosity levels for thorough exploration")
        print("  - Random exploration with novelty detection")
    
    elif exploration_strategy == "exploitation":
        print("Curiosity system: exploitation-focused")
        print("  - Low curiosity levels for focused exploitation")
        print("  - Reward-driven action selection")

# Apply curiosity optimization
optimize_curiosity_system(brain, "balanced")
```

---

## Phase 6 Integration Best Practices

### Phase 6 System Integration

#### **1. Component Integration Order**
```python
# Best practices for integrating Phase 6 components

def integrate_phase6_systems(brain, agent, world):
    """Integrate Phase 6 systems in optimal order."""
    
    # Step 1: Configure memory systems first (foundational for learning)
    agent.enableEpisodicMemory(True)
    agent.enablePrediction(True)
    agent.enableConceptFormation(True)
    
    # Step 2: Configure neuromodulation systems (affects learning rates)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Step 3: Configure cognitive systems (use memory and neuromodulation)
    agent.enablePlanning(True)
    agent.enableAttention(True)
    
    # Step 4: Configure structural components (final integration)
    agent.enableStructuralPlasticity(True)
    
    print("Phase 6 systems integrated in optimal order:")
    print("  1. Memory systems (foundation for learning)")
    print("   gim 2. Neuromodulation systems (control learning rates)")
    print("  3. Cognitive systems (use memory and neuromodulation)")
    print("  4. Structural systems (final network optimization)")

# Integrate Phase 6 systems
integrate_phase6_systems(brain, agent, world)
```

#### **2. System Interaction Optimization**
```python
# Optimize interactions between Phase 6 systems

def optimize_system_interactions(brain, agent):
    """Optimize interactions between Phase 6 systems."""
    
    # Memory-Neuromodulation interactions
    # Dopamine affects memory consolidation
    brain.getDopamine().setAdaptationRate(0.3)
    
    # Prediction-Memory interactions
    # Predictions guide episodic memory storage
    brain.getPredictionSystem().setPredictionThreshold(0.7)
    
    # Curiosity-Cognition interactions
    # Curiosity directs attention and planning
    agent.setCuriosityForAttention(0.5)
    agent.setCuriosityForPlanning(0.3)
    
    # Development-Plasticity interactions
    # Development stages modulate plasticity
    brain.getDevelopmentSystem().setPlasticityModulation(0.8)
    
    print("Phase 6 system interactions optimized for coherent behavior")

# Optimize system interactions
optimize_system_interactions(brain, agent)
```

### Performance Monitoring and Management

#### **1. System Health Monitoring**
```python
# Monitor Phase 6 system health

def monitor_phase6_system_health(brain, agent, check_interval=100):
    """Monitor health of all Phase 6 systems."""
    
    metrics = {
        'working_memory_utilization': 0.0,
        'episodic_memory_health': 0.0,
        'prediction_accuracy': 0.0,
        'curiosity_level': 0.0,
        'novelty_level': 0.0,
        'development_stage': 0,
        'plasticity_effectiveness': 0.0,
        'planning_quality': 0.0
    }
    
    for step in range(check_interval):
        brain.step(step)
        
        # Monitor memory systems
        working_mem = brain.getWorkingMemory()
        if working_mem:
            metrics['working_memory_utilization'] = working_mem.getMemoryUtilization()
            print(f"Working memory utilization: {metrics['working_memory_utilization']:.2f}")
        
        episodic_mem = brain.getEpisodicMemory()
        if episodic_mem:
            metrics['episodic_memory_health'] = episodic_mem.getMemoryUtilization()
            print(f"Episodic memory health: {metrics['episodic_memory_health']:.2f}")
        
        # Monitor neuromodulation
        metrics['curiosity_level'] = agent.getCuriosityLevel()
        metrics['novelty_level'] = agent.getNoveltyLevel()
        print(f"Curiosity level: {metrics['curiosity_level']:.2f}")
        print(f"Novelty level: {metrics['novelty_level']:.2f}")
        
        # Monitor cognitive systems
        prediction = brain.getPredictionSystem()
        if prediction:
            metrics['prediction_accuracy'] = 1.0 - abs(prediction.getPredictionError())
            print(f"Prediction accuracy: {metrics['prediction_accuracy']:.2f}")
        
        planner = brain.getPlanner()
        if planner:
            metrics['planning_quality'] = planner.getLastPlanQuality()
            print(f"Planning quality: {metrics['planning_quality']:.2f}")
        
        # Monitor development
        metrics['development_stage'] = brain.getDevelopmentalStage()
        print(f"Development stage: {metrics['development_stage']}")
        
        time.sleep(check_interval)
    
    return metrics

# Start monitoring
monitor_phase6_system_health(brain, agent, check_interval=100)
```

#### **2. Performance Benchmarking**
```python
# Benchmark Phase 6 performance

def benchmark_phase6_performance(brain, agent, duration_seconds=300):
    """Benchmark Phase 6 performance over time."""
    
    print(f"Starting Phase 6 performance benchmark for {duration_seconds} seconds")
    
    start_time = time.time()
    step_count = 0
    memory_samples = []
    prediction_samples = []
    curiosity_samples = []
    
    while time.time() - start_time < duration_seconds:
        brain.step(step_count)
        step_count += 1
        
        # Collect performance samples
        working_mem = brain.getWorkingMemory()
        if working_mem:
            memory_samples.append(working_mem.getMemoryUtilization())
        
        prediction = brain.getPredictionSystem()
        if prediction:
            prediction_samples.append(1.0 - abs(prediction.getPredictionError()))
        
        curiosity_samples.append(agent.getCuriosityLevel())
        
        # Small delay to control sampling rate
        time.sleep(0.1)
    
    # Calculate statistics
    def calculate_statistics(samples, name):
        if not samples:
            return None
        
        mean = sum(samples) / len(samples)
        variance = sum((x - mean) ** 2 for x in samples) / len(samples)
        std_dev = variance ** 0.5
        
        print(f"{name} Statistics:")
        print(f"  Samples: {len(samples)}")
        print(f"  Mean: {mean:.4f}")
        print(f"  Std Dev: {std_dev:.4f}")
        print(f"  Min: {min(samples):.4f}")
        print(f"  Max: {max(samples):.4f}")
        
        return {
            'mean': mean,
            'std_dev': std_dev,
            'min': min(samples),
            'max': max(samples),
            'count': len(samples)
        }
    
    print("\n=== Phase 6 Performance Benchmark Results ===")
    memory_stats = calculate_statistics(memory_samples, "Working Memory Utilization")
    prediction_stats = calculate_statistics(prediction_samples, "Prediction Accuracy")
    curiosity_stats = calculate_statistics(curiosity_samples, "Curiosity Level")
    
    # Overall performance score
    overall_score = 0
    if memory_stats:
        overall_score += memory_stats['mean'] * 0.3
    if prediction_stats:
        overall_score += prediction_stats['mean'] * 0.4
    if curiosity_stats:
        overall_score += curiosity_stats['mean'] * 0.3
    
    print(f"\nOverall Phase 6 Performance Score: {overall_score:.4f}/1.0")
    
    return {
        'overall_score': overall_score,
        'memory_stats': memory_stats,
        'prediction_stats': prediction_stats,
        'curiosity_stats': curiosity_stats,
        'total_steps': step_count,
        'duration_seconds': time.time() - start_time
    }

# Run performance benchmark
benchmark_results = benchmark_phase6_performance(brain, agent, duration_seconds=60)
```

---

## Debugging and Monitoring

### Common Phase 6 Issues and Solutions

#### **1. Memory System Problems**
```python
# Debug memory system issues
def debug_memory_systems(brain, agent):
    print("=== Memory System Debug ===")
    
    # Check working memory
    working_mem = brain.getWorkingMemory()
    if working_mem:
        print("Working Memory:")
        print(f"  Capacity: {working_mem.getCapacity()}")
        print(f"  Active traces: {working_mem.getActiveTraces()}")
        print(f"  Utilization: {working_mem.getMemoryUtilization():.2f}")
        print(f"  Decay rate: {working_mem.getDecayRate():.4f}")
        print(f"  Activity level: {working_mem.getMemoryActivity():.2f}")
        
        if working_mem.getActiveTraces() == 0:
            print("  WARNING: Working memory has no active traces!")
        if working_mem.getMemoryUtilization() > 0.95:
            print("  WARNING: Working memory near capacity!")
    else:
        print("  ERROR: Working memory not available!")
    
    # Check episodic memory
    episodic_mem = brain.getEpisodicMemory()
    if episodic_mem:
        print("\nEpisodic Memory:")
        print(f"  Episodes: {episodic_mem.getEpisodeCount()}")
        print(f"  Active replays: {episodic_mem.getActiveReplayCount()}")
        print(f"  Health: {episodic_mem.getMemoryUtilization():.2f}")
        
        if episodic_mem.getEpisodeCount() == 0:
            print("  WARNING: No episodic memories stored!")
    else:
        print("\nERROR: Episodic memory not available!")
    
    print("=== End Memory Debug ===")

# Debug memory systems
debug_memory_systems(brain, agent)
```

#### **2. Neuromodulation System Problems**
```python
# Debug neuromodulation system issues
def debug_neuromodulation_systems(brain, agent):
    print("=== Neuromodulation System Debug ===")
    
    # Check dopamine system
    dopamine = brain.getDopamine()
    if dopamine:
        print("Dopamine System:")
        print(f"  Level: {dopamine.getLevel():.2f}")
        print(f"  Learning rate modulation: {dopamine.getLearningRateModulation():.2f}")
        print(f"  Eligibility trace: {dopamine.getEligibilityTrace():.2f}")
        print(f"  Predicted value: {dopamine.getPredictedValue():.2f}")
        print(f"  Bursting: {dopamine.isBursting()}")
        
        if dopamine.getLevel() <= 0:
            print("  WARNING: Dopamine system inactive!")
    else:
        print("ERROR: Dopamine system not available!")
    
    # Check curiosity system
    print(f"\nCuriosity System:")
    print(f"  Level: {agent.getCuriosityLevel():.2f}")
    print(f"  Novelty: {agent.getNoveltyLevel():.2f}")
    print(f"  Prediction error: {agent.getPredictionError():.2f}")
    
    if agent.getCuriosityLevel() <= 0:
        print("  WARNING: Curiosity system not working!")
    
    print("=== End Neuromodulation Debug ===")

# Debug neuromodulation systems
debug_neuromodulation_systems(brain, agent)
```

#### **3. Prediction System Problems**
```python
# Debug prediction system issues
def debug_prediction_system(brain, agent):
    print("=== Prediction System Debug ===")
    
    # Check prediction system
    prediction = brain.getPredictionSystem()
    if prediction:
        print("Prediction System:")
        print(f"  Prediction history length: {len(prediction.getPredictionHistory())}")
        print(f"  Error history length: {len(prediction.getErrorHistory())}")
        print(f"  Confidence: {prediction.getConfidence():.2f}")
        
        # Calculate prediction accuracy if we have history
        if prediction.getErrorHistory():
            avg_error = sum(prediction.getErrorHistory()) / len(prediction.getErrorHistory())
            accuracy = 1.0 - abs(avg_error)
            print(f"  Average prediction accuracy: {accuracy:.2f}")
        
        if prediction.getConfidence() <= 0:
            print("  WARNING: Prediction system confidence too low!")
    else:
        print("ERROR: Prediction system not available!")
    
    print("=== End Prediction System Debug ===")

# Debug prediction system
debug_prediction_system(brain, agent)
```

---

## Simulation Management

### Simulation Lifecycle Management

#### **1. Simulation Setup**
```python
class Phase6SimulationManager:
    def __init__(self, config, world):
        self.config = config
        self.world = world
        self.brain = None
        self.agent = None
        self.simulation_active = False
        self.simulation_start_time = None
        self.step_count = 0
    
    def setup_simulation(self):
        """Setup Phase 6 simulation with all integrated systems."""
        print("Setting up Phase 6 simulation...")
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        print("✓ Brain created and initialized")
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        print("✓ Agent created and initialized")
        
        # Enable Phase 6 features
        self._enable_phase6_features()
        print("✓ Phase 6 features enabled")
        
        # Start simulation
        self.simulation_active = True
        self.simulation_start_time = time.time()
        print("✓ Simulation started")
        
        return True
    
    def _enable_phase6_features(self):
        """Enable all Phase 6 features."""
        # Memory systems
        self.agent.enableEpisodicMemory(True)
        self.agent.enablePrediction(True)
        self.agent.enableConceptFormation(True)
        
        # Neuromodulation systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Cognitive systems
        self.agent.enablePlanning(True)
        self.agent.enableAttention(True)
        
        # Structural systems
        self.agent.enableStructuralPlasticity(True)
    
    def run_simulation(self, duration_steps, checkpoint_interval=None):
        """Run simulation for specified number of steps."""
        if not self.simulation_active:
            raise RuntimeError("Simulation not started. Call setup_simulation() first.")
        
        print(f"Running simulation for {duration_steps} steps...")
        
        for step in range(duration_steps):
            self.step(step)
            
            # Save checkpoint if interval specified
            if checkpoint_interval and step % checkpoint_interval == 0:
                self.save_checkpoint(f"checkpoint_step_{step}.bin")
        
        return self.get_simulation_stats()
    
    def step(self, step_number):
        """Execute one simulation step."""
        if not self.simulation_active:
            return
        
        # Update world
        self.world.update(0.1)
        
        # Get sensory input
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Run brain step
        self.brain.step(step_number)
        
        # Decode motor command
        motor_cmd = self.agent.decodeMotorCommand()
        
        # Apply motor command
        self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
        
        # Apply neuromodulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        predicted_reward = self.agent.getPredictionError()
        self.agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        self.agent.updateDevelopment(0.1)
        
        self.step_count = step_number + 1
        
        # Log progress periodically
        if (step_number + 1) % 100 == 0:
            self.log_simulation_progress(step_number)
    
    def log_simulation_progress(self, step_number):
        """Log simulation progress."""
        elapsed = time.time() - self.simulation_start_time
        steps_per_second = step_number / elapsed if elapsed > 0 else 0
        
        print(f"Step {step_number} ({step_number/1000:.1f}K): "
              f"{steps_per_second:.1f} steps/sec, "
              f"curiosity: {self.agent.getCuriosityLevel():.2f}, "
              f"novelty: {self.agent.getNoveltyLevel():.2f}")
        
        # Memory usage
        working_mem = self.brain.getWorkingMemory()
        if working_mem:
            utilization = working_mem.getMemoryUtilization()
            print(f"  Working memory: {working_mem.getActiveTraces()}/" 
                  f"{working_mem.getCapacity()} ({utilization:.1%})")
    
    def save_checkpoint(self, filename):
        """Save simulation checkpoint."""
        if self.brain:
            self.brain.save(filename)
            print(f"Checkpoint saved: {filename}")
        else:
            print(f"ERROR: Cannot save checkpoint, brain not initialized")
    
    def load_checkpoint(self, filename):
        """Load simulation checkpoint."""
        if self.brain:
            if self.brain.load(filename):
                print(f"Checkpoint loaded: {filename}")
                # Update step count based on checkpoint
                return True
            else:
                print(f"ERROR: Failed to load checkpoint: {filename}")
                return False
        else:
            print(f"ERROR: Cannot load checkpoint, brain not initialized")
            return False
    
    def get_simulation_stats(self):
        """Get simulation statistics."""
        stats = {
            'total_steps': self.step_count,
            'simulation_time': time.time() - self.simulation_start_time,
            'steps_per_second': self.step_count / (time.time() - self.simulation_start_time) if (time.time() - self.simulation_start_time) > 0 else 0
        }
        
        # Add memory stats
        working_mem = self.brain.getWorkingMemory()
        if working_mem:
            stats['working_memory_utilization'] = working_mem.getMemoryUtilization()
            stats['working_memory_traces'] = working_mem.getActiveTraces()
        
        episodic_mem = self.brain.getEpisodicMemory()
        if episodic_mem:
            stats['episodic_memory_count'] = episodic_mem.getEpisodeCount()
        
        # Add cognitive stats
        prediction = self.brain.getPredictionSystem()
        if prediction:
            stats['prediction_accuracy'] = 1.0 - abs(prediction.getPredictionError())
        
        return stats
    
    def cleanup_simulation(self):
        """Cleanup simulation."""
        print("Cleaning up simulation...")
        
        # Save final checkpoint
        self.save_checkpoint("checkpoint_final.bin")
        
        # Print final statistics
        stats = self.get_simulation_stats()
        
        print("\n=== Simulation Statistics ===")
        for key, value in stats.items():
            if isinstance(value, float):
                print(f"{key.replace('_', ' ').title()}: {value:.4f}")
            else:
                print(f"{key.replace('_', ' ').title()}: {value}")
        
        # Stop simulation
        self.simulation_active = False
        print("Simulation cleanup complete")
# Usage example
# simulation = Phase6SimulationManager(config, world)
# simulation.setup_simulation()
# results = simulation.run_simulation(duration_steps=10000, checkpoint_interval=1000)
# simulation.cleanup_simulation()
```

---

## Advanced Tuning

### Advanced Performance Tuning Strategies

#### **1. Multi-Scale Memory Management**
```python
# Implement multi-scale memory management
class MultiScaleMemoryManager:
    def __init__(self, brain):
        self.brain = brain
        self.scales = {
            'short_term': {'window': 1000, 'decay': 0.1, 'capacity': 50},
            'medium_term': {'window': 10000, 'decay': 0.05, 'capacity': 200},
            'long_term': {'window': 100000, 'decay': 0.01, 'capacity': 1000}
        }
    
    def allocate_to_scale(self, pattern_complexity, memory_importance):
        """Allocate memory to appropriate scale based on pattern complexity and importance."""
        
        # Determine appropriate scale
        if memory_importance > 0.8 or pattern_complexity > 0.9:
            scale = 'long_term'
        elif memory_importance > 0.5 or pattern_complexity > 0.6:
            scale = 'medium_term'
        else:
            scale = 'short_term'
        
        # Allocate to selected scale
        config = self.scales[scale]
        self.brain.getWorkingMemory().setCapacity(config['capacity'])
        self.brain.getWorkingMemory().setDecayRate(config['decay'])
        
        return scale
    
    def optimize_scale_transitions(self):
        """Optimize transitions between memory scales."""
        # Analyze memory access patterns
        working_mem = self.brain.getWorkingMemory()
        
        # Check for cross-scale access patterns
        access_patterns = self.analyze_access_patterns()
        
        # Optimize scale boundaries based on access patterns
        if 'frequent_transitions' in access_patterns:
            # Reduce scale boundaries to improve performance
            self.scales['short_term']['window'] *= 0.8
            self.scales['medium_term']['window'] *= 0.8
            print("Optimized scale boundaries for frequent transitions")
```

#### **2. Adaptive Plasticity Control**
```python
# Implement adaptive plasticity control
class AdaptivePlasticityController:
    def __init__(self, brain):
        self.brain = brain
        self.plasticity_history = []
        self.performance_history = []
        self.adaptation_factor = 0.1
    
    def update_plasticity_adaptation(self):
        """Adapt plasticity parameters based on performance."""
        
        # Get current performance metrics
        current_performance = self.calculate_performance()
        
        # Update history
        self.plasticity_history.append(self.get_current_plasticity_state())
        self.performance_history.append(current_performance)
        
        # Keep history size manageable
        if len(self.plasticity_history) > 100:
            self.plasticity_history.pop(0)
            self.performance_history.pop(0)
        
        # Adapt if performance is suboptimal
        if current_performance < 0.7:
            self.adapt_plasticity_up()
            print("Plasticity increased - performance below threshold")
        elif current_performance > 0.9:
            self.adapt_plasticity_down()
            print("Plasticity decreased - performance above threshold")
        
        return current_performance
    
    def calculate_performance(self):
        """Calculate overall performance based on multiple metrics."""
        metrics = []
        
        # Working memory utilization
        working_mem = self.brain.getWorkingMemory()
        if working_mem:
            metrics.append(working_mem.getMemoryUtilization())
        
        # Prediction accuracy
        prediction = self.brain.getPredictionSystem()
        if prediction:
            metrics.append(1.0 - abs(prediction.getPredictionError()))
        
        # Curiosity level (should be moderate for exploration)
        if hasattr(self.brain, 'getCuriosity'):
            curiosity = self.brain.getCuriosity()
            if curiosity:
                metrics.append(curiosity.getLevel())
        
        if not metrics:
            return 0.5
        
        return sum(metrics) / len(metrics)
    
    def adapt_plasticity_up(self):
        """Increase plasticity for better learning."""
        stdp = self.brain.getSTDP()
        if stdp:
            current_rate = stdp.getLTPWeight()
            new_rate = min(current_rate * 1.5, 0.05)
            stdp.setLTPWeight(new_rate)
            print(f"Increased STDP rate from {current_rate:.4f} to {new_rate:.4f}")
        
        # Increase structural plasticity rate
        structural = self.brain.getStructuralPlasticity()
        if structural:
            current_rate = structural.getSynaptogenesisRate()
            new_rate = min(current_rate * 1.5, 0.001)
            structural.setSynaptogenesisRate(new_rate)
            print(f"Increased synaptogenesis rate from {current_rate:.6f} to {new_rate:.6f}")
    
    def adapt_plasticity_down(self):
        """Decrease plasticity for stability."""
        stdp = self.brain.getSTDP()
        if stdp:
            current_rate = stdp.getLTPWeight()
            new_rate = max(current_rate * 0.7, 0.001)
            stdp.setLTPWeight(new_rate)
            print(f"Decreased STDP rate from {current_rate:.4f} to {new_rate:.4f}")
        
        # Decrease structural plasticity rate
        structural = self.brain.getStructuralPlasticity()
        if structural:
            current_rate = structural.getSynaptogenesisRate()
            new_rate = max(current_rate * 0.7, 0.00001)
            structural.setSynaptogenesisRate(new_rate)
            print(f"Decreased synaptogenesis rate from {current_rate:.6f} to {new_rate:.6f}")
```

---

## Troubleshooting Guide

### Common Phase 6 Issues and Solutions

#### **Issue 1: Memory Systems Not Working**

**Problem**: Memory systems returning `nullptr` or empty

**Symptoms**:
- `brain.getWorkingMemory()` returns `nullptr`
- `brain.getEpisodicMemory()` returns `nullptr`
- No memory traces being stored
- Working memory utilization stays at 0

**Diagnostic Commands**:
```python
# Debug memory systems
def debug_memory_issue(brain, agent):
    print("=== Memory System Debug ===")
    
    # Check if memory systems are available
    if not brain.getWorkingMemory():
        print("❌ Working memory not available")
        print("   - Check that agent.enableEpisodicMemory(True) is called")
        print("   - Verify brain initialization")
    else:
        working_mem = brain.getWorkingMemory()
        print(f"✓ Working memory available: {working_mem.getActiveTraces()} traces")
        if working_mem.getActiveTraces() == 0:
            print("   - Working memory has no active traces")
            print("   - Check if store() method is being called")
    
    if not brain.getEpisodicMemory():
        print("❌ Episodic memory not available")
        print("   - Check that agent.enableEpisodicMemory(True) is called")
        print("   - Verify episodic memory initialization")
    else:
        episodic_mem = brain.getEpisodicMemory()
        print(f"✓ Episodic memory available: {episodic_mem.getEpisodeCount()} episodes")
        if episodic_mem.getEpisodeCount() == 0:
            print("   - No episodic memories stored")
            print("   - Check if storeEpisode() method is being called")
```

**Solution**:
1. **Ensure memory systems are enabled**:
   ```python
   agent.enableEpisodicMemory(True)  # Enable episodic memory
   agent.enablePrediction(True)     # Enable prediction system
   ```

2. **Verify configuration**:
   ```python
   # Check if replay interval is set
   replay_interval = config.getOr("replay_interval", 100)
   if replay_interval <= 0:
       print("Warning: replay_interval should be positive")
   ```

3. **Check initialization order**:
   ```python
   # Initialize in correct order
   brain = pynlm.createBrain(config)
   brain.initialize()  # Must be called first
   agent = pynlm.createAgentBrain(brain)
   agent.initialize(world)  # Memory systems initialized here
   ```

#### **Issue 2: Prediction System Not Learning**

**Problem**: Prediction system not updating or providing predictions

**Symptoms**:
- `brain.getPredictionSystem()` returns `nullptr`
- `prediction_error` stays at maximum (1.0)
- No improvement in prediction accuracy

**Diagnostic Commands**:
```python
# Debug prediction system
def debug_prediction_issue(brain, agent):
    print("=== Prediction System Debug ===")
    
    # Check if prediction system is available
    if not brain.getPredictionSystem():
        print("❌ Prediction system not available")
        print("   - Check that agent.enablePrediction(True) is called")
        print("   - Verify prediction system initialization")
    else:
        prediction = brain.getPredictionSystem()
        error = agent.getPredictionError()
        print(f"✓ Prediction system available")
        print(f"  Prediction error: {error:.4f}")
        
        if error > 0.9:
            print("  ❌ Prediction error too high!")
            print("   - Check if prediction system is receiving training data")
            print("   - Verify sensory input is being processed")
        
        # Check prediction system internal state
        if hasattr(prediction, 'getTrainingSamples'):
            samples = prediction.getTrainingSamples()
            print(f"  Training samples: {samples}")
            if samples < 10:
                print("  WARNING: Too few training samples for learning")
```

**Solution**:
1. **Ensure prediction system is enabled**:
   ```python
   agent.enablePrediction(True)  # Enable prediction system
   agent.enableConceptFormation(True)  # Enable concept formation
   ```

2. **Check training data flow**:
   ```python
   # Verify that sensory input is being provided to prediction system
   if world.getSensoryPercept():
       percept = world.getSensoryPercept()
       # Process percept in prediction system
       prediction.processSensoryInput(percept)
   ```

3. **Configure prediction parameters**:
   ```python
   # Adjust prediction learning rate
   config.set("prediction.learning_rate", 0.02)
   config.set("prediction.prediction_window", 10)
   ```

#### **Issue 3: Development System Not Progressing**

**Problem**: Development stages not changing or updating

**Symptoms**:
- `brain.getDevelopmentalStage()` stays at `Initial`
- `development.getAge()` doesn't increase
- Plasticity modifiers remain at initial values

**Diagnostic Commands**:
```python
# Debug development system
def debug_development_issue(brain, agent):
    print("=== Development System Debug ===")
    
    # Check development system
    dev_system = brain.getDevelopmentSystem()
    if dev_system:
        print("Development System:")
        print(f"  Current stage: {brain.getDevelopmentalStage()}")
        print(f"  Age: {dev_system.getAge():.2f}")
        print(f"  Maturation level: {dev_system.getMaturationLevel():.2f}")
        print(f"  Plasticity modifier: {dev_system.getPlasticityModifier():.2f}")
        
        # Check stage progression
        stage = brain.getDevelopmentalStage()
        if stage == DevelopmentalStage.Initial:
            print("  ❌ Still in Initial stage!")
            print("   - Check if development system is updating")
            print("   - Verify that agent.updateDevelopment() is called")
        elif stage == DevelopmentalStage.CriticalPeriod:
            print("  ✓ In Critical Period stage")
        elif stage == DevelopmentalStage.Maturation:
            print("  ✓ In Maturation stage")
        elif stage == DevelopmentalStage.Adult:
            print("  ✓ In Adult stage")
        elif stage == DevelopmentalStage.Aging:
            print("  ✓ In Aging stage")
    else:
        print("❌ Development system not available")
        print("   - Check that agent.enableDevelopment(True) is called")
        print("   - Verify development system initialization")
```

**Solution**:
1. **Ensure development system is enabled**:
   ```python
   agent.enableDevelopment(True)  # Enable development system
   agent.updateDevelopment(0.1)   # Call update_development in simulation loop
   ```

2. **Check development timing**:
   ```python
   # Ensure development is being updated regularly
   if step % 10 == 0:  # Update every 10 steps
       agent.updateDevelopment(0.1)
   ```

3. **Configure development parameters**:
   ```python
   # Adjust development rates
   config.set("development.maturation_rate", 0.001)
   config.set("development.critical_period_strength", 1.0)
   config.set("development.homeostatic_drive", 0.1)
   ```

---

## Performance Benchmarking

### Phase 6 Performance Benchmark Suite

#### **1. Basic Performance Metrics**
```python
# Define basic performance metrics
PERFORMANCE_METRICS = {
    'working_memory_efficiency': {
        'target': 0.8,  # Target 80% utilization
        'weight': 0.3,
        'function': lambda x: min(x / 0.8, 1.0) if x > 0 else 0
    },
    'prediction_accuracy': {
        'target': 0.9,  # Target 90% accuracy
        'weight': 0.4,
        'function': lambda x: x  # Direct accuracy
    },
    'curiosity_balance': {
        'target': 0.5,  # Target moderate curiosity (exploration vs exploitation)
        'weight': 0.2,
        'function': lambda x: 1.0 - abs(x - 0.5) * 2  # Peak at 0.5
    },
    'development_progress': {
        'target': 3,  # Target stage progression
        'weight': 0.1,
        'function': lambda x: min(x / 3, 1.0) if x > 0 else 0
    }
}

# Calculate overall performance score
def calculate_overall_performance(metrics, weights=None):
    """Calculate overall performance score."""
    if weights is None:
        weights = {k: v['weight'] for k, v in metrics.items()}
    
    total_score = 0
    total_weight = 0
    
    for metric_name, metric_config in metrics.items():
        if metric_name in weights:
            value = metric_config.get('value', 0)
            weight = weights[metric_name]
            total_score += value * weight
            total_weight += weight
    
    return total_score / total_weight if total_weight > 0 else 0.0
```

#### **2. Phase 6 Specific Benchmarks**
```python
# Phase 6 specific performance benchmarks
class Phase6BenchmarkSuite:
    def __init__(self, brain, agent):
        self.brain = brain
        self.agent = agent
        self.benchmark_results = {}
    
    def run_all_benchmarks(self):
        """Run all Phase 6 performance benchmarks."""
        print("=== Phase 6 Performance Benchmark Suite ===\n")
        
        # Memory system benchmarks
        self.benchmark_memory_systems()
        
        # Neuromodulation system benchmarks
        self.benchmark_neuromodulation_systems()
        
        # Prediction system benchmarks
        self.benchmark_prediction_system()
        
        # Cognitive system benchmarks
        self.benchmark_cognitive_systems()
        
        # Integration performance benchmarks
        self.benchmark_integration_performance()
        
        # Overall Phase 6 performance
        self.calculate_overall_phase6_score()
        
        print("\n=== Benchmark Complete ===")
    
    def benchmark_memory_systems(self):
        """Benchmark memory system performance."""
        print("1. Memory System Benchmarks")
        
        # Working memory benchmark
        working_mem = self.brain.getWorkingMemory()
        if working_mem:
            utilization = working_mem.getMemoryUtilization()
            capacity = working_mem.getCapacity()
            activity = working_mem.getMemoryActivity()
            
            # Efficiency calculation
            efficiency = utilization * activity
            print(f"   Working Memory Efficiency: {efficiency:.3f}")
            print(f"     Utilization: {utilization:.2f} ({working_mem.getActiveTraces()}/{capacity})")
            print(f"     Activity: {activity:.3f}")
            
            self.benchmark_results['working_memory_efficiency'] = efficiency
        
        # Episodic memory benchmark
        episodic_mem = self.brain.getEpisodicMemory()
        if episodic_mem:
            episodes = episodic_mem.getEpisodeCount()
            replay_count = episodic_mem.getActiveReplayCount()
            memory_utilization = episodic_mem.getMemoryUtilization()
            
            # Memory health score
            memory_health = (episodes / 100) * 0.5 + (replay_count / 10) * 0.5
            memory_health = min(memory_health, 1.0)
            
            print(f"   Episodic Memory Health: {memory_health:.3f}")
            print(f"     Episodes stored: {episodes}")
            print(f"     Active replays: {replay_count}")
            print(f"     Memory utilization: {memory_utilization:.2f}")
            
            self.benchmark_results['episodic_memory_health'] = memory_health
    
    def benchmark_neuromodulation_systems(self):
        """Benchmark neuromodulation system performance."""
        print("2. Neuromodulation System Benchmarks")
        
        # Dopamine system benchmark
        dopamine = self.brain.getDopamine()
        if dopamine:
            level = dopamine.getLevel()
            learning_modulation = dopamine.getLearningRateModulation()
            eligibility_trace = dopamine.getEligibilityTrace()
            
            # Dopamine effectiveness score
            dopamine_effectiveness = (level * learning_modulation * eligibility_trace)
            
            print(f"   Dopamine System Effectiveness: {dopamine_effectiveness:.3f}")
            print(f"     Level: {level:.2f}")
            print(f"     Learning modulation: {learning_modulation:.2f}")
            print(f"     Eligibility trace: {eligibility_trace:.2f}")
            
            self.benchmark_results['dopamine_effectiveness'] = dopamine_effectiveness
        
        # Curiosity system benchmark
        curiosity = self.agent.getCuriosityLevel()
        novelty = self.agent.getNoveltyLevel()
        prediction_error = self.agent.getPredictionError()
        
        # Curiosity balance score
        curiosity_balance = 1.0 - abs(curiosity - 0.5) * 2
        curiosity_balance = max(0.0, min(1.0, curiosity_balance))
        
        print(f"   Curiosity System Balance: {curiosity_balance:.3f}")
        print(f"     Curiosity level: {curiosity:.2f}")
        print(f"     Novelty level: {novelty:.2f}")
        print(f"     Prediction error: {prediction_error:.2f}")
        
        self.benchmark_results['curiosity_balance'] = curiosity_balance
    
    def benchmark_prediction_system(self):
        """Benchmark prediction system performance."""
        print("3. Prediction System Benchmarks")
        
        prediction = self.brain.getPredictionSystem()
        if prediction:
            # Get prediction metrics
            error_history = prediction.getErrorHistory()
            prediction_history = prediction.getPredictionHistory()
            
            if error_history:
                avg_error = sum(error_history) / len(error_history)
                prediction_accuracy = 1.0 - abs(avg_error)
                
                print(f"   Prediction System Accuracy: {prediction_accuracy:.3f}")
                print(f"     Average error: {avg_error:.3f}")
                print(f"     History samples: {len(error_history)}")
                
                # Prediction stability (lower variance = more stable)
                error_variance = sum((x - avg_error) ** 2 for x in error_history) / len(error_history)
                prediction_stability = 1.0 - min(error_variance, 1.0)
                
                print(f"     Prediction stability: {prediction_stability:.3f}")
                
                self.benchmark_results['prediction_accuracy'] = prediction_accuracy
                self.benchmark_results['prediction_stability'] = prediction_stability
    
    def benchmark_cognitive_systems(self):
        """Benchmark cognitive system performance."""
        print("4. Cognitive System Benchmarks")
        
        # Concept formation benchmark
        concept_formation = self.brain.getConceptFormation()
        if concept_formation:
            # This would need to be implemented based on available API
            print(f"   Concept Formation: Available ({type(concept_formation).__name__})")
        
        # Planning benchmark
        planner = self.brain.getPlanner()
        if planner:
            # This would need to be implemented based on available API
            print(f"   Planning System: Available ({type(planner).__name__})")
        
        # Attention benchmark
        attention = self.brain.getAttention()
        if attention:
            # This would need to be implemented based on available API
            print(f"   Attention System: Available ({type(attention).__name__})")
        
        self.benchmark_results['cognitive_systems_available'] = True
    
    def benchmark_integration_performance(self):
        """Benchmark overall integration performance."""
        print("5. Integration Performance Benchmark")
        
        # Integration efficiency score
        integration_score = 0.0
        
        # Memory integration score
        working_mem = self.brain.getWorkingMemory()
        episodic_mem = self.brain.getEpisodicMemory()
        
        if working_mem:
            integration_score += working_mem.getMemoryUtilization() * 0.3
        
        if episodic_mem:
            integration_score += episodic_mem.getMemoryUtilization() * 0.2
        
        # Neuromodulation integration score
        if self.agent.getCuriosityLevel() > 0:
            integration_score += 0.2
        
        # Prediction integration score
        prediction = self.brain.getPredictionSystem()
        if prediction:
            error = self.agent.getPredictionError()
            integration_score += (1.0 - error) * 0.3
        
        print(f"   Integration Efficiency Score: {integration_score:.3f}")
        print(f"     Memory integration: {'✓' if working_mem else '✗'}")
        print(f"     Neuromodulation integration: {'✓' if self.agent.getCuriosityLevel() > 0 else '✗'}")
        print(f"     Prediction integration: {'✓' if prediction else '✗'}")
        
        self.benchmark_results['integration_efficiency'] = integration_score
    
    def calculate_overall_phase6_score(self):
        """Calculate overall Phase 6 performance score."""
        print("6. Overall Phase 6 Performance Score")
        
        # Calculate weighted average of all benchmarks
        weights = {
            'working_memory_efficiency': 0.25,
            'episodic_memory_health': 0.20,
            'dopamine_effectiveness': 0.20,
            'curiosity_balance': 0.15,
            'prediction_accuracy': 0.10
        }
        
        overall_score = 0.0
        total_weight = 0.0
        
        for metric, weight in weights.items():
            if metric in self.benchmark_results:
                score = self.benchmark_results[metric]
                overall_score += score * weight
                total_weight += weight
        
        final_score = overall_score / total_weight if total_weight > 0 else 0.0
        
        print(f"   Overall Phase 6 Performance Score: {final_score:.3f}/1.0")
        print(f"   Interpretation:")
        print(f"     - {'Excellent' if final_score > 0.8 else 'Good' if final_score > 0.6 else 'Fair' if final_score > 0.4 else 'Poor'} Performance")
        
        # Add benchmark summary
        print(f"\n   Benchmark Summary:")
        for metric, score in self.benchmark_results.items():
            if isinstance(score, (int, float)) and score > 0:
                print(f"     {metric.replace('_', ' ').title()}: {score:.3f}")
        
        return final_score
```

#### **3. Usage Example**
```python
# Run Phase 6 performance benchmarks
benchmark_suite = Phase6BenchmarkSuite(brain, agent)
benchmark_suite.run_all_benchmarks()

# Get detailed results
results = benchmark_suite.benchmark_results
print(f"Overall Performance: {results.get('overall_score', 0):.3f}")

# Compare against targets
def check_phase6_benchmarks_against_targets(results):
    print("\n=== Phase 6 Benchmark Against Targets ===")
    
    targets = {
        'working_memory_efficiency': 0.8,
        'episodic_memory_health': 0.8,
        'dopamine_effectiveness': 0.8,
        'curiosity_balance': 0.8,
        'prediction_accuracy': 0.9
    }
    
    for metric, target in targets.items():
        if metric in results:
            score = results[metric]
            status = "✓ PASS" if score >= target else "✗ FAIL"
            print(f"{metric.replace('_', ' ').title()}: {score:.3f} (target: {target}) {status}")
        else:
            print(f"{metric.replace('_', ' ').title()}: No data available")

# Check against targets
check_phase6_benchmarks_against_targets(results)
```

---

## Conclusion

The NLM Phase 6 Best Practices and Performance Tuning Guide provides comprehensive recommendations for optimizing Phase 6 brain simulations across multiple dimensions:

### Key Recommendations:

1. **Use Phase 6 Optimizations**: Enable all Phase 6 features for full capability
2. **Monitor System Performance**: Track memory utilization, prediction accuracy, and curiosity levels
3. **Optimize Configuration**: Use task-specific configurations for different use cases
4. **Benchmark Performance**: Use the Phase 6 benchmark suite to measure and optimize performance
5. **Gradual Integration**: Migrate from Phase 4 to Phase 6 incrementally to manage complexity

### Performance Optimization Priorities:

1. **Memory Efficiency**: Monitor and optimize working memory utilization
2. **Learning Speed**: Balance exploration and exploitation through curiosity control
3. **Prediction Accuracy**: Ensure prediction system is receiving adequate training
4. **Development Progress**: Monitor development stage progression
5. **System Integration**: Verify all Phase 6 systems are properly connected

### Best Practices:

- **Configuration Management**: Use JSON files for experiment reproducibility
- **Performance Monitoring**: Implement real-time monitoring and alerting
- **Benchmarking**: Establish baseline performance metrics
- **Documentation**: Document configuration and results for reproducibility
- **Continuous Improvement**: Iterate on configuration and optimization based on results

This guide provides the foundation for successfully implementing and optimizing Phase 6 NLM brain simulations, enabling users to leverage the full power of the integrated artificial brain architecture.

---

*This guide covers Phase 6 performance optimization and best practices. For detailed API documentation, refer to `NLM_API_Documentation.md` and `NLM_Cpp_API_Reference.md`.*

## References and Further Reading

- **Phase 6 Final Audit**: `docs/PHASE6_FINAL_AUDIT.md` - Comprehensive audit of Phase 6 integration status
- **Phase 6 Results**: `docs/PHASE6_FINAL_RESULTS.md` - Performance metrics and findings from Phase 6 experiments
- **System Architecture**: `docs/SYSTEM_ARCHITECTURE.md` - Detailed system architecture
- **API Documentation**: `NLM_API_Documentation.md` - Complete Python API reference
- **C++ API Reference**: `NLM_Cpp_API_Reference.md` - Complete C++ API documentation
- **Phase 6 Final Audit**: `docs/PHASE6_FINAL_AUDIT.md` - Detailed audit of Phase 6 implementation
- **Phase 6 Results**: `docs/PHASE6_FINAL_RESULTS.md` - Performance metrics and experimental results

---

*This guide provides comprehensive best practices and performance tuning recommendations for NLM Phase 6 integration. Follow these guidelines to optimize your Phase 6 brain simulations for the best performance and results.*