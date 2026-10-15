#!/usr/bin/env python3
"""
NLM Example: Basic Neural Computation
======================================

This example demonstrates the core functionality of NLM for basic neural computation
without a world environment. It shows how to create a brain, run a simulation,
and monitor neural activity.
"""

import pynlm
import time
import json
import os

def basic_neural_computation(num_neurons=1000, num_steps=1000):
    """
    Example 1: Basic neural computation without environment.
    
    This is the simplest way to create and run a neural network simulation.
    """
    print("=== Basic Neural Computation Example ===\n")
    
    # Step 1: Create configuration
    print(f"1. Creating brain with {num_neurons} neurons...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", num_neurons)
    config.set("brain.synapse_density", 0.1)
    
    # Step 2: Create and initialize brain
    print("2. Initializing brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Step 3: Run simulation
    print(f"3. Running simulation for {num_steps} steps...")
    start_time = time.time()
    
    for step in range(num_steps):
        brain.step(step)
        
        # Log progress every 10% completion
        if step % (num_steps // 10) == 0 and step > 0:
            elapsed = time.time() - start_time
            print(f"   Progress: {step/num_steps*100:.0f}%, "
                  f"Steps/sec: {step/elapsed:.0f}, "
                  f"Firing neurons: {brain.getFiringNeuronCount()}")
    
    elapsed = time.time() - start_time
    
    # Step 4: Report results
    print(f"\n4. Simulation Complete!")
    print(f"   Total time: {elapsed:.2f} seconds")
    print(f"   Average steps/sec: {num_steps/elapsed:.0f}")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Current firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"   Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Save brain state for later analysis
    brain.save(f"brain_{num_neurons}_neurons_{num_steps}_steps.bin")
    print(f"   Saved brain state to: brain_{num_neurons}_neurons_{num_steps}_steps.bin")
    
    return brain

def custom_configuration_example():
    """
    Example 2: Custom configuration and advanced settings.
    
    Demonstrates how to use configuration objects for fine-tuning the brain.
    """
    print("\n\n=== Custom Configuration Example ===\n")
    
    # Create and configure custom settings
    print("1. Creating custom configuration...")
    config = pynlm.createDefaultConfig()
    
    # Advanced configuration
    config.set("brain.neuron_count", 5000)
    config.set("brain.synapse_density", 0.15)
    config.set("random_seed", 12345)  # For reproducibility
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create brain with custom config
    print("2. Creating brain with custom configuration...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Print configuration summary
    print("3. Configuration summary:")
    print(brain.getConfig().summary())
    
    # Create simple world for demonstration
    print("4. Setting up simulation environment...")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent brain
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("5. Advanced features enabled:")
    print(f"   - Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"   - Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"   - Development: {agent.isDevelopmentEnabled()}")
    print(f"   - Curiosity: {agent.isCuriosityEnabled()}")
    
    return brain, world, agent

def training_simulation_example():
    """
    Example 3: Training simulation with learning and adaptation.
    
    Demonstrates a complete agent training episode with learning from experience.
    """
    print("\n\n=== Training Simulation Example ===\n")
    
    def run_training_episode(config, world, agent, num_steps=500):
        """Run a single training episode."""
        episode_stats = {
            "steps": 0,
            "spike_count": 0,
            "average_firing_rate": 0.0,
            "curiosity": 0.0,
            "novelty": 0.0,
            "total_reward": 0.0,
            "development_stage": "Initial"
        }
        
        # Run episode
        for step in range(num_steps):
            # Update environment
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action and execute it
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning signals
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Collect statistics
            episode_stats["steps"] += 1
            episode_stats["spike_count"] += brain.getTotalSpikeCount()
            episode_stats["average_firing_rate"] += brain.getAverageFiringRate()
            episode_stats["curiosity"] += agent.getCuriosityLevel()
            episode_stats["novelty"] += agent.getNoveltyLevel()
            episode_stats["total_reward"] += reward
            
            # Print progress
            if step % 100 == 0 and step > 0:
                print(f"   Step {step}: "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}, "
                      f"Development={brain.getDevelopmentalStage()}")
        
        # Calculate averages
        if episode_stats["steps"] > 0:
            episode_stats["average_firing_rate"] /= episode_stats["steps"]
            episode_stats["curiosity"] /= episode_stats["steps"]
            episode_stats["novelty"] /= episode_stats["steps"]
            episode_stats["development_stage"] = brain.getDevelopmentalStage()
        
        return episode_stats
    
    # Setup simulation
    print("1. Setting up training environment...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    world.setMaxEnergy(100.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    print("2. Enabling all learning systems...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Add some environmental resources
    print("3. Setting up environmental resources...")
    world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0))
    world.addObject(pynlm.WorldObject(25.0, 25.0, pynlm.WorldObjectType.Resource, value=2.0))
    world.addObject(pynlm.WorldObject(15.0, 5.0, pynlm.WorldObjectType.Resource, value=1.5))
    
    # Run training episode
    print("4. Running training episode...")
    episode_stats = run_training_episode(config, world, agent, 500)
    
    # Report results
    print("\n5. Training Results:")
    print(f"   Steps completed: {episode_stats['steps']}")
    print(f"   Total spikes: {episode_stats['spike_count']}")
    print(f"   Average firing rate: {episode_stats['average_firing_rate']:.2f}")
    print(f"   Average curiosity level: {episode_stats['curiosity']:.3f}")
    print(f"   Average novelty level: {episode_stats['novelty']:.3f}")
    print(f"   Total reward received: {episode_stats['total_reward']:.2f}")
    print(f"   Final development stage: {episode_stats['development_stage']}")
    print(f"   Final neuron count: {brain.getTotalNeuronCount()}")
    print(f"   Final synapse count: {brain.getTotalSynapseCount()}")
    
    return brain, world, agent, episode_stats

def performance_benchmark_example():
    """
    Example 4: Performance benchmarking and optimization.
    
    Demonstrates how to benchmark different configurations and optimization parameters.
    """
    print("\n\n=== Performance Benchmark Example ===\n")
    
    def benchmark_configuration(config_name, config, num_neurons=2000, num_steps=1000):
        """Benchmark a specific configuration."""
        print(f"\nBenchmarking {config_name}:")
        print(f"  - Neurons: {num_neurons}")
        print(f"  - Steps: {num_steps}")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run benchmark
        start_time = time.time()
        for step in range(num_steps):
            brain.step(step)
        
        elapsed = time.time() - start_time
        
        # Calculate performance metrics
        steps_per_sec = num_steps / elapsed
        neurons_per_sec = num_neurons / elapsed
        spikes_per_neuron = brain.getTotalSpikeCount() / num_neurons if num_neurons > 0 else 0
        
        # Print results
        print(f"  - Total time: {elapsed:.3f}s")
        print(f"  - Steps/sec: {steps_per_sec:.0f}")
        print(f"  - Neurons/sec: {neurons_per_sec:.0f}")
        print(f"  - Spikes per neuron: {spikes_per_neuron:.2f}")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
        
        return {
            "name": config_name,
            "time": elapsed,
            "steps_per_sec": steps_per_sec,
            "neurons_per_sec": neurons_per_sec,
            "spikes_per_neuron": spikes_per_neuron,
            "total_spikes": brain.getTotalSpikeCount()
        }
    
    print("1. Configuration Benchmark Suite")
    print("=" * 50)
    
    # Test different configurations
    configs = {
        "Default": pynlm.createDefaultConfig(),
        "High Density": pynlm.createDefaultConfig(),
        "Low Density": pynlm.createDefaultConfig(),
    }
    
    # Configure different settings
    configs["Default"].set("brain.synapse_density", 0.1)
    configs["High Density"].set("brain.synapse_density", 0.3)
    configs["Low Density"].set("brain.synapse_density", 0.05)
    
    # Run benchmarks
    results = []
    for name, config in configs.items():
        results.append(benchmark_configuration(name, config))
    
    # Compare results
    print("\n\nPerformance Comparison:")
    print("=" * 80)
    print(f"{'Config':<15} {'Time (s)':<10} {'Steps/s':<12} {'Neurons/s':<12} {'Spikes/Neuron':<15} {'Total Spikes':<12}")
    print("-" * 80)
    
    for result in sorted(results, key=lambda x: x['steps_per_sec'], reverse=True):
        print(f"{result['name']:<15} {result['time']:<10.3f} {result['steps_per_sec']:<12.0f} "
              f"{result['neurons_per_sec']:<12.0f} {result['spikes_per_neuron']:<15.2f} "
              f"{result['total_spikes']:<12}")
    
    # Find optimal configuration
    best = max(results, key=lambda x: x['steps_per_sec'])
    print(f"\nBest performing: {best['name']} ({best['steps_per_sec']:.0f} steps/sec)")
    
    return results

def main():
    """Run all examples."""
    print("NLM Python API - Comprehensive Example Suite")
    print("=" * 60)
    
    try:
        # Example 1: Basic neural computation
        brain1 = basic_neural_computation()
        
        # Example 2: Custom configuration
        brain2, world2, agent2 = custom_configuration_example()
        
        # Example 3: Training simulation
        brain3, world3, agent3, stats3 = training_simulation_example()
        
        # Example 4: Performance benchmarking
        bench_results = performance_benchmark_example()
        
        print("\n\n" + "=" * 60)
        print("All examples completed successfully!")
        print("=" * 60)
        
    except Exception as e:
        print(f"\nError running examples: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()