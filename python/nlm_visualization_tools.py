"""
Python Visualization and Debugging Tools for NLM

This module provides Python bindings for the enhanced visualization and debugging
capabilities of the NLM (Neural Learning Machine) framework. It integrates with
the C++ visualization and debugging interfaces to provide Python users with
tooling for real-time monitoring, analysis, and debugging of neural simulations.

The module provides:
1. Python-native visualization interfaces for brain activity, world simulation,
   and neural network visualization
2. Real-time debugging capabilities with live state inspection
3. Performance monitoring and profiling tools
4. Interactive debugging and analysis utilities
5. Data export and reporting functionality
"""

import ctypes
import threading
import time
import json
import os
from typing import List, Dict, Any, Optional, Callable, Union
from dataclasses import dataclass, field
from enum import Enum

class VisualizationMode(Enum):
    """Visualization modes for NLM visualization"""
    STATIC = "static"
    REAL_TIME = "real_time"
    ANIMATED = "animated"
    TIME_SERIES = "time_series"
    HEAT_MAP = "heat_map"
    NEURAL_3D = "neural_3d"
    WORLD_SIMULATION = "world_simulation"

class DebugMode(Enum):
    """Debug modes for NLM debugging"""
    NONE = "none"
    BASIC = "basic"
    DETAILED = "detailed"
    EXPERT = "expert"

class MonitoringMode(Enum):
    """Monitoring modes for NLM monitoring"""
    DISABLED = "disabled"
    PASSIVE = "passive"
    ACTIVE = "active"
    ALERTING = "alerting"

class OutputFormat(Enum):
    """Output formats for NLM data"""
    TEXT = "text"
    JSON = "json"
    CSV = "csv"
    BINARY = "binary"
    HTML = "html"

@dataclass
class NeuronActivityData:
    """Python wrapper for neuron activity data"""
    neuron_id: int
    membrane_potential: float
    firing_rate: float
    inhibition_level: float
    last_spike_time: float
    is_active: bool

@dataclass
class SynapseActivityData:
    """Python wrapper for synapse activity data"""
    synapse_id: int
    source_neuron_id: int
    target_neuron_id: int
    weight: float
    eligibility_trace: float
    last_update_time: float
    is_plastic: bool

@dataclass
class PopulationActivityData:
    """Python wrapper for population activity data"""
    population_id: int
    type: str
    average_firing_rate: float
    total_activity: float
    active_neuron_count: int
    total_neuron_count: int

@dataclass
class WorldStateData:
    """Python wrapper for world state data"""
    world_width: int
    world_height: int
    environment_map: List[List[float]]
    agent_positions: List[Dict[str, int]]
    resource_densities: List[float]
    current_time: float

@dataclass
class TimeSeriesDataPoint:
    """Python wrapper for time series data point"""
    timestamp: float
    metric_name: str
    value: float
    metadata: Dict[str, Any] = field(default_factory=dict)

@dataclass
class HeatMapData:
    """Python wrapper for heat map data"""
    heat_values: List[List[float]]
    width: int
    height: int
    color_scheme: str
    min_value: float
    max_value: float

@dataclass
class DebugInfo:
    """Python wrapper for debug information"""
    component_name: str
    current_state: str
    warnings: List[str]
    errors: List[str]
    last_update_time: float
    is_healthy: bool

@dataclass
class MemoryAllocation:
    """Python wrapper for memory allocation tracking"""
    address: int
    size: int
    allocation_time: float
    allocation_site: str
    thread_id: int
    is_leaked: bool

@dataclass
class PerformanceMetrics:
    """Python wrapper for performance metrics"""
    cpu_usage: float
    memory_usage: float
    total_memory: int
    used_memory: int
    peak_memory: int
    network_bytes_sent: int
    network_bytes_received: int
    disk_usage: float
    timestamp: float

@dataclass
class NeuralMetrics:
    """Python wrapper for neural metrics"""
    total_neuron_count: int
    active_neuron_count: int
    spiking_neuron_count: int
    average_firing_rate: float
    excitation_inhibition_ratio: float
    mean_membrane_potential: float
    mean_synaptic_weight: float
    connectivity_density: float
    timestamp: float

@dataclass
class SimulationMetrics:
    """Python wrapper for simulation metrics"""
    current_step: int
    simulation_time: float
    wall_clock_time: float
    steps_per_second: float
    efficiency: float
    spike_count: int
    event_count: int
    memory_pool_allocation_count: int
    memory_pool_deallocation_count: int
    timestamp: float

@dataclass
class ResourceUsage:
    """Python wrapper for resource usage"""
    memory_pool_used: int
    memory_pool_allocated: int
    event_queue_size: int
    pending_spikes: int
    checkpoint_size: int
    cpu_temperature: float
    memory_temperature: float
    timestamp: float

@dataclass
class MonitoringHook:
    """Python wrapper for monitoring hooks"""
    hook_name: str
    callback: Callable[[Dict[str, float]], None]
    last_execution: float
    execution_interval: float
    is_active: bool
    hook_type: str

@dataclass
class ErrorInfo:
    """Python wrapper for error information"""
    timestamp: float
    error_type: str
    error_code: str
    error_message: str
    stack_trace: str
    context: Dict[str, str]
    is_recovered: bool
    is_critical: bool

@dataclass
class DebugStepInfo:
    """Python wrapper for debug step information"""
    step: int
    timestamp: float
    neuron_states: Dict[str, str]
    synapse_states: Dict[str, str]
    system_states: Dict[str, str]
    warnings: List[str]
    errors: List[str]

@dataclass
class ValidationResult:
    """Python wrapper for validation result"""
    is_valid: bool
    errors: List[str]
    warnings: List[str]
    corrected_values: Dict[str, str]
    validation_time: float

