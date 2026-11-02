#!/usr/bin/env python3
"""
NLM - Neural Learning Machine

This is the enhanced Python interface for the NLM (Neural Learning Machine) framework.
It provides a Pythonic way to create, simulate, and interact with artificial brains.

This script contains comprehensive examples for both beginners and advanced users.
"""

import pynlm
import json
import time
import sys

def example_1_basic_brain():
    """
    Example 1: Basic Brain Simulation
    
    This is the simplest possible example showing how to create and run a brain.
    """
    print("=" * 60)
    print("Example 1: Basic Brain Simulation")
    print("=" * 60)
    
    # Create a brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Initial synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation
    for i in range(100):
        brain.step(i)
    
    print(f"After 100 steps:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print()

def example_2_world_simulation():
    """
    Example 2: Brain in a Simple World
    
    This example shows how to create a complete agent simulation with a world.
    """
    print("=" * 60)
    print("Example 2: Brain in a Simple World")
    print("=" * 60)
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"Created agent simulation in {world.getWidth()}x{world.getHeight()} world")
    
    # Run simulation using enhanced context manager
    with pynlm.agent_simulation(preset="research") as sim:
        sim.runSteps(50)
        print(f"After 50 steps:")
        print(f"  Neursons: {sim.getStatistics()['neurons']}")
        print(f"  Firing neurons: {sim.getStatistics()['firing_neurons']}")
        print(f"  Curiosity: {sim.getStatistics()['curiosity']:.3f}")
        print(f"  Novelty: {sim.getStatistics()['novelty']:.3f}")
    
    # Run additional steps without context manager
    brain.step(51)
    print(f"After step 51:")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print()

def example_3_advanced_configuration():
    """
    Example 3: Advanced Configuration
    
    This example shows the enhanced configuration system with presets and validation.
    """
    print("=" * 60)
    print("Example 3: Advanced Configuration")
    print("=" * 60)
    
    # Method 1: Create from preset
    print("Creating research configuration preset...")
    config1 = pynlm.EnhancedConfig.createFromPreset("research")
    print(f"Research preset neurons: {config1.get('neuron_count')}")
    print(f"Research preset regions: {config1.get('region_count')}")
    print(f"Research preset timestep: {config1.get('simulation_timestep')}")
    print()
    
    # Method 2: Create custom configuration
    print("Creating custom configuration...")
    config2 = pynlm.EnhancedConfig.createFromPreset("basic")
    config2.set("neuron_count", 1000)
    config2.set("region_count", 2)
    config2.set("log_level", "DEBUG")
    print(f"Custom neurons: {config2.get('neuron_count')}")
    print(f"Custom regions: {config2.get('region_count')}")
    print(f"Custom log level: {config2.get('log_level')}")
    print()
    
    # Validate configuration
    print("Validating configurations...")
    if config1.isValid():
        print("✓ Research configuration is valid")
    else:
        print("✗ Research configuration has errors:")
        for error in config1.getValidationErrors():
            print(f"  - {error}")
    
    if config2.isValid():
        print("✓ Custom configuration is valid")
    else:
        print("✗ Custom configuration has errors:")
        for error in config2.getValidationErrors():
            print(f"  - {error}")
    print()

def example_4_batch_simulation():
    """
    Example 4: Batch Simulation for Statistical Analysis
    
    This example shows how to run multiple simulations for statistical analysis.
    """
    print("=" * 60)
    print("Example 4: Batch Simulation for Statistical Analysis")
    print("=" * 60)
    
    print("Running 5 simulation episodes with different random seeds...")
    results = pynlm.SimulationUtils.batchRunEpisodes(
        num_episodes=5,
        steps_per_episode=100,
        preset="research"
    )
    
    # Analyze results
    total_rewards = [r["total_reward"] for r in results]
    avg_rewards = [r["avg_reward_per_step"] for r in results]
    
    print(f"\nResults Summary:")
    print(f"  Episodes: {len(results)}")
    print(f"  Average total reward: {sum(total_rewards) / len(total_rewards):.2f}")
    print(f"  Average reward per step: {sum(avg_rewards) / len(avg_rewards):.4f}")
    print(f"  Standard deviation: {sum((x - sum(avg_rewards)/len(avg_rewards))**2 for x in avg_rewards) / len(avg_rewards):.4f}")
    
    print(f"\nIndividual episode results:")
    for i, result in enumerate(results):
        print(f"  Episode {i}: reward={result['total_reward']:.2f}, "
              f"avg={result['avg_reward_per_step']:.4f}, "
              f"curiosity={result['final_curiosity']:.3f}")
    print()

def example_5_context_manager_usage():
    """
    Example 5: Context Manager Usage
    
    This example demonstrates the enhanced context managers for resource management.
    """
    print("=" * 60)
    print("Example 5: Context Manager Usage")
    print("=" * 60)
    
    print("Using agent simulation context manager...")
    with pynlm.agent_simulation(preset="research") as sim:
        print(f"Created simulation with {sim.getStatistics()['neurons']} neurons")
        
        # Method chaining
        sim.runSteps(10).runSteps(20).runSteps(30)
        
        print(f"After 60 steps:")
        print(f"  Total spikes: {sim.getStatistics()['total_spikes']}")
        print(f"  Statistics table:")
        print(sim.formatStatistics("table"))
    
    print("\nContext manager automatically cleaned up resources")
    print()

