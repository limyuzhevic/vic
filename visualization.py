# NLM Visualization and Debugging Tools

# This module provides comprehensive visualization and debugging capabilities for NLM simulations.
# Includes network visualization, neural activity monitoring, parameter inspection, and debugging tools.

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
from typing import List, Dict, Any, Optional, Tuple
from dataclasses import dataclass, field
from enum import Enum
import json
from pathlib import Path
import time
import threading
from concurrent.futures import ThreadPoolExecutor, as_completed
import psutil
import os

class VisualizationLevel(Enum):
    """Visualization detail levels"""
    LOW = "low"           # Basic network overview
    MEDIUM = "medium"     # Detailed network view
    HIGH = "high"         # High-detail interactive visualization
    DEBUG = "debug"       # Detailed debugging information

class NetworkLayout(Enum):
    """Network layout algorithms"""
    FORCE_ATTRACTOR = "force_atractor"
    CIRCULAR = "circular"
    SPRING = "spring"
    RADIALLAYOUT = "radial"
    HEAVE = "heave"
    CIRCOGRAM = "circo"

class ActivityView(Enum):
    """Neural activity visualization modes"""
    SPIKE_RATES = "spike_rates"
    POSITIONAL = "positional"
    CONNECTION_DENSITY = "connection_density"
    E_I_BALANCE = "excitation_inhibition_balance"
    TEMPORAL_PATTERN = "temporal_pattern"
    SPATIAL_PATTERN = "spatial_pattern"

class DebugLevel(Enum):
    """Debug verbosity levels"""
    NONE = "none"
    BASIC = "basic"
    DETAILED = "detailed"
    DEEP = "deep"

@dataclass
class NetworkNode:
    """Represents a node in the network visualization"""
    id: str
    type: str  # "neuron", "synapse", "region"
    x: float
    y: float
    z: float = 0.0
    radius: float = 1.0
    color: str = "#3498db"
    alpha: float = 1.0
    labels: Dict[str, str] = field(default_factory=dict)
    attributes: Dict[str, Any] = field(default_factory=dict)
    connections: List[str] = field(default_factory=list)

@dataclass
class NetworkLink:
    """Represents a connection in the network visualization"""
    source: str
    target: str
    weight: float = 1.0
    strength: float = 1.0
    type: str = "synapse"
    color: str = "#95a5a6"
    alpha: float = 0.7
    labels: Dict[str, str] = field(default_factory=dict)
    attributes: Dict[str, Any] = field(default_factory=dict)

@dataclass
class NeuralActivityData:
    """Container for neural activity data"""
    timestamp: float
    neuron_ids: List[str]
    spike_counts: List[int]
    firing_rates: List[float]
    membrane_potentials: List[float]
    positions: List[Tuple[float, float, float]]
    region_ids: List[str]
    neuron_types: List[str]

@dataclass
class SimulationMetrics:
    """Container for simulation metrics"""
    simulation_time: float
    total_spikes: int
    firing_rate: float
    active_neurons: int
    total_neurons: int
    total_synapses: int
    excitation_inhibition_ratio: float
    development_stage: str
    memory_usage: float
    cpu_usage: float
    step_time: float
    step_count: int

