"""
Enhanced Python bindings for NLM (Neural Learning Machine)

This module provides the Python interface to the NLM C++ neural simulation framework,
featuring modern Python APIs, NumPy integration, visualization capabilities, and
comprehensive examples.

The module provides:
- Type hints and autocomplete support
- Context managers for resource management
- NumPy integration for sensor data
- Matplotlib and Plotly visualization
- Performance profiling and benchmarking
- Checkpoint/restore capabilities
- Learning and development examples
- Neuromodulation examples
- Advanced world interaction
"""

import sys
import os
import warnings
from typing import List, Optional, Dict, Any, Iterator, Union, Tuple
import numpy as np

# Configure logging before any other imports
import logging
_logging_initialized = False

def _get_logger(name: str = "nlm"):
    global _logging_initialized
    if not _logging_initialized:
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        _logging_initialized = True
    return logging.getLogger(name)

logger = _get_logger()

# Module metadata
__version__ = "1.0.0"
__author__ = "NLM Development Team"
__description__ = "Enhanced Python bindings for NLM neural simulation framework"
__license__ = "MIT"

# Platform detection
import platform
if platform.system() == "Linux":
    __platform__ = "Linux"
elif platform.system() == "Windows":
    __platform__ = "Windows"
elif platform.system() == "Darwin":
    __platform__ = "macOS"
else:
    __platform__ = "Unknown"

# Module-level configuration
_config = {
    'backend': 'numpy',
    'device': 'cpu',
    'precision': 'float32',
    'matplotlib_backend': 'Agg',
    'enable_profiling': False,
    'checkpoint_dir': './checkpoints',
    'log_level': 'INFO',
    'random_seed': 42
}

# Check Python version
if sys.version_info < (3, 8):
    warnings.warn("NLM requires Python 3.8 or higher", DeprecationWarning)

# Check NumPy version
if np.__version__ < "1.20.0":
    warnings.warn("NLM recommends NumPy 1.20.0 or higher", DeprecationWarning)

# Check Matplotlib version
import matplotlib
if matplotlib.__version__ < "3.0.0":
    warnings.warn("NLM recommends Matplotlib 3.0.0 or higher", DeprecationWarning)

# Type aliases for better API
tensor = np.ndarray
epoch = float
step = int
neuron_id = int
synapse_id = int
population_id = int
region_id = int

# Custom exception classes
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ValidationError(NLMError):
    """Error for configuration validation failures."""
    pass

class ConfigurationError(NLMError):
    """Error for configuration errors."""
    pass

class DeviceError(NLMError):
    """Error for device-related issues."""
    pass

# Context managers
class Timer:
    """Context manager for timing operations."""
    def __init__(self, name: str = "operation"):
        self.name = name
        self.start_time = None
        
    def __enter__(self):
        import time
        self.start_time = time.time()
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        import time
        if self.start_time is not None:
            duration = time.time() - self.start_time
            logger.info(f"{self.name}: {duration:.4f}s")

class BrainSession:
    """Context manager for brain simulation sessions."""
    def __init__(self, brain):
        self.brain = brain
        
    def __enter__(self):
        self.brain.initialize()
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.brain.reset()

class WorldSession:
    """Context manager for world simulation sessions."""
    def __init__(self, world):
        self.world = world
        
    def __enter__(self):
        self.world.reset()
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        pass

# Data structures
class SensorData:
    """Container for sensor data with NumPy integration."""
    def __init__(self, vision=None, audio=None, internal=None, touch=None, timestamp=0.0):
        self.vision = vision
        self.audio = audio
        self.internal = internal
        self.touch = touch
        self.timestamp = timestamp
        
    @classmethod
    def from_vision(cls, vision_data, height=64, width=64, channels=3):
        """Create SensorData from vision data.
        
        Args:
            vision_data: Vision data as numpy array
            height: Vision sensor height
            width: Vision sensor width
            channels: Number of vision channels
            
        Returns:
            SensorData object
        """
        if vision_data.ndim == 2:
            vision_data = vision_data.reshape(height, width, channels)
        elif vision_data.ndim == 3:
            vision_data = vision_data.transpose(2, 0, 1)  # CHW format
        return cls(vision=vision_data, timestamp=0.0)

