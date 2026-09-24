"""
pynlm.debug - Debug utilities for NLM project

This module provides debugging and troubleshooting tools including:
- Brain state inspection utilities
- Simulation step debugging
- Connection validation tools
- Memory usage monitoring
- Performance profiling tools
"""

import time
import psutil
import numpy as np
import pandas as pd
from typing import Dict, Any, List, Optional, Tuple, Union, Callable
from dataclasses import dataclass, field
from abc import ABC, abstractmethod
import threading
import queue
import json
from datetime import datetime
from pathlib import Path
import warnings
import sys
import traceback
from collections import defaultdict, deque
import gc
import resource

# Import logging
from pynlm.logging import (
    get_brain_logger, get_memory_logger, get_simulation_logger,
    get_plastic_logger, get_visualization_logger, get_checkpoint_logger
)

logger = get_simulation_logger()


@dataclass
class BrainStateSnapshot:
    """Snapshot of brain state for debugging."""
    timestamp: float = field(default_factory=time.time)
    step: int = 0
    brain_regions: Dict[str, Any] = field(default_factory=dict)
    neurons: Dict[str, Any] = field(default_factory=dict)
    synapses: Dict[str, Any] = field(default_factory=dict)
    connectivity: Dict[str, Any] = field(default_factory=dict)
    memory_states: Dict[str, Any] = field(default_factory=dict)
    neuromodulators: Dict[str, Any] = field(default_factory=dict)
    simulation_parameters: Dict[str, Any] = field(default_factory=dict)
    performance_metrics: Dict[str, Any] = field(default_factory=dict)


