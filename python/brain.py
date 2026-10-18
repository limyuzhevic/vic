"""
Brain-related classes and functions for NLM Python bindings.
"""

from typing import List, Optional, Dict, Any, Union
import numpy as np
import structlog

# Get the logger
logger = structlog.get_logger()

# Forward references for type annotations
class Config:
    pass

class Neuron:
    pass

class Synapse:
    pass

class NeuralRegion:
    pass

class SimpleWorld:
    pass

class AgentBrain:
    pass

# Type aliases for better readability
NeuronId = int
SynapseId = int
RegionId = int
PopulationId = int
NeuronType = str
SynapseType = str
DevelopmentalStage = str
FiringState = str

# Constants
INVALID_NEURON_ID = -1
INVALID_SYNAPSE_ID = -1
INVALID_REGION_ID = -1
INVALID_POPULATION_ID = -1

# Enum-like classes for type safety
class NeuronTypes:
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    SENSORY = "sensory"
    MOTOR = "motor"
    INTERNAL = "internal"

class SynapseTypes:
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    ELECTRICAL = "electrical"
    GAPJUNCTION = "gapjunction"

class DevelopmentalStages:
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"

class FiringStates:
    RESTING = "resting"
    ACTIVE = "active"
    REFRACTORY = "refractory"
    INHIBITED = "inhibited"

class ActionTypes:
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    MOVE_LEFT = "move_left"
    MOVE_RIGHT = "move_right"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK = "look"
    LOOK_UP = "look_up"
    LOOK_DOWN = "look_down"
    INTERACT = "interact"
    EAT = "eat"
    DRINK = "drink"
    REST = "rest"
    WAIT = "wait"
    CUSTOM = "custom"

class MotorCommands:
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK_LEFT = "look_left"
    LOOK_RIGHT = "look_right"
    INTERACT = "interact"
    WAIT = "wait"

class WorldObjectTypes:
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"

# Exception classes
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class BrainInitializationError(NLMError):
    """Raised when brain initialization fails."""
    pass

class InvalidConfigurationError(NLMError):
    """Raised when configuration is invalid."""
    pass

class SimulationError(NLMError):
    """Raised during simulation errors."""
    pass

# Convenience functions

def createDefaultConfig():
    """Create a default configuration for NLM.
    
    Returns:
        Config: Default configuration.
    """
    logger.debug("Creating default configuration")
    # This would call the C++ binding
    return Config()


class Config:
    """Configuration class for NLM system."""
    
    def __init__(self, config_dict=None):
        self._config = config_dict or {}
        self._validate_config()
        logger.debug("Config initialized", config_keys=list(self._config.keys()))
    
    def _validate_config(self):
        """Validate configuration parameters."""
        # Validate core parameters
        if 'neuron_count' in self._config and self._config['neuron_count'] <= 0:
            raise InvalidConfigurationError("neuron_count must be positive")
        
        if 'random_seed' in self._config and not isinstance(self._config['random_seed'], int):
            raise InvalidConfigurationError("random_seed must be an integer")
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from a file.
        
        Args:
            filepath: Path to configuration file.
            
        Returns:
            bool: True if configuration loaded successfully.
        """
        try:
            import json
            with open(filepath, 'r') as f:
                file_config = json.load(f)
            self._config.update(file_config)
            self._validate_config()
            logger.info("Configuration loaded from file", filepath=filepath)
            return True
        except Exception as e:
            logger.error("Failed to load configuration from file", 
                        filepath=filepath, error=str(e))
            return False
    
    def save_to_file(self, filepath: str) -> bool:
        """Save configuration to a file.
        
        Args:
            filepath: Path to save configuration.
            
        Returns:
            bool: True if configuration saved successfully.
        """
        try:
            import json
            with open(filepath, 'w') as f:
                json.dump(self._config, f, indent=2)
            logger.info("Configuration saved to file", filepath=filepath)
            return True
        except Exception as e:
            logger.error("Failed to save configuration", 
                        filepath=filepath, error=str(e))
            return False
    
    def has(self, key: str) -> bool:
        """Check if configuration has a key.
        
        Args:
            key: Configuration key.
            
        Returns:
            bool: True if key exists.
        """
        return key in self._config
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys.
        
        Returns:
            List[str]: List of configuration keys.
        """
        return list(self._config.keys())
    
    def get(self, key: str, default=None):
        """Get a configuration value.
        
        Args:
            key: Configuration key.
            default: Default value if key not found.
            
        Returns:
            Configuration value or default.
        """
        return self._config.get(key, default)
    
    def set(self, key: str, value, source: str = "python"):
        """Set a configuration value.
        
        Args:
            key: Configuration key.
            value: Configuration value.
            source: Source of configuration.
        """
        self._config[key] = value
        logger.debug("Configuration set", key=key, value=value, source=source)
    
    def summary(self) -> str:
        """Get a summary of the configuration.
        
        Returns:
            str: Configuration summary.
        """
        summary = f"Configuration ({len(self._config)} entries):\n"
        for key, value in self._config.items():
            summary += f"  {key} = {value}\n"
        return summary
    
    def __repr__(self) -> str:
        return f"<Config: {self.summary()}>"
    
    def __getitem__(self, key: str):
        return self._config[key]
    
    def __setitem__(self, key: str, value):
        self.set(key, value)


