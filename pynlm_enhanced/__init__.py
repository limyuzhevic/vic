#!/usr/bin/env python3
"""
Enhanced NLM Python Module

This module provides enhanced features for the NLM (Neural Learning Machine)
neural simulation framework, including improved APIs, convenience methods,
performance monitoring, and advanced error handling.

Features:
- High-level, Pythonic interfaces
- Advanced error handling with custom exception types
- Performance monitoring and profiling
- Convenience methods for common operations
- Configuration builders and utilities
- Learning simulation helpers
- Context managers for resource management
"""

import sys
import json
import time
from pathlib import Path
from typing import Dict, Any, Optional, Tuple
from contextlib import contextmanager

sys.path.insert(0, str(Path(__file__).parent))

# Import core NLM components
try:
    import pynlm as nlm
    CORE_AVAILABLE = True
except ImportError as e:
    CORE_AVAILABLE = False
    print(f"Warning: Core NLM bindings not available: {e}")
    print("Enhanced API requires the core NLM module.")

# Custom exception types
class NLMError(Exception):
    """Base exception for all NLM errors."""
    def __init__(self, message: str, error_code: Optional[str] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
    
    def __str__(self):
        return f"NLM Error: {self.message}" if self.message else "NLM Error"


class ConfigurationError(NLMError):
    """Exception for configuration-related errors."""
    def __init__(self, message: str):
        super().__init__(f"Configuration: {message}", "CONFIG_ERROR")


class SimulationError(NLMError):
    """Exception for simulation-related errors."""
    def __init__(self, message: str):
        super().__init__(f"Simulation: {message}", "SIM_ERROR")


class ValidationError(NLMError):
    """Exception for validation-related errors."""
    def __init__(self, message: str):
        super().__init__(f"Validation: {message}", "VALIDATION_ERROR")


class PerformanceError(NLMError):
    """Exception for performance-related errors."""
    def __init__(self, message: str):
        super().__init__(f"Performance: {message}", "PERF_ERROR")


# Enhanced configuration utilities
class ConfigBuilder:
    """
    Enhanced configuration builder with Pythonic API.
    
    Provides a more intuitive interface for configuring NLM simulations.
    """
    
    def __init__(self):
        if not CORE_AVAILABLE:
            raise ImportError("Core NLM bindings are required for ConfigBuilder")
        self.config = nlm.createDefaultConfig()
        self._temp_values = {}
    
    def set(self, key: str, value: Any) -> 'ConfigBuilder':
        """
        Set a configuration value.
        
        Args:
            key: Configuration key
            value: Configuration value (int, float, bool, or str)
            
        Returns:
            Self for method chaining
        """
        try:
            if isinstance(value, bool):
                self.config.set(key, value)
            elif isinstance(value, int):
                self.config.set(key, int64_t(value))
            elif isinstance(value, float):
                self.config.set(key, float(value))
            elif isinstance(value, str):
                self.config.set(key, value)
            else:
                raise ConfigurationError(f"Unsupported value type for key '{key}': {type(value)}")
        except Exception as e:
            raise ConfigurationError(f"Failed to set key '{key}': {str(e)}")
        
        return self
    
    def brain(self, neuron_count: int = 500, region_count: int = 2,
              connection_probability: float = 0.1) -> 'ConfigBuilder':
        """
        Configure brain parameters.
        
        Args:
            neuron_count: Number of neurons
            region_count: Number of brain regions
            connection_probability: Connection probability
            
        Returns:
            Self for method chaining
        """
        self.set("neuron_count", neuron_count)
        self.set("region_count", region_count)
        self.set("connection_probability", connection_probability)
        return self
    
    def plasticity(self, learning_rate: float = 0.01, stdp_ltp: float = 0.01,
                   stdp_ltd: float = 0.012) -> 'ConfigBuilder':
        """
        Configure plasticity parameters.
        
        Args:
            learning_rate: Base learning rate
            stdp_ltp: STDP LTP weight
            stdp_ltd: STDP LTD weight
            
        Returns:
            Self for method chaining
        """
        self.set("plasticity_learning_rate", learning_rate)
        self.set("stdp_ltp_weight", stdp_ltp)
        self.set("stdp_ltd_weight", stdp_ltd)
        return self
    
    def development(self, synaptogenesis_rate: float = 0.001,
                    pruning_rate: float = 0.0001) -> 'ConfigBuilder':
        """
        Configure development parameters.
        
        Args:
            synaptogenesis_rate: Rate of synapse formation
            pruning_rate: Rate of synapse removal
            
        Returns:
            Self for method chaining
        """
        self.set("development_synaptogenesis_rate", synaptogenesis_rate)
        self.set("development_pruning_rate", pruning_rate)
        return self
    
    def neuromodulation(self, dopamine_baseline: float = 0.0) -> 'ConfigBuilder':
        """
        Configure neuromodulation parameters.
        
        Args:
            dopamine_baseline: Baseline dopamine level
            
        Returns:
            Self for method chaining
        """
        self.set("neuromod_dopamine_baseline", dopamine_baseline)
        return self
    
    def simulation(self, timestep: float = 0.001,
                   max_steps: int = 10000) -> 'ConfigBuilder':
        """
        Configure simulation parameters.
        
        Args:
            timestep: Simulation timestep in seconds
            max_steps: Maximum number of simulation steps
            
        Returns:
            Self for method chaining
        """
        self.set("simulation_timestep", timestep)
        self.set("max_simulation_steps", max_steps)
        return self
    
    def load_from_dict(self, config_dict: Dict[str, Any]) -> 'ConfigBuilder':
        """
        Load configuration from Python dictionary.
        
        Args:
            config_dict: Configuration dictionary
            
        Returns:
            Self for method chaining
        """
        for key, value in config_dict.items():
            self.set(key, value)
        return self
    
    def load_from_json(self, filepath: str) -> 'ConfigBuilder':
        """
        Load configuration from JSON file.
        
        Args:
            filepath: Path to JSON configuration file
            
        Returns:
            Self for method chaining
        """
        try:
            with open(filepath, 'r') as f:
                config_dict = json.load(f)
            self.load_from_dict(config_dict)
        except FileNotFoundError:
            raise ConfigurationError(f"Configuration file not found: {filepath}")
        except json.JSONDecodeError as e:
            raise ConfigurationError(f"Invalid JSON in configuration file: {str(e)}")
        
        return self
    
    def save_to_file(self, filepath: str) -> 'ConfigBuilder':
        """
        Save configuration to file.
        
        Args:
            filepath: Path to save configuration file
            
        Returns:
            Self for method chaining
        """
        self.config.saveToFile(filepath)
        return self
    
    def summary(self) -> str:
        """
        Get configuration summary.
        
        Returns:
            Configuration summary string
        """
        return self.config.summary()
    
    def get_config(self) -> Any:
        """
        Get the underlying NLM configuration.
        
        Returns:
            NLM configuration object
        """
        return self.config


class PerformanceStats:
    """
    Performance monitoring statistics.
    
    Collects and reports performance metrics for NLM simulations.
    """
    
    def __init__(self):
        self.reset()
    
    def reset(self):
        """Reset all performance statistics."""
        self.start_time = None
        self.end_time = None
        self.total_steps = 0
        self.total_spikes = 0
        self.firing_neurons = 0
        self.avg_firing_rate = 0.0
        self.ei_ratio = 0.0
        self.memory_usage = 0
        self.cpu_usage = 0.0
    
    def start(self):
        """Start performance monitoring."""
        import time
        self.start_time = time.time()
    
    def stop(self, brain: Any) -> 'PerformanceStats':
        """
        Stop performance monitoring and collect statistics.
        
        Args:
            brain: NLM brain object
            
        Returns:
            Self for method chaining
        """
        import time
        if self.start_time is None:
            raise PerformanceError("Performance monitoring not started")
        
        self.end_time = time.time()
        self.total_steps = brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0
        self.total_spikes = brain.getTotalSpikeCount()
        self.firing_neurons = brain.getFiringNeuronCount()
        self.avg_firing_rate = brain.getAverageFiringRate()
        self.ei_ratio = brain.getExcitationInhibitionRatio()
        
        return self
    
    def simulation_time(self) -> float:
        """
        Get total simulation time in seconds.
        
        Returns:
            Simulation time in seconds
        """
        if self.start_time is None or self.end_time is None:
            return 0.0
        return self.end_time - self.start_time
    
    def to_dict(self) -> Dict[str, Any]:
        """
        Get performance statistics as dictionary.
        
        Returns:
            Dictionary containing performance statistics
        """
        return {
            'simulation_time': self.simulation_time(),
            'total_steps': self.total_steps,
            'total_spikes': self.total_spikes,
            'firing_neurons': self.firing_neurons,
            'avg_firing_rate': self.avg_firing_rate,
            'e_i_ratio': self.ei_ratio,
            'memory_usage': self.memory_usage,
            'cpu_usage': self.cpu_usage
        }
    
    def to_string(self) -> str:
        """
        Get performance statistics as formatted string.
        
        Returns:
            Formatted performance statistics string
        """
        stats = self.to_dict()
        lines = [
            "NLM Performance Statistics",
            "=" * 30,
            f"Simulation Time: {stats['simulation_time']:.3f} s",
            f"Total Steps: {stats['total_steps']}",
            f"Total Spikes: {stats['total_spikes']}",
            f"Firing Neurons: {stats['firing_neurons']}",
            f"Average Firing Rate: {stats['avg_firing_rate']:.2f} Hz",
            f"E/I Ratio: {stats['e_i_ratio']:.2f}",
            f"Memory Usage: {stats['memory_usage']} MB",
            f"CPU Usage: {stats['cpu_usage']:.1f}%"
        ]
        return "\n".join(lines)


class SimulationProfiler:
    """
    Simulation profiler for performance analysis.
    
    Provides profiling capabilities for NLM simulations with detailed
    performance metrics and analysis.
    """
    
    def __init__(self):
        self.stats = PerformanceStats()
        self.step_times = []
        self.memory_snapshots = []
    
    def run_simulation_with_profiling(self, brain: Any, world: Any, agent: Any,
                                     num_steps: int, timestep: float = 0.1,
                                     verbose: bool = False) -> Dict[str, Any]:
        """
        Run simulation with full profiling.
        
        Args:
            brain: NLM brain object
            world: NLM world object
            agent: NLM agent object
            num_steps: Number of simulation steps
            timestep: Simulation timestep
            verbose: Enable verbose output
            
        Returns:
            Dictionary containing simulation results and performance stats
        """
        if not CORE_AVAILABLE:
            raise ImportError("Core NLM bindings required for profiling")
        
        self.stats.start()
        
        try:
            # Initialize
            brain.initialize()
            world.reset()
            agent.initialize(world)
            
            # Run simulation with timing
            for step in range(num_steps):
                step_start = time.time()
                
                world.update(timestep)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step, step * timestep)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Record step timing
                step_time = time.time() - step_start
                self.step_times.append(step_time)
                
                if verbose and step % 100 == 0:
                    print(f"Step {step}: {brain.getFiringNeuronCount()} firing, "
                          f"{self.stats.simulation_time():.2f}s elapsed")
            
            # Collect performance data
            self.stats.stop(brain)
            
            # Calculate additional metrics
            if self.step_times:
                self.stats.avg_firing_rate = brain.getAverageFiringRate()
                self.stats.ei_ratio = brain.getExcitationInhibitionRatio()
            
            return {
                'success': True,
                'steps_completed': num_steps,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'performance_stats': self.stats.to_dict(),
                'average_step_time': sum(self.step_times) / len(self.step_times) if self.step_times else 0.0,
                'min_step_time': min(self.step_times) if self.step_times else 0.0,
                'max_step_time': max(self.step_times) if self.step_times else 0.0
            }
            
        except Exception as e:
            return {
                'success': False,
                'error': str(e),
                'error_type': type(e).__name__
            }
    
    def get_performance_report(self) -> str:
        """
        Get detailed performance report.
        
        Returns:
            Formatted performance report string
        """
        if not self.stats.start_time:
            return "No performance data collected yet."
        
        report_lines = [
            "NLM Simulation Performance Report",
            "=" * 40,
            f"Total Simulation Time: {self.stats.simulation_time():.3f} s",
            f"Steps Completed: {len(self.step_times)}",
            f"Average Step Time: {sum(self.step_times)/len(self.step_times):.6f} s",
            f"Min Step Time: {min(self.step_times):.6f} s",
            f"Max Step Time: {max(self.step_times):.6f} s",
            f"Total Spikes: {self.stats.total_spikes}",
            f"Firing Neurons: {self.stats.firing_neurons}",
            f"Average Firing Rate: {self.stats.avg_firing_rate:.2f} Hz",
            f"E/I Ratio: {self.stats.ei_ratio:.2f}"
        ]
        
        return "\n".join(report_lines)


