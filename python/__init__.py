"""pynlm - Improved Python Bindings for NLM

This is the improved Python bindings module for the NLM (Neural Learning Machine) framework.
It provides enhanced Pythonic API over the original C++ bindings.

Key Improvements:
- Pythonic property access (brain.neuron_count instead of brain.getTotalNeuronCount())
- Convenience functions for common operations
- Automatic file extension handling
- Better error messages and documentation
- Batch operations for multiple simulations
- Improved configuration access

Usage Example:

```python
import pynlm

# Create a simple agent simulation in one line
brain, world, agent = pynlm.createSimpleAgentSimulation()

# Run a complete simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Use Pythonic property access
print(f"Brain has {brain.neuron_count} neurons")
print(f"Firing rate: {brain.average_firing_rate:.2f}")

# Save with automatic extension
brain.save("my_brain")

# Load with automatic extension
brain2 = pynlm.createBrain(config)
brain2.load("my_brain")

# Run multiple simulations in batch
simulations = [pynlm.createSimpleAgentSimulation() for _ in range(5)]
firing_rates = pynlm.run_simulation_multiple(simulations, num_steps=50)
```

For detailed API documentation, refer to the individual class and function docstrings.
"""

__version__ = "0.1.0"
__author__ = "NLM Authors"
__all__ = [
    # Core classes
    'Brain', 'Config', 'AgentBrain', 'SimpleWorld',
    'SensoryPercept', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult',
    
    # Enumerations
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Configuration helper
    'createDefaultConfig',
    'createDefaultConfigWithSettings',
    
    # Convenience functions
    'createSimpleAgentSimulation',
    'run_simulation_multiple',
    'saveBrainWithAutoExtension',
    'loadBrainWithAutoExtension',
    
    # Factory functions
    'createBrain', 'createSimpleWorld', 'createAgentBrain',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
]

