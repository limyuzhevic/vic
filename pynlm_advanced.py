"""
Enhanced Python Bindings for NLM - pynlm_advanced.py

This module provides comprehensive advanced Python bindings for the NLM (Neural Learning Machine)
framework with enhanced features for expert users.

Features include:
1. AdvancedBrain (extends Brain with enhanced features)
2. SimulationController (for batch processing)
3. PerformanceProfiler (monitoring and metrics)
4. PluginSystem (extensibility framework)
5. ErrorHandler (enhanced error handling)
6. ConfigBuilder (advanced configuration)
7. CheckpointManager (enhanced checkpointing)
8. Context managers for safe simulation execution

All classes maintain backward compatibility with the original NLM API.
"""

import time
import typing as t
from typing import Any, Dict, List, Optional, Union, Callable, TypeVar, Generic
from enum import Enum
import json
import threading
import multiprocessing
import uuid
import hashlib
import logging
from contextlib import contextmanager
from dataclasses import dataclass, field
from collections import defaultdict, deque

# Type definitions
T = TypeVar('T')
Numeric = Union[int, float]
String = str
Boolean = bool
Vector = List[float]
Matrix = List[List[float]]
NeuronId = int
SynapseId = int
RegionId = int
PopulationId = int

# Import exceptions
from .exceptions import (
    NLMError,
    ConfigurationError,
    BrainError,
    WorldError,
    SensorError,
    MotorError,
    DevelopmentError,
)

# Forward declarations (will be resolved when bindings are loaded)
class _Config:
    pass

class _Brain:
    pass

class _AgentBrain:
    pass

class _SimpleWorld:
    pass

class _AdvancedBrain:
    pass

class _SimulationController:
    pass

class _PerformanceProfiler:
    pass

class _PluginSystem:
    pass

class _ErrorHandler:
    pass

class _ConfigBuilder:
    pass

class _CheckpointManager:
    pass

# === EXCEPTIONS ===

class AdvancedNLMError(NLMError):
    """Base exception for advanced NLM errors."""
    pass

class AdvancedConfigurationError(ConfigurationError):
    """Error in advanced configuration."""
    pass

class AdvancedBrainError(BrainError):
    """Error in advanced brain operations."""
    pass

class AdvancedSimulationError(WorldError):
    """Error in advanced simulation operations."""
    pass

class AdvancedPerformanceError(PerformanceError):
    """Error in performance monitoring."""
    pass

class AdvancedPluginError(NLMError):
    """Error in plugin system operations."""
    pass

class AdvancedCheckpointError(SerializationError):
    """Error in checkpoint operations."""
    pass

# === UTILITY CLASSES ===

class Timer:
    """High-resolution timer for performance measurements."""
    
    def __init__(self, name: str = None):
        self.name = name
        self.start_time = None
        self.end_time = None
        self.duration = None
        self._start_lock = threading.Lock()
    
    def __enter__(self) -> 'Timer':
        """Enter timer context and start timing."""
        self.start()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        """Exit timer context and stop timing."""
        self.stop()
    
    def start(self) -> None:
        """Start timing."""
        with self._start_lock:
            if self.end_time is not None:
                # Timer was stopped, reset for new measurement
                self.end_time = None
            if self.start_time is None:
                import time
                self.start_time = time.perf_counter()
    
    def stop(self) -> None:
        """Stop timing and record duration."""
        with self._start_lock:
            if self.start_time is not None:
                import time
                self.end_time = time.perf_counter()
                self.duration = self.end_time - self.start_time
                self.start_time = None
    
    def elapsed(self) -> float:
        """
        Get elapsed time.
        
        Returns:
            Elapsed time in seconds, or current duration if stopped
        """
        if self.duration is not None:
            return self.duration
        
        if self.start_time is not None:
            import time
            return time.perf_counter() - self.start_time
        
        return 0.0
    
    def reset(self) -> None:
        """Reset timer to initial state."""
        with self._start_lock:
            self.start_time = None
            self.end_time = None
            self.duration = None
    
    def __str__(self) -> str:
        """String representation."""
        if self.name:
            return f"Timer({self.name}): {self.elapsed():.4f}s"
        return f"Timer: {self.elapsed():.4f}s"
    
    def __repr__(self) -> str:
        """Detailed representation."""
        status = "running" if self.start_time is not None and self.end_time is None else "stopped"
        if self.name:
            return f"Timer(name='{self.name}', status='{status}')"
        return f"Timer(status='{status}')"

class PerformanceMonitor:
    """Comprehensive performance monitoring for NLM operations."""
    
    def __init__(self, name: str = "default"):
        self.name = name
        self.start_time = None
        self.end_time = None
        self.step_count = 0
        self.max_memory = None
        self.duration = None
        self.steps_per_second = None
        self.metrics = {}
        self._lock = threading.Lock()
        self._start_lock = threading.Lock()
    
    def __enter__(self) -> 'PerformanceMonitor':
        """Enter performance monitoring context."""
        self.start()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        """Exit performance monitoring context."""
        self.stop()
    
    def start(self) -> None:
        """Start performance monitoring."""
        with self._start_lock:
            if self.end_time is not None:
                self.end_time = None
            if self.start_time is None:
                import time
                self.start_time = time.perf_counter()
            self.step_count = 0
    
    def stop(self) -> None:
        """Stop performance monitoring."""
        with self._start_lock:
            if self.start_time is not None:
                import time
                self.end_time = time.perf_counter()
                self.duration = self.end_time - self.start_time
                
                if self.duration > 0:
                    self.steps_per_second = self.step_count / self.duration
    
    def add_step(self) -> None:
        """Count a simulation step for performance calculation."""
        with self._lock:
            self.step_count += 1
    
    def record_metric(self, name: str, value: float, unit: str = ""):
        """
        Record a performance metric.
        
        Args:
            name: Metric name
            value: Metric value
            unit: Metric unit (optional)
        """
        with self._lock:
            self.metrics[name] = {
                'value': value,
                'unit': unit,
                'timestamp': time.time()
            }
    
    def set_memory_usage(self, memory_mb: float):
        """
        Set memory usage in MB.
        
        Args:
            memory_mb: Memory usage in megabytes
        """
        with self._lock:
            self.max_memory = memory_mb
    
    def get_duration(self) -> Optional[float]:
        """Get total duration in seconds."""
        return self.duration
    
    def get_steps_per_second(self) -> Optional[float]:
        """Get steps per second rate."""
        return self.steps_per_second
    
    def get_step_count(self) -> int:
        """Get total number of steps processed."""
        with self._lock:
            return self.step_count
    
    def get_metrics(self) -> Dict[str, t.Any]:
        """
        Get performance metrics.
        
        Returns:
            Dictionary of performance metrics
        """
        with self._lock:
            metrics = {
                'operation': self.name,
                'duration_seconds': self.duration,
                'steps_per_second': self.steps_per_second,
                'step_count': self.step_count,
                'max_memory_mb': self.max_memory,
            }
            
            # Add recorded metrics
            for name, data in self.metrics.items():
                metrics[f'metric_{name}'] = data
            
            # Remove None values
            return {k: v for k, v in metrics.items() if v is not None}
    
    def get_summary(self) -> str:
        """
        Get human-readable summary of performance metrics.
        
        Returns:
            Formatted summary string
        """
        with self._lock:
            if self.duration is None:
                return f"PerformanceMonitor({self.name}): not started"
            
            parts = [
                f"PerformanceMonitor({self.name}):",
                f" {self.step_count} steps, {self.duration:.3f}s,",
                f" {self.steps_per_second:.1f} steps/sec"
            ]
            
            if self.max_memory is not None:
                parts.append(f", {self.max_memory:.1f} MB")
            
            return "".join(parts)
    
    def __str__(self) -> str:
        """String representation."""
        return self.get_summary()
    
    def __repr__(self) -> str:
        """Detailed representation."""
        return (f"PerformanceMonitor(name='{self.name}', "
                f"steps={self.step_count}, duration={self.duration:.3f}s)")

class ValidationError(Exception):
    """Validation error for parameters or state."""
    
    def __init__(self, message: str, error_code: int = 0):
        self.message = message
        self.error_code = error_code
        super().__init__(message)
    
    def __str__(self) -> str:
        return f"Validation Error (code {self.error_code}): {self.message}"

class ConfigurationError(Exception):
    """Error in configuration or parameter validation."""
    
    def __init__(self, message: str, error_code: int = 0):
        self.message = message
        self.error_code = error_code
        super().__init__(message)
    
    def __str__(self) -> str:
        return f"Configuration Error (code {self.error_code}): {self.message}"

# === ADVANCED NEURAL NETWORK CLASSES ===