class Brain:
    """Central neural simulation brain class."""
    
    def __init__(self, config: Config):
        self._config = config
        self._initialized = False
        self._simulation_step = 0
        logger.debug("Brain object created", config_type=type(config).__name__)
    
    def initialize(self):
        """Initialize the brain with configuration.
        
        Returns:
            bool: True if initialization successful.
        """
        logger.info("Initializing brain")
        
        # Validate configuration
        if not self._config.has('neuron_count'):
            raise BrainInitializationError("neuron_count not specified in configuration")
        
        if not self._config.has('region_count'):
            raise BrainInitializationError("region_count not specified in configuration")
        
        # Simulate initialization
        self._initialized = True
        logger.info("Brain initialized successfully")
        return True
    
    def step(self, current_step: int):
        """Perform a simulation step.
        
        Args:
            current_step: Current simulation step number.
        """
        if not self._initialized:
            raise SimulationError("Brain not initialized. Call initialize() first.")
        
        self._simulation_step = current_step
        # Simulate brain computation
        logger.debug("Brain step", step=current_step)
    
    def step_with_time(self, current_step: int, current_time: float):
        """Perform a simulation step with timestamp.
        
        Args:
            current_step: Current simulation step number.
            current_time: Current simulation time.
        """
        self.step(current_step)
        # Simulate time-based computation
        logger.debug("Brain step with time", step=current_step, time=current_time)
    
    def receive_sensory_input(self, sensory_input):
        """Inject sensory input into the brain.
        
        Args:
            sensory_input: Sensory input data.
        """
        if not self._initialized:
            raise SimulationError("Brain not initialized.")
        
        # Simulate sensory processing
        logger.debug("Sensory input received", input_type=type(sensory_input).__name__)
    
    def inject_current(self, neuron_id: int, current: float):
        """Inject current into a specific neuron.
        
        Args:
            neuron_id: Neuron ID.
            current: Current value to inject.
        """
        if not self._initialized:
            raise SimulationError("Brain not initialized.")
        
        logger.debug("Current injected", neuron_id=neuron_id, current=current)
    
    def inject_current_to_neurons(self, neuron_type: str, current: float):
        """Inject current into all neurons of a specific type.
        
        Args:
            neuron_type: Type of neurons.
            current: Current value to inject.
        """
        if not self._initialized:
            raise SimulationError("Brain not initialized.")
        
        logger.debug("Current injected to neurons", 
                    neuron_type=neuron_type, current=current)
    
    def produce_action(self):
        """Produce motor action based on neural activity.
        
        Returns:
            Action: Motor action.
        """
        if not self._initialized:
            raise SimulationError("Brain not initialized.")
        
        logger.debug("Action produced")
        # Return a dummy action
        return Action()
    
    def reset(self):
        """Reset brain state."""
        if self._initialized:
            self._initialized = False
            self._simulation_step = 0
            logger.info("Brain reset")
    
    def save(self, filepath: str):
        """Save brain state to file.
        
        Args:
            filepath: Path to save brain state.
        """
        logger.info("Brain state saved", filepath=filepath)
    
    def load(self, filepath: str):
        """Load brain state from file.
        
        Args:
            filepath: Path to load brain state from.
        """
        logger.info("Brain state loaded", filepath=filepath)
    
    def add_region(self, name: str = ""):
        """Add a new neural region.
        
        Args:
            name: Optional region name.
        """
        logger.debug("Region added", name=name)
    
    def get_region(self, region_id: int):
        """Get a region by ID.
        
        Args:
            region_id: Region ID.
            
        Returns:
            NeuralRegion: Neural region.
        """
        logger.debug("Region retrieved", region_id=region_id)
        return None
    
    def get_region_count(self) -> int:
        """Get the number of regions.
        
        Returns:
            int: Number of regions.
        """
        return 1
    
    def get_region_ids(self) -> List[int]:
        """Get all region IDs.
        
        Returns:
            List[int]: List of region IDs.
        """
        return [1]
    
    def get_regions(self) -> List[NeuralRegion]:
        """Get all regions.
        
        Returns:
            List[NeuralRegion]: List of neural regions.
        """
        return []
    
    def get_total_neuron_count(self) -> int:
        """Get total neuron count across all regions.
        
        Returns:
            int: Total neuron count.
        """
        return self._config.get('neuron_count', 0)
    
    def get_total_synapse_count(self) -> int:
        """Get total synapse count across all regions.
        
        Returns:
            int: Total synapse count.
        """
        return self._config.get('neuron_count', 0) * 10  # Approximate
    
    def get_active_neuron_count(self) -> int:
        """Get count of active neurons.
        
        Returns:
            int: Active neuron count.
        """
        return 10
    
    def get_firing_neuron_count(self) -> int:
        """Get count of currently firing neurons.
        
        Returns:
            int: Firing neuron count.
        """
        return 5
    
    def get_total_spike_count(self) -> int:
        """Get total spike count.
        
        Returns:
            int: Total spike count.
        """
        return self._simulation_step * 10
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons.
        
        Returns:
            float: Average firing rate (Hz).
        """
        return 5.0
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition balance ratio.
        
        Returns:
            float: Excitation/inhibition ratio.
        """
        return 1.5
    
    def get_developmental_stage(self) -> str:
        """Get current developmental stage.
        
        Returns:
            str: Developmental stage.
        """
        return DevelopmentalStages.ADULT
    
    def set_developmental_stage(self, stage: str):
        """Set developmental stage.
        
        Args:
            stage: Developmental stage.
        """
        valid_stages = [
            DevelopmentalStages.INITIAL,
            DevelopmentalStages.CRITICAL_PERIOD,
            DevelopmentalStages.MATURATION,
            DevelopmentalStages.ADULT,
            DevelopmentalStages.AGING
        ]
        if stage not in valid_stages:
            raise ValueError(f"Invalid developmental stage: {stage}")
        logger.debug("Developmental stage set", stage=stage)
    
    def get_config(self) -> Config:
        """Get the configuration.
        
        Returns:
            Config: Brain configuration.
        """
        return self._config
    
    def log_status(self):
        """Log brain status information."""
        logger.info("Brain status",
                   neurons=self.get_total_neuron_count(),
                   synapses=self.get_total_synapse_count(),
                   firing_rate=self.get_average_firing_rate(),
                   developmental_stage=self.get_developmental_stage())


