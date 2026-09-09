"""
NLM Python Examples

This module contains examples demonstrating usage of the NLM neural network
for various tasks and applications.
"""

import numpy as np
from typing import List, Any
from .brain import Brain, create_brain, create_simple_world, create_agent_brain
from .environment import Vision, Action, ActionType
from .convenience import run_simulation, create_simple_brain, create_test_world

# Example 1: Simple Neural Network
def example_simple_brain():
    """Create and run a simple brain without world interaction."""
    print("Example 1: Simple Brain")
    print("-" * 40)
    
    # Create brain
    brain = create_brain()
    brain.initialize()
    
    print(f"Created brain with {brain.get_total_neuron_count()} neurons")
    
    # Run for some steps
    for step in range(10):
        brain.step(step)
        print(f"Step {step}: {brain.get_total_spike_count()} spikes, "
              f"{brain.get_firing_neuron_count()} firing")
    
    print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
    print()


# Example 2: Brain in a World
def example_brain_in_world():
    """Create a brain interacting with a world."""
    print("Example 2: Brain in a World")
    print("-" * 40)
    
    # Create everything
    brain = create_brain()
    brain.initialize()
    
    world = create_simple_world()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    world.set_agent_start(5, 5)
    
    agent = create_agent_brain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    # Run simulation
    for step in range(20):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.get_sensory_percept()
        
        # Tell the brain
        agent.process_sensory_input(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decode_motor_command()
        
        # Do action in world
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Print status every 5 steps
        if step % 5 == 0:
            print(f"Step {step}:")
            print(f"  Position: ({world.agent_body.x:.1f}, {world.agent_body.y:.1f})")
            print(f"  Energy: {world.agent_body.energy:.1f}")
            print(f"  Firing neurons: {brain.get_firing_neuron_count()}")
            print(f"  Action: {action.get_name()}")
    
    print()


# Example 3: Learning with Rewards
def example_learning_with_rewards():
    """Demonstrate learning with reward modulation."""
    print("Example 3: Learning with Rewards")
    print("-" * 40)
    
    # Create brain with reward learning enabled
    brain = create_brain()
    brain.initialize()
    
    world = create_simple_world()
    world.configure(width=15, height=15, vision_width=6, vision_height=6)
    world.reset()
    world.set_agent_start(2, 2)
    
    agent = create_agent_brain(brain)
    agent.initialize(world)
    
    # Enable reward learning
    agent.enable_reward_modulation(True)
    
    # Create a goal (target position)
    goal_x, goal_y = 12, 12
    
    # Run simulation with reward-based learning
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.get_sensory_percept()
        
        # Process input
        agent.process_sensory_input(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decode_motor_command()
        
        # Apply action
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Calculate reward based on proximity to goal
        distance = np.sqrt((world.agent_body.x - goal_x)**2 + 
                          (world.agent_body.y - goal_y)**2)
        reward = max(0, 100 - distance)  # Reward for proximity to goal
        
        # Apply reward modulation
        agent.apply_reward_modulation(reward, 0)  # No predicted reward for now
        
        # Print status
        if step % 10 == 0:
            print(f"Step {step}: Reward={reward:.1f}, Distance={distance:.1f}, "
                  f"Firing={brain.get_firing_neuron_count()}, "
                  f"Stage={brain.get_developmental_stage().name}")
    
    print(f"Final position: ({world.agent_body.x:.1f}, {world.agent_body.y:.1f})")
    print(f"Final distance to goal: {np.sqrt((world.agent_body.x - goal_x)**2 + 
                                          (world.agent_body.y - goal_y)**2):.1f}")
    print()


# Example 4: Curiosity-Driven Exploration
def example_curiosity_driven_exploration():
    """Demonstrate curiosity-driven exploration behavior."""
    print("Example 4: Curiosity-Driven Exploration")
    print("-" * 40)
    
    # Create brain
    brain = create_brain()
    brain.initialize()
    
    world = create_simple_world()
    world.configure(width=25, height=25, vision_width=8, vision_height=8)
    world.reset()
    world.set_agent_start(12, 12)
    
    agent = create_agent_brain(brain)
    agent.initialize(world)
    
    # Enable curiosity
    agent.enable_curiosity(True)
    
    # Create some obstacles in the world
    for x in range(5, 21, 5):
        for y in range(5, 21, 5):
            if (x != 12 or y != 12):
                # Add a wall object
                wall = type('Wall', (), {
                    'x': x, 'y': y, 'type': 'Wall', 
                    'radius': 0.8, 'active': True
                })()
                world.add_object(wall)
    
    # Run simulation with curiosity
    for step in range(60):
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.get_sensory_percept()
        
        # Process input
        agent.process_sensory_input(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decode_motor_command()
        
        # Apply action
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Print curiosity level
        curiosity = agent.get_curiosity_level()
        if step % 10 == 0:
            print(f"Step {step}: Curiosity={curiosity:.3f}, "
                  f"Firing={brain.get_firing_neuron_count()}, "
                  f"Energy={world.agent_body.energy:.1f}")
    
    print(f"Final curiosity level: {agent.get_curiosity_level():.3f}")
    print()


# Example 5: Complete Agent with All Features
def example_complete_agent():
    """Complete agent with all NLM features enabled."""
    print("Example 5: Complete Agent")
    print("-" * 40)
    
    # Create configuration
    config = create_default_config()
    
    # Create brain with all features
    brain = create_brain(config)
    brain.initialize()
    
    # Create world
    world = create_simple_world()
    world.configure(width=30, height=30, vision_width=10, vision_height=10)
    world.reset()
    world.set_agent_start(5, 5)
    
    # Create agent
    agent = create_agent_brain(brain)
    agent.initialize(world)
    
    # Enable all features
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Add some resources and hazards
    for x in [10, 20]:
        for y in [10, 20]:
            # Resource
            resource = type('Resource', (), {
                'x': x, 'y': y, 'type': 'Resource',
                'radius': 0.5, 'value': 10.0, 'active': True
            })()
            world.add_object(resource)
            
            # Hazard
            hazard = type('Hazard', (), {
                'x': x + 5, 'y': y + 5, 'type': 'Hazard',
                'radius': 0.7, 'value': -5.0, 'active': True
            })()
            world.add_object(hazard)
    
    # Run complete simulation
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.get_sensory_percept()
        
        # Process input
        agent.process_sensory_input(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decode_motor_command()
        
        # Apply action
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Apply reward modulation based on action result
        action_result = type('ActionResult', (), {
            'reward': 0.0, 'success': False, 'message': ""
        })()
        
        # Simple reward based on energy change
        action_result.reward = world.agent_body.energy - 100  # Change in energy
        action_result.success = world.agent_body.energy > 90
        
        if action_result.success:
            agent.apply_reward_modulation(action_result.reward, 0)
        
        # Update development every 10 steps
        if step % 10 == 0:
            agent.update_development(0.1)
        
        # Print statistics every 20 steps
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Position: ({world.agent_body.x:.1f}, {world.agent_body.y:.1f})")
            print(f"  Energy: {world.agent_body.energy:.1f}, Health: {world.agent_body.health:.1f}")
            print(f"  Development Stage: {brain.get_developmental_stage().name}")
            print(f"  Neuromodulation: {agent.get_neuromodulation_level():.3f}")
            print(f"  Curiosity: {agent.get_curiosity_level():.3f}")
            print(f"  Prediction Error: {agent.get_prediction_error():.3f}")
            print(f"  Firing neurons: {brain.get_firing_neuron_count()}")
    
    print("Complete agent simulation finished!")
    print(f"Final stage: {brain.get_developmental_stage().name}")
    print()


# Example 6: Advanced Brain Configuration
def example_advanced_configuration():
    """Demonstrate advanced brain configuration."""
    print("Example 6: Advanced Configuration")
    print("-" * 40)
    
    # Create custom configuration
    config = create_default_config()
    
    # Set advanced parameters
    config.set('brain.region_count', 5)
    config.set('brain.population_size', 200)
    config.set('brain.connection_probability', 0.05)
    config.set('brain.mean_synaptic_weight', 0.3)
    config.set('brain.weight_variance', 0.2)
    config.set('plasticity.enable_stdp', True)
    config.set('plasticity.enable_hebbian', True)
    config.set('plasticity.enable_reward_modulated', True)
    config.set('development.enable_structural_plasticity', True)
    config.set('development.critical_period_length', 100)
    config.set('neuromodulation.enable_dopamine', True)
    config.set('neuromodulation.enable_curiosity', True)
    config.set('neuromodulation.enable_novelty', True)
    
    # Create brain with custom config
    brain = create_brain(config)
    brain.initialize()
    
    print(f"Advanced brain configuration:")
    print(f"  Regions: {brain.get_region_count()}")
    print(f"  Total neurons: {brain.get_total_neuron_count()}")
    print(f"  Total synapses: {brain.get_total_synapse_count()}")
    print(f"  Development stage: {brain.get_developmental_stage().name}")
    
    # Run simulation
    for step in range(30):
        brain.step(step)
        if step % 10 == 0:
            print(f"  Step {step}: {brain.get_firing_neuron_count()} firing neurons")
    
    print()


# Example 7: Memory Integration
def example_memory_integration():
    """Demonstrate memory integration in the brain."""
    print("Example 7: Memory Integration")
    print("-" * 40)
    
    # Create brain
    brain = create_brain()
    brain.initialize()
    
    world = create_simple_world()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    world.set_agent_start(2, 2)
    
    agent = create_agent_brain(brain)
    agent.initialize(world)
    
    # Store some experiences in memory
    experiences = []
    for step in range(20):
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.get_sensory_percept()
        
        # Store percept as experience
        agent.process_sensory_input(percept)
        
        # Create a simple experience record
        experience = {
            'step': step,
            'position': (world.agent_body.x, world.agent_body.y),
            'vision': percept.get_vision().get_data() if hasattr(percept.get_vision(), 'get_data') else None,
            'action': agent.decode_motor_command().get_name(),
            'reward': world.agent_body.energy - 100
        }
        experiences.append(experience)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decode_motor_command()
        
        # Apply action
        world.apply_motor_command(action, world.get_simulation_time())
        
        if step % 5 == 0:
            print(f"Step {step}: Stored experience {step + 1}")
    
    print(f"Total experiences stored: {len(experiences)}")
    
    # Retrieve recent experiences
    recent = experiences[-5:]
    print("Recent experiences:")
    for i, exp in enumerate(recent):
        print(f"  {i+1}: Step {exp['step']}, Pos=({exp['position'][0]:.1f}, {exp['position'][1]:.1f}), "
              f"Action={exp['action']}, Reward={exp['reward']:.1f}")
    
    print()


# Example 8: Performance Monitoring
def example_performance_monitoring():
    """Demonstrate performance monitoring capabilities."""
    print("Example 8: Performance Monitoring")
    print("-" * 40)
    
    try:
        from nlm_py.convenience import MetricsCollector
        
        # Create metrics collector
        metrics = MetricsCollector()
        
        # Create brain and run simulation
        brain = create_brain()
        brain.initialize()
        
        world = create_simple_world()
        world.configure(width=15, height=15, vision_width=6, vision_height=6)
        world.reset()
        
        agent = create_agent_brain(brain)
        agent.initialize(world)
        
        # Run simulation while collecting metrics
        for step in range(30):
            world.update(0.1)
            
            percept = world.get_sensory_percept()
            agent.process_sensory_input(percept)
            brain.step(step)
            action = agent.decode_motor_command()
            world.apply_motor_command(action, world.get_simulation_time())
            
            # Collect metrics
            metrics.add_metric('firing_rate', brain.get_average_firing_rate())
            metrics.add_metric('spike_count', brain.get_total_spike_count())
            metrics.add_metric('step_time', 0.1)
        
        # Print metrics report
        print(metrics.format_report())
        
    except ImportError:
        print("Performance monitoring requires additional dependencies")
    
    print()


# Main function to run all examples
def run_all_examples():
    """Run all example demonstrations."""
    print("=" * 60)
    print("NLM Python Examples")
    print("=" * 60)
    print()
    
    examples = [
        ("Simple Brain", example_simple_brain),
        ("Brain in a World", example_brain_in_world),
        ("Learning with Rewards", example_learning_with_rewards),
        ("Curiosity-Driven Exploration", example_curiosity_driven_exploration),
        ("Complete Agent", example_complete_agent),
        ("Advanced Configuration", example_advanced_configuration),
        ("Memory Integration", example_memory_integration),
        ("Performance Monitoring", example_performance_monitoring),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"Error in {name}: {e}")
            import traceback
            traceback.print_exc()
        
        print()
        print("=" * 60)
        print()


if __name__ == "__main__":
    run_all_examples()
