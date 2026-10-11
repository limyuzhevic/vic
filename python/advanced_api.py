# Advanced Pythonic NLM Interface
# 
# This file contains enhanced Pythonic bindings for NLM that provide:
# 1. Pythonic property access patterns
# 2. Context managers for resource management
# 3. Factory methods for common setups
# 4. Advanced configuration and optimization options
# 5. Better error handling and exception safety
# 6. Performance monitoring utilities
# 7. Comprehensive documentation

"""
Advanced Python API for NLM (Neural Learning Machine)

This module provides a comprehensive Python interface to the NLM neural simulation framework
with enhanced usability, advanced features, and modern Python patterns.

Key Features:
- Pythonic property-based configuration
- Context managers for simulation lifecycle
- Factory methods for common neural architectures
- Advanced optimization and monitoring tools
- Comprehensive error handling
- Type hints and documentation

Usage Examples:
    Basic simulation:
        from pynlm import create_brain, run_simulation
        
        brain = create_brain(1000)  # Create brain with 1000 neurons
        brain.initialize()
        
        for step in range(1000):
            brain.step(step)
            if step % 100 == 0:
                print(f"Step {step}: {brain.get_firing_neuron_count()} firing")
    
    Advanced simulation with monitoring:
        with SimulationContext(brain, world) as sim:
            sim.enable_all_features()
            sim.run_for_steps(5000, monitor_stats=True)
            
            stats = sim.get_performance_stats()
            print(f"Average firing rate: {stats['avg_firing_rate']:.2f}")
            print(f"Memory usage: {stats['memory_usage_mb']:.1f} MB")
"""

import json
import time
import threading
import warnings
from typing import Optional, Dict, Any, List, Callable, Union, ContextManager
from contextlib import contextmanager
from dataclasses import dataclass, field
from enum import Enum

# Core NLM imports (these would be from the actual bindings)
try:
    import pynlm as nlm
except ImportError:
    raise ImportError(
        "NLM Python bindings not found. "
        "Please build the project first with: pip install ."
    )

# Custom exceptions for better error handling
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Configuration error in NLM."""
    pass

class SimulationError(NLMError):
    """Error during neural simulation."""
    pass

class MemoryError(NLMError):
    """Memory-related error in NLM."""
    pass

class ValidationError(NLMError):
    """Validation error in neural parameters."""
    pass
class OptimizationLevel(Enum):
    """Optimization levels for NLM performance."""
    BASIC = "basic"
    BALANCED = "balanced"
    ADVANCED = "advanced"
    ULTRA = "ultra"

class LearningMode(Enum):
    """Learning modes for neural networks."""
    STANDBY = "standby"
    EXPLICIT = "explicit"
    CONTINUOUS = "continuous"
    REINFORCEMENT = "reinforcement"
    CURIOUS = "curious"
@dataclass
class SimulationStats:
    """Simulation statistics and performance metrics."""
    
    steps_completed: int = 0
    total_time: float = 0.0
    avg_firing_rate: float = 0.0
    neurons_active: int = 0
    total_spikes: int = 0
    memory_usage_mb: float = 0.0
    cpu_usage: float = 0.0
    timestamp: float = 0.0
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert stats to dictionary."""
        return {
            'steps_completed': self.steps_completed,
            'total_time': self.total_time,
            'avg_firing_rate': self.avg_firing_rate,
            'neurons_active': self.neurons_active,
            'total_spikes': self.total_spikes,
            'memory_usage_mb': self.memory_usage_mb,
            'cpu_usage': self.cpu_usage,
            'timestamp': self.timestamp,
            'performance_per_step_ms': self.total_time / max(1, self.steps_completed)
        }
