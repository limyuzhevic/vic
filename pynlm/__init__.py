"""
NLM Python Package for Neural Learning Machine

This package provides a Python interface to the NLM (Neural Learning Machine) 
C++ neural simulation framework. It offers high-level convenience functions
and Pythonic APIs for working with neural networks, simulation environments,
and agents.

Key Features:
- Intuitive Python APIs for brain, world, and agent operations
- Convenience functions for common workflows
- Batch operations for improved performance
- Comprehensive error handling and validation
- Simulation and training utilities
- Checkpoint management with metadata support

Usage Examples:
    >>> import pynlm
    >>> 
    >>> # Create a simple brain
    >>> config = pynlm.create_default_config()
    >>> brain = pynlm.create_brain(config)
    >>> brain.initialize()
    >>> 
    >>> # Run a simulation
    >>> results = pynlm.run_simulation(brain, steps=100)
    >>> print(f"Firing rate: {results['final_firing_rate']}")
    >>> 
    >>> # Train an agent
    >>> training_results = pynlm.train_agent(episodes=10)
    >>> print(f"Average reward: {training_results['avg_total_reward']}")
"""

__version__ = "0.1.0"
__author__ = "NLM Project"
__email__ = "contact@nlm-project.org"
__license__ = "MIT"

# Package metadata
PACKAGE_INFO = {
    'name': 'pynlm',
    'version': __version__,
    'description': 'Python bindings for NLM (Neural Learning Machine)',
    'author': __author__,
    'license': __license__,
    'python_requires': '>=3.8',
    'requires': []
}

# Global configuration
DEFAULT_CONFIG = {
    'neuron_count': 1000,
    'region_count': 1,
    'connection_probability': 0.1,
    'stdp_ltp_weight': 0.01,
    'stdp_ltd_weight': 0.012,
    'synaptogenesis_rate': 0.0001,
    'pruning_rate': 0.00001,
    'simulation_timestep': 0.001,
    'random_seed': 42
}

# Error classes
class NLMRuntimeError(Exception):
    """Base exception for NLM runtime errors."""
    pass

class ConfigurationError(NLMRuntimeError):
    """Configuration-related errors."""
    pass

class SimulationError(NLMRuntimeError):
    """Simulation-related errors."""
    pass

class MemoryError(NLMRuntimeError):
    """Memory-related errors."""
    pass

class ValidationError(NLMRuntimeError):
    """Validation-related errors."""
    pass

# Utility functions
def get_version():
    """Get the current version of pynlm."""
    return __version__

def get_package_info():
    """Get package information as a dictionary."""
    return PACKAGE_INFO.copy()

def create_config(**kwargs):
    """
    Create a configuration with optional overrides.
    
    Args:
        **kwargs: Configuration parameters to override defaults
        
    Returns:
        Config: Configured NLM configuration object
    """
    config = Config()
    
    # Set defaults
    for key, value in DEFAULT_CONFIG.items():
        config.set(key, value)
    
    # Apply overrides
    for key, value in kwargs.items():
        config.set(key, value)
    
    return config

def create_experiment_config(experiment_type='default', **kwargs):
    """
    Create configuration for specific experiment types.
    
    Args:
        experiment_type: Type of experiment ('default', 'cognitive', 'developmental', 'social')
        **kwargs: Additional configuration parameters
        
    Returns:
        Config: Configured NLM configuration object
    """
    config = create_config(**kwargs)
    
    if experiment_type == 'cognitive':
        config.set('brain.neuron_count', 2000)
        config.set('plasticity.stdp.enable', True)
        config.set('plasticity.hebbian.enable', True)
        config.set('plasticity.structural.enable', False)
    elif experiment_type == 'developmental':
        config.set('brain.neuron_count', 1500)
        config.set('development.enable', True)
        config.set('plasticity.structural.enable', True)
        config.set('plasticity.stdp.tau', 50.0)
    elif experiment_type == 'social':
        config.set('brain.neuron_count', 3000)
        config.set('social_learning.enable', True)
        config.set('brain.connection_probability', 0.15)
    
    return config

def validate_configuration(config):
    """
    Validate configuration parameters.
    
    Args:
        config: Config object to validate
        
    Raises:
        ValidationError: If configuration is invalid
    """
    if config is None:
        raise ValidationError("Configuration cannot be None")
    
    if not config.has("neuron_count"):
        raise ValidationError("Configuration must specify neuron_count")
    
    neuron_count = config.getOr("neuron_count", 0)
    if neuron_count <= 0:
        raise ValidationError("neuron_count must be positive")
    if neuron_count > 100000:
        raise ValidationError("neuron_count too large (max 100000)")
    
    if not config.has("region_count"):
        raise ValidationError("Configuration must specify region_count")
    
    region_count = config.getOr("region_count", 0)
    if region_count <= 0:
        raise ValidationError("region_count must be positive")
    
    if region_count > 100:
        raise ValidationError("region_count too large (max 100)")
    
    # Validate plasticity parameters
    if config.has("plasticity.stdp.learning_rate"):
        lrate = config.getOr("plasticity.stdp.learning_rate", 0.0)
        if lrate < 0.0 or lrate > 1.0:
            raise ValidationError("plasticity.stdp.learning_rate must be in [0.0, 1.0]")
    
    print(f"Configuration validated: {config.summary()}")

