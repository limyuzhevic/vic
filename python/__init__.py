from pynlm.bindings import *

__version__ = "0.1.0"

__all__ = [
    # Core classes
    "Brain",
    "Config", 
    "AgentBrain",
    "SimpleWorld",
    "SensoryInput",
    "Vision",
    "Audio",
    "InternalSignals",
    "Action",
    "WorldObject",
    "AgentBody",
    "SensoryPercept",
    "ActionResult",
    # Enums
    "NeuronType",
    "SynapseType", 
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    # Helper functions
    "createDefaultConfig",
    "createBrain",
    "createSimpleWorld",
    "createAgentBrain",
    # Constants
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
]

# Pythonic aliases for common usage
brain = createBrain
world = createSimpleWorld
agent = createAgentBrain

def run_simulation(brain_obj, world_obj, agent_obj, steps=100):
    """Convenience function to run a complete simulation.
    
    Args:
        brain_obj: Brain instance
        world_obj: SimpleWorld instance  
        agent_obj: AgentBrain instance
        steps: Number of simulation steps to run
        
    Returns:
        dict: Statistics about the simulation
    """
    import time
    start_time = time.time()
    
    for step in range(steps):
        # Update world
        world_obj.update(0.1)
        
        # Get and process sensory input
        percept = world_obj.getSensoryPercept()
        agent_obj.processSensoryInput(percept)
        
        # Run brain step
        brain_obj.step(step)
        
        # Decode and apply motor command
        motor_cmd = agent_obj.decodeMotorCommand()
        world_obj.applyMotorCommand(motor_cmd, world_obj.getSimulationTime())
        
        # Get reward for neuromodulation
        reward = world_obj.getSensoryPercept().getInternal()[0] if world_obj.getSensoryPercept().getInternal() else 0.0
        agent_obj.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent_obj.updateDevelopment(0.1)
        
        # Log progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: {brain_obj.getFiringNeuronCount()} neurons firing, "
                  f"curiosity={agent_obj.getCuriosityLevel():.3f}, "
                  f"novelty={agent_obj.getNoveltyLevel():.3f}")
    
    end_time = time.time()
    
    return {
        "steps_completed": steps,
        "total_time": end_time - start_time,
        "final_firing_rate": brain_obj.getAverageFiringRate(),
        "total_spikes": brain_obj.getTotalSpikeCount(),
        "final_curiosity": agent_obj.getCuriosityLevel(),
        "final_novelty": agent_obj.getNoveltyLevel(),
        "final_development_stage": brain_obj.getDevelopmentalStage(),
    }

def create_advanced_brain(neuron_count=1000, learning_rate=0.01, enable_plasticity=True, 
                         enable_development=True, enable_curiosity=True):
    """Create a brain with advanced configuration for complex simulations.
    
    Args:
        neuron_count: Number of neurons in the brain
        learning_rate: Base learning rate for plasticity
        enable_plasticity: Enable synaptic plasticity
        enable_development: Enable developmental processes
        enable_curiosity: Enable curiosity-driven exploration
        
    Returns:
        tuple: (brain, agent, world) configured for advanced usage
    """
    # Create configuration
    config = createDefaultConfig()
    
    # Set advanced parameters
    config.set("brain.neuron_count", neuron_count)
    config.set("plasticity.stdp.learning_rate", learning_rate)
    config.set("plasticity.hebbian.enable", enable_plasticity)
    config.set("plasticity.structural.enable", enable_plasticity)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", enable_curiosity)
    config.set("neuromod.novelty.enable", True)
    
    # Create components
    brain_obj = createBrain(config)
    brain_obj.initialize()
    
    world_obj = createSimpleWorld()
    world_obj.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world_obj.reset()
    
    agent_obj = createAgentBrain(brain_obj)
    agent_obj.initialize(world_obj)
    
    # Enable advanced features
    agent_obj.enableRewardModulation(True)
    agent_obj.enableStructuralPlasticity(enable_plasticity)
    agent_obj.enableDevelopment(enable_development)
    agent_obj.enableCuriosity(enable_curiosity)
    
    return brain_obj, agent_obj, world_obj

def example_complete_agent_simulation(num_steps=1000):
    """Complete agent simulation example for easy use.
    
    This provides a ready-to-use example that demonstrates all major features
    including sensory processing, motor control, reward-based learning, and
    development.
    """
    # Create advanced brain and environment
    brain_obj, agent_obj, world_obj = create_advanced_brain(
        neuron_count=500,
        learning_rate=0.01,
        enable_plasticity=True,
        enable_development=True,
        enable_curiosity=True
    )
    
    # Set agent starting position
    world_obj.setAgentStart(10.0, 10.0)
    
    # Run the simulation
    print(f"Running complete agent simulation with {num_steps} steps...")
    results = run_simulation(brain_obj, world_obj, agent_obj, num_steps)
    
    # Print final statistics
    print("\n=== SIMULATION RESULTS ===")
    print(f"Steps completed: {results['steps_completed']}")
    print(f"Total time: {results['total_time']:.2f}s")
    print(f"Final firing rate: {results['final_firing_rate']:.2f} Hz")
    print(f"Total spikes: {results['total_spikes']}")
    print(f"Final curiosity level: {results['final_curiosity']:.3f}")
    print(f"Final novelty level: {results['final_novelty']:.3f}")
    print(f"Final development stage: {results['final_development_stage']}")
    
    return brain_obj, agent_obj, world_obj, results