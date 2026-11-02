"""
Example demonstrating learning and training patterns
in the agent simulation.
"""
import pynlm
import numpy as np
import matplotlib.pyplot as plt
import time

def training_example():
    """Demonstrate learning and training over multiple episodes."""
    print("=== Agent Training Example ===")
    
    # Training configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    
    # Training parameters
    num_episodes = 5
    steps_per_episode = 100
    exploration_rate = 0.1  # Probability of random action
    
    # Track training statistics
    episode_rewards = []
    episode_lengths = []
    curiosity_levels = []
    novelty_levels = []
    
    print(f"Training for {num_episodes} episodes...")
    
    for episode in range(num_episodes):
        print(f"\n--- Episode {episode + 1}/{num_episodes} ---")
        
        # Reset world and agent for new episode
        world.reset()
        world.setAgentStart(7.5, 7.5)
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run episode
        episode_reward = 0.0
        episode_steps = 0
        
        for step in range(steps_per_episode):
            # Get state
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Choose action (epsilon-greedy)
            motor_cmd = agent.decodeMotorCommand()
            
            # Occasionally take random action for exploration
            if np.random.random() < exploration_rate:
                motor_cmd = pynlm.MotorCommand(pynlm.MotorCommand.Wait)
            
            # Execute action
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            brain.step(step)
            
            # Calculate reward
            internal_signals = percept.getInternal()
            reward = internal_signals[0] if internal_signals else 0.0
            
            # Apply reward with prediction error
            predicted_reward = 0.0  # Simple model
            agent.applyRewardModulation(reward, predicted_reward)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            episode_reward += reward
            episode_steps += 1
            
            # Log statistics periodically
            if step % 20 == 0:
                print(f"  Step {step}: reward={reward:.3f}, "
                      f"curiosity={agent.getCuriosityLevel():.3f}, "
                      f"novelty={agent.getNoveltyLevel():.3f}")
        
        # Record episode statistics
        episode_rewards.append(episode_reward)
        episode_lengths.append(episode_steps)
        curiosity_levels.append(agent.getCuriosityLevel())
        novelty_levels.append(agent.getNoveltyLevel())
        
        print(f"  Episode result: reward={episode_reward:.2f}, steps={episode_steps}")
        
        # Allow for inter-episode development
        time.sleep(0.1)
    
    # Analyze training progress
    print(f"\n=== Training Analysis ===")
    print(f"Average episode reward: {np.mean(episode_rewards):.3f}")
    print(f"Average episode length: {np.mean(episode_lengths):.1f}")
    print(f"Average curiosity level: {np.mean(curiosity_levels):.3f}")
    print(f"Average novelty level: {np.mean(novelty_levels):.3f}")
    
    # Plot training progress
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Episode rewards
    axes[0, 0].plot(range(num_episodes), episode_rewards, 'bo-', linewidth=2, markersize=8)
    axes[0, 0].set_xlabel('Episode')
    axes[0, 0].set_ylabel('Episode Reward')
    axes[0, 0].set_title('Training Progress: Episode Rewards')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Episode lengths
    axes[0, 1].plot(range(num_episodes), episode_lengths, 'ro-', linewidth=2, markersize=8)
    axes[0, 1].set_xlabel('Episode')
    axes[0, 1].set_ylabel('Episode Length (steps)')
    axes[0, 1].set_title('Training Progress: Episode Lengths')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Curiosity levels
    axes[1, 0].plot(range(num_episodes), curiosity_levels, 'go-', linewidth=2, markersize=8)
    axes[1, 0].set_xlabel('Episode')
    axes[1, 0].set_ylabel('Curiosity Level')
    axes[1, 0].set_title('Learning Dynamics: Curiosity')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Novelty levels
    axes[1, 1].plot(range(num_episodes), novelty_levels, 'mo-', linewidth=2, markersize=8)
    axes[1, 1].set_xlabel('Episode')
    axes[1, 1].set_ylabel('Novelty Level')
    axes[1, 1].set_title('Learning Dynamics: Novelty')
    axes[1, 1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('training_progress.png', dpi=150)
    print("\nTraining progress saved to training_progress.png")
    
    return episode_rewards, episode_lengths, curiosity_levels, novelty_levels

if __name__ == "__main__":
    rewards, lengths, curiosity, novelty = training_example()
