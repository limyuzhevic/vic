# Core NLM utilities and helper classes
# This file provides additional utility classes and helper functions

import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, List, Any, Optional, Callable, Union
from dataclasses import dataclass, field
import json
import time
import os

from . import pynlm

@dataclass
class NeuralNetworkStats:
    """Container for neural network statistics"""
    total_neurons: int = 0
    total_synapses: int = 0
    active_neurons: int = 0
    firing_neurons: int = 0
    average_firing_rate: float = 0.0
    excitation_inhibition_ratio: float = 0.0
    total_spikes: int = 0
    developmental_stage: pynlm.DevelopmentalStage = pynlm.DevelopmentalStage.Initial
    neuromodulation_level: float = 0.0
    curiosity_level: float = 0.0
    novelty_level: float = 0.0
    prediction_error: float = 0.0

@dataclass
class PerformanceMetrics:
    """Container for performance metrics"""
    simulation_time: float = 0.0
    steps_per_second: float = 0.0
    memory_usage: float = 0.0
    cpu_usage: float = 0.0
    learning_rate: float = 0.0
    reward_per_step: float = 0.0
    novelty_per_step: float = 0.0
    exploration_efficiency: float = 0.0

class NeuralNetworkAnalyzer:
    """
    Helper class for analyzing neural network data
    """
    
    @staticmethod
    def analyze_brain(brain: pynlm.Brain) -> NeuralNetworkStats:
        """Analyze brain state and return statistics"""
        stats = NeuralNetworkStats()
        
        try:
            stats.total_neurons = brain.getTotalNeuronCount()
            stats.total_synapses = brain.getTotalSynapseCount()
            stats.active_neurons = brain.getActiveNeuronCount()
            stats.firing_neurons = brain.getFiringNeuronCount()
            stats.average_firing_rate = brain.getAverageFiringRate()
            stats.excitation_inhibition_ratio = brain.getExcitationInhibitionRatio()
            stats.total_spikes = brain.getTotalSpikeCount()
            stats.developmental_stage = brain.getDevelopmentalStage()
            
            # Get neuromodulation levels
            agent_brain = pynlm.createAgentBrain(brain)
            stats.neuromodulation_level = agent_brain.getNeuromodulationLevel()
            stats.curiosity_level = agent_brain.getCuriosityLevel()
            stats.novelty_level = agent_brain.getNoveltyLevel()
            stats.prediction_error = agent_brain.getPredictionError()
            
        except Exception as e:
            raise pynlm.BrainError(f"Failed to analyze brain: {str(e)}")
        
        return stats
    
    @staticmethod
    def generate_spike_raster(neuron_ids: List[pynlm.NeuronId], 
                             spike_times: List[float],
                             max_time: float = 100.0) -> np.ndarray:
        """Generate spike raster plot data"""
        # Create time bins
        time_bins = np.linspace(0, max_time, 1000)
        raster_data = np.zeros((len(neuron_ids), len(time_bins)))
        
        # Fill raster data
        for i, (neuron_id, time_) in enumerate(zip(neuron_ids, spike_times)):
            if 0 <= time_ <= max_time:
                time_bin = np.searchsorted(time_bins, time_)
                if time_bin < len(time_bins):
                    raster_data[i, time_bin] = 1
        
        return raster_data
    
    @staticmethod
    def calculate_synchronicity(spike_times: List[List[float]]) -> float:
        """Calculate synchronicity of spike trains"""
        if len(spike_times) < 2:
            return 1.0
        
        # Convert to numpy array
        spike_array = np.array(spike_times)
        
        # Calculate pairwise correlation
        correlations = []
        for i in range(len(spike_array)):
            for j in range(i + 1, len(spike_array)):
                correlation = np.corrcoef(spike_array[i], spike_array[j])[0, 1]
                if not np.isnan(correlation):
                    correlations.append(abs(correlation))
        
        return np.mean(correlations) if correlations else 0.0