# Convenience functions for common operations
def create_simple_agent(config_dict: Optional[Dict[str, Any]] = None) -> Tuple[Any, Any, Any]:
    """
    Create a simple agent with default configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Tuple of (brain, world, agent)
    """
    if not CORE_AVAILABLE:
        raise ImportError("Core NLM bindings required to create agent")
    
    config = nlm.createDefaultConfig()
    
    if config_dict:
        builder = ConfigBuilder()
        builder.load_from_dict(config_dict)
        config = builder.get_config()
    
    # Set default values if not provided
    if not config.has("neuron_count"):
        config.set("neuron_count", 500)
    if not config.has("region_count"):
        config.set("region_count", 2)
    if not config.has("connection_probability"):
        config.set("connection_probability", 0.1)
    if not config.has("random_seed"):
        config.set("random_seed", 42)
    
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    agent = nlm.createAgentBrain(brain)
    
    return brain, world, agent


def run_learning_simulation(brain: Any, world: Any, agent: Any,
                           num_steps: int,
                           enable_reward_modulation: bool = True,
                           enable_curiosity: bool = True,
                           verbose: bool = False) -> Dict[str, Any]:
    """
    Run a learning simulation with all subsystems enabled.
    
    Args:
        brain: NLM brain object
        world: NLM world object
        agent: NLM agent object
        num_steps: Number of simulation steps
        enable_reward_modulation: Enable reward-based learning
        enable_curiosity: Enable curiosity-driven exploration
        verbose: Enable verbose output
        
    Returns:
        Dictionary containing simulation results
    """
    try:
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Enable learning subsystems
        if enable_reward_modulation:
            agent.enableRewardModulation(True)
        if enable_curiosity:
            agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        for step in range(num_steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step, step * 0.1)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if verbose and step % 100 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing, "
                      f"{agent.getCuriosityLevel():.2f} curiosity, "
                      f"{agent.getNeuromodulationLevel():.2f} neuromod")
        
        return {
            'success': True,
            'steps_completed': num_steps,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'final_prediction_error': agent.getPredictionError(),
            'developmental_stage': agent.getDevelopmentalStage()
        }
        
    except Exception as e:
        return {
            'success': False,
            'error': str(e),
            'error_type': type(e).__name__
        }


