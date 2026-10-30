"""
Python visualization API for NLM

This module provides Python-friendly visualization interfaces
for NLM, including network visualization, activity plots, and
debugging tools.
"""

from typing import Dict, List, Optional, Any, Union
import warnings

# Import from C++ bindings if available
try:
    from .bindings import (
        Vision,
        Audio,
        InternalSignals,
        SensorType,
    )
    from .core import (
        Brain,
        SimpleWorld,
        AgentBrain,
        Vision as VisionClass,
        Audio as AudioClass,
        InternalSignals as InternalSignalsClass,
        Action,
        WorldObject,
        ActionResult,
        createDefaultConfig,
        createBrain,
        createAgentBrain,
        createSimpleWorld,
    )
    
    from .enums import (
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
    )
    
    BINDINGS_AVAILABLE = True
    
except ImportError:
    BINDINGS_AVAILABLE = False
    
    # Create stub classes for development
    class Vision:
        """Stub Vision class"""
        def __init__(self, width: int = 0, height: int = 0, channels: int = 0):
            self.width = width
            self.height = height
            self.channels = channels
            
        def setData(self, data: List[float]) -> None:
            pass
    
    class Audio:
        """Stub Audio class"""
        def __init__(self, sampleRate: int = 0, numSamples: int = 0):
            self.sampleRate = sampleRate
            self.numSamples = numSamples
            
        def setData(self, data: List[float]) -> None:
            pass
    
    class InternalSignals:
        """Stub InternalSignals class"""
        def __init__(self):
            pass
            
        def addSignal(self, value: float) -> None:
            pass
            
        def clearSignals(self) -> None:
            pass
    
    class SensorType:
        """Sensor type enumeration"""
        VISION = "vision"
        AUDIO = "audio"
        INTERNAL = "internal"
        PROPRIOCEPTION = "proprioception"
    
    # Stub core classes
    class Brain:
        """Stub Brain class"""
        pass
    
    class SimpleWorld:
        """Stub SimpleWorld class"""
        pass
    
    class AgentBrain:
        """Stub AgentBrain class"""
        pass
    
    class Action:
        """Stub Action class"""
        pass
    
    class WorldObject:
        """Stub WorldObject class"""
        pass
    
    # Stub factory functions
    def createDefaultConfig():
        return None
        
    def createBrain(config):
        return None
        
    def createAgentBrain(brain):
        return None
        
    def createSimpleWorld():
        return None

# Visualization backends
class VisualizationBackend:
    """Base class for visualization backends"""
    
    def __init__(self):
        self.active = False
        self.update_rate = 0.0
    
    def initialize(self) -> bool:
        """Initialize the backend"""
        self.active = True
        return True
    
    def update(self) -> None:
        """Update visualization"""
        pass
    
    def render(self) -> None:
        """Render current state"""
        pass
    
    def close(self) -> None:
        """Close visualization"""
        self.active = False
    
    def is_active(self) -> bool:
        """Check if backend is active"""
        return self.active
    
    def set_update_rate(self, hz: float) -> None:
        """Set update rate"""
        self.update_rate = hz

class MatplotlibBackend(VisualizationBackend):
    """Matplotlib-based visualization backend"""
    
    def __init__(self):
        super().__init__()
        self.figure = None
        self.axes = None
        self.plots = {}
    
    def initialize(self) -> bool:
        """Initialize matplotlib backend"""
        try:
            import matplotlib.pyplot as plt
            import matplotlib
            matplotlib.use('Agg')  # Non-interactive backend
            self.figure = plt.figure()
            self.axes = self.figure.add_subplot(111)
            return super().initialize()
        except ImportError:
            warnings.warn("Matplotlib not available, falling back to stub implementation")
            return False
    
    def render_network(self, brain: Brain) -> None:
        """Render neural network visualization"""
        if not self.is_active():
            return
        
        # Stub implementation
        print(f"[STUB] Rendering network with {brain.getTotalNeuronCount()} neurons")
    
    def render_activity(self, brain: Brain) -> None:
        """Render neural activity visualization"""
        if not self.is_active():
            return
        
        # Stub implementation
        firing_rate = brain.getAverageFiringRate()
        print(f"[STUB] Rendering activity plot (firing rate: {firing_rate:.2f} Hz)")
    
    def render_spike_raster(self, spike_events: List[Dict]) -> None:
        """Render spike raster plot"""
        if not self.is_active():
            return
        
        # Stub implementation
        print(f"[STUB] Rendering spike raster plot with {len(spike_events)} events")
    
    def render_weight_matrix(self, brain: Brain) -> None:
        """Render synaptic weight matrix"""
        if not self.is_active():
            return
        
        # Stub implementation
        neuron_count = brain.getTotalNeuronCount()
        print(f"[STUB] Rendering weight matrix ({neuron_count}x{neuron_count})")

