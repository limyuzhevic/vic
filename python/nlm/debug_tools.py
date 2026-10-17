#!/usr/bin/env python3
"""
Development and Debugging Tools for NLM Neural Simulation

This module provides comprehensive development and debugging tools for neural
simulation, including debugging with breakpoints and step-through, neural network
visualization, performance benchmarking, memory leak detection, code profiling,
and simulation comparison tools.
"""

import time
import threading
import traceback
import inspect
from typing import List, Dict, Tuple, Optional, Any, Callable, Union
from dataclasses import dataclass, field
from enum import Enum
import warnings
import sys
import os
from abc import ABC, abstractmethod

warnings.filterwarnings('ignore')

class DebugLevel(Enum):
    """Debug log levels"""
    DEBUG = "debug"
    INFO = "info"
    WARNING = "warning"
    ERROR = "error"
    CRITICAL = "critical"

class BreakpointType(Enum):
    """Breakpoint types"""
    CONDITIONAL = "conditional"
    STEP = "step"
    FUNCTION_ENTRY = "function_entry"
    FUNCTION_EXIT = "function_exit"
    EXCEPTION = "exception"

@dataclass
class DebugMessage:
    """Container for debug messages"""
    level: DebugLevel
    message: str
    timestamp: float
    source: str
    function: Optional[str] = None
    line: Optional[int] = None
    metadata: Dict[str, Any] = field(default_factory=dict)

@dataclass
class Breakpoint:
    """Container for debugger breakpoints"""
    id: str
    type: BreakpointType
    condition: Optional[Callable[[Any], bool]] = None
    message: Optional[str] = None
    enabled: bool = True
    hit_count: int = 0

@dataclass
class DebuggerState:
    """Container for debugger state"""
    is_running: bool = False
    current_breakpoint: Optional[str] = None
    step_mode: bool = False
    watch_variables: Dict[str, Any] = field(default_factory=dict)
    call_stack: List[Dict[str, Any]] = field(default_factory=list)

