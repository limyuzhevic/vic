'''Enumerations for pynlm'''

from enum import Enum
from typing import Any


class NeuronType(Enum):
    """Enumeration of neuron types in the NLM simulation.
    
    These enums correspond to different functional types of neurons
    in the neural network, affecting how they process and transmit signals.
    """
    
    EXCITATORY = 0  # Neurons that increase firing in other neurons
    INHIBITORY = 1   # Neurons that decrease firing in other neurons
    MODULATORY = 2  # Neurons that adjust overall network excitability
    SENSORY = 3     # Neurons that receive external input
    MOTOR = 4       # Neurons that control agent actions
    INTERNAL = 5    # Neurons for internal processing
    
    def __str__(self) -> str:
        """Get string representation of neuron type.
        
        Returns:
            Human-readable name of the neuron type.
        """
        return self.name.replace("_", " ").title()
    
    def __repr__(self) -> str:
        """Get detailed string representation.
        
        Returns:
            Detailed representation of the neuron type.
        """
        return f"NeuronType.{self.name}"


class SynapseType(Enum):
    """Enumeration of synapse types in the neural network.
    
    Different synapse types determine how signals are transmitted
    between neurons, affecting strength, dynamics, and behavior.
    """
    
    EXCITATORY = 0      # Synapses that increase postsynaptic firing
    INHIBITORY = 1       # Synapses that decrease postsynaptic firing
    MODULATORY = 2       # Synapses that adjust network excitability
    ELECTRICAL = 3       # Direct electrical connections
    GAP_JUNCTION = 4     # Electrical gap junctions
    
    def __str__(self) -> str:
        """Get string representation of synapse type.
        
        Returns:
            Human-readable name of the synapse type.
        """
        return self.name.replace("_", " ").title()
    
    def __repr__(self) -> str:
        """Get detailed string representation.
        
        Returns:
            Detailed representation of the synapse type.
        """
        return f"SynapseType.{self.name}"


class DevelopmentalStage(Enum):
    """Enumeration of developmental stages for the brain.
    
    These stages represent different phases of neural development,
    affecting learning capacity and plasticity.
    """
    
    INITIAL = 0        # Starting state, high plasticity
    CRITICAL_PERIOD = 1  # Sensitive learning phase
    MATURATION = 2      # Refinement and optimization
    ADULT = 3          # Mature, stable network
    AGING = 4          # Gradual decline in function
    
    def __str__(self) -> str:
        """Get string representation of developmental stage.
        
        Returns:
            Human-readable name of the developmental stage.
        """
        return self.name.replace("_", " ").title()
    
    def __repr__(self) -> str:
        """Get detailed string representation.
        
        Returns:
            Detailed representation of the developmental stage.
        """
        return f"DevelopmentalStage.{self.name}"


class FiringState(Enum):
    """Enumeration of neuron firing states.
    
    These states represent the current activation status of neurons
    during simulation.
    """
    
    RESTING = 0   # Not firing, below threshold
    ACTIVE = 1    # Currently firing
    REFRACTORY = 2  # Just fired, cannot fire again immediately
    INHIBITED = 3  # Suppressed by inhibitory input
    
    def __str__(self) -> str:
        """Get string representation of firing state.
        
        Returns:
            Human-readable name of the firing state.
        """
        return self.name.replace("_", " ").title()
    
    def __repr__(self) -> str:
        """Get detailed string representation.
        
        Returns:
            Detailed representation of the firing state.
        """
        return f"FiringState.{self.name}"


class ActionType(Enum):
    """Enumeration of motor actions for agent control.
    
    These are semantic action types that can be performed by the agent,
    distinct from low-level motor commands.
    """
    
    MOVE_FORWARD = 0   # Move forward in the world
    MOVE_BACKWARD = 1  # Move backward
    MOVE_LEFT = 2      # Move left
    MOVE_RIGHT = 3     # Move right
    TURN_LEFT = 4      # Rotate counterclockwise
    TURN_RIGHT = 5     # Rotate clockwise
    LOOK = 6           # Look in current direction
    LOOK_UP = 7        # Look up
    LOOK_DOWN = 8      # Look down
    INTERACT = 9       # Interact with nearby object
    EAT = 10           # Consume food resource
    DRINK = 11         # Drink water resource
    REST = 12          # Stop moving and rest
    WAIT = 13          # Wait for next timestep
    CUSTOM = 14        # User-defined action
    
    def __str__(self) -> str:
        """Get string representation of action type.
        
        Returns:
            Human-readable name of the action type.
        """
        return self.name.replace("_", " ").title()
    
    def __repr__(self) -> str:
        """Get detailed string representation.
        
        Returns:
            Detailed representation of the action type.
        """
        return f"ActionType.{self.name}"


class MotorCommand(Enum):
    """Enumeration of low-level motor commands.
    
    These are innate muscle-like signals that control agent body movement.
    They are distinct from semantic ActionTypes.
    """
    
    MOVE_FORWARD = 0  # Propel agent forward
    MOVE_BACKWARD = 1 # Propel agent backward
    TURN_LEFT = 2     # Rotate agent left
    TURN_RIGHT = 3    # Rotate agent right
    LOOK_LEFT = 4     # Pan sensors left
    LOOK_RIGHT = 5    # Pan sensors right
    INTERACT = 6      # Trigger interaction with nearby object
    WAIT = 7          # No movement, wait for next step
    
    def __str__(self) -> str:
        """Get string representation of motor command.
        
        Returns:
            Human-readable name of the motor command.
        """
        return self.name.replace("_", " ").title()
    
    def __repr__(self) -> str:
        """Get detailed string representation.
        
        Returns:
            Detailed representation of the motor command.
        """
        return f"MotorCommand.{self.name}"


class WorldObjectType(Enum):
    """Enumeration of object types in the world.
    
    These types classify objects in the simulation world for rendering,
    collision detection, and behavior.
    """
    
    EMPTY = 0       # Empty space
    RESOURCE = 1    # Resource that can be consumed
    HAZARD = 2      # Hazardous object that causes damage
    WALL = 3        # Physical barrier
    MARKER = 4      # Navigation aid or goal marker
    
    def __str__(self) -> str:
        """Get string representation of world object type.
        
        Returns:
            Human-readable name of the world object type.
        """
        return self.name.replace("_", " ").title()
    
    def __repr__(self) -> str:
        """Get detailed string representation.
        
        Returns:
            Detailed representation of the world object type.
        """
        return f"WorldObjectType.{self.name}"


class ConfigSource(Enum):
    """Enumeration of configuration sources.
    
    These represent where configuration values come from,
    affecting priority and persistence.
    """
    
    DEFAULT = 0   # Default value
    FILE = 1        # Loaded from file
    COMMAND_LINE = 2  # Loaded from command line
    RUNTIME = 3    # Set at runtime
    
    def __str__(self) -> str:
        """Get string representation of config source.
        
        Returns:
            Human-readable name of the config source.
        """
        return self.name.replace("_", " ").title()
    
    def __repr__(self) -> str:
        """Get detailed string representation.
        
        Returns:
            Detailed representation of the config source.
        """
        return f"ConfigSource.{self.name}"