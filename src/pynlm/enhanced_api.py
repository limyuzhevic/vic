"""
Enhanced Python API for NLM (Neural Learning Machine)

This module provides improved Python bindings for the NLM neural simulation framework,
building upon the existing pynlm library with enhanced usability, type safety,
and advanced features for both beginners and power users.
"""

import json
import time
import threading
from typing import Optional, Dict, List, Any, Union, Callable
from enum import Enum
from dataclasses import dataclass
from contextlib import contextmanager

# Import core NLM modules
try:
    import pynlm
    _PYNLMB_AVAILABLE = True
except ImportError:
    _PYNLMB_AVAILABLE = False
    raise ImportError(
        "pynlm module is required. Install with: pip install pynlm\n"
        "Or build from source: pip install -e ."
    )

# Import enhanced modules
try:
    from .enhanced_agent import (
        EnhancedAgentBrain,
        EnhancedSimpleWorld,
        EnhancedSensoryPercept,
        AgentBehavior,
        LearningMode,
        ActionSelector,
        create_enhanced_agent,
        create_enhanced_world,
    )
    _ENHANCED_MODULES_AVAILABLE = True
except ImportError:
    _ENHANCED_MODULES_AVAILABLE = False

try:
    from .enhanced_visualization import (
        NeuralNetworkVisualizer,
        RegionVisualizer,
        NetworkGraphVisualizer,
        ActivityMonitor,
        ActivityData,
        NeuronVisualizationData,
        ConnectionVisualizationData,
        VisualizationMode,
        DebugLevel,
        SimulationEvent,
        create_neural_network_visualizer,
        create_activity_monitor,
    )
    _VISUALIZATION_MODULES_AVAILABLE = True
except ImportError:
    _VISUALIZATION_MODULES_AVAILABLE = False

# === Enhanced Error Classes ===

class NLMBrainError(Exception):
    """Base exception for all NLM brain-related errors."""
    pass

class NLMConfigurationError(NLMBrainError):
    """Error in brain configuration."""
    pass

class NLMInitializationError(NLMBrainError):
    """Error during brain initialization."""
    pass

class NLMSimulationError(NLMBrainError):
    """Error during simulation."""
    pass

class NLMPersistenceError(NLMBrainError):
    """Error in brain persistence operations."""
    pass

class NLMValidationError(NLMBrainError):
    """Error in brain validation operations."""
    pass

# === Enhanced Enums and Dataclasses ===

class SimulationMode(Enum):
    """Different simulation modes."""
    CONTINUOUS = "continuous"
    BATCH = "batch"
    INTERACTIVE = "interactive"
    DEBUG = "debug"

@dataclass
class SimulationEpisode:
    """A single simulation episode with metrics."""
    episode_id: int
    start_step: int
    end_step: int
    total_reward: float
    average_firing_rate: float
    memory_episodes_stored: float
    steps_per_second: float
    success: bool
    metrics: Dict[str, Any]

@dataclass
class SimulationEpisode:
    """A single simulation episode with metrics."""
    episode_id: int
    start_step: int
    end_step: int
    total_reward: float
    average_firing_rate: float
    memory_episodes_stored: float
    steps_per_second: float
    success: bool
    metrics: Dict[str, Any]

class ConfigurationTemplate(Enum):
    """Predefined configuration templates."""
    MINIMAL = "minimal"
    STANDARD = "standard"
    HIGH_PERFORMANCE = "high_performance"
    RESEARCH = "research"
    REAL_TIME = "real_time"
    EDUCATIONAL = "educational"

# === Enhanced Configuration Manager ===

