# Core brain and neural network classes for NLM

"""
Brain and neural network classes for NLM (Neural Learning Machine).

This module provides the core classes for neural simulation including neurons,
synapses, regions, populations, and the main Brain class that orchestrates
the entire neural network simulation.
"""

import numpy as np
import time
from typing import List, Dict, Optional, Any, Tuple
from enum import Enum

# Import C++ classes
from ._cpp import (
    Brain, Neuron, Synapse, Region, Population,
    NeuronType, SynapseType, ActionType, DevelopmentalStage,
    FiringState, INVALID_NEURON_ID, INVALID_SYNAPSE_ID,
    INVALID_REGION_ID, INVALID_POPULATION_ID
)

# Re-export commonly used types
from ._cpp import (
    NeuronId, SynapseId, RegionId, PopulationId,
    LogLevel, ConfigSource, ConfigValue
)

__all__ = [
    # Core classes
    'Brain', 'Neuron', 'Synapse', 'Region', 'Population',
    # Types
    'NeuronType', 'SynapseType', 'ActionType', 'DevelopmentalStage',
    'FiringState', 'MotorCommand', 'WorldObjectType',
    # Identifiers
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    # Configuration
    'Config', 'ConfigEntry', 'ConfigSource', 'ConfigValue',
    # Logging
    'Logger', 'LogLevel', 'LogEntry', 'LogStream',
    # Sensory and motor
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'ActionResult', 'SensoryPercept',
    # World
    'WorldObject', 'SimpleWorld',
    # Agent
    'AgentBody', 'AgentBrain',
    # Experiment
    'Experiment', 'ExperimentRunner', 'Phase6IntegratedExperiment',
    # Performance
    'PerformanceMetrics', 'CheckpointSystem', 'MemoryPool',
    # Constants
    'PI', 'E', 'FLOAT_EPSILON',
    # Constants IDs
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    # Utility functions
    'createBrain', 'createSimpleWorld', 'createAgentBrain',
    'createDefaultConfig', 'createMinimalConfig', 'createAgentSystem',
    'getVersion', 'setLogLevel',
]


