# pynlm - Improved Python Bindings for NLM
# High-performance neural simulation with comprehensive API

import numpy as np
from typing import Optional, Dict, Any, List, Union, Callable
from dataclasses import dataclass, field
import json
import time
import threading
from enum import Enum

# Import the low-level C++ bindings
import _pynlm

class LogLevel(Enum):
    """Log levels for NLM logging"""
    DEBUG = _pynlm.LogLevel.DEBUG
    INFO = _pynlm.LogLevel.INFO
    WARNING = _pynlm.LogLevel.WARNING
    ERROR = _pynlm.LogLevel.ERROR
    CRITICAL = _pynlm.LogLevel.CRITICAL

class NeuronType(Enum):
    """Neuron types"""
    EXCITATORY = _pynlm.NeuronType.Excitatory
    INHIBITORY = _pynlm.NeuronType.Inhibitory
    MODULATORY = _pynlm.NeuronType.Modulatory
    SENSORY = _pynlm.NeuronType.Sensory
    MOTOR = _pynlm.NeuronType.Motor
    INTERNAL = _pynlm.NeuronType.Internal

class DevelopmentalStage(Enum):
    """Developmental stages"""
    INITIAL = _pynlm.DevelopmentalStage.Initial
    CRITICAL_PERIOD = _pynlm.DevelopmentalStage.CriticalPeriod
    MATURATION = _pynlm.DevelopmentalStage.Maturation
    ADULT = _pynlm.DevelopmentalStage.Adult
    AGING = _pynlm.DevelopmentalStage.Aging

class ActionType(Enum):
    """Action types"""
    MOVE_FORWARD = _pynlm.ActionType.MoveForward
    TURN_LEFT = _pynlm.ActionType.TurnLeft
    TURN_RIGHT = _pynlm.ActionType.TurnRight
    INTERACT = _pynlm.ActionType.Interact
    EAT = _pynlm.ActionType.Eat

@dataclass
class SimulationStats:
    """Simulation statistics"""
    total_spikes: int = 0
    firing_neurons: int = 0
    average_firing_rate: float = 0.0
    excitation_inhibition_ratio: float = 0.0
    memory_usage_mb: float = 0.0
    simulation_time_ms: float = 0.0
    step_count: int = 0

@dataclass
class PerformanceMetrics:
    """Performance monitoring metrics"""
    sim_time: float = 0.0
    step_time_avg: float = 0.0
    step_time_std: float = 0.0
    neurons_per_second: float = 0.0
    memory_peak_mb: float = 0.0
    gc_collections: int = 0