class BrainStateInspector:
    """Utility for inspecting brain state and neural components."""

    def __init__(self, brain_state_file: Optional[str] = None):
        """
        Initialize brain state inspector.

        Parameters
        ----------
        brain_state_file : str, optional
            Path to brain state file for loading/saving
        """
        self.brain_state_file = Path(brain_state_file) if brain_state_file else None
        self._state_cache: Dict[str, Any] = {}
        self._inspection_history: List[BrainStateSnapshot] = []

    def create_snapshot(self, step: int, **kwargs) -> BrainStateSnapshot:
        """
        Create a snapshot of current brain state.

        Parameters
        ----------
        step : int
            Current simulation step
        **kwargs
            Additional brain state data

        Returns
        -------
        BrainStateSnapshot
            Snapshot of brain state
        """
        snapshot = BrainStateSnapshot(step=step, **kwargs)
        self._inspection_history.append(snapshot)

        # Cache the snapshot
        self._state_cache[f"step_{step}"] = snapshot

        logger.debug(f"Created brain state snapshot for step {step}")
        return snapshot

    def get_snapshot(self, step: Optional[int] = None) -> Optional[BrainStateSnapshot]:
        """
        Get a brain state snapshot.

        Parameters
        ----------
        step : int, optional
            Step number, if None returns latest

        Returns
        -------
        Optional[BrainStateSnapshot]
            Brain state snapshot or None
        """
        if step is not None:
            return self._state_cache.get(f"step_{step}")
        elif self._inspection_history:
            return self._inspection_history[-1]
        return None

    def compare_snapshots(self, step1: int, step2: int) -> Dict[str, Any]:
        """
        Compare two brain state snapshots.

        Parameters
        ----------
        step1 : int
            First step number
        step2 : int
            Second step number

        Returns
        -------
        Dict[str, Any]
            Comparison results
        """
        snapshot1 = self.get_snapshot(step1)
        snapshot2 = self.get_snapshot(step2)

        if not snapshot1 or not snapshot2:
            raise ValueError(f"Snapshots not found for steps {step1} or {step2}")

        comparison = {
            "step1": step1,
            "step2": step2,
            "timestamp_difference": snapshot2.timestamp - snapshot1.timestamp,
            "differences": {}
        }

        # Compare key components
        for component in ["neurons", "synapses", "connectivity", "memory_states"]:
            if hasattr(snapshot1, component) and hasattr(snapshot2, component):
                val1 = getattr(snapshot1, component)
                val2 = getattr(snapshot2, component)
                if isinstance(val1, dict) and isinstance(val2, dict):
                    comparison["differences"][component] = {
                        "step1_keys": set(val1.keys()),
                        "step2_keys": set(val2.keys()),
                        "new_in_step2": set(val2.keys()) - set(val1.keys()),
                        "missing_in_step2": set(val1.keys()) - set(val2.keys())
                    }

        logger.info(f"Compared snapshots for steps {step1} and {step2}")
        return comparison

    def export_snapshot(self, step: int, output_path: str) -> None:
        """
        Export a brain state snapshot to file.

        Parameters
        ----------
        step : int
            Step number
        output_path : str
            Output file path
        """
        snapshot = self.get_snapshot(step)
        if not snapshot:
            raise ValueError(f"Snapshot not found for step {step}")

        output_path = Path(output_path)
        output_path.parent.mkdir(parents=True, exist_ok=True)

        # Convert to serializable format
        snapshot_dict = {
            "timestamp": snapshot.timestamp,
            "step": snapshot.step,
            "brain_regions": snapshot.brain_regions,
            "neurons": snapshot.neurons,
            "synapses": snapshot.synapses,
            "connectivity": snapshot.connectivity,
            "memory_states": snapshot.memory_states,
            "neuromodulators": snapshot.neuromodulators,
            "simulation_parameters": snapshot.simulation_parameters,
            "performance_metrics": snapshot.performance_metrics
        }

        with open(output_path, 'w') as f:
            json.dump(snapshot_dict, f, indent=2, default=str)

        logger.info(f"Exported brain state snapshot for step {step} to {output_path}")

    def import_snapshot(self, input_path: str) -> BrainStateSnapshot:
        """
        Import a brain state snapshot from file.

        Parameters
        ----------
        input_path : str
            Input file path

        Returns
        -------
        BrainStateSnapshot
            Imported snapshot
        """
        input_path = Path(input_path)
        with open(input_path, 'r') as f:
            snapshot_dict = json.load(f)

        snapshot = BrainStateSnapshot(
            timestamp=snapshot_dict["timestamp"],
            step=snapshot_dict["step"],
            brain_regions=snapshot_dict["brain_regions"],
            neurons=snapshot_dict["neurons"],
            synapses=snapshot_dict["synapses"],
            connectivity=snapshot_dict["connectivity"],
            memory_states=snapshot_dict["memory_states"],
            neuromodulators=snapshot_dict["neuromodulators"],
            simulation_parameters=snapshot_dict["simulation_parameters"],
            performance_metrics=snapshot_dict["performance_metrics"]
        )

        self._inspection_history.append(snapshot)
        self._state_cache[f"step_{snapshot.step}"] = snapshot

        logger.info(f"Imported brain state snapshot for step {snapshot.step} from {input_path}")
        return snapshot

    def analyze_neuron_states(self, neuron_data: Dict[str, Any]) -> Dict[str, Any]:
        """
        Analyze neuron states for debugging.

        Parameters
        ----------
        neuron_data : Dict[str, Any]
            Neuron state data

        Returns
        -------
        Dict[str, Any]
            Analysis results
        """
        analysis = {
            "total_neurons": len(neuron_data),
            "firing_rates": {},
            "membrane_potentials": {},
            "connection_counts": {},
            "activity_patterns": {}
        }

        for neuron_id, data in neuron_data.items():
            if "firing_rate" in data:
                analysis["firing_rates"][neuron_id] = data["firing_rate"]
            if "membrane_potential" in data:
                analysis["membrane_potentials"][neuron_id] = data["membrane_potential"]
            if "connections" in data:
                analysis["connection_counts"][neuron_id] = len(data["connections"])

        # Identify potential issues
        issues = []

        # Check for abnormal firing rates
        abnormal_firing = [
            neuron_id for neuron_id, rate in analysis["firing_rates"].items()
            if rate > 100 or rate < 0.01
        ]
        if abnormal_firing:
            issues.append({
                "type": "abnormal_firing_rate",
                "neurons": abnormal_firing,
                "description": "Neurons with unusually high or low firing rates"
            })

        # Check for membrane potential issues
        for neuron_id, potential in analysis["membrane_potentials"].items():
            if potential > 0.1 or potential < -0.1:
                issues.append({
                    "type": "abnormal_membrane_potential",
                    "neuron": neuron_id,
                    "potential": potential,
                    "description": "Neurons with membrane potential outside normal range"
                })

        analysis["issues"] = issues
        logger.debug(f"Analyzed neuron states for {len(neuron_data)} neurons")
        return analysis

    def analyze_connection_health(self, connectivity_data: Dict[str, Any]) -> Dict[str, Any]:
        """
        Analyze connection health for debugging.

        Parameters
        ----------
        connectivity_data : Dict[str, Any]
            Connectivity data

        Returns
        -------
        Dict[str, Any]
            Connection health analysis
        """
        analysis = {
            "total_connections": sum(
                len(conns) for conns in connectivity_data.values()
            ) if connectivity_data else 0,
            "connection_types": {},
            "connectivity_patterns": {},
            "isolated_neurons": [],
            "highly_connected_neurons": []
        }

        if not connectivity_data:
            return analysis

        # Analyze connection types and patterns
        for source, connections in connectivity_data.items():
            if "type" in connections:
                conn_type = connections["type"]
                analysis["connection_types"][conn_type] = (
                    analysis["connection_types"].get(conn_type, 0) + 1
                )

            # Check for isolated neurons
            if not connections or len(connections) == 0:
                analysis["isolated_neurons"].append(source)

            # Check for highly connected neurons
            if len(connections) > 10:
                analysis["highly_connected_neurons"].append({
                    "neuron": source,
                    "connection_count": len(connections)
                })

        logger.debug(f"Analyzed connectivity for {len(connectivity_data)} neurons")
        return analysis


