"""
Example demonstrating basic agent-world interaction
and embodied cognition.
"""
import pynlm
import time
import random

def simple_agent_example():
    """Run a simple agent simulation with world interaction."""
    print("=== Simple Agent Example ===")
    
    # 1. Create brain and agent
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # 2. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=6,
        visionHeight=6
    )
    
    # 3. Initialize world and agent
    world.reset()
    world.setAgentStart(10.0, 10.0)
    agent.initialize(world)
    
    # 4. Enable key agent subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Vision dimensions: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")
    
    # 5. Run simulation loop
    num_steps = 200
    reward_history = []
    
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory percept from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Calculate and apply reward
        internal_signals = percept.getInternal()
        reward = internal_signals[0] if internal_signals else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record reward
        reward_history.append(reward)
        
        # Print progress
        if step % 20 == 0:
            print(f"\nStep {step}:")
            print(f"  Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Action: {motor_cmd}")
            print(f"  Reward: {reward:.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    # Calculate statistics
    avg_reward = sum(reward_history) / len(reward_history)
    max_reward = max(reward_history)
    min_reward = min(reward_history)
    
    print(f"\n=== Simulation Results ===")
    print(f"Average reward: {avg_reward:.3f}")
    print(f"Max reward: {max_reward:.3f}")
    print(f"Min reward: {min_reward:.3f}")
    print(f"Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    # Plot reward over time
    import matplotlib.pyplot as plt
    plt.figure(figsize=(10, 4))
    plt.plot(reward_history, 'b-', linewidth=1)
    plt.xlabel('Step')
    plt.ylabel('Reward')
    plt.title('Reward Over Time')
    plt.grid(True, alpha=0.3)
    plt.savefig('agent_reward_history.png', dpi=150)
    print("\nReward history saved to agent_reward_history.png")
    
    return world, agent, reward_history

if __name__ == "__main__":
    world, agent, rewards = simple_agent_example()
