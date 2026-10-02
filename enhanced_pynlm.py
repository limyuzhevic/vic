"""
NLM (Neural Learning Machine) Python Bindings
================================================

This module provides enhanced Python bindings for the NLM neural simulation framework.
It extends the basic C++ bindings with Pythonic features, batch operations,
and integration with the scientific Python ecosystem.

Features:
- Pythonic API with context managers and properties
- Batch processing capabilities
- NumPy integration for array operations
- Enhanced error handling with custom exceptions
- Progress callbacks and event system
- Serialization and checkpoint management
- Visualization integration
"""

import sys
import time
import threading
import warnings
from contextlib import contextmanager
from typing import (
    Any, Callable, Dict, List, Optional, Tuple, Union, 
    Iterator, Iterable, Sequence, overload
)
from dataclasses import dataclass, field
import numpy as np

# Import the core NLM bindings
try:
    import pynlm
    HAS_PYNLM = True
except ImportError:
    HAS_PYNLM = False
    pynlm = None

# Custom exception classes
class NLMBrainError(Exception):
    """Base exception for NLM brain-related errors."""
    pass

class NLMBrainNotInitializedError(NLMBrainError):
    """Raised when brain methods are called before initialization."""
    pass

class NLMInvalidConfigError(NLMBrainError):
    """Raised when configuration is invalid."""
    pass

class NLMBoundaryError(NLMBrainError):
    """Raised when position is outside world bounds."""
    pass

class NLMMemoryError(NLMBrainError):
    """Raised when memory operations fail."""
    pass

class NLMTimeoutError(NLMBrainError):
    """Raised when operations timeout."""
    pass

class NLMBatchError(NLMBrainError):
    """Raised when batch operations fail."""
    pass

@dataclass
class BatchResult:
    """Result of batch processing operations."""
    success: bool = True
    error_message: Optional[str] = None
    processed_count: int = 0
    total_count: int = 0
    processing_time: float = 0.0
    results: List[Any] = field(default_factory=list)

    @property
    def progress(self) -> float:
        """Progress percentage (0-100)."""
        if self.total_count == 0:
            return 100.0
        return (self.processed_count / self.total_count) * 100.0

    @property
    def success_rate(self) -> float:
        """Success rate percentage (0-100)."""
        if self.total_count == 0:
            return 100.0
        return (self.processed_count / self.total_count) * 100.0

class ProgressCallback:
    """Callback class for progress tracking."""
    
    def __init__(self, 
                 update_interval: float = 0.1,
                 on_start: Optional[Callable] = None,
                 on_progress: Optional[Callable] = None,
                 on_complete: Optional[Callable] = None,
                 on_error: Optional[Callable] = None):
        self.update_interval = update_interval
        self.on_start = on_start
        self.on_progress = on_progress
        self.on_complete = on_complete
        self.on_error = on_error
        self.last_update = 0.0
        self.start_time = time.time()
    
    def update(self, processed: int, total: int, message: str = ""):
        """Update progress callback."""
        current_time = time.time()
        elapsed = current_time - self.start_time
        
        if self.on_start and elapsed < self.update_interval:
            self.on_start(processed, total, elapsed)
        
        if (current_time - self.last_update >= self.update_interval or 
            processed == total):
            progress = (processed / total * 100) if total > 0 else 100.0
            if self.on_progress:
                self.on_progress(progress, processed, total, elapsed, message)
            self.last_update = current_time
        
        if processed == total and self.on_complete:
            self.on_complete(processed, total, elapsed)
    
    def error(self, error: Exception):
        """Handle error callback."""
        if self.on_error:
            self.on_error(error)

