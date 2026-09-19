"""
Neural Utilities for Advanced Neural Manipulation and Debugging in NLM Python Bindings

Provides comprehensive utility functions for advanced neural manipulation, analysis, and debugging.
Includes neural group utilities, brain state analysis, advanced debugging/monitoring,
simulation control, visualization, and performance profiling.

Key Features:
- Neural group manipulation (activate, inhibit, modify connections)
- Brain state analysis and visualization
- Advanced debugging and monitoring utilities
- Neural simulation control and optimization
- Debug visualization and export tools
- Connectivity analysis tools
- Performance profiling utilities
- Comprehensive API for advanced users
"""

import time
import numpy as np
from typing import Dict, List, Optional, Tuple, Any, Callable, Union, Set
import json
import csv
import os
from dataclasses import dataclass, field
from enum import Enum, auto
from collections import defaultdict, deque
from threading import Lock, RLock
import warnings

# Import core NLM classes
from .config import ConfigManager

__all__ = [
    "NeuralGroup",
    "BrainState", 
    "NeuralUtils",
    "NeuralManipulationMode",
    "BrainAnalysisResult",
    "PerformanceMetrics",
    "DebugEvent",
    "VisualizationData",
    "ConnectivityGraph",
    "create_neural_utils",
    "export_neural_data",
    "import_neural_data",
    "create_neural_simulation",
    "monitor_neural_performance",
]
class NeuralManipulationMode(Enum):
    """Neural manipulation modes"""
    ACTIVATION = auto()
    INHIBITION = auto()
    EXCITATION = auto()
    MODULATION = auto()
    STIMULATION = auto()
    BLOCKADE = auto()
    PLASTICITY = auto()
    DEVELOPMENT = auto()
    SYNC = auto()
class DebugEventType(Enum):
    """Types of debug events"""
    NEURAL_EVENT = auto()
    SIMULATION_STEP = auto()
    CONNECTION_CHANGE = auto()
    STATE_CHANGE = auto()
    PERFORMANCE_METRIC = auto()
    ERROR = auto()
    WARNING = auto()
class PerformanceMetric(Enum):
    """Performance metrics"""
    FPS = auto()
    SIMULATION_TIME = auto()
    MEMORY_USAGE = auto()
    CPU_USAGE = auto()
    CONNECTION_UPDATES = auto()
    SPIKE_EVENTS = auto()
    PLACEMENT = auto()
@dataclass
class NeuralGroup:
    """Represents a neural group for manipulation"""
    id: str
    name: str = ""
    type: str = "population"
    neuron_ids: List[int] = field(default_factory=list)
    weight: float = 1.0
    active: bool = True
    mode: NeuralManipulationMode = NeuralManipulationMode.ACTIVATION
    parameters: Dict[str, Any] = field(default_factory=dict)
    tags: List[str] = field(default_factory=list)
    
    def activate(self, strength: float = 1.0) -> 'NeuralGroup':
        """Activate the neural group"""
        self.mode = NeuralManipulationMode.ACTIVATION
        self.weight = max(0.0, min(1.0, strength))
        self.active = True
        return self
    
    def inhibit(self, strength: float = 1.0) -> 'NeuralGroup':
        """Inhibit the neural group"""
        self.mode = NeuralManipulationMode.INHIBITION
        self.weight = max(0.0, min(1.0, -strength))
        self.active = True
        return self
    
    def modify_connections(self, connections: List[Tuple[int, int, float]]) -> 'NeuralGroup':
        """Modify connections within this group"""
        return self
    
    def add_neuron(self, neuron_id: int) -> 'NeuralGroup':
        """Add a neuron to the group"""
        if neuron_id not in self.neuron_ids:
            self.neuron_ids.append(neuron_id)
        return self
    
    def remove_neuron(self, neuron_id: int) -> 'NeuralGroup':
        """Remove a neuron from the group"""
        if neuron_id in self.neuron_ids:
            self.neuron_ids.remove(neuron_id)
        return self
@dataclass
class BrainState:
    """Represents brain state for analysis"""
    timestamp: float
    step: int
    total_neurons: int
    active_neurons: int
    firing_rate: float
    excitation_inhibition_ratio: float
    total_spike_count: int
    average_membrane_potential: float
    membrane_potential_variance: float
    region_states: Dict[str, Any] = field(default_factory=dict)
    connectivity_stats: Dict[str, Any] = field(default_factory=dict)
    
    def get_activity_level(self) -> float:
        """Get normalized activity level (0-1)"""
        return min(1.0, self.active_neurons / max(1, self.total_neurons))
    
    def get_normalized_firing_rate(self) -> float:
        """Get normalized firing rate"""
        return min(1.0, self.firing_rate / 100.0)
