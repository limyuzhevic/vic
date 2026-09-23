Enhanced Python Bindings for NLM Neural Simulation Framework

This module provides a modern, Pythonic interface to the NLM (Neural Learning Machine)
C++ neural simulation framework. The bindings offer high-level abstractions, comprehensive
documentation, and advanced features for building sophisticated neural simulations.

Key Features:
- Modern Python API with type hints and property-based interfaces
- Context managers for resource management
- Comprehensive simulation control and monitoring
- Advanced configuration with validation
- Visualization integration
- Error handling with detailed diagnostics
- Async support for non-blocking operations
- Complete documentation and examples

Usage:
    >>> from pynlm import AdvancedBrain, create_simulation
    >>> brain = AdvancedBrain()
    >>> with brain.simulation_context() as sim:
    ...     results = sim.run_episode(max_steps=1000)
    ...     print(f"Episode completed: {results['success']}")
"""

import asyncio
import logging
import time
import warnings
from contextlib import contextmanager
from dataclasses import dataclass, field
from datetime import datetime
from enum import Enum
from typing import Optional, List, Dict, Any, Union, Callable, Tuple, TypeVar, Generic
from pathlib import Path

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

T = TypeVar('T')


class ValidationError(Exception):
    """Exception raised when configuration validation fails."""
    
    def __init__(self, message: str, errors: Optional[List[str]] = None):
        super().__init__(message)
        self.errors = errors or [message]
    
    def __str__(self) -> str:
        if len(self.errors) == 1:
            return self.errors[0]
        return "\n".join(f"- {error}" for error in self.errors)


class NLMError(Exception):
    """Base exception for NLM-specific errors."""
    pass


@dataclass
class SimulationMetrics:
    """Metrics collected during simulation."""
    steps_completed: int = 0
    total_reward: float = 0.0
    average_reward_per_step: float = 0.0
    max_reward: float = float('-inf')
    min_reward: float = float('inf')
    total_spikes: int = 0
    active_neurons: int = 0
    firing_rate: float = 0.0
    simulation_time: float = 0.0
    energy_consumed: float = 0.0


@dataclass
class SimulationState:
    """Current state of the simulation."""
    step: int = 0
    time: float = 0.0
    neurons_active: int = 0
    neurons_firing: int = 0
    total_spikes: int = 0
    current_action: Optional[Any] = None
    reward: float = 0.0
    success: bool = False
    custom_data: Dict[str, Any] = field(default_factory=dict)


class ValidationRules:
    """Validation rules for simulation parameters."""
    
    @staticmethod
    def validate_step_count(value: int) -> List[str]:
        errors = []
        if not isinstance(value, int):
            errors.append(f"Step count must be an integer, got {type(value).__name__}")
        elif value < 0:
            errors.append("Step count cannot be negative")
        elif value > 1000000:
            errors.append("Step count is too large (max 1,000,000)")
        return errors
    
    @staticmethod
    def validate_reward(value: float) -> List[str]:
        errors = []
        if not isinstance(value, (int, float)):
            errors.append(f"Reward must be a number, got {type(value).__name__}")
        elif value < -1000:
            errors.append("Reward too negative (min -1000)")
        elif value > 1000:
            errors.append("Reward too positive (max 1000)")
        return errors
    
    @staticmethod
    def validate_position(value: float) -> List[str]:
        errors = []
        if not isinstance(value, (int, float)):
            errors.append(f"Position must be a number, got {type(value).__name__}")
        elif abs(value) > 1e6:
            errors.append("Position too large (max absolute value 1,000,000)")
        return errors
    
    @staticmethod
    def validate_energy(value: float) -> List[str]:
        errors = []
        if not isinstance(value, (int, float)):
            errors.append(f"Energy must be a number, got {type(value).__name__}")
        elif value < 0:
            errors.append("Energy cannot be negative")
        elif value > 1e6:
            errors.append("Energy too large (max 1,000,000)")
        return errors


class NLMTypes:
    """Python type definitions for NLM entities."""
    
    @dataclass
    class NeuronId:
        value: int
    
    @dataclass
    class SynapseId:
        value: int
    
    @dataclass
    class RegionId:
        value: int
    
    @dataclass
    class PopulationId:
        value: int
    
    class NeuronType(Enum):
        EXCITATORY = "excitatory"
        INHIBITORY = "inhibitory"
        MODULATORY = "modulatory"
        SENSORY = "sensory"
        MOTOR = "motor"
        INTERNAL = "internal"
    
    class SynapseType(Enum):
        EXCITATORY = "excitatory"
        INHIBITORY = "inhibitory"
        MODULATORY = "modulatory"
        ELECTRICAL = "electrical"
        GAP_JUNCTION = "gap_junction"
    
    class DevelopmentalStage(Enum):
        INITIAL = "initial"
        CRITICAL_PERIOD = "critical_period"
        MATURATION = "maturation"
        ADULT = "adult"
        AGING = "aging"
    
    class FiringState(Enum):
        RESTING = "resting"
        ACTIVE = "active"
        REFRACTORY = "refractory"
        INHIBITED = "inhibited"
    
    class ActionType(Enum):
        MOVE_FORWARD = "move_forward"
        MOVE_BACKWARD = "move_backward"
        MOVE_LEFT = "move_left"
        MOVE_RIGHT = "move_right"
        TURN_LEFT = "turn_left"
        TURN_RIGHT = "turn_right"
        LOOK = "look"
        LOOK_UP = "look_up"
        LOOK_DOWN = "look_down"
        INTERACT = "interact"
        EAT = "eat"
        DRINK = "drink"
        REST = "rest"
        WAIT = "wait"
        CUSTOM = "custom"
    
    class MotorCommand(Enum):
        MOVE_FORWARD = "move_forward"
        MOVE_BACKWARD = "move_backward"
        TURN_LEFT = "turn_left"
        TURN_RIGHT = "turn_right"
        LOOK_LEFT = "look_left"
        LOOK_RIGHT = "look_right"
        INTERACT = "interact"
        WAIT = "wait"
    
    class WorldObjectType(Enum):
        EMPTY = "empty"
        RESOURCE = "resource"
        HAZARD = "hazard"
        WALL = "wall"
        MARKER = "marker"
    
    class ActionResult:
        def __init__(self, reward: float, success: bool, message: str = ""):
            self.reward = reward
            self.success = success
            self.message = message
        
        def __repr__(self) -> str:
            return f"ActionResult(reward={self.reward}, success={self.success}, message='{self.message}')"


class WorldObject(NLMTypes.WorldObjectType):
    """World object with spatial and type information."""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, obj_type: Any = None, value: float = 0.0, radius: float = 0.5):
        """
        Initialize a world object.
        
        Args:
            x: X-coordinate position
            y: Y-coordinate position
            obj_type: Type of the object (WorldObjectType)
            value: Numeric value associated with the object
            radius: Physical radius of the object
        """
        self.x = x
        self.y = y
        self.type = obj_type
        self.value = value
        self.radius = radius
        self.active = True
    
    def distance_to(self, other: 'WorldObject') -> float:
        """Calculate distance to another object."""
        return ((self.x - other.x) ** 2 + (self.y - other.y) ** 2) ** 0.5
    
    def is_colliding(self, other: 'WorldObject') -> bool:
        """Check if this object is colliding with another."""
        return self.distance_to(other) < (self.radius + other.radius)


class AgentBody:
    """Agent body state including position, velocity, and attributes."""
    
    def __init__(self):
        self.x: float = 0.0
        self.y: float = 0.0
        self.orientation: float = 0.0  # radians
        self.velocity_x: float = 0.0
        self.velocity_y: float = 0.0
        self.angular_velocity: float = 0.0
        self.energy: float = 100.0
        self.health: float = 1.0
        self.age: float = 0.0
        self.is_moving: bool = False
        self.is_turning: bool = False
        self.last_action_time: float = 0.0
    
    def update(self, dt: float):
        """Update body state based on velocity and time step."""
        self.x += self.velocity_x * dt
        self.y += self.velocity_y * dt
        self.orientation += self.angular_velocity * dt
        self.age += dt
        
        # Decay energy
        self.energy *= 0.99
    
    def reset(self):
        """Reset body state to initial values."""
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocity_x = 0.0
        self.velocity_y = 0.0
        self.angular_velocity = 0.0
        self.energy = 100.0
        self.health = 1.0
        self.age = 0.0
        self.is_moving = False
        self.is_turning = False
        self.last_action_time = 0.0


class SensoryPercept:
    """Sensory percept data from world sensors."""
    
    def __init__(self):
        self.vision: Optional[Any] = None
        self.touch: Optional[Any] = None
        self.internal: Optional[Any] = None
        self.proprioception: Optional[Any] = None
        self.audio: Optional[Any] = None
        self.timestamp: float = time.time()
    
    def get_all_signals(self) -> Dict[str, Any]:
        """Get all sensory signals as a dictionary."""
        return {
            'vision': self.vision,
            'touch': self.touch,
            'internal': self.internal,
            'proprioception': self.proprioception,
            'audio': self.audio,
            'timestamp': self.timestamp
        }
    
    def update_timestamp(self):
        """Update the timestamp to current time."""
        self.timestamp = time.time()


class Action:
    """Action representation for motor output."""
    
    def __init__(self, action_type: Any = None, parameters: Optional[List[float]] = None):
        """
        Initialize an action.
        
        Args:
            action_type: Type of action (ActionType)
            parameters: Optional parameters for the action
        """
        self.type = action_type
        self.parameters = parameters or []
    
    def get_name(self) -> str:
        """Get the name of the action."""
        return self.type.name if self.type else "UNKNOWN"
    
    def clone(self) -> 'Action':
        """Create a copy of this action."""
        return Action(self.type, self.parameters.copy())


class Config:
    """Enhanced configuration with validation and validation rules."""
    
    def __init__(self):
        self._parameters: Dict[str, Any] = {}
        self._validation_rules: Dict[str, Callable] = {}
        self._defaults: Dict[str, Any] = {}
        self._description: Dict[str, str] = {}
    
    def configure(self, **kwargs: Any) -> 'Config':
        """
        Configure simulation parameters with validation and error handling.
        
        Args:
            **kwargs: Configuration parameters to set
            
        Returns:
            Self for method chaining
            
        Raises:
            ValidationError: If configuration validation fails
        """
        errors = []
        
        for key, value in kwargs.items():
            # Validate parameter
            param_errors = self.validate_parameter(key, value)
            if param_errors:
                errors.extend([f"{key}: {error}" for error in param_errors])
            
            # Set parameter
            self._parameters[key] = value
        
        if errors:
            raise ValidationError("Configuration validation failed", errors)
        
        return self
    
    def validate_parameter(self, name: str, value: Any) -> List[str]:
        """
        Validate a parameter value against rules.
        
        Args:
            name: Parameter name
            value: Parameter value
            
        Returns:
            List of validation error messages (empty if valid)
        """
        # Apply built-in validation rules
        rules_to_check = [
            ValidationRules.validate_step_count if name in ['max_steps', 'timestep'] else lambda x: [],
            ValidationRules.validate_reward if name == 'reward' else lambda x: [],
            ValidationRules.validate_position if name in ['x', 'y', 'position'] else lambda x: [],
            ValidationRules.validate_energy if name in ['energy', 'health'] else lambda x: []
        ]
        
        errors = []
        for rule in rules_to_check:
            errors.extend(rule(value))
        
        # Apply custom validation rules
        if name in self._validation_rules:
            errors.extend(self._validation_rules[name](value))
        
        return errors
    
    def get_validation_errors(self, parameters: Dict[str, Any]) -> List[str]:
        """
        Get all validation errors for given parameters.
        
        Args:
            parameters: Dictionary of parameters to validate
            
        Returns:
            List of validation error messages
        """
        errors = []
        for key, value in parameters.items():
            errors.extend(self.validate_parameter(key, value))
        return errors
    
    def add_validation_rule(self, name: str, rule: Callable[[Any], List[str]]):
        """Add a custom validation rule for a parameter."""
        self._validation_rules[name] = rule
    
    def add_description(self, name: str, description: str):
        """Add a description for a configuration parameter."""
        self._description[name] = description
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists."""
        return key in self._parameters
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value with optional default."""
        return self._parameters.get(key, default)
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys."""
        return list(self._parameters.keys())
    
    def clear(self):
        """Clear all configuration entries."""
        self._parameters.clear()
    
    def summary(self) -> str:
        """Get a summary string of the configuration."""
        if not self._parameters:
            return "Empty configuration"
        
        lines = ["Configuration:"]
        for key, value in sorted(self._parameters.items()):
            lines.append(f"  {key}: {value}")
        
        return "\n".join(lines)
    
    def __repr__(self) -> str:
        return f"<Config: {len(self._parameters)} parameters>"


