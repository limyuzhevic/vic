# NLM Python API Examples

## Overview

This document provides comprehensive examples for using the enhanced NLM Python API. It includes:

- **Basic Examples**: Simple usage examples for beginners
- **Advanced Examples**: Complex usage examples for power users
- **Tutorial Examples**: Step-by-step tutorials for learning
- **Research Examples**: Examples for scientific research
- **Performance Examples**: Examples for performance optimization

## Table of Contents

1. [Basic Examples](#basic-examples)
   - [Hello World](#hello-world)
   - [Creating a Brain](#creating-a-brain)
   - [Basic Simulation](#basic-simulation)
   - [Configuration Management](#configuration-management)
2. [Advanced Examples](#advanced-examples)
   - [Parameter Sweep Experiment](#parameter-sweep-experiment)
   - [Multi-Environment Testing](#multi-environment-testing)
   - [Agent Research](#agent-research)
   - [Performance Benchmarking](#performance-benchmarking)
   - [Custom Neuron Populations](#custom-neuron-populations)
   - [Batch Processing](#batch-processing)
3. [Tutorial Examples](#tutorial-examples)
   - [Tutorial: Basic Brain Simulation](#tutorial-basic-brain-simulation)
   - [Tutorial: Environment Configuration](#tutorial-environment-configuration)
   - [Tutorial: Performance Monitoring](#tutorial-performance-monitoring)
   - [Tutorial: Experiment Management](#tutorial-experiment-management)
4. [Research Examples](#research-examples)
   - [Example: Memory System Integration](#example-memory-system-integration)
   - [Example: Neuromodulation Research](#example-neuromodulation-research)
   - [Example: Developmental Studies](#example-developmental-studies)
   - [Example: Social Learning](#example-social-learning)
5. [Performance Examples](#performance-examples)
   - [Example: Simulation Optimization](#example-simulation-optimization)
   - [Example: Memory Optimization](#example-memory-optimization)
   - [Example: Parallel Processing](#example-parallel-processing)

## Basic Examples

### Hello World

This example demonstrates the basic usage of the NLM Python API.

```python
#!/usr/bin/env python3
"""
Hello World example for NLM Python API
"""

from nlm import EnhancedBrain, World, ActionType

def main():
    print("NLM Python API Hello World Example")
    print("=" * 50)
    
    # Create enhanced brain
    brain = EnhancedBrain()
    
    # Create world
    world = World()
    world.configure(100, 100, 10, 10)
    world.setAgentStart(50, 50)
    
    # Initialize brain
    if brain.initialize():
        print("✓ Brain initialized successfully")
    else:
        print("✗ Brain initialization failed")
        return
    
    # Run simulation
    print("\nRunning simulation...")
    for step in range(10):
        # Get sensory input
        percept = world.getPercept()
        
        # Create simple action
        action = Action(type=ActionType.Wait)
        
        # Apply action
        world.applyAction(action)
        
        # Step world
        world.step(0.01)
        
        print(f"Step {step + 1}: Action = {action.getType()}, "
              f"Neurons: {brain.getTotalNeuronCount()}")
    
    # Print final statistics
    print("\nFinal Statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    print("\nHello World example completed successfully!")

if __name__ == "__main__":
    main()
```

### Creating a Brain

This example demonstrates creating a brain with custom configuration.

```python
#!/usr/bin/env python3
"""
Creating a Brain example for NLM Python API
"""

from nlm import PyConfig, EnhancedBrain, DevelopmentalStage

def main():
    print("Creating a Brain with Custom Configuration")
    print("=" * 50)
    
    # Create enhanced configuration
    config = PyConfig()
    
    # Set configuration values
    config.setInt("world.width", 200)
    config.setInt("world.height", 200)
    config.setFloat("energy.decay_rate", 0.01)
    config.setBool("agent.enabled", True)
    config.setString("experiment.name", "my_experiment")
    config.setInt("random.seed", 42)
    
    # Display configuration
    print("Configuration set:")
    print(f"  World width: {config.getInt('world.width', 100)}")
    print(f"  World height: {config.getInt('world.height', 100)}")
    print(f"  Energy decay rate: {config.getFloat('energy.decay_rate', 0.05)}")
    print(f"  Agent enabled: {config.getBool('agent.enabled', False)}")
    print(f"  Experiment name: {config.getString('experiment.name', 'default')}")
    print(f"  Random seed: {config.getInt('random.seed', 0)}")
    
    # Create brain with configuration
    brain = EnhancedBrain()
    print(f"\nBrain created with {brain.getTotalNeuronCount()} neurons")
    
    # Set developmental stage
    brain.setDevelopmentalStage(DevelopmentalStage.Maturation)
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    print("\nBrain creation example completed successfully!")

if __name__ == "__main__":
    main()
```

### Basic Simulation

This example demonstrates a basic simulation loop.

```python
#!/usr/bin/env python3
"""
Basic Simulation example for NLM Python API
"""

from nlm import EnhancedBrain, World, Action, ActionType

def main():
    print("Basic Simulation Example")
    print("=" * 50)
    
    # Create brain and world
    brain = EnhancedBrain()
    world = World()
    world.configure(150, 150, 15, 15)
    world.setAgentStart(75, 75)
    
    # Initialize brain
    brain.initialize()
    
    # Simulation parameters
    num_steps = 50
    simulation_time = 0.0
    
    print(f"Running {num_steps} simulation steps...")
    
    # Main simulation loop
    for step in range(num_steps):
        # Record simulation time
        simulation_time = world.getSimulationTime()
        
        # Get current state
        agent_body = world.getAgentBody()
        percept = world.getSensoryPercept()
        
        # Create action based on percept
        if brain.getActiveNeuronCount() > 0:
            action = Action(type=ActionType.Interact)
        else:
            action = Action(type=ActionType.Wait)
        
        # Apply action
        world.applyAction(action, simulation_time)
        
        # Update world
        world.update(0.01)
        
        # Print progress every 10 steps
        if (step + 1) % 10 == 0:
            print(f"  Step {step + 1}/{num_steps}: "
                  f"Position ({agent_body.x:.1f}, {agent_body.y:.1f}), "
                  f"Energy {agent_body.energy:.1f}, "
                  f"Actions: {brain.getTotalSpikeCount()}")
    
    # Print final statistics
    print("\nSimulation Results:")
    print(f"  Total simulation time: {simulation_time:.2f}")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Save brain state
    brain.save("simulation_result.json")
    print(f"\nBrain state saved to: simulation_result.json")
    
    print("\nBasic simulation example completed successfully!")

if __name__ == "__main__":
    main()
```

### Configuration Management

This example demonstrates configuration management with environment variables.

```python
#!/usr/bin/env python3
"""
Configuration Management example for NLM Python API
"""

from nlm import PyConfig, EnhancedBrain, EnvironmentConfig, ValidatedConfig

def main():
    print("Configuration Management Example")
    print("=" * 50)
    
    # Create base configuration
    config = PyConfig()
    config.setInt("world.width", 100)
    config.setInt("world.height", 100)
    config.setFloat("energy.decay_rate", 0.01)
    
    # Load environment variables
    env_config = EnvironmentConfig()
    env_config.loadFromEnvironment(config)
    
    # Add validation
    validated_config = ValidatedConfig(config)
    validated_config.addValidationRule("world.width", lambda x: x > 0, "Width must be positive")
    validated_config.addValidationRule("world.height", lambda x: x > 0, "Height must be positive")
    validated_config.addValidationRule("energy.decay_rate", lambda x: 0 <= x <= 1, "Decay rate must be between 0 and 1")
    
    # Check validation
    if validated_config.isValid():
        print("✓ Configuration is valid")
        
        # Create brain with validated configuration
        brain = EnhancedBrain()
        print(f"✓ Brain created successfully with {brain.getTotalNeuronCount()} neurons")
    else:
        print(f"✗ Configuration validation errors: {validated_config.getValidationErrors()}")
    
    # Display configuration
    print("\nConfiguration Summary:")
    print(f"  World dimensions: {config.getInt('world.width', 100)} x {config.getInt('world.height', 100)}")
    print(f"  Energy decay rate: {config.getFloat('energy.decay_rate', 0.05)}")
    
    print("\nConfiguration management example completed successfully!")

if __name__ == "__main__":
    main()
```

## Advanced Examples

### Parameter Sweep Experiment

This example demonstrates running a parameter sweep experiment.

```python
#!/usr/bin/env python3
"""
Parameter Sweep Experiment example for NLM Python API
"""

from nlm import AdvancedExperiment, PyConfig, EnhancedBrain, Action, ActionType
import numpy as np

def brain_factory(params):
    """Create brain with specific parameters."""
    config = PyConfig()
    config.setInt("world.width", params.get('width', 100))
    config.setInt("world.height", params.get('height', 100))
    config.setInt("random.seed", params.get('seed', 42))
    config.setFloat("energy.decay_rate", params.get('decay_rate', 0.01))
    return EnhancedBrain(config)

def main():
    print("Parameter Sweep Experiment Example")
    print("=" * 50)
    
    # Create experiment
    experiment = AdvancedExperiment("parameter_sweep")
    
    # Define parameter grid
    param_grid = {
        'width': [50, 100, 200],
        'height': [50, 100, 200],
        'seed': [42, 123, 456],
        'decay_rate': [0.005, 0.01, 0.02]
    }
    
    # Run parameter sweep
    print(f"Running parameter sweep with {len(param_grid['width']) * len(param_grid['height']) * len(param_grid['seed']) * len(param_grid['decay_rate'])} combinations...")
    
    results = experiment.run_parameter_sweep(param_grid, brain_factory)
    
    # Analyze results
    successful_experiments = [r for r in results if r['success']]
    
    print(f"\nResults Summary:")
    print(f"  Total combinations: {len(results)}")
    print(f"  Successful experiments: {len(successful_experiments)}")
    print(f"  Success rate: {len(successful_experiments) / len(results):.2%}")
    
    # Find best experiment
    if successful_experiments:
        best_experiment = None
        best_fitness = 0
        
        for experiment_result in successful_experiments:
            fitness = (experiment_result['result']['statistics']['efficiency'] *
                      experiment_result['result']['statistics']['average_firing_rate'])
            
            if fitness > best_fitness:
                best_fitness = fitness
                best_experiment = experiment_result
        
        if best_experiment:
            print(f"\nBest Experiment:")
            print(f"  Parameters: {best_experiment['parameters']}")
            print(f"  Fitness: {best_fitness:.4f}")
            print(f"  Final efficiency: {best_experiment['result']['statistics']['efficiency']:.3f}")
            print(f"  Final firing rate: {best_experiment['result']['statistics']['average_firing_rate']:.2f} Hz")
    
    # Save results
    experiment.save_results("parameter_sweep_results.json")
    print(f"\nResults saved to: parameter_sweep_results.json")
    
    print("\nParameter sweep experiment example completed successfully!")

if __name__ == "__main__":
    main()
```

### Multi-Environment Testing

This example demonstrates testing across different environments.

```python
#!/usr/bin/env python3
"""
Multi-Environment Testing example for NLM Python API
"""

from nlm import AdvancedExperiment, PyConfig, EnhancedBrain, Action, ActionType

def brain_factory(env_config):
    """Create brain for specific environment."""
    config = PyConfig()
    config.setInt("world.width", env_config['width'])
    config.setInt("world.height", env_config['height'])
    config.setFloat("energy.decay_rate", env_config.get('decay_rate', 0.01))
    config.setInt("random.seed", env_config.get('seed', 42))
    return EnhancedBrain(config)

def main():
    print("Multi-Environment Testing Example")
    print("=" * 50)
    
    # Create experiment
    experiment = AdvancedExperiment("multi_environment")
    
    # Define environments
    environments = [
        {'name': 'simple', 'width': 50, 'height': 50, 'decay_rate': 0.01, 'seed': 42, 'steps': 50},
        {'name': 'moderate', 'width': 100, 'height': 100, 'decay_rate': 0.05, 'seed': 123, 'steps': 100},
        {'name': 'complex', 'width': 200, 'height': 200, 'decay_rate': 0.1, 'seed': 456, 'steps': 200}
    ]
    
    # Run multi-environment experiment
    print(f"Running experiment in {len(environments)} environments...")
    
    results = experiment.run_multi_environment(environments, brain_factory)
    
    # Analyze results
    successful_envs = [r for r in results if r['success']]
    
    print(f"\nEnvironment Results:")
    print(f"  Total environments: {len(environments)}")
    print(f"  Successful environments: {len(successful_envs)}")
    print(f"  Success rate: {len(successful_envs) / len(environments):.2%}")
    
    for env_result in results:
        if env_result['success']:
            stats = env_result['environment_config']
            env_stats = env_result['statistics']
            print(f"\n  Environment: {stats['name']} ({stats['width']}x{stats['height']})")
            print(f"    Efficiency: {env_stats['efficiency']:.3f}")
            print(f"    Total actions: {env_stats['total_actions']}")
            print(f"    Brain stats: {env_stats['brain_stats']}")
    
    # Plot results
    experiment.plot_results("./plots")
    print(f"\nPlots saved to: ./plots")
    
    print("\nMulti-environment testing example completed successfully!")

if __name__ == "__main__":
    main()
```

### Agent Research

This example demonstrates custom agent research.

```python
#!/usr/bin/env python3
"""
Agent Research example for NLM Python API
"""

from nlm import AdvancedExperiment, CustomAgent, EnhancedBrain, Action, ActionType
import random

class CustomAgent:
    """Custom agent implementation for research."""
    
    def __init__(self):
        self.brain = None
        self.world = None
        self.behavior_policy = 'random'
        self.memory_capacity = 1000
        self.learning_rate = 0.01
        
    def set_behavior_policy(self, policy):
        self.behavior_policy = policy
        
    def set_memory_capacity(self, capacity):
        self.memory_capacity = capacity
        
    def set_learning_rate(self, rate):
        self.learning_rate = rate
        
    def getBrain(self):
        return self.brain
        
    def getWorld(self):
        return self.world

def agent_factory(episode):
    """Create custom agent for research."""
    agent = CustomAgent()
    
    # Configure agent
    agent.set_behavior_policy('exploration')
    agent.set_memory_capacity(1000)
    agent.set_learning_rate(0.01)
    
    return agent

def main():
    print("Agent Research Example")
    print("=" * 50)
    
    # Create experiment
    experiment = AdvancedExperiment("agent_research")
    
    # Run agent research experiment
    print("Running agent research experiment...")
    
    results = experiment.run_custom_agent(agent_factory, num_episodes=100)
    
    # Analyze results
    successful_episodes = [r for r in results if r['success']]
    
    print(f"\nAgent Research Results:")
    print(f"  Total episodes: {len(results)}")
    print(f"  Successful episodes: {len(successful_episodes)}")
    print(f"  Success rate: {len(successful_episodes) / len(results):.2%}")
    
    # Calculate average metrics
    if successful_episodes:
        avg_actions = np.mean([r['result']['action_count'] for r in successful_episodes if r['result']])
        avg_efficiency = np.mean([r['result']['brain_stats']['average_firing_rate'] for r in successful_episodes if r['result']])
        
        print(f"\nAverage Metrics:")
        print(f"  Average actions per episode: {avg_actions:.1f}")
        print(f"  Average firing rate: {avg_efficiency:.2f} Hz")
    
    # Save results
    experiment.save_results("agent_research_results.json")
    print(f"\nResults saved to: agent_research_results.json")
    
    print("\nAgent research example completed successfully!")

if __name__ == "__main__":
    main()
```

### Performance Benchmarking

This example demonstrates performance benchmarking.

```python
#!/usr/bin/env python3
"""
Performance Benchmarking example for NLM Python API
"""

from nlm import PerformanceProfiler, PyConfig, EnhancedBrain, SimpleWorld
import time

def benchmark_simulation():
    """Benchmark simulation function."""
    config = PyConfig()
    config.setInt("world.width", 100)
    config.setInt("world.height", 100)
    config.setInt("random.seed", 42)
    
    brain = EnhancedBrain(config)
    world = SimpleWorld()
    world.configure(100, 100, 10, 10)
    
    # Run simulation
    for step in range(100):
        world.update(0.01)
    
    return brain

def main():
    print("Performance Benchmarking Example")
    print("=" * 50)
    
    # Create profiler
    profiler = PerformanceProfiler()
    
    # Start profiling
    profiler.start_profiling()
    
    # Run benchmark
    print("Running simulation benchmark...")
    start_time = time.time()
    
    result = profiler.benchmark_function(benchmark_simulation)
    
    end_time = time.time()
    
    # Print results
    print(f"\nBenchmark Results:")
    print(f"  Function: {result.test_name}")
    print(f"  Execution time: {result.execution_time:.4f} seconds")
    print(f"  Memory usage: {result.memory_usage:.2f} MB")
    print(f"  Success: {result.success}")
    
    if result.success and result.metrics:
        print(f"  Metrics:")
        for key, value in result.metrics.items():
            print(f"    {key}: {value}")
    
    # Generate report
    report = profiler.generate_report()
    print(f"\nPerformance Report:")
    print(report)
    
    # Save results
    profiler.save_results("benchmark_results.json")
    print(f"\nBenchmark results saved to: benchmark_results.json")
    
    print("\nPerformance benchmarking example completed successfully!")

if __name__ == "__main__":
    main()
```

### Custom Neuron Populations

This example demonstrates custom neuron population creation.

```python
#!/usr/bin/env python3
"""
Custom Neuron Populations example for NLM Python API
"""

from nlm import EnhancedBrain, NeuronPopulation
import matplotlib.pyplot as plt

def main():
    print("Custom Neuron Populations Example")
    print("=" * 50)
    
    # Create brain and neuron population
    brain = EnhancedBrain()
    neuron_pop = NeuronPopulation(brain)
    
    # Create different neuron populations
    print("Creating neuron populations...")
    
    # Excitatory population
    excitatory_neurons = neuron_pop.createExcitatoryPopulation(200, 0.1)
    print(f"  Created {len(excitatory_neurons)} excitatory neurons")
    
    # Recurrent network
    recurrent_network = neuron_pop.createRecurrentNetwork(50, 25, 10)
    print(f"  Created recurrent network with {len(recurrent_network)} neurons")
    
    # Custom connectivity
    neuron_pop.createCustomConnectivity(
        sourceNeurons=excitatory_neurons[:10],
        targetNeurons=recurrent_network,
        weights=[0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5]
    )
    print("  Created custom connectivity pattern")
    
    # Get brain statistics
    print(f"\nBrain Statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    
    # Visualize results
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    
    # Plot 1: Neuron population sizes
    pop_sizes = [200, len(recurrent_network), len(excitatory_neurons)]
    axes[0].bar(['Excitatory', 'Recurrent', 'Total'], pop_sizes)
    axes[0].set_title('Neuron Population Sizes')
    axes[0].set_ylabel('Number of Neurons')
    
    # Plot 2: Connectivity
    axes[1].scatter([1, 2, 3, 4, 5], [1, 2, 3, 4, 5], alpha=0.7)
    axes[1].set_title('Connectivity Pattern')
    axes[1].set_xlabel('Source Neurons')
    axes[1].set_ylabel('Target Neurons')
    
    # Plot 3: Brain statistics
    stats = [brain.getTotalNeuronCount(), brain.getTotalSynapseCount(),
             brain.getActiveNeuronCount(), brain.getFiringNeuronCount()]
    axes[2].bar(['Total Neurons', 'Total Synapses', 'Active Neurons', 'Firing Neurons'], stats, color=['skyblue', 'lightgreen', 'salmon', 'gold'])
    axes[2].set_title('Brain Statistics')
    axes[2].set_ylabel('Count')
    
    plt.tight_layout()
    plt.savefig("./neuron_populations.png", dpi=150, bbox_inches='tight')
    print(f"\nVisualization saved to: neuron_populations.png")
    
    print("\nCustom neuron populations example completed successfully!")

if __name__ == "__main__":
    main()
```

### Batch Processing

This example demonstrates batch processing.

```python
#!/usr/bin/env python3
"""
Batch Processing example for NLM Python API
"""

from nlm import BatchProcessor, EnhancedBrain, SimpleWorld, Action, ActionType
import time

def main():
    print("Batch Processing Example")
    print("=" * 50)
    
    # Create batch processor
    batch_processor = BatchProcessor()
    
    # Create world
    world = SimpleWorld()
    world.configure(100, 100, 10, 10)
    
    # Create percepts
    print("Creating percepts...")
    percepts = []
    for i in range(10):
        # Create simple vision percept
        percepts.append(world.getSensoryPercept())
    
    # Process batch
    print("Processing batch...")
    actions = batch_processor.processBatch(percepts)
    
    print(f"Processed {len(percepts)} percepts, generated {len(actions)} actions")
    
    # Simulate batch
    print("Running batch simulation...")
    batch_results = batch_processor.simulateBatch(num_steps=50, batch_size=10)
    
    print(f"Batch simulation completed with {len(batch_results)} time steps")
    
    # Save batch results
    batch_processor.saveBatchResults(batch_results, "batch_results.json")
    print(f"Batch results saved to: batch_results.json")
    
    # Analyze batch results
    total_actions = sum(len(step_actions) for step_actions in batch_results)
    avg_actions_per_step = total_actions / len(batch_results) if batch_results else 0
    
    print(f"\nBatch Analysis:")
    print(f"  Total actions: {total_actions}")
    print(f"  Average actions per step: {avg_actions_per_step:.1f}")
    print(f"  Total steps: {len(batch_results)}")
    
    print("\nBatch processing example completed successfully!")

if __name__ == "__main__":
    main()
```

## Tutorial Examples

### Tutorial: Basic Brain Simulation

This tutorial demonstrates basic brain simulation.

```python
#!/usr/bin/env python3
"""
Tutorial: Basic Brain Simulation
"""

from nlm import EnhancedBrain, World, Action, ActionType

def tutorial_basic_brain_simulation():
    print("Tutorial: Basic Brain Simulation")
    print("=" * 50)
    
    # Step 1: Create brain and world
    print("Step 1: Creating brain and world...")
    brain = EnhancedBrain()
    world = World()
    world.configure(100, 100, 10, 10)
    world.setAgentStart(50, 50)
    
    # Step 2: Initialize brain
    print("Step 2: Initializing brain...")
    brain.initialize()
    print(f"   ✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Step 3: Run simulation
    print("Step 3: Running simulation...")
    for step in range(10):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Create action based on brain activity
        if brain.getActiveNeuronCount() > 0:
            action = Action(type=ActionType.Interact)
        else:
            action = Action(type=ActionType.Wait)
        
        # Apply action
        world.applyAction(action)
        
        # Step world
        world.update(0.01)
        
        if (step + 1) % 5 == 0:
            print(f"   Step {step + 1}: {action.getType()} action")
    
    # Step 4: Analyze results
    print("Step 4: Analyzing results...")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Active neurons: {brain.getActiveNeuronCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    
    print("\nTutorial completed successfully!")
    print("You have successfully run a basic brain simulation!")

if __name__ == "__main__":
    tutorial_basic_brain_simulation()
```

### Tutorial: Environment Configuration

This tutorial demonstrates environment configuration.

```python
#!/usr/bin/env python3
"""
Tutorial: Environment Configuration
"""

from nlm import World, Action, ActionType

def tutorial_environment_configuration():
    print("Tutorial: Environment Configuration")
    print("=" * 50)
    
    # Step 1: Create world
    print("Step 1: Creating world...")
    world = World()
    
    # Step 2: Configure world
    print("Step 2: Configuring world...")
    world.configure(
        width=200,
        height=150,
        vision_width=20,
        vision_height=15
    )
    
    # Step 3: Set agent position
    print("Step 3: Setting agent starting position...")
    world.setAgentStart(100, 75)
    
    # Step 4: Display world configuration
    print("Step 4: World configuration...")
    print(f"   Width: {world.getWidth()}")
    print(f"   Height: {world.getHeight()}")
    print(f"   Vision width: {world.getVisionWidth() if hasattr(world, 'getVisionWidth') else 'N/A'}")
    print(f"   Vision height: {world.getVisionHeight() if hasattr(world, 'getVisionHeight') else 'N/A'}")
    
    # Step 5: Configure agent properties
    print("Step 5: Configuring agent properties...")
    world.setMaxEnergy(200.0)
    world.setEnergyDecayRate(0.005)
    
    print(f"   Max energy: {world.getMaxEnergy()}")
    print(f"   Energy decay rate: {world.getEnergyDecayRate()}")
    
    # Step 6: Validate position
    print("Step 6: Validating position...")
    if world.isValidPosition(105, 80):
        print("   ✓ Position (105, 80) is valid")
    else:
        print("   ✗ Position (105, 80) is invalid")
    
    # Step 7: Reset world
    print("Step 7: Resetting world...")
    world.reset()
    print("   ✓ World reset completed")
    
    print("\nTutorial completed successfully!")
    print("You have successfully configured a world environment!")

if __name__ == "__main__":
    tutorial_environment_configuration()
```

### Tutorial: Performance Monitoring

This tutorial demonstrates performance monitoring.

```python
#!/usr/bin/env python3
"""
Tutorial: Performance Monitoring
"""

from nlm import PerformanceProfiler, EnhancedBrain, SimpleWorld
import time

def tutorial_performance_monitoring():
    print("Tutorial: Performance Monitoring")
    print("=" * 50)
    
    # Step 1: Create profiler
    print("Step 1: Creating performance profiler...")
    profiler = PerformanceProfiler()
    
    # Step 2: Start monitoring
    print("Step 2: Starting monitoring...")
    profiler.start_profiling()
    
    # Step 3: Run simulated task
    print("Step 3: Running simulated task...")
    time.sleep(0.5)
    
    # Create brain
    brain = EnhancedBrain()
    
    # Run simulation
    world = SimpleWorld()
    world.configure(50, 50, 5, 5)
    
    for step in range(20):
        world.update(0.01)
    
    # Step 4: Benchmark task
    print("Step 4: Benchmarking task...")
    
    def example_task():
        # Simulate some work
        time.sleep(0.1)
        return "completed"
    
    result = profiler.benchmark_function(example_task)
    print(f"   Task completed in {result.execution_time:.3f} seconds")
    print(f"   Memory usage: {result.memory_usage:.2f} MB")
    
    # Step 5: Get summary
    print("Step 5: Getting performance summary...")
    summary = profiler.get_summary()
    print(f"   Total tests: {summary['total_tests']}")
    print(f"   Success rate: {summary['success_rate']:.2%}")
    print(f"   Average execution time: {summary['average_execution_time']:.3f} seconds")
    
    # Step 6: Generate report
    print("Step 6: Generating performance report...")
    report = profiler.generate_report()
    print("   Report generated (see console output)")
    
    # Stop monitoring
    profiler.stop_profiling()
    
    # Step 7: Save results
    print("Step 7: Saving results...")
    profiler.save_results("performance_tutorial.json")
    print("   Results saved to: performance_tutorial.json")
    
    print("\nTutorial completed successfully!")
    print("You have successfully used performance monitoring!")

if __name__ == "__main__":
    tutorial_performance_monitoring()
```

### Tutorial: Experiment Management

This tutorial demonstrates experiment management.

```python
#!/usr/bin/env python3
"""
Tutorial: Experiment Management
"""

from nlm import ExperimentSuite, AdvancedExperiment, PyConfig, EnhancedBrain
import numpy as np

def main():
    print("Tutorial: Experiment Management")
    print("=" * 50)
    
    # Step 1: Create experiment suite
    print("Step 1: Creating experiment suite...")
    suite = ExperimentSuite("tutorial_suite")
    
    # Step 2: Create experiments
    print("Step 2: Creating experiments...")
    
    # Experiment 1: Basic simulation
    exp1_config = {
        'name': 'basic_simulation',
        'description': 'Basic brain simulation experiment',
        'experiment_type': 'basic',
        'parameters': {
            'num_steps': 100,
            'width': 100,
            'height': 100
        }
    }
    
    # Experiment 2: Parameter sweep
    exp2_config = {
        'name': 'parameter_sweep',
        'description': 'Parameter sweep experiment',
        'experiment_type': 'parameter_sweep',
        'parameters': {
            'parameter_grid': {
                'width': [50, 100],
                'height': [50, 100]
            }
        }
    }
    
    # Create experiment objects
    experiment1 = AdvancedExperiment(exp1_config)
    experiment2 = AdvancedExperiment(exp2_config)
    
    # Step 3: Add experiments to suite
    print("Step 3: Adding experiments to suite...")
    suite.add_experiment(experiment1)
    suite.add_experiment(experiment2)
    
    # Step 4: Run selected experiments
    print("Step 4: Running selected experiments...")
    selected_results = suite.run_selected(['basic_simulation'])
    
    # Step 5: Analyze results
    print("Step 5: Analyzing results...")
    for exp_name, result in selected_results.items():
        print(f"   Experiment: {exp_name}")
        print(f"     Status: {result.status.value}")
        print(f"     Duration: {result.duration:.2f} seconds")
        print(f"     Results: {len(str(result.results))} characters")
    
    # Step 6: Get suite summary
    print("Step 6: Getting suite summary...")
    summary = suite.get_suite_summary()
    print(f"   Suite name: {summary['suite_name']}")
    print(f"   Total experiments: {summary['total_experiments']}")
    print(f"   Successful experiments: {summary['successful_experiments']}")
    print(f"   Success rate: {summary['success_rate']:.2%}")
    
    # Step 7: Save suite results
    print("Step 7: Saving suite results...")
    suite.save_suite_results("tutorial_suite_results.json")
    print("   Results saved to: tutorial_suite_results.json")
    
    print("\nTutorial completed successfully!")
    print("You have successfully managed experiments using ExperimentSuite!")

if __name__ == "__main__":
    main()
```

## Research Examples

### Example: Memory System Integration

This example demonstrates integrating memory systems into research.

```python
#!/usr/bin/env python3
"""
Example: Memory System Integration
"""

from nlm import EnhancedBrain, PyMemoryManager, AdvancedExperiment

def brain_factory(params):
    """Create brain with memory systems."""
    config = PyConfig()
    config.setInt("world.width", params.get('width', 100))
    config.setInt("world.height", params.get('height', 100))
    config.setBool("memory.working_memory_enabled", True)
    config.setBool("memory.episodic_memory_enabled", True)
    return EnhancedBrain(config)

def main():
    print("Example: Memory System Integration")
    print("=" * 50)
    
    # Create experiment
    experiment = AdvancedExperiment("memory_integration")
    
    # Create brain with memory systems
    brain = brain_factory({'width': 150, 'height': 150})
    memory_manager = PyMemoryManager(brain)
    
    print("Memory System Integration Example")
    print("=" * 50)
    
    # Working memory operations
    print("Working Memory Operations:")
    memory_manager.storeWorkingMemory("current_goal", "explore_environment")
    memory_manager.storeWorkingMemory("energy_level", "high")
    
    retrieved_goal = memory_manager.getWorkingMemory("current_goal")
    retrieved_energy = memory_manager.getWorkingMemory("energy_level")
    
    print(f"  Stored goal: {retrieved_goal}")
    print(f"  Stored energy level: {retrieved_energy}")
    print(f"  Working memory size: {memory_manager.getWorkingMemorySize()}")
    
    # Episodic memory operations
    print("\nEpisodic Memory Operations:")
    memory_manager.storeEpisodicMemory("day_1", [
        "explored_north",
        "found_resource",
        "avoided_hazard"
    ])
    
    episode = memory_manager.getEpisodicMemory("day_1")
    print(f"  Stored episode length: {len(episode)} events")
    print(f"  Episodic memory count: {memory_manager.getEpisodicMemoryCount()}")
    
    # Associative memory operations
    print("\nAssociative Memory Operations:")
    memory_manager.storeAssociation("reward", "approach", 1.0)
    memory_manager.storeAssociation("danger", "avoid", 0.9)
    memory_manager.storeAssociation("curiosity", "explore", 0.7)
    
    reward_strength = memory_manager.getAssociation("reward", "approach")
    danger_strength = memory_manager.getAssociation("danger", "avoid")
    curiosity_strength = memory_manager.getAssociation("curiosity", "explore")
    
    print(f"  Reward-approach association: {reward_strength}")
    print(f"  Danger-avoid association: {danger_strength}")
    print(f"  Curiosity-explore association: {curiosity_strength}")
    
    # Memory consolidation
    print("\nMemory Consolidation:")
    memory_manager.consolidateMemory()
    print("  Memory consolidation completed")
    
    # Memory replay
    print("\nMemory Replay:")
    memory_manager.replayMemory("day_1")
    print("  Memory replay completed")
    
    print("\nMemory system integration example completed successfully!")

if __name__ == "__main__":
    main()
```

### Example: Neuromodulation Research

This example demonstrates neuromodulation research.

```python
#!/usr/bin/env python3
"""
Example: Neuromodulation Research
"""

from nlm import EnhancedBrain, Action, ActionType

def main():
    print("Example: Neuromodulation Research")
    print("=" * 50)
    
    # Create brain
    brain = EnhancedBrain()
    
    # Simulate neuromodulation
    print("Neuromodulation Simulation")
    print("=" * 50)
    
    # Create world for testing
    from nlm import World
    world = World()
    world.configure(100, 100, 10, 10)
    world.setAgentStart(50, 50)
    
    # Run simulation
    for step in range(20):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Simulate neuromodulation effects
        if brain.getActiveNeuronCount() > 0:
            # Simulate curiosity-driven exploration
            action = Action(type=ActionType.Look)
        elif brain.getFiringNeuronCount() > 5:
            # Simulate reward-driven approach
            action = Action(type=ActionType.Interact)
        else:
            # Default behavior
            action = Action(type=ActionType.Wait)
        
        # Apply action
        world.applyAction(action)
        
        # Step world
        world.update(0.01)
        
        if (step + 1) % 5 == 0:
            print(f"  Step {step + 1}: {action.getType()} action, "
                  f"Neuromodulation level: {brain.getNeuromodulationLevel():.2f}, "
                  f"Curiosity level: {brain.getCuriosityLevel():.2f}")
    
    print("\nNeuromodulation research example completed successfully!")

if __name__ == "__main__":
    main()
```

### Example: Developmental Studies

This example demonstrates developmental studies.

```python
#!/usr/bin/env python3
"""
Example: Developmental Studies
"""

from nlm import EnhancedBrain, DevelopmentalStage, World, Action, ActionType
import time

def main():
    print("Example: Developmental Studies")
    print("=" * 50)
    
    # Create brain
    brain = EnhancedBrain()
    
    # Create world
    world = World()
    world.configure(150, 150, 15, 15)
    world.setAgentStart(75, 75)
    
    # Initialize brain
    brain.initialize()
    
    print("Developmental Study")
    print("=" * 50)
    
    # Simulate developmental stages
    stages = [
        (DevelopmentalStage.Initial, "Initial stage"),
        (DevelopmentalStage.CriticalPeriod, "Critical period"),
        (DevelopmentalStage.Maturation, "Maturation stage"),
        (DevelopmentalStage.Adult, "Adult stage"),
        (DevelopmentalStage.Aging, "Aging stage")
    ]
    
    for stage, description in stages:
        # Set developmental stage
        brain.setDevelopmentalStage(stage)
        
        print(f"\n{stage.value}: {description}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
        
        # Run simulation for this stage
        for step in range(5):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Create action
            action = Action(type=ActionType.Wait)
            
            # Apply action
            world.applyAction(action)
            
            # Step world
            world.update(0.01)
            
            # Print status
            print(f"    Step {step + 1}: Neurons {brain.getTotalNeuronCount()}, "
                  f"Firing {brain.getFiringNeuronCount()}, "
                  f"Rate {brain.getAverageFiringRate():.2f} Hz")
        
        # Pause between stages
        print(f"  Paused for developmental transition...")
        time.sleep(0.5)
    
    print("\nDevelopmental studies example completed successfully!")

if __name__ == "__main__":
    main()
```

### Example: Social Learning

This example demonstrates social learning.

```python
#!/usr/bin/env python3
"""
Example: Social Learning
"""

from nlm import EnhancedBrain, World, Action, ActionType
import numpy as np

def main():
    print("Example: Social Learning")
    print("=" * 50)
    
    # Create multiple agents for social learning
    agents = []
    for i in range(3):
        agent = EnhancedBrain()
        agents.append(agent)
    
    # Create shared world
    world = World()
    world.configure(200, 200, 20, 20)
    world.setAgentStart(100, 100)
    
    # Initialize all agents
    for agent in agents:
        agent.initialize()
    
    print("Social Learning Simulation")
    print("=" * 50)
    
    # Social learning simulation
    for step in range(30):
        # Get shared sensory input
        percept = world.getSensoryPercept()
        
        # Each agent processes the same input
        actions = []
        for agent in agents:
            # Simulate social learning - agents learn from each other
            # In this example, we use a simple learning rule
            if agent.getActiveNeuronCount() > 0:
                # Agent observes others' actions
                action = Action(type=ActionType.Interact)
            else:
                # Agent acts independently
                action = Action(type=ActionType.Wait)
            
            actions.append(action)
        
        # Apply first agent's action to world
        world.applyAction(actions[0])
        
        # Step world
        world.update(0.01)
        
        # Calculate social influence
        if step % 5 == 0:
            print(f"\nStep {step + 1}:")
            print(f"  Shared percept: {percept}")
            print(f"  Agent actions: {[a.getType() for a in actions]}")
            print(f"  Agent stats: {[a.getFiringNeuronCount() for a in agents]}")
            
            # Calculate social cohesion
            action_consensus = np.mean([a.getType().value for a in actions])
            print(f"  Social consensus: {action_consensus:.2f}")
    
    print("\nSocial learning example completed successfully!")

if __name__ == "__main__":
    main()
```

## Performance Examples

### Example: Simulation Optimization

This example demonstrates simulation optimization.

```python
#!/usr/bin/env python3
"""
Example: Simulation Optimization
"""

from nlm import PerformanceProfiler, EnhancedBrain, SimpleWorld
import time

def optimize_simulation():
    """Optimized simulation function."""
    # Use efficient configuration
    config = EnhancedBrain()
    
    # Create world
    world = SimpleWorld()
    world.configure(100, 100, 10, 10)
    
    # Run optimized simulation
    for step in range(50):
        # Optimized update
        world.update(0.01)
        
        # Early stopping condition
        if world.getAgentBody().energy <= 0:
            break
    
    return config

def main():
    print("Example: Simulation Optimization")
    print("=" * 50)
    
    # Create profiler
    profiler = PerformanceProfiler()
    
    # Start profiling
    profiler.start_profiling()
    
    # Run optimized simulation
    print("Running optimized simulation...")
    
    # Profile optimized version
    result1 = profiler.benchmark_function(optimize_simulation)
    
    # Simulate less optimized version
    def less_optimized_simulation():
        time.sleep(0.5)  # Simulate slower execution
        return EnhancedBrain()
    
    # Profile less optimized version
    result2 = profiler.benchmark_function(less_optimized_simulation)
    
    # Compare results
    comparison = profiler.compare_results(result1, result2)
    
    print(f"\nComparison Results:")
    print(f"  Optimized time: {result1.execution_time:.3f} seconds")
    print(f"  Less optimized time: {result2.execution_time:.3f} seconds")
    print(f"  Time difference: {comparison['execution_time_comparison']['difference']:.3f} seconds")
    print(f"  Winner: {comparison['winner']}")
    
    # Get performance summary
    summary = profiler.get_summary()
    print(f"\nPerformance Summary:")
    print(f"  Fastest function: {summary['fastest_test']}")
    print(f"  Most memory efficient: {summary['memory_efficient_test']}")
    print(f"  Most CPU efficient: {summary['cpu_efficient_test']}")
    
    print("\nSimulation optimization example completed successfully!")

if __name__ == "__main__":
    main()
```

### Example: Memory Optimization

This example demonstrates memory optimization.

```python
#!/usr/bin/env python3
"""
Example: Memory Optimization
"""

from nlm import MemoryMonitor, EnhancedBrain, SimpleWorld
import gc
import time

def memory_intensive_simulation():
    """Memory-intensive simulation."""
    # Create brain
    brain = EnhancedBrain()
    
    # Create world
    world = SimpleWorld()
    world.configure(80, 80, 8, 8)
    
    # Run simulation with memory monitoring
    for step in range(30):
        world.update(0.01)
        
        # Simulate memory usage
        if step % 5 == 0:
            # Create and clean up objects
            temp_data = [list(range(100)) for _ in range(10)]
            del temp_data
            gc.collect()
    
    return brain

def main():
    print("Example: Memory Optimization")
    print("=" * 50)
    
    # Create memory monitor
    memory_monitor = MemoryMonitor(sampling_interval=0.05)
    
    # Start monitoring
    memory_monitor.start_monitoring()
    
    # Run memory-intensive simulation
    print("Running memory-intensive simulation...")
    start_time = time.time()
    
    result = memory_monitor.benchmark_function(memory_intensive_simulation)
    
    end_time = time.time()
    
    print(f"\nMemory Optimization Results:")
    print(f"  Execution time: {result.execution_time:.3f} seconds")
    print(f"  Memory usage: {result.memory_usage:.2f} MB")
    print(f"  Success: {result.success}")
    
    # Get memory metrics
    current_memory = memory_monitor.get_current_memory()
    peak_memory = memory_monitor.get_peak_memory()
    
    print(f"\nMemory Metrics:")
    print(f"  Current memory: {current_memory.rss_mb:.2f} MB")
    print(f"  Peak memory: {peak_memory.rss_mb:.2f} MB")
    print(f"  Python objects: {current_memory.python_objects}")
    print(f"  NumPy arrays: {current_memory.numpy_arrays}")
    
    # Stop monitoring
    memory_monitor.stop_monitoring()
    
    # Save memory history
    memory_monitor.save_history("memory_optimization.json")
    print(f"\nMemory history saved to: memory_optimization.json")
    
    print("\nMemory optimization example completed successfully!")

if __name__ == "__main__":
    main()
```

### Example: Parallel Processing

This example demonstrates parallel processing.

```python
#!/usr/bin/env python3
"""
Example: Parallel Processing
"""

from nlm import EnhancedBrain, SimpleWorld
import concurrent.futures
import time

def process_simulation(step_id, width, height):
    """Process simulation in parallel."""
    config = EnhancedBrain()
    world = SimpleWorld()
    world.configure(width, height, width//10, height//10)
    
    # Run simulation
    for step in range(10):
        world.update(0.01)
    
    return {
        'step_id': step_id,
        'width': width,
        'height': height,
        'neurons': config.getTotalNeuronCount(),
        'synapses': config.getTotalSynapseCount()
    }

def main():
    print("Example: Parallel Processing")
    print("=" * 50)
    
    # Create simulations for parallel processing
    simulation_configs = [
        (0, 50, 50),
        (1, 100, 100),
        (2, 150, 150),
        (3, 200, 200)
    ]
    
    print("Running simulations in parallel...")
    
    # Run in parallel
    start_time = time.time()
    
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
        futures = [executor.submit(process_simulation, config[0], config[1], config[2])
                  for config in simulation_configs]
        
        results = []
        for future in concurrent.futures.as_completed(futures):
            try:
                result = future.result()
                results.append(result)
                print(f"  Completed simulation {result['step_id']}: "
                      f"{result['width']}x{result['height']}")
            except Exception as e:
                print(f"  Error in simulation: {e}")
    
    end_time = time.time()
    
    print(f"\nParallel Processing Results:")
    print(f"  Total execution time: {end_time - start_time:.3f} seconds")
    print(f"  Number of simulations: {len(results)}")
    print(f"  Average time per simulation: {(end_time - start_time) / len(results):.3f} seconds")
    
    # Print individual results
    print(f"\nIndividual Results:")
    for result in results:
        print(f"  Simulation {result['step_id']}: {result['width']}x{result['height']}, "
              f"{result['neurons']} neurons, {result['synapses']} synapses")
    
    print("\nParallel processing example completed successfully!")

if __name__ == "__main__":
    main()
```

## Conclusion

This document provides comprehensive examples for using the enhanced NLM Python API. The examples cover:

- **Basic Examples**: Simple usage for beginners
- **Advanced Examples**: Complex usage for power users
- **Tutorial Examples**: Step-by-step learning guides
- **Research Examples**: Scientific research applications
- **Performance Examples**: Performance optimization techniques

These examples demonstrate the capabilities of the enhanced NLM Python API and provide starting points for developing your own neural simulation projects.