class NetworkGraph:
    """Interactive network graph visualization"""
    
    def __init__(self, nodes: List[NetworkNode], links: List[NetworkLink],
                 layout: NetworkLayout = NetworkLayout.FORCE_ATTRACTOR):
        self.nodes = {node.id: node for node in nodes}
        self.links = links
        self.layout = layout
        self.current_pos = {node.id: [node.x, node.y] for node in nodes}
        self.node_history = {node.id: [] for node in nodes}
        
    def apply_layout(self):
        """Apply the chosen network layout algorithm"""
        if self.layout == NetworkLayout.FORCE_ATTRACTOR:
            self._force_atractor_layout()
        elif self.layout == NetworkLayout.CIRCULAR:
            self._circular_layout()
        elif self.layout == NetworkLayout.SPRING:
            self._spring_layout()
        elif self.layout == NetworkLayout.RADIALLAYOUT:
            self._radial_layout()
        elif self.layout == NetworkLayout.HEAVE:
            self._heave_layout()
        elif self.layout == NetworkLayout.CIRCOGRAM:
            self._circo_layout()
    
    def _force_atractor_layout(self):
        """Force-directed layout algorithm"""
        # Simplified force-directed layout
        for iteration in range(100):
            # Repulsion between all nodes
            for id1, node1 in self.nodes.items():
                for id2, node2 in self.nodes.items():
                    if id1 != id2:
                        dx = node1.x - node2.x
                        dy = node1.y - node2.y
                        distance = np.sqrt(dx*dx + dy*dy) + 0.1
                        force = 1.0 / distance
                        node1.x += dx * force * 0.01
                        node1.y += dy * force * 0.01
                        node2.x -= dx * force * 0.01
                        node2.y -= dy * force * 0.01
            
            # Attraction along edges
            for link in self.links:
                if link.source in self.nodes and link.target in self.nodes:
                    node1 = self.nodes[link.source]
                    node2 = self.nodes[link.target]
                    dx = node2.x - node1.x
                    dy = node2.y - node1.y
                    distance = np.sqrt(dx*dx + dy*dy)
                    force = link.weight * distance * 0.01
                    if distance > 0:
                        node1.x += dx * force / distance
                        node1.y += dy * force / distance
                        node2.x -= dx * force / distance
                        node2.y -= dy * force / distance
            
            # Update positions
            for node in self.nodes.values():
                node.x = np.clip(node.x, -10, 10)
                node.y = np.clip(node.y, -10, 10)
    
    def _circular_layout(self):
        """Circular layout algorithm"""
        nodes = list(self.nodes.values())
        angle = 2 * np.pi / len(nodes)
        
        for i, node in enumerate(nodes):
            theta = i * angle
            node.x = np.cos(theta) * node.radius
            node.y = np.sin(theta) * node.radius
    
    def _spring_layout(self):
        """Spring layout algorithm (simplified)"""
        # Use force atractor for simplicity
        self._force_atractor_layout()
    
    def _radial_layout(self):
        """Radial layout algorithm"""
        # Simplified radial layout - place nodes in concentric circles
        if not self.nodes:
            return
            
        # Group by region if possible
        regions = {}
        for node in self.nodes.values():
            region = node.attributes.get('region', 'default')
            if region not in regions:
                regions[region] = []
            regions[region].append(node)
        
        # Place regions at different radii
        radii = {region: i * 3 for i, region in enumerate(regions.keys())}
        angles = {region: [] for region in regions.keys()}
        
        for region, nodes_list in regions.items():
            angle_step = 2 * np.pi / len(nodes_list)
            for i, node in enumerate(nodes_list):
                angle = i * angle_step + radii[region] * 0.1
                node.x = radii[region] * np.cos(angle)
                node.y = radii[region] * np.sin(angle)
    
    def _heave_layout(self):
        """Heave layout algorithm"""
        # Simplified implementation - similar to force atractor
        self._force_atractor_layout()
    
    def _circo_layout(self):
        """Circo layout algorithm (simplified)"""
        # Use circular layout for now
        self._circular_layout()
    
    def update_positions(self, time_step: float):
        """Update node positions based on current activity"""
        # Store current positions
        for node_id, pos in self.current_pos.items():
            if node_id in self.nodes:
                self.nodes[node_id].x = pos[0]
                self.nodes[node_id].y = pos[1]
    
    def get_adjacency_matrix(self) -> np.ndarray:
        """Get adjacency matrix for network analysis"""
        node_ids = list(self.nodes.keys())
        matrix = np.zeros((len(node_ids), len(node_ids)))
        
        for i, source_id in enumerate(node_ids):
            for j, target_id in enumerate(node_ids):
                if source_id != target_id:
                    weight = 0.0
                    for link in self.links:
                        if link.source == source_id and link.target == target_id:
                            weight += link.weight
                    matrix[i, j] = weight
        
        return matrix
    
    def get_degree_distribution(self) -> Dict[str, int]:
        """Get degree distribution of the network"""
        degree_dist = {"in": {}, "out": {}, "total": {}}
        
        # In-degree
        in_degree = {node_id: 0 for node_id in self.nodes.keys()}
        # Out-degree
        out_degree = {node_id: 0 for node_id in self.nodes.keys()}
        
        for link in self.links:
            if link.target in in_degree:
                in_degree[link.target] += 1
            if link.source in out_degree:
                out_degree[link.source] += 1
        
        degree_dist["in"] = in_degree
        degree_dist["out"] = out_degree
        
        # Total degree
        total_degree = {}
        for node_id in self.nodes.keys():
            total_degree[node_id] = in_degree[node_id] + out_degree[node_id]
        degree_dist["total"] = total_degree
        
        return degree_dist

