# NLM - Comprehensive Usage Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that implements an artificial neural system inspired by biological principles. This document provides comprehensive examples and documentation for using the NLM Python bindings.

## Key Features Improved

### Python Bindings Enhancements

#### 1. Modular Design
- Better separation of concerns in the Python bindings
- More intuitive class hierarchy and method organization
- Consistent naming conventions across all classes

#### 2. Enhanced Error Handling
- Specific Python exception classes for different error types:
  - `ConfigError` for configuration-related issues
  - `BrainError` for brain operation errors
  - `WorldError` for world interaction problems
  - `SimulationError` for simulation issues

#### 3. Rich Convenience Functions
- Factory methods for creating common configurations
- Simulation utilities for running complete brain-in-world experiments
- Diagnostic tools for brain health monitoring
- Configuration presets optimized for different use cases

#### 4. Pythonic API Design
- Context manager support for proper resource management
- Rich string representations for debugging
- Type-safe getters and setters
- Convenient iterators for collections

### Configuration System Improvements

#### 1. Multiple Presets
- `LearningConfig` - Optimized for learning and adaptation
- `ExplorationConfig` - Optimized for discovery and novelty seeking
- `StabilityConfig` - Optimized for stability and predictability
- `BalancedConfig` - General-purpose balanced configuration
- `ResearchConfig` - High-performance research configuration

#### 2. Enhanced Validation
- Type-specific validation rules
- Range checking for numeric parameters
- Format validation for string parameters
- Dependency validation between parameters

#### 3. Better Defaults
- More sensible default values for all parameters
- Configuration templates for different application scenarios
- Progressive initialization with sensible fallbacks

## Comprehensive Examples

### Example 1: Quick Start - Learning Brain

This example demonstrates creating a learning brain that adapts to its environment.

```python
import pynlm

# Create a learning-focused configuration
config = pynlm.getLearningConfig()
config.setFloat("curiosity_strength", 0.5)  # High curiosity for exploration
config.setFloat("novelty_threshold", 0.3)  # Sensitive to novel experiences
config.setBool("development_enabled", True)  # Enable brain development

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run learning simulation
for step in range(500):
    world.update(0.1)
    
    # Get sensory input and process it
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks and makes decisions
    brain.step(step)
    action = agent.decodeMotorCommand()
    
    # Take action and receive reward
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation for learning
    agent.applyRewardModulation(result.reward, result.reward * 0.8)
    
    # Print progress
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Action: {action.getName()}")
        print(f"  Reward: {result.reward}")

print("Learning simulation complete!")
print(f"Final brain stats: {pynlm.brain_summary(brain)}")
```

### Example 2: Exploratory Agent

This example focuses on exploration behavior with high novelty detection.