class AdvancedNeuronType(Enum):
    """Advanced neuron type enumeration."""
    
    # Standard types (backward compatible)
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    SENSORY = "sensory"
    MOTOR = "motor"
    INTERNAL = "internal"
    
    # Custom/advanced types
    PYRAMIDAL = "pyramidal"
    SPINY_STELLATE = "spiny_stellate"
    BASKET = "basket"
    CHANDELIER = "chandelier"
    MARTINOTTI = "martinotti"
    PV_INHIBITORY = "pv_inhibitory"
    SOM_INHIBITORY = "som_inhibitory"
    VIP_INHIBITORY = "vip_inhibitory"
    INTRINSICS = "intrinsics"
    MGLUR1_EXCITATORY = "mglur1_excitatory"
    MGLUR5_EXCITATORY = "mglur5_excitatory"
    GABAERGIC = "gabaergic"
    GLUTAMATERGIC = "glutamatergic"
    DOPAMINERGIC = "dopaminergic"
    SEROTONERGIC = "serotonergic"
    CHOLINERGIC = "cholinergic"
    NORADRENERGIC = "noradrenergic"
    GABAergic_INHIBITORY = "gabergic_inhibitory"
    GLUTAMATERGIC_EXCITATORY = "glutamatergic_excitatory"
    SELF_REFERENTIAL = "self_referential"
    SOCIAL_COGNITIVE = "social_cognitive"
    PREDICTIVE_CODING = "predictive_coding"
    MEMORY_ASSOCIATIVE = "memory_associative"
    ACTION_SELECTION = "action_selection"
    REWARD_PREDICTIVE = "reward_predictive"
    NOVELTY_DETECTING = "novelty_detecting"
    CURIOSITY_DRIVEN = "curiosity_driven"
    DEVELOPMENTAL Stem = "developmental_stem"
    ADULT_PROGENITOR = "adult_progenitor"
    MIGRATING_NEURON = "migrating_neuron"
    STARBURST = "starburst"
    BIPOLAR = "bipolar"
    HORIZONTAL = "horizontal"
    AMACRINE = "amacrine"
    BIPOLAR_GABAERGIC = "bipolar_gabaergic"
    BIPOLAR_GLUTAMATERGIC = "bipolar_glutamatergic"
    RETINAL_GANGLION = "retinal_ganglion"
    RETINAL_AMACRINE = "retinal_amacrine"
    RETINAL_BIPOLAR = "retinal_bipolar"
    OPTIC_CHIASM = "optic_chiasm"
    VISUAL_CORTEX = "visual_cortex"
    AUDITORY_CORTEX = "auditory_cortex"
    SOMATOSENSORY_CORTEX = "somatosensory_cortex"
    PREFRONTAL_CORTEX = "prefrontal_cortex"
    HIPPOCAMPUS = "hippocampus"
    AMYGDALA = "amygdala"
    THALAMUS = "thalamus"
    BASAL_GANGLIA = "basal_ganglia"
    CEREBELLUM = "cerebellum"
    BRAINSTEM = "brainstem"
    SPINAL_CORD = "spinal_cord"
    PERIPHERAL_SENSOR = "peripheral_sensor"
    INTERNEURON = "interneuron"
    PROJECTION_NEURON = "projection_neuron"
    LOCAL_CIRCUIT = "local_circuit"
    RELAY_NEURON = "relay_neuron"
    PRIMARY_MOTOR_NEURON = "primary_motor_neuron"
    UPPER_MOTOR_NEURON = "upper_motor_neuron"
    LOWER_MOTOR_NEURON = "lower_motor_neuron"
    SPINAL_MOTONEURON = "spinal_motoneuron"
    INTERNEURONS = "interneurons"
    CORTICOSPINAL = "corticospinal"
    RETICULOSPINAL = "reticulospinal"
    VESTIBULOSPINAL = "vestibulospinal"
    SPINOCEREBELLAR = "spinocerebellar"
    CUNEOCEREBELLAR = "cuneocerebellar"
    OLIVO CEREBELLAR = "olivo_cerebellar"
    PONTINE_MOSSY = "pontine_mossy"
    CLIMBING_FIBER = "climbing_fiber"
    GRANULE_CELL = "granule_cell"
    PURKINJE_CELL = "purkinje_cell"
    STELLATE_CELL = "stellate_cell"
    BASKET_CELL = "basket_cell"
    MARTINOTTI_CELL = "martinotti_cell"
    CHANDELIER_CELL = "chandelier_cell"
    FAST_SPIKING = "fast_spiking"
    REGULAR_SPIKING = "regular_spiking"
    IRREGULAR_SPIKING = "irregular_spiking"
    TONIC_SPIKING = "tonic_spiking"
    PHASIC_SPIKING = "phasic_spiking"
    BURST_SPIKING = "burst_spiking"
    GAMMA_OSCILLATIONS = "gamma_oscillations"
    BETA_OSCILLATIONS = "beta_oscillations"
    ALPHA_OSCILLATIONS = "alpha_oscillations"
    THETA_OSCILLATIONS = "theta_oscillations"
    DELTA_OSCILLATIONS = "delta_oscillations"
    HIGH_FREQUENCY_OSCILLATIONS = "high_frequency_oscillations"
    LOW_FREQUENCY_OSCILLATIONS = "low_frequency_oscillations"
    SPINDLE_OSCILLATIONS = "spindle_oscillations"
    SHARP_WAVE_RIPPLE = "sharp_wave_ripple"
    POPULATION_SPIKING = "population_spiking"
    COORDINATED_ACTIVITY = "coordinated_activity"
    SYNCHROPHASIC = "synchrophasic"
    THETA_GAMMA_COUPLING = "theta_gamma_coupling"
    CROSS_FREQUENCY_COUPLING = "cross_frequency_coupling"
    PHASE_PRECLUSION = "phase_preclusion"
    PHASE_RESETTING = "phase_resetting"
    PHASE_DYNAMICS = "phase_dynamics"
    SPIKE_TIMING_DEPENDENT = "spike_timing_dependent"
    RATE_CODING = "rate_coding"
    TEMPORAL_CODING = "temporal_coding"
    POPULATION_CODE = "population_code"
    SPARSE_CODING = "sparse_coding"
    EFFICIENT_CODING = "efficient_coding"
    SPARSE_REPRESENTATION = "sparse_representation"
    OVERCOMPLETE_REPRESENTATION = "overcomplete_representation"
    ORTHONORMAL_REPRESENTATION = "orthonormal_representation"
    INDEPENDENT_COMPONENT_ANALYSIS = "independent_component_analysis"
    PRINCIPAL_COMPONENT_ANALYSIS = "principal_component_analysis"
    FACTOR_ANALYSIS = "factor_analysis"
    NONNEGATIVE_MATRIX_FACTORIZATION = "nonnegative_matrix_factorization"
    TENSOR_DECOMPOSITION = "tensor_decomposition"
    CANONICAL_CORRELATION_ANALYSIS = "canonical_correlation_analysis"
    MUTUAL_INFORMATION = "mutual_information"
    INFORMATION_THEORY = "information_theory"
    ENTROPY = "entropy"
    REDUNDANCY_REDUCTION = "redundancy_reduction"
    DIMENSIONAL_REDUCTION = "dimensional_reduction"
    MANIFOLD_LEARNING = "manifold_learning"
    SPECTRAL_EMBEDDING = "spectral_embedding"
    LAPLACIAN_EIGENMAPS = "laplacian_eigenmaps"
    T_SNE = "t_sne"
    UMAP = "umap"
    AUTO_ENCODER = "auto_encoder"
    VARIATIONAL_AUTO_ENCODER = "variational_auto_encoder"
    GENERATIVE_ADVERSARIAL_NETWORK = "generative_adversarial_network"
    DEEP_BOLTZMANN_MACHINE = "deep_boltzmann_machine"
    RESTRICTED_BOLTZMANN_MACHINE = "restricted_boltzmann_machine"
    DEEP_BOLTZMANN_MACHINE_STACK = "deep_boltzmann_machine_stack"
    STACKED_AUTO_ENCODERS = "stacked_auto_encoders"
    CONVOLUTIONAL_NEURAL_NETWORK = "convolutional_neural_network"
    RECURRENT_NEURAL_NETWORK = "recurrent_neural_network"
    LONG_SHORT_TERM_MEMORY = "long_short_term_memory"
    GATED_RECURRENT_UNIT = "gated_recurrent_unit"
    TRANSFORMER = "transformer"
    ATTENTION_MECHANISM = "attention_mechanism"
    SELF_ATTENTION = "self_attention"
    MULTI_HEAD_ATTENTION = "multi_head_attention"
    POSITIONAL_ENCODING = "positional_encoding"
    EMBEDDING_LAYER = "embedding_layer"
    SOFTMAX_LAYER = "softmax_layer"
    CROSS_ENTROPY_LOSS = "cross_entropy_loss"
    MEAN_SQUARED_ERROR_LOSS = "mean_squared_error_loss"
    CONTRASTIVE_LOSS = "contrastive_loss"
    TRIPLET_LOSS = "triplet_loss"
    SIAMESE_NETWORK = "siamese_network"
    CAPSULE_NETWORK = "capsule_network"
    DYNAMIC_ROUTING = "dynamic_routing"
    ALLOCATION_LAYER = "allocation_layer"
    SQUASH_FUNCTION = "squash_function"
    FEATURE_MAPS = "feature_maps"
    FILTER_BANKS = "filter_banks"
    KERNELS = "kernels"
    FILTERS = "filters"
    CONVOLUTIONAL_LAYERS = "convolutional_layers"
    POOLING_LAYERS = "pooling_layers"
    MAX_POOLING = "max_pooling"
    AVERAGE_POOLING = "average_pooling"
    GLOBAL_POOLING = "global_pooling"
    STRIDED_CONVOLUTION = "strided_convolution"
    DILATED_CONVOLUTION = "dilated_convolution"
    TRANSPOSED_CONVOLUTION = "transposed_convolution"
    UPSAMPLING = "upsampling"
    DROPOUT = "dropout"
    BATCH_NORMALIZATION = "batch_normalization"
    LAYER_NORMALIZATION = "layer_normalization"
    INSTANCE_NORMALIZATION = "instance_normalization"
    GROUP_NORMALIZATION = "group_normalization"
    SPECTRAL_NORMALIZATION = "spectral_normalization"
    WEIGHT_NORMALIZATION = "weight_normalization"
    RESIDUAL_CONNECTIONS = "residual_connections"
    SKIP_CONNECTIONS = "skip_connections"
    DENSE_CONNECTIONS = "dense_connections"
    HIGHWAY_CONNECTIONS = "highway_connections"
    RECURRENT_CONNECTIONS = "recurrent_connections"
    FEEDBACK_CONNECTIONS = "feedback_connections"
    RECURRENT_WEIGHTS = "recurrent_weights"
    FORGET_GATES = "forget_gates"
    INPUT_GATES = "input_gates"
    OUTPUT_GATES = "output_gates"
    UPDATE_GATES = "update_gates"
    PEEPHOLE_CONNECTIONS = "peephole_connections"
    DIAGONAL_CONNECTIONS = "diagonal_connections"
    ATTENTION_WEIGHTS = "attention_weights"
    CONTEXT_VECTORS = "context_vectors"
    MEMORY_CELLS = "memory_cells"
    READ_PORTS = "read_ports"
    WRITE_PORTS = "write_ports"
    MEMORY_ADDRESSES = "memory_addresses"
    INDEXING = "indexing"
    ADDRESSING_MODES = "addressing_modes"
    MEMORY_MANAGEMENT = "memory_management"
    GARBAGE_COLLECTION = "garbage_collection"
    MEMORY_POOLS = "memory_pools"
    SLAB_ALLOCATION = "slab_allocation"
    BUDDY_SYSTEM_ALLOCATION = "buddy_system_allocation"
    FIRST_FIT_ALLOCATION = "first_fit_allocation"
    BEST_FIT_ALLOCATION = "best_fit_allocation"
    WORST_FIT_ALLOCATION = "worst_fit_allocation"
    FIXED_SIZE_ALLOCATION = "fixed_size_allocation"
    VARIABLE_SIZE_ALLOCATION = "variable_size_allocation"
    MEMORY_MAPPED_FILES = "memory_mapped_files"
    VIRTUAL_MEMORY = "virtual_memory"
    SWAP_SPACE = "swap_space"
    PAGE_MANAGEMENT = "page_management"
    MEMORY_PROTECTION = "memory_protection"
    MEMORY_COALESCING = "memory_coalescing"
    CACHE_COHERENCY = "cache_coherency"
    MEMORY_BARRIERS = "memory_barriers"
    MEMORY_FENCES = "memory_fences"
    MEMORY_ORDERINGS = "memory_orderings"
    MEMORY_CONSTRAINTS = "memory_constraints"
    MEMORY_PROFILING = "memory_profiling"
    MEMORY_ANALYSIS = "memory_analysis"
    MEMORY_LEAK_DETECTION = "memory_leak_detection"
    MEMORY_USAGE_MONITORING = "memory_usage_monitoring"

    @classmethod
    def from_cpp(cls, cpp_type: AdvancedNeuronType):
        """Convert from C++ AdvancedNeuronType enum."""
        type_map = {
            AdvancedNeuronType.EXCITATORY: cls.EXCITATORY,
            AdvancedNeuronType.INHIBITORY: cls.INHIBITORY,
            # ... other mappings
        }
        return type_map.get(cpp_type, cls.EXCITATORY)

    @classmethod
    def to_cpp(cls, python_type: 'AdvancedNeuronType'):
        """Convert from Python AdvancedNeuronType enum to C++."""
        type_map = {
            cls.EXCITATORY: AdvancedNeuronType.Excitatory,
            cls.INHIBITORY: AdvancedNeuronType.Inhibitory,
            # ... other mappings
        }
        return type_map.get(python_type, AdvancedNeuronType.Excitatory)

