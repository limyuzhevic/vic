# NLM Python Utilities and Convenience Functions
"""
High-level utilities and convenience functions for NLM Python bindings.

This module provides user-friendly APIs for common neural simulation tasks,
making it easier for users to work with the NLM framework without needing
to understand all the low-level details.
"""

import pynlm
import json
import time
from typing import List, Dict, Optional, Tuple, Callable
from dataclasses import dataclass

@dataclass
class BrainConfiguration:
    """High-level brain configuration for easy setup."""
    neuron_count: int = 1000
    region_count: int = 5
    synaptic_density: float = 0.1
    excitation_inhibition_ratio: float = 1.0
    learning_rate: float = 0.01
    development_enabled: bool = True
    neuromodulation_enabled: bool = True

def create_simple_brain(config: Optional[BrainConfiguration] = None) -> 'pynlm.Brain':
    """
    Create a simple, well-configured brain for typical use cases.
    
    Args:
        config: Brain configuration options. If None, uses sensible defaults.
        
    Returns:
        Configured and initialized brain.
        
    Example:
        >>> brain = create_simple_brain()
        >>> print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    """
    if config is None:
        config = BrainConfiguration()
    
    # Create configuration
    cfg = pynlm.createDefaultConfig()
    
    # Set configuration parameters
    cfg.set("brain.neuron_count", config.neuron_count)
    cfg.set("brain.region_count", config.region_count)
    cfg.set("brain.synapse_density", config.synaptic_density)
    cfg.set("brain.ei_ratio", config.excitation_inhibition_ratio)
    cfg.set("plasticity.learning_rate", config.learning_rate)
    
    # Create and initialize brain
    brain = pynlm.createBrain(cfg)
    brain.initialize()
    
    return brain

def create_complete_agent(config: Optional[BrainConfiguration] = None) -> Tuple['pynlm.Brain', 'pynlm.AgentBrain', 'pynlm.SimpleWorld']:
    """
    Create a complete agent setup with brain, world, and agent interface.
    
    Args:
        config: Brain configuration. If None, uses defaults.
        
    Returns:
        Tuple of (brain, agent, world)
        
    Example:
        >>> brain, agent, world = create_complete_agent()
        >>> agent.initialize(world)
        >>> agent.enableRewardModulation(True)
        >>> agent.enableCuriosity(True)
    """
    if config is None:
        config = BrainConfiguration()
    
    # Create brain
    brain = create_simple_brain(config)
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    return brain, agent, world

