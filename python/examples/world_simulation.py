#!/usr/bin/env python3
"""
NLM World Simulation Example

This example demonstrates world simulation with agent integration.
"""

import pynlm

def main():
    print("=== World Simulation Example ===")
    
    # Create world and brain
    print("1. Creating world and brain...")
    world = pynlm.createSimpleWorld()
    world.configure(20.0, 20.0, 10, 10)
    
    config = pynlm.create_agent_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Add environment objects
    print("2. Adding environment objects...")
    world.addResource(5.0, 5.0, 20.0)
    world.addHazard(15.0, 15.0, 10.0)
    world.addWall(10.0, 10.0)
    
    # Run simulation steps
    print("3. Running simulation...")
    for step in range(20):
        world.update(0.001)
        
        if step % 5 == 0:
            percept = world.getSensoryPercept()
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            print(f"   - Step {step}: Action={action.getType()}, "
                  f"Success={result.success}, Reward={result.reward:.1f}")
    
    print("=== Complete ===")

if __name__ == "__main__":
    main()