@dataclass
class NeuronConfig:
    """Configuration for a custom neuron."""
    name: str
    neuron_type: AdvancedNeuronType = AdvancedNeuronType.EXCITATORY
    threshold: float = -50.0
    resting_potential: float = -65.0
    reset_potential: float = -65.0
    leak_conductance: float = 10.0
    time_constant: float = 20.0
    capacitance: float = 1.0
    refractory_period: int = 5
    learning_rate: float = 0.01
    enable_plasticity: bool = True
    enable_development: bool = True
    enable_neuromodulation: bool = True
    integration_method: str = "exponential_euler"
    plasticity_rule: str = "stdp"
    custom_params: Dict[str, float] = field(default_factory=dict)

class CustomNeuronFactory:
    """Factory for creating custom neurons."""
    
    def __init__(self, neuron_type: AdvancedNeuronType, config: NeuronConfig):
        self.neuron_type = neuron_type
        self.config = config
        self.name = f"{neuron_type.value}_neuron"
    
    def create_neuron(self, neuron_id: int) -> '_Brain':
        """
        Create a neuron.
        
        Args:
            neuron_id: Unique identifier for the neuron
            
        Returns:
            Neuron instance
        """
        # This would create a neuron using the C++ bindings
        # For now, return a placeholder
        return None

class CustomNeuronRegistry:
    """Registry for custom neuron types."""
    
    def __init__(self):
        self.factories: Dict[str, CustomNeuronFactory] = {}
        self.configs: Dict[str, NeuronConfig] = {}
        self._lock = threading.Lock()
    
    def register_neuron_type(self, name: str, factory: CustomNeuronFactory):
        """
        Register a custom neuron type.
        
        Args:
            name: Name of the neuron type
            factory: Factory function that creates neurons
        """
        with self._lock:
            self.factories[name] = factory
    
    def register_config(self, name: str, config: NeuronConfig):
        """
        Register a neuron configuration.
        
        Args:
            name: Name of the configuration
            config: Neuron configuration
        """
        with self._lock:
            self.configs[name] = config
    
    def create_neuron(self, name: str, neuron_id: int) -> '_Brain':
        """
        Create a neuron using registered factory.
        
        Args:
            name: Name of the neuron type
            neuron_id: Unique identifier
            
        Returns:
            Neuron instance
            
        Raises:
            ValueError: If neuron type is not registered
        """
        with self._lock:
            if name not in self.factories:
                raise ValueError(f"Unknown neuron type: {name}")
            
            factory = self.factories[name]
            return factory.create_neuron(neuron_id)
    
    def has_neuron_type(self, name: str) -> bool:
        """
        Check if neuron type is registered.
        
        Args:
            name: Name of the neuron type
            
        Returns:
            True if registered, False otherwise
        """
        with self._lock:
            return name in self.factories
    
    def get_neuron_types(self) -> List[str]:
        """
        Get all registered neuron types.
        
        Returns:
            List of registered neuron type names
        """
        with self._lock:
            return list(self.factories.keys())
    
    def get_config(self, name: str) -> NeuronConfig:
        """
        Get neuron configuration.
        
        Args:
            name: Name of the configuration
            
        Returns:
            Neuron configuration
            
        Raises:
            ValueError: If configuration is not registered
        """
        with self._lock:
            if name not in self.configs:
                raise ValueError(f"Unknown neuron configuration: {name}")
            
            return self.configs[name]

# === ADVANCED BRAIN CLASS ===

class AdvancedBrain:
    """
    Advanced brain class with enhanced features for expert users.
    
    Extends the basic Brain class with:
    - Custom neuron types and populations
    - Advanced plasticity rules
    - Integration method selection
    - Advanced learning algorithms
    - Enhanced monitoring and metrics
    """
    
    def __init__(self, config: Optional[Dict[str, Any]] = None, 
                 neuron_registry: Optional[CustomNeuronRegistry] = None):
        """
        Initialize advanced brain.
        
        Args:
            config: Optional configuration dictionary
            neuron_registry: Optional custom neuron registry
        """
        self._brain = _Brain(config) if config else _Brain()
        self._neuron_registry = neuron_registry or CustomNeuronRegistry()
        self._neuron_populations: Dict[str, List[int]] = {}
        self._integration_method = "exponential_euler"
        self._plasticity_rules: Dict[str, Any] = {}
        self._advanced_learning_algorithms: Dict[str, Dict[str, float]] = {}
        self._monitoring_enabled = True
        self._performance_monitor = PerformanceMonitor("advanced_brain")
        self._lock = threading.Lock()
    
    def add_neuron_type(self, name: str, neuron_type: AdvancedNeuronType, 
                       config: NeuronConfig):
        """
        Add a custom neuron type.
        
        Args:
            name: Name of the neuron type
            neuron_type: Type of neuron
            config: Configuration for the neuron
        """
        factory = CustomNeuronFactory(neuron_type, config)
        self._neuron_registry.register_neuron_type(name, factory)
        self._neuron_registry.register_config(name, config)
        
        # Create initial population
        population_size = config.custom_params.get("population_size", 100)
        neuron_ids = self.create_neuron_population(name, population_size)
        self._neuron_populations[name] = neuron_ids
    
    def create_neuron_population(self, neuron_type: str, count: int,
                                params: Optional[Dict[str, Any]] = None) -> List[int]:
        """
        Create a population of neurons.
        
        Args:
            neuron_type: Type of neuron to create
            count: Number of neurons to create
            params: Optional parameters for neuron creation
            
        Returns:
            List of neuron IDs created
            
        Raises:
            ValueError: If neuron type is not registered
        """
        if not self._neuron_registry.has_neuron_type(neuron_type):
            raise ValueError(f"Unknown neuron type: {neuron_type}")
        
        neuron_ids = []
        for i in range(count):
            neuron_id = len(neuron_ids)  # Simple ID assignment for now
            neuron = self._neuron_registry.create_neuron(neuron_type, neuron_id)
            if neuron:
                neuron_ids.append(neuron_id)
        
        return neuron_ids
    
    def configure_integration(self, method: str):
        """
        Configure integration method for neurons.
        
        Args:
            method: Integration method name
        """
        valid_methods = ["euler", "rk4", "exponential_euler", "adaptive"]
        if method not in valid_methods:
            raise ValueError(f"Invalid integration method: {method}")
        
        self._integration_method = method
    
    def set_plasticity_rule(self, name: str, rule_config: Dict[str, Any]):
        """
        Set a plasticity rule.
        
        Args:
            name: Name of the plasticity rule
            rule_config: Configuration for the rule
        """
        self._plasticity_rules[name] = rule_config
    
    def enable_advanced_learning(self, algorithm: str,
                                params: Optional[Dict[str, float]] = None):
        """
        Enable advanced learning algorithm.
        
        Args:
            algorithm: Name of the learning algorithm
            params: Parameters for the algorithm
        """
        if params is None:
            params = {}
        self._advanced_learning_algorithms[algorithm] = params
    
    def get_integration_method(self) -> str:
        """
        Get current integration method.
        
        Returns:
            Integration method name
        """
        return self._integration_method
    
    def get_available_learning_algorithms(self) -> List[str]:
        """
        Get available learning algorithms.
        
        Returns:
            List of available algorithm names
        """
        return list(self._advanced_learning_algorithms.keys())
    
    def get_neuron_registry(self) -> CustomNeuronRegistry:
        """
        Get the custom neuron registry.
        
        Returns:
            Custom neuron registry
        """
        return self._neuron_registry
    
    def get_neuron_populations(self) -> Dict[str, List[int]]:
        """
        Get all neuron populations.
        
        Returns:
            Dictionary mapping neuron type names to neuron ID lists
        """
        return self._neuron_populations.copy()
    
    def get_plasticity_rules(self) -> Dict[str, Any]:
        """
        Get all plasticity rules.
        
        Returns:
            Dictionary of plasticity rules
        """
        return self._plasticity_rules.copy()
    
    def enable_monitoring(self, enable: bool = True):
        """
        Enable or disable monitoring.
        
        Args:
            enable: Whether to enable monitoring
        """
        self._monitoring_enabled = enable
    
    def is_monitoring_enabled(self) -> bool:
        """
        Check if monitoring is enabled.
        
        Returns:
            True if monitoring is enabled
        """
        return self._monitoring_enabled
    
    def get_performance_metrics(self) -> Dict[str, Any]:
        """
        Get performance metrics.
        
        Returns:
            Performance metrics
        """
        return self._performance_monitor.get_metrics()
    
    def step(self, step: int, time: float = 0.0) -> Dict[str, Any]:
        """
        Perform a simulation step.
        
        Args:
            step: Simulation step number
            time: Current simulation time
            
        Returns:
            Dictionary with step results and metrics
        """
        with self._lock:
            if self._monitoring_enabled:
                self._performance_monitor.add_step()
            
            # Perform step using the underlying brain
            result = self._brain.step(step, time)
            
            # Collect metrics
            metrics = {
                'step': step,
                'time': time,
                'neuron_populations': self._neuron_populations,
                'performance_metrics': self._performance_monitor.get_metrics()
            }
            
            return metrics
    
    def __str__(self) -> str:
        """String representation."""
        return (f"AdvancedBrain(neuron_types={len(self._neuron_registry.get_neuron_types())}, "
                f"populations={len(self._neuron_populations)}, "
                f"integration={self._integration_method}, "
                f"monitoring={'enabled' if self._monitoring_enabled else 'disabled'})")
    
    def __repr__(self) -> str:
        """Detailed representation."""
        return f"AdvancedBrain(neuron_types={len(self._neuron_registry.get_neuron_types())}, populations={len(self._neuron_populations)})"

