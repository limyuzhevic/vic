#!/usr/bin/env python3
"""
Sensory-Motor Integration Example

This example demonstrates how to create a complete agent that interacts with its
environment through sensory input and produces motor actions.
"""

import pynlm
import numpy as np
import time

def create_test_environment():
    """Create a test world with various objects."""
    world = pynlm.createSimpleWorld()
    world.configure(100, 100, 20, 20)  # width, height, vision width, vision height
    
    # Set agent starting position
    world.setAgentStart(50, 50)
    
    # Add some resources and hazards
    # Resource (valuable object)
    resource = pynlm.WorldObject()
    resource.x, resource.y = 20, 20
    resource.type = pynlm.WorldObjectType.Resource
    resource.value = 10.0
    resource.radius = 0.5
    world.addObject(resource)
    
    # Hazard (dangerous object)
    hazard = pynlm.WorldObject()
    hazard.x, hazard.y = 80, 80
    hazard.type = pynlm.WorldObjectType.Hazard
    hazard.value = -5.0
    hazard.radius = 0.5
    world.addObject(hazard)
    
    # Wall
    wall = pynlm.WorldObject()
    wall.x, wall.y = 60, 40
    wall.type = pynlm.WorldObjectType.Wall
    wall.radius = 0.6
    world.addObject(wall)
    
    return world

def process_percept(percept):
    """Extract and process sensory information."""
    processed = {
        'vision': percept.getVision(),
        'touch': percept.getTouch(),
        'internal': percept.getInternal(),
        'proprioception': percept.getProprioception(),
        'audio': percept.getAudio()
    }
    return processed

def create_agent(config_dict=None):
    """Create an agent brain with custom configuration."""
    config = pynlm.Config()
    
    # Default configuration
    default_config = {
        "neuron_count": 400,
        "region_count": 2,
        "simulation_timestep": 0.001,
        "random_seed": 123,
        "connection_probability": 0.1,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    }
    
    # Apply user configuration
    if config_dict:
        for key, value in config_dict.items():
            config.set(key, value)
    
    # Apply defaults for missing keys
    for key, value in default_config.items():
        if not config.has(key):
            config.set(key, value)
    
    brain = pynlm.createBrain(config)
    agent_brain = pynlm.createAgentBrain(brain)
    
    return brain, agent_brain

def run_sensory_motor_loop(world, brain, agent_brain, max_steps=500):
    """Run the main sensory-motor loop."""
    print("Starting sensory-motor loop...")
    
    # Initialize episode tracking
    agent_brain.startEpisode()
    episode_reward = 0.0
    
    # Main loop
    for step in range(max_steps):
        # Get current state
        body = world.getAgentBody()
        percept = world.getSensoryPercept()
        
        # Process sensory input
        processed = process_percept(percept)
        
        # Log every 50 steps
        if step % 50 == 0:
            vision_data = processed['vision']
            # Calculate visual summary
            if vision_data:
                avg_brightness = sum(vision_data) / len(vision_data)
                print(f"\nStep {step}: "
                      f"Position=({body.x:.1f},{body.y:.1f}), "
                      f"Energy={body.energy:.1f}, "
                      f"Vision avg={avg_brightness:.3f}")
        
        # Agent brain processes sensory input
        agent_brain.processSensoryInput(percept)
        
        # Get motor command
        motor_cmd = agent_brain.decodeMotorCommand()
        
        # Apply motor command to world
        current_time = step * 0.001
        world.applyMotorCommand(motor_cmd, current_time)
        
        # Get updated state
        body = world.getAgentBody()
        
        # Calculate reward based on actions and environment
        reward = calculate_reward(body, processed, world)
        prediction = agent_brain.getPredictionError()
        
        # Update episode tracking
        episode_reward += reward
        agent_brain.updateEpisodeReward(reward)
        
        # Apply neuromodulation
        agent_brain.applyRewardModulation(reward, prediction)
        
        # Update development
        agent_brain.updateDevelopment(0.001)
        
        # Log performance
        if step % 100 == 0:
            stats = agent_brain.getPerformanceStats()
            print(f"  Progress: {step}/{max_steps} steps, "
                  f"Reward avg={stats.episodeReward/max(1, step):.3f}")
    
    print(f"Episode completed: {step + 1} steps, total reward: {episode_reward:.3f}")
    return episode_reward

