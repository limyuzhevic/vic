"""
Debugging and profiling utilities for NLM neural simulations.

Provides neural activity visualization, connection graph debugging,
spike tracing and analysis, and memory state inspection.
"""

import time
import json
import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, List, Any, Optional, Tuple
from datetime import datetime
import networkx as nx
from dataclasses import dataclass, asdict
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend

@dataclass
class SpikeEvent:
    """Represents a neural spike event."""
    
    neuron_id: int
    timestamp: float
    region_id: int
    population_id: int
    neuron_type: str
    membrane_potential: float
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)
class DebugData:
    """Container for debugging data."""
    
    def __init__(self):
        self.timestamp: float = time.time()
        self.step: int = 0
        self.spike_events: List[SpikeEvent] = []
        self.neuron_states: Dict[int, Dict[str, Any]] = {}
        self.synapse_weights: Dict[int, Dict[str, Any]] = {}
        self.memory_states: Dict[str, Any] = {}
        self.plasticity_state: Dict[str, Any] = {}
        self.motor_commands: List[Dict[str, Any]] = []
        
    def to_dict(self) -> Dict[str, Any]:
        return {
            'timestamp': self.timestamp,
            'step': self.step,
            'spike_events': [event.to_dict() for event in self.spike_events],
            'neuron_states': self.neuron_states,
            'synapse_weights': self.synapse_weights,
            'memory_states': self.memory_states,
            'plasticity_state': self.plasticity_state,
            'motor_commands': self.motor_commands
        }