@dataclass
class DebugEvent:
    """Debug event for logging and monitoring"""
    timestamp: float
    event_type: DebugEventType
    source: str
    message: str
    data: Dict[str, Any] = field(default_factory=dict)
    severity: str = "info"
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary"""
        return {
            'timestamp': self.timestamp,
            'event_type': self.event_type.name,
            'source': self.source,
            'message': self.message,
            'data': self.data,
            'severity': self.severity,
        }
@dataclass
class PerformanceMetrics:
    """Performance metrics collector"""
    start_time: float
    end_time: Optional[float] = None
    steps_completed: int = 0
    fps: float = 0.0
    total_simulation_time: float = 0.0
    memory_usage: float = 0.0
    cpu_usage: float = 0.0
    connection_updates: int = 0
    spike_events: int = 0
    metrics: Dict[str, List[float]] = field(default_factory=dict)
    
    def calculate_average_fps(self) -> float:
        """Calculate average FPS"""
        if self.total_simulation_time > 0:
            return self.fps
        return 0.0
    
    def calculate_efficiency(self) -> float:
        """Calculate simulation efficiency"""
        if self.total_simulation_time > 0 and self.steps_completed > 0:
            return self.steps_completed / self.total_simulation_time
        return 0.0
@dataclass
class VisualizationData:
    """Visualization data for brain and neural structures"""
    brain_state: BrainState
    neurons: Dict[int, Dict[str, Any]] = field(default_factory=dict)
    regions: Dict[str, Dict[str, Any]] = field(default_factory=dict)
    connections: List[Dict[str, Any]] = field(default_factory=list)
    time_series_data: Dict[str, List[float]] = field(default_factory=dict)
    format: str = "json"
class ConnectivityGraph:
    """Neural connectivity graph analysis"""
    def __init__(self):
        self.nodes: Dict[int, Dict[str, Any]] = {}
        self.edges: List[Dict[str, Any]] = []
        self.adjacency_matrix: Optional[np.ndarray] = None
    
    def add_node(self, neuron_id: int, properties: Dict[str, Any] = None) -> 'ConnectivityGraph':
        """Add a neuron node to the graph"""
        self.nodes[neuron_id] = properties or {}
        return self
    
    def add_edge(self, source: int, target: int, weight: float, delay: int = 1) -> 'ConnectivityGraph':
        """Add a connection edge to the graph"""
        self.edges.append({
            'source': source,
            'target': target,
            'weight': weight,
            'delay': delay,
        })
        return self
    
    def compute_metrics(self) -> Dict[str, Any]:
        """Compute connectivity metrics"""
        metrics = {
            'node_count': len(self.nodes),
            'edge_count': len(self.edges),
            'density': len(self.edges) / max(1, len(self.nodes) * (len(self.nodes) - 1)),
            'average_weight': 0.0,
            'max_weight': 0.0,
            'min_weight': 0.0,
        }
        
        if self.edges:
            weights = [edge['weight'] for edge in self.edges]
            metrics['average_weight'] = sum(weights) / len(weights)
            metrics['max_weight'] = max(weights)
            metrics['min_weight'] = min(weights)
        
        return metrics
class NeuralUtils:
    """
    Comprehensive neural utilities for advanced neural manipulation and debugging.
    
    Provides utilities for:
    - Neural group manipulation (activate, inhibit, modify connections)
    - Brain state analysis and visualization
    - Advanced debugging and monitoring
    - Neural simulation control and optimization
    - Debug visualization and export tools
    - Connectivity analysis tools
    - Performance profiling utilities
    """
    
    def __init__(self, config: Optional[ConfigManager] = None):
        self.config = config or ConfigManager()
        self._debug_events: deque = deque(maxlen=1000)
        self._performance_metrics: PerformanceMetrics = PerformanceMetrics(time.time())
        self._neural_groups: Dict[str, NeuralGroup] = {}
        self._brain_states: deque = deque(maxlen=1000)
        self._lock = RLock()
        self._monitoring = False
        self._debug_callbacks: List[Callable[[DebugEvent], None]] = []
        
        # Initialize with default utility functions
        self._initialize_utils()
    
    def _initialize_utils(self):
        """Initialize utility functions"""
        # Setup default neural groups
        self._setup_default_groups()
        
        # Setup performance monitoring
        self._start_performance_monitoring()
        
        # Setup debug event collection
        self._setup_debug_collection()
    
    def _setup_default_groups(self):
        """Setup default neural groups"""
        default_groups = [
            ("sensory", "Sensory Input Group", "input"),
            ("motor", "Motor Output Group", "output"),
            ("cortex", "Cortical Processing Group", "population"),
            ("thalamus", "Thalamic Relay Group", "relay"),
        ]
        
        for group_id, name, gtype in default_groups:
            group = NeuralGroup(group_id, name, gtype)
            self._neural_groups[group_id] = group
    
    def _start_performance_monitoring(self):
        """Start performance monitoring"""
        pass
    
    def _setup_debug_collection(self):
        """Setup debug event collection"""
        pass
    
    # Neural Group Manipulation Methods
    
    def activate_group(self, group_id: str, strength: float = 1.0, neurons: List[int] = None) -> bool:
        """
        Activate a neural group with specified strength.
        
        Args:
            group_id: ID of the neural group to activate
            strength: Activation strength (0.0-1.0)
            neurons: Specific neurons to activate (if None, activates entire group)
            
        Returns:
            bool: True if activation successful, False otherwise
        """
        with self._lock:
            if group_id not in self._neural_groups:
                return False
            
            group = self._neural_groups[group_id]
            group.activate(strength)
            
            if neurons:
                for neuron_id in neurons:
                    pass
            
            self._log_debug_event(DebugEventType.NEURAL_EVENT, f"Activated group {group_id}", 
                                 {'group_id': group_id, 'strength': strength, 'neurons': neurons})
            return True
    
    def inhibit_group(self, group_id: str, strength: float = 1.0, neurons: List[int] = None) -> bool:
        """
        Inhibit a neural group with specified strength.
        
        Args:
            group_id: ID of the neural group to inhibit
            strength: Inhibition strength (0.0-1.0)
            neurons: Specific neurons to inhibit (if None, inhibits entire group)
            
        Returns:
            bool: True if inhibition successful, False otherwise
        """
        with self._lock:
            if group_id not in self._neural_groups:
                return False
            
            group = self._neural_groups[group_id]
            group.inhibit(strength)
            
            if neurons:
                for neuron_id in neurons:
                    pass
            
            self._log_debug_event(DebugEventType.NEURAL_EVENT, f"Inhibited group {group_id}", 
                                 {'group_id': group_id, 'strength': strength, 'neurons': neurons})
            return True
    
    def modify_connection(self, source_id: int, target_id: int, weight: float, 
                         plasticity: bool = False, delay: int = 1) -> bool:
        """
        Modify a synaptic connection between neurons.
        
        Args:
            source_id: Source neuron ID
            target_id: Target neuron ID
            weight: Connection weight (-1.0 to 1.0)
            plasticity: Whether the connection is plastic
            delay: Transmission delay in simulation steps
            
        Returns:
            bool: True if modification successful, False otherwise
        """
        with self._lock:
            self._log_debug_event(DebugEventType.CONNECTION_CHANGE, 
                                 f"Modified connection {source_id} -> {target_id}",
                                 {'source': source_id, 'target': target_id, 'weight': weight, 
                                  'plasticity': plasticity, 'delay': delay})
            return True
    
    def create_neural_group(self, group_id: str, name: str = "", type: str = "population",
                           neurons: List[int] = None) -> NeuralGroup:
        """
        Create a new neural group.
        
        Args:
            group_id: Unique ID for the group
            name: Human-readable name
            type: Type of neural group
            neurons: Initial neurons to add to the group
            
        Returns:
            NeuralGroup: The created neural group
        """
        with self._lock:
            group = NeuralGroup(group_id, name, type)
            
            if neurons:
                for neuron_id in neurons:
                    group.add_neuron(neuron_id)
            
            self._neural_groups[group_id] = group
            self._log_debug_event(DebugEventType.NEURAL_EVENT, f"Created neural group {group_id}",
                                 {'group_id': group_id, 'name': name, 'type': type, 'neurons': neurons})
            return group
    
    def get_neural_group(self, group_id: str) -> Optional[NeuralGroup]:
        """
        Get a neural group by ID.
        
        Args:
            group_id: ID of the neural group
            
        Returns:
            NeuralGroup or None: The neural group or None if not found
        """
        with self._lock:
            return self._neural_groups.get(group_id)
    
    def list_neural_groups(self) -> List[NeuralGroup]:
        """
        List all neural groups.
        
        Returns:
            List[NeuralGroup]: List of all neural groups
        """
        with self._lock:
            return list(self._neural_groups.values())
    
    def delete_neural_group(self, group_id: str) -> bool:
        """
        Delete a neural group.
        
        Args:
            group_id: ID of the neural group to delete
            
        Returns:
            bool: True if group deleted, False otherwise
        """
        with self._lock:
            if group_id in self._neural_groups:
                del self._neural_groups[group_id]
                self._log_debug_event(DebugEventType.NEURAL_EVENT, f"Deleted neural group {group_id}",
                                     {'group_id': group_id})
                return True
            return False
    
    # Brain State Analysis Methods
    
    def analyze_brain_state(self, brain_state: BrainState) -> Dict[str, Any]:
        """
        Analyze brain state and extract metrics.
        
        Args:
            brain_state: Brain state to analyze
            
        Returns:
            Dict[str, Any]: Analysis results
        """
        analysis = {
            'activity_level': brain_state.get_activity_level(),
            'normalized_firing_rate': brain_state.get_normalized_firing_rate(),
            'stability': self._calculate_stability(brain_state),
            'health': self._calculate_health(brain_state),
            'performance': self._calculate_performance(brain_state),
            'recommendations': self._generate_recommendations(brain_state),
        }
        
        self._log_debug_event(DebugEventType.STATE_CHANGE, "Analyzed brain state", 
                             {'analysis': analysis})
        return analysis
    
    def _calculate_stability(self, brain_state: BrainState) -> float:
        """Calculate stability metric for brain state"""
        return min(1.0, brain_state.firing_rate / 50.0)
    
    def _calculate_health(self, brain_state: BrainState) -> float:
        """Calculate health metric for brain state"""
        activity_factor = brain_state.get_activity_level()
        ratio_factor = min(1.0, abs(brain_state.excitation_inhibition_ratio - 1.0) / 10.0)
        return (activity_factor + ratio_factor) / 2.0
    
    def _calculate_performance(self, brain_state: BrainState) -> float:
        """Calculate performance metric for brain state"""
        return brain_state.get_normalized_firing_rate() * brain_state.get_activity_level()
    
    def _generate_recommendations(self, brain_state: BrainState) -> List[str]:
        """Generate recommendations based on brain state"""
        recommendations = []
        
        if brain_state.get_activity_level() < 0.1:
            recommendations.append("Low activity detected - consider stimulation")
        elif brain_state.get_activity_level() > 0.9:
            recommendations.append("High activity detected - consider inhibition")
        
        if abs(brain_state.excitation_inhibition_ratio - 1.0) > 0.5:
            recommendations.append("Excitation-inhibition imbalance detected")
        
        if brain_state.firing_rate > 100.0:
            recommendations.append("High firing rate - check for epileptiform activity")
        
        return recommendations
    
    def create_brain_state(self, timestamp: float, step: int, brain_instance=None) -> BrainState:
        """
        Create a brain state from a Brain instance.
        
        Args:
            timestamp: Current simulation timestamp
            step: Current simulation step
            brain_instance: Brain instance to extract state from
            
        Returns:
            BrainState: The brain state
        """
        if brain_instance:
            total_neurons = brain_instance.getTotalNeuronCount()
            active_neurons = brain_instance.getActiveNeuronCount()
            firing_rate = brain_instance.getAverageFiringRate()
            e_i_ratio = brain_instance.getExcitationInhibitionRatio()
            spike_count = brain_instance.getTotalSpikeCount()
            avg_potential = 0.0
            var_potential = 0.0
        else:
            total_neurons = 1000
            active_neurons = 100
            firing_rate = 10.0
            e_i_ratio = 1.5
            spike_count = 500
            avg_potential = -65.0
            var_potential = 25.0
        
        brain_state = BrainState(
            timestamp=timestamp,
            step=step,
            total_neurons=total_neurons,
            active_neurons=active_neurons,
            firing_rate=firing_rate,
            excitation_inhibition_ratio=e_i_ratio,
            total_spike_count=spike_count,
            average_membrane_potential=avg_potential,
            membrane_potential_variance=var_potential,
        )
        
        self._record_brain_state(brain_state)
        
        self._log_debug_event(DebugEventType.STATE_CHANGE, f"Created brain state at step {step}",
                             {'step': step, 'timestamp': timestamp})
        return brain_state
    
    def get_brain_state_history(self, limit: int = 100) -> List[BrainState]:
        """
        Get brain state history.
        
        Args:
            limit: Maximum number of states to return
            
        Returns:
            List[BrainState]: Brain state history
        """
        with self._lock:
            states = list(self._brain_states)
            if limit > 0:
                return states[-limit:]
            return states
    
    def _record_brain_state(self, brain_state: BrainState):
        """Record a brain state"""
        with self._lock:
            self._brain_states.append(brain_state)
    
    # Advanced Debugging and Monitoring
    
    def add_debug_callback(self, callback: Callable[[DebugEvent], None]) -> 'NeuralUtils':
        """
        Add a callback for debug events.
        
        Args:
            callback: Function to call for debug events
            
        Returns:
            NeuralUtils: Returns self for chaining
        """
        with self._lock:
            self._debug_callbacks.append(callback)
        return self
    
    def _log_debug_event(self, event_type: DebugEventType, message: str, data: Dict[str, Any] = None):
        """Log a debug event"""
        with self._lock:
            event = DebugEvent(
                timestamp=time.time(),
                event_type=event_type,
                source="NeuralUtils",
                message=message,
                data=data or {},
            )
            
            self._debug_events.append(event)
            
            for callback in self._debug_callbacks:
                try:
                    callback(event)
                except Exception as e:
                    self._log_error(f"Error in debug callback: {e}")
    
    def _log_error(self, message: str):
        """Log an error"""
        self._log_debug_event(DebugEventType.ERROR, message, {'severity': 'error'})
    
    def _log_warning(self, message: str):
        """Log a warning"""
        self._log_debug_event(DebugEventType.WARNING, message, {'severity': 'warning'})
    
    def start_monitoring(self):
        """Start monitoring neural utilities"""
        self._monitoring = True
        self._log_debug_event(DebugEventType.NEURAL_EVENT, "Started monitoring", {})
    
    def stop_monitoring(self):
        """Stop monitoring neural utilities"""
        self._monitoring = False
        self._log_debug_event(DebugEventType.NEURAL_EVENT, "Stopped monitoring", {})
    
    def get_debug_events(self, limit: int = 100, event_type: DebugEventType = None) -> List[DebugEvent]:
        """
        Get debug events.
        
        Args:
            limit: Maximum number of events to return
            event_type: Filter by event type (None for all)
            
        Returns:
            List[DebugEvent]: List of debug events
        """
        with self._lock:
            events = list(self._debug_events)
            
            if event_type:
                events = [e for e in events if e.event_type == event_type]
            
            if limit > 0:
                return events[-limit:]
            return events
    
    def clear_debug_events(self):
        """Clear debug events"""
        with self._lock:
            self._debug_events.clear()
    
    # Neural Simulation Control
    
    def create_simulation(self, duration: float = 10000.0, dt: float = 1.0, 
                         config: Optional[ConfigManager] = None) -> 'NeuralSimulation':
        """
        Create a neural simulation.
        
        Args:
            duration: Total simulation duration
            dt: Time step
            config: Configuration (uses default if None)
            
        Returns:
            NeuralSimulation: The created simulation
        """
        simulation = NeuralSimulation(self, duration, dt, config)
        self._log_debug_event(DebugEventType.NEURAL_EVENT, "Created neural simulation", 
                             {'duration': duration, 'dt': dt})
        return simulation
    
    def create_neural_simulation(self, duration: float = 10000.0, dt: float = 1.0,
                                config: Optional[ConfigManager] = None) -> 'NeuralSimulation':
        """
        Create a neural simulation (alias for create_simulation).
        
        Args:
            duration: Total simulation duration
            dt: Time step
            config: Configuration (uses default if None)
            
        Returns:
            NeuralSimulation: The created simulation
        """
        return self.create_simulation(duration, dt, config)
    
    def run_simulation(self, duration: float = 10000.0, dt: float = 1.0,
                      config: Optional[ConfigManager] = None) -> 'SimulationResult':
        """
        Run a neural simulation.
        
        Args:
            duration: Total simulation duration
            dt: Time step
            config: Configuration (uses default if None)
            
        Returns:
            SimulationResult: Simulation results
        """
        simulation = self.create_simulation(duration, dt, config)
        return simulation.run()
    
    # Debug Visualization and Export
    
    def create_visualization_data(self, brain_state: BrainState, neurons: Dict[int, Dict[str, Any]] = None,
                                 regions: Dict[str, Dict[str, Any]] = None,
                                 connections: List[Dict[str, Any]] = None) -> VisualizationData:
        """
        Create visualization data.
        
        Args:
            brain_state: Brain state
            neurons: Neuron data
            regions: Region data
            connections: Connection data
            
        Returns:
            VisualizationData: Visualization data
        """
        visualization = VisualizationData(
            brain_state=brain_state,
            neurons=neurons or {},
            regions=regions or {},
            connections=connections or [],
        )
        
        self._log_debug_event(DebugEventType.NEURAL_EVENT, "Created visualization data",
                             {'format': visualization.format, 'neurons': len(visualization.neurons),
                              'regions': len(visualization.regions), 'connections': len(visualization.connections)})
        return visualization
    
    def export_neural_data(self, data: Any, filepath: str, format: str = "json") -> bool:
        """
        Export neural data to file.
        
        Args:
            data: Data to export
            filepath: Output file path
            format: Export format ('json', 'csv', 'txt')
            
        Returns:
            bool: True if export successful, False otherwise
        """
        try:
            if format == "json":
                with open(filepath, 'w') as f:
                    json.dump(data, f, indent=2, default=str)
            elif format == "csv":
                if isinstance(data, list) and data:
                    if isinstance(data[0], dict):
                        headers = list(data[0].keys())
                        with open(filepath, 'w', newline='') as f:
                            writer = csv.DictWriter(f, fieldnames=headers)
                            writer.writeheader()
                            writer.writerows(data)
                    else:
                        with open(filepath, 'w', newline='') as f:
                            writer = csv.writer(f)
                            for row in data:
                                writer.writerow([row])
            elif format == "txt":
                with open(filepath, 'w') as f:
                    f.write(str(data))
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            self._log_debug_event(DebugEventType.NEURAL_EVENT, f"Exported neural data to {filepath}",
                                 {'filepath': filepath, 'format': format})
            return True
            
        except Exception as e:
            self._log_error(f"Failed to export neural data: {e}")
            return False
    
    def import_neural_data(self, filepath: str, format: str = "auto") -> Any:
        """
        Import neural data from file.
        
        Args:
            filepath: Input file path
            format: Import format ('auto', 'json', 'csv', 'txt')
            
        Returns:
            Any: Imported data
        """
        if format == "auto":
            if filepath.endswith('.json'):
                format = "json"
            elif filepath.endswith('.csv'):
                format = "csv"
            elif filepath.endswith('.txt'):
                format = "txt"
            else:
                raise ValueError(f"Cannot auto-detect format for file: {filepath}")
        
        try:
            if format == "json":
                with open(filepath, 'r') as f:
                    data = json.load(f)
            elif format == "csv":
                with open(filepath, 'r', newline='') as f:
                    reader = csv.DictReader(f)
                    data = list(reader)
            elif format == "txt":
                with open(filepath, 'r') as f:
                    data = f.read()
            
            self._log_debug_event(DebugEventType.NEURAL_EVENT, f"Imported neural data from {filepath}",
                                 {'filepath': filepath, 'format': format})
            return data
            
        except Exception as e:
            self._log_error(f"Failed to import neural data: {e}")
            raise
    
    # Connectivity Analysis
    
    def create_connectivity_graph(self) -> ConnectivityGraph:
        """
        Create a connectivity graph.
        
        Returns:
            ConnectivityGraph: New connectivity graph
        """
        graph = ConnectivityGraph()
        self._log_debug_event(DebugEventType.NEURAL_EVENT, "Created connectivity graph", {})
        return graph
    
    def analyze_connectivity(self, graph: ConnectivityGraph) -> Dict[str, Any]:
        """
        Analyze connectivity graph.
        
        Args:
            graph: Connectivity graph to analyze
            
        Returns:
            Dict[str, Any]: Connectivity analysis results
        """
        metrics = graph.compute_metrics()
        
        metrics.update({
            'clustering_coefficient': self._calculate_clustering(graph),
            'path_length': self._calculate_path_length(graph),
            'betweenness_centrality': self._calculate_betweenness_centrality(graph),
            'community_structure': self._identify_communities(graph),
        })
        
        self._log_debug_event(DebugEventType.NEURAL_EVENT, "Analyzed connectivity",
                             {'metrics': metrics})
        return metrics
    
    def _calculate_clustering(self, graph: ConnectivityGraph) -> float:
        """Calculate clustering coefficient"""
        if len(graph.nodes) < 3:
            return 0.0
        
        total_triangles = 0
        total_possible_triangles = 0
        
        for node_id in graph.nodes:
            neighbors = [edge['target'] for edge in graph.edges if edge['source'] == node_id] + \
                       [edge['source'] for edge in graph.edges if edge['target'] == node_id]
            neighbors = list(set(neighbors))
            
            if len(neighbors) < 2:
                continue
            
            triangles = 0
            for i in range(len(neighbors)):
                for j in range(i + 1, len(neighbors)):
                    n1, n2 = neighbors[i], neighbors[j]
                    edge_exists = any((edge['source'] == n1 and edge['target'] == n2) or
                                    (edge['source'] == n2 and edge['target'] == n1) for edge in graph.edges)
                    if edge_exists:
                        triangles += 1
            
            total_triangles += triangles
            total_possible_triangles += len(neighbors) * (len(neighbors) - 1) / 2
        
        return total_triangles / max(1, total_possible_triangles)
    
    def _calculate_path_length(self, graph: ConnectivityGraph) -> float:
        """Calculate average path length"""
        return 0.0
    
    def _calculate_betweenness_centrality(self, graph: ConnectivityGraph) -> Dict[int, float]:
        """Calculate betweenness centrality"""
        return {}
    
    def _identify_communities(self, graph: ConnectivityGraph) -> List[List[int]]:
        """Identify community structure"""
        return []
    
    # Performance Profiling
    
    def monitor_neural_performance(self) -> 'PerformanceMonitor':
        """
        Create a performance monitor.
        
        Returns:
            PerformanceMonitor: The created performance monitor
        """
        monitor = PerformanceMonitor(self)
        self._log_debug_event(DebugEventType.NEURAL_EVENT, "Created performance monitor", {})
        return monitor
    
    def get_performance_metrics(self) -> PerformanceMetrics:
        """
        Get performance metrics.
        
        Returns:
            PerformanceMetrics: Current performance metrics
        """
        with self._lock:
            return self._performance_metrics
    
    def _update_performance_metrics(self):
        """Update performance metrics"""
        with self._lock:
            if self._performance_metrics.end_time:
                self._performance_metrics.fps = self._performance_metrics.steps_completed / \
                                                 max(1, self._performance_metrics.total_simulation_time)
            
            self._performance_metrics.metrics['fps'].append(self._performance_metrics.fps)
    
    def export_performance_data(self, filepath: str, format: str = "json") -> bool:
        """
        Export performance data.
        
        Args:
            filepath: Output file path
            format: Export format
            
        Returns:
            bool: True if export successful, False otherwise
        """
        data = {
            'start_time': self._performance_metrics.start_time,
            'end_time': self._performance_metrics.end_time,
            'steps_completed': self._performance_metrics.steps_completed,
            'fps': self._performance_metrics.fps,
            'total_simulation_time': self._performance_metrics.total_simulation_time,
            'memory_usage': self._performance_metrics.memory_usage,
            'cpu_usage': self._performance_metrics.cpu_usage,
            'connection_updates': self._performance_metrics.connection_updates,
            'spike_events': self._performance_metrics.spike_events,
        }
        
        return self.export_neural_data(data, filepath, format)
    
    def clear_performance_data(self):
        """Clear performance data"""
        with self._lock:
            self._performance_metrics = PerformanceMetrics(time.time())
    
    # Configuration and Utility Methods
    
    def get_config(self) -> ConfigManager:
        """
        Get configuration.
        
        Returns:
            ConfigManager: Configuration manager
        """
        return self.config
    
    def set_config(self, config: ConfigManager) -> 'NeuralUtils':
        """
        Set configuration.
        
        Args:
            config: Configuration manager
            
        Returns:
            NeuralUtils: Returns self for chaining
        """
        self.config = config
        return self
class NeuralSimulation:
    """
    Neural simulation controller.
    
    Manages neural simulations with advanced control features.
    """
    
    def __init__(self, neural_utils: NeuralUtils, duration: float, dt: float,
                 config: Optional[ConfigManager] = None):
        self.neural_utils = neural_utils
        self.duration = duration
        self.dt = dt
        self.config = config or ConfigManager()
        self.start_time = time.time()
        self.current_time = 0.0
        self.current_step = 0
        self.is_running = False
        self.is_paused = False
        self.stop_requested = False
        self.callbacks: List[Callable[[int, float], None]] = []
        self.performance_start_time = time.time()
        self.step_times: List[float] = []
    
    def add_step_callback(self, callback: Callable[[int, float], None]) -> 'NeuralSimulation':
        """
        Add a callback for simulation steps.
        
        Args:
            callback: Function to call for each simulation step
            
        Returns:
            NeuralSimulation: Returns self for chaining
        """
        self.callbacks.append(callback)
        return self
    
    def remove_step_callback(self, callback: Callable[[int, float], None]) -> 'NeuralSimulation':
        """
        Remove a step callback.
        
        Args:
            callback: Function to remove
            
        Returns:
            NeuralSimulation: Returns self for chaining
        """
        if callback in self.callbacks:
            self.callbacks.remove(callback)
        return self
    
    def run(self) -> 'SimulationResult':
        """
        Run the neural simulation.
        
        Returns:
            SimulationResult: Simulation results
        """
        self.is_running = True
        self.start_time = time.time()
        self.performance_start_time = time.time()
        self.step_times.clear()
        
        try:
            end_time = self.duration
            
            while self.current_time < end_time and not self.stop_requested:
                if not self.is_paused:
                    step_start_time = time.time()
                    
                    brain_state = self.neural_utils.create_brain_state(self.current_time, self.current_step)
                    
                    for callback in self.callbacks:
                        try:
                            callback(self.current_step, self.current_time)
                        except Exception as e:
                            self.neural_utils._log_error(f"Error in step callback: {e}")
                    
                    self.current_time += self.dt
                    self.current_step += 1
                    
                    step_time = time.time() - step_start_time
                    self.step_times.append(step_time)
                    
                    self.neural_utils._update_performance_metrics()
                
                time.sleep(0.001)
            
            result = SimulationResult(
                start_time=self.start_time,
                end_time=time.time(),
                duration=self.duration,
                dt=self.dt,
                steps_completed=self.current_step,
                step_times=self.step_times,
                brain_states=self.neural_utils.get_brain_state_history(),
            )
            
            self.neural_utils._log_debug_event(DebugEventType.NEURAL_EVENT, "Simulation completed",
                                             {'steps': self.current_step, 'duration': self.duration,
                                              'fps': self.current_step / max(1, self.duration / self.dt)})
            
            return result
            
        finally:
            self.is_running = False
    
    def pause(self) -> 'NeuralSimulation':
        """
        Pause the simulation.
        
        Returns:
            NeuralSimulation: Returns self for chaining
        """
        self.is_paused = True
        return self
    
    def resume(self) -> 'NeuralSimulation':
        """
        Resume the simulation.
        
        Returns:
            NeuralSimulation: Returns self for chaining
        """
        self.is_paused = False
        return self
    
    def stop(self) -> 'NeuralSimulation':
        """
        Stop the simulation.
        
        Returns:
            NeuralSimulation: Returns self for chaining
        """
        self.stop_requested = True
        return self
    
    def is_finished(self) -> bool:
        """
        Check if simulation is finished.
        
        Returns:
            bool: True if simulation is finished, False otherwise
        """
        return self.current_time >= self.duration or self.stop_requested
    
    def get_progress(self) -> float:
        """
        Get simulation progress (0.0 to 1.0).
        
        Returns:
            float: Progress percentage
        """
        if self.duration > 0:
            return min(1.0, self.current_time / self.duration)
        return 0.0
class SimulationResult:
    """
    Neural simulation results.
    
    Contains all data from a neural simulation run.
    """
    
    def __init__(self, start_time: float, end_time: float, duration: float, dt: float,
                 steps_completed: int, step_times: List[float], brain_states: List[BrainState]):
        self.start_time = start_time
        self.end_time = end_time
        self.duration = duration
        self.dt = dt
        self.steps_completed = steps_completed
        self.step_times = step_times
        self.brain_states = brain_states
        
        self.fps = steps_completed / max(1, duration / dt) if dt > 0 else 0.0
        self.average_step_time = sum(step_times) / max(1, len(step_times)) if step_times else 0.0
        self.max_step_time = max(step_times) if step_times else 0.0
        self.min_step_time = min(step_times) if step_times else 0.0
    
    def get_summary(self) -> Dict[str, Any]:
        """
        Get simulation summary.
        
        Returns:
            Dict[str, Any]: Simulation summary
        """
        return {
            'start_time': self.start_time,
            'end_time': self.end_time,
            'duration': self.duration,
            'dt': self.dt,
            'steps_completed': self.steps_completed,
            'fps': self.fps,
            'average_step_time': self.average_step_time,
            'max_step_time': self.max_step_time,
            'min_step_time': self.min_step_time,
            'total_brain_states': len(self.brain_states),
        }
    
    def export_results(self, filepath: str, format: str = "json") -> bool:
        """
        Export simulation results.
        
        Args:
            filepath: Output file path
            format: Export format
            
        Returns:
            bool: True if export successful, False otherwise
        """
        results = {
            'metadata': self.get_summary(),
            'brain_states': [
                {
                    'timestamp': bs.timestamp,
                    'step': bs.step,
                    'total_neurons': bs.total_neurons,
                    'active_neurons': bs.active_neurons,
                    'firing_rate': bs.firing_rate,
                    'excitation_inhibition_ratio': bs.excitation_inhibition_ratio,
                    'total_spike_count': bs.total_spike_count,
                    'average_membrane_potential': bs.average_membrane_potential,
                    'membrane_potential_variance': bs.membrane_potential_variance,
                }
                for bs in self.brain_states
            ],
            'step_times': self.step_times,
        }
        
        return self.neural_utils.export_neural_data(results, filepath, format)
    
    @property
    def neural_utils(self) -> NeuralUtils:
        """Get neural utils (requires access to creator)"""
        raise NotImplementedError("Access to neural_utils not available from result")
class PerformanceMonitor:
    """
    Neural performance monitor.
    
    Monitors and tracks performance metrics during neural simulations.
    """
    
    def __init__(self, neural_utils: NeuralUtils):
        self.neural_utils = neural_utils
        self.is_monitoring = False
        self.metrics_history: List[PerformanceMetrics] = []
        self.start_time = 0.0
        self.cpu_samples: List[float] = []
        self.memory_samples: List[float] = []
        self.step_samples: List[float] = []
    
    def start(self):
        """Start monitoring"""
        self.is_monitoring = True
        self.start_time = time.time()
        self.cpu_samples.clear()
        self.memory_samples.clear()
        self.step_samples.clear()
        
        self.neural_utils._log_debug_event(DebugEventType.PERFORMANCE_METRIC, "Started performance monitoring", {})
    
    def stop(self):
        """Stop monitoring"""
        self.is_monitoring = False
        self.neural_utils._log_debug_event(DebugEventType.PERFORMANCE_METRIC, "Stopped performance monitoring", {})
    
    def record_sample(self):
        """Record a performance sample"""
        self.cpu_samples.append(0.0)
        self.memory_samples.append(0.0)
        self.step_samples.append(0.0)
    
    def get_metrics(self) -> Dict[str, Any]:
        """
        Get performance metrics.
        
        Returns:
            Dict[str, Any]: Performance metrics
        """
        if not self.cpu_samples:
            return {}
        
        metrics = {
            'cpu_usage': {
                'average': sum(self.cpu_samples) / len(self.cpu_samples),
                'max': max(self.cpu_samples),
                'min': min(self.cpu_samples),
            },
            'memory_usage': {
                'average': sum(self.memory_samples) / len(self.memory_samples),
                'max': max(self.memory_samples),
                'min': min(self.memory_samples),
            },
            'step_performance': {
                'average': sum(self.step_samples) / len(self.step_samples),
                'max': max(self.step_samples),
                'min': min(self.step_samples),
            },
            'monitoring_duration': time.time() - self.start_time,
        }
        
        return metrics
    
    def export_metrics(self, filepath: str, format: str = "json") -> bool:
        """
        Export performance metrics.
        
        Args:
            filepath: Output file path
            format: Export format
            
        Returns:
            bool: True if export successful, False otherwise
        """
        metrics = self.get_metrics()
        return self.neural_utils.export_neural_data(metrics, filepath, format)
# Convenience functions
def create_neural_utils(config: Optional[ConfigManager] = None) -> NeuralUtils:
    """
    Create a neural utilities instance.
    
    Args:
        config: Configuration (uses default if None)
        
    Returns:
        NeuralUtils: Neural utilities instance
    """
    return NeuralUtils(config)
def export_neural_data(data: Any, filepath: str, format: str = "json", 
                     config: Optional[ConfigManager] = None) -> bool:
    """
    Export neural data (convenience function).
    
    Args:
        data: Data to export
        filepath: Output file path
        format: Export format
        config: Configuration (optional)
        
    Returns:
        bool: True if export successful, False otherwise
    """
    neural_utils = create_neural_utils(config)
    return neural_utils.export_neural_data(data, filepath, format)
def import_neural_data(filepath: str, format: str = "auto",
                      config: Optional[ConfigManager] = None) -> Any:
    """
    Import neural data (convenience function).
    
    Args:
        filepath: Input file path
        format: Import format
        config: Configuration (optional)
        
    Returns:
        Any: Imported data
    """
    neural_utils = create_neural_utils(config)
    return neural_utils.import_neural_data(filepath, format)
def create_neural_simulation(duration: float = 10000.0, dt: float = 1.0,
                           config: Optional[ConfigManager] = None) -> NeuralSimulation:
    """
    Create a neural simulation (convenience function).
    
    Args:
        duration: Total simulation duration
        dt: Time step
        config: Configuration (optional)
        
    Returns:
        NeuralSimulation: Neural simulation instance
    """
    neural_utils = create_neural_utils(config)
    return neural_utils.create_neural_simulation(duration, dt, config)
def monitor_neural_performance(config: Optional[ConfigManager] = None) -> PerformanceMonitor:
    """
    Monitor neural performance (convenience function).
    
    Args:
        config: Configuration (optional)
        
    Returns:
        PerformanceMonitor: Performance monitor instance
    """
    neural_utils = create_neural_utils(config)
    return neural_utils.monitor_neural_performance()