def calculate_reward(body, processed, world):
    """Calculate reward based on agent's state and actions."""
    reward = 0.0
    
    # Energy reward - maintain energy
    if body.energy > 50:
        reward += 0.1
    elif body.energy < 20:
        reward -= 0.5
    
    # Vision processing - detect objects in vision
    vision = processed['vision']
    if vision:
        # Find object locations from vision data
        # In a real implementation, we'd decode vision into object positions
        # For this example, we'll use simplified logic
        
        # Check for nearby resources
        for i, intensity in enumerate(vision[:10]):  # Check first 10 vision cells
            if intensity > 0.3:  # High intensity likely indicates an object
                # Calculate approximate object position
                obj_x = body.x - 10 + (i * 2)
                obj_y = body.y - 10
                
                # Find objects in world
                for obj in get_objects_in_world(world):
                    if (obj.x - obj.radius < obj_x < obj.x + obj.radius and
                        obj.y - obj.radius < obj_y < obj.y + obj.radius):
                        if obj.type == pynlm.WorldObjectType.Resource:
                            reward += obj.value * 0.1
                            print(f"    Found resource worth {obj.value}!")
                        elif obj.type == pynlm.WorldObjectType.Hazard:
                            reward += obj.value * 0.1
                            print(f"    Encountered hazard! Reward: {obj.value}")
                        break
    
    # Movement reward - encourage exploration
    if body.isMoving:
        reward += 0.05
    
    # Learning novelty reward
    novelty = agent_brain.getNoveltyLevel()
    reward += novelty * 0.2
    
    return reward

def get_objects_in_world(world):
    """Extract objects from world for reward calculation."""
    # This is a simplified implementation
    # In a real system, we'd have better object tracking
    objects = []
    # We would need to access world's internal object list
    # For now, return empty list
    return objects

def demonstrate_learning_curves():
    """Show how the agent learns over multiple episodes."""
    print("\n=== Learning Curves Demonstration ===")
    
    # Create environment
    world = create_test_environment()
    
    # Run multiple episodes
    episodes = 10
    rewards = []
    novelties = []
    
    for episode in range(episodes):
        print(f"\nEpisode {episode + 1}/{episodes}:")
        
        # Create new agent for each episode (simulating development)
        brain, agent_brain = create_agent({
            "random_seed": 100 + episode  # Different seed for variety
        })
        
        world.reset()
        reward = run_sensory_motor_loop(world, brain, agent_brain, max_steps=200)
        
        # Record statistics
        stats = agent_brain.getPerformanceStats()
        rewards.append(reward)
        novelties.append(stats.episodeNovelty)
        
        print(f"  Episode reward: {reward:.3f}")
        print(f"  Episode novelty: {stats.episodeNovelty:.3f}")
        print(f"  Avg firing rate: {stats.avgFiringRate:.2f} Hz")
        
        # Clean up
        del brain, agent_brain
    
    # Analyze learning
    print(f"\nLearning Analysis:")
    print(f"  Rewards: {rewards}")
    print(f"  Mean reward: {np.mean(rewards):.3f} ± {np.std(rewards):.3f}")
    print(f"  Novelties: {novelties}")
    print(f"  Mean novelty: {np.mean(novelties):.3f}")
    
    # Check for learning patterns
    if np.mean(rewards[-3:]) > np.mean(rewards[:3]):
        print("  ✓ Learning detected: Rewards improved over time")
    else:
        print("  ! No clear learning pattern detected")
    
    return rewards, novelties

def demonstrate_batch_processing():
    """Demonstrate batch processing capabilities."""
    print("\n=== Batch Processing Demo ===")
    
    # Create brain
    config = pynlm.Config()
    config.set("neuron_count", 300)
    
    brain = pynlm.createBrain(config)
    agent_brain = pynlm.createAgentBrain(brain)
    
    # Create multiple worlds for batch processing
    worlds = []
    for i in range(5):
        world = pynlm.createSimpleWorld()
        world.configure(50, 50, 10, 10)
        world.setAgentStart(i * 10, i * 10)
        worlds.append(world)
    
    # Batch process sensory inputs
    print("Creating batch of sensory percepts...")
    all_percepts = []
    
    for world in worlds:
        world.reset()
        percept = world.getSensoryPercept()
        all_percepts.append(percept)
    
    print(f"Created {len(all_percepts)} sensory percepts")
    
    # Process all percepts at once
    print("Processing batch with agent brain...")
    agent_brain.batchProcessSensoryInput(all_percepts)
    
    # Get batch results
    stats = agent_brain.getPerformanceStats()
    print(f"Batch processing complete. Stats: {stats.totalSteps} steps processed")
    
    return all_percepts

def main():
    """Run all sensory-motor demonstrations."""
    print("NLM Sensory-Motor Integration Examples")
    print("=" * 50)
    
    try:
        # Basic sensory-motor loop
        print("\n1. Basic Sensory-Motor Loop")
        world = create_test_environment()
        brain, agent_brain = create_agent()
        agent_brain.initialize(world)
        brain.initialize()
        
        reward = run_sensory_motor_loop(world, brain, agent_brain, max_steps=300)
        print(f"Basic loop reward: {reward:.3f}")
        
        # Learning curves
        print("\n2. Learning Curves")
        rewards, novelties = demonstrate_learning_curves()
        
        # Batch processing
        print("\n3. Batch Processing")
        percepts = demonstrate_batch_processing()
        
        # Summary
        print("\n" + "=" * 50)
        print("All demonstrations completed successfully!")
        print("\nKey insights:")
        print("1. Agents can process sensory input and produce appropriate actions")
        print("2. Learning occurs through reward-based neuromodulation")
        print("3. Novelty drives exploration and discovery")
        print("4. Batch processing enables efficient parallel computation")
        
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
