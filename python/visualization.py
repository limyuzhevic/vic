# NLM Visualization Python Module

"""
Visualization module for NLM Python interface
"""

from . import __init__ as nlm_init
from typing import List, Optional, Union, Any, Dict, Tuple
from pathlib import Path
import matplotlib.pyplot as plt
import numpy as np

# Re-export from main module
Brain = nlm_init.Brain
Config = nlm_init.Config
createBrain = nlm_init.createBrain
createDefaultConfig = nlm_init.createDefaultConfig
SensoryPercept = nlm_init.SensoryPercept
Vision = nlm_init.Vision
Audio = nlm_init.Audio
AgentBody = nlm_init.AgentBody
ActionResult = nlm_init.ActionResult
Action = nlm_init.Action
ActionType = nlm_init.ActionType
MotorCommand = nlm_init.MotorCommand
WorldObject = nlm_init.WorldObject
WorldObjectType = nlm_init.WorldObjectType

# Plotting utilities
class NeuralActivityPlotter:
    """Plot neural activity and statistics"""
    
    @staticmethod
    def plot_activity(brain: Brain, duration: int = 100) -> None:
        """
        Plot neural activity over time
        
        Args:
            brain: Brain object
            duration: Number of steps to simulate
        """
        # Simulate if needed
        for step in range(duration):
            brain.step(step)
        
        # Collect statistics
        spikes = []
        firing_rates = []
        
        for step in range(duration):
            spikes.append(brain.getTotalSpikeCount())
            firing_rates.append(brain.getAverageFiringRate())
        
        # Create plot
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
        
        # Spike count plot
        ax1.plot(spikes)
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Total Spikes')
        ax1.set_title('Neural Activity Over Time')
        ax1.grid(True)
        
        # Firing rate plot
        ax2.plot(firing_rates)
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Average Firing Rate')
        ax2.set_title('Average Firing Rate Over Time')
        ax2.grid(True)
        
        plt.tight_layout()
        plt.show()
    
    @staticmethod
    def plot_neuron_distribution(brain: Brain) -> None:
        """
        Plot neuron type distribution
        
        Args:
            brain: Brain object
        """
        # Get neuron type counts
        # This is a placeholder - actual implementation would need
        # access to neuron type information
        
        neuron_types = ['Excitatory', 'Inhibitory', 'Sensory', 'Motor', 'Internal']
        counts = [brain.getTotalNeuronCount() // 2] * len(neuron_types)
        
        plt.figure(figsize=(8, 6))
        bars = plt.bar(neuron_types, counts, color=['red', 'blue', 'green', 'orange', 'purple'])
        plt.title('Neuron Type Distribution')
        plt.ylabel('Count')
        
        # Add value labels on bars
        for bar in bars:
            height = bar.get_height()
            plt.text(bar.get_x() + bar.get_width()/2., height,
                    f'{int(height)}', ha='center', va='bottom')
        
        plt.tight_layout()
        plt.show()


class MemoryUsagePlotter:
    """Plot memory usage and statistics"""
    
    @staticmethod
    def plot_memory_usage(brain: Brain, duration: int = 100) -> None:
        """
        Plot memory usage over time
        
        Args:
            brain: Brain object
            duration: Number of steps to simulate
        """
        # Simulate if needed
        for step in range(duration):
            brain.step(step)
        
        # Collect memory statistics
        # This is a placeholder - actual implementation would need
        # access to memory system statistics
        
        steps = list(range(duration))
        working_memory = np.random.random(duration) * 100
        episodic_memory = np.random.random(duration) * 50
        semantic_memory = np.random.random(duration) * 30
        
        plt.figure(figsize=(10, 6))
        plt.plot(steps, working_memory, label='Working Memory')
        plt.plot(steps, episodic_memory, label='Episodic Memory')
        plt.plot(steps, semantic_memory, label='Semantic Memory')
        
        plt.xlabel('Step')
        plt.ylabel('Memory Size')
        plt.title('Memory Usage Over Time')
        plt.legend()
        plt.grid(True)
        plt.tight_layout()
        plt.show()


class BrainStateVisualizer:
    """Visualize brain state and connections"""
    
    @staticmethod
    def create_connection_matrix(brain: Brain) -> None:
        """
        Create a connection matrix visualization
        
        Args:
            brain: Brain object
        """
        # This is a placeholder - actual implementation would need
        # access to connection information
        
        # Get number of regions
        num_regions = brain.getRegionCount()
        
        # Create random connection matrix
        np.random.seed(42)
        connections = np.random.random((num_regions, num_regions))
        
        plt.figure(figsize=(8, 6))
        plt.imshow(connections, cmap='hot', interpolation='nearest')
        plt.colorbar()
        plt.title('Connection Matrix (Region to Region)')
        plt.xlabel('Target Region')
        plt.ylabel('Source Region')
        
        plt.tight_layout()
        plt.show()


class ExperimentVisualizer:
    """Visualize experiment results"""
    
    @staticmethod
    def plot_experiment_results(results: Dict[str, Any]) -> None:
        """
        Plot experiment results
        
        Args:
            results: Dictionary of experiment results
        """
        if 'steps' not in results or 'total_spikes' not in results:
            print("Invalid experiment results format")
            return
        
        steps = results['steps']
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('Experiment Results')
        
        # Total spikes
        axes[0, 0].plot(steps, results['total_spikes'])
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Total Spikes')
        axes[0, 0].set_title('Spike Count Over Time')
        axes[0, 0].grid(True)
        
        # Firing rate
        axes[0, 1].plot(steps, results['firing_rates'])
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Firing Rate')
        axes[0, 1].set_title('Firing Rate Over Time')
        axes[0, 1].grid(True)
        
        # Reward
        if 'rewards' in results:
            axes[1, 0].plot(steps[:len(results['rewards'])], results['rewards'])
            axes[1, 0].set_xlabel('Step')
            axes[1, 0].set_ylabel('Reward')
            axes[1, 0].set_title('Reward Over Time')
            axes[1, 0].grid(True)
        
        # Curiosity level
        if 'curiosity_levels' in results:
            axes[1, 1].plot(steps[:len(results['curiosity_levels'])], results['curiosity_levels'])
            axes[1, 1].set_xlabel('Step')
            axes[1, 1].set_ylabel('Curiosity Level')
            axes[1, 1].set_title('Curiosity Level Over Time')
            axes[1, 1].grid(True)
        
        plt.tight_layout()
        plt.show()


class InteractiveBrainVisualizer:
    """Interactive brain visualization"""
    
    def __init__(self):
        self._brain = None
        self._current_step = 0
        self._history = []
    
    def set_brain(self, brain: Brain) -> None:
        """Set the brain to visualize"""
        self._brain = brain
        self._current_step = 0
        self._history = []
    
    def update(self) -> None:
        """Update visualization with next step"""
        if self._brain:
            self._brain.step(self._current_step)
            self._history.append(self._get_current_state())
            self._current_step += 1
    
    def _get_current_state(self) -> Dict[str, Any]:
        """Get current brain state"""
        return {
            'step': self._current_step,
            'spikes': self._brain.getTotalSpikeCount(),
            'firing_rate': self._brain.getAverageFiringRate(),
            'neurons': self._brain.getTotalNeuronCount(),
            'synapses': self._brain.getTotalSynapseCount(),
        }
    
    def plot_history(self) -> None:
        """Plot brain history"""
        if not self._history:
            print("No history to plot")
            return
        
        steps = [state['step'] for state in self._history]
        spikes = [state['spikes'] for state in self._history]
        firing_rates = [state['firing_rate'] for state in self._history]
        
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
        
        ax1.plot(steps, spikes)
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Total Spikes')
        ax1.set_title('Brain Activity History')
        ax1.grid(True)
        
        ax2.plot(steps, firing_rates)
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Firing Rate')
        ax2.set_title('Firing Rate History')
        ax2.grid(True)
        
        plt.tight_layout()
        plt.show()


# Quick visualization functions
def plot_brain_activity(brain: Brain, duration: int = 100) -> None:
    """Quick function to plot brain activity"""
    NeuralActivityPlotter.plot_activity(brain, duration)


def plot_memory_usage(brain: Brain, duration: int = 100) -> None:
    """Quick function to plot memory usage"""
    MemoryUsagePlotter.plot_memory_usage(brain, duration)


def visualize_experiment(results: Dict[str, Any]) -> None:
    """Quick function to visualize experiment results"""
    ExperimentVisualizer.plot_experiment_results(results)


__all__ = [
    "NeuralActivityPlotter", "MemoryUsagePlotter", "BrainStateVisualizer",
    "ExperimentVisualizer", "InteractiveBrainVisualizer",
    "plot_brain_activity", "plot_memory_usage", "visualize_experiment"
]
