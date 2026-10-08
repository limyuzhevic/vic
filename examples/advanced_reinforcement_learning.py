import pynlm
import numpy as np
import time
from typing import Dict, List, Tuple

def advanced_reinforcement_learning_example():
    """
    Advanced reinforcement learning simulation demonstrating complex agent-environment
    interactions with integrated memory, prediction, and neuromodulation systems.
    """
    print("=== Advanced Reinforcement Learning Example ===\n")
    
    # Create sophisticated configuration for RL
    config = pynlm.createDefaultConfig()
    
    # Advanced RL configuration
    config.set("brain.region_count", 4)
    config.set("brain.neurons_per_region", 300)
    config.set("brain.initial_excitatory_ratio", 0.7)
    config.set("memory.working_memory_size", 150)
    config.set("memory.episodic_capacity", 2000)
    config.set("memory.associative_pattern_size", 75)
    config.set("neuromodulation.enable_dopamine", True)
    config.set("neuromodulation.dopamine_gain", 2.0)
    config.set("neuromodulation.enable_curiosity", True)
    config.set("neuromodulation.curiosity_threshold", 0.3)
    config.set("neuromodulation.enable_prediction_error", True)
    config.set("neuromodulation.prediction_error_gain", 1.5)
    config.set("development.enabled", True)
    config.set("development.maturation_rate", 0.001)
    config.set("plasticity.stdp_learning_rate", 0.02)
    config.set("plasticity.hebbian_learning_rate", 0.01)
    config.set("visualization.enabled", False)  # Turn off for performance
    
    # Initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup world with dynamic environment
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
    world.setAgentStart(20, 20)
    world.setMaxEnergy(200.0)
    world.setEnergyDecayRate(0.005)
    
    # Add environmental features
    _create_dynamic_environment(world)
    
    # Create agent brain with all systems
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # RL tracking
    rl_metrics = {
        "total_rewards": [],
        "episode_rewards": [],
        "action_distribution": {},
        "learning_progress": [],
        "memory_efficiency": [],
        "curiosity_driven_actions": 0,
        "exploration_rate": 0.0
    }
    
    # Run multiple episodes
    num_episodes = 10
    steps_per_episode = 500
    
    for episode in range(num_episodes):
        print(f"\nEpisode {episode + 1}/{num_episodes}")
        episode_reward = 0.0
        
        for step in range(steps_per_episode):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process through agent brain
            agent.processSensoryInput(percept)
            
            # Brain processes with prediction
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            
            # Apply action in world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate reward (complex, multi-factor)
            reward = _calculate_rl_reward(world, agent, percept, action, step)
            
            # Apply reward with prediction error
            predicted = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted)
            
            # Update development periodically
            if step % 50 == 0:
                agent.updateDevelopment(step * 0.1)
            
            # Track RL metrics
            episode_reward += reward
            rl_metrics["total_rewards"].append(reward)
            rl_metrics["action_distribution"][str(action)] = \
                rl_metrics["action_distribution"].get(str(action), 0) + 1
            
            if agent.getCuriosityLevel() > agent.getCuriosityThreshold():
                rl_metrics["curiosity_driven_actions"] += 1
            
            rl_metrics["memory_efficiency"].append(
                brain.getWorkingMemory().getActiveMemoryCount() if brain.getWorkingMemory() else 0
            )
            
            rl_metrics["learning_progress"].append(
                brain.getAverageFiringRate() * brain.getTotalSpikeCount() / max(step, 1)
            )
            
            # Adaptive exploration rate
            rl_metrics["exploration_rate"] = max(0.1, 1.0 - (episode * 0.05))
            
            # Print progress every 100 steps
            if step % 100 == 0:
                print(f"  Step {step}: Reward={reward:.3f}, "
                      f"Total Reward={episode_reward:.1f}, "
                      f"Firing Rate={brain.getAverageFiringRate():.2f}")
        
        # Episode summary
        rl_metrics["episode_rewards"].append(episode_reward)
        print(f"  Episode completed: Total Reward = {episode_reward:.1f}")
        print(f"  Agent Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
        print(f"  Agent Energy: {world.getAgentBody().energy:.1f}/{world.getMaxEnergy()}")
    
    # Analysis and reporting
    print("\n=== RL Analysis ===")
    
    avg_reward_per_episode = np.mean(rl_metrics["episode_rewards"])
    avg_reward_per_step = np.mean(rl_metrics["total_rewards"])
    
    print(f"Average reward per episode: {avg_reward_per_episode:.2f}")
    print(f"Average reward per step: {avg_reward_per_step:.4f}")
    print(f"Total curiosity-driven actions: {rl_metrics['curiosity_driven_actions']}")
    print(f"Final exploration rate: {rl_metrics['exploration_rate']:.2%}")
    
    # Action distribution analysis
    print("\nAction Distribution:")
    for action, count in rl_metrics["action_distribution"].items():
        percentage = (count / sum(rl_metrics["action_distribution"].values())) * 100
        action_name = _get_action_name(action)
        print(f"  {action_name}: {count} ({percentage:.1f}%)")
    
    # Performance metrics
    final_firing_rate = brain.getAverageFiringRate()
    final_developmental_stage = brain.getDevelopmentalStage()
    
    print(f"\nFinal Brain State:")
    print(f"  Firing Rate: {final_firing_rate:.2f} Hz")
    print(f"  Developmental Stage: {final_developmental_stage}")
    print(f"  Total Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Memory Usage: {brain.getWorkingMemory().getActiveMemoryCount() if brain.getWorkingMemory() else 0} active")
    
    return rl_metrics

def _create_dynamic_environment(world: pynlm.SimpleWorld):
    """Create a dynamic environment with various features."""
    # Add resources
    for i in range(10):
        obj = pynlm.WorldObject()
        obj.x = np.random.uniform(5, 35)
        obj.y = np.random.uniform(5, 35)
        obj.type = pynlm.WorldObjectType.Resource
        obj.value = 15.0 + np.random.uniform(-5, 5)
        world.addObject(obj)
    
    # Add hazards
    for i in range(5):
        obj = pynlm.WorldObject()
        obj.x = np.random.uniform(10, 30)
        obj.y = np.random.uniform(10, 30)
        obj.type = pynlm.WorldObjectType.Hazard
        obj.value = -10.0
        obj.radius = 0.8
        world.addObject(obj)
    
    # Add walls
    # Left wall
    for y in range(5, 35):
        obj = pynlm.WorldObject()
        obj.x = 3
        obj.y = y
        obj.type = pynlm.WorldObjectType.Wall
        obj.value = 0.0
        world.addObject(obj)
    
    # Right wall
    for y in range(5, 35):
        obj = pynlm.WorldObject()
        obj.x = 37
        obj.y = y
        obj.type = pynlm.WorldObjectType.Wall
        obj.value = 0.0
        world.addObject(obj)
    
    # Add markers (goals)
    for i in range(3):
        obj = pynlm.WorldObject()
        obj.x = np.random.uniform(15, 25)
        obj.y = np.random.uniform(15, 25)
        obj.type = pynlm.WorldObjectType.Marker
        obj.value = 50.0  # High reward marker
        world.addObject(obj)

def _calculate_rl_reward(world: pynlm.SimpleWorld, agent: pynlm.AgentBrain,
                        percept: pynlm.SensoryPercept, action: pynlm.MotorCommand,
                        step: int) -> float:
    """Calculate complex reward based on multiple factors."""
    reward = 0.0
    agent_body = world.getAgentBody()
    
    # Vision-based rewards
    vision = percept.getVision()
    resource_locations = []
    hazard_locations = []
    marker_locations = []
    
    for y in range(len(vision)):
        for x in range(len(vision[0])):
            obj_type = vision[y][x]
            rel_x = x - world.getVisionWidth() // 2
            rel_y = y - world.getVisionHeight() // 2
            world_x = agent_body.x + rel_x * 2.0
            world_y = agent_body.y + rel_y * 2.0
            
            if obj_type == pynlm.WorldObjectType.Resource:
                distance = ((world_x - agent_body.x)**2 + (world_y - agent_body.y)**2)**0.5
                resource_locations.append((distance, obj_type))
            elif obj_type == pynlm.WorldObjectType.Hazard:
                hazard_locations.append((0, obj_type))
            elif obj_type == pynlm.WorldObjectType.Marker:
                distance = ((world_x - agent_body.x)**2 + (world_y - agent_body.y)**2)**0.5
                marker_locations.append((distance, obj_type))
    
    # Resource rewards (with distance decay)
    for distance, _ in sorted(resource_locations, key=lambda x: x[0])[:3]:
        reward += 10.0 / (1.0 + distance / 2.0)
    
    # Hazard penalties
    for _, _ in hazard_locations:
        reward -= 5.0
    
    # Marker rewards (high value)
    for distance, _ in marker_locations:
        reward += 20.0 / (1.0 + distance / 1.5)
    
    # Action-based rewards
    if action in [pynlm.MotorCommand.MoveForward, pynlm.MotorCommand.MoveRight]:
        reward += 0.1  # Small step reward
    elif action in [pynlm.MotorCommand.TurnLeft, pynlm.MotorCommand.TurnRight]:
        reward += 0.05  # Turning reward
    elif action == pynlm.MotorCommand.Interact:
        reward += 1.0  # Interaction reward
    
    # Novelty bonus (curiosity-driven exploration)
    novelty_bonus = min(agent.getNoveltyLevel() * 2.0, 5.0)
    reward += novelty_bonus
    
    # Development bonus
    if agent.getDevelopmentalStage() > pynlm.DevelopmentalStage.Initial:
        reward += float(agent.getDevelopmentalStage()) * 0.1
    
    # Energy cost
    reward -= agent_body.energy * 0.01
    
    # Health penalty
    if agent_body.health < 50:
        reward -= 2.0
    
    # Prediction error bonus (learning)
    prediction_error = agent.getPredictionError()
    if prediction_error > 0.5:
        reward += prediction_error * 2.0
    
    # Time-based bonus (encourage efficiency)
    if step < 100:
        reward += 5.0
    
    return reward

def _get_action_name(action: int) -> str:
    """Get human-readable action name."""
    action_names = {
        0: "MoveForward",
        1: "MoveBackward", 
        2: "TurnLeft",
        3: "TurnRight",
        4: "Look",
        5: "LookUp",
        6: "LookDown",
        7: "Interact",
        8: "Eat",
        9: "Drink",
        10: "Rest",
        11: "Wait",
        12: "Custom"
    }
    return action_names.get(action, f"Unknown({action})")

if __name__ == "__main__":
    print("Starting Advanced Reinforcement Learning Example...\n")
    
    start_time = time.time()
    metrics = advanced_reinforcement_learning_example()
    end_time = time.time()
    
    print(f"\n=== Example Complete ===")
    print(f"Total execution time: {end_time - start_time:.2f} seconds")
    print(f"Total steps simulated: {len(metrics['total_rewards'])}")
    print(f"Average episode reward: {np.mean(metrics['episode_rewards']):.2f}")
    print(f"Example demonstrates advanced RL with integrated NLM systems.")
