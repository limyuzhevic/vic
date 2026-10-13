"""Exception classes for NLM errors and utilities"""

class NLMError(Exception):
    """Base exception for all NLM errors"""
    pass

class ConfigurationError(NLMError):
    """Configuration-related errors"""
    pass

class SimulationError(NLMError):
    """Simulation or step execution errors"""
    pass

class MemoryError(NLMError):
    """Memory system errors"""
    pass

class PlasticityError(NLMError):
    """Plasticity computation errors"""
    pass

class SensoryError(NLMError):
    """Sensory processing errors"""
    pass

class MotorError(NLMError):
    """Motor decoding errors"""
    pass

class WorldError(NLMError):
    """World simulation errors"""
    pass

class AgentBrainError(NLMError):
    """Agent brain interface errors"""
    pass


class ErrorHandler:
    """
    Centralized error handling for NLM operations.
    
    This class provides consistent error handling across all NLM modules,
    including error classification, recovery suggestions, and logging.
    """
    
    @staticmethod
    def handle_error(error: Exception) -> Dict[str, Any]:
        """
        Handle and classify an error.
        
        Args:
            error: The exception to handle.
            
        Returns:
            Dictionary with error information and recovery suggestions.
        """
        error_info = {
            'type': type(error).__name__,
            'message': str(error),
            'recoverable': False,
            'recovery_suggestion': None,
            'severity': 'medium',
        }
        
        # Classify based on error type
        if isinstance(error, ConfigurationError):
            error_info['recoverable'] = True
            error_info['recovery_suggestion'] = "Check configuration parameters and values"
            error_info['severity'] = 'low'
            
        elif isinstance(error, SimulationError):
            error_info['recoverable'] = True
            error_info['recovery_suggestion'] = "Check brain initialization and step parameters"
            error_info['severity'] = 'medium'
            
        elif isinstance(error, MemoryError):
            error_info['recoverable'] = False
            error_info['recovery_suggestion'] = "Consider reducing memory capacity or checkpoint frequency"
            error_info['severity'] = 'high'
            
        elif isinstance(error, PlasticityError):
            error_info['recoverable'] = True
            error_info['recovery_suggestion'] = "Check plasticity parameters and neural activity"
            error_info['severity'] = 'medium'
            
        elif isinstance(error, SensoryError):
            error_info['recoverable'] = True
            error_info['recovery_suggestion'] = "Check world configuration and sensory input ranges"
            error_info['severity'] = 'low'
            
        elif isinstance(error, MotorError):
            error_info['recoverable'] = True
            error_info['recovery_suggestion'] = "Check motor neuron configuration and action mappings"
            error_info['severity'] = 'medium'
            
        elif isinstance(error, WorldError):
            error_info['recoverable'] = False
            error_info['recovery_suggestion'] = "Check world boundaries and object configurations"
            error_info['severity'] = 'high'
            
        elif isinstance(error, AgentBrainError):
            error_info['recoverable'] = True
            error_info['recovery_suggestion'] = "Check agent initialization and world connection"
            error_info['severity'] = 'medium'
        
        return error_info
    
    @staticmethod
    def safe_operation(operation: callable, *args, **kwargs) -> Any:
        """
        Execute an operation with error handling.
        
        Args:
            operation: The operation to execute.
            *args: Arguments to pass to the operation.
            **kwargs: Keyword arguments to pass to the operation.
            
        Returns:
            Result of the operation.
            
        Raises:
            NLMError: If the operation fails and is not recoverable.
        """
        try:
            return operation(*args, **kwargs)
        except Exception as e:
            error_info = ErrorHandler.handle_error(e)
            
            if error_info['recoverable']:
                # For recoverable errors, return a default/safe value
                # The type of safe value depends on the operation
                # This is a generic implementation
                if operation.__name__ in ['get', 'compute', 'calculate']:
                    return None
                elif operation.__name__ in ['step', 'update', 'process']:
                    return False
                else:
                    return None
            else:
                # Re-raise non-recoverable errors
                raise NLMError(f"Non-recoverable error in {operation.__name__}: {e}")


