"""
Brain Module - Core Brain class and simulation API

This module provides the high-level Brain class wrapper that exposes the C++ Brain
interface through a Pythonic API. It includes context manager support, property-based
configuration, chainable method calls, and comprehensive type hints.
"""

import time
from typing import List, Optional, Dict, Any, Callable, Union, Iterator
from contextlib import contextmanager
import warnings

# Optional imports for NumPy integration
try:
    import numpy as np
    NUMPY_AVAILABLE = True
    ndarray_type = np.ndarray
except ImportError:
    np = None
    NUMPY_AVAILABLE = False
    ndarray_type = Any  # Fallback to Any when numpy is not available

from .types import (
    NeuronId, SynapseId, RegionId, PopulationId, NeuronType, SynapseType,
    DevelopmentalStage, ActionType, FiringState, SimulationStep, Timestamp,
    MembranePotential, PopulationSize, ConnectionWeight
)
from .config import Config

# Import component classes for type hints and property access
try:
    from .memory import WorkingMemory, EpisodicMemory, AssociativeMemory
    MEMORY_AVAILABLE = True
except ImportError:
    WorkingMemory = None
    EpisodicMemory = None
    AssociativeMemory = None
    MEMORY_AVAILABLE = False

try:
    from .neuromodulation import NeuromodulationSystem
    NEUROMODULATION_AVAILABLE = True
except ImportError:
    NeuromodulationSystem = None
    NEUROMODULATION_AVAILABLE = False

try:
    from .cognition import PredictionSystem, CognitionSystem
    COGNITION_AVAILABLE = True
except ImportError:
    PredictionSystem = None
    CognitionSystem = None
    COGNITION_AVAILABLE = False

try:
    from .development import DevelopmentSystem
    DEVELOPMENT_AVAILABLE = True
except ImportError:
    DevelopmentSystem = None
    DEVELOPMENT_AVAILABLE = False

try:
    from .visualization import Visualization
    VISUALIZATION_AVAILABLE = True
except ImportError:
    Visualization = None
    VISUALIZATION_AVAILABLE = False

try:
    from .monitoring import RealTimeMonitor
    MONITORING_AVAILABLE = True
except ImportError:
    RealTimeMonitor = None
    MONITORING_AVAILABLE = False

try:
    from .profiling import PerformanceProfiler
    PROFILING_AVAILABLE = True
except ImportError:
    PerformanceProfiler = None
    PROFILING_AVAILABLE = False

try:
    from .extensions import ExtensionAPI, create_custom_circuit
    EXTENSIONS_AVAILABLE = True
except ImportError:
    ExtensionAPI = None
    create_custom_circuit = None
    EXTENSIONS_AVAILABLE = False