class BrainMetrics:
    """Container for brain performance metrics."""
    def __init__(self, total_neurons=0, active_neurons=0, total_synapses=0,
                 firing_rate=0.0, efficiency=0.0, memory_usage=0.0,
                 steps_per_second=0.0, energy_consumed=0.0):
        self.total_neurons = total_neurons
        self.active_neurons = active_neurons
        self.total_synapses = total_synapses
        self.firing_rate = firing_rate
        self.efficiency = efficiency
        self.memory_usage = memory_usage
        self.steps_per_second = steps_per_second
        self.energy_consumed = energy_consumed
        
    @property
    def neuron_density(self):
        """Calculate neuron density."""
        return self.total_neurons / max(1.0, self.memory_usage)
    
    @property
    def energy_efficiency(self):
        """Calculate energy efficiency."""
        return self.firing_rate / max(1.0, self.energy_consumed)

# Mock ID classes (when C++ bindings are not available)
class NeuronId:
    """Mock NeuronId class."""
    def __init__(self, value=0):
        self.value = value
        
    def __repr__(self):
        return f"<NeuronId: {self.value}>"

class SynapseId:
    """Mock SynapseId class."""
    def __init__(self, value=0):
        self.value = value
        
    def __repr__(self):
        return f"<SynapseId: {self.value}>"

class RegionId:
    """Mock RegionId class."""
    def __init__(self, value=0):
        self.value = value
        
    def __repr__(self):
        return f"<RegionId: {self.value}>"

class PopulationId:
    """Mock PopulationId class."""
    def __init__(self, value=0):
        self.value = value
        
    def __repr__(self):
        return f"<PopulationId: {self.value}>"

# Mock enums
class NeuronType:
    """Mock NeuronType enum."""
    Excitatory = "Excitatory"
    Inhibitory = "Inhibitory"
    Modulatory = "Modulatory"
    Sensory = "Sensory"
    Motor = "Motor"
    Internal = "Internal"

class SynapseType:
    """Mock SynapseType enum."""
    Excitatory = "Excitatory"
    Inhibitory = "Inhibitory"
    Modulatory = "Modulatory"
    Electrical = "Electrical"
    GapJunction = "GapJunction"

class DevelopmentalStage:
    """Mock DevelopmentalStage enum."""
    Initial = "Initial"
    CriticalPeriod = "CriticalPeriod"
    Maturation = "Maturation"
    Adult = "Adult"
    Aging = "Aging"

class FiringState:
    """Mock FiringState enum."""
    Resting = "Resting"
    Active = "Active"
    Refractory = "Refractory"
    Inhibited = "Inhibited"

class ActionType:
    """Mock ActionType enum."""
    MoveForward = "MoveForward"
    MoveBackward = "MoveBackward"
    MoveLeft = "MoveLeft"
    MoveRight = "MoveRight"
    TurnLeft = "TurnLeft"
    TurnRight = "TurnRight"
    Look = "Look"
    LookUp = "LookUp"
    LookDown = "LookDown"
    Interact = "Interact"
    Eat = "Eat"
    Drink = "Drink"
    Rest = "Rest"
    Wait = "Wait"
    Custom = "Custom"

class MotorCommand:
    """Mock MotorCommand enum."""
    MoveForward = "MoveForward"
    MoveBackward = "MoveBackward"
    TurnLeft = "TurnLeft"
    TurnRight = "TurnRight"
    LookLeft = "LookLeft"
    LookRight = "LookRight"
    Interact = "Interact"
    Wait = "Wait"