```python
import pynlm
import time

def run_exploratory_agent(duration_steps=1000):
    """Run an agent that explores its environment extensively."""
    
    # Create exploratory configuration
    config = pynlm.getExplorationConfig()
    config.setInt("neuron_count", 2000)  # Larger brain for more capacity
    config.setFloat("curiosity_strength", 0.8)  # Very high curiosity
    config.setFloat("novelty_threshold", 0.2)  # Low threshold for novelty
    config.setBool("structural_plasticity_enabled", True)  # Brain can grow
    
    # Initialize brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.setMaxEnergy(100.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable exploration systems
    agent.enableCuriosity(True)
    agent.enableNovelty(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Track exploration metrics
    exploration_log = []
    novelty_events = 0
    area_visited = set()
    
    start_time = time.time()
    
    for step in range(duration_steps):
        world.update(0.1)
        
        # Get current position
        body = world.getAgentBody()
        pos = (body.x, body.y)
        area_visited.add(pos)
        
        # Check for novel experiences
        if agent.getNoveltyLevel() > 0.5:
            novelty_events += 1
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain makes decisions
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Take action
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, result.reward * 0.9)
        
        # Log exploration data
        if step % 100 == 0:
            exploration_log.append({
                'step': step,
                'novelty': agent.getNoveltyLevel(),
                'curiosity': agent.getCuriosityLevel(),
                'action': action.getName(),
                'reward': result.reward,
                'position': pos,
                'visited_areas': len(area_visited)
            })
    
    end_time = time.time()
    
    # Generate exploration report
    report = {
        'total_steps': duration_steps,
        'novelty_events': novelty_events,
        'unique_areas_visited': len(area_visited),
        'exploration_density': len(area_visited) / (30 * 30),  # What fraction of grid visited
        'average_novelty': sum(log['novelty'] for log in exploration_log) / len(exploration_log),
        'total_time': end_time - start_time,
        'steps_per_second': duration_steps / (end_time - start_time),
        'brain_stats': pynlm.get_system_stats(brain)
    }
    
    return report, exploration_log

# Run exploratory agent
print("Starting exploratory agent simulation...")
report, logs = run_exploratory_agent(1000)

print("\n=== EXPLORATION REPORT ===")
print(f"Total steps: {report['total_steps']}")
print(f"Novelty events: {report['novelty_events']}")
print(f"Unique areas visited: {report['unique_areas_visited']}")
print(f"Exploration density: {report['exploration_density']:.2%}")
print(f"Average novelty level: {report['average_novelty']:.3f}")
print(f"Simulation speed: {report['steps_per_second']:.1f} steps/second")

print("\n=== BRAIN STATISTICS ===")
for key, value in report['brain_stats'].items():
    print(f"{key}: {value}")
```

### Example 3: Development Agent

This example demonstrates brain development and maturation over time.

```python
import pynlm

def run_development_simulation(total_steps=3000):
    """Simulate brain development over time."""
    
    # Use balanced configuration that allows development
    config = pynlm.getBalancedConfig()
    config.setInt("neuron_count", 1500)
    config.setBool("development_enabled", True)
    config.setBool("structural_plasticity_enabled", True)
    config.set("developmental_stage", 1)  # Start with Initial stage
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all development systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Track development stages
    development_log = []
    stage_transitions = []
    previous_stage = agent.getDevelopmentalStage()
    
    # Simulation phases
    phases = {
        'initial': {'steps': 500, 'description': 'Initial development phase'},
        'critical_period': {'steps': 1000, 'description': 'Critical period for learning'},
        'maturation': {'steps': 1500, 'description': 'Brain maturation phase'},
    }
    
    for phase_name, phase_info in phases.items():
        print(f"\n=== {phase_info['description']} ===")
        
        for step in range(phase_info['steps']):
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processes and potentially develops
            brain.step(step)
            action = agent.decodeMotorCommand()
            
            # Take action and provide learning feedback
            result = world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(result.reward, result.reward * 0.85)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Log development state
            current_stage = agent.getDevelopmentalStage()
            if current_stage != previous_stage:
                stage_transitions.append({
                    'from_stage': previous_stage,
                    'to_stage': current_stage,
                    'step': step,
                    'time': step * 0.1
                })
                previous_stage = current_stage
            
            development_log.append({
                'step': step,
                'stage': current_stage,
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'neurosynapses': brain.getTotalSynapseCount(),
                'neurons_firing': brain.getFiringNeuronCount()
            })
            
            if step % 200 == 0:
                print(f"  Stage {current_stage}, Step {step}, "
                      f"Neurons: {brain.getTotalNeuronCount()}, "
                      f"Synaptuses: {brain.getTotalSynapseCount()}")
    
    # Generate development analysis
    analysis = analyze_development(development_log, stage_transitions)
    
    return {
        'development_log': development_log,
        'stage_transitions': stage_transitions,
        'analysis': analysis
    }

def analyze_development(log, transitions):
    """Analyze development patterns and transitions."""
    if not log:
        return {}
    
    stages = set(entry['stage'] for entry in log)
    
    # Calculate stage statistics
    stage_stats = {}
    for stage in stages:
        stage_entries = [e for e in log if e['stage'] == stage]
        if stage_entries:
            stage_stats[stage] = {
                'average_curiosity': sum(e['curiosity'] for e in stage_entries) / len(stage_entries),
                'average_novelty': sum(e['novelty'] for e in stage_entries) / len(stage_entries),
                'synapse_growth': (stage_entries[-1]['neurosynapses'] - stage_entries[0]['neurosynapses']) / stage_entries[0]['neurosynapses'] * 100
            }
    
    return {
        'total_stages': len(stages),
        'stage_transitions': len(transitions),
        'stage_statistics': stage_stats,
        'final_stage': log[-1]['stage'] if log else None,
        'total_neurons_at_end': log[-1]['neurosynapses'] if log else 0
    }

# Run development simulation
print("Starting brain development simulation...")
result = run_development_simulation()

print("\n=== DEVELOPMENT ANALYSIS ===")
analysis = result['analysis']
print(f"Total developmental stages: {analysis['total_stages']}")
print(f"Stage transitions: {analysis['stage_transitions']}")
print(f"Final developmental stage: {analysis['final_stage']}")

print("\n=== STAGE STATISTICS ===")
for stage, stats in analysis['stage_statistics'].items():
    print(f"Stage {stage}:")
    print(f"  Average curiosity: {stats['average_curiosity']:.3f}")
    print(f"  Average novelty: {stats['average_novelty']:.3f}")
    print(f"  Synapse growth: {stats['synapse_growth']:.1f}%")
```