class MonitoringSystem:
    """Advanced monitoring and diagnostics system."""
    
    def __init__(self):
        self.metrics: Dict[str, Any] = {}
        self.history: List[Dict[str, Any]] = []
        self.alerts: List[Dict[str, Any]] = []
        self.start_time = time.time()
    
    def collect_metrics(self, brain: 'Brain'):
        """
        Collect performance and state metrics from brain.
        
        Args:
            brain: Brain instance to collect metrics from
        """
        timestamp = time.time()
        
        # Collect basic brain metrics
        metrics = {
            'timestamp': timestamp,
            'step': getattr(brain, '_step', 0),
            'neurons_active': brain.get_active_neuron_count() if hasattr(brain, 'get_active_neuron_count') else 0,
            'neurons_firing': brain.get_firing_neuron_count() if hasattr(brain, 'get_firing_neuron_count') else 0,
            'total_spikes': brain.get_total_spike_count() if hasattr(brain, 'get_total_spike_count') else 0,
            'simulation_time': time.time() - self.start_time,
            'memory_usage': self._get_memory_usage(),
            'cpu_usage': self._get_cpu_usage()
        }
        
        self.metrics = metrics
        self.history.append(metrics)
        
        # Check for alerts
        self._check_alerts(metrics)
    
    def _get_memory_usage(self) -> float:
        """Get current memory usage."""
        try:
            import psutil
            return psutil.virtual_memory().percent
        except ImportError:
            return 0.0
    
    def _get_cpu_usage(self) -> float:
        """Get current CPU usage."""
        try:
            import psutil
            return psutil.cpu_percent(interval=0.1)
        except ImportError:
            return 0.0
    
    def _check_alerts(self, metrics: Dict[str, Any]):
        """Check metrics for potential alerts."""
        alerts = []
        
        if metrics.get('memory_usage', 0) > 90:
            alerts.append({
                'level': 'warning',
                'message': f"High memory usage: {metrics['memory_usage']}%",
                'timestamp': metrics['timestamp']
            })
        
        if metrics.get('cpu_usage', 0) > 80:
            alerts.append({
                'level': 'warning',
                'message': f"High CPU usage: {metrics['cpu_usage']}%",
                'timestamp': metrics['timestamp']
            })
        
        if metrics.get('neurons_firing', 0) > 1000:
            alerts.append({
                'level': 'critical',
                'message': f"High firing rate: {metrics['neurons_firing']} neurons firing",
                'timestamp': metrics['timestamp']
            })
        
        self.alerts.extend(alerts)
    
    def get_performance_stats(self) -> Dict[str, Any]:
        """Get performance statistics."""
        if not self.history:
            return {}
        
        metrics = self.history[-1]
        return {
            'current_step': metrics.get('step', 0),
            'current_neurons_active': metrics.get('neurons_active', 0),
            'current_neurons_firing': metrics.get('neurons_firing', 0),
            'current_memory_usage': metrics.get('memory_usage', 0),
            'current_cpu_usage': metrics.get('cpu_usage', 0),
            'total_simulation_time': metrics.get('simulation_time', 0),
            'history_length': len(self.history),
            'recent_alerts': len([a for a in self.alerts if time.time() - a['timestamp'] < 60])
        }
    
    def get_system_status(self) -> Dict[str, Any]:
        """Get comprehensive system status."""
        return {
            'metrics': self.metrics,
            'performance_stats': self.get_performance_stats(),
            'alert_count': len(self.alerts),
            'critical_alerts': [a for a in self.alerts if a['level'] == 'critical'],
            'warning_alerts': [a for a in self.alerts if a['level'] == 'warning']
        }
    
    def generate_report(self) -> str:
        """Generate human-readable performance report."""
        if not self.history:
            return "No metrics collected."
        
        current = self.metrics
        stats = self.get_performance_stats()
        
        report = f"NLM Simulation Report\n"
        report += f"======================\n"
        report += f"Simulation Time: {stats.get('total_simulation_time', 0):.2f}s\n"
        report += f"Current Step: {stats.get('current_step', 0)}\n"
        report += f"Active Neurons: {stats.get('current_neurons_active', 0)}\n"
        report += f"Firing Neurons: {stats.get('current_neurons_firing', 0)}\n"
        report += f"Memory Usage: {stats.get('current_memory_usage', 0):.1f}%\n"
        report += f"CPU Usage: {stats.get('current_cpu_usage', 0):.1f}%\n"
        report += f"Total Alerts: {stats.get('alert_count', 0)}\n"
        report += f"Warning Alerts: {len(stats.get('warning_alerts', []))}\n"
        report += f"Critical Alerts: {len(stats.get('critical_alerts', []))}\n"
        
        return report