class WorldObjectType:
    """Mock WorldObjectType enum."""
    Empty = "Empty"
    Resource = "Resource"
    Hazard = "Hazard"
    Wall = "Wall"
    Marker = "Marker"

# Mock constants
INVALID_NEURON_ID = NeuronId()
INVALID_SYNAPSE_ID = SynapseId()
INVALID_REGION_ID = RegionId()
INVALID_POPULATION_ID = PopulationId()

# Mock C++ classes
class Config:
    """Mock configuration class."""
    def __init__(self):
        self.data = {}
        
    def loadFromFile(self, filepath):
        print(f"Mock: Loading config from {filepath}")
        return True
        
    def saveToFile(self, filepath):
        print(f"Mock: Saving config to {filepath}")
        return True
        
    def has(self, key):
        return key in self.data
        
    def getKeys(self):
        return list(self.data.keys())
        
    def clear(self):
        self.data.clear()
        
    def summary(self):
        return f"Mock Config with {len(self.data)} entries"
        
    def __repr__(self):
        return f"<MockConfig: {self.summary()}>"

class SimpleWorld:
    """Mock simple world class."""
    def __init__(self):
        self.width = 0.0
        self.height = 0.0
        
    def configure(self, width, height, vision_width, vision_height):
        self.width = width
        self.height = height
        print(f"Mock: Configured world {width}x{height} with vision {vision_width}x{vision_height}")
        
    def reset(self):
        print("Mock: World reset")
        
    def setAgentStart(self, x, y):
        print(f"Mock: Agent start set to ({x}, {y})")
        
    def update(self, timestep):
        print(f"Mock: World updated with timestep {timestep}")
        
    def getWidth(self):
        return self.width
        
    def getHeight(self):
        return self.height
        
    def getAgentBody(self):
        return MockAgentBody()
        
    def getSensoryPercept(self):
        return MockSensoryPercept()

class MockAgentBody:
    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocityX = 0.0
        self.velocityY = 0.0
        self.angularVelocity = 0.0
        self.energy = 100.0
        self.health = 100.0
        self.age = 0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = 0.0
        
    def reset(self):
        self.energy = 100.0
        self.health = 100.0
        self.age = 0
        self.isMoving = False
        self.isTurning = False

class MockSensoryPercept:
    def __init__(self):
        pass
        
    def getVision(self):
        return np.zeros((64, 64, 3))

class Brain:
    """Mock brain class."""
    def __init__(self, config):
        self.config = config
        
    def getTotalNeuronCount(self):
        return 100
        
    def getTotalSynapseCount(self):
        return 1000
        
    def getActiveNeuronCount(self):
        return 10
        
    def getAverageFiringRate(self):
        return 2.5
        
    def save(self, filepath):
        print(f"Mock: Brain saved to {filepath}")
        return True
        
    def load(self, filepath):
        print(f"Mock: Brain loaded from {filepath}")
        return True

class MockAction:
    def __init__(self, action_type="MoveForward"):
        self.action_type = action_type
        
    def getType(self):
        return self.action_type
        
    def getName(self):
        return self.action_type

class AgentBrain:
    """Mock agent brain class."""
    def __init__(self, brain):
        self.brain = brain
        
    def initialize(self, world):
        print("Mock: Agent brain initialized")
        
    def processSensoryInput(self, percept):
        print("Mock: Agent brain processed sensory input")
        
    def decodeMotorCommand(self):
        return "MoveForward"
        
    def reset(self):
        print("Mock: Agent brain reset")
        
    def getBrain(self):
        return self.brain

# Helper functions
def createDefaultConfig():
    return Config()
    

def createBrain(config):
    return Brain(config)
    

def createSimpleWorld():
    return SimpleWorld()
    

def createAgentBrain(brain):
    return AgentBrain(brain)

