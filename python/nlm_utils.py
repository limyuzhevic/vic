"""
Python utility module for NLM (熙然) bindings.

This module provides enhanced Python bindings with:
- Convenience methods for common operations
- Validation and type checking helpers
- Better error handling with custom exceptions
- High-level utilities for working with NLM
"""

import sys
import warnings
from typing import Any, Dict, List, Optional, Union, Callable, Tuple
import numpy as np

# Try to import the core NLM module
try:
    import nlm
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False
    nlm = None

# Custom exception hierarchy
class NLMError(Exception):
    """Base exception for all NLM-related errors."""
    pass

class NLMConfigurationError(NLMError):
    """Error in configuration or parameter setup."""
    pass

class NLMExecutionError(NLMError):
    """Error during brain execution or simulation."""
    pass

class NLMValidationError(NLMError):
    """Error in validation or parameter checking."""
    pass

class NLMTypeError(NLMError, TypeError):
    """Type error in Python bindings."""
    pass

# Validation utilities
class NLMValidator:
    """Utility class for validating NLM parameters and configurations."""
    
    @staticmethod
    def validate_config(config: Any, strict: bool = False) -> bool:
        """Validate a configuration object.
        
        Args:
            config: Configuration object to validate
            strict: Whether to perform strict validation
            
        Returns:
            True if configuration is valid
            
        Raises:
            NLMValidationError: If configuration is invalid
        """
        if not NLM_AVAILABLE:
            raise NLMError("NLM module is not available")
        
        if not hasattr(config, 'getKeys'):
            raise NLMValidationError("Invalid configuration object: missing getKeys method")
        
        # Check for required keys in strict mode
        if strict:
            required_keys = ['neuron_count', 'region_count']
            for key in required_keys:
                if not config.has(key):
                    raise NLMValidationError(f"Missing required configuration key: {key}")
        
        return True
    
    @staticmethod
    def validate_neuron_count(count: int) -> bool:
        """Validate neuron count parameter.
        
        Args:
            count: Number of neurons
            
        Returns:
            True if count is valid
            
        Raises:
            NLMValidationError: If count is invalid
        """
        if not isinstance(count, int):
            raise NLMValidationError(f"Neuron count must be an integer, got {type(count)}")
        
        if count <= 0:
            raise NLMValidationError(f"Neuron count must be positive, got {count}")
        
        if count > 1000000:  # Reasonable upper limit
            raise NLMValidationError(f"Neuron count too large: {count}")
        
        return True
    
    @staticmethod
    def validate_step_count(step: int) -> bool:
        """Validate simulation step parameter.
        
        Args:
            step: Simulation step number
            
        Returns:
            True if step is valid
            
        Raises:
            NLMValidationError: If step is invalid
        """
        if not isinstance(step, int):
            raise NLMValidationError(f"Step count must be an integer, got {type(step)}")
        
        if step < 0:
            raise NLMValidationError(f"Step count must be non-negative, got {step}")
        
        return True
    
    @staticmethod
    def validate_array_shape(data: Any, expected_shape: Optional[Tuple[int, ...]] = None) -> bool:
        """Validate array shape and type.
        
        Args:
            data: Array-like data to validate
            expected_shape: Expected shape (optional)
            
        Returns:
            True if array is valid
            
        Raises:
            NLMValidationError: If array is invalid
        """
        if not isinstance(data, (list, tuple, np.ndarray)):
            raise NLMValidationError(f"Data must be array-like, got {type(data)}")
        
        if expected_shape is not None:
            if isinstance(data, np.ndarray):
                actual_shape = data.shape
            else:
                actual_shape = (len(data),)
            
            if actual_shape != expected_shape:
                raise NLMValidationError(
                    f"Expected shape {expected_shape}, got {actual_shape}"
                )
        
        return True
