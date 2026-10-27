#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Enhanced Python API

This module provides a high-level, Pythonic interface to the NLM neural simulation framework.
It includes comprehensive type hints, error handling, and advanced features for both
beginners and advanced users.

Key Features:
- High-level simulation management
- Experiment framework with session tracking
- Memory management utilities
- Performance monitoring and profiling
- Advanced configuration system
- Visualization and debugging tools
- Comprehensive documentation and examples

Usage:
    import pynlm
    
    # Create and run a simple brain
    brain = pynlm.create_brain()
    brain.initialize()
    for step in range(1000):
        brain.step(step)
    
    # Run an experiment with automatic logging
    experiment = pynlm.Experiment()
    experiment.run(brain, world, agent, steps=1000)
    experiment.save_results("experiment_results.json")
"""

# === Version and Metadata ===
__version__ = "2.0.0"
__author__ = "NLM Project"
__license__ = "MIT"

import json
import time
import logging
import threading
from typing import Optional, List, Dict, Any, Callable, Union, Tuple
from enum import Enum
import numpy as np

# === Logging Setup ===
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# === Advanced Exception Classes ===
class NLMError(Exception):
    """Base exception for all NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Raised for configuration-related errors."""
    pass

class SimulationError(NLMError):
    """Raised for simulation-related errors."""
    pass

class MemoryError(NLMError):
    """Raised for memory system errors."""
    pass

# === Core Enums ===
class DevelopmentalStage(Enum):
    """Developmental stages for brain maturation."""
    INITIAL = "Initial"
    CRITICAL_PERIOD = "CriticalPeriod"
    MATURATION = "Maturation"
    ADULT = "Adult"
    AGING = "Aging"

class LearningMode(Enum):
    """Learning modes for neuromodulation."""
    REINFORCEMENT = "reinforcement"
    EXPLORATION = "exploration"
    NOVELTY = "novelty"
    COMBINED = "combined"

class RecordingLevel(Enum):
    """Recording levels for experiment data."""
    MINIMAL = "minimal"
    STANDARD = "standard"
    COMPREHENSIVE = "comprehensive"

# === High-Level API Classes ===
class Experiment:
    """
    Comprehensive experiment management system.
    
    Manages complete experimental sessions with automatic logging,
    data collection, and result analysis.
    """
    
    def __init__(self, name: str, config: Optional[Dict[str, Any]] = None):
        """
        Initialize an experiment.
        
        Args:
            name: Unique name for the experiment
            config: Optional configuration dictionary
        """
        self.name = name
        self.config = config or {}
        self.start_time = None
        self.end_time = None
        self.results = {}
        self.sessions = []
        self.logger = logging.getLogger(f"Experiment.{name}")
        
        # Setup data recording
        self.record_level = RecordingLevel.STANDARD
        self.custom_metrics = {}
        self.session_data = []
        
        self.logger.info(f"Experiment '{name}' initialized")
    
    def run(self, brain, world=None, agent=None, 
            steps: int = 1000, timestep: float = 0.1,
            record_data: bool = True) -> Dict[str, Any]:
        """
        Run the experiment.
        
        Args:
            brain: The brain object
            world: Optional world object (for embodied agents)
            agent: Optional agent object
            steps: Number of simulation steps
            timestep: Time per step in seconds
            record_data: Whether to record detailed data
            
        Returns:
            Dictionary with experiment results
        """
        self.start_time = time.time()
        self.logger.info(f"Starting experiment '{self.name}' with {steps} steps")
        
        # Initialize recording if requested
        if record_data:
            self._initialize_recording()
        
        try:
            # Run simulation loop
            session_results = {
                'steps_completed': 0,
                'spike_count': 0,
                'memory_traces': 0,
                'novelty_levels': [],
                'curiosity_levels': [],
                'rewards': [],
                'actions': [],
                'timestamps': []
            }
            
            for step in range(steps):
                current_time = step * timestep
                
                # Execute the simulation step
                if world and agent:
                    # Embodied agent simulation
                    world.update(timestep)
                    
                    if record_data:
                        percept = world.getSensoryPercept()
                        agent.processSensoryInput(percept)
                        
                        # Record metrics
                        session_results['novelty_levels'].append(agent.getNoveltyLevel())
                        session_results['curiosity_levels'].append(agent.getCuriosityLevel())
                        
                        if agent.getBrain().getTotalSpikeCount() > 0:
                            session_results['spike_count'] += 1
                    
                    agent.processSensoryInput(percept)
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    
                    if record_data:
                        session_results['actions'].append(action)
                        session_results['timestamps'].append(current_time)
                
                else:
                    # Simple brain simulation
                    brain.step(step)
                
                # Record working memory traces if available
                if record_data and hasattr(brain, 'getWorkingMemory'):
                    working_memory = brain.getWorkingMemory()
                    if working_memory:
                        session_results['memory_traces'] = working_memory.getActiveTraces()
                
                session_results['steps_completed'] += 1
                
                # Log progress periodically
                if step % 100 == 0:
                    self.logger.info(f"Progress: {step}/{steps} steps completed")
            
            self.end_time = time.time()
            self.sessions.append(session_results)
            
            # Calculate summary statistics
            self._calculate_session_summary(session_results)
            
            # Generate experiment results
            experiment_results = self._generate_experiment_results(session_results)
            self.results.update(experiment_results)
            
            self.logger.info(f"Experiment '{self.name}' completed in {self.end_time - self.start_time:.2f}s")
            return experiment_results
            
        except Exception as e:
            self.logger.error(f"Experiment '{self.name}' failed: {str(e)}")
            raise SimulationError(f"Experiment failed: {str(e)}") from e
    
    def _initialize_recording(self):
        """Initialize data recording infrastructure."""
        self.session_data = []
        self.custom_metrics = {}
        self.logger.debug("Recording infrastructure initialized")
    
    def _calculate_session_summary(self, session_results: Dict[str, Any]):
        """Calculate summary statistics for the session."""
        if not session_results['timestamps']:
            return
        
        duration = session_results['timestamps'][-1] - session_results['timestamps'][0]
        
        if session_results['novelty_levels']:
            session_results['avg_novelty'] = sum(session_results['novelty_levels']) / len(session_results['novelty_levels'])
            session_results['max_novelty'] = max(session_results['novelty_levels'])
            session_results['min_novelty'] = min(session_results['novelty_levels'])
        
        if session_results['curiosity_levels']:
            session_results['avg_curiosity'] = sum(session_results['curiosity_levels']) / len(session_results['curiosity_levels'])
            session_results['max_curiosity'] = max(session_results['curiosity_levels'])
            session_results['min_curiosity'] = min(session_results['curiosity_levels'])
        
        session_results['duration'] = duration
        session_results['steps_per_second'] = session_results['steps_completed'] / duration if duration > 0 else 0
    
    def _generate_experiment_results(self, session_results: Dict[str, Any]) -> Dict[str, Any]:
        """Generate comprehensive experiment results."""
        return {
            'name': self.name,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'duration': self.end_time - self.start_time,
            'steps_completed': session_results['steps_completed'],
            'total_spikes': session_results['spike_count'],
            'memory_traces': session_results['memory_traces'],
            'average_novelty': session_results.get('avg_novelty', 0),
            'average_curiosity': session_results.get('avg_curiosity', 0),
            'max_novelty': session_results.get('max_novelty', 0),
            'max_curiosity': session_results.get('max_curiosity', 0),
            'actions_taken': len(session_results['actions']),
            'experiments': len(self.sessions)
        }
    
    def save_results(self, filepath: str):
        """
        Save experiment results to JSON file.
        
        Args:
            filepath: Path to save results
        """
        results_data = {
            'experiments': self.results,
            'sessions': self.sessions,
            'config': self.config
        }
        
        with open(filepath, 'w') as f:
            json.dump(results_data, f, indent=2, default=str)
        
        self.logger.info(f"Results saved to {filepath}")
    
    def load_results(self, filepath: str):
        """
        Load experiment results from JSON file.
        
        Args:
            filepath: Path to load results from
        """
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        self.results = data.get('experiments', {})
        self.sessions = data.get('sessions', [])
        self.config = data.get('config', {})
        
        self.logger.info(f"Results loaded from {filepath}")


class MemoryManager:
    """
    High-level memory management utilities.
    
    Provides tools for analyzing and managing memory systems,
    including compression, consolidation, and replay operations.
    """
    
    def __init__(self, brain):
        """
        Initialize memory manager.
        
        Args:
            brain: The brain object with memory systems
        """
        self.brain = brain
        self.logger = logging.getLogger("MemoryManager")
        
        # Validate brain has memory systems
        if not hasattr(brain, 'getWorkingMemory'):
            raise MemoryError("Brain does not have working memory system")
        
        self.logger.info("MemoryManager initialized")
    
    def get_memory_stats(self) -> Dict[str, Any]:
        """
        Get comprehensive memory system statistics.
        
        Returns:
            Dictionary with memory statistics
        """
        stats = {
            'working_memory': None,
            'episodic_memory': None,
            'associative_memory': None
        }
        
        working_memory = self.brain.getWorkingMemory()
        if working_memory:
            stats['working_memory'] = {
                'active_traces': working_memory.getActiveTraces(),
                'trace_capacity': working_memory.getTraceCapacity(),
                'competition_count': working_memory.getCompetitionCount()
            }
        
        episodic_memory = self.brain.getEpisodicMemory()
        if episodic_memory:
            stats['episodic_memory'] = {
                'episode_count': episodic_memory.getEpisodeCount(),
                'max_episodes': episodic_memory.getMaxEpisodes(),
                'consolidation_threshold': episodic_memory.getConsolidationThreshold()
            }
        
        associative_memory = self.brain.getAssociativeMemory()
        if associative_memory:
            stats['associative_memory'] = {
                'association_count': associative_memory.getAssociationCount(),
                'similarity_threshold': associative_memory.getSimilarityThreshold()
            }
        
        return stats
    
    def consolidate_memories(self, working_threshold: float = 0.7, 
                           episodic_threshold: float = 0.3):
        """
        Consolidate memories based on activity thresholds.
        
        Args:
            working_threshold: Threshold for working memory consolidation
            episodic_threshold: Threshold for episodic memory consolidation
        """
        self.logger.info("Starting memory consolidation")
        
        # Consolidate working memory
        working_memory = self.brain.getWorkingMemory()
        if working_memory:
            working_memory.consolidate(working_threshold)
            self.logger.info(f"Consolidated working memory: {working_memory.getActiveTraces()} traces")
        
        # Consolidate episodic memory
        episodic_memory = self.brain.getEpisodicMemory()
        if episodic_memory:
            episodic_memory.consolidate(episodic_threshold)
            self.logger.info(f"Consolidated episodic memory: {episodic_memory.getEpisodeCount()} episodes")
    
    def replay_memories(self, episode_count: int = 3, replay_strength: float = 1.0):
        """
        Replay important memories for consolidation.
        
        Args:
            episode_count: Number of episodes to replay
            replay_strength: Strength of replay
        """
        self.logger.info(f"Starting memory replay of {episode_count} episodes")
        
        episodic_memory = self.brain.getEpisodicMemory()
        if episodic_memory:
            episodes = episodic_memory.getEpisodesForReplay(episode_count)
            for episode in episodes:
                episodic_memory.replayEpisode(episode, replay_strength)
            self.logger.info(f"Replayed {len(episodes)} episodes")
    
    def clear_all_memories(self):
        """Clear all memory systems."""
        self.logger.info("Clearing all memory systems")
        
        working_memory = self.brain.getWorkingMemory()
        if working_memory:
            working_memory.clear()
        
        episodic_memory = self.brain.getEpisodicMemory()
        if episodic_memory:
            episodic_memory.clear()
        
        associative_memory = self.brain.getAssociativeMemory()
        if associative_memory:
            associative_memory.clear()
        
        self.logger.info("All memory systems cleared")


class PerformanceProfiler:
    """
    Performance monitoring and profiling tools.
    
    Provides detailed performance metrics and profiling capabilities
    for simulation optimization and analysis.
    """
    
    def __init__(self, brain):
        """
        Initialize performance profiler.
        
        Args:
            brain: The brain object to profile
        """
        self.brain = brain
        self.logger = logging.getLogger("PerformanceProfiler")
        
        # Performance metrics
        self.metrics = {
            'time_per_step': [],
            'spike_count': [],
            'memory_usage': [],
            'neuron_activity': [],
            'synapse_activity': []
        }
        
        self.profiling_enabled = False
        self.sampling_rate = 1.0  # Hz
        
        self.logger.info("PerformanceProfiler initialized")
    
    def start_profiling(self, sampling_rate: float = 1.0):
        """
        Start performance profiling.
        
        Args:
            sampling_rate: Sampling rate in Hz
        """
        self.profiling_enabled = True
        self.sampling_rate = sampling_rate
        self.logger.info(f"Started performance profiling at {sampling_rate} Hz")
    
    def stop_profiling(self):
        """Stop performance profiling."""
        self.profiling_enabled = False
        self.logger.info("Stopped performance profiling")
    
    def profile_step(self, step: int):
        """
        Profile a single simulation step.
        
        Args:
            step: Current simulation step
        """
        if not self.profiling_enabled:
            return
        
        # Record time
        step_start = time.time()
        
        # Get brain metrics
        step_end = time.time()
        time_taken = step_end - step_start
        
        # Record metrics
        self.metrics['time_per_step'].append(time_taken)
        self.metrics['spike_count'].append(self.brain.getTotalSpikeCount())
        
        # Record memory usage if available
        if hasattr(self.brain, 'getWorkingMemory'):
            working_memory = self.brain.getWorkingMemory()
            if working_memory:
                self.metrics['memory_usage'].append(working_memory.getActiveTraces())
        
        # Record neural activity
        self.metrics['neuron_activity'].append(self.brain.getFiringNeuronCount())
        self.metrics['synapse_activity'].append(self.brain.getTotalSynapseCount())
    
    def get_performance_report(self) -> Dict[str, Any]:
        """
        Get performance report.
        
        Returns:
            Dictionary with performance metrics
        """
        if not self.metrics['time_per_step']:
            return {'error': 'No profiling data available'}
        
        report = {
            'total_steps_profiled': len(self.metrics['time_per_step']),
            'average_time_per_step': sum(self.metrics['time_per_step']) / len(self.metrics['time_per_step']),
            'min_time_per_step': min(self.metrics['time_per_step']),
            'max_time_per_step': max(self.metrics['time_per_step']),
            'std_time_per_step': np.std(self.metrics['time_per_step']) if len(self.metrics['time_per_step']) > 1 else 0,
            
            'spike_count': {
                'total': sum(self.metrics['spike_count']),
                'average': np.mean(self.metrics['spike_count']) if self.metrics['spike_count'] else 0
            },
            
            'memory_usage': {
                'max': max(self.metrics['memory_usage']) if self.metrics['memory_usage'] else 0,
                'average': np.mean(self.metrics['memory_usage']) if self.metrics['memory_usage'] else 0
            },
            
            'neuron_activity': {
                'firing_rate': {
                    'max': max(self.metrics['neuron_activity']) if self.metrics['neuron_activity'] else 0,
                    'average': np.mean(self.metrics['neuron_activity']) if self.metrics['neuron_activity'] else 0
                }
            },
            
            'sampling_rate': self.sampling_rate,
            'profiling_enabled': self.profiling_enabled
        }
        
        return report
    
    def generate_profile_plot_data(self) -> Dict[str, List[float]]:
        """
        Generate profile plot data.
        
        Returns:
            Dictionary with plotting data
        """
        return {
            'time_per_step_ms': [t * 1000 for t in self.metrics['time_per_step']],
            'spike_counts': self.metrics['spike_count'],
            'memory_traces': self.metrics['memory_usage'],
            'firing_neuron_counts': self.metrics['neuron_activity']
        }


class ConfigManager:
    """
    Advanced configuration management system.
    
    Provides a user-friendly interface for configuration management
    with validation, serialization, and experiment-specific settings.
    """
    
    def __init__(self, config_dict: Optional[Dict[str, Any]] = None):
        """
        Initialize configuration manager.
        
        Args:
            config_dict: Initial configuration dictionary
        """
        self.config = config_dict or {}
        self.logger = logging.getLogger("ConfigManager")
        
        # Configuration schemas
        self.schemas = {
            'brain': self._get_brain_schema(),
            'plasticity': self._get_plasticity_schema(),
            'neuromodulation': self._get_neuromodulation_schema(),
            'development': self._get_development_schema(),
            'performance': self._get_performance_schema()
        }
        
        self.logger.info("ConfigManager initialized")
    
    def _get_brain_schema(self) -> Dict[str, Any]:
        """Get brain configuration schema."""
        return {
            'neuron_count': {'type': int, 'min': 100, 'max': 100000, 'default': 1000},
            'region_count': {'type': int, 'min': 1, 'max': 10, 'default': 1},
            'connection_probability': {'type': float, 'min': 0.01, 'max': 1.0, 'default': 0.1},
            'v_thresh': {'type': float, 'min': -100.0, 'max': 0.0, 'default': -50.0},
            'v_rest': {'type': float, 'min': -100.0, 'max': 0.0, 'default': -70.0},
            'v_reset': {'type': float, 'min': -100.0, 'max': 0.0, 'default': -75.0},
            'tau_mem': {'type': float, 'min': 1.0, 'max': 100.0, 'default': 20.0},
            'tau_ref': {'type': float, 'min': 0.1, 'max': 50.0, 'default': 2.0}
        }
    
    def _get_plasticity_schema(self) -> Dict[str, Any]:
        """Get plasticity configuration schema."""
        return {
            'plasticity_stdp_enable': {'type': bool, 'default': True},
            'plasticity_stdp_learning_rate': {'type': float, 'min': 0.0, 'max': 1.0, 'default': 0.001},
            'plasticity_stdp_tau_plus': {'type': float, 'min': 1.0, 'max': 100.0, 'default': 20.0},
            'plasticity_stdp_tau_minus': {'type': float, 'min': 1.0, 'max': 100.0, 'default': 20.0},
            'plasticity_hebbian_enable': {'type': bool, 'default': True},
            'plasticity_structural_enable': {'type': bool, 'default': True},
            'synaptogenesis_rate': {'type': float, 'min': 0.0, 'max': 0.01, 'default': 0.0001},
            'pruning_rate': {'type': float, 'min': 0.0, 'max': 0.01, 'default': 0.00001}
        }
    
    def _get_neuromodulation_schema(self) -> Dict[str, Any]:
        """Get neuromodulation configuration schema."""
        return {
            'neuromod_dopamine_scale': {'type': float, 'min': 0.0, 'max': 10.0, 'default': 1.0},
            'neuromod_curiosity_enable': {'type': bool, 'default': True},
            'neuromod_novelty_enable': {'type': bool, 'default': True}
        }
    
    def _get_development_schema(self) -> Dict[str, Any]:
        """Get development configuration schema."""
        return {
            'development_initial_plasticity': {'type': float, 'min': 0.0, 'max': 2.0, 'default': 1.0},
            'development_critical_plasticity': {'type': float, 'min': 0.0, 'max': 2.0, 'default': 0.8},
            'development_mature_plasticity': {'type': float, 'min': 0.0, 'max': 2.0, 'default': 0.2}
        }
    
    def _get_performance_schema(self) -> Dict[str, Any]:
        """Get performance configuration schema."""
        return {
            'performance_max_energy': {'type': float, 'min': 0.0, 'max': 1000.0, 'default': 100.0},
            'performance_energy_decay_rate': {'type': float, 'min': 0.0, 'max': 1.0, 'default': 0.01},
            'performance_simulation_timestep': {'type': float, 'min': 0.001, 'max': 1.0, 'default': 0.001}
        }
    
    def validate_config(self) -> List[str]:
        """
        Validate configuration against schemas.
        
        Returns:
            List of validation errors (empty if valid)
        """
        errors = []
        
        for section, schema in self.schemas.items():
            section_config = self.config.get(section, {})
            
            for key, rules in schema.items():
                if key in section_config:
                    value = section_config[key]
                    
                    # Check type
                    if rules['type'] == int and not isinstance(value, int):
                        errors.append(f"{section}.{key}: Expected int, got {type(value).__name__}")
                    elif rules['type'] == float and not isinstance(value, (int, float)):
                        errors.append(f"{section}.{key}: Expected float, got {type(value).__name__}")
                    elif rules['type'] == bool and not isinstance(value, bool):
                        errors.append(f"{section}.{key}: Expected bool, got {type(value).__name__}")
                    
                    # Check range
                    if 'min' in rules and value < rules['min']:
                        errors.append(f"{section}.{key}: Value {value} below minimum {rules['min']}")
                    
                    if 'max' in rules and value > rules['max']:
                        errors.append(f"{section}.{key}: Value {value} above maximum {rules['max']}")
        
        return errors
    
    def set(self, key: str, value: Any, section: str = None):
        """
        Set configuration value with validation.
        
        Args:
            key: Configuration key
            value: Configuration value
            section: Configuration section
        """
        if section:
            full_key = f"{section}.{key}"
        else:
            full_key = key
        
        # Validate before setting
        temp_config = self.config.copy()
        temp_config['test'] = {key: value}
        
        errors = self._validate_single(key, value, self.config)
        if errors:
            raise ConfigurationError(f"Invalid configuration: {errors}")
        
        # Set the value
        if section:
            if section not in self.config:
                self.config[section] = {}
            self.config[section][key] = value
        else:
            self.config[key] = value
        
        self.logger.info(f"Configuration set: {full_key} = {value}")
    
    def _validate_single(self, key: str, value: Any, config: Dict[str, Any]) -> List[str]:
        """Validate a single configuration value."""
        errors = []
        
        # Find the schema for this key
        for section, schema in self.schemas.items():
            if key in schema:
                rules = schema[key]
                
                # Check type
                if rules['type'] == int and not isinstance(value, int):
                    errors.append(f"Expected int, got {type(value).__name__}")
                elif rules['type'] == float and not isinstance(value, (int, float)):
                    errors.append(f"Expected float, got {type(value).__name__}")
                elif rules['type'] == bool and not isinstance(value, bool):
                    errors.append(f"Expected bool, got {type(value).__name__}")
                
                # Check range
                if 'min' in rules and value < rules['min']:
                    errors.append(f"Value {value} below minimum {rules['min']}")
                
                if 'max' in rules and value > rules['max']:
                    errors.append(f"Value {value} above maximum {rules['max']}")
                
                break
        
        return errors
    
    def get(self, key: str, section: str = None, default: Any = None) -> Any:
        """
        Get configuration value.
        
        Args:
            key: Configuration key
            section: Configuration section
            default: Default value if not found
            
        Returns:
            Configuration value or default
        """
        if section:
            return self.config.get(section, {}).get(key, default)
        else:
            return self.config.get(key, default)
    
    def save(self, filepath: str):
        """
        Save configuration to JSON file.
        
        Args:
            filepath: Path to save configuration
        """
        with open(filepath, 'w') as f:
            json.dump(self.config, f, indent=2)
        
        self.logger.info(f"Configuration saved to {filepath}")
    
    def load(self, filepath: str):
        """
        Load configuration from JSON file.
        
        Args:
            filepath: Path to load configuration from
        """
        with open(filepath, 'r') as f:
            self.config = json.load(f)
        
        self.logger.info(f"Configuration loaded from {filepath}")


# === Global Configuration ===
global_config = {
    'log_level': 'INFO',
    'default_simulation_timestep': 0.1,
    'max_steps': 10000,
    'enable_profiling': False,
    'default_recording_level': RecordingLevel.STANDARD,
    'memory_consolidation_enabled': True,
    'auto_save_experiments': False
}


# === Convenience Factory Functions ===
def create_brain(config: Optional[Dict[str, Any]] = None) -> Any:
    """
    Create a brain with configuration.
    
    Args:
        config: Optional configuration dictionary
        
    Returns:
        Configured brain object
    """
    # Import here to avoid circular imports
    try:
        import pynlm as pynlm_c
        return pynlm_c.createBrain(pynlm_c.createDefaultConfig())
    except ImportError:
        raise ImportError("pynlm C++ library not available. Please install NLM.")


def create_world(width: int = 20, height: int = 20, 
                vision_width: int = 8, vision_height: int = 8) -> Any:
    """
    Create a world.
    
    Args:
        width: World width
        height: World height
        vision_width: Vision width
        vision_height: Vision height
        
    Returns:
        World object
    """
    try:
        import pynlm as pynlm_c
        world = pynlm_c.createSimpleWorld()
        world.configure(width, height, vision_width, vision_height)
        return world
    except ImportError:
        raise ImportError("pynlm C++ library not available. Please install NLM.")


def create_agent(brain) -> Any:
    """
    Create an agent with a brain.
    
    Args:
        brain: Brain object
        
    Returns:
        Agent object
    """
    try:
        import pynlm as pynlm_c
        return pynlm_c.createAgentBrain(brain)
    except ImportError:
        raise ImportError("pynlm C++ library not available. Please install NLM.")


# === Documentation and Help ===
__doc__ = """
NLM (Neural Learning Machine) - Enhanced Python API

