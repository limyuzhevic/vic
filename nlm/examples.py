"""
NLM Python Package Examples

Collection of example scripts demonstrating various usage patterns
of the NLM Python API.
"""

from typing import List, Optional
import time
import os
from . import Brain, Agent, SimpleWorld, ConfigManager


def minimal_example():
    """
    Minimal example - just create and run a brain.
    """
    print("=== Minimal Example ===")
    
    # Create brain with default configuration
    brain = Brain()
    print(f"Created brain with {brain.get_total_neuron_count()} neurons")
    
    # Initialize brain
    brain.initialize()
    
    # Run simulation
    for i in range(50):
        brain.step(i)
    
    print(f"Simulation complete! Total spikes: {brain.get_total_spike_count()}")


def world_interaction_example():
    """
    Example of agent-world interaction.
    """
    print("\n=== World Interaction Example ===")
    
    # Create brain
    brain = Brain()
    
    # Create world
    world = SimpleWorld()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    world.set_agent_start(10.0, 10.0)
    
    # Create agent
    agent = Agent(brain)
    agent.initialize(world)
    
    # Enable features
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    # Run simulation loop
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process sensory input
        agent.process_sensory_input(percept)
        
        # Decode motor command
        motor_cmd = agent.decode_motor_command()
        
        # Apply to world
        world.apply_motor_command(motor_cmd, world.get_simulation_time())
        
        # Show progress
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.get_firing_neuron_count()}")
            print(f"  Curiosity: {agent.get_curiosity_level():.3f}")
            print(f"  Agent position: ({agent.get_brain().get_config().get('agent.x', 0.0)}, "
                  f"{agent.get_brain().get_config().get('agent.y', 0.0)})")


def batch_simulation_example():
    """
    Example using batch simulation and performance optimization.
    """
    print("\n=== Batch Simulation Example ===")
    
    # Create brain with optimized configuration
    config = ConfigManager()
    config.set("brain.neuron_count", 1000)
    config.set("brain.connection_probability", 0.05)
    
    brain = Brain(config)
    brain.initialize()
    
    print(f"Running batch simulation with {brain.get_total_neuron_count()} neurons...")
    start_time = time.time()
    
    # Run simulation in batches for better performance
    batch_size = 100
    num_batches = 10
    
    for batch in range(num_batches):
        for step in range(batch_size):
            step_num = batch * batch_size + step
            brain.step(step_num)
        
        # Print batch statistics
        print(f"Batch {batch + 1}/{num_batches}: "
              f"Total spikes: {brain.get_total_spike_count()}, "
              f"Firing: {brain.get_firing_neuron_count()}")
    
    end_time = time.time()
    print(f"\nBatch simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Average steps per second: {(num_batches * batch_size) / (end_time - start_time):.0f}")


def learning_curve_example():
    """
    Example showing learning curve with reward modulation.
    """
    print("\n=== Learning Curve Example ===")
    
    # Create agent with learning enabled
    agent = Agent()
    brain = agent.get_brain()
    
    # Create world with rewards
    world = SimpleWorld()
    world.configure(width=15, height=15, vision_width=8, vision_height=8)
    world.reset()
    world.set_agent_start(7.5, 7.5)
    
    agent.initialize(world)
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    # Track learning progress
    rewards = []
    curiosity_levels = []
    
    for episode in range(20):
        episode_reward = 0.0
        
        for step in range(50):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.get_sensory_percept()
            
            # Process input
            agent.process_sensory_input(percept)
            
            # Decode action
            motor_cmd = agent.decode_motor_command()
            
            # Apply action
            world.apply_motor_command(motor_cmd, world.get_simulation_time())
            
            # Get reward (simulate)
            reward = percept.get_internal()[0] if percept.get_internal() else 0.0
            agent.apply_reward_modulation(reward, 0.0)
            episode_reward += reward
            
            curiosity_levels.append(agent.get_curiosity_level())
        
        rewards.append(episode_reward)
        
        if episode % 5 == 0:
            avg_curiosity = sum(curiosity_levels[-50:]) / len(curiosity_levels[-50:])
            print(f"Episode {episode + 1}: Reward = {episode_reward:.2f}, "
                  f"Avg Curiosity = {avg_curiosity:.3f}")
    
    print(f"\nLearning curve:")
    print(f"  Total rewards: {[f'{r:.2f}' for r in rewards]}")
    print(f"  Average curiosity: {sum(curiosity_levels) / len(curiosity_levels):.3f}")