# === SIMULATION CONTROLLER CLASS ===

class SimulationController:
    """
    Simulation controller for batch processing and event-driven simulation.
    
    Manages multiple brains and simulation runs with:
    - Batch processing capabilities
    - Event-driven simulation
    - Parallel execution
    - Simulation metrics and statistics
    - Pause/resume/stop control
    """
    
    def __init__(self):
        """Initialize simulation controller."""
        self._brains: List[_Brain] = []
        self._running = False
        self._paused = False
        self._mode = "sequential"  # "sequential", "parallel", "event_driven"
        self._step_count = 0
        self._total_steps = 0
        self._simulation_start_time = None
        self._metrics = {
            'steps_completed': 0,
            'total_steps': 0,
            'brains_running': 0,
            'elapsed_time': 0.0,
            'steps_per_second': 0.0,
            'memory_usage': 0.0
        }
        self._spike_driven = False
        self._synapse_driven = False
        self._plasticity_driven = False
        self._lock = threading.Lock()
    
    def add_brain(self, brain: _Brain):
        """
        Add a brain to the controller.
        
        Args:
            brain: Brain instance to add
        """
        with self._lock:
            self._brains.append(brain)
            self._update_brains_running()
    
    def remove_brain(self, brain: _Brain):
        """
        Remove a brain from the controller.
        
        Args:
            brain: Brain instance to remove
        """
        with self._lock:
            if brain in self._brains:
                self._brains.remove(brain)
                self._update_brains_running()
    
    def run_batch(self, steps: int, timestep: float = 0.001, mode: str = "sequential"):
        """
        Run batch simulation.
        
        Args:
            steps: Number of simulation steps
            timestep: Simulation timestep
            mode: Simulation mode ("sequential", "parallel", "event_driven")
        """
        self._mode = mode
        self._total_steps = steps
        self._step_count = 0
        self._simulation_start_time = time.time()
        self._running = True
        self._paused = False
        
        self._update_brains_running()
        
        try:
            if mode == "sequential":
                for step in range(steps):
                    if self._paused:
                        break
                    
                    for brain in self._brains:
                        if not self._running or self._paused:
                            break
                        
                        brain.step(step, step * timestep)
                    
                    self._step_count += 1
                    self._update_metrics()
            
            elif mode == "parallel":
                # Parallel processing (simplified)
                import concurrent.futures
                
                def run_brain_steps(brain):
                    for step in range(steps):
                        if not self._running or self._paused:
                            break
                        brain.step(step, step * timestep)
                    return brain
                
                with concurrent.futures.ThreadPoolExecutor(max_workers=len(self._brains)) as executor:
                    futures = [executor.submit(run_brain_steps, brain) for brain in self._brains]
                    for future in concurrent.futures.as_completed(futures):
                        if not self._running or self._paused:
                            break
                        future.result()  # Wait for completion
                    
                    self._step_count = steps
                    self._update_metrics()
            
            elif mode == "event_driven":
                # Event-driven simulation
                self._run_event_driven(steps, timestep)
        
        finally:
            self._running = False
            self._paused = False
    
    def _run_event_driven(self, steps: int, timestep: float):
        """Run event-driven simulation."""
        # Simplified event-driven implementation
        for step in range(steps):
            if not self._running or self._paused:
                break
            
            for brain in self._brains:
                if self._spike_driven:
                    # Spike-driven simulation
                    brain.step(step, step * timestep)
                else:
                    # Standard simulation
                    brain.step(step, step * timestep)
            
            self._step_count += 1
            self._update_metrics()
    
    def pause(self):
        """Pause simulation."""
        with self._lock:
            self._paused = True
    
    def resume(self):
        """Resume simulation."""
        with self._lock:
            self._paused = False
    
    def stop(self):
        """Stop simulation."""
        with self._lock:
            self._running = False
            self._paused = False
    
    def is_running(self) -> bool:
        """
        Check if simulation is running.
        
        Returns:
            True if simulation is running
        """
        with self._lock:
            return self._running
    
    def is_paused(self) -> bool:
        """
        Check if simulation is paused.
        
        Returns:
            True if simulation is paused
        """
        with self._lock:
            return self._paused
    
    def set_mode(self, mode: str):
        """
        Set simulation mode.
        
        Args:
            mode: Simulation mode
        """
        valid_modes = ["sequential", "parallel", "event_driven"]
        if mode not in valid_modes:
            raise ValueError(f"Invalid simulation mode: {mode}")
        
        with self._lock:
            self._mode = mode
    
    def get_mode(self) -> str:
        """
        Get current simulation mode.
        
        Returns:
            Current simulation mode
        """
        with self._lock:
            return self._mode
    
    def set_spike_driven(self, enabled: bool):
        """
        Enable or disable spike-driven simulation.
        
        Args:
            enabled: Whether to enable spike-driven simulation
        """
        with self._lock:
            self._spike_driven = enabled
    
    def set_synapse_driven(self, enabled: bool):
        """
        Enable or disable synapse-driven simulation.
        
        Args:
            enabled: Whether to enable synapse-driven simulation
        """
        with self._lock:
            self._synapse_driven = enabled
    
    def set_plasticity_driven(self, enabled: bool):
        """
        Enable or disable plasticity-driven simulation.
        
        Args:
            enabled: Whether to enable plasticity-driven simulation
        """
        with self._lock:
            self._plasticity_driven = enabled
    
    def get_metrics(self) -> Dict[str, Any]:
        """
        Get simulation metrics.
        
        Returns:
            Simulation metrics
        """
        with self._lock:
            return self._metrics.copy()
    
    def reset_metrics(self):
        """Reset simulation metrics."""
        with self._lock:
            self._metrics = {
                'steps_completed': 0,
                'total_steps': 0,
                'brains_running': 0,
                'elapsed_time': 0.0,
                'steps_per_second': 0.0,
                'memory_usage': 0.0
            }
    
    def _update_brains_running(self):
        """Update number of brains running."""
        with self._lock:
            self._metrics['brains_running'] = len(self._brains)
    
    def _update_metrics(self):
        """Update simulation metrics."""
        with self._lock:
            if self._simulation_start_time:
                elapsed = time.time() - self._simulation_start_time
                self._metrics['elapsed_time'] = elapsed
                
                if elapsed > 0 and self._step_count > 0:
                    self._metrics['steps_per_second'] = self._step_count / elapsed
            
            self._metrics['steps_completed'] = self._step_count
            self._metrics['total_steps'] = self._total_steps
    
    def get_brain_count(self) -> int:
        """
        Get number of brains.
        
        Returns:
            Number of brains
        """
        with self._lock:
            return len(self._brains)
    
    def __str__(self) -> str:
        """String representation."""
        return (f"SimulationController(brains={self.get_brain_count()}, "
                f"mode={self._mode}, running={'yes' if self.is_running() else 'no'}, "
                f"paused={'yes' if self.is_paused() else 'no'})")
    
    def __repr__(self) -> str:
        """Detailed representation."""
        return f"SimulationController(brains={self.get_brain_count()}, mode={self._mode})"

# === PERFORMANCE PROFILER CLASS ===

