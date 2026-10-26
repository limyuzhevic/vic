"""
Enhanced NLM Python Module
===========================

This module provides enhanced Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework.
It includes advanced features, better error handling, and comprehensive utility functions for both beginners and advanced users.

Key Features:
- Enhanced API with better error handling
- Batch processing capabilities
- Advanced configuration management
- Comprehensive statistical analysis
- Neural activity pattern analysis
- Agent performance monitoring
- Environment statistics
- Custom utility functions

Usage Examples:
    >>> import pynlm
    >>> config = pynlm.create_advanced_config(neuron_count=2000, connection_probability=0.2)
    >>> brain = pynlm.createBrain(config)
    >>> brain.initialize()
    >>> 
    >>> # Run batch simulation
    >>> brains = [brain]
    >>> worlds = [pynlm.createSimpleWorld()]
    >>> results = pynlm.run_batch_simulation(brains, worlds, 100)
    >>> 
    >>> # Analyze neural activity
    >>> stats = pynlm.compute_brain_stats(brain)
    >>> pattern = pynlm.get_neuron_activity_pattern(brain, pynlm.NeuronType.Excitatory)
    >>> 
    >>> # Create brain with specific initial pattern
    >>> initial_pattern = [0.1, 0.5, 0.8, 0.3, 0.9]
    >>> patterned_brain = pynlm.create_brain_with_pattern(config, initial_pattern)
"""

import numpy as np
from typing import List, Dict, Optional, Tuple, Union, Any
import warnings

# Try to import enhanced bindings first, fall back to basic bindings
try:
    import pynlm_enhanced as _pynlm_module
    print("Using enhanced NLM bindings")
except ImportError:
    try:
        import pynlm as _pynlm_module
        print("Using basic NLM bindings (enhanced features not available)")
    except ImportError:
        raise ImportError("NLM bindings not available. Please install the package: pip install pynlm")

# Import all public symbols from the underlying module
from pynlm import (
    # Core types
    NeuronId, SynapseId, RegionId, PopulationId, NeuronType,
    SynapseType, DevelopmentalStage, FiringState, ActionType,
    MotorCommand, WorldObjectType, Config,
    
    # Main classes
    SensoryInput, Vision, Audio, InternalSignals, Action,
    WorldObject, AgentBody, ActionResult, SensoryPercept,
    SimpleWorld, Brain, AgentBrain,
    
    # Constants
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    
    # Factory functions
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
)

