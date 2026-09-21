# pynlm - Python Bindings for NLM
# 
# This module provides Python access to the NLM (Neural Learning Machine) C++ framework.
# It exposes the complete brain simulation functionality to Python.
#
# Installation:
#   pip install -e .
#
# Usage:
#   import pynlm
#   brain = pynlm.createBrain(pynlm.createDefaultConfig())
#   brain.initialize()
#   for i in range(100):
#       brain.step(i)

from pynlm import (
    # Core types and enumerations
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # Main classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    
    # Module-level functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
)

# Export version
__version__ = "0.1.0"

# Common constants for user convenience
DEFAULT_NEURON_COUNT = 1000
DEFAULT_REGION_COUNT = 1
DEFAULT_CONNECTION_PROBABILITY = 0.1

def create_test_config(neuron_count=DEFAULT_NEURON_COUNT, 
                      region_count=DEFAULT_REGION_COUNT,
                      connection_probability=DEFAULT_CONNECTION_PROBABILITY):
    """Create a common test configuration.
    
    Args:
        neuron_count: Number of neurons in the brain
        region_count: Number of brain regions
        connection_probability: Probability of connections between neurons
        
    Returns:
        Config object with common settings
    """
    config = createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", region_count)
    config.set("brain.connection_probability", connection_probability)
    return config

def create_simple_world(width=20, height=20, vision_width=8, vision_height=8):
    """Create a simple 2D world.
    
    Args:
        width: World width in tiles
        height: World height in tiles
        vision_width: Agent's vision width
        vision_height: Agent's vision height
        
    Returns:
        SimpleWorld object
    """
    world = createSimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    return world

def run_simulation_example():
    """Example: Run a simple brain simulation.
    
    This function demonstrates the basic usage pattern for simulating
    a spiking neural network with the NLM framework.
    """
    # Create brain with default config
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    # Run simulation steps
    print(f"Starting simulation with {brain.getTotalNeuronCount()} neurons")
    for step in range(100):
        brain.step(step)
        
        # Print periodic updates
        if step % 20 == 0:
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            total_spikes = brain.getTotalSpikeCount()
            print(f"  Step {step}: {firing} firing neurons, {rate:.2f} Hz, {total_spikes} total spikes")
    
    print("Simulation complete!")

def run_agent_simulation_example():
    """Example: Run a complete agent simulation with world interaction.
    
    This function demonstrates how an agent can interact with an environment
    using NLM brain components.
    """
    import pynlm
    
    # Create brain and agent
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    agent = createAgentBrain(brain)
    
    # Create world
    world = createSimpleWorld(width=15, height=15, vision_width=8, vision_height=8)
    world.reset()
    
    agent.initialize(world)
    
    # Run episode
    for step in range(200):
        # World update
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Print progress
        if step % 40 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    print("Agent simulation complete!")

# Convenience classes for common use cases

class SimpleAgent:
    """Helper class for common agent behavior patterns.
    
    This class provides common functionality for using NLM brains
    in agent-based simulations.
    """
    
    def __init__(self, brain_config=None, world_config=None):
        """Initialize agent with brain and world.
        
        Args:
            brain_config: Brain configuration (optional)
            world_config: World configuration dict (optional)
        """
        self.brain = createBrain(brain_config or createDefaultConfig())
        self.agent = createAgentBrain(self.brain)
        self.world = createSimpleWorld()
        self.setup(world_config or {})
        
    def setup(self, world_config):
        """Setup world configuration.
        
        Args:
            world_config: Dict with configuration keys:
                width: World width
                height: World height
                vision_width: Vision width
                vision_height: Vision height
        """
        self.world.configure(
            world_config.get('width', 20),
            world_config.get('height', 20),
            world_config.get('vision_width', 8),
            world_config.get('vision_height', 8)
        )
        self.world.reset()
        self.agent.initialize(self.world)
        
    def run_step(self):
        """Run a single simulation step.
        
        Returns:
            Dict with state information
        """
        # Update world
        self.world.update(0.1)
        
        # Get perception
        percept = self.world.getSensoryPercept()
        
        # Process input
        self.agent.processSensoryInput(percept)
        self.brain.step(0)
        
        # Get action
        action = self.agent.decodeMotorCommand()
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Return state
        return {
            'step': self.world.getSimulationTime(),
            'firing': self.brain.getFiringNeuronCount(),
            'rate': self.brain.getAverageFiringRate(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'action': action
        }
    
    def run_episode(self, steps, enable_learning=True):
        """Run an episode of simulation.
        
        Args:
            steps: Number of steps to run
            enable_learning: Enable learning systems
            
        Returns:
            List of step results
        """
        results = []
        
        # Enable subsystems if requested
        if enable_learning:
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
            self.agent.enableDevelopment(True)
            self.agent.enableStructuralPlasticity(True)
        
        for _ in range(steps):
            results.append(self.run_step())
            
        return results

# Documentation for users
def get_usage_examples():
    """Get examples of how to use NLM in Python.
    
    Returns:
        Dictionary of usage examples
    """
    return {
        'simple_brain': '''
import pynlm

# Create and run a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
''',
        
        'agent_simulation': '''
import pynlm

# Setup brain, agent, and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent.initialize(world)

# Run simulation
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
''',
        
        'learning_simulation': '''
import pynlm

# Create learning agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(15, 15, 8, 8)
world.reset()

agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run learning simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward based on world state
    percept = world.getSensoryPercept()
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
'''
    }
