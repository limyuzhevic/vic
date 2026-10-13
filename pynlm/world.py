"""Simple world implementation."""

from typing import List, Any
from .brain.neuron import NeuronId
from .sensory import SensoryPercept

class WorldObject:
    """World object representation."""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, obj_type: int = 0, 
                 value: float = 0.0, radius: float = 0.5):
        self._x = x
        self._y = y
        self._type = obj_type
        self._value = value
        self._radius = radius
        self._active = True
    
    def getX(self) -> float:
        return self._x
    
    def getY(self) -> float:
        return self._y
    
    def getType(self) -> int:
        return self._type
    
    def getValue(self) -> float:
        return self._value
    
    def getRadius(self) -> float:
        return self._radius
    
    def isActive(self) -> bool:
        return self._active
    
    def setActive(self, active: bool):
        self._active = active

class AgentBody:
    """Agent body state."""
    
    def __init__(self):
        self._x = 0.0
        self._y = 0.0
        self._orientation = 0.0
        self._velocity_x = 0.0
        self._velocity_y = 0.0
        self._angular_velocity = 0.0
        self._energy = 100.0
        self._health = 1.0
        self._age = 0
        self._is_moving = False
        self._is_turning = False
        self._last_action_time = 0.0
    
    def reset(self):
        self._x = 0.0
        self._y = 0.0
        self._orientation = 0.0
        self._velocity_x = 0.0
        self._velocity_y = 0.0
        self._angular_velocity = 0.0
        self._energy = 100.0
        self._health = 1.0
        self._age = 0
        self._is_moving = False
        self._is_turning = False
        self._last_action_time = 0.0

class SensoryPercept:
    """Sensory percept data."""
    
    def __init__(self):
        self._vision = None
        self._touch: List[float] = []
        self._internal: List[float] = []
        self._proprioception: List[float] = []
        self._audio: List[float] = []
        self._timestamp = 0.0
    
    def getVision(self):
        return self._vision
    
    def setVision(self, vision):
        self._vision = vision
    
    def getTouch(self) -> List[float]:
        return self._touch
    
    def setTouch(self, touch: List[float]):
        self._touch = touch
    
    def getInternal(self) -> List[float]:
        return self._internal
    
    def setInternal(self, internal: List[float]):
        self._internal = internal
    
    def getProprioception(self) -> List[float]:
        return self._proprioception
    
    def setProprioception(self, proprioception: List[float]):
        self._proprioception = proprioception
    
    def getAudio(self) -> List[float]:
        return self._audio
    
    def setAudio(self, audio: List[float]):
        self._audio = audio
    
    def getAllSignals(self) -> List[float]:
        signals = []
        signals.extend(self._vision.getData() if self._vision else [])
        signals.extend(self._touch)
        signals.extend(self._internal)
        signals.extend(self._proprioception)
        signals.extend(self._audio)
        return signals
    
    def getTimestamp(self) -> float:
        return self._timestamp
    
    def setTimestamp(self, timestamp: float):
        self._timestamp = timestamp

class SimpleWorld:
    """Simple 2D world for NLM simulation."""
    
    def __init__(self):
        self._width = 0
        self._height = 0
        self._vision_width = 0
        self._vision_height = 0
        self._agent_x = 0.0
        self._agent_y = 0.0
        self._agent_orientation = 0.0
        self._simulation_time = 0.0
        self._max_energy = 100.0
        self._energy_decay_rate = 0.01
        self._random_seed = 42
        self._objects: List[WorldObject] = []
        self._agent_body = AgentBody()
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int):
        self._width = width
        self._height = height
        self._vision_width = vision_width
        self._vision_height = vision_height
    
    def reset(self):
        self._agent_x = 0.0
        self._agent_y = 0.0
        self._agent_orientation = 0.0
        self._simulation_time = 0.0
        self._agent_body.reset()
        self._objects.clear()
    
    def setAgentStart(self, x: float, y: float):
        self._agent_x = x
        self._agent_y = y
        self._agent_body._x = x
        self._agent_body._y = y
    
    def update(self, timestep: float):
        self._simulation_time += timestep
    
    def getSensoryPercept(self) -> SensoryPercept:
        percept = SensoryPercept()
        percept.setTimestamp(self._simulation_time)
        
        # Simple vision (just center)
        from .sensory import Vision
        vision = Vision(self._vision_width, self._vision_height)
        vision.setData([0.5] * (self._vision_width * self._vision_height * 3))
        percept.setVision(vision)
        
        # Simple internal signals
        percept.setInternal([self._agent_body._energy, self._agent_body._health])
        
        return percept
    
    def getAgentBody(self) -> AgentBody:
        return self._agent_body
    
    def addObject(self, obj: WorldObject):
        self._objects.append(obj)
    
    def removeObject(self, x: float, y: float):
        self._objects = [obj for obj in self._objects if not (abs(obj.getX() - x) < 0.1 and abs(obj.getY() - y) < 0.1)]
    
    def isValidPosition(self, x: float, y: float) -> bool:
        return (0.0 <= x < self._width and 0.0 <= y < self._height and
                not any(abs(obj.getX() - x) < 0.1 and abs(obj.getY() - y) < 0.1 for obj in self._objects if not obj.isActive()))
    
    def getWidth(self) -> int:
        return self._width
    
    def getHeight(self) -> int:
        return self._height
    
    def getMaxEnergy(self) -> float:
        return self._max_energy
    
    def setMaxEnergy(self, energy: float):
        self._max_energy = energy
    
    def getEnergyDecayRate(self) -> float:
        return self._energy_decay_rate
    
    def setEnergyDecayRate(self, rate: float):
        self._energy_decay_rate = rate
    
    def getSimulationTime(self) -> float:
        return self._simulation_time
    
    def setRandomSeed(self, seed: int):
        self._random_seed = seed
    
    def getRandomSeed(self) -> int:
        return self._random_seed

__all__ = ['WorldObject', 'AgentBody', 'SensoryPercept', 'SimpleWorld']