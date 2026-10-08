#!/usr/bin/env python3
"""
NLM Python Example: Agent-Based Simulation
===========================================

This example demonstrates the enhanced NLM API with agent-based simulation,
neuromodulation, and development systems.
"""

import pynlm as nlm
import numpy as np
import pynlm.utils as utils

def main():
    print("NLM Agent-Based Simulation Example")
    print("=" * 50)
    
    # Create configuration for research
    print("1. Creating research configuration...")
    config = utils.ConfigUtils.create_research_config(neuron_count=1000)
    config.set("neuromodulation.dopamine.strength", 1.5)
    config.set("neuromodulation.curiosity.strength", 0.8)
    config.set("development.initial_plasticity", 1.2)
    
    # Create brain and agent
    print("\n2. Creating brain and agent...")
    brain = nlm.create_brain(config)
    agent = nlm.create_agent_brain(brain)
    
    # Create world
    print("\n3. Creating world...")
    world = utils.WorldUtils.create_standard_environment(width=200, height=200)
    
    # Add resources and hazards
    print("\n4. Adding environment features...")
    resources = utils.WorldUtils.add_resources(world, count=15)
    hazards = utils.WorldUtils.add_hazards(world, count=5)
    print(f"   Added {len(resources)} resources and {len(hazards)} hazards")
    
    # Initialize agent
    print("\n5. Initializing agent...")
    agent.initialize(world)
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_development(True)
    
    # Run episode
    print("\n6. Running simulation episode...")
    episode_steps = 50
    
    for step in range(episode_steps):
        # Get sensory percept
        percept = world.get_sensory_percept()
        if percept:
            agent.process_sensory_input(0, 1, 2, 3)  # Dummy values for vision/touch
        
        # Update world
        world.update(step)
        
        # Update agent development
        agent.update_development(step)
        
        # Print progress every 10 steps
        if step % 10 == 0:
            status = agent.get_agent_status()
            body = world.get_agent_body()
            print(f"   Step {step}:")
            print(f"     Position: ({body.x:.1f}, {body.y:.1f})")
            print(f"     Energy: {body.energy:.1f}/{body.health}")
            print(f"     Age: {body.age}")
            print(f"     Developmental stage: {status['developmental_stage']}")
            print(f"     Neuromodulation: {status['neuromodulation_level']:.2f}")
            print(f"     Curiosity: {status['curiosity_level']:.2f}")
            print(f"     Novelty: {status['novelty_level']:.2f}")
            print(f"     Prediction error: {status['prediction_error']:.2f}")
    
    # Analyze results
    print("\n7. Analyzing results...")
    brain_stats = nlm.BrainUtils.get_neuron_statistics(brain)
    print(f"   Brain statistics:")
    print(f"     Total neurons: {brain_stats['total_neurons']}")
    print(f"     Average firing rate: {brain_stats['avg_rate']:.2f} Hz")
    print(f"     E/I ratio: {brain_stats['e_i_ratio']:.2f}")
    
    # Plot results if matplotlib is available
    try:
        import matplotlib.pyplot as plt
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Plot neuromodulation levels
        axes[0, 0].plot([agent.get_agent_status()['neuromodulation_level'] for _ in range(episode_steps)])
        axes[0, 0].set_title('Neuromodulation Level')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Level')
        
        # Plot curiosity levels
        axes[0, 1].plot([agent.get_agent_status()['curiosity_level'] for _ in range(episode_steps)])
        axes[0, 1].set_title('Curiosity Level')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Level')
        
        # Plot agent position over time
        # This would require recording position history
        axes[1, 0].set_title('Agent Trajectory')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Position')
        
        # Plot brain statistics
        axes[1, 1].plot([brain_stats['avg_rate'] for _ in range(episode_steps)])
        axes[1, 1].set_title('Average Firing Rate')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('Hz')
        
        plt.tight_layout()
        plt.savefig('agent_simulation_results.png', dpi=150, bbox_inches='tight')
        print("   Plot saved: agent_simulation_results.png")
        
    except ImportError:
        print("   Matplotlib not available, skipping plots")
    
    # Save results
    print("\n8. Saving results...")
    brain.save("agent_brain.nlm")
    utils.DataUtils.save_to_file(brain_stats, "brain_stats.json")
    print("   Saved brain to: agent_brain.nlm")
    print("   Saved stats to: brain_stats.json")
    
    print("\nExample completed successfully!")

if __name__ == "__main__":
    main()
