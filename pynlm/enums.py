"""
Enumerators and constants for NLM Python bindings.
"""

# Neuron types for brain cells
class NeuronType:
    """Types of neurons in the brain."""
    EXCITATORY = "excitatory"      # Neurons that promote firing
    INHIBITORY = "inhibitory"      # Neurons that suppress firing
    SENSORY = "sensory"            # Neurons that process sensory input
    MOTOR = "motor"                # Neurons that control actions
    MODULATORY = "modulatory"      # Neurons that modulate activity
    INTERNAL = "internal"          # Neurons for internal processing

    # Common usage constants
    EXCITATORY_INHIBITORY_RATIO = 0.8  # Typical E/I ratio


# Motor commands for agent actions
class MotorCommand:
    """Actions that an agent can take."""
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK_LEFT = "look_left"
    LOOK_RIGHT = "look_right"
    INTERACT = "interact"
    WAIT = "wait"
    EAT = "eat"
    SLEEP = "sleep"
    PLAY = "play"

    # Direction commands (aliases)
    FORWARD = MOVE_FORWARD
    BACKWARD = MOVE_BACKWARD
    LEFT = TURN_LEFT
    RIGHT = TURN_RIGHT

    # Utility methods
    @classmethod
    def all_commands(cls):
        """Get all motor commands as a list."""
        return [
            cls.MOVE_FORWARD, cls.MOVE_BACKWARD, cls.TURN_LEFT, cls.TURN_RIGHT,
            cls.LOOK_LEFT, cls.LOOK_RIGHT, cls.INTERACT, cls.WAIT,
            cls.EAT, cls.SLEEP, cls.PLAY
        ]

    @classmethod
    def get_movable_commands(cls):
        """Get commands that move the agent."""
        return [cls.MOVE_FORWARD, cls.MOVE_BACKWARD]

    @classmethod
    def get_turn_commands(cls):
        """Get commands that turn the agent."""
        return [cls.TURN_LEFT, cls.TURN_RIGHT, cls.LOOK_LEFT, cls.LOOK_RIGHT]


# Developmental stages for brain maturation
class DevelopmentalStage:
    """Stages of brain development."""
    INITIAL = "initial"            # Newborn, high plasticity
    CRITICAL_PERIOD = "critical_period"  # Sensitive learning phase
    MATURATION = "maturation"      # Organizes connections
    ADULT = "adult"                # Stable, efficient operation
    AGING = "aging"                # Gradual decline

    # Property methods for development
    @classmethod
    def get_plasticity_factor(cls, stage):
        """Get plasticity factor for a developmental stage."""
        factors = {
            cls.INITIAL: 1.0,
            cls.CRITICAL_PERIOD: 0.8,
            cls.MATURATION: 0.5,
            cls.ADULT: 0.2,
            cls.AGING: 0.1
        }
        return factors.get(stage, 0.5)

    @classmethod
    def is_high_plasticity(cls, stage):
        """Check if a developmental stage has high plasticity."""
        return stage in [cls.INITIAL, cls.CRITICAL_PERIOD]


# World object types
class WorldObjectType:
    """Types of objects in the world."""
    EMPTY = "empty"                # No object
    RESOURCE = "resource"          # Something valuable (food, etc.)
    HAZARD = "hazard"              # Dangerous (fire, etc.)
    WALL = "wall"                  # Physical barrier
    MARKER = "marker"              # Reference point
    AGENT = "agent"                # The agent itself
    GOAL = "goal"                  # Target to reach

    # Properties
    @classmethod
    def is_passable(cls, obj_type):
        """Check if an object type is passable by the agent."""
        return obj_type in [cls.EMPTY, cls.RESOURCE, cls.MARKER, cls.GOAL]

    @classmethod
    def is_consumable(cls, obj_type):
        """Check if an object type can be consumed by the agent."""
        return obj_type == cls.RESOURCE

    @classmethod
    def is_hazardous(cls, obj_type):
        """Check if an object type is hazardous."""
        return obj_type == cls.HAZARD


# Helper functions for working with enumerations

def get_enum_values(enum_class):
    """Get all values from an enumeration class."""
    return [value for value in enum_class.__dict__.values() 
            if not value.startswith('_')]


def get_enum_info(enum_class):
    """Get information about an enumeration class."""
    return {
        'name': enum_class.__name__,
        'values': get_enum_values(enum_class),
        'count': len(get_enum_values(enum_class))
    }


__all__ = [
    'NeuronType',
    'MotorCommand',
    'DevelopmentalStage',
    'WorldObjectType',
    'get_enum_values',
    'get_enum_info'
]
