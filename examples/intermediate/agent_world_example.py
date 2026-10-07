#!/usr/bin/env python3
"""
NLM Agent-World Example
=======================

This example demonstrates a complete agent-world simulation using NLM.
It shows how to:
1. Create and configure a simulated world
2. Create and initialize an agent brain
3. Run a complete simulation loop
4. Monitor and visualize agent behavior
5. Save and load simulation state

This is a "Hello World" example for NLM's agent-based simulation capabilities.
"""

import pynlm as nlm

def create_agent_world_simulation(width=20, height=20, vision_width=8, vision_height=8):
    """
    Create a complete agent-world simulation environment.
    
    Args:
        width (int): World width in cells
        height (int): World height in cells  
        vision_width (int): Agent's vision width
        vision_height (int): Agent's vision height
        
    Returns:
        tuple: (brain, world, agent) components
    """
    print(f"Creating agent-world simulation: {width}x{height}, vision {vision_width}x{vision_height}")
    
    # 1. Create and configure world
    print("1. Creating and configuring world...")
    world = nlm.createSimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    world.setRandomSeed(42)  # For reproducible results
    world.reset()
    
    # Set agent starting position
    world.setAgentStart(width // 2, height // 2)
    
    # 2. Create brain with configuration
    print("2. Creating and configuring brain...")
    config = nlm.createDefaultConfig()
    
    # Customize configuration for better performance
    config.set("brain.neuron_count", 500)
    config.set("brain.region_count", 2)
    config.set("brain.connection_probability", 0.1)
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    print("3. Creating agent brain interface...")
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable all learning features
    print("4. Enabling learning features...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"   - Reward modulation: {'enabled' if agent.isRewardModulationEnabled() else 'disabled'}")
    print(f"   - Structural plasticity: {'enabled' if agent.isStructuralPlasticityEnabled() else 'disabled'}")
    print(f"   - Development: {'enabled' if agent.isDevelopmentEnabled() else 'disabled'}")
    print(f"   - Curiosity: {'enabled' if agent.isCuriosityEnabled() else 'disabled'}")
    
    return brain, world, agent

def run_simulation_loop(brain, world, agent, num_steps=100, log_interval=20):
    """
    Run the main simulation loop.
    
    Args:
        brain: The NLM brain
        world: The simulation world
        agent: The agent brain interface
        num_steps (int): Number of simulation steps
        log_interval (int): How often to log progress
        
    Returns:
        dict: Simulation statistics and metrics
    """
    print(f"\n5. Running simulation loop ({num_steps} steps)...")
    
    # Initialize metrics tracking
    metrics = {
        "total_reward": 0.0,
        "max_reward": 0.0,
        "min_reward": float('inf'),
        "avg_reward": 0.0,
        "steps_with_activity": 0,
        "max_firing_neurons": 0,
        "action_counts": {
            "move_forward": 0,
            "move_backward": 0,
            "turn_left": 0,
            "turn_right": 0,
            "interact": 0,
            "wait": 0
        },
        "curiosity_levels": [],
        "novelty_levels": [],
        "development_stages": []
    }
    
    for step in range(num_steps):
        # 1. Update world state
        world.update(0.1)  # 100ms timestep
        
        # 2. Get sensory input from world
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks and processes
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Apply action to world
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Apply reward modulation
        agent.applyRewardModulation(result.reward, 0.0)
        
        # 8. Update development
        agent.updateDevelopment(0.1)
        
        # Track metrics
        metrics["total_reward"] += result.reward
        metrics["max_reward"] = max(metrics["max_reward"], result.reward)
        metrics["min_reward"] = min(metrics["min_reward"], result.reward)
        
        if brain.getFiringNeuronCount() > 0:
            metrics["steps_with_activity"] += 1
        metrics["max_firing_neurons"] = max(metrics["max_firing_neurons"], brain.getFiringNeuronCount())
        
        # Count actions
        action_str = "wait"  # default
        if action == nlm.MotorCommand.MoveForward:
            action_str = "move_forward"
        elif action == nlm.MotorCommand.MoveBackward:
            action_str = "move_backward"
        elif action == nlm.MotorCommand.TurnLeft:
            action_str = "turn_left"
        elif action == nlm.MotorCommand.TurnRight:
            action_str = "turn_right"
        elif action == nlm.MotorCommand.Interact:
            action_str = "interact"
            
        metrics["action_counts"][action_str] += 1
        
        # Track neuromodulation levels
        metrics["curiosity_levels"].append(agent.getCuriosityLevel())
        metrics["novelty_levels"].append(agent.getNoveltyLevel())
        metrics["development_stages"].append(agent.getDevelopmentalStage())
        
        # Log progress periodically
        if step % log_interval == 0:
            print(f"   Step {step:4d}: reward={result.reward:6.2f}, "
                  f"action={action_str}, "
                  f"firing={brain.getFiringNeuronCount():3d}, "
                  f"curiosity={agent.getCuriosityLevel():.3f}, "
                  f"energy={world.getAgentBody().energy:5.1f}")
    
    # Calculate final metrics
    metrics["avg_reward"] = metrics["total_reward"] / num_steps
    metrics["novelty_levels"].sort(reverse=True)
    metrics["curiosity_levels"].sort(reverse=True)
    
    return metrics

def print_simulation_summary(brain, world, agent, metrics):
    """
    Print a comprehensive summary of the simulation results.
    
    Args:
        brain: The NLM brain
        world: The simulation world
        agent: The agent brain interface
        metrics (dict): Simulation metrics
    """
    print("\n6. Simulation Summary:")
    print("=" * 50)
    
    # Basic statistics
    print("Basic Statistics:")
    print(f"  Simulation steps: {sum(metrics['action_counts'].values())}")
    print(f"  World size: {world.getWidth()} x {world.getHeight()} cells")
    print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"  Agent orientation: {world.getAgentBody().orientation:.1f} degrees")
    print(f"  Agent energy: {world.getAgentBody().energy:.1f}")
    
    # Brain statistics
    print("\nBrain Statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Agent statistics
    print("\nAgent Statistics:")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Development stage: {agent.getDevelopmentalStage()}")
    
    # Action statistics
    print("\nAction Statistics:")
    total_actions = sum(metrics["action_counts"].values())
    for action, count in metrics["action_counts"].items():
        percentage = (count / total_actions) * 100 if total_actions > 0 else 0
        print(f"  {action.replace('_', ' ').title()}: {count:4d} ({percentage:5.1f}%)")
    
    # Reward statistics
    print("\nReward Statistics:")
    print(f"  Total reward: {metrics['total_reward']:8.2f}")
    print(f"  Average reward per step: {metrics['avg_reward']:8.3f}")
    print(f"  Maximum reward: {metrics['max_reward']:8.2f}")
    print(f"  Minimum reward: {metrics['min_reward']:8.2f}")
    print(f"  Steps with activity: {metrics['steps_with_activity']} ({metrics['steps_with_activity']/total_actions*100:.1f}%)")
    
    # Neuromodulation statistics
    print("\nNeuromodulation Statistics:")
    if metrics["curiosity_levels"]:
        print(f"  Max curiosity: {max(metrics['curiosity_levels']):.3f}")
        print(f"  Min curiosity: {min(metrics['curiosity_levels']):.3f}")
        print(f"  Average curiosity: {sum(metrics['curiosity_levels'])/len(metrics['curiosity_levels']):.3f}")
    if metrics["novelty_levels"]:
        print(f"  Max novelty: {max(metrics['novelty_levels']):.3f}")
        print(f"  Min novelty: {min(metrics['novelty_levels']):.3f}")
        print(f"  Average novelty: {sum(metrics['novelty_levels'])/len(metrics['novelty_levels']):.3f}")
    
    print("\n" + "=" * 50)
    print("✓ Agent-world simulation completed successfully!")

def save_simulation_checkpoint(brain, world, agent, filename="nlm_simulation_checkpoint.bin"):
    """
    Save the current simulation state to a checkpoint file.
    
    Args:
        brain: The NLM brain
        world: The simulation world
        agent: The agent brain interface
        filename (str): Checkpoint filename
        
    Returns:
        bool: True if save was successful, False otherwise
    """
    print(f"\n7. Saving simulation checkpoint to '{filename}'...")
    
    # Save brain state
    brain.save(filename)
    
    if os.path.exists(filename):
        print(f"   ✓ Simulation checkpoint saved successfully")
        print(f"   File size: {os.path.getsize(filename)} bytes")
        return True
    else:
        print(f"   ✗ Failed to save simulation checkpoint")
        return False

def main():
    """
    Main function to run the complete agent-world example.
    """
    print("=== NLM Agent-World Simulation Example ===\n")
    
    try:
        # Create the simulation environment
        brain, world, agent = create_agent_world_simulation()
        
        # Run the simulation loop
        metrics = run_simulation_loop(brain, world, agent, num_steps=100)
        
        # Print comprehensive summary
        print_simulation_summary(brain, world, agent, metrics)
        
        # Save checkpoint (optional)
        save_simulation_checkpoint(brain, world, agent, "nlm_example_checkpoint.bin")
        
        print("\n🎉 Agent-world simulation example completed successfully!")
        print("\nThis example demonstrated:")
        print("  ✓ Creating and configuring a simulated world")
        print("  ✓ Setting up NLM brain with custom configuration")
        print("  ✓ Creating agent brain interface for world interaction")
        print("  ✓ Enabling and monitoring learning features")
        print("  ✓ Running complete simulation loop")
        print("  ✓ Tracking and analyzing simulation metrics")
        print("  ✓ Saving simulation state to checkpoint")
        
        return True
        
    except Exception as e:
        print(f"\n❌ Error during simulation: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    import os
    success = main()
    exit(0 if success else 1)