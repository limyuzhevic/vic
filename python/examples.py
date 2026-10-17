#!/usr/bin/env python3
"""
NLM Python Examples
===================

Comprehensive examples demonstrating the NLM Python interface.

This file contains practical examples that showcase different aspects of the
NLM Python API, from basic usage to advanced agent simulations.
"""

import nlm
import sys
import os

def run_example_1():
    """Example 1: Basic Brain Creation and Simulation"""
    print("=" * 60)
    print("Example 1: Basic Brain Creation and Simulation")
    print("=" * 60)

    # Create a simple brain
    config = nlm.create_config(preset='quick_dev')
    brain = nlm.create_brain(size='medium')
    brain.initialize()

    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Initial firing rate: {brain.getAverageFiringRate():.3f}")

    # Run a simple simulation
    for step in range(50):
        brain.step(step)

        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")

    print("Basic simulation completed successfully!\n")

def run_example_2():
    """Example 2: Complete Agent Simulation with World"""
    print("=" * 60)
    print("Example 2: Complete Agent Simulation with World")
    print("=" * 60)

    def run_agent_simulation_example(num_steps=200):
        """Run a complete agent simulation demonstrating the full NLM ecosystem."""

        # Create configuration
        config = nlm.create_config(preset='learning')

        # Create and initialize brain
        brain = nlm.create_brain(preset='learning')
        brain.initialize()

        # Create world
        world = nlm.create_world(width=20, height=20,
                                 vision_width=8, vision_height=8,
                                 agent_start=(10.0, 10.0))

        # Create agent brain interface
        agent = nlm.createAgentBrain(brain)

        # Initialize agent with world
        agent.initialize(world)

        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)

        print(f"Agent simulation initialized:")
        print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  - Expected sensory input: {agent.getSensoryInputSize()} features")
        print(f"  - Expected motor output: {agent.getMotorOutputSize()} actions")
        print()

        # Run simulation with callback for monitoring
        def progress_callback(step_stats):
            if step_stats['step'] % 50 == 0:
                print(f"Step {step_stats['step']}:")
                print(f"  - Active neurons: {step_stats['active_neurons']}")
                print(f"  - Firing rate: {step_stats['firing_rate']:.3f}")
                print(f"  - Curiosity: {step_stats['curiosity']:.3f}")
                print(f"  - Novelty: {step_stats['novelty']:.3f}")
                print(f"  - Development stage: {step_stats['dev_stage']}")
                print()

        stats = nlm.run_simulation(world, brain, agent, num_steps,
                                   timestep=0.1, callback=progress_callback)

        # Print final statistics
        print("=" * 60)
        print("Simulation Results:")
        print("=" * 60)
        print(f"Total simulation steps: {stats.step_count}")
        print(f"Final active neurons: {stats.get_active_neurons()}")
        print(f"Average firing rate: {stats.get_firing_rate():.3f}")
        print(f"Final E/I ratio: {stats.get_ei_ratio():.3f}")
        print(f"Final spike count: {stats.get_total_spikes()}")
        print(f"Final curiosity level: {stats.get_curiosity_level():.3f}")
        print(f"Final novelty level: {stats.get_novelty_level():.3f}")
        print(f"Final development stage: {stats.get_development_stage()}")

        return stats

    # Run the example
    agent_stats = run_agent_simulation_example(200)
    print("Agent simulation completed successfully!\n")

def run_example_3():
    """Example 3: Configuration Management and Presets"""
    print("=" * 60)
    print("Example 3: Configuration Management and Presets")
    print("=" * 60)

    # Demonstrate different presets
    presets = ['quick_dev', 'high_perf', 'learning', 'production']

    for preset in presets:
        print(f"\nPreset: {preset.upper()}")
        print("-" * 40)

        config = nlm.create_config(preset=preset)

        # Display some key configuration values
        key_params = [
            'brain.neuron_count',
            'brain.synapse_density',
            'plasticity.stdp.enable',
            'neuromod.curiosity.enable',
            'neuromod.novelty.enable'
        ]

        for param in key_params:
            if config.has(param):
                value = config.get(param)
                print(f"  {param}: {value}")