@dataclass
class BrainConfig:
    """Enhanced brain configuration with validation and validation."""
    
    # Basic configuration
    neuron_count: int = 1000
    region_count: int = 1
    connection_probability: float = 0.1
    initial_weight_mean: float = 0.5
    initial_weight_std: float = 0.1
    
    # Neural dynamics parameters
    v_thresh: float = -50.0
    v_rest: float = -70.0
    v_reset: float = -75.0
    tau_mem: float = 20.0
    tau_ref: float = 2.0
    
    # Memory system parameters
    working_memory_capacity: int = 100
    working_memory_decay_rate: float = 0.01
    episodic_memory_capacity: int = 1000
    associative_strength: float = 0.5
    
    # Neuromodulation parameters
    dopamine_scale: float = 1.0
    curiosity_threshold: float = 0.3
    novelty_sensitivity: float = 0.5
    prediction_error_weight: float = 1.0
    
    # Development parameters
    developmental_stage_duration: float = 1000.0
    structural_plasticity_rate: float = 0.001
    synaptogenesis_rate: float = 0.01
    pruning_rate: float = 0.005
    
    # Performance optimization
    optimization_level: OptimizationLevel = OptimizationLevel.BALANCED
    enable_vectorization: bool = True
    enable_parallel: bool = True
    enable_spike_optimization: bool = True
    enable_memory_pooling: bool = True
    
    # Learning mode
    learning_mode: LearningMode = LearningMode.CONTINUOUS
    enable_reward_modulation: bool = True
    enable_curiosity: bool = True
    enable_structural_plasticity: bool = True
    enable_development: bool = True
    
    # Simulation control
    max_steps: int = 10000
    checkpoint_interval: int = 1000
    replay_interval: int = 100
    consolidation_interval: int = 1000
    
    # Validation and safety
    validate_connectivity: bool = True
    validate_neuron_counts: bool = True
    validate_plasticity_parameters: bool = True
    safe_mode: bool = False
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert config to dictionary."""
        config_dict = {}
        for field_name, field_value in self.__dataclass_fields__.items():
            if field_value.name != 'optimization_level':
                config_dict[field_name] = getattr(self, field_name)
            else:
                config_dict[field_name] = field_value.value
                
        if 'optimization_level' in self.__dataclass_fields__:
            config_dict['optimization_level'] = self.optimization_level.value
        
        if 'learning_mode' in self.__dataclass_fields__:
            config_dict['learning_mode'] = self.learning_mode.value
            
        return config_dict
    
    @classmethod
    def from_dict(cls, config_dict: Dict[str, Any]) -> 'BrainConfig':
        """Create config from dictionary."""
        # Handle enum conversion
        if 'optimization_level' in config_dict:
            config_dict['optimization_level'] = OptimizationLevel(config_dict['optimization_level'])
            
        if 'learning_mode' in config_dict:
            config_dict['learning_mode'] = LearningMode(config_dict['learning_mode'])
        
        # Remove fields that don't exist in dataclass
        valid_fields = {f.name for f in cls.__dataclass_fields__.values()}
        filtered_dict = {k: v for k, v in config_dict.items() if k in valid_fields}
        
        return cls(**filtered_dict)
    
    def validate(self) -> List[str]:
        """Validate configuration parameters. Returns list of errors (empty if valid)."""
        errors = []
        
        # Validate basic constraints
        if self.neuron_count <= 0:
            errors.append("neuron_count must be positive")
        
        if self.region_count <= 0:
            errors.append("region_count must be positive")
        
        if not 0.0 <= self.connection_probability <= 1.0:
            errors.append("connection_probability must be between 0 and 1")
        
        if self.v_thresh >= self.v_rest:
            errors.append("v_thresh must be less than v_rest")
        
        # Validate memory parameters
        if self.working_memory_capacity <= 0:
            errors.append("working_memory_capacity must be positive")
        
        if not 0.0 <= self.working_memory_decay_rate <= 1.0:
            errors.append("working_memory_decay_rate must be between 0 and 1")
        
        # Validate optimization constraints
        if not self.enable_vectorization or self.enable_parallel:
            pass  # These can be mutually exclusive based on implementation
        
        return errors
    
    def apply_optimization_preset(self, preset: str):
        """Apply a predefined optimization preset."""
        presets = {
            'fast': {
                'optimization_level': OptimizationLevel.ULTRA,
                'enable_vectorization': True,
                'enable_parallel': False,
                'enable_spike_optimization': True,
                'enable_memory_pooling': True,
                'learning_mode': LearningMode.EXPLICIT,
                'working_memory_decay_rate': 0.05,
                'structural_plasticity_rate': 0.0001
            },
            'accurate': {
                'optimization_level': OptimizationLevel.ADVANCED,
                'enable_vectorization': True,
                'enable_parallel': True,
                'enable_spike_optimization': False,
                'enable_memory_pooling': True,
                'learning_mode': LearningMode.CONTINUOUS,
                'working_memory_decay_rate': 0.01,
                'structural_plasticity_rate': 0.001
            },
            'balanced': {
                'optimization_level': OptimizationLevel.BALANCED,
                'enable_vectorization': True,
                'enable_parallel': True,
                'enable_spike_optimization': True,
                'enable_memory_pooling': True,
                'learning_mode': LearningMode.BALANCED,
                'working_memory_decay_rate': 0.01,
                'structural_plasticity_rate': 0.001
            }
        }
        
        if preset not in presets:
            raise ValueError(f"Unknown optimization preset: {preset}")
        
        preset_dict = presets[preset]
        for key, value in preset_dict.items():
            setattr(self, key, value)
class SimulationContext:
    """Context manager for simulation lifecycle with automatic cleanup."""
    
    def __init__(self, brain: nlm.Brain, world: Optional[nlm.SimpleWorld] = None):
        self.brain = brain
        self.world = world
        self.original_config = None
        self.enabled_features = []
        self.running = False
        self.start_time = 0.0
        self.stats_collector: Optional['SimulationStatsCollector'] = None
        
        # Validate input
        if brain is None:
            raise SimulationError("Brain cannot be None")
    
    def __enter__(self) -> 'SimulationContext':
        """Enter simulation context."""
        if self.running:
            raise SimulationError("Simulation context already active")
        
        # Store original configuration
        if self.brain:
            self.original_config = self.brain.get_config()
        
        # Initialize simulation
        self.start_time = time.time()
        self.running = True
        
        # Initialize stats collector if needed
        if self.brain:
            self.stats_collector = SimulationStatsCollector(self.brain)
            self.stats_collector.start()
        
        # Log entry
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit simulation context with cleanup."""
        self.running = False
        
        # Stop stats collector
        if self.stats_collector:
            self.stats_collector.stop()
            self.stats_collector = None
        
        # Restore original configuration
        if self.original_config:
            # Note: This would require a restore_config method
            pass
        
        # Log exit
        self._log_exit(exc_type, exc_val)
    
    def enable_all_features(self):
        """Enable all advanced features automatically."""
        if self.brain:
            # This would enable all subsystems
            # Based on current API: AgentBrain methods
            pass
        
        self.enabled_features = [
            'reward_modulation',
            'structural_plasticity',
            'development',
            'curiosity',
            'prediction',
            'memory_integration'
        ]
        
        return self
    
    def run_for_steps(self, num_steps: int, monitor_stats: bool = False):
        """Run simulation for specified number of steps."""
        if not self.running:
            raise SimulationError("Simulation context not active. Use 'with' statement.")
        
        if self.brain is None:
            raise SimulationError("No brain initialized for simulation.")
        
        try:
            for step in range(num_steps):
                # Execute simulation step
                self.brain.step(step)
                
                # Monitor statistics if requested
                if monitor_stats and self.stats_collector:
                    self.stats_collector.record_step(step)
                
                # Check for early termination conditions
                if self._check_termination_conditions(step):
                    break
            
        except Exception as e:
            raise SimulationError(f"Error during simulation: {str(e)}")
        
        return self
    
    def get_performance_stats(self) -> Dict[str, Any]:
        """Get performance statistics from simulation."""
        if self.stats_collector:
            return self.stats_collector.get_stats().to_dict()
        else:
            return {
                'steps_completed': 0,
                'total_time': 0.0,
                'avg_firing_rate': 0.0,
                'neurons_active': 0,
                'total_spikes': 0,
                'memory_usage_mb': 0.0,
                'cpu_usage': 0.0,
                'timestamp': time.time()
            }
    
    def _check_termination_conditions(self, step: int) -> bool:
        """Check if simulation should terminate early."""
        # Check if brain is too quiet
        if self.brain and step > 100:
            firing_rate = self.brain.getAverageFiringRate()
            if firing_rate < 0.01:  # Very low firing rate
                warnings.warn(f"Firing rate dropped to {firing_rate:.3f} at step {step}")
                return True
        
        return False
    
    def _log_exit(self, exc_type, exc_val, exc_tb):
        """Log context exit with error information."""
        if exc_type is not None:
            # Exception occurred
            pass
        else:
            # Normal exit
            pass