class NeuralNetworkManager:
    """
    Enhanced neural network manager with Pythonic API and batch operations.
    
    This class provides a high-level interface to the NLM brain with
    Pythonic features like context managers, batch processing, and
    enhanced error handling.
    """
    
    def __init__(self, config: Optional[Any] = None):
        """
        Initialize neural network manager.
        
        Args:
            config: Configuration object. If None, uses default config.
        """
        if not HAS_PYNLM:
            raise ImportError("pynlm module not found. Please install NLM Python bindings.")
        
        self._config = config if config is not None else pynlm.createDefaultConfig()
        self._brain = None
        self._initialized = False
        self._progress_callbacks: List[ProgressCallback] = []
        self._lock = threading.RLock()
    
    @property
    def config(self) -> Any:
        """Get the configuration object."""
        return self._config
    
    @property
    def brain(self) -> Any:
        """Get the brain object (raises if not initialized)."""
        if not self._initialized:
            raise NLMBrainNotInitializedError("Brain not initialized. Call initialize() first.")
        return self._brain
    
    @property
    def is_initialized(self) -> bool:
        """Check if brain is initialized."""
        return self._initialized
    
    @property
    def neuron_count(self) -> int:
        """Get total number of neurons."""
        return self.brain.getTotalNeuronCount() if self._initialized else 0
    
    @property
    def synapse_count(self) -> int:
        """Get total number of synapses."""
        return self.brain.getTotalSynapseCount() if self._initialized else 0
    
    @property
    def firing_neuron_count(self) -> int:
        """Get count of currently firing neurons."""
        return self.brain.getFiringNeuronCount() if self._initialized else 0
    
    @property
    def average_firing_rate(self) -> float:
        """Get average firing rate across all neurons."""
        return self.brain.getAverageFiringRate() if self._initialized else 0.0
    
    @property
    def developmental_stage(self) -> Any:
        """Get current developmental stage."""
        return self.brain.getDevelopmentalStage() if self._initialized else None
    
    def initialize(self, num_steps: int = 1000) -> 'NeuralNetworkManager':
        """
        Initialize the brain.
        
        Args:
            num_steps: Number of steps to run for initialization.
        
        Returns:
            Self for method chaining.
        """
        with self._lock:
            if self._initialized:
                raise NLMBrainError("Brain already initialized.")
            
            try:
                self._brain = pynlm.createBrain(self._config)
                self._brain.initialize()
                self._initialized = True
                
                # Run initialization steps
                for i in range(num_steps):
                    self._brain.step(i)
                
                return self
                
            except Exception as e:
                self._initialized = False
                self._brain = None
                raise NLMBrainError(f"Failed to initialize brain: {e}") from e
    
    def reset(self) -> 'NeuralNetworkManager':
        """
        Reset the brain to its initial state.
        
        Returns:
            Self for method chaining.
        """
        if not self._initialized:
            raise NLMBrainNotInitializedError("Brain not initialized.")
        
        try:
            self._brain.reset()
            return self
        except Exception as e:
            raise NLMBrainError(f"Failed to reset brain: {e}") from e
    
    def step(self, step_number: int) -> 'NeuralNetworkManager':
        """
        Perform one simulation step.
        
        Args:
            step_number: Current step number.
        
        Returns:
            Self for method chaining.
        """
        if not self._initialized:
            raise NLMBrainNotInitializedError("Brain not initialized.")
        
        try:
            self._brain.step(step_number)
            return self
        except Exception as e:
            raise NLMBrainError(f"Failed to step brain: {e}") from e
    
    def run_batch(self, 
                  num_steps: int, 
                  progress_callback: Optional[ProgressCallback] = None,
                  timeout: Optional[float] = None) -> BatchResult:
        """
        Run a batch of simulation steps.
        
        Args:
            num_steps: Number of steps to run.
            progress_callback: Optional progress callback.
            timeout: Maximum time in seconds (None for no limit).
        
        Returns:
            BatchResult object with processing information.
        """
        if not self._initialized:
            raise NLMBrainNotInitializedError("Brain not initialized.")
        
        result = BatchResult(total_count=num_steps)
        start_time = time.time()
        
        try:
            if progress_callback:
                result.on_start = lambda p, t, e: progress_callback.update(p, t, "Starting batch")
                result.on_progress = lambda p, pr, t, e, m: progress_callback.update(p, t, f"Processing: {m}")
                result.on_complete = lambda p, t, e: progress_callback.update(p, t, "Batch complete")
                result.on_error = progress_callback.error
            
            for i in range(num_steps):
                try:
                    self._brain.step(i)
                    result.processed_count = i + 1
                    result.results.append(i)
                    
                    if progress_callback:
                        progress_callback.update(i + 1, num_steps, f"Step {i}")
                    
                    if timeout and (time.time() - start_time) > timeout:
                        raise NLMTimeoutError(f"Batch processing timed out after {timeout} seconds")
                
                except Exception as e:
                    result.success = False
                    result.error_message = str(e)
                    if progress_callback:
                        progress_callback.error(e)
                    raise NLMBatchError(f"Error at step {i}: {e}") from e
            
            result.processing_time = time.time() - start_time
            return result
            
        except Exception as e:
            if isinstance(e, NLMBatchError):
                raise
            raise NLMBatchError(f"Batch processing failed: {e}") from e
    
    def get_statistics(self, include_history: bool = False) -> Dict[str, Any]:
        """
        Get comprehensive brain statistics.
        
        Args:
            include_history: Whether to include processing history.
        
        Returns:
            Dictionary with statistics.
        """
        if not self._initialized:
            raise NLMBrainNotInitializedError("Brain not initialized.")
        
        stats = {
            'neurons': {
                'total': self.neuron_count,
                'firing': self.firing_neuron_count,
                'average_firing_rate': self.average_firing_rate,
            },
            'synapses': {
                'total': self.synapse_count,
            },
            'development': {
                'stage': self.developmental_stage,
            },
            'performance': {
                'initialization_time': None,  # Could be stored during initialization
                'last_step_time': None,
                'steps_processed': None,
            }
        }
        
        if include_history:
            stats['history'] = {
                'firing_rate_history': [],
                'neuron_count_history': [],
            }
        
        return stats
    
    def add_progress_callback(self, callback: ProgressCallback):
        """
        Add a progress callback.
        
        Args:
            callback: ProgressCallback instance.
        """
        self._progress_callbacks.append(callback)
    
    def remove_progress_callback(self, callback: ProgressCallback):
        """
        Remove a progress callback.
        
        Args:
            callback: ProgressCallback instance to remove.
        """
        if callback in self._progress_callbacks:
            self._progress_callbacks.remove(callback)
    
    @contextmanager
    def simulation_context(self, steps: int, timeout: Optional[float] = None) -> Iterator['NeuralNetworkManager']:
        """
        Context manager for simulation runs.
        
        Args:
            steps: Number of steps to run.
            timeout: Maximum time in seconds.
        
        Yields:
            Self for use within context.
        """
        if not self._initialized:
            raise NLMBrainNotInitializedError("Brain not initialized.")
        
        try:
            for i in range(steps):
                self.step(i)
                if timeout and i > 0:
                    # Check elapsed time every 100 steps to avoid overhead
                    if i % 100 == 0 and time.time() > self._start_time + timeout:
                        raise NLMTimeoutError(f"Simulation timed out after {timeout} seconds")
            
            yield self
            
        except Exception as e:
            raise
        finally:
            self._start_time = time.time()
    
    def save_checkpoint(self, filepath: str) -> 'NeuralNetworkManager':
        """
        Save brain state to checkpoint file.
        
        Args:
            filepath: Path to save checkpoint.
        
        Returns:
            Self for method chaining.
        """
        if not self._initialized:
            raise NLMBrainNotInitializedError("Brain not initialized.")
        
        try:
            self._brain.save(filepath)
            return self
        except Exception as e:
            raise NLMBrainError(f"Failed to save checkpoint: {e}") from e
    
    def load_checkpoint(self, filepath: str) -> 'NeuralNetworkManager':
        """
        Load brain state from checkpoint file.
        
        Args:
            filepath: Path to load checkpoint from.
        
        Returns:
            Self for method chaining.
        """
        if not self._initialized:
            raise NLMBrainNotInitializedError("Brain not initialized.")
        
        try:
            self._brain.load(filepath)
            return self
        except Exception as e:
            raise NLMBrainError(f"Failed to load checkpoint: {e}") from e

