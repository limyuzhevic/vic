#!/usr/bin/env python3
"""
Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework

This module provides enhanced Python bindings with:
- Comprehensive error handling and validation
- High-level convenience methods for common operations
- Advanced features for power users
- Better documentation and type safety
- Integration with Python scientific ecosystem
"""

import sys
import logging
from typing import List, Dict, Optional, Tuple, Any, Union, Callable
import numpy as np
from pathlib import Path
from enum import Enum

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

# Try to import the C++ bindings
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    logger.warning(f"pynlm module not available: {e}")
    PYNLM_AVAILABLE = False

__version__ = "0.1.0"
__all__ = [
    'NLMError', 'ValidationError', 'ConfigurationError', 'SimulationError',
    'Brain', 'Config', 'AgentBrain', 'SimpleWorld', 'SensoryPercept',
    'Vision', 'Audio', 'InternalSignals', 'Action', 'WorldObject',
    'AgentBody', 'ActionResult', 'NeuromodulationLevel', 'DevelopmentalStage',
    'NeuronType', 'SynapseType', 'ActionType', 'MotorCommand', 'WorldObjectType',
    'SimulationResult', 'BrainStats', 'AgentSimulation', 'MemoryManager',
    'create_default_config', 'create_brain', 'create_world', 'create_agent',
    'run_simulation', 'benchmark_brain', 'visualize_brain', 'export_brain',
    'load_config_from_file', 'save_config_to_file'
]
class NLMError(Exception):
    """Base exception for NLM errors."""
    
    def __init__(self, message: str, error_code: Optional[str] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
    
    def __str__(self) -> str:
        if self.error_code:
            return f"[{self.error_code}] {self.message}"
        return self.message
class ValidationError(NLMError):
    """Exception raised when validation fails."""
    
    def __init__(self, message: str, field: Optional[str] = None):
        super().__init__(message, "VALIDATION_ERROR")
        self.field = field
class ConfigurationError(NLMError):
    """Exception raised when configuration is invalid."""
    
    def __init__(self, message: str, config_key: Optional[str] = None):
        super().__init__(message, "CONFIG_ERROR")
        self.config_key = config_key
class SimulationError(NLMError):
    """Exception raised when simulation fails."""
    
    def __init__(self, message: str, step: Optional[int] = None):
        super().__init__(message, "SIMULATION_ERROR")
        self.step = step
class NeuromodulationLevel(Enum):
    """Enumeration for neuromodulation levels."""
    
    LOW = "low"
    MEDIUM = "medium"
    HIGH = "high"
    CRITICAL = "critical"
class DevelopmentalStage(Enum):
    """Enumeration for developmental stages."""
    
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"
class SimulationResult:
    """Container for simulation results with analysis tools."""
    
    def __init__(self, brain: 'Brain', world: Optional['SimpleWorld'] = None, 
                 agent: Optional['AgentBrain'] = None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.step_count = 0
        self.spike_history: List[float] = []
        self.firing_rate_history: List[float] = []
        self.memory_usage: Dict[str, float] = {}
        self.plasticity_history: List[Dict[str, float]] = []
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get comprehensive statistics about the simulation."""
        stats = {
            'neurons': self.brain.get_total_neuron_count(),
            'synapses': self.brain.get_total_synapse_count(),
            'active_neurons': self.brain.get_active_neuron_count(),
            'firing_neurons': self.brain.get_firing_neuron_count(),
            'average_firing_rate': self.brain.get_average_firing_rate(),
            'excitatory_inhibition_ratio': self.brain.get_excitation_inhibition_ratio(),
            'total_spikes': self.brain.get_total_spike_count(),
            'developmental_stage': self.brain.get_developmental_stage(),
            'steps': self.step_count,
            'memory_usage': self.memory_usage.copy()
        }
        return stats
    
    def analyze_plasticity(self) -> Dict[str, float]:
        """Analyze plasticity patterns from the simulation."""
        if not self.plasticity_history:
            return {'plasticity_effectiveness': 0.0, 'learning_rate': 0.0}
        
        learning_rates = [h.get('learning_rate', 0.0) for h in self.plasticity_history]
        effectiveness_scores = [h.get('effectiveness', 0.0) for h in self.plasticity_history]
        
        return {
            'plasticity_effectiveness': np.mean(effectiveness_scores) if effectiveness_scores else 0.0,
            'learning_rate': np.mean(learning_rates) if learning_rates else 0.0,
            'plasticity_variance': np.var(learning_rates) if len(learning_rates) > 1 else 0.0,
            'adaptation_rate': (learning_rates[-1] - learning_rates[0]) / len(learning_rates) if learning_rates else 0.0
        }
    
    def export_results(self, filepath: str) -> None:
        """Export simulation results to a file."""
        try:
            results = {
                'brain_stats': self.get_statistics(),
                'plasticity_analysis': self.analyze_plasticity(),
                'step_count': self.step_count,
                'spike_history': self.spike_history,
                'firing_rate_history': self.firing_rate_history,
                'memory_usage': self.memory_usage
            }
            
            import json
            with open(filepath, 'w') as f:
                json.dump(results, f, indent=2)
            
            logger.info(f"Results exported to {filepath}")
        except Exception as e:
            raise NLMError(f"Failed to export results: {e}", "EXPORT_ERROR")
class BrainStats:
    """Enhanced brain statistics with analysis capabilities."""
    
    def __init__(self, brain: 'Brain'):
        self.brain = brain
    
    def get_comprehensive_stats(self) -> Dict[str, Any]:
        """Get comprehensive statistics about the brain."""
        stats = {
            'basic': {
                'neuron_count': self.brain.get_total_neuron_count(),
                'synapse_count': self.brain.get_total_synapse_count(),
                'region_count': self.brain.get_region_count(),
                'population_count': self.brain.get_population_count()
            },
            'activity': {
                'active_neurons': self.brain.get_active_neuron_count(),
                'firing_neurons': self.brain.get_firing_neuron_count(),
                'average_firing_rate': self.brain.get_average_firing_rate(),
                'total_spikes': self.brain.get_total_spike_count(),
                'excitation_inhibition_ratio': self.brain.get_excitation_inhibition_ratio()
            },
            'development': {
                'developmental_stage': self.brain.get_developmental_stage(),
                'region_stats': self._get_region_statistics()
            },
            'performance': self._get_performance_metrics()
        }
        return stats
    
    def _get_region_statistics(self) -> List[Dict[str, Any]]:
        """Get statistics for each brain region."""
        regions = self.brain.get_regions()
        region_stats = []
        
        for region in regions:
            region_info = {
                'id': region.get_id(),
                'name': region.get_name(),
                'neuron_count': region.get_neuron_count(),
                'synapse_count': region.get_synapse_count(),
                'excitatory_ratio': region.get_excitatory_ratio(),
                'inhibitory_ratio': region.get_inhibitory_ratio(),
                'spiking_activity': region.get_spiking_activity()
            }
            region_stats.append(region_info)
        
        return region_stats
    
    def _get_performance_metrics(self) -> Dict[str, Any]:
        """Get performance metrics for the brain."""
        return {
            'spike_efficiency': self._calculate_spike_efficiency(),
            'connectivity_strength': self._calculate_connectivity_strength(),
            'stability': self._calculate_stability()
        }
    
    def _calculate_spike_efficiency(self) -> float:
        """Calculate efficiency of spike generation."""
        total_neurons = self.brain.get_total_neuron_count()
        if total_neurons == 0:
            return 0.0
        
        total_spikes = self.brain.get_total_spike_count()
        return total_spikes / total_neurons
    
    def _calculate_connectivity_strength(self) -> float:
        """Calculate overall connectivity strength."""
        regions = self.brain.get_regions()
        if not regions:
            return 0.0
        
        total_weight = 0.0
        total_synapses = 0
        
        for region in regions:
            synapses = region.get_synapses()
            for synapse in synapses:
                total_weight += abs(synapse.get_weight())
                total_synapses += 1
        
        if total_synapses == 0:
            return 0.0
        
        return total_weight / total_synapses
    
    def _calculate_stability(self) -> float:
        """Calculate brain stability (simplified metric)."""
        # This is a simplified stability metric based on firing rate variance
        recent_spikes = self.brain.get_total_spike_count() - getattr(self, '_previous_spike_count', 0)
        self._previous_spike_count = self.brain.get_total_spike_count()
        
        # Lower spike rate change indicates higher stability
        if recent_spikes == 0:
            return 1.0
        elif recent_spikes > 1000:
            return 0.1  # Very unstable
        else:
            return 1.0 / (1.0 + recent_spikes / 100.0)
class AgentSimulation:
    """High-level agent simulation with advanced features."""
    
    def __init__(self, brain: 'Brain', world: Optional['SimpleWorld'] = None):
        if not PYNLM_AVAILABLE:
            raise NLMError("pynlm module is not available")
        
        self.brain = brain
        self.world = world
        self.agent: Optional['AgentBrain'] = None
        self.config = None
        
        if world:
            self.agent = pynlm.createAgentBrain(brain)
            self.agent.initialize(world)
    
    def run_episode(self, num_steps: int, enable_learning: bool = True,
                   enable_curiosity: bool = False, enable_development: bool = False) -> SimulationResult:
        """Run a complete simulation episode."""
        if num_steps <= 0:
            raise ValidationError("Number of steps must be positive", "step_count")
        
        result = SimulationResult(self.brain, self.world, self.agent)
        
        # Configure agent features
        if enable_learning:
            if self.agent:
                self.agent.enable_reward_modulation(True)
        
        if enable_curiosity:
            if self.agent:
                self.agent.enable_curiosity(True)
        
        if enable_development:
            if self.agent:
                self.agent.enable_development(True)
        
        # Run simulation
        for step in range(num_steps):
            try:
                if self.world:
                    self.world.update(0.1)
                    
                    if self.agent:
                        percept = self.world.get_sensory_percept()
                        self.agent.process_sensory_input(percept)
                
                self.brain.step(step)
                
                if self.agent:
                    action = self.agent.decode_motor_command()
                    if self.world:
                        self.world.apply_motor_command(action, self.world.get_simulation_time())
                
                # Update result tracking
                result.step_count = step + 1
                result.spike_history.append(self.brain.get_total_spike_count())
                result.firing_rate_history.append(self.brain.get_average_firing_rate())
                
                # Log progress
                if (step + 1) % 100 == 0:
                    logger.info(f"Step {step + 1}/{num_steps}: "
                              f"Spikes: {self.brain.get_total_spike_count()}, "
                              f"Firing: {self.brain.get_firing_neuron_count()} neurons")
                
            except Exception as e:
                raise SimulationError(f"Simulation failed at step {step}: {e}", step)
        
        return result
    
    def run_continuous(self, duration: float, timestep: float = 0.1,
                      callbacks: Optional[Dict[str, Callable]] = None) -> SimulationResult:
        """Run a continuous simulation for a given duration."""
        if duration <= 0 or timestep <= 0:
            raise ValidationError("Duration and timestep must be positive", "time_params")
        
        num_steps = int(duration / timestep)
        result = self.run_episode(num_steps)
        
        # Execute callbacks
        if callbacks:
            for event, callback in callbacks.items():
                try:
                    callback(result)
                except Exception as e:
                    logger.warning(f"Callback {event} failed: {e}")
        
        return result
class MemoryManager:
    """Manager for brain memory and checkpoint operations."""
    
    def __init__(self, brain: 'Brain'):
        self.brain = brain
    
    def save_checkpoint(self, filepath: str, metadata: Optional[Dict[str, Any]] = None) -> bool:
        """Save brain state as a checkpoint."""
        try:
            if metadata is None:
                metadata = {}
            
            # Add timestamp
            import datetime
            metadata['timestamp'] = datetime.datetime.now().isoformat()
            metadata['neuron_count'] = self.brain.get_total_neuron_count()
            metadata['spike_count'] = self.brain.get_total_spike_count()
            
            self.brain.save(filepath)
            
            logger.info(f"Checkpoint saved to {filepath}")
            return True
        except Exception as e:
            raise NLMError(f"Failed to save checkpoint: {e}", "CHECKPOINT_ERROR")
    
    def load_checkpoint(self, filepath: str) -> bool:
        """Load brain state from a checkpoint."""
        try:
            self.brain.load(filepath)
            logger.info(f"Checkpoint loaded from {filepath}")
            return True
        except Exception as e:
            raise NLMError(f"Failed to load checkpoint: {e}", "CHECKPOINT_ERROR")
    
    def get_memory_stats(self) -> Dict[str, Any]:
        """Get memory usage statistics."""
        return {
            'neuron_count': self.brain.get_total_neuron_count(),
            'synapse_count': self.brain.get_total_synapse_count(),
            'region_count': self.brain.get_region_count(),
            'population_count': self.brain.get_population_count(),
            'estimated_memory_usage': self._estimate_memory_usage()
        }
    
    def _estimate_memory_usage(self) -> float:
        """Estimate memory usage in MB."""
        # Very rough estimate - in practice, this would depend on many factors
        neuron_count = self.brain.get_total_neuron_count()
        synapse_count = self.brain.get_total_synapse_count()
        
        # Neurons: ~100 bytes each (simplified)
        # Synapses: ~200 bytes each (simplified)
        return (neuron_count * 100 + synapse_count * 200) / (1024 * 1024)
def create_default_config() -> 'Config':
    """Create a default configuration for NLM."""
    if not PYNLM_AVAILABLE:
        raise NLMError("pynlm module is not available")
    
    try:
        config = pynlm.create_default_config()
        return Config(config)
    except Exception as e:
        raise NLMError(f"Failed to create default config: {e}", "CONFIG_ERROR")
def create_brain(config: Optional['Config'] = None) -> 'Brain':
    """Create a new NLM brain."""
    if not PYNLM_AVAILABLE:
        raise NLMError("pynlm module is not available")
    
    try:
        if config is None:
            config = create_default_config()
        
        brain = pynlm.create_brain(config.get_config())
        return Brain(brain)
    except Exception as e:
        raise NLMError(f"Failed to create brain: {e}", "BRAIN_ERROR")
def create_world() -> 'SimpleWorld':
    """Create a new NLM world."""
    if not PYNLM_AVAILABLE:
        raise NLMError("pynlm module is not available")
    
    try:
        world = pynlm.create_simple_world()
        return SimpleWorld(world)
    except Exception as e:
        raise NLMError(f"Failed to create world: {e}", "WORLD_ERROR")
def create_agent(brain: 'Brain') -> 'AgentBrain':
    """Create an agent brain interface."""
    if not PYNLM_AVAILABLE:
        raise NLMError("pynlm module is not available")
    
    try:
        agent = pynlm.create_agent_brain(brain.get_brain())
        return AgentBrain(agent)
    except Exception as e:
        raise NLMError(f"Failed to create agent: {e}", "AGENT_ERROR")
def run_simulation(brain: 'Brain', world: 'SimpleWorld', agent: 'AgentBrain', 
                  num_steps: int, enable_learning: bool = True) -> SimulationResult:
    """Run a complete simulation episode."""
    simulation = AgentSimulation(brain, world)
    simulation.agent = agent
    return simulation.run_episode(num_steps, enable_learning)
def benchmark_brain(brain: 'Brain', num_runs: int = 10, duration: float = 1.0) -> Dict[str, Any]:
    """Run benchmarks on a brain."""
    if num_runs <= 0 or duration <= 0:
        raise ValidationError("Number of runs and duration must be positive", "benchmark_params")
    
    results = []
    
    for run in range(num_runs):
        brain.reset()
        brain.initialize()
        
        start_time = import time
        timestep = duration / 1000  # Assume 1000Hz timestep
        
        for step in range(1000):
            brain.step(step)
        
        end_time = time.time()
        duration_ms = (end_time - start_time) * 1000
        
        stats = {
            'run': run + 1,
            'duration_ms': duration_ms,
            'neuron_count': brain.get_total_neuron_count(),
            'synapse_count': brain.get_total_synapse_count(),
            'total_spikes': brain.get_total_spike_count(),
            'firing_rate': brain.get_average_firing_rate(),
            'efficiency': brain.get_total_spike_count() / brain.get_total_neuron_count() if brain.get_total_neuron_count() > 0 else 0
        }
        
        results.append(stats)
    
    # Calculate averages
    avg_duration = np.mean([r['duration_ms'] for r in results])
    avg_spikes = np.mean([r['total_spikes'] for r in results])
    avg_efficiency = np.mean([r['efficiency'] for r in results])
    
    return {
        'average_duration_ms': avg_duration,
        'average_total_spikes': avg_spikes,
        'average_efficiency': avg_efficiency,
        'all_results': results
    }
def load_config_from_file(filepath: str) -> 'Config':
    """Load configuration from a file."""
    if not PYNLM_AVAILABLE:
        raise NLMError("pynlm module is not available")
    
    try:
        config = pynlm.create_default_config()
        config.load_from_file(filepath)
        return Config(config)
    except Exception as e:
        raise NLMError(f"Failed to load config from file: {e}", "CONFIG_ERROR")
def save_config_to_file(config: 'Config', filepath: str) -> None:
    """Save configuration to a file."""
    if not PYNLM_AVAILABLE:
        raise NLMError("pynlm module is not available")
    
    try:
        config.save_to_file(filepath)
    except Exception as e:
        raise NLMError(f"Failed to save config to file: {e}", "CONFIG_ERROR")
# Import time at module level for benchmarking
import time

# Backward compatibility functions
def _create_default_config():
    """Backward compatibility function."""
    return create_default_config()
def _create_brain(config=None):
    """Backward compatibility function."""
    return create_brain(config)
def _create_world():
    """Backward compatibility function."""
    return create_world()
def _create_agent(brain):
    """Backward compatibility function."""
    return create_agent(brain)
def _run_simulation(brain, world, agent, num_steps, enable_learning=True):
    """Backward compatibility function."""
    return run_simulation(brain, world, agent, num_steps, enable_learning)
# Initialize module
logger.info(f"NLM Python bindings initialized (v{__version__})")
if PYNLM_AVAILABLE:
    logger.info("C++ pynlm module is available")
else:
    logger.warning("C++ pynlm module is not available - functionality will be limited")
