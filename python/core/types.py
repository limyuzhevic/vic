"""
Core NLM types and enumerations

This module contains the basic type definitions and enumerations used throughout the NLM system.
"""

import ctypes

# Basic numeric types
MembranePotential = float
Current = float
Weight = float
Delay = int
SimulationStep = int
Timestamp = float

# Neuron types
class NeuronType:
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Sensory = 3
    Motor = 4
    Internal = 5

    @classmethod
    def get_name(cls, value):
        mapping = {
            cls.Excitatory: "Excitatory",
            cls.Inhibitory: "Inhibitory",
            cls.Modulatory: "Modulatory",
            cls.Sensory: "Sensory",
            cls.Motor: "Motor",
            cls.Internal: "Internal"
        }
        return mapping.get(value, f"Unknown({value})")

# Synapse types
class SynapseType:
    Excitatory = 0
    Inhibitory = 1
    Modulatory = 2
    Electrical = 3
    GapJunction = 4

    @classmethod
    def get_name(cls, value):
        mapping = {
            cls.Excitatory: "Excitatory",
            cls.Inhibitory: "Inhibitory",
            cls.Modulatory: "Modulatory",
            cls.Electrical: "Electrical",
            cls.GapJunction: "GapJunction"
        }
        return mapping.get(value, f"Unknown({value})")

# Developmental stages
class DevelopmentalStage:
    Initial = 0
    CriticalPeriod = 1
    Maturation = 2
    Adult = 3
    Aging = 4

    @classmethod
    def get_name(cls, value):
        mapping = {
            cls.Initial: "Initial",
            cls.CriticalPeriod: "CriticalPeriod",
            cls.Maturation: "Maturation",
            cls.Adult: "Adult",
            cls.Aging: "Aging"
        }
        return mapping.get(value, f"Unknown({value})")

# Firing states
class FiringState:
    Resting = 0
    Active = 1
    Refractory = 2
    Inhibited = 3

    @classmethod
    def get_name(cls, value):
        mapping = {
            cls.Resting: "Resting",
            cls.Active: "Active",
            cls.Refractory: "Refractory",
            cls.Inhibited: "Inhibited"
        }
        return mapping.get(value, f"Unknown({value})")

# Action types
class ActionType:
    MoveForward = 0
    MoveBackward = 1
    MoveLeft = 2
    MoveRight = 3
    TurnLeft = 4
    TurnRight = 5
    Look = 6
    LookUp = 7
    LookDown = 8
    Interact = 9
    Eat = 10
    Drink = 11
    Rest = 12
    Wait = 13
    Custom = 14

    @classmethod
    def get_name(cls, value):
        mapping = {
            cls.MoveForward: "MoveForward",
            cls.MoveBackward: "MoveBackward",
            cls.MoveLeft: "MoveLeft",
            cls.MoveRight: "MoveRight",
            cls.TurnLeft: "TurnLeft",
            cls.TurnRight: "TurnRight",
            cls.Look: "Look",
            cls.LookUp: "LookUp",
            cls.LookDown: "LookDown",
            cls.Interact: "Interact",
            cls.Eat: "Eat",
            cls.Drink: "Drink",
            cls.Rest: "Rest",
            cls.Wait: "Wait",
            cls.Custom: "Custom"
        }
        return mapping.get(value, f"Unknown({value})")

# Motor commands
class MotorCommand:
    MoveForward = 0
    MoveBackward = 1
    TurnLeft = 2
    TurnRight = 3
    LookLeft = 4
    LookRight = 5
    Interact = 6
    Wait = 7

    @classmethod
    def get_name(cls, value):
        mapping = {
            cls.MoveForward: "MoveForward",
            cls.MoveBackward: "MoveBackward",
            cls.TurnLeft: "TurnLeft",
            cls.TurnRight: "TurnRight",
            cls.LookLeft: "LookLeft",
            cls.LookRight: "LookRight",
            cls.Interact: "Interact",
            cls.Wait: "Wait"
        }
        return mapping.get(value, f"Unknown({value})")

# World object types
class WorldObjectType:
    Empty = 0
    Resource = 1
    Hazard = 2
    Wall = 3
    Marker = 4

    @classmethod
    def get_name(cls, value):
        mapping = {
            cls.Empty: "Empty",
            cls.Resource: "Resource",
            cls.Hazard: "Hazard",
            cls.Wall: "Wall",
            cls.Marker: "Marker"
        }
        return mapping.get(value, f"Unknown({value})")

# Unique identifiers
class NeuronId:
    def __init__(self, value=0):
        self.value = value

    def index(self):
        return self.value

    def __eq__(self, other):
        return self.value == other.value

    def __ne__(self, other):
        return self.value != other.value

    def __hash__(self):
        return hash(self.value)

    def __repr__(self):
        return f"<NeuronId: {self.value}>"

class SynapseId:
    def __init__(self, value=0):
        self.value = value

    def index(self):
        return self.value

    def __eq__(self, other):
        return self.value == other.value

    def __ne__(self, other):
        return self.value != other.value

    def __hash__(self):
        return hash(self.value)

    def __repr__(self):
        return f"<SynapseId: {self.value}>"

class RegionId:
    def __init__(self, value=0):
        self.value = value

    def index(self):
        return self.value

    def __eq__(self, other):
        return self.value == other.value

    def __ne__(self, other):
        return self.value != other.value

    def __hash__(self):
        return hash(self.value)

    def __repr__(self):
        return f"<RegionId: {self.value}>"

class PopulationId:
    def __init__(self, value=0):
        self.value = value

    def index(self):
        return self.value

    def __eq__(self, other):
        return self.value == other.value

    def __ne__(self, other):
        return self.value != other.value

    def __repr__(self):
        return f"<PopulationId: {self.value}>"

# Constants
INVALID_NEURON_ID = NeuronId()
INVALID_SYNAPSE_ID = SynapseId()
INVALID_REGION_ID = RegionId()
INVALID_POPULATION_ID = PopulationId()

# Predefined instances
DEFAULT_NEURON_ID = NeuronId(0)
DEFAULT_SYNAPSE_ID = SynapseId(0)
DEFAULT_REGION_ID = RegionId(0)
DEFAULT_POPULATION_ID = PopulationId(0)