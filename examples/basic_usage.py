#!/usr/bin/env python3
"""
Basic NLM Usage Example

This example demonstrates the enhanced Python API for the NLM (Neural Learning Machine) framework.
It shows how to create and run a simple brain simulation using the new Pythonic interface.
"""

import time
import nlm

def main():
    print("=== NLM Python API Example ===\n")
    
    # Create a brain with enhanced configuration
    print("1. Creating brain with custom configuration...")
    config = nlm.create_default_config()
    config.update({
        'neuron_count': 500,
        'region_count': 2,
        'simulation_timestep': 0.001,
        'enable_reward_modulation': True,
        'enable_curiosity': True,
        'enable_development': True,
    })
    
    brain = nlm.create_brain(config)
    print(f"   Created brain with {brain.get_total_neuron_count()} neurons")
    
    # Initialize the brain
    print("\n2. Initializing brain...")
    brain.initialize()
    print("   Brain initialized successfully")
    
    # Create a world
    print("\n3. Creating simulation world...")
    world = nlm.create_simple_world()
    world.configure(width=20.0, height=20.0, vision_width=8, vision_height=8)
    world.set_agent_start(10.0, 10.0)
    print(f"   Created world {world.get_width()}x{world.get_height()} with vision {world.get_vision_width()}x{world.get_vision_height()}")
    
    # Create an agent
    print("\n4. Creating agent interface...")
    agent = nlm.create_agent_brain(brain)
    agent.initialize(world)
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    print("   Agent initialized successfully")
    
    # Run simulation
    print("\n5. Running simulation...")
    num_steps = 100
    
    for step in range(num_steps):
        # Get sensory percept
        percept = world.get_sensory_percept()
        
        # Process sensory input
        agent.process_sensory_input(percept)
        
        # Step the brain
        brain.step(step)
        
        # Get motor command
        action = agent.decode_motor_command()
        
        # Apply action to world
        result = world.apply_motor_command(action, step * 0.1)
        
        # Apply reward modulation
        agent.apply_reward_modulation(result.reward)
        
        # Update development
        agent.update_development(0.1)
        
        # Print progress every 20 steps
        if step % 20 == 0:
            stats = {
                'step': step,
                'neurons_firing': brain.get_firing_neuron_count(),
                'total_spikes': brain.get_total_spike_count(),
                'avg_firing_rate': brain.get_average_firing_rate(),
                'dopamine': agent.get_neuromodulation_level(),
                'curiosity': agent.get_curiosity_level(),
                'reward': result.reward,
            }
            print(f"   Step {step}: {stats}")
    
    # Print final statistics
    print("\n6. Final Simulation Statistics...")
    
    # Get brain statistics
    brain_stats = {
        'neurons': brain.get_total_neuron_count(),
        'synapses': brain.get_total_synapse_count(),
        'firing': brain.get_firing_neuron_count(),
        'spikes': brain.get_total_spike_count(),
        'rate': brain.get_average_firing_rate(),
        'e/i_ratio': brain.get_excitation_inhibition_ratio(),
    }
    
    print(f"   Brain Statistics:")
    for key, value in brain_stats.items():
        if isinstance(value, float):
            print(f"     {key}: {value:.2f}")
        else:
            print(f"     {key}: {value}")
    
    # Get agent statistics
    agent_stats = {
        'neuromodulation': agent.get_neuromodulation_level(),
        'curiosity': agent.get_curiosity_level(),
        'novelty': agent.get_novelty_level(),
        'prediction_error': agent.get_prediction_error(),
        'developmental_stage': agent.get_developmental_stage().name,
    }
    
    print(f"\n   Agent Statistics:")
    for key, value in agent_stats.items():
        print(f"     {key}: {value}")
    
    # Save simulation state
    print("\n7. Saving simulation state...")
    brain.save("./test_simulation.bin")
    print("   Simulation saved to './test_simulation.bin'")
    
    # Demonstrate context manager usage
    print("\n8. Demonstrating context manager usage...")
    
    # Use the BrainSession context manager
    with nlm.BrainSession(config) as session:
        session.create_world(16.0, 16.0, 8, 8)
        session.create_agent()
        session.run(50)
        
        session_stats = session.get_stats()
        print(f"   Session stats: {session_stats}")
    
    print("\n=== Simulation Complete ===")
    print("\nKey improvements demonstrated:")
    print("• Enhanced Pythonic API (easy to use)")
    print("• Context manager for automatic cleanup")
    print("• Comprehensive configuration management")
    print("• Rich monitoring and statistics")
    print("• Memory-efficient session management")
    print("• Advanced features (neuromodulation, development, curiosity)")

if __name__ == "__main__":
    main()