class DebugLogger:
    """Comprehensive debug logger for neural simulation"""
    
    def __init__(self, name: str = "nlm_debug", level: DebugLevel = DebugLevel.INFO):
        self.name = name
        self.level = level
        self.messages: List[DebugMessage] = []
        self.handlers: List[Callable[[DebugMessage], None]] = []
        self.log_file: Optional[str] = None
        self.formatter = self._create_default_formatter()
    
    def _create_default_formatter(self) -> Callable[[DebugMessage], str]:
        """Create default message formatter"""
        def formatter(message: DebugMessage) -> str:
            timestamp_str = datetime.fromtimestamp(message.timestamp).strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
            level_str = message.level.value.upper()
            source_str = message.source
            func_str = f" ({message.function})" if message.function else ""
            return f"[{timestamp_str}] {level_str} {source_str}{func_str}: {message.message}"
        
        return formatter
    
    def set_level(self, level: DebugLevel):
        """Set debug log level"""
        self.level = level
    
    def set_log_file(self, filepath: str):
        """Set log file"""
        self.log_file = filepath
    
    def add_handler(self, handler: Callable[[DebugMessage], None]):
        """Add message handler"""
        self.handlers.append(handler)
    
    def debug(self, message: str, source: str = "unknown", 
              function: Optional[str] = None, line: Optional[int] = None,
              metadata: Optional[Dict[str, Any]] = None):
        """Log debug message"""
        if self.level.value != "debug" and DebugLevel.DEBUG.value == DebugLevel.INFO.value:
            return
        
        debug_msg = DebugMessage(
            level=DebugLevel.DEBUG,
            message=message,
            timestamp=time.time(),
            source=source,
            function=function,
            line=line,
            metadata=metadata or {}
        )
        
        self._log_message(debug_msg)
    
    def info(self, message: str, source: str = "unknown",
             function: Optional[str] = None, line: Optional[int] = None,
             metadata: Optional[Dict[str, Any]] = None):
        """Log info message"""
        if self.level.value not in ["info", "debug"]:
            return
        
        debug_msg = DebugMessage(
            level=DebugLevel.INFO,
            message=message,
            timestamp=time.time(),
            source=source,
            function=function,
            line=line,
            metadata=metadata or {}
        )
        
        self._log_message(debug_msg)
    
    def warning(self, message: str, source: str = "unknown",
                function: Optional[str] = None, line: Optional[int] = None,
                metadata: Optional[Dict[str, Any]] = None):
        """Log warning message"""
        if self.level.value not in ["warning", "info", "debug"]:
            return
        
        debug_msg = DebugMessage(
            level=DebugLevel.WARNING,
            message=message,
            timestamp=time.time(),
            source=source,
            function=function,
            line=line,
            metadata=metadata or {}
        )
        
        self._log_message(debug_msg)
    
    def error(self, message: str, source: str = "unknown",
              function: Optional[str] = None, line: Optional[int] = None,
              metadata: Optional[Dict[str, Any]] = None):
        """Log error message"""
        if self.level.value not in ["error", "warning", "info", "debug"]:
            return
        
        debug_msg = DebugMessage(
            level=DebugLevel.ERROR,
            message=message,
            timestamp=time.time(),
            source=source,
            function=function,
            line=line,
            metadata=metadata or {}
        )
        
        self._log_message(debug_msg)
    
    def critical(self, message: str, source: str = "unknown",
                 function: Optional[str] = None, line: Optional[int] = None,
                 metadata: Optional[Dict[str, Any]] = None):
        """Log critical message"""
        if DebugLevel.CRITICAL.value not in ["critical", "error", "warning", "info", "debug"]:
            return
        
        debug_msg = DebugMessage(
            level=DebugLevel.CRITICAL,
            message=message,
            timestamp=time.time(),
            source=source,
            function=function,
            line=line,
            metadata=metadata or {}
        )
        
        self._log_message(debug_msg)
    
    def _log_message(self, message: DebugMessage):
        """Log debug message"""
        self.messages.append(message)
        
        # Call handlers
        for handler in self.handlers:
            try:
                handler(message)
            except Exception:
                pass
        
        # Write to file if specified
        if self.log_file:
            try:
                with open(self.log_file, 'a') as f:
                    formatted = self.formatter(message)
                    f.write(formatted + "\n")
            except Exception:
                pass
    
    def get_messages(self, level: Optional[DebugLevel] = None,
                     source: Optional[str] = None) -> List[DebugMessage]:
        """Get debug messages"""
        messages = self.messages
        
        if level:
            messages = [m for m in messages if m.level == level]
        
        if source:
            messages = [m for m in messages if m.source == source]
        
        return messages
    
    def clear(self):
        """Clear all messages"""
        self.messages.clear()
    
    def export(self, filepath: str):
        """Export messages to file"""
        with open(filepath, 'w') as f:
            for message in self.messages:
                f.write(f"{self.formatter(message)}\n")
