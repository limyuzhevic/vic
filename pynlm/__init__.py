"""
NLM (Neural Learning Machine) Python Bindings

An experimental artificial developmental brain simulation framework.

This package provides Python bindings for the NLM C++ neural simulation framework,
enabling users to create, configure, and interact with virtual brains through Python.

Key Features:
- Intuitive brain creation and configuration
- Real-time simulation control
- Sensory-motor integration
- Learning and plasticity support
- Comprehensive statistical tracking
- Easy visualization and analysis tools

Basic Usage Example:
    import pynlm

    # Create a brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()

    # Run simulation
    for i in range(100):
        brain.step(i)

    # Get statistics
    print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")

Available Modules:
    pynlm.brain    - Brain simulation classes and methods
    pynlm.config   - Configuration management
    pynlm.world    - Environment and world simulation
    pynlm.agent    - Agent and sensory-motor integration
    pynlm.learning - Learning and plasticity utilities
    pynlm.analysis - Analysis and statistics tools
    pynlm.visualization - Visualization utilities

For comprehensive documentation, see the README.md file.
"""

from .brain import *
from .config import *
from .world import *
from .agent import *
from .learning import *
from .analysis import *
from .visualization import *

__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"
__all__ = [
    # Brain classes
    'Brain', 'Neuron', 'Synapse', 'Region', 'Population',
    # Configuration
    'Config', 'createDefaultConfig', 'createMinimalConfig',
    # World simulation
    'SimpleWorld', 'createSimpleWorld',
    # Agent interface
    'AgentBrain', 'createAgentBrain',
    # Learning utilities
    'createAgentSystem', 'runLearningExperiment',
    # Analysis tools
    'analyzeSimulation', 'plotResults',
    # Visualization
    'createVisualization', 'Visualizations',
    # Common types
    'NeuronType', 'SynapseType', 'ActionType', 'DevelopmentalStage',
    'FiringState', 'MotorCommand', 'WorldObjectType',
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID', 'PI', 'E', 'FLOAT_EPSILON',
    # Utility functions
    'getVersion', 'getAvailableDevices', 'setLogLevel',
    # Main entry points
    'main', 'cli'
]


def createBrain(config=None):
    """
    Create a new brain with the specified configuration.

    Args:
        config (Config, optional): Configuration for the brain. 
                                  If None, creates a default configuration.

    Returns:
        Brain: A new brain instance ready for initialization.

    Example:
        >>> import pynlm
        >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
        >>> brain.initialize()
    """
    if config is None:
        config = createDefaultConfig()
    
    # Import here to avoid circular imports
    from .brain import Brain
    return Brain(config)


def createSimpleWorld():
    """
    Create a new simple world for simulation.

    Returns:
        SimpleWorld: A new world instance.

    Example:
        >>> import pynlm
        >>> world = pynlm.createSimpleWorld()
        >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    """
    # Import here to avoid circular imports
    from .world import SimpleWorld
    return SimpleWorld()


def createAgentBrain(brain):
    """
    Create an agent brain interface for a given brain.

    Args:
        brain (Brain): The brain to create an interface for.

    Returns:
        AgentBrain: An agent brain interface instance.

    Example:
        >>> import pynlm
        >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
        >>> agent = pynlm.createAgentBrain(brain)
    """
    # Import here to avoid circular imports
    from .agent import AgentBrain
    return AgentBrain(brain)


def createAgentSystem():
    """
    Create a complete agent system with brain, world, and agent.

    Returns:
        tuple: (brain, world, agent) - A complete agent system.

    Example:
        >>> import pynlm
        >>> brain, world, agent = pynlm.createAgentSystem()
        >>> agent.initialize(world)
    """
    # Import here to avoid circular imports
    from .brain import Brain
    from .world import SimpleWorld
    from .agent import AgentBrain
    
    config = createDefaultConfig()
    brain = Brain(config)
    world = SimpleWorld()
    agent = AgentBrain(brain)
    
    return brain, world, agent


def getVersion():
    """
    Get the version of the NLM Python bindings.

    Returns:
        str: Version string.
    """
    return __version__


def setLogLevel(level):
    """
    Set the global logging level.

    Args:
        level (str): Log level ('DEBUG', 'INFO', 'WARNING', 'ERROR').
    """
    # Import here to avoid circular imports
    from .config import Logger
    
    if level.upper() == "DEBUG":
        Logger::setGlobalLevel(LogLevel.Debug)
    elif level.upper() == "INFO":
        Logger::setGlobalLevel(LogLevel.Info)
    elif level.upper() == "WARNING":
        Logger::setGlobalLevel(LogLevel.Warning)
    elif level.upper() == "ERROR":
        Logger::setGlobalLevel(LogLevel.Error)


def main():
    """
    Main entry point for the NLM Python package.
    
    This function is used by the command-line interface and provides
    programmatic access to NLM functionality.
    """
    import argparse
    import sys
    
    parser = argparse.ArgumentParser(
        prog="pynlm",
        description="NLM (Neural Learning Machine) Python Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Use 'pynlm --help' for command-line help, or run 'import pynlm' for programmatic access.
        """
    )
    
    parser.add_argument(
        "action",
        nargs="?",
        help="Action to perform (create, run, analyze, learn)"
    )
    
    args = parser.parse_args()
    
    if args.action:
        print(f"Action '{args.action}' not recognized in Python mode.")
        print("Use 'pynlm' command for CLI access, or 'import pynlm' for programmatic access.")
        return 1
    
    print("NLM Python bindings loaded successfully!")
    print(f"Version: {__version__}")
    print("\nFor usage examples, see the documentation or import pynlm and use help()")
    
    return 0


# Initialize logging
from .config import Logger, LogLevel
Logger::setGlobalLevel(LogLevel.Info)