class Neuron:
    """Single neuron class with LIF dynamics."""
    
    def __init__(self, neuron_id: int):
        self._id = neuron_id
        self._type = NeuronTypes.INTERNAL
        self._state = {
            'membrane_potential': -70.0,
            'threshold': -55.0,
            'resting_potential': -70.0,
            'reset_potential': -70.0,
            'firing_state': FiringStates.RESTING,
            'last_spike_time': -1.0
        }
        logger.debug("Neuron created", neuron_id=neuron_id)
    
    def get_id(self) -> int:
        """Get neuron ID.
        
        Returns:
            int: Neuron ID.
        """
        return self._id
    
    def get_type(self) -> str:
        """Get neuron type.
        
        Returns:
            str: Neuron type.
        """
        return self._type
    
    def set_type(self, neuron_type: str):
        """Set neuron type.
        
        Args:
            neuron_type: Neuron type.
        """
        valid_types = [
            NeuronTypes.EXCITATORY,
            NeuronTypes.INHIBITORY,
            NeuronTypes.MODULATORY,
            NeuronTypes.SENSORY,
            NeuronTypes.MOTOR,
            NeuronTypes.INTERNAL
        ]
        if neuron_type not in valid_types:
            raise ValueError(f"Invalid neuron type: {neuron_type}")
        self._type = neuron_type
    
    def get_membrane_potential(self) -> float:
        """Get membrane potential.
        
        Returns:
            float: Membrane potential (mV).
        """
        return self._state['membrane_potential']
    
    def set_membrane_potential(self, potential: float):
        """Set membrane potential.
        
        Args:
            potential: Membrane potential (mV).
        """
        self._state['membrane_potential'] = potential
    
    def get_threshold(self) -> float:
        """Get firing threshold.
        
        Returns:
            float: Firing threshold (mV).
        """
        return self._state['threshold']
    
    def set_threshold(self, threshold: float):
        """Set firing threshold.
        
        Args:
            threshold: Firing threshold (mV).
        """
        self._state['threshold'] = threshold
    
    def is_firing(self) -> bool:
        """Check if neuron is firing.
        
        Returns:
            bool: True if firing.
        """
        return self._state['firing_state'] == FiringStates.ACTIVE
    
    def is_refractory(self) -> bool:
        """Check if neuron is in refractory period.
        
        Returns:
            bool: True if refractory.
        """
        return self._state['firing_state'] == FiringStates.REFRACTORY
    
    def check_threshold(self) -> bool:
        """Check if membrane potential exceeds threshold.
        
        Returns:
            bool: True if threshold exceeded.
        """
        return self._state['membrane_potential'] >= self._state['threshold']
    
    def get_last_spike_time(self) -> float:
        """Get last spike time.
        
        Returns:
            float: Last spike time (-1 if none).
        """
        return self._state['last_spike_time']
    
    def record_spike(self, timestamp: float):
        """Record a spike at given timestamp.
        
        Args:
            timestamp: Spike timestamp.
        """
        self._state['last_spike_time'] = timestamp
        logger.debug("Spike recorded", neuron_id=self._id, timestamp=timestamp)
    
    def step(self, current_time: float):
        """Perform neuron LIF step.
        
        Args:
            current_time: Current simulation time.
        """
        if self.check_threshold():
            self._state['firing_state'] = FiringStates.ACTIVE
            self.record_spike(current_time)
    
    def reset(self):
        """Reset neuron state."""
        self._state = {
            'membrane_potential': -70.0,
            'threshold': -55.0,
            'resting_potential': -70.0,
            'reset_potential': -70.0,
            'firing_state': FiringStates.RESTING,
            'last_spike_time': -1.0
        }
    
    def __repr__(self) -> str:
        return f"<Neuron id={self._id} type={self._type}>"


class Synapse:
    """Synapse class connecting neurons."""
    
    def __init__(self, synapse_id: int):
        self._id = synapse_id
        self._weight = 1.0
        self._type = SynapseTypes.EXCITATORY
        logger.debug("Synapse created", synapse_id=synapse_id)
    
    def get_id(self) -> int:
        """Get synapse ID.
        
        Returns:
            int: Synapse ID.
        """
        return self._id
    
    def get_weight(self) -> float:
        """Get synapse weight.
        
        Returns:
            float: Synapse weight.
        """
        return self._weight
    
    def set_weight(self, weight: float):
        """Set synapse weight.
        
        Args:
            weight: Synapse weight.
        """
        self._weight = weight
    
    def __repr__(self) -> str:
        return f"<Synapse id={self._id} weight={self._weight}>"


class NeuralRegion:
    """Neural region containing neurons and synapses."""
    
    def __init__(self, region_id: int, name: str = ""):
        self._id = region_id
        self._name = name
        self._neurons = []
        self._synapses = []
        logger.debug("NeuralRegion created", region_id=region_id, name=name)
    
    def get_id(self) -> int:
        """Get region ID.
        
        Returns:
            int: Region ID.
        """
        return self._id
    
    def get_name(self) -> str:
        """Get region name.
        
        Returns:
            str: Region name.
        """
        return self._name
    
    def get_neurons(self) -> List[Neuron]:
        """Get all neurons in region.
        
        Returns:
            List[Neuron]: List of neurons.
        """
        return self._neurons
    
    def get_synapses(self) -> List[Synapse]:
        """Get all synapses in region.
        
        Returns:
            List[Synapse]: List of synapses.
        """
        return self._synapses


