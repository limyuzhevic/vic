#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Core Types Module

This module provides type definitions and data structures for the NLM neural simulation framework.
It includes neuron IDs, synapse IDs, region IDs, population IDs, neuron types, synapse types,
development stages, firing states, action types, motor commands, world object types,
and other fundamental data types.

License: MIT
Author: NLM Authors
Version: 0.1.0
"""

import numpy as np
from typing import Dict, List, Any, Optional, Union, Tuple
from dataclasses import dataclass, field
from enum import Enum
import time
from uuid import uuid4

# Version
__version__ = "0.1.0"

@dataclass
class NeuronId:
    """Neuron identifier."""
    id: str
    
    def __post_init__(self):
        if not self.id:
            self.id = str(uuid4())
class SynapseId:
    """Synapse identifier."""
    id: str
    
    def __post_init__(self):
        if not self.id:
            self.id = str(uuid4())
class RegionId:
    """Neural region identifier."""
    id: str
    
    def __post_init__(self):
        if not self.id:
            self.id = str(uuid4())
class PopulationId:
    """Neural population identifier."""
    id: str
    
    def __post_init__(self):
        if not self.id:
            self.id = str(uuid4())
class NeuronType(Enum):
    """Neuron type enumeration."""
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    SENSORY = "sensory"
    INTERNEURON = "interneuron"
    MOTOR = "motor"
    GLIAL = "glial"
    STEM = "stem"
class SynapseType(Enum):
    """Synapse type enumeration."""
    CHEMICAL = "chemical"
    ELECTRICAL = "electrical"
    GATED = "gated"
    GAP_JUNCTION = "gap_junction"
    NEUROMODULATORY = "neuromodulatory"
    STRUCTURAL = "structural"
    PLASTIC = "plastic"
    STATIC = "static"
class DevelopmentalStage(Enum):
    """Brain development stage enumeration."""
    EMBRYONIC = "embryonic"
    FETAL = "fetal"
    NEONATAL = "neonatal"
    JUVENILE = "juvenile"
    ADULT = "adult"
    AGED = "aged"
class FiringState(Enum):
    """Neuron firing state enumeration."""
    RESTING = "resting"
    DEPOLARIZING = "depolarizing"
    FIRING = "firing"
    REFRACTORY = "refractory"
    HYPERPOLARIZING = "hyperpolarizing"
    RESET = "reset"
class ActionType(Enum):
    """Action type enumeration."""
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    ROTATE = "rotate"
    GRAB = "grab"
    RELEASE = "release"
    SPEAK = "speak"
    LISTEN = "listen"
    LOOK = "look"
    EXPLORE = "explore"
    ATTACK = "attack"
    DEFEND = "defend"
class MotorCommand:
    """Motor command structure."""
    
    def __init__(self, command_type: ActionType = ActionType.MOVE_FORWARD, 
                 parameters: Dict[str, float] = None):
        self.command_type = command_type
        self.parameters = parameters or {}
        self.timestamp = time.time()
        self.duration = parameters.get('duration', 0.1) if parameters else 0.1
        
    def get_parameter(self, key: str, default: float = 0.0) -> float:
        """Get parameter value."""
        return self.parameters.get(key, default)
        
    def set_parameter(self, key: str, value: float) -> None:
        """Set parameter value."""
        self.parameters[key] = value
class WorldObjectType(Enum):
    """World object type enumeration."""
    AGENT = "agent"
    RESOURCE = "resource"
    OBSTACLE = "obstacle"
    WALL = "wall"
    DOOR = "door"
    WINDOW = "window"
    PLANT = "plant"
    ANIMAL = "animal"
    BUILDING = "building"
    LIGHT = "light"
    WATER = "water"
    FOOD = "food"
    TOY = "toy"
class MembranePotential:
    """Neuron membrane potential."""
    
    def __init__(self, resting_potential: float = -70.0, 
                 threshold_potential: float = -55.0,
                 max_potential: float = 50.0,
                 min_potential: float = -100.0):
        self.resting_potential = resting_potential
        self.threshold_potential = threshold_potential
        self.max_potential = max_potential
        self.min_potential = min_potential
        self.current_potential = resting_potential
        self.time_constant = 10.0
        self.capacitance = 1.0
        
    def update_potential(self, delta_v: float) -> None:
        """Update membrane potential."""
        self.current_potential += delta_v
        self.current_potential = max(self.min_potential, 
                                   min(self.max_potential, self.current_potential))
        
    def is_firing(self) -> bool:
        """Check if neuron is firing."""
        return self.current_potential >= self.threshold_potential
        
    def get_excitability(self) -> float:
        """Get neuron excitability."""
        return (self.current_potential - self.resting_potential) / (
            self.threshold_potential - self.resting_potential)
class SimulationStep:
    """Simulation step counter."""
    
    def __init__(self, step: int = 0):
        self.step = step
        
    def increment(self) -> None:
        """Increment step counter."""
        self.step += 1
        
    def reset(self) -> None:
        """Reset step counter."""
        self.step = 0
class Timestamp:
    """Simulation timestamp."""
    
    def __init__(self, seconds: float = 0.0):
        self.seconds = seconds
        self.step = 0
        
    def advance(self, delta_seconds: float) -> None:
        """Advance timestamp."""
        self.seconds += delta_seconds
class Delay:
    """Synaptic delay."""
    
    def __init__(self, delay_ms: float = 5.0):
        self.delay_ms = delay_ms
        self.samples = int(delay_ms * 0.001 * 1000)  # Assuming 1kHz sampling
class PlasticityFlags:
    """Synaptic plasticity flags."""
    
    def __init__(self, stdp_enabled: bool = True, 
                 hebbian_enabled: bool = True,
                 neuromodulation_enabled: bool = True,
                 structural_enabled: bool = False):
        self.stdp_enabled = stdp_enabled
        self.hebbian_enabled = hebbian_enabled
        self.neuromodulation_enabled = neuromodulation_enabled
        self.structural_enabled = structural_enabled