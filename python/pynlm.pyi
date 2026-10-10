# Type stubs for Enhanced NLM Python Bindings
# Generated for better IDE support and static type checking

from __future__ import annotations
from typing import (
    Any,
    Callable,
    ContextManager,
    Dict,
    List,
    Optional,
    Tuple,
    Union,
    Iterator,
    Generator,
    overload,
)
import sys

if sys.version_info >= (3, 8):
    from typing import TypedDict
else:
    from typing_extensions import TypedDict

# Core exception types
class NLMException(Exception):
    type: str
    
class ConfigurationError(NLMException):
    pass
    
class SimulationError(NLMException):
    pass
    
class MemoryError(NLMException):
    pass

# Identifier types
class NeuronId:
    value: int
    
    def __init__(self, value: int = 0) -> None: ...
    def index(self) -> int: ...
    def __eq__(self, other: Any) -> bool: ...
    def __ne__(self, other: Any) -> bool: ...
    def __hash__(self) -> int: ...
    def __repr__(self) -> str: ...
    def __str__(self) -> str: ...

class SynapseId:
    value: int
    
    def __init__(self, value: int = 0) -> None: ...
    def index(self) -> int: ...
    def __eq__(self, other: Any) -> bool: ...
    def __ne__(self, other: Any) -> bool: ...
    def __hash__(self) -> int: ...
    def __repr__(self) -> str: ...
    def __str__(self) -> str: ...

class RegionId:
    value: int
    
    def __init__(self, value: int = 0) -> None: ...
    def index(self) -> int: ...
    def __eq__(self, other: Any) -> bool: ...
    def __ne__(self, other: Any) -> bool: ...
    def __hash__(self) -> int: ...
    def __repr__(self) -> str: ...
    def __str__(self) -> str: ...

class PopulationId:
    value: int
    
    def __init__(self, value: int = 0) -> None: ...
    def index(self) -> int: ...
    def __eq__(self, other: Any) -> bool: ...
    def __ne__(self, other: Any) -> bool: ...
    def __hash__(self) -> int: ...
    def __repr__(self) -> str: ...
    def __str__(self) -> str: ...

# Enum types
class NeuronType:
    Excitatory: int
    Inhibitory: int
    Modulatory: int
    Sensory: int
    Motor: int
    Internal: int

class SynapseType:
    Excitatory: int
    Inhibitory: int
    Modulatory: int
    Electrical: int
    GapJunction: int

class DevelopmentalStage:
    Initial: int
    CriticalPeriod: int
    Maturation: int
    Adult: int
    Aging: int

class FiringState:
    Resting: int
    Active: int
    Refractory: int
    Inhibited: int

class ActionType:
    MoveForward: int
    MoveBackward: int
    MoveLeft: int
    MoveRight: int
    TurnLeft: int
    TurnRight: int
    Look: int
    LookUp: int
    LookDown: int
    Interact: int
    Eat: int
    Drink: int
    Rest: int
    Wait: int
    Custom: int

class MotorCommand:
    MoveForward: int
    MoveBackward: int
    TurnLeft: int
    TurnRight: int
    LookLeft: int
    LookRight: int
    Interact: int
    Wait: int

class WorldObjectType:
    Empty: int
    Resource: int
    Hazard: int
    Wall: int
    Marker: int

# Type aliases
NeuronIndex: TypeAlias = int
SimulationStep: TypeAlias = int
Timestamp: TypeAlias = float
PopulationIndex: TypeAlias = int
RegionIndex: TypeAlias = int
SynapticWeight: TypeAlias = float
MembranePotential: TypeAlias = float
FiringRate: TypeAlias = float
Delay: TypeAlias = int

# Constants
INVALID_NEURON_ID: NeuronId
INVALID_SYNAPSE_ID: SynapseId
INVALID_REGION_ID: RegionId
INVALID_POPULATION_ID: PopulationId

# Config TypedDict
class ConfigDict(TypedDict, total=False):
    neuron_count: int
    synaptic_connectivity: float
    learning_rate: float
    excitation_ratio: float
    inhibition_ratio: float
    simulation_timestep: float
    max_memory_usage: int

# Data type definitions
SpikeData = List[Tuple[NeuronId, float]]
SynapseData = List[Tuple[NeuronId, NeuronId, SynapticWeight]]
NeuronConfig = Dict[str, Any]
WorldConfig = Dict[str, Any]