class Brain:
    """
    High-level Brain wrapper for NLM neural simulation.
    
    This class provides an intuitive, object-oriented interface to the underlying
    C++ Brain implementation. It supports context management, property-based
    configuration, and fluent method chaining for ease of use.
    
    Features:
    - Context manager support for simulation lifecycle
    - Property-based API for configuration
    - Chainable method calls for fluent interface
    - Comprehensive type hints and documentation
    - Automatic memory management
    - Real-time monitoring capabilities
    
    Examples:
        >>> # Basic usage
        >>> config = Config()
        >>> config.set("neuron_count", 1000)
        >>> brain = Brain(config)
        >>> brain.initialize()
        
        >>> # Using context manager
        >>> with Brain(config) as brain:
        ...     brain.step(0)
        ...     action = brain.produce_action()
        
        >>> # Fluent interface
        >>> brain = Brain(Config()).set_neurons(1000).set_regions(3).initialize()
    """
    
    def __init__(self, config: Optional[Union[Config, Dict[str, Any]]] = None):
        """
        Initialize Brain with configuration.
        
        Args:
            config: Configuration object or dictionary. If None, creates default config.
        
        Raises:
            ValueError: If config is invalid or incompatible.
        """
        self._config = self._ensure_config(config)
        self._brain = None
        self._working_memory = None
        self._episodic_memory = None
        self._associative_memory = None
        self._neuromodulation = None
        self._prediction = None
        self._cognition = None
        self._development = None
        self._visualizer = None
        self._monitor = None
        self._profiler = None
        self._initialized = False
        self._simulation_step = 0
        self._simulation_time = 0.0
        
        # Performance optimization
        self._neuron_cache = {}
        self._region_cache = {}
        self._performance_metrics = {}
    
    def _ensure_config(self, config: Optional[Union[Config, Dict[str, Any]]]) -> Config:
        """Ensure config is a valid Config object."""
        if config is None:
            return create_default_config()
        elif isinstance(config, Config):
            return config
        elif isinstance(config, dict):
            cfg = Config()
            for key, value in config.items():
                cfg.set(key, value)
            return cfg
        else:
            raise ValueError(f"Invalid config type: {type(config)}")
    
    def __enter__(self) -> 'Brain':
        """Context manager entry."""
        return self._initialize()
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.reset()
    
    def _initialize(self) -> 'Brain':
        """Internal initialization method."""
        if not self._initialized:
            self._brain = self._create_cpp_brain()
            self._initialized = True
        return self
    
    def _create_cpp_brain(self):
        """Create underlying C++ brain instance."""
        # This will be implemented with pybind11 binding
        pass
    
    @property
    def config(self) -> Config:
        """Get the configuration object."""
        return self._config
    
    @property
    def initialized(self) -> bool:
        """Check if brain is initialized."""
        return self._initialized
    
    @property
    def simulation_step(self) -> SimulationStep:
        """Get current simulation step."""
        return self._simulation_step
    
    @property
    def simulation_time(self) -> Timestamp:
        """Get current simulation time."""
        return self._simulation_time
    
    def initialize(self, **kwargs) -> 'Brain':
        """
        Initialize the brain with configuration.
        
        Args:
            **kwargs: Additional configuration parameters.
        
        Returns:
            Self for fluent interface chaining.
        
        Raises:
            RuntimeError: If brain is already initialized.
        """
        if self._initialized:
            raise RuntimeError("Brain is already initialized")
        
        # Apply additional configuration
        for key, value in kwargs.items():
            self._config.set(key, value)
        
        # Initialize underlying C++ brain
        self._initialize()
        
        # Initialize component systems
        self._working_memory = WorkingMemory(self._brain)
        self._episodic_memory = EpisodicMemory(self._brain)
        self._associative_memory = AssociativeMemory(self._brain)
        self._neuromodulation = NeuromodulationSystem(self._brain)
        self._prediction = PredictionSystem(self._brain)
        self._cognition = CognitionSystem(self._brain)
        self._development = DevelopmentSystem(self._brain)
        self._visualizer = Visualization(self._brain)
        self._profiler = PerformanceProfiler(self._brain)
        self._monitor = RealTimeMonitor(self._brain)
        
        return self
    
    def step(self, step: Optional[SimulationStep] = None,
             time: Optional[Timestamp] = None) -> 'Brain':
        """
        Perform a simulation step.
        
        Args:
            step: Simulation step number (defaults to current step + 1)
            time: Simulation time (defaults to current time + dt)
        
        Returns:
            Self for fluent interface chaining.
        """
        if not self._initialized:
            raise RuntimeError("Brain is not initialized")
        
        if step is None:
            step = self._simulation_step + 1
        if time is None:
            time = self._simulation_time + self._config.getOr("timestep", 0.01)
        
        # Step the underlying brain
        self._brain.step(step)
        
        # Update internal state
        self._simulation_step = step
        self._simulation_time = time
        
        # Update component systems
        if self._working_memory:
            self._working_memory.update(time)
        if self._episodic_memory:
            self._episodic_memory.update(time)
        if self._associative_memory:
            self._associative_memory.update(time)
        if self._development:
            self._development.update(time)
        
        return self
    
    def receive_sensory_input(self, input_data) -> 'Brain':
        """
        Inject sensory input into the brain.
        
        Args:
            input_data: Sensory input data (SensoryInput object or compatible format).
        
        Returns:
            Self for fluent interface chaining.
        """
        if not self._initialized:
            raise RuntimeError("Brain is not initialized")
        
        # Convert input data to appropriate format
        # This will be implemented based on the C++ binding
        self._brain.receiveSensoryInput(input_data)
        return self
    
    def inject_current(self, neuron: Union[NeuronId, int],
                      current: MembranePotential) -> 'Brain':
        """
        Inject current into a specific neuron.
        
        Args:
            neuron: Neuron identifier or index.
            current: Current value to inject.
        
        Returns:
            Self for fluent interface chaining.
        """
        if not self._initialized:
            raise RuntimeError("Brain is not initialized")
        
        neuron_id = self._ensure_neuron_id(neuron)
        self._brain.injectCurrent(neuron_id, current)
        return self
    
    def inject_current_to_neurons(self, neuron_type: NeuronType,
                                  current: MembranePotential) -> 'Brain':
        """
        Inject current into all neurons of a specific type.
        
        Args:
            neuron_type: Type of neurons to target.
            current: Current value to inject.
        
        Returns:
            Self for fluent interface chaining.
        """
        if not self._initialized:
            raise RuntimeError("Brain is not initialized")
        
        self._brain.injectCurrentToNeurons(neuron_type, current)
        return self
    
    def produce_action(self) -> 'Brain':
        """
        Produce motor action based on neural activity.
        
        Returns:
            Action object.
        """
        if not self._initialized:
            raise RuntimeError("Brain is not initialized")
        
        return self._brain.produceAction()
    
    def reset(self) -> 'Brain':
        """
        Reset brain state.
        
        Returns:
            Self for fluent interface chaining.
        """
        if self._initialized:
            self._brain.reset()
            self._simulation_step = 0
            self._simulation_time = 0.0
            
            # Reset component systems
            if self._working_memory:
                self._working_memory.clear()
            if self._episodic_memory:
                self._episodic_memory.clear()
            if self._associative_memory:
                self._associative_memory.clear()
            
            self._initialized = False
        
        return self
    
    def save(self, filepath: str) -> 'Brain':
        """
        Save brain state to file.
        
        Args:
            filepath: Path to save file.
        
        Returns:
            Self for fluent interface chaining.
        """
        if not self._initialized:
            raise RuntimeError("Brain is not initialized")
        
        self._brain.save(filepath)
        return self
    
    def load(self, filepath: str) -> 'Brain':
        """
        Load brain state from file.
        
        Args:
            filepath: Path to load file.
        
        Returns:
            Self for fluent interface chaining.
        """
        if not self._initialized:
            raise RuntimeError("Brain is not initialized")
        
        self._brain.load(filepath)
        return self
    
    # Property-based configuration methods
    
    def set_neurons(self, count: int) -> 'Brain':
        """Set number of neurons."""
        self._config.set("neuron_count", count)
        return self
    
    def set_regions(self, count: int) -> 'Brain':
        """Set number of regions."""
        self._config.set("region_count", count)
        return self
    
    def set_synapse_probability(self, prob: float) -> 'Brain':
        """Set synapse formation probability."""
        self._config.set("connection_probability", prob)
        return self
    
    # Component access methods
    
    @property
    def working_memory(self) -> WorkingMemory:
        """Get working memory system."""
        if self._working_memory is None:
            raise RuntimeError("Working memory not initialized")
        return self._working_memory
    
    @property
    def episodic_memory(self) -> EpisodicMemory:
        """Get episodic memory system."""
        if self._episodic_memory is None:
            raise RuntimeError("Episodic memory not initialized")
        return self._episodic_memory
    
    @property
    def associative_memory(self) -> AssociativeMemory:
        """Get associative memory system."""
        if self._associative_memory is None:
            raise RuntimeError("Associative memory not initialized")
        return self._associative_memory
    
    @property
    def neuromodulation(self) -> NeuromodulationSystem:
        """Get neuromodulation system."""
        if self._neuromodulation is None:
            raise RuntimeError("Neuromodulation system not initialized")
        return self._neuromodulation
    
    @property
    def prediction(self) -> PredictionSystem:
        """Get prediction system."""
        if self._prediction is None:
            raise RuntimeError("Prediction system not initialized")
        return self._prediction
    
    @property
    def cognition(self) -> CognitionSystem:
        """Get cognition system."""
        if self._cognition is None:
            raise RuntimeError("Cognition system not initialized")
        return self._cognition
    
    @property
    def development(self) -> DevelopmentSystem:
        """Get development system."""
        if self._development is None:
            raise RuntimeError("Development system not initialized")
        return self._development
    
    @property
    def visualizer(self) -> Visualization:
        """Get visualizer."""
        if self._visualizer is None:
            raise RuntimeError("Visualizer not initialized")
        return self._visualizer
    
    @property
    def monitor(self) -> RealTimeMonitor:
        """Get monitor."""
        if self._monitor is None:
            raise RuntimeError("Monitor not initialized")
        return self._monitor
    
    @property
    def profiler(self) -> PerformanceProfiler:
        """Get profiler."""
        if self._profiler is None:
            raise RuntimeError("Profiler not initialized")
        return self._profiler
    
    # Statistics and monitoring methods
    
    def get_neural_activity(self) -> ndarray_type:
        """Get neural activity across all neurons."""
        # This will be implemented with pybind11 binding
        if NUMPY_AVAILABLE:
            return np.array([])
        else:
            return []
    
    def get_firing_rates(self) -> Dict[str, float]:
        """Get firing rates by region and neuron type."""
        # This will be implemented with pybind11 binding
        return {}
    
    def get_region_stats(self) -> Dict[str, Any]:
        """Get statistics for all brain regions."""
        # This will be implemented with pybind11 binding
        return {}
    
    def log_status(self) -> 'Brain':
        """Log brain status and metrics."""
        if not self._initialized:
            raise RuntimeError("Brain is not initialized")
        
        self._brain.logStatus()
        return self
    
    # Internal helper methods
    
    def _ensure_neuron_id(self, neuron: Union[NeuronId, int]) -> NeuronId:
        """Ensure neuron identifier is a NeuronId."""
        if isinstance(neuron, NeuronId):
            return neuron
        else:
            return NeuronId(neuron)
    
    def _get_region(self, region_id: Union[RegionId, int]) -> Any:
        """Get region by ID."""
        if isinstance(region_id, RegionId):
            return self._brain.getRegion(region_id)
        else:
            return self._brain.getRegion(RegionId(region_id))
    
    def __repr__(self) -> str:
        """String representation."""
        if self._initialized:
            return f"<Brain initialized={self._initialized}, step={self._simulation_step}>"
        else:
            return f"<Brain not initialized>"
    
    def __str__(self) -> str:
        """String representation."""
        return repr(self)