class NLMConfig:
    """Configuration management for NLM simulations"""
    
    def __init__(self, config_dict: Optional[Dict[str, Any]] = None):
        """Initialize configuration with optional dict"""
        self._config = config_dict or {}
        self._apply_defaults()
    
    def _apply_defaults(self):
        """Apply default configuration values"""
        defaults = {
            "brain": {
                "neuron_count": 1000,
                "region_count": 2,
                "connection_probability": 0.1,
                "initial_weight_mean": 0.5,
                "initial_weight_std": 0.1,
                "v_thresh": -50.0,
                "v_rest": -70.0,
                "v_reset": -75.0,
                "tau_mem": 20.0,
                "tau_ref": 2.0
            },
            "plasticity": {
                "stdp": {"enable": True, "learning_rate": 0.001, "tau_plus": 20.0, "tau_minus": 20.0},
                "hebbian": {"enable": True},
                "structural": {"enable": True}
            },
            "neuromod": {
                "dopamine": {"scale": 1.0},
                "curiosity": {"enable": True},
                "novelty": {"enable": True}
            },
            "simulation": {
                "timestep": 0.001,
                "max_steps": 10000,
                "time_limit": 0.0
            },
            "development": {
                "synaptogenesis_rate": 0.0001,
                "pruning_rate": 0.00001
            }
        }
        
        # Merge defaults with provided config
        self._merge_nested(self._config, defaults)
    
    def _merge_nested(self, target: Dict, source: Dict):
        """Merge nested dictionaries"""
        for key, value in source.items():
            if key in target and isinstance(target[key], dict) and isinstance(value, dict):
                self._merge_nested(target[key], value)
            else:
                target[key] = value
    
    def get(self, key_path: str, default=None) -> Any:
        """Get configuration value using dot notation"""
        keys = key_path.split('.')
        value = self._config
        for key in keys:
            if isinstance(value, dict) and key in value:
                value = value[key]
            else:
                return default
        return value
    
    def set(self, key_path: str, value: Any):
        """Set configuration value using dot notation"""
        keys = key_path.split('.')
        config = self._config
        for i, key in enumerate(keys[:-1]):
            if key not in config:
                config[key] = {}
            config = config[key]
        config[keys[-1]] = value
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary"""
        return self._config.copy()
    
    def save(self, filepath: str):
        """Save configuration to JSON file"""
        with open(filepath, 'w') as f:
            json.dump(self._config, f, indent=2)
    
    @classmethod
    def load(cls, filepath: str) -> 'NLMConfig':
        """Load configuration from JSON file"""
        with open(filepath, 'r') as f:
            config_dict = json.load(f)
        return cls(config_dict)

class PerformanceProfiler:
    """Performance monitoring and profiling for NLM simulations"""
    
    def __init__(self):
        self.metrics_history: List[PerformanceMetrics] = []
        self.step_times: List[float] = []
        self._start_time = 0.0
        self._step_count = 0
    
    def start_step(self):
        """Start timing a simulation step"""
        self._start_time = time.time()
        self._step_count += 1
    
    def end_step(self):
        """End timing a simulation step and record metrics"""
        step_time = time.time() - self._start_time
        self.step_times.append(step_time)
        
        # Calculate running statistics
        if len(self.step_times) > 1:
            avg = np.mean(self.step_times)
            std = np.std(self.step_times)
        else:
            avg = step_time
            std = 0.0
        
        # Update metrics
        if self.metrics_history:
            last_metrics = self.metrics_history[-1]
            new_metrics = PerformanceMetrics(
                sim_time=last_metrics.sim_time + step_time,
                step_time_avg=avg,
                step_time_std=std,
                neurons_per_second=last_metrics.neurons_per_second,
                memory_peak_mb=last_metrics.memory_peak_mb,
                gc_collections=last_metrics.gc_collections
            )
        else:
            new_metrics = PerformanceMetrics(
                sim_time=step_time,
                step_time_avg=step_time,
                step_time_std=0.0,
                neurons_per_second=0.0,
                memory_peak_mb=0.0,
                gc_collections=0
            )
        
        self.metrics_history.append(new_metrics)
    
    def get_current_metrics(self) -> PerformanceMetrics:
        """Get current performance metrics"""
        if not self.metrics_history:
            return PerformanceMetrics()
        return self.metrics_history[-1]
    
    def get_summary(self) -> Dict[str, Any]:
        """Get performance summary"""
        if not self.metrics_history:
            return {}
        
        total_sim_time = sum(m.sim_time for m in self.metrics_history)
        avg_step_time = np.mean(self.step_times) if self.step_times else 0.0
        std_step_time = np.std(self.step_times) if len(self.step_times) > 1 else 0.0
        
        return {
            "total_simulation_time": total_sim_time,
            "average_step_time_ms": avg_step_time * 1000,
            "step_time_std_ms": std_step_time * 1000,
            "steps_per_second": len(self.step_times) / total_sim_time if total_sim_time > 0 else 0,
            "total_steps": len(self.step_times)
        }
    
    def reset(self):
        """Reset performance profiler"""
        self.metrics_history.clear()
        self.step_times.clear()
        self._step_count = 0

class NLMBrain:
    """High-performance neural brain simulation with Pythonic API"""
    
    def __init__(self, config: Optional[NLMConfig] = None):
        """Initialize brain with optional configuration"""
        self._config = config or NLMConfig()
        self._profiler = PerformanceProfiler()
        
        # Create brain using low-level bindings
        self._brain = _pynlm.createBrain(self._config.to_dict())
        self._brain.initialize()
        
        # Store metadata
        self._creation_time = time.time()
        self._last_step_time = 0.0
        self._total_steps = 0
    
    def step(self, step_number: int = -1) -> bool:
        """Advance brain by one simulation step"""
        self._profiler.start_step()
        
        if step_number >= 0:
            self._brain.step(step_number)
        else:
            self._brain.step(self._total_steps)
        
        self._profiler.end_step()
        self._total_steps += 1
        self._last_step_time = time.time() - self._creation_time
        
        return True
    
    def reset(self):
        """Reset brain to initial state"""
        self._brain.reset()
        self._profiler.reset()
        self._total_steps = 0
        self._last_step_time = 0.0
    
    def get_stats(self) -> SimulationStats:
        """Get current simulation statistics"""
        return SimulationStats(
            total_spikes=self._brain.getTotalSpikeCount(),
            firing_neurons=self._brain.getFiringNeuronCount(),
            average_firing_rate=self._brain.getAverageFiringRate(),
            excitation_inhibition_ratio=self._brain.getExcitationInhibitionRatio(),
            memory_usage_mb=0.0,  # Would require platform-specific implementation
            simulation_time_ms=self._last_step_time * 1000,
            step_count=self._total_steps
        )
    
    def get_performance(self) -> PerformanceMetrics:
        """Get performance metrics"""
        return self._profiler.get_current_metrics()
    
    def enable_plasticity(self, hebbian: bool = True, stdp: bool = True, 
                          reward_modulated: bool = False, structural: bool = False):
        """Enable plasticity mechanisms"""
        if hasattr(self._brain, 'enablePlasticity'):
            self._brain.enablePlasticity(hebbian, stdp, reward_modulated)
    
    def enable_developmental_changes(self, enabled: bool = True):
        """Enable developmental changes"""
        if hasattr(self._brain, 'enableDevelopment'):
            self._brain.enableDevelopment(enabled)
    
    def enable_curiosity(self, enabled: bool = True):
        """Enable curiosity-driven exploration"""
        if hasattr(self._brain, 'enableCuriosity'):
            self._brain.enableCuriosity(enabled)
    
    def save(self, filepath: str):
        """Save brain state to file"""
        return self._brain.save(filepath)
    
    def load(self, filepath: str) -> bool:
        """Load brain state from file"""
        result = self._brain.load(filepath)
        if result:
            self._total_steps = 0
        return result
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.reset()

class SimpleWorld:
    """Simple 2D world simulation for agent interaction"""
    
    def __init__(self, width: int = 20, height: int = 20, 
                 vision_width: int = 8, vision_height: int = 8):
        """Initialize world with dimensions"""
        self._world = _pynlm.createSimpleWorld()
        self._world.configure(width, height, vision_width, vision_height)
        self._world.reset()
    
    def update(self, timestep: float):
        """Update world state"""
        self._world.update(timestep)
    
    def get_percept(self) -> Any:
        """Get sensory percept from world"""
        return self._world.getSensoryPercept()
    
    def get_agent_body(self) -> Any:
        """Get agent body information"""
        return self._world.getAgentBody()
    
    def apply_action(self, action: Any, current_time: float):
        """Apply agent action to world"""
        self._world.applyMotorCommand(action, current_time)
    
    def reset(self):
        """Reset world to initial state"""
        self._world.reset()

class AgentBrain:
    """Agent brain interface for sensory-motor processing"""
    
    def __init__(self, brain: NLMBrain, world: SimpleWorld):
        """Initialize agent with brain and world"""
        self._brain = brain
        self._world = world
        
        # Create agent using low-level bindings
        self._agent = _pynlm.createAgentBrain(brain._brain)
        self._agent.initialize(world._world)
    
    def process_sensory_input(self, percept: Any):
        """Process sensory input from world"""
        self._agent.processSensoryInput(percept)
    
    def decode_motor_command(self) -> Any:
        """Decode motor command from brain"""
        return self._agent.decodeMotorCommand()
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float):
        """Apply reward modulation to brain"""
        self._agent.applyRewardModulation(reward, predicted_reward)
    
    def update_development(self, timestep: float):
        """Update developmental state"""
        self._agent.updateDevelopment(timestep)
    
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage"""
        if hasattr(self._brain._brain, 'getDevelopmentalStage'):
            stage = self._brain._brain.getDevelopmentalStage()
            return DevelopmentalStage(stage)
        return DevelopmentalStage.ADULT
    
    def enable_subsystems(self, **kwargs):
        """Enable various subsystems"""
        for subsystem, enabled in kwargs.items():
            if hasattr(self._agent, f'enable{subsystem.title()}'):
                getattr(self._agent, f'enable{subsystem.title()}')(enabled)
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level"""
        return self._agent.getCuriosityLevel() if hasattr(self._agent, 'getCuriosityLevel') else 0.0
    
    def get_novelty_level(self) -> float:
        """Get novelty level"""
        return self._agent.getNoveltyLevel() if hasattr(self._agent, 'getNoveltyLevel') else 0.0
    
    def get_prediction_error(self) -> float:
        """Get prediction error"""
        return self._agent.getPredictionError() if hasattr(self._agent, 'getPredictionError') else 0.0

# Global configuration management
_global_config = NLMConfig()

# Create default configuration
_default_config = NLMConfig()

# Convenience functions

def create_brain(config: Optional[NLMConfig] = None) -> NLMBrain:
    """Create a new brain with optional configuration"""
    return NLMBrain(config)

def create_default_config() -> NLMConfig:
    """Create default configuration"""
    return NLMConfig()

def set_log_level(level: LogLevel):
    """Set global log level"""
    _global_config.set("log.level", level.value)
    if hasattr(_pynlm, 'setLogLevel'):
        _pynlm.setLogLevel(level.value)

# Performance monitoring functions
_performance_profiler = PerformanceProfiler()

def get_performance_stats() -> Dict[str, Any]:
    """Get global performance statistics"""
    return _performance_profiler.get_summary()

def reset_performance_stats():
    """Reset performance statistics"""
    _performance_profiler.reset()

# Simulation utilities

def run_simulation(brain: NLMBrain, world: Optional[SimpleWorld] = None, 
                   agent: Optional[AgentBrain] = None, 
                   num_steps: int = 1000, verbose: bool = False) -> Dict[str, Any]:
    """Run complete simulation with optional agent in world"""
    start_time = time.time()
    
    stats_history = []
    
    for step in range(num_steps):
        # Update world if present
        if world:
            world.update(0.1)
            
            # Get percept and process if agent present
            if agent:
                percept = world.get_percept()
                agent.process_sensory_input(percept)
                
                # Apply brain step
                brain.step(step)
                
                # Get action and apply to world
                action = agent.decode_motor_command()
                world.apply_action(action, world._world.getSimulationTime())
        else:
            # Just run brain
            brain.step(step)
        
        # Collect statistics periodically
        if step % 100 == 0:
            stats = brain.get_stats()
            stats.step_count = step
            stats_history.append(stats)
            
            if verbose:
                print(f"Step {step}: {stats.firing_neurons} firing, "
                      f"{stats.total_spikes} total spikes, "
                      f"{stats.average_firing_rate:.2f} Hz firing rate")
    
    total_time = time.time() - start_time
    
    # Return final results
    final_stats = brain.get_stats()
    final_stats.simulation_time_ms = total_time * 1000
    
    return {
        "final_stats": final_stats,
        "stats_history": stats_history,
        "total_time": total_time,
        "steps_per_second": num_steps / total_time if total_time > 0 else 0
    }

# Advanced utilities for power users

def create_advanced_brain(neuron_count: int = 5000, regions: int = 5, 
                          connectivity: float = 0.15, enable_all: bool = True) -> NLMBrain:
    """Create advanced brain with optimized configuration"""
    config = NLMConfig()
    
    # Configure brain parameters
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", regions)
    config.set("brain.connection_probability", connectivity)
    
    # Enable all plasticity mechanisms
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Enable neuromodulation
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    
    # Set developmental parameters
    config.set("development.synaptogenesis_rate", 0.0002)
    config.set("development.pruning_rate", 0.00002)
    
    return NLMBrain(config)

def create_reduced_brain(neuron_count: int = 100, regions: int = 1) -> NLMBrain:
    """Create reduced brain for rapid testing and prototyping"""
    config = NLMConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", regions)
    config.set("simulation.timestep", 0.01)  # Larger timestep for speed
    return NLMBrain(config)

# Batch processing utilities

def run_multiple_simulations(num_simulations: int, brain_factory: Callable[[], NLMBrain],
                            steps_per_simulation: int = 500, parallel: bool = False) -> List[Dict[str, Any]]:
    """Run multiple simulations in batch"""
    results = []
    
    if parallel:
        # TODO: Implement parallel execution
        import concurrent.futures
        with concurrent.futures.ThreadPoolExecutor() as executor:
            futures = []
            for i in range(num_simulations):
                future = executor.submit(run_single_simulation, brain_factory, steps_per_simulation)
                futures.append(future)
            
            for future in concurrent.futures.as_completed(futures):
                results.append(future.result())
    else:
        for i in range(num_simulations):
            result = run_single_simulation(brain_factory, steps_per_simulation)
            result['simulation_id'] = i
            results.append(result)
    
    return results

def run_single_simulation(brain_factory: Callable[[], NLMBrain], steps_per_simulation: int) -> Dict[str, Any]:
    """Run single simulation (internal helper)"""
    brain = brain_factory()
    return run_simulation(brain, num_steps=steps_per_simulation)

# Memory management utilities

def cleanup_memory():
    """Perform memory cleanup (platform-specific)"""
    # In a real implementation, this would include:
    # - Garbage collection hints
    # - Memory pool cleanup
    # - Cache invalidation
    pass

# Export key classes and functions
__all__ = [
    # Core classes
    'NLMConfig', 'NLMBrain', 'SimpleWorld', 'AgentBrain',
    'PerformanceProfiler', 'PerformanceMetrics', 'SimulationStats',
    
    # Enums
    'LogLevel', 'NeuronType', 'DevelopmentalStage', 'ActionType',
    
    # Convenience functions
    'create_brain', 'create_default_config', 'set_log_level',
    'get_performance_stats', 'reset_performance_stats',
    'run_simulation', 'create_advanced_brain', 'create_reduced_brain',
    'run_multiple_simulations', 'cleanup_memory'
]

# Version and metadata
__version__ = "2.0.0"
__author__ = "NLM Team"
__description__ = "Improved Python bindings for NLM with enhanced API design"

print(f"NLM Python bindings v{__version__} loaded successfully")
