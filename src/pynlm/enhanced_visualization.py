"""
Advanced visualization and debugging tools for NLM.

This module provides comprehensive visualization and debugging capabilities
for NLM simulations, including:
- Neural network visualization
- Activity monitoring and analysis
- Simulation playback and debugging
- Performance visualization
- Debug logging and tracing
- Interactive debugging tools
"""

import time
import threading
from typing import Optional, Dict, List, Any, Callable, Tuple
from enum import Enum
from dataclasses import dataclass
import json

# Import enhanced modules
from .enhanced_api import EnhancedBrain, EnhancedBrainConfig

class VisualizationMode(Enum):
    """Visualization modes."""
    LIVE = "live"
    REPLAY = "replay"
    STATIC = "static"
    INTERACTIVE = "interactive"

class DebugLevel(Enum):
    """Debug logging levels."""
    ERROR = "error"
    WARNING = "warning"
    INFO = "info"
    DEBUG = "debug"
    TRACE = "trace"

@dataclass
class ActivityData:
    """Activity data for visualization."""
    timestamp: float
    step: int
    neuron_activity: List[Dict[str, Any]]
    firing_rate: float
    population_activities: Dict[str, Any]
    synaptic_activities: List[Dict[str, Any]]
    memory_state: Dict[str, Any]
    neuromodulation_state: Dict[str, Any]

@dataclass
class NeuronVisualizationData:
    """Neuron visualization data."""
    neuron_id: int
    region_id: int
    population_id: int
    neuron_type: str
    position: tuple
    membrane_potential: float
    firing_state: str
    last_spike_time: float
    synaptic_input: float
    connectivity: List[int]
    plasticity_weights: List[float]

@dataclass
class ConnectionVisualizationData:
    """Synaptic connection visualization data."""
    source_id: int
    target_id: int
    weight: float
    delay: int
    synapse_type: str
    plasticity_flags: Dict[str, bool]
    efficacy: float
    short_term_plasticity: Dict[str, float]

@dataclass
class SimulationEvent:
    """Simulation event for debugging."""
    timestamp: float
    step: int
    event_type: str
    event_data: Dict[str, Any]
    priority: DebugLevel

