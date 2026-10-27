"""
NLM (Neural Learning Machine) - Python Package

This is the main package initialization for the NLM (Neural Learning Machine) Python bindings.
It provides a unified interface to the NLM neural simulation framework.

The NLM Python package is built on top of the C++ NLM library using pybind11.
It provides Pythonic APIs for creating, configuring, and running neural simulations.
"""

import sys
from pathlib import Path

# Add the current directory to the path
sys.path.insert(0, str(Path(__file__).parent))

# Import core classes and functionality
from .core import Brain, Config
from .agent import AgentBrain
from .world import SimpleWorld
from .sensory import Vision, Audio, InternalSignals
from .motor import Action, MotorCommand
from .visualization import NLMTools, VisualizationTools, DebugTools, PerformanceMonitor
from .performance import PerformanceMonitor
from .debug import DebugTools
from .cli import NLMCLI

# Re-export commonly used classes
__all__ = [
    # Core classes
    'Brain',
    'Config',
    
    # Agent and world
    'AgentBrain',
    'SimpleWorld',
    
    # Sensory inputs
    'Vision',
    'Audio',
    'InternalSignals',
    
    # Motor outputs
    'Action',
    'MotorCommand',
    
    # Utilities
    'NLMTools',
    'VisualizationTools',
    'DebugTools',
    'PerformanceMonitor',
    
    # CLI
    'NLMCLI',
    
    # Helper functions
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
]

# Helper functions for backward compatibility
def createDefaultConfig():
    """Create a default configuration."""
    return Config()

def createBrain(config):
    """Create a brain with configuration."""
    return Brain(config)

def createSimpleWorld():
    """Create a simple world."""
    return SimpleWorld()

def createAgentBrain(brain):
    """Create an agent brain interface."""
    return AgentBrain(brain)

# Version information
__version__ = '1.0.0'
__description__ = 'NLM (Neural Learning Machine) - Python Bindings for Neural Simulation'
__author__ = 'NLM Development Team'
__license__ = 'MIT'
__url__ = 'https://github.com/nlm-project/nlm'

# Package metadata
__package_info__ = {
    'version': __version__,
    'description': __description__,
    'author': __author__,
    'license': __license__,
    'url': __url__,
    'python_requires': '>=3.8',
    'classifiers': [
        'Development Status :: 4 - Beta',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
        'Topic :: Software Development :: Libraries',
    ],
}

# Initialize logging
import logging
logger = logging.getLogger(__name__)

# Package initialization function
def initialize_nlm(output_dir: str = 'nlm_output'):
    """
    Initialize NLM package with specified output directory.
    
    Args:
        output_dir: Output directory for NLM tools
        
    Returns:
        NLMTools: NLM tools instance
    """
    from .visualization import NLMTools
    return NLMTools(output_dir)

# Example usage function
def example_basic():
    """
    Run basic NLM example.
    
    This function demonstrates the basic usage of NLM.
    """
    print("NLM Basic Example")
    print("=" * 50)
    
    # Create configuration
    config = createDefaultConfig()
    config.set("brain.neuron_count", 100)
    config.set("plasticity.stdp.enable", True)
    
    # Create brain
    brain = createBrain(config)
    brain.initialize()
    
    # Run simulation
    for i in range(10):
        brain.step(i)
        if i % 5 == 0:
            print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print("Basic example completed!")

# Advanced example function
def example_advanced():
    """
    Run advanced NLM example.
    
    This function demonstrates advanced usage of NLM with agent-world interaction.
    """
    print("NLM Advanced Example")
    print("=" * 50)
    
    # Create configuration
    config = createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    
    # Create brain and agent
    brain = createBrain(config)
    brain.initialize()
    agent = createAgentBrain(brain)
    
    # Create world
    world = createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(50):
        world.update(0.1)
        
        # Get sensory input and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinking
        brain.step(step)
        
        # Agent action selection
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning
        reward = 1.0 if step % 10 == 0 else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 10 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    print("Advanced example completed!")

# Command line entry point
def main():
    """
    Main entry point for command line interface.
    
    This function is called when the package is executed as a module.
    """
    import argparse
    
    parser = argparse.ArgumentParser(
        description='NLM Command Line Interface',
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        'command',
        choices=['help', 'version', 'basic', 'advanced', 'simulate'],
        help='Command to execute'
    )
    
    parser.add_argument(
        '--output-dir',
        default='nlm_output',
        help='Output directory for NLM tools'
    )
    
    args = parser.parse_args()
    
    if args.command == 'help':
        print("NLM Command Line Interface")
        print("=" * 50)
        print("Available commands:")
        print("  help       Show help information")
        print("  version    Show version information")
        print("  basic      Run basic example")
        print("  advanced   Run advanced example")
        print("  simulate   Run simulation with configuration")
        print()
        print("Usage examples:")
        print("  python -m nlm.main basic")
        print("  python -m nlm.main advanced")
        print("  python -m nlm.main simulate --config config.json --steps 100")
    
    elif args.command == 'version':
        print(f"NLM Python Package v{__version__}")
        print(f"NLM Core Library v0.1.0 (Phase 6: Final Integration)")
        print(f"Python {sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}")
    
    elif args.command == 'basic':
        example_basic()
    
    elif args.command == 'advanced':
        example_advanced()
    
    elif args.command == 'simulate':
        # For simulation, use the CLI module
        from .cli import main as cli_main
        sys.argv = ['nlm.cli', 'simulate']
        cli_main()

# Package metadata for pip
setup = {
    'name': 'nlm',
    'version': __version__,
    'description': __description__,
    'author': __author__,
    'license': __license__,
    'url': __url__,
    'python_requires': '>=3.8',
    'packages': [
        'nlm',
        'nlm.core',
        'nlm.agent',
        'nlm.world',
        'nlm.sensory',
        'nlm.motor',
        'nlm.visualization',
        'nlm.performance',
        'nlm.debug',
    ],
    'install_requires': [
        'numpy>=1.20',
        'matplotlib>=3.5.0',
        'pybind11>=2.11.0',
        'scikit-build-core>=0.5.0',
    ],
    'extras_require': {
        'dev': [
            'pytest>=7.0',
            'sphinx>=4.0',
            'myst-parser>=0.15.0',
        ],
        'visualization': [
            'matplotlib>=3.5.0',
            'networkx>=3.0',
        ],
        'performance': [
            'psutil>=5.9.0',
        ],
    },
    'classifiers': __package_info__['classifiers'],
}

# Export setup for external use
__setup__ = setup

# Make the package discoverable by pip
if __name__ == '__main__':
    main()