# Forward class declarations
class Config:
    def __init__(self) -> None: ...
    
    # Property-based access
    def get(self, key: str) -> str: ...
    def get_int(self, key: str) -> int: ...
    def get_float(self, key: str) -> float: ...
    def get_bool(self, key: str) -> bool: ...
    def get_vector(self, key: str) -> List[float]: ...
    
    # Property setters
    def set(self, key: str, value: Union[str, int, float, bool, List[float]]) -> None: ...
    
    # Convenience methods
    def has(self, key: str) -> bool: ...
    def get_all_keys(self) -> List[str]: ...
    def summary(self) -> str: ...
    
    # File operations
    def save(self, filepath: str) -> bool: ...
    def load(self, filepath: str) -> bool: ...
    
    # Validation
    def validate(self) -> bool: ...
    def get_validation_errors(self) -> str: ...
    
    # Context manager
    def __enter__(self) -> Config: ...
    def __exit__(self, exc_type: Optional[type], exc_val: Optional[Exception], exc_tb: Optional[Any]) -> None: ...
    
    # Factory methods
    @staticmethod
    def create() -> Config: ...
    
    @staticmethod
    def from_dict(data: Dict[str, Any]) -> Config: ...
    
    @staticmethod
    def from_file(filepath: str) -> Config: ...

class Brain:
    def __init__(self, brain: Any = ...) -> None: ...
    
    # Context manager
    def __enter__(self) -> Brain: ...
    def __exit__(self, exc_type: Optional[type], exc_val: Optional[Exception], exc_tb: Optional[Any]) -> None: ...
    
    # Fluent interface
    def add_region(self, name: str = "") -> Brain: ...
    def inject_to_type(self, type: NeuronType, current: MembranePotential) -> Brain: ...
    
    # Simulation helpers
    def run_for(self, steps: SimulationStep) -> None: ...
    def simulate_episode(self, inputs: List[Any]) -> None: ...
    def simulate_steps(self, start: SimulationStep, end: SimulationStep) -> List[Action]: ...
    
    # Data access helpers
    def get_spike_array(self) -> Any: ...  # numpy array
    def get_weight_matrix(self) -> Any: ...  # numpy array
    
    # Export/import utilities
    def export_state(self, filepath: str) -> bool: ...
    def import_state(self, filepath: str) -> bool: ...
    
    # Getters
    def get_config(self) -> Config: ...
    def get_total_steps(self) -> SimulationStep: ...
    def get_ei_ratio(self) -> float: ...
    def get_neuron_count(self) -> int: ...
    def get_synapse_count(self) -> int: ...
    
    # Step iterator
    def step_iterator(self, start: SimulationStep, end: SimulationStep) -> Iterator[Tuple[SimulationStep, Action]]: ...
    
    # Visualization helpers
    def plot_activity(self, filename: str = "activity_plot.png") -> None: ...
    
    # Machine learning helpers
    def prepare_features(self, options: Optional[Dict[str, Any]] = None) -> Dict[str, Any]: ...
    def train_model(self, model_options: Optional[Dict[str, Any]] = None) -> None: ...
    
    # Performance profiling
    def profile(self, operation: str = "step") -> float: ...
    
    # Parallel simulation
    def parallel_simulate(self, steps: SimulationStep, num_threads: int = 4) -> None: ...
    
    # Streaming simulation
    def simulation_streamer(self, start: SimulationStep = 0) -> Iterator[Tuple[SimulationStep, Action]]: ...
    
    # Multi-process simulation
    def multi_process_simulate(self, num_processes: int) -> None: ...
    
    # Builder pattern
    class Builder:
        def with_region(self, name: str = "") -> Builder: ...
        def with_neuron_count(self, count: int) -> Builder: ...
        def build(self) -> Brain: ...

# Simplified class definitions for other components
class AgentBrain:
    def __init__(self, brain: Brain) -> None: ...
    def initialize(self, world: Any) -> None: ...
    def process_sensory_input(self, percept: Any) -> None: ...
    def decode_motor_command(self) -> Any: ...
    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> None: ...

class SimpleWorld:
    def __init__(self) -> None: ...
    def configure(self, width: int, height: int, vision_width: int, vision_height: int) -> None: ...
    def reset(self) -> None: ...
    def update(self, timestep: float) -> None: ...
    def get_sensory_percept(self) -> Any: ...
    def get_agent_body(self) -> Any: ...

class Action:
    def __init__(self, action_type: ActionType = ActionType.Wait) -> None: ...
    def get_type(self) -> ActionType: ...
    def get_parameters(self) -> List[float]: ...

class Vision:
    def __init__(self) -> None: ...
    def __init__(self, width: int, height: int, channels: int = 3) -> None: ...

class Audio:
    def __init__(self) -> None: ...
    def __init__(self, sample_rate: int, num_samples: int) -> None: ...

class InternalSignals:
    def __init__(self) -> None: ...
    def add_signal(self, value: float) -> None: ...
    def clear_signals(self) -> None: ...

class ActionResult:
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = "") -> None: ...
    reward: float
    success: bool
    message: str