def checkpoint_example():
    """
    Example demonstrating checkpoint/save functionality.
    """
    print("\n=== Checkpoint Example ===")
    
    # Create and initialize brain
    brain = Brain()
    brain.initialize()
    
    # Run some simulation steps
    for i in range(200):
        brain.step(i)
    
    print(f"Initial state: {brain.get_total_spike_count()} spikes")
    
    # Save checkpoint
    checkpoint_file = "nlm_checkpoint.bin"
    brain.save(checkpoint_file)
    print(f"✓ Saved checkpoint to {checkpoint_file}")
    
    # Modify brain state
    for i in range(50):
        brain.step(200 + i)
    
    print(f"Modified state: {brain.get_total_spike_count()} spikes")
    
    # Reset and reload
    brain.reset()
    brain.load(checkpoint_file)
    print(f"✓ Reloaded checkpoint: {brain.get_total_spike_count()} spikes")
    
    # Clean up
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
        print(f"✓ Cleaned up checkpoint file")


def exploration_excitation_example():
    """
    Example showing curiosity-driven exploration.
    """
    print("\n=== Exploration Example ===")
    
    # Create agent with high curiosity
    agent = Agent()
    world = SimpleWorld()
    world.configure(width=25, height=25, vision_width=8, vision_height=8)
    world.reset()
    world.set_agent_start(12.5, 12.5)
    
    agent.initialize(world)
    agent.enable_curiosity(True)
    agent.enable_reward_modulation(True)
    
    print("Running with curiosity-driven exploration...")
    
    # Track exploration
    movements = {"forward": 0, "backward": 0, "left": 0, "right": 0, "interact": 0, "wait": 0}
    
    for step in range(100):
        world.update(0.1)
        
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        
        motor_cmd = agent.decode_motor_command()
        world.apply_motor_command(motor_cmd, world.get_simulation_time())
        
        # Count movements
        if motor_cmd.name.lower() in movements:
            movements[motor_cmd.name.lower()] += 1
        
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Curiosity: {agent.get_curiosity_level():.3f}")
            print(f"  Prediction error: {agent.get_prediction_error():.3f}")
            print(f"  Novelty: {agent.get_novelty_level():.3f}")
            print(f"  Movement distribution: {movements}")


def comprehensive_demo():
    """
    Comprehensive demonstration combining multiple features.
    """
    print("\n=== Comprehensive Demo ===")
    print("This demo combines multiple NLM features for a complete agent simulation.")
    
    # Create configuration with optimized parameters
    config = ConfigManager()
    config.set("brain.neuron_count", 800)
    config.set("brain.connection_probability", 0.08)
    config.set("neuromod.dopamine.scale", 1.0)
    
    # Create brain, agent, and world
    brain = Brain(config)
    agent = Agent(brain)
    world = SimpleWorld()
    world.configure(width=30, height=30, vision_width=10, vision_height=10)
    world.reset()
    world.set_agent_start(15.0, 15.0)
    
    agent.initialize(world)
    
    # Enable all learning features
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    print(f"Starting comprehensive simulation...")
    print(f"  Brain: {brain.get_total_neuron_count()} neurons, {brain.get_total_synapse_count()} synapses")
    print(f"  Agent enabled: Reward modulation, Structural plasticity, Development, Curiosity")
    
    # Run simulation with detailed logging
    for episode in range(3):
        print(f"\n--- Episode {episode + 1} ---")
        
        for step in range(75):
            # Update world
            world.update(0.05)
            
            # Get observations
            percept = world.get_sensory_percept()
            
            # Process sensory input
            agent.process_sensory_input(percept)
            
            # Run brain
            brain.step(episode * 100 + step)
            
            # Get action
            motor_cmd = agent.decode_motor_command()
            
            # Apply action
            world.apply_motor_command(motor_cmd, world.get_simulation_time())
            
            # Apply learning
            reward = percept.get_internal()[0] if percept.get_internal() else 0.0
            agent.apply_reward_modulation(reward, 0.0)
            
            # Update development
            agent.update_development(0.05)
            
            # Log progress every 15 steps
            if step % 15 == 0:
                print(f"  Step {step + episode * 75}: "
                      f"Neurons={brain.get_firing_neuron_count()}, "
                      f"Curiosity={agent.get_curiosity_level():.3f}, "
                      f"Dev={agent.get_developmental_stage().name}")
    
    print("\n✓ Comprehensive demo completed successfully!")
    print(f"Final stats:")
    print(f"  Total spikes: {brain.get_total_spike_count()}")
    print(f"  Final curiosity: {agent.get_curiosity_level():.3f}")
    print(f"  Final novelty: {agent.get_novelty_level():.3f}")
    print(f"  Development stage: {agent.get_developmental_stage().name}")


def main():
    """Run all examples."""
    print("NLM Python Package Examples")
    print("=" * 40)
    
    try:
        minimal_example()
        world_interaction_example()
        batch_simulation_example()
        learning_curve_example()
        checkpoint_example()
        exploration_excitation_example()
        comprehensive_demo()
        
        print("\n" + "=" * 40)
        print("All examples completed successfully!")
        print("\nFor more advanced usage, refer to the documentation.")
        
    except Exception as e:
        print(f"\nExample failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0


if __name__ == "__main__":
    exit(main())
