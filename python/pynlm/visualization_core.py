"""
Core visualization components for NLM system.

This module provides the foundational visualization capabilities including:
- Neural Network structure and activity visualization
- Spike train time-series visualization
- Memory system visualization
- Plasticity tracking visualization
- Development stage visualization
- Neuromodulation level visualization
"""

from pynlm.interface import ScriptingInterface
from pynlm.plugin_system import Plugin, PluginInfo, PluginType
from pynlm.monitoring import MonitoringInterface
from pynlm.visualization import PlotManager, VisualizationBackend

import asyncio
import time
import threading
from typing import Dict, List, Any, Optional, Tuple, Callable
from dataclasses import dataclass, field
from enum import Enum
import numpy as np
import matplotlib.pyplot as plt
import plotly.graph_objects as go
import plotly.subplots as sp
from abc import ABC, abstractmethod

# Color schemes for visualization
COLOR_SCHEMES = {
    'neural': {
        'exciting': '#FF6B6B',
        'inhibitory': '#4ECDC4',
        'active': '#FFE66D',
        'resting': '#95E1D3',
        'firing': '#FF8C69'
    },
    'memory': {
        'working': '#FFD93D',
        'episodic': '#6BCF7F',
        'conceptual': '#4D9DE0',
        'spatial': '#FF6B9D'
    },
    'plasticity': {
        'strengthening': '#2ECC71',
        'weakening': '#E74C3C',
        'stable': '#F39C12',
        'new_connection': '#3498DB'
    },
    'development': {
        'stage1': '#E67E22',
        'stage2': '#9B59B6',
        'stage3': '#34495E',
        'stage4': '#1ABC9C',
        'stage5': '#D35400'
    }
}
class VisualizationComponent(ABC):
    """Base class for visualization components"""
    
    def __init__(self, name: str, config: Dict[str, Any] = None):
        self.name = name
        self.config = config or {}
        self.enabled = True
        self.last_update = 0
        self.data_cache = {}
        self.update_listeners = []
    
    @abstractmethod
    def update(self, data: Dict[str, Any]) -> Any:
        """Update visualization with new data"""
        pass
    
    @abstractmethod
    def render(self) -> Any:
        """Render visualization"""
        pass
    
    def add_update_listener(self, listener: Callable):
        """Add listener for update events"""
        self.update_listeners.append(listener)
    
    def remove_update_listener(self, listener: Callable):
        """Remove update listener"""
        if listener in self.update_listeners:
            self.update_listeners.remove(listener)
    
    def notify_listeners(self, data: Dict[str, Any]):
        """Notify all listeners of update"""
        for listener in self.update_listeners:
            try:
                listener(data)
            except Exception:
                pass
