"""
pynlm - Python Interface for NLM (Neural Learning Machine)

A comprehensive Python interface for the NLM C++ neural simulation framework.
This module provides a user-friendly API for creating, configuring, and running
neural simulations with advanced features for both beginners and experts.
"""

# Import submodules first to avoid circular imports
from pynlm import utils, advanced_actions, monitoring, experiment

from pynlm import (
    # Core API
    createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain,
    
    # Classes
    Brain, Config, SimpleWorld, AgentBrain, Vision, Audio, 
    InternalSignals, Action, SensoryPercept, AgentBody, WorldObject,
    ActionResult,
    
    # Enums
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    
    # IDs
    NeuronId, SynapseId, RegionId, PopulationId,
    
    # Invalid constants
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID,
)

# Re-export advanced classes and functions
from pynlm.utils import (
    SimulationState, MetricType, MetricsCalculator, SimulationController,
    ExperimentRunner, VisualizationHelper, benchmark_configuration,
    generate_performance_report
)

from pynlm.advanced_actions import (
    ActionType as AdvancedActionType, ActionPriority, ActionContext,
    ActionSequence, ActionPlanner, BehaviorTree, Node, ActionNode,
    CompositeNode, NodeStatus, create_advanced_behavior_tree,
    create_action_sequence_from_context
)

from pynlm.monitoring import (
    MonitoringSystem, VisualizationHelper as MonitoringVisualizationHelper
)

from pynlm.experiment import (
    ExperimentStatus, ExperimentConfig, ExperimentResult,
    Experiment, LearningExperiment, AdaptationExperiment,
    ExperimentManager, create_learning_experiment,
    create_adaptation_experiment
)

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine)"
__license__ = "MIT"

# Convenience function aliases
brain = createBrain
config = createDefaultConfig
world = createSimpleWorld
agent = createAgentBrain

# Example usage function
def quick_demo():
    """Run a quick demonstration of NLM capabilities."""
    print("=== NLM Quick Demo ===")
    print("This demonstrates basic NLM functionality.")
    print()
    print("1. Creating a simple brain...")
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
    print()
    
    print("2. Running simulation...")
    for i in range(10):
        brain.step(i)
    
    print(f"   Simulated {brain.getTotalSpikeCount()} spikes")
    print()
    
    print("3. Creating a world and agent...")
    world = createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"   World created with {world.getWidth()}x{world.getHeight()} cells")
    print()
    
    print("4. Running agent simulation...")
    for step in range(10):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"   Simulation completed with {brain.getTotalSpikeCount()} total spikes")
    print()
    print("=== Demo Complete ===")
    return True
# Help and documentation functions
def get_help(topic: str = "general"):
    """Get help information about NLM topics."""
    
    help_topics = {
        'general': {
            'title': 'General NLM Usage',
            'content': """
NLM (Neural Learning Machine) is a Python interface for simulating neural networks.

Basic Usage:
------------
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
"""
        },
        
        'agent': {
            'title': 'Agent-Based Simulation',
            'content': """
Agent-based simulation allows you to create autonomous agents that interact with their environment.

Example:
--------
import pynlm

# Create brain, world, and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
"""
        },
        
        'learning': {
            'title': 'Learning Systems',
            'content': """
NLM supports multiple learning mechanisms:

1. Reward Modulation - Learn from rewards and punishments
2. Curiosity - Drive for exploration and novel experiences
3. Development - Brain maturation and plasticity changes
4. Structural Plasticity - Create and eliminate connections
5. STDP - Spike-Timing Dependent Plasticity
6. Hebbian Learning - Co-activation based learning

Enable with:
---------
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
"""
        },
        
        'advanced': {
            'title': 'Advanced Features',
            'content': """
Advanced features are available in the submodules:

- utils: Metrics calculation, simulation control, experiment management
- advanced_actions: Action planning, behavior trees, action sequences
- monitoring: Real-time monitoring and visualization
- experiment: Experiment framework and automation

Examples:
---------
# Monitoring
from pynlm.monitoring import MonitoringSystem
monitor = MonitoringSystem(brain, agent, world)
monitor.start()

# Advanced actions
from pynlm.advanced_actions import create_advanced_behavior_tree
tree = create_advanced_behavior_tree()

# Experiments
from pynlm.experiment import LearningExperiment
exp = LearningExperiment("My Experiment")
result = exp.run(1000)
"""
        }
    }
    
    if topic in help_topics:
        help_info = help_topics[topic]
        print(f"=== {help_info['title']} ===")
        print(help_info['content'])
    else:
        print(f"Help topic '{topic}' not found.")
        print("Available topics:", list(help_topics.keys()))
    
    return help_topics.get(topic, {})
