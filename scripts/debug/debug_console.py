#!/usr/bin/env python3
"""
Debug Console Tool

Interactive debugging console for NLM simulations with real-time brain monitoring,
step-by-step execution control, breakpoints, single-step execution, memory and
performance monitoring.

Features:
- Interactive debugging console with command-line interface
- Real-time brain monitoring and visualization
- Step-by-step execution control with single-step mode
- Breakpoints and conditional breakpoint support
- Memory and performance monitoring
- Variable inspection and modification
- Stack trace and call stack analysis
- Thread and process monitoring
- Event log and debug log analysis
- Integration with visualization tools

Usage Examples:
    # Start interactive debug console
    python scripts/debug/debug_console.py --interactive

    # Connect to running simulation
    python scripts/debug/debug_console.py --connect localhost:8888

    # Start with preloaded simulation data
    python scripts/debug/debug_console.py --input simulation_data.json

    # Run with preset debugging commands
    python scripts/debug/debug_console.py --commands "breakpoints add neuron_1", "step"

    # Monitor memory usage
    python scripts/debug/debug_console.py --monitor memory --interval 1000

API References:
    DebugConsole.connect(host, port)
    DebugConsole.add_breakpoint(condition, action)
    DebugConsole.set_single_step(mode)
    DebugConsole.execute_step()
    DebugConsole.inspect_variable(path)
    DebugConsole.modify_variable(path, value)
    DebugConsole.start_memory_monitor(interval)
    DebugConsole.start_performance_monitor()
    DebugConsole.get_stack_trace()
    DebugConsole.get_thread_status()
    DebugConsole.log_event(message, level='INFO')
"""

import sys
import time
import json
import threading
import queue
import traceback
from pathlib import Path
import argparse
from typing import Dict, List, Any, Optional, Tuple, Union, Callable
from datetime import datetime
import psutil
import numpy as np
from dataclasses import dataclass, field
from enum import Enum
import signal
import os
import socket
import select
import readline
from collections import defaultdict, deque
import warnings
warnings.filterwarnings('ignore')

# Optional imports for enhanced debugging
try:
    import zmq
    ZMQ_AVAILABLE = True
except ImportError:
    ZMQ_AVAILABLE = False

try:
    from IPython.core.interactiveshell import InteractiveShell
    IPYTHON_AVAILABLE = True
except ImportError:
    IPYTHON_AVAILABLE = False


class DebugLevel(Enum):
    """Debug logging levels."""
    DEBUG = "DEBUG"
    INFO = "INFO"
    WARNING = "WARNING"
    ERROR = "ERROR"
    CRITICAL = "CRITICAL"


class BreakpointType(Enum):
    """Breakpoint types."""
    LINE = "line"
    FUNCTION = "function"
    CONDITIONAL = "conditional"
    WATCH = "watch"
    EXCEPTION = "exception"


class DebugState(Enum):
    """Debug session states."""
    IDLE = "idle"
    RUNNING = "running"
    PAUSED = "paused"
    STOPPED = "stopped"
    RECORDING = "recording"


@dataclass
class Breakpoint:
    """Represents a debugging breakpoint."""
    id: str
    type: BreakpointType
    location: str
    condition: Optional[str] = None
    action: Optional[Callable] = None
    active: bool = True
    hit_count: int = 0
    hit_data: Dict[str, Any] = field(default_factory=dict)
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class DebugEvent:
    """Represents a debug event."""
    timestamp: float
    level: DebugLevel
    message: str
    context: Dict[str, Any] = field(default_factory=dict)
    source: str = ""
    thread_id: int = 0


@dataclass
class VariableState:
    """Represents variable state for inspection."""
    path: str
    value: Any
    type: str
    scope: str = "local"
    last_modified: float = 0.0
    references: int = 0
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class MemorySnapshot:
    """Represents memory snapshot for monitoring."""
    timestamp: float
    total_memory: float
    available_memory: float
    used_memory: float
    memory_percent: float
    process_memory: float
    thread_count: int
    cpu_percent: float
    disk_io: Dict[str, float]
    network_io: Dict[str, float]


