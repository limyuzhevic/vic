#!/usr/bin/env python3

"""
Advanced Python Debugging API for NLM (Neural Language Model)

This module provides comprehensive debugging capabilities for NLM including:
- Real-time visualization of system state
- Trace analysis and visualization
- Memory debugging and visualization
- Performance metrics visualization
- Error recovery mechanisms
- Incident management and tracking
"""

import asyncio
import time
import threading
import json
import sys
import os
from typing import Dict, List, Optional, Callable, Any, Union
from dataclasses import dataclass, field
from enum import Enum
import numpy as np
from datetime import datetime, timedelta
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend
import plotly.graph_objects as go
import plotly.io as pio
import traceback
from pathlib import Path
import psutil
import subprocess
import signal
import queue
import logging
from concurrent.futures import ThreadPoolExecutor
import weakref
import pickle
import base64
import zlib
from collections import defaultdict, deque

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)
class DebugLevel(Enum):
    """Debug logging levels."""
    TRACE = "TRACE"
    DEBUG = "DEBUG"
    INFO = "INFO"
    WARNING = "WARNING"
    ERROR = "ERROR"
    CRITICAL = "CRITICAL"
class LogCategory(Enum):
    """Log categories for structured logging."""
    SYSTEM = "System"
    SIMULATION = "Simulation"
    NEURAL = "Neural"
    MEMORY = "Memory"
    PERFORMANCE = "Performance"
    NETWORK = "Network"
    GUI = "GUI"
    ERROR = "Error"
    AUDIT = "Audit"
@dataclass
class CorrelationContext:
    """Correlation context for tracking related operations."""
    correlation_id: str
    operation: str
    start_time: datetime
    metadata: Dict[str, Any] = field(default_factory=dict)
    parent_id: Optional[str] = None
    child_ids: List[str] = field(default_factory=list)
    
    def get_duration(self) -> timedelta:
        """Get the duration of the correlation context."""
        return datetime.now() - self.start_time
@dataclass
class StructuredLogEntry:
    """Structured log entry with correlation ID and metadata."""
    correlation_id: str
    timestamp: datetime
    level: DebugLevel
    category: LogCategory
    message: str
    metadata: Dict[str, Any] = field(default_factory=dict)
    metrics: Dict[str, float] = field(default_factory=dict)
    file: str = ""
    line: int = 0
    function: str = ""
@dataclass
class PerformanceMetric:
    """Performance metric data structure."""
    name: str
    value: float
    unit: str
    timestamp: datetime
    tags: Dict[str, str] = field(default_factory=dict)
@dataclass
class MemoryBlock:
    """Memory block information for debugging."""
    id: str
    pointer: Optional[int]
    size: int
    allocation_site: str
    allocation_function: str
    allocation_file: str
    allocation_line: int
    allocation_time: datetime
    thread_id: str
    is_active: bool = True
    metadata: Dict[str, Any] = field(default_factory=dict)
@dataclass
class TraceEvent:
    """Trace event for simulation analysis."""
    id: str
    timestamp: datetime
    event_type: str
    simulation_step: int
    data: Dict[str, Any] = field(default_factory=dict)
    duration: Optional[float] = None
@dataclass
class ErrorContext:
    """Enhanced error context with recovery information."""
    error_id: str
    error_type: str
    error_message: str
    timestamp: datetime
    stack_trace: str
    context: Dict[str, Any] = field(default_factory=dict)
    recovery_actions: List[Dict[str, Any]] = field(default_factory=list)
    is_recovered: bool = False
    recovery_steps: List[str] = field(default_factory=list)
@dataclass
class SystemState:
    """System state snapshot for debugging and rollback."""
    state_id: str
    timestamp: datetime
    simulation_step: int
    cpu_usage: float
    memory_usage: float
    neural_network_state: Dict[str, Any] = field(default_factory=dict)
    performance_metrics: Dict[str, float] = field(default_factory=dict)
    custom_data: Dict[str, Any] = field(default_factory=dict)