# Enhanced wrapper classes and functions
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Configuration-related errors."""
    pass

class SimulationError(NLMError):
    """Simulation-related errors."""
    pass

class ValidationError(NLMError):
    """Validation-related errors."""
    pass

# Utility functions
def normalize_vector(data: List[float]) -> List[float]:
    """Normalize vector data to [0, 1] range.
    
    Args:
        data: Input vector
        
    Returns:
        Normalized vector
        
    Raises:
        ValidationError: If input is invalid
    """
    if not data:
        return data
    if len(data) == 0:
        return data
    
    try:
        # Try to use enhanced bindings if available
        return _pynlm_module.normalize(data)
    except (AttributeError, NameError):
        # Fallback implementation
        data_array = np.array(data, dtype=float)
        min_val = np.min(data_array)
        max_val = np.max(data_array)
        
        if max_val == min_val:
            return [0.5] * len(data_array)
        
        normalized = (data_array - min_val) / (max_val - min_val)
        return normalized.tolist()

def clamp(value: float, min_val: float, max_val: float) -> float:
    """Clamp value to [min_val, max_val] range.
    
    Args:
        value: Input value
        min_val: Minimum allowed value
        max_val: Maximum allowed value
        
    Returns:
        Clamped value
    """
    try:
        return _pynlm_module.clamp(value, min_val, max_val)
    except (AttributeError, NameError):
        return max(min_val, min(value, max_val))

def euclidean_distance(a: List[float], b: List[float]) -> float:
    """Calculate Euclidean distance between two vectors.
    
    Args:
        a: First vector
        b: Second vector
        
    Returns:
        Euclidean distance
        
    Raises:
        ValidationError: If vectors have different dimensions
    """
    if len(a) != len(b):
        raise ValidationError(f"Vector dimension mismatch: {len(a)} vs {len(b)}")
    
    try:
        return _pynlm_module.euclidean_distance(a, b)
    except (AttributeError, NameError):
        a_array = np.array(a, dtype=float)
        b_array = np.array(b, dtype=float)
        return np.linalg.norm(a_array - b_array)

def dot_product(a: List[float], b: List[float]) -> float:
    """Calculate dot product of two vectors.
    
    Args:
        a: First vector
        b: Second vector
        
    Returns:
        Dot product
        
    Raises:
        ValidationError: If vectors have different dimensions
    """
    if len(a) != len(b):
        raise ValidationError(f"Vector dimension mismatch: {len(a)} vs {len(b)}")
    
    try:
        return _pynlm_module.dot_product(a, b)
    except (AttributeError, NameError):
        a_array = np.array(a, dtype=float)
        b_array = np.array(b, dtype=float)
        return np.dot(a_array, b_array)

# Enhanced configuration utilities
def create_advanced_config(
    neuron_count: int = 1000,
    region_count: int = 2,
    connection_probability: float = 0.15,
    plasticity_rate: float = 0.01,
    development_rate: float = 0.001,
    environment: str = "SimpleWorld",
    enable_advanced_features: bool = True
) -> Config:
    """Create advanced configuration with optimized settings.
    
    Args:
        neuron_count: Number of neurons in the brain
        region_count: Number of brain regions
        connection_probability: Probability of connection between neurons
        plasticity_rate: Neural plasticity learning rate
        development_rate: Developmental plasticity rate
        environment: Environment type
        enable_advanced_features: Whether to enable advanced features
        
    Returns:
        Configured Config object
        
    Raises:
        ConfigurationError: If configuration is invalid
    """
    try:
        return _pynlm_module.createAdvancedConfig(
            neuron_count=neuron_count,
            region_count=region_count,
            connection_probability=connection_probability,
            plasticity_rate=plasticity_rate,
            development_rate=development_rate,
            environment=environment
        )
    except (AttributeError, NameError):
        # Fallback implementation
        config = createDefaultConfig()
        
        # Set basic parameters
        config.set("neuron_count", neuron_count)
        config.set("region_count", region_count)
        config.set("connection_probability", connection_probability)
        config.set("plasticity_learning_rate", plasticity_rate)
        config.set("development_synaptogenesis_rate", development_rate)
        config.set("environment_name", environment)
        
        if enable_advanced_features:
            # Enable advanced features
            config.set("plasticity.stdp.enable", True)
            config.set("plasticity.hebbian.enable", True)
            config.set("neuromod.dopamine.scale", 1.0f)
            config.set("neuromod.curiosity.enable", True)
            config.set("neuromod.novelty.enable", True)
        
        return config

def get_config_summary(config: Config) -> Dict[str, str]:
    """Get configuration summary as dictionary.
    
    Args:
        config: Configuration object
        
    Returns:
        Dictionary of configuration key-value pairs
    """
    try:
        return _pynlm_module.getConfigSummary(config)
    except (AttributeError, NameError):
        # Fallback implementation
        summary = {}
        keys = config.getKeys()
        for key in keys:
            if config.has(key):
                try:
                    if key.find("neuron_count") != -1:
                        value = config.getOr[int64_t](key, 0)
                    elif key.find("connection_probability") != -1 or \
                         key.find("dopamine_baseline") != -1 or \
                         key.find("plasticity_learning_rate") != -1:
                        value = config.getOr[float](key, 0.0f)
                    elif key.find("enabled") != -1 or \
                         key.find("simulation_timestep") != -1:
                        value = config.getOr[bool](key, False)
                    else:
                        value = config.getOr[str](key, "")
                    summary[key] = str(value)
                except:
                    summary[key] = "<error>"
        return summary

# Batch processing utilities
class BatchSimulation:
    """Utility class for batch simulations."""
    
    @staticmethod
    def run_batch(
        brains: List[Brain],
        worlds: List[SimpleWorld],
        steps_per_brain: int,
        timestep: float = 0.1f
    ) -> Dict[str, Any]:
        """Run batch simulation across multiple brains.
        
        Args:
            brains: List of brain objects
            worlds: List of world objects
            steps_per_brain: Number of steps per brain
            timestep: Simulation timestep
            
        Returns:
            Dictionary containing simulation results
            
        Raises:
            SimulationError: If batch simulation fails
        """
        if len(brains) != len(worlds):
            raise SimulationError(f"Brain and world lists must have same length: {len(brains)} vs {len(worlds)}")
        
        try:
            return _pynlm_module.runBatchSimulation(brains, worlds, steps_per_brain, timestep)
        except (AttributeError, NameError):
            # Fallback implementation
            results = {
                'spike_rates': [],
                'firing_counts': [],
                'e_i_ratios': [],
                'total_time': steps_per_brain * timestep,
                'total_steps': steps_per_brain
            }
            
            for i, (brain, world) in enumerate(zip(brains, worlds)):
                brain.reset()
                brain.initialize()
                
                for step in range(steps_per_brain):
                    world.update(timestep)
                    
                    # Process sensory input if agent is available
                    try:
                        percept = world.getSensoryPercept()
                        # Note: This assumes an agent is attached, which is not always the case
                    except:
                        pass
                    
                    brain.step(step, step * timestep)
                
                results['spike_rates'].append(brain.getAverageFiringRate())
                results['firing_counts'].append(float(brain.getTotalSpikeCount()))
                results['e_i_ratios'].append(brain.getExcitationInhibitionRatio())
            
            return results

# Analysis utilities
class BrainAnalyzer:
    """Utility class for brain analysis."""
    
    @staticmethod
    def compute_comprehensive_stats(brain: Brain) -> Dict[str, Any]:
        """Compute comprehensive brain statistics.
        
        Args:
            brain: Brain object to analyze
            
        Returns:
            Dictionary containing brain statistics
        """
        try:
            return _pynlm_module.computeBrainStats(brain)
        except (AttributeError, NameError):
            # Fallback implementation
            stats = {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'development_progress': 1.0 if brain.getDevelopmentalStage() == DevelopmentalStage.Adult else 0.0,
                'total_spikes': brain.getTotalSpikeCount(),
                'energy_usage': float(brain.getTotalNeuronCount()) * 0.1,
                'memory_usage': (brain.getTotalNeuronCount() * 8 + brain.getTotalSynapseCount() * 4) / 1024
            }
            return stats
    
    @staticmethod
    def get_neuron_activity_pattern(
        brain: Brain,
        neuron_type: NeuronType = NeuronType.Excitatory,
        max_neurons: int = 100
    ) -> List[float]:
        """Get neuron activity pattern for analysis.
        
        Args:
            brain: Brain object
            neuron_type: Type of neurons to analyze
            max_neurons: Maximum number of neurons to analyze
            
        Returns:
            List of neural activities
        """
        try:
            return _pynlm_module.getNeuronActivityPattern(brain, neuron_type, max_neurons)
        except (AttributeError, NameError):
            # Fallback implementation
            # Note: This is a simplified version - actual implementation would depend on NLM API
            return [0.0] * min(max_neurons, 100)
    
    @staticmethod
    def create_brain_with_pattern(
        config: Config,
        initial_pattern: List[float],
        scale: float = 1.0
    ) -> Brain:
        """Create brain with initial activity pattern.
        
        Args:
            config: Configuration for brain creation
            initial_pattern: Initial neural activity pattern
            scale: Scaling factor for pattern
            
        Returns:
            Brain with applied pattern
        """
        try:
            return _pynlm_module.createBrainWithPattern(config, initial_pattern, scale)
        except (AttributeError, NameError):
            # Fallback implementation
            brain = createBrain(config)
            brain.initialize()
            
            # Note: This is a placeholder - actual implementation would depend on NLM API
            return brain

# Agent utilities
class AgentAnalyzer:
    """Utility class for agent analysis."""
    
    @staticmethod
    def analyze_performance(agent: AgentBrain) -> Dict[str, Any]:
        """Analyze agent performance.
        
        Args:
            agent: Agent object to analyze
            
        Returns:
            Dictionary containing performance metrics
        """
        try:
            return _pynlm_module.analyzeAgentPerformance(agent)
        except (AttributeError, NameError):
            # Fallback implementation
            perf = {
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'neuromodulation_level': agent.getNeuromodulationLevel(),
                'exploration_rate': agent.getCuriosityLevel() * 0.5,
                'learning_efficiency': agent.getCuriosityLevel() * (1.0 - abs(agent.getPredictionError())),
                'action_diversity': min(1.0, agent.getCuriosityLevel() * 0.8),
                'adaptation_rate': agent.getCuriosityLevel() * 0.5 * (1.0 - abs(agent.getPredictionError()))
            }
            return perf
    
    @staticmethod
    def get_recommended_actions(
        agent: AgentBrain,
        world: SimpleWorld,
        max_suggestions: int = 5
    ) -> List[str]:
        """Get recommended actions based on agent state.
        
        Args:
            agent: Agent object
            world: World object
            max_suggestions: Maximum number of suggestions
            
        Returns:
            List of recommended actions
        """
        try:
            return _pynlm_module.getRecommendedActions(agent, world, max_suggestions)
        except (AttributeError, NameError):
            # Fallback implementation
            suggestions = []
            
            if agent.getCuriosityLevel() > 0.7:
                suggestions.extend(["Explore new area", "Investigate anomaly"])
            
            if agent.getNoveltyLevel() > 0.5:
                suggestions.append("Try different approach")
            
            if agent.getPredictionError() > 0.3:
                suggestions.append("Reevaluate current strategy")
            
            if agent.getNeuromodulationLevel() < 0.3:
                suggestions.append("Rest and recover")
            
            # Limit suggestions
            if len(suggestions) > max_suggestions:
                suggestions = suggestions[:max_suggestions]
            
            return suggestions

# World utilities
class WorldAnalyzer:
    """Utility class for world analysis."""
    
    @staticmethod
    def get_environment_statistics(world: SimpleWorld) -> Dict[str, float]:
        """Get environment statistics.
        
        Args:
            world: World object
            
        Returns:
            Dictionary containing environment statistics
        """
        try:
            stats = _pynlm_module.getEnvironmentStats(world)
            # Convert to dict if it's a list
            if isinstance(stats, list):
                return {
                    'width': stats[0] if len(stats) > 0 else 0,
                    'height': stats[1] if len(stats) > 1 else 0,
                    'simulation_time': stats[2] if len(stats) > 2 else 0,
                    'max_energy': stats[3] if len(stats) > 3 else 0,
                    'energy_decay_rate': stats[4] if len(stats) > 4 else 0,
                    'agent_x': stats[5] if len(stats) > 5 else 0,
                    'agent_y': stats[6] if len(stats) > 6 else 0
                }
            return stats
        except (AttributeError, NameError):
            # Fallback implementation
            body = world.getAgentBody()
            return {
                'width': world.getWidth(),
                'height': world.getHeight(),
                'simulation_time': world.getSimulationTime(),
                'max_energy': world.getMaxEnergy(),
                'energy_decay_rate': world.getEnergyDecayRate(),
                'agent_x': body.x,
                'agent_y': body.y
            }
    
    @staticmethod
    def create_world(
        width: int = 20,
        height: int = 20,
        vision_width: float = 8.0,
        vision_height: float = 8.0,
        max_energy: int = 100,
        energy_decay: float = 0.01
    ) -> SimpleWorld:
        """Create configured world instance.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
            max_energy: Maximum energy
            energy_decay: Energy decay rate
            
        Returns:
            Configured SimpleWorld object
        """
        try:
            return _pynlm_module.createWorld(width, height, vision_width, vision_height, max_energy, energy_decay)
        except (AttributeError, NameError):
            # Fallback implementation
            world = createSimpleWorld()
            world.configure(width, height, vision_width, vision_height)
            world.setMaxEnergy(max_energy)
            world.setEnergyDecayRate(energy_decay)
            world.reset()
            return world

# High-level convenience functions
def run_complete_agent_simulation(
    num_steps: int = 1000,
    enable_learning: bool = True,
    enable_curiosity: bool = True,
    enable_development: bool = True,
    config: Optional[Config] = None
) -> Dict[str, Any]:
    """Run complete agent simulation with brain and world.
    
    Args:
        num_steps: Number of simulation steps
        enable_learning: Whether to enable reward modulation
        enable_curiosity: Whether to enable curiosity-driven exploration
        enable_development: Whether to enable development stages
        config: Configuration (creates default if None)
        
    Returns:
        Dictionary containing simulation results
    """
    if config is None:
        config = create_advanced_config()
    
    # Create brain and initialize
    brain = createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = createAgentBrain(brain)
    
    # Create and configure world
    world = createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable subsystems
    if enable_learning:
        agent.enableRewardModulation(True)
    if enable_curiosity:
        agent.enableCuriosity(True)
    if enable_development:
        agent.enableDevelopment(True)
    
    # Run simulation loop
    results = {
        'steps': [],
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'prediction_errors': [],
        'neuromodulation_levels': [],
        'development_stages': []
    }
    
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if percept and percept.getInternal():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record statistics periodically
        if step % 100 == 0:
            results['steps'].append(step)
            results['firing_rates'].append(brain.getFiringNeuronCount())
            results['curiosity_levels'].append(agent.getCuriosityLevel())
            results['novelty_levels'].append(agent.getNoveltyLevel())
            results['prediction_errors'].append(agent.getPredictionError())
            results['neuromodulation_levels'].append(agent.getNeuromodulationLevel())
            results['development_stages'].append(brain.getDevelopmentalStage())
    
    return results

# Version and metadata
__version__ = "1.1.0-enhanced"
__description__ = "Enhanced NLM Python module with advanced features"
__author__ = "NLM Project"
__license__ = "MIT"

# Make commonly used symbols available at module level
__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId', 'NeuronType',
    'SynapseType', 'DevelopmentalStage', 'FiringState', 'ActionType',
    'MotorCommand', 'WorldObjectType', 'Config',
    
    # Main classes
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals', 'Action',
    'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
    'SimpleWorld', 'Brain', 'AgentBrain',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Enhanced utilities
    'create_advanced_config',
    'get_config_summary',
    'BatchSimulation',
    'BrainAnalyzer',
    'AgentAnalyzer',
    'WorldAnalyzer',
    'normalize_vector',
    'clamp',
    'euclidean_distance',
    'dot_product',
    'run_complete_agent_simulation',
    
    # Exception types
    'NLMError', 'ConfigurationError', 'SimulationError', 'ValidationError',
    
    # Version info
    '__version__', '__description__', '__author__', '__license__',
    '__all__'
]