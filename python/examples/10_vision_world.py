"""NLM Vision World - Brain interacting with a visual world"""
import pynlm

def vision_world_simulation():
    """Create a brain that sees and acts in a world."""
    # Setup components
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Simulation loop
    for step in range(200):
        world.update(0.1)
        
        # Get visual input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Make decision
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.simulation_time)
        
        # Print progress
        if step % 50 == 0:
            print(f"Step {step}: "
                  f"Firing: {brain.firing_neuron_count}, "
                  f"Curiosity: {agent.curiosity_level:.3f}")
    
    return brain, world, agent

brain, world, agent = vision_world_simulation()
print("Vision world simulation complete!")
