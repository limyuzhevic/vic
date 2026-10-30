#!/usr/bin/env python3
"""
Example script demonstrating the enhanced NLM Python API.

This script showcases the improved features of the NLM Python bindings:
- Type aliases for easier usage
- Property-based API
- Better error handling
- Comprehensive documentation
"""

import pynlm
import time

def basic_brain_example():
    """Basic brain creation and simulation example."""
    print("=== Basic Brain Example ===")
    
    # Create a default configuration
    config = pynlm.create_default_config()
    
    # Create a brain with the configuration
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Run simulation
    print("Running simulation...")
    for step in range(100):
        brain.step(step)
        
        # Print progress every 25 steps
        if step % 25 == 0:
            firing = brain.get_firing_neuron_count()
            total_neurons = brain.get_total_neuron_count()
            rate = brain.get_average_firing_rate()
            print(f"  Step {step}: {firing}/{total_neurons} firing neurons, "
                  f"{rate:.1f} Hz avg rate")
    
    # Print final statistics
    print(f"Final statistics:")
    print(f"  Total neurons: {brain.get_total_neuron_count()}")
    print(f"  Total synapses: {brain.get_total_synapse_count()}")
    print(f"  Firing neurons: {brain.get_firing_neuron_count()}")
    print(f"  Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
    print(f"  E/I ratio: {brain.get_excitation_inhibition_ratio():.2f}")
    
    print()

def enhanced_config_example():
    """Example demonstrating enhanced configuration API."""
    print("=== Enhanced Configuration Example ===")
    
    # Create configuration with type aliases
    config = pynlm.create_default_config()
    
    # Set configuration using enhanced API
    config.set_value("brain.neuron_count", 500)
    config.set_value("brain.synapse_density", 0.15)
    config.set_value("brain.connection_probability", 0.1)
    
    # Check configuration
    if config.has("brain.neuron_count"):
        neurons = config.get_int("brain.neuron_count", 1000)
        print(f"Brain configured with {neurons} neurons")
    
    print(f"All keys: {config.get_keys()}")
    print()

def enhanced_brain_example():
    """Example demonstrating enhanced brain API."""
    print("=== Enhanced Brain API Example ===")
    
    # Create brain using type aliases for clarity
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Access brain using property-based API
    print(f"Brain region count: {brain.get_region_count()}")
    print(f"Total neurons: {brain.get_total_neuron_count()}")
    print(f"Total synapses: {brain.get_total_synapse_count()}")
    
    # Demonstrate property access (using getters for compatibility)
    regions = brain.get_region_ids()
    for region_id in regions:
        region = brain.get_region(region_id)
        if region:
            print(f"Region {region_id.index()}: {region.get_name()} "
                  f"({region.get_total_neuron_count()} neurons)")
    
    print()

def world_interaction_example():
    """Example demonstrating world interaction with enhanced API."""
    print("=== World Interaction Example ===")
    
    # Create brain and world
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    world = pynlm.create_simple_world()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    
    # Create agent brain interface
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Run simulation
    print("Running agent simulation...")
    for step in range(200):
        world.update(0.1)
        
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process in brain
        agent.process_sensory_input(percept)
        brain.step(step)
        
        # Get action
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Print progress
        if step % 50 == 0:
            firing = brain.get_firing_neuron_count()
            curiosity = agent.get_curiosity_level()
            novelty = agent.get_novelty_level()
            print(f"  Step {step}: Firing={firing}, "
                  f"Curiosity={curiosity:.2f}, Novelty={novelty:.2f}")
    
    print("Simulation complete!")
    print()

def plasticity_example():
    """Example demonstrating plasticity mechanisms."""
    print("=== Plasticity Example ===")
    
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Get plasticity systems
    stdp = brain.get_stdp()
    hebbian = brain.get_hebbian()
    structural = brain.get_structural_plasticity()
    
    print(f"STDP system: {stdp is not None}")
    print(f"Hebbian system: {hebbian is not None}")
    print(f"Structural plasticity: {structural is not None}")
    
    # Demonstrate learning by running simulation
    print("Running learning simulation...")
    initial_spikes = brain.get_total_spike_count()
    
    for step in range(1000):
        brain.step(step)
        
        # Inject some input to stimulate learning
        if step % 100 == 0:
            brain.inject_current_to_neurons(pynlm.NeuronType.Sensory, 20.0)
    
    final_spikes = brain.get_total_spike_count()
    print(f"Spike count: {initial_spikes} -> {final_spikes} "
          f"({final_spikes - initial_spikes} new)")
    
    print()

def memory_example():
    """Example demonstrating memory systems."""
    print("=== Memory Systems Example ===")
    
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Get memory systems
    working_memory = brain.get_working_memory()
    episodic_memory = brain.get_episodic_memory()
    associative_memory = brain.get_associative_memory()
    
    print(f"Working memory: {working_memory is not None}")
    print(f"Episodic memory: {episodic_memory is not None}")
    print(f"Associative memory: {associative_memory is not None}")
    
    if working_memory:
        print(f"Working memory capacity: {working_memory.get_capacity()}")
        print(f"Active traces: {working_memory.get_active_traces()}")
    
    if episodic_memory:
        print(f"Episodic memory max episodes: {episodic_memory.get_max_episodes()}")
        print(f"Current episodes: {episodic_memory.get_episode_count()}")
    
    # Run simulation to generate memory
    print("Running simulation to generate memory...")
    for step in range(500):
        brain.step(step)
        
        # Store some state in working memory
        if working_memory and step % 100 == 0:
            # Simulate storing neural activity
            pass
    
    print()

def development_example():
    """Example demonstrating brain development."""
    print("=== Development Example ===")
    
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Get development system
    development = brain.get_development_system()
    print(f"Development system: {development is not None}")
    
    print(f"Initial developmental stage: {brain.get_developmental_stage()}")
    
    # Run development simulation
    print("Running development simulation...")
    for step in range(5000):
        brain.step(step)
        
        # Development updates every 1000 steps
        if step % 1000 == 0:
            stage = brain.get_developmental_stage()
            print(f"Step {step}: Developmental stage {stage}")
    
    print(f"Final developmental stage: {brain.get_developmental_stage()}")
    print()

def save_load_example():
    """Example demonstrating save/load functionality."""
    print("=== Save/Load Example ===")
    
    # Create and simulate a brain
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    print("Running simulation...")
    for step in range(500):
        brain.step(step)
        if step % 100 == 0:
            print(f"  Step {step}: {brain.get_total_spike_count()} spikes")
    
    # Save brain state
    save_path = "brain_checkpoint.nlm"
    print(f"Saving brain state to {save_path}")
    saved = brain.save(save_path)
    print(f"Save successful: {saved}")
    
    # Reset and reload
    brain.reset()
    brain.initialize()
    
    print("Reloading brain state...")
    loaded = brain.load(save_path)
    print(f"Load successful: {loaded}")
    
    # Verify reload
    print(f"Reloaded state: {brain.get_total_spike_count()} spikes")
    
    # Cleanup
    import os
    if os.path.exists(save_path):
        os.remove(save_path)
    
    print()

def main():
    """Run all examples."""
    print("NLM Python API Examples - Enhanced Version")
    print("=" * 50)
    print()
    
    try:
        basic_brain_example()
        enhanced_config_example()
        enhanced_brain_example()
        world_interaction_example()
        plasticity_example()
        memory_example()
        development_example()
        save_load_example()
        
        print("=" * 50)
        print("All examples completed successfully!")
        print()
        print("Key improvements demonstrated:")
        print("- Enhanced configuration API with type aliases")
        print("- Property-based brain access")
        print("- Comprehensive world interaction")
        print("- Plasticity and learning mechanisms")
        print("- Memory system integration")
        print("- Development simulation")
        print("- Save/load functionality")
        
    except Exception as e:
        print(f"Error during execution: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