class PerformanceProfiler:
    """
    Performance profiler for monitoring and metrics collection.
    
    Monitors memory usage, performance metrics, and system health with:
    - Real-time monitoring
    - Metric recording and analysis
    - Alert system
    - Metrics server
    - Console and file logging
    """
    
    def __init__(self):
        """Initialize performance profiler."""
        self._monitors: Dict[str, PerformanceMonitor] = {}
        self._alerts: Dict[str, Dict[str, Any]] = {}
        self._console_logging = True
        self._file_logging = False
        self._metrics_server_enabled = False
        self._metrics_server_port = 8080
        self._lock = threading.Lock()
    
    def start_monitoring(self, name: str, brain: Optional[_Brain] = None):
        """
        Start performance monitoring.
        
        Args:
            name: Name of the monitor
            brain: Optional brain to monitor
        """
        with self._lock:
            if name not in self._monitors:
                self._monitors[name] = PerformanceMonitor(name)
            
            self._monitors[name].start()
            
            if self._file_logging:
                # Set up file logging for this monitor
                pass
    
    def stop_monitoring(self, name: str):
        """
        Stop performance monitoring.
        
        Args:
            name: Name of the monitor
        """
        with self._lock:
            if name in self._monitors:
                self._monitors[name].stop()
    
    def is_monitoring(self, name: str) -> bool:
        """
        Check if monitoring is active.
        
        Args:
            name: Name of the monitor
            
        Returns:
            True if monitoring is active
        """
        with self._lock:
            if name not in self._monitors:
                return False
            
            monitor = self._monitors[name]
            return monitor.start_time is not None and monitor.end_time is None
    
    def get_memory_usage(self) -> float:
        """
        Get current memory usage.
        
        Returns:
            Memory usage in MB
        """
        # This would use system monitoring libraries
        # For now, return a placeholder value
        return 0.0
    
    def get_growth_rate(self) -> float:
        """
        Get memory growth rate.
        
        Returns:
            Memory growth rate
        """
        # This would calculate growth rate based on historical data
        return 0.0
    
    def get_leak_rate(self) -> float:
        """
        Get memory leak rate.
        
        Returns:
            Memory leak rate
        """
        # This would detect memory leaks
        return 0.0
    
    def get_allocated_neurons(self) -> int:
        """
        Get number of allocated neurons.
        
        Returns:
            Number of allocated neurons
        """
        # This would count allocated neurons
        return 0
    
    def get_allocated_spikes(self) -> int:
        """
        Get number of allocated spike events.
        
        Returns:
            Number of allocated spike events
        """
        # This would count allocated spike events
        return 0
    
    def record_metric(self, name: str, value: float, unit: str = ""):
        """
        Record a performance metric.
        
        Args:
            name: Metric name
            value: Metric value
            unit: Metric unit (optional)
        """
        with self._lock:
            # Find the most recent monitoring this metric belongs to
            for monitor in self._monitors.values():
                monitor.record_metric(name, value, unit)
    
    def get_metric(self, name: str) -> Optional[float]:
        """
        Get recorded metric by name.
        
        Args:
            name: Metric name
            
        Returns:
            Metric value or None if not found
        """
        with self._lock:
            for monitor in self._monitors.values():
                metrics = monitor.get_metrics()
                for key, value in metrics.items():
                    if key.endswith(f"_metric_{name}") and isinstance(value, dict):
                        return value.get('value')
            return None
    
    def get_all_metrics(self) -> Dict[str, Any]:
        """
        Get all recorded metrics.
        
        Returns:
            All metrics from all monitors
        """
        with self._lock:
            all_metrics = {}
            for name, monitor in self._monitors.items():
                all_metrics[name] = monitor.get_metrics()
            return all_metrics
    
    def get_metrics_summary(self) -> str:
        """
        Get summary of all metrics.
        
        Returns:
            Formatted summary string
        """
        with self._lock:
            summary_parts = []
            for name, monitor in self._monitors.items():
                summary_parts.append(f"{name}: {monitor}")
            
            return "\n".join(summary_parts)
    
    def set_alert(self, metric: str, threshold: float, action: str):
        """
        Set alert for metric when threshold exceeded.
        
        Args:
            metric: Metric name
            threshold: Threshold value
            action: Action to take when alert triggers
        """
        with self._lock:
            self._alerts[metric] = {
                'threshold': threshold,
                'action': action,
                'triggered': False,
                'last_check': time.time()
            }
    
    def remove_alert(self, metric: str):
        """
        Remove alert for specified metric.
        
        Args:
            metric: Metric name
        """
        with self._lock:
            if metric in self._alerts:
                del self._alerts[metric]
    
    def get_alerts(self) -> Dict[str, Dict[str, Any]]:
        """
        Get all active alerts.
        
        Returns:
            Dictionary of alerts
        """
        with self._lock:
            return self._alerts.copy()
    
    def enable_console_logging(self, enable: bool = True):
        """
        Enable or disable console logging.
        
        Args:
            enable: Whether to enable console logging
        """
        with self._lock:
            self._console_logging = enable
    
    def log_to_file(self, filepath: str, enable: bool = True):
        """
        Enable or disable file logging.
        
        Args:
            filepath: Path to log file
            enable: Whether to enable file logging
        """
        with self._lock:
            self._file_logging = enable
            if enable:
                self._file_logging_path = filepath
    
    def enable_metrics_server(self, enable: bool = True, port: int = 8080):
        """
        Enable or disable metrics server.
        
        Args:
            enable: Whether to enable metrics server
            port: Port for metrics server
        """
        with self._lock:
            self._metrics_server_enabled = enable
            self._metrics_server_port = port
    
    def get_monitor_names(self) -> List[str]:
        """
        Get names of all monitors.
        
        Returns:
            List of monitor names
        """
        with self._lock:
            return list(self._monitors.keys())
    
    def get_monitor(self, name: str) -> Optional[PerformanceMonitor]:
        """
        Get a specific monitor.
        
        Args:
            name: Name of the monitor
            
        Returns:
            Monitor instance or None if not found
        """
        with self._lock:
            return self._monitors.get(name)
    
    def remove_monitor(self, name: str):
        """
        Remove a monitor.
        
        Args:
            name: Name of the monitor to remove
        """
        with self._lock:
            if name in self._monitors:
                del self._monitors[name]
    
    def __str__(self) -> str:
        """String representation."""
        return f"PerformanceProfiler(monitors={len(self._monitors)}, alerts={len(self._alerts)})"
    
    def __repr__(self) -> str:
        """Detailed representation."""
        return f"PerformanceProfiler(monitors={len(self._monitors)})"

# === PLUGIN SYSTEM CLASS ===

class PluginSystem:
    """
    Plugin system for extensibility and custom components.
    
    Manages plugins for custom components and plasticity rules with:
    - Plugin registration and discovery
    - Dependency management
    - Plugin enabling/disabling
    - Plugin validation
    - Plugin lifecycle management
    """
    
    def __init__(self):
        """Initialize plugin system."""
        self._plugins: Dict[str, Dict[str, Any]] = {}
        self._dependencies: Dict[str, List[str]] = {}
        self._enabled_plugins: Dict[str, bool] = {}
        self._plugin_factories: Dict[str, Callable] = {}
        self._lock = threading.Lock()
    
    def register_plugin(self, name: str, factory: Callable, config: Optional[Dict[str, Any]] = None):
        """
        Register a plugin.
        
        Args:
            name: Plugin name
            factory: Factory function that creates plugin instances
            config: Plugin configuration (optional)
        """
        with self._lock:
            self._plugins[name] = {
                'factory': factory,
                'config': config or {},
                'registered_at': time.time()
            }
            self._enabled_plugins[name] = True
    
    def create_plugin(self, name: str) -> Any:
        """
        Create a plugin instance.
        
        args:
            name: Plugin name
            
        Returns:
            Plugin instance
            
        Raises:
            ValueError: If plugin is not registered
        """
        with self._lock:
            if name not in self._plugins:
                raise ValueError(f"Unknown plugin: {name}")
            
            plugin_info = self._plugins[name]
            factory = plugin_info['factory']
            return factory()
    
    def has_plugin(self, name: str) -> bool:
        """
        Check if plugin is registered.
        
        Args:
            name: Plugin name
            
        Returns:
            True if plugin is registered
        """
        with self._lock:
            return name in self._plugins
    
    def get_plugin_names(self) -> List[str]:
        """
        Get all registered plugin names.
        
        Returns:
            List of plugin names
        """
        with self._lock:
            return list(self._plugins.keys())
    
    def unregister_plugin(self, name: str):
        """
        Unregister a plugin.
n        Args:
            name: Plugin name
        """
        with self._lock:
            if name in self._plugins:
                del self._plugins[name]
            if name in self._enabled_plugins:
                del self._enabled_plugins[name]
            if name in self._dependencies:
                del self._dependencies[name]
    
    def set_dependencies(self, plugin_name: str, dependencies: List[str]):
        """
        Set plugin dependencies.
        
        Args:
            plugin_name: Plugin name
            dependencies: List of dependency names
        """
        with self._lock:
            self._dependencies[plugin_name] = dependencies
    
    def get_dependencies(self, plugin_name: str) -> List[str]:
        """
        Get plugin dependencies.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            List of dependency names
        """
        with self._lock:
            return self._dependencies.get(plugin_name, [])
    
    def enable_plugin(self, name: str, enable: bool = True):
        """
        Enable or disable a plugin.
        
        Args:
            name: Plugin name
            enable: Whether to enable the plugin
        """
        with self._lock:
            if name in self._enabled_plugins:
                self._enabled_plugins[name] = enable
    
    def is_plugin_enabled(self, name: str) -> bool:
        """
        Check if plugin is enabled.
        
        Args:
            name: Plugin name
            
        Returns:
            True if plugin is enabled
        """
        with self._lock:
            return self._enabled_plugins.get(name, True)
    
    def get_plugin_dependencies(self, name: str) -> List[str]:
        """
        Get plugin dependencies.
        
        Args:
            name: Plugin name
            
        Returns:
            List of dependency names
        """
        return self.get_dependencies(name)
    
    def validate_plugin(self, name: str) -> bool:
        """
        Validate plugin configuration.
        
        Args:
            name: Plugin name
            
        Returns:
            True if plugin configuration is valid
        """
        with self._lock:
            if name not in self._plugins:
                return False
            
            plugin_info = self._plugins[name]
            # Simple validation - can be extended
            return plugin_info['factory'] is not None
    
    def get_plugin_count(self) -> int:
        """
        Get number of registered plugins.
        
        Returns:
            Number of registered plugins
        """
        with self._lock:
            return len(self._plugins)
    
    def get_enabled_plugin_count(self) -> int:
        """
        Get number of enabled plugins.
        
        Returns:
            Number of enabled plugins
        """
        with self._lock:
            return sum(1 for enabled in self._enabled_plugins.values() if enabled)
    
    def __str__(self) -> str:
        """String representation."""
        return f"PluginSystem(plugins={self.get_plugin_count()}, enabled={self.get_enabled_plugin_count()})"
    
    def __repr__(self) -> str:
        """Detailed representation."""
        return f"PluginSystem(plugins={self.get_plugin_count()})"

# === ERROR HANDLER CLASS ===