class Debugger:
    """
    Debugging and profiling utilities for NLM neural simulations.
    
    Features:
    - Neural activity visualization
    - Connection graph debugging
    - Spike tracing and analysis
    - Memory state inspection
    """
    
    def __init__(self, brain=None):
        """
        Initialize debugger.
        
        Args:
            brain: Reference to the brain object (optional)
        """
        self.brain = brain
        self.debug_data: List[DebugData] = []
        self.is_recording = False
        self.record_interval = 0.1  # seconds
        self.connection_graph = nx.Graph()
        
    def start_recording(self):
        """Start recording debugging data."""
        self.is_recording = True
        self.debug_session_start = time.time()
        
    def stop_recording(self):
        """Stop recording debugging data."""
        self.is_recording = False
        
    def capture_debug_data(self, step: int):
        """
        Capture current debugging data.
        
        Args:
            step: Current simulation step
        """
        if not self.is_recording:
            return
            
        debug_data = DebugData()
        debug_data.step = step
        
        # Capture spike events
        if self.brain:
            # Get spike information from brain
            spike_count = self.brain.getTotalSpikeCount()
            active_neuron_count = self.brain.getActiveNeuronCount()
            
            # Capture neuron states
            for region in self.brain.getRegions():
                for neuron in region.getNeurons():
                    debug_data.neuron_states[neuron.getId()] = {
                        'membrane_potential': neuron.getMembranePotential(),
                        'firing_state': neuron.getFiringState(),
                        'region_id': region.getId(),
                        'population_id': neuron.getPopulationId(),
                        'neuron_type': neuron.getType()
                    }
                    
                # Capture synapse information
                synapses = region.getSynapses()
                for synapse in synapses:
                    debug_data.synapse_weights[synapse.getId()] = {
                        'weight': synapse.getWeight(),
                        'delay': synapse.getDelay(),
                        'source_id': synapse.getSourceId(),
                        'target_id': synapse.getTargetId(),
                        'type': synapse.getType()
                    }
                    
            # Update connection graph
            self._update_connection_graph()
            
        self.debug_data.append(debug_data)
        
    def _update_connection_graph(self):
        """Update the neural connection graph."""
        self.connection_graph.clear()
        
        if not self.brain:
            return
            
        # Add nodes for neurons
        for region in self.brain.getRegions():
            for neuron in region.getNeurons():
                self.connection_graph.add_node(
                    neuron.getId(),
                    type=neuron.getType(),
                    region_id=region.getId(),
                    population_id=neuron.getPopulationId(),
                    membrane_potential=neuron.getMembranePotential(),
                    firing_state=neuron.getFiringState()
                )
                
        # Add edges for synapses
        for region in self.brain.getRegions():
            for synapse in region.getSynapses():
                self.connection_graph.add_edge(
                    synapse.getSourceId(),
                    synapse.getTargetId(),
                    weight=synapse.getWeight(),
                    delay=synapse.getDelay(),
                    type=synapse.getType()
                )
                
    def visualize_activity(self, output_file: Optional[str] = None,
                         neurons_subset: Optional[List[int]] = None,
                         time_window: Optional[Tuple[int, int]] = None) -> str:
        """
        Visualize neural activity.
        
        Args:
            output_file: Output file path (optional)
            neurons_subset: Subset of neurons to visualize (optional)
            time_window: Time window (start_step, end_step) (optional)
            
        Returns:
            Path to output file
        """
        if not self.debug_data:
            raise ValueError("No debug data available")
            
        # Prepare data for visualization
        if time_window:
            start_step, end_step = time_window
            data_subset = [d for d in self.debug_data if start_step <= d.step <= end_step]
        else:
            data_subset = self.debug_data
            
        if not data_subset:
            raise ValueError("No debug data in specified time window")
            
        # Create spike raster plot
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        fig.suptitle('Neural Activity Visualization', fontsize=16)
        
        # Extract spike data
        all_spikes = []
        for debug in data_subset:
            for spike in debug.spike_events:
                all_spikes.append((spike.step, spike.neuron_id))
                
        if all_spikes:
            steps, neurons = zip(*all_spikes)
            axes[0, 0].scatter(steps, neurons, s=1, alpha=0.7)
            axes[0, 0].set_xlabel('Step')
            axes[0, 0].set_ylabel('Neuron ID')
            axes[0, 0].set_title('Spike Raster Plot')
            axes[0, 0].invert_yaxis()
            
        # Plot membrane potential for selected neurons
        if neurons_subset:
            for neuron_id in neurons_subset[:5]:  # Limit to 5 neurons for clarity
                potentials = []
                for debug in data_subset:
                    if neuron_id in debug.neuron_states:
                        potentials.append(debug.neuron_states[neuron_id]['membrane_potential'])
                    else:
                        potentials.append(None)
                        
                valid_indices = [i for i, p in enumerate(potentials) if p is not None]
                if valid_indices:
                    axes[0, 1].plot([data_subset[i].step for i in valid_indices],
                                  [potentials[i] for i in valid_indices])
                    axes[0, 1].set_xlabel('Step')
                    axes[0, 1].set_ylabel('Membrane Potential')
                    axes[0, 1].set_title(f'Membrane Potential - Neuron {neuron_id}')
                    axes[0, 1].set_ylim([-80, 50])
                    
        # Plot synaptic weight distribution
        all_weights = []
        for debug in data_subset:
            all_weights.extend(debug.synapse_weights.values())
            
        if all_weights:
            weights = [w.get('weight', 0) for w in all_weights]
            axes[1, 0].hist(weights, bins=50, alpha=0.7)
            axes[1, 0].set_xlabel('Synaptic Weight')
            axes[1, 0].set_ylabel('Count')
            axes[1, 0].set_title('Synaptic Weight Distribution')
            
        # Plot region activity
        region_activity = {}
        for debug in data_subset:
            for neuron_id, state in debug.neuron_states.items():
                region_id = state['region_id']
                if region_id not in region_activity:
                    region_activity[region_id] = 0
                if state['firing_state'] == 1:  # Active
                    region_activity[region_id] += 1
                    
        if region_activity:
            regions = list(region_activity.keys())
            activity = list(region_activity.values())
            axes[1, 1].bar(regions, activity)
            axes[1, 1].set_xlabel('Region ID')
            axes[1, 1].set_ylabel('Active Neuron Count')
            axes[1, 1].set_title('Region Activity')
            
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=150)
            plt.close()
            return output_file
        else:
            # Return matplotlib figure for display in notebooks
            return fig
            
    def trace_spikes(self, neuron_id: Optional[int] = None,
                    start_step: int = 0, end_step: Optional[int] = None,
                    region_id: Optional[int] = None) -> List[SpikeEvent]:
        """
        Trace spikes for specific neurons or regions.
        
        Args:
            neuron_id: Specific neuron ID (optional)
            start_step: Start step for tracing (optional)
            end_step: End step for tracing (optional)
            region_id: Region ID (optional)
            
        Returns:
            List of spike events matching criteria
        """
        all_spikes = []
        
        for debug in self.debug_data:
            if debug.step < start_step:
                continue
            if end_step and debug.step > end_step:
                continue
                
            for spike in debug.spike_events:
                if neuron_id and spike.neuron_id != neuron_id:
                    continue
                if region_id and spike.region_id != region_id:
                    continue
                    
                all_spikes.append(spike)
                
        # Sort by timestamp
        all_spikes.sort(key=lambda x: x.timestamp)
        return all_spikes
        
    def analyze_spike_patterns(self) -> Dict[str, Any]:
        """
        Analyze spike patterns from recorded data.
        
        Returns:
            Dictionary with spike pattern analysis
        """
        all_spikes = []
        for debug in self.debug_data:
            all_spikes.extend(debug.spike_events)
            
        if not all_spikes:
            return {'error': 'No spike data available'}
            
        # Convert to numpy arrays for analysis
        spike_times = np.array([spike.timestamp for spike in all_spikes])
        neuron_ids = np.array([spike.neuron_id for spike in all_spikes])
        
        analysis = {
            'total_spikes': len(spike_times),
            'unique_neurons': len(np.unique(neuron_ids)),
            'spike_rate_per_second': len(spike_times) / (max(spike_times) - min(spike_times)) if len(spike_times) > 1 else 0,
            'mean_inter_spike_interval': np.mean(np.diff(spike_times)) if len(spike_times) > 1 else 0,
            'std_inter_spike_interval': np.std(np.diff(spike_times)) if len(spike_times) > 1 else 0,
            'firing_rate_per_neuron': len(spike_times) / len(np.unique(neuron_ids)) if len(np.unique(neuron_ids)) > 0 else 0
        }
        
        # Analyze firing patterns
        neurons, spike_counts = np.unique(neuron_ids, return_counts=True)
        if len(spike_counts) > 0:
            top_neurons = neurons[np.argsort(spike_counts)[-10:][::-1]]
            analysis['top_spiking_neurons'] = dict(zip(top_neurons, 
                                                     np.sort(spike_counts)[-10:][::-1]))
            
        return analysis
        
    def inspect_memory_state(self, memory_type: Optional[str] = None) -> Dict[str, Any]:
        """
        Inspect memory system states.
        
        Args:
            memory_type: Specific memory type to inspect (optional)
            
        Returns:
            Memory state information
        """
        memory_info = {}
        
        for debug in self.debug_data:
            for mem_type, mem_state in debug.memory_states.items():
                if memory_type and mem_type != memory_type:
                    continue
                    
                if mem_type not in memory_info:
                    memory_info[mem_type] = []
                    
                memory_info[mem_type].append(mem_state)
                
        # Analyze memory states
        results = {}
        for mem_type, states in memory_info.items():
            if not states:
                continue
                
            # Convert to arrays for analysis
            if isinstance(states[0], dict):
                # Extract numeric values for analysis
                numeric_values = {}
                for key, values in states[0].items():
                    if isinstance(values, (int, float)):
                        numeric_values[key] = [s[key] for s in states if key in s]
                        
                results[mem_type] = {
                    'size': len(states),
                    'numeric_fields': numeric_values,
                    'latest_state': states[-1]
                }
            else:
                results[mem_type] = {
                    'size': len(states),
                    'sample': states[-1] if states else None
                }
                
        return results
        
    def visualize_connection_graph(self, output_file: Optional[str] = None,
                                 highlight_neuron: Optional[int] = None,
                                 highlight_region: Optional[int] = None) -> str:
        """
        Visualize neural connection graph.
        
        Args:
            output_file: Output file path (optional)
            highlight_neuron: Neuron ID to highlight (optional)
            highlight_region: Region ID to highlight (optional)
            
        Returns:
            Path to output file
        """
        if not self.connection_graph.nodes():
            raise ValueError("No connection graph data available")
            
        plt.figure(figsize=(12, 8))
        
        # Layout
        pos = nx.spring_layout(self.connection_graph, k=1, iterations=50)
        
        # Node colors by type
        node_colors = []
        for node_id in self.connection_graph.nodes():
            node_type = self.connection_graph.nodes[node_id].get('type', 'unknown')
            if node_type == 'Excitatory':
                node_colors.append('red')
            elif node_type == 'Inhibitory':
                node_colors.append('blue')
            elif node_type == 'Sensory':
                node_colors.append('green')
            elif node_type == 'Motor':
                node_colors.append('orange')
            else:
                node_colors.append('gray')
                
        # Highlight specific neuron or region
        if highlight_neuron and highlight_neuron in self.connection_graph.nodes():
            node_colors[list(self.connection_graph.nodes()).index(highlight_neuron)] = 'yellow'
            
        if highlight_region:
            for node_id in self.connection_graph.nodes():
                if self.connection_graph.nodes[node_id].get('region_id') == highlight_region:
                    idx = list(self.connection_graph.nodes()).index(node_id)
                    node_colors[idx] = 'cyan'
                    
        # Draw graph
        nx.draw_networkx_nodes(self.connection_graph, pos, node_color=node_colors, node_size=50)
        nx.draw_networkx_edges(self.connection_graph, pos, alpha=0.5, width=1)
        nx.draw_networkx_labels(self.connection_graph, pos, font_size=8)
        
        plt.title('Neural Connection Graph')
        plt.axis('off')
        
        if output_file:
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            plt.close()
            return output_file
        else:
            return plt.gcf()
            
    def export_debug_data(self, filepath: str, format: str = 'json'):
        """
        Export debugging data to file.
        
        Args:
            filepath: Output file path
            format: Export format ('json', 'csv', 'pickle')
        """
        data = [debug.to_dict() for debug in self.debug_data]
        
        if format == 'json':
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
        elif format == 'csv':
            import pandas as pd
            df = pd.json_normalize(data)
            df.to_csv(filepath, index=False)
        elif format == 'pickle':
            import pickle
            with open(filepath, 'wb') as f:
                pickle.dump(self.debug_data, f)
                
    def import_debug_data(self, filepath: str):
        """
        Import debugging data from file.
        
        Args:
            filepath: Input file path
        """
        if filepath.endswith('.json'):
            with open(filepath, 'r') as f:
                data = json.load(f)
                self.debug_data = [self._dict_to_debug_data(item) for item in data]
        elif filepath.endswith('.csv'):
            import pandas as pd
            df = pd.read_csv(filepath)
            # Convert to debug data format (simplified)
            self.debug_data = [DebugData()] * len(df)  # Placeholder
        elif filepath.endswith('.pkl') or filepath.endswith('.pickle'):
            import pickle
            with open(filepath, 'rb') as f:
                self.debug_data = pickle.load(f)
                
    def _dict_to_debug_data(self, data: Dict[str, Any]) -> DebugData:
        """Convert dictionary to DebugData object (simplified)."""
        debug = DebugData()
        debug.step = data.get('step', 0)
        debug.timestamp = data.get('timestamp', time.time())
        
        # Restore spike events
        for spike_dict in data.get('spike_events', []):
            spike = SpikeEvent(**spike_dict)
            debug.spike_events.append(spike)
            
        return debug
        
    def generate_debug_report(self) -> str:
        """
        Generate a comprehensive debug report.
        
        Returns:
            Debug report as string
        """
        report = f"""NLM Debug Report
===================
Generated: {datetime.now().isoformat()}
Debug Sessions: {len(self.debug_data)}
Total Spike Events: {sum(len(d.spike_events) for d in self.debug_data)}
Total Steps Recorded: {max((d.step for d in self.debug_data), default=0)}

Spike Pattern Analysis:
"""
        
        spike_analysis = self.analyze_spike_patterns()
        for key, value in spike_analysis.items():
            if key != 'error':
                report += f"  {key}: {value}\n"
                
        report += "\nMemory State Summary:\n"
        memory_info = self.inspect_memory_state()
        for mem_type, info in memory_info.items():
            report += f"  {mem_type}: {info.get('size', 0)} entries\n"
            
        if self.connection_graph.nodes():
            report += f"\nConnection Graph Summary:\n"
            report += f"  Total Neurons: {len(self.connection_graph.nodes())}\n"
            report += f"  Total Connections: {len(self.connection_graph.edges())}\n"
            
        return report