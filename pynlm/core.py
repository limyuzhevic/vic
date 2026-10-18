"""
Core NLM Python bindings with enhanced Pythonic features.

This module provides enhanced Python bindings on top of the C++ NLM framework,
adding Pythonic conventions, error handling, context managers, and utility functions.
"""

import os
import json
import time
import contextlib
from typing import Dict, List, Optional, Union, Any, Callable
from pathlib import Path

# Try to import C++ bindings
try:
    from .cpp_bindings import (
        Brain,
        AgentBrain,
        SimpleWorld,
        Config,
        Action,
        SensoryPercept,
        WorldObject,
        Vision,
        Audio,
        InternalSignals,
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
        createDefaultConfig,
        createBrain,
        createSimpleWorld,
        createAgentBrain,
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
    )
    HAS_CPP_BINDINGS = True
except ImportError:
    HAS_CPP_BINDINGS = False
    # Create placeholder classes for testing/development
    Brain = AgentBrain = SimpleWorld = Config = Action = None
    Vision = Audio = InternalSignals = None
    NeuronId = SynapseId = RegionId = PopulationId = None
    NeuronType = SynapseType = DevelopmentalStage = FiringState = None
    ActionType = MotorCommand = WorldObjectType = None

# Error handling classes
class NLMError(Exception):
    """Base exception class for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Error in configuration or setup."""
    pass

class SimulationError(NLMError):
    """Error during simulation."""
    pass

class MemoryError(NLMError):
    """Error related to memory operations."""
    pass

class ValidationError(NLMError):
    """Error in input validation."""
    pass

# Enhanced configuration management
class ConfigManager:
    """Enhanced configuration management with validation and utilities."""
    
    def __init__(self, config_file=None):
        if HAS_CPP_BINDINGS:
            self._config = createDefaultConfig()
        else:
            self._config = None
        self.config_file = config_file
        
    def load_from_file(self, filepath, validate=True):
        """Load configuration from file with validation."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        if not os.path.exists(filepath):
            raise FileNotFoundError(f"Configuration file not found: {filepath}")
            
        # Try to load as JSON first
        if filepath.endswith('.json'):
            with open(filepath, 'r') as f:
                data = json.load(f)
                self._config.clear()
                for key, value in data.items():
                    self._config.set(key, value)
        else:
            # Fall back to original file-based loading
            self._config.loadFromFile(filepath)
            
        self.config_file = filepath
        return self
        
    def save_to_file(self, filepath, pretty=False):
        """Save configuration to file with optional pretty printing."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        if filepath.endswith('.json'):
            data = {}
            keys = self._config.getKeys()
            for key in keys:
                data[key] = self._config.get(key)
                
            with open(filepath, 'w') as f:
                if pretty:
                    json.dump(data, f, indent=2)
                else:
                    json.dump(data, f)
        else:
            self._config.saveToFile(filepath)
            
        return self
        
    def validate(self):
        """Validate configuration."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        # Basic validation
        if not self._config:
            raise ConfigurationError("No configuration available")
            
        # Check for required keys
        required_keys = ['brain.neuron_count']
        for key in required_keys:
            if not self._config.has(key):
                raise ConfigurationError(f"Missing required configuration key: {key}")
                
        return True
        
    def set(self, key, value):
        """Set configuration value."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        self._config.set(key, value)
        return self
        
    def get(self, key, default=None):
        """Get configuration value."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        return self._config.get(key, default)
        
    def has(self, key):
        """Check if configuration has key."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        return self._config.has(key)
        
    def get_keys(self):
        """Get all configuration keys."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        return self._config.getKeys()
        
    def summary(self):
        """Get configuration summary."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        return self._config.summary()
        
    def __repr__(self):
        return f"ConfigManager(config_file={self.config_file})"