def example_6_configuration_validation():
    """
    Example 6: Configuration Validation
    
    This example shows the configuration validation system.
    """
    print("=" * 60)
    print("Example 6: Configuration Validation")
    print("=" * 60)
    
    # Create invalid configuration
    print("Testing invalid configuration...")
    config = pynlm.EnhancedConfig.createFromPreset("basic")
    config.set("neuron_count", -1)  # Invalid: negative neurons
    
    try:
        pynlm.validateConfig(config)
        print("✓ Configuration is valid (unexpected)")
    except Exception as e:
        print(f"✓ Configuration validation caught error: {e}")
    
    # Show valid configuration
    print("\nTesting valid configuration...")
    config2 = pynlm.EnhancedConfig.createFromPreset("research")
    config2.set("neuron_count", 500)
    
    try:
        pynlm.validateConfig(config2)
        print("✓ Configuration is valid")
    except Exception as e:
        print(f"✗ Configuration validation failed: {e}")
    
    print()

def example_7_complete_agent_example():
    """
    Example 7: Complete Agent Example
    
    This is a complete agent simulation example that demonstrates all major features.
    """
    print("=" * 60)
    print("Example 7: Complete Agent Example")
    print("=" * 60)
    
    def run_agent_simulation(num_steps=500):
        """Run a complete agent simulation with all features."""
        
        # 1. Create configuration
        print("1. Creating configuration...")
        config = pynlm.EnhancedConfig.createFromPreset("research")
        
        # 2. Create and initialize brain
        print("2. Creating and initializing brain...")
        brain = pynlm.createBrain(config.getConfig())
        brain.initialize()
        
        # 3. Create agent brain interface
        print("3. Creating agent brain interface...")
        agent = pynlm.createAgentBrain(brain)
        
        # 4. Create and configure world
        print("4. Creating and configuring world...")
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # 5. Initialize agent with world
        print("5. Initializing agent with world...")
        agent.initialize(world)
        
        # 6. Enable all subsystems
        print("6. Enabling subsystems...")
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # 7. Run simulation
        print("7. Running simulation...")
        with pynlm.agent_simulation("advanced", brain, world, agent) as sim:
            # Run in chunks with progress reporting
            for chunk in range(0, num_steps, 100):
                chunk_size = min(100, num_steps - chunk)
                sim.runSteps(chunk_size)
                
                if chunk % 200 == 0:
                    stats = sim.getStatistics()
                    print(f"  Step {chunk}: {stats['firing_neurons']} firing neurons, "
                          f"curiosity={stats['curiosity']:.3f}, "
                          f"novelty={stats['novelty']:.3f}")
                    
                    # Save checkpoint
                    brain.save(f"checkpoint_step_{chunk}.bin")
        
        # 8. Save final statistics
        print("8. Saving final statistics...")
        with open("simulation_stats.txt", "w") as f:
            f.write(sim.formatStatistics("json"))
        
        print("Agent simulation complete!")
    
    # Run the simulation
    run_agent_simulation(500)
    print()

def example_8_importing_and_using():
    """
    Example 8: Basic Import and Usage
    
    This shows the basic import and usage patterns.
    """
    print("=" * 60)
    print("Example 8: Basic Import and Usage")
    print("=" * 60)
    
    print("Import: import pynlm")
    print()
    
    print("Creating brain:")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    print(f"  Created brain with {brain.getTotalNeuronCount()} neurons")
    print()
    
    print("Initializing brain:")
    brain.initialize()
    print("  Brain initialized")
    print()
    
    print("Running simulation:")
    for i in range(10):
        brain.step(i)
    print(f"  Completed 10 steps, {brain.getTotalSpikeCount()} spikes")
    print()
    
    print("Checking development stage:")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print()
    
    print("Getting configuration summary:")
    print(f"  {brain.getConfig().summary()}")
    print()

def main():
    """
    Main function that runs all examples.
    """
    print("NLM (Neural Learning Machine) - Enhanced Python Bindings Examples")
    print("=" * 70)
    print()
    
    # Run all examples
    example_8_importing_and_using()
    example_1_basic_brain()
    example_2_world_simulation()
    example_3_advanced_configuration()
    example_4_batch_simulation()
    example_5_context_manager_usage()
    example_6_configuration_validation()
    example_7_complete_agent_example()
    
    print("=" * 60)
    print("All examples completed successfully!")
    print("=" * 60)
    print()
    print("To run individual examples:")
    print("  python examples.py --example 1  # Example 1: Basic Brain Simulation")
    print("  python examples.py --example 2  # Example 2: Brain in a Simple World")
    print("  ... and so on")
    print()

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "--help":
        print(__doc__)
    else:
        main()