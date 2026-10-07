"""NLM Simple World - Agent navigation and exploration"""
import pynlm

def simple_world_exploration():
    """Agent exploring a simple 2D world."""
    # Create world with resources and obstacles
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=7, visionHeight=7)
    world.reset()
    
    # Add some resources and hazards
    # (In a real implementation, you would add specific world objects)
    
    # Create brain with exploration configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 800)
    config.set("connection_probability", 0.08)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable exploration systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Starting world exploration simulation...")
    print("Agent will learn to navigate and find resources.")
    
    # Run exploration
    for step in range(300):
        world.update(0.1)
        
        # Get visual input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes and decides
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        result = world.applyMotorCommand(action, world.simulation_time)
        
        # Learn from outcome
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Print exploration progress
        if step % 50 == 0:
            print(f"Step {step}: "
                  f"Position: ({world.agent_body.x:.1f}, {world.agent_body.y:.1f}), "
                  f"Energy: {world.agent_body.energy:.1f}, "
                  f"Curiosity: {agent.curiosity_level:.3f}")
            
            # Check for notable events
            if result.success:
                print(f"    Success: {result.message}")
    
    return brain, world, agent

brain, world, agent = simple_world_exploration()
print("\nExploration complete!")
print(f"Final agent stats:")
print(f"  - Final position: ({world.agent_body.x:.1f}, {world.agent_body.y:.1f})")
print(f"  - Final energy: {world.agent_body.energy:.1f}")
print(f"  - Final curiosity: {agent.curiosity_level:.3f}")
print(f"  - Total spikes: {brain.total_spike_count}")
print(f"  - Average firing rate: {brain.average_firing_rate:.2f} Hz")