### Example 4: Configuration Management

This example demonstrates advanced configuration management.

```python
import pynlm

def demonstrate_configuration_management():
    """Demonstrate advanced configuration management features."""
    
    print("=== CONFIGURATION MANAGEMENT DEMONSTRATION ===\n")
    
    # Start with a basic configuration
    config = pynlm.createDefaultConfig()
    
    # Show initial configuration
    print("1. Initial Configuration:")
    print(config.summary())
    print()
    
    # Modify specific parameters
    print("2. Modifying specific parameters...")
    config.setFloat("learning_rate", 0.005)
    config.setBool("development_enabled", True)
    config.setInt("neuron_count", 2000)
    
    print("3. Modified Configuration:")
    print(config.summary())
    print()
    
    # Demonstrate configuration presets
    print("4. Configuration Presets:")
    
    # Learning configuration
    learning_config = pynlm.getLearningConfig()
    print("\n   Learning Configuration:")
    print(f"   Neurons: {learning_config.getInt('neuron_count', 0)}")
    print(f"   Curiosity: {learning_config.getFloat('curiosity_strength', 0.0)}")
    print(f"   Development: {learning_config.getBool('development_enabled', False)}")
    
    # Exploration configuration  
    exploration_config = pynlm.getExplorationConfig()
    print("\n   Exploration Configuration:")
    print(f"   Neurons: {exploration_config.getInt('neuron_count', 0)}")
    print(f"   Curiosity: {exploration_config.getFloat('curiosity_strength', 0.0)}")
    print(f"   Novelty threshold: {exploration_config.getFloat('novelty_threshold', 0.0)}")
    
    # Balanced configuration
    balanced_config = pynlm.getBalancedConfig()
    print("\n   Balanced Configuration:")
    print(f"   Neurons: {balanced_config.getInt('neuron_count', 0)}")
    print(f"   Learning rate: {balanced_config.getFloat('learning_rate', 0.0)}")
    print(f"   Development: {balanced_config.getBool('development_enabled', False)}")
    
    # Demonstrate configuration validation
    print("\n5. Configuration Validation:")
    is_valid = config.validate()
    print(f"   Configuration valid: {is_valid}")
    
    errors = config.getValidationErrors()
    if errors:
        print(f"   Validation errors:")
        for error in errors:
            print(f"     - {error}")
    else:
        print("   No validation errors found.")
    
    # Demonstrate configuration comparison
    print("\n6. Configuration Comparison:")
    
    def compare_configs(config1, config2, name1="Config 1", name2="Config 2"):
        keys1 = config1.getKeys()
        keys2 = config2.getKeys()
        
        all_keys = set(keys1) | set(keys2)
        differences = []
        
        for key in all_keys:
            val1 = config1.get(key)
            val2 = config2.get(key)
            
            if val1 != val2:
                differences.append((key, val1, val2))
        
        print(f"\n   Differences between {name1} and {name2}:")
        for key, val1, val2 in differences:
            print(f"     {key}: {val1} vs {val2}")
        
        return differences
    
    compare_configs(learning_config, exploration_config, "Learning", "Exploration")
    
    # Demonstrate configuration save/load
    print("\n7. Configuration Save/Load:")
    print("   (In a real application, you would save to a file)")
    print("   Example save path: './my_brain_config.nlm'")
    
    # Create a sample world for other examples
    print("\n8. Creating sample world...")
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
    print(f"   World dimensions: {world.getWidth()}x{world.getHeight()}")
    print(f"   Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    
    return {
        'config': config,
        'presets': {
            'learning': learning_config,
            'exploration': exploration_config,
            'balanced': balanced_config
        },
        'world': world
    }

# Run configuration management demo
config_demo = demonstrate_configuration_management()
print("\nConfiguration management demonstration complete!")
```