def run_example_4():
    """Example 4: Error Handling and Robust Operations"""
    print("\n" + "=" * 60)
    print("Example 4: Error Handling and Robust Operations")
    print("=" * 60)

    # Demonstrate error handling
    print("\n1. Testing invalid preset error handling:")
    try:
        config = nlm.create_config(preset='invalid_preset')
    except nlm.ConfigError as e:
        print(f"   Caught ConfigError: {e}")

    print("\n2. Testing operation error handling:")
    config = nlm.createDefaultConfig()
    result = nlm.save_config(config, "test_config.json")

    if result.success:
        print("   Configuration saved successfully")
        # Clean up
        if os.path.exists("test_config.json"):
            os.remove("test_config.json")
            print("   Cleanup completed")
    else:
        print(f"   Error saving configuration: {result.error}")

def run_example_5():
    """Example 5: Brain Factory and Advanced Configuration"""
    print("\n" + "=" * 60)
    print("Example 5: Brain Factory and Advanced Configuration")
    print("=" * 60)

    # Demonstrate BrainFactory usage
    factory = nlm.BrainFactory()

    print("\n1. Creating different brain sizes:")
    for size in ['small', 'medium', 'large']:
        brain = factory.create_brain(size=size)
        print(f"   {size.capitalize()} brain: {brain.getTotalNeuronCount()} neurons")

    print("\n2. Creating brain with specific features:")
    brain_with_dev = factory.create_brain_with_dev(development_enabled=True)
    print(f"   Brain with development: {brain_with_dev.getDevelopmentalStage()}")

    brain_with_neuromod = factory.create_brain_with_neuromod(neuromodulation_enabled=True)
    print(f"   Brain with neuromodulation created")

    print("\n3. Creating custom brain configuration:")
    custom_config = {
        'brain.neuron_count': 3000,
        'brain.synapse_density': 0.3,
        'plasticity.stdp.learning_rate': 0.002,
        'neuromod.curiosity.enable': True
    }

    custom_brain = factory.create_brain(custom_config=custom_config)
    print(f"   Custom brain: {custom_brain.getTotalNeuronCount()} neurons")

def run_example_6():
    """Example 6: World Helper and Simulation Variations"""
    print("\n" + "=" * 60)
    print("Example 6: World Helper and Simulation Variations")
    print("=" * 60)

    world_helper = nlm.WorldHelper()

    print("\n1. Creating different world sizes:")
    for width, height in [(10, 10), (20, 20), (30, 30)]:
        world = world_helper.create_world(width=width, height=height)
        print(f"   {width}x{height} world: agent start at ({world.getAgentBody().x}, {world.getAgentBody().y})")

    print("\n2. Creating world with specific agent start:")
    world_with_start = world_helper.create_world(width=25, height=25,
                                                agent_start=(12.5, 12.5))
    agent_body = world_with_start.getAgentBody()
    print(f"   Agent at position: ({agent_body.x}, {agent_body.y})")

def run_example_7():
    """Example 7: Simulation Statistics and Analysis"""
    print("\n" + "=" * 60)
    print("Example 7: Simulation Statistics and Analysis")
    print("=" * 60)

    # Create necessary objects for this example
    brain = nlm.create_brain(preset='quick_dev')
    brain.initialize()

    world = nlm.create_world(width=10, height=10)
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)

    # Run a simulation to collect statistics
    stats = nlm.run_simulation(world, brain, agent, steps=100)

    print("\nSimulation Statistics Analysis:")
    print("-" * 40)
    print(f"Step count: {stats.step_count}")
    print(f"Active neurons: {stats.get_active_neurons()}")
    print(f"Firing rate: {stats.get_firing_rate():.3f}")
    print(f"E/I ratio: {stats.get_ei_ratio():.3f}")
    print(f"Total spikes: {stats.get_total_spikes()}")
    print(f"Curiosity level: {stats.get_curiosity_level():.3f}")
    print(f"Novelty level: {stats.get_novelty_level():.3f}")
    print(f"Development stage: {stats.get_development_stage()}")

    # Export statistics to JSON
    import json
    stats_dict = stats.to_dict()
    print(f"\nStatistics exported to dictionary with {len(stats_dict)} keys")