class SimpleWorldManager:
    """
    Enhanced world manager with Pythonic API.
    
    This class provides a high-level interface to the SimpleWorld with
    Pythonic features and enhanced functionality.
    """
    
    def __init__(self, width: int = 20, height: int = 20, 
                 vision_width: int = 8, vision_height: int = 8):
        """
        Initialize world manager.
        
        Args:
            width: World width.
            height: World height.
            vision_width: Vision width (sensory range).
            vision_height: Vision height (sensory range).
        """
        if not HAS_PYNLM:
            raise ImportError("pynlm module not found. Please install NLM Python bindings.")
        
        self._width = width
        self._height = height
        self._vision_width = vision_width
        self._vision_height = vision_height
        self._world = None
        self._agent = None
        self._brain = None
        self._initialized = False
    
    @property
    def world(self) -> Any:
        """Get the world object (raises if not initialized)."""
        if not self._initialized:
            raise NLMBrainNotInitializedError("World not initialized. Call initialize() first.")
        return self._world
    
    @property
    def agent(self) -> Any:
        """Get the agent object (raises if not initialized)."""
        if not self._initialized:
            raise NLMBrainNotInitializedError("World not initialized. Call initialize() first.")
        return self._agent
    
    @property
    def brain(self) -> Optional[Any]:
        """Get the brain object."""
        return self._brain
    
    @property
    def is_initialized(self) -> bool:
        """Check if world is initialized."""
        return self._initialized
    
    @property
    def width(self) -> int:
        """Get world width."""
        return self._width
    
    @property
    def height(self) -> int:
        """Get world height."""
        return self._height
    
    def configure(self, 
                  width: Optional[int] = None,
                  height: Optional[int] = None,
                  vision_width: Optional[int] = None,
                  vision_height: Optional[int] = None) -> 'SimpleWorldManager':
        """
        Configure world parameters.
        
        Args:
            width: World width (optional).
            height: World height (optional).
            vision_width: Vision width (optional).
            vision_height: Vision height (optional).
        
        Returns:
            Self for method chaining.
        """
        if width is not None:
            self._width = width
        if height is not None:
            self._height = height
        if vision_width is not None:
            self._vision_width = vision_width
        if vision_height is not None:
            self._vision_height = vision_height
        
        if self._world is not None:
            self._world.configure(
                self._width, self._height,
                self._vision_width, self._vision_height
            )
        
        return self
    
    def initialize(self, brain: Optional[Any] = None) -> 'SimpleWorldManager':
        """
        Initialize the world.
        
        Args:
            brain: Brain object to connect to world. If None, will be set later.
        
        Returns:
            Self for method chaining.
        """
        if self._initialized:
            raise NLMBrainError("World already initialized.")
        
        try:
            self._brain = brain
            self._world = pynlm.createSimpleWorld()
            self._world.configure(self._width, self._height, 
                                 self._vision_width, self._vision_height)
            
            if brain is not None:
                self._agent = pynlm.createAgentBrain(brain)
                self._agent.initialize(self._world)
            
            self._world.reset()
            self._initialized = True
            
            return self
            
        except Exception as e:
            self._initialized = False
            self._world = None
            self._agent = None
            raise NLMBrainError(f"Failed to initialize world: {e}") from e
    
    def connect_agent(self, brain: Any) -> 'SimpleWorldManager':
        """
        Connect an agent brain to the world.
        
        Args:
            brain: Brain object to connect.
        
        Returns:
            Self for method chaining.
        """
        if not self._initialized:
            raise NLMBrainNotInitializedError("World not initialized.")
        
        try:
            self._brain = brain
            self._agent = pynlm.createAgentBrain(brain)
            self._agent.initialize(self._world)
            
            return self
            
        except Exception as e:
            raise NLMBrainError(f"Failed to connect agent: {e}") from e
    
    def update(self, timestep: float) -> 'SimpleWorldManager':
        """
        Update the world by the given timestep.
        
        Args:
            timestep: Time step to advance.
        
        Returns:
            Self for method chaining.
        """
        if not self._initialized:
            raise NLMBrainNotInitializedError("World not initialized.")
        
        try:
            self._world.update(timestep)
            return self
        except Exception as e:
            raise NLMBrainError(f"Failed to update world: {e}") from e
    
    def reset(self) -> 'SimpleWorldManager':
        """
        Reset the world to its initial state.
        
        Returns:
            Self for method chaining.
        """
        if not self._initialized:
            raise NLMBrainNotInitializedError("World not initialized.")
        
        try:
            self._world.reset()
            return self
        except Exception as e:
            raise NLMBrainError(f"Failed to reset world: {e}") from e
    
    def get_sensory_percept(self) -> Any:
        """
        Get the current sensory percept from the world.
        
        Returns:
            Sensory percept object.
        """
        if not self._initialized:
            raise NLMBrainNotInitializedError("World not initialized.")
        
        try:
            return self._world.getSensoryPercept()
        except Exception as e:
            raise NLMBrainError(f"Failed to get sensory percept: {e}") from e
    
    def get_agent_body(self) -> Any:
        """
        Get the agent's body state.
        
        Returns:
            Agent body object.
        """
        if not self._initialized:
            raise NLMBrainNotInitializedError("World not initialized.")
        
        try:
            return self._world.getAgentBody()
        except Exception as e:
            raise NLMBrainError(f"Failed to get agent body: {e}") from e
    
    def apply_motor_command(self, action: Any, current_time: float) -> 'SimpleWorldManager':
        """
        Apply a motor command to the world.
        
        Args:
            action: Motor command to apply.
            current_time: Current simulation time.
        
        Returns:
            Self for method chaining.
        """
        if not self._initialized:
            raise NLMBrainNotInitializedError("World not initialized.")
        
        try:
            self._world.applyMotorCommand(action, current_time)
            return self
        except Exception as e:
            raise NLMBrainError(f"Failed to apply motor command: {e}") from e
    
    def run_episode(self, 
                    num_steps: int,
                    brain: Optional[Any] = None,
                    progress_callback: Optional[ProgressCallback] = None,
                    timeout: Optional[float] = None) -> Dict[str, Any]:
        """
        Run a complete episode with world and agent.
        
        Args:
            num_steps: Number of steps to run.
            brain: Brain object to connect. If None, uses connected brain.
            progress_callback: Optional progress callback.
            timeout: Maximum time in seconds.
        
        Returns:
            Dictionary with episode results.
        """
        if not self._initialized:
            raise NLMBrainNotInitializedError("World not initialized.")
        
        if brain is not None:
            self.connect_agent(brain)
        elif self._brain is None:
            raise NLMBrainError("No brain connected. Provide brain parameter or connect first.")
        
        results = {
            'steps_completed': 0,
            'final_percept': None,
            'final_action': None,
            'success': True,
            'error': None,
            'statistics': {}
        }
        
        start_time = time.time()
        
        try:
            for step in range(num_steps):
                # Update world
                self.update(0.1)
                
                # Get sensory percept
                percept = self.get_sensory_percept()
                
                # Process sensory input if agent exists
                if self._agent is not None:
                    self._agent.processSensoryInput(percept)
                    
                    # Get motor command from brain
                    if self._brain is not None:
                        # Note: This is a simplified interface
                        # In practice, you'd use the agent to decode motor commands
                        pass
                
                # Store results
                results['final_percept'] = percept
                
                if progress_callback:
                    progress_callback.update(step + 1, num_steps, 
                                           f"Step {step}: {self._brain.getFiringNeuronCount() if self._brain else 0} neurons firing")
                
                # Check timeout
                if timeout and (time.time() - start_time) > timeout:
                    raise NLMTimeoutError(f"Episode timed out after {timeout} seconds")
            
            results['steps_completed'] = num_steps
            
        except Exception as e:
            results['success'] = False
            results['error'] = str(e)
            if progress_callback:
                progress_callback.error(e)
            raise
        
        return results