class BrainSimulator:
    """
    High-level interface for brain simulation and experiment running.

    This class provides a convenient interface for running complex simulations,
    managing experiments, and analyzing results.
    """

    def __init__(self, config=None):
        """
        Initialize a brain simulator.

        Args:
            config (Config, optional): Configuration for the brain.
                                     If None, uses default configuration.
        """
        self.config = config or createDefaultConfig()
        self.brain = createBrain(self.config)
        self.experiments = []
        self.results = []

    def initialize(self, **kwargs):
        """
        Initialize the brain with configuration.

        Args:
            **kwargs: Additional configuration options.
        """
        # Apply any additional configuration
        for key, value in kwargs.items():
            if hasattr(self.config, 'set'):
                self.config.set(key, value)
        
        self.brain.initialize()

    def step(self, step_num, time=None):
        """
        Advance the simulation by one step.

        Args:
            step_num (int): Current simulation step number.
            time (float, optional): Current simulation time.
        """
        if time is None:
            time = step_num * self.config.getFloat("simulation_timestep", 0.001)
        
        self.brain.step(step_num, time)

    def injectCurrent(self, neuron_id, current):
        """
        Inject current into a specific neuron.

        Args:
            neuron_id (NeuronId): ID of the neuron to inject current into.
            current (float): Current value to inject.
        """
        self.brain.injectCurrent(neuron_id, current)

    def injectCurrentToType(self, neuron_type, current):
        """
        Inject current into all neurons of a specific type.

        Args:
            neuron_type (NeuronType): Type of neurons to inject current into.
            current (float): Current value to inject.
        """
        self.brain.injectCurrentToNeurons(neuron_type, current)

    def getState(self):
        """
        Get the current state of the brain.

        Returns:
            dict: Dictionary containing brain state information.
        """
        return {
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'excitatory_inhibitory_ratio': self.brain.getExcitationInhibitionRatio(),
            'developmental_stage': self.brain.getDevelopmentalStage(),
            'working_memory_active': self.brain.getWorkingMemory() is not None,
            'episodic_memory_active': self.brain.getEpisodicMemory() is not None,
            'dopamine_active': self.brain.getDopamine() is not None,
            'curiosity_active': self.brain.getCuriosity() is not None,
        }

    def runSimulation(self, steps, input_pattern=None, verbose=False):
        """
        Run a simulation with specified number of steps.

        Args:
            steps (int): Number of simulation steps to run.
            input_pattern (dict, optional): Input pattern configuration.
            verbose (bool): Whether to print progress information.

        Returns:
            list: List of state dictionaries for each step.
        """
        states = []
        
        for step in range(steps):
            # Apply input pattern if specified
            if input_pattern:
                self._applyInputPattern(input_pattern, step)
            
            # Take simulation step
            self.step(step)
            
            # Record state
            state = self.getState()
            state['step'] = step
            states.append(state)
            
            if verbose and step % 100 == 0:
                print(f"Step {step}/{steps}: "
                      f"Firing rate: {state['average_firing_rate']:.2f}, "
                      f"Total spikes: {state['total_spikes']}")
        
        self.results.extend(states)
        return states

    def _applyInputPattern(self, pattern, step):
        """Apply input pattern based on step number."""
        pattern_type = pattern.get('type', 'sensory')
        base_current = pattern.get('current', 10.0)
        
        if pattern_type == 'sensory':
            self.injectCurrentToType(NeuronType.Sensory, base_current)
        elif pattern_type == 'excitatory':
            self.injectCurrentToType(NeuronType.Excitatory, base_current)
        elif pattern_type == 'inhibitory':
            self.injectCurrentToType(NeuronType.Inhibitory, base_current)
        elif pattern_type == 'internal':
            self.injectCurrentToType(NeuronType.Internal, base_current)
        elif pattern_type == 'modulatory':
            self.injectCurrentToType(NeuronType.Modulatory, base_current)

    def runLearningExperiment(self, learning_config):
        """
        Run a learning experiment.

        Args:
            learning_config (dict): Configuration for learning experiment.

        Returns:
            dict: Results of the learning experiment.
        """
        # Reset brain for learning
        self.brain.reset()
        self.brain.initialize()
        
        # Configure learning parameters
        if 'plasticity_learning_rate' in learning_config:
            self.config.setFloat('plasticity_learning_rate', 
                               learning_config['plasticity_learning_rate'])
        
        if 'stdp_ltp_weight' in learning_config:
            self.config.setFloat('stdp_ltp_weight', 
                               learning_config['stdp_ltp_weight'])
        
        # Run learning steps
        steps = learning_config.get('steps', 1000)
        results = {'steps': [], 'final_state': None}
        
        for step in range(steps):
            # Generate input based on pattern
            pattern = learning_config.get('input_pattern', 'mixed')
            if pattern == 'sensory':
                self.injectCurrentToType(NeuronType.Sensory, 10.0)
            elif pattern == 'excitatory':
                self.injectCurrentToType(NeuronType.Excitatory, 5.0)
            elif pattern == 'inhibitory':
                self.injectCurrentToType(NeuronType.Inhibitory, 3.0)
            elif pattern == 'internal':
                self.injectCurrentToType(NeuronType.Internal, 2.0)
            else:  # mixed
                # Random pattern
                import random
                patterns = ['sensory', 'excitatory', 'inhibitory', 'internal']
                chosen = random.choice(patterns)
                current = random.uniform(2.0, 10.0)
                self.injectCurrentToType(
                    getattr(NeuronType, chosen.capitalize()), current
                )
            
            # Take step
            self.step(step)
            
            # Record state
            state = self.getState()
            state['step'] = step
            results['steps'].append(state)
        
        # Get final state
        results['final_state'] = self.getState()
        
        return results

    def saveCheckpoint(self, filepath):
        """
        Save brain state to a checkpoint file.

        Args:
            filepath (str): Path to save the checkpoint file.

        Returns:
            bool: True if successful, False otherwise.
        """
        return self.brain.save(filepath)

    def loadCheckpoint(self, filepath):
        """
        Load brain state from a checkpoint file.

        Args:
            filepath (str): Path to the checkpoint file.

        Returns:
            bool: True if successful, False otherwise.
        """
        return self.brain.load(filepath)

    def logStatus(self):
        """Log the current status of the brain."""
        self.brain.logStatus()


# Pythonic convenience functions

