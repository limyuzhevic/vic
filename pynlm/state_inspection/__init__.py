"""
pynlm.state_inspection - Tools for inspecting brain state and simulation state

This module provides:
- Brain state snapshot capabilities
- Connection inspection tools
- Memory content inspection
- Neural activity visualization tools
"""

import time
import json
import numpy as np
import pandas as pd
from typing import Dict, Any, List, Optional, Tuple, Union, Set
from dataclasses import dataclass, field, asdict
from abc import ABC, abstractmethod
from pathlib import Path
import copy
import threading
from collections import defaultdict, deque
import warnings

# Import logging components
from pynlm.logging import (
    get_brain_logger, get_memory_logger, get_simulation_logger,
    get_plastic_logger, get_visualization_logger
)

logger = get_simulation_logger()


@dataclass
class BrainStateInfo:
    """Information about brain state."""
    timestamp: float = field(default_factory=time.time)
    step: int = 0
    brain_regions: Dict[str, Any] = field(default_factory=dict)
    neuron_populations: Dict[str, Any] = field(default_factory=dict)
    synaptic_weights: Dict[str, Any] = field(default_factory=dict)
    connectivity_matrix: Optional[np.ndarray] = None
    memory_states: Dict[str, Any] = field(default_factory=dict)
    neuromodulator_levels: Dict[str, Any] = field(default_factory=dict)
    simulation_parameters: Dict[str, Any] = field(default_factory=dict)
    performance_metrics: Dict[str, Any] = field(default_factory=dict)
    metadata: Dict[str, Any] = field(default_factory=dict)


class StateInspector:
    """Base class for state inspection tools."""

    def __init__(self, inspection_id: str):
        """
        Initialize state inspector.

        Parameters
        ----------
        inspection_id : str
            Unique identifier for the inspector
        """
        self.inspection_id = inspection_id
        self.inspection_history: List[Dict[str, Any]] = []
        self.current_state: Optional[BrainStateInfo] = None
        self._in_inspection_mode = False

    def start_inspection(self) -> None:
        """Start state inspection."""
        self._in_inspection_mode = True
        logger.info(f"Started state inspection: {self.inspection_id}")

    def stop_inspection(self) -> None:
        """Stop state inspection."""
        self._in_inspection_mode = False
        logger.info(f"Stopped state inspection: {self.inspection_id}")

    def is_inspecting(self) -> bool:
        """Check if currently inspecting."""
        return self._in_inspection_mode

    @abstractmethod
    def inspect_state(self) -> Dict[str, Any]:
        """Inspect current state. Must be implemented by subclasses."""
        pass