class SimulationRunner:
    """
    Helper class for running NLM simulations with advanced features
    """
    
    def __init__(self, brain: pynlm.Brain, world: pynlm.SimpleWorld, 
                 config: Optional[pynlm.Config] = None):
        """Initialize simulation runner"""
        self.brain = brain
        self.world = world
        self.config = config or pynlm.createDefaultConfig()
        self.step_count = 0
        self.simulation_time = 0.0
        self.performance_metrics = PerformanceMetrics()
    
    def run_step(self, timestep: float = 0.001) -> Dict[str, Any]:
        """Run a single simulation step and return results"""
        step_start = time.time()
        
        # Get sensory input
        percept = self.world.getSensoryPercept()
        
        # Process input through brain
        self.brain.receiveSensoryInput(percept)
        
        # Update world
        self.world.update(timestep)
        
        # Produce action
        action = self.brain.produceAction()
        
        # Step brain
        self.brain.step(self.step_count, self.simulation_time)
        
        # Update metrics
        self.step_count += 1
        self.simulation_time += timestep
        
        step_end = time.time()
        step_duration = step_end - step_start
        
        # Collect results
        results = {
            'step': self.step_count - 1,
            'time': self.simulation_time,
            'step_duration': step_duration,
            'action': action,
            'spike_count': self.brain.getTotalSpikeCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
        }
        
        # Update performance metrics
        self.performance_metrics.simulation_time = self.simulation_time
        self.performance_metrics.steps_per_second = self.step_count / self.simulation_time if self.simulation_time > 0 else 0
        self.performance_metrics.learning_rate = self.brain.getAverageFiringRate()
        
        return results
    
    def run_simulation(self, steps: int, timestep: float = 0.001,
                      callback: Optional[Callable[[Dict[str, Any]], None]] = None) -> List[Dict[str, Any]]:
        """Run simulation for specified number of steps"""
        results = []
        
        for i in range(steps):
            step_result = self.run_step(timestep)
            results.append(step_result)
            
            if callback:
                callback(step_result)
        
        return results
    
    def get_performance_summary(self) -> PerformanceMetrics:
        """Get performance metrics summary"""
        return self.performance_metrics
    
    def save_brain_state(self, filepath: str) -> bool:
        """Save brain state to file"""
        return self.brain.save(filepath)
    
    def load_brain_state(self, filepath: str) -> bool:
        """Load brain state from file"""
        return self.brain.load(filepath)

class VisualizationTools:
    """
    Visualization and plotting tools for NLM data
    """
    
    @staticmethod
    def plot_neural_activity(brain: pynlm.Brain, 
                            save_path: Optional[str] = None,
                            show: bool = False) -> plt.Figure:
        """Plot neural activity statistics"""
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Total neurons
        axes[0, 0].bar(['Neurons', 'Synapses'], 
                      [brain.getTotalNeuronCount(), brain.getTotalSynapseCount()])
        axes[0, 0].set_title('Neural Network Structure')
        axes[0, 0].set_ylabel('Count')
        
        # Active vs Firing neurons
        axes[0, 1].pie([brain.getActiveNeuronCount(), brain.getFiringNeuronCount()],
                      labels=['Active', 'Firing'], autopct='%1.1f%%')
        axes[0, 1].set_title('Neuron Activity')
        
        # Firing rate over time (placeholder)
        axes[1, 0].plot([0, 1], [brain.getAverageFiringRate(), brain.getAverageFiringRate()])
        axes[1, 0].set_title('Average Firing Rate')
        axes[1, 0].set_ylabel('Hz')
        
        # Excitation/Inhibition ratio
        ratio = brain.getExcitationInhibitionRatio()
        axes[1, 1].bar(['E/I Ratio'], [ratio])
        axes[1, 1].set_title('Excitation/Inhibition Ratio')
        axes[1, 1].set_ylim(0, 2)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        if show:
            plt.show()
        
        return fig
    
    @staticmethod
    def plot_spike_histogram(spike_count: int, filename: str = 'spike_histogram.png') -> plt.Figure:
        """Plot spike count histogram"""
        fig, ax = plt.subplots(figsize=(8, 6))
        
        # Create histogram data
        bins = np.logspace(1, 6, 50)
        ax.hist([spike_count], bins=bins, alpha=0.7, color='skyblue', edgecolor='black')
        ax.set_xscale('log')
        ax.set_xlabel('Spike Count')
        ax.set_ylabel('Frequency')
        ax.set_title(f'Spike Count Distribution: {spike_count}')
        ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(filename, dpi=300, bbox_inches='tight')
        
        return fig
    
    @staticmethod
    def create_neural_connectivity_matrix(brain: pynlm.Brain, 
                                          save_path: Optional[str] = None) -> np.ndarray:
        """Create neural connectivity matrix (placeholder)"""
        # This is a placeholder - actual connectivity matrix would require
        # accessing synaptic connections from each region
        num_regions = brain.getRegionCount()
        connectivity_matrix = np.random.rand(num_regions, num_regions) * 0.1
        
        if save_path:
            np.savetxt(save_path, connectivity_matrix, fmt='%.6f')
        
        return connectivity_matrix

