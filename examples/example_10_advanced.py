import pynlm
import numpy as np

def advanced_agent_learning():
    """Advanced agent with complex learning strategies."""
    
    print("Testing advanced agent learning...")
    
    # Create sophisticated configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("brain.synapse_density", 0.12)
    config.set("brain.v_thresh", -55.0)
    config.set("brain.v_rest", -70.0)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.threshold", 0.5)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(25.0, 25.0)
    world.setMaxEnergy(100.0)
    world.setEnergyDecayRate(0.01)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Advanced agent initialized with all learning systems")
    print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")
    
    # Run simulation loop
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command with curiosity exploration
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step}: Curiosity: {agent.getCuriosityLevel():.3f}, Novelty: {agent.getNoveltyLevel():.3f}, Firing: {brain.getFiringNeuronCount()}")
    
    print("✓ Advanced agent learning test passed")
    return True

if __name__ == "__main__":
    advanced_agent_learning()