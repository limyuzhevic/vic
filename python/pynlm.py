"""NLM (Neural Learning Machine) Python bindings

NLM is a brain simulator that creates virtual brains that can learn, remember things,
and make decisions. This module provides Python bindings to the C++ NLM neural
simulation framework.

The module exposes classes for:
- Brain: Virtual brain with neurons and synapses
- Config: Configuration settings for the brain
- AgentBrain: Interface connecting brain to world
- SimpleWorld: Virtual environment
- Vision: Visual sensory input
- Audio: Audio sensory input
- SensoryPercept: Sensory perception data
- Action: Motor actions

Example usage:
    import pynlm

    # Create a brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()

    # Run for some steps
    for i in range(100):
        brain.step(i)

    print(f"Brain produced {brain.getTotalSpikeCount()} spikes")

For detailed documentation, see the HOW_TO_USE.md file.
"""

import sys
from pathlib import Path

# Add current directory to path so we can import pynlm module
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    from pynlm import (
        createBrain,
        createDefaultConfig,
        createSimpleWorld,
        createAgentBrain,
        Vision,
        Audio,
        InternalSignals,
        Action,
        SensoryPercept,
        WorldObject,
        AgentBody,
        ActionResult,
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
    )
    from typing import List, Optional, Union

    # Create a convenience alias for common patterns
    class BrainConfig:
        """Convenience class for creating brain configurations."""

        @staticmethod
        def create_default():
            """Create a default configuration for general use."""
            return createDefaultConfig()

        @staticmethod
        def create_basic_brain(neuron_count: int = 1000):
            """Create a basic configuration for a simple brain."""
            config = createDefaultConfig()
            config.set("neuron_count", neuron_count)
            config.set("connection_probability", 0.1)
            config.set("plasticity_learning_rate", 0.01)
            return config

    class SimpleSimulation:
        """Helper class for common simulation patterns."""

        @staticmethod
        def run_silent_brain(config, steps: int = 100):
            """Run a brain without world interaction."""
            brain = createBrain(config)
            brain.initialize()

            for step in range(steps):
                brain.step(step)

            return {
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "average_rate": brain.getAverageFiringRate(),
                "excitement_ratio": brain.getExcitationInhibitionRatio(),
            }

        @staticmethod
        def run_agent_simulation(brain, world, steps: int = 100):
            """Run a complete agent simulation with world interaction."""
            agent = createAgentBrain(brain)
            agent.initialize(world)

            for step in range(steps):
                # Get what the agent sees
                percept = world.getSensoryPercept()

                # Process sensory input
                agent.processSensoryInput(percept)

                # Brain thinks
                brain.step(step)

                # Get action from brain
                action = agent.decodeMotorCommand()

                # Do action in world
                world.applyMotorCommand(action, world.getSimulationTime())

            return {
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "average_rate": brain.getAverageFiringRate(),
                "development_stage": brain.getDevelopmentalStage(),
                "neuromodulation": agent.getNeuromodulationLevel(),
            }

    # Export everything
    __all__ = [
        "pynlm",
        "createBrain",
        "createDefaultConfig",
        "createSimpleWorld",
        "createAgentBrain",
        "Vision",
        "Audio",
        "InternalSignals",
        "Action",
        "SensoryPercept",
        "WorldObject",
        "AgentBody",
        "ActionResult",
        "INVALID_NEURON_ID",
        "INVALID_SYNAPSE_ID",
        "INVALID_REGION_ID",
        "INVALID_POPULATION_ID",
        "NeuronType",
        "SynapseType",
        "DevelopmentalStage",
        "FiringState",
        "ActionType",
        "MotorCommand",
        "WorldObjectType",
        "BrainConfig",
        "SimpleSimulation",
    ]

    # Add module metadata
    pynlm.__version__ = "1.0.0"
    pynlm.__description__ = "Neural Learning Machine - Experimental Artificial Developmental Brain"

except ImportError as e:
    # If pybind11 bindings are not available, provide a helpful error message
    def _error_handler(*args, **kwargs):
        raise ImportError(
            f"Failed to import NLM Python bindings: {e}\n"
            "Please ensure the NLM C++ library has been built and installed correctly."
        )

    # Create dummy classes that raise helpful errors
    class BrainConfig:
        pass

    class SimpleSimulation:
        pass

    # Add error-raising versions of functions
    def createBrain(*args, **kwargs):
        raise ImportError("NLM C++ bindings not available. Please build the library first.")

    def createDefaultConfig():
        raise ImportError("NLM C++ bindings not available. Please build the library first.")

    # etc.

    __all__ = ["BrainConfig", "SimpleSimulation"]

    # Make the error-raising functions available at module level
    globals()["createBrain"] = createBrain
    globals()["createDefaultConfig"] = createDefaultConfig
    globals()["createSimpleWorld"] = lambda: None  # noqa
    globals()["createAgentBrain"] = lambda x: None  # noqa

    # Set a helpful error for anyone who tries to use the module
    pynlm = sys.modules[__name__]
    pynlm.__version__ = "1.0.0"
    pynlm.__description__ = "NLM Python bindings - C++ bindings not loaded"

# Make the module callable as a package
__path__ = [Path(__file__).parent]