class SimulationDebugger:
    """Debugger for simulation steps and execution."""

    def __init__(self, max_history: int = 1000):
        """
        Initialize simulation debugger.

        Parameters
        ----------
        max_history : int
            Maximum number of steps to keep in history
        """
        self.max_history = max_history
        self.step_history: deque = deque(maxlen=max_history)
        self.breakpoints: List[int] = []
        self.watch_vars: Dict[str, Any] = {}
        self.debug_info: Dict[str, Any] = {}
        self._in_debug_mode = False

    def record_step(self, step: int, **kwargs) -> None:
        """
        Record a simulation step for debugging.

        Parameters
        ----------
        step : int
        **kwargs
            Step data to record
        """
        step_record = {
            "step": step,
            "timestamp": time.time(),
            "data": kwargs
        }

        self.step_history.append(step_record)

        # Check for breakpoints
        if step in self.breakpoints:
            self._enter_debug_mode(step, kwargs)

        # Watch variables if set
        for var_name, var_info in self.watch_vars.items():
            if var_name in kwargs:
                var_info["history"].append((step, kwargs[var_name]))
                if len(var_info["history"]) > 100:
                    var_info["history"].pop(0)

    def _enter_debug_mode(self, step: int, kwargs: Dict[str, Any]) -> None:
        """Enter debug mode for a specific step."""
        self._in_debug_mode = True
        self.debug_info = {
            "step": step,
            "data": kwargs,
            "timestamp": time.time()
        }

        logger.warning(f"BREAKPOINT reached at step {step}")
        logger.warning(f"Debug info: {json.dumps(kwargs, default=str, indent=2)}")

    def exit_debug_mode(self) -> None:
        """Exit debug mode."""
        self._in_debug_mode = False
        self.debug_info = {}
        logger.info("Exited debug mode")

    def add_breakpoint(self, step: int) -> None:
        """Add a breakpoint at a specific step."""
        if step not in self.breakpoints:
            self.breakpoints.append(step)
            self.breakpoints.sort()
            logger.debug(f"Added breakpoint at step {step}")

    def remove_breakpoint(self, step: int) -> None:
        """Remove a breakpoint."""
        if step in self.breakpoints:
            self.breakpoints.remove(step)
            logger.debug(f"Removed breakpoint at step {step}")

    def add_watch_var(self, name: str, initial_value: Any = None) -> None:
        """
        Add a variable to watch.

        Parameters
        ----------
        name : str
            Variable name
        initial_value : Any, optional
            Initial value
        """
        self.watch_vars[name] = {
            "initial": initial_value,
            "history": []
        }
        logger.debug(f"Added watch variable: {name}")

    def get_step_history(self, steps: Optional[int] = None) -> List[Dict[str, Any]]:
        """
        Get step history.

        Parameters
        ----------
        steps : int, optional
            Number of recent steps to return

        Returns
        -------
        List[Dict[str, Any]]
            Step history
        """
        if steps is None:
            return list(self.step_history)
        else:
            return list(self.step_history)[-steps:]

    def get_debug_info(self) -> Dict[str, Any]:
        """Get current debug information."""
        return self.debug_info.copy()

    def is_in_debug_mode(self) -> bool:
        """Check if in debug mode."""
        return self._in_debug_mode

    def analyze_step_trends(self, steps: Optional[int] = None) -> Dict[str, Any]:
        """
        Analyze trends in step history.

        Parameters
        ----------
        steps : int, optional
            Number of steps to analyze

        Returns
        -------
        Dict[str, Any]
            Trend analysis
        """
        history = self.get_step_history(steps)
        if not history:
            return {}

        analysis = {
            "total_steps": len(history),
            "start_step": history[0]["step"],
            "end_step": history[-1]["step"],
            "time_range": history[-1]["timestamp"] - history[0]["timestamp"],
            "variable_trends": {}
        }

        # Collect all unique variables
        all_vars = set()
        for step_record in history:
            all_vars.update(step_record["data"].keys())

        # Analyze trends for each variable
        for var in all_vars:
            values = []
            for step_record in history:
                if var in step_record["data"]:
                    values.append(step_record["data"][var])

            if values:
                # Simple trend analysis
                if len(values) > 2:
                    # Check for monotonic trends
                    increasing = all(values[i] <= values[i+1] for i in range(len(values)-1))
                    decreasing = all(values[i] >= values[i+1] for i in range(len(values)-1))

                    analysis["variable_trends"][var] = {
                        "values": values,
                        "min": min(values),
                        "max": max(values),
                        "mean": sum(values) / len(values),
                        "increasing": increasing,
                        "decreasing": decreasing,
                        "std": np.std(values) if len(values) > 1 else 0
                    }

        logger.debug(f"Analyzed trends for {len(history)} steps")
        return analysis


