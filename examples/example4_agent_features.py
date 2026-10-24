#!/usr/bin/env python3
"""
Example 4: Agent Behavior with Enhanced Features
Demonstrates AgentBrain convenience methods and complete agent simulation
"""
import pynlm
import time

def agent_enhanced_features():
    """Demonstrate enhanced AgentBrain features and complete agent simulation"""
    print("=== Example 4: Agent Behavior with Enhanced Features ===\n")
    
    # 1. Setup complete agent system
    print("1. Setting up complete agent system:")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Configure for agent simulation
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.1)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create brain, agent, and world
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent.initialize(world)
    
    print(f"   Created brain: {brain.getTotalNeuronCount()} neurons")
    print(f"   Agent sensory input size: {agent.getSensoryInputSize()}")
    print(f"   Agent motor output size: {agent.getMotorOutputSize()}")
    print(f"   World size: {world.getWidth()}x{world.getHeight()}")
    
    print()
    
    # 2. Enable all agent systems
    print("2. Enabling all agent systems:")
    
    # Using the new enableAllSystems() convenience method
    agent.enableAllSystems()
    
    system_status = agent.getSystemStatus()
    print("   Enabled systems:")
    for system, enabled in system_status.items():
        status = "✓" if enabled else "✗"
        print(f"     {system}: {status}")
    
    print()
    
    # 3. Run complete agent simulation
    print("3. Running complete agent simulation:")
    
    simulation_results = []
    
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get agent statistics
        agent_stats = agent.getAgentStats()
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Get reward and apply neuromodulation
        # Use internal signals as reward (first signal is typically energy/reward)
        internal_signals = percept.getInternal()
        reward = internal_signals[0] if internal_signals else 0.0
        
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect step data
        step_data = {
            "step": step,
            "time": world.getSimulationTime(),
            "action": str(motor_cmd),
            "reward": reward,
            "agent_stats": agent_stats,
            "brain_stats": brain.getBrainStats(),
            "world_stats": world.getWorldInfo(),
            "position": world.getAgentBody().x, world.getAgentBody().y,
            "energy": world.getAgentBody().energy,
            "health": world.getAgentBody().health
        }
        
        simulation_results.append(step_data)
        
        # Print progress periodically
        if step % 10 == 0:
            print(f"   Step {step}: Action={step_data['action']}, "
                  f"Reward={reward:.3f}, Curiosity={agent_stats['curiosity']:.3f}, "
                  f"Position=({step_data['position'][0]:.1f},{step_data['position'][1]:.1f})")
    
    print()
    
    # 4. Analyze agent behavior
    print("4. Analyzing agent behavior:")
    
    # Get action recommendations
    context = {"step": 49, "time": world.getSimulationTime()}
    recommendation = agent.getActionRecommendation(context)
    print(f"   Final action recommendation: {recommendation}")
    
    # Analyze agent behavior patterns
    actions = [r["action"] for r in simulation_results]
    rewards = [r["reward"] for r in simulation_results]
    curiosity_levels = [r["agent_stats"]["curiosity"] for r in simulation_results]
    
    unique_actions = list(set(actions))
    total_reward = sum(rewards)
    avg_curiosity = sum(curiosity_levels) / len(curiosity_levels)
    max_curiosity = max(curiosity_levels)
    
    print(f"   Unique actions taken: {unique_actions}")
    print(f"   Total reward accumulated: {total_reward:.3f}")
    print(f"   Average curiosity level: {avg_curiosity:.3f}")
    print(f"   Maximum curiosity level: {max_curiosity:.3f}")
    print(f"   Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"   Final energy: {world.getAgentBody().energy:.1f}")
    print(f"   Final health: {world.getAgentBody().health:.1f}")
    
    print()
    
    # 5. Demonstrate AgentBrain convenience methods
    print("5. Using AgentBrain convenience methods:")
    
    # Get comprehensive agent stats using getAgentStats()
    final_agent_stats = agent.getAgentStats()
    print("   Comprehensive agent statistics:")
    for key, value in final_agent_stats.items():
        if isinstance(value, dict):
            print(f"     {key}:")
            for subkey, subvalue in value.items():
                print(f"       {subkey}: {subvalue}")
        elif isinstance(value, float):
            print(f"     {key}: {value:.4f}")
        else:
            print(f"     {key}: {value}")
    
    print()
    
    # 6. Save simulation results
    print("6. Saving simulation results:")
    
    try:
        # Create a simple text summary
        with open("agent_simulation_summary.txt", "w") as f:
            f.write("Agent Simulation Summary\n")
            f.write("=" * 50 + "\n")
            f.write(f"Total steps: {len(simulation_results)}\n")
            f.write(f"Unique actions: {unique_actions}\n")
            f.write(f"Total reward: {total_reward:.3f}\n")
            f.write(f"Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})\n")
            f.write(f"Final energy: {world.getAgentBody().energy:.1f}\n")
            f.write(f"Final health: {world.getAgentBody().health:.1f}\n")
            f.write(f"Final developmental stage: {brain.getDevelopmentalStage()}\n")
            f.write("\n")
        
        print("   ✓ Simulation summary saved to agent_simulation_summary.txt")
    except Exception as e:
        print(f"   ✗ Error saving simulation: {e}")
    
    print("\n=== Agent behavior example complete ===\n")

if __name__ == "__main__":
    agent_enhanced_features()