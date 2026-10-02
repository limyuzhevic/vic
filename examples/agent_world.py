"""
Complete agent-world simulation example for NLM.

This example demonstrates a complete agent-world simulation including
agent creation, sensory input, motor output, and world interaction.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from pynlm import (
    create_default_config, create_brain, create_simple_world, create_agent_brain,
    create_simple_sensory_input, create_vision_input, create_audio_input,
    create_internal_signals_input
)
import time
import random

def create_sensory_percept_from_data(vision_data=None, audio_data=None, 
                                      internal_data=None, proprioception_data=None):
    """Create a sensory percept from data."""
    from pynlm.core_ext import SensoryPercept
    
    percept = SensoryPercept()
    
    if vision_data:
        vision = create_vision_input(
            width=len(vision_data[0]) if vision_data else 64,
            height=len(vision_data) if vision_data else 64,
            channels=3
        )
        vision.set_data(vision_data)
        percept.set_vision(vision)
    
    if audio_data:
        audio = create_audio_input(
            sample_rate=22050,
            num_samples=len(audio_data)
        )
        audio.set_data(audio_data)
        percept.set_audio(audio)
    
    if internal_data:
        internal = create_internal_signals_input()
        internal.add_signals(internal_data)
        percept.set_internal(internal)
    
    if proprioception_data:
        percept.set_proprioception(proprioception_data)
    
    return percept

def agent_world_simulation():
    """Run agent-world simulation."""
    print("=== Agent-World Simulation Example ===")
    
    # Create world
    print("Creating world...")
    world = create_simple_world()
    world.configure(width=200, height=200, vision_width=32, vision_height=32)
    world.set_agent_start(100, 100)
    world.set_max_energy(100.0)
    world.set_energy_decay_rate(0.01)
    
    print(f"World configured: {world.get_width()}x{world.get_height()}")
    
    # Create brain and agent
    print("\nCreating neural brain...")
    config = create_default_config()
    config.set("neuron_count", 1000)
    config.set("region_count", 3)
    config.set("connection_probability", 0.1)
    config.set("seed", 123)
    
    brain = create_brain(config)
    brain.initialize()
    
    print(f"Brain initialized: {brain.get_total_neuron_count()} neurons")
    
    # Create agent brain
    agent_brain = create_agent_brain(brain)
    agent_brain.initialize(world)
    
    print(f"Agent brain initialized")
    
    # Enable advanced features
    agent_brain.enable_reward_modulation(True)
    agent_brain.enable_structural_plasticity(True)
    agent_brain.enable_development(True)
    agent_brain.enable_curiosity(True)
    
    print("\nAdvanced features enabled:")
    print(f"  Reward modulation: {agent_brain.is_reward_modulation_enabled()}")
    print(f"  Structural plasticity: {agent_brain.is_structural_plasticity_enabled()}")
    print(f"  Development: {agent_brain.is_development_enabled()}")
    print(f"  Curiosity: {agent_brain.is_curiosity_enabled()}")
    
    # Simulation loop
    print("\nStarting simulation loop...")
    start_time = time.time()
    
    for step in range(200):
        # Get sensory input
        vision_data = [
            [random.random() * 0.1 for _ in range(32)] 
            for _ in range(32)
        ]
        audio_data = [random.random() for _ in range(100)]
        internal_data = [random.random() for _ in range(10)]
        
        percept = create_sensory_percept_from_data(
            vision_data, audio_data, internal_data
        )
        
        # Process sensory input
        agent_brain.process_sensory_input(percept)
        
        # Get action
        action = agent_brain.decode_motor_command()
        
        # Apply action in world
        result = world.apply_motor_command(action, world.get_simulation_time())
        
        # Apply reward modulation
        reward = result.reward if result.success else -0.1
        agent_brain.apply_reward_modulation(reward, 0.5)
        
        # Update development
        agent_brain.update_development(1.0)
        
        # Print progress
        if step % 40 == 0:
            brain_status = brain.log_status()
            body = world.get_agent_body()
            print(f"\nStep {step}:")
            print(f"  Action: {action.getType().name if hasattr(action, 'getType') else 'Unknown'}")
            print(f"  Reward: {reward}")
            print(f"  Agent pos: ({body.x:.1f}, {body.y:.1f})")
            print(f"  Agent energy: {body.energy:.1f}")
            print(f"  Neuromodulation: {agent_brain.get_neuromodulation_level():.3f}")
            print(f"  Curiosity: {agent_brain.get_curiosity_level():.3f}")
            print(f"  Novelty: {agent_brain.get_novelty_level():.3f}")
            print(f"  Development stage: {agent_brain.get_developmental_stage().name}")
    
    end_time = time.time()
    
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    
    # Final status
    print("\nFinal agent status:")
    brain.log_status()
    
    body = world.get_agent_body()
    print(f"\nFinal agent state:")
    print(f"  Position: ({body.x:.1f}, {body.y:.1f})")
    print(f"  Energy: {body.energy:.1f}")
    print(f"  Health: {body.health}")
    print(f"  Age: {body.age}")
    print(f"  Simulation time: {world.get_simulation_time():.1f}")
    
    # Save results
    brain.save("agent_world_brain.nml")
    config.save_to_file("agent_world_config.json")
    
    print("\nResults saved:")
    print("  Brain state: 'agent_world_brain.nml'")
    print("  Configuration: 'agent_world_config.json'")
    
    return brain, world, agent_brain

if __name__ == "__main__":
    agent_world_simulation()