class ConnectionValidator:
    """Tools for validating neural connections."""

    def __init__(self):
        """Initialize connection validator."""
        self.validation_rules: Dict[str, Callable] = {}
        self.validation_history: List[Dict[str, Any]] = []

    def add_validation_rule(self, name: str, rule_func: Callable,
                           description: str = "") -> None:
        """
        Add a validation rule.

        Parameters
        ----------
        name : str
        rule_func : Callable
            Function that takes connectivity data and returns (bool, str)
        description : str
            Description of the rule
        """
        self.validation_rules[name] = lambda data: (rule_func(data), description)

    def validate_connectivity(self, connectivity_data: Dict[str, Any]) -> Dict[str, Any]:
        """
        Validate neural connectivity.

        Parameters
        ----------
        connectivity_data : Dict[str, Any]
            Connectivity data to validate

        Returns
        -------
        Dict[str, Any]
            Validation results
        """
        validation_result = {
            "timestamp": time.time(),
            "valid": True,
            "errors": [],
            "warnings": [],
            "rules_applied": [],
            "details": {}
        }

        # Apply all validation rules
        for rule_name, rule_func in self.validation_rules.items():
            validation_result["rules_applied"].append(rule_name)
            try:
                is_valid, result = rule_func(connectivity_data)
                if not is_valid:
                    validation_result["valid"] = False
                    validation_result["errors"].append({
                        "rule": rule_name,
                        "message": result
                    })
                elif result and "warning" in result.lower():
                    validation_result["warnings"].append({
                        "rule": rule_name,
                        "message": result
                    })
                validation_result["details"][rule_name] = {
                    "valid": is_valid,
                    "result": result
                }
            except Exception as e:
                validation_result["valid"] = False
                validation_result["errors"].append({
                    "rule": rule_name,
                    "message": f"Validation error: {str(e)}"
                })

        self.validation_history.append(validation_result)
        logger.info(f"Connectivity validation completed: {'valid' if validation_result['valid'] else 'invalid'}")

        return validation_result

    def get_validation_history(self, limit: int = 100) -> List[Dict[str, Any]]:
        """
        Get validation history.

        Parameters
        ----------
        limit : int
            Maximum number of history entries

        Returns
        -------
        List[Dict[str, Any]]
            Validation history
        """
        return self.validation_history[-limit:]