# Import all classes from the _pynlm module
try:
    from ._pynlm import (
        # Core classes
        Brain, Config, AgentBrain, SimpleWorld,
        SensoryPercept, Vision, Audio, InternalSignals,
        Action, WorldObject, AgentBody, ActionResult,
        
        # Enumerations
        NeuronType, SynapseType, DevelopmentalStage,
        FiringState, ActionType, MotorCommand, WorldObjectType,
        
        # Configuration helper
        createDefaultConfig,
        createDefaultConfigWithSettings,
        
        # Convenience functions
        createSimpleAgentSimulation,
        run_simulation_multiple,
        saveBrainWithAutoExtension,
        loadBrainWithAutoExtension,
        
        # Factory functions
        createBrain, createSimpleWorld, createAgentBrain,
        
        # Constants
        INVALID_NEURON_ID, INVALID_SYNAPSE_ID,
        INVALID_REGION_ID, INVALID_POPULATION_ID,
    )
    
    # Create a Pythonic wrapper for Config class
    class ConfigWrapper:
        """Enhanced configuration class with Pythonic access patterns.
        
        This class provides convenient access to configuration values using
        dot notation and property-style access while maintaining full
        compatibility with the underlying C++ Config class.
        """
        
        def __init__(self, config):
            self._config = config
        
        def get(self, key, default=None):
            """Get a configuration value.
            
            Args:
                key: Configuration key (e.g., "brain.neuron_count")
                default: Default value if key not found
                
            Returns:
                Configuration value or default
            """
            value = self._config.get(key)
            return value if value is not None else default
        
        def __getitem__(self, key):
            """Access configuration value using bracket notation.
            
            Args:
                key: Configuration key
                
            Returns:
                Configuration value
                
            Raises:
                KeyError: If key not found
            """
            value = self._config.get(key)
            if value is None:
                raise KeyError(f"Configuration key '{key}' not found")
            return value
        
        def __setitem__(self, key, value):
            """Set configuration value using bracket notation.
            
            Args:
                key: Configuration key
                value: Configuration value
            """
            self._config.set(key, value)
        
        def __contains__(self, key):
            """Check if configuration key exists.
            
            Args:
                key: Configuration key
                
            Returns:
                True if key exists, False otherwise
            """
            return self._config.has(key)
        
        def has(self, key):
            """Check if configuration key exists.
            
            Args:
                key: Configuration key
                
            Returns:
                True if key exists, False otherwise
            """
            return self._config.has(key)
        
        def keys(self):
            """Get all configuration keys.
            
            Returns:
                List of configuration keys
            """
            return self._config.getKeys()
        
        def values(self):
            """Get all configuration values.
            
            Returns:
                Dictionary of key-value pairs
            """
            return {key: self._config.get(key) 
                   for key in self._config.getKeys()
                   if self._config.get(key) is not None}
        
        def items(self):
            """Get all configuration key-value pairs.
            
            Returns:
                List of (key, value) tuples
            """
            return [(key, self._config.get(key))
                   for key in self._config.getKeys()
                   if self._config.get(key) is not None]
        
        def clear(self):
            """Clear all configuration entries."""
            self._config.clear()
        
        def summary(self):
            """Get configuration summary.
            
            Returns:
                Summary string
            """
            return self._config.summary()
        
        def __repr__(self):
            return f"ConfigWrapper({len(self.keys())} entries)"
        
        # Convenience properties for brain configuration
        @property
        def brain_neuron_count(self):
            """Get brain neuron count."""
            return self.get('brain.neuron_count', 1000)
        
        @brain_neuron_count.setter
        def brain_neuron_count(self, value):
            """Set brain neuron count."""
            self.set('brain.neuron_count', value)
        
        @property
        def brain_synapse_density(self):
            """Get brain synapse density."""
            return self.get('brain.synapse_density', 0.1)
        
        @brain_synapse_density.setter
        def brain_synapse_density(self, value):
            """Set brain synapse density."""
            self.set('brain.synapse_density', value)
        
        @property
        def brain_connection_probability(self):
            """Get brain connection probability."""
            return self.get('brain.connection_probability', 0.05)
        
        @brain_connection_probability.setter
        def brain_connection_probability(self, value):
            """Set brain connection probability."""
            self.set('brain.connection_probability', value)
        
        @property
        def brain_v_thresh(self):
            """Get brain threshold potential."""
            return self.get('brain.v_thresh', -50.0)
        
        @brain_v_thresh.setter
        def brain_v_thresh(self, value):
            """Set brain threshold potential."""
            self.set('brain.v_thresh', value)
        
        @property
        def brain_v_rest(self):
            """Get brain resting potential."""
            return self.get('brain.v_rest', -70.0)
        
        @brain_v_rest.setter
        def brain_v_rest(self, value):
            """Set brain resting potential."""
            self.set('brain.v_rest', value)
        
        @property
        def brain_v_reset(self):
            """Get brain reset potential."""
            return self.get('brain.v_reset', -75.0)
        
        @brain_v_reset.setter
        def brain_v_reset(self, value):
            """Set brain reset potential."""
            self.set('brain.v_reset', value)
        
        @property
        def brain_tau_mem(self):
            """Get brain membrane time constant."""
            return self.get('brain.tau_mem', 20.0)
        
        @brain_tau_mem.setter
        def brain_tau_mem(self, value):
            """Set brain membrane time constant."""
            self.set('brain.tau_mem', value)
        
        @property
        def brain_tau_ref(self):
            """Get brain refractory period."""
            return self.get('brain.tau_ref', 2.0)
        
        @brain_tau_ref.setter
        def brain_tau_ref(self, value):
            """Set brain refractory period."""
            self.set('brain.tau_ref', value)
        
        # Convenience methods
        def enable_plasticity(self, enable=True):
            """Enable or disable plasticity.
            
            Args:
                enable: True to enable, False to disable
            """
            self.set('plasticity.enabled', enable)
        
        def set_stdp_params(self, learning_rate=0.001, tau_plus=20.0, tau_minus=20.0):
            """Set STDP parameters.
            
            Args:
                learning_rate: STDP learning rate
                tau_plus: LTP time constant
                tau_minus: LTD time constant
            """
            self.set('plasticity.stdp.learning_rate', learning_rate)
            self.set('plasticity.stdp.tau_plus', tau_plus)
            self.set('plasticity.stdp.tau_minus', tau_minus)
        
        def set_neuromodulation(self, dopamine_scale=1.0, curiosity_enabled=True, novelty_enabled=True):
            """Set neuromodulation parameters.
            
            Args:
                dopamine_scale: Dopamine modulation scale
                curiosity_enabled: Whether curiosity is enabled
                novelty_enabled: Whether novelty detection is enabled
            """
            self.set('neuromod.dopamine.scale', dopamine_scale)
            self.set('neuromod.curiosity.enable', curiosity_enabled)
            self.set('neuromod.novelty.enable', novelty_enabled)
        
        def save_to_file(self, filepath):
            """Save configuration to JSON file.
            
            Args:
                filepath: File path to save to
            """
            self._config.saveToFile(filepath)
        
        def load_from_file(self, filepath):
            """Load configuration from JSON file.
            
            Args:
                filepath: File path to load from
                
            Returns:
                True if successful, False otherwise
            """
            return self._config.loadFromFile(filepath)
        
        def load_from_args(self, argc, argv):
            """Load configuration from command line arguments.
            
            Args:
                argc: Argument count
                argv: Argument vector
                
            Returns:
                True if successful, False otherwise
            """
            return self._config.loadFromArgs(argc, argv)
    
    # Monkey-patch the Config class to use the wrapper
    original_config = Config
    
    class Config(original_config):
        """Enhanced configuration class with Pythonic API.
        
        This is a subclass of the original Config class that provides
        enhanced Pythonic access patterns through the ConfigWrapper.
        """
        
        def __init__(self):
            super().__init__()
            self._wrapper = ConfigWrapper(self)
        
        def wrap(self):
            """Get a wrapper for Pythonic access.
            
            Returns:
                ConfigWrapper instance
            """
            return self._wrapper
        
        # Forward all original methods
        def get(self, key, default=None):
            """Get configuration value using Pythonic API."""
            return self._wrapper.get(key, default)
        
        def set(self, key, value, source=None):
            """Set configuration value using Pythonic API."""
            self._wrapper[key] = value
        
        def has(self, key):
            """Check if key exists."""
            return self._wrapper.has(key)
        
        def getKeys(self):
            """Get all keys."""
            return self._wrapper.keys()
        
        def getOr(self, key, default_value):
            """Get value or return default."""
            value = self._wrapper.get(key, None)
            return value if value is not None else default_value
        
        def clear(self):
            """Clear all configuration."""
            self._wrapper.clear()
        
        def summary(self):
            """Get configuration summary."""
            return self._wrapper.summary()
        
        def saveToFile(self, filepath):
            """Save to JSON file."""
            return self._wrapper.save_to_file(filepath)
        
        def loadFromFile(self, filepath):
            """Load from JSON file."""
            return self._wrapper.load_from_file(filepath)
        
        def loadFromArgs(self, argc, argv):
            """Load from command line arguments."""
            return self._wrapper.load_from_args(argc, argv)
        
        # Special Pythonic attribute access
        def __getattr__(self, name):
            """Allow access to configuration via dot notation."""
            # Convert snake_case to dot notation
            key = name.replace('_', '.')
            if self._wrapper.has(key):
                return self._wrapper.get(key)
            raise AttributeError(f"'Config' object has no attribute '{name}'")
        
        def __setattr__(self, name, value):
            """Allow setting configuration via dot notation."""
            # Skip private attributes
            if name.startswith('_'):
                super().__setattr__(name, value)
                return
            
            # Convert snake_case to dot notation
            key = name.replace('_', '.')
            self._wrapper[key] = value
        
        def __getitem__(self, key):
            """Allow bracket notation access."""
            return self._wrapper[key]
        
        def __setitem__(self, key, value):
            """Allow bracket notation setting."""
            self._wrapper[key] = value
        
        def __contains__(self, key):
            """Check if key exists."""
            return self._wrapper.has(key)
        
        def __repr__(self):
            return f"Config({len(self._wrapper.keys())} entries)"
        
        # Special helper methods for brain configuration
        def get_brain_neuron_count(self, default=1000):
            """Get brain neuron count with default."""
            return self.getOr('brain.neuron_count', default)
        
        def set_brain_neuron_count(self, value):
            """Set brain neuron count."""
            self.set('brain.neuron_count', value)
        
        def get_simulation_dt(self, default=0.1):
            """Get simulation timestep with default."""
            return self.getOr('simulation.dt', default)
        
        def set_simulation_dt(self, value):
            """Set simulation timestep."""
            self.set('simulation.dt', value)
        
        def get_plasticity_stdp_learning_rate(self, default=0.001):
            """Get STDP learning rate with default."""
            return self.getOr('plasticity.stdp.learning_rate', default)
        
        def set_plasticity_stdp_learning_rate(self, value):
            """Set STDP learning rate."""
            self.set('plasticity.stdp.learning_rate', value)
        
        def enable_structural_plasticity(self, enable=True):
            """Enable or disable structural plasticity."""
            self.set('plasticity.structural.enable', enable)
        
        def enable_curiosity(self, enable=True):
            """Enable or disable curiosity."""
            self.set('neuromod.curiosity.enable', enable)
        
        def enable_novelty(self, enable=True):
            """Enable or disable novelty detection."""
            self.set('neuromod.novelty.enable', enable)
        
        def set_development_stage(self, stage):
            """Set developmental stage."""
            self.set('development.initial_stage', stage)
        
        def get_development_stage(self, default='Initial'):
            """Get developmental stage with default."""
            return self.getOr('development.initial_stage', default)
        
        def apply_neuromodulation_settings(self, dopamine_scale=1.0, curiosity_enabled=True, novelty_enabled=True):
            """Apply neuromodulation settings."""
            self.set('neuromod.dopamine.scale', dopamine_scale)
            self.set('neuromod.curiosity.enable', curiosity_enabled)
            self.set('neuromod.novelty.enable', novelty_enabled)
    
    # Replace the original Config class with our enhanced version
    # Note: This is a simplified approach - in practice, you might want to use
    # a different approach to preserve backward compatibility
    Config.__doc__ = """Enhanced configuration class with Pythonic API.
    
    This class provides convenient access to configuration values using
    dot notation and property-style access while maintaining full
    compatibility with the underlying C++ Config class.
    
    Examples:
        # Using Pythonic API
        config = Config()
        config.brain_neuron_count = 2000
        config.brain_v_thresh = -55.0
        config.set_simulation_dt(0.05)
        
        # Using bracket notation
        config['brain.neuron_count'] = 1500
        neuron_count = config['brain.neuron_count']
        
        # Using attribute-style access
        config.set_brain_neuron_count(3000)
        count = config.get_brain_neuron_count(500)
        
        # Save configuration
        config.save_to_file('my_config.json')
        config.load_from_file('my_config.json')
    """

except ImportError:
    # If _pynlm module is not available (e.g., during development)
    # Create placeholder classes
    
    class Brain:
        def __init__(self):
            pass
    
    class Config:
        def __init__(self):
            pass
    
    class AgentBrain:
        def __init__(self):
            pass
    
    class SimpleWorld:
        def __init__(self):
            pass
    
    # Placeholder constants
    INVALID_NEURON_ID = None
    INVALID_SYNAPSE_ID = None
    INVALID_REGION_ID = None
    INVALID_POPULATION_ID = None

# Create module-level documentation
__doc__ += """

Compatibility Notes:
- All original C++ API functions are preserved for backward compatibility
- The original low-level API is still available via the _pynlm module
- For advanced users, direct access to C++ classes is available
- Performance-critical code can still use the original API

Migration Guide:
- Use brain.neuron_count instead of brain.getTotalNeuronCount()
- Use config.brain_neuron_count instead of config.set('brain.neuron_count', value)
- Use createSimpleAgentSimulation() for common setup patterns
- Use automatic file extensions with saveBrainWithAutoExtension() and loadBrainWithAutoExtension()
- Use run_simulation_multiple() for batch operations
"""
