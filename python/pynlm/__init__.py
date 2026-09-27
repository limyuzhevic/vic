"""
pynlm - Python bindings for NLM (Neural Learning Machine)

Enhanced Python API with comprehensive error handling, type safety, and consistent design patterns.

This package provides a user-friendly interface to the NLM neural simulation framework
with improved error handling, validation, and documentation.
"""

import sys
import warnings

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Enable deprecation warnings
warnings.filterwarnings('default', category=DeprecationWarning)

# Import core types and classes
from .bindings import (
    # Core types
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    
    # Enumerations
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # Core classes
    Config,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    SimpleWorld,
    Brain,
    AgentBrain,
    
    # Factory functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    
    # Global constants
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    
    # Custom exceptions
    NLMException,
    ConfigurationError,
    BrainInitializationError,
    SimulationError,
    InvalidParameterError,
    ResourceError,
)

# Public API
__all__ = [
    # Version and metadata
    '__version__',
    '__author__',
    '__license__',
    
    # Core types
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    
    # Enumerations
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    
    # Core classes
    'Config',
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'SensoryPercept',
    'SimpleWorld',
    'Brain',
    'AgentBrain',
    
    # Factory functions
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    
    # Global constants
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Custom exceptions
    'NLMException',
    'ConfigurationError',
    'BrainInitializationError',
    'SimulationError',
    'InvalidParameterError',
    'ResourceError',
]

# Validation function for type safety
def _validate_type(value, expected_type, param_name):
    """Validate parameter type with clear error messages.
    
    Args:
        value: The value to validate
        expected_type: Expected type or tuple of types
        param_name: Name of the parameter for error messages
        
    Raises:
        InvalidParameterError: If value type doesn't match expected type
    """
    if not isinstance(value, expected_type):
        type_names = expected_type if isinstance(expected_type, tuple) else (expected_type,)
        type_str = ' or '.join(t.__name__ for t in type_names)
        raise InvalidParameterError(
            param_name,
            f"type {type_str}",
            f"type {type(value).__name__}"
        )

# Convenience function for creating brains with validation
def create_brain_with_validation(config=None, **kwargs):
    """
    Create a brain with enhanced validation and error handling.
    
    Args:
        config (Config, optional): Brain configuration. If None, creates default.
        **kwargs: Additional configuration parameters to set on the config.
    
    Returns:
        Brain: Configured brain instance
        
    Raises:
        InvalidParameterError: If configuration parameters are invalid
        BrainInitializationError: If brain cannot be initialized
    """
    if config is None:
        config = createDefaultConfig()
    
    # Set additional configuration parameters
    for key, value in kwargs.items():
        config.set(key, value)
    
    brain = createBrain(config)
    
    if not brain.initialize():
        raise BrainInitializationError("Failed to initialize brain")
    
    return brain

# Convenience function for running simulations
def run_simulation(brain, world=None, agent=None, num_steps=100, **kwargs):
    """
    Run a simulation with enhanced error handling and monitoring.
    
    Args:
        brain (Brain): Brain instance
        world (SimpleWorld, optional): World instance. If None, creates default.
        agent (AgentBrain, optional): Agent instance. If None, creates from brain.
        num_steps (int): Number of simulation steps
        **kwargs: Additional configuration parameters
    
    Returns:
        dict: Simulation results including statistics
        
    Raises:
        InvalidParameterError: If parameters are invalid
        SimulationError: If simulation fails
    """
    if not isinstance(num_steps, int) or num_steps <= 0:
        raise InvalidParameterError("num_steps", "positive integer", f"{num_steps}")
    
    # Initialize world and agent if not provided
    if world is None:
        world = createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
    
    if agent is None:
        agent = createAgentBrain(brain)
        agent.initialize(world)
    
    # Run simulation
    results = {
        'steps_completed': 0,
        'total_spikes': 0,
        'max_firing_rate': 0.0,
        'active_neurons': 0,
        'firing_neuron_count': 0,
        'excitation_inhibition_ratio': 0.0,
        'neuromodulation_level': 0.0,
        'curiosity_level': 0.0,
        'simulation_time': 0.0,
    }
    
    try:
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Update results
            results['steps_completed'] += 1
            results['total_spikes'] = brain.getTotalSpikeCount()
            results['max_firing_rate'] = max(results['max_firing_rate'], brain.getAverageFiringRate())
            results['active_neurons'] = brain.getActiveNeuronCount()
            results['firing_neuron_count'] = brain.getFiringNeuronCount()
            results['excitation_inhibition_ratio'] = brain.getExcitationInhibitionRatio()
            results['neuromodulation_level'] = agent.getNeuromodulationLevel()
            results['curiosity_level'] = agent.getCuriosityLevel()
            results['simulation_time'] = world.getSimulationTime()
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
    except Exception as e:
        raise SimulationError(f"Simulation failed at step {results['steps_completed']}: {e}")
    
    return results

