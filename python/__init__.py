"""
NLM (Neural Learning Machine) - Python Bindings

This package provides Python bindings for the NLM C++ neural simulation framework.
It enables brain simulation, learning, and development through a Python interface.

Key Features:
- Create and simulate neural brains with spiking neurons
- Learn through STDP, Hebbian, and reward-based plasticity
- Develop through age-dependent structural changes
- Navigate and interact with 2D worlds
- Control behavior through sensory input and motor output
- Save and load brain states for persistence

Basic Usage:
```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Save the trained brain
brain.save("trained_brain.bin")
```

The package follows a brain-loop architecture inspired by biological neural systems:
WORLD → SENSORY INPUT → NEURAL PROCESSING → MEMORY/PREDICTION → MOTIVATION/NEUROMODULATION → ACTION → WORLD CONSEQUENCE

For detailed documentation, see:
- docs/ARCHITECTURE.md - System architecture
- docs/SCIENCE.md - Scientific background
- docs/EXPERIMENTS.md - Experiment descriptions
"""

__version__ = "0.1.0"

# Core classes
from .nlm import (
    Config,
    createDefaultConfig,
    Brain,
    createBrain,
    SimpleWorld,
    createSimpleWorld,
    AgentBrain,
    createAgentBrain,
    Action,
    ActionType,
    WorldObject,
    WorldObjectType,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    SensoryPercept,
    AgentBody,
    ActionResult,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    MotorCommand,
)

__all__ = [
    # Version
    "__version__",
    
    # Config
    "Config",
    "createDefaultConfig",
    
    # Brain
    "Brain",
    "createBrain",
    
    # World
    "SimpleWorld",
    "createSimpleWorld",
    
    # Agent
    "AgentBrain",
    "createAgentBrain",
    
    # Data types
    "Action",
    "ActionType",
    "WorldObject",
    "WorldObjectType",
    "SensoryInput",
    "Vision",
    "Audio",
    "InternalSignals",
    "SensoryPercept",
    "AgentBody",
    "ActionResult",
    
    # IDs
    "NeuronId",
    "SynapseId",
    "RegionId",
    "PopulationId",
    
    # Enumerations
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "MotorCommand",
]