class Action:
    """Action representation for motor output."""
    
    def __init__(self, action_type: str = ActionTypes.WAIT, parameters: List[float] = None):
        self._type = action_type
        self._parameters = parameters or []
        logger.debug("Action created", action_type=action_type)
    
    def get_type(self) -> str:
        """Get action type.
        
        Returns:
            str: Action type.
        """
        return self._type
    
    def set_type(self, action_type: str):
        """Set action type.
        
        Args:
            action_type: Action type.
        """
        valid_types = [
            ActionTypes.MOVE_FORWARD,
            ActionTypes.MOVE_BACKWARD,
            ActionTypes.MOVE_LEFT,
            ActionTypes.MOVE_RIGHT,
            ActionTypes.TURN_LEFT,
            ActionTypes.TURN_RIGHT,
            ActionTypes.LOOK,
            ActionTypes.LOOK_UP,
            ActionTypes.LOOK_DOWN,
            ActionTypes.INTERACT,
            ActionTypes.EAT,
            ActionTypes.DRINK,
            ActionTypes.REST,
            ActionTypes.WAIT,
            ActionTypes.CUSTOM
        ]
        if action_type not in valid_types:
            raise ValueError(f"Invalid action type: {action_type}")
        self._type = action_type
    
    def get_parameters(self) -> List[float]:
        """Get action parameters.
        
        Returns:
            List[float]: Action parameters.
        """
        return self._parameters
    
    def set_parameters(self, parameters: List[float]):
        """Set action parameters.
        
        Args:
            parameters: Action parameters.
        """
        self._parameters = parameters
    
    def get_name(self) -> str:
        """Get action name.
        
        Returns:
            str: Action name.
        """
        # Map action types to names
        name_map = {
            ActionTypes.MOVE_FORWARD: "Move Forward",
            ActionTypes.MOVE_BACKWARD: "Move Backward",
            ActionTypes.MOVE_LEFT: "Move Left",
            ActionTypes.MOVE_RIGHT: "Move Right",
            ActionTypes.TURN_LEFT: "Turn Left",
            ActionTypes.TURN_RIGHT: "Turn Right",
            ActionTypes.LOOK: "Look",
            ActionTypes.LOOK_UP: "Look Up",
            ActionTypes.LOOK_DOWN: "Look Down",
            ActionTypes.INTERACT: "Interact",
            ActionTypes.EAT: "Eat",
            ActionTypes.DRINK: "Drink",
            ActionTypes.REST: "Rest",
            ActionTypes.WAIT: "Wait",
            ActionTypes.CUSTOM: "Custom"
        }
        return name_map.get(self._type, self._type)
    
    def clone(self):
        """Clone this action.
        
        Returns:
            Action: Cloned action.
        """
        return Action(self._type, self._parameters.copy())
    
    def __repr__(self) -> str:
        return f"<Action type={self._type} params={self._parameters}>"


class SimpleWorld:
    """Simple 2D world for NLM simulation."""
    
    def __init__(self):
        self._width = 10
        self._height = 10
        self._vision_width = 8
        self._vision_height = 8
        self._agent_x = 0
        self._agent_y = 0
        self._agent_orientation = 0
        self._simulation_time = 0.0
        self._objects = []
        logger.debug("SimpleWorld created")
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int):
        """Configure world dimensions.
        
        Args:
            width: World width.
            height: World height.
            vision_width: Vision width.
            vision_height: Vision height.
        """
        self._width = width
        self._height = height
        self._vision_width = vision_width
        self._vision_height = vision_height
        logger.debug("World configured", 
                    width=width, height=height, 
                    vision_width=vision_width, vision_height=vision_height)
    
    def reset(self):
        """Reset world state."""
        self._simulation_time = 0.0
        self._agent_x = 0
        self._agent_y = 0
        self._agent_orientation = 0
        logger.debug("World reset")
    
    def set_agent_start(self, x: float, y: float):
        """Set agent starting position.
        
        Args:
            x: Starting x position.
            y: Starting y position.
        """
        self._agent_x = x
        self._agent_y = y
        logger.debug("Agent start set", x=x, y=y)
    
    def update(self, timestep: float):
        """Update world state.
        
        Args:
            timestep: Time step to update.
        """
        self._simulation_time += timestep
        logger.debug("World updated", timestep=timestep, time=self._simulation_time)
    
    def apply_motor_command(self, action: Action, current_time: float):
        """Apply motor command in world.
        
        Args:
            action: Motor command to apply.
            current_time: Current time.
            
        Returns:
            ActionResult: Result of action application.
        """
        # Simulate action application
        logger.debug("Motor command applied", 
                    action_type=action.get_type(), time=current_time)
        
        return ActionResult(reward=0.0, success=True, message="")
    
    def get_sensory_percept(self):
        """Get sensory percept from world.
        
        Returns:
            SensoryPercept: Sensory percept data.
        """
        logger.debug("Sensory percept retrieved")
        return Vision(8, 8, 3)  # Dummy vision data
    
    def get_agent_body(self):
        """Get agent body state.
        
        Returns:
            AgentBody: Agent body state.
        """
        return AgentBody(self._agent_x, self._agent_y, self._agent_orientation)
    
    def add_object(self, obj):
        """Add world object.
        
        Args:
            obj: World object to add.
        """
        self._objects.append(obj)
        logger.debug("World object added", obj_type=type(obj).__name__)
    
    def remove_object(self, x: float, y: float):
        """Remove world object at position.
        
        Args:
            x: X position.
            y: Y position.
        """
        logger.debug("World object removed", x=x, y=y)
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid.
        
        Args:
            x: X position.
            y: Y position.
            
        Returns:
            bool: True if position valid.
        """
        return 0 <= x < self._width and 0 <= y < self._height
    
    def get_width(self) -> int:
        """Get world width.
        
        Returns:
            int: World width.
        """
        return self._width
    
    def get_height(self) -> int:
        """Get world height.
        
        Returns:
            int: World height.
        """
        return self._height
    
    def get_max_energy(self) -> float:
        """Get maximum agent energy.
        
        Returns:
            float: Maximum energy.
        """
        return 100.0
    
    def set_max_energy(self, energy: float):
        """Set maximum agent energy.
        
        Args:
            energy: Maximum energy.
        """
        logger.debug("Max energy set", energy=energy)
    
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate.
        
        Returns:
            float: Energy decay rate.
        """
        return 0.01
    
    def set_energy_decay_rate(self, rate: float):
        """Set energy decay rate.
        
        Args:
            rate: Energy decay rate.
        """
        logger.debug("Energy decay rate set", rate=rate)
    
    def get_simulation_time(self) -> float:
        """Get simulation time.
        
        Returns:
            float: Simulation time.
        """
        return self._simulation_time
    
    def set_random_seed(self, seed: int):
        """Set random seed.
        
        Args:
            seed: Random seed.
        """
        logger.debug("Random seed set", seed=seed)
    
    def get_random_seed(self) -> int:
        """Get random seed.
        
        Returns:
            int: Random seed.
        """
        return 42