class VisualizationIntegration:
    """Visualization integration for NLM simulations."""
    
    def __init__(self):
        self.plotter: Optional[Any] = None
        self.renderers: Dict[str, Any] = {}
    
    def setup_plotting(self, backend: str = 'matplotlib') -> bool:
        """
        Setup plotting backend.
        
        Args:
            backend: Plotting backend ('matplotlib', 'plotly', 'bokeh')
            
        Returns:
            True if setup successful, False otherwise
        """
        try:
            if backend == 'matplotlib':
                import matplotlib.pyplot as plt
                self.plotter = plt
                self.renderers['brain_network'] = self._create_matplotlib_brain_plot
                return True
            elif backend == 'plotly':
                import plotly.graph_objects as go
                self.plotter = go
                self.renderers['brain_network'] = self._create_plotly_brain_plot
                return True
            elif backend == 'bokeh':
                from bokeh.plotting import figure
                self.plotter = figure
                self.renderers['brain_network'] = self._create_bokeh_brain_plot
                return True
        except ImportError:
            logger.warning(f"Backend {backend} not available")
            return False
        
        return False
    
    def create_brain_network_plot(self, brain: 'Brain') -> Any:
        """
        Create interactive brain network visualization.
        
        Args:
            brain: Brain instance to visualize
            
        Returns:
            Plot object or None if visualization not available
        """
        if 'brain_network' not in self.renderers:
            return None
        
        return self.renderers['brain_network'](brain)
    
    def create_training_monitor(self, brain: 'Brain') -> Any:
        """
        Create real-time training monitoring interface.
        
        Args:
            brain: Brain instance to monitor
            
        Returns:
            Monitor object or None if visualization not available
        """
        if not self.plotter:
            return None
        
        # Create a simple real-time monitoring plot
        if hasattr(self.plotter, 'subplots'):
            import matplotlib.pyplot as plt
            fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
            
            # Reward plot
            self.reward_line, = ax1.plot([], [])
            ax1.set_title('Reward over Time')
            ax1.set_xlabel('Step')
            ax1.set_ylabel('Reward')
            ax1.grid(True)
            
            # Neurons activity plot
            self.activity_line, = ax2.plot([], [])
            ax2.set_title('Active Neurons over Time')
            ax2.set_xlabel('Step')
            ax2.set_ylabel('Active Neurons')
            ax2.grid(True)
            
            plt.tight_layout()
            return fig
        
        return None
    
    def _create_matplotlib_brain_plot(self, brain: 'Brain'):
        """Create matplotlib-based brain network plot."""
        try:
            import matplotlib.pyplot as plt
            import matplotlib.patches as patches
            
            fig, ax = plt.subplots(figsize=(10, 8))
            
            # Simple visualization: show brain regions as circles
            # This would be expanded based on actual brain structure
            ax.set_xlim(-10, 10)
            ax.set_ylim(-10, 10)
            ax.set_aspect('equal')
            ax.set_title('Brain Network Visualization')
            ax.set_xlabel('X Position')
            ax.set_ylabel('Y Position')
            ax.grid(True)
            
            return fig
        except ImportError:
            return None
    
    def _create_plotly_brain_plot(self, brain: 'Brain'):
        """Create plotly-based brain network plot."""
        try:
            import plotly.graph_objects as go
            
            fig = go.Figure()
            
            # Add brain regions and connections
            # This would be expanded based on actual brain structure
            
            fig.update_layout(
                title='Brain Network Visualization',
                xaxis_title='X Position',
                yaxis_title='Y Position',
                showlegend=True
            )
            
            return fig
        except ImportError:
            return None
    
    def _create_bokeh_brain_plot(self, brain: 'Brain'):
        """Create bokeh-based brain network plot."""
        try:
            from bokeh.plotting import figure
            from bokeh.models import ColumnDataSource
            
            source = ColumnDataSource(data={'x': [], 'y': [], 'size': []})
            
            fig = figure(
                title='Brain Network Visualization',
                x_range=(-10, 10),
                y_range=(-10, 10),
                tools="pan,wheel_zoom,box_zoom,reset,save"
            )
            
            # Add circle glyphs
            fig.circle(x='x', y='y', size='size', source=source)
            
            return fig
        except ImportError:
            return None