def createBrain(config=None):
    """
    Create a new brain with the specified configuration.

    This is a convenience function that wraps the Brain constructor
    and provides a simple interface for brain creation.

    Args:
        config (Config, optional): Configuration for the brain.
                                 If None, creates a default configuration.

    Returns:
        Brain: A new brain instance.

    Example:
        >>> import pynlm
        >>> brain = pynlm.createBrain()
        >>> brain.initialize()
        >>> brain.step(0)
    """
    if config is None:
        config = createDefaultConfig()
    
    return Brain(config)


def createSimpleWorld():
    """
    Create a new simple world for simulation.

    This is a convenience function that wraps the SimpleWorld constructor
    and provides a simple interface for world creation.

    Returns:
        SimpleWorld: A new world instance.

    Example:
        >>> import pynlm
        >>> world = pynlm.createSimpleWorld()
        >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    """
    return SimpleWorld()


def createAgentBrain(brain):
    """
    Create an agent brain interface for a given brain.

    This is a convenience function that wraps the AgentBrain constructor
    and provides a simple interface for agent creation.

    Args:
        brain (Brain): The brain to create an interface for.

    Returns:
        AgentBrain: An agent brain interface instance.

    Example:
        >>> import pynlm
        >>> brain = pynlm.createBrain()
        >>> agent = pynlm.createAgentBrain(brain)
    """
    return AgentBrain(brain)


def createDefaultConfig():
    """
    Create a default configuration with sensible defaults.

    This function creates a configuration with typical values for
    starting a simulation.

    Returns:
        Config: A configuration instance with default values.

    Example:
        >>> import pynlm
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
    """
    return Config()


def createMinimalConfig():
    """
    Create a minimal configuration with basic settings.

    This function creates a configuration with only the essential
    settings needed for a basic simulation.

    Returns:
        Config: A minimal configuration instance.

    Example:
        >>> import pynlm
        >>> config = pynlm.createMinimalConfig()
        >>> brain = pynlm.createBrain(config)
    """
    config = Config()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1)
    config.set("simulation_timestep", 0.001)
    config.set("random_seed", 42)
    return config


def createAgentSystem():
    """
    Create a complete agent system with brain, world, and agent.

    This is a convenience function that creates a complete agent system
    with default configuration.

    Returns:
        tuple: (brain, world, agent) - A complete agent system.

    Example:
        >>> import pynlm
        >>> brain, world, agent = pynlm.createAgentSystem()
        >>> agent.initialize(world)
    """
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
    if level.upper() == 'DEBUG':
        Logger::setGlobalLevel(LogLevel.Debug)
    elif level.upper() == 'INFO':
        Logger::setGlobalLevel(LogLevel.Info)
    elif level.upper() == 'WARNING':
        Logger::setGlobalLevel(LogLevel.Warning)
    elif level.upper() == 'ERROR':
        Logger::setGlobalLevel(LogLevel.Error)


def analyzeSimulation(results, output_file=None):
    """
    Analyze simulation results and generate statistics.

    Args:
        results (list): List of simulation state dictionaries.
        output_file (str, optional): File to save analysis results.

    Returns:
        dict: Analysis results including statistics and metrics.
    """
    analysis = {
        'total_steps': len(results),
        'statistics': {},
        'patterns': {},
        'trends': {}
    }
    
    if not results:
        return analysis
    
    # Basic statistics
    for key in ['firing_rate', 'total_spikes', 'average_firing_rate',
                'excitatory_inhibitory_ratio']:
        values = [r.get(key) for r in results if key in r and r[key] is not None]
        if values:
            analysis['statistics'][key] = {
                'mean': sum(values) / len(values),
                'min': min(values),
                'max': max(values),
                'std': (sum((x - sum(values)/len(values))**2 for x in values) / len(values))**0.5
            }
    
    # Find patterns
    if 'firing_rate' in results[0]:
        high_firing = sum(1 for r in results if r['firing_rate'] > 1.0)
        low_firing = sum(1 for r in results if r['firing_rate'] <= 1.0)
        analysis['patterns']['firing_distribution'] = {
            'high_firing': high_firing,
            'low_firing': low_firing,
            'high_percentage': (high_firing / len(results)) * 100
        }
    
    # Calculate trends (simple linear regression)
    if 'step' in results[0] and 'firing_rate' in results[0]:
        steps = [r['step'] for r in results]
        rates = [r['firing_rate'] for r in results]
        
        # Simple trend calculation
        if len(steps) > 1:
            x_mean = sum(steps) / len(steps)
            y_mean = sum(rates) / len(rates)
            
            numerator = sum((x - x_mean) * (y - y_mean) for x, y in zip(steps, rates))
            denominator = sum((x - x_mean)**2 for x in steps)
            
            if denominator != 0:
                slope = numerator / denominator
                analysis['trends']['firing_rate'] = {
                    'slope': slope,
                    'direction': 'increasing' if slope > 0 else 'decreasing'
                }
    
    # Save analysis if requested
    if output_file:
        import json
        with open(output_file, 'w') as f:
            json.dump(analysis, f, indent=2)
    
    return analysis


