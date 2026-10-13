#!/usr/bin/env python3
"""
NLM - Comprehensive Examples and Demo Scripts

This file contains a collection of example scripts demonstrating
various aspects of the NLM (Neural Learning Machine) framework.
Each example builds on previous concepts to show different capabilities.
"""

import pynlm
import time
from typing import List, Dict, Any

# Example 1: Silent Brain (Basic Neuron Activity)
def example_silent_brain():
    """
    Example 1: Silent Brain
    Demonstrates the simplest possible brain - just neurons firing.
    """
    print("=== Example 1: Silent Brain ===")
    
    # One line to create a virtual brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize it
    brain.initialize()
    
    # Make it active
    for i in range(10):
        brain.step(i)
    
    print(f"Silent brain test done!")
    print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print()

# Example 2: Brain Watching a World
def example_brain_watching_world():
    """
    Example 2: Brain Watching a World
    Demonstrates a brain perceiving and interacting with an environment.
    """
    print("=== Example 2: Brain Watching a World ===")
    
    # Setup
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable some features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Watch the world for 30 steps
    for i in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
    
    print("Watched world for 30 steps")
    print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    print()

# Example 3: Complete Agent with Learning
def example_complete_agent():
    """
    Example 3: Complete Agent
    Demonstrates a full agent with all learning systems enabled.
    """
    print("=== Example 3: Complete Agent with Learning ===")
    
    # Setup with learning capabilities
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Starting complete agent simulation...")
    
    # Run agent for 100 steps
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"novelty: {agent.getNoveltyLevel():.3f}")
    
    print("Agent simulation complete!")
    print(f"Final stats - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Final stats - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final stats - Avg firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Final stats - Development stage: {brain.getDevelopmentalStage()}")
    print()

# Example 4: Brain Statistics and Monitoring
def example_brain_statistics():
    """
    Example 4: Brain Statistics and Monitoring
    Demonstrates comprehensive brain statistics and monitoring.
    """
    print("=== Example 4: Brain Statistics and Monitoring ===")
    
    # Create and initialize brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation while collecting stats
    print("Running simulation with detailed statistics...")
    
    for step in range(200):
        # Inject some activity to make it interesting
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0f)
        
        brain.step(step)
        
        if step % 50 == 0:
            # Get brain statistics using the convenience method
            stats = brain.getStats()
            print(f"Step {step}:")
            print(f"  Total neurons: {stats['total_neurons']}")
            print(f"  Total synapses: {stats['total_synapses']}")
            print(f"  Firing neurons: {stats['firing_neurons']}")
            print(f"  Active neurons: {stats['active_neurons']}")
            print(f"  Avg firing rate: {stats['avg_firing_rate']:.3f}")
            print(f"  Total spikes: {stats['total_spikes']}")
            print(f"  Development stage: {stats['dev_stage']}")
    
    print("Simulation complete!")
    print()

# Example 5: Multi-Agent Competition
def example_multi_agent_competition():
    """
    Example 5: Multi-Agent Competition
    Demonstrates multiple brains competing in the same environment.
    """
    print("=== Example 5: Multi-Agent Competition ===")
    
    # Create multiple brains
    brains = []
    agents = []
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    for i in range(3):
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Each agent gets different characteristics
        if i == 0:
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
        elif i == 1:
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
        else:
            agent.enableCuriosity(True)
            agent.enableRewardModulation(True)
        
        brains.append(brain)
        agents.append(agent)
    
    print("Running competition between 3 agents...")
    
    for step in range(50):
        world.update(0.1)
        
        for i, (brain, agent) in enumerate(zip(brains, agents)):
            # Each agent perceives the world
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action and apply it
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"Step {step}:")
            for i, (brain, agent) in enumerate(zip(brains, agents)):
                print(f"  Agent {i+1}: {brain.getFiringNeuronCount()} firing, "
                      f"curiosity: {agent.getCuriosityLevel():.3f}, "
                      f"reward: {agent.getNeuromodulationLevel():.3f}")
    
    print("Competition complete!")
    
    # Analyze results
    print("\nFinal Results:")
    for i, (brain, agent) in enumerate(zip(brains, agents)):
        print(f"  Agent {i+1}: {brain.getTotalSpikeCount()} spikes, "
              f"{agent.getCuriosityLevel():.3f} curiosity, "
              f"{agent.getNoveltyLevel():.3f} novelty")
    print()