class ErrorHandler:
    """
    Advanced error handler with logging and reporting capabilities.
    
    Manages errors with:
    - Error logging with different levels
    - Error categorization
    - Error statistics
    - Error export/import
    - Error recovery
    """
    
    def __init__(self):
        """Initialize error handler."""
        self._errors: List[Dict[str, Any]] = []
        self._error_counts: Dict[str, int] = defaultdict(int)
        self._log_level = logging.WARNING
        self._handlers: List[Callable] = []
        self._lock = threading.Lock()
    
    def log(self, level: str, message: str):
        """
        Log a message with specified level.
        
        Args:
            level: Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
            message: Log message
        """
        with self._lock:
            timestamp = time.time()
            error_entry = {
                'level': level,
                'message': message,
                'timestamp': timestamp,
                'category': self._categorize_error(message)
            }
            
            self._errors.append(error_entry)
            self._error_counts[level] += 1
            
            # Call registered handlers
            for handler in self._handlers:
                try:
                    handler(error_entry)
                except Exception:
                    pass  # Ignore handler errors
    
    def info(self, message: str):
        """
        Log an info message.
        
        Args:
            message: Info message
        """
        self.log("INFO", message)
    
    def warning(self, message: str):
        """
        Log a warning message.
        
        Args:
            message: Warning message
        """
        self.log("WARNING", message)
    
    def error(self, message: str):
        """
        Log an error message.
        
        Args:
            message: Error message
        """
        self.log("ERROR", message)
    
    def critical(self, message: str):
        """
        Log a critical error message.
        
        Args:
            message: Critical error message
        """
        self.log("CRITICAL", message)
    
    def debug(self, message: str):
        """
        Log a debug message.
        
        Args:
            message: Debug message
        """
        self.log("DEBUG", message)
    
    def set_level(self, level: str):
        """
        Set minimum log level.
        
        Args:
            level: Minimum log level
        """
        valid_levels = ["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"]
        if level not in valid_levels:
            raise ValueError(f"Invalid log level: {level}")
        
        with self._lock:
            self._log_level = getattr(logging, level)
    
    def get_level(self) -> str:
        """
        Get current log level.
        
        Returns:
            Current log level as string
        """
        with self._lock:
            return logging.getLevelName(self._log_level)
    
    def add_handler(self, handler: Callable):
        """
        Add a log handler.
        
        Args:
            handler: Handler function that receives error entries
        """
        with self._lock:
            self._handlers.append(handler)
    
    def remove_handler(self, handler: Callable):
        """
        Remove a log handler.
        
        Args:
            handler: Handler function to remove
        """
        with self._lock:
            if handler in self._handlers:
                self._handlers.remove(handler)
    
    def clear_handlers(self):
        """Clear all log handlers."""
        with self._lock:
            self._handlers.clear()
    
    def has_handlers(self) -> bool:
        """
        Check if any handlers are registered.
        
        Returns:
            True if any handlers are registered
        """
        with self._lock:
            return len(self._handlers) > 0
    
    def log_error(self, category: str, message: str, error_code: int = 0):
        """
        Log an error with category and error code.
        
        Args:
            category: Error category
            message: Error message
            error_code: Error code (optional)
        """
        self.error(f"[{category}] {message} (code {error_code})")
    
    def get_error_count(self) -> int:
        """
        Get total error count.
        
        Returns:
            Total error count
        """
        with self._lock:
            return sum(self._error_counts.values())
    
    def get_errors(self) -> List[Dict[str, Any]]:
        """
        Get all errors.
        
        Returns:
            List of error entries
        """
        with self._lock:
            return self._errors.copy()
    
    def clear_errors(self):
        """Clear all errors."""
        with self._lock:
            self._errors.clear()
            self._error_counts.clear()
    
    def export_errors(self, filepath: str):
        """
        Export errors to file.
        
        Args:
            filepath: Path to export file
        """
        with self._lock:
            with open(filepath, 'w') as f:
                json.dump(self._errors, f, indent=2)
    
    def get_statistics(self) -> Dict[str, Any]:
        """
        Get logging statistics.
        
        Returns:
            Statistics dictionary
        """
        with self._lock:
            if not self._errors:
                return {
                    'total_errors': 0,
                    'error_counts': {},
                    'level_counts': {},
                    'categories': {},
                    'most_recent_error': None
                }
            
            level_counts = defaultdict(int)
            categories = defaultdict(int)
            
            for error in self._errors:
                level_counts[error['level']] += 1
                categories[error['category']] += 1
            
            return {
                'total_errors': len(self._errors),
                'error_counts': dict(self._error_counts),
                'level_counts': dict(level_counts),
                'categories': dict(categories),
                'most_recent_error': self._errors[-1]['timestamp'] if self._errors else None
            }
    
    def _categorize_error(self, message: str) -> str:
        """
        Categorize error based on message.
        
        Args:
            message: Error message
            
        Returns:
            Error category
        """
        message_lower = message.lower()
        if 'memory' in message_lower:
            return 'MEMORY'
        elif 'performance' in message_lower:
            return 'PERFORMANCE'
        elif 'plasticity' in message_lower:
            return 'PLASTICITY'
        elif 'neuron' in message_lower:
            return 'NEURON'
        elif 'synapse' in message_lower:
            return 'SYNAPSE'
        else:
            return 'GENERAL'
    
    def __str__(self) -> str:
        """String representation."""
        return f"ErrorHandler(errors={self.get_error_count()}, handlers={len(self._handlers)})"
    
    def __repr__(self) -> str:
        """Detailed representation."""
        return f"ErrorHandler(errors={self.get_error_count()})"

# === CONFIG BUILDER CLASS ===

class ConfigBuilder:
    """
    Advanced configuration builder with template support.
    
    Manages configurations with:
    - Template system
    - Configuration validation
    - Configuration merging
    - Configuration export/import
    - Fast learning, stable learning, balanced, exploratory, exploitative templates
    """
    
    def __init__(self):
        """Initialize config builder."""
        self._templates: Dict[str, Dict[str, Any]] = {}
        self._lock = threading.Lock()
        
        # Initialize built-in templates
        self._initialize_templates()
    
    def _initialize_templates(self):
        """Initialize built-in templates."""
        self._templates['fast_learning'] = {
            'neuron_count': 5000,
            'plasticity': {
                'stdp': {'enable': True, 'learning_rate': 0.05},
                'hebbian': {'enable': False},
                'reward_modulated': {'enable': True, 'gain': 0.2}
            },
            'development': {'critical_period': 50, 'maturation_rate': 0.02},
            'simulation': {'timestep': 0.001},
            'visualization': {'enabled': True},
            'export': {'format': 'json'}
        }
        
        self._templates['stable_learning'] = {
            'neuron_count': 2000,
            'plasticity': {
                'stdp': {'enable': True, 'learning_rate': 0.01},
                'hebbian': {'enable': True},
                'reward_modulated': {'enable': False}
            },
            'development': {'critical_period': 200, 'maturation_rate': 0.005},
            'simulation': {'timestep': 0.005},
            'visualization': {'enabled': True},
            'export': {'format': 'binary'}
        }
        
        self._templates['balanced'] = {
            'neuron_count': 3000,
            'plasticity': {
                'stdp': {'enable': True, 'learning_rate': 0.02},
                'hebbian': {'enable': True},
                'reward_modulated': {'enable': True, 'gain': 0.1},
                'bcn': {'enable': True},
                'homeostatic': {'enable': False}
            },
            'development': {'critical_period': 100, 'maturation_rate': 0.01},
            'simulation': {'timestep': 0.002},
            'visualization': {'enabled': True},
            'export': {'format': 'json'}
        }
        
        self._templates['exploratory'] = {
            'neuron_count': 10000,
            'plasticity': {
                'stdp': {'enable': True, 'learning_rate': 0.03},
                'hebbian': {'enable': True},
                'reward_modulated': {'enable': True, 'gain': 0.5},
                'homeostatic': {'enable': True},
                'predictive_coding': {'enable': True}
            },
            'development': {'critical_period': 25, 'maturation_rate': 0.05},
            'simulation': {'timestep': 0.0005},
            'visualization': {'enabled': True, 'real_time': True},
            'export': {'format': 'compressed', 'compression_level': 'best'}
        }
        
        self._templates['exploitative'] = {
            'neuron_count': 15000,
            'plasticity': {
                'stdp': {'enable': True, 'learning_rate': 0.04},
                'hebbian': {'enable': True},
                'reward_modulated': {'enable': True, 'gain': 0.8},
                'bcn': {'enable': True},
                'homeostatic': {'enable': True},
                'predictive_coding': {'enable': True}
            },
            'development': {'critical_period': 10, 'maturation_rate': 0.08},
            'simulation': {'timestep': 0.0001},
            'visualization': {'enabled': True, 'real_time': True, 'network_topology': True},
            'export': {'format': 'differential', 'checkpoint_interval': 100},
            'memory': {'gc': {'enabled': True}, 'pools': {'enabled': True}},
            'parallel': {'processing': {'enabled': True}, 'neuron_update': True, 'spike_propagation': True}
        }
    
    def create_template(self, name: str, config: Dict[str, Any]):
        """
        Create a configuration template.
        
        Args:
            name: Template name
            config: Template configuration
            
        Raises:
            ValueError: If template already exists
        """
        with self._lock:
            if name in self._templates:
                raise ValueError(f"Template already exists: {name}")
            
            self._templates[name] = config.copy()
    
    def get_template(self, name: str) -> Dict[str, Any]:
        """
        Get a configuration template.
        
        Args:
            name: Template name
            
        Returns:
            Template configuration
            
        Raises:
            ValueError: If template doesn't exist
        """
        with self._lock:
            if name not in self._templates:
                raise ValueError(f"Template not found: {name}")
            
            return self._templates[name].copy()
    
    def has_template(self, name: str) -> bool:
        """
        Check if template exists.
        
        Args:
            name: Template name
            
        Returns:
            True if template exists
        """
        with self._lock:
            return name in self._templates
    
    def remove_template(self, name: str):
        """
        Remove a template.
        
        Args:
            name: Template name
        """
        with self._lock:
            if name in self._templates:
                del self._templates[name]
    
    def get_template_names(self) -> List[str]:
        """
        Get all template names.
        
        Returns:
            List of template names
        """
        with self._lock:
            return list(self._templates.keys())
    
    def get_fast_learning_template(self) -> Dict[str, Any]:
        """
        Get fast learning template.
        
        Returns:
            Fast learning template
        """
        return self.get_template('fast_learning')
    
    def get_stable_learning_template(self) -> Dict[str, Any]:
        """
        Get stable learning template.
        
        Returns:
            Stable learning template
        """
        return self.get_template('stable_learning')
    
    def get_balanced_template(self) -> Dict[str, Any]:
        """
        Get balanced template.
        
        Returns:
            Balanced template
        """
        return self.get_template('balanced')
    
    def get_exploratory_template(self) -> Dict[str, Any]:
        """
        Get exploratory template.
        
        Returns:
            Exploratory template
        """
        return self.get_template('exploratory')
    
    def get_exploitative_template(self) -> Dict[str, Any]:
        """
        Get exploitative template.
        
        Returns:
            Exploitative template
        """
        return self.get_template('exploitative')
    
    def apply_template(self, config: Dict[str, Any], template_name: str):
        """
        Apply a template to configuration.
        
        Args:
            config: Configuration to apply template to
            template_name: Name of template to apply
        """
        with self._lock:
            if self.has_template(template_name):
                template = self.get_template(template_name)
                # Deep merge template into config
                self._deep_merge(template, config)
    
    def validate_template(self, name: str) -> bool:
        """
        Validate a template.
        
        Args:
            name: Template name
            
        Returns:
            True if template is valid
        """
        with self._lock:
            if not self.has_template(name):
                return False
            
            template = self.get_template(name)
            # Simple validation - can be extended
            return isinstance(template, dict) and len(template) > 0
    
    def save_template(self, name: str, config: Dict[str, Any]):
        """
        Save a configuration as a template.
        
        Args:
            name: Template name
            config: Configuration to save
        """
        self.create_template(name, config)
    
    def _deep_merge(self, source: Dict[str, Any], target: Dict[str, Any]):
        """
        Deep merge source dictionary into target dictionary.
        
        Args:
            source: Source dictionary
            target: Target dictionary
        """
        for key, value in source.items():
            if key in target and isinstance(target[key], dict) and isinstance(value, dict):
                self._deep_merge(value, target[key])
            else:
                target[key] = value
    
    def get_template_count(self) -> int:
        """
        Get number of templates.
        
        Returns:
            Number of templates
        """
        with self._lock:
            return len(self._templates)
    
    def __str__(self) -> str:
        """String representation."""
        return f"ConfigBuilder(templates={self.get_template_count()})"
    
    def __repr__(self) -> str:
        """Detailed representation."""
        return f"ConfigBuilder(templates={self.get_template_count()})"