class NeuralActivityMonitor:
    """Monitor and analyze neural activity"""
    
    def __init__(self, brain_simulator=None):
        self.brain_simulator = brain_simulator
        self.activity_history: List[NeuralActivityData] = []
        self.current_activity = NeuralActivityData(
            timestamp=0.0,
            neuron_ids=[],
            spike_counts=[],
            firing_rates=[],
            membrane_potentials=[],
            positions=[],
            region_ids=[],
            neuron_types=[]
        )
        self.metrics_history: List[SimulationMetrics] = []
        self.sampling_interval = 0.1  # seconds
        self.is_monitoring = False
        self.monitor_thread = None
    
    def start_monitoring(self, sampling_interval: float = None):
        """Start monitoring neural activity"""
        if sampling_interval is not None:
            self.sampling_interval = sampling_interval
        
        self.is_monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop monitoring neural activity"""
        self.is_monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
            self.monitor_thread = None
    
    def _monitor_loop(self):
        """Main monitoring loop"""
        last_time = time.time()
        
        while self.is_monitoring:
            current_time = time.time()
            elapsed = current_time - last_time
            
            if elapsed >= self.sampling_interval:
                self._update_activity_data(current_time)
                last_time = current_time
            
            time.sleep(0.01)  # Small sleep to prevent busy waiting
    
    def _update_activity_data(self, timestamp: float):
        """Update activity data with current simulation state"""
        if not self.brain_simulator:
            return
        
        # Collect data from brain simulator (simplified)
        neuron_ids = [f"neuron_{i}" for i in range(100)]  # Example
        spike_counts = [i % 5 for i in range(100)]  # Example
        firing_rates = [count * 10.0 for count in spike_counts]  # Hz
        membrane_potentials = np.random.normal(-70, 10, 100).tolist()  # mV
        positions = [(np.random.uniform(-5,5), np.random.uniform(-5,5), 0) for _ in range(100)]
        region_ids = [f"region_{i % 10}" for i in range(100)]
        neuron_types = ["excitatory" if i % 2 == 0 else "inhibitory" for i in range(100)]
        
        activity_data = NeuralActivityData(
            timestamp=timestamp,
            neuron_ids=neuron_ids,
            spike_counts=spike_counts,
            firing_rates=firing_rates,
            membrane_potentials=membrane_potentials,
            positions=positions,
            region_ids=region_ids,
            neuron_types=neuron_types
        )
        
        self.activity_history.append(activity_data)
        
        # Keep only recent data (last 1000 samples)
        if len(self.activity_history) > 1000:
            self.activity_history = self.activity_history[-1000:]
    
    def get_activity_summary(self, time_window: float = 10.0) -> Dict[str, Any]:
        """Get activity summary for a time window"""
        if not self.activity_history:
            return {}
        
        # Filter data within time window
        current_time = self.activity_history[-1].timestamp
        start_time = current_time - time_window
        
        window_data = [d for d in self.activity_history if d.timestamp >= start_time]
        
        if not window_data:
            return {}
        
        # Calculate statistics
        all_spike_counts = []
        all_firing_rates = []
        all_potentials = []
        
        for data in window_data:
            all_spike_counts.extend(data.spike_counts)
            all_firing_rates.extend(data.firing_rates)
            all_potentials.extend(data.membrane_potentials)
        
        return {
            "time_window": time_window,
            "num_samples": len(window_data),
            "total_spikes": sum(all_spike_counts),
            "average_firing_rate": np.mean(all_firing_rates) if all_firing_rates else 0,
            "max_firing_rate": np.max(all_firing_rates) if all_firing_rates else 0,
            "min_firing_rate": np.min(all_firing_rates) if all_firing_rates else 0,
            "std_firing_rate": np.std(all_firing_rates) if all_firing_rates else 0,
            "average_potential": np.mean(all_potentials) if all_potentials else 0,
            "potential_std": np.std(all_potentials) if all_potentials else 0,
            "potential_min": np.min(all_potentials) if all_potentials else 0,
            "potential_max": np.max(all_potentials) if all_potentials else 0,
        }
    
    def plot_activity_over_time(self, output_file: str = "activity_over_time.png"):
        """Plot neural activity over time"""
        if not self.activity_history:
            print("No activity data to plot")
            return
        
        plt.figure(figsize=(12, 8))
        
        # Subplot 1: Spike counts over time
        plt.subplot(2, 3, 1)
        for i in range(min(5, len(self.activity_history)):
            data = self.activity_history[i]
            plt.plot([data.timestamp] * len(data.spike_counts), data.spike_counts, 'o-', alpha=0.6)
        plt.xlabel('Time (s)')
        plt.ylabel('Spike Count')
        plt.title('Spike Activity Over Time')
        plt.grid(True, alpha=0.3)
        
        # Subplot 2: Firing rates distribution
        plt.subplot(2, 3, 2)
        all_rates = []
        for data in self.activity_history:
            all_rates.extend(data.firing_rates)
        if all_rates:
            plt.hist(all_rates, bins=20, alpha=0.7, edgecolor='black')
            plt.xlabel('Firing Rate (Hz)')
            plt.ylabel('Count')
            plt.title('Firing Rate Distribution')
            plt.grid(True, alpha=0.3)
        
        # Subplot 3: Membrane potential over time (sample neurons)
        plt.subplot(2, 3, 3)
        for i in range(min(3, len(self.activity_history)):
            data = self.activity_history[i]
            plt.plot([data.timestamp] * len(data.membrane_potentials), data.membrane_potentials, 'o-', alpha=0.6)
        plt.xlabel('Time (s)')
        plt.ylabel('Membrane Potential (mV)')
        plt.title('Membrane Potential Over Time')
        plt.grid(True, alpha=0.3)
        
        # Subplot 4: Activity summary statistics
        plt.subplot(2, 3, 4)
        summary = self.get_activity_summary()
        if summary:
            metrics = ['average_firing_rate', 'max_firing_rate', 'potential_mean', 'potential_std']
            values = [summary[m] for m in metrics if m in summary]
            labels = [m.replace('_', '\n').replace('firing', 'firing') for m in metrics if m in summary]
            
            bars = plt.bar(range(len(values)), values, alpha=0.7)
            plt.xticks(range(len(values)), labels, rotation=45)
            plt.ylabel('Value')
            plt.title('Activity Summary')
            plt.grid(True, alpha=0.3)
        
        # Subplot 5: Activity by neuron type
        plt.subplot(2, 3, 5)
        neuron_type_counts = {}
        for data in self.activity_history:
            for ntype in data.neuron_types:
                neuron_type_counts[ntype] = neuron_type_counts.get(ntype, 0) + 1
        
        if neuron_type_counts:
            plt.pie(neuron_type_counts.values(), labels=neuron_type_counts.keys(), autopct='%1.1f%%')
            plt.title('Neuron Type Distribution')
        
        # Subplot 6: Activity by region
        plt.subplot(2, 3, 6)
        region_counts = {}
        for data in self.activity_history:
            for region in data.region_ids:
                region_counts[region] = region_counts.get(region, 0) + 1
        
        if region_counts:
            regions = list(region_counts.keys())[:5]  # Show top 5 regions
            counts = [region_counts[r] for r in regions]
            plt.bar(range(len(regions)), counts, alpha=0.7)
            plt.xticks(range(len(regions)), regions, rotation=45)
            plt.ylabel('Activity Count')
            plt.title('Activity by Region (Top 5)')
            plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
    
    def plot_connection_density(self, output_file: str = "connection_density.png"):
        """Plot connection density over time"""
        if not self.activity_history:
            print("No activity data to plot")
            return
        
        plt.figure(figsize=(10, 6))
        
        # Calculate connection density over time (simplified)
        times = []
        densities = []
        
        for i in range(1, len(self.activity_history)):
            prev_data = self.activity_history[i-1]
            curr_data = self.activity_history[i]
            
            # Simplified density calculation
            density = len([s for s in curr_data.spike_counts if s > 0]) / len(curr_data.spike_counts)
            times.append(curr_data.timestamp)
            densities.append(density)
        
        if times:
            plt.plot(times, densities, 'b-', linewidth=2, alpha=0.7)
            plt.xlabel('Time (s)')
            plt.ylabel('Connection Density')
            plt.title('Connection Density Over Time')
            plt.grid(True, alpha=0.3)
            
            # Add horizontal line for average density
            avg_density = np.mean(densities)
            plt.axhline(y=avg_density, color='r', linestyle='--', alpha=0.5, label=f'Average: {avg_density:.3f}')
            plt.legend()
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()

class SimulationDebugger:
    """Debug simulation and provide detailed analysis"""
    
    def __init__(self, brain_simulator=None):
        self.brain_simulator = brain_simulator
        self.debug_level = DebugLevel.DETAILED
        self.breakpoints: List[int] = []
        self.watch_variables: Dict[str, Any] = {}
        self.breakpoint_hit = False
        self.debug_log: List[Dict[str, Any]] = []
    
    def set_debug_level(self, level: DebugLevel):
        """Set debug verbosity level"""
        self.debug_level = level
    
    def add_breakpoint(self, step: int):
        """Add a breakpoint at a specific simulation step"""
        self.breakpoints.append(step)
    
    def remove_breakpoint(self, step: int):
        """Remove a breakpoint"""
        if step in self.breakpoints:
            self.breakpoints.remove(step)
    
    def add_watch_variable(self, name: str, expression: str):
        """Add a variable to watch"""
        self.watch_variables[name] = expression
    
    def remove_watch_variable(self, name: str):
        """Remove a watched variable"""
        if name in self.watch_variables:
            del self.watch_variables[name]
    
    def evaluate_watch_expression(self, expression: str):
        """Evaluate a watch expression (simplified)"""
        # This is a simplified implementation
        # In practice, you'd need a proper expression evaluator
        try:
            # Safe evaluation - only allow basic operations
            if expression.isdigit():
                return int(expression)
            elif expression.replace('.', '').isdigit():
                return float(expression)
            elif expression in ["spikes", "neurons", "synapses"]:
                return 0  # Placeholder
            else:
                return 0  # Default
        except:
            return 0
    
    def check_breakpoints(self, step: int):
        """Check if we've hit any breakpoints"""
        self.breakpoint_hit = step in self.breakpoints
        if self.breakpoint_hit:
            self.log_debug_event("BREAKPOINT", f"Hit breakpoint at step {step}")
            return True
        return False
    
    def log_debug_event(self, event_type: str, message: str, data: Dict[str, Any] = None):
        """Log a debug event"""
        debug_entry = {
            "timestamp": time.time(),
            "event_type": event_type,
            "message": message,
            "data": data or {},
            "step": getattr(self.brain_simulator, 'current_step', 0) if self.brain_simulator else 0
        }
        self.debug_log.append(debug_entry)
        
        if self.debug_level != DebugLevel.NONE:
            print(f"[{event_type}] {message}")
    
    def get_debug_summary(self) -> Dict[str, Any]:
        """Get debug summary"""
        return {
            "total_events": len(self.debug_log),
            "event_types": list(set(entry["event_type"] for entry in self.debug_log)),
            "breakpoints_hit": len([e for e in self.debug_log if e["event_type"] == "BREAKPOINT"]),
            "latest_events": self.debug_log[-10:] if self.debug_log else [],
            "watch_variables": {name: self.evaluate_watch_expression(expr) for name, expr in self.watch_variables.items()}
        }
    
    def export_debug_log(self, filepath: str):
        """Export debug log to a file"""
        with open(filepath, 'w') as f:
            json.dump(self.debug_log, f, indent=2)
    
    def import_debug_log(self, filepath: str):
        """Import debug log from a file"""
        with open(filepath, 'r') as f:
            self.debug_log = json.load(f)
    
    def analyze_simulation_issues(self) -> Dict[str, Any]:
        """Analyze potential simulation issues"""
        issues = {}
        
        # Check for NaN values in debug log
        nan_issues = [entry for entry in self.debug_log if 'data' in entry and any(isinstance(v, float) and np.isnan(v) for v in entry['data'].values())]
        issues["nan_values"] = len(nan_issues)
        
        # Check for errors in debug log
        error_issues = [entry for entry in self.debug_log if entry["event_type"] in ["ERROR", "CRITICAL"]]
        issues["errors_critical"] = len(error_issues)
        
        # Check for long-running operations
        if self.debug_log:
            timestamps = [entry["timestamp"] for entry in self.debug_log]
            if len(timestamps) > 1:
                max_gap = max(timestamps[i+1] - timestamps[i] for i in range(len(timestamps)-1))
                issues["max_time_gap"] = max_gap
        
        return issues