class BrainStateSnapshotter(BrainStateInspector):
    """Take snapshots of brain state for analysis."""

    def __init__(self, snapshot_id: str, auto_snapshot: bool = False,
                 snapshot_interval: float = 1.0):
        """
        Initialize brain state snapshotter.

        Parameters
        ----------
        snapshot_id : str
            Unique identifier for the snapshotter
        auto_snapshot : bool
            Enable automatic snapshots
        snapshot_interval : float
            Interval between automatic snapshots in seconds
        """
        super().__init__(snapshot_id)
        self.auto_snapshot = auto_snapshot
        self.snapshot_interval = snapshot_interval
        self._snapshot_thread = None
        self._snapshot_running = False
        self._snapshots_taken = 0

    def inspect_state(self) -> Dict[str, Any]:
        """Create a snapshot of current brain state."""
        # This would typically capture state from the actual brain simulation
        # For now, we create a sample snapshot structure
        snapshot = BrainStateInfo(
            step=self.current_state.step + self._snapshots_taken if self.current_state else 0,
            metadata={
                "inspector_id": self.inspection_id,
                "timestamp": time.time()
            }
        )

        # Sample data for demonstration
        if self.current_state:
            # Copy relevant parts from current state
            snapshot.brain_regions = copy.deepcopy(self.current_state.brain_regions)
            snapshot.neuron_populations = copy.deepcopy(self.current_state.neuron_populations)
            snapshot.synaptic_weights = copy.deepcopy(self.current_state.synaptic_weights)
            snapshot.connectivity_matrix = self.current_state.connectivity_matrix.copy() if self.current_state.connectivity_matrix is not None else None
            snapshot.memory_states = copy.deepcopy(self.current_state.memory_states)
            snapshot.neuromodulator_levels = copy.deepcopy(self.current_state.neuromodulator_levels)
            snapshot.simulation_parameters = copy.deepcopy(self.current_state.simulation_parameters)
            snapshot.performance_metrics = copy.deepcopy(self.current_state.performance_metrics)

        self.inspection_history.append(asdict(snapshot))
        self._snapshots_taken += 1

        logger.debug(f"Created brain state snapshot for step {snapshot.step}")
        return asdict(snapshot)

    def start_auto_snapshot(self):
        """Start automatic snapshot collection."""
        if self.auto_snapshot and not self._snapshot_running:
            self._snapshot_running = True
            self._snapshot_thread = threading.Thread(target=self._auto_snapshot_loop)
            self._snapshot_thread.daemon = True
            self._snapshot_thread.start()
            logger.info(f"Started automatic snapshots for {self.inspection_id}")

    def stop_auto_snapshot(self):
        """Stop automatic snapshot collection."""
        self._snapshot_running = False
        if self._snapshot_thread:
            self._snapshot_thread.join(timeout=1.0)
            self._snapshot_thread = None
        logger.info(f"Stopped automatic snapshots for {self.inspection_id}")

    def _auto_snapshot_loop(self):
        """Automatic snapshot collection loop."""
        while self._snapshot_running:
            try:
                self.inspect_state()
                time.sleep(self.snapshot_interval)
            except Exception as e:
                logger.error(f"Error in auto-snapshot loop: {e}")

    def get_recent_snapshots(self, limit: int = 10) -> List[Dict[str, Any]]:
        """
        Get recent snapshots.

        Parameters
        ----------
        limit : int
            Maximum number of snapshots to return

        Returns
        -------
        List[Dict[str, Any]]
            Recent snapshots
        """
        return self.inspection_history[-limit:]

    def compare_snapshots(self, snapshot1_idx: int, snapshot2_idx: int) -> Dict[str, Any]:
        """
        Compare two snapshots.

        Parameters
        ----------
        snapshot1_idx : int
            Index of first snapshot
        snapshot2_idx : int
            Index of second snapshot

        Returns
        -------
        Dict[str, Any]
            Comparison results
        """
        if snapshot1_idx >= len(self.inspection_history):
            raise ValueError(f"Snapshot index {snapshot1_idx} out of range")
        if snapshot2_idx >= len(self.inspection_history):
            raise ValueError(f"Snapshot index {snapshot2_idx} out of range")

        snap1 = self.inspection_history[snapshot1_idx]
        snap2 = self.inspection_history[snapshot2_idx]

        comparison = {
            "snapshot1_index": snapshot1_idx,
            "snapshot2_index": snapshot2_idx,
            "step_difference": snap2.get("step", 0) - snap1.get("step", 0),
            "timestamp_difference": snap2.get("timestamp", 0) - snap1.get("timestamp", 0),
            "differences": {}
        }

        # Compare brain regions
        if "brain_regions" in snap1 and "brain_regions" in snap2:
            regions1 = snap1["brain_regions"]
            regions2 = snap2["brain_regions"]
            comparison["differences"]["brain_regions"] = {
                "new_regions": set(regions2.keys()) - set(regions1.keys()),
                "removed_regions": set(regions1.keys()) - set(regions2.keys()),
                "changed_regions": {
                    region: {
                        "step1": regions1.get(region),
                        "step2": regions2.get(region)
                    }
                    for region in set(regions1.keys()) & set(regions2.keys())
                    if regions1.get(region) != regions2.get(region)
                }
            }

        # Compare neuron populations
        if "neuron_populations" in snap1 and "neuron_populations" in snap2:
            pops1 = snap1["neuron_populations"]
            pops2 = snap2["neuron_populations"]
            comparison["differences"]["neuron_populations"] = {
                "population_changes": {
                    pop: {
                        "step1": pops1.get(pop, 0),
                        "step2": pops2.get(pop, 0),
                        "change": pops2.get(pop, 0) - pops1.get(pop, 0)
                    }
                    for pop in set(pops1.keys()) | set(pops2.keys())
                }
            }

        logger.info(f"Compared snapshots {snapshot1_idx} and {snapshot2_idx}")
        return comparison


