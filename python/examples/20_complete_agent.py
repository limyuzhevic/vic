"""Complete NLM Agent - Full learning agent with curiosity and development"""
import pynlm

def complete_learning_agent():
    """Create a complete learning agent with all systems enabled."""
    # Setup with research configuration for full learning
    config = pynlm.createResearchConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run extended simulation
    print("Running complete learning agent simulation...")
    for step in range(500):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Make decision
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.simulation_time)
        
        # Get reward and learn
        reward = world.computeReward(brain.getRegions()[0].get())
        agent.applyRewardModulation(reward, 0.0)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing: {brain.firing_neuron_count}, "
                  f"Curiosity: {agent.curiosity_level:.3f}, "
                  f"Reward: {reward:.3f}, "
                  f"Development: {agent.developmental_stage}")
    
    return brain, world, agent

brain, world, agent = complete_learning_agent()
print("Complete learning agent simulation finished!")
print(f"Final metrics:")
print(f"  - Total spikes: {brain.total_spike_count}")
print(f"  - Average firing rate: {brain.average_firing_rate:.2f} Hz")
print(f"  - Final curiosity: {agent.curiosity_level:.3f}")
print(f"  - Final neuromodulation: {agent.neuromodulation_level:.3f}")