class NLMDebugAPI:
    """
    Comprehensive debugging API for NLM.
    
    This class provides advanced debugging capabilities including:
    - Structured logging with correlation IDs
    - Performance monitoring and visualization
    - Memory debugging and tracking
    - Trace analysis and visualization
    - Error recovery mechanisms
    - Incident management
    - Live system visualization
    """
    
    def __init__(self, data_dir: str = "./debug_data"):
        """Initialize the NLM Debug API."""
        self.data_dir = Path(data_dir)
        self.data_dir.mkdir(parents=True, exist_ok=True)
        
        # Core data structures
        self.structured_logs: List[StructuredLogEntry] = []
        self.performance_metrics: List[PerformanceMetric] = []
        self.memory_blocks: Dict[str, MemoryBlock] = {}
        self.trace_events: List[TraceEvent] = []
        self.error_contexts: List[ErrorContext] = []
        self.system_states: List[SystemState] = []
        
        # Correlation contexts
        self.correlation_contexts: Dict[str, CorrelationContext] = {}
        
        # Performance tracking
        self.benchmark_results: Dict[str, List[Dict[str, Any]]] = defaultdict(list)
        self.performance_scopes: Dict[str, float] = {}
        
        # Threading and concurrency
        self.log_lock = threading.Lock()
        self.data_lock = threading.Lock()
        self.background_threads: List[threading.Thread] = []
        self.executor = ThreadPoolExecutor(max_workers=4)
        
        # Real-time monitoring
        self.monitoring_active = False
        self.monitor_thread: Optional[threading.Thread] = None
        self.real_time_data = queue.Queue()
        
        # Visualization
        self.plot_cache: Dict[str, str] = {}  # cache visualizations as base64 strings
        
        # Error recovery
        self.recovery_actions: List[Callable[[], bool]] = []
        
        # Incident management
        self.active_incidents: List[Dict[str, Any]] = []
        
        # Initialize components
        self._initialize_default_handlers()
        
        logger.info("NLM Debug API initialized")
    
    def _initialize_default_handlers(self):
        """Initialize default handlers and monitors."""
        # Register default performance metric collector
        self.register_performance_collector("system_cpu", self._collect_cpu_usage)
        self.register_performance_collector("system_memory", self._collect_memory_usage)
        self.register_performance_collector("simulation_step_time", self._collect_simulation_step_time)
        
        # Register default error recovery actions
        self.register_recovery_action("memory_cleanup", self._cleanup_memory)
        self.register_recovery_action("simulation_reset", self._reset_simulation)
    
    # Core logging methods
    def log_trace(self, message: str, category: str = "System", 
                  correlation_id: Optional[str] = None,
                  metadata: Optional[Dict[str, Any]] = None) -> None:
        """Log a TRACE level message with structured data."""
        entry = StructuredLogEntry(
            correlation_id=correlation_id or self._generate_correlation_id(),
            timestamp=datetime.now(),
            level=DebugLevel.TRACE,
            category=LogCategory(category) if isinstance(category, str) else category,
            message=message,
            metadata=metadata or {},
            file="",
            line=0,
            function=""
        )
        
        with self.log_lock:
            self.structured_logs.append(entry)
        
        self._emit_log(entry)
        self._update_correlation_context(entry.correlation_id, entry)
        
        # Emit to real-time data stream
        self.real_time_data.put(entry)
    
    def log_debug(self, message: str, category: str = "System",
                  correlation_id: Optional[str] = None,
                  metadata: Optional[Dict[str, Any]] = None) -> None:
        """Log a DEBUG level message with structured data."""
        entry = StructuredLogEntry(
            correlation_id=correlation_id or self._generate_correlation_id(),
            timestamp=datetime.now(),
            level=DebugLevel.DEBUG,
            category=LogCategory(category) if isinstance(category, str) else category,
            message=message,
            metadata=metadata or {},
            file="",
            line=0,
            function=""
        )
        
        with self.log_lock:
            self.structured_logs.append(entry)
        
        self._emit_log(entry)
        self._update_correlation_context(entry.correlation_id, entry)
        self.real_time_data.put(entry)
    
    def log_info(self, message: str, category: str = "System",
                 correlation_id: Optional[str] = None,
                 metadata: Optional[Dict[str, Any]] = None) -> None:
        """Log an INFO level message with structured data."""
        entry = StructuredLogEntry(
            correlation_id=correlation_id or self._generate_correlation_id(),
            timestamp=datetime.now(),
            level=DebugLevel.INFO,
            category=LogCategory(category) if isinstance(category, str) else category,
            message=message,
            metadata=metadata or {},
            file="",
            line=0,
            function=""
        )
        
        with self.log_lock:
            self.structured_logs.append(entry)
        
        self._emit_log(entry)
        self._update_correlation_context(entry.correlation_id, entry)
        self.real_time_data.put(entry)
    
    def log_warning(self, message: str, category: str = "System",
                    correlation_id: Optional[str] = None,
                    metadata: Optional[Dict[str, Any]] = None) -> None:
        """Log a WARNING level message with structured data."""
        entry = StructuredLogEntry(
            correlation_id=correlation_id or self._generate_correlation_id(),
            timestamp=datetime.now(),
            level=DebugLevel.WARNING,
            category=LogCategory(category) if isinstance(category, str) else category,
            message=message,
            metadata=metadata or {},
            file="",
            line=0,
            function=""
        )
        
        with self.log_lock:
            self.structured_logs.append(entry)
        
        self._emit_log(entry)
        self._update_correlation_context(entry.correlation_id, entry)
        self.real_time_data.put(entry)
    
    def log_error(self, message: str, category: str = "Error",
                  correlation_id: Optional[str] = None,
                  metadata: Optional[Dict[str, Any]] = None) -> None:
        """Log an ERROR level message with structured data and context."""
        entry = StructuredLogEntry(
            correlation_id=correlation_id or self._generate_correlation_id(),
            timestamp=datetime.now(),
            level=DebugLevel.ERROR,
            category=LogCategory(category) if isinstance(category, str) else category,
            message=message,
            metadata=metadata or {},
            file="",
            line=0,
            function=""
        )
        
        with self.log_lock:
            self.structured_logs.append(entry)
        
        self._emit_log(entry)
        self._update_correlation_context(entry.correlation_id, entry)
        self.real_time_data.put(entry)
        
        # Create error context for recovery
        error_context = ErrorContext(
            error_id=f"err_{int(time.time())}",
            error_type=category,
            error_message=message,
            timestamp=datetime.now(),
            stack_trace=traceback.format_stack(),
            context=metadata or {}
        )
        
        with self.data_lock:
            self.error_contexts.append(error_context)
        
        # Trigger recovery mechanisms
        self._trigger_error_recovery(error_context)
    
    def log_critical(self, message: str, category: str = "Error",
                     correlation_id: Optional[str] = None,
                     metadata: Optional[Dict[str, Any]] = None) -> None:
        """Log a CRITICAL level message with structured data."""
        entry = StructuredLogEntry(
            correlation_id=correlation_id or self._generate_correlation_id(),
            timestamp=datetime.now(),
            level=DebugLevel.CRITICAL,
            category=LogCategory(category) if isinstance(category, str) else category,
            message=message,
            metadata=metadata or {},
            file="",
            line=0,
            function=""
        )
        
        with self.log_lock:
            self.structured_logs.append(entry)
        
        self._emit_log(entry)
        self._update_correlation_context(entry.correlation_id, entry)
        self.real_time_data.put(entry)
        
        # Create incident
        self._create_incident("CRITICAL", message, category, metadata or {})
    
    # Performance monitoring
    def log_performance(self, metric_name: str, value: float, unit: str = "ms",
                       category: str = "Performance",
                       correlation_id: Optional[str] = None,
                       tags: Optional[Dict[str, str]] = None) -> None:
        """Log a performance metric."""
        metric = PerformanceMetric(
            name=metric_name,
            value=value,
            unit=unit,
            timestamp=datetime.now(),
            tags=tags or {}
        )
        
        with self.data_lock:
            self.performance_metrics.append(metric)
        
        # Log as structured message
        metadata = {"value": str(value), "unit": unit}
        if tags:
            metadata.update(tags)
        
        self.log_info(
            f"Performance metric: {metric_name} = {value} {unit}",
            category,
            correlation_id,
            metadata
        )
        
        # Emit to real-time data
        self.real_time_data.put(metric)
    
    def start_performance_scope(self, scope_name: str,
                               metadata: Optional[Dict[str, Any]] = None) -> None:
        """Start a performance scope for timing measurements."""
        if metadata is None:
            metadata = {}
        
        self.performance_scopes[scope_name] = time.time()
        self.log_debug(f"Performance scope started: {scope_name}", "Performance", metadata=metadata)
    
    def end_performance_scope(self, scope_name: str,
                             metadata: Optional[Dict[str, Any]] = None) -> None:
        """End a performance scope and log the duration."""
        if scope_name not in self.performance_scopes:
            return
        
        start_time = self.performance_scopes.pop(scope_name)
        duration = time.time() - start_time
        
        # Log performance metric
        self.log_performance(
            f"scope_{scope_name}",
            duration * 1000,  # convert to milliseconds
            "ms",
            "Performance",
            metadata=metadata or {"scope_name": scope_name}
        )
    
    # Memory debugging
    def track_memory_allocation(self, pointer: Optional[int], size: int,
                               allocation_site: str = "",
                               metadata: Optional[Dict[str, Any]] = None) -> str:
        """Track memory allocation for debugging."""
        block_id = f"mem_{int(time.time())}_{len(self.memory_blocks)}"
        
        block = MemoryBlock(
            id=block_id,
            pointer=pointer,
            size=size,
            allocation_site=allocation_site,
            allocation_function="",
            allocation_file="",
            allocation_line=0,
            allocation_time=datetime.now(),
            thread_id=str(threading.get_ident()),
            is_active=True,
            metadata=metadata or {}
        )
        
        with self.data_lock:
            self.memory_blocks[block_id] = block
        
        self.log_info(
            f"Memory allocated: {block_id} ({size} bytes)",
            "Memory",
            metadata={"block_id": block_id, "size": str(size), "pointer": str(pointer) if pointer else "null"}
        )
        
        return block_id
    
    def track_memory_deallocation(self, block_id: str) -> None:
        """Track memory deallocation for debugging."""
        with self.data_lock:
            if block_id in self.memory_blocks:
                block = self.memory_blocks[block_id]
                block.is_active = False
                block.metadata["deallocation_time"] = datetime.now().isoformat()
    
    # Network debugging
    def log_network_activity(self, operation: str, endpoint: str,
                            latency: float, bytes_sent: int = 0,
                            bytes_received: int = 0,
                            correlation_id: Optional[str] = None,
                            metadata: Optional[Dict[str, Any]] = None) -> None:
        """Log network activity for debugging."""
        self.log_info(
            f"Network activity: {operation} -> {endpoint} ({latency:.2f}ms)",
            "Network",
            correlation_id,
            {
                "operation": operation,
                "endpoint": endpoint,
                "latency_ms": str(latency),
                "bytes_sent": str(bytes_sent),
                "bytes_received": str(bytes_received),
                **(metadata or {})
            }
        )
    
    # GUI debugging
    def log_gui_click(self, element_id: str, element_type: str,
                     correlation_id: Optional[str] = None) -> None:
        """Log GUI click events for debugging."""
        self.log_debug(
            f"GUI click: {element_type} (id: {element_id})",
            "GUI",
            correlation_id,
            {"element_id": element_id, "element_type": element_type}
        )
    
    def log_gui_update(self, element_id: str, update_type: str,
                      correlation_id: Optional[str] = None) -> None:
        """Log GUI update events for debugging."""
        self.log_debug(
            f"GUI update: {update_type} (element: {element_id})",
            "GUI",
            correlation_id,
            {"element_id": element_id, "update_type": update_type}
        )
    
    # Simulation debugging
    def log_simulation_step(self, step: int, simulation_time: float,
                           active_neurons: int, total_neurons: int,
                           correlation_id: Optional[str] = None,
                           metadata: Optional[Dict[str, Any]] = None) -> None:
        """Log simulation step for debugging."""
        self.log_info(
            f"Simulation step: {step} ({simulation_time:.2f}ms, {active_neurons}/{total_neurons} neurons)",
            "Simulation",
            correlation_id,
            {
                "step": str(step),
                "simulation_time_ms": str(simulation_time),
                "active_neurons": str(active_neurons),
                "total_neurons": str(total_neurons),
                **(metadata or {})
            }
        )
    
    # Trace and visualization
    def start_trace(self, trace_name: str,
                   metadata: Optional[Dict[str, Any]] = None) -> str:
        """Start a trace for analysis."""
        trace_id = f"trace_{int(time.time())}_{len(self.trace_events)}"
        
        trace_event = TraceEvent(
            id=trace_id,
            timestamp=datetime.now(),
            event_type="trace_start",
            simulation_step=0,
            data=metadata or {}
        )
        
        with self.data_lock:
            self.trace_events.append(trace_event)
        
        self.log_info(f"Trace started: {trace_name} (ID: {trace_id})", "System", metadata={"trace_id": trace_id, "trace_name": trace_name})
        
        return trace_id
    
    def end_trace(self, trace_id: str, success: bool = True,
                 result: str = "",
                 metadata: Optional[Dict[str, Any]] = None) -> None:
        """End a trace."""
        with self.data_lock:
            for event in self.trace_events:
                if event.id == trace_id:
                    event.event_type = "trace_end"
                    event.duration = time.time() - event.timestamp.timestamp()
                    event.data.update(metadata or {})
                    event.data["success"] = success
                    event.data["result"] = result
                    break
        
        self.log_info(f"Trace completed: {trace_id} ({'success' if success else 'failed'})", "System", metadata=metadata or {})
    
    # Error recovery
    def attempt_recovery(self, operation: Callable, error_context: str,
                        max_retries: int = 3, retry_delay: float = 1.0,
                        metadata: Optional[Dict[str, Any]] = None) -> Any:
        """Attempt error recovery with retries."""
        last_exception = None
        
        for attempt in range(max_retries):
            try:
                result = operation()
                self.log_info(f"Recovery successful: {error_context} (attempt {attempt + 1})", "Error", metadata=metadata)
                return result
            except Exception as e:
                last_exception = e
                self.log_warning(f"Recovery attempt failed: {error_context} (attempt {attempt + 1}/{max_retries})", "Error", metadata=metadata)
                
                if attempt < max_retries - 1:
                    time.sleep(retry_delay)
        
        self.log_error(f"Recovery failed after {max_retries} attempts: {error_context}", "Error", metadata=metadata)
        raise last_exception
    
    # Incident management
    def create_incident(self, incident_type: str, severity: str,
                       description: str,
                       details: Optional[Dict[str, Any]] = None) -> str:
        """Create a system incident."""
        incident_id = f"inc_{int(time.time())}_{len(self.active_incidents)}"
        
        incident = {
            "id": incident_id,
            "type": incident_type,
            "severity": severity,
            "description": description,
            "timestamp": datetime.now().isoformat(),
            "details": details or {},
            "status": "active"
        }
        
        self.active_incidents.append(incident)
        self.log_critical(f"Incident created: {incident_id} - {description}", "Error", metadata=incident)
        
        return incident_id
    
    def resolve_incident(self, incident_id: str, resolution: str) -> None:
        """Resolve an incident."""
        for incident in self.active_incidents:
            if incident["id"] == incident_id:
                incident["status"] = "resolved"
                incident["resolution"] = resolution
                incident["resolved_time"] = datetime.now().isoformat()
                break
        
        self.log_info(f"Incident resolved: {incident_id}", "Error", metadata={"incident_id": incident_id, "resolution": resolution})
    
    # System state management
    def capture_system_state(self, state_name: str,
                           metadata: Optional[Dict[str, Any]] = None) -> str:
        """Capture system state snapshot for debugging."""
        state_id = f"state_{int(time.time())}_{len(self.system_states)}"
        
        # Get system metrics
        cpu_usage = psutil.cpu_usage()
        memory = psutil.virtual_memory()
        
        state = SystemState(
            state_id=state_id,
            timestamp=datetime.now(),
            simulation_step=0,  # Would need actual step number from simulation
            cpu_usage=cpu_usage.percent,
            memory_usage=memory.percent,
            performance_metrics=self._collect_performance_metrics(),
            custom_data=metadata or {}
        )
        
        with self.data_lock:
            self.system_states.append(state)
        
        self.log_info(f"System state captured: {state_name} (ID: {state_id})", "System", metadata={"state_id": state_id, "state_name": state_name})
        
        return state_id
    
    def rollback_system_state(self, target_state_id: str) -> None:
        """Rollback system to a previous state."""
        with self.data_lock:
            for state in self.system_states:
                if state.state_id == target_state_id:
                    self.log_warning(f"System rollback to state: {target_state_id}", "System", metadata={"target_state": target_state_id})
                    # In a real implementation, this would restore the system state
                    break
    
    # Performance benchmarking
    def run_benchmark(self, benchmark_name: str,
                     benchmark_function: Callable,
                     iterations: int = 10,
                     metadata: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Run a performance benchmark."""
        times = []
        errors = []
        
        for i in range(iterations):
            try:
                start_time = time.time()
                result = benchmark_function()
                end_time = time.time()
                
                execution_time = (end_time - start_time) * 1000  # Convert to milliseconds
                times.append(execution_time)
                
                self.log_info(f"Benchmark iteration {i+1}/{iterations}: {benchmark_name} ({execution_time:.2f}ms)", "Performance", metadata=metadata)
            except Exception as e:
                errors.append(str(e))
                self.log_error(f"Benchmark iteration failed: {i+1}/{iterations}", "Performance", metadata={**(metadata or {}), "error": str(e)})
        
        if not times:
            raise RuntimeError("Benchmark failed on all iterations")
        
        result = {
            "name": benchmark_name,
            "iterations": iterations,
            "successful_iterations": len(times),
            "failed_iterations": len(errors),
            "execution_times": times,
            "average_time_ms": np.mean(times) if times else 0,
            "min_time_ms": np.min(times) if times else 0,
            "max_time_ms": np.max(times) if times else 0,
            "std_time_ms": np.std(times) if len(times) > 1 else 0,
            "metadata": metadata or {},
            "timestamp": datetime.now().isoformat()
        }
        
        self.benchmark_results[benchmark_name].append(result)
        
        return result
    
    # Visualization methods
    def create_memory_visualization(self, output_file: str = "memory_report.html") -> str:
        """Create memory allocation visualization."""
        with self.data_lock:
            active_blocks = [block for block in self.memory_blocks.values() if block.is_active]
            total_memory = sum(block.size for block in active_blocks)
            
            if not active_blocks:
                return "No memory allocations found"
            
            # Create pie chart
            fig = go.Figure(data=[go.Pie(
                labels=[f"{block.id[:8]}..." for block in active_blocks],
                values=[block.size for block in active_blocks],
                textinfo="percent+value",
                insidetextorientation='radial'
            )])
            
            fig.update_layout(
                title=f"Memory Allocation Report - Total: {total_memory} bytes",
                showlegend=True
            )
            
            # Save and cache visualization
            self._save_visualization(fig, output_file)
            
            return output_file
    
    def create_performance_visualization(self, metric_name: str, output_file: str = "performance_report.html") -> str:
        """Create performance metrics visualization."""
        with self.data_lock:
            metrics = [m for m in self.performance_metrics if m.name == metric_name]
            
            if not metrics:
                return "No performance metrics found"
            
            # Create time series chart
            fig = go.Figure()
            
            fig.add_trace(go.Scatter(
                x=[m.timestamp for m in metrics],
                y=[m.value for m in metrics],
                mode='lines+markers',
                name=metric_name
            ))
            
            fig.update_layout(
                title=f"Performance Metrics: {metric_name}",
                xaxis_title="Time",
                yaxis_title=f"Value ({[m.unit for m in metrics][0] if metrics else 'ms'})",
                hovermode='x unified'
            )
            
            self._save_visualization(fig, output_file)
            
            return output_file
    
    def create_trace_visualization(self, output_file: str = "trace_analysis.html") -> str:
        """Create trace analysis visualization."""
        with self.data_lock:
            # Group traces by simulation step
            traces_by_step = defaultdict(list)
            for trace in self.trace_events:
                traces_by_step[trace.simulation_step].append(trace)
            
            # Create activity chart
            fig = go.Figure()
            
            steps = list(traces_by_step.keys())
            activity_counts = [len(traces) for traces in traces_by_step.values()]
            
            fig.add_trace(go.Bar(
                x=steps,
                y=activity_counts,
                name="Trace Events"
            ))
            
            fig.update_layout(
                title="Trace Activity Analysis",
                xaxis_title="Simulation Step",
                yaxis_title="Number of Trace Events"
            )
            
            self._save_visualization(fig, output_file)
            
            return output_file
    
    # Real-time monitoring
    def start_real_time_monitoring(self, interval: float = 0.1) -> None:
        """Start real-time monitoring of system state."""
        if self.monitoring_active:
            return
        
        self.monitoring_active = True
        
        def monitoring_loop():
            while self.monitoring_active:
                try:
                    # Collect system metrics
                    system_metrics = self._collect_system_metrics()
                    self.real_time_data.put(system_metrics)
                    
                    # Collect performance metrics
                    for collector_name, collector in self.performance_collectors.items():
                        try:
                            metric = collector()
                            self.real_time_data.put(metric)
                        except Exception as e:
                            self.log_error(f"Performance collector failed: {collector_name}", "Performance", metadata={"error": str(e)})
                    
                    time.sleep(interval)
                except Exception as e:
                    self.log_error(f"Monitoring loop error", "System", metadata={"error": str(e)})
                    time.sleep(interval)
        
        self.monitor_thread = threading.Thread(target=monitoring_loop, daemon=True)
        self.monitor_thread.start()
        
        self.log_info("Real-time monitoring started", "System")
    
    def stop_real_time_monitoring(self) -> None:
        """Stop real-time monitoring."""
        self.monitoring_active = False
        
        if self.monitor_thread and self.monitor_thread.is_alive():
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
        
        self.log_info("Real-time monitoring stopped", "System")
    
    # Export and import
    def export_debug_data(self, output_dir: str = "./debug_export") -> None:
        """Export all debug data to files."""
        output_path = Path(output_dir)
        output_path.mkdir(parents=True, exist_ok=True)
        
        # Export structured logs
        logs_file = output_path / "structured_logs.json"
        with open(logs_file, 'w') as f:
            json.dump([
                {
                    "correlation_id": log.correlation_id,
                    "timestamp": log.timestamp.isoformat(),
                    "level": log.level.value,
                    "category": log.category.value,
                    "message": log.message,
                    "metadata": log.metadata,
                    "metrics": log.metrics
                }
                for log in self.structured_logs
            ], f, indent=2)
        
        # Export performance metrics
        metrics_file = output_path / "performance_metrics.json"
        with open(metrics_file, 'w') as f:
            json.dump([
                {
                    "name": metric.name,
                    "value": metric.value,
                    "unit": metric.unit,
                    "timestamp": metric.timestamp.isoformat(),
                    "tags": metric.tags
                }
                for metric in self.performance_metrics
            ], f, indent=2)
        
        # Export memory blocks
        memory_file = output_path / "memory_blocks.json"
        with open(memory_file, 'w') as f:
            json.dump([
                {
                    "id": block.id,
                    "size": block.size,
                    "allocation_site": block.allocation_site,
                    "allocation_time": block.allocation_time.isoformat(),
                    "is_active": block.is_active,
                    "metadata": block.metadata
                }
                for block in self.memory_blocks.values()
            ], f, indent=2)
        
        # Export trace events
        traces_file = output_path / "trace_events.json"
        with open(traces_file, 'w') as f:
            json.dump([
                {
                    "id": trace.id,
                    "timestamp": trace.timestamp.isoformat(),
                    "event_type": trace.event_type,
                    "simulation_step": trace.simulation_step,
                    "data": trace.data,
                    "duration": trace.duration
                }
                for trace in self.trace_events
            ], f, indent=2)
        
        # Export error contexts
        errors_file = output_path / "error_contexts.json"
        with open(errors_file, 'w') as f:
            json.dump([
                {
                    "error_id": error.error_id,
                    "error_type": error.error_type,
                    "error_message": error.error_message,
                    "timestamp": error.timestamp.isoformat(),
                    "context": error.context,
                    "is_recovered": error.is_recovered,
                    "recovery_steps": error.recovery_steps
                }
                for error in self.error_contexts
            ], f, indent=2)
        
        self.log_info(f"Debug data exported to {output_dir}", "System")
    
    # Internal helper methods
    def _generate_correlation_id(self) -> str:
        """Generate a unique correlation ID."""
        return f"corr_{int(time.time())}_{hash(str(time.time()) % 1000000)}"
    
    def _update_correlation_context(self, correlation_id: str, log_entry: StructuredLogEntry) -> None:
        """Update correlation context with new log entry."""
        with self.data_lock:
            if correlation_id in self.correlation_contexts:
                context = self.correlation_contexts[correlation_id]
                context.metadata[f"log_{log_entry.level.value}"] = log_entry.message
    
    def _emit_log(self, entry: StructuredLogEntry) -> None:
        """Emit log entry to various handlers."""
        # Default implementation just prints to console
        timestamp_str = entry.timestamp.strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
        
        # Format log entry
        log_line = f"[{timestamp_str}] [CID:{entry.correlation_id}] [{entry.category.value}/{entry.level.value}] {entry.message}"
        
        if entry.metadata:
            log_line += f" {{meta:{json.dumps(entry.metadata)}}}"
        
        if entry.metrics:
            log_line += f" {{metrics:{json.dumps(entry.metrics)}}}"
        
        print(log_line)
    
    def _collect_system_metrics(self) -> Dict[str, Any]:
        """Collect system metrics for real-time monitoring."""
        cpu_usage = psutil.cpu_usage()
        memory = psutil.virtual_memory()
        disk = psutil.disk_usage('/')
        
        return {
            "timestamp": datetime.now().isoformat(),
            "cpu_usage_percent": cpu_usage.percent,
            "memory_usage_percent": memory.percent,
            "memory_available_gb": memory.available / (1024**3),
            "disk_usage_percent": disk.percent,
            "disk_available_gb": disk.free / (1024**3)
        }
    
    def _collect_performance_metrics(self) -> Dict[str, float]:
        """Collect performance metrics."""
        metrics = {}
        
        # Collect recent metrics
        recent_metrics = self.performance_metrics[-10:] if self.performance_metrics else []
        
        for metric in recent_metrics:
            key = f"{metric.name}_{metric.unit}"
            metrics[key] = metric.value
        
        return metrics
    
    # Performance collector registration
    def register_performance_collector(self, name: str, collector: Callable) -> None:
        """Register a performance metric collector."""
        self.performance_collectors[name] = collector
    
    # Error recovery action registration
    def register_recovery_action(self, name: str, action: Callable) -> None:
        """Register an error recovery action."""
        self.recovery_actions.append(action)
    
    # Default handler implementations
    def _collect_cpu_usage(self) -> PerformanceMetric:
        """Default CPU usage collector."""
        cpu_percent = psutil.cpu_usage()
        return PerformanceMetric(
            name="system_cpu",
            value=cpu_percent,
            unit="percent",
            timestamp=datetime.now(),
            tags={"host": psutil.hostname()}
        )
    
    def _collect_memory_usage(self) -> PerformanceMetric:
        """Default memory usage collector."""
        memory = psutil.virtual_memory()
        return PerformanceMetric(
            name="system_memory",
            value=memory.percent,
            unit="percent",
            timestamp=datetime.now(),
            tags={"host": psutil.hostname()}
        )
    
    def _collect_simulation_step_time(self) -> PerformanceMetric:
        """Default simulation step time collector."""
        # This would need actual simulation timing data
        return PerformanceMetric(
            name="simulation_step_time",
            value=0.0,  # Placeholder
            unit="ms",
            timestamp=datetime.now(),
            tags={"simulation": "default"}
        )
    
    def _cleanup_memory(self) -> bool:
        """Default memory cleanup action."""
        try:
            # Force garbage collection
            import gc
            gc.collect()
            
            # Log memory cleanup
            self.log_info("Memory cleanup performed", "System")
            return True
        except Exception as e:
            self.log_error(f"Memory cleanup failed: {str(e)}", "System")
            return False
    
    def _reset_simulation(self) -> bool:
        """Default simulation reset action."""
        try:
            # This would reset the actual simulation
            self.log_info("Simulation reset performed", "System")
            return True
        except Exception as e:
            self.log_error(f"Simulation reset failed: {str(e)}", "System")
            return False
    
    def _trigger_error_recovery(self, error_context: ErrorContext) -> None:
        """Trigger error recovery mechanisms."""
        # Try registered recovery actions
        for action in self.recovery_actions:
            try:
                if action():
                    error_context.is_recovered = True
                    error_context.recovery_steps.append(f"Recovery action executed: {action.__name__}")
                    break
            except Exception as e:
                self.log_error(f"Recovery action failed: {str(e)}", "System")
    
    def _save_visualization(self, fig, filename: str) -> None:
        """Save visualization and cache it."""
        try:
            # Save as HTML
            html_path = self.data_dir / f"{filename}.html"
            pio.write_html(fig, str(html_path))
            
            # Save as image
            img_path = self.data_dir / f"{Path(filename).stem}.png"
            fig.write_image(str(img_path))
            
            # Cache as base64
            with open(img_path, 'rb') as f:
                img_data = f.read()
                cached_key = f"viz_{filename}"
                self.plot_cache[cached_key] = base64.b64encode(img_data).decode()
            
        except Exception as e:
            self.log_error(f"Failed to save visualization: {str(e)}", "System")
    
    # Context manager for performance scopes
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.stop_real_time_monitoring()
    
    # Cleanup
    def cleanup(self):
        """Clean up resources."""
        self.stop_real_time_monitoring()
        
        if self.monitor_thread and self.monitor_thread.is_alive():
            self.monitor_thread.join(timeout=5.0)
        
        self.executor.shutdown(wait=True)
        
        # Clear caches
        self.plot_cache.clear()
        
        logger.info("NLM Debug API cleanup completed")
    
    # Static method for global access
    @staticmethod
    def get_global_api() -> 'NLMDebugAPI':
        """Get the global NLM Debug API instance."""
        if not hasattr(NLMDebugAPI, '_global_instance'):
            NLMDebugAPI._global_instance = NLMDebugAPI()
        return NLMDebugAPI._global_instance
# Global instance for easy access
_debug_api = NLMDebugAPI.get_global_api()
def get_debug_api() -> NLMDebugAPI:
    """Get the global NLM Debug API instance."""
    return _debug_api
def log(message: str, level: str = "INFO", category: str = "System",
        correlation_id: Optional[str] = None, metadata: Optional[Dict[str, Any]] = None) -> None:
    """Convenience function for logging."""
    api = get_debug_api()
    
    if level == "TRACE":
        api.log_trace(message, category, correlation_id, metadata)
    elif level == "DEBUG":
        api.log_debug(message, category, correlation_id, metadata)
    elif level == "INFO":
        api.log_info(message, category, correlation_id, metadata)
    elif level == "WARNING":
        api.log_warning(message, category, correlation_id, metadata)
    elif level == "ERROR":
        api.log_error(message, category, correlation_id, metadata)
    elif level == "CRITICAL":
        api.log_critical(message, category, correlation_id, metadata)
def performance(metric_name: str, value: float, unit: str = "ms",
                category: str = "Performance", correlation_id: Optional[str] = None,
                tags: Optional[Dict[str, str]] = None) -> None:
    """Convenience function for logging performance metrics."""
    api = get_debug_api()
    api.log_performance(metric_name, value, unit, category, correlation_id, tags)
if __name__ == "__main__":
    # Example usage
    api = NLMDebugAPI()
    
    # Log some example messages
    api.log_info("Application started")
    api.log_debug("Loading neural network model")
    api.performance("model_loading", 150.5, "ms")
    
    # Start a benchmark
    def dummy_benchmark():
        time.sleep(0.01)
        return "done"
    
    result = api.run_benchmark("dummy_benchmark", dummy_benchmark, iterations=5)
    print(f"Benchmark result: {result}")
    
    # Start real-time monitoring
    api.start_real_time_monitoring(interval=0.5)
    
    # Create visualizations
    api.create_memory_visualization()
    api.create_performance_visualization("model_loading")
    
    # Export data
    api.export_debug_data("./debug_export")
    
    # Cleanup
    api.cleanup()