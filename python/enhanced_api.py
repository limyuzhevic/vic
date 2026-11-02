# Enhanced NLMSystem - High-level interface for neural simulation

This class provides a comprehensive, high-level interface for working with NLM brains.
It encapsulates common patterns, provides advanced simulation modes, and offers
enhanced debugging and monitoring capabilities.
"""

import time
import threading
from typing import Optional, List, Dict, Any, Callable, Union
from dataclasses import dataclass, field
from enum import Enum
import numpy as np

class SimulationMode(Enum):
    """Available simulation modes"""
    BASIC = "basic"           # Simple forward simulation
    LEARNING = "learning"     # With plasticity and learning
    ADAPTIVE = "adaptive"     # With adaptation and development
    EXPLORATION = "exploration" # With curiosity-driven exploration
    COMPREHENSIVE = "comprehensive"  # Full feature set

@dataclass
class SimulationStats:
    """Simulation statistics and metrics"""
    steps_completed: int = 0
    total_spikes: int = 0
    firing_rate: float = 0.0
    learning_rate: float = 0.0
    plasticity_effect: float = 0.0
    energy_consumed: float = 0.0
    simulation_time: float = 0.0
    memory_usage: float = 0.0

class NLMEnhancedError(Exception):
    """Base exception for enhanced NLM errors"""
    pass

class ConfigurationError(NLMEnhancedError):
    """Error in configuration"""
    pass

class SimulationError(NLMEnhancedError):
    """Error during simulation"""
    pass

class NLMSystem:
    """
    Enhanced Neural Learning Machine system with high-level interface.
    
    This class provides a comprehensive interface for creating, configuring,
    and running neural simulations with advanced features and monitoring.
    
    Examples:
        >>> # Basic usage
        >>> config = nlm.createDefaultConfig()
        >>> brain = nlm.createBrain(config)
        >>> brain.initialize()
        >>> 
        >>> # Run simulation with monitoring
        >>> system = nlm.NLMSystem(brain)
        >>> for step in range(100):
        ...     stats = system.step()
        ...     if stats.firing_rate > 50:
        ...         print(f"High activity at step {step}")
        >>> 
        >>> # Learning simulation
        >>> learning_system = nlm.NLMSystem.createLearningSimulation(1000)
        >>> results = learning_system.run()
    """
    
    def __init__(self, brain: Any, config: Optional[Any] = None):
        """
        Initialize an enhanced NLM system.
        
        Args:
            brain: The NLM brain instance
            config: Optional configuration override
            
        Raises:
            ConfigurationError: If brain or config is invalid
        """
        if not brain:
            raise ConfigurationError("Brain cannot be None")
        
        try:
            # Verify brain interface
            if not hasattr(brain, 'initialize') or not callable(brain.initialize):
                raise ConfigurationError("Brain must have initialize() method")
            if not hasattr(brain, 'step') or not callable(brain.step):
                raise ConfigurationError("Brain must have step() method")
            if not hasattr(brain, 'getTotalNeuronCount'):
                raise ConfigurationError("Brain must have getTotalNeuronCount() method")
                
        except Exception as e:
            if isinstance(e, ConfigurationError):
                raise
            raise ConfigurationError(f"Invalid brain interface: {e}")
        
        self.brain = brain
        self.config = config or self._create_default_config()
        self.stats = SimulationStats()
        self.is_running = False
        self.step_callbacks: List[Callable] = []
        self.error_callbacks: List[Callable] = []
        self._lock = threading.Lock()
        
    def _create_default_config(self) -> Any:
        """Create default configuration for enhanced system"""
        try:
            config = nlm_core.createDefaultConfig()
            # Enhanced defaults
            config.set("simulation.simulation_mode", "comprehensive")
            config.set("learning.plasticity_enabled", True)
            config.set("learning.stdp_enabled", True)
            config.set("learning.hebbian_enabled", True)
            config.set("neuromodulation.curiosity_enabled", True)
            config.set("neuromodulation.novelty_enabled", True)
            config.set("neuromodulation.dopamine_enabled", True)
            return config
        except Exception as e:
            raise ConfigurationError(f"Failed to create default config: {e}")
    
    def initialize(self) -> bool:
        """
        Initialize the NLM system.
        
        Returns:
            True if initialization successful, False otherwise
            
        Raises:
            SimulationError: If initialization fails
        """
        try:
            with self._lock:
                if self.is_running:
                    return True
                
                # Initialize brain with enhanced error handling
                if not self.brain.initialize():
                    raise SimulationError("Brain initialization failed")
                
                self.is_running = True
                self.stats.steps_completed = 0
                self._reset_stats()
                
                return True
                
        except Exception as e:
            self.is_running = False
            if isinstance(e, (SimulationError, ConfigurationError)):
                raise
            raise SimulationError(f"Initialization failed: {e}")
    
    def step(self) -> SimulationStats:
        """
        Perform one simulation step and return statistics.
        
        Returns:
            Simulation statistics for this step
            
        Raises:
            SimulationError: If step fails
        """
        if not self.is_running:
            raise SimulationError("System not running. Call initialize() first.")
        
        try:
            with self._lock:
                step_start = time.time()
                
                # Execute brain step
                self.brain.step(self.stats.steps_completed)
                
                # Update statistics
                self._update_stats()
                
                # Execute callbacks
                for callback in self.step_callbacks:
                    try:
                        callback(self.stats)
                    except Exception as e:
                        # Log but don't fail step
                        pass
                
                self.stats.steps_completed += 1
                self.stats.simulation_time = time.time() - step_start
                
                return self.stats
                
        except Exception as e:
            if isinstance(e, SimulationError):
                raise
            raise SimulationError(f"Step failed: {e}")
    
    def run_simulation(self, num_steps: int) -> List[SimulationStats]:
        """
        Run multiple simulation steps.
        
        Args:
            num_steps: Number of steps to run
            
        Returns:
            List of statistics for each step
            
        Raises:
            SimulationError: If simulation fails
        """
        if num_steps <= 0:
            raise ConfigurationError("Number of steps must be positive")
        
        try:
            results = []
            for step in range(num_steps):
                stats = self.step()
                results.append(stats)
                
                # Progress monitoring
                if (step + 1) % 100 == 0:
                    progress = (step + 1) / num_steps * 100
                    print(f"Progress: {progress:.1f}% ({step + 1}/{num_steps} steps)")
            
            return results
            
        except Exception as e:
            if isinstance(e, SimulationError):
                raise
            raise SimulationError(f"Simulation failed: {e}")
    
    def add_step_callback(self, callback: Callable[[SimulationStats], None]):
        """
        Add a callback to be called after each step.
        
        Args:
            callback: Function that takes SimulationStats as parameter
        """
        self.step_callbacks.append(callback)
    
    def remove_step_callback(self, callback: Callable[[SimulationStats], None]):
        """
        Remove a step callback.
        
        Args:
            callback: The callback function to remove
        """
        try:
            self.step_callbacks.remove(callback)
        except ValueError:
            pass
    
    def get_learning_summary(self) -> Dict[str, Any]:
        """
        Get a summary of learning and adaptation.
        
        Returns:
            Dictionary containing learning metrics
        """
        try:
            return {
                'total_steps': self.stats.steps_completed,
                'total_spikes': self.stats.total_spikes,
                'average_firing_rate': self.stats.firing_rate,
                'learning_efficiency': self.stats.learning_rate,
                'plasticity_effect': self.stats.plasticity_effect,
                'adaptation_level': self._calculate_adaptation(),
                'energy_efficiency': self.stats.energy_consumed / max(1, self.stats.simulation_time)
            }
        except Exception as e:
            raise NLMEnhancedError(f"Failed to get learning summary: {e}")
    
    def export_brain_state(self, filepath: str) -> bool:
        """
        Export brain state to file.
        
        Args:
            filepath: Path to output file
            
        Returns:
            True if export successful, False otherwise
        """
        try:
            return self.brain.save(filepath)
        except Exception as e:
            raise SimulationError(f"Failed to export brain state: {e}")
    
    def import_brain_state(self, filepath: str) -> bool:
        """
        Import brain state from file.
        
        Args:
            filepath: Path to input file
            
        Returns:
            True if import successful, False otherwise
        """
        try:
            return self.brain.load(filepath)
        except Exception as e:
            raise SimulationError(f"Failed to import brain state: {e}")
    
    def _reset_stats(self):
        """Reset simulation statistics"""
        self.stats = SimulationStats()
    
    def _update_stats(self):
        """Update simulation statistics from brain"""
        try:
            # Get basic stats from brain
            self.stats.total_spikes = self.brain.getTotalSpikeCount()
            self.stats.firing_rate = self.brain.getAverageFiringRate()
            
            # Calculate derived metrics
            if self.stats.steps_completed > 0:
                self.stats.learning_rate = self.stats.plasticity_effect / self.stats.steps_completed
                self.stats.plasticity_effect = min(1.0f, self.stats.plasticity_effect + 0.01f)
            
            # Memory usage estimation
            self.stats.memory_usage = self.brain.getTotalNeuronCount() * 0.001  # Rough estimate
            
        except Exception:
            # Keep default values if stats update fails
            pass
    
    def _calculate_adaptation(self) -> float:
        """Calculate adaptation level"""
        # Simple adaptation calculation based on firing rate changes
        return min(1.0f, self.stats.firing_rate / 100.0)
    
    @classmethod
    def create_learning_simulation(cls, num_steps: int, learning_mode: str = "balanced") -> 'NLMSystem':
        """
        Create a learning simulation with predefined parameters.
        
        Args:
            num_steps: Number of simulation steps
            learning_mode: Learning mode ('balanced', 'intensive', 'exploratory', 'conservative')
            
        Returns:
            Configured NLMSystem instance
            
        Raises:
            ConfigurationError: If learning mode is invalid
        """
        try:
            config = nlm_core.createDefaultConfig()
            
            # Set configuration based on learning mode
            if learning_mode == "balanced":
                config.set("brain.neuron_count", 500)
                config.set("brain.synapse_density", 0.1)
                config.set("learning.plasticity_rate", 0.1)
                config.set("neuromodulation.curiosity_weight", 0.5)
                
            elif learning_mode == "intensive":
                config.set("brain.neuron_count", 1000)
                config.set("brain.synapse_density", 0.15)
                config.set("learning.plasticity_rate", 0.2)
                config.set("neuromodulation.curiosity_weight", 0.8)
                
            elif learning_mode == "exploratory":
                config.set("brain.neuron_count", 300)
                config.set("brain.synapse_density", 0.08)
                config.set("learning.plasticity_rate", 0.05)
                config.set("neuromodulation.curiosity_weight", 1.0)
                
            elif learning_mode == "conservative":
                config.set("brain.neuron_count", 200)
                config.set("brain.synapse_density", 0.05)
                config.set("learning.plasticity_rate", 0.02)
                config.set("neuromodulation.curiosity_weight", 0.3)
                
            else:
                raise ConfigurationError(f"Unknown learning mode: {learning_mode}")
            
            brain = nlm_core.createBrain(config)
            return cls(brain, config)
            
        except Exception as e:
            if isinstance(e, ConfigurationError):
                raise
            raise ConfigurationError(f"Failed to create learning simulation: {e}")
    
    @classmethod
    def create_reproduction_test(cls, template_brain_path: str) -> 'NLMSystem':
        """
        Create a system using an existing brain state for reproduction testing.
        
        Args:
            template_brain_path: Path to brain state file
            
        Returns:
            Configured NLMSystem instance
            
        Raises:
            ConfigurationError: If brain state cannot be loaded
        """
        try:
            # Try to load existing brain state
            config = nlm_core.createDefaultConfig()
            brain = nlm_core.createBrain(config)
            
            if not brain.load(template_brain_path):
                raise ConfigurationError(f"Failed to load brain state from {template_brain_path}")
            
            return cls(brain, config)
            
        except Exception as e:
            if isinstance(e, ConfigurationError):
                raise
            raise ConfigurationError(f"Failed to create reproduction test: {e}")


# Factory functions for common use cases
def create_default_config() -> Any:
    """Create a default configuration optimized for most use cases."""
    return nlm_core.createDefaultConfig()


def create_brain(config: Optional[Any] = None) -> Any:
    """Create a brain with the given configuration."""
    if config is None:
        config = create_default_config()
    return nlm_core.createBrain(config)


def run_simulation(brain: Any, num_steps: int, monitor_callback: Optional[Callable] = None) -> List[Dict[str, Any]]:
    """
    Run a complete simulation with optional monitoring.
    
    Args:
        brain: The brain to simulate
        num_steps: Number of steps to run
        monitor_callback: Optional callback for monitoring progress
        
    Returns:
        List of step statistics
    """
    system = NLMSystem(brain)
    system.initialize()
    
    if monitor_callback:
        system.add_step_callback(monitor_callback)
    
    return system.run_simulation(num_steps)


def benchmark_performance(brain: Any, num_steps: int) -> Dict[str, Any]:
    """
    Benchmark brain performance over multiple steps.
    
    Args:
        brain: The brain to benchmark
        num_steps: Number of steps to run
        
    Returns:
        Performance metrics
    """
    import time
    
    system = NLMSystem(brain)
    system.initialize()
    
    start_time = time.time()
    results = system.run_simulation(num_steps)
    end_time = time.time()
    
    duration = end_time - start_time
    steps_per_second = num_steps / duration if duration > 0 else 0
    
    return {
        'total_steps': num_steps,
        'duration_seconds': duration,
        'steps_per_second': steps_per_second,
        'average_firing_rate': sum(r.firing_rate for r in results) / len(results) if results else 0,
        'peak_firing_rate': max(r.firing_rate for r in results) if results else 0,
        'total_spikes': sum(r.total_spikes for r in results) if results else 0,
        'efficiency_score': calculate_efficiency(results)
    }


def calculate_efficiency(results: List[SimulationStats]) -> float:
    """Calculate simulation efficiency score."""
    if not results:
        return 0.0
    
    # Simple efficiency calculation based on learning per step
    total_plasticity = sum(r.plasticity_effect for r in results)
    total_steps = len(results)
    
    return min(1.0, total_plasticity / total_steps) * 0.5 + 0.5  # Normalize to 0-1