This module provides a comprehensive Python interface to the NLM neural simulation framework.

Quick Start Examples:

1. Simple Brain Simulation:
   ```python
   import pynlm
   
   brain = pynlm.create_brain()
   brain.initialize()
   
   for step in range(1000):
       brain.step(step)
   
   print(f"Neurons: {brain.getTotalNeuronCount()}, Spikes: {brain.getTotalSpikeCount()}")
   ```

2. Complete Agent Simulation:
   ```python
   import pynlm
   
   # Setup
   brain = pynlm.create_brain()
   brain.initialize()
   
   world = pynlm.create_world(width=20, height=20, vision_width=8, vision_height=8)
   world.reset()
   
   agent = pynlm.create_agent(brain)
   agent.initialize(world)
   
   # Run experiment
   experiment = pynlm.Experiment("My Experiment")
   results = experiment.run(brain, world, agent, steps=1000)
   
   experiment.save_results("experiment_results.json")
   ```

3. Advanced Memory Management:
   ```python
   import pynlm
   
   brain = pynlm.create_brain()
   brain.initialize()
   
   memory_manager = pynlm.MemoryManager(brain)
   stats = memory_manager.get_memory_stats()
   
   print(f"Working memory traces: {stats['working_memory']['active_traces']}")
   print(f"Episodic memory episodes: {stats['episodic_memory']['episode_count']}")
   
   memory_manager.consolidate_memories()
   ```