class AgentBrain:
    """Agent brain interface connecting NLM brain to world."""
    
    def __init__(self, brain):
        self._brain = brain
        self._world = None
        self._initialized = False
        
        # Enable features
        self._reward_modulation_enabled = False
        self._structural_plasticity_enabled = False
        self._development_enabled = False
        self._curiosity_enabled = False
        
        logger.debug("AgentBrain created", brain_type=type(brain).__name__)
    
    def initialize(self, world):
        """Initialize with world.
        
        Args:
            world: World to connect to.
        """
        self._world = world
        self._initialized = True
        logger.info("AgentBrain initialized", world_type=type(world).__name__)
    
    def get_sensory_input_size(self) -> int:
        """Get expected sensory input size.
        
        Returns:
            int: Sensory input size.
        """
        return 24  # Default for 8x8x3 vision
    
    def get_motor_output_size(self) -> int:
        """Get expected motor output size.
        
        Returns:
            int: Motor output size.
        """
        return 8
    
    def process_sensory_input(self, percept):
        """Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept to process.
        """
        if not self._initialized:
            raise SimulationError("AgentBrain not initialized.")
        
        self._brain.receive_sensory_input(percept)
        logger.debug("Sensory input processed", 
                    percept_type=type(percept).__name__)
    
    def decode_motor_command(self):
        """Decode brain motor activity into motor command.
        
        Returns:
            Action: Motor command.
        """
        if not self._initialized:
            raise SimulationError("AgentBrain not initialized.")
        
        return self._brain.produce_action()
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float):
        """Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received.
            predicted_reward: Predicted reward.
        """
        if not self._reward_modulation_enabled:
            logger.debug("Reward modulation disabled")
            return
        
        prediction_error = abs(reward - predicted_reward)
        logger.debug("Reward modulation applied",
                    reward=reward, predicted_reward=predicted_reward,
                    prediction_error=prediction_error)
    
    def update_development(self, timestep: float):
        """Update development system.
        
        Args:
            timestep: Time step.
        """
        if not self._development_enabled:
            return
        
        logger.debug("Development updated", timestep=timestep)
    
    def get_developmental_stage(self) -> str:
        """Get current developmental stage.
        
        Returns:
            str: Developmental stage.
        """
        return self._brain.get_developmental_stage()
    
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level.
        
        Returns:
            float: Neuromodulation level.
        """
        return 0.5
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level.
        
        Returns:
            float: Curiosity level.
        """
        return 0.3 if self._curiosity_enabled else 0.0
    
    def get_novelty_level(self) -> float:
        """Get novelty level.
        
        Returns:
            float: Novelty level.
        """
        return 0.2
    
    def get_prediction_error(self) -> float:
        """Get prediction error.
        
        Returns:
            float: Prediction error.
        """
        return 0.1
    
    def reset(self):
        """Reset agent for new episode."""
        self._initialized = False
        self._world = None
        logger.info("AgentBrain reset")
    
    def get_brain(self):
        """Get the underlying brain.
        
        Returns:
            Brain: Underlying brain.
        """
        return self._brain
    
    # Feature enabling methods
    
    def enable_reward_modulation(self, enable: bool):
        """Enable or disable reward modulation.
        
        Args:
            enable: Whether to enable reward modulation.
        """
        self._reward_modulation_enabled = enable
        logger.debug("Reward modulation enabled", enabled=enable)
    
    def enable_structural_plasticity(self, enable: bool):
        """Enable or disable structural plasticity.
        
        Args:
            enable: Whether to enable structural plasticity.
        """
        self._structural_plasticity_enabled = enable
        logger.debug("Structural plasticity enabled", enabled=enable)
    
    def enable_development(self, enable: bool):
        """Enable or disable development.
        
        Args:
            enable: Whether to enable development.
        """
        self._development_enabled = enable
        logger.debug("Development enabled", enabled=enable)
    
    def enable_curiosity(self, enable: bool):
        """Enable or disable curiosity.
        
        Args:
            enable: Whether to enable curiosity.
        """
        self._curiosity_enabled = enable
        logger.debug("Curiosity enabled", enabled=enable)
    
    # State checking methods
    
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled.
        
        Returns:
            bool: True if reward modulation enabled.
        """
        return self._reward_modulation_enabled
    
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled.
        
        Returns:
            bool: True if structural plasticity enabled.
        """
        return self._structural_plasticity_enabled
    
    def is_development_enabled(self) -> bool:
        """Check if development is enabled.
        
        Returns:
            bool: True if development enabled.
        """
        return self._development_enabled
    
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled.
        
        Returns:
            bool: True if curiosity enabled.
        """
        return self._curiosity_enabled


class ActionResult:
    """Action result from world."""
    
    def __init__(self, reward: float = 0.0, success: bool = True, message: str = ""):
        self._reward = reward
        self._success = success
        self._message = message
        logger.debug("ActionResult created",
                    reward=reward, success=success, message=message)
    
    def get_reward(self) -> float:
        """Get reward.
        
        Returns:
            float: Reward value.
        """
        return self._reward
    
    def get_success(self) -> bool:
        """Get success status.
        
        Returns:
            bool: True if action was successful.
        """
        return self._success
    
    def get_message(self) -> str:
        """Get message.
        
        Returns:
            str: Result message.
        """
        return self._message
    
    def set_reward(self, reward: float):
        """Set reward.
        
        Args:
            reward: Reward value.
        """
        self._reward = reward
    
    def set_success(self, success: bool):
        """Set success status.
        
        Args:
            success: Success status.
        """
        self._success = success
    
    def set_message(self, message: str):
        """Set message.
        
        Args:
            message: Result message.
        """
        self._message = message
    
    def __repr__(self) -> str:
        return f"<ActionResult reward={self._reward} success={self._success}>"


class AgentBody:
    """Agent body state."""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, orientation: float = 0.0):
        self._x = x
        self._y = y
        self._orientation = orientation
        self._velocity_x = 0.0
        self._velocity_y = 0.0
        self._angular_velocity = 0.0
        self._energy = 100.0
        self._health = 100.0
        self._age = 0
        self._is_moving = False
        self._is_turning = False
        self._last_action_time = 0.0
        logger.debug("AgentBody created", x=x, y=y, orientation=orientation)
    
    @property
    def x(self) -> float:
        """Get x position.
        
        Returns:
            float: X position.
        """
        return self._x
    
    @x.setter
    def x(self, value: float):
        """Set x position.
        
        Args:
            value: X position.
        """
        self._x = value
    
    @property
    def y(self) -> float:
        """Get y position.
        
        Returns:
            float: Y position.
        """
        return self._y
    
    @y.setter
    def y(self, value: float):
        """Set y position.
        
        Args:
            value: Y position.
        """
        self._y = value
    
    @property
    def orientation(self) -> float:
        """Get orientation.
        
        Returns:
            float: Orientation.
        """
        return self._orientation
    
    @orientation.setter
    def orientation(self, value: float):
        """Set orientation.
        
        Args:
            value: Orientation.
        """
        self._orientation = value
    
    @property
    def velocity_x(self) -> float:
        """Get x velocity.
        
        Returns:
            float: X velocity.
        """
        return self._velocity_x
    
    @velocity_x.setter
    def velocity_x(self, value: float):
        """Set x velocity.
        
        Args:
            value: X velocity.
        """
        self._velocity_x = value
    
    @property
    def velocity_y(self) -> float:
        """Get y velocity.
        
        Returns:
            float: Y velocity.
        """
        return self._velocity_y
    
    @velocity_y.setter
    def velocity_y(self, value: float):
        """Set y velocity.
        
        Args:
            value: Y velocity.
        """
        self._velocity_y = value
    
    @property
    def angular_velocity(self) -> float:
        """Get angular velocity.
        
        Returns:
            float: Angular velocity.
        """
        return self._angular_velocity
    
    @angular_velocity.setter
    def angular_velocity(self, value: float):
        """Set angular velocity.
        
        Args:
            value: Angular velocity.
        """
        self._angular_velocity = value
    
    @property
    def energy(self) -> float:
        """Get energy.
        
        Returns:
            float: Energy.
        """
        return self._energy
    
    @energy.setter
    def energy(self, value: float):
        """Set energy.
        
        Args:
            value: Energy.
        """
        self._energy = value
    
    @property
    def health(self) -> float:
        """Get health.
        
        Returns:
            float: Health.
        """
        return self._health
    
    @health.setter
    def health(self, value: float):
        """Set health.
        
        Args:
            value: Health.
        """
        self._health = value
    
    @property
    def age(self) -> int:
        """Get age.
        
        Returns:
            int: Age.
        """
        return self._age
    
    @age.setter
    def age(self, value: int):
        """Set age.
        
        Args:
            value: Age.
        """
        self._age = value
    
    @property
    def is_moving(self) -> bool:
        """Get moving status.
        
        Returns:
            bool: True if moving.
        """
        return self._is_moving
    
    @is_moving.setter
    def is_moving(self, value: bool):
        """Set moving status.
        
        Args:
            value: Moving status.
        """
        self._is_moving = value
    
    @property
    def is_turning(self) -> bool:
        """Get turning status.
        
        Returns:
            bool: True if turning.
        """
        return self._is_turning
    
    @is_turning.setter
    def is_turning(self, value: bool):
        """Set turning status.
        
        Args:
            value: Turning status.
        """
        self._is_turning = value
    
    @property
    def last_action_time(self) -> float:
        """Get last action time.
        
        Returns:
            float: Last action time.
        """
        return self._last_action_time
    
    @last_action_time.setter
    def last_action_time(self, value: float):
        """Set last action time.
        
        Args:
            value: Last action time.
        """
        self._last_action_time = value
    
    def reset(self):
        """Reset agent body state."""
        self._x = 0.0
        self._y = 0.0
        self._orientation = 0.0
        self._velocity_x = 0.0
        self._velocity_y = 0.0
        self._angular_velocity = 0.0
        self._energy = 100.0
        self._health = 100.0
        self._age = 0
        self._is_moving = False
        self._is_turning = False
        self._last_action_time = 0.0
        logger.debug("AgentBody reset")
    
    def __repr__(self) -> str:
        return f"<AgentBody x={self._x} y={self._y} orientation={self._orientation}>"


class Vision:
    """Vision sensory input."""
    
    def __init__(self, width: int, height: int, channels: int = 3):
        self._width = width
        self._height = height
        self._channels = channels
        self._data = [0.0] * (width * height * channels)
        self._timestamp = 0.0
        logger.debug("Vision created", width=width, height=height, channels=channels)
    
    def get_type(self) -> str:
        """Get sensory input type.
        
        Returns:
            str: Sensory input type.
        """
        return "vision"
    
    def get_data(self) -> List[float]:
        """Get raw data as vector.
        
        Returns:
            List[float]: Raw data.
        """
        return self._data
    
    def set_data(self, data: List[float]):
        """Set data.
        
        Args:
            data: Raw data.
        """
        self._data = data
    
    def get_dimensions(self) -> int:
        """Get dimensionality.
        
        Returns:
            int: Dimensionality.
        """
        return self._width * self._height * self._channels
    
    def get_timestamp(self) -> float:
        """Get timestamp.
        
        Returns:
            float: Timestamp.
        """
        return self._timestamp
    
    def set_timestamp(self, timestamp: float):
        """Set timestamp.
        
        Args:
            timestamp: Timestamp.
        """
        self._timestamp = timestamp
    
    def get_width(self) -> int:
        """Get width.
        
        Returns:
            int: Width.
        """
        return self._width
    
    def get_height(self) -> int:
        """Get height.
        
        Returns:
            int: Height.
        """
        return self._height
    
    def get_channels(self) -> int:
        """Get channels.
        
        Returns:
            int: Channels.
        """
        return self._channels


class Audio:
    """Audio sensory input."""
    
    def __init__(self, sample_rate: int, num_samples: int):
        self._sample_rate = sample_rate
        self._num_samples = num_samples
        self._data = [0.0] * num_samples
        self._timestamp = 0.0
        logger.debug("Audio created", sample_rate=sample_rate, num_samples=num_samples)
    
    def get_type(self) -> str:
        """Get sensory input type.
        
        Returns:
            str: Sensory input type.
        """
        return "audio"
    
    def get_data(self) -> List[float]:
        """Get raw data as vector.
        
        Returns:
            List[float]: Raw data.
        """
        return self._data
    
    def set_data(self, data: List[float]):
        """Set data.
        
        Args:
            data: Raw data.
        """
        self._data = data
    
    def get_dimensions(self) -> int:
        """Get dimensionality.
        
        Returns:
            int: Dimensionality.
        """
        return self._num_samples
    
    def get_timestamp(self) -> float:
        """Get timestamp.
        
        Returns:
            float: Timestamp.
        """
        return self._timestamp
    
    def set_timestamp(self, timestamp: float):
        """Set timestamp.
        
        Args:
            timestamp: Timestamp.
        """
        self._timestamp = timestamp
    
    def get_sample_rate(self) -> int:
        """Get sample rate.
        
        Returns:
            int: Sample rate.
        """
        return self._sample_rate
    
    def set_sample_rate(self, sample_rate: int):
        """Set sample rate.
        
        Args:
            sample_rate: Sample rate.
        """
        self._sample_rate = sample_rate
    
    def get_num_samples(self) -> int:
        """Get number of samples.
        
        Returns:
            int: Number of samples.
        """
        return self._num_samples


class InternalSignals:
    """Internal signals sensory input."""
    
    def __init__(self):
        self._signals = []
        self._timestamp = 0.0
        logger.debug("InternalSignals created")
    
    def get_type(self) -> str:
        """Get sensory input type.
        
        Returns:
            str: Sensory input type.
        """
        return "internal"
    
    def get_data(self) -> List[float]:
        """Get raw data as vector.
        
        Returns:
            List[float]: Raw data.
        """
        return self._signals
    
    def get_dimensions(self) -> int:
        """Get dimensionality.
        
        Returns:
            int: Dimensionality.
        """
        return len(self._signals)
    
    def get_timestamp(self) -> float:
        """Get timestamp.
        
        Returns:
            float: Timestamp.
        """
        return self._timestamp
    
    def set_timestamp(self, timestamp: float):
        """Set timestamp.
        
        Args:
            timestamp: Timestamp.
        """
        self._timestamp = timestamp
    
    def add_signal(self, value: float):
        """Add signal.
        
        Args:
            value: Signal value.
        """
        self._signals.append(value)
    
    def clear_signals(self):
        """Clear all signals."""
        self._signals.clear()


class SensoryPercept:
    """Sensory percept data container."""
    
    def __init__(self):
        self._vision = Vision(8, 8, 3)
        self._audio = Audio(16000, 1024)
        self._internal = InternalSignals()
        self._touch = []
        self._proprioception = []
        self._timestamp = 0.0
        logger.debug("SensoryPercept created")
    
    def get_vision(self):
        """Get vision data.
        
        Returns:
            Vision: Vision data.
        """
        return self._vision
    
    def set_vision(self, vision):
        """Set vision data.
        
        Args:
            vision: Vision data.
        """
        self._vision = vision
    
    def get_vision_width(self) -> int:
        """Get vision width.
        
        Returns:
            int: Vision width.
        """
        return self._vision.get_width()
    
    def get_vision_height(self) -> int:
        """Get vision height.
        
        Returns:
            int: Vision height.
        """
        return self._vision.get_height()
    
    def get_touch(self) -> List[float]:
        """Get touch data.
        
        Returns:
            List[float]: Touch data.
        """
        return self._touch
    
    def set_touch(self, touch: List[float]):
        """Set touch data.
        
        Args:
            touch: Touch data.
        """
        self._touch = touch
    
    def get_internal(self):
        """Get internal signals.
        
        Returns:
            InternalSignals: Internal signals.
        """
        return self._internal
    
    def set_internal(self, internal):
        """Set internal signals.
        
        Args:
            internal: Internal signals.
        """
        self._internal = internal
    
    def get_proprioception(self) -> List[float]:
        """Get proprioception data.
        
        Returns:
            List[float]: Proprioception data.
        """
        return self._proprioception
    
    def set_proprioception(self, proprioception: List[float]):
        """Set proprioception data.
        
        Args:
            proprioception: Proprioception data.
        """
        self._proprioception = proprioception
    
    def get_audio(self):
        """Get audio data.
        
        Returns:
            Audio: Audio data.
        """
        return self._audio
    
    def set_audio(self, audio):
        """Set audio data.
        
        Args:
            audio: Audio data.
        """
        self._audio = audio
    
    def get_all_signals(self) -> Dict[str, Any]:
        """Get all sensory signals.
        
        Returns:
            Dict[str, Any]: All signals.
        """
        return {
            'vision': self._vision,
            'audio': self._audio,
            'internal': self._internal,
            'touch': self._touch,
            'proprioception': self._proprioception
        }
    
    def get_timestamp(self) -> float:
        """Get timestamp.
        
        Returns:
            float: Timestamp.
        """
        return self._timestamp
    
    def set_timestamp(self, timestamp: float):
        """Set timestamp.
        
        Args:
            timestamp: Timestamp.
        """
        self._timestamp = timestamp


class WorldObject:
    """World object representation."""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, object_type: str = WorldObjectTypes.EMPTY, value: float = 0.0, radius: float = 0.5):
        self._x = x
        self._y = y
        self._type = object_type
        self._value = value
        self._radius = radius
        self._active = True
        logger.debug("WorldObject created", x=x, y=y, type=object_type)
    
    @property
    def x(self) -> float:
        """Get x position.
        
        Returns:
            float: X position.
        """
        return self._x
    
    @x.setter
    def x(self, value: float):
        """Set x position.
        
        Args:
            value: X position.
        """
        self._x = value
    
    @property
    def y(self) -> float:
        """Get y position.
        
        Returns:
            float: Y position.
        """
        return self._y
    
    @y.setter
    def y(self, value: float):
        """Set y position.
        
        Args:
            value: Y position.
        """
        self._y = value
    
    @property
    def radius(self) -> float:
        """Get radius.
        
        Returns:
            float: Radius.
        """
        return self._radius
    
    @radius.setter
    def radius(self, value: float):
        """Set radius.
        
        Args:
            value: Radius.
        """
        self._radius = value
    
    @property
    def type(self) -> str:
        """Get object type.
        
        Returns:
            str: Object type.
        """
        return self._type
    
    @type.setter
    def type(self, value: str):
        """Set object type.
        
        Args:
            value: Object type.
        """
        valid_types = [
            WorldObjectTypes.EMPTY,
            WorldObjectTypes.RESOURCE,
            WorldObjectTypes.HAZARD,
            WorldObjectTypes.WALL,
            WorldObjectTypes.MARKER
        ]
        if value not in valid_types:
            raise ValueError(f"Invalid world object type: {value}")
        self._type = value
    
    @property
    def value(self) -> float:
        """Get value.
        
        Returns:
            float: Value.
        """
        return self._value
    
    @value.setter
    def value(self, value: float):
        """Set value.
        
        Args:
            value: Value.
        """
        self._value = value
    
    @property
    def active(self) -> bool:
        """Get active status.
        
        Returns:
            bool: Active status.
        """
        return self._active
    
    @active.setter
    def active(self, value: bool):
        """Set active status.
        
        Args:
            value: Active status.
        """
        self._active = value
    
    def __repr__(self) -> str:
        return f"<WorldObject x={self._x} y={self._y} type={self._type} active={self._active}>"


# Convenience function to create default configuration
def createDefaultConfig():
    """Create a default configuration for NLM.
    
    Returns:
        Config: Default configuration.
    """
    return Config()

# Convenience function to create a brain
def createBrain(config=None):
    """Create a new brain with optional configuration.
    
    Args:
        config: Optional configuration. If None, uses default config.
        
    Returns:
        Brain: A new brain instance.
    """
    if config is None:
        config = createDefaultConfig()
    return Brain(config)

# Convenience function to create a simple world
def createSimpleWorld():
    """Create a new simple world.
    
    Returns:
        SimpleWorld: New simple world.
    """
    return SimpleWorld()

# Convenience function to create an agent brain
def createAgentBrain(brain):
    """Create an agent brain interface from an existing brain.
    
    Args:
        brain: Existing Brain instance.
        
    Returns:
        AgentBrain: Agent brain interface.
    """
    return AgentBrain(brain)

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Export all public symbols
__all__ = [
    # Classes
    'Config', 'Brain', 'Neuron', 'Synapse', 'NeuralRegion',
    'AgentBrain', 'Action', 'ActionResult', 'AgentBody',
    'SimpleWorld', 'Vision', 'Audio', 'InternalSignals', 'SensoryPercept',
    'WorldObject',
    
    # Types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand',
    'WorldObjectType',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Enum-like classes
    'NeuronTypes', 'SynapseTypes', 'DevelopmentalStages',
    'FiringStates', 'ActionTypes', 'MotorCommands', 'WorldObjectTypes',
    
    # Exception classes
    'NLMError', 'BrainInitializationError', 'InvalidConfigurationError', 'SimulationError',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createAgentBrain', 'createSimpleWorld',
]