class ConnectionInspector:
    """Inspect neural connections and connectivity."""

    def __init__(self):
        """Initialize connection inspector."""
        self.connection_history: List[Dict[str, Any]] = []
        self._connection_cache: Dict[str, Any] = {}

    def inspect_connectivity(self, connectivity_data: Dict[str, Any],
                           inspection_type: str = "full") -> Dict[str, Any]:
        """
        Inspect neural connectivity.

        Parameters
        ----------
        connectivity_data : Dict[str, Any]
            Connectivity data to inspect
        inspection_type : str
            Type of inspection ("full", "summary", "health")

        Returns
        -------
        Dict[str, Any]
            Inspection results
        """
        inspection_result = {
            "timestamp": time.time(),
            "inspection_type": inspection_type,
            "total_connections": 0,
            "connection_types": {},
            "connectivity_patterns": {},
            "connectivity_health": {},
            "isolated_neurons": [],
            "hub_neurons": [],
            "weight_statistics": {},
            "connectivity_issues": []
        }

        # Basic connectivity analysis
        if isinstance(connectivity_data, dict):
            # Count connections
            for source, connections in connectivity_data.items():
                if isinstance(connections, dict):
                    inspection_result["total_connections"] += len(connections)

                    # Analyze connection types
                    for target, conn_info in connections.items():
                        if isinstance(conn_info, dict):
                            conn_type = conn_info.get("type", "unknown")
                            weight = conn_info.get("weight", 0)

                            inspection_result["connection_types"][conn_type] = (
                                inspection_result["connection_types"].get(conn_type, 0) + 1
                            )

                            # Analyze weights
                            if "weight" in conn_info:
                                if "weight_distribution" not in inspection_result["weight_statistics"]:
                                    inspection_result["weight_statistics"] = {
                                        "weights": [],
                                        "mean": 0,
                                        "std": 0,
                                        "min": float('inf'),
                                        "max": float('-inf'),
                                        "zero_connections": 0
                                    }

                                inspection_result["weight_statistics"]["weights"].append(weight)

                                if weight == 0:
                                    inspection_result["weight_statistics"]["zero_connections"] += 1

            # Identify isolated neurons (neurons with no outgoing connections)
            for source, connections in connectivity_data.items():
                if not connections or (isinstance(connections, dict) and len(connections) == 0):
                    inspection_result["isolated_neurons"].append(source)

            # Identify hub neurons (neurons with many connections)
            for source, connections in connectivity_data.items():
                if isinstance(connections, dict) and len(connections) > 10:
                    inspection_result["hub_neurons"].append({
                        "neuron": source,
                        "connection_count": len(connections)
                    })

        # Calculate weight statistics
        if "weight_statistics" in inspection_result and inspection_result["weight_statistics"]["weights"]:
            weights = inspection_result["weight_statistics"]["weights"]
            inspection_result["weight_statistics"]["mean"] = np.mean(weights)
            inspection_result["weight_statistics"]["std"] = np.std(weights)
            inspection_result["weight_statistics"]["min"] = min(weights)
            inspection_result["weight_statistics"]["max"] = max(weights)
            del inspection_result["weight_statistics"]["weights"]

        # Identify connectivity issues
        if "connection_types" in inspection_result:
            for conn_type, count in inspection_result["connection_types"].items():
                if count == 0:
                    inspection_result["connectivity_issues"].append(
                        f"No connections of type: {conn_type}"
                    )

        if inspection_result["isolated_neurons"]:
            inspection_result["connectivity_issues"].append(
                f"{len(inspection_result['isolated_neurons'])} isolated neurons"
            )

        self.connection_history.append(inspection_result)
        logger.debug(f"Inspected connectivity: {inspection_result['total_connections']} connections")

        return inspection_result

    def get_connection_history(self, limit: int = 100) -> List[Dict[str, Any]]:
        """
        Get connection inspection history.

        Parameters
        ----------
        limit : int
            Maximum number of history entries

        Returns
        -------
        List[Dict[str, Any]]
            Connection inspection history
        """
        return self.connection_history[-limit:]

    def analyze_connection_evolution(self, steps: Optional[int] = None) -> Dict[str, Any]:
        """
        Analyze how connectivity evolves over time.

        Parameters
        ----------
        steps : int, optional
            Number of steps to analyze

        Returns
        -------
        Dict[str, Any]
            Connection evolution analysis
        """
        history = self.get_connection_history(steps)
        if not history:
            return {}

        analysis = {
            "total_inspections": len(history),
            "start_step": 0,
            "end_step": 0,
            "connection_growth": [],
            "type_changes": defaultdict(list),
            "stability_metrics": {}
        }

        # Extract step numbers if available
        steps_list = []
        for entry in history:
            if "step" in entry:
                steps_list.append(entry["step"])

        if steps_list:
            analysis["start_step"] = min(steps_list)
            analysis["end_step"] = max(steps_list)

        # Analyze connection growth trends
        for i in range(1, len(history)):
            prev = history[i-1]
            curr = history[i]

            growth = {
                "step": curr.get("step", i),
                "total_connections": curr.get("total_connections", 0) - prev.get("total_connections", 0),
                "new_types": set(curr.get("connection_types", {}).keys()) - set(prev.get("connection_types", {}).keys())
            }

            analysis["connection_growth"].append(growth)

            # Track type changes
            for conn_type in set(prev.get("connection_types", {}).keys()) | set(curr.get("connection_types", {}).keys()):
                prev_count = prev.get("connection_types", {}).get(conn_type, 0)
                curr_count = curr.get("connection_types", {}).get(conn_type, 0)
                if prev_count != curr_count:
                    analysis["type_changes"][conn_type].append({
                        "step": curr.get("step", i),
                        "change": curr_count - prev_count
                    })

        logger.info(f"Analyzed connection evolution for {len(history)} inspections")
        return analysis