class Debugger:
    """Advanced debugger for neural simulation"""
    
    def __init__(self, logger: Optional[DebugLogger] = None):
        self.logger = logger or DebugLogger("nlm_debugger")
        self.breakpoints: Dict[str, Breakpoint] = {}
        self.state = DebuggerState()
        self.watch_variables: Dict[str, Any] = {}
        self.execution_stack: List[Dict[str, Any]] = []
        self.exception_handlers: List[Callable[[Exception], bool]] = []
    
    def set_breakpoint(self, id: str, type: BreakpointType, 
                      condition: Optional[Callable[[Any], bool]] = None,
                      message: Optional[str] = None) -> Breakpoint:
        """Set a breakpoint"""
        breakpoint = Breakpoint(id, type, condition, message)
        self.breakpoints[id] = breakpoint
        return breakpoint
    
    def remove_breakpoint(self, id: str):
        """Remove a breakpoint"""
        if id in self.breakpoints:
            del self.breakpoints[id]
    
    def enable_breakpoint(self, id: str):
        """Enable a breakpoint"""
        if id in self.breakpoints:
            self.breakpoints[id].enabled = True
    
    def disable_breakpoint(self, id: str):
        """Disable a breakpoint"""
        if id in self.breakpoints:
            self.breakpoints[id].enabled = False
    
    def evaluate_condition(self, breakpoint: Breakpoint, context: Any) -> bool:
        """Evaluate breakpoint condition"""
        if not breakpoint.enabled or breakpoint.condition is None:
            return True
        
        try:
            return breakpoint.condition(context)
        except Exception:
            return False
    
    def check_breakpoints(self, context: Any) -> List[str]:
        """Check if any breakpoints should be triggered"""
        triggered = []
        
        for id, breakpoint in self.breakpoints.items():
            if breakpoint.enabled and self.evaluate_condition(breakpoint, context):
                breakpoint.hit_count += 1
                triggered.append(id)
        
        return triggered
    
    def step_into(self):
        """Step into the next function call"""
        self.state.step_mode = True
        self.logger.info("Step mode enabled", source="debugger")
    
    def step_over(self):
        """Step over the current function call"""
        self.logger.info("Step over mode enabled", source="debugger")
    
    def step_out(self):
        """Step out of the current function call"""
        self.logger.info("Step out mode enabled", source="debugger")
    
    def add_watch_variable(self, name: str, value: Any):
        """Add a watch variable"""
        self.watch_variables[name] = value
    
    def remove_watch_variable(self, name: str):
        """Remove a watch variable"""
        if name in self.watch_variables:
            del self.watch_variables[name]
    
    def get_watch_variables(self) -> Dict[str, Any]:
        """Get all watch variables"""
        return self.watch_variables.copy()
    
    def add_exception_handler(self, handler: Callable[[Exception], bool]):
        """Add exception handler"""
        self.exception_handlers.append(handler)
    
    def remove_exception_handler(self, handler: Callable[[Exception], bool]):
        """Remove exception handler"""
        if handler in self.exception_handlers:
            self.exception_handlers.remove(handler)
    
    def handle_exception(self, exception: Exception, context: Any = None) -> bool:
        """Handle an exception"""
        self.logger.error(f"Exception: {str(exception)}", 
                         source="debugger",
                         metadata={"exception": exception, "context": context})
        
        for handler in self.exception_handlers:
            try:
                if handler(exception):
                    return True
            except Exception:
                pass
        
        return False
    
    def trace_call_stack(self):
        """Trace the current call stack"""
        self.state.call_stack = []
        
        for thread_id, thread in threading.enumerate():
            if thread is threading.current_thread():
                continue
            
            if thread.is_alive():
                for frame in thread.enumerate():
                    self.state.call_stack.append({
                        "thread_id": thread_id,
                        "filename": frame.f_code.co_filename,
                        "function": frame.f_code.co_name,
                        "line": frame.f_lineno
                    })
        
        return self.state.call_stack
    
    def get_current_stack(self) -> List[Dict[str, Any]]:
        """Get current thread stack"""
        return self.trace_call_stack()
    
    def export_debug_info(self) -> Dict[str, Any]:
        """Export debug information"""
        return {
            "breakpoints": {id: asdict(bp) for id, bp in self.breakpoints.items()},
            "state": asdict(self.state),
            "watch_variables": self.watch_variables,
            "execution_stack": self.execution_stack,
            "log_messages": [asdict(msg) for msg in self.logger.messages]
        }
    
    def import_debug_info(self, debug_info: Dict[str, Any]):
        """Import debug information"""
        self.breakpoints = {bp["id"]: Breakpoint(**bp) for bp in debug_info.get("breakpoints", {}).values()}
        self.state = DebuggerState(**debug_info.get("state", {}))
        self.watch_variables = debug_info.get("watch_variables", {})
        self.execution_stack = debug_info.get("execution_stack", [])
