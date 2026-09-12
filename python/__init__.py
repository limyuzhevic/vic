# NLM Python Bindings (Main Entry Point)
# This file serves as the main entry point for the NLM Python package

from .pynlm import (
    # Core brain classes
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
    
    # Enumerations and types
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

# Convenience functions for common operations
def quick_test():
    """Run a quick test of the NLM brain initialization and basic functionality."""
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    print("=== Quick NLM Test ===")
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Synapses: {brain.getTotalSynapseCount()}")
    print(f"Regions: {brain.getRegionCount()}")
    
    # Run 100 steps to verify basic functionality
    for step in range(100):
        brain.step(step)
    
    print(f"Completed {100} simulation steps")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    return brain

def run_simulation(brain, world, agent, num_steps):
    """Run a complete agent simulation with brain, world, and agent."""
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
    
    # Enumerations and types
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
    
    # Convenience functions
    'quick_test',
    'run_simulation',
]