### Example 5: Benchmarking and Performance Testing

This example demonstrates benchmarking capabilities.

```python
import pynlm
import time

def benchmark_brain_performance():
    """Benchmark brain creation and processing performance."""
    
    print("=== BRAIN PERFORMANCE BENCHMARKING ===\n")
    
    # Test 1: Brain creation speed
    print("1. Brain Creation Benchmark:")
    
    creation_times = []
    for i in range(5):
        config = pynlm.getBalancedConfig()
        config.setInt("neuron_count", 500 + i * 200)
        
        start_time = time.time()
        brain = pynlm.createBrain(config)
        brain.initialize()
        end_time = time.time()
        
        creation_time = (end_time - start_time) * 1000  # Convert to milliseconds
        creation_times.append(creation_time)
        
        print(f"   Test {i+1}: {creation_time:.2f} ms (neurons: {brain.getTotalNeuronCount()})")
    
    avg_creation_time = sum(creation_times) / len(creation_times)
    print(f"   Average creation time: {avg_creation_time:.2f} ms")
    print()
    
    # Test 2: Step processing speed
    print("2. Step Processing Benchmark:")
    
    config = pynlm.getBalancedConfig()
    config.setInt("neuron_count", 1000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    step_times = []
    for i in range(10):
        start_time = time.time()
        brain.step(i)
        end_time = time.time()
        
        step_time = (end_time - start_time) * 1000  # Convert to milliseconds
        step_times.append(step_time)
    
    avg_step_time = sum(step_times) / len(step_times)
    min_step_time = min(step_times)
    max_step_time = max(step_times)
    
    print(f"   Average step time: {avg_step_time:.3f} ms")
    print(f"   Min step time: {min_step_time:.3f} ms")
    print(f"   Max step time: {max_step_time:.3f} ms")
    print(f"   Steps per second: {1000 / avg_step_time:.0f}")
    print()
    
    # Test 3: Full simulation benchmark
    print("3. Full Simulation Benchmark:")
    
    def run_simulation(brain_config, steps=100):
        config = brain_config
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for i in range(steps):
            brain.step(i)
        end_time = time.time()
        
        total_time = end_time - start_time
        steps_per_second = steps / total_time
        
        return {
            'total_time': total_time,
            'steps_per_second': steps_per_second,
            'time_per_step_ms': (total_time / steps) * 1000
        }
    
    # Benchmark with different brain sizes
    brain_sizes = [500, 1000, 1500, 2000]
    simulation_results = []
    
    for size in brain_sizes:
        config = pynlm.getBalancedConfig()
        config.setInt("neuron_count", size)
        
        result = run_simulation(config, steps=50)
        simulation_results.append({
            'neurons': size,
            'total_time': result['total_time'],
            'steps_per_second': result['steps_per_second'],
            'time_per_step_ms': result['time_per_step_ms']
        })
        
        print(f"   {size} neurons: {result['steps_per_second']:.0f} steps/second "
              f"({result['time_per_step_ms']:.3f} ms/step)")
    
    # Analyze scaling
    print("\n4. Performance Scaling Analysis:")
    
    if len(simulation_results) >= 2:
        base_neuron_count = simulation_results[0]['neurons']
        base_steps_per_sec = simulation_results[0]['steps_per_second']
        
        for result in simulation_results[1:]:
            neuron_ratio = result['neurons'] / base_neuron_count
            step_ratio = result['steps_per_second'] / base_steps_per_sec
            
            print(f"   {result['neurons']} neurons vs {base_neuron_count} neurons:")
            print(f"     Neuron count ratio: {neuron_ratio:.1f}x")
            print(f"     Performance ratio: {step_ratio:.1f}x")
            print(f"     Scaling efficiency: {step_ratio/neuron_ratio:.2f}")
    
    # Test 4: Memory efficiency
    print("\n5. Memory Efficiency Test:")
    
    import sys
    
    config = pynlm.getBalancedConfig()
    config.setInt("neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Estimate memory usage (rough approximation)
    memory_info = sys.getsizeof(brain) + 1024  # Add rough estimate for internal data
    
    print(f"   Estimated memory usage: {memory_info / 1024:.1f} KB")
    print(f"   Memory per neuron: {memory_info / 1000:.2f} KB")
    
    # Test 5: Concurrent operations
    print("\n6. Concurrent Operations Test:")
    
    def concurrent_operations_test():
        config = pynlm.getBalancedConfig()
        config.setInt("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Perform multiple operations concurrently
        start_time = time.time()
        
        for i in range(20):
            # Multiple operations in parallel
            brain.step(i)
            _ = brain.getTotalNeuronCount()
            _ = brain.getTotalSynapseCount()
            _ = brain.getAverageFiringRate()
            _ = brain.getExcitationInhibitionRatio()
            _ = brain.getDevelopmentalStage()
        
        end_time = time.time()
        
        total_time = end_time - start_time
        operations = 20 * 5  # 20 iterations × 5 operations each
        
        return {
            'total_time': total_time,
            'operations_per_second': operations / total_time,
            'average_operation_time_ms': (total_time / operations) * 1000
        }
    
    result = concurrent_operations_test()
    print(f"   Operations per second: {result['operations_per_second']:.0f}")
    print(f"   Average operation time: {result['average_operation_time_ms']:.3f} ms")
    
    return {
        'creation_times': creation_times,
        'step_times': step_times,
        'simulation_results': simulation_results,
        'concurrent_result': result
    }

# Run benchmarking
benchmark_results = benchmark_brain_performance()
print("\nPerformance benchmarking complete!")
```