# Global convenience functions
def create_default_brain(config: Optional[Any] = None) -> NeuralNetworkManager:
    """
    Create a default brain with Pythonic API.
    
    Args:
        config: Optional configuration. If None, uses default config.
    
    Returns:
        NeuralNetworkManager instance.
    """
    return NeuralNetworkManager(config)


def create_default_world(width: int = 20, height: int = 20,
                        vision_width: int = 8, vision_height: int = 8) -> SimpleWorldManager:
    """
    Create a default world with Pythonic API.
    
    Args:
        width: World width.
        height: World height.
        vision_width: Vision width.
        vision_height: Vision height.
    
    Returns:
        SimpleWorldManager instance.
    """
    return SimpleWorldManager(width, height, vision_width, vision_height)


# NumPy integration functions
@overload
def to_numpy(vision: Any) -> np.ndarray: ...

@overload
def to_numpy(audio: Any) -> np.ndarray: ...

@overload
def to_numpy(internal: Any) -> np.ndarray: ...

@overload
def to_numpy(percept: Any) -> Dict[str, np.ndarray]: ...

def to_numpy(data: Any) -> Union[np.ndarray, Dict[str, np.ndarray]]:
    """
    Convert NLM data to NumPy arrays for analysis.
    
    Args:
        data: Vision, audio, internal signals, or percept data.
    
    Returns:
        NumPy array or dictionary of arrays.
    """
    if hasattr(data, 'getVision') and hasattr(data, 'getWidth'):
        # Vision data
        width = data.getWidth()
        height = data.getHeight()
        channels = data.getChannels() if hasattr(data, 'getChannels') else 1
        
        # Get vision data (this depends on the actual API)
        if hasattr(data, 'getData'):
            vision_data = data.getData()
            return np.array(vision_data).reshape(height, width, channels)
        
    elif hasattr(data, 'getNumSamples'):
        # Audio data
        sample_rate = data.getSampleRate()
        num_samples = data.getNumSamples()
        
        if hasattr(data, 'getData'):
            audio_data = data.getData()
            return np.array(audio_data)
    
    elif hasattr(data, 'getInternal'):
        # Internal signals
        if hasattr(data, 'getInternal'):
            internal_data = data.getInternal()
            return np.array(internal_data)
    
    elif hasattr(data, 'getAllSignals'):
        # Percept data
        signals = data.getAllSignals()
        result = {}
        for key, value in signals.items():
            if hasattr(value, '__iter__'):
                result[key] = np.array(value)
            else:
                result[key] = value
        return result
    
    return data


