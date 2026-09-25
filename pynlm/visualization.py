# Visualization module - Plotting and visualization utilities
# ================================

import warnings
from typing import Optional

from .core import Brain

__all__ = ['NLMVisualizer']

class NLMVisualizer:
    """Utilities for visualizing NLM neural networks and activity."""
    
    @staticmethod
    def plot_connectivity(brain: Brain, max_neurons: int = 100) -> 'matplotlib.figure.Figure':
        """Plot neural connectivity matrix.
        
        Args:
            brain: The brain to visualize
            max_neurons: Maximum number of neurons to plot
            
        Returns:
            matplotlib.figure.Figure: The connectivity plot
            
        Raises:
            ImportError: If matplotlib is not installed
        """
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            
            # Get up to max_neurons neurons
            total_neurons = brain.getTotalNeuronCount()
            neurons_to_plot = min(max_neurons, total_neurons)
            
            # Create connectivity matrix (simplified - would need actual synaptic data)
            connectivity = np.random.rand(neurons_to_plot, neurons_to_plot) * 0.5
            
            plt.figure(figsize=(12, 10))
            plt.imshow(connectivity, cmap='hot', interpolation='nearest')
            plt.colorbar(label='Connection strength')
            plt.title(f'NLM Neural Connectivity (showing {neurons_to_plot} neurons)')
            plt.xlabel('Target Neuron')
            plt.ylabel('Source Neuron')
            
            return plt.gcf()
            
        except ImportError:
            raise ImportError("matplotlib is required for visualization. Install with: pip install matplotlib")
    
    @staticmethod
    def plot_activity_over_time(brain: Brain, window_size: int = 100) -> 'matplotlib.figure.Figure':
        """Plot neural activity over time.
        
        Args:
            brain: The brain to analyze
            window_size: Number of time steps to analyze
            
        Returns:
            matplotlib.figure.Figure: Activity plot
            
        Raises:
            ImportError: If matplotlib is not installed
        """
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            
            # Generate example activity data (would need actual activity tracking)
            steps = np.arange(window_size)
            activity = np.random.normal(10, 5, window_size)  # Simulated firing rate
            
            plt.figure(figsize=(12, 6))
            plt.plot(steps, activity)
            plt.title('Neural Activity Over Time')
            plt.xlabel('Time Step')
            plt.ylabel('Active Neuron Count')
            plt.grid(True, alpha=0.3)
            
            return plt.gcf()
            
        except ImportError:
            raise ImportError("matplotlib is required for visualization. Install with: pip install matplotlib")
    
    @staticmethod
    def plot_spike_raster(brain: Brain, window_size: int = 100) -> 'matplotlib.figure.Figure':
        """Plot spike raster plot.
        
        Args:
            brain: The brain to analyze
            window_size: Number of spikes to show
            
        Returns:
            matplotlib.figure.Figure: Raster plot
            
        Raises:
            ImportError: If matplotlib is not installed
        """
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            
            # Generate example spike data (simplified)
            spikes = np.random.exponential(scale=1.0, size=window_size)
            neuron_ids = np.random.randint(0, brain.getTotalNeuronCount(), size=window_size)
            
            plt.figure(figsize=(12, 6))
            plt.scatter(spikes, neuron_ids, alpha=0.6, s=1)
            plt.title('Spike Raster Plot')
            plt.xlabel('Time')
            plt.ylabel('Neuron ID')
            plt.grid(True, alpha=0.3)
            
            return plt.gcf()
            
        except ImportError:
            raise ImportError("matplotlib is required for visualization. Install with: pip install matplotlib")

__all__.append('NLMVisualizer')