class NeuralNetworkVisualizer:
    """Visualization tools for neural networks"""
    
    def __init__(self):
        self._setup_matplotlib_style()
    
    def _setup_matplotlib_style(self):
        """Setup matplotlib style"""
        import matplotlib.pyplot as plt
        plt.style.use('seaborn-v0_8')
    
    def plot_network_topology(self, layers: List[int], 
                             connections: Optional[List[Tuple[int, int]]] = None,
                             save_path: Optional[str] = None) -> plt.Figure:
        """Plot neural network topology"""
        import matplotlib.pyplot as plt
        import numpy as np
        
        fig, ax = plt.subplots(figsize=(12, 8))
        
        # Create network visualization
        n_layers = len(layers)
        max_neurons = max(layers)
        
        # Calculate positions
        x_positions = np.linspace(1, n_layers, n_layers)
        
        # Draw layers
        for i, (layer_idx, n_neurons) in enumerate(zip(x_positions, layers)):
            layer_center = i + 1
            
            # Draw neurons
            y_positions = np.linspace(1, max_neurons, n_neurons)
            for j, y_pos in enumerate(y_positions):
                neuron = plt.Circle((layer_center, y_pos), radius=0.3, 
                                   facecolor='lightblue', edgecolor='blue', linewidth=2)
                ax.add_patch(neuron)
                
                # Add neuron label
                ax.text(layer_center, y_pos, str(j+1), 
                       horizontalalignment='center', verticalalignment='center',
                       fontsize=8)
            
            # Draw layer label
            ax.text(layer_center, max_neurons + 0.5, f"Layer {i+1}\n{n_neurons} neurons",
                   horizontalalignment='center', verticalalignment='center',
                   fontsize=10)
        
        # Draw connections if provided
        if connections:
            for src, dst in connections:
                src_layer = src // max_neurons
                src_neuron = src % max_neurons
                dst_layer = dst // max_neurons
                dst_neuron = dst % max_neurons
                
                x1 = src_layer + 1
                y1 = src_neuron + 0.7
                x2 = dst_layer + 1
                y2 = dst_neuron + 0.7
                
                ax.plot([x1, x2], [y1, y2], 'k-', linewidth=0.5, alpha=0.5)
        
        ax.set_xlim(0.5, n_layers + 1.5)
        ax.set_ylim(0.5, max_neurons + 1.5)
        ax.set_aspect('equal')
        ax.axis('off')
        ax.set_title("Neural Network Topology")
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
        
        return fig
    
    def plot_activation_maps(self, activations: List[np.ndarray],
                            layer_names: Optional[List[str]] = None,
                            save_path: Optional[str] = None) -> plt.Figure:
        """Plot activation maps for neural network layers"""
        import matplotlib.pyplot as plt
        
        n_layers = len(activations)
        if layer_names is None:
            layer_names = [f"Layer {i+1}" for i in range(n_layers)]
        
        # Determine subplot layout
        cols = min(4, n_layers)
        rows = int(np.ceil(n_layers / cols))
        
        fig, axes = plt.subplots(rows, cols, figsize=(4*cols, 4*rows))
        
        if n_layers == 1:
            axes = [axes]
        elif rows == 1 or cols == 1:
            axes = axes.flatten()
        else:
            axes = axes.flatten()
        
        for i, (activation, name) in enumerate(zip(activations, layer_names)):
            if i >= len(axes):
                break
            
            ax = axes[i]
            
            # Handle different activation shapes
            if activation.ndim == 1:
                # 1D activation (e.g., neuron outputs)
                ax.bar(range(len(activation)), activation, alpha=0.7)
                ax.set_xlabel("Neuron")
                ax.set_ylabel("Activation")
                ax.set_title(f"{name} - Neuron Activations")
                
            elif activation.ndim == 2:
                # 2D activation (e.g., feature maps)
                im = ax.imshow(activation, cmap='viridis', aspect='auto')
                ax.set_xlabel("X")
                ax.set_ylabel("Y")
                ax.set_title(f"{name} - Activation Map")
                plt.colorbar(im, ax=ax)
                
            elif activation.ndim == 3:
                # 3D activation (e.g., volume data)
                ax.voxels(activation > 0, alpha=0.5)
                ax.set_xlabel("X")
                ax.set_ylabel("Y")
                ax.set_zlabel("Z")
                ax.set_title(f"{name} - 3D Activations")
            
            ax.grid(True, alpha=0.3)
        
        # Hide unused subplots
        for i in range(n_layers, len(axes)):
            axes[i].set_visible(False)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
        
        return fig
    
    def plot_learning_curves(self, history: Dict[str, List[float]],
                            metrics: Optional[List[str]] = None,
                            save_path: Optional[str] = None) -> plt.Figure:
        """Plot learning curves (training/validation metrics over time)"""
        import matplotlib.pyplot as plt
        
        if not history:
            return plt.figure()
        
        if metrics is None:
            metrics = list(history.keys())
        
        n_metrics = len(metrics)
        cols = int(np.ceil(np.sqrt(n_metrics)))
        rows = int(np.ceil(n_metrics / cols))
        
        fig, axes = plt.subplots(rows, cols, figsize=(5*cols, 4*rows))
        
        if n_metrics == 1:
            axes = [axes]
        elif rows == 1 or cols == 1:
            axes = axes.flatten()
        else:
            axes = axes.flatten()
        
        for i, metric in enumerate(metrics):
            if i >= len(axes):
                break
            
            ax = axes[i]
            if metric in history:
                data = history[metric]
                epochs = list(range(len(data)))
                ax.plot(epochs, data, 'b-', linewidth=2, label=metric)
                ax.set_xlabel('Epoch')
                ax.set_ylabel(metric.replace('_', ' ').title())
                ax.set_title(f'{metric.replace("_", " ").title()} Over Time')
                ax.legend()
                ax.grid(True, alpha=0.3)
        
        # Hide unused subplots
        for i in range(n_metrics, len(axes)):
            axes[i].set_visible(False)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
        
        return fig
    
    def create_interactive_network_viewer(self, network_data: Dict[str, Any],
                                         save_path: Optional[str] = None) -> Any:
        """Create interactive network viewer"""
        try:
            import networkx as nx
            import matplotlib.pyplot as plt
            
            # Create network graph
            G = nx.Graph()
            
            # Add nodes
            for node_id, node_data in network_data.get('nodes', {}).items():
                G.add_node(node_id, **node_data)
            
            # Add edges
            for edge in network_data.get('edges', []):
                G.add_edge(edge[0], edge[1], weight=edge[2] if len(edge) > 2 else 1.0)
            
            # Create layout
            if len(G.nodes()) < 50:
                pos = nx.spring_layout(G, seed=42)
            else:
                pos = nx.kamada_kawai_layout(G)
            
            # Create figure
            fig, ax = plt.subplots(figsize=(12, 8))
            
            # Draw network
            nx.draw_networkx_nodes(G, pos, node_size=500, node_color='lightblue', 
                                  alpha=0.8, ax=ax)
            nx.draw_networkx_edges(G, pos, alpha=0.3, edge_color='gray', ax=ax)
            nx.draw_networkx_labels(G, pos, font_size=8, ax=ax)
            
            ax.set_title("Interactive Neural Network")
            ax.axis('off')
            
            plt.tight_layout()
            
            if save_path:
                plt.savefig(save_path, dpi=150, bbox_inches='tight')
            
            return fig
        
        except ImportError:
            print("networkx or matplotlib not available for interactive viewer")
            return None