class DebugLogger:
    """
    Debug logging utility for NLM operations.
    
    This class provides structured logging for debugging NLM simulations,
    including detailed trace information, performance metrics, and
    error context.
    """
    
    def __init__(self, level: str = "info"):
        self.level = level
        self.log_entries: List[Dict[str, Any]] = []
        
    def set_level(self, level: str) -> None:
        """Set logging level"""
        self.level = level
        
    def log(self, level: str, message: str, context: Optional[Dict[str, Any]] = None) -> None:
        """
        Log a message with level and optional context.
        
        Args:
            level: Log level (debug, info, warning, error).
            message: Log message.
            context: Optional context dictionary.
        """
        if self._should_log(level):
            entry = {
                'timestamp': time.time(),
                'level': level,
                'message': message,
                'context': context or {},
            }
            self.log_entries.append(entry)
            
            # Also print to stdout
            print(f"[{level.upper()}] {message}")
            
    def debug(self, message: str, context: Optional[Dict[str, Any]] = None) -> None:
        """Log debug message"""
        self.log("debug", message, context)
        
    def info(self, message: str, context: Optional[Dict[str, Any]] = None) -> None:
        """Log info message"""
        self.log("info", message, context)
        
    def warning(self, message: str, context: Optional[Dict[str, Any]] = None) -> None:
        """Log warning message"""
        self.log("warning", message, context)
        
    def error(self, message: str, context: Optional[Dict[str, Any]] = None) -> None:
        """Log error message"""
        self.log("error", message, context)
        
    def _should_log(self, level: str) -> bool:
        """Check if message should be logged based on level"""
        levels = {"debug": 0, "info": 1, "warning": 2, "error": 3}
        return levels.get(level, 1) >= levels.get(self.level, 1)
        
    def get_log_entries(self, level: Optional[str] = None) -> List[Dict[str, Any]]:
        """
        Get log entries, optionally filtered by level.
        
        Args:
            level: Optional level to filter by.
            
        Returns:
            List of log entries.
        """
        if level is None:
            return self.log_entries
        return [entry for entry in self.log_entries if entry['level'] == level]
        
    def clear(self) -> None:
        """Clear all log entries"""
        self.log_entries.clear()


class PerformanceMonitor:
    """
    Performance monitoring for NLM operations.
    
    This class tracks and reports performance metrics for NLM simulations,
    including timing, memory usage, and computational efficiency.
    """
    
    def __init__(self):
        self.metrics: Dict[str, Any] = {
            'timings': {},
            'memory_usage': {},
            'cpu_usage': {},
            'simulation_stats': {},
        }
        self.start_time = time.time()
        
    def start_operation(self, name: str) -> None:
        """Start timing an operation"""
        self.metrics['timings'][name] = {
            'start_time': time.time(),
            'end_time': None,
            'duration': None,
        }
        
    def end_operation(self, name: str) -> float:
        """
        End timing an operation.
        
        Args:
            name: Operation name.
            
        Returns:
            Duration in seconds.
        """
        if name in self.metrics['timings']:
            entry = self.metrics['timings'][name]
            entry['end_time'] = time.time()
            entry['duration'] = entry['end_time'] - entry['start_time']
            return entry['duration']
        return 0.0
        
    def record_memory_usage(self, component: str, bytes_used: int) -> None:
        """
        Record memory usage for a component.
        
        Args:
            component: Component name.
            bytes_used: Memory used in bytes.
        """
        self.metrics['memory_usage'][component] = bytes_used
        
    def record_cpu_usage(self, component: str, usage_percent: float) -> None:
        """
        Record CPU usage for a component.
        
        Args:
            component: Component name.
            usage_percent: CPU usage percentage.
        """
        self.metrics['cpu_usage'][component] = usage_percent
        
    def record_simulation_stats(self, step: int, neurons: int, 
                               spikes: int, time: float) -> None:
        """
        Record simulation statistics.
        
        Args:
            step: Simulation step number.
            neurons: Number of neurons.
            spikes: Number of spikes.
            time: Simulation time elapsed.
        """
        if 'steps' not in self.metrics['simulation_stats']:
            self.metrics['simulation_stats']['steps'] = []
        
        self.metrics['simulation_stats']['steps'].append({
            'step': step,
            'neurons': neurons,
            'spikes': spikes,
            'time': time,
            'spike_rate': spikes / time if time > 0 else 0.0,
        })
        
    def get_average_timing(self, name: str) -> float:
        """
        Get average timing for an operation.
        
        Args:
            name: Operation name.
            
        Returns:
            Average duration in seconds.
        """
        durations = []
        for entry in self.metrics['timings'].values():
            if entry.get('duration') is not None:
                durations.append(entry['duration'])
        
        return sum(durations) / len(durations) if durations else 0.0
        
    def get_peak_memory_usage(self) -> int:
        """Get peak memory usage in bytes"""
        return max(self.metrics['memory_usage'].values()) if self.metrics['memory_usage'] else 0
        
    def get_average_cpu_usage(self) -> float:
        """Get average CPU usage percentage"""
        usages = list(self.metrics['cpu_usage'].values())
        return sum(usages) / len(usages) if usages else 0.0
        
    def get_simulation_summary(self) -> Dict[str, Any]:
        """
        Get summary of simulation statistics.
        
        Returns:
            Dictionary with simulation summary.
        """
        steps = self.metrics['simulation_stats'].get('steps', [])
        if not steps:
            return {}
        
        total_steps = len(steps)
        total_neurons = sum(s['neurons'] for s in steps)
        total_spikes = sum(s['spikes'] for s in steps)
        total_time = steps[-1]['time'] if steps else 0.0
        
        return {
            'total_steps': total_steps,
            'average_neurons': total_neurons / total_steps,
            'average_spikes': total_spikes / total_steps,
            'total_simulation_time': total_time,
            'average_spike_rate': total_spikes / total_time if total_time > 0 else 0.0,
            'peak_memory_mb': self.get_peak_memory_usage() / (1024 * 1024),
            'average_cpu_percent': self.get_average_cpu_usage(),
        }
        
    def print_summary(self) -> None:
        """Print performance summary"""
        print("=== Performance Summary ===")
        
        summary = self.get_simulation_summary()
        if not summary:
            print("No simulation data available")
            return
            
        print(f"Total steps: {summary['total_steps']}")
        print(f"Average neurons: {summary['average_neurons']:.1f}")
        print(f"Average spikes per step: {summary['average_spikes']:.1f}")
        print(f"Total simulation time: {summary['total_simulation_time']:.2f}s")
        print(f"Average spike rate: {summary['average_spike_rate']:.2f} Hz")
        print(f"Peak memory: {summary['peak_memory_mb']:.1f} MB")
        print(f"Average CPU: {summary['average_cpu_percent']:.1f}%")