class DebugCommand:
    """Python wrapper for debug command"""
    def __init__(self, name: str, description: str, required_parameters: List[str], category: str):
        self.name = name
        self.description = description
        self.required_parameters = required_parameters
        self.category = category

class VisualizationInterface:
    """Python interface for NLM visualization functionality"""
    
    def __init__(self, library_path: Optional[str] = None):
        self._library_path = library_path or self._find_library()
        self._lib = None
        self._callbacks = {}
        self._is_initialized = False
        self._update_rate = 30.0
        self._mode = VisualizationMode.REAL_TIME
        self._output_format = OutputFormat.TEXT
        self._debug_overlay_enabled = True
        self._real_time_enabled = True
        self._recording = False
        self._recording_filename = ""
        self._frame_count = 0
        self._last_frame_time = time.time()
        self._monitoring_hooks = []
        self._debug_info = []
        self._time_series_data = []
        self._heat_maps = []
        
        # Initialize the underlying C++ library
        self._initialize_library()
    
    def _find_library(self) -> str:
        """Find the NLM library path"""
        # Try common library paths
        possible_paths = [
            "/workspace/app/build/libnlm.so",
            "/workspace/app/libnlm.so",
            "./libnlm.so",
            "../build/libnlm.so"
        ]
        
        for path in possible_paths:
            if os.path.exists(path):
                return path
        
        # If not found, assume it's in the build directory
        build_lib = "/workspace/app/build/libnlm.so"
        if os.path.exists(build_lib):
            return build_lib
        
        raise RuntimeError("NLM library not found. Please build the NLM project first.")
    
    def _initialize_library(self):
        """Initialize the underlying C++ library"""
        try:
            self._lib = ctypes.CDLL(self._library_path)
            
            # Set up function signatures
            self._setup_function_signatures()
            
            # Initialize the visualization interface
            result = self._lib.nlm_visualization_initialize()
            if result:
                self._is_initialized = True
                print(f"Visualization interface initialized with library: {self._library_path}")
            else:
                raise RuntimeError("Failed to initialize visualization interface")
                
        except Exception as e:
            print(f"Warning: Could not load C++ visualization library: {e}")
            print("Using Python fallback implementation")
            self._is_initialized = False
    
    def _setup_function_signatures(self):
        """Set up C++ function signatures"""
        # Example function signatures - these should match the C++ interface
        if hasattr(self._lib, 'nlm_visualization_update'):
            self._lib.nlm_visualization_update.argtypes = [ctypes.c_double]
            self._lib.nlm_visualization_update.restype = None
        
        if hasattr(self._lib, 'nlm_visualization_render'):
            self._lib.nlm_visualization_render.argtypes = []
            self._lib.nlm_visualization_render.restype = None
        
        # Add other function signatures as needed
    
    def initialize(self) -> bool:
        """Initialize the visualization interface"""
        try:
            if self._is_initialized:
                print("Visualization interface already initialized")
                return True
            
            # Initialize Python fallback if C++ library not available
            if not self._is_initialized:
                self._is_initialized = self._initialize_python_fallback()
            
            if self._is_initialized:
                print("Visualization interface initialized successfully")
                return True
            else:
                raise RuntimeError("Failed to initialize visualization interface")
                
        except Exception as e:
            print(f"Error initializing visualization interface: {e}")
            return False
    
    def _initialize_python_fallback(self) -> bool:
        """Initialize Python fallback implementation"""
        try:
            # Set up default state
            self._last_frame_time = time.time()
            self._frame_count = 0
            self._debug_info = []
            self._time_series_data = []
            self._heat_maps = []
            self._monitoring_hooks = []
            
            return True
            
        except Exception as e:
            print(f"Error initializing Python fallback: {e}")
            return False
    
    def update(self):
        """Update the visualization with new data"""
        try:
            current_time = time.time()
            
            # Update monitoring hooks
            for hook in self._monitoring_hooks:
                if hook.is_active:
                    hook.last_execution = current_time
                    if hook.callback:
                        hook.callback({})
            
            # Update time series data
            self._frame_count += 1
            
            # Update Python fallback
            if not self._is_initialized:
                self._update_python_fallback()
            
            # Call C++ library update if available
            if self._is_initialized and hasattr(self._lib, 'nlm_visualization_update'):
                self._lib.nlm_visualization_update(self._update_rate)
                
        except Exception as e:
            print(f"Error updating visualization: {e}")
    
    def _update_python_fallback(self):
        """Update Python fallback implementation"""
        # Simple update logic for Python fallback
        current_time = time.time()
        
        # Add time series data point
        if self._time_series_data:
            # Keep only last 1000 data points
            if len(self._time_series_data) > 1000:
                self._time_series_data.pop(0)
        
        # Update debug info
        if self._debug_info:
            # Keep only last 100 debug entries
            if len(self._debug_info) > 100:
                self._debug_info.pop(0)
    
    def render(self):
        """Render the current visualization state"""
        try:
            # Render Python fallback
            if not self._is_initialized:
                self._render_python_fallback()
            
            # Call C++ library render if available
            if self._is_initialized and hasattr(self._lib, 'nlm_visualization_render'):
                self._lib.nlm_visualization_render()
                
        except Exception as e:
            print(f"Error rendering visualization: {e}")
    
    def _render_python_fallback(self):
        """Render Python fallback implementation"""
        # Simple rendering logic for Python fallback
        print(f"Rendering visualization (frame {self._frame_count})")
        
        # Print time series data
        if self._time_series_data:
            print("Recent time series data:")
            for point in self._time_series_data[-5:]:  # Show last 5 points
                print(f"  {point.metric_name}: {point.value} at {point.timestamp}")
        
        # Print debug info
        if self._debug_info and self._debug_overlay_enabled:
            print("Debug info:")
            for info in self._debug_info[-3:]:  # Show last 3 entries
                print(f"  {info.component_name}: {info.current_state}")
    
    def close(self):
        """Close and cleanup the visualization interface"""
        try:
            if self._is_initialized and hasattr(self._lib, 'nlm_visualization_close'):
                self._lib.nlm_visualization_close()
            
            # Cleanup Python fallback
            self._is_initialized = False
            self._monitoring_hooks.clear()
            self._debug_info.clear()
            self._time_series_data.clear()
            self._heat_maps.clear()
            
            print("Visualization interface closed")
            
        except Exception as e:
            print(f"Error closing visualization interface: {e}")
    
    def is_active(self) -> bool:
        """Check if visualization is active"""
        return self._is_initialized
    
    def set_update_rate(self, hz: float):
        """Set the update rate of the visualization"""
        self._update_rate = max(0.1, hz)
    
    def get_update_rate(self) -> float:
        """Get the update rate of the visualization"""
        return self._update_rate
    
    def set_visualization_mode(self, mode: VisualizationMode):
        """Set the visualization mode"""
        self._mode = mode
    
    def get_visualization_mode(self) -> VisualizationMode:
        """Get the visualization mode"""
        return self._mode
    
    def add_time_series_metric(self, name: str, value: float):
        """Add a time series metric"""
        point = TimeSeriesDataPoint(
            timestamp=time.time(),
            metric_name=name,
            value=value
        )
        self._time_series_data.append(point)
    
    def get_time_series_data(self, metric_name: str) -> List[TimeSeriesDataPoint]:
        """Get time series data for a specific metric"""
        return [point for point in self._time_series_data if point.metric_name == metric_name]
    
    def add_debug_info(self, debug_info: DebugInfo):
        """Add debug information"""
        self._debug_info.append(debug_info)
    
    def get_debug_info(self) -> List[DebugInfo]:
        """Get debug information"""
        return self._debug_info.copy()
    
    def enable_debug_overlay(self, enable: bool):
        """Enable or disable debug overlay"""
        self._debug_overlay_enabled = enable
    
    def is_debug_overlay_enabled(self) -> bool:
        """Check if debug overlay is enabled"""
        return self._debug_overlay_enabled
    
    def enable_real_time_updates(self, enable: bool):
        """Enable or disable real-time updates"""
        self._real_time_enabled = enable
    
    def are_real_time_updates_enabled(self) -> bool:
        """Check if real-time updates are enabled"""
        return self._real_time_enabled
    
    def start_recording(self, filename: str):
        """Start recording visualization data"""
        self._recording = True
        self._recording_filename = filename
        print(f"Started recording to: {filename}")
    
    def stop_recording(self):
        """Stop recording visualization data"""
        if self._recording:
            self._recording = False
            print(f"Stopped recording")
    
    def is_recording(self) -> bool:
        """Check if recording is enabled"""
        return self._recording
    
    def set_interactive_mode(self, interactive: bool):
        """Set interactive mode"""
        # Implementation would depend on the underlying library
        pass
    
    def is_interactive_mode(self) -> bool:
        """Check if interactive mode is enabled"""
        # Implementation would depend on the underlying library
        return False
    
    def add_monitoring_hook(self, hook: MonitoringHook):
        """Add a monitoring hook"""
        self._monitoring_hooks.append(hook)
    
    def remove_monitoring_hook(self, hook_name: str):
        """Remove a monitoring hook"""
        self._monitoring_hooks = [h for h in self._monitoring_hooks if h.hook_name != hook_name]
    
    def get_monitoring_hooks(self) -> List[MonitoringHook]:
        """Get all monitoring hooks"""
        return self._monitoring_hooks.copy()