def plotResults(results, output_file=None, show_plot=False):
    """
    Plot simulation results.

    Args:
        results (list): List of simulation state dictionaries.
        output_file (str, optional): File to save the plot.
        show_plot (bool): Whether to display the plot.

    Returns:
        matplotlib.figure: The matplotlib figure object.
    """
    try:
        import matplotlib.pyplot as plt
        
        if not results:
            print("No results to plot")
            return None
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('NLM Simulation Results', fontsize=16)
        
        # Plot 1: Firing rate over time
        if 'step' in results[0] and 'firing_rate' in results[0]:
            steps = [r['step'] for r in results]
            rates = [r['firing_rate'] for r in results]
            
            axes[0, 0].plot(steps, rates, 'b-', linewidth=1)
            axes[0, 0].set_xlabel('Step')
            axes[0, 0].set_ylabel('Firing Rate')
            axes[0, 0].set_title('Firing Rate Over Time')
            axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Total spikes over time
        if 'step' in results[0] and 'total_spikes' in results[0]:
            steps = [r['step'] for r in results]
            spikes = [r['total_spikes'] for r in results]
            
            axes[0, 1].plot(steps, spikes, 'r-', linewidth=1)
            axes[0, 1].set_xlabel('Step')
            axes[0, 1].set_ylabel('Total Spikes')
            axes[0, 1].set_title('Total Spikes Over Time')
            axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Firing rate distribution
        if 'firing_rate' in results[0]:
            rates = [r['firing_rate'] for r in results]
            axes[1, 0].hist(rates, bins=50, alpha=0.7, color='green', density=True)
            axes[1, 0].set_xlabel('Firing Rate')
            axes[1, 0].set_ylabel('Density')
            axes[1, 0].set_title('Firing Rate Distribution')
            axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Neuronal statistics
        if all(key in results[0] for key in ['total_neurons', 'firing_neurons']):
            neurons = [r['total_neurons'] for r in results]
            firing = [r['firing_neurons'] for r in results]
            
            axes[1, 1].plot(steps, neurons, 'b-', label='Total Neurons', linewidth=2)
            axes[1, 1].plot(steps, firing, 'r-', label='Firing Neurons', linewidth=2)
            axes[1, 1].set_xlabel('Step')
            axes[1, 1].set_ylabel('Count')
            axes[1, 1].set_title('Neuronal Statistics')
            axes[1, 1].legend()
            axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            print(f"Plot saved to {output_file}")
        
        if show_plot:
            plt.show()
        
        return fig
        
    except ImportError:
        print("matplotlib not available - skipping plot generation")
        return None


