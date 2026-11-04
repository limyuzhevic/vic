"""
Agent-based NLM simulation example.

This example demonstrates agent-environment interaction:
- Complete agent setup with brain, agent, and world
- Sensory processing pipeline
- Action execution and reward collection
- Episode management
"""

import pynlm
from pynlm import AgentSimulator, NLMVisualizer

def agent_simulation():
    """Run a complete agent simulation."""
    print("=== Agent-Based NLM Simulation ===")
    
    # Create agent simulator with enhanced configuration
    simulator = (AgentSimulator()
                .initialize())
    
    print(f"Agent simulation initialized:")
    print(f"  World: {simulator.world_width}x{simulator.world_height}")
    print(f"  Vision: {simulator.vision_width}x{simulator.vision_height}")
    
    # Run episode
    def progress_callback(step, metrics):
        if step % 50 == 0:
            print(f"Step {step}: {metrics['brain_stats']['firing_neurons']} "
                  f"firing, {metrics['action_type']} action")
    
    episode_data = simulator.run_episode(200, progress_callback)
    
    # Get episode statistics
    stats = simulator.get_episode_stats()
    print(f"\nEpisode statistics:")
    print(f"  Total steps: {stats['total_steps']}")
    print(f"  Unique actions: {stats['unique_actions']}")
    print(f"  Average firing rate: {stats['average_firing_rate']:.2f} Hz")
    print(f"  Total spikes: {stats['total_spikes']}")
    print(f"  Final development stage: {stats['final_dev_stage']}")
    
    # Visualize results
    fig, plt = NLMVisualizer.plot_action_distribution(episode_data)
    if fig:
        plt.savefig('agent_actions.png', dpi=150, bbox_inches='tight')
        print("Action distribution saved to 'agent_actions.png'")
        plt.close()
    
    return simulator

def advanced_agent_simulation():
    """Advanced agent simulation with learning."""
    print("\n=== Advanced Agent Simulation ===")
    
    # Create simulator with advanced configuration
    simulator = AgentSimulator(
        world_width=30,
        world_height=30,
        vision_width=12,
        vision_height=12
    )
    
    # Enable all learning subsystems
    from pynlm import NLMConfigBuilder
    config = (NLMConfigBuilder()
             .apply_preset('advanced')
             .get_config())
    
    # Replace simulator's config
    simulator.config = config
    
    # Re-initialize with new config
    simulator.initialize()
    
    print("Advanced agent simulation initialized with:")
    print("  - Reward modulation enabled")
    print("  - Structural plasticity enabled")
    print("  - Development enabled")
    print("  - Curiosity enabled")
    
    # Run extended episode
    episode_data = simulator.run_episode(500)
    
    # Analyze learning patterns
    stats = simulator.get_episode_stats()
    print(f"\nAdvanced learning statistics:")
    print(f"  Steps completed: {stats['total_steps']}")
    print(f"  Learning efficiency: {stats['average_firing_rate']/stats['total_steps']:.4f}")
    print(f"  Final development stage: {stats['final_dev_stage']}")
    
    return simulator

if __name__ == "__main__":
    # Run examples
    agent_sim = agent_simulation()
    advanced_sim = advanced_agent_simulation()
    
    print("\n=== Agent examples completed successfully! ===")