# System utilities
def get_version() -> str:
    """Get NLM Python package version"""
    return "0.1.0"


def get_build_info() -> Dict[str, Any]:
    """Get build and system information"""
    import platform
    import sys
    
    return {
        'version': get_version(),
        'platform': platform.system(),
        'architecture': platform.machine(),
        'python_version': platform.python_version(),
        'python_implementation': platform.python_implementation(),
        'system': sys.platform,
        'build_date': '2024-10-13',  # Placeholder
        'compiler': 'C++17',  # Placeholder
    }


def validate_config(config_dict: Dict[str, Any]) -> List[str]:
    """
    Validate configuration dictionary.
    
    Args:
        config_dict: Configuration dictionary to validate.
        
    Returns:
        List of validation errors (empty if valid).
    """
    errors = []
    
    # Required keys
    required_keys = ['neuron_count', 'simulation_timestep']
    for key in required_keys:
        if key not in config_dict:
            errors.append(f"Missing required key: {key}")
    
    # Type and value validation
    if 'neuron_count' in config_dict:
        neuron_count = config_dict['neuron_count']
        if not isinstance(neuron_count, (int, float)) or neuron_count <= 0:
            errors.append("neuron_count must be a positive number")
            
    if 'simulation_timestep' in config_dict:
        timestep = config_dict['simulation_timestep']
        if not isinstance(timestep, (int, float)) or timestep <= 0:
            errors.append("simulation_timestep must be a positive number")
            
    if 'connection_probability' in config_dict:
        prob = config_dict['connection_probability']
        if not isinstance(prob, (int, float)) or not (0 <= prob <= 1):
            errors.append("connection_probability must be between 0 and 1")
    
    return errors


def print_system_info() -> None:
    """Print system and NLM information"""
    import platform
    
    print("=== NLM System Information ===")
    print(f"NLM Python Package: {get_version()}")
    print(f"Platform: {platform.system()} {platform.machine()}")
    print(f"Python: {platform.python_version()}")
    print(f"Architecture: {platform.architecture()[0]}")
    
    build_info = get_build_info()
    print(f"Build: {build_info['platform']} {build_info['architecture']}")
    
    print("\n=== Features ===")
    print("- Spiking neural network simulation")
    print("- Event-driven dynamics")
    print("- Multiple plasticity rules (STDP, Hebbian)")
    print("- Integrated memory systems (working, episodic)")
    print("- Prediction and cognition")
    print("- Neuromodulation (dopamine, curiosity, novelty)")
    print("- Development and maturation")
    print("- World simulation and interaction")
    print("- Agent-based architecture")
    print("- Checkpoint system for persistence")
    print("- Performance optimizations (SIMD, parallel)")
    
    print("\n=== Quick Start ===")
    print("1. Import pynlm")
    print("2. Create brain with createBrain(createDefaultConfig())")
    print("3. Initialize brain with brain.initialize()")
    print("4. Run simulation loop with brain.step(step)")
    print("5. Optionally use AgentBrain for world interaction")


# Export commonly used classes and functions
__all__ = [
    # Exceptions
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'MemoryError',
    'PlasticityError',
    'SensoryError',
    'MotorError',
    'WorldError',
    'AgentBrainError',
    
    # Classes
    'ErrorHandler',
    'DebugLogger',
    'PerformanceMonitor',
    
    # Utilities
    'get_version',
    'get_build_info',
    'validate_config',
    'print_system_info',
]