def runLearningExperiment(brain, learning_config):
    """
    Run a learning experiment with the given brain.

    Args:
        brain (Brain): The brain to run the experiment on.
        learning_config (dict): Configuration for the learning experiment.

    Returns:
        dict: Results of the learning experiment.
    """
    # Reset brain
    brain.reset()
    brain.initialize()
    
    # Configure learning parameters
    for key, value in learning_config.items():
        if hasattr(brain.config, 'set'):
            brain.config.set(key, value)
    
    # Run learning simulation
    steps = learning_config.get('steps', 1000)
    results = {'steps': [], 'final_state': None}
    
    for step in range(steps):
        # Generate input based on pattern
        pattern = learning_config.get('input_pattern', 'mixed')
        if pattern == 'sensory':
            brain.injectCurrentToNeurons(NeuronType.Sensory, 10.0)
        elif pattern == 'excitatory':
            brain.injectCurrentToNeurons(NeuronType.Excitatory, 5.0)
        elif pattern == 'inhibitory':
            brain.injectCurrentToNeurons(NeuronType.Inhibitory, 3.0)
        elif pattern == 'internal':
            brain.injectCurrentToNeurons(NeuronType.Internal, 2.0)
        else:  # mixed
            # Simple random pattern
            import random
            current = random.uniform(2.0, 10.0)
            brain.injectCurrentToNeurons(
                random.choice(list(NeuronType)), current
            )
        
        brain.step(step)
        
        # Record state
        state = {
            'step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'excitatory_inhibitory_ratio': brain.getExcitationInhibitionRatio(),
        }
        results['steps'].append(state)
    
    # Get final state
    results['final_state'] = {
        'firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'excitatory_inhibitory_ratio': brain.getExcitationInhibitionRatio(),
    }
    
    return results


# Constants
PI = 3.14159265358979323846
E = 2.71828182845904523536
FLOAT_EPSILON = 1e-8