## Utility Functions and Helper Methods

### Diagnostic Tools

The NLM bindings provide several utility functions for diagnostics and debugging:

```python
# Get detailed brain summary
brain_summary = pynlm.brain_summary(brain)

# Get world summary  
world_summary = pynlm.world_summary(world)

# Get system statistics
stats = pynlm.get_system_stats(brain)

# Validate configurations
brain_errors = pynlm.validate_brain(brain)
world_errors = pynlm.validate_world(world)

# Quick help and examples
examples = pynlm.examples()
```

### Configuration Presets

Predefined configuration presets optimized for different use cases:

```python
# Get different configuration presets
learning_config = pynlm.getLearningConfig()
exploration_config = pynlm.getExplorationConfig()
balanced_config = pynlm.getBalancedConfig()
stability_config = pynlm.getStabilityConfig()
research_config = pynlm.getResearchConfig()
```

### Factory Methods

Convenient factory methods for creating common brain-agent pairs:

```python
# Create learning agent
brain, agent = pynlm.create_learning_agent_example()

# Create exploration agent  
brain, agent = pynlm.create_exploration_agent_example()

# Create development agent
brain, agent = pynlm.create_development_agent_example()

# Create simple simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
```

## Advanced Usage Patterns

### 1. Pipeline Processing

