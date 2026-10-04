#!/usr/bin/env python3
"""
Example script demonstrating the use of NLM Python bindings.

This script shows how to create and use an artificial brain with NLM,
demonstrating the core functionality of the neural simulation framework.
"""

import pynlm

def example_silent_brain():
    """Example 1: Create a silent brain (just neurons)"""
    print("=== Example 1: Silent Brain ===")
    
    # Create a brain with default configuration
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    
    # Initialize the brain
    brain.initialize()
    
    # Run the brain for 10 steps (neurons are active but we don't process sensory input)
    for i in range(10):
        brain.step(i)
        
    print(f"Neurons: {brain.get_total_neuron_count()}")
    print(f"Total spikes: {brain.get_total_spike_count()}")
    print(f"Firing neurons: {brain.get_firing_neuron_count()}")
    print()

def example_brain_watching_world():
    """Example 2: Brain watching a world"""
    print("=== Example 2: Brain Watching a World ===")
    
    # Setup
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    world = pynlm.create_simple_world()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    
    # Watch the world for 30 steps
    for i in range(30):
        world.update(0.1)
        agent.process_sensory_input(world.get_sensory_percept())
        brain.step(i)
        
    print(f"Total neurons: {brain.get_total_neuron_count()}")
    print(f"Total spikes: {brain.get_total_spike_count()}")
    print(f"Firing neurons: {brain.get_firing_neuron_count()}")
    print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
    print()

def example_complete_agent():
    """Example 3: Complete agent with learning capabilities"""
    print("=== Example 3: Complete Agent ===")
    
    # Setup
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    world = pynlm.create_simple_world()
    world.configure(width=15, height=15, vision_width=8, vision_height=8)
    world.reset()
    
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    
    # Run agent
    for step in range(100):
        world.update(0.1)
        agent.process_sensory_input(world.get_sensory_percept())
        brain.step(step)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.get_firing_neuron_count()}")
            print(f"  Curiosity: {agent.get_curiosity_level():.3f}")
            print(f"  Novelty: {agent.get_novelty_level():.3f}")
            print(f"  Dev Stage: {brain.get_developmental_stage()}")
    
    print(f"Agent simulation complete!")
    print(f"Final stats:")
    print(f"  Total neurons: {brain.get_total_neuron_count()}")
    print(f"  Total spikes: {brain.get_total_spike_count()}")
    print(f"  Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
    print()

def example_configuration():
    """Example 4: Advanced configuration"""
    print("=== Example 4: Advanced Configuration ===")
    
    # Create and configure a brain
    config = pynlm.create_default_config()
    
    # Modify configuration parameters
    config.set("neuron_count", 500)
    config.set("region_count", 2)
    config.set("connection_probability", 0.15)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.025)
    config.set("stdp_tau", 30.0)
    
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    print("Brain configuration:")
    print(f"  Neurons: {brain.get_total_neuron_count()}")
    print(f"  Regions: {brain.get_region_count()}")
    print(f"  Connections: {brain.get_total_synapse_count()}")
    print()

def example_checkpointing():
    """Example 5: Checkpoint save/load"""
    print("=== Example 5: Checkpoint Save/Load ===")
    
    # Create and run a brain
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    for i in range(50):
        brain.step(i)
    
    print(f"Running brain for {brain.get_total_spike_count()} steps")
    
    # Save brain state
    brain.save("checkpoint_brain.nlm")
    print("Saved brain state to 'checkpoint_brain.nlm'")
    
    # Create a new brain and load the saved state
    config2 = pynlm.create_default_config()
    brain2 = pynlm.create_brain(config2)
    brain2.load("checkpoint_brain.nlm")
    
    print("Loaded brain state from checkpoint")
    print(f"Loaded brain neurons: {brain2.get_total_neuron_count()}")
    print(f"Loaded brain spikes: {brain2.get_total_spike_count()}")
    print()

def example_multi_step_simulation():
    """Example 6: Multi-step simulation with statistics"""
    print("=== Example 6: Multi-Step Simulation ===")
    
    # Setup complete agent
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    world = pynlm.create_simple_world()
    world.configure(width=10, height=10, vision_width=6, vision_height=6)
    world.reset()
    
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    # Run simulation with periodic statistics
    for step in range(200):
        world.update(0.1)
        agent.process_sensory_input(world.get_sensory_percept())
        brain.step(step)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Print statistics every 50 steps
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  E/I ratio: {brain.get_excitation_inhibition_ratio():.2f}")
            print(f"  Firing neurons: {brain.get_firing_neuron_count()}")
            print(f"  Total spikes: {brain.get_total_spike_count()}")
            print(f"  Curiosity: {agent.get_curiosity_level():.3f}")
            print(f"  Novelty: {agent.get_novelty_level():.3f}")
    
    print(f"Final simulation complete!")
    print(f"Final stats:")
    print(f"  E/I ratio: {brain.get_excitation_inhibition_ratio():.2f}")
    print(f"  Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
    print()

def main():
    """Run all examples"""
    print("NLM Python Bindings Examples")
    print("=" * 40)
    print()
    
    example_silent_brain()
    example_brain_watching_world()
    example_complete_agent()
    example_configuration()
    example_checkpointing()
    example_multi_step_simulation()
    
    print("All examples completed successfully!")
    print()
    print("For more information, see:")
    print("- HOW_TO_USE.md - Comprehensive documentation")
    print("- easy_usage.md - Quick start guide")
    print("- docs/ARCHITECTURE.md - System architecture")

if __name__ == "__main__":
    main()