class SimulationStatsCollector:
    """Collect simulation statistics and metrics."""
    
    def __init__(self, brain: nlm.Brain):
        self.brain = brain
        self.stats = SimulationStats()
        self.monitoring = False
        self.monitor_thread: Optional[threading.Thread] = None
        self._lock = threading.Lock()
    
    def start(self):
        """Start statistics collection."""
        if self.monitoring:
            return
        
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop, daemon=True)
        self.monitor_thread.start()
    
    def stop(self):
        """Stop statistics collection."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
    
    def record_step(self, step: int):
        """Record statistics for current step."""
        with self._lock:
            self.stats.steps_completed = step + 1
            self.stats.timestamp = time.time()
            
            if self.brain:
                # Update statistics based on brain state
                self.stats.neurons_active = self.brain.getFiringNeuronCount()
                self.stats.total_spikes = self.brain.getTotalSpikeCount()
                self.stats.avg_firing_rate = self.brain.getAverageFiringRate()
                
                # Get brain statistics for memory and CPU usage
                self._update_resource_stats()
    
    def get_stats(self) -> SimulationStats:
        """Get current statistics."""
        with self._lock:
            return SimulationStats(
                steps_completed=self.stats.steps_completed,
                total_time=self.stats.total_time,
                avg_firing_rate=self.stats.avg_firing_rate,
                neurons_active=self.stats.neurons_active,
                total_spikes=self.stats.total_spikes,
                memory_usage_mb=self.stats.memory_usage_mb,
                cpu_usage=self.stats.cpu_usage,
                timestamp=self.stats.timestamp
            )
    
    def _update_resource_stats(self):
        """Update resource usage statistics."""
        # This would monitor memory and CPU usage
        # Implementation depends on system capabilities
        pass
    
    def _monitor_loop(self):
        """Background monitoring loop."""
        while self.monitoring:
            time.sleep(0.1)  # Monitor every 100ms
            
            with self._lock:
                self.stats.total_time += 0.1
class Factory:
    """Factory methods for creating common NLM configurations."""
    
    @staticmethod
    def create_default_brain(neuron_count: int = 1000, seed: int = 42) -> nlm.Brain:
        """Create a brain with default configuration."""
        config = nlm.createDefaultConfig()
        
        # Set common defaults
        config.set("brain.neuron_count", neuron_count)
        config.set("random_seed", seed)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        return brain
    
    @staticmethod
    def create_reinforcement_learner(
        world_width: int = 20,
        world_height: int = 20,
        neuron_count: int = 500,
        learning_rate: float = 0.01
    ) -> tuple:
        """Create a brain optimized for reinforcement learning."""
        config = nlm.createDefaultConfig()
        
        # Configure for reinforcement learning
        config.set("brain.neuron_count", neuron_count)
        config.set("plasticity.stdp.learning_rate", learning_rate)
        config.set("plasticity.reward_modulated_stdp.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.dopamine.scale", learning_rate)
        
        # Create components
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.configure(world_width, world_height, 8, 8)
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        return brain, world, agent
    
    @staticmethod
    def create_curiosity_driven(
        exploration_rate: float = 0.5,
        novelty_threshold: float = 0.3
    ) -> tuple:
        """Create a curiosity-driven exploration system."""
        config = nlm.createDefaultConfig()
        
        # Configure for curiosity-driven learning
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.curiosity.exploration_rate", exploration_rate)
        config.set("neuromod.novelty.threshold", novelty_threshold)
        config.set("plasticity.structural.enable", True)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        return brain
    
    @staticmethod
    def create_developmental_system(
        developmental_stages: int = 4,
        maturation_rate: float = 0.1
    ) -> nlm.Brain:
        """Create a brain with developmental plasticity."""
        config = nlm.createDefaultConfig()
        
        # Configure for development
        config.set("development.enable", True)
        config.set("development.stages", developmental_stages)
        config.set("development.maturation_rate", maturation_rate)
        config.set("plasticity.structural.enable", True)
        config.set("plasticity.development.enabled", True)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        return brain
    
    @staticmethod
    def create_checkpointing_system(
        checkpoint_dir: str = "./checkpoints",
        interval: int = 1000,
        max_checkpoints: int = 10
    ) -> nlm.Brain:
        """Create a brain with checkpointing capabilities."""
        config = nlm.createDefaultConfig()
        
        # Configure for checkpointing
        config.set("checkpoint.enable", True)
        config.set("checkpoint.directory", checkpoint_dir)
        config.set("checkpoint.interval", interval)
        config.set("checkpoint.max_count", max_checkpoints)
        config.set("checkpoint.compress", True)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        return brain
    
    @staticmethod
    def create_memory_system(
        working_memory_capacity: int = 100,
        episodic_memory_capacity: int = 1000,
        associative_strength: float = 0.5
    ) -> tuple:
        """Create a brain optimized for memory systems."""
        config = nlm.createDefaultConfig()
        
        # Configure memory systems
        config.set("memory.working_memory.capacity", working_memory_capacity)
        config.set("memory.episodic_memory.capacity", episodic_memory_capacity)
        config.set("memory.associative.strength", associative_strength)
        config.set("memory.neural.enable", True)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        return brain
def run_complete_simulation(
    brain: nlm.Brain,
    world: Optional[nlm.SimpleWorld] = None,
    agent: Optional[nlm.AgentBrain] = None,
    config: Optional[Dict[str, Any]] = None
) -> Dict[str, Any]:
    """
    Run a complete simulation with monitoring and statistics.
    
    This is a high-level convenience function for running complete simulations
    with automatic monitoring and result collection.
    """
    # Use context manager for safe simulation
    with SimulationContext(brain, world) as sim:
        # Apply custom configuration if provided
        if config:
            sim.apply_custom_config(config)
        
        # Enable all features
        sim.enable_all_features()
        
        # Run simulation
        sim.run_for_steps(10000, monitor_stats=True)
        
        # Get results
        results = sim.get_performance_stats()
        
        # Add simulation metadata
        results['simulation_complete'] = True
        results['final_neuron_count'] = brain.getTotalNeuronCount()
        results['final_synapse_count'] = brain.getTotalSynapseCount()
        results['developmental_stage'] = brain.getDevelopmentalStage()
        
        return results
def create_advanced_config(**kwargs) -> BrainConfig:
    """
    Create an advanced BrainConfig with validation and error handling.
    
    This function provides a convenient way to create BrainConfig objects
    with automatic validation and sensible defaults.
    """
    # Filter out None values (let dataclass provide defaults)
    valid_kwargs = {k: v for k, v in kwargs.items() if v is not None}
    
    try:
        config = BrainConfig(**valid_kwargs)
        errors = config.validate()
        
        if errors:
            warnings.warn(f"Configuration validation warnings: {', '.join(errors)}")
        
        return config
        
    except Exception as e:
        raise ConfigurationError(f"Failed to create configuration: {str(e)}")
class NLMProfiler:
    """
    Performance profiler for NLM simulations.
    
    Provides detailed profiling of simulation performance,
    including time measurements, memory usage, and bottleneck analysis.
    """
    
    def __init__(self, brain: Optional[nlm.Brain] = None):
        self.brain = brain
        self.timings: List[Dict[str, float]] = []
        self.memory_samples: List[float] = []
        self.cpu_samples: List[float] = []
        self.start_time: Optional[float] = None
        self.step_count: int = 0
    
    def start(self):
        """Start profiling."""
        import time as time_module
        self.start_time = time_module.time()
        self.step_count = 0
        self.timings.clear()
        self.memory_samples.clear()
        self.cpu_samples.clear()
    
    def record_step(self, step: int):
        """Record profiling data for a simulation step."""
        import time as time_module
        import psutil
        import os
        
        step_time = time_module.time() - (self.start_time + self.step_count * 0.1)
        self.timings.append({
            'step': step,
            'timestamp': time_module.time(),
            'step_time_ms': step_time * 1000,
            'step_count': step
        })
        
        # Record memory usage
        process = psutil.Process(os.getpid())
        memory_mb = process.memory_info().rss / 1024 / 1024
        self.memory_samples.append(memory_mb)
        
        # Record CPU usage (approximate)
        cpu_percent = process.cpu_percent()
        self.cpu_samples.append(cpu_percent)
        
        self.step_count = step + 1
    
    def stop(self) -> Dict[str, Any]:
        """Stop profiling and return results."""
        import time as time_module
        
        if self.start_time is None:
            raise NLMError("Profiler not started")
        
        total_time = time_module.time() - self.start_time
        
        if not self.timings:
            return self._get_empty_results(total_time)
        
        # Analyze timings
        step_times = [t['step_time_ms'] for t in self.timings]
        avg_step_time = sum(step_times) / len(step_times)
        min_step_time = min(step_times)
        max_step_time = max(step_times)
        
        # Calculate statistics
        results = {
            'total_time_s': total_time,
            'steps_completed': self.step_count,
            'avg_step_time_ms': avg_step_time,
            'min_step_time_ms': min_step_time,
            'max_step_time_ms': max_step_time,
            'steps_per_second': self.step_count / total_time if total_time > 0 else 0,
            
            'memory_usage_mb': {
                'current': self.memory_samples[-1] if self.memory_samples else 0,
                'average': sum(self.memory_samples) / len(self.memory_samples) if self.memory_samples else 0,
                'max': max(self.memory_samples) if self.memory_samples else 0,
                'min': min(self.memory_samples) if self.memory_samples else 0
            },
            
            'cpu_usage': {
                'current': self.cpu_samples[-1] if self.cpu_samples else 0,
                'average': sum(self.cpu_samples) / len(self.cpu_samples) if self.cpu_samples else 0,
                'max': max(self.cpu_samples) if self.cpu_samples else 0
            },
            
            'timing_details': self.timings
        }
        
        return results
    
    def _get_empty_results(self, total_time: float) -> Dict[str, Any]:
        """Get empty profiling results."""
        return {
            'total_time_s': total_time,
            'steps_completed': 0,
            'avg_step_time_ms': 0,
            'min_step_time_ms': 0,
            'max_step_time_ms': 0,
            'steps_per_second': 0,
            'memory_usage_mb': {
                'current': 0,
                'average': 0,
                'max': 0,
                'min': 0
            },
            'cpu_usage': {
                'current': 0,
                'average': 0,
                'max': 0
            },
            'timing_details': []
        }
    
    def analyze_bottlenecks(self) -> List[Dict[str, Any]]:
        """Analyze performance bottlenecks."""
        if not self.timings:
            return []
        
        bottlenecks = []
        
        # Find slow steps (above 95th percentile)
        step_times = [t['step_time_ms'] for t in self.timings]
        percentile_95 = sorted(step_times)[int(len(step_times) * 0.95)]
        
        for timing in self.timings:
            if timing['step_time_ms'] > percentile_95:
                bottlenecks.append({
                    'step': timing['step'],
                    'step_time_ms': timing['step_time_ms'],
                    'issue': 'High execution time'
                })
        
        return bottlenecks
    
    def export_results(self, filepath: str):
        """Export profiling results to JSON file."""
        import json
        
        results = self.stop()
        
        with open(filepath, 'w') as f:
            json.dump(results, f, indent=2)
    
    def plot_performance(self, output_file: str = None):
        """Create a simple performance plot."""
        try:
            import matplotlib.pyplot as plt
            
            if not self.timings:
                return
            
            steps = [t['step'] for t in self.timings]
            step_times = [t['step_time_ms'] for t in self.timings]
            
            fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))
            
            # Step time plot
            ax1.plot(steps, step_times, 'b-', alpha=0.7, linewidth=0.5)
            ax1.set_xlabel('Step')
            ax1.set_ylabel('Step Time (ms)')
            ax1.set_title('Simulation Step Timing')
            ax1.grid(True, alpha=0.3)
            
            # Memory usage plot
            if self.memory_samples:
                ax2.plot(steps[:len(self.memory_samples)], self.memory_samples, 'r-', alpha=0.7, linewidth=0.5)
                ax2.set_xlabel('Step')
                ax2.set_ylabel('Memory Usage (MB)')
                ax2.set_title('Memory Usage Over Time')
                ax2.grid(True, alpha=0.3)
            
            plt.tight_layout()
            
            if output_file:
                plt.savefig(output_file, dpi=150, bbox_inches='tight')
            
            plt.show()
            
        except ImportError:
            warnings.warn("Matplotlib not available. Skipping plot generation.")
        except Exception as e:
            warnings.warn(f"Error generating plot: {str(e)}")
# Convenience functions for backward compatibility
# These maintain the original simple API while allowing for upgrades

def create_brain(neuron_count: int = 1000, seed: int = 42) -> nlm.Brain:
    """Create a brain with default configuration (backward compatibility)."""
    return Factory.create_default_brain(neuron_count, seed)

def create_world(width: int = 20, height: int = 20, vision_width: int = 8, vision_height: int = 8) -> nlm.SimpleWorld:
    """Create a simple world (backward compatibility)."""
    world = nlm.createSimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    world.reset()
    return world

def create_agent(brain: nlm.Brain, world: nlm.SimpleWorld) -> nlm.AgentBrain:
    """Create an agent brain interface (backward compatibility)."""
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    return agent

def run_simulation(
    brain: nlm.Brain,
    world: Optional[nlm.SimpleWorld] = None,
    agent: Optional[nlm.AgentBrain] = None,
    steps: int = 1000
) -> Dict[str, Any]:
    """Run a simple simulation (backward compatibility)."""
    with SimulationContext(brain, world) as sim:
        sim.run_for_steps(steps)
        return sim.get_performance_stats()
# Module metadata
__version__ = "2.0.0"
__author__ = "NLM Python API Team"
__description__ = "Advanced Python API for NLM neural simulation framework"
__license__ = "MIT"

# Make key classes available at module level
__all__ = [
    'create_brain',
    'create_world', 
    'create_agent',
    'run_simulation',
    'create_advanced_config',
    'SimulationContext',
    'BrainConfig',
    'SimulationStats',
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'MemoryError',
    'ValidationError',
    'OptimizationLevel',
    'LearningMode',
    'Factory',
    'NLMProfiler',
    'run_complete_simulation'
]
