#!/usr/bin/env python3
"""
NLM Sensory Module

Provides sensory processing capabilities for NLM (Neural Learning Machine).
"""

from typing import List, Optional
from .world import WorldObject

class SensoryInput:
    """
    Represents sensory input received by an NLM agent.
    
    This class encapsulates the information that an NLM agent receives
    from its environment through its sensory systems.
    """
    
    def __init__(self, inputs: List[float], timestamp: float = 0.0):
        """Create sensory input from a list of sensory values."""
        self._inputs = inputs
        self._timestamp = timestamp
    
    def getInputs(self) -> List[float]:
        """Get the sensory inputs."""
        return self._inputs.copy()
    
    def getValue(self, index: int) -> float:
        """Get the sensory input value at a specific index."""
        if 0 <= index < len(self._inputs):
            return self._inputs[index]
        return 0.0
    
    def __str__(self):
        return f"SensoryInput(inputs={self._inputs}, timestamp={self._timestamp})"

class Vision:
    """
    Visual sensory system for NLM agents.
    
    This class provides vision capabilities, allowing NLM agents to
    perceive objects in their environment through a 2D visual field.
    """
    
    def __init__(self, width: int = 20, height: int = 20):
        """Create a visual system with the given field of view."""
        self._width = width
        self._height = height
        self._field_of_view = [[0.0 for _ in range(height)] for _ in range(width)]
    
    def processWorld(self, objects: List[WorldObject], agent_x: float, agent_y: float):
        """Process objects in the world and update the visual field."""
        # Clear field of view
        for i in range(self._width):
            for j in range(self._height):
                self._field_of_view[i][j] = 0.0
        
        # Process objects within vision range
        vision_range = max(self._width, self._height) // 2
        
        for obj in objects:
            dx = obj.getX() - agent_x
            dy = obj.getY() - agent_y
            distance = (dx*dx + dy*dy) ** 0.5
            
            if distance <= vision_range:
                angle = (dx / vision_range) * 3.14159
                magnitude = 1.0 / (1.0 + distance)
                
                # Add object to field of view
                for i in range(self._width):
                    for j in range(self._height):
                        field_angle = (i / self._width) * 3.14159
                        field_dist = (j / self._height) * vision_range
                        
                        angle_diff = abs(angle - field_angle)
                        dist_diff = abs(distance - field_dist)
                        
                        if angle_diff < 0.1 and dist_diff < 1.0:
                            self._field_of_view[i][j] += magnitude * obj.getValue()
    
    def getFieldOfView(self) -> List[List[float]]:
        """Get the current field of view."""
        return [row.copy() for row in self._field_of_view]
    
    def getInputVector(self) -> List[float]:
        """Get the sensory input as a flattened vector."""
        input_vector = []
        for row in self._field_of_view:
            input_vector.extend(row)
        return input_vector
    
    def __str__(self):
        return f"Vision(field_of_view_size={self._width}x{self._height})"

class Audio:
    """
    Auditory sensory system for NLM agents.
    
    This class provides auditory capabilities, allowing NLM agents to
    hear sounds in their environment.
    """
    
    def __init__(self, range: float = 50.0):
        """Create an auditory system with the given range."""
        self._range = range
        self._sounds: List[dict] = []
    
    def addSound(self, x: float, y: float, intensity: float, frequency: float = 440.0):
        """Add a sound source at the given position."""
        self._sounds.append({
            'x': x,
            'y': y,
            'intensity': intensity,
            'frequency': frequency
        })
    
    def processSounds(self, agent_x: float, agent_y: float) -> SensoryInput:
        """Process sounds in the environment and return sensory input."""
        inputs = []
        
        for sound in self._sounds:
            dx = sound['x'] - agent_x
            dy = sound['y'] - agent_y
            distance = (dx*dx + dy*dy) ** 0.5
            
            if distance <= self._range:
                intensity = sound['intensity'] / (1.0 + distance)
                pitch = sound['frequency'] / 440.0  # Normalize to A4
                inputs.append(intensity * pitch)
        
        return SensoryInput(inputs)
    
    def __str__(self):
        return f"Audio(range={self._range}, sounds={len(self._sounds)})"

class InternalSignals:
    """
    Internal sensory system for NLM agents.
    
    This class provides internal sensory capabilities, allowing NLM agents
    to perceive their own internal state, such as hunger, thirst, and
    developmental needs.
    """
    
    def __init__(self):
        """Create an internal sensory system."""
        self._hunger = 0.0
        self._thirst = 0.0
        self._energy = 100.0
        self._developmental_need = 0.0
    
    def updateHunger(self, amount: float):
        """Update hunger level."""
        self._hunger = max(0.0, min(1.0, self._hunger + amount))
    
    def updateThirst(self, amount: float):
        """Update thirst level."""
        self._thirst = max(0.0, min(1.0, self._thirst + amount))
    
    def updateEnergy(self, amount: float):
        """Update energy level."""
        self._energy = max(0.0, min(100.0, self._energy + amount))
    
    def updateDevelopmentalNeed(self, amount: float):
        """Update developmental need level."""
        self._developmental_need = max(0.0, min(1.0, self._developmental_need + amount))
    
    def getSensoryInput(self) -> SensoryInput:
        """Get internal sensory input as a vector."""
        inputs = [
            self._hunger,
            self._thirst,
            self._energy / 100.0,  # Normalize to 0-1
            self._developmental_need,
        ]
        return SensoryInput(inputs)
    
    def __str__(self):
        return f"InternalSignals(hunger={self._hunger:.2f}, thirst={self._thirst:.2f}, energy={self._energy:.1f})"