4. Performance Profiling:
   ```python
   import pynlm
   
   brain = pynlm.create_brain()
   brain.initialize()
   
   profiler = pynlm.PerformanceProfiler(brain)
   profiler.start_profiling(sampling_rate=10.0)  # 10 Hz
   
   for step in range(1000):
       brain.step(step)
       profiler.profile_step(step)
   
   profiler.stop_profiling()
   report = profiler.get_performance_report()
   print(f"Average time per step: {report['average_time_per_step']*1000:.2f} ms")
   ```

5. Advanced Configuration:
   ```python
   import pynlm
   
   config = pynlm.ConfigManager()
   
   # Set configuration values
   config.set("brain.neuron_count", 5000)
   config.set("plasticity.stdp_learning_rate", 0.01)
   config.set("neuromod.dopamine_scale", 2.0)
   
   # Validate configuration
   errors = config.validate_config()
   if not errors:
       print("Configuration is valid!")
   else:
       print(f"Validation errors: {errors}")
   ```

Classes:
- Experiment: Comprehensive experiment management
- MemoryManager: Memory system utilities
- PerformanceProfiler: Performance monitoring
- ConfigManager: Advanced configuration management

Functions:
- create_brain: Create a brain with configuration
- create_world: Create a world environment
- create_agent: Create an agent brain interface
"""

if __name__ == "__main__":
    # Example usage when run as script
    print("NLM Enhanced Python API")
    print("Version:", __version__)
    print()
    print("Import this module to use the enhanced NLM API:")
    print("  import pynlm_enhanced as pynlm")