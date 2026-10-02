#!/usr/bin/env python3
"""
Example usage of pynlm with improved Pythonic API

This demonstrates the improved Python bindings for NLM (Neural Learning Machine).
The API now supports context managers, operator overloading, and convenience methods.
"""

import pynlm as nlm

def basic_usage_example():
    """Basic usage example showing context manager and convenience methods"""
    print("=== Basic Usage Example ===")
    
    # Use context manager for automatic resource cleanup
    with nlm.Brain.create() as brain:
        # Configure brain with parameters
        config = brain.get_config()
        config.set("neuron_count", 1000)
        config.set("region_count", 3)
        config.set("connection_probability", 0.1)
        
        # Initialize brain
        brain.initialize()
        
        print(f"Brain initialized: {brain.get_total_neuron_count()} neurons")
        print(f"Regions: {brain.get_region_count()}")
        
        # Run simulation for a few steps
        for step in range(5):
            brain.step(step)
            if step % 2 == 0:
                print(f"Step {step}: Active neurons = {brain.get_active_neuron_count()}")
        
        # Get statistics
        print(f"Total spikes: {brain.get_total_spike_count()}")
        print(f"Average firing rate: {brain.get_average_firing_rate():.2f}")
        
    print("Brain automatically cleaned up (context manager)\n")

def sensory_motor_loop_example():
    """Example of sensory-motor loop using AgentBrain"""
    print("=== Sensory-Motor Loop Example ===")
    
    # Create brain and agent components
    brain = nlm.create_brain()
    world = nlm.create_simple_world(64, 64, 16, 16)  # width, height, vision width, vision height
    agent_brain = nlm.create_agent_brain(brain)
    
    # Initialize agent
    agent_brain.initialize(world)
    
    # Run episode
    world.set_agent_start(32, 32)
    
    for episode in range(3):
        print(f"\n--- Episode {episode} ---")
        
        # Reset world and agent
        world.reset()
        agent_brain.reset()
        
        # Run simulation steps
        for step in range(10):
            # Get current state
            percept = world.get_sensory_percept()
            body = world.get_agent_body()
            
            print(f"  Step {step}: Position ({body.x:.1f}, {body.y:.1f}), "
                  f"Energy {body.energy:.1f}, Health {body.health:.1f}")
            
            # Process sensory input
            agent_brain.process_sensory_input(percept)
            
            # Decode action and apply to world
            action = agent_brain.decode_motor_command()
            world.apply_motor_command(action, step)
            
            # Apply reward and neuromodulation
            reward = body.health / 100.0  # Simple reward based on health
            agent_brain.apply_reward_modulation(reward, 0.0)
            
            # Update development
            agent_brain.update_development(0.1)
            
            # Advance world
            world.update(0.1)
        
        # Print final stats
        print(f"  Episode end: Reward = {agent_brain.getNeuromodulationLevel():.2f}, "
              f"Curiosity = {agent_brain.getCuriosityLevel():.2f}")

def batch_operations_example():
    """Example of batch operations for advanced usage"""
    print("=== Batch Operations Example ===")
    
    # Create multiple brains for parallel simulation
    brains = []
    for i in range(3):
        brain = nlm.Brain.create(f"brain_{i}")  # Use specific name
        config = brain.get_config()
        config.set("neuron_count", 500 + i * 100)
        config.set("region_count", 2)
        config.set("random_seed", 42 + i)
        brain.initialize()
        brains.append(brain)
    
    print(f"Created {len(brains)} brains for parallel simulation")
    
    # Run synchronized steps
    for step in range(5):
        step_data = []
        
        for brain in brains:
            brain.step(step)
            step_data.append({
                'spikes': brain.get_total_spike_count(),
                'active': brain.get_active_neuron_count(),
                'rate': brain.get_average_firing_rate()
            })
        
        # Print batch statistics
        print(f"Step {step}:")
        for i, data in enumerate(step_data):
            print(f"  Brain {i}: {data['spikes']} spikes, "
                  f"{data['active']} active, rate={data['rate']:.2f}")
    
    # Clean up
    for brain in brains:
        brain.reset()

