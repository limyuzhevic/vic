#!/usr/bin/env python3
"""
NLM Example 2: Agent in World Simulation

This script demonstrates how to create an agent that interacts with a world,
processes sensory input, and produces motor actions.

Key concepts covered:
- Creating world environment
- Agent initialization and setup
- Sensory perception
- Motor command generation
- World interaction
"""
import pynlm
import sys

def main():
    """Create an agent that interacts with a world"""
    print("=== Agent in World Example ===")
    
    # 1. Setup brain
    print("1. Setting up brain...")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world environment
    print("2. Creating world environment...")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setAgentStart(10, 10)  # Start in middle
    world.reset()
    
    # 3. Create agent
    print("3. Creating agent...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable learning features
    print("4. Enabling learning features...")
    agent.enableAllFeatures()
    
    # 5. Create some objects in the world
    print("5. Creating world objects...")
    world.addObject(pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, 10))  # Food
    world.addObject(pynlm.WorldObject(15, 15, pynlm.WorldObjectType.Resource, 10))  # Food
    world.addObject(pynlm.WorldObject(10, 5, pynlm.WorldObjectType.Hazard, 5))  # Hazard
    
    # 6. Run simulation
    print("6. Running simulation...")
    num_steps = 200
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        if not percept:
            print(f"   Step {step}: No sensory input!")
            continue
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and apply it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print status every 50 steps
        if step % 50 == 0:
            agent_body = world.getAgentBody()
            print(f"   Step {step}: "
                  f"Position=({agent_body.x:.1f}, {agent_body.y:.1f}), "
                  f"Action={action}, "
                  f"Firing={brain.getFiringNeuronCount()}")
    
    # 7. Print final results
    print("7. Final results:")
    agent_body = world.getAgentBody()
    print(f"   - Agent final position: ({agent_body.x:.1f}, {agent_body.y:.1f})")
    print(f"   - Total steps: {num_steps}")
    print(f"   - Final brain stats: {brain.getStatistics()}")
    
    return True

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)