class PerformanceBenchmark:
    """Benchmarking tools for neural simulation performance"""
    
    def __init__(self):
        self.benchmark_results: List[Dict[str, Any]] = []
        self.profiler = Profiler()
    
    def run_benchmark(self, name: str, func: Callable, 
                     iterations: int = 100,
                     warmup: int = 10) -> Dict[str, Any]:
        """Run a performance benchmark"""
        # Warmup
        for _ in range(warmup):
            try:
                func()
            except Exception:
                pass
        
        # Actual benchmark
        times = []
        for _ in range(iterations):
            start_time = time.time()
            try:
                func()
                elapsed = time.time() - start_time
                times.append(elapsed)
            except Exception as e:
                continue
        
        if not times:
            return {
                "name": name,
                "error": "Benchmark failed",
                "iterations": iterations,
                "warmup": warmup
            }
        
        # Calculate statistics
        times_array = np.array(times)
        result = {
            "name": name,
            "iterations": len(times),
            "warmup": warmup,
            "total_time": np.sum(times_array),
            "average_time": np.mean(times_array),
            "min_time": np.min(times_array),
            "max_time": np.max(times_array),
            "median_time": np.median(times_array),
            "std_deviation": np.std(times_array),
            "variance": np.var(times_array),
            "throughput": len(times) / np.sum(times_array) if np.sum(times_array) > 0 else 0,
            "per_iteration_ms": np.mean(times_array) * 1000
        }
        
        self.benchmark_results.append(result)
        return result
    
    def compare_benchmarks(self, benchmark_names: List[str]) -> Dict[str, Any]:
        """Compare multiple benchmarks"""
        selected_results = [b for b in self.benchmark_results if b["name"] in benchmark_names]
        
        if len(selected_results) < 2:
            return {"error": "Need at least 2 benchmarks for comparison"}
        
        comparison = {}
        
        # Compare each metric across benchmarks
        metrics = ["average_time", "min_time", "max_time", "std_deviation", "throughput"]
        
        for metric in metrics:
            values = [b.get(metric) for b in selected_results if metric in b and b.get(metric) is not None]
            if values:
                comparison[f"{metric}_mean"] = np.mean(values)
                comparison[f"{metric}_median"] = np.median(values)
                comparison[f"{metric}_std"] = np.std(values)
                comparison[f"{metric}_min"] = np.min(values)
                comparison[f"{metric}_max"] = np.max(values)
        
        return comparison
    
    def get_best_performer(self, benchmark_names: List[str]) -> Optional[str]:
        """Get the best performing benchmark based on throughput"""
        selected_results = [b for b in self.benchmark_results if b["name"] in benchmark_names]
        
        if not selected_results:
            return None
        
        # Find best throughput
        best = max(selected_results, key=lambda b: b.get("throughput", 0))
        return best["name"]
    
    def export_results(self, filepath: str, format: str = "csv"):
        """Export benchmark results to file"""
        if format == "csv":
            df = pd.DataFrame(self.benchmark_results)
            df.to_csv(filepath, index=False)
        elif format == "json":
            with open(filepath, 'w') as f:
                json.dump(self.benchmark_results, f, indent=2)
        elif format == "excel":
            df = pd.DataFrame(self.benchmark_results)
            df.to_excel(filepath, index=False)
        
        return True