class MemoryContentInspector:
    """Inspect memory content and states."""

    def __init__(self):
        """Initialize memory content inspector."""
        self.memory_history: List[Dict[str, Any]] = []
        self.memory_analyzers: Dict[str, Callable] = {}

    def add_memory_analyzer(self, name: str, analyzer_func: Callable,
                          description: str = "") -> None:
        """
        Add a memory analyzer.

        Parameters
        ----------
        name : str
        analyzer_func : Callable
            Function that takes memory data and returns analysis
        description : str
            Description of the analyzer
        """
        self.memory_analyzers[name] = lambda data: (analyzer_func(data), description)

    def inspect_memory(self, memory_data: Dict[str, Any],
                      inspection_depth: str = "full") -> Dict[str, Any]:
        """
        Inspect memory content.

        Parameters
        ----------
        memory_data : Dict[str, Any]
            Memory data to inspect
        inspection_depth : str
            Depth of inspection ("quick", "full", "deep")

        Returns
        -------
        Dict[str, Any]
            Memory inspection results
        """
        inspection_result = {
            "timestamp": time.time(),
            "inspection_depth": inspection_depth,
            "memory_types": {},
            "total_memories": 0,
            "memory_sizes": {},
            "access_patterns": {},
            "memory_health": {},
            "memory_issues": []
        }

        # Analyze memory by type
        if isinstance(memory_data, dict):
            for memory_type, memories in memory_data.items():
                if isinstance(memories, dict):
                    inspection_result["memory_types"][memory_type] = len(memories)
                    inspection_result["total_memories"] += len(memories)

                    # Analyze memory sizes
                    sizes = []
                    for memory_id, memory_info in memories.items():
                        if isinstance(memory_info, dict):
                            # Extract size if available
                            if "size" in memory_info:
                                size = memory_info["size"]
                                sizes.append(size)
                            elif "data" in memory_info and isinstance(memory_info["data"], (np.ndarray, list)):
                                if isinstance(memory_info["data"], np.ndarray):
                                    size = memory_info["data"].size
                                else:
                                    size = len(memory_info["data"])
                                sizes.append(size)

                            # Analyze access patterns
                            if "access_count" in memory_info:
                                access_count = memory_info["access_count"]
                                inspection_result["access_patterns"][memory_id] = {
                                    "access_count": access_count,
                                    "last_access": memory_info.get("last_access", 0)
                                }

                    if sizes:
                        inspection_result["memory_sizes"][memory_type] = {
                            "mean_size": np.mean(sizes),
                            "max_size": max(sizes),
                            "min_size": min(sizes),
                            "total_size": sum(sizes)
                        }

        # Apply memory analyzers
        for analyzer_name, analyzer_func in self.memory_analyzers.items():
            try:
                analysis_result = analyzer_func(memory_data)
                if analysis_result[0]:
                    inspection_result["memory_health"][analyzer_name] = analysis_result
            except Exception as e:
                inspection_result["memory_issues"].append(
                    f"Analyzer {analyzer_name} failed: {str(e)}"
                )

        # Identify memory issues
        if inspection_result["total_memories"] == 0:
            inspection_result["memory_issues"].append("No memories found")

        # Check for large memories
        for mem_type, stats in inspection_result.get("memory_sizes", {}).items():
            if stats["max_size"] > 1000000:  # 1M elements
                inspection_result["memory_issues"].append(
                    f"Large memory detected in type {mem_type}: {stats['max_size']} elements"
                )

        self.memory_history.append(inspection_result)
        logger.debug(f"Inspected memory: {inspection_result['total_memories']} total memories")

        return inspection_result

    def get_memory_history(self, limit: int = 100) -> List[Dict[str, Any]]:
        """
        Get memory inspection history.

        Parameters
        ----------
        limit : int
            Maximum number of history entries

        Returns
        -------
        List[Dict[str, Any]]
            Memory inspection history
        """
        return self.memory_history[-limit:]

    def analyze_memory_usage(self, hours: Optional[int] = None) -> Dict[str, Any]:
        """
        Analyze memory usage patterns.

        Parameters
        ----------
        hours : int, optional
            Number of hours to analyze

        Returns
        -------
        Dict[str, Any]
            Memory usage analysis
        """
        history = self.get_memory_history()
        if not history:
            return {}

        # Filter by time if specified
        if hours:
            cutoff_time = time.time() - (hours * 3600)
            history = [h for h in history if h["timestamp"] >= cutoff_time]

        analysis = {
            "total_inspections": len(history),
            "memory_growth": [],
            "memory_type_changes": defaultdict(list),
            "access_patterns": {},
            "memory_efficiency": {}
        }

        # Analyze memory growth over time
        for i in range(1, len(history)):
            prev = history[i-1]
            curr = history[i]

            growth = {
                "timestamp": curr["timestamp"],
                "total_memories": curr["total_memories"] - prev["total_memories"],
                "memory_types_added": set(curr["memory_types"].keys()) - set(prev["memory_types"].keys()),
                "memory_types_removed": set(prev["memory_types"].keys()) - set(curr["memory_types"].keys())
            }

            analysis["memory_growth"].append(growth)

        # Analyze access patterns
        for entry in history:
            for memory_id, access_info in entry.get("access_patterns", {}).items():
                if memory_id not in analysis["access_patterns"]:
                    analysis["access_patterns"][memory_id] = {
                        "access_counts": [],
                        "last_access_times": []
                    }

                analysis["access_patterns"][memory_id]["access_counts"].append(
                    access_info["access_count"]
                )
                analysis["access_patterns"][memory_id]["last_access_times"].append(
                    access_info["last_access"]
                )

        logger.info(f"Analyzed memory usage for {len(history)} inspections")
        return analysis


