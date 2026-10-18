"""
World Interaction - Brain in Environment
=========================================

This example demonstrates how a brain interacts with a simulated world.
It shows the complete agent-world loop including sensory processing and motor output.

Key Concepts Demonstrated:
- Agent creation and world setup
- Sensory transduction (world -> agent -> brain)
- Motor decoding (brain -> agent -> world)
- Complete simulation loop

Expected Output:
---------------
Starting world interaction simulation...
World size: 20x20
Agent starting position: (10.00, 10.00)

Simulation steps:
Step 0: Vision: 8x8 (3 channels), Touch: 8 signals, Internal: 0 signals
Action: MoveForward
World position: (10.00, 11.00)

Simulation complete!
Final stats: 45 spikes, agent moved 15 units
"""

import pynlm

def main():
    print("Starting world interaction simulation...")
    
    # Create complete experiment setup using enhanced API
    brain, world, agent = pynlm.createReadyToRunSimpleExperiment()
    
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent starting position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Run simulation loop
    num_steps = 25
    print(f"\nRunning {num_steps} simulation steps:")
    
    for step in range(num_steps):
        # Update world dynamics
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain computation step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Display step information
        vision_width = percept.getVisionWidth()
        vision_height = percept.getVisionHeight()
        touch_signals = len(percept.getTouch())
        internal_signals = len(percept.getInternal())
        
        print(f"Step {step}: Vision: {vision_width}x{vision_height} "
              f"({percept.getVision()[0]:.2f} avg pixel value), "
              f"Touch: {touch_signals} signals, Internal: {internal_signals} signals")
        print(f"  Action: {action.getName()}, "
              f"World pos: ({world.getAgentBody().x:.2f}, {world.getAgentBody().y:.2f})")
    
    # Final statistics
    total_spikes = brain.getTotalSpikeCount()
    agent_x = world.getAgentBody().x
    agent_y = world.getAgentBody().y
    
    # Calculate distance traveled
    start_pos = (10.0, 10.0)
    distance = ((agent_x - start_pos[0])**2 + (agent_y - start_pos[1])**2)**0.5
    
    print(f"\nSimulation complete!")
    print(f"Final stats: {total_spikes} spikes, agent moved {distance:.1f} units")

if __name__ == "__main__":
    main()