```python
def create_processing_pipeline(config_preset="balanced"):
    """Create a complete processing pipeline."""
    
    # Step 1: Create configuration
    if config_preset == "balanced":
        config = pynlm.getBalancedConfig()
    elif config_preset == "exploratory":
        config = pynlm.getExplorationConfig()
    else:
        config = pynlm.createDefaultConfig()
    
    # Step 2: Create brain
    brain = pynlm.createBrain(config)
    
    # Step 3: Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    # Step 4: Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Step 5: Configure learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'config': config
    }

# Use the pipeline
pipeline = create_processing_pipeline("exploratory")
```

### 2. Batch Processing

```python
def run_batch_simulation(config, scenarios, steps_per_scenario):
    """Run multiple simulation scenarios in batch."""
    
    results = []
    
    for scenario_name, scenario_config in scenarios.items():
        print(f"Running scenario: {scenario_name}")
        
        # Merge scenario config with base config
        merged_config = config
        for key, value in scenario_config.items():
            merged_config.set(key, value)
        
        # Create and run simulation
        brain = pynlm.createBrain(merged_config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(steps_per_scenario):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record results
        results.append({
            'scenario': scenario_name,
            'final_neurons': brain.getTotalNeuronCount(),
            'final_spikes': brain.getTotalSpikeCount(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'performance': brain.getAverageFiringRate()
        })
    
    return results

# Define multiple scenarios
scenarios = {
    'basic': {
        'neuron_count': 500,
        'curiosity_strength': 0.1
    },
    'enhanced': {
        'neuron_count': 1000,
        'curiosity_strength': 0.5,
        'development_enabled': True
    },
    'exploratory': {
        'neuron_count': 1500,
        'curiosity_strength': 0.8,
        'novelty_threshold': 0.2
    }
}

# Run batch simulation
results = run_batch_simulation(pynlm.getBalancedConfig(), scenarios, 100)
```

### 3. Continuous Learning

```python
class ContinuousLearningSystem:
    """Manage continuous learning over time."""
    
    def __init__(self, config_preset="balanced"):
        self.config = pynlm.getBalancedConfig() if config_preset == "balanced" else pynlm.getExplorationConfig()
        self.world = pynlm.createSimpleWorld()
        self.brain = None
        self.agent = None
        self.step_count = 0
        self.episode_count = 0
    
    def initialize(self, world_config=None):
        """Initialize the learning system."""
        if world_config:
            self.world.configure(**world_config)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        print("Continuous learning system initialized.")
    
    def process_episode(self, max_steps=100):
        """Process a single learning episode."""
        episode_data = {
            'steps': [],
            'rewards': [],
            'actions': [],
            'novelty_levels': []
        }
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain makes decision
            self.brain.step(self.step_count)
            action = self.agent.decodeMotorCommand()
            
            # Take action
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply learning
            self.agent.applyRewardModulation(result.reward, result.reward * 0.9)
            
            # Record data
            episode_data['steps'].append(self.step_count)
            episode_data['rewards'].append(result.reward)
            episode_data['actions'].append(action.getName())
            episode_data['novelty_levels'].append(self.agent.getNoveltyLevel())
            
            self.step_count += 1
            
            # Check for episode termination
            if len(self.world.getObjects()) > 0:
                # Simple termination condition
                break
        
        self.episode_count += 1
        return episode_data
    
    def run_continuous_learning(self, episodes=10, steps_per_episode=100):
        """Run continuous learning over multiple episodes."""
        print(f"Starting continuous learning: {episodes} episodes, {steps_per_episode} steps each")
        
        all_episodes = []
        learning_summary = {
            'total_steps': 0,
            'total_curiosity': 0.0,
            'total_novelty': 0.0,
            'average_reward': 0.0,
            'action_distribution': {}
        }
        
        for episode in range(episodes):
            print(f"Episode {episode + 1}/{episodes}")
            
            episode_data = self.process_episode(steps_per_episode)
            all_episodes.append(episode_data)
            
            # Update learning summary
            learning_summary['total_steps'] += len(episode_data['steps'])
            learning_summary['total_curiosity'] += sum(episode_data['novelty_levels'])
            learning_summary['total_novelty'] += sum(episode_data['novelty_levels'])
            learning_summary['average_reward'] += sum(episode_data['rewards'])
            
            for action in episode_data['actions']:
                learning_summary['action_distribution'][action] = \
                    learning_summary['action_distribution'].get(action, 0) + 1
            
            print(f"  Final curiosity: {episode_data['novelty_levels'][-1]:.3f}")
            print(f"  Average reward: {sum(episode_data['rewards']) / len(episode_data['rewards']):.3f}")
        
        # Calculate final summary
        learning_summary['average_curiosity'] = \
            learning_summary['total_curiosity'] / learning_summary['total_steps']
        learning_summary['average_novelty'] = \
            learning_summary['total_novelty'] / learning_summary['total_steps']
        learning_summary['average_reward'] = \
            learning_summary['average_reward'] / episodes
        
        return {
            'episodes': all_episodes,
            'summary': learning_summary
        }

# Use continuous learning system
learning_system = ContinuousLearningSystem()
learning_system.initialize({
    'width': 15,
    'height': 15,
    'visionWidth': 6,
    'visionHeight': 6
})

results = learning_system.run_continuous_learning(episodes=5, steps_per_episode=50)
```