class NeuralActivityVisualizer:
    """Tools for visualizing neural activity."""

    def __init__(self, output_dir: Optional[str] = None):
        """
        Initialize neural activity visualizer.

        Parameters
        ----------
        output_dir : str, optional
            Directory for output files
        """
        self.output_dir = Path(output_dir) if output_dir else None
        if self.output_dir:
            self.output_dir.mkdir(parents=True, exist_ok=True)

    def create_activity_heatmap(self, neuron_activity: Dict[str, Any],
                               region: Optional[str] = None,
                               time_window: Optional[Tuple[float, float]] = None) -> Dict[str, Any]:
        """
        Create activity heatmap visualization.

        Parameters
        ----------
        neuron_activity : Dict[str, Any]
            Neuron activity data
        region : str, optional
            Specific brain region to visualize
        time_window : Tuple[float, float], optional
            Time window for analysis

        Returns
        -------
        Dict[str, Any]
            Heatmap visualization data
        """
        visualization_result = {
            "type": "activity_heatmap",
            "timestamp": time.time(),
            "region": region,
            "time_window": time_window,
            "data": {},
            "statistics": {},
            "metadata": {}
        }

        # Process neuron activity
        if isinstance(neuron_activity, dict):
            # Filter by region if specified
            if region and region in neuron_activity:
                activity_data = neuron_activity[region]
            else:
                activity_data = neuron_activity

            # Create activity matrix
            activity_matrix = {}
            for neuron_id, activity in activity_data.items():
                if isinstance(activity, dict):
                    # Extract activity values
                    if "firing_rate" in activity:
                        activity_matrix[neuron_id] = activity["firing_rate"]
                    elif "activity" in activity:
                        activity_matrix[neuron_id] = activity["activity"]
                    elif "spike_count" in activity:
                        activity_matrix[neuron_id] = activity["spike_count"]

            visualization_result["data"]["activity_matrix"] = activity_matrix

            # Calculate statistics
            if activity_matrix:
                values = list(activity_matrix.values())
                visualization_result["statistics"] = {
                    "mean_activity": np.mean(values),
                    "max_activity": max(values),
                    "min_activity": min(values),
                    "std_activity": np.std(values),
                    "total_neurons": len(activity_matrix)
                }

            visualization_result["metadata"]["neuron_count"] = len(activity_matrix)

        logger.debug(f"Created activity heatmap for {region or 'all regions'}")
        return visualization_result

    def create_connectivity_plot(self, connectivity_data: Dict[str, Any],
                                 plot_type: str = "adjacency") -> Dict[str, Any]:
        """
        Create connectivity visualization.

        Parameters
        ----------
        connectivity_data : Dict[str, Any]
            Connectivity data
        plot_type : str
            Type of plot ("adjacency", "degree_distribution", "cluster_analysis")

        Returns
        -------
        Dict[str, Any]
            Connectivity visualization data
        """
        visualization_result = {
            "type": "connectivity_plot",
            "plot_type": plot_type,
            "timestamp": time.time(),
            "data": {},
            "statistics": {},
            "metadata": {}
        }

        if plot_type == "adjacency" and isinstance(connectivity_data, dict):
            # Create adjacency-like data for visualization
            adjacency_data = {}
            for source, connections in connectivity_data.items():
                if isinstance(connections, dict):
                    adjacency_data[source] = list(connections.keys())

            visualization_result["data"]["adjacency"] = adjacency_data
            visualization_result["metadata"]["nodes"] = len(adjacency_data)
            visualization_result["metadata"]["edges"] = sum(len(conns) for conns in adjacency_data.values())

        elif plot_type == "degree_distribution" and isinstance(connectivity_data, dict):
            # Calculate degree distribution
            degrees = []
            for source, connections in connectivity_data.items():
                if isinstance(connections, dict):
                    degrees.append(len(connections))

            if degrees:
                visualization_result["data"]["degree_distribution"] = {
                    "degrees": sorted(degrees),
                    "histogram": self._create_degree_histogram(degrees)
                }
                visualization_result["statistics"]["mean_degree"] = np.mean(degrees)
                visualization_result["statistics"]["max_degree"] = max(degrees)
                visualization_result["statistics"]["min_degree"] = min(degrees)

        elif plot_type == "cluster_analysis" and isinstance(connectivity_data, dict):
            # Simple cluster analysis
            clusters = self._detect_clusters(connectivity_data)
            visualization_result["data"]["clusters"] = clusters
            visualization_result["metadata"]["num_clusters"] = len(clusters)

        logger.debug(f"Created {plot_type} visualization")
        return visualization_result

    def _create_degree_histogram(self, degrees: List[int]) -> Dict[str, Any]:
        """Create degree distribution histogram."""
        if not degrees:
            return {}

        max_degree = max(degrees)
        histogram = {}

        for degree in degrees:
            if degree not in histogram:
                histogram[degree] = 0
            histogram[degree] += 1

        return {
            "degrees": list(histogram.keys()),
            "counts": list(histogram.values()),
            "max_degree": max_degree
        }

    def _detect_clusters(self, connectivity_data: Dict[str, Any]) -> List[List[str]]:
        """Detect clusters in connectivity data."""
        if not isinstance(connectivity_data, dict):
            return []

        # Simple connected components detection
        nodes = list(connectivity_data.keys())
        adjacency = {node: set() for node in nodes}

        for source, connections in connectivity_data.items():
            if isinstance(connections, dict):
                for target in connections.keys():
                    if target in adjacency:
                        adjacency[source].add(target)
                        adjacency[target].add(source)

        # Find connected components
        visited = set()
        clusters = []

        for node in nodes:
            if node not in visited:
                stack = [node]
                cluster = []
                while stack:
                    current = stack.pop()
                    if current not in visited:
                        visited.add(current)
                        cluster.append(current)
                        stack.extend(adjacency[current] - visited)

                if cluster:
                    clusters.append(cluster)

        return clusters

    def create_visualization_dashboard(self, visualization_data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Create a comprehensive visualization dashboard.

        Parameters
        ----------
        visualization_data : List[Dict[str, Any]]
            List of visualization data

        Returns
        -------
        Dict[str, Any]
            Dashboard data
        """
        dashboard = {
            "type": "visualization_dashboard",
            "timestamp": time.time(),
            "components": [],
            "layout": {
                "grid_columns": 2,
                "grid_rows": 2,
                "component_sizes": ["large", "large", "medium", "medium"]
            },
            "metadata": {
                "total_visualizations": len(visualization_data),
                "dashboard_id": f"dashboard_{int(time.time())}"
            }
        }

        # Add each visualization as a component
        for i, data in enumerate(visualization_data):
            component = {
                "id": f"component_{i}",
                "type": data.get("type", "unknown"),
                "data": data,
                "position": {
                    "row": i // 2,
                    "column": i % 2
                }
            }
            dashboard["components"].append(component)

        logger.debug(f"Created visualization dashboard with {len(visualization_data)} components")
        return dashboard


# Convenience instances
brain_state_snapshotter = BrainStateSnapshotter("default")
connection_inspector = ConnectionInspector()
memory_content_inspector = MemoryContentInspector()
neural_activity_visualizer = NeuralActivityVisualizer()


def get_brain_state_snapshotter(snapshot_id: Optional[str] = None) -> BrainStateSnapshotter:
    """Get brain state snapshotter instance."""
    if snapshot_id:
        return BrainStateSnapshotter(snapshot_id)
    return brain_state_snapshotter


def get_connection_inspector() -> ConnectionInspector:
    """Get connection inspector instance."""
    return connection_inspector


def get_memory_content_inspector() -> MemoryContentInspector:
    """Get memory content inspector instance."""
    return memory_content_inspector


def get_neural_activity_visualizer(output_dir: Optional[str] = None) -> NeuralActivityVisualizer:
    """Get neural activity visualizer instance."""
    if output_dir:
        return NeuralActivityVisualizer(output_dir)
    return neural_activity_visualizer