"""
NLM (熙然) - Neural Learning Machine Python Interface

An experimental artificial developmental brain implemented in Python.
This module provides a Python interface to the C++ NLM neural simulation framework.

Features:
- Brain simulation with Leaky Integrate-and-Fire (LIF) neurons
- Synaptic plasticity (STDP, Hebbian, structural)
- Memory systems (working memory, episodic memory)
- Neuromodulation (dopamine, curiosity, novelty detection)
- Prediction and cognition systems
- Development and maturation
- World interaction and agent behavior
- Checkpoint save/load for persistence

See docs/ for detailed documentation and examples.
"""

import sys
import platform

# Version information
__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Python interface to NLM (Neural Learning Machine) neural simulation framework"

# Platform information
python_version = f"{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}"
platform_info = f"{platform.system()} {platform.release()} on {platform.machine()}"

# Import key components from C++ bindings
try:
    from .bindings import *
    has_cpp_bindings = True
except ImportError as e:
    has_cpp_bindings = False
    _import_error = e

# Pythonic convenience classes and functions
class NLMError(Exception):
    """Base exception for NLM errors"""
    pass
class ConfigurationError(NLMError):
    """Error in configuration"""
    pass
class SimulationError(NLMError):
    """Error during simulation"""
    pass
class InitializationError(NLMError):
    """Error during brain initialization"""
    pass
def create_brain_with_config(**kwargs):
    """
    Convenience function to create a brain with custom configuration.
    
    Args:
        **kwargs: Configuration parameters
        
    Returns:
        Brain: Configured brain instance
        
    Example:
        >>> brain = create_brain_with_config(
        ...     neuron_count=500,
        ...     region_count=2,
        ...     connection_probability=0.1
        ... )
    """
    if not has_cpp_bindings:
        raise RuntimeError("C++ bindings not available. Cannot create brain.")
    
    config = createDefaultConfig()
    
    # Map common configuration parameters
    config_map = {
        "neuron_count": ("brain.neuron_count", lambda x: int(x)),
        "region_count": ("brain.region_count", lambda x: int(x)),
        "connection_probability": ("brain.connection_probability", lambda x: float(x)),
        "simulation_timestep": ("simulation_timestep", lambda x: float(x)),
        "random_seed": ("random_seed", lambda x: int(x)),
    }
    
    for key, value in kwargs.items():
        if key in config_map:
            param_path, converter = config_map[key]
            config.set(param_path, converter(value))
    
    return createBrain(config)
def run_agent_simulation(brain, world, agent, num_steps=1000, verbose=False):
    """
    Convenience function to run a complete agent simulation.
    
    Args:
        brain: Brain instance
        world: World instance  
        agent: AgentBrain instance
        num_steps: Number of simulation steps
        verbose: Print progress information
        
    Returns:
        dict: Simulation results including statistics
    """
    if not all([has_cpp_bindings, brain, world, agent]):
        raise ValueError("All components (brain, world, agent) must be provided and valid")
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    results = {
        'total_spikes': 0,
        'firing_neuron_counts': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'developmental_stages': [],
        'final_spike_count': 0
    }
    
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Get reward and apply modulation
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        results['total_spikes'] += brain.getFiringNeuronCount()
        results['firing_neuron_counts'].append(brain.getFiringNeuronCount())
        results['curiosity_levels'].append(agent.getCuriosityLevel())
        results['novelty_levels'].append(agent.getNoveltyLevel())
        results['developmental_stages'].append(brain.getDevelopmentalStage())
        
        if verbose and step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    results['final_spike_count'] = brain.getTotalSpikeCount()
    return results
def benchmark_brain_performance(brain, num_steps=1000, warmup_steps=100):
    """
    Benchmark brain performance metrics.
    
    Args:
        brain: Brain instance
        num_steps: Number of simulation steps to run
        warmup_steps: Number of steps to skip for warmup
        
    Returns:
        dict: Performance metrics
    """
    if not has_cpp_bindings:
        raise RuntimeError("C++ bindings not available. Cannot benchmark.")
    
    import time
    
    # Warmup
    for step in range(warmup_steps):
        brain.step(step)
    
    # Benchmark
    start_time = time.time()
    for step in range(num_steps):
        brain.step(step)
    end_time = time.time()
    
    elapsed = end_time - start_time
    steps_per_second = num_steps / elapsed if elapsed > 0 else 0
    
    return {
        'total_steps': num_steps,
        'elapsed_time': elapsed,
        'steps_per_second': steps_per_second,
        'final_spike_count': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'final_development_stage': brain.getDevelopmentalStage()
    }
def validate_configuration(config):
    """
    Validate a configuration for errors and warnings.
    
    Args:
        config: Config instance to validate
        
    Returns:
        dict: Validation results with errors and warnings
    """
    if not has_cpp_bindings:
        raise RuntimeError("C++ bindings not available. Cannot validate configuration.")
    
    results = {
        'is_valid': True,
        'errors': [],
        'warnings': [],
        'recommendations': []
    }
    
    # Check neuron count
    neuron_count = config.getOr<int64_t>("brain.neuron_count", 1000)
    if neuron_count < 10:
        results['warnings'].append("Very small neuron count (< 10) may not produce meaningful results")
    elif neuron_count > 10000:
        results['warnings'].append("Large neuron count (> 10000) may impact performance")
    
    # Check connection probability
    connection_prob = config.getOr<float>("brain.connection_probability", 0.1)
    if connection_prob > 0.5:
        results['warnings'].append("High connection probability (> 0.5) may cause excessive connectivity")
    
    # Check timestep
    timestep = config.getOr<double>("simulation_timestep", 0.001)
    if timestep < 0.0001 or timestep > 0.1:
        results['warnings'].append("Simulation timestep may be too small or too large for accurate results")
    
    # Check for obvious errors
    if not hasattr(config, 'getKeys'):
        results['is_valid'] = False
        results['errors'].append("Configuration object missing required methods")
    
    if results['warnings']:
        results['recommendations'].append("Consider adjusting warning parameters for better simulation results")
    
    return results
# Make key functions available at module level
__all__ = [
    'NLMError', 'ConfigurationError', 'SimulationError', 'InitializationError',
    'createBrain', 'createDefaultConfig', 'createSimpleWorld', 'createAgentBrain',
    'create_brain_with_config', 'run_agent_simulation', 'benchmark_brain_performance',
    'validate_configuration',
    'NeuronType', 'ActionType', 'DevelopmentalStage', 'FiringState', 'WorldObjectType',
    'Vision', 'Audio', 'InternalSignals', 'WorldObject', 'Action',
    'SensoryPercept', 'AgentBody', 'ActionResult'
]

if has_cpp_bindings:
    # Re-export all symbols from C++ bindings
    globals().update({k: v for k, v in locals().items() if k in __all__ and k != '__all__'})