def run_simulation(brain: Any, world: Any, agent: Any,
                   num_steps: int, verbose: bool = False) -> Dict[str, Any]:
    """
    Run a complete simulation with brain, world, and agent.
    
    Args:
        brain: NLM brain object
        world: NLM world object
        agent: NLM agent object
        num_steps: Number of simulation steps
        verbose: Enable verbose output
        
    Returns:
        Dictionary containing simulation results
    """
    try:
        # Initialize
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        # Run simulation
        for step in range(num_steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step, step * 0.1)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if verbose and step % 100 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing, "
                      f"{brain.getTotalSpikeCount()} total spikes")
        
        return {
            'success': True,
            'steps_completed': num_steps,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio()
        }
        
    except Exception as e:
        return {
            'success': False,
            'error': str(e),
            'error_type': type(e).__name__
        }


# Context managers for resource management
@contextmanager
def nlm_session(brain: Any, world: Any, agent: Any, timestep: float = 0.1):
    """
    Context manager for NLM simulation sessions.
    
    Args:
        brain: NLM brain object
        world: NLM world object
        agent: NLM agent object
        timestep: Simulation timestep
        
    Yields:
        Tuple of (brain, world, agent)
    """
    try:
        brain.initialize()
        world.reset()
        agent.initialize(world)
        yield brain, world, agent
    finally:
        # Cleanup
        pass