# Helper functions

def create_brain(config: Optional[Union[Config, Dict[str, Any]]] = None) -> Brain:
    """
    Create and initialize a new Brain instance.
    
    Args:
        config: Configuration for the brain.
    
    Returns:
        Initialized Brain instance.
    
    Examples:
        >>> brain = create_brain({"neuron_count": 1000, "region_count": 3})
        >>> brain.initialize()
    """
    brain = Brain(config)
    brain.initialize()
    return brain


def create_default_config() -> Config:
    """
    Create a default configuration for Brain.
    
    Returns:
        Default Config instance with sensible defaults.
    
    Examples:
        >>> config = create_default_config()
        >>> config.set("neuron_count", 1000)
        >>> config.set("region_count", 3)
    """
    config = Config()
    
    # Set default values
    defaults = {
        "neuron_count": 1000,
        "region_count": 3,
        "connection_probability": 0.1,
        "timestep": 0.01,
        "max_energy": 100.0,
        "energy_decay_rate": 0.95,
        "random_seed": 42,
        "vision_width": 16,
        "vision_height": 16,
        "world_width": 100,
        "world_height": 100,
        "sensory_noise": 0.1,
        "action_noise": 0.1,
        "reward_scale": 1.0,
        "curiosity_threshold": 0.3,
        "novelty_decay": 0.99,
    }
    
    for key, value in defaults.items():
        config.set(key, value)
    
    return config