# Enhanced classes
class NeuralSystem:
    """High-level interface for NLM with modern Python features."""
    
    def __init__(self, config=None, device='cpu'):
        """Initialize neural system.
        
        Args:
            config: Configuration object
            device: Device to use ('cpu' or 'cuda')
        """
        if config is None:
            config = createDefaultConfig()
        
        self._brain = createBrain(config)
        self._agent_brain = None
        self._world = None
        self._device = device
        self._metrics_history = []
        self._episode_rewards = []
        
    @property
    def brain(self):
        """Get the underlying brain."""
        return self._brain
    
    @property
    def metrics(self):
        """Get metrics history."""
        return self._metrics_history
    
    @property
    def episode_rewards(self):
        """Get episode rewards."""
        return self._episode_rewards
    
    def create_world(self, width=100.0, height=100.0, 
                     vision_width=64, vision_height=64):
        """Create and configure a world.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision sensor width
            vision_height: Vision sensor height
            
        Returns:
            Configured world
        """
        self._world = createSimpleWorld()
        self._world.configure(width, height, vision_width, vision_height)
        
        # Create agent brain if brain exists
        if self._brain:
            self._agent_brain = createAgentBrain(self._brain)
            
        return self._world
    
    def create_agent(self, x=0.0, y=0.0):
        """Create agent at specified position.
        
        Args:
            x: Initial x position
            y: Initial y position
            
        Returns:
            Agent body
        """
        if self._world:
            self._world.setAgentStart(x, y)
        
        return self._agent_brain.getBrain().getAgentBody() if self._agent_brain else None
    
    def step(self, sensor_data=None):
        """Perform one simulation step.
        
        Args:
            sensor_data: Sensor input data
            
        Returns:
            Tuple of (action, reward, success)
        """
        # Convert sensor data to percept if provided
        if sensor_data is not None and self._world:
            percept = self._world.getSensoryPercept()
            
            # Update brain with sensor data
            self._brain.step(0)  # Simple step for now
            
        # Produce action
        action = self._brain.produceAction()
        
        # Execute action in world if available
        if self._world and action:
            result = self._world.applyMotorCommand(action.getType(), 0.0)
            return action, result.reward, result.success
            
        return action, 0.0, False
    
    def run_episode(self, max_steps=1000):
        """Run a complete episode.
        
        Args:
            max_steps: Maximum number of steps
            
        Returns:
            List of (action, reward, success) tuples and total reward
        """
        with BrainSession(self._brain):
            with WorldSession(self._world):
                episode_data = []
                total_reward = 0.0
                
                for step in range(max_steps):
                    action, reward, success = self.step()
                    episode_data.append((action, reward, success))
                    total_reward += reward
                    
                    # Record reward for metrics
                    self._episode_rewards.append(reward)
                    
                    # Check if episode should end
                    if success or reward < -1000:  # Failure condition
                        break
                        
                return episode_data, total_reward
    
    def get_metrics(self):
        """Get current brain metrics.
        
        Returns:
            Current metrics
        """
        total_neurons = self._brain.getTotalNeuronCount()
        active_neurons = self._brain.getActiveNeuronCount()
        total_synapses = self._brain.getTotalSynapseCount()
        firing_rate = self._brain.getAverageFiringRate()
        
        # Calculate efficiency
        efficiency = (active_neurons / total_neurons) if total_neurons > 0 else 0.0
        
        metrics = BrainMetrics(
            total_neurons=total_neurons,
            active_neurons=active_neurons,
            total_synapses=total_synapses,
            firing_rate=firing_rate,
            efficiency=efficiency
        )
        
        self._metrics_history.append(metrics)
        return metrics
    
    def save_checkpoint(self, path=None):
        """Save brain checkpoint.
        
        Args:
            path: Optional path, uses default if None
            
        Returns:
            Path where checkpoint was saved
        """
        if path is None:
            path = f"checkpoints/brain_checkpoint_{np.random.random():.2f}.pth"
            
        # Ensure directory exists
        os.makedirs(os.path.dirname(path), exist_ok=True)
        
        # Save brain state
        self._brain.save(path)
        return path
    
    def load_checkpoint(self, path):
        """Load brain checkpoint.
        
        Args:
            path: Path to checkpoint file
            
        Returns:
            True if successful, False otherwise
        """
        return self._brain.load(path)

