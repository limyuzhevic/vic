#!/usr/bin/env python3
"""
Example 4: Neuromodulation and Learning

This example demonstrates how neuromodulation systems (dopamine, curiosity,
novelty) affect learning and behavior in NLM.
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt

def neuromodulation_learning():
    """Demonstrate neuromodulation effects on learning."""
    
    print("=== Example 4: Neuromodulation and Learning ===")
    
    # Create brain with neuromodulation
    config = pynlm.createDefaultConfig()
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.curiosity.noise_level", 0.3)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=12, height=12, visionWidth=5, visionHeight=5)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Neuromodulation configuration:")
    print(f"  Dopamine scale: {config.get('neuromod.dopamine.scale')}")
    print(f"  Curiosity enabled: {config.get('neuromod.curiosity.enable')}")
    print(f"  Novelty enabled: {config.get('neuromod.novelty.enable')}")
    print(f"  Curiosity noise level: {config.get('neuromod.curiosity.noise_level')}")
    
    # Collect neuromodulation metrics
    data = {
        'steps': [],
        'dopamine_levels': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'rewards': [],
        'learning_rates': []
    }
    
    # Run simulation
    print("\nRunning simulation with neuromodulation...")
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get actions and rewards
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get neuromodulation levels
        dopamine = agent.getNeuromodulationLevel()
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        
        # Get reward
        reward = np.random.random() * 0.3  # Simulated reward
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Collect data
        data['steps'].append(step)
        data['dopamine_levels'].append(dopamine)
        data['curiosity_levels'].append(curiosity)
        data['novelty_levels'].append(novelty)
        data['rewards'].append(reward)
        
        # Calculate learning rate (simplified)
        learning_rate = dopamine * curiosity * 0.01
        data['learning_rates'].append(learning_rate)
        
        # Print progress
        if step % 50 == 0:
            print(f"  Step {step}: Dopamine={dopamine:.3f}, "
                  f"Curiosity={curiosity:.3f}, Novelty={novelty:.3f}, "
                  f"Reward={reward:.3f}")
    
    # Analyze neuromodulation effects
    print("\n--- Neuromodulation Analysis ---")
    
    # Plot neuromodulation levels
    plt.figure(figsize=(12, 10))
    
    # Dopamine levels
    plt.subplot(3, 2, 1)
    plt.plot(data['steps'], data['dopamine_levels'], 'r-', linewidth=2)
    plt.title('Dopamine Levels Over Time')
    plt.ylabel('Dopamine Level')
    plt.xlabel('Step')
    plt.grid(True, alpha=0.3)
    
    # Curiosity levels
    plt.subplot(3, 2, 2)
    plt.plot(data['steps'], data['curiosity_levels'], 'g-', linewidth=2)
    plt.title('Curiosity Levels Over Time')
    plt.ylabel('Curiosity Level')
    plt.xlabel('Step')
    plt.grid(True, alpha=0.3)
    
    # Novelty levels
    plt.subplot(3, 2, 3)
    plt.plot(data['steps'], data['novelty_levels'], 'b-', linewidth=2)
    plt.title('Novelty Levels Over Time')
    plt.ylabel('Novelty Level')
    plt.xlabel('Step')
    plt.grid(True, alpha=0.3)
    
    # Reward vs Curiosity correlation
    plt.subplot(3, 2, 4)
    plt.scatter(data['rewards'], data['curiosity_levels'], alpha=0.6, c=data['steps'], cmap='viridis')
    plt.colorbar(label='Step')
    plt.title('Reward vs Curiosity')
    plt.xlabel('Reward')
    plt.ylabel('Curiosity Level')
    
    # Learning rate analysis
    plt.subplot(3, 2, 5)
    plt.plot(data['steps'], data['learning_rates'], 'm-', linewidth=2)
    plt.title('Learning Rate Over Time')
    plt.ylabel('Learning Rate')
    plt.xlabel('Step')
    plt.grid(True, alpha=0.3)
    
    # Dopamine vs Learning Rate correlation
    plt.subplot(3, 2, 6)
    correlation = np.corrcoef(data['dopamine_levels'], data['learning_rates'])[0, 1]
    plt.scatter(data['dopamine_levels'], data['learning_rates'], alpha=0.6)
    plt.title(f'Dopamine vs Learning Rate (r={correlation:.3f})')
    plt.xlabel('Dopamine Level')
    plt.ylabel('Learning Rate')
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('neuromodulation_analysis.png')
    plt.show()
    
    # Statistical analysis
    print("\n--- Neuromodulation Statistics ---")
    
    dopamine_mean = np.mean(data['dopamine_levels'])
    dopamine_std = np.std(data['dopamine_levels'])
    curiosity_mean = np.mean(data['curiosity_levels'])
    curiosity_std = np.std(data['curiosity_levels'])
    novelty_mean = np.mean(data['novelty_levels'])
    novelty_std = np.std(data['novelty_levels'])
    reward_mean = np.mean(data['rewards'])
    
    print(f"Dopamine: mean={dopamine_mean:.3f}, std={dopamine_std:.3f}")
    print(f"Curiosity: mean={curiosity_mean:.3f}, std={curiosity_std:.3f}")
    print(f"Novelty: mean={novelty_mean:.3f}, std={novelty_std:.3f}")
    print(f"Reward: mean={reward_mean:.3f}, std={np.std(data['rewards']):.3f}")
    print(f"Learning rate correlation with dopamine: {correlation:.3f}")
    
    print("\nNeuromodulation demonstration complete!")
    return brain, world, agent

if __name__ == "__main__":
    neuromodulation_learning()