# Main entry point
def main():
    """
    Main entry point for the NLM Python interface.

    This function provides a command-line interface for NLM operations.
    """
    import argparse
    import sys
    
    parser = argparse.ArgumentParser(
        prog="pynlm",
        description="NLM (Neural Learning Machine) Python Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  pynlm create --neurons 500 --steps 1000
  pynlm run --config config.json --output results.json
  pynlm analyze --file results.json --plot
  pynlm learn --duration 300 --pattern sensory
        """
    )
    
    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Enable verbose output"
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Create command
    create_parser = subparsers.add_parser(
        "create",
        help="Create and run a simple brain simulation"
    )
    create_parser.add_argument(
        "--neurons", "-n",
        type=int,
        default=100,
        help="Number of neurons in the brain"
    )
    create_parser.add_argument(
        "--steps", "-s",
        type=int,
        default=100,
        help="Number of simulation steps"
    )
    create_parser.add_argument(
        "--output", "-o",
        help="Output file for simulation results"
    )
    
    # Run command
    run_parser = subparsers.add_parser(
        "run",
        help="Run an NLM simulation with custom configuration"
    )
    run_parser.add_argument(
        "--config", "-c",
        help="Configuration file path"
    )
    run_parser.add_argument(
        "--steps", "-s",
        type=int,
        default=500,
        help="Number of simulation steps"
    )
    run_parser.add_argument(
        "--output", "-o",
        help="Output file for results"
    )
    
    # Analyze command
    analyze_parser = subparsers.add_parser(
        "analyze",
        help="Analyze simulation results"
    )
    analyze_parser.add_argument(
        "--file", "-f",
        required=True,
        help="Results file to analyze"
    )
    analyze_parser.add_argument(
        "--plot", "-p",
        action="store_true",
        help="Generate plots"
    )
    
    # Learn command
    learn_parser = subparsers.add_parser(
        "learn",
        help="Run a learning experiment"
    )
    learn_parser.add_argument(
        "--duration", "-d",
        type=float,
        default=60.0,
        help="Learning duration in seconds"
    )
    learn_parser.add_argument(
        "--pattern", "-p",
        choices=["sensory", "excitatory", "inhibitory", "internal", "mixed"],
        default="mixed",
        help="Input pattern to use for learning"
    )
    learn_parser.add_argument(
        "--steps", "-s",
        type=int,
        default=1000,
        help="Number of learning steps"
    )
    learn_parser.add_argument(
        "--output", "-o",
        help="Output file for learning results"
    )
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    try:
        if args.command == "create":
            createSimulation(args)
        elif args.command == "run":
            runSimulation(args)
        elif args.command == "analyze":
            analyzeResults(args)
        elif args.command == "learn":
            runLearning(args)
    except KeyboardInterrupt:
        print("\nOperation cancelled by user")
        return 1
    except Exception as e:
        print(f"Error: {e}")
        if args.verbose:
            import traceback
            traceback.print_exc()
        return 1
    
    return 0


def createSimulation(args):
    """Create and run a simple brain simulation."""
    print(f"Creating simulation with {args.neurons} neurons for {args.steps} steps")
    
    # Create brain
    brain = createBrain()
    brain.config.setFloat("neuron_count", args.neurons)
    brain.initialize()
    
    # Run simulation
    for step in range(args.steps):
        # Inject some input
        if step % 4 == 0:
            brain.injectCurrentToNeurons(NeuronType.Sensory, 5.0)
        elif step % 4 == 1:
            brain.injectCurrentToNeurons(NeuronType.Excitatory, 3.0)
        
        brain.step(step)
    
    # Print results
    print(f"\nSimulation completed!")
    print(f"Final statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    # Save results if requested
    if args.output:
        import json
        results = {
            "neurons": args.neurons,
            "steps": args.steps,
            "final_stats": {
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "total_spikes": brain.getTotalSpikeCount(),
            }
        }
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Results saved to {args.output}")


def runSimulation(args):
    """Run an NLM simulation with custom configuration."""
    print(f"Running simulation with {args.steps} steps")
    
    # Create brain
    brain = createBrain()
    
    # Run simulation
    for step in range(args.steps):
        brain.step(step)
        
        if step % 500 == 0:
            print(f"Step {step}/{args.steps}")
    
    # Print final statistics
    print(f"\nSimulation completed!")
    print(f"Final statistics:")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Excitatory/inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")


def analyzeResults(args):
    """Analyze simulation results."""
    print(f"Analyzing results from {args.file}")
    
    try:
        import json
        
        with open(args.file, 'r') as f:
            results = json.load(f)
        
        print(f"Loaded {len(results) if isinstance(results, list) else 'unknown'} data points")
        
        # Basic analysis
        analysis = analyzeSimulation(results)
        print(f"\nAnalysis results:")
        print(f"  Total steps: {analysis['total_steps']}")
        
        if 'statistics' in analysis:
            print(f"  Firing rate mean: {analysis['statistics']['firing_rate']['mean']:.2f}")
            print(f"  Total spikes: {analysis['statistics']['total_spikes']['total']}")
        
        # Generate plot if requested
        if args.plot:
            plotResults(results, output_file=f"{args.file}_analysis.png")
        
    except FileNotFoundError:
        print(f"File not found: {args.file}")
    except json.JSONDecodeError:
        print(f"Invalid JSON format in file: {args.file}")


def runLearning(args):
    """Run a learning experiment."""
    print(f"Running learning experiment with {args.pattern} pattern")
    print(f"Duration: {args.duration} seconds")
    print(f"Steps: {args.steps}")
    
    # Create brain
    brain = createBrain()
    
    # Configure learning parameters
    brain.config.setFloat("plasticity_learning_rate", 0.05)
    brain.config.setFloat("stdp_ltp_weight", 0.02)
    brain.config.setFloat("stdp_ltd_weight", 0.015)
    
    # Run learning simulation
    for step in range(args.steps):
        # Generate input based on pattern
        if args.pattern == "sensory":
            brain.injectCurrentToNeurons(NeuronType.Sensory, 10.0)
        elif args.pattern == "excitatory":
            brain.injectCurrentToNeurons(NeuronType.Excitatory, 5.0)
        elif args.pattern == "inhibitory":
            brain.injectCurrentToNeurons(NeuronType.Inhibitory, 3.0)
        elif args.pattern == "internal":
            brain.injectCurrentToNeurons(NeuronType.Internal, 2.0)
        else:  # mixed
            # Simple random pattern
            import random
            current = random.uniform(2.0, 10.0)
            brain.injectCurrentToNeurons(
                random.choice(list(NeuronType)), current
            )
        
        brain.step(step)
        
        if step % 200 == 0:
            print(f"Step {step}/{args.steps}: Firing rate = {brain.getAverageFiringRate():.2f}")
    
    # Print learning results
    print(f"\nLearning experiment completed!")
    print(f"Final learning statistics:")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Excitatory/inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    # Save results if requested
    if args.output:
        import json
        results = runLearningExperiment(brain, {
            'steps': args.steps,
            'input_pattern': args.pattern,
            'plasticity_learning_rate': 0.05,
            'stdp_ltp_weight': 0.02,
            'stdp_ltd_weight': 0.015
        })
        
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Learning results saved to {args.output}")


if __name__ == "__main__":
    sys.exit(main())