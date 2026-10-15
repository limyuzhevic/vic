#!/usr/bin/env python3

"""
Example 3: Training Loop

Demonstrates training loops with exploration strategies.
This example shows how to implement learning algorithms.
"""

import sys
import os

# Add parent directory to path for NLM imports
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pynlm
import json
from pathlib import Path

def main():
    print("=== NLM Example 3: Training Loop ===\n")
    
    # Create brain
    print("1. Creating training brain...")
    brain = pynlm.create_brain()
    brain.initialize()
    
    print(f"   Created training brain with {brain.get_total_neuron_count()} neurons")
    
    # Training parameters
    episodes = 10
    steps_per_episode = 100
    exploration_rate = 0.2  # 20% exploration
    
    # Training loop
    print(f"\n2. Starting training ({episodes} episodes, {steps_per_episode} steps each)...")
    
    episode_rewards = []
    episode_steps = []
    
    for episode in range(episodes):
        episode_reward = 0.0
        episode_steps_count = 0
        
        print(f"\n   Training Episode {episode + 1}:")
        
        for step in range(steps_per_episode):
            # Exploration vs exploitation
            explore = (hash(str(episode) + str(step)) % 100) < (exploration_rate * 100)
            
            # Inject input based on exploration strategy
            input_strength = 40.0 if explore else 20.0
            brain.inject_current_to_neurons(pynlm.constants.NEURON_TYPES.SENSORY, 
                input_strength)
            
            # Brain step
            brain.step(step, step * 0.001)
            
            # Collect statistics
            episode_reward += 1.0  # Simple reward
            episode_steps_count += 1
            
            # Print progress every 25 steps
            if step % 25 == 0:
                print(f"     Step {step}: Exploration({explore}), "
                      f"Firing({brain.get_firing_neuron_count()}), "
                      f"Spikes({brain.get_total_spike_count()})")
        
        episode_rewards.append(episode_reward)
        episode_steps.append(episode_steps_count)
        
        # Apply some plasticity
        brain.inject_current_to_neurons(pynlm.constants.NEURON_TYPES.Motor, 10.0f)
    
    # Calculate training statistics
    avg_reward = sum(episode_rewards) / len(episode_rewards)
    avg_steps = sum(episode_steps) / len(episode_steps)
    
    print(f"\n3. Training Results:")
    print(f"   Episodes: {episodes}")
    print(f"   Steps per episode: {steps_per_episode}")
    print(f"   Average reward per episode: {avg_reward:.1f}")
    print(f"   Total steps completed: {sum(episode_steps)}")
    print(f"   Exploration rate: {exploration_rate * 100}%")
    
    # Final brain status
    print(f"\n4. Final Brain Status:")
    print(f"   Total neurons: {brain.get_total_neuron_count()}")
    print(f"   Total synapses: {brain.get_total_synapse_count()}")
    print(f"   Total spikes: {brain.get_total_spike_count()}")
    print(f"   Average firing rate: {brain.get_average_firing_rate():.2f}")
    print(f"   Development stage: {brain.get_developmental_stage()}")
    
    # Save results
    print(f"\n5. Saving results...")
    
    results = {
        "example": "training_loop",
        "episodes": episodes,
        "steps_per_episode": steps_per_episode,
        "exploration_rate": exploration_rate,
        "episode_rewards": episode_rewards,
        "average_reward_per_episode": avg_reward,
        "total_steps_completed": sum(episode_steps),
        "final_brain_stats": {
            "total_neurons": brain.get_total_neuron_count(),
            "total_synapses": brain.get_total_synapse_count(),
            "total_spikes": brain.get_total_spike_count(),
            "average_firing_rate": brain.get_average_firing_rate(),
            "development_stage": str(brain.get_developmental_stage())
        }
    }
    
    results_file = Path("results") / "training_loop_results.json"
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