class PlotlyBackend(VisualizationBackend):
    """Plotly-based visualization backend"""
    
    def __init__(self):
        super().__init__()
        self.figures = {}
    
    def initialize(self) -> bool:
        """Initialize plotly backend"""
        try:
            import plotly.graph_objects as go
            import plotly.io as pio
            return super().initialize()
        except ImportError:
            warnings.warn("Plotly not available, falling back to stub implementation")
            return False
    
    def render_network_interactive(self, brain: Brain) -> None:
        """Render interactive network visualization"""
        if not self.is_active():
            return
        
        # Stub implementation
        print(f"[STUB] Creating interactive network plot with {brain.getTotalNeuronCount()} neurons")
    
    def create_time_series(self, data: List[float], title: str = "Time Series") -> None:
        """Create time series plot"""
        if not self.is_active():
            return
        
        # Stub implementation
        print(f"[STUB] Creating time series plot: {title}")

class NLMVisualizer:
    """
    High-level NLM visualization interface with support for multiple backends.
    
    This class provides comprehensive visualization capabilities for NLM simulations,
    including network diagrams, activity plots, spike rasters, and weight matrices.
    """
    
    def __init__(self, backend: str = "matplotlib"):
        self.backend = backend
        self.current_backend = None
        self.plots = {}
        
        # Initialize selected backend
        self._initialize_backend(backend)
    
    def _initialize_backend(self, backend: str) -> bool:
        """Initialize visualization backend"""
        try:
            if backend == "matplotlib":
                self.current_backend = MatplotlibBackend()
            elif backend == "plotly":
                self.current_backend = PlotlyBackend()
            elif backend == "stub":
                self.current_backend = VisualizationBackend()
            else:
                warnings.warn(f"Unknown backend: {backend}, using matplotlib")
                self.current_backend = MatplotlibBackend()
            
            return self.current_backend.initialize()
        except Exception as e:
            warnings.warn(f"Failed to initialize backend {backend}: {e}")
            self.current_backend = VisualizationBackend()
            return False
    
    def switch_backend(self, backend: str) -> bool:
        """
        Switch to a different visualization backend.
        
        Args:
            backend: Backend name ("matplotlib", "plotly", "stub")
            
        Returns:
            True if switch successful
        """
        if self.current_backend:
            self.current_backend.close()
        
        success = self._initialize_backend(backend)
        if success:
            self.backend = backend
        
        return success
    
    def render_network(self, brain: Brain, title: str = "Neural Network") -> None:
        """
        Render neural network visualization.
        
        Args:
            brain: Brain object to visualize
            title: Plot title
        """
        if not self.current_backend:
            return
        
        self.current_backend.render_network(brain)
        self.plots['network'] = {
            'title': title,
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'timestamp': None
        }
    
    def render_activity(self, brain: Brain, title: str = "Neural Activity") -> None:
        """
        Render neural activity visualization.
n        
        Args:
            brain: Brain object to visualize
            title: Plot title
        """
        if not self.current_backend:
            return
        
        self.current_backend.render_activity(brain)
        self.plots['activity'] = {
            'title': title,
            'firing_rate': brain.getAverageFiringRate(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'timestamp': None
        }
    
    def render_spike_raster(self, spike_events: List[Dict], 
                           title: str = "Spike Raster Plot") -> None:
        """
        Render spike raster plot.
        
        Args:
            spike_events: List of spike event dictionaries
            title: Plot title
        """
        if not self.current_backend:
            return
        
        self.current_backend.render_spike_raster(spike_events)
        self.plots['spike_raster'] = {
            'title': title,
            'event_count': len(spike_events),
            'timestamp': None
        }
    
    def render_weight_matrix(self, brain: Brain, title: str = "Synaptic Weight Matrix") -> None:
        """
        Render synaptic weight matrix visualization.
        
        Args:
            brain: Brain object to visualize
            title: Plot title
        """
        if not self.current_backend:
            return
        
        self.current_backend.render_weight_matrix(brain)
        self.plots['weight_matrix'] = {
            'title': title,
            'neuron_count': brain.getTotalNeuronCount(),
            'timestamp': None
        }
    
    def create_comparison_plot(self, brains: List[Brain], 
                              titles: Optional[List[str]] = None) -> None:
        """
        Create comparison plot of multiple brains.
        
        Args:
            brains: List of brain objects to compare
            titles: Optional list of plot titles
        """
        if not self.current_backend:
            return
        
        print(f"[STUB] Creating comparison plot of {len(brains)} brains")
    
    def create_time_series(self, data: List[float], 
                          title: str = "Time Series", 
                          xlabel: str = "Time", 
                          ylabel: str = "Value") -> None:
        """
        Create time series plot.
n        
        Args:
            data: Time series data
            title: Plot title
            xlabel: X-axis label
            ylabel: Y-axis label
        """
        if not self.current_backend:
            return
        
        self.current_backend.create_time_series(data, title)
    
    def save_plots(self, filepath: str) -> bool:
        """
        Save all plots to file.
n        
        Args:
            filepath: Output file path
            
        Returns:
            True if save successful
        """
        try:
            if self.current_backend and hasattr(self.current_backend, 'figure'):
                self.current_backend.figure.savefig(filepath)
                return True
            return False
        except Exception as e:
            warnings.warn(f"Failed to save plots: {e}")
            return False
    
    def get_available_backends(self) -> List[str]:
        """
        Get list of available visualization backends.
        
        Returns:
            List of backend names
        """
        backends = []
        
        if BINDINGS_AVAILABLE:
            try:
                import matplotlib
                backends.append("matplotlib")
            except ImportError:
                pass
            
            try:
                import plotly
                backends.append("plotly")
            except ImportError:
                pass
        
        backends.append("stub")
        
        return backends
    
    def is_active(self) -> bool:
        """Check if visualizer is active"""
        return self.current_backend and self.current_backend.is_active()
    
    def close(self) -> None:
        """Close visualizer and all backends"""
        if self.current_backend:
            self.current_backend.close()
            self.current_backend = None

# Convenience functions for visualization
def create_visualizer(backend: str = "matplotlib") -> NLMVisualizer:
    """
    Create NLM visualizer with specified backend.
n    
    Args:
        backend: Visualization backend
        
    Returns:
        NLMVisualizer instance
    """
    return NLMVisualizer(backend)

def visualize_brain(brain: Brain, backend: str = "matplotlib") -> None:
    """
    Quick visualization of brain activity.
n    
    Args:
        brain: Brain object to visualize
        backend: Visualization backend
    """
    visualizer = create_visualizer(backend)
    visualizer.render_network(brain)
    visualizer.render_activity(brain)

def visualize_simulation(brain: Brain, world: SimpleWorld, 
                        agent: AgentBrain, steps: int = 100) -> None:
    """
    Visualize complete simulation.
n    
    Args:
        brain: Brain object
        world: World object
        agent: AgentBrain object
        steps: Number of simulation steps
    """
    visualizer = create_visualizer()
    
    for i in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        
        if i % 10 == 0:  # Update visualization every 10 steps
            visualizer.render_network(brain)
            visualizer.render_activity(brain)

# Export everything
__all__ = [
    # Backend classes
    'VisualizationBackend',
    'MatplotlibBackend',
    'PlotlyBackend',
    
    # Main visualizer class
    'NLMVisualizer',
    
    # Convenience functions
    'create_visualizer',
    'visualize_brain',
    'visualize_simulation',
    
    # Backend utilities
    'get_available_backends',
]

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python visualization API for NLM neural simulation"

print(f"NLM visualization module loaded (version {__version__})")