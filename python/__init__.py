# NLM Python Package - Advanced Bindings

This package provides advanced Python bindings for the NLM (Neural Learning Machine)
C++ neural simulation framework, with enhanced functionality, error handling,
and convenience features for Python users.

## Package Structure

```
python/
├── __init__.py              # Package exports and main API
├── advanced_nlm.py          # Advanced utilities and examples
├── README.md                # Package documentation
├── examples/
│   ├── basic_usage.py       # Basic usage examples
│   ├── advanced_tutorial.py # Advanced tutorial
│   ├── agent_training.py    # Agent training examples
│   └── world_simulation.py  # World simulation examples
└── tests/
    ├── test_advanced_api.py  # Advanced API tests
    └── test_utilities.py     # Utility function tests
```

## Main API (__init__.py)

```python
"""
NLM Python Advanced API - Enhanced bindings for neural simulation

This module provides an enhanced Python interface to the NLM C++ neural
simulation framework with improved error handling, convenience methods,
and advanced functionality for both beginners and experts.
"""

from .advanced_nlm import (
    # Core utilities
    AdvancedBrainUtils,
    AdvancedWorldUtils,
    AdvancedSerializationUtils,
    AdvancedTrainingUtils,
    
    # Main functions
    get_advanced_examples,
    get_advanced_troubleshooting_guide,
    print_advanced_help,
)

# Re-export core NLM classes for convenience
from pynlm import (
    # Core classes
    Brain,
    Config,
    AgentBrain,
    SimpleWorld,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    
    # Enums
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # Factory functions
    createBrain,
    createDefaultConfig,
    createSimpleWorld,
    createAgentBrain,
    createBrainWithConfig,
    createAgentWithWorld,
    
    # Utility functions
    runBrainSimulation,
    runAgentEpisode,
    batchStepBrains,
    getSystemInfo,
    exportBrainStats,
    benchmarkBrain,
    validateConfig,
)

__all__ = [
    # Advanced utilities
    "AdvancedBrainUtils",
    "AdvancedWorldUtils", 
    "AdvancedSerializationUtils",
    "AdvancedTrainingUtils",
    
    # Main API functions
    "get_advanced_examples",
    "get_advanced_troubleshooting_guide",
    "print_advanced_help",
    
    # Core NLM classes
    "Brain",
    "Config",
    "AgentBrain",
    "SimpleWorld",
    "Vision",
    "Audio",
    "InternalSignals",
    "Action",
    "WorldObject",
    "AgentBody",
    "ActionResult",
    "SensoryPercept",
    
    # Enums
    "NeuronType",
    "SynapseType",
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    
    # Factory functions
    "createBrain",
    "createDefaultConfig",
    "createSimpleWorld",
    "createAgentBrain",
    "createBrainWithConfig",
    "createAgentWithWorld",
    
    # Utility functions
    "runBrainSimulation",
    "runAgentEpisode",
    "batchStepBrains",
    "getSystemInfo",
    "exportBrainStats",
    "benchmarkBrain",
    "validateConfig",
]

# Package metadata
__version__ = "1.0.0"
__author__ = "NLM Project Team"
__license__ = "MIT"
__description__ = "Enhanced Python bindings for NLM neural simulation framework"

# Enable advanced features
import os

# Set default environment variables
os.environ.setdefault("NLM_PYTHON_VERBOSE", "false")
os.environ.setdefault("NLM_PYTHON_VALIDATION", "true")
os.environ.setdefault("NLM_PYTHON_PERFORMANCE_LOGGING", "false")

# Performance optimization flags
PERFORMANCE_MODE = os.environ.get("NLM_PYTHON_PERFORMANCE_MODE", "balanced").lower() in ("true", "1", "yes", "on")
VERBOSE_LOGGING = os.environ.get("NLM_PYTHON_VERBOSE", "false").lower() in ("true", "1", "yes", "on")
VALIDATION_ENABLED = os.environ.get("NLM_PYTHON_VALIDATION", "true").lower() in ("true", "1", "yes", "on")

def get_version():
    """Get package version."""
    return __version__

def enable_performance_mode():
    """Enable performance mode with optimizations."""
    global PERFORMANCE_MODE
    PERFORMANCE_MODE = True
    if VERBOSE_LOGGING:
        print("[NLM] Performance mode enabled")

def disable_performance_mode():
    """Disable performance mode."""
    global PERFORMANCE_MODE
    PERFORMANCE_MODE = False
    if VERBOSE_LOGGING:
        print("[NLM] Performance mode disabled")

def set_verbose_logging(enabled: bool = True):
    """Enable or disable verbose logging."""
    global VERBOSE_LOGGING
    VERBOSE_LOGGING = enabled
    if VERBOSE_LOGGING:
        print("[NLM] Verbose logging enabled")
    else:
        print("[NLM] Verbose logging disabled")

def enable_validation(enabled: bool = True):
    """Enable or disable configuration validation."""
    global VALIDATION_ENABLED
    VALIDATION_ENABLED = enabled
    if VERBOSE_LOGGING:
        print(f"[NLM] Validation {'enabled' if enabled else 'disabled'}")

def get_package_info():
    """Get comprehensive package information."""
    return {
        "version": __version__,
        "author": __author__,
        "license": __license__,
        "description": __description__,
        "performance_mode": PERFORMANCE_MODE,
        "verbose_logging": VERBOSE_LOGGING,
        "validation_enabled": VALIDATION_ENABLED,
        "features": [
            "Enhanced Python bindings",
            "Configuration validation",
            "Performance optimization",
            "Convenience methods",
            "Advanced utilities",
            "Error handling",
            "Serialization support",
            "Agent training",
            "World simulation",
            "Benchmarking",
            "Troubleshooting guides",
            "Examples and tutorials",
        ],
    }

# Initialize logging
if VERBOSE_LOGGING:
    print("[NLM] NLM Python Advanced API loaded successfully")
    print(f"[NLM] Version {__version__}")
    print(f"[NLM] Performance mode: {'enabled' if PERFORMANCE_MODE else 'disabled'}")
    print(f"[NLM] Validation: {'enabled' if VALIDATION_ENABLED else 'disabled'}")
    
# Performance optimizations for high-performance mode
if PERFORMANCE_MODE:
    import pybind11
    
    # Enable pybind11 optimizations
    pybind11.set_optimization_level(3)
    
    # Enable vectorization
    import numpy as np
    np.seterr(all='ignore')  # Ignore numerical warnings for performance
    
    if VERBOSE_LOGGING:
        print("[NLM] Performance optimizations applied")

# Provide helpful error messages for common issues
class NLMError(Exception):
    """Base exception class for NLM Python errors."""
    pass

class ConfigurationError(NLMError):
    """Configuration-related error."""
    pass

class SimulationError(NLMError):
    """Simulation-related error."""
    pass

class ImportError(NLMError):
    """Import-related error."""
    pass
def format_error_message(error: Exception, context: str = "") -> str:
    """
    Format error message with helpful context and suggestions.
    
    Args:
        error: The exception that occurred
        context: Context where the error occurred
        
    Returns:
        Formatted error message
    """
    error_type = type(error).__name__
    error_msg = str(error)
    
    # Add context if provided
    if context:
        error_msg = f"{context}: {error_msg}"
    
    # Add helpful suggestions based on error type
    suggestions = {
        "ImportError": "\nPossible causes:\n  - pynlm not installed\n  - Missing dependencies\n  - Python bindings not built correctly\n\nSolutions:\n  - pip install pynlm\n  - pip install -e .\n  - Check build logs for errors",
        "RuntimeError": "\nPossible causes:\n  - Brain not initialized\n  - Invalid configuration\n  - Invalid parameters\n\nSolutions:\n  - Call brain.initialize() before step()\n  - Use createDefaultConfig() for safe defaults\n  - Validate configuration parameters",
        "TypeError": "\nPossible causes:\n  - Wrong argument type\n  - Invalid method call\n  - Data type conversion issue\n\nSolutions:\n  - Check argument types\n  - Use appropriate data types\n  - Convert data properly",
        "ValueError": "\nPossible causes:\n  - Value out of range\n  - Invalid parameter value\n  - Configuration validation failed\n\nSolutions:\n  - Check value ranges\n  - Use valid parameter values\n  - Use validation functions",
    }
    
    suggestion = suggestions.get(error_type, "\nCheck documentation and error messages for more details.")
    
    return f"NLM Error ({error_type}): {error_msg}{suggestion}"
def safe_execute(func, *args, **kwargs):
    """
    Execute function with error handling and validation.
    
    Args:
        func: Function to execute
        *args: Function arguments
        **kwargs: Function keyword arguments
        
    Returns:
        Function result or raises formatted error
        
    Raises:
        NLMError: Formatted error with helpful message
    """
    try:
        if VALIDATION_ENABLED:
            # Basic validation for common functions
            if func.__name__ in ["createBrain", "createDefaultConfig", "createSimpleWorld"]:
                print(f"[NLM] Creating object with validation enabled")
        
        result = func(*args, **kwargs)
        
        if VERBOSE_LOGGING:
            print(f"[NLM] Successfully executed {func.__name__}")
        
        return result
        
    except Exception as e:
        error_msg = format_error_message(e, f"Error executing {func.__name__}")
        
        # Convert to specific error types
        if isinstance(e, (ImportError, SyntaxError)):
            raise ImportError(error_msg) from e
        elif isinstance(e, (ValueError, TypeError)):
            raise ConfigurationError(error_msg) from e
        else:
            raise SimulationError(error_msg) from e
# Convenience wrappers for common operations
class BrainBuilder:
    """
    Convenience class for building and configuring brains.
    
    This class provides a fluent interface for creating and configuring
    NLM brains with validation and helpful error messages.
    """
    
    def __init__(self):
        self._config = createDefaultConfig()
        self._name = "Brain"
    
    def set_name(self, name: str) -> "BrainBuilder":
        """Set brain name."""
        self._name = name
        return self
    
    def set_neurons(self, count: int) -> "BrainBuilder":
        """Set neuron count with validation."""
        if count <= 0 or count > 1000000:
            raise ValueError(f"Invalid neuron count: {count}. Must be between 1 and 1,000,000")
        self._config.setNeuronCount(count)
        return self
    
    def set_connections(self, probability: float) -> "BrainBuilder":
        """Set connection probability with validation."""
        if probability < 0.0 or probability > 1.0:
            raise ValueError(f"Invalid connection probability: {probability}. Must be between 0.0 and 1.0")
        self._config.setConnectionProbability(probability)
        return self
    
    def set_stdp(self, ltp_weight: float, ltd_weight: float, tau: float = 20.0) -> "BrainBuilder":
        """Set STDP parameters."""
        if ltp_weight <= 0 or ltd_weight <= 0 or tau <= 0:
            raise ValueError("STDP parameters must be positive")
        self._config.set("stdp_ltp_weight", ltp_weight)
        self._config.set("stdp_ltd_weight", ltd_weight)
        self._config.set("stdp_tau", tau)
        return self
    
    def enable_features(self, **kwargs) -> "BrainBuilder":
        """Enable learning features."""
        for feature, enabled in kwargs.items():
            if not isinstance(enabled, bool):
                raise ValueError(f"Feature {feature} must be boolean")
            if feature == "reward_modulation":
                self._config.set("enable_reward_modulation", enabled)
            elif feature == "curiosity":
                self._config.set("enable_curiosity", enabled)
            elif feature == "development":
                self._config.set("enable_development", enabled)
            elif feature == "structural_plasticity":
                self._config.set("enable_structural_plasticity", enabled)
        return self
    
    def build(self) -> Brain:
        """Build and return the brain."""
        if VERBOSE_LOGGING:
            print(f"[NLM] Building brain '{self._name}' with {self._config.getNeuronCount()} neurons")
        
        brain = createBrain(self._config)
        brain.initialize()
        
        if VERBOSE_LOGGING:
            print(f"[NLM] Brain '{self._name}' built successfully")
        
        return brain
class EpisodeSimulator:
    """
    Convenience class for simulating agent episodes.
    
    This class provides a fluent interface for running simulation
    episodes with detailed tracking and statistics.
    """
    
    def __init__(self, brain: Brain, world: SimpleWorld):
        self._brain = brain
        self._world = world
        self._agent = None
        self._episode_stats = {}
    
    def with_agent(self, agent: AgentBrain = None) -> "EpisodeSimulator":
        """Set agent or create one if not provided."""
        if agent is None:
            self._agent = createAgentBrain(self._brain)
            self._agent.initialize(self._world)
        else:
            self._agent = agent
        return self
    
    def run_episode(self, num_steps: int, enable_learning: bool = True) -> "EpisodeSimulator":
        """
        Run a single episode.
        
        Args:
            num_steps: Number of steps to simulate
            enable_learning: Whether to enable learning during simulation
        """
        if self._agent is None:
            self.with_agent()
        
        if VERBOSE_LOGGING:
            print(f"[NLM] Running episode with {num_steps} steps")
        
        episode_reward = 0.0
        
        for step in range(num_steps):
            # Get sensory input
            percept = self._world.getSensoryPercept()
            
            # Process sensory input
            self._agent.processSensoryInput(percept)
            
            # Run brain step
            self._brain.step(step)
            
            # Decode motor command
            action = self._agent.decodeMotorCommand()
            self._world.applyMotorCommand(action, self._world.getSimulationTime())
            
            # Apply learning if enabled
            if enable_learning:
                reward = self._agent.getNeuromodulationLevel()
                episode_reward += reward
                self._agent.applyRewardModulation(reward, 0.0)
                self._agent.updateDevelopment(0.1)
        
        # Collect episode statistics
        self._episode_stats = {
            "steps_completed": num_steps,
            "final_reward": episode_reward,
            "final_firing_rate": self._brain.getAverageFiringRate(),
            "final_spike_count": self._brain.getTotalSpikeCount(),
        }
        
        if VERBOSE_LOGGING:
            print(f"[NLM] Episode completed: Reward = {episode_reward:.2f}, "
                  f"Firing rate = {self._brain.getAverageFiringRate():.2f}")
        
        return self
    
    def get_stats(self) -> Dict[str, Any]:
        """Get episode statistics."""
        return self._episode_stats.copy()
    
    def reset(self):
        """Reset simulation state."""
        self._brain.reset()
        self._brain.initialize()
        self._world.reset()
        if self._agent:
            self._agent.reset()
        self._episode_stats = {}
        
        if VERBOSE_LOGGING:
            print("[NLM] Simulation reset")
# Global functions for backward compatibility
def create_brain_with_validation(neuron_count: int = 1000, connection_prob: float = 0.1):
    """Create brain with validation (backward compatibility)."""
    return safe_execute(
        lambda: createBrainWithConfig(neuron_count, connection_prob)
    )

def train_agent_episode(world: SimpleWorld, brain: Brain, num_steps: int = 1000):
    """Train agent episode (backward compatibility)."""
    simulator = EpisodeSimulator(brain, world)
    simulator.run_episode(num_steps)
    return simulator.get_stats()

# Export convenience symbols
__all__.extend([
    "NLMError",
    "ConfigurationError",
    "SimulationError",
    "ImportError",
    "BrainBuilder",
    "EpisodeSimulator",
    "safe_execute",
    "format_error_message",
    "get_version",
    "enable_performance_mode",
    "disable_performance_mode",
    "set_verbose_logging",
    "enable_validation",
    "get_package_info",
    "create_brain_with_validation",
    "train_agent_episode",
])