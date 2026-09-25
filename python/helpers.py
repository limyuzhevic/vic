# NLM Python Helper Functions

This module provides convenience functions and utilities for working with the NLM framework.

Key features:
- Configuration factory functions for common use cases
- Simulation helpers for step execution and reset operations
- Serialization utilities for saving and loading brain states
- Statistical analysis helpers for monitoring simulation performance
- Debugging and monitoring utilities
- Parameter validation and safety checks
"""

import numpy as np
from typing import Dict, List, Optional, Union, Any, Callable
from dataclasses import dataclass, field
from enum import Enum
import json
import time
import logging

# Import NLM bindings
import pynlm as nlm

logger = logging.getLogger(__name__)

@dataclass
class SimulationStats:
    """Statistical data collected during simulation."""
    steps_completed: int = 0
    total_neurons: int = 0
    active_neurons: int = 0
    total_spikes: int = 0
    firing_rate: float = 0.0
    mean_synaptic_activity: float = 0.0
    simulation_time: float = 0.0
    memory_usage: Dict[str, float] = field(default_factory=dict)
    neuromodulation_levels: Dict[str, float] = field(default_factory=dict)
    learning_rate: float = 0.0
    convergence_score: float = 0.0

class SimulationStatus(Enum):
    """Status of simulation run."""
    IDLE = "idle"
    RUNNING = "running"
    PAUSED = "paused"
    COMPLETED = "completed"
    ERROR = "error"
    TERMINATED = "terminated"

def createStandardConfig(
    neuron_count: int = 1000,
    synapse_density: float = 0.1,
    world_dimensions: tuple = (100.0, 100.0),
    learning_rate: float = 0.01,
    time_step: float = 0.1,
    max_neurons_per_region: Optional[int] = None,
    enable_structural_plasticity: bool = True,
    enable_reward_modulation: bool = True,
    enable_development: bool = True,
    enable_curiosity: bool = True,
    enable_prediction_error: bool = True
) -> nlm.Config:
    """Create a standard NLM configuration with typical parameters.
    
    Args:
        neuron_count: Total number of neurons to create
        synapse_density: Proportion of possible connections that are synapses
        world_dimensions: (width, height) of the simulation world
        learning_rate: Learning rate for synaptic plasticity
        time_step: Simulation time step size
        max_neurons_per_region: Maximum neurons per brain region
        enable_structural_plasticity: Enable structural plasticity mechanisms
        enable_reward_modulation: Enable reward-based modulation
        enable_development: Enable developmental processes
        enable_curiosity: Enable curiosity-driven learning
        enable_prediction_error: Enable prediction error learning
        
    Returns:
        Configured Config object
        
    Raises:
        NLMError: If invalid parameters are provided
    """
    from pynlm import Config, NLMError
    
    config = nlm.Config()
    
    # Validate parameters
    if neuron_count <= 0:
        raise nlm.NLMError(f"Neuron count must be positive, got {neuron_count}")
    if not (0.0 < synapse_density <= 1.0):
        raise nlm.NLMError(f"Synapse density must be between 0 and 1, got {synapse_density}")
    if world_dimensions[0] <= 0 or world_dimensions[1] <= 0:
        raise nlm.NLMError(f"World dimensions must be positive, got {world_dimensions}")
    if not (0.0 < learning_rate <= 1.0):
        raise nlm.NLMError(f"Learning rate must be between 0 and 1, got {learning_rate}")
    if not (0.0 < time_step <= 1.0):
        raise nlm.NLMError(f"Time step must be between 0 and 1, got {time_step}")
    
    # Set configuration parameters
    config.setNumNeurons(neuron_count)
    config.setSynapseDensity(synapse_density)
    config.setLearningRate(learning_rate)
    config.setTimeStep(time_step)
    config.setWorldWidth(world_dimensions[0])
    config.setWorldHeight(world_dimensions[1])
    
    if max_neurons_per_region is not None:
        config.setMaxNeuronsPerRegion(max_neurons_per_region)
    
    # Set feature flags
    config.setEnableStructuralPlasticity(enable_structural_plasticity)
    config.setEnableRewardModulation(enable_reward_modulation)
    config.setEnableDevelopment(enable_development)
    config.setEnableCuriosity(enable_curiosity)
    config.setEnablePredictionError(enable_prediction_error)
    
    logger.info(f"Created standard configuration with {neuron_count} neurons")
    return config

def runCompleteSimulation(
    steps: int,
    config: Optional[nlm.Config] = None,
    world: Optional[nlm.SimpleWorld] = None,
    callbacks: Optional[Dict[str, Callable]] = None,
    enable_learning: bool = True,
    validate_at_each_step: bool = True
) -> Dict[str, Any]:
    """Run a complete simulation with comprehensive error handling and monitoring.
    
    Args:
        steps: Number of simulation steps to run
        config: NLM configuration (will create default if None)
        world: World instance (will create default if None)
        callbacks: Dictionary of callback functions for different events
        enable_learning: Enable or disable learning mechanisms
        validate_at_each_step: Validate configuration at each step
        
    Returns:
        Dictionary containing simulation results and statistics
        
    Raises:
        NLMError: If simulation fails
    """
    from pynlm import NLMError, BrainError
    
    try:
        # Initialize simulation components
        if config is None:
            config = nlm.createDefaultConfig()
        
        brain = nlm.createBrain(config)
        
        if world is None:
            world = nlm.createSimpleWorld()
        
        agent = nlm.createAgentBrain(brain)
        
        # Initialize callbacks
        if callbacks is None:
            callbacks = {}
        
        start_time = time.time()
        stats = SimulationStats()
        
        # Run simulation steps
        for step in range(steps):
            try:
                # Validate configuration if requested
                if validate_at_each_step:
                    validateSimulation(brain, world, agent)
                
                # Execute step
                brain.step()
                
                # Apply learning if enabled
                if enable_learning:
                    agent.applyRewardModulation(0.1, 0.05)
                
                # Collect statistics
                stats.steps_completed = step + 1
                stats.total_neurons = brain.getTotalNeuronCount()
                stats.active_neurons = brain.getActiveNeuronCount()
                stats.total_spikes = brain.getTotalSpikeCount()
                stats.firing_rate = brain.getAverageFiringRate()
                stats.mean_synaptic_activity = brain.getAverageSynapseStrength()
                
                # Execute callbacks
                if 'on_step' in callbacks:
                    callbacks['on_step'](step, stats)
                
                # Check for completion conditions
                if checkCompletionConditions(brain, world, agent):
                    logger.info(f"Simulation completed early at step {step}")
                    break
                    
            except Exception as e:
                logger.error(f"Error at step {step}: {e}")
                if 'on_error' in callbacks:
                    callbacks['on_error'](step, e, stats)
                raise NLMError(f"Simulation failed at step {step}: {e}")
        
        # Final statistics
        stats.simulation_time = time.time() - start_time
        stats.memory_usage = getMemoryUsage()
        stats.neuromodulation_levels = getNeuromodulationLevels(agent)
        stats.learning_rate = config.getLearningRate()
        stats.convergence_score = calculateConvergenceScore(brain)
        
        # Execute completion callbacks
        if 'on_complete' in callbacks:
            callbacks['on_complete'](stats)
        
        results = {
            'brain': brain,
            'world': world,
            'agent': agent,
            'stats': stats,
            'success': True,
            'error': None
        }
        
        logger.info(f"Simulation completed successfully: {steps} steps in {stats.simulation_time:.2f}s")
        return results
        
    except Exception as e:
        logger.error(f"Simulation failed: {e}")
        return {
            'brain': None,
            'world': None,
            'agent': None,
            'stats': SimulationStats(),
            'success': False,
            'error': str(e)
        }

def validateSimulation(
    brain: nlm.Brain,
    world: nlm.SimpleWorld,
    agent: nlm.AgentBrain
) -> bool:
    """Validate simulation state and configuration.
    
    Args:
        brain: Brain instance
        world: World instance  
        agent: AgentBrain instance
        
    Returns:
        True if validation passes, False otherwise
        
    Raises:
        NLMError: If validation fails
    """
    config = brain.getConfig()
    
    # Validate configuration parameters
    neuron_count = config.getNumNeurons()
    if neuron_count <= 0:
        raise nlm.NLMError(f"Invalid neuron count: {neuron_count}")
    
    learning_rate = config.getLearningRate()
    if not (0.0 < learning_rate <= 1.0):
        raise nlm.NLMError(f"Invalid learning rate: {learning_rate}")
    
    time_step = config.getTimeStep()
    if not (0.0 < time_step <= 1.0):
        raise nlm.NLMError(f"Invalid time step: {time_step}")
    
    # Validate world dimensions
    world_width = world.getWorldWidth()
    world_height = world.getWorldHeight()
    if world_width <= 0 or world_height <= 0:
        raise nlm.NLMError(f"Invalid world dimensions: {world_width}x{world_height}")
    
    # Validate brain statistics
    total_neurons = brain.getTotalNeuronCount()
    active_neurons = brain.getActiveNeuronCount()
    if active_neurons > total_neurons:
        raise nlm.NLMError(f"Active neurons ({active_neurons}) exceed total ({total_neurons})")
    
    return True

def saveBrainState(
    brain: nlm.Brain,
    filename: str,
    format: str = "json",
    compress: bool = True,
    validate: bool = True
) -> bool:
    """Save brain state to file with validation and compression options.
    
    Args:
        brain: Brain instance to save
        filename: Output file path
        format: Serialization format ('json', 'binary', 'compressed')
        compress: Enable compression for binary formats
        validate: Validate before saving
        
    Returns:
        True if save successful, False otherwise
        
    Raises:
        NLMError: If save fails
    """
    import json
    
    try:
        if validate:
            # Validate brain state
            if brain.getTotalNeuronCount() <= 0:
                raise nlm.NLMError("Cannot save brain with zero neurons")
        
        # Collect brain state data
        state = {
            'config': {
                'neuron_count': brain.getConfig().getNumNeurons(),
                'learning_rate': brain.getConfig().getLearningRate(),
                'time_step': brain.getConfig().getTimeStep(),
                'synapse_density': brain.getConfig().getSynapseDensity(),
                'world_width': brain.getConfig().getWorldWidth(),
                'world_height': brain.getConfig().getWorldHeight()
            },
            'brain_stats': {
                'total_neurons': brain.getTotalNeuronCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'total_spike_count': brain.getTotalSpikeCount(),
                'developmental_stage': str(brain.getDevelopmentalStage())
            },
            'timestamp': time.time()
        }
        
        # Save with appropriate format
        if format.lower() == 'json':
            with open(filename, 'w') as f:
                json.dump(state, f, indent=2)
        elif format.lower() in ['binary', 'compressed']:
            import pickle
            with open(filename, 'wb') as f:
                if compress:
                    import gzip
                    with gzip.open(filename + '.gz', 'wb') as gz:
                        pickle.dump(state, gz)
                else:
                    pickle.dump(state, f)
        else:
            raise nlm.NLMError(f"Unsupported format: {format}")
        
        logger.info(f"Brain state saved to {filename} ({format} format)")
        return True
        
    except Exception as e:
        logger.error(f"Failed to save brain state: {e}")
        raise nlm.NLMError(f"Failed to save brain state: {e}")

def loadBrainState(
    filename: str,
    create_brain: bool = True,
    validate_loaded: bool = True
) -> Union[nlm.Brain, Dict[str, Any]]:
    """Load brain state from file with validation.
    
    Args:
        filename: Input file path
        create_brain: Create brain instance from loaded state
        validate_loaded: Validate loaded state
        
    Returns:
        Brain instance if create_brain=True, otherwise loaded state dict
        
    Raises:
        NLMError: If load fails
    """
    import json
    
    try:
        # Load state
        if filename.endswith('.gz'):
            import gzip
            import pickle
            with gzip.open(filename, 'rb') as f:
                state = pickle.load(f)
        else:
            with open(filename, 'r') as f:
                state = json.load(f)
        
        # Validate loaded state if requested
        if validate_loaded:
            config = state.get('config', {})
            if config.get('neuron_count', 0) <= 0:
                raise nlm.NLMError("Invalid loaded state: non-positive neuron count")
            if config.get('learning_rate', 1.0) <= 0:
                raise nlm.NLMError("Invalid loaded state: non-positive learning rate")
        
        # Create brain instance if requested
        if create_brain:
            config = nlm.createDefaultConfig()
            config.setNumNeurons(state['config']['neuron_count'])
            config.setLearningRate(state['config']['learning_rate'])
            config.setTimeStep(state['config']['time_step'])
            config.setSynapseDensity(state['config']['synapse_density'])
            config.setWorldWidth(state['config']['world_width'])
            config.setWorldHeight(state['config']['world_height'])
            
            brain = nlm.createBrain(config)
            logger.info(f"Brain state loaded from {filename}")
            return brain
        else:
            logger.info(f"Brain state loaded from {filename}")
            return state
        
    except Exception as e:
        logger.error(f"Failed to load brain state: {e}")
        raise nlm.NLMError(f"Failed to load brain state: {e}")

def getMemoryUsage() -> Dict[str, float]:
    """Get current memory usage statistics.
    
    Returns:
        Dictionary with memory usage information
    """
    import psutil
    import os
    
    process = psutil.Process(os.getpid())
    memory_info = process.memory_info()
    
    return {
        'rss_mb': memory_info.rss / 1024 / 1024,
        'vms_mb': memory_info.vms / 1024 / 1024,
        'percent': process.memory_percent()
    }

def getNeuromodulationLevels(agent: nlm.AgentBrain) -> Dict[str, float]:
    """Get current neuromodulation levels from agent.
    
    Args:
        agent: AgentBrain instance
        
    Returns:
        Dictionary with neuromodulation levels
    """
    return {
        'reward_modulation': agent.getNeuromodulationLevel(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'prediction_error': agent.getPredictionError()
    }

def checkCompletionConditions(
    brain: nlm.Brain,
    world: nlm.SimpleWorld,
    agent: nlm.AgentBrain
) -> bool:
    """Check if simulation should be completed early.
    
    Args:
        brain: Brain instance
        world: World instance
        agent: AgentBrain instance
        
    Returns:
        True if completion conditions are met
    """
    # Check for high convergence
    convergence_score = calculateConvergenceScore(brain)
    if convergence_score > 0.95:
        logger.info(f"High convergence detected: {convergence_score:.3f}")
        return True
    
    # Check for excessive spike rate
    firing_rate = brain.getAverageFiringRate()
    if firing_rate > 100.0:  # Threshold in Hz
        logger.info(f"High firing rate detected: {firing_rate:.2f} Hz")
        return True
    
    # Check developmental stage completion
    if brain.getDevelopmentalStage() == nlm.DevelopmentalStage.Aging:
        logger.info("Developmental stage completed (Aging)")
        return True
    
    return False

def calculateConvergenceScore(brain: nlm.Brain) -> float:
    """Calculate convergence score based on brain state.
    
    Args:
        brain: Brain instance
        
    Returns:
        Convergence score between 0.0 and 1.0
    """
    total_neurons = brain.getTotalNeuronCount()
    if total_neurons == 0:
        return 0.0
    
    # Use multiple factors for convergence calculation
    firing_rate = brain.getAverageFiringRate()
    excitation_inhibition_ratio = brain.getExcitationInhibitionRatio()
    spike_rate = brain.getTotalSpikeCount()
    
    # Normalize firing rate (ideal around 10 Hz)
    firing_rate_score = max(0.0, 1.0 - abs(firing_rate - 10.0) / 50.0)
    
    # Ideal excitation/inhibition ratio
    ei_score = max(0.0, 1.0 - abs(excitation_inhibition_ratio - 1.0) / 2.0)
    
    # Spike rate should stabilize over time
    spike_score = min(1.0, spike_rate / 10000.0)
    
    # Weighted average
    convergence = (firing_rate_score * 0.4 + ei_score * 0.3 + spike_score * 0.3)
    
    return max(0.0, min(1.0, convergence))

def benchmarkPerformance(
    brain: nlm.Brain,
    steps: int = 1000,
    warmup_steps: int = 100,
    measure_memory: bool = True,
    measure_cpu: bool = True
) -> Dict[str, float]:
    """Benchmark NLM brain performance.
    
    Args:
        brain: Brain instance to benchmark
        steps: Number of steps for benchmarking
        warmup_steps: Steps for warm-up (not included in measurements)
        measure_memory: Measure memory usage
        measure_cpu: Measure CPU usage
        
    Returns:
        Dictionary with performance metrics
    """
    import time
    
    start_time = time.time()
    
    # Warm-up
    for _ in range(warmup_steps):
        brain.step()
    
    # Benchmark
    step_times = []
    for step in range(steps):
        step_start = time.time()
        brain.step()
        step_times.append(time.time() - step_start)
    
    total_time = time.time() - start_time
    avg_step_time = sum(step_times) / len(step_times)
    min_step_time = min(step_times)
    max_step_time = max(step_times)
    
    # Calculate statistics
    results = {
        'total_time': total_time,
        'avg_step_time': avg_step_time,
        'min_step_time': min_step_time,
        'max_step_time': max_step_time,
        'steps_per_second': steps / total_time,
        'neurons_per_second': brain.getTotalNeuronCount() * (steps / total_time)
    }
    
    if measure_memory:
        memory_info = getMemoryUsage()
        results.update({f'memory_{k}': v for k, v in memory_info.items()})
    
    if measure_cpu:
        import psutil
        cpu_percent = psutil.cpu_percent()
        results['cpu_percent'] = cpu_percent
    
    logger.info(f"Performance benchmark completed: {steps} steps in {total_time:.2f}s")
    return results

# Simulation status management
_simulation_status = SimulationStatus.IDLE

def setSimulationStatus(status: SimulationStatus):
    """Set simulation status.
    
    Args:
        status: New simulation status
    """
    global _simulation_status
    _simulation_status = status
    logger.info(f"Simulation status changed to: {status.value}")

def getSimulationStatus() -> SimulationStatus:
    """Get current simulation status.
    
    Returns:
        Current simulation status
    """
    return _simulation_status

def getSimulationInfo() -> Dict[str, Any]:
    """Get comprehensive simulation information.
    
    Returns:
        Dictionary with simulation information
    """
    return {
        'status': _simulation_status.value,
        'timestamp': time.time(),
        'pynlm_version': nlm.__version__,
        'system_info': {
            'platform': sys.platform,
            'python_version': f"{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}"
        }
    }