@contextmanager
def config_session(config_file: str = None, config_dict: Optional[Dict[str, Any]] = None):
    """
    Context manager for configuration sessions.
    
    Args:
        config_file: Path to configuration file
        config_dict: Configuration dictionary
        
    Yields:
        ConfigBuilder instance
    """
    builder = ConfigBuilder()
    try:
        if config_file:
            builder.load_from_file(config_file)
        if config_dict:
            builder.load_from_dict(config_dict)
        yield builder
    finally:
        # Cleanup
        pass


# Version and feature information
__version__ = "1.0.0"
__description__ = "Enhanced NLM Python API with high-level convenience methods"

# Check available features
_features_available = {
    'core_bindings': CORE_AVAILABLE,
    'config_builder': True,
    'performance_monitoring': True,
    'learning_utilities': True,
    'context_managers': True,
    'error_handling': True
}

# Create module-level functions for backward compatibility
__all__ = [
    'ConfigBuilder',
    'PerformanceStats',
    'SimulationProfiler',
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'ValidationError',
    'PerformanceError',
    'create_simple_agent',
    'run_simulation',
    'run_learning_simulation',
    'nlm_session',
    'config_session',
    'features_available'
]

# Print initialization message
if CORE_AVAILABLE:
    print(f"Enhanced NLM Python API v{__version__} loaded successfully")
    print(f"Available features: {sum(_features_available.values())}/{len(_features_available)}")
    
    for feature, available in _features_available.items():
        if available:
            print(f"  ✓ {feature.replace('_', ' ').title()}")
else:
    print("Warning: Enhanced NLM API loaded but core bindings are not available")
    print("This module provides enhanced interfaces but requires the core NLM module to function.")