class NLMConvenience:
    """Convenience class providing high-level methods for common NLM operations."""
    
    def __init__(self, brain: Any, world: Any):
        """Initialize convenience wrapper.
        
        Args:
            brain: NLM brain object
            world: NLM world object
        """
        if not NLM_AVAILABLE:
            raise NLMError("NLM module is not available")
        
        self.brain = brain
        self.world = world
        self.config = brain.getConfig()
        
        # Validate inputs
        if not hasattr(brain, 'step'):
            raise NLMValidationError("Invalid brain object: missing step method")
        
        if not hasattr(world, 'update'):
            raise NLMValidationError("Invalid world object: missing update method")
    
    def run_episode(self, steps: int = 100, verbose: bool = False) -> Dict[str, Any]:
        """Run a complete simulation episode.
        
        Args:
            steps: Number of simulation steps to run
            verbose: Whether to print progress information
            
        Returns:
            Dictionary containing episode results
        """
        NLMValidator.validate_step_count(steps)
        
        # Record initial state
        initial_spikes = self.brain.getTotalSpikeCount()
        initial_neurons = self.brain.getTotalNeuronCount()
        initial_weight_mean = self._get_average_synaptic_weight()
        
        # Run simulation
        for step in range(steps):
            # Update world
            self.world.update(0.001)  # Fixed timestep for now
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.brain.receiveSensoryInput(percept)
            
            # Generate action
            action = self.brain.produceAction()
            
            # Apply action to world (simplified)
            if hasattr(self.world, 'applyAction'):
                self.world.applyAction(action)
            
            # Step brain
            self.brain.step(step)
            
            if verbose and (step % 20 == 0 or step == steps - 1):
                spikes = self.brain.getTotalSpikeCount() - initial_spikes
                print(f"Step {step:4d}: {spikes} spikes")
        
        # Record final state
        final_spikes = self.brain.getTotalSpikeCount() - initial_spikes
        final_neurons = self.brain.getTotalNeuronCount()
        final_weight_mean = self._get_average_synaptic_weight()
        
        # Calculate statistics
        weight_change = final_weight_mean - initial_weight_mean if initial_weight_mean > 0 else 0
        spike_rate = final_spikes / steps if steps > 0 else 0
        
        return {
            'steps': steps,
            'initial_spikes': initial_spikes,
            'final_spikes': self.brain.getTotalSpikeCount(),
            'spike_delta': final_spikes - initial_spikes,
            'spike_rate': spike_rate,
            'initial_neurons': initial_neurons,
            'final_neurons': final_neurons,
            'initial_weight_mean': initial_weight_mean,
            'final_weight_mean': final_weight_mean,
            'weight_change': weight_change,
            'learning_detected': abs(weight_change) > 0.001
        }
    
    def _get_average_synaptic_weight(self) -> float:
        """Get average synaptic weight across all regions."""
        total_weight = 0
        total_synapses = 0
        
        for region_id in self.brain.getRegionIds():
            region = self.brain.getRegion(region_id)
            if region:
                # Get synapses from region
                for synapse in region.getSynapses():
                    total_weight += synapse.getWeight()
                    total_synapses += 1
        
        return total_weight / total_synapses if total_synapses > 0 else 0.0
    
    def train_model(self, episodes: int = 10, steps_per_episode: int = 100, 
                   learning_rate: float = 0.01) -> List[Dict[str, Any]]:
        """Train a model using reinforcement learning.
        
        Args:
            episodes: Number of training episodes
            steps_per_episode: Steps per episode
            learning_rate: Learning rate for plasticity
            
        Returns:
            List of episode results
        """
        if not 0.0 <= learning_rate <= 1.0:
            raise NLMValidationError(f"Learning rate must be between 0 and 1, got {learning_rate}")
        
        results = []
        
        for episode in range(episodes):
            print(f"Episode {episode + 1}/{episodes}")
            
            # Run episode
            episode_result = self.run_episode(steps_per_episode, verbose=False)
            
            # Apply learning
            if episode_result['learning_detected']:
                # Scale weights by learning rate
                self._scale_weights(learning_rate)
            
            # Add episode metadata
            episode_result['episode'] = episode
            episode_result['learning_rate'] = learning_rate
            
            results.append(episode_result)
        
        return results
    
    def _scale_weights(self, factor: float):
        """Scale all synaptic weights by a factor."""
        for region_id in self.brain.getRegionIds():
            region = self.brain.getRegion(region_id)
            if region:
                for synapse in region.getSynapses():
                    current_weight = synapse.getWeight()
                    new_weight = current_weight * factor
                    # Note: This is a simplified weight scaling
                    # Real implementations would use proper plasticity rules
    
    def create_experiment_suite(self, experiment_configs: List[Dict[str, Any]]) -> List[Any]:
        """Create multiple experiments with different configurations.
        
        Args:
            experiment_configs: List of experiment configuration dictionaries
            
        Returns:
            List of experiment objects
        """
        experiments = []
        
        for config in experiment_configs:
            # Validate config
            if not isinstance(config, dict):
                raise NLMValidationError(f"Experiment config must be a dict, got {type(config)}")
            
            # Create brain with custom config
            brain_config = nlm.createDefaultConfig()
            
            # Apply experiment-specific settings
            for key, value in config.items():
                brain_config.set(key, value)
            
            brain = nlm.createBrain(brain_config)
            brain.initialize()
            
            # Create agent brain
            agent_brain = nlm.createAgentBrain(brain)
            
            # Configure world if specified
            world = nlm.createSimpleWorld()
            if 'world_width' in config:
                world.configure(
                    config.get('world_width', 100),
                    config.get('world_height', 100),
                    config.get('vision_width', 20),
                    config.get('vision_height', 20)
                )
            
            agent_brain.initialize(world)
            
            experiments.append({
                'brain': brain,
                'agent_brain': agent_brain,
                'world': world,
                'config': config
            })
        
        return experiments