class MemoryMonitor:
    """Monitor memory usage and performance."""

    def __init__(self, sample_interval: float = 1.0):
        """
        Initialize memory monitor.

        Parameters
        ----------
        sample_interval : float
            Sampling interval in seconds
        """
        self.sample_interval = sample_interval
        self.memory_samples: deque = deque(maxlen=1000)
        self.cpu_samples: deque = deque(maxlen=1000)
        self.thread = None
        self.running = False
        self._start_monitoring()

    def _sample_memory(self):
        """Sample memory and CPU usage."""
        process = psutil.Process()
        memory_info = process.memory_info()
        cpu_percent = process.cpu_percent()

        sample = {
            "timestamp": time.time(),
            "rss_mb": memory_info.rss / 1024 / 1024,
            "vms_mb": memory_info.vms / 1024 / 1024,
            "cpu_percent": cpu_percent,
            "num_threads": process.num_threads(),
            "num_fds": process.num_fds() if hasattr(process, 'num_fds') else 0
        }

        self.memory_samples.append(sample)
        self.cpu_samples.append(sample)

    def _start_monitoring(self):
        """Start monitoring thread."""
        self.running = True
        self.thread = threading.Thread(target=self._monitoring_loop)
        self.thread.daemon = True
        self.thread.start()
        logger.info("Memory monitoring started")

    def _monitoring_loop(self):
        """Monitoring loop."""
        while self.running:
            self._sample_memory()
            time.sleep(self.sample_interval)

    def stop_monitoring(self):
        """Stop monitoring."""
        self.running = False
        if self.thread:
            self.thread.join(timeout=1.0)
        logger.info("Memory monitoring stopped")

    def get_memory_stats(self, duration: Optional[float] = None) -> Dict[str, Any]:
        """
        Get memory statistics.

        "Parameters"
        ----------
        duration : float, optional
            Time duration in seconds to analyze, if None uses all samples

        Returns
        -------
        Dict[str, Any]
            Memory statistics
        """
        if not self.memory_samples:
            return {}

        samples = list(self.memory_samples)
        if duration:
            cutoff = time.time() - duration
            samples = [s for s in samples if s["timestamp"] >= cutoff]

        if not samples:
            return {}

        rss_values = [s["rss_mb"] for s in samples]
        vms_values = [s["vms_mb"] for s in samples]
        cpu_values = [s["cpu_percent"] for s in samples]

        stats = {
            "total_samples": len(samples),
            "time_range": samples[-1]["timestamp"] - samples[0]["timestamp"],
            "rss": {
                "min_mb": min(rss_values),
                "max_mb": max(rss_values),
                "mean_mb": np.mean(rss_values),
                "std_mb": np.std(rss_values),
                "current_mb": rss_values[-1]
            },
            "vms": {
                "min_mb": min(vms_values),
                "max_mb": max(vms_values),
                "mean_mb": np.mean(vms_values),
                "std_mb": np.std(vms_values),
                "current_mb": vms_values[-1]
            },
            "cpu": {
                "min_percent": min(cpu_values),
                "max_percent": max(cpu_values),
                "mean_percent": np.mean(cpu_values),
                "std_percent": np.std(cpu_values),
                "current_percent": cpu_values[-1]
            }
        }

        return stats

    def detect_memory_leak(self, window_size: int = 100) -> Dict[str, Any]:
        """
        Detect potential memory leaks.

        Parameters
        ----------
        window_size : int
            Window size for leak detection

        Returns
        -------
        Dict[str, Any]
            Leak detection results
        """
        if len(self.memory_samples) < window_size * 2:
            return {"status": "insufficient_data", "message": "Not enough data for leak detection"}

        # Split samples into two halves
        half = len(self.memory_samples) // 2
        first_half = list(self.memory_samples)[:half]
        second_half = list(self.memory_samples)[half:]

        first_rss = np.array([s["rss_mb"] for s in first_half])
        second_rss = np.array([s["rss_mb"] for s in second_half])

        # Calculate linear regression slopes
        first_time = np.array([s["timestamp"] for s in first_half])
        second_time = np.array([s["timestamp"] for s in second_half])

        if len(first_time) > 1 and len(second_time) > 1:
            first_slope = np.polyfit(first_time, first_rss, 1)[0]
            second_slope = np.polyfit(second_time, second_rss, 1)[0]

            # Check for increasing trend in second half
            increasing_trend = second_slope > first_slope and second_slope > 0.01

            result = {
                "status": "leak_detected" if increasing_trend else "no_leak_detected",
                "first_half_slope_mb_per_sec": first_slope,
                "second_half_slope_mb_per_sec": second_slope,
                "first_half_avg_rss": np.mean(first_rss),
                "second_half_avg_rss": np.mean(second_rss),
                "growth_rate": (np.mean(second_rss) - np.mean(first_rss)) / np.mean(first_rss) * 100
            }

            logger.warning(f"Memory leak detection: {result['status']}")
            return result
        else:
            return {"status": "insufficient_data", "message": "Not enough data points for regression"}