class LearningCurveAnalyzer:
    """
    Analyze learning curves and performance metrics
    """
    
    @staticmethod
    def calculate_learning_curve(rewards: List[float], 
                                window: int = 100) -> Dict[str, List[float]]:
        """Calculate learning curve metrics"""
        if len(rewards) < window:
            window = len(rewards)
        
        smoothed_rewards = []
        for i in range(len(rewards) - window + 1):
            window_rewards = rewards[i:i + window]
            smoothed_rewards.append(np.mean(window_rewards))
        
        # Calculate moving average
        moving_avg = np.convolve(rewards, np.ones(window)/window, mode='valid')
        
        return {
            'raw_rewards': rewards,
            'smoothed_rewards': smoothed_rewards,
            'moving_average': moving_avg.tolist(),
            'mean_reward': np.mean(rewards),
            'std_reward': np.std(rewards),
            'max_reward': np.max(rewards),
            'min_reward': np.min(rewards),
        }
    
    @staticmethod
    def calculate_improvement_rate(metrics: Dict[str, List[float]]) -> List[float]:
        """Calculate improvement rate over time"""
        if 'smoothed_rewards' not in metrics or len(metrics['smoothed_rewards']) < 2:
            return []
        
        smoothed = metrics['smoothed_rewards']
        improvements = []
        
        for i in range(1, len(smoothed)):
            improvement = smoothed[i] - smoothed[i-1]
            improvements.append(improvement)
        
        return improvements

# Export key classes and functions
__all__ = [
    'NeuralNetworkStats',
    'PerformanceMetrics',
    'NeuralNetworkAnalyzer',
    'SimulationRunner',
    'VisualizationTools',
    'LearningCurveAnalyzer',
]

# Example usage functions
def example_basic_simulation():
    """Example of basic simulation usage"""
    print("=== Basic NLM Simulation Example ===")
    
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(100, 100, 10, 10)
    world.setAgentStart(50, 50)
    
    # Run simulation
    print(f"Starting simulation with {config.get('neuron_count')} neurons")
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    
    # This would run the actual simulation
    # for step in range(100):
    #     percept = world.getSensoryPercept()
    #     brain.receiveSensoryInput(percept)
    #     brain.step(step, step * 0.001)
    
    print("Simulation completed!")

def example_neural_analysis():
    """Example of neural network analysis"""
    print("=== Neural Network Analysis Example ===")
    
    # This would require an actual brain object
    # stats = NeuralNetworkAnalyzer.analyze_brain(brain)
    # print(f"Total neurons: {stats.total_neurons}")
    # print(f"Total synapses: {stats.total_synapses}")
    # print(f"Average firing rate: {stats.average_firing_rate:.2f} Hz")
    
    print("Analysis example would run here!")

def example_performance_monitoring():
    """Example of performance monitoring"""
    print("=== Performance Monitoring Example ===")
    
    # This would set up performance monitoring
    # runner = SimulationRunner(brain, world, config)
    # metrics = runner.get_performance_summary()
    
    print("Performance monitoring example would run here!")

# Run examples if this module is executed directly
if __name__ == '__main__':
    example_basic_simulation()
    example_neural_analysis()
    example_performance_monitoring()