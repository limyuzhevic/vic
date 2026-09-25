import enhanced_pynlm as nlm

def main():
    print("=== NLM Enhanced API Example: Agent-World Exploration ===")
    
    # Setup environment
    print("Setting up environment...")
    brain = nlm.Brain()
    world = nlm.World()
    world.setup(width=25, height=25, visionWidth=8, visionHeight=8)
    world.setAgentPosition(12.5, 12.5)  # Center of world
    
    # Create and initialize agent
    print("Creating agent...")
    agent = nlm.Agent(brain)
    agent.initialize()
    agent.enableAll()  # Enable all learning features
    
    # Run exploration episode
    print("Starting exploration episode (500 steps)...")
    results = agent.runEpisode(maxSteps=500)
    
    # Display results
    print(f"\nExploration Results:")
    print(f"  Average reward per step: {results['total_reward']:.4f}")
    print(f"  Final position: ({results['final_x']:.1f}, {results['final_y']:.1f})")
    print(f"  Success rate: {100 * (abs(results['final_x'] - 12.5) < 5):.1f}%")
    
    # Run additional episodes for better statistics
    print("\nRunning additional exploration episodes...")
    total_rewards = []
    final_positions = []
    
    for episode in range(3):
        print(f"  Episode {episode + 1}/3...")
        agent = nlm.Agent(brain)  # New agent for each episode
        agent.initialize()
        agent.enableAll()
        
        episode_results = agent.runEpisode(maxSteps=200)
        total_rewards.append(episode_results['total_reward'])
        final_positions.append((episode_results['final_x'], episode_results['final_y']))
    
    # Calculate average statistics
    avg_reward = sum(total_rewards) / len(total_rewards)
    avg_final_x = sum(p[0] for p in final_positions) / len(final_positions)
    avg_final_y = sum(p[1] for p in final_positions) / len(final_positions)
    
    print(f"\nAggregate Results (3 episodes):")
    print(f"  Average reward per step: {avg_reward:.4f}")
    print(f"  Average final position: ({avg_final_x:.1f}, {avg_final_y:.1f})")
    print(f"  Overall success rate: {100 * (abs(avg_final_x - 12.5) < 5):.1f}%")
    
    # Save experiment results
    brain.save("agent_exploration_example")
    
    print("\n=== Example Complete ===")

if __name__ == "__main__":
    main()
