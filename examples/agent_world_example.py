#!/usr/bin/env python3

"""
Example 2: Agent-World Interaction

Demonstrates how an agent interacts with a simulated world.
This example shows sensory processing, motor control, and learning.
"""

import sys
import os

# Add parent directory to path for NLM imports
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pynlm
import json
import time
from pathlib import Path

def main():
    print("=== NLM Example 2: Agent-World Interaction ===\n")
    
    # Setup
    print("1. Setting up simulation...")
    
    # Create brain
    brain = pynlm.create_brain()
    brain.initialize()
    
    # Create world
    world = pynlm.create_simple_world()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.set_agent_start(10.0, 10.0)
    
    # Create agent
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    print(f"   World size: {world.get_width()}x{world.get_height()}")
    print(f"   Agent start: (10.0, 10.0)")
    print(f"   Enabled subsystems: Reward, Structural Plasticity, Development, Curiosity")
    
    # Run simulation
    print("\n2. Running agent-world simulation...")
    
    total_reward = 0.0
    episodes = 5
    steps_per_episode = 50
    
    for episode in range(episodes):
        print(f"\n   Episode {episode + 1}:")
        
        for step in range(steps_per_episode):
            # Update world
            world.update(0.1)
            
            # Get percept and process
            percept = world.get_sensory_percept()
            agent.process_sensory_input(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action and execute
            action = agent.decode_motor_command()
            world.apply_motor_command(action, world.get_simulation_time())
            
            # Compute reward
            body = world.get_agent_body()
            reward = 1.0 if (0 <= body.x < world.get_width() and 
                           0 <= body.y < world.get_height()) else 0.0
            total_reward += reward
            
            # Apply neuromodulation
            agent.apply_reward_modulation(reward, 0.0)
            agent.update_development(0.1)
            
            # Print progress every 10 steps
            if step % 10 == 0:
                print(f"     Step {step}: Pos({body.x:.1f}, {body.y:.1f}), "
                      f"Reward({reward:.1f}), "
                      f"Curiosity({agent.get_curiosity_level():.2f})")
    
    # Get final statistics
    print("\n3. Collecting final statistics...")
    
    final_curiosity = agent.get_curiosity_level()
    final_novelty = agent.get_novelty_level()
    final_dopamine = agent.get_neuromodulation_level()
    
    print(f"\n   Episode Results:")
    print(f"     Total reward: {total_reward:.1f}")
    print(f"     Average reward per step: {total_reward / (episodes * steps_per_episode):.3f}")
    print(f"     Final curiosity: {final_curiosity:.3f}")
    print(f"     Final novelty: {final_novelty:.3f}")
    print(f"     Final dopamine level: {final_dopamine:.3f}")
    print(f"     Development stage: {brain.get_developmental_stage()}")
    
    # Save results
    print("\n4. Saving results...")
    
    results = {
        "example": "agent_world",
        "episodes": episodes,
        "steps_per_episode": steps_per_episode,
        "total_reward": total_reward,
        "average_reward_per_step": total_reward / (episodes * steps_per_episode),
        "final_curiosity": final_curiosity,
        "final_novelty": final_novelty,
        "final_dopamine": final_dopamine,
        "final_development_stage": str(brain.get_developmental_stage())
    }
    
    results_file = Path("results") / "agent_world_results.json"
    with open(results_file, "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"   Results saved to: {results_file}")
    
    print(f"\n✓ Example completed successfully!")
    return True

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\n✗ Error: {e}")
        sys.exit(1)