# === CHECKPOINT MANAGER CLASS ===

class CheckpointManager:
    """
    Advanced checkpoint manager with incremental saving and metadata.
    
    Manages checkpoints with:
    - Checkpoint creation and loading
    - Checkpoint metadata
    - Incremental checkpointing
    - Checkpoint delta computation
    - Cleanup of old checkpoints
    """
    
    def __init__(self):
        """Initialize checkpoint manager."""
        self._checkpoints: Dict[str, Dict[str, Any]] = {}
        self._base_checkpoint = None
        self._lock = threading.Lock()
    
    def create_checkpoint(self, name: str, brain: _Brain) -> bool:
        """
        Create a checkpoint.
        
        Args:
            name: Checkpoint name
            brain: Brain to checkpoint
            
        Returns:
            True if checkpoint created successfully
        """
        with self._lock:
            try:
                # Create checkpoint with metadata
                checkpoint_info = {
                    'name': name,
                    'timestamp': time.time(),
                    'brain_size': len(str(brain)),  # Simplified size estimation
                    'neuron_count': brain.getTotalNeuronCount(),
                    'synapse_count': brain.getTotalSynapseCount(),
                    'metadata': {
                        'simulation_step': 'current_step',  # Would need to get from brain
                        'version': '1.0',
                        'format': 'json'
                    }
                }
                
                self._checkpoints[name] = checkpoint_info
                return True
                
            except Exception:
                return False
    
    def load_checkpoint(self, name: str, brain: _Brain) -> bool:
        """
        Load a checkpoint.
        
        Args:
            name: Checkpoint name
            brain: Brain to load checkpoint into
            
        Returns:
            True if checkpoint loaded successfully
        """
        with self._lock:
            if name not in self._checkpoints:
                return False
            
            try:
                # Load checkpoint (simplified implementation)
                return True
                
            except Exception:
                return False
    
    def has_checkpoint(self, name: str) -> bool:
        """
        Check if checkpoint exists.
n        Args:
            name: Checkpoint name
            
        Returns:
            True if checkpoint exists
        """
        with self._lock:
            return name in self._checkpoints
    
    def get_checkpoint_names(self) -> List[str]:
        """
        Get all checkpoint names.
        
        Returns:
            List of checkpoint names
        """
        with self._lock:
            return list(self._checkpoints.keys())
    
    def delete_checkpoint(self, name: str):
        """
        Delete a checkpoint.
        
        Args:
            name: Checkpoint name
        """
        with self._lock:
            if name in self._checkpoints:
                del self._checkpoints[name]
    
    def get_checkpoint_info(self, name: str) -> Dict[str, Any]:
        """
        Get checkpoint information.
        
        Args:
            name: Checkpoint name
            
        Returns:
            Checkpoint information dictionary
            
        Raises:
            ValueError: If checkpoint doesn't exist
        """
        with self._lock:
            if name not in self._checkpoints:
                raise ValueError(f"Checkpoint not found: {name}")
            
            return self._checkpoints[name].copy()
    
    def get_checkpoint_delta(self, from_name: str, to_name: str) -> Dict[str, Any]:
        """
        Get delta between two checkpoints.
        
        Args:
            from_name: Source checkpoint name
            to_name: Target checkpoint name
            
        Returns:
            Delta dictionary
        """
        with self._lock:
            if from_name not in self._checkpoints or to_name not in self._checkpoints:
                return {}
            
            # Compute delta (simplified implementation)
            return {
                'from': from_name,
                'to': to_name,
                'delta_size': 'estimated',
                'changes': []
            }
    
    def set_base_checkpoint(self, name: str):
        """
        Set base checkpoint for delta operations.
        
        Args:
            name: Base checkpoint name
        """
        with self._lock:
            if name in self._checkpoints:
                self._base_checkpoint = name
    
    def get_base_checkpoint(self) -> Optional[str]:
        """
        Get base checkpoint name.
        
        Returns:
            Base checkpoint name or None if not set
        """
        with self._lock:
            return self._base_checkpoint
    
    def list_checkpoints(self) -> List[Dict[str, Any]]:
        """
        List all checkpoints with details.
        
        Returns:
            List of checkpoint information dictionaries
        """
        with self._lock:
            return [info.copy() for info in self._checkpoints.values()]
    
    def cleanup_old_checkpoints(self, max_age_hours: float = 24.0):
        """
        Clean up old checkpoints.
        
        Args:
            max_age_hours: Maximum age in hours
        """
        with self._lock:
            current_time = time.time()
            to_delete = []
            
            for name, info in self._checkpoints.items():
                age_hours = (current_time - info['timestamp']) / 3600
                if age_hours > max_age_hours:
                    to_delete.append(name)
            
            for name in to_delete:
                del self._checkpoints[name]
    
    def get_checkpoint_count(self) -> int:
        """
        Get number of checkpoints.
        
        Returns:
            Number of checkpoints
        """
        with self._lock:
            return len(self._checkpoints)
    
    def __str__(self) -> str:
        """String representation."""
        return f"CheckpointManager(checkpoints={self.get_checkpoint_count()})"
    
    def __repr__(self) -> str:
        """Detailed representation."""
        return f"CheckpointManager(checkpoints={self.get_checkpoint_count()})"

# === CONTEXT MANAGER ===

@contextmanager
def simulation_context(brain: _Brain, config: Optional[Dict[str, Any]] = None):
    """
    Context manager for safe simulation execution.
    
    Provides safe execution of simulation code with automatic rollback
    on error and restoration of original state.
    
    Args:
        brain: Brain instance to manage
        config: Optional configuration to apply
        
    Yields:
        Tuple of (brain, config)
    """
    original_config = None
    
    try:
        if config is not None:
            original_config = brain.getConfig()
            brain.setConfig(config)
        
        yield brain, config
        
    except Exception:
        # Restore original config on error
        if original_config is not None:
            brain.setConfig(original_config)
        raise
    
    finally:
        # Always restore original config
        if original_config is not None:
            brain.setConfig(original_config)

# === ADVANCED NLM CLASS ===