class NLMVisualization:
    """Main visualization interface for NLM"""
    
    def __init__(self, brain_simulator=None):
        self.brain_simulator = brain_simulator
        self.network_graph = None
        self.activity_monitor = NeuralActivityMonitor(brain_simulator)
        self.debugger = SimulationDebugger(brain_simulator)
        self.visualization_level = VisualizationLevel.MEDIUM
    
    def set_visualization_level(self, level: VisualizationLevel):
        """Set visualization detail level"""
        self.visualization_level = level
    
    def create_network_graph(self, output_file: str = "network_graph.png"):
        """Create a network graph visualization"""
        if not self.brain_simulator:
            print("No brain simulator available")
            return
        
        # Create sample nodes and links (simplified)
        nodes = []
        links = []
        
        # Create neuron nodes
        for i in range(50):
            node = NetworkNode(
                id=f"neuron_{i}",
                type="neuron",
                x=np.random.uniform(-5, 5),
                y=np.random.uniform(-5, 5),
                radius=0.5 + np.random.random(),
                color="#3498db" if i % 2 == 0 else "#e74c3c",
                labels={"id": f"N{i:03d}", "type": "neuron"},
                attributes={"region": f"region_{i % 10}", "firing_rate": i % 5}
            )
            nodes.append(node)
        
        # Create synapse links
        for i in range(30):
            source = f"neuron_{i % 50}"
            target = f"neuron_{(i + 10) % 50}"
            link = NetworkLink(
                source=source,
                target=target,
                weight=np.random.random() * 2 + 0.5,
                strength=np.random.random(),
                type="synapse",
                color="#95a5a6",
                alpha=0.7
            )
            links.append(link)
        
        # Create network graph
        self.network_graph = NetworkGraph(nodes, links, NetworkLayout.FORCE_ATTRACTOR)
        
        # Apply layout
        self.network_graph.apply_layout()
        
        # Create visualization
        self._plot_network_graph(output_file)
    
    def _plot_network_graph(self, output_file: str):
        """Plot the network graph"""
        if not self.network_graph:
            return
        
        plt.figure(figsize=(12, 8))
        
        # Draw links
        for link in self.network_graph.links:
            if link.source in self.network_graph.nodes and link.target in self.network_graph.nodes:
                source_node = self.network_graph.nodes[link.source]
                target_node = self.network_graph.nodes[link.target]
                
                plt.plot([source_node.x, target_node.x], 
                        [source_node.y, target_node.y], 
                        'k-', alpha=link.alpha*0.3, linewidth=link.strength*2)
        
        # Draw nodes
        for node in self.network_graph.nodes.values():
            circle = plt.Circle((node.x, node.y), node.radius, 
                              facecolor=node.color, alpha=node.alpha, edgecolor='black', linewidth=1)
            plt.gca().add_patch(circle)
            
            # Add label if needed
            if self.visualization_level != VisualizationLevel.LOW:
                plt.text(node.x, node.y + node.radius + 0.1, node.labels.get("id", node.id),
                        ha='center', va='bottom', fontsize=8, alpha=0.7)
        
        plt.xlim(-10, 10)
        plt.ylim(-10, 10)
        plt.gca().set_aspect('equal')
        plt.title("NLM Network Graph")
        plt.grid(True, alpha=0.3)
        
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
    
    def create_activity_plots(self, output_file: str = "activity_plots.png"):
        """Create comprehensive activity plots"""
        if not self.brain_simulator:
            print("No brain simulator available")
            return
        
        # Start monitoring if not already started
        if not self.activity_monitor.is_monitoring:
            self.activity_monitor.start_monitoring()
        
        # Let it collect some data
        time.sleep(2)
        
        # Create plots
        self.activity_monitor.plot_activity_over_time("activity_over_time.png")
        self.activity_monitor.plot_connection_density("connection_density.png")
        
        # Combine plots
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        
        # Activity over time
        if plt.pathlib.Path("activity_over_time.png").exists():
            img1 = plt.imread("activity_over_time.png")
            axes[0, 0].imshow(img1)
            axes[0, 0].set_title("Activity Over Time")
        
        # Connection density
        if plt.pathlib.Path("connection_density.png").exists():
            img2 = plt.imread("connection_density.png")
            axes[0, 1].imshow(img2)
            axes[0, 1].set_title("Connection Density")
        
        # Network graph
        if plt.pathlib.Path("network_graph.png").exists():
            img3 = plt.imread("network_graph.png")
            axes[1, 0].imshow(img3)
            axes[1, 0].set_title("Network Graph")
        
        # Debug summary
        debug_summary = self.debugger.get_debug_summary()
        axes[1, 1].bar(range(len(debug_summary)), list(debug_summary.values())[:4], alpha=0.7)
        axes[1, 1].set_xticks(range(len(debug_summary)))
        axes[1, 1].set_xticklabels(list(debug_summary.keys())[:4], rotation=45)
        axes[1, 1].set_title("Debug Summary")
        axes[1, 1].set_ylabel("Count")
        
        plt.suptitle("NLM Visualization Dashboard", fontsize=16)
        plt.tight_layout()
        
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
        
        # Clean up temporary files
        for temp_file in ["activity_over_time.png", "connection_density.png", "network_graph.png"]:
            if plt.pathlib.Path(temp_file).exists():
                os.remove(temp_file)
    
    def create_debug_dashboard(self, output_file: str = "debug_dashboard.png"):
        """Create a debug dashboard"""
        plt.figure(figsize=(16, 12))
        
        # Create subplots
        gs = plt.GridSpec(3, 4, hspace=0.3, wspace=0.3)
        
        # Debug log overview
        ax1 = plt.subplot(gs[0, :2])
        debug_summary = self.debugger.get_debug_summary()
        if debug_summary.get("latest_events"):
            events = debug_summary["latest_events"]
            ax1.bar(range(len(events)), [1] * len(events), alpha=0.7)
            ax1.set_xticks(range(len(events)))
            ax1.set_xticklabels([f"{e['event_type']} at step {e['step']}" for e in events], rotation=45)
            ax1.set_title("Latest Debug Events")
            ax1.set_ylabel("Event")
        
        # Watch variables
        ax2 = plt.subplot(gs[0, 2:])
        if debug_summary.get("watch_variables"):
            variables = debug_summary["watch_variables"]
            ax2.bar(variables.keys(), variables.values(), alpha=0.7)
            ax2.set_title("Watch Variables")
            ax2.set_ylabel("Value")
            plt.setp(ax2.get_xticklabels(), rotation=45)
        
        # Simulation metrics (placeholder)
        ax3 = plt.subplot(gs[1, :])
        ax3.plot([0, 1, 2, 3, 4], [0, 1, 0.5, 1.2, 0.8], 'b-', alpha=0.7, linewidth=2)
        ax3.set_title("Simulation Metrics (Placeholder)")
        ax3.set_ylabel("Metric Value")
        ax3.grid(True, alpha=0.3)
        
        # Performance metrics (placeholder)
        ax4 = plt.subplot(gs[2, :2])
        ax4.plot([0, 1, 2, 3, 4], [100, 95, 98, 102, 97], 'r-', alpha=0.7, linewidth=2)
        ax4.set_title("Performance Metrics (Placeholder)")
        ax4.set_ylabel("Performance")
        ax4.grid(True, alpha=0.3)
        
        # Network statistics (placeholder)
        ax5 = plt.subplot(gs[2, 2:])
        categories = ['Neurons', 'Synapses', 'Regions', 'Populations']
        values = [50, 120, 10, 8]
        ax5.bar(categories, values, alpha=0.7, color=['#3498db', '#2ecc71', '#f39c12', '#e74c3c'])
        ax5.set_title("Network Statistics")
        ax5.set_ylabel("Count")
        
        plt.suptitle("NLM Debug Dashboard", fontsize=16, fontweight='bold')
        plt.tight_layout()
        
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
    
    def export_all_visualizations(self, output_dir: str = "nlm_visualizations"):
        """Export all visualizations to a directory"""
        os.makedirs(output_dir, exist_ok=True)
        
        # Create visualizations
        self.create_network_graph(os.path.join(output_dir, "network_graph.png"))
        self.create_activity_plots(os.path.join(output_dir, "activity_plots.png"))
        self.create_debug_dashboard(os.path.join(output_dir, "debug_dashboard.png"))
        
        # Export debug log
        self.debugger.export_debug_log(os.path.join(output_dir, "debug_log.json"))
        
        # Create a summary report
        summary = {
            "timestamp": time.time(),
            "network_graph": "network_graph.png",
            "activity_plots": "activity_plots.png",
            "debug_dashboard": "debug_dashboard.png",
            "debug_log": "debug_log.json",
            "debug_summary": self.debugger.get_debug_summary(),
            "activity_summary": self.activity_monitor.get_activity_summary()
        }
        
        with open(os.path.join(output_dir, "visualization_summary.json"), 'w') as f:
            json.dump(summary, f, indent=2)
        
        return summary

