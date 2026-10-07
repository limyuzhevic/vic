# pynlm - Python Bindings for NLM Neural Learning Machine
# Main package initialization

from .core.config import Config, create_default_config
from .core.brain import Brain, createBrain
from .world.world import World, createSimpleWorld
from .world.sensory import SensoryPercept
from .agent.agent import AgentBrain, createAgentBrain
from .neural_types import (
    NeuronType,
    DevelopmentalStage,
    ActionType,
    WorldObjectType
)
from .utils import (
    plot_neural_activity,
    analyze_performance,
    demo
)

# Constants and enums
EXCITATORY = NeuronType.EXCITATORY
INHIBITORY = NeuronType.INHIBITORY
SENSORY = NeuronType.SENSORY
MOTOR = NeuronType.MOTOR
MODULATORY = NeuronType.MODULATORY
INTERNAL = NeuronType.INTERNAL

INITIAL = DevelopmentalStage.INITIAL
CRITICAL_PERIOD = DevelopmentalStage.CRITICAL_PERIOD
MATURATION = DevelopmentalStage.MATURATION
ADULT = DevelopmentalStage.ADULT
AGING = DevelopmentalStage.AGING

MOVE_FORWARD = ActionType.MOVE_FORWARD
TURN_LEFT = ActionType.TURN_LEFT
TURN_RIGHT = ActionType.TURN_RIGHT
INTERACT = ActionType.INTERACT
EAT = ActionType.EAT
SLEEP = ActionType.SLEEP
EXPLORE = ActionType.EXPLORE
STAY = ActionType.STAY

__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Package metadata
__all__ = [
    # Core classes
    'Config',
    'create_default_config',
    'Brain',
    'createBrain',
    'World',
    'createSimpleWorld',
    'AgentBrain',
    'createAgentBrain',
    'SensoryPercept',
    # Enums
    'NeuronType',
    'DevelopmentalStage',
    'ActionType',
    'WorldObjectType',
    # Constants
    'EXCITATORY', 'INHIBITORY', 'SENSORY', 'MOTOR', 'MODULATORY', 'INTERNAL',
    'INITIAL', 'CRITICAL_PERIOD', 'MATURATION', 'ADULT', 'AGING',
    'MOVE_FORWARD', 'TURN_LEFT', 'TURN_RIGHT', 'INTERACT', 'EAT', 'SLEEP', 'EXPLORE', 'STAY',
    # Utilities
    'plot_neural_activity',
    'analyze_performance',
    'demo'
]

# Basic functionality test
def quick_test():
    """Run a quick test of basic functionality."""
    print(f"NLM Python Package v{__version__}")
    print(f"Author: {__author__}")
    print(f"Description: {__description__}")
    print("\nBasic functionality available:")
    print("- Config: Brain configuration management")
    print("- Brain: Neural simulation")
    print("- World: Environment simulation")
    print("- AgentBrain: Agent-world interface")
    print("- SensoryPercept: Sensory processing")
    print("- Neural types: Neuron, developmental stages, actions")
    print("- Utilities: Plotting, analysis, demo")
    print("\nFor detailed documentation, see:")
    print("  - pynlm/README.md")
    print("  - pynlm/HOW_TO_USE.md")
    print("  - pynlm/docs/ARCHITECTURE.md")

if __name__ == "__main__":
    quick_test()