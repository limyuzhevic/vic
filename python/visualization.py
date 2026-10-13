#!/usr/bin/env python3
"""
NLM Visualization Module for Python

This module provides Python interface to the NLM visualization system,
including network visualization, activity monitoring, and data export.
"""

import sys
import os
import json
from typing import List, Dict, Any, Optional
from dataclasses import dataclass
from enum import Enum

# Add the visualization directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

from visualization import VisualizationInterface, NetworkVisualizer

class VisualizationType(Enum):
    """Enumeration of visualization types"""
    NETWORK = "network"
    ACTIVITY = "activity"
    SPIKES = "spikes"
    WEIGHTS = "weights"
    HEATMAP = "heatmap"
    RASTER = "raster"

class VisualizationOptions:
    """Configuration options for visualization"""
    
    def __init__(self):
        self.show_neurons = True
        self.show_connections = True
        self.show_spikes = True
        self.show_weights = True
        self.show_activity = True
        self.neuron_size = 1.0
        self.connection_thickness = 0.5
        self.color_scheme = "default"
        self.export_data = False
        self.output_file = "visualization.png"
        self.data_file = "visualization_data.json"
        self.update_rate = 30.0
        self.width = 800
        self.height = 600
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert options to dictionary"""
        return {
            'show_neurons': self.show_neurons,
            'show_connections': self.show_connections,
            'show_spikes': self.show_spikes,
            'show_weights': self.show_weights,
            'show_activity': self.show_activity,
            'neuron_size': self.neuron_size,
            'connection_thickness': self.connection_thickness,
            'color_scheme': self.color_scheme,
            'export_data': self.export_data,
            'output_file': self.output_file,
            'data_file': self.data_file,
            'update_rate': self.update_rate,
            'width': self.width,
            'height': self.height,
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'VisualizationOptions':
        """Create options from dictionary"""
        opts = cls()
        for key, value in data.items():
            if hasattr(opts, key):
                setattr(opts, key, value)
        return opts

@dataclass
class NeuronData:
    """Data for a neuron visualization"""
    id: int
    type: str
    x: float
    y: float
    radius: float
    color: str
    activity: float
    spike_count: int
    active: bool

@dataclass
class ConnectionData:
    """Data for a connection visualization"""
    source: int
    target: int
    weight: float
    thickness: float
    color: str
    active: bool

@dataclass
class StatisticsData:
    """Statistical data for visualization"""
    neurons: int
    connections: int
    density: float
    avg_weight: float
    active_neurons: int
    avg_activity: float
    neuron_types: Dict[str, int]
    weight_distribution: Dict[str, int]

class NLMVisualizer:
    """Python interface to NLM visualization system"""
    
    def __init__(self):
        self._visualization = None
        self._options = VisualizationOptions()
        self._neurons = []
        self._connections = []
        self._statistics = None
    
    def initialize(self) -> bool:
        """Initialize the visualization system"""
        try:
            self._visualization = NetworkVisualizer()
            if self._visualization.initialize():
                self._visualization.setOptions(self._options.to_dict())
                return True
            return False
        except Exception as e:
            print(f"Error initializing visualization: {e}")
            return False
    
    def update(self, brain, timestep: float) -> bool:
        """Update visualization with brain state"""
        if not self._visualization:
            return False
        
        try:
            self._visualization.update(brain, timestep)
            self._update_local_data(brain)
            return True
        except Exception as e:
            print(f"Error updating visualization: {e}")
            return False
    
    def render(self) -> bool:
        """Render visualization"""
        if not self._visualization:
            return False
        
        try:
            self._visualization.render()
            return True
        except Exception as e:
            print(f"Error rendering visualization: {e}")
            return False
    
    def close(self) -> bool:
        """Close visualization system"""
        if self._visualization:
            self._visualization.close()
            self._visualization = None
        self._neurons.clear()
        self._connections.clear()
        self._statistics = None
        return True
    
    def is_active(self) -> bool:
        """Check if visualization is active"""
        return self._visualization is not None and self._visualization.isActive()
    
    def set_options(self, options: VisualizationOptions):
        """Set visualization options"""
        self._options = options
        if self._visualization:
            self._visualization.setOptions(options.to_dict())
    
    def get_options(self) -> VisualizationOptions:
        """Get current visualization options"""
        return self._options
    
    def visualize_network(self, brain) -> bool:
        """Visualize network structure"""
        if not self._visualization:
            return False
        
        try:
            self._visualization.visualizeNetwork(brain)
            self._update_local_data(brain)
            return True
        except Exception as e:
            print(f"Error visualizing network: {e}")
            return False
    
    def visualize_activity(self, brain) -> bool:
        """Visualize network activity"""
        if not self._visualization:
            return False
        
        try:
            self._visualization.visualizeActivity(brain)
            self._update_local_data(brain)
            return True
        except Exception as e:
            print(f"Error visualizing activity: {e}")
            return False
    
    def visualize_spikes(self, spikes: List) -> bool:
        """Visualize spike activity"""
        if not self._visualization:
            return False
        
        try:
            self._visualization.visualizeSpikes(spikes)
            return True
        except Exception as e:
            print(f"Error visualizing spikes: {e}")
            return False
    
    def visualize_weights(self, brain) -> bool:
        """Visualize connection weights"""
        if not self._visualization:
            return False
        
        try:
            self._visualization.visualizeWeights(brain)
            return True
        except Exception as e:
            print(f"Error visualizing weights: {e}")
            return False
    
    def export_data(self) -> bool:
        """Export visualization data"""
        if not self._visualization:
            return False
        
        try:
            self._visualization.exportData()
            return True
        except Exception as e:
            print(f"Error exporting data: {e}")
            return False
    
    def get_statistics(self) -> Optional[StatisticsData]:
        """Get visualization statistics"""
        return self._statistics
    
    def save_data_to_file(self, filepath: str) -> bool:
        """Save visualization data to file"""
        try:
            data = self._get_data_dict()
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
            return True
        except Exception as e:
            print(f"Error saving data to file: {e}")
            return False
    
    def load_data_from_file(self, filepath: str) -> bool:
        """Load visualization data from file"""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            self._load_data_dict(data)
            return True
        except Exception as e:
            print(f"Error loading data from file: {e}")
            return False
    
    def generate_report(self) -> str:
        """Generate visualization report"""
        if not self._statistics:
            return "No statistics available"
        
        report = f"NLM Visualization Report\n"
        report += f"Generated at: {self._get_timestamp()}\n\n"
        
        report += f"Network Statistics:\n"
        report += f"  - Total Neurons: {self._statistics.neurons}\n"
        report += f"  - Total Connections: {self._statistics.connections}\n"
        report += f"  - Network Density: {self._statistics.density:.4f}\n"
        report += f"  - Average Weight: {self._statistics.avg_weight:.4f}\n"
        report += f"  - Active Neurons: {self._statistics.active_neurons}\n"
        report += f"  - Average Activity: {self._statistics.avg_activity:.4f}\n\n"
        
        report += f"Neuron Types:\n"
        for neuron_type, count in self._statistics.neuron_types.items():
            report += f"  - {neuron_type}: {count}\n"
        
        report += f"\nWeight Distribution:\n"
        for category, count in self._statistics.weight_distribution.items():
            report += f"  - {category}: {count}\n"
        
        return report
    
    def _update_local_data(self, brain):
        """Update local visualization data from brain"""
        # This would extract data from the brain and update
        # local neuron and connection lists for Python access
        pass
    
    def _get_data_dict(self) -> Dict[str, Any]:
        """Get visualization data as dictionary"""
        return {
            'neurons': [n.__dict__ for n in self._neurons],
            'connections': [c.__dict__ for c in self._connections],
            'statistics': self._statistics.__dict__ if self._statistics else None,
            'timestamp': self._get_timestamp(),
        }
    
    def _load_data_dict(self, data: Dict[str, Any]):
        """Load visualization data from dictionary"""
        self._neurons = [NeuronData(**n) for n in data.get('neurons', [])]
        self._connections = [ConnectionData(**c) for c in data.get('connections', [])]
        if data.get('statistics'):
            self._statistics = StatisticsData(**data['statistics'])
    
    def _get_timestamp(self) -> str:
        """Get current timestamp"""
        import datetime
        return datetime.datetime.now().isoformat()

# Global visualizer instance
_global_visualizer = None

def get_visualizer() -> NLMVisualizer:
    """Get the global visualizer instance"""
    global _global_visualizer
    if _global_visualizer is None:
        _global_visualizer = NLMVisualizer()
    return _global_visualizer

def initialize_visualization() -> bool:
    """Initialize the global visualization system"""
    return get_visualizer().initialize()

def update_visualization(brain, timestep: float) -> bool:
    """Update the global visualization system"""
    return get_visualizer().update(brain, timestep)

def render_visualization() -> bool:
    """Render the global visualization"""
    return get_visualizer().render()

def close_visualization() -> bool:
    """Close the global visualization system"""
    return get_visualizer().close()

def is_visualization_active() -> bool:
    """Check if visualization is active"""
    return get_visualizer().is_active()

def set_visualization_options(options: VisualizationOptions):
    """Set global visualization options"""
    get_visualizer().set_options(options)

def get_visualization_options() -> VisualizationOptions:
    """Get global visualization options"""
    return get_visualizer().get_options()

def visualize_network(brain) -> bool:
    """Visualize network structure globally"""
    return get_visualizer().visualize_network(brain)

def visualize_activity(brain) -> bool:
    """Visualize network activity globally"""
    return get_visualizer().visualize_activity(brain)

def visualize_spikes(spikes: List) -> bool:
    """Visualize spike activity globally"""
    return get_visualizer().visualize_spikes(spikes)

def visualize_weights(brain) -> bool:
    """Visualize connection weights globally"""
    return get_visualizer().visualize_weights(brain)

def export_visualization_data() -> bool:
    """Export visualization data globally"""
    return get_visualizer().export_data()

def generate_visualization_report() -> str:
    """Generate visualization report globally"""
    return get_visualizer().generate_report()

# Example usage function
if __name__ == "__main__":
    print("NLM Visualization Module")
    print("=========================")
    print()
    print("This module provides Python interface to NLM visualization system.")
    print("For usage examples, see the documentation.")
    print()
    print("Available classes:")
    print("  - NLMVisualizer: Main visualization interface")
    print("  - VisualizationOptions: Configuration options")
    print("  - NeuronData, ConnectionData, StatisticsData: Data structures")
    print()
    print("Available functions:")
    print("  - initialize_visualization()")
    print("  - update_visualization(brain, timestep)")
    print("  - render_visualization()")
    print("  - close_visualization()")
    print("  - set_visualization_options(options)")
    print("  - visualize_network(brain)")
    print("  - visualize_activity(brain)")
    print("  - visualize_spikes(spikes)")
    print("  - visualize_weights(brain)")
    print("  - generate_visualization_report()")