def run_all_examples():
    """Run all examples in sequence."""
    print("Running NLM Python Interface Examples\n")

    run_example_1()
    run_example_2()
    run_example_3()
    run_example_4()
    run_example_5()
    run_example_6()
    run_example_7()

    print("\n" + "=" * 60)
    print("All examples completed successfully!")
    print("=" * 60)

def main():
    """Main entry point for examples script."""
    # Check if we can import and use the bindings
    try:
        # Simple test to see if bindings are available
        print("Testing NLM Python interface...")
        print(f"Version: {nlm.__version__}")
        print(f"Description: {nlm.__description__}")
        print()

        # Run all examples
        run_all_examples()

    except ImportError as e:
        print(f"Error: Could not import pynlm C++ bindings: {e}")
        print("The C++ bindings may not be built yet.")
        print("To build them, run:")
        print("  cd build")
        print("  cmake ..")
        print("  make")
        print("  pip install -e python/")
        return 1
    except Exception as e:
        print(f"Error running examples: {e}")
        return 1

    return 0

if __name__ == '__main__':
    sys.exit(main())

# Create a simple brain
config = nlm.create_config(preset='quick_dev')
brain = nlm.create_brain(size='medium')
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"Initial firing rate: {brain.getAverageFiringRate():.3f}")

# Run a simple simulation
for step in range(50):
    brain.step(step)
    
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
              f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")

print("Basic simulation completed successfully!\n")

# Example 2: Complete Agent Simulation with World
print("=" * 60)
print("Example 2: Complete Agent Simulation with World")
print("=" * 60)

def run_agent_simulation_example(num_steps=200):
    """Run a complete agent simulation demonstrating the full NLM ecosystem."""
    
    # Create configuration
    config = nlm.create_config(preset='learning')
    
    # Create and initialize brain
    brain = nlm.create_brain(preset='learning')
    brain.initialize()
    
    # Create world
    world = nlm.create_world(width=20, height=20, 
                            vision_width=8, vision_height=8,
                            agent_start=(10.0, 10.0))
    
    # Create agent brain interface
    agent = nlm.createAgentBrain(brain)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Agent simulation initialized:")
    print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Expected sensory input: {agent.getSensoryInputSize()} features")
    print(f"  - Expected motor output: {agent.getMotorOutputSize()} actions")
    print()
    
    # Run simulation with callback for monitoring
    def progress_callback(step_stats):
        if step_stats['step'] % 50 == 0:
            print(f"Step {step_stats['step']}:")
            print(f"  - Active neurons: {step_stats['active_neurons']}")
            print(f"  - Firing rate: {step_stats['firing_rate']:.3f}")
            print(f"  - Curiosity: {step_stats['curiosity']:.3f}")
            print(f"  - Novelty: {step_stats['novelty']:.3f}")
            print(f"  - Development stage: {step_stats['dev_stage']}")
            print()
    
    stats = nlm.run_simulation(world, brain, agent, num_steps, 
                              timestep=0.1, callback=progress_callback)
    
    # Print final statistics
    print("=" * 60)
    print("Simulation Results:")
    print("=" * 60)
    print(f"Total simulation steps: {stats.step_count}")
    print(f"Final active neurons: {stats.get_active_neurons()}")
    print(f"Average firing rate: {stats.get_firing_rate():.3f}")
    print(f"Final E/I ratio: {stats.get_ei_ratio():.3f}")
    print(f"Final spike count: {stats.get_total_spikes()}")
    print(f"Final curiosity level: {stats.get_curiosity_level():.3f}")
    print(f"Final novelty level: {stats.get_novelty_level():.3f}")
    print(f"Final development stage: {stats.get_development_stage()}")
    
    return stats

# Run the example
agent_stats = run_agent_simulation_example(200)
print("Agent simulation completed successfully!\n")

# Example 3: Configuration Management and Presets
print("=" * 60)
print("Example 3: Configuration Management and Presets")
print("=" * 60)

# Demonstrate different presets
presets = ['quick_dev', 'high_perf', 'learning', 'production']