class PerformanceProfiler:
    """Profile performance of NLM components."""

    def __init__(self):
        """Initialize performance profiler."""
        self.profiling_data: Dict[str, Any] = {}
        self._active_profilers: Dict[str, Any] = {}
        self._profiler_threads: Dict[str, threading.Thread] = {}

    def start_profiling(self, name: str, interval: float = 0.1):
        """
        Start profiling a component.

        Parameters
        ----------
        name : str
        interval : float
            Profiling interval in seconds
        """
        if name in self._active_profilers:
            return

        profiler = {
            "start_time": time.time(),
            "samples": [],
            "interval": interval,
            "thread": threading.Thread(
                target=self._profile_worker,
                args=(name, interval)
            )
        }

        profiler["thread"].daemon = True
        profiler["thread"].start()

        self._active_profilers[name] = profiler
        self._profiler_threads[name] = profiler["thread"]

        logger.debug(f"Started profiling for {name}")

    def _profile_worker(self, name: str, interval: float):
        """Profiling worker thread."""
        while name in self._active_profilers:
            sample = self._sample_profiling_data(name)
            self._active_profilers[name]["samples"].append(sample)
            time.sleep(interval)

    def _sample_profiling_data(self, name: str) -> Dict[str, Any]:
        """Sample profiling data."""
        process = psutil.Process()

        return {
            "timestamp": time.time(),
            "cpu_percent": process.cpu_percent(),
            "memory_mb": process.memory_info().rss / 1024 / 1024,
            "thread_count": process.num_threads(),
            "open_files": len(process.open_files()) if hasattr(process, 'open_files') else 0
        }

    def stop_profiling(self, name: str) -> Dict[str, Any]:
        """
        Stop profiling a component.

        Parameters
        ----------
        name : str

        Returns
        -------
        Dict[str, Any]
            Profiling results
        """
        if name not in self._active_profilers:
            return {}

        profiler = self._active_profilers[name]
        profiler["end_time"] = time.time()
        profiler["thread"].join(timeout=1.0)

        del self._active_profilers[name]
        if name in self._profiler_threads:
            del self._profiler_threads[name]

        samples = profiler["samples"]
        if not samples:
            return {}

        # Calculate statistics
        timestamps = [s["timestamp"] for s in samples]
        cpu_values = [s["cpu_percent"] for s in samples]
        memory_values = [s["memory_mb"] for s in samples]
        thread_values = [s["thread_count"] for s in samples]

        duration = profiler["end_time"] - profiler["start_time"]

        results = {
            "name": name,
            "duration_seconds": duration,
            "sample_count": len(samples),
            "cpu": {
                "min_percent": min(cpu_values),
                "max_percent": max(cpu_values),
                "mean_percent": np.mean(cpu_values),
                "std_percent": np.std(cpu_values)
            },
            "memory": {
                "min_mb": min(memory_values),
                "max_mb": max(memory_values),
                "mean_mb": np.mean(memory_values),
                "std_mb": np.std(memory_values),
                "growth_rate_mb_per_sec": (memory_values[-1] - memory_values[0]) / duration if duration > 0 else 0
            },
            "threads": {
                "min": min(thread_values),
                "max": max(thread_values),
                "mean": np.mean(thread_values),
                "std": np.std(thread_values)
            }
        }

        self.profiling_data[name] = results
        logger.info(f"Stopped profiling for {name}: {duration:.2f}s duration")
        return results

    def analyze_bottlenecks(self) -> Dict[str, Any]:
        """
        Analyze performance bottlenecks.

        Returns
        -------
        Dict[str, Any]
            Bottleneck analysis
        """
        if not self.profiling_data:
            return {"status": "no_data", "message": "No profiling data available"}

        analysis = {
            "bottlenecks": [],
            "top_cpu_components": [],
            "top_memory_components": [],
            "recommendations": []
        }

        # Sort by CPU usage
        cpu_components = sorted(
            self.profiling_data.items(),
            key=lambda x: x[1]["cpu"]["mean_percent"],
            reverse=True
        )

        # Sort by memory usage
        memory_components = sorted(
            self.profiling_data.items(),
            key=lambda x: x[1]["memory"]["mean_mb"],
            reverse=True
        )

        # Identify bottlenecks
        for name, data in cpu_components[:3]:
            if data["cpu"]["mean_percent"] > 50:
                analysis["bottlenecks"].append({
                    "component": name,
                    "issue": "high_cpu_usage",
                    "mean_cpu_percent": data["cpu"]["mean_percent"],
                    "severity": "high" if data["cpu"]["mean_percent"] > 80 else "medium"
                })

        for name, data in memory_components[:3]:
            if data["memory"]["growth_rate_mb_per_sec"] > 1:
                analysis["bottlenecks"].append({
                    "component": name,
                    "issue": "memory_growth",
                    "growth_rate_mb_per_sec": data["memory"]["growth_rate_mb_per_sec"],
                    "severity": "high" if data["memory"]["growth_rate_mb_per_sec"] > 5 else "medium"
                })

        # Generate recommendations
        if analysis["bottlenecks"]:
            analysis["recommendations"].append(
                "Consider optimizing components with high CPU or memory usage"
            )
            analysis["recommendations"].append(
                "Add more efficient data structures or algorithms"
            )

        logger.info(f"Identified {len(analysis['bottlenecks'])} performance bottlenecks")
        return analysis


# Convenience instances
brain_state_inspector = BrainStateInspector()
simulation_debugger = SimulationDebugger()
connection_validator = ConnectionValidator()
memory_monitor = MemoryMonitor()
performance_profiler = PerformanceProfiler()


def get_brain_state_inspector() -> BrainStateInspector:
    """Get brain state inspector instance."""
    return brain_state_inspector


def get_simulation_debugger() -> SimulationDebugger:
    """Get simulation debugger instance."""
    return simulation_debugger


def get_connection_validator() -> ConnectionValidator:
    """Get connection validator instance."""
    return connection_validator


def get_memory_monitor() -> MemoryMonitor:
    """Get memory monitor instance."""
    return memory_monitor


def get_performance_profiler() -> PerformanceProfiler:
    """Get performance profiler instance."""
    return performance_profiler