# Example 6: Development Stages
def example_development_stages():
    """
    Example 6: Development Stages
    Demonstrates how brains develop over time.
    """
    print("=== Example 6: Development Stages ===")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print("Simulating brain development over time...")
    
    for step in range(300):
        brain.step(step)
        
        # Update development every 50 steps
        if step % 50 == 0:
            old_stage = brain.getDevelopmentalStage()
            brain.setDevelopmentalStage(
                pynlm.DevelopmentalStage((step // 50) % 6)
            )
            print(f"Step {step}: Development stage changed from "
                  f"{old_stage} to {brain.getDevelopmentalStage()}")
            
            # Observe changes in brain activity
            stats = brain.getStats()
            print(f"  Stats - Firing: {stats['firing_neurons']}, "
                  f"Spikes: {stats['total_spikes']}, "
                  f"Rate: {stats['avg_firing_rate']:.3f}")
    
    print("Development simulation complete!")
    print()

# Example 7: Error Handling and Validation
def example_error_handling():
    """
    Example 7: Error Handling and Validation
    Demonstrates configuration validation and error handling.
    """
    print("=== Example 7: Error Handling and Validation ===")
    
    # Create a custom configuration
    config = pynlm.createDefaultConfig()
    
    # Modify some settings (some may be invalid)
    config.set("brain.neuron_count", 5000)  # Valid
    config.set("brain.connection_probability", 1.5)  # Invalid (>1.0)
    config.set("brain.v_thresh", -100.0)  # Valid
    
    # Validate the configuration
    validation = pynlm.validateConfig(config)
    
    print("Configuration Validation Results:")
    print(f"  Valid: {validation['valid']}")
    print(f"  Errors: {validation['errors']}")
    print(f"  Neuron count: {validation['neuron_count']}")
    print(f"  Connection probability: {validation['connection_probability']}")
    
    # Try to create a brain with invalid config
    if validation['valid']:
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("Brain created successfully with invalid config!")
    else:
        print("Config is invalid - creating with defaults instead")
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
    
    print()

# Example 8: Performance Benchmark
def example_performance_benchmark():
    """
    Example 8: Performance Benchmark
    Demonstrates performance measurement and benchmarking.
    """
    print("=== Example 8: Performance Benchmark ===")
    
    import time
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test different step counts
    test_sizes = [100, 500, 1000]
    
    for size in test_sizes:
        print(f"Testing {size} steps...")
        
        start_time = time.time()
        
        for step in range(size):
            brain.step(step)
            # Inject occasional activity
            if step % 10 == 0:
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 2.0f)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        stats = brain.getStats()
        print(f"  Time: {elapsed:.3f}s")
        print(f"  Rate: {size/elapsed:.1f} steps/second")
        print(f"  Spikes per step: {stats['total_spikes']/size:.2f}")
        print(f"  Avg firing rate: {stats['avg_firing_rate']:.3f}")
    
    print()

# Example 9: Simulation with Callbacks
def example_simulation_callbacks():
    """
    Example 9: Simulation with Callbacks
    Demonstrates running simulations with custom callbacks and monitoring.
    """
    print("=== Example 9: Simulation with Callbacks ===")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Callback function for monitoring
    def step_callback(step: int, stats: dict):
        """Callback called every 10 steps"""
        if step % 10 == 0:
            print(f"Step {step}: {stats['firing_neurons']} firing, "
                  f"{stats['total_spikes']} total spikes")
    
    # Run simulation with callbacks
    print("Running simulation with monitoring...")
    
    for step in range(100):
        # Inject some activity
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 
                                   10.0f if step % 20 == 0 else 5.0f)
        
        brain.step(step)
        
        # Get statistics
        stats = brain.getStats()
        
        # Call callback
        step_callback(step, stats)
    
    print("Simulation with callbacks complete!")
    print()

# Example 10: Advanced Configuration
def example_advanced_configuration():
    """
    Example 10: Advanced Configuration
    Demonstrates advanced configuration options and customization.
    """
    print("=== Example 10: Advanced Configuration ===")
    
    # Create configuration with advanced settings
    config = pynlm.createDefaultConfig()
    
    # Modify various configuration parameters
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.v_thresh", -45.0)
    config.set("brain.v_rest", -65.0)
    config.set("brain.tau_mem", 15.0)
    
    # Plasticity settings
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.005)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Neuromodulation settings
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    print("Advanced configuration:")
    print(f"  Neuron count: {config.getOr[int64_t]('brain.neuron_count', 1000)}")
    print(f"  Synapse density: {config.getOr[float]('brain.synapse_density', 0.1)}")
    print(f"  STDP enabled: {config.getOr[bool]('plasticity.stdp.enable', True)}")
    print(f"  Dopamine scale: {config.getOr[float]('neuromod.dopamine.scale', 1.0)}")
    print(f"  Curiosity enabled: {config.getOr[bool]('neuromod.curiosity.enable', True)}")
    
    # Create brain with advanced config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print()

# Main function to run all examples
def run_all_examples():
    """
    Run all example scripts to demonstrate NLM capabilities.
    """
    print("=" * 60)
    print("NLM - Comprehensive Examples and Demo Scripts")
    print("=" * 60)
    print()
    
    examples = [
        example_silent_brain,
        example_brain_watching_world,
        example_complete_agent,
        example_brain_statistics,
        example_multi_agent_competition,
        example_development_stages,
        example_error_handling,
        example_performance_benchmark,
        example_simulation_callbacks,
        example_advanced_configuration,
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"Error in {example.__name__}: {e}")
            print()
    
    print("=" * 60)
    print("All examples completed!")
    print("=" * 60)

if __name__ == "__main__":
    # Check if pynlm module has the advanced functions
    if hasattr(pynlm, 'validateConfig'):
        run_all_examples()
    else:
        print("Running basic examples (validateConfig not available)")
        example_silent_brain()
        example_brain_watching_world()
        example_complete_agent()
        print("\nFor advanced features, build the Python bindings with improved API.")