# Configuration presets
def get_preset_config(preset_name: str):
    """Get preset configuration for common scenarios."""
    
    presets = {
        'simple': {
            'brain_neuron_count': 500,
            'plasticity.stdp.enable': True,
            'neuromod.dopamine.scale': 1.0,
            'development.stage': 'Adult'
        },
        
        'learning': {
            'brain_neuron_count': 1000,
            'plasticity.stdp.enable': True,
            'plasticity.hebbian.enable': True,
            'plasticity.structural.enable': True,
            'neuromod.curiosity.enable': True,
            'neuromod.reward.enable': True,
            'neuromod.dopamine.scale': 1.5,
            'development.enable': True
        },
        
        'exploration': {
            'brain_neuron_count': 2000,
            'neuromod.curiosity.enable': True,
            'neuromod.curiosity.exploration_rate': 0.8,
            'neuromod.novelty.enable': True,
            'neuromod.novelty.threshold': 0.7,
            'development.stage': 'CriticalPeriod'
        },
        
        'performance': {
            'brain_neuron_count': 5000,
            'plasticity.stdp.enable': True,
            'plasticity.stdp.learning_rate': 0.002,
            'plasticity.structural.enable': True,
            'plasticity.structural.synaptogenesis_rate': 0.0001,
            'neuromod.curiosity.enable': False,
            'neuromod.reward.enable': True,
            'development.enable': False
        }
    }
    
    if preset_name in presets:
        return presets[preset_name]
    else:
        raise ValueError(f"Preset '{preset_name}' not found. Available presets: {list(presets.keys())}")
def apply_preset(config, preset_name: str):
    """Apply a preset configuration to a Config object."""
    
    preset = get_preset_config(preset_name)
    
    for key, value in preset.items():
        config.set(key, value)
    
    return config
# Re-export submodules
__all__ = [
    # Core API
    'createBrain', 'createDefaultConfig', 'createSimpleWorld', 'createAgentBrain',
    'Brain', 'Config', 'SimpleWorld', 'AgentBrain', 'Vision', 'Audio',
    'InternalSignals', 'Action', 'SensoryPercept', 'AgentBody', 'WorldObject',
    'ActionResult',
    
    # Enums
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # IDs
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Submodules
    'utils', 'advanced_actions', 'monitoring', 'experiment',
    
    # Advanced classes
    'SimulationState', 'MetricType', 'MetricsCalculator', 'SimulationController',
    'ExperimentRunner', 'VisualizationHelper', 'benchmark_configuration',
    'generate_performance_report',
    
    'ActionType as AdvancedActionType', 'ActionPriority', 'ActionContext',
    'ActionSequence', 'ActionPlanner', 'BehaviorTree', 'Node', 'ActionNode',
    'CompositeNode', 'NodeStatus', 'create_advanced_behavior_tree',
    'create_action_sequence_from_context',
    
    'MonitoringSystem', 'MonitoringVisualizationHelper',
    
    'ExperimentStatus', 'ExperimentConfig', 'ExperimentResult',
    'Experiment', 'LearningExperiment', 'AdaptationExperiment',
    'ExperimentManager', 'create_learning_experiment',
    'create_adaptation_experiment',
    
    # Convenience aliases
    'brain', 'config', 'world', 'agent',
    
    # Version and info
    '__version__', '__author__', '__description__', '__license__',
    
    # Functions
    'quick_demo', 'get_help', 'get_preset_config', 'apply_preset'
]