# Convenience functions
def create_default_visualization(brain_simulator=None) -> NLMVisualization:
    """Create a default NLM visualization"""
    return NLMVisualization(brain_simulator)

def export_visualization_summary(output_dir: str = "nlm_visualizations") -> Dict[str, Any]:
    """Export visualization summary"""
    visualization = create_default_visualization()
    return visualization.export_all_visualizations(output_dir)

# Main execution
if __name__ == "__main__":
    # Create visualization
    vis = create_default_visualization()
    
    # Create network graph
    vis.create_network_graph("network_graph.png")
    
    # Create activity plots
    vis.create_activity_plots("activity_plots.png")
    
    # Create debug dashboard
    vis.create_debug_dashboard("debug_dashboard.png")
    
    # Export all visualizations
    summary = vis.export_all_visualizations("nlm_visualizations")
    
    print(f"Visualizations created in 'nlm_visualizations' directory")
    print(f"Summary: {json.dumps(summary, indent=2)}")

# Export functions
__all__ = [
    'VisualizationLevel',
    'NetworkLayout',
    'ActivityView',
    'DebugLevel',
    'NetworkNode',
    'NetworkLink',
    'NeuralActivityData',
    'SimulationMetrics',
    'NetworkGraph',
    'NeuralActivityMonitor',
    'SimulationDebugger',
    'NLMVisualization',
    'create_default_visualization',
    'export_visualization_summary'
]