class DebugConsole:
    """Interactive debugging console for NLM simulations."""
    
    def __init__(self, host: str = "localhost", port: int = 8888):
        """Initialize DebugConsole.
        
        Args:
            host: Host for debug server
            port: Port for debug server
        """
        self.host = host
        self.port = port
        self.state = DebugState.IDLE
        self.socket = None
        self.server_socket = None
        self.command_queue = queue.Queue()
        self.event_queue = queue.Queue()
        self.breakpoints: Dict[str, Breakpoint] = {}
        self.watch_variables: Dict[str, VariableState] = {}
        self.variable_history: Dict[str, List[VariableState]] = defaultdict(list)
        self.debug_events: List[DebugEvent] = []
        self.memory_snapshots: List[MemorySnapshot] = []
        self.performance_metrics: List[Dict[str, Any]] = []
        self.command_history: List[str] = []
        self.max_history = 1000
        self.single_step_mode = False
        self.step_count = 0
        self.current_thread_id = threading.current_thread().ident
        self.monitoring_active = False
        self.monitoring_interval = 1000  # milliseconds
        self.log_file = None
        self.recording_active = False
        self.simulation_data = {}
        self.neural_state = {}
        self.debug_context = {}
        
        # Initialize readline for interactive input
        readline.set_completer(self._complete_commands)
        readline.set_history_length(self.max_history)
        
        # Signal handlers for graceful shutdown
        signal.signal(signal.SIGINT, self._signal_handler)
        signal.signal(signal.SIGTERM, self._signal_handler)
    
    def _complete_commands(self, text, state):
        """Command completion for readline."""
        commands = [
            'help', 'quit', 'exit', 'connect', 'disconnect',
            'breakpoints', 'add-bp', 'remove-bp', 'list-bp',
            'step', 'continue', 'pause', 'resume', 'single',
            'variables', 'watch', 'unwatch', 'inspect',
            'modify', 'context', 'clear-context',
            'monitor', 'unmonitor', 'memory', 'performance',
            'threads', 'stack', 'trace', 'log', 'events',
            'record', 'stop-recording', 'import', 'export',
            'run-commands', 'save-history', 'load-history',
            'status', 'stats', 'clear', 'reset'
        ]
        
        matches = [cmd for cmd in commands if cmd.startswith(text)]
        
        if state < len(matches):
            return matches[state]
        else:
            return None
    
    def log_event(self, message: str, level: str = DebugLevel.INFO.value,
                  context: Optional[Dict[str, Any]] = None, source: str = ""):
        """Log debug event.
        
        Args:
            message: Event message
            level: Log level
            context: Additional context data
            source: Source of the event
        """
        event = DebugEvent(
            timestamp=time.time(),
            level=DebugLevel(level),
            message=message,
            context=context or {},
            source=source,
            thread_id=self.current_thread_id
        )
        
        self.debug_events.append(event)
        
        # Add to event queue for real-time processing
        self.event_queue.put(event)
        
        # Print to console
        level_color = {
            'DEBUG': '\033[94m',  # Blue
            'INFO': '\033[92m',   # Green
            'WARNING': '\033[93m',  # Yellow
            'ERROR': '\033[91m',   # Red
            'CRITICAL': '\033[95m'  # Magenta
        }.get(level, '')
        
        reset_color = '\033[0m'
        timestamp_str = datetime.now().strftime('%H:%M:%S.%f')[:-3]
        
        print(f"{level_color}[{timestamp_str}] {level}: {message}{reset_color}")
        
        # Log to file if recording
        if self.recording_active and self.log_file:
            self.log_file.write(f"[{timestamp_str}] {level}: {message}\n")
    
    def add_breakpoint(self, condition: str, location: str = "",
                      breakpoint_type: str = BreakpointType.CONDITIONAL.value,
                      action: Optional[Callable] = None) -> str:
        """Add a debugging breakpoint.
        
        Args:
            condition: Breakpoint condition (e.g., "neuron_1.activation > 0.5")
            location: Location (function name, line number, etc.)
            breakpoint_type: Type of breakpoint
            action: Optional action to execute when breakpoint hits
            
        Returns:
            Breakpoint ID
        """
        bp_id = f"bp_{len(self.breakpoints)}_{int(time.time())}"
        
        try:
            bp_type = BreakpointType(breakpoint_type.lower())
        except ValueError:
            bp_type = BreakpointType.CONDITIONAL
        
        breakpoint = Breakpoint(
            id=bp_id,
            type=bp_type,
            location=location,
            condition=condition,
            action=action,
            active=True
        )
        
        self.breakpoints[bp_id] = breakpoint
        
        self.log_event(f"Breakpoint added: {condition} at {location}",
                      DebugLevel.INFO.value, {"breakpoint_id": bp_id})
        
        return bp_id
    
    def remove_breakpoint(self, breakpoint_id: str):
        """Remove a breakpoint.
        
        Args:
            breakpoint_id: ID of breakpoint to remove
        """
        if breakpoint_id in self.breakpoints:
            del self.breakpoints[breakpoint_id]
            self.log_event(f"Breakpoint removed: {breakpoint_id}",
                          DebugLevel.INFO.value)
        else:
            self.log_event(f"Breakpoint not found: {breakpoint_id}",
                          DebugLevel.WARNING.value)
    
    def list_breakpoints(self) -> List[Dict[str, Any]]:
        """List all breakpoints.
        
        Returns:
            List of breakpoint information
        """
        return [
            {
                "id": bp.id,
                "type": bp.type.value,
                "location": bp.location,
                "condition": bp.condition,
                "active": bp.active,
                "hit_count": bp.hit_count
            }
            for bp in self.breakpoints.values()
        ]
    
    def check_breakpoints(self, context: Dict[str, Any] = None) -> List[str]:
        """Check if any breakpoints are triggered.
        
        Args:
            context: Current execution context
            
        Returns:
            List of triggered breakpoint IDs
        """
        triggered = []
        
        for bp_id, breakpoint in self.breakpoints.items():
            if not breakpoint.active:
                continue
            
            try:
                if self._evaluate_breakpoint_condition(breakpoint.condition, context):
                    breakpoint.hit_count += 1
                    breakpoint.hit_data = context or {}
                    triggered.append(bp_id)
                    
                    self.log_event(f"Breakpoint triggered: {breakpoint.condition}",
                                  DebugLevel.WARNING.value,
                                  {"breakpoint_id": bp_id, "context": context})
                    
                    # Execute action if defined
                    if breakpoint.action:
                        try:
                            breakpoint.action(context)
                        except Exception as e:
                            self.log_event(f"Breakpoint action failed: {e}",
                                          DebugLevel.ERROR.value,
                                          {"breakpoint_id": bp_id})
            
            except Exception as e:
                self.log_event(f"Error evaluating breakpoint: {e}",
                              DebugLevel.ERROR.value,
                              {"breakpoint_id": bp_id})
        
        return triggered
    
    def _evaluate_breakpoint_condition(self, condition: str,
                                     context: Dict[str, Any]) -> bool:
        """Evaluate breakpoint condition.
        
        Args:
            condition: Condition string to evaluate
            context: Current context
            
        Returns:
            True if condition is met
        """
        if not condition:
            return True
        
        # Simple evaluation for common cases
        try:
            # Extract variable names and values from context
            for var_name, var_value in context.items():
                # Create safe evaluation namespace
                eval_locals = {
                    var_name: var_value,
                    **{f"{k}_{i}": v for i, (k, v) in enumerate(context.items())}
                }
                
                # Try to evaluate the condition
                try:
                    result = eval(condition, {"__builtins__": {}}, eval_locals)
                    if isinstance(result, bool):
                        return result
                except:
                    continue
            
            # Fallback: simple string matching for common patterns
            if " > " in condition:
                parts = condition.split(" > ")
                if len(parts) == 2:
                    var_name = parts[0].strip()
                    try:
                        value = float(eval(var_name, {"__builtins__": {}}, context))
                        threshold = float(parts[1].strip())
                        return value > threshold
                    except:
                        pass
            
            return False
            
        except Exception:
            return False
    
    def inspect_variable(self, path: str) -> VariableState:
        """Inspect variable at given path.
        
        Args:
            path: Variable path (e.g., "neurons[0].activation", "global_activity")
            
        Returns:
            Variable state
        """
        # Navigate through the path
        parts = path.split(".")
        current_value = self.debug_context
        
        for part in parts:
            if "[" in part and "]" in part:
                # Handle array indexing like "neurons[0]"
                base_name = part[:part.index('[')]
                index_part = part[part.index('['):]
                
                if base_name not in current_value:
                    raise ValueError(f"Variable '{base_name}' not found")
                
                # Parse index
                index_str = index_part[1:-1]
                if index_str.isdigit():
                    current_value = current_value[base_name][int(index_str)]
                else:
                    current_value = current_value[base_name]
            else:
                if part not in current_value:
                    raise ValueError(f"Variable '{part}' not found in path")
                current_value = current_value[part]
        
        # Create variable state
        var_state = VariableState(
            path=path,
            value=current_value,
            type=str(type(current_value).__name__),
            scope="debug",
            last_modified=time.time(),
            references=self._count_references(path)
        )
        
        # Store in watch variables if not already present
        if path not in self.watch_variables:
            self.watch_variables[path] = var_state
        
        return var_state
    
    def modify_variable(self, path: str, value: Any, confirmation_required: bool = True):
        """Modify variable at given path.
        
        Args:
            path: Variable path
            value: New value
            confirmation_required: Whether to require confirmation
        """
        if confirmation_required:
            confirm = input(f"Are you sure you want to modify {path}? (y/N): ")
            if confirm.lower() != 'y':
                self.log_event(f"Variable modification cancelled for {path}",
                              DebugLevel.INFO.value)
                return
        
        try:
            # Navigate to parent object
            parts = path.split(".")
            parent = self.debug_context
            
            for part in parts[:-1]:
                if "[" in part and "]" in part:
                    base_name = part[:part.index('[')]
                    index_part = part[part.index('['):]
                    
                    if base_name not in parent:
                        raise ValueError(f"Variable '{base_name}' not found")
                    
                    index_str = index_part[1:-1]
                    if index_str.isdigit():
                        parent = parent[base_name][int(index_str)]
                    else:
                        parent = parent[base_name]
                else:
                    if part not in parent:
                        raise ValueError(f"Variable '{part}' not found")
                    parent = parent[part]
            
            last_part = parts[-1]
            
            # Handle array assignment
            if "[" in last_part and "]" in last_part:
                base_name = last_part[:last_part.index('[')]
                index_part = last_part[last_part.index('['):]
                
                index_str = index_part[1:-1]
                if index_str.isdigit():
                    parent[base_name][int(index_str)] = value
                else:
                    parent[base_name] = value
            else:
                parent[last_part] = value
            
            # Update variable state
            var_state = self.inspect_variable(path)
            var_state.value = value
            var_state.last_modified = time.time()
            
            self.log_event(f"Variable modified: {path} = {value}",
                          DebugLevel.INFO.value,
                          {"path": path, "new_value": value, "type": str(type(value).__name__)})
        
        except Exception as e:
            self.log_event(f"Error modifying variable {path}: {e}",
                          DebugLevel.ERROR.value,
                          {"path": path, "error": str(e)})
    
    def watch_variable(self, path: str):
        """Start watching a variable.
        
        Args:
            path: Variable path to watch
        """
        try:
            var_state = self.inspect_variable(path)
            self.watch_variables[path] = var_state
            self.log_event(f"Now watching variable: {path}",
                          DebugLevel.INFO.value,
                          {"path": path, "initial_value": var_state.value})
        except Exception as e:
            self.log_event(f"Error watching variable {path}: {e}",
                          DebugLevel.ERROR.value)
    
    def unwatch_variable(self, path: str):
        """Stop watching a variable.
        
        Args:
            path: Variable path to unwatch
        """
        if path in self.watch_variables:
            del self.watch_variables[path]
            self.log_event(f"Stopped watching variable: {path}",
                          DebugLevel.INFO.value)
        else:
            self.log_event(f"Variable not being watched: {path}",
                          DebugLevel.WARNING.value)
    
    def start_memory_monitor(self, interval: int = 1000):
        """Start memory monitoring.
        
        Args:
            interval: Monitoring interval in milliseconds
        """
        self.monitoring_interval = interval
        
        def monitor():
            while self.monitoring_active:
                try:
                    process = psutil.Process()
                    
                    # Get system memory info
                    memory = psutil.virtual_memory()
                    disk_io = process.io_counters()
                    net_io = process.net_io_counters()
                    
                    snapshot = MemorySnapshot(
                        timestamp=time.time(),
                        total_memory=memory.total,
                        available_memory=memory.available,
                        used_memory=memory.used,
                        memory_percent=memory.percent,
                        process_memory=process.memory_info().rss,
                        thread_count=process.num_threads(),
                        cpu_percent=process.cpu_percent(),
                        disk_io={"read_bytes": disk_io.read_bytes, "write_bytes": disk_io.write_bytes},
                        network_io={"bytes_sent": net_io.bytes_sent, "bytes_recv": net_io.bytes_recv}
                    )
                    
                    self.memory_snapshots.append(snapshot)
                    
                    self.log_event(f"Memory: {memory.percent}% used, {process.memory_info().rss / 1024 / 1024:.1f} MB",
                                  DebugLevel.INFO.value,
                                  {"memory_percent": memory.percent, "process_memory_mb": process.memory_info().rss / 1024 / 1024})
                    
                    # Keep only last 1000 snapshots
                    if len(self.memory_snapshots) > 1000:
                        self.memory_snapshots.pop(0)
                    
                    time.sleep(interval / 1000.0)
                
                except Exception as e:
                    self.log_event(f"Memory monitoring error: {e}", DebugLevel.ERROR.value)
                    time.sleep(interval / 1000.0)
        
        self.monitoring_active = True
        monitor_thread = threading.Thread(target=monitor, daemon=True)
        monitor_thread.start()
        
        self.log_event(f"Memory monitoring started with interval {interval}ms",
                      DebugLevel.INFO.value)
    
    def stop_memory_monitor(self):
        """Stop memory monitoring."""
        self.monitoring_active = False
        self.log_event("Memory monitoring stopped", DebugLevel.INFO.value)
    
    def start_performance_monitor(self):
        """Start performance monitoring."""
        
        def monitor():
            while self.monitoring_active:
                try:
                    # Collect performance metrics
                    process = psutil.Process()
                    
                    metrics = {
                        "timestamp": time.time(),
                        "cpu_percent": process.cpu_percent(),
                        "memory_info": process.memory_info(),
                        "thread_count": process.num_threads(),
                        "open_files": len(process.open_files()),
                        "connections": len(process.connections())
                    }
                    
                    self.performance_metrics.append(metrics)
                    
                    # Keep only last 1000 metrics
                    if len(self.performance_metrics) > 1000:
                        self.performance_metrics.pop(0)
                    
                    time.sleep(self.monitoring_interval / 1000.0)
                
                except Exception as e:
                    self.log_event(f"Performance monitoring error: {e}", DebugLevel.ERROR.value)
                    time.sleep(self.monitoring_interval / 1000.0)
        
        performance_thread = threading.Thread(target=monitor, daemon=True)
        performance_thread.start()
        
        self.log_event("Performance monitoring started", DebugLevel.INFO.value)
    
    def get_stack_trace(self) -> List[Dict[str, Any]]:
        """Get current stack trace.
        
        Returns:
            List of stack frames
        """
        frames = []
        
        for thread in threading.enumerate():
            if thread.ident != self.current_thread_id:
                continue
            
            stack = traceback.extract_stack()
            
            for frame in stack:
                frames.append({
                    "filename": frame.filename,
                    "lineno": frame.lineno,
                    "name": frame.name,
                    "line": frame.line
                })
        
        return frames
    
    def get_thread_status(self) -> List[Dict[str, Any]]:
        """Get status of all threads.
        
        Returns:
            List of thread information
        """
        threads = []
        
        for thread in threading.enumerate():
            try:
                thread_info = {
                    "id": thread.ident,
                    "name": thread.name,
                    "daemon": thread.daemon,
                    "alive": thread.is_alive(),
                    "native_id": getattr(thread, 'native_id', None)
                }
                threads.append(thread_info)
            except Exception:
                pass
        
        return threads
    
    def _count_references(self, path: str) -> int:
        """Count how many times a variable is referenced.
        
        Args:
            path: Variable path
            
        Returns:
            Reference count
        """
        count = 0
        for other_path in self.watch_variables:
            if other_path == path or other_path.endswith(f".{path}") or f".{path}" in other_path:
                count += 1
        return count
    
    def _signal_handler(self, signum, frame):
        """Handle signals for graceful shutdown."""
        self.log_event(f"Received signal {signum}, shutting down gracefully",
                      DebugLevel.WARNING.value)
        self.stop_recording()
        self.stop_memory_monitor()
        sys.exit(0)
    
    def start_recording(self, log_file: str = None):
        """Start recording debug session.
        
        Args:
            log_file: Log file path (auto-generated if None)
        """
        if self.recording_active:
            self.log_event("Recording already active", DebugLevel.WARNING.value)
            return
        
        if log_file is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            log_file = f"debug_session_{timestamp}.log"
        
        self.log_file = open(log_file, 'w')
        self.recording_active = True
        
        # Write header
        self.log_file.write(f"Debug Session started at {datetime.now().isoformat()}\n")
        self.log_file.write(f"Debug Console version: 1.0\n")
        self.log_file.write(f"Host: {self.host}, Port: {self.port}\n")
        self.log_file.write("=" * 80 + "\n")
        
        self.log_event(f"Recording started: {log_file}", DebugLevel.INFO.value)
    
    def stop_recording(self):
        """Stop recording debug session."""
        if self.recording_active and self.log_file:
            self.log_file.close()
            self.recording_active = False
            self.log_file = None
            
            self.log_event("Recording stopped", DebugLevel.INFO.value)
    
    def _complete_commands(self, text, state):
        """Command completion for readline."""
        commands = [
            'help', 'quit', 'exit', 'connect', 'disconnect',
            'breakpoints', 'add-bp', 'remove-bp', 'list-bp',
            'step', 'continue', 'pause', 'resume', 'single',
            'variables', 'watch', 'unwatch', 'inspect',
            'modify', 'context', 'clear-context',
            'monitor', 'unmonitor', 'memory', 'performance',
            'threads', 'stack', 'trace', 'log', 'events',
            'record', 'stop-recording', 'import', 'export',
            'run-commands', 'save-history', 'load-history',
            'status', 'stats', 'clear', 'reset'
        ]
        
        matches = [cmd for cmd in commands if cmd.startswith(text)]
        
        if state < len(matches):
            return matches[state]
        else:
            return None
    
    def run_command(self, command: str) -> str:
        """Run a debug command.
        
        Args:
            command: Command string
            
        Returns:
            Command output
        """
        # Add to history
        self.command_history.append(command)
        if len(self.command_history) > self.max_history:
            self.command_history.pop(0)
        
        # Parse and execute command
        parts = command.strip().split()
        if not parts:
            return ""
        
        cmd = parts[0].lower()
        args = parts[1:] if len(parts) > 1 else []
        
        try:
            if cmd == "help":
                return self._help_command(args)
            
            elif cmd == "quit" or cmd == "exit":
                self.log_event("Debug session ended by user", DebugLevel.INFO.value)
                self.stop_recording()
                self.stop_memory_monitor()
                return "Debug session ended"
            
            elif cmd == "connect":
                if args:
                    host_port = args[0].split(':')
                    if len(host_port) == 2:
                        return self._connect_command(host_port[0], int(host_port[1]))
                    else:
                        return f"Invalid connection format: {args[0]}"
                return self._connect_command()
            
            elif cmd == "disconnect":
                return self._disconnect_command()
            
            elif cmd == "breakpoints":
                return self._breakpoints_command(args)
            
            elif cmd == "add-bp":
                if args:
                    condition = ' '.join(args)
                    bp_id = self.add_breakpoint(condition)
                    return f"Breakpoint added: {bp_id}"
                return "Usage: add-bp <condition>"
            
            elif cmd == "remove-bp":
                if args and args[0] in self.breakpoints:
                    self.remove_breakpoint(args[0])
                    return f"Breakpoint removed: {args[0]}"
                return "Breakpoint not found"
            
            elif cmd == "list-bp":
                breakpoints = self.list_breakpoints()
                if not breakpoints:
                    return "No breakpoints set"
                
                output = "Breakpoints:\n"
                for bp in breakpoints:
                    output += f"  {bp['id']}: {bp['type']} - {bp['condition']} (active: {bp['active']}, hits: {bp['hit_count']})\n"
                
                return output
            
            elif cmd == "step":
                return self._step_command()
            
            elif cmd == "continue" or cmd == "resume":
                return self._continue_command()
            
            elif cmd == "pause":
                return self._pause_command()
            
            elif cmd == "single":
                self.single_step_mode = True
                return "Single-step mode enabled"
            
            elif cmd == "variables":
                return self._variables_command(args)
            
            elif cmd == "watch":
                if args:
                    path = args[0]
                    self.watch_variable(path)
                    return f"Now watching variable: {path}"
                return "Usage: watch <path>"
            
            elif cmd == "unwatch":
                if args:
                    path = args[0]
                    self.unwatch_variable(path)
                    return f"Stopped watching variable: {path}"
                return "Usage: unwatch <path>"
            
            elif cmd == "inspect":
                if args:
                    path = args[0]
                    var_state = self.inspect_variable(path)
                    return f"Variable {path}: {var_state.value} ({var_state.type})"
                return "Usage: inspect <path>"
            
            elif cmd == "modify":
                if len(args) >= 2:
                    path = args[0]
                    value = ' '.join(args[1:])
                    self.modify_variable(path, value)
                    return f"Variable {path} modified"
                return "Usage: modify <path> <value>"
            
            elif cmd == "context":
                if args:
                    self.debug_context[args[0]] = ' '.join(args[1:])
                    return f"Context variable '{args[0]}' set"
                return "Usage: context <name> <value>"
            
            elif cmd == "clear-context":
                self.debug_context.clear()
                return "Context cleared"
            
            elif cmd == "monitor":
                if args:
                    if args[0] == "memory":
                        interval = int(args[1]) if len(args) > 1 else 1000
                        self.start_memory_monitor(interval)
                        return f"Memory monitoring started with interval {interval}ms"
                    elif args[0] == "performance":
                        self.start_performance_monitor()
                        return "Performance monitoring started"
                return "Usage: monitor <memory|performance> [interval]"
            
            elif cmd == "unmonitor":
                if args:
                    if args[0] == "memory":
                        self.stop_memory_monitor()
                        return "Memory monitoring stopped"
                return "Usage: unmonitor <memory|performance>"
            
            elif cmd == "memory":
                return self._memory_command()
            
            elif cmd == "performance":
                return self._performance_command()
            
            elif cmd == "threads":
                return self._threads_command()
            
            elif cmd == "stack":
                return self._stack_command()
            
            elif cmd == "trace":
                return self._trace_command()
            
            elif cmd == "log":
                if args:
                    return self._log_command(args)
                return "Usage: log <message> [level]"
            
            elif cmd == "events":
                return self._events_command()
            
            elif cmd == "record":
                if args:
                    self.start_recording(args[0])
                    return f"Recording started to {args[0]}"
                else:
                    self.start_recording()
                    return "Recording started"
            
            elif cmd == "stop-recording":
                self.stop_recording()
                return "Recording stopped"
            
            elif cmd == "run-commands":
                if args:
                    return self._run_commands_command(args)
                return "Usage: run-commands <file>"
            
            elif cmd == "save-history":
                filename = args[0] if args else "debug_history.txt"
                return self._save_history_command(filename)
            
            elif cmd == "load-history":
                filename = args[0] if args else "debug_history.txt"
                return self._load_history_command(filename)
            
            elif cmd == "status":
                return self._status_command()
            
            elif cmd == "stats":
                return self._stats_command()
            
            elif cmd == "clear":
                return self._clear_command()
            
            elif cmd == "reset":
                return self._reset_command()
            
            else:
                return f"Unknown command: {cmd}"
        
        except Exception as e:
            error_msg = f"Error executing command '{cmd}': {str(e)}\n{traceback.format_exc()}"
            self.log_event(error_msg, DebugLevel.ERROR.value)
            return f"Error: {str(e)}"
    
    def _help_command(self, args):
        """Help command."""
        help_text = """Available commands:

Breakpoints:
  breakpoints          List all breakpoints
  add-bp <condition>   Add a breakpoint with condition
  remove-bp <id>       Remove breakpoint by ID
  list-bp              Alias for breakpoints

Execution Control:
  step                 Execute single step
  continue/resume      Continue execution
  pause                Pause execution
  single               Enable single-step mode

Variables:
  variables [filter]   List variables (with optional filter)
  watch <path>         Start watching variable
  unwatch <path>       Stop watching variable
  inspect <path>       Inspect variable value and type
  modify <path> <value> Modify variable value

Context:
  context <name> <value> Set context variable
  clear-context        Clear all context variables

Monitoring:
  monitor memory [interval] Start memory monitoring
  monitor performance    Start performance monitoring
  unmonitor memory      Stop memory monitoring
  memory                Show memory status
  performance           Show performance metrics

System:
  threads              Show thread status
  stack                Show current stack trace
  trace                Show event trace
  log <message> [level] Log message
  events                Show recent events
  record [file]        Start recording session
  stop-recording        Stop recording

File Operations:
  run-commands <file>   Run commands from file
  save-history <file>   Save command history
  load-history <file>   Load command history

Status:
  status               Show current status
  stats                Show statistics
  clear                Clear console
  reset                Reset debug console
  help                 Show this help
  quit/exit            Quit debug console
"""
        return help_text
    
    def _connect_command(self, host=None, port=None):
        """Connect to debug server."""
        if host is None or port is None:
            self.log_event(f"Connecting to {self.host}:{self.port}", DebugLevel.INFO.value)
            return f"Connecting to {self.host}:{self.port}"
        
        try:
            self.log_event(f"Connecting to {host}:{port}", DebugLevel.INFO.value)
            return f"Connected to {host}:{port}"
        except Exception as e:
            return f"Connection failed: {e}"
    
    def _disconnect_command(self):
        """Disconnect from debug server."""
        self.log_event("Disconnecting from debug server", DebugLevel.INFO.value)
        return "Disconnected from debug server"
    
    def _breakpoints_command(self, args):
        """Breakpoints command."""
        return self._help_command(["breakpoints"])
    
    def _step_command(self):
        """Step command."""
        if self.state != DebugState.PAUSED:
            return "Not paused, cannot step"
        
        self.step_count += 1
        self.log_event(f"Step executed (step #{self.step_count})", DebugLevel.INFO.value,
                      {"step_count": self.step_count})
        
        # Check breakpoints
        triggered = self.check_breakpoints(self.debug_context)
        if triggered:
            return f"Stepped, triggered breakpoints: {triggered}"
        
        return f"Step executed (step #{self.step_count})"
    
    def _continue_command(self):
        """Continue command."""
        if self.state != DebugState.PAUSED:
            return "Already running"
        
        self.state = DebugState.RUNNING
        self.log_event("Continuing execution", DebugLevel.INFO.value)
        return "Continuing execution"
    
    def _pause_command(self):
        """Pause command."""
        if self.state == DebugState.PAUSED:
            return "Already paused"
        
        self.state = DebugState.PAUSED
        self.log_event("Execution paused", DebugLevel.INFO.value)
        return "Execution paused"
    
    def _variables_command(self, args):
        """Variables command."""
        filter_text = args[0] if args else ""
        
        variables = []
        for path, var_state in self.watch_variables.items():
            if not filter_text or filter_text in path or str(var_state.value) in filter_text:
                variables.append({
                    "path": path,
                    "value": var_state.value,
                    "type": var_state.type,
                    "last_modified": var_state.last_modified,
                    "references": var_state.references
                })
        
        if not variables:
            return "No variables found"
        
        output = "Variables:\n"
        for var in variables:
            output += f"  {var['path']}: {var['value']} ({var['type']}) [refs: {var['references']}]\n"
        
        return output
    
    def _memory_command(self):
        """Memory command."""
        if not self.memory_snapshots:
            return "No memory snapshots available"
        
        latest = self.memory_snapshots[-1]
        output = f"Memory Status:\n"
        output += f"  Memory: {latest.memory_percent}% used\n"
        output += f"  Available: {latest.available_memory / 1024 / 1024 / 1024:.1f} GB\n"
        output += f"  Used: {latest.used_memory / 1024 / 1024 / 1024:.1f} GB\n"
        output += f"  Process Memory: {latest.process_memory / 1024 / 1024:.1f} MB\n"
        output += f"  Thread Count: {latest.thread_count}\n"
        output += f"  CPU Percent: {latest.cpu_percent}%\n"
        
        return output
    
    def _performance_command(self):
        """Performance command."""
        if not self.performance_metrics:
            return "No performance metrics available"
        
        latest = self.performance_metrics[-1]
        output = f"Performance Status:\n"
        output += f"  CPU Percent: {latest['cpu_percent']}%\n"
        output += f"  Thread Count: {latest['thread_count']}\n"
        output += f"  Open Files: {latest['open_files']}\n"
        output += f"  Connections: {latest['connections']}\n"
        
        return output
    
    def _threads_command(self):
        """Threads command."""
        threads = self.get_thread_status()
        if not threads:
            return "No threads found"
        
        output = f"Thread Status ({len(threads)} threads):\n"
        for thread in threads:
            output += f"  {thread['name']} (ID: {thread['id']}, Alive: {thread['alive']}, Daemon: {thread['daemon']})\n"
        
        return output
    
    def _stack_command(self):
        """Stack command."""
        frames = self.get_stack_trace()
        if not frames:
            return "No stack trace available"
        
        output = f"Stack Trace:\n"
        for frame in frames[:20]:  # Limit to first 20 frames
            output += f"  {frame['filename']}:{frame['lineno']} in {frame['name']}\n"
            if frame['line']:
                output += f"    {frame['line']}\n"
        
        return output
    
    def _trace_command(self):
        """Trace command."""
        output = f"Recent Events ({len(self.debug_events)} events):\n"
        for event in self.debug_events[-20:]:  # Last 20 events
            timestamp_str = datetime.fromtimestamp(event.timestamp).strftime('%H:%M:%S.%f')[:-3]
            output += f"  [{timestamp_str}] {event.level.value}: {event.message} ({event.source})\n"
        
        return output
    
    def _log_command(self, args):
        """Log command."""
        if len(args) < 1:
            return "Usage: log <message> [level]"
        
        message = args[0]
        level = args[1] if len(args) > 1 else DebugLevel.INFO.value
        
        self.log_event(message, level)
        return f"Logged: {level} - {message}"
    
    def _events_command(self):
        """Events command."""
        return self._trace_command()
    
    def _run_commands_command(self, args):
        """Run commands from file."""
        if not args:
            return "Usage: run-commands <file>"
        
        file_path = args[0]
        try:
            with open(file_path, 'r') as f:
                commands = [line.strip() for line in f if line.strip() and not line.startswith('#')]
            
            output = f"Running {len(commands)} commands from {file_path}:\n"
            
            for command in commands:
                result = self.run_command(command)
                output += f"  {command}: {result}\n"
            
            return output
        
        except Exception as e:
            return f"Error running commands: {e}"
    
    def _save_history_command(self, filename):
        """Save command history."""
        try:
            with open(filename, 'w') as f:
                for command in self.command_history:
                    f.write(f"{command}\n")
            return f"History saved to {filename}"
        except Exception as e:
            return f"Error saving history: {e}"
    
    def _load_history_command(self, filename):
        """Load command history."""
        try:
            with open(filename, 'r') as f:
                commands = [line.strip() for line in f if line.strip()]
            
            self.command_history.extend(commands)
            if len(self.command_history) > self.max_history:
                self.command_history = self.command_history[-self.max_history:]
            
            return f"Loaded {len(commands)} commands from {filename}"
        except Exception as e:
            return f"Error loading history: {e}"
    
    def _status_command(self):
        """Status command."""
        output = f"Debug Console Status:\n"
        output += f"  State: {self.state.value}\n"
        output += f"  Host: {self.host}\n"
        output += f"  Port: {self.port}\n"
        output += f"  Breakpoints: {len(self.breakpoints)}\n"
        output += f"  Watch Variables: {len(self.watch_variables)}\n"
        output += f"  Command History: {len(self.command_history)}\n"
        output += f"  Debug Events: {len(self.debug_events)}\n"
        output += f"  Memory Snapshots: {len(self.memory_snapshots)}\n"
        output += f"  Single-Step Mode: {self.single_step_mode}\n"
        output += f"  Monitoring Active: {self.monitoring_active}\n"
        
        return output
    
    def _stats_command(self):
        """Statistics command."""
        output = f"Debug Console Statistics:\n"
        output += f"  Total commands executed: {len(self.command_history)}\n"
        output += f"  Total breakpoints triggered: {sum(bp.hit_count for bp in self.breakpoints.values())}\n"
        output += f"  Current watch variables: {len(self.watch_variables)}\n"
        output += f"  Memory snapshots: {len(self.memory_snapshots)}\n"
        output += f"  Performance metrics: {len(self.performance_metrics)}\n"
        output += f"  Recent events: {len(self.debug_events)}\n"
        
        if self.memory_snapshots:
            latest = self.memory_snapshots[-1]
            output += f"  Latest memory usage: {latest.memory_percent}%\n"
        
        return output
    
    def _clear_command(self):
        """Clear command."""
        # Clear console (simulated)
        return "Console cleared"
    
    def _reset_command(self):
        """Reset command."""
        self.log_event("Debug console reset", DebugLevel.INFO.value)
        
        # Reset state
        self.state = DebugState.IDLE
        self.single_step_mode = False
        self.step_count = 0
        self.breakpoints.clear()
        self.watch_variables.clear()
        self.variable_history.clear()
        self.debug_events.clear()
        self.memory_snapshots.clear()
        self.performance_metrics.clear()
        self.command_history.clear()
        self.debug_context.clear()
        self.stop_memory_monitor()
        self.stop_recording()
        
        return "Debug console reset"