class EnhancedBrainConfig:
    """
    Enhanced configuration manager for NLM brains with advanced features.
    
    This class provides a more intuitive and powerful interface for configuring
    NLM brains compared to the basic Config class from pynlm.
    """
    
    def __init__(self, template: ConfigurationTemplate = ConfigurationTemplate.STANDARD):
        """
        Initialize configuration with a template.
        
        Args:
            template: Predefined configuration template
        """
        self._config = pynlm.createDefaultConfig()
        self._template = template
        self._apply_template(template)
    
    def _apply_template(self, template: ConfigurationTemplate):
        """Apply template configuration."""
        templates = {
            ConfigurationTemplate.MINIMAL: {
                "brain.neuron_count": 100,
                "brain.connection_probability": 0.05,
                "plasticity.stdp.enable": True,
                "neuromod.dopamine.scale": 1.0,
            },
            ConfigurationTemplate.STANDARD: {
                "brain.neuron_count": 1000,
                "brain.connection_probability": 0.1,
                "brain.initial_weight_mean": 0.5,
                "brain.initial_weight_std": 0.1,
                "plasticity.stdp.learning_rate": 0.001,
                "plasticity.hebbian.enable": True,
                "neuromod.dopamine.scale": 1.0,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
            },
            ConfigurationTemplate.HIGH_PERFORMANCE: {
                "brain.neuron_count": 5000,
                "brain.connection_probability": 0.05,
                "brain.initial_weight_mean": 0.5,
                "brain.initial_weight_std": 0.1,
                "plasticity.stdp.learning_rate": 0.01,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.dopamine.scale": 1.5,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.prediction_error.enable": True,
                "performance.optimizations": True,
            },
            ConfigurationTemplate.RESEARCH: {
                "brain.neuron_count": 10000,
                "brain.connection_probability": 0.15,
                "brain.initial_weight_mean": 0.6,
                "brain.initial_weight_std": 0.2,
                "plasticity.stdp.learning_rate": 0.005,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.dopamine.scale": 2.0,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.prediction_error.enable": True,
                "performance.optimizations": True,
                "performance.multithreading": True,
                "performance.simd": True,
                "performance.memory_pool": True,
            },
            ConfigurationTemplate.REAL_TIME: {
                "brain.neuron_count": 2000,
                "brain.connection_probability": 0.08,
                "brain.initial_weight_mean": 0.5,
                "brain.initial_weight_std": 0.1,
                "plasticity.stdp.learning_rate": 0.002,
                "plasticity.hebbian.enable": True,
                "neuromod.dopamine.scale": 1.0,
                "neuromod.curiosity.enable": False,  # Faster, less exploration
                "neuromod.novelty.enable": False,
                "performance.optimizations": True,
                "performance.event_driven": True,
                "performance.sparse_connectivity": True,
            },
            ConfigurationTemplate.EDUCATIONAL: {
                "brain.neuron_count": 500,
                "brain.connection_probability": 0.12,
                "brain.initial_weight_mean": 0.5,
                "brain.initial_weight_std": 0.1,
                "plasticity.stdp.learning_rate": 0.001,
                "plasticity.hebbian.enable": True,
                "neuromod.dopamine.scale": 1.0,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "performance.optimizations": False,  # Slower but simpler
                "performance.event_driven": False,
            }
        }
        
        for key, value in templates[template].items():
            self._config.set(key, value)
    
    def set_neuron_count(self, count: int, **kwargs):
        """Set number of neurons with intelligent defaults."""
        if count < 10:
            raise NLMConfigurationError("Neuron count must be at least 10")
        if count > 100000:
            raise NLMConfigurationError("Neuron count cannot exceed 100000 for stability")
        
        self._config.set("brain.neuron_count", count)
        
        # Automatically adjust related parameters
        if count > 5000:
            self._config.set("brain.connection_probability", 0.03)
            self._config.set("plasticity.stdp.learning_rate", 0.005)
        elif count > 1000:
            self._config.set("brain.connection_probability", 0.05)
            self._config.set("plasticity.stdp.learning_rate", 0.01)
        else:
            self._config.set("brain.connection_probability", 0.1)
            self._config.set("plasticity.stdp.learning_rate", 0.02)
        
        return self
    
    def set_development_mode(self, enabled: bool = True, stage: str = "normal"):
        """Configure development settings."""
        if enabled:
            self._config.set("development.enabled", True)
            self._config.set("development.initial_stage", stage)
            self._config.set("development.plasticity_rate", 1.0)
            self._config.set("development.structural_plasticity", True)
        else:
            self._config.set("development.enabled", False)
        
        return self
    
    def set_optimization_level(self, level: str = "auto"):
        """
        Set optimization level for performance.
        
        Args:
            level: "minimal", "balanced", "maximum", or "auto"
        """
        optimizations = {
            "minimal": {
                "performance.optimizations": False,
                "performance.event_driven": False,
                "performance.multithreading": False,
                "performance.simd": False,
                "performance.sparse_connectivity": False,
                "performance.memory_pool": False,
            },
            "balanced": {
                "performance.optimizations": True,
                "performance.event_driven": True,
                "performance.multithreading": True,
                "performance.simd": False,
                "performance.sparse_connectivity": True,
                "performance.memory_pool": True,
            },
            "maximum": {
                "performance.optimizations": True,
                "performance.event_driven": True,
                "performance.multithreading": True,
                "performance.simd": True,
                "performance.sparse_connectivity": True,
                "performance.memory_pool": True,
                "performance.parallel_processing": True,
                "performance.vectorization": True,
            },
            "auto": {
                "performance.optimizations": True,
                "performance.event_driven": True,
                "performance.multithreading": True,
                "performance.simd": True,
                "performance.sparse_connectivity": True,
                "performance.memory_pool": True,
            }
        }
        
        for key, value in optimizations[level].items():
            self._config.set(key, value)
        
        return self
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value with validation."""
        try:
            return self._config.getOr(
                key,
                default if default is not None else self._get_default_value(key)
            )
        except:
            return default
    
    def _get_default_value(self, key: str) -> Any:
        """Get default value for configuration key."""
        defaults = {
            "brain.neuron_count": 1000,
            "brain.connection_probability": 0.1,
            "brain.initial_weight_mean": 0.5,
            "brain.initial_weight_std": 0.1,
            "brain.v_thresh": -50.0,
            "brain.v_rest": -70.0,
            "brain.tau_mem": 20.0,
            "plasticity.stdp.learning_rate": 0.001,
            "plasticity.stdp.tau_plus": 20.0,
            "plasticity.stdp.tau_minus": 20.0,
            "neuromod.dopamine.scale": 1.0,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
        }
        return defaults.get(key)
    
    def validate(self) -> List[str]:
        """
        Validate configuration and return list of errors if any.
        
        Returns:
            List of validation error messages, empty if valid
        """
        errors = []
        
        neuron_count = self._config.getOr("brain.neuron_count", 1000)
        if neuron_count < 10:
            errors.append("Neuron count must be at least 10")
        if neuron_count > 100000:
            errors.append("Neuron count cannot exceed 100000 for stability")
        
        conn_prob = self._config.getOr("brain.connection_probability", 0.1)
        if conn_prob < 0.0 or conn_prob > 0.5:
            errors.append("Connection probability must be between 0.0 and 0.5")
        
        if errors:
            raise NLMValidationError("; ".join(errors))
        
        return errors
    
    def save_to_file(self, filepath: str):
        """Save configuration to JSON file."""
        config_dict = self._config_to_dict()
        with open(filepath, 'w') as f:
            json.dump(config_dict, f, indent=2)
        return self
    
    def load_from_file(self, filepath: str):
        """Load configuration from JSON file."""
        with open(filepath, 'r') as f:
            config_dict = json.load(f)
        self._dict_to_config(config_dict)
        return self
    
    def _config_to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        # This is a simplified version - in practice, you'd need to extract
        # all configuration values from the Config object
        return {}
    
    def _dict_to_config(self, config_dict: Dict[str, Any]):
        """Load configuration from dictionary."""
        for key, value in config_dict.items():
            self._config.set(key, value)
    
    def clone(self) -> 'EnhancedBrainConfig':
        """Create a copy of this configuration."""
        new_config = EnhancedBrainConfig(self._template)
        return new_config
    
    def __getattr__(self, name: str) -> Any:
        """Delegate to underlying config object."""
        return getattr(self._config, name)
    
    def __repr__(self) -> str:
        return f"EnhancedBrainConfig(template={self._template})"

# === Enhanced Brain Wrapper ===

class EnhancedBrain:
    """
    Enhanced brain wrapper with advanced features and monitoring.
    
    This class wraps the pynlm brain with enhanced functionality,
    including comprehensive monitoring, performance tracking, and
    advanced debugging capabilities.
    """
    
    def __init__(self, config: Optional[EnhancedBrainConfig] = None):
        """
        Initialize enhanced brain.
        
        Args:
            config: Enhanced configuration (auto-created if None)
        """
        if config is None:
            config = EnhancedBrainConfig()
        
        self._config = config
        self._brain = pynlm.createBrain(config._config)
        self._brain.initialize()
        
        # Performance monitoring
        self._performance_monitor = PerformanceMonitor()
        self._simulation_history: List[SimulationEpisode] = []
        self._episode_counter = 0
        self._is_simulating = False
        self._simulation_thread: Optional[threading.Thread] = None
        self._simulation_condition = threading.Condition()
    
    def initialize(self) -> bool:
        """
        Initialize the brain with enhanced error handling.
        
        Returns:
            True if successful, False otherwise
        
        Raises:
            NLMInitializationError: If initialization fails
        """
        try:
            success = self._brain.initialize()
            if not success:
                raise NLMInitializationError("Brain initialization returned False")
            
            # Validate configuration
            self._config.validate()
            
            return True
        except Exception as e:
            if not isinstance(e, NLMBrainError):
                raise NLMInitializationError(f"Initialization failed: {e}")
            raise
    
    def step(self, step_number: int, time: Optional[float] = None) -> Dict[str, Any]:
        """
        Run a single simulation step with enhanced monitoring.
        
        Args:
            step_number: Current step number
            time: Current simulation time (auto-calculated if None)
        
        Returns:
            Dictionary with step statistics
        """
        try:
            step_start = time.time()
            
            if time is None:
                time = step_number * 0.001  # Default 1ms per step
            
            # Collect pre-step metrics
            pre_stats = self._get_comprehensive_stats()
            
            # Execute simulation step
            self._brain.step(step_number, time)
            
            # Collect post-step metrics
            post_stats = self._get_comprehensive_stats()
            
            # Calculate step metrics
            step_metrics = {
                "step": step_number,
                "time": time,
                "duration_ms": (time.time() - step_start) * 1000,
                "pre_stats": pre_stats,
                "post_stats": post_stats,
                "delta": self._calculate_delta(pre_stats, post_stats),
                "timestamp": time.time()
            }
            
            # Update performance monitoring
            self._performance_monitor.record_step(step_metrics)
            
            return step_metrics
        
        except Exception as e:
            if not isinstance(e, NLMBrainError):
                raise NLMSimulationError(f"Simulation step {step_number} failed: {e}")
            raise
    
    def run_simulation(self, num_steps: int, mode: SimulationMode = SimulationMode.CONTINUOUS,
                      callback: Optional[Callable[[int, Dict[str, Any]], None]] = None,
                      save_history: bool = True) -> List[Dict[str, Any]]:
        """
        Run simulation with enhanced control and monitoring.
        
        Args:
            num_steps: Number of simulation steps
            mode: Simulation mode
            callback: Function called after each step
            save_history: Whether to save simulation history
        
        Returns:
            List of step metrics (if save_history is True)
        """
        if num_steps <= 0:
            raise NLMConfigurationError("Number of steps must be positive")
        
        self._is_simulating = True
        step_metrics_list = []
        
        try:
            for step in range(num_steps):
                if mode == SimulationMode.DEBUG:
                    # Debug mode with extra validation
                    self._validate_brain_state()
                
                step_metrics = self.step(step, step * 0.001)
                
                if save_history:
                    step_metrics_list.append(step_metrics)
                
                if callback:
                    callback(step, step_metrics)
                
                # Check for simulation stop condition
                if not self._is_simulating:
                    break
                
                # Mode-specific behavior
                if mode == SimulationMode.INTERACTIVE:
                    # Wait for external signal
                    with self._simulation_condition:
                        self._simulation_condition.wait(timeout=0.1)
                
        finally:
            self._is_simulating = False
        
        return step_metrics_list
    
    def run_episode(self, max_steps: int, episode_id: Optional[int] = None,
                   reward_function: Optional[Callable] = None) -> SimulationEpisode:
        """
        Run a single simulation episode.
        
        Args:
            max_steps: Maximum steps for this episode
            episode_id: Episode ID (auto-incremented if None)
            reward_function: Function to calculate reward
        
        Returns:
            Simulation episode with metrics
        """
        if episode_id is None:
            episode_id = self._episode_counter
            self._episode_counter += 1
        
        episode_start_step = self._brain.getTotalSpikeCount()
        episode_start_time = time.time()
        
        episode_metrics = []
        
        def episode_callback(step: int, step_metrics: Dict[str, Any]):
            episode_metrics.append(step_metrics)
        
        # Run episode
        self.run_simulation(max_steps, callback=episode_callback)
        
        # Calculate episode statistics
        episode_end_step = self._brain.getTotalSpikeCount()
        total_reward = sum(m.get("reward", 0.0) for m in episode_metrics)
        firing_rates = [m["post_stats"]["average_firing_rate"] for m in episode_metrics]
        
        episode = SimulationEpisode(
            episode_id=episode_id,
            start_step=episode_start_step,
            end_step=episode_end_step,
            total_reward=total_reward,
            average_firing_rate=sum(firing_rates) / len(firing_rates) if firing_rates else 0.0,
            memory_episodes_stored=self._brain.getEpisodicMemory().getEpisodeCount() if self._brain.getEpisodicMemory() else 0.0,
            steps_per_second=sum(m["post_stats"]["average_firing_rate"] for m in episode_metrics) / max_steps if max_steps > 0 else 0.0,
            success=(total_reward > 0.0),
            metrics=self._aggregate_metrics(episode_metrics)
        )
        
        self._simulation_history.append(episode)
        
        return episode
    
    def get_statistics(self) -> BrainStatistics:
        """
        Get comprehensive brain statistics.
        
        Returns:
            BrainStatistics object with all metrics
        """
        return self._get_comprehensive_stats()
    
    def get_performance_metrics(self) -> PerformanceMetrics:
        """
        Get performance monitoring metrics.
        
        Returns:
            PerformanceMetrics object
        """
        return self._performance_monitor.get_metrics()
    
    def get_simulation_history(self, limit: Optional[int] = None) -> List[SimulationEpisode]:
        """
        Get simulation history.
        
        Args:
            limit: Maximum number of episodes to return (most recent)
        
        Returns:
            List of simulation episodes
        """
        if limit is None:
            return self._simulation_history.copy()
        else:
            return self._simulation_history[-limit:]
    
    def get_development_stage(self) -> str:
        """Get current developmental stage."""
        return self._brain.getDevelopmentalStage()
    
    def set_development_stage(self, stage: str):
        """Set developmental stage."""
        self._brain.setDevelopmentalStage(stage)
        return self
    
    def save_brain(self, filepath: str) -> bool:
        """
        Save brain state to file.
        
        Args:
            filepath: File path to save to
        
        Returns:
            True if successful, False otherwise
        
        Raises:
            NLMPersistenceError: If save operation fails
        """
        try:
            success = self._brain.save(filepath)
            if not success:
                raise NLMPersistenceError("Brain save operation returned False")
            return True
        except Exception as e:
            if not isinstance(e, NLMBrainError):
                raise NLMPersistenceError(f"Brain save failed: {e}")
            raise
    
    def load_brain(self, filepath: str) -> bool:
        """
        Load brain state from file.
        
        Args:
            filepath: File path to load from
        
        Returns:
            True if successful, False otherwise
        
        Raises:
            NLMPersistenceError: If load operation fails
        """
        try:
            success = self._brain.load(filepath)
            if not success:
                raise NLMPersistenceError("Brain load operation returned False")
            return True
        except Exception as e:
            if not isinstance(e, NLMBrainError):
                raise NLMPersistenceError(f"Brain load failed: {e}")
            raise
    
    def reset(self):
        """Reset brain state."""
        self._brain.reset()
        self._performance_monitor.reset()
        return self
    
    def pause_simulation(self):
        """Pause simulation if running in interactive mode."""
        with self._simulation_condition:
            self._is_simulating = False
    
    def resume_simulation(self):
        """Resume simulation if paused."""
        with self._simulation_condition:
            self._is_simulating = True
    
    def validate_brain_state(self) -> List[str]:
        """
        Validate current brain state.
        
        Returns:
            List of validation errors (empty if valid)
        """
        return self._validate_brain_state()
    
    # === Convenience Methods ===
    
    def quick_test(self, test_type: str = "connectivity") -> bool:
        """
        Run quick test of brain functionality.
        
        Args:
            test_type: Type of test to run
        
        Returns:
            True if test passes, False otherwise
        """
        if test_type == "connectivity":
            return self._test_connectivity()
        elif test_type == "plasticity":
            return self._test_plasticity()
        elif test_type == "memory":
            return self._test_memory()
        elif test_type == "neuromodulation":
            return self._test_neuromodulation()
        else:
            raise NLMValidationError(f"Unknown test type: {test_type}")
    
    def create_agent(self):
        """Create and return an enhanced agent brain wrapper."""
        from .enhanced_agent import EnhancedAgentBrain
        return EnhancedAgentBrain(self)
    
    def create_world(self):
        """Create and return an enhanced world wrapper."""
        from .enhanced_world import EnhancedSimpleWorld
        return EnhancedSimpleWorld()
    
    # === Private Methods ===
    
    def _get_comprehensive_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics as dictionary."""
        stats = self._brain.getTotalNeuronCount()
        return {
            "total_neurons": stats,
            "total_synapses": self._brain.getTotalSynapseCount(),
            "firing_neurons": self._brain.getFiringNeuronCount(),
            "average_firing_rate": self._brain.getAverageFiringRate(),
            "excitation_inhibition_ratio": self._brain.getExcitationInhibitionRatio(),
            "development_stage": self.get_development_stage(),
            "curiosity_level": self._brain.getCuriosity().getLevel() if self._brain.getCuriosity() else 0.0,
            "novelty_level": self._brain.getNovelty().getLevel() if self._brain.getNovelty() else 0.0,
            "dopamine_level": self._brain.getDopamine().getLevel() if self._brain.getDopamine() else 0.0,
            "memory_traces": self._brain.getWorkingMemory().getActiveTraces() if self._brain.getWorkingMemory() else 0,
            "episodic_episodes": self._brain.getEpisodicMemory().getEpisodeCount() if self._brain.getEpisodicMemory() else 0,
            "simulation_steps": self._performance_monitor.get_stats().get("total_steps", 0),
            "total_spikes": self._brain.getTotalSpikeCount(),
            "working_memory_integrated": self._brain.getWorkingMemory() is not None,
            "episodic_memory_integrated": self._brain.getEpisodicMemory() is not None,
            "neuromodulation_integrated": self._brain.getDopamine() is not None,
            "prediction_integrated": self._brain.getPredictionSystem() is not None,
            "development_integrated": self._brain.getDevelopmentSystem() is not None,
        }
    
    def _calculate_delta(self, before: Dict[str, Any], after: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate delta between two state dictionaries."""
        delta = {}
        for key in set(before.keys()) | set(after.keys()):
            if key in before and key in after:
                if isinstance(before[key], (int, float)) and isinstance(after[key], (int, float)):
                    delta[key] = after[key] - before[key]
                else:
                    delta[key] = after[key]
            elif key in after:
                delta[key] = after[key]
        return delta
    
    def _aggregate_metrics(self, metrics_list: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Aggregate metrics from multiple steps."""
        if not metrics_list:
            return {}
        
        aggregated = {}
        for key in metrics_list[0].keys():
            values = [m[key] for m in metrics_list if key in m]
            if isinstance(values[0], (int, float)) if values else False:
                aggregated[key] = sum(values) / len(values)
            else:
                aggregated[key] = values[-1]  # Use last value for non-numeric
        
        return aggregated
    
    def _validate_brain_state(self) -> List[str]:
        """Validate brain state for potential issues."""
        errors = []
        
        # Check neuron count
        if self._brain.getTotalNeuronCount() == 0:
            errors.append("Brain has no neurons")
        
        # Check for unrealistic firing rates
        firing_rate = self._brain.getAverageFiringRate()
        if firing_rate < 0.0:
            errors.append("Average firing rate is negative")
        elif firing_rate > 100.0:  # Hz
            errors.append("Average firing rate is extremely high (> 100 Hz)")
        
        # Check spike count
        if self._brain.getTotalSpikeCount() == 0:
            errors.append("No spikes generated")
        
        return errors
    
    def _test_connectivity(self) -> bool:
        """Test neural connectivity."""
        # Simple connectivity test
        neurons = self._brain.getTotalNeuronCount()
        if neurons < 10:
            return False
        
        # Run a few steps and check for activity
        initial_spikes = self._brain.getTotalSpikeCount()
        for _ in range(10):
            self._brain.step(0, 0.0)
        
        final_spikes = self._brain.getTotalSpikeCount()
        return (final_spikes - initial_spikes) > 0
    
    def _test_plasticity(self) -> bool:
        """Test synaptic plasticity."""
        if not self._brain.getSTDP():
            return False
        
        # Record initial weights
        initial_weights = []
        region = self._brain.getRegion(0)
        if region:
            for syn in region.getSynapses():
                initial_weights.append(syn.getWeight())
        
        if not initial_weights:
            return False
        
        # Apply plasticity triggers
        for _ in range(5):
            self._brain.step(0, 0.0)
        
        # Check for weight changes
        final_weights = []
        if region:
            for syn in region.getSynapses():
                final_weights.append(syn.getWeight())
        
        # Check if any weights changed significantly
        weight_changes = [abs(f - i) for f, i in zip(final_weights, initial_weights)]
        return any(change > 0.001 for change in weight_changes)
    
    def _test_memory(self) -> bool:
        """Test memory systems."""
        working_memory = self._brain.getWorkingMemory()
        episodic_memory = self._brain.getEpisodicMemory()
        
        if not working_memory or not episodic_memory:
            return False
        
        # Inject some activity to create memory traces
        for _ in range(50):
            self._brain.step(0, 0.0)
        
        return (working_memory.getActiveTraces() > 0 or 
                episodic_memory.getEpisodeCount() > 0)
    
    def _test_neuromodulation(self) -> bool:
        """Test neuromodulation systems."""
        dopamine = self._brain.getDopamine()
        curiosity = self._brain.getCuriosity()
        novelty = self._brain.getNovelty()
        
        if not dopamine or not curiosity or not novelty:
            return False
        
        # Check for neuromodulator levels
        return (dopamine.getLevel() >= 0.0 and 
                curiosity.getLevel() >= 0.0 and 
                novelty.getLevel() >= 0.0)

# === Performance Monitor ===

class PerformanceMonitor:
    """Comprehensive performance monitoring for NLM simulations."""
    
    def __init__(self):
        self._step_metrics: List[Dict[str, Any]] = []
        self._lock = threading.Lock()
        self._reset()
    
    def _reset(self):
        """Reset monitoring state."""
        self._start_time = time.time()
        self._total_steps = 0
        self._peak_cpu_usage = 0.0
        self._total_memory_usage = 0.0
        self._min_step_time = float('inf')
        self._max_step_time = 0.0
    
    def record_step(self, step_metrics: Dict[str, Any]):
        """Record step metrics."""
        with self._lock:
            self._step_metrics.append(step_metrics)
            self._total_steps += 1
            
            # Track performance
            duration_ms = step_metrics.get("duration_ms", 0.0)
            if duration_ms < self._min_step_time:
                self._min_step_time = duration_ms
            if duration_ms > self._max_step_time:
                self._max_step_time = duration_ms
            
            self._total_memory_usage += step_metrics.get("post_stats", {}).get("memory_usage", 0.0)
    
    def get_metrics(self) -> PerformanceMetrics:
        """Get aggregated performance metrics."""
        with self._lock:
            if not self._step_metrics:
                return PerformanceMetrics()
            
            total_time = time.time() - self._start_time
            num_steps = len(self._step_metrics)
            
            # Calculate average metrics
            avg_steps_per_sec = num_steps / total_time if total_time > 0 else 0.0
            
            spikes_per_step_values = []
            firing_rates = []
            memory_usages = []
            
            for step in self._step_metrics:
                post_stats = step.get("post_stats", {})
                spikes_per_step_values.append(post_stats.get("average_firing_rate", 0.0))
                firing_rates.append(post_stats.get("average_firing_rate", 0.0))
                memory_usages.append(post_stats.get("memory_usage", 0.0))
            
            avg_spikes_per_step = (sum(spikes_per_step_values) / len(spikes_per_step_values) 
                                 if spikes_per_step_values else 0.0)
            avg_firing_rate = (sum(firing_rates) / len(firing_rates) 
                             if firing_rates else 0.0)
            avg_memory_usage = (sum(memory_usages) / len(memory_usages) 
                              if memory_usages else 0.0)
            
            return PerformanceMetrics(
                steps_per_second=avg_steps_per_sec,
                spikes_per_step=avg_spikes_per_step,
                synaptic_events_per_second=avg_steps_per_sec * avg_spikes_per_step,
                real_time_factor=1.0,  # Simplified calculation
                active_neurons=0,  # Would need to track this separately
                firing_neurons=0,  # Would need to track this separately
                pending_spikes=0,  # Would need to track this separately
                pending_delayed_spikes=0,  # Would need to track this separately
                avg_firing_rate=avg_firing_rate,
                memory_usage_mb=avg_memory_usage / 1024 / 1024,  # Convert to MB
                step_time_ms=(self._min_step_time + self._max_step_time) / 2,
                cpu_usage=self._peak_cpu_usage,
                simulation_efficiency=avg_steps_per_sec / 1000.0  # Simplified
            )
    
    def get_stats(self) -> Dict[str, Any]:
        """Get raw statistics."""
        with self._lock:
            return {
                "total_steps": self._total_steps,
                "num_step_metrics": len(self._step_metrics),
                "min_step_time_ms": self._min_step_time,
                "max_step_time_ms": self._max_step_time,
                "avg_step_time_ms": (self._min_step_time + self._max_step_time) / 2,
                "total_time_seconds": time.time() - self._start_time,
            }
    
    def reset(self):
        """Reset monitoring."""
        with self._lock:
            self._reset()
            self._step_metrics.clear()

# === Main Factory Functions ===

def create_enhanced_brain(config: Optional[EnhancedBrainConfig] = None) -> EnhancedBrain:
    """
    Create an enhanced brain instance.
    
    Args:
        config: Enhanced configuration (auto-created if None)
    
    Returns:
        EnhancedBrain instance
    """
    return EnhancedBrain(config)

def create_brain_with_template(template: ConfigurationTemplate) -> EnhancedBrain:
    """
    Create brain with predefined configuration template.
    
    Args:
        template: Configuration template to use
    
    Returns:
        EnhancedBrain instance
    """
    config = EnhancedBrainConfig(template)
    return EnhancedBrain(config)

def create_research_brain(neuron_count: int = 10000, enable_gpu: bool = False) -> EnhancedBrain:
    """
    Create a research-grade brain with advanced features.
    
    Args:
        neuron_count: Number of neurons
        enable_gpu: Whether to enable GPU optimizations
    
    Returns:
        EnhancedBrain instance optimized for research
    """
    config = EnhancedBrainConfig(ConfigurationTemplate.RESEARCH)
    config.set_neuron_count(neuron_count)
    
    if enable_gpu:
        config.set_optimization_level("maximum")
    
    return EnhancedBrain(config)

def create_real_time_brain(neuron_count: int = 2000) -> EnhancedBrain:
    """
    Create a real-time brain optimized for low-latency operation.
    
    Args:
        neuron_count: Number of neurons
    
    Returns:
        EnhancedBrain instance optimized for real-time
    """
    config = EnhancedBrainConfig(ConfigurationTemplate.REAL_TIME)
    config.set_neuron_count(neuron_count)
    
    return EnhancedBrain(config)

# === Backward Compatibility ===

# Re-export original pynlm classes for backward compatibility
__all__ = [
    # Original pynlm classes
    "createBrain",
    "createDefaultConfig",
    "createSimpleWorld",
    "createAgentBrain",
    
    # Enhanced classes
    "EnhancedBrainConfig",
    "EnhancedBrain",
    "EnhancedBrainError",
    "EnhancedBrainConfigurationError",
    "EnhancedBrainInitializationError",
    "EnhancedBrainSimulationError",
    "EnhancedBrainPersistenceError",
    "EnhancedBrainValidationError",
    "SimulationMode",
    "BrainStatistics",
    "PerformanceMetrics",
    "SimulationEpisode",
    "ConfigurationTemplate",
    
    # Factory functions
    "create_enhanced_brain",
    "create_brain_with_template",
    "create_research_brain",
    "create_real_time_brain",
    
    # Error classes
    "NLMBrainError",
    "NLMConfigurationError",
    "NLMInitializationError",
    "NLMSimulationError",
    "NLMPersistenceError",
    "NLMValidationError",
]

# Try to import and re-export original pynlm functions
if _PYNLMB_AVAILABLE:
    try:
        from pynlm import (
            createBrain,
            createDefaultConfig,
            createSimpleWorld,
            createAgentBrain,
            NeuronType,
            ActionType,
            DevelopmentalStage,
            WorldObjectType,
        )
        
        __all__.extend([
            "createBrain",
            "createDefaultConfig", 
            "createSimpleWorld",
            "createAgentBrain",
            "NeuronType",
            "ActionType",
            "DevelopmentalStage",
            "WorldObjectType",
        ])
    except ImportError:
        pass

# === Module Metadata ===

__version__ = "2.0.0-enhanced"
__author__ = "NLM Development Team"
__description__ = "Enhanced Python bindings for NLM with advanced features, type safety, and comprehensive monitoring."

if __name__ == "__main__":
    # Example usage
    print("Enhanced NLM Python Bindings")
    print("=" * 50)
    
    # Create enhanced brain
    brain = create_enhanced_brain()
    print(f"Created enhanced brain with {brain.get_statistics()['total_neurons']} neurons")
    
    # Run simulation
    metrics = brain.run_simulation(100)
    print(f"Ran 100 simulation steps")
    
    # Get statistics
    stats = brain.get_statistics()
    print(f"Total spikes: {stats['total_spikes']}")
    print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
    
    print("\nEnhanced NLM API ready for use!")