@overload
def from_numpy(vision_array: np.ndarray, dtype: Any = None) -> Any: ...

@overload
def from_numpy(audio_array: np.ndarray, dtype: Any = None) -> Any: ...

@overload
def from_numpy(internal_array: np.ndarray, dtype: Any = None) -> Any: ...

def from_numpy(data: Any, dtype: Any = None) -> Any:
    """
    Convert NumPy arrays to NLM data.
    
    Args:
        data: NumPy array or dictionary of arrays.
        dtype: Optional target type.
    
    Returns:
        NLM data object.
    """
    if isinstance(data, dict):
        # Convert dictionary of arrays
        result = {}
        for key, value in data.items():
            if isinstance(value, np.ndarray):
                result[key] = value.tolist()
        return result
    elif isinstance(data, np.ndarray):
        # Convert array to list
        return data.tolist()
    
    return data


# Visualization integration
class VisualizationExporter:
    """
    Export NLM data for visualization.
    """
    
    @staticmethod
    def export_brain_data(brain: Any, filepath: str, format: str = "json"):
        """
        Export brain data for visualization.
        
        Args:
            brain: Brain object.
            filepath: Output file path.
            format: Export format ("json", "csv", "npy").
        """
        # Implementation depends on actual data structures
        # This is a placeholder for visualization integration
        warnings.warn("Visualization export not yet implemented", ImportWarning)
    
    @staticmethod
    def export_episode_data(world_manager: SimpleWorldManager, 
                          filepath: str, 
                          include_percepts: bool = True,
                          include_actions: bool = True) -> Dict[str, np.ndarray]:
        """
        Export episode data for visualization.
        
        Args:
            world_manager: World manager instance.
            filepath: Output file path.
            include_percepts: Include sensory percepts.
            include_actions: Include motor actions.
        
        Returns:
            Dictionary of arrays for plotting.
        """
        # Placeholder for visualization export
        warnings.warn("Episode data export not yet implemented", ImportWarning)
        return {}


