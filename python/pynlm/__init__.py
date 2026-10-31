"""
NLM (Neural Learning Machine) - Python Bindings

An experimental computational brain project for neural simulation.
This package provides Python bindings to the C++ NLM neural simulation framework.
"""

from .core import *
from .brain import *
from .world import *
from .agent import *
from .sensory import *
from .motor import *
from .neuromodulation import *
from .memory import *
from .cognition import *
from .development import *
from .visualization import *
from .utils import *
from .exceptions import *
from .config import *

__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Core exports
__all__ = [
    # Core types
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    # Enumerations
    "NeuronType", "SynapseType", "DevelopmentalStage",
    "FiringState", "ActionType", "MotorCommand", "WorldObjectType",
    # Main classes
    "Config", "SensoryInput", "Vision", "Audio", "InternalSignals",
    "Action", "WorldObject", "AgentBody", "ActionResult",
    "SensoryPercept", "SimpleWorld", "Brain", "AgentBrain",
    # Factory functions
    "createDefaultConfig", "createBrain", "createSimpleWorld",
    "createAgentBrain",
    # Constants
    "INVALID_NEURON_ID", "INVALID_SYNAPSE_ID", "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    # Helper classes
    "SimulationStep", "Timestamp", "SimulationTime",
    # Utilities
    "run_simulation", "run_agent_simulation",
    "validate_config", "configure_default",
    # Exceptions
    "NLMError", "NLMConfigurationError", "NLMInitializationError",
    "NLMExecutionError",
    # Advanced modules
    "WorkingMemory", "EpisodicMemory", "SemanticMemory", "LongTermMemory",
    "AttentionState", "Concept", "Conceptualizer", "NeuralPlanner",
    "CognitiveSystem", "DevelopmentalProcess", "DevelopmentalSystem",
    "NeuromodulationState", "RewardProcessor", "NoveltyDetector",
    "CuriositySystem", "EnhancedAgentBrain", "BrainVisualizer",
    "PlotStyle", "Timer", "PerformanceProfiler", "SimulationRecorder",
    "ConfigurationManager", "RandomSeedManager", "BatchProcessor",
    # Visualization
    "VisualizationBackend", "BrainVisualizer",
    # Sensor fusion
    "SensoryState", "VisionProcessor", "AudioProcessor", "InternalSignalsProcessor",
    "SensorySystem",
    # Motor control
    "MovementVector", "MotorCommandBuilder", "MovementPlanner", "MotorSystem",
    # Advanced features
    "Simulation", "AgentSession", "WorldSimulation",
]

# Global configuration
_DEFAULT_CONFIG_FILE = "nlm_config.json"
_ENABLE_LOGGING = False
_MAX_SIMULATION_STEPS = 10000


def enable_logging(level=20):
    """Enable logging for NLM operations.
    
    Args:
        level: Logging level (10=DEBUG, 20=INFO, 30=WARNING, 40=ERROR, 50=CRITICAL)
    """
    global _ENABLE_LOGGING
    import logging
    _ENABLE_LOGGING = True
    logging.basicConfig(level=level)
    logger = logging.getLogger("pynlm")
    logger.info("NLM logging enabled")


def disable_logging():
    """Disable logging for NLM operations."""
    global _ENABLE_LOGGING
    _ENABLE_LOGGING = False


def get_version():
    """Get the NLM Python bindings version."""
    return __version__


def create_simulation(config_file=None):
    """Create a pre-configured simulation from a configuration file.
    
    Args:
        config_file: Path to JSON configuration file (optional)
    
    Returns:
        A configured simulation context manager
    """
    from .simulations import Simulation
    return Simulation(config_file)


def load_pretrained_brain(filepath):
    """Load a pretrained brain from file.
    
    Args:
        filepath: Path to saved brain file
    
    Returns:
        Loaded Brain object
    """
    brain = createBrain(createDefaultConfig())
    brain.load(filepath)
    return brain


def export_brain(brain, filepath):
    """Export brain to file for later use.
    
    Args:
        brain: Brain object to save
        filepath: Path where to save the brain
    """
    brain.save(filepath)


def benchmark_simulation(brain, world, agent, steps=100):
    """Benchmark simulation performance.
    
    Args:
        brain: Brain object
        world: World object  
        agent: AgentBrain object
        steps: Number of simulation steps
    
    Returns:
        dict: Performance metrics
    """
    import time
    start_time = time.time()
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        agent.decodeMotorCommand()
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        agent.applyRewardModulation(0.0, 0.0)
    
    elapsed = time.time() - start_time
    return {
        'steps': steps,
        'elapsed_time': elapsed,
        'steps_per_second': steps / elapsed,
        'average_step_time': elapsed / steps,
    }


# Re-export commonly used items for convenience
brain = createBrain
world = createSimpleWorld
agent = createAgentBrain
