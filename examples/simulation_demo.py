#!/usr/bin/env python3
"""Demonstrate complete simulation with world interaction.

This example shows a full agent simulation loop:
brain → actions → world changes → sensory input → reward → plasticity
"""

import pynlm
import sys
import math
import random

def create_world_objects(world):
    """Create some objects in the world for interaction."""
    # Add a resource at position (5, 5)
    resource = pynlm.WorldObject(
        x=5.0, y=5.0,
        type=pynlm.WorldObjectType.Resource,
        value=10.0
    )
    world.addObject(resource)
    
    # Add a wall at position (15, 15)
    wall = pynlm.WorldObject(
        x=15.0, y=15.0,
        type=pynlm.WorldObjectType.Wall,
        radius=1.0
    )
    world.addObject(wall)
    
    # Add a hazard at position (2, 18)
    hazard = pynlm.WorldObject(
        x=2.0, y=18.0,
        type=pynlm.WorldObjectType.Hazard,
        value=-5.0
    )
    world.addObject(hazard)
    
    return [resource, wall, hazard]

def calculate_reward(world, agent, brain, action):
    """Calculate reward based on action and world state."""
    body = world.getAgentBody()
    reward = 0.0
    
    # Reward for moving to resource
    percept = world.getSensoryPercept()
    vision = percept.getVision()
    
    # Check if we're close to resource
    resource_pos = (5.0, 5.0)
    distance = math.sqrt((body.x - resource_pos[0])**2 + 
                        (body.y - resource_pos[1])**2)
    if distance < 2.0:
        reward += 10.0
    
    # Penalty for moving toward hazard
    hazard_pos = (2.0, 18.0)
    hazard_distance = math.sqrt((body.x - hazard_pos[0])**2 + 
                               (body.y - hazard_pos[1])**2)
    if hazard_distance < 3.0:
        reward -= 8.0
    
    # Reward for exploration (curiosity)
    reward += agent.getCuriosityLevel() * 0.5
    
    # Small negative reward for large movements (efficiency)
    if action:
        # Check action parameters (for movement actions)
        if hasattr(action, 'getParameters'):
            params = action.getParameters()
            if len(params) >= 2:
                movement_cost = math.sqrt(params[0]**2 + params[1]**2)
                reward -= movement_cost * 0.1
    
    return reward

def main():
    print("=== NLM Simulation Demo ===\n")
    
    # 1. Setup complete agent system
    print("Step 1: Setting up complete agent system")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(1.0, 1.0)  # Start in corner
    
    # Create world objects
    objects = create_world_objects(world)
    print(f"  Created {len(objects)} world objects")
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("✓ Complete agent system ready")
    print()
    
    # 2. Run simulation episode
    print("Step 2: Running simulation episode")
    
    num_steps = 100
    total_reward = 0.0
    food_collected = 0
    hazards_encountered = 0
    
    for step in range(num_steps):
        # 2.1 Update world
        world.update(0.1)
        
        # 2.2 Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # 2.3 Brain processes and makes decision
        brain.step(step)
        
        # 2.4 Decode action
        action = agent.decodeMotorCommand()
        
        # 2.5 Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 2.6 Calculate and apply reward
        reward = calculate_reward(world, agent, brain, action)
        
        # Check for events
        body = world.getAgentBody()
        
        # Check if collected resource
        resource_pos = (5.0, 5.0)
        if abs(body.x - resource_pos[0]) < 1.0 and abs(body.y - resource_pos[1]) < 1.0:
            food_collected += 1
        
        # Check hazard encounter
        hazard_pos = (2.0, 18.0)
        if abs(body.x - hazard_pos[0]) < 2.0 and abs(body.y - hazard_pos[1]) < 2.0:
            hazards_encountered += 1
        
        total_reward += reward
        agent.applyRewardModulation(reward, 0.0)  # Zero predicted reward
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print status every 20 steps
        if step % 20 == 0:
            print(f"  Step {step}: Pos({body.x:.1f},{body.y:.1f}), "
                  f"Reward:{reward:+.2f}, "
                  f"Curiosity:{agent.getCuriosityLevel():.3f}, "
                  f"DevStage:{brain.getDevelopmentalStage()}")
    
    print(f"✓ Episode complete after {num_steps} steps")
    print(f"  Total reward: {total_reward:.2f}")
    print(f"  Resources collected: {food_collected}")
    print(f"  Hazards encountered: {hazards_encountered}")
    print()
    
    # 3. Show final brain statistics
    print("Step 3: Final brain analysis")
    
    body = world.getAgentBody()
    print(f"  Final agent position: ({body.x:.1f}, {body.y:.1f})")
    print(f"  Agent age: {body.age:.1f}")
    print(f"  Energy level: {body.energy:.1f}/1000")
    print(f"  Health: {body.health:.1f}/100")
    
    print(f"  Brain statistics:")
    print(f"    Total neurons: {brain.getTotalNeuronCount()}")
    print(f"    Currently firing: {brain.getFiringNeuronCount()}")
    print(f"    Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"    Total spikes: {brain.getTotalSpikeCount()}")
    print(f"    Development stage: {brain.getDevelopmentalStage()}")
    print(f"    E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    print(f"  Neuromodulation levels:")
    print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"    Prediction error: {agent.getPredictionError():.3f}")
    
    print()
    print("=== Simulation Demo Complete ===")

if __name__ == "__main__":
    try:
        main()
    except ImportError as e:
        print(f"Error importing pynlm: {e}")
        print("Make sure NLM Python bindings are installed.")
        sys.exit(1)
    except Exception as e:
        print(f"Error running simulation demo: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
