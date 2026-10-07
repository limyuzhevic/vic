"""NLM Development and Growth - Agent development over time"""
import pynlm

def developmental_simulation():
    """Agent that develops and matures over time."""
    # Create developmental configuration
    config = pynlm.createResearchConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all developmental systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Starting developmental simulation...")
    print("Agent will develop through multiple stages.")
    
    # Run long-term development
    for step in range(800):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks and develops
        brain.step(step)
        
        # Agent acts
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.simulation_time)
        
        # Apply reward and develop
        reward = world.computeReward(brain.getRegions()[0].get())
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Track development stages
        if step % 100 == 0:
            stage = agent.developmental_stage
            print(f"Step {step}: "
                  f"Stage: {stage.name}, "
                  f"Firing: {brain.firing_neuron_count}, "
                  f"Curiosity: {agent.curiosity_level:.3f}")
            
            # Print stage descriptions
            if stage == pynlm.DevelopmentalStage.CriticalPeriod:
                print("    -> Critical period: High learning rate!")
            elif stage == pynlm.DevelopmentalStage.Maturation:
                print("    -> Maturation: Structural plasticity active!")
            elif stage == pynlm.DevelopmentalStage.Adult:
                print("    -> Adult: Optimized performance!")
    
    return brain, world, agent

brain, world, agent = developmental_simulation()
print("\nDevelopmental simulation complete!")
print(f"Developmental journey summary:")
print(f"  - Total simulation time: {800 * 0.1:.1f} seconds")
print(f"  - Final developmental stage: {agent.developmental_stage.name}")
print(f"  - Final curiosity level: {agent.curiosity_level:.3f}")
print(f"  - Total spikes generated: {brain.total_spike_count}")
print(f"  - Average firing rate: {brain.average_firing_rate:.2f} Hz")
