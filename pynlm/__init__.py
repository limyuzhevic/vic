"""
NLM (熙然) - Neural Learning Machine

Python bindings for the NLM C++ neural simulation framework.

This module provides access to brain simulation, neural computation,
and artificial developmental systems through a Python-friendly API.

Key Features:
- Brain simulation with spiking neurons and synapses
- Real-world neural dynamics and learning
- World interaction and agent-based simulation
- Neuromodulation and development systems
- Full integration with C++ performance
"""

__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Import core functionality from pybind11 bindings
from .core import *
from .world import *
from .agent import *
from .brain import *
from .neuromodulation import *
from .plasticity import *
from .memory import *
from .cognition import *

__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Configuration and brain
    'Config', 'Brain', 'AgentBrain',
    
    # World and interaction
    'SimpleWorld', 'WorldObject', 'AgentBody',
    'ActionResult', 'SensoryPercept',
    
    # Sensory systems
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput',
    
    # Performance utilities
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
]

# Platform and version information
import sys
import platform

def get_platform_info():
    """Get platform information for debugging and compatibility."""
    return {
        'python_version': sys.version,
        'platform': platform.platform(),
        'processor': platform.processor(),
        'architecture': platform.architecture(),
        'machine': platform.machine(),
        'python_implementation': platform.python_implementation(),
    }

def get_module_info():
    """Get information about the pynlm module."""
    return {
        'name': 'pynlm',
        'version': __version__,
        'description': __description__,
        'author': __author__,
        'platform_info': get_platform_info(),
        'build_info': {
            'built_with': 'pybind11',
            'cxx_standard': 'C++20',
            'compiled': True,
        }
    }

def check_version(required_version=None):
    """
    Check if the installed version meets a minimum requirement.
    
    Parameters
    ----------
    required_version : str, optional
        Minimum version required (e.g., "0.1.0").
        If None, just returns current version.
    
    Returns
    -------
    bool or str
        If required_version is None, returns current version string.
        Otherwise, returns True if version is sufficient, False otherwise.
    """
    from packaging import version
    
    if required_version is None:
        return __version__
    
    return version.parse(__version__) >= version.parse(required_version)

def print_banner():
    """Print a nice banner with module information."""
    info = get_module_info()
    print(f"\n{'='*60}")
    print(f"NLM (熙然) - Neural Learning Machine - Version {info['version']}")
    print(f"{'='*60}")
    print(f"Description: {info['description']}")
    print(f"Author: {info['author']}")
    print(f"Python: {info['platform_info']['python_version'].split()[0]}")
    print(f"Platform: {info['platform_info']['platform']}")
    print(f"{'='*60}\n")

# Module-level convenience functions

def create_brain_with_config(config_dict=None):
    """
    Create a brain with optional configuration.
    
    Parameters
    ----------
    config_dict : dict, optional
        Configuration parameters. If None, uses default configuration.
    
    Returns
    -------
    Brain
        Initialized brain object.
    """
    if config_dict is None:
        return createBrain(createDefaultConfig())
    else:
        config = createDefaultConfig()
        for key, value in config_dict.items():
            config.set(key, value)
        return createBrain(config)

def create_agent_with_world(brain_config=None, world_config=None):
    """
    Create a complete agent system with brain and world.
    
    Parameters
    ----------
    brain_config : dict, optional
        Brain configuration parameters.
    world_config : dict, optional
        World configuration parameters.
    
    Returns
    -------
    tuple
        (brain, agent, world) - The created components.
    """
    brain = create_brain_with_config(brain_config)
    brain.initialize()
    
    world = createSimpleWorld()
    if world_config:
        if 'width' in world_config:
            world.configure(**world_config)
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    return brain, agent, world

def run_episode(agent_config=None, world_config=None, num_steps=100):
    """
    Run a complete agent episode.
    
    Parameters
    ----------
    agent_config : dict, optional
        Configuration for the agent and subsystems.
    world_config : dict, optional
        Configuration for the world environment.
    num_steps : int
        Number of simulation steps to run.
    
    Returns
    -------
    dict
        Episode statistics and results.
    """
    # Create agent system
    brain, agent, world = create_agent_with_world(agent_config, world_config)
    
    # Enable subsystems if configured
    if agent_config:
        agent.enableRewardModulation(agent_config.get('reward_modulation', True))
        agent.enableStructuralPlasticity(agent_config.get('structural_plasticity', True))
        agent.enableDevelopment(agent_config.get('development', True))
        agent.enableCuriosity(agent_config.get('curiosity', True))
    
    # Run simulation
    stats = {
        'steps': num_steps,
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'development_stages': [],
        'total_spikes': 0,
        'max_firing': 0,
        'final_position': (world.getAgentBody().x, world.getAgentBody().y),
        'final_energy': world.getAgentBody().energy,
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process and run brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Decode and execute action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        internal = percept.getInternal()
        reward = internal[0] if internal and len(internal) > 0 else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        stats['firing_rates'].append(brain.getFiringNeuronCount())
        stats['curiosity_levels'].append(agent.getCuriosityLevel())
        stats['novelty_levels'].append(agent.getNoveltyLevel())
        stats['development_stages'].append(brain.getDevelopmentalStage())
        
        # Update running totals
        stats['total_spikes'] += brain.getTotalSpikeCount()
        stats['max_firing'] = max(stats['max_firing'], brain.getFiringNeuronCount())
    
    return stats

# Example usage functions
def demo_simple_brain():
    """Simple demonstration of brain simulation."""
    print("Creating a simple brain...")
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    print("Running 10 simulation steps...")
    for i in range(10):
        brain.step(i)
    
    print(f"Brain has {brain.getTotalNeuronCount()} neurons")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    
    return brain

def demo_agent_world():
    """Demonstration of agent in world interaction."""
    print("Setting up agent in world...")
    
    # Create components
    brain = create_brain_with_config({'brain.neuron_count': 500})
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    print("Running 50 simulation steps...")
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Final position: ({world.getAgentBody().x:.2f}, {world.getAgentBody().y:.2f})")
    print(f"Final energy: {world.getAgentBody().energy:.2f}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    return brain, agent, world

# Main entry point for examples
def main():
    """Main entry point for command-line examples."""
    import argparse
    import sys
    
    parser = argparse.ArgumentParser(
        description="NLM Python Examples and Demos",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  pynlm-demo simple    # Run simple brain demo
  pynlm-demo agent     # Run agent world demo
  pynlm-demo episode   # Run a complete episode
  pynlm-demo info      # Show module information
        """
    )
    
    parser.add_argument('command', choices=['simple', 'agent', 'episode', 'info'],
                       help='Demo command to run')
    parser.add_argument('--steps', type=int, default=100,
                       help='Number of simulation steps')
    
    args = parser.parse_args()
    
    if args.command == 'info':
        print_banner()
    elif args.command == 'simple':
        demo_simple_brain()
    elif args.command == 'agent':
        demo_agent_world()
    elif args.command == 'episode':
        stats = run_episode(num_steps=args.steps)
        print(f"\nEpisode completed in {stats['steps']} steps:")
        print(f"  Total spikes: {stats['total_spikes']}")
        print(f"  Max firing neurons: {stats['max_firing']}")
        print(f"  Average firing rate: {sum(stats['firing_rates'])/len(stats['firing_rates']):.1f}")
        print(f"  Final position: {stats['final_position']}")
        print(f"  Final energy: {stats['final_energy']:.2f}")
        print(f"  Curiosity levels: min={min(stats['curiosity_levels']):.3f}, max={max(stats['curiosity_levels']):.3f}")

if __name__ == '__main__':
    main()