class DataManager:
    """Manage data collection and analysis."""
    
    def __init__(self):
        self.data = {}
        
    def add_episode_data(self, episode_id, data):
        """Add episode data."""
        self.data[episode_id] = data
        
    def get_average_reward(self, episodes=None):
        """Get average reward for specified episodes."""
        if episodes is None:
            episodes = list(self.data.keys())
            
        rewards = []
        for ep_id in episodes:
            if 'rewards' in self.data[ep_id]:
                rewards.extend(self.data[ep_id]['rewards'])
                
        return np.mean(rewards) if rewards else 0.0
    
    def get_reward_distribution(self, episodes=None, bins=10):
        """Get reward distribution histogram."""
        if episodes is None:
            episodes = list(self.data.keys())
            
        all_rewards = []
        for ep_id in episodes:
            if 'rewards' in self.data[ep_id]:
                all_rewards.extend(self.data[ep_id]['rewards'])
                
        return np.histogram(all_rewards, bins=bins)[0]

class PerformanceProfiler:
    """Profile and analyze performance."""
    
    def __init__(self):
        self.timings = []
        self.memory_usage = []
        
    def start_profiling(self):
        """Start performance profiling."""
        import psutil
        import time
        
        self.process = psutil.Process()
        self.start_time = time.time()
        self.initial_memory = self.process.memory_info().rss
        
    def end_profiling(self):
        """End performance profiling and record metrics."""
        import time
        import psutil
        
        if hasattr(self, 'process'):
            end_time = time.time()
            final_memory = self.process.memory_info().rss
            
            self.timings.append(end_time - self.start_time)
            self.memory_usage.append((final_memory - self.initial_memory) / 1024 / 1024)  # MB
            
    @property
    def avg_time(self):
        """Get average time."""
        return np.mean(self.timings) if self.timings else 0.0
        
    @property
    def avg_memory(self):
        """Get average memory usage."""
        return np.mean(self.memory_usage) if self.memory_usage else 0.0

class Visualizer:
    """Visualization tools for NLM results."""
    
    def __init__(self, backend='matplotlib'):
        self.backend = backend
        self.figures = []
        
    def plot_metrics(self, metrics, save_path=None):
        """Plot brain metrics.
        
        Args:
            metrics: List of BrainMetrics objects
            save_path: Optional path to save plot
        """
        if self.backend == 'matplotlib':
            import matplotlib.pyplot as plt
            
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            
            # Plot 1: Neuron counts
            axes[0, 0].plot([m.total_neurons for m in metrics])
            axes[0, 0].set_title('Total Neurons')
            axes[0, 0].set_xlabel('Step')
            axes[0, 0].set_ylabel('Count')
            
            # Plot 2: Firing rate
            axes[0, 1].plot([m.firing_rate for m in metrics])
            axes[0, 1].set_title('Firing Rate')
            axes[0, 1].set_xlabel('Step')
            axes[0, 1].set_ylabel('Rate')
            
            # Plot 3: Efficiency
            axes[1, 0].plot([m.efficiency for m in metrics])
            axes[1, 0].set_title('Efficiency')
            axes[1, 0].set_xlabel('Step')
            axes[1, 0].set_ylabel('Efficiency')
            
            plt.tight_layout()
            
            if save_path:
                plt.savefig(save_path)
            
            plt.show()
            
        elif self.backend == 'plotly':
            import plotly.graph_objects as go
            
            # Create interactive plots with Plotly
            data = {
                'total_neurons': [m.total_neurons for m in metrics],
                'active_neurons': [m.active_neurons for m in metrics],
                'firing_rate': [m.firing_rate for m in metrics],
                'efficiency': [m.efficiency for m in metrics]
            }
            
            fig = go.Figure()
            fig.add_trace(go.Scatter(x=list(range(len(metrics))), 
                                    y=data['total_neurons'],
                                    mode='lines',
                                    name='Total Neurons'))
            
            if save_path:
                fig.write_html(save_path)
            
            fig.show()
    
    def plot_brain_connectivity(self, brain, save_path=None):
        """Plot brain connectivity graph.
        
        Args:
            brain: Brain object
            save_path: Optional path to save plot
        """
        print("Brain connectivity visualization not implemented yet")
        
    def plot_world_dynamics(self, world, save_path=None):
        """Plot world dynamics.
        
        Args:
            world: World object
            save_path: Optional path to save plot
        """
        print("World dynamics visualization not implemented yet")