class NeuralNetworkVisualizer(VisualizationComponent):
    """Visualize neural network structure and activity"""
    
    def __init__(self, interface: ScriptingInterface, config: Dict[str, Any] = None):
        super().__init__("NeuralNetworkVisualizer", config)
        self.interface = interface
        self.context = interface.context
        self.neuron_positions = {}
        self.connection_weights = {}
        self.activity_patterns = {}
        self.region_colors = {}
        self.layout_type = config.get('layout_type', 'force_directed')
    
    def update(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Update neural network visualization with new data"""
        current_time = time.time()
        self.last_update = current_time
        
        # Update neuron positions if layout changed
        if 'neuron_positions' in data:
            self.neuron_positions = data['neuron_positions']
        
        # Update connection weights
        if 'connection_weights' in data:
            self.connection_weights = data['connection_weights']
        
        # Update activity patterns
        if 'activity_patterns' in data:
            self.activity_patterns = data['activity_patterns']
        
        # Update region colors
        if 'region_colors' in data:
            self.region_colors = data['region_colors']
        
        # Update neuron data for rendering
        self._update_neuron_data(data)
        
        # Calculate layout if needed
        self._calculate_layout()
        
        # Notify listeners
        self.notify_listeners({
            'type': 'neural_network_update',
            'timestamp': current_time,
            'data': data
        })
        
        return {
            'neuron_count': len(self.neuron_positions),
            'connection_count': len(self.connection_weights),
            'activity_count': sum(1 for activity in self.activity_patterns.values() if activity > 0),
            'update_time': current_time
        }
    
    def _update_neuron_data(self, data: Dict[str, Any]):
        """Update internal neuron data structures"""
        self.neuron_data = []
        
        if not self.context.brain:
            return
        
        brain = self.context.brain
        regions = brain.getRegions()
        
        for region in regions:
            region_id = region.getId()
            neurons = region.getNeurons()
            
            for neuron in neurons:
                neuron_id = neuron.getId()
                position = self.neuron_positions.get(neuron_id, {'x': 0, 'y': 0, 'z': 0})
                
                neuron_info = {
                    'id': neuron_id,
                    'region_id': region_id,
                    'type': neuron.getType(),
                    'position': position,
                    'activity': neuron.getFiringState(),
                    'potential': neuron.getMembranePotential(),
                    'firing_rate': neuron.getFiringRate(),
                    'connections': [],
                    'synapse_count': len(neuron.getSynapses())
                }
                
                # Get connections
                synapses = neuron.getSynapses()
                for synapse in synapses:
                    target_neuron_id = synapse.getTargetId()
                    weight = synapse.getStrength()
                    neuron_info['connections'].append({
                        'target_id': target_neuron_id,
                        'weight': weight,
                        'type': 'excitatory' if weight > 0 else 'inhibitory'
                    })
                
                self.neuron_data.append(neuron_info)
    
    def _calculate_layout(self):
        """Calculate neuron positions for visualization"""
        if self.layout_type == 'force_directed':
            self._force_directed_layout()
        elif self.layout_type == 'circular':
            self._circular_layout()
        elif self.layout_type == 'hierarchical':
            self._hierarchical_layout()
    
    def _force_directed_layout(self):
        """Calculate force-directed layout for neurons"""
        # Implementation would use physics simulation
        # For now, create a simple grid layout
        neurons_by_region = {}
        
        for neuron in self.neuron_data:
            region_id = neuron['region_id']
            if region_id not in neurons_by_region:
                neurons_by_region[region_id] = []
            neurons_by_region[region_id].append(neuron)
        
        # Assign positions in a grid pattern
        for region_id, neurons in neurons_by_region.items():
            rows = int(np.ceil(np.sqrt(len(neurons))))
            cols = int(np.ceil(len(neurons) / rows))
            
            for i, neuron in enumerate(neurons):
                row = i // cols
                col = i % cols
                neuron['position'] = {
                    'x': col * 2.0,
                    'y': row * 2.0,
                    'z': 0,
                    'region_x': col * 1.5,
                    'region_y': row * 1.5
                }
    
    def _circular_layout(self):
        """Calculate circular layout for neurons"""
        # Implementation would arrange neurons in circular patterns
        # For now, use radial distance based on activity
        for neuron in self.neuron_data:
            activity = neuron.get('activity', 0)
            radius = 0.5 + activity * 3.0
            angle = neuron.get('id', 0) * (2 * np.pi / len(self.neuron_data))
            
            neuron['position'] = {
                'x': radius * np.cos(angle),
                'y': radius * np.sin(angle),
                'z': 0
            }
    
    def _hierarchical_layout(self):
        """Calculate hierarchical layout for neurons"""
        # Implementation would create layered hierarchy
        # For now, use region-based positioning
        regions = list(set(neuron['region_id'] for neuron in self.neuron_data))
        
        for i, neuron in enumerate(self.neuron_data):
            region_index = regions.index(neuron['region_id'])
            neuron['position'] = {
                'x': region_index * 5.0,
                'y': i * 1.0,
                'z': 0
            }
    
    def render(self) -> Dict[str, Any]:
        """Render neural network visualization"""
        if not self.neuron_data:
            return {'type': 'empty', 'message': 'No neural data available'}
        
        # Generate different visualization types
        visualizations = {}
        
        # Static network structure
        visualizations['network_structure'] = self._render_network_structure()
        
        # Activity heatmap
        visualizations['activity_heatmap'] = self._render_activity_heatmap()
        
        # Connection strength visualization
        visualizations['connections'] = self._render_connections()
        
        # Regional activity distribution
        visualizations['regional_activity'] = self._render_regional_activity()
        
        # Interactive neuron network
        visualizations['interactive_network'] = self._render_interactive_network()
        
        # Topology analysis
        visualizations['topology'] = self._render_topology_analysis()
        
        return {
            'type': 'neural_network',
            'timestamp': time.time(),
            'data': visualizations,
            'neuron_count': len(self.neuron_data),
            'connection_count': sum(len(n['connections']) for n in self.neuron_data)
        }
    
    def _render_network_structure(self) -> Dict[str, Any]:
        """Render network structure visualization"""
        nodes = []
        edges = []
        
        # Add neurons as nodes
        for neuron in self.neuron_data:
            node_type = neuron['type']
            activity = neuron['activity']
            
            # Determine node color based on activity
            if activity == 'Firing':
                color = COLOR_SCHEMES['neural']['firing']
            elif activity == 'Active':
                color = COLOR_SCHEMES['neural']['active']
            elif activity == 'Refractory':
                color = COLOR_SCHEMES['neural']['refractory']
            else:
                color = COLOR_SCHEMES['neural']['resting']
            
            nodes.append({
                'id': neuron['id'],
                'label': f"N{neuron['id']}",
                'x': neuron['position']['x'],
                'y': neuron['position']['y'],
                'z': neuron['position']['z'],
                'color': color,
                'size': 10 + neuron['synapse_count'] * 0.5,
                'region': neuron['region_id'],
                'type': node_type,
                'activity': activity
            })
            
            # Add connections as edges
            for conn in neuron['connections']:
                edges.append({
                    'source': neuron['id'],
                    'target': conn['target_id'],
                    'weight': conn['weight'],
                    'type': conn['type'],
                    'color': '#2E86AB' if conn['type'] == 'excitatory' else '#A23B72'
                })
        
        return {
            'type': 'network_graph',
            'nodes': nodes,
            'edges': edges,
            'layout': self.layout_type,
            'physics': True
        }
    
    def _render_activity_heatmap(self) -> Dict[str, Any]:
        """Render activity heatmap visualization"""
        regions = list(set(neuron['region_id'] for neuron in self.neuron_data))
        
        # Calculate activity for each region
        region_activity = {}
        for region_id in regions:
            region_neurons = [n for n in self.neuron_data if n['region_id'] == region_id]
            if region_neurons:
                activities = [n['activity'] for n in region_neurons]
                # Convert activity to numeric values
                activity_scores = []
                for activity in activities:
                    if activity == 'Firing':
                        score = 3
                    elif activity == 'Active':
                        score = 2
                    elif activity == 'Refractory':
                        score = 1
                    else:
                        score = 0
                    activity_scores.append(score)
                
                region_activity[region_id] = {
                    'average_activity': np.mean(activity_scores) if activity_scores else 0,
                    'max_activity': max(activity_scores) if activity_scores else 0,
                    'neuron_count': len(region_neurons),
                    'firing_neuron_count': sum(1 for a in activity_scores if a == 3)
                }
        
        return {
            'type': 'heatmap',
            'data': region_activity,
            'regions': regions,
            'color_scale': 'viridis'
        }
    
    def _render_connections(self) -> Dict[str, Any]:
        """Render connection strength visualization"""
        # Aggregate connection statistics
        total_connections = len(self.neuron_data) * 2  # Approximate
        excitatory_connections = sum(1 for neuron in self.neuron_data 
                                    for conn in neuron['connections'] if conn['type'] == 'excitatory')
        inhibitory_connections = sum(1 for neuron in self.neuron_data 
                                    for conn in neuron['connections'] if conn['type'] == 'inhibitory')
        
        # Calculate weight statistics
        weights = []
        for neuron in self.neuron_data:
            for conn in neuron['connections']:
                weights.append(abs(conn['weight']))
        
        return {
            'type': 'connections',
            'statistics': {
                'total_connections': total_connections,
                'excitatory': excitatory_connections,
                'inhibitory': inhibitory_connections,
                'excitatory_ratio': excitatory_connections / max(total_connections, 1),
                'average_weight': np.mean(weights) if weights else 0,
                'max_weight': np.max(weights) if weights else 0,
                'min_weight': np.min(weights) if weights else 0,
                'weight_std': np.std(weights) if weights else 0
            },
            'weight_distribution': {
                'strong': len([w for w in weights if w > 0.1]),
                'moderate': len([w for w in weights if 0.01 <= w <= 0.1]),
                'weak': len([w for w in weights if w < 0.01])
            }
        }
    
    def _render_regional_activity(self) -> Dict[str, Any]:
        """Render regional activity distribution"""
        regions = list(set(neuron['region_id'] for neuron in self.neuron_data))
        
        # Prepare regional data
        regional_data = []
        for region_id in regions:
            region_neurons = [n for n in self.neuron_data if n['region_id'] == region_id]
            
            # Calculate metrics
            neuron_count = len(region_neurons)
            if neuron_count > 0:
                activity_levels = []
                for neuron in region_neurons:
                    if neuron['activity'] == 'Firing':
                        activity_levels.append(3)
                    elif neuron['activity'] == 'Active':
                        activity_levels.append(2)
                    elif neuron['activity'] == 'Refractory':
                        activity_levels.append(1)
                    else:
                        activity_levels.append(0)
                
                regional_data.append({
                    'region_id': region_id,
                    'neuron_count': neuron_count,
                    'average_activity': np.mean(activity_levels),
                    'max_activity': max(activity_levels),
                    'activity_distribution': {
                        'resting': sum(1 for a in activity_levels if a == 0),
                        'refractory': sum(1 for a in activity_levels if a == 1),
                        'active': sum(1 for a in activity_levels if a == 2),
                        'firing': sum(1 for a in activity_levels if a == 3)
                    }
                })
        
        return {
            'type': 'regional_activity',
            'data': regional_data,
            'total_regions': len(regions),
            'total_neurons': sum(r['neuron_count'] for r in regional_data)
        }
    
    def _render_interactive_network(self) -> Dict[str, Any]:
        """Render interactive network visualization"""
        return {
            'type': 'interactive_network',
            'mode': 'hover_to_highlight',
            'features': [
                'neuron_click_highlight',
                'connection_hover_tooltip',
                'region_filter',
                'activity_slider',
                'zoom_and_pan',
                'node_drag_reposition'
            ],
            'controls': {
                'layout_options': ['force_directed', 'circular', 'hierarchical'],
                'node_size_by': ['activity', 'connections', 'region'],
                'edge_width_by': ['weight', 'type'],
                'color_by': ['activity', 'region', 'type']
            }
        }
    
    def _render_topology_analysis(self) -> Dict[str, Any]:
        """Render topology analysis visualization"""
        # Calculate network topology metrics
        if len(self.neuron_data) < 2:
            return {'type': 'topology', 'message': 'Insufficient neurons for topology analysis'}
        
        # Degree distribution
        degrees = []
        for neuron in self.neuron_data:
            degree = len(neuron['connections'])
            degrees.append(degree)
        
        # Clustering coefficients (simplified)
        clustering_coeffs = []
        # For each neuron, count connections among its neighbors
        for neuron in self.neuron_data:
            if len(neuron['connections']) >= 2:
                neighbor_ids = [conn['target_id'] for conn in neuron['connections']]
                mutual_connections = 0
                
                for i, neighbor1 in enumerate(neighbor_ids):
                    for neighbor2 in neighbor_ids[i+1:]:
                        # Check if neighbor1 and neighbor2 are connected
                        for conn in self.neuron_data[neighbor1]['connections']:
                            if conn['target_id'] == neighbor2:
                                mutual_connections += 1
                                break
                
                clustering = (2 * mutual_connections) / (len(neighbor_ids) * (len(neighbor_ids) - 1)) if len(neighbor_ids) > 1 else 0
                clustering_coeffs.append(clustering)
        
        return {
            'type': 'topology',
            'metrics': {
                'average_degree': np.mean(degrees) if degrees else 0,
                'max_degree': np.max(degrees) if degrees else 0,
                'min_degree': np.min(degrees) if degrees else 0,
                'degree_std': np.std(degrees) if degrees else 0,
                'average_clustering': np.mean(clustering_coeffs) if clustering_coeffs else 0,
                'network_density': (sum(degrees) * 2) / (len(self.neuron_data) * (len(self.neuron_data) - 1)) if len(self.neuron_data) > 1 else 0,
                'small_world_coefficient': 0.5,  # Simplified
                'scale_free_coefficient': 0.3   # Simplified
            },
            'distribution_curves': {
                'degree_distribution': degrees,
                'clustering_distribution': clustering_coeffs
            }
        }
class SpikeTrainVisualizer(VisualizationComponent):
    """Visualize neural spiking activity as time-series"""
    
    def __init__(self, interface: ScriptingInterface, config: Dict[str, Any] = None):
        super().__init__("SpikeTrainVisualizer", config)
        self.interface = interface
        self.spike_trains = {}
        self.time_window = config.get('time_window', 60.0)  # seconds
        self.sampling_rate = config.get('sampling_rate', 1000)  # Hz
        self.max_neurons = config.get('max_neurons', 100)
        self.spike_detection_threshold = config.get('spike_detection_threshold', 20.0)
    
    def update(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Update spike train visualization with new data"""
        current_time = time.time()
        self.last_update = current_time
        
        # Update spike trains
        if 'spike_trains' in data:
            self.spike_trains = data['spike_trains']
        
        # Process spike data
        if 'spikes' in data:
            self._process_spike_data(data['spikes'], current_time)
        
        # Calculate statistics
        stats = self._calculate_statistics()
        
        # Notify listeners
        self.notify_listeners({
            'type': 'spike_train_update',
            'timestamp': current_time,
            'statistics': stats
        })
        
        return {
            'spike_count': sum(len(trains) for trains in self.spike_trains.values()),
            'active_neurons': len([trains for trains in self.spike_trains.values() if trains]),
            'last_update': current_time,
            'statistics': stats
        }
    
    def _process_spike_data(self, spikes: List[Dict[str, Any]], current_time: float):
        """Process spike data for visualization"""
        # Limit number of neurons for performance
        if len(self.spike_trains) > self.max_neurons:
            # Keep only most active neurons
            neuron_activity = {}
            for neuron_id, trains in self.spike_trains.items():
                neuron_activity[neuron_id] = len(trains)
            
            # Sort by activity and keep top neurons
            sorted_neurons = sorted(neuron_activity.items(), key=lambda x: x[1], reverse=True)
            top_neurons = {neuron_id for neuron_id, _ in sorted_neurons[:self.max_neurons]}
            
            # Filter spike trains
            filtered_trains = {}
            for neuron_id, trains in self.spike_trains.items():
                if neuron_id in top_neurons:
                    filtered_trains[neuron_id] = trains
            
            self.spike_trains = filtered_trains
        
        # Add new spikes
        for spike in spikes:
            neuron_id = spike.get('neuron_id')
            spike_time = spike.get('time', current_time)
            
            if neuron_id not in self.spike_trains:
                self.spike_trains[neuron_id] = []
            
            self.spike_trains[neuron_id].append({
                'time': spike_time,
                'amplitude': spike.get('amplitude', 1.0),
                'width': spike.get('width', 0.5),
                'type': spike.get('type', 'regular')
            })
        
        # Clean old spikes (outside time window)
        cutoff_time = current_time - self.time_window
        for neuron_id in list(self.spike_trains.keys()):
            self.spike_trains[neuron_id] = [
                spike for spike in self.spike_trains[neuron_id]
                if spike['time'] > cutoff_time
            ]
            
            if not self.spike_trains[neuron_id]:
                del self.spike_trains[neuron_id]
    
    def _calculate_statistics(self) -> Dict[str, Any]:
        """Calculate spike train statistics"""
        if not self.spike_trains:
            return {}
        
        all_spikes = []
        for trains in self.spike_trains.values():
            all_spikes.extend(trains)
        
        if not all_spikes:
            return {}
        
        # Temporal statistics
        spike_times = [spike['time'] for spike in all_spikes]
        
        # Inter-spike intervals
        spike_times_sorted = sorted(spike_times)
        inter_spike_intervals = []
        for i in range(1, len(spike_times_sorted)):
            interval = spike_times_sorted[i] - spike_times_sorted[i-1]
            inter_spike_intervals.append(interval)
        
        # Per-neuron statistics
        per_neuron_stats = {}
        for neuron_id, trains in self.spike_trains.items():
            if trains:
                times = [spike['time'] for spike in trains]
                times_sorted = sorted(times)
                
                if len(times_sorted) >= 2:
                    isi = []
                    for i in range(1, len(times_sorted)):
                        isi.append(times_sorted[i] - times_sorted[i-1])
                    
                    per_neuron_stats[neuron_id] = {
                        'spike_count': len(trains),
                        'firing_rate': len(trains) / self.time_window,
                        'average_isi': np.mean(isi) if isi else 0,
                        'isi_std': np.std(isi) if isi else 0,
                        'cv_isi': (np.std(isi) / np.mean(isi)) if isi and np.mean(isi) > 0 else 0
                    }
                else:
                    per_neuron_stats[neuron_id] = {
                        'spike_count': len(trains),
                        'firing_rate': len(trains) / self.time_window,
                        'average_isi': 0,
                        'isi_std': 0,
                        'cv_isi': 0
                    }
        
        return {
            'total_spikes': len(all_spikes),
            'active_neurons': len(self.spike_trains),
            'overall_firing_rate': len(all_spikes) / self.time_window,
            'mean_isi': np.mean(inter_spike_intervals) if inter_spike_intervals else 0,
            'isi_std': np.std(inter_spike_intervals) if inter_spike_intervals else 0,
            'cv_isi': (np.std(inter_spike_intervals) / np.mean(inter_spike_intervals)) 
                     if inter_spike_intervals and np.mean(inter_spike_intervals) > 0 else 0,
            'per_neuron_stats': per_neuron_stats,
            'time_window': self.time_window,
            'sampling_rate': self.sampling_rate
        }
    
    def render(self) -> Dict[str, Any]:
        """Render spike train visualization"""
        if not self.spike_trains:
            return {'type': 'empty', 'message': 'No spike data available'}
        
        # Generate different visualization types
        visualizations = {}
        
        # Raster plot
        visualizations['raster_plot'] = self._render_raster_plot()
        
        # Peri-stimulus time histogram
        visualizations['pst_histogram'] = self._render_pst_histogram()
        
        # Inter-spike interval distribution
        visualizations['isi_distribution'] = self._render_isi_distribution()
        
        # Firing rate over time
        visualizations['firing_rate_over_time'] = self._render_firing_rate_over_time()
        
        # Spike shape analysis
        visualizations['spike_shapes'] = self._render_spike_shapes()
        
        # Population activity
        visualizations['population_activity'] = self._render_population_activity()
        
        # Burst detection
        visualizations['burst_analysis'] = self._render_burst_analysis()
        
        return {
            'type': 'spike_train',
            'timestamp': time.time(),
            'data': visualizations,
            'statistics': self._calculate_statistics()
        }
    
    def _render_raster_plot(self) -> Dict[str, Any]:
        """Render raster plot visualization"""
        # Prepare data for raster plot
        neurons = list(self.spike_trains.keys())
        spike_events = []
        
        for neuron_id, trains in self.spike_trains.items():
            for spike in trains:
                spike_events.append({
                    'neuron_id': neuron_id,
                    'time': spike['time'],
                    'amplitude': spike['amplitude']
                })
        
        # Sort by time
        spike_events.sort(key=lambda x: x['time'])
        
        # Calculate neuron positions for display
        neuron_y_positions = {}
        for i, neuron_id in enumerate(neurons):
            neuron_y_positions[neuron_id] = i
        
        return {
            'type': 'raster',
            'spike_events': spike_events,
            'neurons': {
                neuron_id: {
                    'y_position': neuron_y_positions[neuron_id],
                    'firing_rate': len(trains) / self.time_window
                }
                for neuron_id, trains in self.spike_trains.items()
            },
            'time_window': self.time_window,
            'neuron_count': len(neurons),
            'spike_count': len(spike_events)
        }
    
    def _render_pst_histogram(self) -> Dict[str, Any]:
        """Render peri-stimulus time histogram"""
        # Calculate PSTH around a reference event
        if not self.spike_trains:
            return {'type': 'pst_histogram', 'data': []}
        
        # Use the first neuron's spikes as reference
        reference_spikes = []
        for neuron_id, trains in self.spike_trains.items():
            reference_spikes.extend([(spike['time'], neuron_id) for spike in trains])
        
        if not reference_spikes:
            return {'type': 'pst_histogram', 'data': []}
        
        reference_times = sorted([t for t, _ in reference_spikes])
        
        # Bin spikes around each reference time
        bin_width = 0.01  # 10 ms bins
        time_range = 1.0  # 1 second around reference
        num_bins = int(time_range / bin_width)
        
        # Calculate PSTH
        psth = np.zeros(num_bins)
        bin_counts = np.zeros(num_bins)
        
        for ref_time in reference_times:
            for spike_time, neuron_id in reference_spikes:
                delta_time = spike_time - ref_time
                if -time_range/2 <= delta_time < time_range/2:
                    bin_index = int((delta_time + time_range/2) / bin_width)
                    if 0 <= bin_index < num_bins:
                        bin_counts[bin_index] += 1
        
        # Smooth the PSTH
        window_size = min(5, num_bins // 10)
        if window_size > 1:
            weights = np.ones(window_size) / window_size
            psth = np.convolve(bin_counts, weights, mode='same')
        
        # Convert to firing rate
        total_spikes = len(reference_spikes)
        for i in range(num_bins):
            psth[i] = (psth[i] * 1000) / (total_spikes * bin_width)  # spikes per second
        
        time_axis = np.arange(-time_range/2, time_range/2, bin_width)
        
        return {
            'type': 'pst_histogram',
            'time_axis': time_axis.tolist(),
            'psth': psth.tolist(),
            'bin_width': bin_width,
            'reference_times': reference_times,
            'total_spikes': total_spikes
        }
    
    def _render_isi_distribution(self) -> Dict[str, Any]:
        """Render inter-spike interval distribution"""
        if not self.spike_trains:
            return {'type': 'isi_distribution', 'data': []}
        
        # Collect all ISIs
        all_isi = []
        for neuron_id, trains in self.spike_trains.items():
            if len(trains) >= 2:
                times = sorted([spike['time'] for spike in trains])
                for i in range(1, len(times)):
                    isi = times[i] - times[i-1]
                    all_isi.append(isi)
        
        if not all_isi:
            return {'type': 'isi_distribution', 'data': []}
        
        # Calculate histogram
        bins = np.linspace(0, max(all_isi) * 1.1, 50)
        hist, bin_edges = np.histogram(all_isi, bins=bins)
        
        # Calculate statistics
        stats = {
            'mean_isi': np.mean(all_isi),
            'median_isi': np.median(all_isi),
            'std_isi': np.std(all_isi),
            'cv_isi': np.std(all_isi) / np.mean(all_isi),
            'min_isi': np.min(all_isi),
            'max_isi': np.max(all_isi),
            'isi_count': len(all_isi)
        }
        
        # Identify bursting patterns
        burst_threshold = stats['mean_isi'] * 2
        burst_count = sum(1 for isi in all_isi if isi < burst_threshold)
        
        return {
            'type': 'isi_distribution',
            'hist': hist.tolist(),
            'bin_edges': bin_edges.tolist(),
            'isi_values': all_isi,
            'statistics': stats,
            'burst_threshold': burst_threshold,
            'burst_count': burst_count,
            'burst_percentage': (burst_count / len(all_isi)) * 100 if all_isi else 0
        }
    
    def _render_firing_rate_over_time(self) -> Dict[str, Any]:
        """Render firing rate over time"""
        if not self.spike_trains:
            return {'type': 'firing_rate', 'data': []}
        
        # Calculate firing rate using rolling window
        current_time = time.time()
        window_size = self.time_window * 0.5  # 50% of time window
        
        # Collect all spikes
        all_spikes = []
        for neuron_id, trains in self.spike_trains.items():
            for spike in trains:
                all_spikes.append(spike['time'])
        
        if not all_spikes:
            return {'type': 'firing_rate', 'data': []}
        
        # Calculate rolling firing rate
        times = sorted(all_spikes)
        max_time = max(times)
        min_time = min(times)
        
        # Create time bins
        num_bins = int(window_size / (max_time - min_time) * 1000)
        num_bins = max(10, min(num_bins, 1000))
        
        bin_width = (max_time - min_time) / num_bins
        
        firing_rates = []
        time_axis = []
        
        for i in range(num_bins):
            bin_start = min_time + i * bin_width
            bin_end = bin_start + bin_width
            
            # Count spikes in this bin
            spike_count = sum(1 for t in times if bin_start <= t < bin_end)
            
            # Calculate firing rate (spikes per second)
            rate = (spike_count / bin_width) if bin_width > 0 else 0
            
            firing_rates.append(rate)
            time_axis.append(bin_start)
        
        return {
            'type': 'firing_rate',
            'time_axis': time_axis,
            'firing_rates': firing_rates,
            'window_size': window_size,
            'bin_width': bin_width,
            'total_spikes': len(times),
            'time_range': max_time - min_time
        }
    
    def _render_spike_shapes(self) -> Dict[str, Any]:
        """Render spike shape analysis"""
        # Collect spike shapes from all neurons
        all_spikes = []
        for neuron_id, trains in self.spike_trains.items():
            for spike in trains:
                all_spikes.append(spike)
        
        if not all_spikes:
            return {'type': 'spike_shapes', 'data': []}
        
        # Group spikes by type for analysis
        spike_types = {}
        for spike in all_spikes:
            spike_type = spike.get('type', 'regular')
            if spike_type not in spike_types:
                spike_types[spike_type] = []
            spike_types[spike_type].append(spike)
        
        # Analyze spike shapes
        spike_analysis = {}
        for spike_type, spikes in spike_types.items():
            if spikes:
                amplitudes = [spike['amplitude'] for spike in spikes]
                widths = [spike['width'] for spike in spikes]
                
                spike_analysis[spike_type] = {
                    'count': len(spikes),
                    'mean_amplitude': np.mean(amplitudes),
                    'std_amplitude': np.std(amplitudes),
                    'mean_width': np.mean(widths),
                    'std_width': np.std(widths),
                    'amplitude_width_correlation': np.corrcoef(
                        amplitudes, widths
                    )[0, 1] if len(amplitudes) > 1 else 0
                }
        
        return {
            'type': 'spike_shapes',
            'spike_types': spike_analysis,
            'total_spikes': len(all_spikes),
            'unique_types': list(spike_types.keys())
        }
    
    def _render_population_activity(self) -> Dict[str, Any]:
        """Render population activity visualization"""
        if not self.spike_trains:
            return {'type': 'population_activity', 'data': []}
        
        # Calculate population activity over time
        current_time = time.time()
        time_bins = int(self.time_window / 0.1)  # 100ms bins
        
        # Initialize activity array
        activity = np.zeros(time_bins)
        
        # Collect all spikes
        all_spikes = []
        for neuron_id, trains in self.spike_trains.items():
            for spike in trains:
                all_spikes.append(spike['time'])
        
        if not all_spikes:
            return {'type': 'population_activity', 'data': []}
        
        # Bin spikes into time bins
        for spike_time in all_spikes:
            if current_time - spike_time < self.time_window:
                bin_index = int((spike_time - (current_time - self.time_window)) / 0.1)
                if 0 <= bin_index < time_bins:
                    activity[bin_index] += 1
        
        # Calculate firing rate
        firing_rate = (activity * 10) / self.time_window  # spikes per second
        
        # Calculate statistics
        stats = {
            'mean_activity': np.mean(activity),
            'max_activity': np.max(activity),
            'std_activity': np.std(activity),
            'cv_activity': (np.std(activity) / np.mean(activity)) if np.mean(activity) > 0 else 0,
            'total_spikes': len(all_spikes),
            'firing_rate_mean': np.mean(firing_rate),
            'firing_rate_std': np.std(firing_rate)
        }
        
        return {
            'type': 'population_activity',
            'activity': activity.tolist(),
            'firing_rate': firing_rate.tolist(),
            'time_bins': time_bins,
            'bin_width': 0.1,
            'current_time': current_time,
            'statistics': stats
        }
    
    def _render_burst_analysis(self) -> Dict[str, Any]:
        """Render burst analysis visualization"""
        if not self.spike_trains:
            return {'type': 'burst_analysis', 'data': []}
        
        # Collect all ISIs
        all_isi = []
        neuron_isi = {}
        
        for neuron_id, trains in self.spike_trains.items():
            if len(trains) >= 2:
                times = sorted([spike['time'] for spike in trains])
                neuron_isi_list = []
                for i in range(1, len(times)):
                    isi = times[i] - times[i-1]
                    neuron_isi_list.append(isi)
                    all_isi.append(isi)
                neuron_isi[neuron_id] = neuron_isi_list
        
        if not all_isi:
            return {'type': 'burst_analysis', 'data': []}
        
        # Detect bursts using threshold
        mean_isi = np.mean(all_isi)
        std_isi = np.std(all_isi)
        burst_threshold = mean_isi * 2.5  # 2.5 times mean ISI indicates burst
        
        # Classify ISIs as burst or not
        is_burst = [isi < burst_threshold for isi in all_isi]
        
        # Analyze bursts
        burst_analysis = {}
        for neuron_id, isi_list in neuron_isi.items():
            if isi_list:
                neuron_bursts = sum(1 for isi in isi_list if isi < burst_threshold)
                burst_rate = neuron_bursts / len(isi_list)
                
                burst_analysis[neuron_id] = {
                    'isi_count': len(isi_list),
                    'burst_count': neuron_bursts,
                    'burst_rate': burst_rate,
                    'mean_isi': np.mean(isi_list),
                    'cv_isi': (np.std(isi_list) / np.mean(isi_list)) if np.mean(isi_list) > 0 else 0
                }
        
        # Global burst statistics
        total_bursts = sum(is_burst)
        global_burst_rate = (total_bursts / len(all_isi)) * 100
        
        return {
            'type': 'burst_analysis',
            'isi_values': all_isi,
            'is_burst': is_burst,
            'burst_threshold': burst_threshold,
            'mean_isi': mean_isi,
            'std_isi': std_isi,
            'total_isi': len(all_isi),
            'total_bursts': total_bursts,
            'global_burst_rate': global_burst_rate,
            'neuron_analysis': burst_analysis,
            'high_activity_neurons': [nid for nid, stats in burst_analysis.items() 
                                    if stats['burst_rate'] > 0.5]
        }
class MemoryVisualizer(VisualizationComponent):
    """Visualize memory system contents and activities"""
    
    def __init__(self, interface: ScriptingInterface, config: Dict[str, Any] = None):
        super().__init__("MemoryVisualizer", config)
        self.interface = interface
        self.context = interface.context
        self.memory_contents = {}
        self.memory_activities = {}
        self.memory_types = config.get('memory_types', ['working', 'episodic', 'conceptual'])
        self.visualization_mode = config.get('visualization_mode', 'spatial')
    
    def update(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Update memory visualization with new data"""
        current_time = time.time()
        self.last_update = current_time
        
        # Update memory contents
        if 'memory_contents' in data:
            self.memory_contents = data['memory_contents']
        
        # Update memory activities
        if 'memory_activities' in data:
            self.memory_activities = data['memory_activities']
        
        # Update memory type visibility
        if 'memory_types' in data:
            self.memory_types = data['memory_types']
        
        # Process memory data
        self._process_memory_data(data)
        
        # Notify listeners
        self.notify_listeners({
            'type': 'memory_update',
            'timestamp': current_time,
            'data': data
        })
        
        return {
            'memory_type_count': len(self.memory_types),
            'content_count': sum(len(contents) for contents in self.memory_contents.values()),
            'activity_count': sum(len(activities) for activities in self.memory_activities.values()),
            'last_update': current_time
        }
    
    def _process_memory_data(self, data: Dict[str, Any]):
        """Process memory data for visualization"""
        # Process memory contents for each type
        for memory_type in self.memory_types:
            if memory_type in self.memory_contents:
                contents = self.memory_contents[memory_type]
                
                # Calculate statistics for this memory type
                if memory_type == 'working':
                    self._process_working_memory(contents)
                elif memory_type == 'episodic':
                    self._process_episodic_memory(contents)
                elif memory_type == 'conceptual':
                    self._process_conceptual_memory(contents)
                elif memory_type == 'spatial':
                    self._process_spatial_memory(contents)
                elif memory_type == 'temporal':
                    self._process_temporal_memory(contents)
    
    def _process_working_memory(self, contents: List[Dict[str, Any]]):
        """Process working memory contents"""
        self.working_memory_data = {
            'item_count': len(contents),
            'average_active_duration': np.mean([c.get('duration', 0) for c in contents]) if contents else 0,
            'total_items': sum(c.get('item_count', 1) for c in contents),
            'most_recent_item': max([c.get('timestamp', 0) for c in contents]) if contents else 0,
            'activity_level': sum(1 for c in contents if c.get('active', False))
        }
    
    def _process_episodic_memory(self, contents: List[Dict[str, Any]]):
        """Process episodic memory contents"""
        self.episodic_memory_data = {
            'event_count': len(contents),
            'time_range': {
                'start': min([c.get('timestamp', 0) for c in contents]) if contents else 0,
                'end': max([c.get('timestamp', 0) for c in contents]) if contents else 0
            },
            'average_event_strength': np.mean([c.get('strength', 0) for c in contents]) if contents else 0,
            'most_recent_event': max([c.get('timestamp', 0) for c in contents]) if contents else 0,
            'unique_contexts': len(set([c.get('context', '') for c in contents]))
        }
    
    def _process_conceptual_memory(self, contents: List[Dict[str, Any]]):
        """Process conceptual memory contents"""
        self.conceptual_memory_data = {
            'concept_count': len(contents),
            'average_stability': np.mean([c.get('stability', 0) for c in contents]) if contents else 0,
            'total_associations': sum([c.get('association_count', 0) for c in contents]),
            'concepts_with_high_stability': sum(1 for c in contents if c.get('stability', 0) > 0.7)
        }
    
    def _process_spatial_memory(self, contents: List[Dict[str, Any]]):
        """Process spatial memory contents"""
        self.spatial_memory_data = {
            'location_count': len(contents),
            'average_accuracy': np.mean([c.get('accuracy', 0) for c in contents]) if contents else 0,
            'coverage_area': {
                'width': max([c.get('width', 0) for c in contents]) if contents else 0,
                'height': max([c.get('height', 0) for c in contents]) if contents else 0
            }
        }
    
    def _process_temporal_memory(self, contents: List[Dict[str, Any]]):
        """Process temporal memory contents"""
        self.temporal_memory_data = {
            'sequence_count': len(contents),
            'average_sequence_length': np.mean([c.get('sequence_length', 0) for c in contents]) if contents else 0,
            'temporal_relationships': sum([c.get('temporal_relationships', 0) for c in contents]),
            'before_after_pairs': sum([c.get('before_after_count', 0) for c in contents])
        }
    
    def render(self) -> Dict[str, Any]:
        """Render memory visualization"""
        if not self.memory_contents and not self.memory_activities:
            return {'type': 'empty', 'message': 'No memory data available'}
        
        # Generate different visualization types
        visualizations = {}
        
        # Memory type distribution
        visualizations['memory_distribution'] = self._render_memory_distribution()
        
        # Memory content visualization
        visualizations['memory_contents'] = self._render_memory_contents()
        
        # Memory activity patterns
        visualizations['memory_activities'] = self._render_memory_activities()
        
        # Memory timeline
        visualizations['memory_timeline'] = self._render_memory_timeline()
        
        # Memory relationships
        visualizations['memory_relationships'] = self._render_memory_relationships()
        
        # Memory stability analysis
        visualizations['memory_stability'] = self._render_memory_stability()
        
        return {
            'type': 'memory',
            'timestamp': time.time(),
            'data': visualizations,
            'memory_types': self.memory_types,
            'content_count': sum(len(contents) for contents in self.memory_contents.values()),
            'activity_count': sum(len(activities) for activities in self.memory_activities.values())
        }
    
    def _render_memory_distribution(self) -> Dict[str, Any]:
        """Render memory type distribution visualization"""
        if not self.memory_contents:
            return {'type': 'distribution', 'data': []}
        
        # Calculate memory distribution
        memory_stats = {}
        total_content = 0
        
        for memory_type, contents in self.memory_contents.items():
            if memory_type in self.memory_types:
                count = len(contents)
                total_content += count
                
                memory_stats[memory_type] = {
                    'count': count,
                    'percentage': (count / total_content) * 100 if total_content > 0 else 0,
                    'color': COLOR_SCHEMES['memory'].get(memory_type, '#808080')
                }
        
        return {
            'type': 'distribution',
            'memory_stats': memory_stats,
            'total_content': total_content,
            'has_data': len(memory_stats) > 0
        }
    
    def _render_memory_contents(self) -> Dict[str, Any]:
        """Render memory contents visualization"""
        # Create a comprehensive view of memory contents
        contents_overview = {}
        
        for memory_type in self.memory_types:
            if memory_type in self.memory_contents and self.memory_contents[memory_type]:
                contents = self.memory_contents[memory_type]
                
                if memory_type == 'working':
                    contents_overview[memory_type] = self._render_working_memory_contents(contents)
                elif memory_type == 'episodic':
                    contents_overview[memory_type] = self._render_episodic_memory_contents(contents)
                elif memory_type == 'conceptual':
                    contents_overview[memory_type] = self._render_conceptual_memory_contents(contents)
                elif memory_type == 'spatial':
                    contents_overview[memory_type] = self._render_spatial_memory_contents(contents)
                elif memory_type == 'temporal':
                    contents_overview[memory_type] = self._render_temporal_memory_contents(contents)
        
        return {
            'type': 'contents',
            'contents_overview': contents_overview,
            'total_memory_types': len(self.memory_types),
            'has_memory_data': len(contents_overview) > 0
        }
    
    def _render_working_memory_contents(self, contents: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Render working memory contents"""
        return {
            'type': 'working_memory',
            'items': contents,
            'current_active': sum(1 for c in contents if c.get('active', False)),
            'average_duration': np.mean([c.get('duration', 0) for c in contents]) if contents else 0,
            'item_details': [{
                'id': c.get('id', 0),
                'content': c.get('content', ''),
                'active': c.get('active', False),
                'duration': c.get('duration', 0),
                'strength': c.get('strength', 0)
            } for c in contents]
        }
    
    def _render_episodic_memory_contents(self, contents: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Render episodic memory contents"""
        return {
            'type': 'episodic_memory',
            'events': contents,
            'time_range': {
                'start': min([c.get('timestamp', 0) for c in contents]) if contents else 0,
                'end': max([c.get('timestamp', 0) for c in contents]) if contents else 0
            },
            'event_details': [{
                'id': c.get('id', 0),
                'timestamp': c.get('timestamp', 0),
                'event': c.get('event', ''),
                'context': c.get('context', ''),
                'strength': c.get('strength', 0)
            } for c in contents]
        }
    
    def _render_conceptual_memory_contents(self, contents: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Render conceptual memory contents"""
        return {
            'type': 'conceptual_memory',
            'concepts': contents,
            'concept_details': [{
                'id': c.get('id', 0),
                'concept': c.get('concept', ''),
                'stability': c.get('stability', 0),
                'association_count': c.get('association_count', 0),
                'properties': c.get('properties', [])
            } for c in contents]
        }
    
    def _render_spatial_memory_contents(self, contents: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Render spatial memory contents"""
        return {
            'type': 'spatial_memory',
            'locations': contents,
            'location_details': [{
                'id': c.get('id', 0),
                'name': c.get('name', ''),
                'x': c.get('x', 0),
                'y': c.get('y', 0),
                'width': c.get('width', 0),
                'height': c.get('height', 0),
                'accuracy': c.get('accuracy', 0)
            } for c in contents]
        }
    
    def _render_temporal_memory_contents(self, contents: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Render temporal memory contents"""
        return {
            'type': 'temporal_memory',
            'sequences': contents,
            'sequence_details': [{
                'id': c.get('id', 0),
                'sequence': c.get('sequence', ''),
                'length': c.get('sequence_length', 0),
                'temporal_relationships': c.get('temporal_relationships', 0)
            } for c in contents]
        }
    
    def _render_memory_activities(self) -> Dict[str, Any]:
        """Render memory activities visualization"""
        if not self.memory_activities:
            return {'type': 'activities', 'data': []}
        
        # Prepare activity data
        activities_data = {}
        for memory_type, activities in self.memory_activities.items():
            if memory_type in self.memory_types:
                activities_data[memory_type] = {
                    'activity_count': len(activities),
                    'activity_events': activities,
                    'average_activity': np.mean([a.get('activity', 0) for a in activities]) if activities else 0,
                    'max_activity': max([a.get('activity', 0) for a in activities]) if activities else 0
                }
        
        return {
            'type': 'activities',
            'activities_data': activities_data,
            'has_activity_data': len(activities_data) > 0
        }
    
    def _render_memory_timeline(self) -> Dict[str, Any]:
        """Render memory timeline visualization"""
        # Combine all memory events into a timeline
        timeline_events = []
        
        for memory_type in self.memory_types:
            if memory_type in self.memory_contents:
                for content in self.memory_contents[memory_type]:
                    timeline_events.append({
                        'type': memory_type,
                        'timestamp': content.get('timestamp', 0),
                        'description': content.get('description', ''),
                        'strength': content.get('strength', 0)
                    })
        
        # Sort by timestamp
        timeline_events.sort(key=lambda x: x['timestamp'])
        
        return {
            'type': 'timeline',
            'events': timeline_events,
            'time_range': {
                'start': min([e['timestamp'] for e in timeline_events]) if timeline_events else 0,
                'end': max([e['timestamp'] for e in timeline_events]) if timeline_events else 0
            },
            'event_count': len(timeline_events)
        }
    
    def _render_memory_relationships(self) -> Dict[str, Any]:
        """Render memory relationships visualization"""
        if not self.memory_contents:
            return {'type': 'relationships', 'data': []}
        
        # Build relationship graph
        nodes = []
        edges = []
        
        for memory_type in self.memory_types:
            if memory_type in self.memory_contents:
                for i, content1 in enumerate(self.memory_contents[memory_type]):
                    # Add node for content
                    nodes.append({
                        'id': f"{memory_type}_{i}",
                        'label': content1.get('description', f"Item {i}"),
                        'type': memory_type,
                        'strength': content1.get('strength', 0)
                    })
                    
                    # Find relationships with other contents
                    for j, content2 in enumerate(self.memory_contents[memory_type]):
                        if i < j:
                            # Check if there are relationships between content1 and content2
                            relationship_strength = 0
                            if memory_type == 'conceptual' and 'related_concepts' in content1:
                                if content2.get('id') in content1.get('related_concepts', []):
                                    relationship_strength = content1.get('related_concepts_strength', {}).get(
                                        content2.get('id'), 0
                                    )
                            
                            if relationship_strength > 0:
                                edges.append({
                                    'source': f"{memory_type}_{i}",
                                    'target': f"{memory_type}_{j}",
                                    'strength': relationship_strength,
                                    'type': 'concept_relationship'
                                })
        
        return {
            'type': 'relationships',
            'nodes': nodes,
            'edges': edges,
            'node_count': len(nodes),
            'edge_count': len(edges)
        }
    
    def _render_memory_stability(self) -> Dict[str, Any]:
        """Render memory stability analysis"""
        if not self.memory_contents:
            return {'type': 'stability', 'data': []}
        
        # Analyze stability across all memory types
        stability_data = {}
        
        for memory_type in self.memory_types:
            if memory_type in self.memory_contents:
                contents = self.memory_contents[memory_type]
                
                if memory_type == 'conceptual':
                    stabilities = [c.get('stability', 0) for c in contents]
                    if stabilities:
                        stability_data[memory_type] = {
                            'mean_stability': np.mean(stabilities),
                            'std_stability': np.std(stabilities),
                            'high_stability_count': sum(1 for s in stabilities if s > 0.7),
                            'medium_stability_count': sum(1 for s in stabilities if 0.4 <= s <= 0.7),
                            'low_stability_count': sum(1 for s in stabilities if s < 0.4),
                            'stability_values': stabilities
                        }
        
        return {
            'type': 'stability',
            'stability_data': stability_data,
            'has_stability_data': len(stability_data) > 0
        }
class PlasticityVisualizer(VisualizationComponent):
    """Visualize synaptic weight changes and learning"""
    
    def __init__(self, interface: ScriptingInterface, config: Dict[str, Any] = None):
        super().__init__("PlasticityVisualizer", config)
        self.interface = interface
        self.context = interface.context
        self.plasticity_events = {}
        self.weight_changes = {}
        self.plasticity_rules = {}
        self.learning_curves = {}
        self.plasticity_metrics = {}
        self.time_window = config.get('time_window', 3600.0)  # seconds
    
    def update(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Update plasticity visualization with new data"""
        current_time = time.time()
        self.last_update = current_time
        
        # Update plasticity events
        if 'plasticity_events' in data:
            self.plasticity_events = data['plasticity_events']
        
        # Update weight changes
        if 'weight_changes' in data:
            self.weight_changes = data['weight_changes']
        
        # Update plasticity rules
        if 'plasticity_rules' in data:
            self.plasticity_rules = data['plasticity_rules']
        
        # Process plasticity data
        self._process_plasticity_data(data)
        
        # Calculate metrics
        self._calculate_metrics()
        
        # Notify listeners
        self.notify_listeners({
            'type': 'plasticity_update',
            'timestamp': current_time,
            'data': data
        })
        
        return {
            'plasticity_event_count': sum(len(events) for events in self.plasticity_events.values()),
            'weight_change_count': len(self.weight_changes),
            'rule_count': len(self.plasticity_rules),
            'last_update': current_time,
            'metrics': self.plasticity_metrics
        }
    
    def _process_plasticity_data(self, data: Dict[str, Any]):
        """Process plasticity data for visualization"""
        # Filter recent events (within time window)
        cutoff_time = time.time() - self.time_window
        
        for event_type, events in self.plasticity_events.items():
            self.plasticity_events[event_type] = [
                event for event in events
                if event.get('timestamp', 0) > cutoff_time
            ]
    
    def _calculate_metrics(self):
        """Calculate plasticity metrics"""
        if not self.plasticity_events and not self.weight_changes:
            self.plasticity_metrics = {}
            return
        
        metrics = {
            'total_events': sum(len(events) for events in self.plasticity_events.values()),
            'weight_change_magnitude': sum(abs(change.get('magnitude', 0)) 
                                           for change in self.weight_changes.values()),
            'learning_rate': 0.0,
            'stability_index': 0.0,
            'convergence_rate': 0.0
        }
        
        # Calculate learning rate
        if 'weight_changes' in self.weight_changes:
            changes = self.weight_changes['weight_changes']
            if changes:
                magnitudes = [abs(c.get('magnitude', 0)) for c in changes]
                metrics['learning_rate'] = np.mean(magnitudes)
        
        # Calculate stability index
        if 'plasticity_events' in self.plasticity_events:
            events = self.plasticity_events['plasticity_events']
            if events:
                # Stability based on variance of weight changes
                weights = [e.get('weight_change', 0) for e in events]
                if weights:
                    metrics['stability_index'] = 1.0 - (np.std(weights) / (np.mean(weights) + 1e-10))
        
        self.plasticity_metrics = metrics
    
    def render(self) -> Dict[str, Any]:
        """Render plasticity visualization"""
        if not self.plasticity_events and not self.weight_changes:
            return {'type': 'empty', 'message': 'No plasticity data available'}
        
        # Generate different visualization types
        visualizations = {}
        
        # Plasticity events timeline
        visualizations['plasticity_events'] = self._render_plasticity_events()
        
        # Weight change distribution
        visualizations['weight_changes'] = self._render_weight_changes()
        
        # Learning curves
        visualizations['learning_curves'] = self._render_learning_curves()
        
        # Plasticity rule effectiveness
        visualizations['rule_effectiveness'] = self._render_rule_effectiveness()
        
        # Synaptic strengthening and weakening
        visualizations['synaptic_dynamics'] = self._render_synaptic_dynamics()
        
        # Plasticity efficiency
        visualizations['plasticity_efficiency'] = self._render_plasticity_efficiency()
        
        return {
            'type': 'plasticity',
            'timestamp': time.time(),
            'data': visualizations,
            'metrics': self.plasticity_metrics,
            'event_count': sum(len(events) for events in self.plasticity_events.values()),
            'weight_change_count': len(self.weight_changes)
        }
    
    def _render_plasticity_events(self) -> Dict[str, Any]:
        """Render plasticity events timeline"""
        if not self.plasticity_events:
            return {'type': 'events', 'data': []}
        
        # Prepare events data
        events_data = []
        for event_type, events in self.plasticity_events.items():
            for event in events:
                events_data.append({
                    'type': event_type,
                    'timestamp': event.get('timestamp', 0),
                    'magnitude': event.get('magnitude', 0),
                    'location': event.get('location', ''),
                    'neuron_ids': event.get('neuron_ids', []),
                    'synapse_ids': event.get('synapse_ids', [])
                })
        
        # Sort by timestamp
        events_data.sort(key=lambda x: x['timestamp'])
        
        return {
            'type': 'events',
            'events': events_data,
            'event_count': len(events_data),
            'time_range': {
                'start': min([e['timestamp'] for e in events_data]) if events_data else 0,
                'end': max([e['timestamp'] for e in events_data]) if events_data else 0
            }
        }
    
    def _render_weight_changes(self) -> Dict[str, Any]:
        """Render weight change visualization"""
        if not self.weight_changes:
            return {'type': 'weight_changes', 'data': []}
        
        # Prepare weight changes data
        weight_changes = []
        for change_type, changes in self.weight_changes.items():
            for change in changes:
                weight_changes.append({
                    'source': change.get('source', ''),
                    'target': change.get('target', ''),
                    'weight_before': change.get('weight_before', 0),
                    'weight_after': change.get('weight_after', 0),
                    'change_magnitude': change.get('change_magnitude', 0),
                    'change_type': change.get('change_type', 'unknown'),
                    'timestamp': change.get('timestamp', 0)
                })
        
        # Calculate statistics
        if weight_changes:
            magnitudes = [abs(w['change_magnitude']) for w in weight_changes]
            weight_changes['statistics'] = {
                'total_changes': len(weight_changes),
                'mean_magnitude': np.mean(magnitudes),
                'max_magnitude': np.max(magnitudes),
                'min_magnitude': np.min(magnitudes),
                'positive_changes': sum(1 for w in weight_changes if w['change_magnitude'] > 0),
                'negative_changes': sum(1 for w in weight_changes if w['change_magnitude'] < 0),
                'stability_changes': sum(1 for w in weight_changes if abs(w['change_magnitude']) < 0.01)
            }
        
        return {
            'type': 'weight_changes',
            'weight_changes': weight_changes,
            'statistics': weight_changes.get('statistics', {}) if weight_changes else {}
        }
    
    def _render_learning_curves(self) -> Dict[str, Any]:
        """Render learning curves visualization"""
        if not self.plasticity_events:
            return {'type': 'learning_curves', 'data': []}
        
        # Extract learning data from plasticity events
        learning_data = []
        for event_type, events in self.plasticity_events.items():
            for event in events:
                learning_data.append({
                    'timestamp': event.get('timestamp', 0),
                    'event_type': event_type,
                    'magnitude': event.get('magnitude', 0),
                    'weight_change': event.get('weight_change', 0)
                })
        
        if not learning_data:
            return {'type': 'learning_curves', 'data': []}
        
        # Sort by timestamp
        learning_data.sort(key=lambda x: x['timestamp'])
        
        # Calculate cumulative learning
        timestamps = [d['timestamp'] for d in learning_data]
        cumulative_learning = []
        current_learning = 0
        
        for event in learning_data:
            current_learning += abs(event['magnitude'])
            cumulative_learning.append(current_learning)
        
        return {
            'type': 'learning_curves',
            'timestamps': timestamps,
            'learning_data': learning_data,
            'cumulative_learning': cumulative_learning,
            'total_learning': cumulative_learning[-1] if cumulative_learning else 0,
            'learning_events': len(learning_data)
        }
    
    def _render_rule_effectiveness(self) -> Dict[str, Any]:
        """Render plasticity rule effectiveness"""
        if not self.plasticity_rules:
            return {'type': 'rule_effectiveness', 'data': []}
        
        # Analyze rule effectiveness
        rule_effectiveness = {}
        for rule_name, rule_data in self.plasticity_rules.items():
            effectiveness = {
                'rule_name': rule_name,
                'event_count': rule_data.get('event_count', 0),
                'average_magnitude': rule_data.get('average_magnitude', 0),
                'success_rate': rule_data.get('success_rate', 0),
                'learning_efficiency': rule_data.get('learning_efficiency', 0)
            }
            rule_effectiveness[rule_name] = effectiveness
        
        return {
            'type': 'rule_effectiveness',
            'rule_effectiveness': rule_effectiveness,
            'total_rules': len(rule_effectiveness),
            'average_effectiveness': np.mean([r['learning_efficiency'] for r in rule_effectiveness.values()]) 
                                    if rule_effectiveness else 0
        }
    
    def _render_synaptic_dynamics(self) -> Dict[str, Any]:
        """Render synaptic strengthening and weakening"""
        if not self.plasticity_events:
            return {'type': 'synaptic_dynamics', 'data': []}
        
        # Analyze synaptic strengthening vs weakening
        strengthening = []
        weakening = []
        
        for event_type, events in self.plasticity_events.items():
            for event in events:
                magnitude = event.get('magnitude', 0)
                if magnitude > 0:
                    strengthening.append(magnitude)
                else:
                    weakening.append(abs(magnitude))
        
        return {
            'type': 'synaptic_dynamics',
            'strengthening': strengthening,
            'weakening': weakening,
            'strengthening_count': len(strengthening),
            'weakening_count': len(weakening),
            'net_change': sum(strengthening) - sum(weakening),
            'balance_ratio': sum(strengthening) / (sum(weakening) + 1e-10)
        }
    
    def _render_plasticity_efficiency(self) -> Dict[str, Any]:
        """Render plasticity efficiency analysis"""
        if not self.plasticity_events:
            return {'type': 'efficiency', 'data': []}
        
        # Calculate efficiency metrics
        events_by_type = {}
        for event_type, events in self.plasticity_events.items():
            if event_type not in events_by_type:
                events_by_type[event_type] = []
            events_by_type[event_type].extend(events)
        
        efficiency_data = {}
        for event_type, events in events_by_type.items():
            if events:
                magnitudes = [abs(e.get('magnitude', 0)) for e in events]
                efficiencies = []
                
                for event in events:
                    # Calculate efficiency as magnitude divided by time (simplified)
                    efficiency = abs(event.get('magnitude', 0)) / (event.get('timestamp', 0) + 1e-10)
                    efficiencies.append(efficiency)
                
                efficiency_data[event_type] = {
                    'event_count': len(events),
                    'average_magnitude': np.mean(magnitudes),
                    'std_magnitude': np.std(magnitudes),
                    'average_efficiency': np.mean(efficiencies),
                    'max_efficiency': np.max(efficiencies),
                    'efficiency_rank': sorted(
                        efficiency_data.keys(),
                        key=lambda k: efficiency_data[k]['average_efficiency'],
                        reverse=True
                    ).index(event_type) + 1 if event_type in efficiency_data else 0
                }
        
        return {
            'type': 'efficiency',
            'efficiency_data': efficiency_data,
            'total_event_types': len(efficiency_data),
            'best_efficiency_rule': max(efficiency_data.keys(), 
                                      key=lambda k: efficiency_data[k]['average_efficiency']) 
                                      if efficiency_data else None
        }
class DevelopmentVisualizer(VisualizationComponent):
    """Visualize developmental stages and changes"""
    
    def __init__(self, interface: ScriptingInterface, config: Dict[str, Any] = None):
        super().__init__("DevelopmentVisualizer", config)
        self.interface = interface
        self.context = interface.context
        self.development_stages = {}
        self.development_events = {}
        self.stage_metrics = {}
        self.development_timeline = []
        self.current_stage = config.get('initial_stage', 'stage1')
        self.stage_thresholds = config.get('stage_thresholds', {})
    
    def update(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Update development visualization with new data"""
        current_time = time.time()
        self.last_update = current_time
        
        # Update development stages
        if 'development_stages' in data:
            self.development_stages = data['development_stages']
        
        # Update development events
        if 'development_events' in data:
            self.development_events = data['development_events']
        
        # Update stage metrics
        if 'stage_metrics' in data:
            self.stage_metrics = data['stage_metrics']
        
        # Update current stage
        if 'current_stage' in data:
            self.current_stage = data['current_stage']
        
        # Process development data
        self._process_development_data(data)
        
        # Calculate development statistics
        self._calculate_development_stats()
        
        # Notify listeners
        self.notify_listeners({
            'type': 'development_update',
            'timestamp': current_time,
            'data': data
        })
        
        return {
            'stage_count': len(self.development_stages),
            'event_count': sum(len(events) for events in self.development_events.values()),
            'current_stage': self.current_stage,
            'last_update': current_time,
            'stats': self._get_development_stats()
        }
    
    def _process_development_data(self, data: Dict[str, Any]):
        """Process development data for visualization"""
        # Update development timeline
        if 'development_events' in data:
            for event_type, events in data['development_events'].items():
                for event in events:
                    self.development_timeline.append({
                        'type': event_type,
                        'timestamp': event.get('timestamp', 0),
                        'stage': event.get('stage', self.current_stage),
                        'description': event.get('description', ''),
                        'impact': event.get('impact', 0)
                    })
    
    def _calculate_development_stats(self):
        """Calculate development statistics"""
        if not self.development_stages:
            self.development_stats = {}
            return
        
        # Calculate stage progression statistics
        stages_completed = len(self.development_stages)
        current_stage = self.current_stage
        
        # Calculate total development time
        if self.development_timeline:
            timestamps = [event['timestamp'] for event in self.development_timeline]
            total_time = max(timestamps) - min(timestamps) if timestamps else 0
            
            self.development_stats = {
                'stages_completed': stages_completed,
                'current_stage': current_stage,
                'total_development_time': total_time,
                'stage_progression_rate': stages_completed / max(total_time, 1),
                'average_stage_duration': total_time / max(stages_completed, 1),
                'events_in_current_stage': sum(1 for event in self.development_timeline 
                                             if event['stage'] == current_stage)
            }
        else:
            self.development_stats = {
                'stages_completed': stages_completed,
                'current_stage': current_stage,
                'total_development_time': 0,
                'stage_progression_rate': stages_completed,
                'average_stage_duration': 0,
                'events_in_current_stage': 0
            }
    
    def _get_development_stats(self):
        """Get development statistics"""
        return {
            'stages_completed': self.development_stats.get('stages_completed', 0),
            'current_stage': self.development_stats.get('current_stage', 'stage1'),
            'total_development_time': self.development_stats.get('total_development_time', 0),
            'stage_progression_rate': self.development_stats.get('stage_progression_rate', 0),
            'average_stage_duration': self.development_stats.get('average_stage_duration', 0),
            'events_in_current_stage': self.development_stats.get('events_in_current_stage', 0)
        }
    
    def render(self) -> Dict[str, Any]:
        """Render development visualization"""
        if not self.development_stages:
            return {'type': 'empty', 'message': 'No development data available'}
        
        # Generate different visualization types
        visualizations = {}
        
        # Development stage progression
        visualizations['stage_progression'] = self._render_stage_progression()
        
        # Development events timeline
        visualizations['development_events'] = self._render_development_events()
        
        # Stage characteristics
        visualizations['stage_characteristics'] = self._render_stage_characteristics()
        
        # Development metrics
        visualizations['development_metrics'] = self._render_development_metrics()
        
        # Milestone analysis
        visualizations['milestone_analysis'] = self._render_milestone_analysis()
        
        # Development speed
        visualizations['development_speed'] = self._render_development_speed()
        
        return {
            'type': 'development',
            'timestamp': time.time(),
            'data': visualizations,
            'stats': self._get_development_stats(),
            'stage_count': len(self.development_stages),
            'current_stage': self.current_stage
        }
    
    def _render_stage_progression(self) -> Dict[str, Any]:
        """Render development stage progression"""
        # Calculate stage progression
        stage_order = list(self.development_stages.keys())
        current_index = stage_order.index(self.current_stage) if self.current_stage in stage_order else 0
        
        stages_data = []
        for i, stage in enumerate(stage_order):
            completed = i < current_index
            active = stage == self.current_stage
            
            stage_info = {
                'id': stage,
                'name': f"Stage {i + 1}",
                'completed': completed,
                'active': active,
                'position': i,
                'progress': (i + 1) / len(stage_order) * 100 if stage_order else 0
            }
            
            # Add stage-specific metrics if available
            if stage in self.stage_metrics:
                stage_info.update(self.stage_metrics[stage])
            
            stages_data.append(stage_info)
        
        return {
            'type': 'stage_progression',
            'stages': stages_data,
            'current_stage_index': current_index,
            'total_stages': len(stage_order),
            'completion_percentage': (current_index + 1) / len(stage_order) * 100 if stage_order else 0
        }
    
    def _render_development_events(self) -> Dict[str, Any]:
        """Render development events timeline"""
        if not self.development_timeline:
            return {'type': 'events', 'data': []}
        
        # Sort events by timestamp
        events = sorted(self.development_timeline, key=lambda x: x['timestamp'])
        
        # Group events by stage
        events_by_stage = {}
        for event in events:
            stage = event['stage']
            if stage not in events_by_stage:
                events_by_stage[stage] = []
            events_by_stage[stage].append(event)
        
        return {
            'type': 'events',
            'events': events,
            'events_by_stage': events_by_stage,
            'total_events': len(events),
            'current_stage_events': len(events_by_stage.get(self.current_stage, []))
        }
    
    def _render_stage_characteristics(self) -> Dict[str, Any]:
        """Render stage characteristics visualization"""
        if not self.development_stages:
            return {'type': 'characteristics', 'data': []}
        
        # Prepare stage characteristics data
        characteristics = []
        for stage, stage_data in self.development_stages.items():
            stage_info = {
                'stage': stage,
                'name': f"Stage {stage}",
                'neurons_created': stage_data.get('neurons_created', 0),
                'synapses_formed': stage_data.get('synapses_formed', 0),
                'activity_level': stage_data.get('activity_level', 0),
                'stability': stage_data.get('stability', 0),
                'learning_rate': stage_data.get('learning_rate', 0),
                'duration': stage_data.get('duration', 0),
                'key_milestones': stage_data.get('key_milestones', [])
            }
            characteristics.append(stage_info)
        
        return {
            'type': 'characteristics',
            'characteristics': characteristics,
            'stage_count': len(characteristics)
        }
    
    def _render_development_metrics(self) -> Dict[str, Any]:
        """Render development metrics visualization"""
        if not self.development_stats:
            return {'type': 'metrics', 'data': {}}
        
        return {
            'type': 'metrics',
            'development_stats': self.development_stats,
            'metrics_available': True
        }
    
    def _render_milestone_analysis(self) -> Dict[str, Any]:
        """Render milestone analysis visualization"""
        if not self.development_timeline:
            return {'type': 'milestones', 'data': []}
        
        # Identify milestones (major development events)
        milestones = []
        for event in self.development_timeline:
            # Consider events with high impact as milestones
            if event.get('impact', 0) > 0.7:
                milestones.append(event)
        
        return {
            'type': 'milestones',
            'milestones': milestones,
            'milestone_count': len(milestones),
            'milestone_percentage': (len(milestones) / len(self.development_timeline)) * 100 
                                   if self.development_timeline else 0
        }
    
    def _render_development_speed(self) -> Dict[str, Any]:
        """Render development speed visualization"""
        if not self.development_stats:
            return {'type': 'speed', 'data': {}}
        
        # Calculate development speed metrics
        total_time = self.development_stats.get('total_development_time', 0)
        stages_completed = self.development_stats.get('stages_completed', 0)
        
        speed_metrics = {
            'average_stage_duration': self.development_stats.get('average_stage_duration', 0),
            'development_completion_rate': self.development_stats.get('stage_progression_rate', 0),
            'current_stage_progress': (stages_completed / max(len(self.development_stages), 1)) * 100
        }
        
        return {
            'type': 'speed',
            'speed_metrics': speed_metrics,
            'has_metrics': True
        }
class NeuromodulationVisualizer(VisualizationComponent):
    """Visualize neuromodulator levels and effects"""
    
    def __init__(self, interface: ScriptingInterface, config: Dict[str, Any] = None):
        super().__init__("NeuromodulationVisualizer", config)
        self.interface = interface
        self.context = interface.context
        self.neuromodulators = {}
        self.neuromodulation_effects = {}
        self.neuromodulation_dynamics = {}
        self.biological_factors = {}
        self.reward_signals = {}
        self.learning_modulation = {}
        self.adaptation_levels = {}
    
    def update(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Update neuromodulation visualization with new data"""
        current_time = time.time()
        self.last_update = current_time
        
        # Update neuromodulators
        if 'neuromodulators' in data:
            self.neuromodulators = data['neuromodulators']
        
        # Update neuromodulation effects
        if 'neuromodulation_effects' in data:
            self.neuromodulation_effects = data['neuromodulation_effects']
        
        # Update neuromodulation dynamics
        if 'neuromodulation_dynamics' in data:
            self.neuromodulation_dynamics = data['neuromodulation_dynamics']
        
        # Update biological factors
        if 'biological_factors' in data:
            self.biological_factors = data['biological_factors']
        
        # Update reward signals
        if 'reward_signals' in data:
            self.reward_signals = data['reward_signals']
        
        # Update learning modulation
        if 'learning_modulation' in data:
            self.learning_modulation = data['learning_modulation']
        
        # Update adaptation levels
        if 'adaptation_levels' in data:
            self.adaptation_levels = data['adaptation_levels']
        
        # Process neuromodulation data
        self._process_neuromodulation_data(data)
        
        # Calculate dynamics
        self._calculate_dynamics()
        
        # Notify listeners
        self.notify_listeners({
            'type': 'neuromodulation_update',
            'timestamp': current_time,
            'data': data
        })
        
        return {
            'neuromodulator_count': len(self.neuromodulators),
            'effect_count': len(self.neuromodulation_effects),
            'biological_factor_count': len(self.biological_factors),
            'last_update': current_time,
            'dynamics': self.neuromodulation_dynamics
        }
    
    def _process_neuromodulation_data(self, data: Dict[str, Any]):n        """Process neuromodulation data for visualization"""
        # Process neuromodulator data
        if 'neuromodulators' in self.neuromodulators:
            for neuromodulator, levels in self.neuromodulators.items():
                self.neuromodulators[neuromodulator] = self._update_neuromodulator_levels(
                    neuromodulator, levels
                )
    
    def _update_neuromodulator_levels(self, neuromodulator: str, levels: List[float]) -> List[float]:
        """Update neuromodulator levels with smoothing"""
        # Apply smoothing to levels
        smoothed_levels = []
        for i, level in enumerate(levels):
            if i == 0:
                smoothed_levels.append(level)
            else:
                # Simple exponential moving average
                alpha = 0.1
                smoothed_level = alpha * level + (1 - alpha) * smoothed_levels[-1]
                smoothed_levels.append(smoothed_level)
        
        return smoothed_levels
    
    def _calculate_dynamics(self):
        """Calculate neuromodulation dynamics"""
        if not self.neuromodulators:
            self.neuromodulation_dynamics = {}
            return
        
        dynamics = {}
        
        for neuromodulator, levels in self.neuromodulators.items():
            if len(levels) >= 2:
                # Calculate rate of change
                levels_array = np.array(levels)
                time_array = np.arange(len(levels))
                
                # Linear fit
                coeff = np.polyfit(time_array, levels_array, 1)
                slope = coeff[0]  # rate of change
                intercept = coeff[1]
                
                # Calculate volatility
                changes = np.diff(levels_array)
                volatility = np.std(changes)
                
                dynamics[neuromodulator] = {
                    'current_level': levels[-1],
                    'rate_of_change': slope,
                    'volatility': volatility,
                    'trend': 'increasing' if slope > 0 else 'decreasing' if slope < 0 else 'stable',
                    'stability': 1.0 / (1.0 + abs(volatility)),
                    'efficiency': 1.0 / (1.0 + abs(slope))
                }
        
        self.neuromodulation_dynamics = dynamics
    
    def render(self) -> Dict[str, Any]:
        """Render neuromodulation visualization"""
        if not self.neuromodulators:
            return {'type': 'empty', 'message': 'No neuromodulation data available'}
        
        # Generate different visualization types
        visualizations = {}
        
        # Neuromodulator levels
        visualizations['neuromodulator_levels'] = self._render_neuromodulator_levels()
        
        # Neuromodulation effects
        visualizations['neuromodulation_effects'] = self._render_neuromodulation_effects()
        
        # Dynamic interactions
        visualizations['dynamic_interactions'] = self._render_dynamic_interactions()
        
        # Biological factors
        visualizations['biological_factors'] = self._render_biological_factors()
        
        # Reward signals
        visualizations['reward_signals'] = self._render_reward_signals()
        
        # Learning modulation
        visualizations['learning_modulation'] = self._render_learning_modulation()
        
        # Adaptation levels
        visualizations['adaptation_levels'] = self._render_adaptation_levels()
        
        return {
            'type': 'neuromodulation',
            'timestamp': time.time(),
            'data': visualizations,
            'dynamics': self.neuromodulation_dynamics,
            'neuromodulator_count': len(self.neuromodulators),
            'effect_count': len(self.neuromodulation_effects)
        }
    
    def _render_neuromodulator_levels(self) -> Dict[str, Any]:
        """Render neuromodulator levels visualization"""
        if not self.neuromodulators:
            return {'type': 'levels', 'data': []}
        
        # Prepare data for radar chart or circular visualization
        neuromodulators = list(self.neuromodulators.keys())
        
        # Get current levels (last value in each time series)
        current_levels = []
        for neuromodulator in neuromodulators:
            levels = self.neuromodulators.get(neuromodulator, [])
            current_levels.append(levels[-1] if levels else 0)
        
        # Normalize to 0-1 range
        max_level = max(current_levels) if current_levels else 1
        normalized_levels = [(level / max_level) for level in current_levels]
        
        # Create radar chart data
        radar_data = []
        for neuromodulator, level in zip(neuromodulators, normalized_levels):
            # Get range information
            levels = self.neuromodulators.get(neuromodulator, [])
            if levels:
                min_level = min(levels)
                max_level_range = max(levels) - min_level
                radar_data.append({
                    'neuromodulator': neuromodulator,
                    'current': level,
                    'min': min_level,
                    'max': max(levels),
                    'range': max_level_range,
                    'color': COLOR_SCHEMES['neuromodulation'].get(neuromodulator, '#808080')
                })
        
        return {
            'type': 'levels',
            'neuromodulators': neuromodulators,
            'current_levels': current_levels,
            'normalized_levels': normalized_levels,
            'radar_data': radar_data,
            'max_level': max_level
        }
    
    def _render_neuromodulation_effects(self) -> Dict[str, Any]:
        """Render neuromodulation effects visualization"""
        if not self.neuromodulation_effects:
            return {'type': 'effects', 'data': []}
        
        # Prepare effects data
        effects_data = []
        for effect_type, effects in self.neuromodulation_effects.items():
            for effect in effects:
                effects_data.append({
                    'type': effect_type,
                    'target': effect.get('target', ''),
                    'magnitude': effect.get('magnitude', 0),
                    'duration': effect.get('duration', 0),
                    'timestamp': effect.get('timestamp', 0),
                    'neuromodulator_source': effect.get('neuromodulator_source', '')
                })
        
        # Group by effect type
        effects_by_type = {}
        for effect in effects_data:
            effect_type = effect['type']
            if effect_type not in effects_by_type:
                effects_by_type[effect_type] = []
            effects_by_type[effect_type].append(effect)
        
        return {
            'type': 'effects',
            'effects_by_type': effects_by_type,
            'total_effects': len(effects_data),
            'effect_types': list(effects_by_type.keys())
        }
    
    def _render_dynamic_interactions(self) -> Dict[str, Any]:
        """Render dynamic interactions between neuromodulators"""
        if not self.neuromodulation_dynamics:
            return {'type': 'interactions', 'data': []}
        
        # Create interaction matrix
        neuromodulators = list(self.neuromodulation_dynamics.keys())
        interaction_matrix = {}
        
        for neuromodulator in neuromodulators:
            interactions = {}
            for other in neuromodulators:
                if neuromodulator != other:
                    # Calculate interaction strength (simplified)
                    dynamics1 = self.neuromodulation_dynamics.get(neuromodulator, {})
                    dynamics2 = self.neuromodulation_dynamics.get(other, {})
                    
                    # Calculate correlation based on trends and volatility
                    trend1 = dynamics1.get('trend', 'stable')
                    trend2 = dynamics2.get('trend', 'stable')
                    
                    # Simple interaction score
                    interaction_score = 0.5
                    if trend1 == 'increasing' and trend2 == 'increasing':
                        interaction_score = 0.8
                    elif trend1 == 'decreasing' and trend2 == 'decreasing':
                        interaction_score = 0.8
                    
                    interactions[other] = {
                        'interaction_strength': interaction_score,
                        'trend_correlation': trend1 == trend2,
                        'volatility_synchronization': abs(
                            dynamics1.get('volatility', 0) - dynamics2.get('volatility', 0)
                        ) < 0.1
                    }
            
            interaction_matrix[neuromodulator] = interactions
        
        return {
            'type': 'interactions',
            'neuromodulators': neuromodulators,
            'interaction_matrix': interaction_matrix,
            'total_neuromodulators': len(neuromodulators)
        }
    
    def _render_biological_factors(self) -> Dict[str, Any]:
        """Render biological factors visualization"""
        if not self.biological_factors:
            return {'type': 'biological_factors', 'data': []}
        
        # Prepare biological factors data
        factors_data = []
        for factor, data in self.biological_factors.items():
            factors_data.append({
                'factor': factor,
                'value': data.get('value', 0),
                'unit': data.get('unit', ''),
                'impact': data.get('impact', 0),
                'trend': data.get('trend', 'stable'),
                'timestamp': data.get('timestamp', 0)
            })
        
        # Calculate biological balance
        if factors_data:
            weights = [1.0 / max(1.0, abs(f['impact'])) for f in factors_data]
            weighted_sum = sum(f['value'] * w for f, w in zip(factors_data, weights))
            weight_sum = sum(weights)
            
            biological_balance = weighted_sum / weight_sum if weight_sum > 0 else 0
        else:
            biological_balance = 0
        
        return {
            'type': 'biological_factors',
            'factors': factors_data,
            'biological_balance': biological_balance,
            'factor_count': len(factors_data)
        }
    
    def _render_reward_signals(self) -> Dict[str, Any]:
        """Render reward signals visualization"""
        if not self.reward_signals:
            return {'type': 'reward_signals', 'data': []}
        
        # Prepare reward signals data
        reward_data = []
        for signal_type, signals in self.reward_signals.items():
            for signal in signals:
                reward_data.append({
                    'type': signal_type,
                    'value': signal.get('value', 0),
                    'timestamp': signal.get('timestamp', 0),
                    'source': signal.get('source', ''),
                    'strength': signal.get('strength', 0)
                })
        
        # Sort by timestamp
        reward_data.sort(key=lambda x: x['timestamp'])
        
        # Calculate reward statistics
        if reward_data:
            values = [r['value'] for r in reward_data]
            strengths = [r['strength'] for r in reward_data]
            
            reward_stats = {
                'mean_value': np.mean(values),
                'max_value': np.max(values),
                'min_value': np.min(values),
                'mean_strength': np.mean(strengths),
                'total_reward_signals': len(reward_data)
            }
        else:
            reward_stats = {
                'mean_value': 0,
                'max_value': 0,
                'min_value': 0,
                'mean_strength': 0,
                'total_reward_signals': 0
            }
        
        return {
            'type': 'reward_signals',
            'reward_data': reward_data,
            'reward_stats': reward_stats,
            'total_signals': len(reward_data)
        }
    
    def _render_learning_modulation(self) -> Dict[str, Any]:
        """Render learning modulation visualization"""
        if not self.learning_modulation:
            return {'type': 'learning_modulation', 'data': []}
        
        # Prepare learning modulation data
        learning_data = []
        for modulation_type, modulations in self.learning_modulation.items():
            for modulation in modulations:
                learning_data.append({
                    'type': modulation_type,
                    'value': modulation.get('value', 0),
                    'timestamp': modulation.get('timestamp', 0),
                    'target_neuron': modulation.get('target_neuron', ''),
                    'effectiveness': modulation.get('effectiveness', 0)
                })
        
        # Calculate learning efficiency
        if learning_data:
            efficiencies = [m['effectiveness'] for m in learning_data]
            values = [m['value'] for m in learning_data]
            
            learning_stats = {
                'mean_effectiveness': np.mean(efficiencies),
                'max_effectiveness': np.max(efficiencies),
                'average_modulation': np.mean(values),
                'total_modulations': len(learning_data)
            }
        else:
            learning_stats = {
                'mean_effectiveness': 0,
                'max_effectiveness': 0,
                'average_modulation': 0,
                'total_modulations': 0
            }
        
        return {
            'type': 'learning_modulation',
            'learning_data': learning_data,
            'learning_stats': learning_stats,
            'total_modulations': len(learning_data)
        }
    
    def _render_adaptation_levels(self) -> Dict[str, Any]:
        """Render adaptation levels visualization"""
        if not self.adaptation_levels:
            return {'type': 'adaptation_levels', 'data': []}
        
        # Prepare adaptation levels data
        adaptation_data = []
        for adaptation_type, levels in self.adaptation_levels.items():
            for level in levels:
                adaptation_data.append({
                    'type': adaptation_type,
                    'value': level.get('value', 0),
                    'timestamp': level.get('timestamp', 0),
                    'target_system': level.get('target_system', ''),
                    'adaptation_rate': level.get('adaptation_rate', 0)
                })
        
        # Calculate adaptation statistics
        if adaptation_data:
            values = [a['value'] for a in adaptation_data]
            rates = [a['adaptation_rate'] for a in adaptation_data]
            
            adaptation_stats = {
                'mean_adaptation': np.mean(values),
                'max_adaptation': np.max(values),
                'average_rate': np.mean(rates),
                'total_adaptations': len(adaptation_data)
            }
        else:
            adaptation_stats = {
                'mean_adaptation': 0,
                'max_adaptation': 0,
                'average_rate': 0,
                'total_adaptations': 0
            }
        
        return {
            'type': 'adaptation_levels',
            'adaptation_data': adaptation_data,
            'adaptation_stats': adaptation_stats,
            'total_adaptations': len(adaptation_data)
        }
class NLMVisualizer:
    """Main NLM visualization manager"""
    
    def __init__(self, interface: ScriptingInterface, config: Dict[str, Any] = None):
        self.interface = interface
        self.context = interface.context
        self.config = config or {}
        
        # Initialize visualization components
        self.neural_network_viz = NeuralNetworkVisualizer(interface, config)
        self.spike_train_viz = SpikeTrainVisualizer(interface, config)
        self.memory_viz = MemoryVisualizer(interface, config)
        self.plasticity_viz = PlasticityVisualizer(interface, config)
        self.development_viz = DevelopmentVisualizer(interface, config)
        self.neuromodulation_viz = NeuromodulationVisualizer(interface, config)
        
        # Store components
        self.components = {
            'neural_network': self.neural_network_viz,
            'spike_train': self.spike_train_viz,
            'memory': self.memory_viz,
            'plasticity': self.plasticity_viz,
            'development': self.development_viz,
            'neuromodulation': self.neuromodulation_viz
        }
        
        # Setup event listeners
        self._setup_event_listeners()
        
        # Start update loop
        self._start_update_loop()
    
    def _setup_event_listeners(self):
        """Setup event listeners for real-time updates"""
        # Listen to brain state changes
        self.interface.register_event_handler(
            EventType.STATE_CHANGED,
            self._handle_state_change
        )
        
        # Listen to neuron fired events
        self.interface.register_event_handler(
            EventType.NEURON_FIRED,
            self._handle_neuron_fired
        )
        
        # Listen to simulation events
        self.interface.register_event_handler(
            EventType.SIMULATION_STARTED,
            self._handle_simulation_started
        )
        
        self.interface.register_event_handler(
            EventType.SIMULATION_STOPPED,
            self._handle_simulation_stopped
        )
    
    def _start_update_loop(self):
        """Start the visualization update loop"""
        def update_loop():
            while True:
                try:
                    # Get current brain state
                    state = self.interface.get_state()
                    
                    # Update each visualization component
                    for component_name, component in self.components.items():
                        component.update(state)
                    
                    # Sleep briefly to prevent busy waiting
                    time.sleep(0.1)
                    
                except Exception as e:
                    logger = __import__('logging').getLogger(__name__)
                    logger.error(f"Visualization update loop error: {e}")
                    time.sleep(1.0)
        
        # Start update loop in background thread
        self.update_thread = threading.Thread(target=update_loop, daemon=True)
        self.update_thread.start()
    
    def _handle_state_change(self, event: ScriptingEvent):
        """Handle state change events"""
        # Trigger updates for relevant visualizations
        state = event.data.get('state', {})
        
        for component_name, component in self.components.items():
            component.update(state)
    
    def _handle_neuron_fired(self, event: ScriptingEvent):
        """Handle neuron fired events"""
        # Trigger spike train visualization update
        if 'neuron_id' in event.data:
            spike_data = {
                'spikes': [{
                    'neuron_id': event.data['neuron_id'],
                    'time': event.timestamp,
                    'amplitude': event.data.get('amplitude', 1.0)
                }]
            }
            self.spike_train_viz.update(spike_data)
    
    def _handle_simulation_started(self, event: ScriptingEvent):
        """Handle simulation started events"""
        # Reset visualization components
        for component in self.components.values():
            component.update({})
    
    def _handle_simulation_stopped(self, event: ScriptingEvent):
        """Handle simulation stopped events"""
        # Clear visualization data
        for component in self.components.values():
            component.update({})
    
    def get_visualizations(self) -> Dict[str, Any]:
        """Get all current visualizations"""
        visualizations = {}
        
        for component_name, component in self.components.items():
            visualizations[component_name] = component.render()
        
        return {
            'timestamp': time.time(),
            'visualizations': visualizations,
            'component_count': len(self.components)
        }
    
    def export_visualizations(self, format: str = 'json', filepath: Optional[str] = None) -> Dict[str, Any]:
        """Export visualizations to file"""
        visualizations = self.get_visualizations()
        
        if format.lower() == 'json':
            import json
            
            data = {
                'timestamp': visualizations['timestamp'],
                'visualizations': visualizations['visualizations'],
                'component_count': visualizations['component_count']
            }
            
            if filepath:
                with open(filepath, 'w') as f:
                    json.dump(data, f, indent=2, default=str)
                
                logger = __import__('logging').getLogger(__name__)
                logger.info(f"Visualizations exported to {filepath}")
            
            return data
            
        elif format.lower() == 'python':
            data = f"visualizations_data = {visualizations}"
            
            if filepath:
                with open(filepath, 'w') as f:
                    f.write(data)
                
                logger = __import__('logging').getLogger(__name__)
                logger.info(f"Visualizations exported to {filepath}")
            
            return {'data': data}
            
        else:
            raise ValueError(f"Unsupported export format: {format}")
    
    def add_update_listener(self, listener: Callable):
        """Add listener for visualization updates"""
        for component in self.components.values():
            component.add_update_listener(listener)
    
    def remove_update_listener(self, listener: Callable):
        """Remove update listener"""
        for component in self.components.values():
            component.remove_update_listener(listener)
    
    def get_component(self, component_name: str) -> Optional[VisualizationComponent]:
        """Get a specific visualization component"""
        return self.components.get(component_name)
    
    def list_components(self) -> List[str]:
        """List available visualization components"""
        return list(self.components.keys())
    
    def stop(self):
        """Stop the visualization system"""
        # Clear event listeners
        self.interface.register_event_handler(
            EventType.STATE_CHANGED,
            None
        )
        
        # Wait for update thread to finish
        if hasattr(self, 'update_thread'):
            self.update_thread.join(timeout=5.0)
# Export key classes and functions
__all__ = [
    'VisualizationComponent',
    'NeuralNetworkVisualizer',
    'SpikeTrainVisualizer',
    'MemoryVisualizer',
    'PlasticityVisualizer',
    'DevelopmentVisualizer',
    'NeuromodulationVisualizer',
    'NLMVisualizer'
]

# Example usage
def create_nlm_visualizer(interface: ScriptingInterface, config: Dict[str, Any] = None) -> NLMVisualizer:
    """Create NLM visualizer with default configuration"""
    return NLMVisualizer(interface, config)