# Plugin system
class Plugin:
    """Base class for NLM plugins."""
    
    def __init__(self, name: str):
        self.name = name
    
    def initialize(self, *args, **kwargs) -> None:
        """Initialize the plugin."""
        pass
    
    def shutdown(self, *args, **kwargs) -> None:
        """Shutdown the plugin."""
        pass
    
    def process(self, data: Any, *args, **kwargs) -> Any:
        """Process data. Override in subclasses."""
        return data


class PluginManager:
    """Manage NLM plugins."""
    
    def __init__(self):
        self._plugins: Dict[str, Plugin] = {}
    
    def register(self, plugin: Plugin) -> None:
        """
        Register a plugin.
        
        Args:
            plugin: Plugin instance.
        """
        self._plugins[plugin.name] = plugin
        plugin.initialize()
    
    def unregister(self, name: str) -> None:
        """
        Unregister a plugin.
        
        Args:
            name: Plugin name.
        """
        if name in self._plugins:
            self._plugins[name].shutdown()
            del self._plugins[name]
    
    def get_plugin(self, name: str) -> Optional[Plugin]:
        """
        Get a plugin by name.
        
        Args:
            name: Plugin name.
        
        Returns:
            Plugin instance or None.
        """
        return self._plugins.get(name)
    
    def process(self, data: Any, plugin_name: str = None, *args, **kwargs) -> Any:
        """
        Process data through plugins.
        
        Args:
            data: Input data.
            plugin_name: Specific plugin to use. If None, use all plugins.
            *args: Additional arguments.
            **kwargs: Additional keyword arguments.
        
        Returns:
            Processed data.
        """
        if plugin_name:
            plugin = self.get_plugin(plugin_name)
            if plugin:
                return plugin.process(data, *args, **kwargs)
            else:
                raise NLMBrainError(f"Plugin not found: {plugin_name}")
        else:
            result = data
            for plugin in self._plugins.values():
                result = plugin.process(result, *args, **kwargs)
            return result

