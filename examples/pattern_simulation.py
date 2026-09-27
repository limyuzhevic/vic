#!/usr/bin/env python3
"""Pattern 1: Run a Simulation

This demonstrates the common pattern of running a complete simulation with
brain, world, and agent components working together.
"""

import pynlm

def run_simulation(brain, world, agent, num_steps):
    """Run a complete agent simulation with brain and world."""
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain what it sees
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks/recomputes
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

def example_run_simulation():
    """Example of using the run_simulation pattern."""
    print("=== Running Simulation Pattern ===")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Use the pattern
    run_simulation(brain, world, agent, 1000)
    
    print("Simulation complete!")

if __name__ == "__main__":
    example_run_simulation()