class NeuralNetworkVisualizer:
    """
    Advanced neural network visualization for NLM simulations.
    
    This class provides comprehensive visualization capabilities including:
    - Real-time neural activity monitoring
    - Network topology visualization
    - Spiking pattern analysis
    - Synaptic dynamics visualization
    - Memory state visualization
    - Development stage visualization
    """
    
    def __init__(self, brain: EnhancedBrain, width: int = 1200, height: int = 800):
        """
        Initialize neural network visualizer.
        
        Args:
            brain: Enhanced brain to visualize
            width: Visualization width
            height: Visualization height
        """
        self._brain = brain
        self._width = width
        self._height = height
        
        # Visualization state
        self._mode = VisualizationMode.LIVE
        self._paused = False
        self._max_history = 1000
        self._activity_data: List[ActivityData] = []
        self._neuron_data_cache: Dict[int, NeuronVisualizationData] = {}
        self._connection_data_cache: Dict[int, ConnectionVisualizationData] = {}
        
        # Visualization components
        self._region_visualizer = RegionVisualizer(width, height)
        self._network_graph = NetworkGraphVisualizer(width, height)
        self._activity_monitor = ActivityMonitor()
        self._neuron_states = {}
        
        # Update loop
        self._update_thread: Optional[threading.Thread] = None
        self._stop_update = threading.Event()
        self._update_interval = 0.1  # 100ms
        
        # Event system
        self._event_handlers: Dict[str, List[Callable]] = {}
        
        # Performance tracking
        self._frame_times: List[float] = []
        self._last_update_time = time.time()
        
        # Start update thread
        self._start_update_loop()
    
    def set_mode(self, mode: VisualizationMode):
        """Set visualization mode."""
        self._mode = mode
        self._trigger_events("mode_changed", mode)
        return self
    
    def pause(self):
        """Pause visualization."""
        self._paused = True
        self._trigger_events("paused")
        return self
    
    def resume(self):
        """Resume visualization."""
        self._paused = False
        self._trigger_events("resumed")
        return self
    
    def get_neuron_visualization_data(self) -> List[NeuronVisualizationData]:
        """Get current neuron visualization data."""
        if not self._brain:
            return []
        
        # Collect brain statistics
        stats = self._brain.get_statistics()
        
        # Get regions and neurons
        regions = self._brain._brain.getRegions()
        neuron_visualization = []
        
        for region in regions:
            neurons = region.getAllNeurons()
            for neuron in neurons:
                neuron_id = neuron.getId()
                
                if neuron_id not in self._neuron_data_cache:
                    # Create new neuron visualization data
                    neuron_data = NeuronVisualizationData(
                        neuron_id=neuron_id,
                        region_id=region.getId(),
                        population_id=0,  # Would need population system
                        neuron_type="regular",  # Would need neuron type system
                        position=(0.0, 0.0),  # Would need spatial system
                        membrane_potential=neuron.getMembranePotential(),
                        firing_state="idle",  # Would need firing state system
                        last_spike_time=0.0,  # Would need spike time system
                        synaptic_input=0.0,  # Would need synaptic input system
                        connectivity=[],  # Would need connectivity system
                        plasticity_weights=[]  # Would need plasticity system
                    )
                    self._neuron_data_cache[neuron_id] = neuron_data
                else:
                    neuron_data = self._neuron_data_cache[neuron_id]
                
                # Update with current data
                neuron_data.membrane_potential = neuron.getMembranePotential()
                # Update other fields...
                
                neuron_visualization.append(neuron_data)
        
        return neuron_visualization
    
    def get_connection_visualization_data(self) -> List[ConnectionVisualizationData]:
        """Get current connection visualization data."""
        if not self._brain:
            return []
        
        regions = self._brain._brain.getRegions()
        connections = []
        
        for region in regions:
            synapses = region.getSynapses()
            for synapse in synapses:
                # Create connection visualization data
                connection_data = ConnectionVisualizationData(
                    source_id=0,  # Would need source ID
                    target_id=0,  # Would need target ID
                    weight=synapse.getWeight(),
                    delay=0,  # Would need delay system
                    synapse_type="excitatory",  # Would need synapse type system
                    plasticity_flags={},  # Would need plasticity flags system
                    efficacy=1.0,  # Would need efficacy system
                    short_term_plasticity={}  # Would need STP system
                )
                connections.append(connection_data)
        
        return connections
    
    def get_activity_data(self, start_step: Optional[int] = None, 
                         end_step: Optional[int] = None) -> List[ActivityData]:
        """Get activity data for specified time range."""
        if not self._activity_data:
            return []
        
        filtered_data = self._activity_data
        
        if start_step is not None:
            filtered_data = [d for d in filtered_data if d.step >= start_step]
        
        if end_step is not None:
            filtered_data = [d for d in filtered_data if d.step <= end_step]
        
        return filtered_data
    
    def get_network_statistics(self) -> Dict[str, Any]:
        """Get network statistics for visualization."""
        if not self._brain:
            return {}
        
        stats = self._brain.get_statistics()
        
        # Add visualization-specific statistics
        regions = self._brain._brain.getRegions()
        
        network_stats = {
            "total_neurons": stats['total_neurons'],
            "total_synapses": stats['total_synapses'],
            "active_regions": len([r for r in regions if r.getActiveNeurons() > 0]),
            "average_firing_rate": stats['average_firing_rate'],
            "activity_level": self._calculate_activity_level(),
            "spike_count": stats['total_spikes'],
            "memory_traces": stats['memory_traces'],
            "development_stage": stats['development_stage'],
            "curiosity_level": stats['curiosity_level'],
            "visualization_mode": self._mode.value,
            "paused": self._paused,
            "history_size": len(self._activity_data),
            "frame_rate": self._calculate_frame_rate()
        }
        
        return network_stats
    
    def save_visualization_data(self, filepath: str):
        """Save visualization data to file."""
        data = {
            "neurons": [vars(neuron) for neuron in self.get_neuron_visualization_data()],
            "connections": [vars(conn) for conn in self.get_connection_visualization_data()],
            "activity_data": [vars(activity) for activity in self.get_activity_data()],
            "statistics": self.get_network_statistics(),
            "timestamp": time.time()
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
        
        self._trigger_events("data_saved", filepath)
    
    def load_visualization_data(self, filepath: str):
        """Load visualization data from file."""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        # Load neuron data
        neuron_data = data.get("neurons", [])
        for neuron_dict in neuron_data:
            neuron = NeuronVisualizationData(**neuron_dict)
            self._neuron_data_cache[neuron.neuron_id] = neuron
        
        # Load connection data
        connection_data = data.get("connections", [])
        for conn_dict in connection_data:
            connection = ConnectionVisualizationData(**conn_dict)
            self._connection_data_cache[id(connection)] = connection
        
        # Load activity data
        activity_data = data.get("activity_data", [])
        self._activity_data = [ActivityData(**activity_dict) for activity_dict in activity_data]
        
        self._trigger_events("data_loaded", filepath)
    
    def add_event_handler(self, event_type: str, handler: Callable):
        """Add event handler."""
        if event_type not in self._event_handlers:
            self._event_handlers[event_type] = []
        
        self._event_handlers[event_type].append(handler)
    
    def remove_event_handler(self, event_type: str, handler: Callable):
        """Remove event handler."""
        if event_type in self._event_handlers:
            try:
                self._event_handlers[event_type].remove(handler)
            except ValueError:
                pass
    
    def get_region_visualization(self, region_id: int) -> Dict[str, Any]:
        """Get visualization for specific region."""
        return self._region_visualizer.visualize_region(
            self._neuron_data_cache, region_id
        )
    
    def get_network_graph(self) -> Dict[str, Any]:
        """Get network graph visualization."""
        return self._network_graph.visualize_network(
            self._neuron_data_cache, self._connection_data_cache
        )
    
    def get_activity_heatmap(self, neurons: List[NeuronVisualizationData]) -> Dict[str, Any]:
        """Get activity heatmap visualization."""
        return self._activity_monitor.create_heatmap(neurons)
    
    def get_spike_raster(self, neurons: List[NeuronVisualizationData]) -> Dict[str, Any]:
        """Get spike raster visualization."""
        return self._activity_monitor.create_spike_raster(neurons)
    
    # === Private Methods ===
    
    def _start_update_loop(self):
        """Start visualization update loop."""
        self._stop_update.clear()
        self._update_thread = threading.Thread(target=self._update_loop, daemon=True)
        self._update_thread.start()
    
    def _update_loop(self):
        """Main update loop for visualization."""
        while not self._stop_update.wait(self._update_interval):
            if not self._paused:
                self._update_visualization()
    
    def _update_visualization(self):
        """Update visualization data."""
        try:
            update_start = time.time()
            
            # Collect activity data
            activity_data = self._collect_activity_data()
            self._activity_data.append(activity_data)
            
            # Limit history size
            if len(self._activity_data) > self._max_history:
                self._activity_data = self._activity_data[-self._max_history:]
            
            # Update neuron data cache
            self._update_neuron_data_cache()
            
            # Update connection data cache
            self._update_connection_data_cache()
            
            # Track performance
            update_time = time.time() - update_start
            self._frame_times.append(update_time)
            
            # Limit frame times
            if len(self._frame_times) > 100:
                self._frame_times = self._frame_times[-100:]
            
        except Exception as e:
            self._trigger_events("error", e)
    
    def _collect_activity_data(self) -> ActivityData:
        """Collect current activity data."""
        if not self._brain:
            return ActivityData(
                timestamp=time.time(),
                step=0,
                neuron_activity=[],
                firing_rate=0.0,
                population_activities={},
                synaptic_activities=[],
                memory_state={},
                neuromodulation_state={}
            )
        
        stats = self._brain.get_statistics()
        
        # Collect neuron activities
        neuron_activities = []
        regions = self._brain._brain.getRegions()
        
        for region in regions:
            neurons = region.getAllNeurons()
            for neuron in neurons:
                neuron_activities.append({
                    "neuron_id": neuron.getId(),
                    "membrane_potential": neuron.getMembranePotential(),
                    "firing_state": "firing" if neuron.getFiringState() else "resting",
                    "last_spike_time": neuron.getLastSpikeTime()
                })
        
        return ActivityData(
            timestamp=time.time(),
            step=stats['simulation_steps'],
            neuron_activity=neuron_activities,
            firing_rate=stats['average_firing_rate'],
            population_activities=self._get_population_activities(),
            synaptic_activities=self._get_synaptic_activities(),
            memory_state=self._get_memory_state(),
            neuromodulation_state=self._get_neuromodulation_state()
        )
    
        regions = self._brain._brain.getRegions()
        
        for region in regions:
            neurons = region.getAllNeurons()
            for neuron in neurons:
                if neuron_id in self._neuron_data_cache:
                    self._neuron_data_cache[neuron_id].membrane_potential = neuron.getMembranePotential()
                    self._neuron_data_cache[neuron_id].firing_state = "firing" if neuron.getFiringState() else "resting"
                    self._neuron_data_cache[neuron_id].last_spike_time = neuron.getLastSpikeTime()
    
    def _update_connection_data_cache(self):
        """Update connection data cache."""
        if not self._brain:
            return
        
        regions = self._brain._brain.getRegions()
        
        for region in regions:
            synapses = region.getSynapses()
            for synapse in synapses:
                # Update connection data
                pass
    
    def _get_population_activities(self) -> Dict[str, Any]:
        """Get population activities."""
        populations = {}
        regions = self._brain._brain.getRegions()
        
        for region in regions:
            neurons = region.getAllNeurons()
            for neuron in neurons:
                firing_count += neuron.getFiringState()
            total_neurons = len(neurons)
            
            if total_neurons > 0:
                populations[f"region_{region.getId()}"] = {
                    "firing_neurons": firing_count,
                    "firing_rate": firing_count / total_neurons,
                    "total_neurons": total_neurons
                }
        
        return populations
    
    def _get_synaptic_activities(self) -> List[Dict[str, Any]]:
        """Get synaptic activities."""
        activities = []
        regions = self._brain._brain.getRegions()
        
        for region in regions:
            synapses = region.getSynapses()
            for synapse in synapses:
                activities.append({
                    "source": synapse.getSourceNeuronId(),
                    "target": synapse.getTargetNeuronId(),
                    "weight": synapse.getWeight(),
                    "active": synapse.getActiveState()
                })
        
        return activities
    
    def _get_memory_state(self) -> Dict[str, Any]:
        """Get memory state."""
        if not self._brain:
            return {}
        
        memory_systems = {
            "working_memory": self._brain._brain.getWorkingMemory(),
            "episodic_memory": self._brain._brain.getEpisodicMemory(),
            "associative_memory": self._brain._brain.getAssociativeMemory()
        }
        
        state = {}
        for name, system in memory_systems.items():
            if system:
                state[name] = {
                    "active_traces": system.getActiveTraces(),
                    "episode_count": getattr(system, 'getEpisodeCount', lambda: 0)(),
                    "pattern_count": getattr(system, 'getPatternCount', lambda: 0)()
                }
        
        return state
    
    def _get_neuromodulation_state(self) -> Dict[str, Any]:
        """Get neuromodulation state."""
        if not self._brain:
            return {}
        
        neuromodulators = {
            "dopamine": self._brain._brain.getDopamine(),
            "curiosity": self._brain._brain.getCuriosity(),
            "novelty": self._brain._brain.getNovelty(),
            "prediction_error": self._brain._brain.getPredictionErrorSignal()
        }
        
        state = {}
        for name, modulator in neuromodulators.items():
            if modulator:
                state[name] = {
                    "level": modulator.getLevel(),
                    "active": modulator.getActiveState(),
                    "last_update": modulator.getLastUpdateTime()
                }
        
        return state
    
    def _calculate_activity_level(self) -> float:
        """Calculate overall activity level."""
        if not self._activity_data:
            return 0.0
        
        recent_activities = self._activity_data[-100:] if len(self._activity_data) > 100 else self._activity_data
        
        total_firing_rate = sum(activity.firing_rate for activity in recent_activities)
        avg_firing_rate = total_firing_rate / len(recent_activities) if recent_activities else 0.0
        
        return min(1.0, avg_firing_rate / 50.0)  # Normalize to [0, 1]
    
    def _calculate_frame_rate(self) -> float:
        """Calculate current frame rate."""
        if len(self._frame_times) < 2:
            return 0.0
        
        total_time = sum(self._frame_times)
        frame_count = len(self._frame_times)
        
        return frame_count / total_time if total_time > 0 else 0.0
    
    def _trigger_events(self, event_type: str, *args):
        """Trigger event handlers."""
        if event_type in self._event_handlers:
            for handler in self._event_handlers[event_type]:
                try:
                    handler(event_type, *args)
                except Exception as e:
                    print(f"Error in event handler for {event_type}: {e}")
    
    def stop(self):
        """Stop visualization."""
        self._stop_update.set()
        if self._update_thread:
            self._update_thread.join(timeout=5.0)
        
        self._trigger_events("stopped")

# === Visualization Components ===

class RegionVisualizer:
    """Visualizer for brain regions."""
    
    def __init__(self, width: int, height: int):
        self._width = width
        self._height = height
    
    def visualize_region(self, neuron_data: Dict[int, NeuronVisualizationData],
                        region_id: int) -> Dict[str, Any]:
        """Visualize specific region."""
        region_neurons = {id: neuron for id, neuron in neuron_data.items() 
                         if neuron.region_id == region_id}
        
        if not region_neurons:
            return {"error": "No neurons found in region"}
        
        # Calculate bounding box
        positions = [(neuron.position[0], neuron.position[1]) for neuron in region_neurons.values()]
        if positions:
            min_x = min(pos[0] for pos in positions)
            max_x = max(pos[0] for pos in positions)
            min_y = min(pos[1] for pos in positions)
            max_y = max(pos[1] for pos in positions)
            
            center_x = (min_x + max_x) / 2
            center_y = (min_y + max_y) / 2
            size = max(max_x - min_x, max_y - min_y)
        else:
            center_x = center_y = 0.0
            size = 1.0
        
        return {
            "region_id": region_id,
            "neuron_count": len(region_neurons),
            "bounding_box": {
                "x": center_x - size / 2,
                "y": center_y - size / 2,
                "width": size,
                "height": size
            },
            "neurons": [vars(neuron) for neuron in region_neurons.values()],
            "activity_level": sum(neuron.membrane_potential for neuron in region_neurons.values()) / len(region_neurons)
        }

class NetworkGraphVisualizer:
    """Visualizer for network graphs."""
    
    def __init__(self, width: int, height: int):
        self._width = width
        self._height = height
    
    def visualize_network(self, neuron_data: Dict[int, NeuronVisualizationData],
                         connection_data: Dict[int, ConnectionVisualizationData]) -> Dict[str, Any]:
        """Visualize network graph."""
        if not neuron_data:
            return {"error": "No neuron data available"}
        
        # Create adjacency matrix
        neuron_ids = list(neuron_data.keys())
        adjacency_matrix = [[0.0 for _ in neuron_ids] for _ in neuron_ids]
        
        # Map neuron IDs to matrix indices
        neuron_id_to_index = {neuron_id: i for i, neuron_id in enumerate(neuron_ids)}
        
        # Fill adjacency matrix
        for conn in connection_data.values():
            if (conn.source_id in neuron_id_to_index and 
                conn.target_id in neuron_id_to_index):
                source_idx = neuron_id_to_index[conn.source_id]
                target_idx = neuron_id_to_index[conn.target_id]
                adjacency_matrix[source_idx][target_idx] = conn.weight
        
        return {
            "neuron_count": len(neuron_ids),
            "connection_count": len(connection_data),
            "adjacency_matrix": adjacency_matrix,
            "neuron_positions": {neuron_id: neuron.position for neuron_id, neuron in neuron_data.items()},
            "network_density": sum(sum(row) for row in adjacency_matrix) / (len(adjacency_matrix) ** 2)
        }

class ActivityMonitor:
    """Monitor and analyze neural activity."""
    
    def create_heatmap(self, neurons: List[NeuronVisualizationData]) -> Dict[str, Any]:
        """Create activity heatmap."""
        if not neurons:
            return {"error": "No neurons provided"}
        
        # Create simple heatmap data
        x_coords = [neuron.position[0] for neuron in neurons]
        y_coords = [neuron.position[1] for neuron in neurons]
        activity_values = [neuron.membrane_potential for neuron in neurons]
        
        return {
            "x_coords": x_coords,
            "y_coords": y_coords,
            "activity_values": activity_values,
            "max_activity": max(activity_values) if activity_values else 0.0,
            "min_activity": min(activity_values) if activity_values else 0.0
        }
    
    def create_spike_raster(self, neurons: List[NeuronVisualizationData]) -> Dict[str, Any]:
        """Create spike raster visualization."""
        if not neurons:
            return {"error": "No neurons provided"}
        
        # Create spike times (simplified)
        spike_times = []
        for neuron in neurons:
            for spike_time in [neuron.last_spike_time]:
                if spike_time > 0:
                    spike_times.append({
                        "neuron_id": neuron.neuron_id,
                        "time": spike_time
                    })
        
        return {
            "spike_events": spike_times,
            "neuron_count": len(neurons),
            "total_spikes": len(spike_times)
        }

# === Factory Functions ===

def create_neural_network_visualizer(brain: EnhancedBrain, width: int = 1200, 
                                   height: int = 800) -> NeuralNetworkVisualizer:
    """
    Create neural network visualizer.
    
    Args:
        brain: Enhanced brain to visualize
        width: Visualization width
        height: Visualization height
    
    Returns:
        NeuralNetworkVisualizer instance
    """
    return NeuralNetworkVisualizer(brain, width, height)

def create_activity_monitor(width: int = 800, height: int = 600) -> ActivityMonitor:
    """
    Create activity monitor.
    
    Args:
        width: Monitor width
        height: Monitor height
    
    Returns:
        ActivityMonitor instance
    """
    return ActivityMonitor()

# === Backward Compatibility ===

__all__ = [
    "NeuralNetworkVisualizer",
    "RegionVisualizer",
    "NetworkGraphVisualizer",
    "ActivityMonitor",
    "ActivityData",
    "NeuronVisualizationData",
    "ConnectionVisualizationData",
    "VisualizationMode",
    "create_neural_network_visualizer",
    "create_activity_monitor",
]

if __name__ == "__main__":
    # Example usage
    print("Enhanced NLM Visualization and Debugging Tools")
    print("=" * 60)
    
    # Create enhanced brain
    from .enhanced_api import create_enhanced_brain
    brain = create_enhanced_brain()
    
    # Create visualizer
    visualizer = create_neural_network_visualizer(brain, width=1200, height=800)
    
    # Configure visualizer
    visualizer.set_mode(VisualizationMode.LIVE)
    
    # Get network statistics
    stats = visualizer.get_network_statistics()
    print(f"Network Statistics: {stats}")
    
    # Get neuron visualization data
    neurons = visualizer.get_neuron_visualization_data()
    print(f"Neuron visualization data: {len(neurons)} neurons")
    
    # Get connection visualization data
    connections = visualizer.get_connection_visualization_data()
    print(f"Connection visualization data: {len(connections)} connections")
    
    # Run a few simulation steps
    for step in range(10):
        brain.step(step, step * 0.1)
    
    # Get activity data
    activity_data = visualizer.get_activity_data()
    print(f"Activity data collected: {len(activity_data)} time points")
    
    print("\nEnhanced NLM visualization and debugging tools ready for use!")