class AdvancedBrain:
    """
    High-level interface for NLM neural simulation with intelligent defaults
    and comprehensive simulation control.
    """
    
    def __init__(self, config: Optional[Config] = None, 
                 enable_optimization: bool = True, 
                 verbose: bool = False):
        """
        Initialize brain with intelligent configuration management.
        
        Args:
            config: Configuration object (optional)
            enable_optimization: Enable performance optimizations
            verbose: Enable verbose logging
        """
        self._brain = None  # C++ brain interface
        self._config = config or Config()
        self._enable_optimization = enable_optimization
        self._verbose = verbose
        
        # Simulation state
        self._simulation_running = False
        self._episode_history: List[SimulationState] = []
        self._diagnostics: Dict[str, Any] = {}
        self._callbacks: Dict[str, Callable] = {}
        
        # Monitoring and visualization
        self.monitoring = MonitoringSystem()
        self.visualization = VisualizationIntegration()
        
        # Default configuration
        self._setup_default_configuration()
        
        logger.info("AdvancedBrain initialized")
    
    def _setup_default_configuration(self):
        """Setup default configuration values."""
        default_config = {
            'max_steps': 1000,
            'simulation_speed': 1.0,
            'reward_threshold': 0.0,
            'enable_visualization': False,
            'visualization_backend': 'matplotlib',
            'enable_monitoring': True,
            'monitoring_interval': 0.1,
            'enable_reward_modulation': True,
            'enable_structural_plasticity': False,
            'enable_development': True,
            'enable_curiosity': False
        }
        
        self._config.configure(**default_config)
    
    def configure_simulation(self, **kwargs: Any) -> 'AdvancedBrain':
        """
        Configure simulation parameters with validation.
        
        Args:
            **kwargs: Simulation parameters to configure
            
        Returns:
            Self for method chaining
            
        Raises:
            ValidationError: If configuration validation fails
        """
        self._config.configure(**kwargs)
        
        # Apply configuration to underlying brain if available
        if self._brain:
            self._apply_configuration()
        
        return self
    
    def _apply_configuration(self):
        """Apply configuration to underlying brain."""
        # This would apply configuration to the C++ brain interface
        pass
    
    def run_episode(self, max_steps: Optional[int] = None, 
                    render: bool = False, verbose: bool = None,
                    callbacks: Optional[Dict[str, Callable]] = None) -> Dict[str, Any]:
        """
        Run a complete simulation episode with advanced controls and monitoring.
        
        Args:
            max_steps: Maximum number of simulation steps (uses configured default if None)
            render: If True, render the simulation state for visualization
            verbose: If True, provide detailed progress output (uses instance verbose if None)
            callbacks: Dictionary of callback functions for simulation events
            
        Returns:
            Dictionary containing episode results including:
                - 'steps_completed': Number of steps actually executed
                - 'total_reward': Sum of rewards received during episode
                - 'success': Boolean indicating if episode completed successfully
                - 'diagnostics': Dictionary of performance and state metrics
                - 'history': List of step-wise state snapshots (if verbose=True)
                - 'metrics': Performance metrics collected during simulation
        """
        if verbose is None:
            verbose = self._verbose
        
        if max_steps is None:
            max_steps = self._config.get('max_steps', 1000)
        
        # Setup callbacks
        if callbacks:
            self._callbacks.update(callbacks)
        
        # Reset episode state
        self._episode_history.clear()
        self.reset_diagnostics()
        
        # Call before_episode callbacks
        if 'before_episode' in self._callbacks:
            self._callbacks['before_episode'](self)
        
        if self._verbose:
            logger.info(f"Starting episode with max_steps={max_steps}")
        
        # Initialize simulation metrics
        metrics = SimulationMetrics()
        metrics.max_steps = max_steps
        
        self._simulation_running = True
        
        try:
            # Run episode steps
            for step in range(max_steps):
                if not self._simulation_running:
                    break
                
                # Record state
                state = SimulationState(
                    step=step,
                    time=step * self._config.get('simulation_speed', 1.0),
                    neurons_active=self.get_active_neuron_count() if hasattr(self, 'get_active_neuron_count') else 0,
                    neurons_firing=self.get_firing_neuron_count() if hasattr(self, 'get_firing_neuron_count') else 0,
                    total_spikes=self.get_total_spike_count() if hasattr(self, 'get_total_spike_count') else 0
                )
                
                self._episode_history.append(state)
                
                # Call step callback
                if 'step' in self._callbacks:
                    self._callbacks['step'](state)
                
                # Execute simulation step
                # This would call the underlying C++ brain interface
                if self._brain:
                    self._brain.step(step)
                
                # Collect diagnostics
                if self._config.get('enable_monitoring', True):
                    self.monitoring.collect_metrics(self)
                
                # Render if requested
                if render and self.visualization.plotter:
                    self._render_frame(step)
                
                # Call after_step callbacks
                if 'after_step' in self._callbacks:
                    self._callbacks['after_step'](state)
            
            # Episode completed
            success = step == max_steps - 1
            
            # Call after_episode callbacks
            if 'after_episode' in self._callbacks:
                self._callbacks['after_episode'](self, state)
            
            if self._verbose:
                logger.info(f"Episode completed: steps={step + 1}, success={success}")
            
            # Compile results
            results = {
                'steps_completed': step + 1,
                'total_reward': metrics.total_reward,
                'success': success,
                'diagnostics': self.get_diagnostics(),
                'history': self._episode_history.copy() if verbose else [],
                'metrics': self.monitoring.get_system_status()
            }
            
            return results
            
        except Exception as e:
            logger.error(f"Episode interrupted with error: {e}")
            if 'on_error' in self._callbacks:
                self._callbacks['on_error'](e, self)
            
            raise NLMError(f"Episode failed: {e}") from e
        
        finally:
            self._simulation_running = False
    
    def _render_frame(self, step: int):
        """Render current simulation frame."""
        if not self.visualization.plotter:
            return
        
        # This would render the current simulation state
        # Implementation depends on visualization backend
        pass
    
    def run_episodes(self, num_episodes: int, max_steps_per_episode: int = 1000) -> List[Dict[str, Any]]:
        """
        Run multiple episodes with aggregated results.
        
        Args:
            num_episodes: Number of episodes to run
            max_steps_per_episode: Maximum steps per episode
            
        Returns:
            List of results dictionaries for each episode
        """
        results = []
        
        for episode in range(num_episodes):
            if self._verbose:
                logger.info(f"Running episode {episode + 1}/{num_episodes}")
            
            episode_results = self.run_episode(
                max_steps=max_steps_per_episode,
                render=self._config.get('enable_visualization', False),
                verbose=self._verbose
            )
            
            episode_results['episode_number'] = episode + 1
            results.append(episode_results)
            
            # Reset brain for next episode
            self.reset()
        
        return results
    
    def start_simulation(self, mode: str = 'episode'):
        """
        Start simulation with specified mode.
        
        Args:
            mode: Simulation mode ('episode', 'continuous', or 'custom')
        """
        if mode not in ['episode', 'continuous', 'custom']:
            raise ValueError(f"Invalid simulation mode: {mode}")
        
        self._simulation_running = True
        self._config.configure(simulation_mode=mode)
        
        if self._verbose:
            logger.info(f"Starting simulation in {mode} mode")
    
    def pause_simulation(self):
        """Pause the current simulation."""
        self._simulation_running = False
        logger.info("Simulation paused")
    
    def resume_simulation(self):
        """Resume paused simulation."""
        self._simulation_running = True
        logger.info("Simulation resumed")
    
    def stop_simulation(self):
        """Stop the simulation and cleanup resources."""
        self._simulation_running = False
        self._cleanup()
        logger.info("Simulation stopped")
    
    def run_continuous(self, max_steps: int = 0, duration: float = 0):
        """
        Run continuous simulation with optional limits.
        
        Args:
            max_steps: Maximum number of steps (0 for unlimited)
            duration: Maximum duration in seconds (0 for unlimited)
        """
        start_time = time.time()
        step_count = 0
        
        self.start_simulation('continuous')
        
        try:
            while (max_steps == 0 or step_count < max_steps) and \
                  (duration == 0 or (time.time() - start_time) < duration):
                
                if not self._simulation_running:
                    break
                
                # Execute one step
                if self._brain:
                    self._brain.step(step_count)
                
                step_count += 1
                
                # Collect metrics
                if self._config.get('enable_monitoring', True):
                    self.monitoring.collect_metrics(self)
                
                # Small delay to control simulation speed
                time.sleep(1.0 / self._config.get('simulation_speed', 1.0))
            
            logger.info(f"Continuous simulation completed: {step_count} steps")
            
        finally:
            self.stop_simulation()
    
    def get_diagnostics(self) -> Dict[str, Any]:
        """Get comprehensive system diagnostics."""
        diagnostics = {
            'simulation_state': {
                'running': self._simulation_running,
                'episode_count': len(self._episode_history),
                'current_step': self._episode_history[-1].step if self._episode_history else 0
            },
            'performance': self.monitoring.get_performance_stats(),
            'configuration': dict(self._config._parameters),
            'callbacks': list(self._callbacks.keys()),
            'custom_metrics': self._diagnostics
        }
        
        return diagnostics
    
    def reset_diagnostics(self):
        """Reset diagnostic counters."""
        self._diagnostics.clear()
        self.monitoring = MonitoringSystem()
    
    def add_callback(self, event: str, callback: Callable):
        """
        Add a callback function for simulation events.
        
        Args:
            event: Event name ('step', 'before_episode', 'after_episode', etc.)
            callback: Callback function
        """
        self._callbacks[event] = callback
    
    def remove_callback(self, event: str):
        """Remove callback for a specific event."""
        if event in self._callbacks:
            del self._callbacks[event]
    
    def get_active_neuron_count(self) -> int:
        """Get count of active neurons."""
        # This would query the underlying brain interface
        return 0
    
    def get_firing_neuron_count(self) -> int:
        """Get count of currently firing neurons."""
        # This would query the underlying brain interface
        return 0
    
    def get_total_spike_count(self) -> int:
        """Get total spike count."""
        # This would query the underlying brain interface
        return 0
    
    def reset(self):
        """Reset brain state."""
        if self._brain:
            self._brain.reset()
        
        self._episode_history.clear()
        self.reset_diagnostics()
        
        if self._verbose:
            logger.info("Brain reset")
    
    def _cleanup(self):
        """Cleanup resources."""
        # Cleanup visualization
        if hasattr(self.visualization, 'plotter') and self.visualization.plotter:
            if hasattr(self.visualization.plotter, 'close'):
                self.visualization.plotter.close()
        
        # Cleanup other resources
        pass
    
    def __enter__(self):
        """Enter context manager for automatic cleanup."""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit context manager and cleanup."""
        self._cleanup()
    
    async def async_step(self, steps: int = 1) -> List[Dict[str, Any]]:
        """
        Asynchronous step execution with results.
        
        Args:
            steps: Number of steps to execute
            
        Returns:
            List of step results
        """
        results = []
        
        for _ in range(steps):
            # Execute one step asynchronously
            if self._brain:
                # In real implementation, this would be async
                self._brain.step(0)
            
            # Collect metrics
            if self._config.get('enable_monitoring', True):
                self.monitoring.collect_metrics(self)
            
            results.append(self.monitoring.get_performance_stats())
        
        return results
    
    @contextmanager
    def simulation_context(self, duration: float = 0):
        """
        Context manager for simulation with automatic cleanup.
        
        Args:
            duration: Maximum duration for the simulation (0 for unlimited)
            
        Yields:
            Self for use within context
        """
        try:
            self.start_simulation('episode')
            if duration > 0:
                # Monitor duration
                start_time = time.time()
                while time.time() - start_time < duration and self._simulation_running:
                    time.sleep(0.1)
            
            yield self
        finally:
            self.stop_simulation()


class AgentBrain:
    """Complete agent-environment interface for intelligent agents."""
    
    def __init__(self, brain: AdvancedBrain):
        """
        Initialize agent brain interface.
        
        Args:
            brain: AdvancedBrain instance to control
        """
        self._brain = brain
        self._world = None
        self._sensory_percept = SensoryPercept()
        self._action_queue: List[Action] = []
        self._reward_history: List[float] = []
        
        # Agent state
        self._is_initialized = False
        self._development_enabled = True
        self._curiosity_enabled = False
        self._reward_modulation_enabled = True
        self._structural_plasticity_enabled = False
    
    def initialize(self, world: Any):
        """
        Initialize with world.
        
        Args:
            world: World instance to interact with
        """
        self._world = world
        self._is_initialized = True
        
        if self._brain._verbose:
            logger.info("AgentBrain initialized with world")
    
    def process_sensory_input(self, percept: SensoryPercept):
        """
        Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept to process
        """
        self._sensory_percept = percept
        
        # Convert sensory input to brain format (would need implementation)
        # self._brain.receive_sensory_input(sensory_input)
        
        if self._brain._verbose:
            logger.debug(f"Processed sensory input: vision shape={getattr(percept.vision, 'shape', 'N/A')}")
    
    def decode_motor_command(self) -> Action:
        """
        Decode brain motor activity into motor command.
        
        Returns:
            Action for the agent to execute
        """
        # This would decode brain activity into motor commands
        # For now, return a default action
        return Action(NLMTypes.ActionType.WAIT)
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float):
        """
        Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
        """
        self._reward_history.append(reward)
        
        # Apply reward modulation to brain
        # self._brain.apply_reward_modulation(reward, predicted_reward)
        
        if self._brain._verbose:
            logger.debug(f"Applied reward modulation: {reward} (predicted: {predicted_reward})")
    
    def update_development(self, timestep: float):
        """
        Update development system.
        
        Args:
            timestep: Time step for development update
        """
        if self._development_enabled:
            # Update development system
            # self._brain.update_development(timestep)
            pass
    
    def reset(self):
        """Reset agent for new episode."""
        self._action_queue.clear()
        self._reward_history.clear()
        self._sensory_percept = SensoryPercept()
        
        if self._brain._verbose:
            logger.info("AgentBrain reset")
    
    def enable_reward_modulation(self, enable: bool):
        """Enable or disable reward modulation."""
        self._reward_modulation_enabled = enable
    
    def enable_structural_plasticity(self, enable: bool):
        """Enable or disable structural plasticity."""
        self._structural_plasticity_enabled = enable
    
    def enable_development(self, enable: bool):
        """Enable or disable development."""
        self._development_enabled = enable
    
    def enable_curiosity(self, enable: bool):
        """Enable or disable curiosity."""
        self._curiosity_enabled = enable
    
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled."""
        return self._reward_modulation_enabled
    
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled."""
        return self._structural_plasticity_enabled
    
    def is_development_enabled(self) -> bool:
        """Check if development is enabled."""
        return self._development_enabled
    
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled."""
        return self._curiosity_enabled
    
    def get_brain(self) -> AdvancedBrain:
        """Get the underlying brain."""
        return self._brain
    
    def get_sensory_input_size(self) -> int:
        """Get expected sensory input size."""
        # This would be determined by the brain's sensory system
        return 0
    
    def get_motor_output_size(self) -> int:
        """Get expected motor output size."""
        # This would be determined by the brain's motor system
        return 0