## Installation and Setup

### Prerequisites

```bash
# Install Python dependencies
pip install pybind11 numpy

# Install NLM Python bindings
pip install -e .  # From NLM root directory
```

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j4

# Install Python bindings
pip install .
```

### Quick Test

```python
import pynlm

print("Testing NLM Python bindings...")

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Test basic functionality
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Brain step: {brain.step(0)}")

# Create world and agent
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run a simple simulation
for i in range(10):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Basic test passed!")
print(f"Final brain stats: {pynlm.brain_summary(brain)}")
```

## Troubleshooting

### Common Issues

#### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if sensory input is being processed

#### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify the agent's motor neurons are active

#### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration parameters

#### "Configuration validation fails"
- Check that all required parameters are set
- Verify parameter types and ranges
- Run with validation disabled temporarily: `config.validateAll = False`

### Debugging Tips

1. **Use the diagnostic tools**:
   ```python
   print(pynlm.brain_summary(brain))
   print(pynlm.world_summary(world))
   ```

2. **Check configuration**:
   ```python
   print(config.summary())
   errors = config.getValidationErrors()
   ```

3. **Enable verbose logging**:
   ```python
   # Add logging to your simulation loop
   if verbose:
       print(f"Step {step}: Firing neurons = {brain.getFiringNeuronCount()}")
   ```

4. **Test with smaller configurations**:
   ```python
   config = pynlm.getBalancedConfig()
   config.setInt("neuron_count", 100)  # Smaller brain for testing
   ```

## Performance Recommendations

### For Production Use

1. **Use configuration presets**: Choose from `learning`, `exploration`, `balanced`, or `stability` presets
2. **Optimize brain size**: Use the smallest brain size that meets your requirements
3. **Enable efficient learning**: Only enable learning systems you actually need
4. **Use appropriate simulation parameters**: Match timestep duration to your use case

### For Research

1. **Use research configuration**: `pynlm.getResearchConfig()` for maximum flexibility
2. **Monitor system resources**: Track memory usage and CPU time
3. **Use checkpointing**: Save and load brain states for long-running experiments
4. **Batch process**: Run multiple simulations with different parameters

### For Development

1. **Start small**: Test with 100-500 neurons initially
2. **Use balanced configuration**: For general development and testing
3. **Enable debugging tools**: Use verbose logging and validation
4. **Iterate quickly**: Make small changes and test frequently

## Version Information

```python
import pynlm

print(f"NLM Python Bindings Version: {pynlm.__version__}")
print(f"Author: {pynlm.__author__}")
print(f"Description: {pynlm.__description__}")

info = pynlm.get_version_info()
print(f"Version tuple: {info}")
```

## License

MIT License. See LICENSE file for details.

---

This comprehensive guide covers all the improvements made to the NLM Python bindings, including enhanced error handling, configuration management, factory methods, and advanced usage patterns. The examples demonstrate practical applications and best practices for using the NLM framework in various scenarios.