class SimulationComparator:
    """Compare different neural simulation runs"""
    
    def __init__(self):
        self.comparisons: List[Dict[str, Any]] = []
    
    def compare_simulations(self, sim1_data: Dict[str, Any], 
                           sim2_data: Dict[str, Any],
                           metrics: Optional[List[str]] = None) -> Dict[str, Any]:
        """Compare two simulation runs"""
        if metrics is None:
            metrics = ["total_time", "steps_completed", "average_firing_rate", "spike_count"]
        
        comparison = {
            "sim1_name": sim1_data.get("name", "Simulation 1"),
            "sim2_name": sim2_data.get("name", "Simulation 2"),
            "similarity_score": 0.0,
            "metrics_comparison": {}
        }
        
        for metric in metrics:
            val1 = sim1_data.get(metric)
            val2 = sim2_data.get(metric)
            
            if val1 is not None and val2 is not None:
                comparison["metrics_comparison"][metric] = {
                    "sim1": val1,
                    "sim2": val2,
                    "difference": abs(val1 - val2),
                    "relative_difference": abs(val1 - val2) / max(abs(val1), abs(val2)) if max(abs(val1), abs(val2)) > 0 else 0
                }
        
        # Calculate similarity score
        if comparison["metrics_comparison"]:
            differences = []
            for metric_data in comparison["metrics_comparison"].values():
                differences.append(metric_data["relative_difference"])
            
            if differences:
                comparison["similarity_score"] = 1.0 - np.mean(differences)
        
        self.comparisons.append(comparison)
        return comparison
    
    def generate_comparison_report(self, output_format: str = "text") -> str:
        """Generate comparison report"""
        if not self.comparisons:
            return "No comparisons available"
        
        if output_format == "text":
            report = "Simulation Comparison Report\n"
            report += "=" * 50 + "\n\n"
            
            for i, comp in enumerate(self.comparisons, 1):
                report += f"Comparison {i}: {comp['sim1_name']} vs {comp['sim2_name']}\n"
                report += f"Similarity Score: {comp['similarity_score']:.3f}\n\n"
                
                for metric, data in comp['metrics_comparison'].items():
                    report += f"  {metric}: {data['sim1']:.4f} vs {data['sim2']:.4f} "
                    report += f"(diff: {data['difference']:.4f}, rel: {data['relative_difference']:.3f})\n"
                
                report += "\n" + "-" * 50 + "\n\n"
            
            return report
        
        elif output_format == "json":
            return json.dumps(self.comparisons, indent=2)
        
        else:
            return f"Comparison report format '{output_format}' not supported"

# Convenience functions

def create_debug_logger(name: str = "nlm_debug", level: DebugLevel = DebugLevel.INFO) -> DebugLogger:
    """Convenience function to create a debug logger"""
    return DebugLogger(name, level)

def create_debugger(logger: Optional[DebugLogger] = None) -> Debugger:
    """Convenience function to create a debugger"""
    return Debugger(logger)

def create_network_visualizer() -> NeuralNetworkVisualizer:
    """Convenience function to create a network visualizer"""
    return NeuralNetworkVisualizer()

def create_benchmarker() -> PerformanceBenchmark:
    """Convenience function to create a benchmarker"""
    return PerformanceBenchmark()

def create_simulator_comparator() -> SimulationComparator:
    """Convenience function to create a simulation comparator"""
    return SimulationComparator()