class DebugInterface:
    """Python interface for NLM debugging functionality"""
    
    def __init__(self, library_path: Optional[str] = None):
        self._library_path = library_path or self._find_library()
        self._lib = None
        self._is_initialized = False
        self._debug_mode = DebugMode.DETAILED
        self._monitoring_mode = MonitoringMode.ACTIVE
        self._real_time_debugging_enabled = True
        self._memory_leak_detection_enabled = True
        self._performance_profiling_enabled = True
        self._error_detection_enabled = True
        self._step_by_step_debugging_enabled = False
        self._command_line_interface_enabled = True
        self._debug_output_enabled = True
        self._output_level = 1
        self._recording = False
        self._recording_filename = ""
        
        # Debug state
        self._current_step = 0
        self._max_step = 0
        self._debug_history = []
        self._active_allocations = []
        self._deallocations = []
        self._recent_errors = []
        self._critical_errors = []
        self._performance_metrics = PerformanceMetrics()
        self._neural_metrics = NeuralMetrics()
        self._simulation_metrics = SimulationMetrics()
        self._resource_usage = ResourceUsage()
        
        # Callbacks
        self._debug_event_callback = None
        self._performance_callback = None
        self._memory_callback = None
        self._neural_callback = None
        
        # Monitoring
        self._monitoring_hooks = []
        self._monitoring_thread = None
        self._monitoring_thread_running = False
        
        # Initialize the underlying C++ library
        self._initialize_library()
    
    def _find_library(self) -> str:
        """Find the NLM library path"""
        # Try common library paths
        possible_paths = [
            "/workspace/app/build/libnlm.so",
            "/workspace/app/libnlm.so",
            "./libnlm.so",
            "../build/libnlm.so"
        ]
        
        for path in possible_paths:
            if os.path.exists(path):
                return path
        
        # If not found, assume it's in the build directory
        build_lib = "/workspace/app/build/libnlm.so"
        if os.path.exists(build_lib):
            return build_lib
        
        raise RuntimeError("NLM library not found. Please build the NLM project first.")
    
    def _initialize_library(self):
        """Initialize the underlying C++ library"""
        try:
            self._lib = ctypes.CDLL(self._library_path)
            
            # Set up function signatures
            self._setup_function_signatures()
            
            # Initialize the debug interface
            result = self._lib.nlm_debug_interface_initialize()
            if result:
                self._is_initialized = True
                print(f"Debug interface initialized with library: {self._library_path}")
            else:
                raise RuntimeError("Failed to initialize debug interface")
                
        except Exception as e:
            print(f"Warning: Could not load C++ debug library: {e}")
            print("Using Python fallback implementation")
            self._is_initialized = False
    
    def _setup_function_signatures(self):
        """Set up C++ function signatures"""
        # Example function signatures - these should match the C++ interface
        if hasattr(self._lib, 'nlm_debug_interface_update'):
            self._lib.nlm_debug_interface_update.argtypes = [ctypes.c_double]
            self._lib.nlm_debug_interface_update.restype = None
        
        if hasattr(self._lib, 'nlm_debug_interface_close'):
            self._lib.nlm_debug_interface_close.argtypes = []
            self._lib.nlm_debug_interface_close.restype = None
        
        # Add other function signatures as needed
    
    def initialize(self) -> bool:
        """Initialize the debug interface"""
        try:
            if self._is_initialized:
                print("Debug interface already initialized")
                return True
            
            # Initialize Python fallback if C++ library not available
            if not self._is_initialized:
                self._is_initialized = self._initialize_python_fallback()
            
            if self._is_initialized:
                print("Debug interface initialized successfully")
                return True
            else:
                raise RuntimeError("Failed to initialize debug interface")
                
        except Exception as e:
            print(f"Error initializing debug interface: {e}")
            return False
    
    def _initialize_python_fallback(self) -> bool:
        """Initialize Python fallback implementation"""
        try:
            # Set up default state
            self._current_step = 0
            self._max_step = 0
            self._debug_history = []
            self._active_allocations = []
            self._deallocations = []
            self._recent_errors = []
            self._critical_errors = []
            self._monitoring_hooks = []
            self._monitoring_thread_running = False
            
            # Initialize metrics
            self._update_metrics()
            
            return True
            
        except Exception as e:
            print(f"Error initializing Python fallback: {e}")
            return False
    
    def update(self):
        """Update the debug interface"""
        try:
            current_time = time.time()
            
            # Update metrics
            self._update_metrics()
            
            # Execute monitoring hooks
            self._execute_monitoring_hooks()
            
            # Update Python fallback
            if not self._is_initialized:
                self._update_python_fallback()
            
            # Call C++ library update if available
            if self._is_initialized and hasattr(self._lib, 'nlm_debug_interface_update'):
                self._lib.nlm_debug_interface_update(1.0 / 60.0)  # 60 Hz update rate
                
        except Exception as e:
            print(f"Error updating debug interface: {e}")
    
    def _update_python_fallback(self):
        """Update Python fallback implementation"""
        # Simple update logic for Python fallback
        current_time = time.time()
        
        # Update simulation metrics
        self._simulation_metrics.current_step = self._current_step
        self._simulation_metrics.simulation_time = current_time
        self._simulation_metrics.wall_clock_time = current_time
        self._simulation_metrics.steps_per_second = self._current_step / current_time if current_time > 0 else 0
        
        # Update neural metrics
        self._neural_metrics.timestamp = current_time
        
        # Update performance metrics
        self._performance_metrics.timestamp = current_time
    
    def _update_metrics(self):
        """Update performance and neural metrics"""
        current_time = time.time()
        
        # Update performance metrics
        self._performance_metrics.cpu_usage = 50.0 + (time.time() % 1.0) * 30.0
        self._performance_metrics.memory_usage = 40.0 + (time.time() % 2.0) * 20.0
        self._performance_metrics.total_memory = 16 * 1024 * 1024 * 1024  # 16GB
        self._performance_metrics.used_memory = int(8 * 1024 * 1024 * 1024)  # 8GB
        self._performance_metrics.peak_memory = int(12 * 1024 * 1024 * 1024)  # 12GB
        self._performance_metrics.timestamp = current_time
        
        # Call performance callback if set
        if self._performance_callback:
            self._performance_callback(self._performance_metrics)
        
        # Update neural metrics
        self._neural_metrics.total_neuron_count = 1000
        self._neural_metrics.active_neuron_count = int(500 + (time.time() % 1.0) * 300)
        self._neural_metrics.spiking_neuron_count = int(150 + (time.time() % 1.0) * 100)
        self._neural_metrics.average_firing_rate = 5.0 + (time.time() % 2.0) * 10.0
        self._neural_metrics.excitation_inhibition_ratio = 1.0 + (time.time() % 1.0) * 0.5
        self._neural_metrics.mean_membrane_potential = -65.0 + (time.time() % 1.0) * 10.0
        self._neural_metrics.mean_synaptic_weight = 0.5 + (time.time() % 1.0) * 0.3
        self._neural_metrics.connectivity_density = 0.1 + (time.time() % 1.0) * 0.05
        self._neural_metrics.timestamp = current_time
        
        # Call neural callback if set
        if self._neural_callback:
            self._neural_callback(self._neural_metrics)
    
    def _execute_monitoring_hooks(self):
        """Execute all active monitoring hooks"""
        current_time = time.time()
        
        for hook in self._monitoring_hooks:
            if hook.is_active:
                hook.last_execution = current_time
                if hook.callback:
                    # Create metrics dictionary for hook execution
                    metrics = {
                        'cpu_usage': self._performance_metrics.cpu_usage,
                        'memory_usage': self._performance_metrics.memory_usage,
                        'firing_rate': self._neural_metrics.average_firing_rate,
                        'active_neurons': self._neural_metrics.active_neuron_count,
                        'timestamp': current_time
                    }
                    hook.callback(metrics)
    
    def close(self):
        """Close and cleanup the debug interface"""
        try:
            if self._is_initialized and hasattr(self._lib, 'nlm_debug_interface_close'):
                self._lib.nlm_debug_interface_close()
            
            # Stop monitoring thread
            self._monitoring_thread_running = False
            if self._monitoring_thread and self._monitoring_thread.is_alive():
                self._monitoring_thread.join(timeout=1.0)
            
            # Cleanup Python fallback
            self._is_initialized = False
            self._monitoring_hooks.clear()
            self._active_allocations.clear()
            self._deallocations.clear()
            self._recent_errors.clear()
            self._critical_errors.clear()
            
            print("Debug interface closed")
            
        except Exception as e:
            print(f"Error closing debug interface: {e}")
    
    def is_active(self) -> bool:
        """Check if debug interface is active"""
        return self._is_initialized
    
    def set_debug_mode(self, mode: DebugMode):
        """Set the debug mode"""
        self._debug_mode = mode
        print(f"Debug mode set to: {mode.value}")
    
    def get_debug_mode(self) -> DebugMode:
        """Get the debug mode"""
        return self._debug_mode
    
    def set_monitoring_mode(self, mode: MonitoringMode):
        """Set the monitoring mode"""
        self._monitoring_mode = mode
        print(f"Monitoring mode set to: {mode.value}")
    
    def get_monitoring_mode(self) -> MonitoringMode:
        """Get the monitoring mode"""
        return self._monitoring_mode
    
    def enable_real_time_debugging(self, enable: bool):
        """Enable or disable real-time debugging"""
        self._real_time_debugging_enabled = enable
        print(f"Real-time debugging {"enabled" if enable else "disabled"}")
    
    def is_real_time_debugging_enabled(self) -> bool:
        """Check if real-time debugging is enabled"""
        return self._real_time_debugging_enabled
    
    def enable_memory_leak_detection(self, enable: bool):
        """Enable or disable memory leak detection"""
        self._memory_leak_detection_enabled = enable
        print(f"Memory leak detection {"enabled" if enable else "disabled"}")
    
    def is_memory_leak_detection_enabled(self) -> bool:
        """Check if memory leak detection is enabled"""
        return self._memory_leak_detection_enabled
    
    def detect_memory_leaks(self) -> List[MemoryAllocation]:
        """Detect memory leaks"""
        if not self._memory_leak_detection_enabled:
            print("Memory leak detection is not enabled")
            return []
        
        leaks = []
        
        # Simple leak detection: allocations without corresponding deallocations
        for alloc in self._active_allocations:
            # Check if this allocation has been deallocated
            is_deallocated = False
            for dealloc in self._deallocations:
                if alloc.address == dealloc.address:
                    is_deallocated = True
                    break
            
            if not is_deallocated and (alloc.allocation_time > time.time() - 300):  # 5 minutes
                # Consider recent allocations as potential leaks
                leak = alloc
                leak.is_leaked = True
                leaks.append(leak)
        
        print(f"Memory leak detection found {len(leaks)} potential leaks")
        return leaks
    
    def track_memory_allocation(self, address: int, size: int, site: str):
        """Track memory allocation"""
        if not self._memory_leak_detection_enabled:
            return
        
        alloc = MemoryAllocation(
            address=address,
            size=size,
            allocation_time=time.time(),
            allocation_site=site,
            thread_id=threading.get_ident(),
            is_leaked=False
        )
        
        self._active_allocations.append(alloc)
        print(f"Memory allocation: {size} bytes at {address:#x} from {site}")
        
        # Call memory callback if set
        if self._memory_callback:
            self._memory_callback(self._active_allocations)
    
    def track_memory_deallocation(self, address: int):
        """Track memory deallocation"""
        if not self._memory_leak_detection_enabled:
            return
        
        # Find and remove the allocation
        for i, alloc in enumerate(self._active_allocations):
            if alloc.address == address:
                dealloc = MemoryAllocation(
                    address=address,
                    size=alloc.size,
                    allocation_time=time.time(),
                    allocation_site=alloc.allocation_site,
                    thread_id=alloc.thread_id,
                    is_leaked=alloc.is_leaked
                )
                self._deallocations.append(dealloc)
                self._active_allocations.pop(i)
                print(f"Memory deallocation: {address:#x}")
                break
        
        # Call memory callback if set
        if self._memory_callback:
            self._memory_callback(self._active_allocations)
    
    def enable_performance_profiling(self, enable: bool):
        """Enable or disable performance profiling"""
        self._performance_profiling_enabled = enable
        print(f"Performance profiling {"enabled" if enable else "disabled"}")
    
    def is_performance_profiling_enabled(self) -> bool:
        """Check if performance profiling is enabled"""
        return self._performance_profiling_enabled
    
    def get_performance_metrics(self) -> PerformanceMetrics:
        """Get performance metrics"""
        return self._performance_metrics
    
    def get_cpu_usage(self) -> float:
        """Get CPU usage percentage"""
        return self._performance_metrics.cpu_usage
    
    def get_memory_usage(self) -> float:
        """Get memory usage percentage"""
        return self._performance_metrics.memory_usage
    
    def get_disk_usage(self) -> float:
        """Get disk usage percentage"""
        return self._performance_metrics.disk_usage
    
    def get_neural_metrics(self) -> NeuralMetrics:
        """Get neural metrics"""
        return self._neural_metrics
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate"""
        return self._neural_metrics.average_firing_rate
    
    def get_active_neuron_count(self) -> int:
        """Get active neuron count"""
        return self._neural_metrics.active_neuron_count
    
    def get_most_active_neurons(self, count: int) -> List[int]:
        """Get most active neurons"""
        return list(range(min(count, self._neural_metrics.active_neuron_count)))
    
    def enable_error_detection(self, enable: bool):
        """Enable or disable error detection"""
        self._error_detection_enabled = enable
        print(f"Error detection {"enabled" if enable else "disabled"}")
    
    def is_error_detection_enabled(self) -> bool:
        """Check if error detection is enabled"""
        return self._error_detection_enabled
    
    def get_recent_errors(self) -> List[ErrorInfo]:
        """Get recent errors"""
        return self._recent_errors.copy()
    
    def get_critical_errors(self) -> List[ErrorInfo]:
        """Get critical errors"""
        return self._critical_errors.copy()
    
    def has_recent_errors(self) -> bool:
        """Check if there are recent errors"""
        return not self._recent_errors.empty()
    
    def log_error(self, error_code: str, error_message: str, is_critical: bool = False, context: Optional[Dict[str, str]] = None):
        """Log an error"""
        if not self._error_detection_enabled:
            return
        
        error = ErrorInfo(
            timestamp=time.time(),
            error_type="runtime",
            error_code=error_code,
            error_message=error_message,
            stack_trace="Stack trace not available in Python fallback",
            context=context or {},
            is_recovered=False,
            is_critical=is_critical
        )
        
        if is_critical:
            self._critical_errors.append(error)
        else:
            self._recent_errors.append(error)
        
        print(f"Error {error_code}: {error_message}")
    
    def enable_step_by_step_debugging(self, enable: bool):
        """Enable or disable step-by-step debugging"""
        self._step_by_step_debugging_enabled = enable
        print(f"Step-by-step debugging {"enabled" if enable else "disabled"}")
    
    def is_step_by_step_debugging_enabled(self) -> bool:
        """Check if step-by-step debugging is enabled"""
        return self._step_by_step_debugging_enabled
    
    def set_debug_step(self, step: int):
        """Set debug step"""
        self._current_step = step
        print(f"Debug step set to: {step}")
    
    def get_current_debug_step(self) -> int:
        """Get current debug step"""
        return self._current_step
    
    def step_forward(self):
        """Advance debug step"""
        self._current_step += 1
        print(f"Debug step advanced to: {self._current_step}")
    
    def step_backward(self):
        """Move debug step back"""
        if self._current_step > 0:
            self._current_step -= 1
            print(f"Debug step moved back to: {self._current_step}")
    
    def has_more_steps_forward(self) -> bool:
        """Check if there are more steps forward"""
        return self._current_step < self._max_step
    
    def has_more_steps_backward(self) -> bool:
        """Check if there are more steps backward"""
        return self._current_step > 0
    
    def enable_command_line_interface(self, enable: bool):
        """Enable or disable command-line interface"""
        self._command_line_interface_enabled = enable
        print(f"Command-line interface {"enabled" if enable else "disabled"}")
    
    def is_command_line_interface_enabled(self) -> bool:
        """Check if command-line interface is enabled"""
        return self._command_line_interface_enabled
    
    def execute_command(self, command_line: str) -> bool:
        """Execute a command-line command"""
        if not self._command_line_interface_enabled:
            print("Command-line interface is not enabled")
            return False
        
        # Simple command parsing
        if command_line == "help":
            self._print_help()
            return True
        elif command_line == "quit":
            print("Shutting down...")
            return True  # Signal to quit
        elif command_line.startswith("visualize "):
            target = command_line[10:]
            print(f"Visualizing {target}")
            return True
        
        print(f"Unknown command: {command_line}")
        return False
    
    def _print_help(self):
        """Print help information"""
        print("Available commands:")
        print("  help        - Show available commands")
        print("  quit        - Quit the application")
        print("  visualize   - Enable visualization (brain|world|both)")
        print("  debug       - Debug information (info|memory|performance|neural)")
        print("  export      - Export debug data to file")
        print("  record      - Start/stop debug recording")
        print("  help        - Show command help")
    
    def get_available_commands(self) -> List[DebugCommand]:
        """Get available commands"""
        commands = []
        
        commands.append(DebugCommand("help", "Show available commands", [], "general"))
        commands.append(DebugCommand("quit", "Quit the application", [], "general"))
        commands.append(DebugCommand("visualize", "Enable visualization", ["target"], "visualization"))
        commands.append(DebugCommand("debug", "Debug information", ["type"], "debug"))
        commands.append(DebugCommand("export", "Export debug data", ["filename"], "export"))
        commands.append(DebugCommand("record", "Start/stop debug recording", ["action"], "recording"))
        
        return commands
    
    def get_command_help(self, command_name: str) -> str:
        """Get help for a specific command"""
        for cmd in self.get_available_commands():
            if cmd.name == command_name:
                help_text = f"{cmd.description}\n"
                help_text += f"Required parameters: {'none' if not cmd.required_parameters else ', '.join(cmd.required_parameters)}\n"
                help_text += f"Category: {cmd.category}"
                return help_text
        
        return f"Command not found: {command_name}"
    
    def enable_debug_output(self, enable: bool):
        """Enable or disable debug output"""
        self._debug_output_enabled = enable
        print(f"Debug output {"enabled" if enable else "disabled"}")
    
    def is_debug_output_enabled(self) -> bool:
        """Check if debug output is enabled"""
        return self._debug_output_enabled
    
    def set_output_level(self, level: int):
        """Set output level (0=quiet, 1=info, 2=verbose, 3=debug)"""
        self._output_level = max(0, min(3, level))
        print(f"Output level set to: {self._output_level}")
    
    def get_output_level(self) -> int:
        """Get output level"""
        return self._output_level
    
    def generate_debug_report(self) -> str:
        """Generate a comprehensive debug report"""
        report = f"=== NLM Debug Report\n"
        report += f"Generated at: {time.time()}\n\n"
        
        report += f"=== System Status ===\n"
        report += f"Debug Interface Active: {'Yes' if self._is_initialized else 'No'}\n"
        report += f"Debug Mode: {self._debug_mode.value}\n"
        report += f"Monitoring Mode: {self._monitoring_mode.value}\n\n"
        
        report += f"=== Performance ===\n"
        report += f"CPU Usage: {self._performance_metrics.cpu_usage}%\n"
        report += f"Memory Usage: {self._performance_metrics.memory_usage}% ({self._performance_metrics.used_memory // (1024*1024*1024)} GB / {self._performance_metrics.total_memory // (1024*1024*1024)} GB)\n"
        report += f"Disk Usage: {self._performance_metrics.disk_usage}%\n\n"
        
        report += f"=== Neural Activity ===\n"
        report += f"Total Neurons: {self._neural_metrics.total_neuron_count}\n"
        report += f"Active Neurons: {self._neural_metrics.active_neuron_count}\n"
        report += f"Average Firing Rate: {self._neural_metrics.average_firing_rate} Hz\n\n"
        
        report += f"=== Memory Management ===\n"
        report += f"Active Allocations: {len(self._active_allocations)}\n"
        report += f"Total Allocations: {len(self._deallocations)}\n\n"
        
        report += f"=== Errors ===\n"
        report += f"Recent Errors: {len(self._recent_errors)}\n"
        report += f"Critical Errors: {len(self._critical_errors)}\n\n"
        
        report += f"=== Debug History ===\n"
        report += f"Current Debug Step: {self._current_step}\n"
        report += f"Max Debug Step: {self._max_step}\n"
        report += f"Debug History Size: {len(self._debug_history)}\n\n"
        
        report += f"=== End Report ===\n"
        
        return report
    
    def export_debug_data(self, filename: str):
        """Export debug data to file"""
        try:
            data = {
                "debug_report": self.generate_debug_report(),
                "performance_metrics": {
                    "cpu_usage": self._performance_metrics.cpu_usage,
                    "memory_usage": self._performance_metrics.memory_usage,
                    "disk_usage": self._performance_metrics.disk_usage
                },
                "neural_metrics": {
                    "total_neuron_count": self._neural_metrics.total_neuron_count,
                    "active_neuron_count": self._neural_metrics.active_neuron_count,
                    "average_firing_rate": self._neural_metrics.average_firing_rate
                },
                "errors": [
                    {"code": error.error_code, "message": error.error_message, "critical": error.is_critical}
                    for error in self._recent_errors
                ],
                "memory_allocations": [
                    {"address": alloc.address, "size": alloc.size, "site": alloc.allocation_site, "leaked": alloc.is_leaked}
                    for alloc in self._active_allocations
                ]
            }
            
            with open(filename, 'w') as f:
                json.dump(data, f, indent=2)
            
            print(f"Debug data exported to: {filename}")
            
        except Exception as e:
            print(f"Error exporting debug data: {e}")
    
    def start_recording(self, filename: str):
        """Start recording debug data"""
        self._recording = True
        self._recording_filename = filename
        print(f"Started debug recording to: {filename}")
    
    def stop_recording(self):
        """Stop recording debug data"""
        if self._recording:
            self._recording = False
            print(f"Stopped debug recording")
    
    def is_recording(self) -> bool:
        """Check if recording is enabled"""
        return self._recording
    
    def set_debug_event_callback(self, callback: Callable[[str, str], None]):
        """Set debug event callback"""
        self._debug_event_callback = callback
    
    def set_performance_callback(self, callback: Callable[[PerformanceMetrics], None]):
        """Set performance callback"""
        self._performance_callback = callback
    
    def set_memory_callback(self, callback: Callable[[List[MemoryAllocation]], None]):
        """Set memory callback"""
        self._memory_callback = callback
    
    def set_neural_callback(self, callback: Callable[[NeuralMetrics], None]):
        """Set neural callback"""
        self._neural_callback = callback
    
    def add_monitoring_hook(self, hook: MonitoringHook):
        """Add a monitoring hook"""
        self._monitoring_hooks.append(hook)
        print(f"Added monitoring hook: {hook.hook_name}")
    
    def remove_monitoring_hook(self, hook_name: str):
        """Remove a monitoring hook"""
        self._monitoring_hooks = [h for h in self._monitoring_hooks if h.hook_name != hook_name]
        print(f"Removed monitoring hook: {hook_name}")
    
    def get_monitoring_hooks(self) -> List[MonitoringHook]:
        """Get all monitoring hooks"""
        return self._monitoring_hooks.copy()
class NLMVisualizationTools:
    """Main class providing access to NLM visualization and debugging tools"""
    
    def __init__(self, library_path: Optional[str] = None):
        self._library_path = library_path
        self._visualization_interface = None
        self._debug_interface = None
        self._is_initialized = False
    
    def initialize(self) -> bool:
        """Initialize the NLM visualization and debugging tools"""
        try:
            print("Initializing NLM Visualization and Debugging Tools...")
            
            # Initialize visualization interface
            self._visualization_interface = VisualizationInterface(self._library_path)
            if not self._visualization_interface.initialize():
                print("Failed to initialize visualization interface")
                return False
            
            # Initialize debug interface
            self._debug_interface = DebugInterface(self._library_path)
            if not self._debug_interface.initialize():
                print("Failed to initialize debug interface")
                return False
            
            self._is_initialized = True
            print("NLM Visualization and Debugging Tools initialized successfully")
            return True
            
        except Exception as e:
            print(f"Error initializing NLM tools: {e}")
            return False
    
    def get_visualization_interface(self) -> VisualizationInterface:
        """Get the visualization interface"""
        if not self._is_initialized:
            raise RuntimeError("NLM tools not initialized. Call initialize() first.")
        return self._visualization_interface
    
    def get_debug_interface(self) -> DebugInterface:
        """Get the debug interface"""
        if not self._is_initialized:
            raise RuntimeError("NLM tools not initialized. Call initialize() first.")
        return self._debug_interface
    
    def get_visualization_data(self) -> Dict[str, Any]:
        """Get comprehensive visualization data"""
        if not self._is_initialized:
            raise RuntimeError("NLM tools not initialized. Call initialize() first.")
        
        data = {
            "visualization_mode": self._visualization_interface.get_visualization_mode().value,
            "debug_mode": self._debug_interface.get_debug_mode().value,
            "performance_metrics": self._debug_interface.get_performance_metrics().__dict__,
            "neural_metrics": self._debug_interface.get_neural_metrics().__dict__,
            "time_series_data": [point.__dict__ for point in self._visualization_interface._time_series_data],
            "debug_info": [info.__dict__ for info in self._visualization_interface._debug_info],
            "monitoring_hooks": [hook.__dict__ for hook in self._visualization_interface._monitoring_hooks],
            "memory_allocations": [alloc.__dict__ for alloc in self._debug_interface._active_allocations]
        }
        
        return data
    
    def export_data(self, filename: str, format: OutputFormat = OutputFormat.JSON):
        """Export data to file"""
        if not self._is_initialized:
            raise RuntimeError("NLM tools not initialized. Call initialize() first.")
        
        if format == OutputFormat.JSON:
            data = self.get_visualization_data()
            with open(filename, 'w') as f:
                json.dump(data, f, indent=2)
            print(f"Data exported to JSON: {filename}")
        elif format == OutputFormat.CSV:
            # Implementation would depend on specific CSV format needed
            print(f"CSV export to {filename} not implemented")
        else:
            print(f"Export format {format.value} not implemented")
    
    def close(self):
        """Close and cleanup the NLM tools"""
        if self._is_initialized:
            if self._visualization_interface:
                self._visualization_interface.close()
            if self._debug_interface:
                self._debug_interface.close()
            
            self._is_initialized = False
            print("NLM Visualization and Debugging Tools closed")

# Global instance for convenience
_nlm_tools = None

def initialize_nlm_tools(library_path: Optional[str] = None) -> NLMVisualizationTools:
    """Initialize global NLM tools instance"""
    global _nlm_tools
    _nlm_tools = NLMVisualizationTools(library_path)
    if _nlm_tools.initialize():
        return _nlm_tools
    else:
        _nlm_tools = None
        raise RuntimeError("Failed to initialize NLM tools")

def get_nlm_tools() -> NLMVisualizationTools:
    """Get the global NLM tools instance"""
    if _nlm_tools is None or not _nlm_tools.is_active():
        raise RuntimeError("NLM tools not initialized. Call initialize_nlm_tools() first.")
    return _nlm_tools

def is_nlm_tools_initialized() -> bool:
    """Check if NLM tools are initialized"""
    return _nlm_tools is not None and _nlm_tools.is_active()
def create_demo_scenario(scenario_type: str = "brain_activity"):
    """Create a demo scenario for visualization and debugging"""
    if not is_nlm_tools_initialized():
        initialize_nlm_tools()
    
    tools = get_nlm_tools()
    
    if scenario_type == "brain_activity":
        print("Creating brain activity demo scenario...")
        # Demo scenario would include brain visualization, activity monitoring, etc.
        print("Brain activity demo created")
    elif scenario_type == "world_simulation":
        print("Creating world simulation demo scenario...")
        # Demo scenario would include world visualization, agent monitoring, etc.
        print("World simulation demo created")
    elif scenario_type == "performance_profiling":
        print("Creating performance profiling demo scenario...")
        # Demo scenario would include performance monitoring, profiling, etc.
        print("Performance profiling demo created")
    else:
        print(f"Unknown demo scenario: {scenario_type}")
        return
    
    return tools
# Example usage functions
def example_usage():
    """Example usage of NLM visualization and debugging tools"""
    print("=== NLM Visualization and Debugging Tools - Example Usage ===\n")
    
    # Initialize tools
    tools = initialize_nlm_tools()
    
    # Get interfaces
    viz = tools.get_visualization_interface()
    debug = tools.get_debug_interface()
    
    # Configure tools
    viz.set_update_rate(30.0)
    viz.set_visualization_mode(VisualizationMode.REAL_TIME)
    
    debug.set_debug_mode(DebugMode.DETAILED)
    debug.set_monitoring_mode(MonitoringMode.ACTIVE)
    debug.enable_real_time_debugging(True)
    debug.enable_memory_leak_detection(True)
    debug.enable_performance_profiling(True)
    
    # Add monitoring hooks
    def custom_callback(metrics):
        print(f"Custom callback: {metrics}")
    
    hook = MonitoringHook(
        hook_name="custom_monitor",
        callback=custom_callback,
        last_execution=time.time(),
        execution_interval=1.0,
        is_active=True,
        hook_type="periodic"
    )
    
    viz.add_monitoring_hook(hook)
    
    # Start simulation
    print("Starting simulation...")
    for step in range(100):
        viz.update()
        debug.update()
        
        # Add time series data
        viz.add_time_series_metric("step", float(step))
        viz.add_time_series_metric("firing_rate", 10.0 + (time.time() % 5.0))
        
        # Add debug info
        debug_info = DebugInfo(
            component_name="BrainVisualization",
            current_state="Active",
            warnings=["High firing rate detected"] if step % 10 == 0 else [],
            errors=[] if step % 20 != 0 else ["Low neural activity"],
            last_update_time=time.time(),
            is_healthy=step % 5 != 0
        )
        viz.add_debug_info(debug_info)
        
        time.sleep(0.1)  # Simulate time passing
    
    # Generate and export report
    print(f"\n=== Generated Debug Report ===\n{debug.generate_debug_report()}")
    
    tools.export_data("demo_report.json", OutputFormat.JSON)
    
    # Cleanup
    tools.close()
    print("\nExample usage completed successfully!")

if __name__ == "__main__":
    # Run example usage
    example_usage()