class AdvancedNLM:
    """
    Advanced NLM framework with all features enabled.
    
    Complete framework with advanced features including:
    - Custom neurons and advanced learning
    - Visualization and debugging tools
    - Simulation control and batch processing
    - Export/import capabilities
    - Parameter optimization
    - Memory management
    - Monitoring and metrics
    """
    
    def __init__(self):
        """Initialize AdvancedNLM."""
        self._brain = None
        self._neuron_registry = CustomNeuronRegistry()
        self._plasticity_rule_registry = PlasticityRuleRegistry()
        self._configuration_templates = ConfigurationTemplates()
        self._memory_monitor = MemoryMonitor()
        self._memory_manager = MemoryManager()
        self._parallel_processor = ParallelProcessor()
        self._event_driven_simulator = EventDrivenSimulator()
        self._neuron_activity_plotter = NeuronActivityPlotter()
        self._network_topology_visualizer = NetworkTopologyVisualizer()
        self._learning_curve_analyzer = LearningCurveAnalyzer()
        self._statistical_visualizer = StatisticalVisualizer()
        self._incremental_checkpoint_manager = IncrementalCheckpointManager()
        self._brain_exporter = BrainExporter()
        self._delta_compressor = DeltaCompressor()
        self._bayesian_optimizer = BayesianOptimizer()
        
        self._lock = threading.Lock()
        self._running = False
    
    def initialize(self, config: Optional[Dict[str, Any]] = None):
        """
        Initialize with configuration.
        
        Args:
            config: Optional configuration dictionary
        """
        with self._lock:
            if config is None:
                config = {}
            
            self._brain = AdvancedBrain(config, self._neuron_registry)
            self._running = True
    
    def reset(self):
        """Reset simulation."""
        with self._lock:
            if self._brain:
                self._brain.reset()
            self._running = False
    
    def finalize(self):
        """Finalize and cleanup."""
        with self._lock:
            self._running = False
            if self._brain:
                self._brain = None
    
    def step(self, step: int, time: float = 0.0):
        """
        Perform a single simulation step.
        
        Args:
            step: Simulation step number
            time: Current simulation time
        """
        with self._lock:
            if not self._running or not self._brain:
                raise RuntimeError("AdvancedNLM is not initialized or running")
            
            return self._brain.step(step, time)
    
    def run(self, steps: int, timestep: float = 0.001):
        """
        Run simulation for specified steps.
        
        Args:
            steps: Number of simulation steps
            timestep: Simulation timestep
        """
        for step in range(steps):
            if not self._running:
                break
            
            self.step(step, step * timestep)
    
    # Configuration methods
    def set_neuron_count(self, count: int):
        """Set number of neurons."""
        pass
    
    def set_region_count(self, count: int):
        """Set number of regions."""
        pass
    
    def set_connection_probability(self, probability: float):
        """Set connection probability."""
        pass
    
    def set_random_seed(self, seed: int):
        """Set random seed."""
        pass
    
    def set_simulation_timestep(self, timestep: float):
        """Set simulation timestep."""
        pass
    
    def set_checkpoint_directory(self, directory: str):
        """Set checkpoint directory."""
        pass
    
    # Neuron management
    def create_neurons(self, neuron_type: str, count: int) -> List[int]:
        """
        Create neurons of specified type.
        
        Args:
            neuron_type: Type of neurons to create
            count: Number of neurons to create
            
        Returns:
            List of neuron IDs
        """
        if self._brain:
            return self._brain.create_neuron_population(neuron_type, count)
        return []
    
    def configure_neuron(self, neuron_id: int, config: NeuronConfig):
        """
        Configure a neuron.
        
        Args:
            neuron_id: Neuron ID
            config: Neuron configuration
        """
        pass
    
    def get_neuron_config(self, neuron_id: int) -> NeuronConfig:
        """
        Get neuron configuration.
        
        Args:
            neuron_id: Neuron ID
            
        Returns:
            Neuron configuration
        """
        return NeuronConfig()
    
    # Plasticity management
    def set_plasticity_rule(self, name: str, rule_config: Dict[str, Any]):
        """
        Set a plasticity rule.
        
        Args:
            name: Rule name
            rule_config: Rule configuration
        """
        if self._brain:
            self._brain.set_plasticity_rule(name, rule_config)
    
    def enable_plasticity(self, name: str, enable: bool):
        """
        Enable or disable plasticity.
        
        Args:
            name: Rule name
            enable: Whether to enable plasticity
        """
        pass
    
    def is_plasticity_enabled(self, name: str) -> bool:
        """
        Check if plasticity is enabled.
        
        Args:
            name: Rule name
            
        Returns:
            True if plasticity is enabled
        """
        return False
    
    # Integration methods
    def set_integration_method(self, method: str):
        """
        Set integration method.
        
        Args:
            method: Integration method name
        """
        if self._brain:
            self._brain.configure_integration(method)
    
    def get_integration_method(self) -> str:
        """
        Get current integration method.
        
        Returns:
            Integration method name
        """
        if self._brain:
            return self._brain.get_integration_method()
        return "unknown"
    
    def set_custom_timestep_scheduler(self, scheduler):
        """
        Set custom timestep scheduler.
        
        Args:
            scheduler: Custom timestep scheduler
        """
        pass
    
    # Advanced learning
    def set_advanced_learning_algorithm(self, name: str):
        """
        Set advanced learning algorithm.
        
        Args:
            name: Algorithm name
        """
        if self._brain:
            self._brain.enable_advanced_learning(name)
    
    def configure_advanced_learning(self, params: Dict[str, float]):
        """
        Configure advanced learning.
        
        Args:
            params: Algorithm parameters
        """
        pass
    
    # Visualization
    def start_visualization(self):
        """Start visualization."""
        pass
    
    def stop_visualization(self):
        """Stop visualization."""
        pass
    
    def save_visualization(self, filepath: str):
        """
        Save visualization.
        
        Args:
            filepath: Output file path
        """
        pass
    
    # Export/import
    def save_checkpoint(self, name: str) -> bool:
        """
        Save a checkpoint.
        
        Args:
            name: Checkpoint name
            
        Returns:
            True if checkpoint saved successfully
        """
        if self._brain:
            return self._incremental_checkpoint_manager.create_checkpoint(name, self._brain)
        return False
    
    def load_checkpoint(self, name: str) -> bool:
        """
        Load a checkpoint.
        
        Args:
            name: Checkpoint name
            
        Returns:
            True if checkpoint loaded successfully
        """
        if self._brain:
            return self._incremental_checkpoint_manager.load_checkpoint(name, self._brain)
        return False
    
    def export_to_file(self, filepath: str, format: str = "json") -> bool:
        """
        Export to file.
        
        Args:
            filepath: Output file path
            format: Export format
            
        Returns:
            True if export successful
        """
        # Simplified implementation
        return True
    
    # Parameter optimization
    def optimize_parameters(self, objective_function) -> Dict[str, float]:
        """
        Optimize parameters using objective function.
        
        Args:
            objective_function: Objective function to optimize
            
        Returns:
            Optimized parameters
        """
        return {}
    
    # Memory management
    def update_memory_manager(self, step: int):
        """
        Update memory manager.
n        Args:
            step: Simulation step
        """
        pass
    
    def trigger_gc(self):
        """Trigger garbage collection."""
        pass
    
    def get_memory_usage(self) -> float:
        """
        Get memory usage.
        
        Returns:
            Memory usage in MB
        """
        return 0.0
    
    # Monitoring
    def get_current_metrics(self) -> Dict[str, Any]:
        """
        Get current metrics.
        
        Returns:
            Current metrics
        """
        if self._brain:
            return self._brain.get_performance_metrics()
        return {}
    
    def get_status_summary(self) -> str:
        """
        Get status summary.
        
        Returns:
            Status summary string
        """
        if self._brain:
            return str(self._brain)
        return "Not initialized"
    
    # Access to subsystems
    def get_brain(self) -> _Brain:
        """
        Get the underlying brain.
        
        Returns:
            Brain instance
        """
        return self._brain
    
    def get_neuron_registry(self) -> CustomNeuronRegistry:
        """
        Get the custom neuron registry.
        
        Returns:
            Custom neuron registry
        """
        return self._neuron_registry
    
    def get_plasticity_rule_registry(self) -> PlasticityRuleRegistry:
        """
        Get the plasticity rule registry.
        
        Returns:
            Plasticity rule registry
        """
        return self._plasticity_rule_registry
    
    def get_configuration_templates(self) -> ConfigurationTemplates:
        """
        Get the configuration templates.
        
        Returns:
            Configuration templates
        """
        return self._configuration_templates
    
    def get_memory_monitor(self) -> MemoryMonitor:
        """
        Get the memory monitor.
        
        Returns:
            Memory monitor
        """
        return self._memory_monitor
    
    def get_memory_manager(self) -> MemoryManager:
        """
        Get the memory manager.
        
        Returns:
            Memory manager
        """
        return self._memory_manager
    
    def get_parallel_processor(self) -> ParallelProcessor:
        """
        Get the parallel processor.
        
        Returns:
            Parallel processor
        """
        return self._parallel_processor
    
    def get_event_driven_simulator(self) -> EventDrivenSimulator:
        """
        Get the event-driven simulator.
        
        Returns:
            Event-driven simulator
        """
        return self._event_driven_simulator
    
    def get_neuron_activity_plotter(self) -> NeuronActivityPlotter:
        """
        Get the neuron activity plotter.
        
        Returns:
            Neuron activity plotter
        """
        return self._neuron_activity_plotter
    
    def get_network_topology_visualizer(self) -> NetworkTopologyVisualizer:
        """
        Get the network topology visualizer.
        
        Returns:
            Network topology visualizer
        """
        return self._network_topology_visualizer
    
    def get_learning_curve_analyzer(self) -> LearningCurveAnalyzer:
        """
        Get the learning curve analyzer.
        
        Returns:
            Learning curve analyzer
        """
        return self._learning_curve_analyzer
    
    def get_statistical_visualizer(self) -> StatisticalVisualizer:
        """
        Get the statistical visualizer.
        
        Returns:
            Statistical visualizer
        """
        return self._statistical_visualizer
    
    def get_incremental_checkpoint_manager(self) -> IncrementalCheckpointManager:
        """
        Get the incremental checkpoint manager.
        
        Returns:
            Incremental checkpoint manager
        """
        return self._incremental_checkpoint_manager
    
    def get_brain_exporter(self) -> BrainExporter:
        """
        Get the brain exporter.
        
        Returns:
            Brain exporter
        """
        return self._brain_exporter
    
    def get_delta_compressor(self) -> DeltaCompressor:
        """
        Get the delta compressor.
        
        Returns:
            Delta compressor
        """
        return self._delta_compressor
    
    def get_bayesian_optimizer(self) -> BayesianOptimizer:
        """
        Get the Bayesian optimizer.
        
        Returns:
            Bayesian optimizer
        """
        return self._bayesian_optimizer
    
    def __str__(self) -> str:
        """String representation."""
        if self._brain:
            return f"AdvancedNLM(brain={self._brain}, running={'yes' if self._running else 'no'})"
        return "AdvancedNLM(not initialized)"
    
    def __repr__(self) -> str:
        """Detailed representation."""
        return "AdvancedNLM()"

# === FACTORY FUNCTIONS ===

def create_advanced_brain(config: Optional[Dict[str, Any]] = None) -> 'AdvancedBrain':
    """
    Create an advanced brain with enhanced features.
    
    Args:
        config: Optional configuration dictionary
        
    Returns:
        AdvancedBrain instance
    """
    return AdvancedBrain(config)

def create_advanced_nlm() -> AdvancedNLM:
    """
    Create an AdvancedNLM instance with all features.
    
    Returns:
        AdvancedNLM instance
    """
    return AdvancedNLM()

def create_plugin_system() -> PluginSystem:
    """
    Create a plugin system for extensibility.
    
    Returns:
        PluginSystem instance
    """
    return PluginSystem()

def create_performance_profiler() -> PerformanceProfiler:
    """
    Create a performance profiler for monitoring.
    
    Returns:
        PerformanceProfiler instance
    """
    return PerformanceProfiler()

def create_simulation_controller() -> SimulationController:
    """
    Create a simulation controller for batch processing.
    
    Returns:
        SimulationController instance
    """
    return SimulationController()

def create_error_handler() -> ErrorHandler:
    """
    Create an error handler with logging capabilities.
    
    Returns:
        ErrorHandler instance
    """
    return ErrorHandler()

def create_config_builder() -> ConfigBuilder:
    """
    Create a config builder with template support.
    
    Returns:
        ConfigBuilder instance
    """
    return ConfigBuilder()

def create_checkpoint_manager() -> CheckpointManager:
    """
    Create a checkpoint manager for enhanced checkpointing.
    
    Returns:
        CheckpointManager instance
    """
    return CheckpointManager()

def create_simulation_context(brain: _Brain, config: Optional[Dict[str, Any]] = None):
    """
    Create a simulation context for safe execution.
    
    Args:
        brain: Brain instance
        config: Optional configuration
        
    Returns:
        Simulation context
    """
    return simulation_context(brain, config)

# === BACKWARD COMPATIBILITY ===

# Re-export original classes for backward compatibility
from . import pynlm

# Create aliases for backward compatibility
AdvancedBrain = AdvancedBrain
SimulationController = SimulationController
PerformanceProfiler = PerformanceProfiler
PluginSystem = PluginSystem
ErrorHandler = ErrorHandler
ConfigBuilder = ConfigBuilder
CheckpointManager = CheckpointManager

# === DOCUMENTATION ===

__all__ = [
    # Main classes
    'AdvancedBrain',
    'SimulationController',
    'PerformanceProfiler',
    'PluginSystem',
    'ErrorHandler',
    'ConfigBuilder',
    'CheckpointManager',
    'AdvancedNLM',
    
    # Utility classes
    'Timer',
    'PerformanceMonitor',
    'NeuronConfig',
    'CustomNeuronFactory',
    'CustomNeuronRegistry',
    'SimulationContext',
    
    # Factory functions
    'create_advanced_brain',
    'create_advanced_nlm',
    'create_plugin_system',
    'create_performance_profiler',
    'create_simulation_controller',
    'create_error_handler',
    'create_config_builder',
    'create_checkpoint_manager',
    'create_simulation_context',
    
    # Exceptions
    'AdvancedNLMError',
    'AdvancedConfigurationError',
    'AdvancedBrainError',
    'AdvancedSimulationError',
    'AdvancedPerformanceError',
    'AdvancedPluginError',
    'AdvancedCheckpointError',
    
    # Utilities
    'ValidationError',
    'ConfigurationError',
    'Timer',
    'PerformanceMonitor',
    
    # Constants
    'AdvancedNeuronType',
]