class World:
    """Advanced world simulation with visualization."""
    
    def __init__(self):
        self.width: float = 100.0
        self.height: float = 100.0
        self.objects: List[WorldObject] = []
        self.agent_body: Optional[AgentBody] = None
        self.simulation_time: float = 0.0
        self.max_energy: float = 100.0
        self.energy_decay_rate: float = 0.01
        self.random_seed: Optional[int] = None
    
    def configure(self, width: float, height: float, 
                  vision_width: float = 10.0, vision_height: float = 10.0):
        """
        Configure world dimensions and vision parameters.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision sensor width
            vision_height: Vision sensor height
        """
        self.width = width
        self.height = height
        
        # Setup agent
        self.agent_body = AgentBody()
        self.agent_body.x = width / 2
        self.agent_body.y = height / 2
    
    def reset(self):
        """Reset world state."""
        self.objects.clear()
        self.agent_body.reset() if self.agent_body else None
        self.simulation_time = 0.0
    
    def set_agent_start(self, x: float, y: float):
        """Set agent starting position."""
        if self.agent_body:
            self.agent_body.x = x
            self.agent_body.y = y
    
    def update(self, timestep: float):
        """
        Update world state.
        
        Args:
            timestep: Time step for update
        """
        self.simulation_time += timestep
        
        if self.agent_body:
            self.agent_body.update(timestep)
            
            # Apply energy decay
            if self.agent_body.energy > 0:
                self.agent_body.energy *= (1.0 - self.energy_decay_rate)
    
    def apply_motor_command(self, action: Action, current_time: float):
        """
        Apply motor command to world.
        
        Args:
            action: Action to execute
            current_time: Current simulation time
        """
        if not self.agent_body:
            return
        
        # Apply action to agent body
        if action.type == NLMTypes.ActionType.MOVE_FORWARD:
            self.agent_body.velocity_x += 1.0
        elif action.type == NLMTypes.ActionType.MOVE_BACKWARD:
            self.agent_body.velocity_x -= 1.0
        elif action.type == NLMTypes.ActionType.TURN_LEFT:
            self.agent_body.angular_velocity += 0.1
        elif action.type == NLMTypes.ActionType.TURN_RIGHT:
            self.agent_body.angular_velocity -= 0.1
        elif action.type == NLMTypes.ActionType.LOOK:
            # Look action - update agent orientation
            pass
        
        self.agent_body.last_action_time = current_time
    
    def get_sensory_percept(self) -> SensoryPercept:
        """Get sensory percept from world."""
        percept = SensoryPercept()
        
        # Get vision data
        if self.agent_body:
            # Simple vision simulation
            vision_data = []
            for x in range(int(self.agent_body.x) - 5, int(self.agent_body.x) + 6):
                for y in range(int(self.agent_body.y) - 5, int(self.agent_body.y) + 6):
                    # Find object at this position
                    for obj in self.objects:
                        if abs(obj.x - x) < obj.radius and abs(obj.y - y) < obj.radius:
                            vision_data.append(obj.value)
                            break
                    else:
                        vision_data.append(0.0)
            
            # Set vision data (would need Vision class from C++ bindings)
            # percept.vision = Vision(len(vision_data), len(vision_data), 1)
            # percept.vision.setData(vision_data)
        
        return percept
    
    def get_agent_body(self) -> AgentBody:
        """Get agent body state."""
        return self.agent_body if self.agent_body else AgentBody()
    
    def add_object(self, obj: WorldObject):
        """Add object to world."""
        self.objects.append(obj)
    
    def remove_object(self, x: float, y: float):
        """Remove object at given position."""
        self.objects = [obj for obj in self.objects 
                       if abs(obj.x - x) >= obj.radius or abs(obj.y - y) >= obj.radius]
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid (not colliding with walls or obstacles)."""
        # Check boundaries
        if x < 0 or x >= self.width or y < 0 or y >= self.height:
            return False
        
        # Check collisions with objects
        for obj in self.objects:
            if obj.is_colliding(x, y):
                return False
        
        return True
    
    def get_width(self) -> float:
        """Get world width."""
        return self.width
    
    def get_height(self) -> float:
        """Get world height."""
        return self.height
    
    def get_max_energy(self) -> float:
        """Get maximum energy."""
        return self.max_energy
    
    def set_max_energy(self, e: float):
        """Set maximum energy."""
        self.max_energy = e
    
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate."""
        return self.energy_decay_rate
    
    def set_energy_decay_rate(self, r: float):
        """Set energy decay rate."""
        self.energy_decay_rate = r
    
    def get_simulation_time(self) -> float:
        """Get simulation time."""
        return self.simulation_time
    
    def set_random_seed(self, seed: int):
        """Set random seed."""
        self.random_seed = seed
    
    def get_random_seed(self) -> Optional[int]:
        """Get random seed."""
        return self.random_seed