# Development and debugging utilities
class DebugMonitor:
    """
    Debug monitoring utilities for NLM development and debugging.
    
    Provides tools for monitoring system state, detecting issues, and
    analyzing performance during development.
    """
    
    def __init__(self, brain, world=None, agent=None):
        """
        Initialize debug monitor.
        
        Args:
            brain (Brain): Brain instance
            world (SimpleWorld, optional): World instance
            agent (AgentBrain, optional): Agent instance
        """
        self.brain = brain
        self.world = world
        self.agent = agent
        self.history = []
    
    def capture_state(self, description=""):
        """
        Capture current system state.
        
        Args:
            description (str): Description of the state capture
        """
        state = {
            'timestamp': self.brain.getConfig().getOr('simulation_time', 0.0),
            'step': len(self.history),
            'description': description,
            'neurons': {
                'total': self.brain.getTotalNeuronCount(),
                'active': self.brain.getActiveNeuronCount(),
                'firing': self.brain.getFiringNeuronCount(),
                'spikes': self.brain.getTotalSpikeCount(),
            },
            'world': {
                'width': self.world.getWidth() if self.world else 0,
                'height': self.world.getHeight() if self.world else 0,
                'position': {
                    'x': self.world.getAgentBody().x if self.world else 0,
                    'y': self.world.getAgentBody().y if self.world else 0,
                } if self.world else {},
            } if self.world else {},
            'agent': {
                'stage': self.agent.getDevelopmentalStage() if self.agent else None,
                'curiosity': self.agent.getCuriosityLevel() if self.agent else 0,
                'neuromodulation': self.agent.getNeuromodulationLevel() if self.agent else 0,
            } if self.agent else {},
        }
        self.history.append(state)
        return state
    
    def get_stability_report(self):
        """
        Get stability analysis report.
        
        Returns:
            dict: Stability analysis report
        """
        if not self.history:
            return {'stable': True, 'issues': [], 'warnings': []}
        
        issues = []
        warnings = []
        
        # Check for neuron count changes (should be stable)
        neuron_counts = [s['neurons']['total'] for s in self.history]
        if len(set(neuron_counts)) > 1:
            issues.append("Neuron count changed during simulation")
        
        # Check for firing rate (should not explode)
        firing_rates = [s['neurons']['spikes'] / max(s['timestamp'], 1) for s in self.history]
        if firing_rates and max(firing_rates) > 1000:
            warnings.append("High firing rate detected")
        
        # Check for developmental stage progression
        if self.agent:
            stages = [s['agent']['stage'] for s in self.history if s['agent']['stage'] is not None]
            if len(stages) > 1 and stages[-1] != stages[0]:
                print(f"Developmental stage progression: {stages[0]} -> {stages[-1]}")
        
        return {
            'stable': len(issues) == 0,
            'issues': issues,
            'warnings': warnings,
            'total_captures': len(self.history),
        }
    
    def export_history(self, filepath):
        """
        Export capture history to file.
        
        Args:
            filepath (str): Path to export file
        """
        import json
        
        with open(filepath, 'w') as f:
            json.dump(self.history, f, indent=2, default=str)

# Make debug monitor available
DebugMonitor = DebugMonitor

# Set up module-level utilities
__version_info__ = {
    'version': __version__,
    'author': __author__,
    'license': __license__,
    'description': 'Python bindings for NLM (Neural Learning Machine)',
}

# Set up type checking support
if False:
    # This block is only for type checking tools
    import typing as t
    
    # Type stubs for better IDE support
    ConfigDict = t.Dict[str, t.Union[int, float, str, bool, t.List[t.Any]]]
    VisionData = t.List[float]
    AudioSamples = t.List[float]
    ActionParams = t.List[float]
    
    # Define the public API for type checkers
    class Config:
        def __init__(self) -> None: ...
        def loadFromFile(self, filepath: str) -> bool: ...
        def saveToFile(self, filepath: str) -> bool: ...
        def has(self, key: str) -> bool: ...
        def getKeys(self) -> t.List[str]: ...
        def clear(self) -> None: ...
        def summary(self) -> str: ...
        def getOr(self, key: str, default: t.Any) -> t.Any: ...
        def set(self, key: str, value: t.Any) -> None: ...
    
    # Type checkers will understand the API structure
    # Actual implementations are in the C++ bindings