# Example modules
class LearningExamples:
    """Examples for learning and memory."""
    
    @staticmethod
    def goal_directed_learning(brain, world, target):
        """Example: Goal-directed learning."""
        print("Goal-directed learning example not implemented yet")
        
    @staticmethod
    def habit_formation(brain, reward, action):
        """Example: Habit formation."""
        print("Habit formation example not implemented yet")

class DevelopmentExamples:
    """Examples for development system."""
    
    @staticmethod
    def critical_period(brain, stage):
        """Example: Critical period development."""
        print("Critical period example not implemented yet")
        
    @staticmethod
    def maturation(brain, stage):
        """Example: Maturation development."""
        print("Maturation example not implemented yet")

class NeuromodulationExamples:
    """Examples for neuromodulation."""
    
    @staticmethod
    def reward_modulation(agent_brain, reward, predicted_reward):
        """Example: Reward modulation."""
        print("Reward modulation example not implemented yet")
        
    @staticmethod
    def curiosity(agent_brain, novelty):
        """Example: Curiosity."""
        print("Curiosity example not implemented yet")
        
    @staticmethod
    def novelty(agent_brain, novelty):
        """Example: Novelty."""
        print("Novelty example not implemented yet")

class EnvironmentExamples:
    """Examples for complex environments."""
    
    @staticmethod
    def multi_agent_scenario(world, agents):
        """Example: Multi-agent scenario."""
        print("Multi-agent scenario example not implemented yet")

class PerformanceExamples:
    """Examples for performance and monitoring."""
    
    @staticmethod
    def benchmark(brain, steps, neurons_per_step):
        """Example: Performance benchmarking."""
        print("Benchmarking example not implemented yet")

class CheckpointExamples:
    """Examples for checkpoint/restore."""
    
    @staticmethod
    def save_and_restore(brain, path):
        """Example: Save and restore brain state."""
        print("Checkpoint example not implemented yet")

# Utility functions
def configure_logging(level='INFO'):
    """Configure logging.
    
    Args:
        level: Logging level (DEBUG, INFO, WARNING, ERROR)
    """
    global _config
    _config['log_level'] = level
    logger.setLevel(getattr(logging, level.upper()))

def set_config(key, value):
    """Set global configuration.
    
    Args:
        key: Configuration key
        value: Configuration value
    """
    global _config
    _config[key] = value

def get_config(key, default=None):
    """Get global configuration.
    
    Args:
        key: Configuration key
        default: Default value if key not found
        
    Returns:
        Configuration value or default
    """
    return _config.get(key, default)

# Exception raising helper functions
def raise_validation_error(message):
    """Raise validation error.
    
    Args:
        message: Error message
    """
    raise ValidationError(message)

def raise_configuration_error(message):
    """Raise configuration error.
    
    Args:
        message: Error message
    """
    raise ConfigurationError(message)

def raise_device_error(message):
    """Raise device error.
    
    Args:
        message: Error message
    """
    raise DeviceError(message)

# Module initialization
if __name__ == '__main__':
    configure_logging()
    print("NLM Python bindings initialized successfully")