for preset in presets:
    print(f"\nPreset: {preset.upper()}")
    print("-" * 40)
    
    config = nlm.create_config(preset=preset)
    
    # Display some key configuration values
    key_params = [
        'brain.neuron_count',
        'brain.synapse_density', 
        'plasticity.stdp.enable',
        'neuromod.curiosity.enable',
        'neuromod.novelty.enable'
    ]
    
    for param in key_params:
        if config.has(param):
            value = config.get(param)
            print(f"  {param}: {value}")

# Example 4: Error Handling and Robust Operations
print("\n" + "=" * 60)
print("Example 4: Error Handling and Robust Operations")
print("=" * 60)

# Demonstrate error handling
print("\n1. Testing invalid preset error handling:")
try:
    config = nlm.create_config(preset='invalid_preset')
except nlm.ConfigError as e:
    print(f"   Caught ConfigError: {e}")

print("\n2. Testing operation error handling:")
config = nlm.createDefaultConfig()
result = nlm.save_config(config, "test_config.json")

if result.success:
    print("   Configuration saved successfully")
    # Clean up
    import os
    if os.path.exists("test_config.json"):
        os.remove("test_config.json")
        print("   Cleanup completed")
else:
    print(f"   Error saving configuration: {result.error}")

# Example 5: Brain Factory and Advanced Configuration
print("\n" + "=" * 60)
print("Example 5: Brain Factory and Advanced Configuration")
print("=" * 60)

# Demonstrate BrainFactory usage
factory = nlm.BrainFactory()

print("\n1. Creating different brain sizes:")
for size in ['small', 'medium', 'large']:
    brain = factory.create_brain(size=size)
    print(f"   {size.capitalize()} brain: {brain.getTotalNeuronCount()} neurons")

print("\n2. Creating brain with specific features:")
brain_with_dev = factory.create_brain_with_dev(development_enabled=True)
print(f"   Brain with development: {brain_with_dev.getDevelopmentalStage()}")

brain_with_neuromod = factory.create_brain_with_neuromod(neuromodulation_enabled=True)
print(f"   Brain with neuromodulation created")

print("\n3. Creating custom brain configuration:")
custom_config = {
    'brain.neuron_count': 3000,
    'brain.synapse_density': 0.3,
    'plasticity.stdp.learning_rate': 0.002,
    'neuromod.curiosity.enable': True
}

custom_brain = factory.create_brain(custom_config=custom_config)
print(f"   Custom brain: {custom_brain.getTotalNeuronCount()} neurons")

# Example 6: World Helper and Simulation Variations
print("\n" + "=" * 60)
print("Example 6: World Helper and Simulation Variations")
print("=" * 60)

world_helper = nlm.WorldHelper()

print("\n1. Creating different world sizes:")
for width, height in [(10, 10), (20, 20), (30, 30)]:
    world = world_helper.create_world(width=width, height=height)
    print(f"   {width}x{height} world: agent start at ({world.getAgentBody().x}, {world.getAgentBody().y})")

print("\n2. Creating world with specific agent start:")
world_with_start = world_helper.create_world(width=25, height=25, 
                                            agent_start=(12.5, 12.5))
agent_body = world_with_start.getAgentBody()
print(f"   Agent at position: ({agent_body.x}, {agent_body.y})")

# Example 7: Simulation Statistics and Analysis
print("\n" + "=" * 60)
print("Example 7: Simulation Statistics and Analysis")
print("=" * 60)

# Run a simulation to collect statistics
stats = nlm.run_simulation(world, brain, agent, steps=100)

print("\nSimulation Statistics Analysis:")
print("-" * 40)
print(f"Step count: {stats.step_count}")
print(f"Active neurons: {stats.get_active_neurons()}")
print(f"Firing rate: {stats.get_firing_rate():.3f}")
print(f"E/I ratio: {stats.get_ei_ratio():.3f}")
print(f"Total spikes: {stats.get_total_spikes()}")
print(f"Curiosity level: {stats.get_curiosity_level():.3f}")
print(f"Novelty level: {stats.get_novelty_level():.3f}")
print(f"Development stage: {stats.get_development_stage()}")

# Export statistics to JSON
import json
stats_dict = stats.to_dict()
print(f"\nStatistics exported to dictionary with {len(stats_dict)} keys")

print("\n" + "=" * 60)
print("All examples completed successfully!")
print("=" * 60)