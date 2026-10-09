"""
Factory functions for creating NLM objects
"""

from typing import List, Dict, Any, Optional
from .core import Brain, Config, Action, Vision, Audio, InternalSignals, 
                    NeuronId, SynapseId, RegionId, PopulationId
from .agent import AgentBrain, AgentBody, SensoryPercept
from .world import SimpleWorld, WorldObject
from .sensory import Vision as VisionSensory, Audio as AudioSensory, InternalSignals as InternalSignalsSensory
from .exceptions import NLMError

def create_brain(config: Optional[Config] = None) -> Brain:
    """Create a new NLM brain"""
    if config is None:
        config = create_default_config()
    return Brain(config)

def create_default_config() -> Config:
    """Create a default configuration"""
    return Config.create_default()

def create_simple_world() -> SimpleWorld:
    """Create a simple 2D world"""
    return SimpleWorld()

def create_agent_brain(brain: Brain) -> AgentBrain:
    """Create an agent brain interface"""
    return AgentBrain(brain)

def create_sensory_percept() -> SensoryPercept:
    """Create a sensory percept"""
    return SensoryPercept()

def create_action(action_type: str = "wait", parameters: Optional[List[float]] = None) -> Action:
    """Create an action"""
    from .motor import Action as ActionClass, ActionType
    try:
        action_type_enum = ActionType(action_type)
    except ValueError:
        action_type_enum = ActionType.WAIT
    return ActionClass(action_type_enum, parameters)

def create_world_object(x: float = 0.0, y: float = 0.0, obj_type: str = "empty", 
                        value: float = 0.0, radius: float = 0.5) -> WorldObject:
    """Create a world object"""
    from .world import WorldObjectType
    try:
        obj_type_enum = WorldObjectType(obj_type)
    except ValueError:
        obj_type_enum = WorldObjectType.EMPTY
    return WorldObject(x, y, obj_type_enum, value, radius)

def create_vision(width: int = 0, height: int = 0, channels: int = 3) -> VisionSensory:
    """Create vision sensory input"""
    return VisionSensory(width, height, channels)

def create_audio(sample_rate: int = 0, num_samples: int = 0) -> AudioSensory:
    """Create audio sensory input"""
    return AudioSensory(sample_rate, num_samples)

def create_internal_signals() -> InternalSignalsSensory:
    """Create internal signals sensory input"""
    return InternalSignalsSensory()

def create_config_from_dict(config_dict: Dict[str, Any]) -> Config:
    """Create configuration from dictionary"""
    config = create_default_config()
    for key, value in config_dict.items():
        config.set(key, value)
    return config

def create_brain_from_config_dict(config_dict: Dict[str, Any]) -> Brain:
    """Create brain from configuration dictionary"""
    config = create_config_from_dict(config_dict)
    return create_brain(config)

def create_agent_from_brain_world(brain: Brain, world: SimpleWorld) -> AgentBrain:
    """Create and initialize agent from brain and world"""
    agent = create_agent_brain(brain)
    agent.initialize(world)
    return agent

def create_simulation_config(neuron_count: int = 1000, region_count: int = 1,
                             connection_probability: float = 0.1, 
                             simulation_timestep: float = 0.001) -> Config:
    """Create simulation configuration with common parameters"""
    config = create_default_config()
    
    # Set common configuration parameters
    config.set("neuron_count", neuron_count)
    config.set("region_count", region_count)
    config.set("connection_probability", connection_probability)
    config.set("simulation_timestep", simulation_timestep)
    config.set("random_seed", 42)  # Fixed seed for reproducibility
    
    return config

def create_brain_simulation_config(neuron_count: int = 1000) -> Config:
    """Create brain-specific configuration"""
    return create_simulation_config(neuron_count=neuron_count)

def create_world_config(width: int = 20, height: int = 20, 
                       vision_width: int = 8, vision_height: int = 8) -> Dict[str, Any]:
    """Create world configuration dictionary"""
    return {
        'width': width,
        'height': height,
        'vision_width': vision_width,
        'vision_height': vision_height
    }

# Default configuration factory functions
def create_small_world_config() -> Dict[str, Any]:
    """Create small world configuration"""
    return create_world_config(10, 10, 8, 8)

def create_medium_world_config() -> Dict[str, Any]:
    """Create medium world configuration"""
    return create_world_config(20, 20, 16, 16)

def create_large_world_config() -> Dict[str, Any]:
    """Create large world configuration"""
    return create_world_config(40, 40, 32, 32)

def create_default_brain_config() -> Config:
    """Create default brain configuration"""
    return create_simulation_config(neuron_count=1000, region_count=1)

def create_experimental_brain_config() -> Config:
    """Create experimental brain configuration"""
    return create_simulation_config(neuron_count=5000, region_count=3, 
                                   connection_probability=0.2)

def create_large_brain_config() -> Config:
    """Create large brain configuration"""
    return create_simulation_config(neuron_count=10000, region_count=5, 
                                   connection_probability=0.05)