def advanced_config_example():
    """Example of advanced configuration and analysis"""
    print("=== Advanced Configuration Example ===")
    
    # Create config and brain
    config = nlm.Config.create()
    brain = nlm.Brain(config)
    
    # Configure with complex settings
    config.set("neuron_count", 2000)
    config.set("region_count", 5)
    config.set("connection_probability", 0.05)
    config.set("simulation_timestep", 0.0005)
    config.set("replay_interval", 200)
    config.set("consolidation_interval", 2000)
    
    # Set plasticity parameters
    config.set("stdp_ltp_weight", 0.015)
    config.set("stdp_ltd_weight", 0.018)
    config.set("stdp_tau", 25.0)
    config.set("synaptogenesis_rate", 0.0002)
    config.set("pruning_rate", 0.00002f)
    
    brain.initialize()
    
    # Analyze configuration
    print("Configuration summary:")
    print(f"  Total neurons: {brain.get_total_neuron_count()}")
    print(f"  Total synapses: {brain.get_total_synapse_count()}")
    print(f"  E/I ratio: {brain.get_excitation_inhibition_ratio():.3f}")
    print(f"  Simulation timestep: {brain.get_config().getOr('simulation_timestep', 0.0)}")
    
    # Run simulation
    for step in range(20):
        brain.step(step)
        
        if step % 10 == 0:
            print(f"Step {step}: {brain.get_total_spike_count()} total spikes, "
                  f"{brain.get_average_firing_rate():.2f} Hz average rate")
    
    # Save brain state
    brain.save("advanced_brain_checkpoint.nlm")
    print(f"Brain saved to checkpoint file")

def error_handling_example():
    """Example of improved error handling"""
    print("=== Error Handling Example ===")
    
    try:
        # Create brain with invalid configuration
        brain = nlm.Brain.create()
        config = brain.get_config()
        
        # This should handle invalid values gracefully
        config.set("neuron_count", -100)  # Invalid value
        brain.initialize()  # Should use defaults or warn
        
        print("Initialized with invalid config (should use defaults)")
        
    except Exception as e:
        print(f"Error caught: {type(e).__name__}: {e}")

def introspection_example():
    """Example of introspection and analysis capabilities"""
    print("=== Introspection Example ===")
    
    brain = nlm.Brain.create()
    config = brain.get_config()
    
    # Configure brain
    config.set("neuron_count", 1000)
    config.set("region_count", 3)
    brain.initialize()
    
    # Introspect brain structure
    print("Brain Structure:")
    print(f"  Regions: {brain.get_region_count()}")
    for i, region_id in enumerate(brain.get_region_ids()):
        region = brain.get_region(region_id)
        if region:
            print(f"    Region {i+1} (ID {region_id.index()}): "
                  f"{region.get_total_neuron_count()} neurons, "
                  f"{region.get_synapse_count()} synapses")
    
    # Get detailed statistics
    print(f"\nDetailed Statistics:")
    print(f"  Total neuron count: {brain.get_total_neuron_count()}")
    print(f"  Total synapse count: {brain.get_total_synapse_count()}")
    print(f"  Active neuron count: {brain.get_active_neuron_count()}")
    print(f"  Firing neuron count: {brain.get_firing_neuron_count()}")
    print(f"  Total spike count: {brain.get_total_spike_count()}")
    print(f"  Pending spike events: {brain.get_pending_spike_event_count()}")
    print(f"  Excitation/Inhibition ratio: {brain.get_excitation_inhibition_ratio():.3f}")
    print(f"  Average firing rate: {brain.get_average_firing_rate():.3f} Hz")
    
    # Get memory system status
    working_memory = brain.get_working_memory()
    episodic_memory = brain.get_episodic_memory()
    
    if working_memory:
        print(f"  Working memory traces: {working_memory.getActiveTraces()}")
    if episodic_memory:
        print(f"  Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
    
    # Get neuromodulation status
    dopamine = brain.get_dopamine()
    curiosity = brain.get_curiosity()
    
    if dopamine:
        print(f"  Dopamine level: {dopamine.get_level():.3f}")
    if curiosity:
        print(f"  Curiosity level: {curiosity.get_level():.3f}")

if __name__ == "__main__":
    print("pynlm - Improved Neural Learning Machine Python Bindings\n")
    
    basic_usage_example()
    sensory_motor_loop_example()
    batch_operations_example()
    advanced_config_example()
    error_handling_example()
    introspection_example()
    
    print("\n=== All examples completed successfully! ===")