class SensoryPercept:
    def get_vision(self) -> Vision: ...
    def set_vision(self, vision: Vision) -> None: ...
    def get_vision_width(self) -> int: ...
    def get_vision_height(self) -> int: ...
    def get_touch(self) -> Any: ...
    def set_touch(self, touch: Any) -> None: ...

class WorldObject:
    def __init__(self) -> None: ...
    x: float
    y: float
    radius: float
    type: WorldObjectType
    value: float
    active: bool

class AgentBody:
    def __init__(self) -> None: ...
    x: float
    y: float
    orientation: float
    velocityX: float
    velocityY: float
    angularVelocity: float
    energy: float
    health: float
    age: float
    isMoving: bool
    isTurning: bool
    lastActionTime: float

# Builder classes
class ConfigBuilder:
    def set(self, key: str, value: Union[int, float, str, bool, List[float]]) -> ConfigBuilder: ...
    def build(self) -> Config: ...

# Factory functions
class _Factory:
    @staticmethod
    def create_config() -> Config: ...
    
    @staticmethod
    def create_brain(config: Config) -> Brain: ...
    
    @staticmethod
    def create_simple_world() -> SimpleWorld: ...
    
    @staticmethod
    def create_agent_brain(brain: Brain) -> AgentBrain: ...

# Utility functions and classes
class NLMSession(ContextManager):
    def __init__(self) -> None: ...
    def __enter__(self) -> NLMSession: ...
    def __exit__(self, exc_type: Optional[type], exc_val: Optional[Exception], exc_tb: Optional[Any]) -> None: ...
    def create_brain(self, config: Optional[Config] = None) -> Brain: ...
    def create_world(self, **kwargs: Any) -> SimpleWorld: ...
    def cleanup(self) -> None: ...

class NLMTools:
    @staticmethod
    def normalize_spike_data(spike_array: Any, min_val: float = 0.0, max_val: float = 1.0) -> List[float]: ...
    
    @staticmethod
    def compute_statistics(data: List[float]) -> Dict[str, float]: ...
    
    @staticmethod
    def save_simulation_data(brain: Brain, filepath: str, format: str = "json") -> None: ...
    
    @staticmethod
    def load_simulation_data(filepath: str, format: str = "json") -> Dict[str, Any]: ...
    
    @staticmethod
    def benchmark_operation(operation: Callable[..., Any], *args: Any, **kwargs: Any) -> Dict[str, Any]: ...

# Numpy integration stubs (conditional)
if sys.version_info >= (3, 9):
    _ArrayLike = Union[List[float], Tuple[float, ...], Any]  # pybind11 numpy array
else:
    _ArrayLike = Union[List[float], Tuple[float, ...], Any]

# Visualization and ML stubs (conditional)
class _Visualization:
    def plot_neural_activity(self, spike_data: _ArrayLike, title: str = "Neural Activity") -> Any: ...
    def plot_weight_distribution(self, weight_data: _ArrayLike, title: str = "Weight Distribution") -> Any: ...

class _MLIntegration:
    def prepare_ml_features(self, spike_array: _ArrayLike, weight_matrix: _ArrayLike) -> Dict[str, Any]: ...
    def train_neural_model(self, features: _ArrayLike, targets: _ArrayLike, model_options: Optional[Dict[str, Any]] = None) -> Dict[str, Any]: ...

# Error handling
_ErrorType = Union[type[NLMException], type[ConfigurationError], type[SimulationError], type[MemoryError]]

def safe_execute(func: Callable[..., Any], context: str = "") -> Any: ...

def set_error_handler(handler: Callable[[_ErrorType, str], None]) -> None: ...

# Default constants
DEFAULT_CONFIG: ConfigDict = {
    "neuron_count": 1000,
    "synaptic_connectivity": 0.1,
    "learning_rate": 0.01,
    "excitation_ratio": 0.8,
    "inhibition_ratio": 0.2,
    "simulation_timestep": 0.001,
    "max_memory_usage": 1024,
}

# Package utilities
packages_available: Dict[str, bool] = {
    "numpy": False,
    "matplotlib": False,
    "scikit_learn": False,
}

# Session management
_active_sessions: List[NLMSession] = []

def check_dependencies() -> Dict[str, bool]: ...

# Quick creation shortcuts
brain_builder: Optional[ConfigBuilder] = None

def quick_brain(**kwargs: Any) -> Config: ...

# Main module exports
__version__: str
__author__: str
__license__: str
__copyright__: str

# Package configuration
_package_config: Dict[str, Any] = {}

# Make tools available
tools: NLMTools = NLMTools()

# Global error handling
_error_handler: Optional[Callable[[_ErrorType, str], None]] = None