# Create convenience functions
def create_default_config() -> Config:
    """Create a default configuration."""
    return Config()


def create_brain(config: Optional[Config] = None) -> AdvancedBrain:
    """Create a new brain with configuration."""
    return AdvancedBrain(config)


def create_simple_world() -> World:
    """Create a new simple world."""
    return World()


def create_agent_brain(brain: AdvancedBrain) -> AgentBrain:
    """Create a new agent brain interface."""
    return AgentBrain(brain)


def create_simulation():
    """
    Create a complete simulation with brain, world, and agent brain.
    
    Returns:
        Tuple of (brain, world, agent_brain)
    """
    brain = create_brain()
    world = create_simple_world()
    agent_brain = create_agent_brain(brain)
    
    agent_brain.initialize(world)
    
    return brain, world, agent_brain


# For backward compatibility - create a simple wrapper for the C++ bindings
try:
    import pybind11
    from pybind11 import get_current_pybind11_module
    
    # Import the C++ module
    import pynlm
    
    # Create wrapper classes that delegate to C++ implementation
    class CCPPBrainWrapper:
        """Wrapper for C++ Brain implementation."""
        
        def __init__(self, config: Optional[Config] = None):
            self._cpp_brain = pynlm.create_brain(config._parameters if config else {})
        
        def step(self, step: int):
            self._cpp_brain.step(step)
        
        def reset(self):
            self._cpp_brain.reset()
        
        def get_active_neuron_count(self) -> int:
            return self._cpp_brain.get_active_neuron_count()
        
        def get_firing_neuron_count(self) -> int:
            return self._cpp_brain.get_firing_neuron_count()
        
        def get_total_spike_count(self) -> int:
            return self._cpp_brain.get_total_spike_count()
    
    # Add to AdvancedBrain class
    AdvancedBrain._cpp_available = True
    
except ImportError:
    AdvancedBrain._cpp_available = False
    logger.warning("C++ NLM bindings not available. Using pure Python implementation.")


# Version information
__version__ = "2.0.0"
__author__ = "Enhanced NLM Python Bindings Team"
__description__ = "Enhanced Python bindings for NLM neural simulation framework"


if __name__ == "__main__":
    # Simple test when run directly
    print(f"NLM Python Bindings v{__version__}")
    print(__description__)
    
    # Test basic functionality
    brain = AdvancedBrain(verbose=True)
    print(f"Brain created: {brain}")
    
    config = Config()
    config.configure(max_steps=100, simulation_speed=1.0)
    print(f"Config created: {config.summary()}")