# Global plugin manager
_plugin_manager = PluginManager()


def get_plugin_manager() -> PluginManager:
    """Get the global plugin manager."""
    return _plugin_manager


# Configuration helpers
class ConfigurationManager:
    """Manage NLM configurations."""
    
    @staticmethod
    def create_default_config() -> Any:
        """Create default configuration."""
        if HAS_PYNLM:
            return pynlm.createDefaultConfig()
        raise ImportError("pynlm module not found.")
    
    @staticmethod
    def load_config(filepath: str) -> Any:
        """
        Load configuration from file.
        
        Args:
            filepath: Configuration file path.
        """
        if HAS_PYNLM:
            config = pynlm.createDefaultConfig()
            # This depends on the actual Config API
            # config.loadFromFile(filepath)
            return config
        raise ImportError("pynlm module not found.")
    
    @staticmethod
    def save_config(config: Any, filepath: str) -> None:
        """
        Save configuration to file.
        
        Args:
            config: Configuration object.
            filepath: File path.
        """
        if hasattr(config, 'saveToFile'):
            config.saveToFile(filepath)
        else:
            raise AttributeError("Config object does not have saveToFile method.")


# Main entry points
__all__ = [
    # Core classes
    'NeuralNetworkManager',
    'SimpleWorldManager',
    'ProgressCallback',
    'BatchResult',
    
    # Exceptions
    'NLMBrainError',
    'NLMBrainNotInitializedError',
    'NLMInvalidConfigError',
    'NLMBoundaryError',
    'NLMMemoryError',
    'NLMTimeoutError',
    'NLMBatchError',
    
    # Convenience functions
    'create_default_brain',
    'create_default_world',
    'to_numpy',
    'from_numpy',
    
    # Utilities
    'VisualizationExporter',
    'Plugin',
    'PluginManager',
    'get_plugin_manager',
    'ConfigurationManager',
    
    # Backwards compatibility
    'pynlm',
]

if HAS_PYNLM:
    # Add original pynlm attributes to this module
    for attr in dir(pynlm):
        if not attr.startswith('_'):
            globals()[attr] = getattr(pynlm, attr)

# Print helpful message if dependencies are missing
if not HAS_PYNLM:
    warnings.warn(
        "NLM Python bindings not available. Install with: pip install pynlm",
        ImportWarning
    )

__version__ = "2.0.0"
__author__ = "NLM Project"
__description__ = "Enhanced Python bindings for the NLM neural simulation framework"