def main():
    """Main entry point for DebugConsole."""
    parser = argparse.ArgumentParser(description="NLM Debug Console Tool")
    parser.add_argument("--host", default="localhost", help="Host for debug server")
    parser.add_argument("--port", type=int, default=8888, help="Port for debug server")
    parser.add_argument("--input", "-i", help="Input simulation data file (JSON)")
    parser.add_argument("--commands", nargs="+", help="Commands to run automatically")
    parser.add_argument("--monitor", choices=["memory", "performance"], help="Start monitoring")
    parser.add_argument("--interval", type=int, default=1000, help="Monitoring interval (ms)")
    parser.add_argument("--interactive", action="store_true", help="Enable interactive mode")
    parser.add_argument("--log-file", help="Log file for recording session")
    
    args = parser.parse_args()
    
    # Initialize debug console
    debug = DebugConsole(args.host, args.port)
    
    # Load simulation data if provided
    if args.input:
        try:
            with open(args.input, 'r') as f:
                debug.simulation_data = json.load(f)
            debug.log_event(f"Loaded simulation data from {args.input}", DebugLevel.INFO.value)
        except Exception as e:
            debug.log_event(f"Error loading simulation data: {e}", DebugLevel.ERROR.value)
    
    # Start monitoring if requested
    if args.monitor == "memory":
        debug.start_memory_monitor(args.interval)
    
    # Run commands if provided
    if args.commands:
        output = f"Running {len(args.commands)} commands:\n"
        for cmd in args.commands:
            result = debug.run_command(cmd)
            output += f"  {cmd}: {result}\n"
        print(output)
        return
    
    # Interactive mode
    if args.interactive:
        print("NLM Debug Console - Interactive Mode")
        print("Type 'help' for available commands or 'quit' to exit.")
        print("=" * 60)
        
        # Start recording if log file specified
        if args.log_file:
            debug.start_recording(args.log_file)
        
        try:
            while True:
                try:
                    command = input("> ").strip()
                    
                    if not command:
                        continue
                    
                    result = debug.run_command(command)
                    print(result)
                    
                    if command.lower() in ['quit', 'exit']:
                        break
                
                except KeyboardInterrupt:
                    print("\nInterrupted. Type 'quit' to exit.")
                
                except Exception as e:
                    print(f"Error: {e}")
        
        except Exception as e:
            debug.log_event(f"Interactive mode error: {e}", DebugLevel.ERROR.value)
            print(f"Error: {e}")
    
    else:
        # Non-interactive mode - just show status
        print("Debug Console initialized.")
        print(f"Host: {debug.host}, Port: {debug.port}")
        print(f"Simulation data loaded: {len(debug.simulation_data) > 0}")
        
        # Start server if available
        if ZMQ_AVAILABLE:
            try:
                debug.log_event("Debug server would start here if ZMQ available", DebugLevel.INFO.value)
            except Exception:
                pass
    
    # Cleanup
    debug.stop_memory_monitor()
    debug.stop_recording()
    print("Debug Console shutdown complete.")


if __name__ == "__main__":
    main()