def validate_brain(brain):
    """
    Validate brain object state.
    
    Args:
        brain: Brain object to validate
        
    Raises:
        SimulationError: If brain is not properly initialized
    """
    if brain is None:
        raise SimulationError("Brain object cannot be None")
    
    # Check essential brain properties
    try:
        neuron_count = brain.getTotalNeuronCount()
        if neuron_count == 0:
            raise SimulationError("Brain has not been properly initialized")
        
        # Check if we can get a stats summary
        stats = brain.getTotalSynapseCount()
        if stats < 0:
            raise SimulationError("Invalid brain state: negative synapse count")
            
    except Exception as e:
        raise SimulationError(f"Invalid brain state: {str(e)}")

def create_simple_environment(width=20, height=20, vision_width=8, vision_height=8):
    """
    Create a simple 2D environment for NLM simulation.
    
    Args:
        width: World width in cells
        height: World height in cells  
        vision_width: Agent vision width
        vision_height: Agent vision height
        
    Returns:
        SimpleWorld: Configured world object
    """
    world = SimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    return world

def create_agent_environment(brain, world):
    """
    Create and initialize an agent brain interface.
    
    Args:
        brain: Brain object
        world: World object
        
    Returns:
        AgentBrain: Initialized agent brain
    """
    agent = AgentBrain(brain)
    agent.initialize(world)
    return agent

def quick_test_simulation(steps=100):
    """
    Run a quick test simulation to verify the system is working.
    
    Args:
        steps: Number of simulation steps to run
        
    Returns:
        dict: Test results
    """
    try:
        config = create_config()
        brain = create_brain(config)
        brain.initialize()
        
        results = {
            'steps_completed': 0,
            'final_firing_rate': 0.0,
            'total_spikes': 0,
            'success': False,
            'error_message': None
        }
        
        for step in range(steps):
            brain.step(step)
            if step % 10 == 0:
                results['final_firing_rate'] = brain.getAverageFiringRate()
                results['total_spikes'] = brain.getTotalSpikeCount()
        
        results['steps_completed'] = steps
        results['success'] = True
        return results
        
    except Exception as e:
        return {
            'steps_completed': 0,
            'final_firing_rate': 0.0,
            'total_spikes': 0,
            'success': False,
            'error_message': str(e)
        }

# Performance monitoring utilities
class PerformanceMonitor:
    """Monitor simulation performance metrics."""
    
    def __init__(self):
        self.start_time = None
        self.end_time = None
        self.metrics = {}
    
    def start(self):
        """Start performance monitoring."""
        import time
        self.start_time = time.time()
        self.metrics = {
            'steps_completed': 0,
            'avg_firing_rate': 0.0,
            'total_spikes': 0,
            'wall_time': 0.0,
            'steps_per_second': 0.0
        }
    
    def step_completed(self, step, firing_rate, total_spikes):
        """Record completion of a simulation step."""
        self.metrics['steps_completed'] = step + 1
        self.metrics['avg_firing_rate'] = firing_rate
        self.metrics['total_spikes'] = total_spikes
    
    def stop(self):
        """Stop performance monitoring and calculate final metrics."""
        import time
        if self.start_time:
            self.end_time = time.time()
            self.metrics['wall_time'] = self.end_time - self.start_time
            
            if self.metrics['wall_time'] > 0:
                self.metrics['steps_per_second'] = (
                    self.metrics['steps_completed'] / self.metrics['wall_time']
                )
    
    def get_metrics(self):
        """Get current performance metrics."""
        return self.metrics.copy()
    
    def __enter__(self):
        """Context manager entry."""
        self.start()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.stop()

# Backwards compatibility aliases
brain = create_brain  # For backwards compatibility
world = create_simple_environment  # For backwards compatibility
agent = create_agent_brain  # For backwards compatibility

# Public API exports
__all__ = [
    # Core classes
    'Config',
    'Brain',
    'AgentBrain',
    'SimpleWorld',
    'Vision',
    'Audio', 
    'InternalSignals',
    'SensoryInput',
    'Action',
    'ActionResult',
    'AgentBody',
    'SensoryPercept',
    
    # Factory functions
    'create_default_config',
    'create_brain',
    'create_simple_world',
    'create_agent_brain',
    
    # Convenience functions
    'run_simulation',
    'train_agent', 
    'batch_steps',
    'get_brain_stats',
    'save_checkpoint',
    'load_checkpoint',
    
    # Configuration helpers
    'create_config',
    'create_experiment_config',
    'validate_configuration',
    'validate_brain',
    
    # Environment helpers
    'create_simple_environment',
    'create_agent_environment',
    
    # Utilities
    'PerformanceMonitor',
    'quick_test_simulation',
    'get_version',
    'get_package_info',
    
    # Error classes
    'NLMRuntimeError',
    'ConfigurationError',
    'SimulationError',
    'MemoryError',
    'ValidationError',
    
    # Backwards compatibility
    'brain',
    'world',
    'agent'
]