# Enhanced Brain class with Pythonic features
class EnhancedBrain:
    """Enhanced Brain class with Pythonic features and utilities."""
    
    def __init__(self, config=None):
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        if config is None:
            self.config = createDefaultConfig()
        else:
            self.config = config
            
        self._brain = createBrain(self.config)
        self._brain.initialize()
        self._initialized = True
        self._step_count = 0
        self._start_time = time.time()
        
    def initialize(self):
        """Initialize the brain."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        self._brain.initialize()
        self._initialized = True
        self._step_count = 0
        self._start_time = time.time()
        return self
        
    def step(self, step_number, current_time=None):
        """Perform a simulation step."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        if not self._initialized:
            raise SimulationError("Brain not initialized. Call initialize() first.")
            
        if current_time is not None:
            self._brain.step(step_number, current_time)
        else:
            self._brain.step(step_number)
            
        self._step_count += 1
        return self
        
    def reset(self):
        """Reset brain state."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        self._brain.reset()
        self._step_count = 0
        self._start_time = time.time()
        return self
        
    def receive_sensory_input(self, sensory_input):
        """Receive sensory input."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        self._brain.receiveSensoryInput(sensory_input)
        return self
        
    def inject_current(self, neuron_id, current):
        """Inject current into a specific neuron."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        self._brain.injectCurrent(neuron_id, current)
        return self
        
    def inject_current_to_neurons(self, neuron_type, current):
        """Inject current into all neurons of a specific type."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        self._brain.injectCurrentToNeurons(neuron_type, current)
        return self
        
    # Statistics and monitoring
    def get_stats(self):
        """Get comprehensive brain statistics."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        return {
            'total_steps': self._step_count,
            'elapsed_time': time.time() - self._start_time,
            'firing_neurons': self._brain.getFiringNeuronCount(),
            'active_neurons': self._brain.getActiveNeuronCount(),
            'total_neurons': self._brain.getTotalNeuronCount(),
            'total_synapses': self._brain.getTotalSynapseCount(),
            'total_spikes': self._brain.getTotalSpikeCount(),
            'average_firing_rate': self._brain.getAverageFiringRate(),
            'excitatory_inhibition_ratio': self._brain.getExcitationInhibitionRatio(),
            'developmental_stage': self._brain.getDevelopmentalStage(),
            'brain_status': self._brain.logStatus(),
        }
        
    # Context manager
    def __enter__(self):
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.reset()
        
    def __repr__(self):
        return f"EnhancedBrain(steps={self._step_count}, neurons={self._brain.getTotalNeuronCount()})"

# Enhanced AgentBrain class
class EnhancedAgentBrain:
    """Enhanced AgentBrain class with Pythonic features."""
    
    def __init__(self, brain):
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        self._brain = brain
        self._agent = createAgentBrain(brain)
        
    def initialize(self, world):
        """Initialize agent with world."""
        self._agent.initialize(world)
        return self
        
    def process_sensory_input(self, percept):
        """Process sensory input."""
        self._agent.processSensoryInput(percept)
        return self
        
    def decode_motor_command(self):
        """Decode motor command from brain activity."""
        return self._agent.decodeMotorCommand()
        
    def apply_reward_modulation(self, reward, predicted_reward):
        """Apply reward-based neuromodulation."""
        self._agent.applyRewardModulation(reward, predicted_reward)
        return self
        
    def update_development(self, timestep):
        """Update development system."""
        self._agent.updateDevelopment(timestep)
        return self
        
    # Subsystem enable/disable
    def enable_features(self, features):
        """Enable multiple features at once."""
        if isinstance(features, dict):
            for feature, enabled in features.items():
                method_name = f"enable_{feature}"
                if hasattr(self._agent, method_name):
                    getattr(self._agent, method_name)(enabled)
        return self
        
    def get_features(self):
        """Get current feature states."""
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        return {
            'reward_modulation': self._agent.isRewardModulationEnabled(),
            'structural_plasticity': self._agent.isStructuralPlasticityEnabled(),
            'development': self._agent.isDevelopmentEnabled(),
            'curiosity': self._agent.isCuriosityEnabled(),
            'neuromodulation_level': self._agent.getNeuromodulationLevel(),
            'curiosity_level': self._agent.getCuriosityLevel(),
            'novelty_level': self._agent.getNoveltyLevel(),
            'prediction_error': self._agent.getPredictionError(),
            'developmental_stage': self._agent.getDevelopmentalStage(),
        }
        
    def __repr__(self):
        return f"EnhancedAgentBrain(features={self.get_features()})"

# Enhanced SimpleWorld class
class EnhancedSimpleWorld:
    """Enhanced SimpleWorld class with Pythonic features."""
    
    def __init__(self):
        if not HAS_CPP_BINDINGS:
            raise ImportError("C++ bindings not available")
            
        self._world = createSimpleWorld()
        
    def configure(self, width=20, height=20, vision_width=8, vision_height=8):
        """Configure world dimensions."""
        self._world.configure(width, height, vision_width, vision_height)
        return self
        
    def reset(self):
        """Reset world to initial state."""
        self._world.reset()
        return self
        
    def update(self, timestep):
        """Update world by timestep."""
        self._world.update(timestep)
        return self
        
    def set_agent_start(self, x, y):
        """Set agent starting position."""
        self._world.setAgentStart(x, y)
        return self
        
    def apply_motor_command(self, command, current_time):
        """Apply motor command to world."""
        self._world.applyMotorCommand(command, current_time)
        return self
        
    def get_sensory_percept(self):
        """Get sensory percept."""
        return self._world.getSensoryPercept()
        
    def get_agent_body(self):
        """Get agent body state."""
        return self._world.getAgentBody()
        
    def add_object(self, obj):
        """Add object to world."""
        self._world.addObject(obj)
        return self
        
    def remove_object(self, x, y):
        """Remove object from world."""
        self._world.removeObject(x, y)
        return self
        
    def is_valid_position(self, x, y):
        """Check if position is valid."""
        return self._world.isValidPosition(x, y)
        
    # World properties
    def get_properties(self):
        """Get world properties."""
        return {
            'width': self._world.getWidth(),
            'height': self._world.getHeight(),
            'simulation_time': self._world.getSimulationTime(),
            'max_energy': self._world.getMaxEnergy(),
            'energy_decay_rate': self._world.getEnergyDecayRate(),
            'random_seed': self._world.getRandomSeed(),
        }
        
    def set_random_seed(self, seed):
        """Set random seed."""
        self._world.setRandomSeed(seed)
        return self
        
    def __repr__(self):
        props = self.get_properties()
        return f"EnhancedSimpleWorld(width={props['width']}, height={props['height']}, time={props['simulation_time']})"

# Utility functions

def create_default_config():
    """Create a default configuration."""
    if HAS_CPP_BINDINGS:
        return ConfigManager(createDefaultConfig())
    else:
        return ConfigManager()

def create_enhanced_brain(config=None):
    """Create an enhanced brain."""
    if HAS_CPP_BINDINGS:
        return EnhancedBrain(config)
    else:
        raise ImportError("C++ bindings not available")

def create_enhanced_agent_brain(brain):
    """Create an enhanced agent brain."""
    if HAS_CPP_BINDINGS:
        return EnhancedAgentBrain(brain)
    else:
        raise ImportError("C++ bindings not available")

def create_enhanced_world():
    """Create an enhanced world."""
    if HAS_CPP_BINDINGS:
        return EnhancedSimpleWorld()
    else:
        raise ImportError("C++ bindings not available")

def enable_features(agent_brain, features):
    """Enable multiple features on agent brain."""
    agent_brain.enable_features(features)
    return agent_brain

def brain_stats(brain):
    """Get brain statistics dictionary."""
    return brain.get_stats()

def run_simulation(brain, num_steps, enable_features=None):
    """Run a complete simulation."""
    if not isinstance(brain, EnhancedBrain):
        brain = EnhancedBrain(brain)
        
    if enable_features:
        brain.enable_features(enable_features)
        
    for step in range(num_steps):
        brain.step(step)
        
    return brain.get_stats()

def run_complete_agent_simulation(brain, world, agent, num_steps):
    """Run a complete agent simulation with brain and world."""
    if not isinstance(brain, EnhancedBrain):
        brain = EnhancedBrain(brain)
    if not isinstance(world, EnhancedSimpleWorld):
        world = EnhancedSimpleWorld()
        world.configure(world.get_width(), world.get_height())
    if not isinstance(agent, EnhancedAgentBrain):
        agent = EnhancedAgentBrain(brain)
        
    agent.initialize(world)
    
    for step in range(num_steps):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        brain.step(step)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
    return {
        'brain_stats': brain.get_stats(),
        'agent_features': agent.get_features(),
        'world_properties': world.get_properties(),
    }

def benchmark_simulation(brain_factory, num_steps=1000, num_runs=5):
    """Benchmark a simulation."""
    import time
    
    results = []
    for run in range(num_runs):
        brain = brain_factory()
        start_time = time.time()
        
        for step in range(num_steps):
            brain.step(step)
            
        end_time = time.time()
        elapsed = end_time - start_time
        stats = brain.get_stats()
        
        results.append({
            'run': run,
            'elapsed_time': elapsed,
            'steps_per_second': num_steps / elapsed,
            'brain_stats': stats,
        })
        
    return results

def save_simulation_checkpoint(brain, filepath):
    """Save simulation checkpoint."""
    if not HAS_CPP_BINDINGS:
        raise ImportError("C++ bindings not available")
        
    return brain.save(filepath)

def load_simulation_checkpoint(brain, filepath):
    """Load simulation checkpoint."""
    if not HAS_CPP_BINDINGS:
        raise ImportError("C++ bindings not available")
        
    return brain.load(filepath)

# Context manager utilities
@contextlib.contextmanager
def brain_session(config=None, world_config=None):
    """
    Context manager for managing complete brain simulation sessions.
    
    Example:
        with brain_session() as session:
            brain = session.create_brain()
            world = session.create_world()
            agent = session.create_agent()
            
            # Run simulation...
            for step in range(100):
                world.update(0.1)
                agent.process_sensory_input(world.get_sensory_percept())
                brain.step(step)
                action = agent.decode_motor_command()
                world.apply_motor_command(action, world.get_simulation_time())
    """
    brain = EnhancedBrain(config)
    world = EnhancedSimpleWorld()
    
    if world_config:
        world.configure(**world_config)
        
    agent = EnhancedAgentBrain(brain)
    agent.initialize(world)
    
    try:
        yield type('Session', (), {
            'brain': brain,
            'world': world,
            'agent': agent,
        })()
    finally:
        brain.reset()
        world.reset()
        agent.reset()

# Main exports
if HAS_CPP_BINDINGS:
    # Re-export C++ classes for compatibility
    Brain = EnhancedBrain
    AgentBrain = EnhancedAgentBrain
    SimpleWorld = EnhancedSimpleWorld
    Config = ConfigManager
    
    # Also provide original C++ classes
    __all__ = [
        # Core classes
        'Brain', 'AgentBrain', 'SimpleWorld', 'Config',
        
        # Identifier types (from C++ bindings)
        'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
        
        # Enumerations
        'NeuronType', 'SynapseType', 'DevelopmentalStage', 
        'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
        
        # Factory functions
        'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
        
        # Enhanced versions
        'EnhancedBrain', 'EnhancedAgentBrain', 'EnhancedSimpleWorld', 'ConfigManager',
        
        # Utility functions
        'enable_features', 'brain_stats', 'run_simulation', 'run_complete_agent_simulation',
        'benchmark_simulation', 'save_simulation_checkpoint', 'load_simulation_checkpoint',
        
        # Context managers
        'brain_session',
        
        # Error classes
        'NLMError', 'ConfigurationError', 'SimulationError', 'MemoryError', 'ValidationError',
        
        # Constants
        'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    ]
else:
    __all__ = [
        # Error classes
        'NLMError', 'ConfigurationError', 'SimulationError', 'MemoryError', 'ValidationError',
        
        # Factory functions (will raise ImportError if C++ bindings not available)
        'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
        
        # Enhanced versions (will raise ImportError if C++ bindings not available)
        'EnhancedBrain', 'EnhancedAgentBrain', 'EnhancedSimpleWorld', 'ConfigManager',
        
        # Utility functions
        'enable_features', 'brain_stats', 'run_simulation', 'run_complete_agent_simulation',
        'benchmark_simulation', 'save_simulation_checkpoint', 'load_simulation_checkpoint',
        
        # Context managers
        'brain_session',
    ]

# Set up C++ bindings import if available
if not HAS_CPP_BINDINGS:
    try:
        from .cpp_bindings import (
            Brain as CPlusPlusBrain,
            AgentBrain as CPlusPlusAgentBrain,
            SimpleWorld as CPlusPlusSimpleWorld,
            Config as CPlusPlusConfig,
            Action,
            SensoryPercept,
            WorldObject,
            Vision,
            Audio,
            InternalSignals,
            NeuronId,
            SynapseId,
            RegionId,
            PopulationId,
            NeuronType,
            SynapseType,
            DevelopmentalStage,
            FiringState,
            ActionType,
            MotorCommand,
            WorldObjectType,
            createDefaultConfig as cpp_createDefaultConfig,
            createBrain as cpp_createBrain,
            createSimpleWorld as cpp_createSimpleWorld,
            createAgentBrain as cpp_createAgentBrain,
            INVALID_NEURON_ID,
            INVALID_SYNAPSE_ID,
            INVALID_REGION_ID,
            INVALID_POPULATION_ID,
        )
        
        # Override placeholder classes with real C++ bindings
        HAS_CPP_BINDINGS = True
        
    except ImportError:
        pass