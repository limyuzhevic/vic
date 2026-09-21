# This file makes the Python package directory a proper package
# The actual bindings are defined in python/bindings.cpp and compiled into pynlm.so

# The main API is exposed through the pybind11 module defined in bindings.cpp
# This file simply contains package metadata and documentation

__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Import the main classes from the C++ extension
# These are dynamically created by pybind11 in python/bindings.cpp
import pynlm

# Re-export the main classes for easier access
__all__ = [
    'createBrain',
    'createDefaultConfig', 
    'createSimpleWorld',
    'createAgentBrain',
    'Brain',
    'AgentBrain',
    'SimpleWorld',
    'Config',
    'Vision',
    'Audio',
    'InternalSignals',
    'SensoryInput',
    'Action',
    'WorldObject',
    'AgentBody',
    'SensoryPercept',
    'ActionResult',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    '__version__',
    '__author__',
    '__license__',
    '__description__'
]

# Add helpful functions for common operations

def create_agent(config=None):
    """Create an agent brain with optional configuration.
    
    This is a convenience function that creates a complete agent setup
    with brain, config, and agent interface.
    
    Parameters
    ----------
    config : Config, optional
        Configuration for the brain. If None, uses default config.
        
    Returns
    -------
    tuple
        (brain, agent) tuple for the created agent.
    """
    if config is None:
        config = createDefaultConfig()
    
    brain = createBrain(config)
    agent = createAgentBrain(brain)
    return brain, agent

def create_world(width=20, height=20, vision_width=8, vision_height=8):
    """Create a simple world with specified dimensions.
    
    Parameters
    ----------
    width : float
        World width.
    height : float
        World height.
    vision_width : int
        Vision sensor width.
    vision_height : int
        Vision sensor height.
        
    Returns
    -------
    SimpleWorld
        The created world.
    """
    world = createSimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    return world

def run_simple_simulation(steps=100):
    """Run a simple demonstration simulation.
    
    This is a convenience function that creates a minimal agent-world
    setup and runs a simple simulation.
    
    Parameters
    ----------
    steps : int
        Number of simulation steps to run.
        
    Returns
    -------
    tuple
        (brain, agent, world) tuple from the simulation.
    """
    # Create brain
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    # Create world
    world = createWorld()
    world.reset()
    
    # Create agent
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 50 steps
        if step % 50 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Simulation complete. Ran {steps} steps.")
    return brain, agent, world

# Example usage patterns
examples = {
    'simple_brain': """
import pynlm

# Simple brain example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
    
print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
""",
    
    'agent_world': """
import pynlm

# Complete agent-world example
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
print("Agent simulation complete!")
""",
    
    'brain_stats': """
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)

# Get brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Pending spike events: {brain.getPendingSpikeEventCount()}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
"""
}
