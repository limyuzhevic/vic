# NLM Python Package
# Main package for NLM Python bindings

# Import from the python module where the actual bindings are defined
from python.pynlm import (
    # Brain classes
    Brain,
    createBrain,
    createDefaultConfig,
    
    # Agent classes
    AgentBrain,
    createAgentBrain,
    
    # World classes
    SimpleWorld,
    createSimpleWorld,
    
    # Action and percept classes
    Action,
    SensoryPercept,
    
    # Configuration
    Config,
    
    # Types and enumerations
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    
    # Constants
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    
    # Module metadata
    __version__,
    __author__,
    __description__,
)

# Convenience functions
def quick_test():
    """Run a quick test of the NLM brain"""
    import sys
    from .python.pynlm import createBrain, createDefaultConfig
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    print("Quick NLM Test:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Regions: {brain.getRegionCount()}")
    
    for i in range(100):
        brain.step(i)
    
    print(f"  Steps completed: 100")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    return brain

def run_simulation(brain, world, agent, num_steps):
    """Run a complete simulation with brain, world, and agent"""
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return {
        'steps_completed': num_steps,
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neutrons': brain.getFiringNeuronCount(),
        'final_curiosity': agent.getCuriosityLevel()
    }

# Python module documentation
__all__ = [
    # Brain classes
    'Brain',
    'createBrain',
    'createDefaultConfig',
    
    # Agent classes
    'AgentBrain',
    'createAgentBrain',
    
    # World classes
    'SimpleWorld',
    'createSimpleWorld',
    
    # Action and percept classes
    'Action',
    'SensoryPercept',
    
    # Configuration
    'Config',
    
    # Types and enumerations
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    
    # Constants
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Module metadata
    '__version__',
    '__author__',
    '__description__',
    
    # Functions
    'quick_test',
    'run_simulation',
]