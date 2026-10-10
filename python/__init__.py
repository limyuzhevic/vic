"""
NLM (Neural Learning Machine) - Python Bindings
================================================

A computational brain simulation framework with neural development,
learning, and memory systems.

This package provides Python bindings to the NLM C++ neural simulation
framework, enabling research and experimentation with artificial brains
that can learn, remember, and adapt through interaction with environments.

Key Features:
------------
- **Spiking Neural Networks**: Leaky Integrate-and-Fire (LIF) neurons
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection, prediction error
- **Development**: Neural development and plasticity changes over time
- **Cognition**: Planning, concept formation, attention
- **Prediction**: Forward models and prediction error signals
- **World Interaction**: Simple 2D worlds with sensory processing and motor control
- **Learning**: Multiple plasticity rules (STDP, Hebbian, structural)
- **Persistence**: Checkpoint save/load for continuing simulations

Usage Examples:
--------------

Basic Usage:
```python
import pynlm

# Create a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

Complete Agent Example:
```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run episode
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

Advanced Usage with Utilities:
```python
import pynlm
from python.nlm_utils import NLMExperimentLogger, NLMBenchmark, NLMVisualizer

# Setup logging
logger = NLMExperimentLogger("my_experiment")

# Run simulation with logging
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

    # Log step data
    logger.log_step({
        'step': step,
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'prediction_error': agent.getPredictionError()
    })

# Analyze results
analysis = logger.analyze()
print(f"Experiment had {analysis['total_steps']} steps")
print(f"Average firing rate: {analysis['statistics']['average_firing_rate']['mean']:.2f}")
```

Visualization:
```python
from python.nlm_utils import NLMVisualizer

# Create visualizations
visualizer = NLMVisualizer()
visualizer.create_training_curves(logger.step_data)
```

Performance Benchmarking:
```python
from python.nlm_utils import NLMBenchmark

# Setup benchmark
benchmark = NLMBenchmark(brain, world, agent)

# Benchmark performance
benchmark.benchmark_step(steps=1000, timestep=0.1)
```

Version:
--------
This package provides Python bindings for NLM (Neural Learning Machine) v0.1.0.

For more information, visit: https://github.com/nlm-project/nlm
Documentation: https://nlm.readthedocs.io
"""

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Core functionality
from .pynlm import *  # This would import from compiled bindings

# Advanced utilities
try:
    from .nlm_utils import (
        NLMExperimentLogger,
        NLMBenchmark,
        NLMVisualizer,
        create_default_config,
        create_brain,
        create_simple_world,
        create_agent_brain,
        get_version,
        is_available
    )
except ImportError:
    # Fallback if nlm_utils is not available
    NLMExperimentLogger = None
    NLMBenchmark = None
    NLMVisualizer = None
    create_default_config = None
    create_brain = None
    create_simple_world = None
    create_agent_brain = None

    def get_version():
        return "NLM Python Bindings v0.1.0"

    def is_available():
        try:
            import pynlm
            return True
        except ImportError:
            return False

# Backward compatibility aliases
__all__ = [
    '__version__',
    '__author__',
    '__license__',
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    'NLMExperimentLogger',
    'NLMBenchmark',
    'NLMVisualizer',
    'get_version',
    'is_available'
]

# Make commonly used classes and functions available at package level
try:
    import pynlm
    # Re-export key classes from pynlm
    NeuronId = pynlm.NeuronId
    SynapseId = pynlm.SynapseId
    RegionId = pynlm.RegionId
    PopulationId = pynlm.PopulationId
    NeuronType = pynlm.NeuronType
    SynapseType = pynlm.SynapseType
    DevelopmentalStage = pynlm.DevelopmentalStage
    FiringState = pynlm.FiringState
    ActionType = pynlm.ActionType
    MotorCommand = pynlm.MotorCommand
    WorldObjectType = pynlm.WorldObjectType
    Config = pynlm.Config
    SensoryInput = pynlm.SensoryInput
    Vision = pynlm.Vision
    Audio = pynlm.Audio
    InternalSignals = pynlm.InternalSignals
    Action = pynlm.Action
    WorldObject = pynlm.WorldObject
    AgentBody = pynlm.AgentBody
    ActionResult = pynlm.ActionResult
    SensoryPercept = pynlm.SensoryPercept
    SimpleWorld = pynlm.SimpleWorld
    Brain = pynlm.Brain
    AgentBrain = pynlm.AgentBrain

    # Add to __all__
    __all__.extend([
        'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
        'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
        'ActionType', 'MotorCommand', 'WorldObjectType',
        'Config', 'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
        'Action', 'WorldObject', 'AgentBody', 'ActionResult',
        'SensoryPercept', 'SimpleWorld', 'Brain', 'AgentBrain'
    ])

except ImportError:
    pass

# Initialize logging for the package
import sys
if not hasattr(sys, '_nlm_initialized'):
    print(f"NLM Python Bindings v{__version__} loaded successfully")
    print("For documentation and examples, visit: https://nlm.readthedocs.io")
    sys._nlm_initialized = True