# High-level API functions
def create_brain_from_dict(config_dict: Dict[str, Any]) -> Any:
    """Create a brain from a configuration dictionary.
    
    Args:
        config_dict: Dictionary with brain configuration
        
    Returns:
        Initialized brain object
    """
    if not NLM_AVAILABLE:
        raise NLMError("NLM module is not available")
    
    config = nlm.createDefaultConfig()
    
    for key, value in config_dict.items():
        config.set(key, value)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    return brain
def create_world_from_spec(width: int = 100, height: int = 100, 
                          vision_width: int = 20, vision_height: int = 20) -> Any:
    """Create a world with specified dimensions.
    
    Args:
        width: World width
        height: World height
        vision_width: Vision sensor width
        vision_height: Vision sensor height
        
    Returns:
        Configured world object
    """
    if not NLM_AVAILABLE:
        raise NLMError("NLM module is not available")
    
    world = nlm.createSimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    
    return world
def run_training_session(brain_config: Dict[str, Any], world_config: Dict[str, Any], 
                        episodes: int = 10, steps_per_episode: int = 100,
                        learning_rate: float = 0.01) -> Dict[str, Any]:
    """Run a complete training session.
    
    Args:
        brain_config: Brain configuration dictionary
        world_config: World configuration dictionary
        episodes: Number of training episodes
        steps_per_episode: Steps per episode
        learning_rate: Learning rate
        
    Returns:
        Training results summary
    """
    # Create brain and world
    brain = create_brain_from_dict(brain_config)
    world = create_world_from_spec(**world_config)
    
    # Create agent brain
    agent_brain = nlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Create convenience wrapper
    convenience = NLMConvenience(brain, world)
    
    # Run training
    results = convenience.train_model(episodes, steps_per_episode, learning_rate)
    
    # Calculate summary
    final_result = results[-1]
    
    return {
        'final_result': final_result,
        'all_results': results,
        'total_episodes': episodes,
        'total_steps': episodes * steps_per_episode,
        'average_spike_rate': sum(r.get('spike_rate', 0) for r in results) / episodes,
        'learning_episodes': sum(1 for r in results if r.get('learning_detected', False)),
        'final_weight_change': final_result.get('weight_change', 0)
    }
