"""NLM Core Type Definitions"""

# Note: These type definitions are primarily for documentation and type checking.
# The actual types are defined in the C++ bindings and exposed through the Python module.

from typing import Dict, List, Tuple, Optional, Union, Any

# Core identifier types
class NeuronId:
    """Unique identifier for a neuron"""
    pass

class SynapseId:
    """Unique identifier for a synapse"""
    pass

class RegionId:
    """Unique identifier for a brain region"""
    pass

class PopulationId:
    """Unique identifier for a neuron population"""
    pass

# Enumeration types
class NeuronType:
    """Enumeration of neuron types"""
    Excitatory = 1
    Inhibitory = 2
    Modulatory = 3
    Sensory = 4
    Motor = 5
    Internal = 6

class SynapseType:
    """Enumeration of synapse types"""
    Excitatory = 1
    Inhibitory = 2
    Modulatory = 3
    Electrical = 4
    GapJunction = 5

class DevelopmentalStage:
    """Enumeration of developmental stages"""
    Initial = 1
    CriticalPeriod = 2
    Maturation = 3
    Adult = 4
    Aging = 5

class FiringState:
    """Enumeration of neuron firing states"""
    Resting = 1
    Active = 2
    Refractory = 3
    Inhibited = 4

class ActionType:
    """Enumeration of action types"""
    MoveForward = 1
    MoveBackward = 2
    MoveLeft = 3
    MoveRight = 4
    TurnLeft = 5
    TurnRight = 6
    Look = 7
    LookUp = 8
    LookDown = 9
    Interact = 10
    Eat = 11
    Drink = 12
    Rest = 13
    Wait = 14
    Custom = 15

class MotorCommand:
    """Enumeration of motor commands"""
    MoveForward = 1
    MoveBackward = 2
    TurnLeft = 3
    TurnRight = 4
    LookLeft = 5
    LookRight = 6
    Interact = 7
    Wait = 8

class WorldObjectType:
    """Enumeration of world object types"""
    Empty = 1
    Resource = 2
    Hazard = 3
    Wall = 4
    Marker = 5

# Type aliases for better documentation
Vector3D = Tuple[float, float, float]
Matrix3D = List[List[float]]
SignalVector = List[float]
NeuralState = Dict[str, Any]
SimulationState = Dict[str, Any]