class SimulationRunner:
    """High-level simulation runner with common patterns."""
    
    def __init__(self, brain: 'pynlm.Brain', world: Optional['pynlm.SimpleWorld'] = None, 
                 agent: Optional['pynlm.AgentBrain'] = None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.clock = pynlm.SimulationClock()
    
    def run_steps(self, num_steps: int, timestep: float = 0.1, verbose: bool = False) -> List[Dict]:
        """
        Run simulation for a given number of steps.
        
        Args:
            num_steps: Number of simulation steps
            timestep: Duration of each timestep
            verbose: Whether to print progress
            
        Returns:
            List of statistics dictionaries for each step
        """
        stats = []
        
        for step in range(num_steps):
            self.clock.step(timestep)
            
            # Process if agent and world are available
            if self.agent and self.world:
                # Get sensory input
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                # Run brain step
                self.brain.step(step)
                
                # Decode motor command
                motor_cmd = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(motor_cmd, self.clock.getTime())
                
                # Apply neuromodulation
                reward = self.getRewardFromPercept(percept)
                self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                self.agent.updateDevelopment(timestep)
            else:
                # Simple brain-only simulation
                self.brain.step(step)
            
            # Collect statistics
            step_stats = {
                'step': step,
                'time': self.clock.getTime(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'developmental_stage': self.brain.getDevelopmentalStage()
            }
            
            if self.agent:
                step_stats.update({
                    'curiosity_level': self.agent.getCuriosityLevel(),
                    'novelty_level': self.agent.getNoveltyLevel(),
                    'dopamine_level': self.agent.getNeuromodulationLevel()
                })
            
            stats.append(step_stats)
            
            if verbose and step % 100 == 0:
                print(f"Step {step}: {step_stats['active_neurons']} active neurons, "
                      f"{step_stats['average_firing_rate']:.2f} Hz firing rate")
        
        return stats
    
    def getRewardFromPercept(self, percept: 'pynlm.SensoryPercept') -> float:
        """Extract reward from sensory percept (customize as needed)."""
        # Default: use internal signal 0 as reward
        internal = percept.getInternal()
        return internal[0] if internal else 0.0

class NeuralNetworkVisualizer:
    """Utilities for visualizing neural network state."""
    
    @staticmethod
    def create_neurons_grid(brain: 'pynlm.Brain', rows: int = 10, cols: int = 10) -> List[List['pynlm.NeuronId']]:
        """
        Create a grid of neurons for visualization.
        
        Args:
            brain: The brain to get neurons from
            rows: Number of rows in grid
            cols: Number of columns in grid
            
        Returns:
            2D list of neuron IDs
        """
        neurons = brain.getTotalNeuronCount()
        grid_size = rows * cols
        
        if neurons < grid_size:
            raise ValueError(f"Brain has only {neurons} neurons, need at least {grid_size}")
        
        # Get all neuron IDs
        neuron_ids = []
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            # Note: This is a simplified approach - real implementation would need
            # access to all neurons in the system
        
        # For now, return a placeholder grid
        return [[None for _ in range(cols)] for _ in range(rows)]
    
    @staticmethod
    def analyze_connectivity(brain: 'pynlm.Brain') -> Dict:
        """
        Analyze network connectivity patterns.
        
        Args:
            brain: The brain to analyze
            
        Returns:
            Dictionary with connectivity statistics
        """
        stats = {
            'total_synapses': brain.getTotalSynapseCount(),
            'total_neurons': brain.getTotalNeuronCount(),
            'synapse_density': 0.0,
            'region_count': brain.getRegionCount(),
            'region_sizes': [],
            'connectivity_patterns': {}
        }
        
        if stats['total_neurons'] > 0:
            stats['synapse_density'] = stats['total_synapses'] / (
                stats['total_neurons'] * stats['total_neurons'])
        
        # Analyze region sizes
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            stats['region_sizes'].append({
                'id': region_id.value,
                'neuron_count': region.getTotalNeuronCount(),
                'synapse_count': region.getSynapseCount()
            })
        
        return stats

class LearningCurveAnalyzer:
    """Analyze learning curves and performance metrics."""
    
    def __init__(self):
        self.metrics_history = []
    
    def add_metrics(self, step: int, metrics: Dict):
        """Add metrics for a simulation step."""
        self.metrics_history.append({
            'step': step,
            'timestamp': time.time(),
            **metrics
        })
    
    def compute_learning_curve(self, metric: str, window: int = 100) -> List[Dict]:
        """
        Compute learning curve for a specific metric.
        
        Args:
            metric: Metric name to analyze
            window: Moving average window size
            
        Returns:
            List of dictionaries with smoothed metric values
        """
        if not self.metrics_history:
            return []
        
        curve_data = []
        
        for i in range(len(self.metrics_history)):
            # Calculate moving average
            start_idx = max(0, i - window + 1)
            window_data = self.metrics_history[start_idx:i + 1]
            
            if metric in window_data[0]:
                values = [d[metric] for d in window_data if metric in d]
                avg_value = sum(values) / len(values)
                
                curve_data.append({
                    'step': self.metrics_history[i]['step'],
                    'value': avg_value,
                    'raw_value': self.metrics_history[i].get(metric)
                })
        
        return curve_data
    
    def get_performance_summary(self) -> Dict:
        """Get summary of performance metrics."""
        if not self.metrics_history:
            return {}
        
        last_metrics = self.metrics_history[-1]
        
        return {
            'final_step': last_metrics['step'],
            'final_active_neurons': last_metrics.get('active_neurons', 0),
            'final_firing_rate': last_metrics.get('average_firing_rate', 0),
            'final_e_i_ratio': last_metrics.get('e_i_ratio', 0),
            'total_spikes': last_metrics.get('total_spikes', 0),
            'learning_efficiency': self.compute_efficiency()
        }
    
    def compute_efficiency(self) -> float:
        """Compute learning efficiency (normalized metric change)."""
        if len(self.metrics_history) < 2:
            return 0.0
        
        initial = self.metrics_history[0].get('average_firing_rate', 0)
        final = self.metrics_history[-1].get('average_firing_rate', 0)
        
        if initial == 0:
            return final
        
        return (final - initial) / initial

def benchmark_configuration(config_name: str, num_steps: int = 1000) -> Dict:
    """
    Benchmark a specific brain configuration.
    
    Args:
        config_name: Name of configuration to benchmark
        num_steps: Number of simulation steps to run
        
    Returns:
        Benchmark results
    """
    configs = {
        'minimal': BrainConfiguration(neuron_count=100, region_count=2),
        'small': BrainConfiguration(neuron_count=500, region_count=3),
        'medium': BrainConfiguration(neuron_count=1000, region_count=5),
        'large': BrainConfiguration(neuron_count=2000, region_count=8),
        'complex': BrainConfiguration(neuron_count=5000, region_count=12, synaptic_density=0.2)
    }
    
    if config_name not in configs:
        raise ValueError(f"Unknown configuration: {config_name}")
    
    config = configs[config_name]
    
    # Create brain
    brain = create_simple_brain(config)
    
    # Run benchmark
    runner = SimulationRunner(brain)
    stats = runner.run_steps(num_steps, verbose=False)
    
    # Calculate performance metrics
    final_stats = stats[-1]
    initial_stats = stats[0]
    
    duration = num_steps * 0.1  # Assuming 0.1 timestep
    
    return {
        'configuration': config_name,
        'neuron_count': config.neuron_count,
        'region_count': config.region_count,
        'steps_per_second': num_steps / duration,
        'final_firing_rate': final_stats['average_firing_rate'],
        'firing_rate_change': final_stats['average_firing_rate'] - initial_stats['average_firing_rate'],
        'final_e_i_ratio': final_stats['e_i_ratio'],
        'total_spikes': final_stats['total_spikes'],
        'active_neuron_ratio': final_stats['active_neurons'] / config.neuron_count,
        'simulation_time': duration,
        'performance_score': (final_stats['average_firing_rate'] * 
                             (1.0 / final_stats['e_i_ratio']) * 
                             (final_stats['active_neurons'] / config.neuron_count))
    }

def export_brain_state(brain: 'pynlm.Brain', filepath: str):
    """
    Export brain state to JSON file.
    
    Args:
        brain: Brain to export
        filepath: Output file path
    """
    # Get comprehensive brain state
    state = {
        'timestamp': time.time(),
        'brain_info': {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'active_neuron_count': brain.getActiveNeuronCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'total_spike_count': brain.getTotalSpikeCount(),
            'developmental_stage': brain.getDevelopmentalStage()
        },
        'region_info': []
    }
    
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        state['region_info'].append({
            'id': region_id.value,
            'name': region.getName(),
            'neuron_count': region.getTotalNeuronCount(),
            'synapse_count': region.getSynapseCount()
        })
    
    # Save to file
    with open(filepath, 'w') as f:
        json.dump(state, f, indent=2)
    
    print(f"Brain state exported to {filepath}")

def import_brain_state(filepath: str) -> 'pynlm.Brain':
    """
    Import brain state from JSON file.
    
    Args:
        filepath: Input file path
        
    Returns:
        New brain with imported state (note: full state restoration may require
        additional functionality beyond current bindings)
    """
    with open(filepath, 'r') as f:
        state = json.load(f)
    
    # Create new brain (note: current bindings don't support full state restoration)
    brain = pynlm.createDefaultConfig()
    # In a full implementation, this would restore the brain state
    
    print(f"Brain state imported from {filepath} (partial implementation)")
    return brain

# High-level example patterns

def example_basic_neural_network():
    """Example: Create and run a basic neural network."""
    brain = create_simple_brain()
    runner = SimulationRunner(brain)
    
    print("Running basic neural network simulation...")
    stats = runner.run_steps(500, verbose=True)
    
    return stats

def example_complete_agent_simulation():
    """Example: Complete agent with brain, world, and learning."""
    brain, agent, world = create_complete_agent()
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Running complete agent simulation...")
    runner = SimulationRunner(brain, world, agent)
    stats = runner.run_steps(1000, verbose=True)
    
    return stats, agent

def example_brain_training():
    """Example: Train brain with different configurations."""
    configurations = ['minimal', 'small', 'medium']
    results = {}
    
    for config_name in configurations:
        print(f"Benchmarking configuration: {config_name}")
        results[config_name] = benchmark_configuration(config_name, num_steps=500)
    
    # Compare results
    best_config = max(results.keys(), key=lambda k: results[k]['performance_score'])
    print(f"Best configuration: {best_config} (score: {results[best_config]['performance_score']:.3f})")
    
    return results

# Export public API
__all__ = [
    'BrainConfiguration',
    'create_simple_brain',
    'create_complete_agent',
    'SimulationRunner',
    'NeuralNetworkVisualizer',
    'LearningCurveAnalyzer',
    'benchmark_configuration',
    'export_brain_state',
    'import_brain_state',
    'example_basic_neural_network',
    'example_complete_agent_simulation',
    'example_brain_training'
]