# Performance monitoring utilities
class NLMBenchmark:
    """Utility class for benchmarking NLM performance."""
    
    def __init__(self):
        self.timings = {}
        self.memory_usage = []
    
    def time_operation(self, name: str, func: Callable, *args, **kwargs) -> Any:
        """Time an NLM operation.
        
        Args:
            name: Name of the operation
            func: Function to time
            *args: Function arguments
            **kwargs: Function keyword arguments
            
        Returns:
            Function result
        """
        import time
        
        start_time = time.time()
        result = func(*args, **kwargs)
        end_time = time.time()
        
        elapsed = end_time - start_time
        
        if name not in self.timings:
            self.timings[name] = []
        
        self.timings[name].append(elapsed)
        
        return result
    
    def get_timing_stats(self, name: str) -> Dict[str, float]:
        """Get timing statistics for an operation.
        
        Args:
            name: Operation name
            
        Returns:
            Dictionary with timing statistics
        """
        if name not in self.timings:
            return {}
        
        times = self.timings[name]
        
        return {
            'count': len(times),
            'mean': sum(times) / len(times),
            'min': min(times),
            'max': max(times),
            'std': (sum((t - sum(times) / len(times)) ** 2 for t in times) / len(times)) ** 0.5
        }
    
    def print_summary(self):
        """Print benchmark summary."""
        print("\n" + "="*60)
        print("BENCHMARK SUMMARY")
        print("="*60)
        
        for name, times in self.timings.items():
            stats = self.get_timing_stats(name)
            print(f"\n{name}:")
            print(f"  Count: {stats['count']}")
            print(f"  Mean: {stats['mean']:.4f}s")
            print(f"  Min: {stats['min']:.4f}s")
            print(f"  Max: {stats['max']:.4f}s")
            print(f"  Std: {stats['std']:.4f}s")

# Example usage demonstration
def demonstrate_basic_usage():
    """Demonstrate basic NLM usage with examples."""
    print("Basic NLM Usage Examples")
    print("=" * 40)
    
    if not NLM_AVAILABLE:
        print("NLM module not available. Skipping examples.")
        return
    
    # Example 1: Create and initialize a brain
    print("\n1. Creating a brain...")
    config = nlm.createDefaultConfig()
    config.set('neuron_count', 100)
    config.set('region_count', 1)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"   Brain initialized: {brain.getTotalNeuronCount()} neurons")
    
    # Example 2: Run simulation steps
    print("\n2. Running simulation steps...")
    initial_spikes = brain.getTotalSpikeCount()
    
    for step in range(10):
        brain.step(step)
        current_spikes = brain.getTotalSpikeCount() - initial_spikes
        print(f"   Step {step}: {current_spikes} spikes")
    
    # Example 3: Create world and agent brain
    print("\n3. Creating world and agent brain...")
    world = nlm.createSimpleWorld()
    world.configure(100, 100, 20, 20)
    
    agent_brain = nlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    print(f"   World created: {world.getWidth()}x{world.getHeight()}")
    print(f"   Agent brain initialized")
    
    # Example 4: Use convenience wrapper
    print("\n4. Using convenience wrapper...")
    convenience = NLMConvenience(brain, world)
    
    # Run a short episode
    results = convenience.run_episode(steps=5, verbose=True)
    
    print(f"   Episode completed: {results['spike_delta']} spikes change")
    print(f"   Learning detected: {results['learning_detected']}")
    
    # Example 5: Configuration validation
    print("\n5. Configuration validation...")
    try:
        validator = NLMValidator()
        validator.validate_neuron_count(50)
        print("   Neuron count validation passed")
    except NLMValidationError as e:
        print(f"   Validation failed: {e}")
    
    print("\n" + "=" * 40)
    print("Basic usage demonstration complete!")
# Version information
__version__ = getattr(nlm, '__version__', 'unknown')
__author__ = getattr(nlm, '__author__', 'NLM Authors')
__description__ = getattr(nlm, '__description__', 'Python bindings for NLM')

# Create convenience instance for common operations
convention = None
if NLM_AVAILABLE:
    try:
        convention = NLMConvenience(
            nlm.createBrain(nlm.createDefaultConfig()),
            nlm.createSimpleWorld()
        )
    except:
        convention = None

# Export commonly used classes and functions
__all__ = [
    'NLM_AVAILABLE',
    'NLMError',
    'NLMConfigurationError',
    'NLMExecutionError',
    'NLMValidationError',
    'NLMTypeError',
    'NLMValidator',
    'NLMConvenience',
    'NLMBenchmark',
    'create_brain_from_dict',
    'create_world_from_spec',
    'run_training_session',
    'demonstrate_basic_usage',
    '__version__',
    '__author__',
    '__description__'
]

if __name__ == '__main__':
    # Run demonstration when module is executed directly
    demonstrate_basic_usage()
