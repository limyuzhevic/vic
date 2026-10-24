"""
Core NLM classes - Direct bindings to C++ API with enhanced Python interface

Note: This is a stub implementation that provides the Python interface
for the C++ NLM framework. The actual C++ bindings are handled by pybind11
in the python/bindings.cpp file.
"""

import time
import math
from typing import List, Optional, Dict, Any, Union
from enum import Enum

# Import from the actual pybind11 bindings when available
try:
    # These would be imported from the C++ bindings
    # from .bindings import Brain as CPPBrain, Config as CPPConfig, etc.
    # For now, we'll provide stub classes that can be used
    pass
except ImportError:
    pass

# Import from local modules
from .types import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, ActionType, MotorCommand as MotorCommandEnum,
    WorldObjectType, DevelopmentalStage, FiringState,
    SynapseType, PlasticityFlags, FiringRate,
    MembranePotential, FiringRate, Timestamp, SimulationStep,
)

from .wrapper import Config as EnhancedConfig
class Config(EnhancedConfig):
    """Enhanced configuration class with Python-friendly interface"""
    
    def __init__(self):
        super().__init__()
        self._validation_rules = {}
    
    def load_from_json_file(self, filepath: str):
        """Load configuration from JSON file"""
        return self.loadFromFile(filepath)
    
    def save_to_json_file(self, filepath: str):
        """Save configuration to JSON file"""
        return self.saveToFile(filepath)
    
    def get_int(self, key: str, default: int = 0) -> int:
        """Get integer configuration value"""
        return self.getOrInt(key, default)
    
    def get_float(self, key: str, default: float = 0.0) -> float:
        """Get float configuration value"""
        return self.getOrFloat(key, default)
    
    def get_string(self, key: str, default: str = "") -> str:
        """Get string configuration value"""
        return self.getOrString(key, default)
    
    def validate(self) -> List[str]:
        """Validate configuration and return list of errors"""
        errors = []
        
        # Check required keys
        if not self.has("random_seed"):
            errors.append("Missing required key: random_seed")
        if not self.has("simulation_timestep"):
            errors.append("Missing required key: simulation_timestep")
        
        # Validate values
        if self.has("random_seed") and self.get_int("random_seed") < 0:
            errors.append("random_seed must be non-negative")
        if self.has("simulation_timestep") and self.get_float("simulation_timestep") <= 0:
            errors.append("simulation_timestep must be positive")
        
        return errors

    def summary_dict(self) -> Dict[str, Any]:
        """Get configuration as dictionary"""
        summary = {}
        keys = self.getKeys()
        for key in keys:
            if key in ["random_seed", "neuron_count", "region_count",
                      "connection_probability", "simulation_timestep",
                      "stdp_ltp_weight", "stdp_ltd_weight"]:
                summary[key] = self.get(key)
        return summary
    
    def __repr__(self):
        return f"<Config: {self.summary()}>"

    def __str__(self):
        return self.summary()

    # Add missing methods to match the enhanced interface
    def getOrInt(self, key: str, default: int = 0) -> int:
        return self.get_int(key, default)
    
    def getOrFloat(self, key: str, default: float = 0.0) -> float:
        return self.get_float(key, default)
    
    def getOrString(self, key: str, default: str = "") -> str:
        return self.get_string(key, default)
class Brain:
    """Enhanced brain class with Python-friendly interface and convenience methods"""
    
    def __init__(self, config: Optional[Config] = None):
        # Initialize with config
        self.config = config if config else Config()
        self._simulation_step = 0
        self._total_spikes = 0
        self._firing_neuron_count = 0
        self._neurons = []
        self._regions = []
        self._active_neuron_count = 0
        self._developmental_stage = DevelopmentalStage.Initial
        self._plasticity_flags = PlasticityFlags()
        
        # Quick initialization with defaults
        self.quick_initialize()
    
    def quick_initialize(self, neuron_count: int = 1000, regions: int = 1,
                        timestep: float = 0.001, seed: int = 42):
        """Quick initialization with common parameters"""
        self.config.set("random_seed", seed, None)
        self.config.set("simulation_timestep", timestep, None)
        self.config.set("neuron_count", neuron_count, None)
        self.config.set("region_count", regions, None)
        self.config.set("connection_probability", 0.15, None)
        
        self._simulation_step = 0
        self._total_spikes = 0
        self._firing_neuron_count = 0
        self._active_neuron_count = 0
        self._developmental_stage = DevelopmentalStage.Initial
        
        return self
    
    def easy_simulation_step(self, steps: int, current_time: Optional[float] = None):
        """Run multiple simulation steps with optional time"""
        if current_time is None:
            current_time = 0.0
        
        for step in range(steps):
            self.step(step, current_time + step * self.config.getOrFloat("simulation_timestep", 0.001))
        
        return self
    
    def inject_sensory_current(self, neuron_type: NeuronType, strength: float, duration: Optional[int] = None):
        """Inject sensory current for specified duration"""
        if duration is None:
            duration = 100
        
        for step in range(duration):
            self.injectCurrentToNeurons(neuron_type, strength)
            self.step(step, step * 0.001)
        
        return self
    
    def get_connectivity_stats(self) -> Dict[str, Any]:
        """Get brain connectivity statistics"""
        stats = {}
        stats["total_neurons"] = len(self._neurons)
        stats["total_synapses"] = sum(len(n.getIncomingSynapses()) for n in self._neurons)
        stats["active_neurons"] = self._active_neuron_count
        stats["firing_neurons"] = self._firing_neuron_count
        stats["excitation_inhibition_ratio"] = 1.0  # Simplified
        stats["total_spikes"] = self._total_spikes
        stats["average_firing_rate"] = self._firing_neuron_count / max(1, len(self._neurons))
        
        return stats
    
    def create_simple_action(self, action_type: ActionType, **parameters) -> Action:
        """Create a simple action with parameters"""
        from .motor import Action
        action = Action()
        action.setType(action_type)
        
        if parameters:
            action_params = list(parameters.values())
            action.setParameters(action_params)
        
        return action
    
    def get_developmental_summary(self) -> Dict[str, Any]:
        """Get developmental stage and related info"""
        return {
            "stage": self._developmental_stage,
            "stage_name": self._developmental_stage.name,
            "plasticity_enabled": self._plasticity_flags,
            "total_neurons": len(self._neurons),
            "active_neurons": self._active_neuron_count,
        }
    
    def simulate_plasticity_experiment(self, steps: int, sensory_strength: float = 30.0) -> Dict[str, Any]:
        """Run a plasticity learning experiment"""
        # Record initial state
        initial_weights = []
        
        initial_mean = sum(initial_weights) / len(initial_weights) if initial_weights else 0.0
        
        # Run simulation with plasticity enabled
        for step in range(steps):
            self.injectCurrentToNeurons(NeuronType.Sensory, sensory_strength)
            self.step(step, step * 0.001)
        
        # Record final state
        final_weights = []
        final_mean = sum(final_weights) / len(final_weights) if final_weights else 0.0
        
        # Calculate learning
        learning_change = final_mean - initial_mean
        
        return {
            "initial_mean_weight": initial_mean,
            "final_mean_weight": final_mean,
            "weight_change": learning_change,
            "learning_percent": (learning_change / initial_mean * 100) if initial_mean != 0 else 0.0,
            "total_spikes": self._total_spikes,
            "firing_rate": self._firing_neuron_count / max(1, len(self._neurons)),
        }
    
    # Stub implementations for C++ bindings methods
    def initialize(self) -> bool:
        """Initialize the brain with configuration"""
        self._simulation_step = 0
        self._total_spikes = 0
        self._firing_neuron_count = 0
        self._active_neuron_count = 0
        return True
    
    def step(self, currentStep: SimulationStep, currentTime: Optional[Timestamp] = None):
        """Perform a simulation step"""
        if currentTime is None:
            currentTime = currentStep * 0.001
        
        self._simulation_step = currentStep
        self._total_spikes += 5  # Simulate some spike activity
        self._firing_neuron_count = min(50, self._active_neuron_count + 5)
        
        return self
    
    def receiveSensoryInput(self, input):
        """Inject sensory input into the brain"""
        pass
    
    def injectCurrent(self, neuron: NeuronId, current: float):
        """Inject current into a specific neuron"""
        pass
    
    def injectCurrentToNeurons(self, type: NeuronType, current: float):
        """Inject current into all neurons of a specific type"""
        pass
    
    def produceAction(self):
        """Produce motor/action output based on neural activity"""
        from .motor import Action
        return Action()
    
    def reset(self):
        """Reset brain state"""
        self._simulation_step = 0
        self._total_spikes = 0
        self._firing_neuron_count = 0
        self._active_neuron_count = 0
        return self
    
    def save(self, filepath: str) -> bool:
        """Save brain state to file"""
        return True
    
    def load(self, filepath: str) -> bool:
        """Load brain state from file"""
        return True
    
    def addRegion(self, name: str = ""):
        """Add a new neural region"""
        return RegionId(len(self._regions))
    
    def getRegion(self, id: RegionId):
        """Get a region by ID"""
        return None
    
    def getRegionCount(self) -> int:
        """Get the number of regions"""
        return len(self._regions)
    
    def getRegionIds(self) -> List[RegionId]:
        """Get all region IDs"""
        return []
    
    def getRegions(self):
        """Get all regions"""
        return self._regions
    
    def getTotalNeuronCount(self) -> int:
        """Get total neuron count across all regions"""
        return len(self._neurons)
    
    def getTotalSynapseCount(self) -> int:
        """Get total synapse count across all regions"""
        return sum(len(n.getIncomingSynapses()) for n in self._neurons)
    
    def getActiveNeuronCount(self) -> int:
        """Get count of active neurons"""
        return self._active_neuron_count
    
    def getFiringNeuronCount(self) -> int:
        """Get count of currently firing neurons"""
        return self._firing_neuron_count
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate across all neurons"""
        return self._firing_neuron_count / max(1, len(self._neurons))
    
    def getExcitationInhibitionRatio(self) -> float:
        """Get excitation/inhibition balance ratio"""
        return 1.0
    
    def getTotalSpikeCount(self) -> int:
        """Get total spike count"""
        return self._total_spikes
    
    def getPendingSpikeEventCount(self) -> int:
        """Get pending spike event count"""
        return 0
    
    def getDevelopmentalStage(self) -> DevelopmentalStage:
        """Get current developmental stage"""
        return self._developmental_stage
    
    def setDevelopmentalStage(self, stage: DevelopmentalStage):
        """Set developmental stage"""
        self._developmental_stage = stage
        return self
    
    def getConfig(self):
        """Get the configuration"""
        return self.config
    
    def logStatus(self):
        """Log brain status"""
        print(f"Brain status: "
              f"Step={self._simulation_step}, "
              f"Total spikes={self._total_spikes}, "
              f"Active neurons={self._active_neuron_count}, "
              f"Firing neurons={self._firing_neuron_count}, "
              f"Stage={self._developmental_stage.name}")
    
    def getPlasticityFlags(self) -> PlasticityFlags:
        """Get plasticity flags"""
        return self._plasticity_flags
class AgentBrain:
    """Agent brain interface connecting NLM brain to world"""
    
    def __init__(self, brain: Brain):
        self.brain = brain
        self.reward_modulation_enabled = False
        self.structural_plasticity_enabled = False
        self.development_enabled = False
        self.curiosity_enabled = False
    
    def initialize(self, world=None) -> bool:
        """Initialize with world"""
        return True
    
    def getSensoryInputSize(self) -> int:
        """Get expected sensory input size"""
        return 1
    
    def getMotorOutputSize(self) -> int:
        """Get expected motor output size"""
        return 1
    
    def processSensoryInput(self, percept):
        """Process sensory percept and inject into brain"""
        pass
    
    def decodeMotorCommand(self):
        """Decode brain motor activity into motor command"""
        from .motor import Action
        return Action()
    
    def applyRewardModulation(self, reward: float, predicted_reward: float):
        """Apply reward-based neuromodulation"""
        pass
    
    def updateDevelopment(self, timestep: float):
        """Update development system"""
        pass
    
    def getDevelopmentalStage(self) -> DevelopmentalStage:
        """Get current developmental stage"""
        return self.brain.getDevelopmentalStage()
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level"""
        return 0.5
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level"""
        return 0.3
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level"""
        return 0.2
    
    def getPredictionError(self) -> float:
        """Get prediction error"""
        return 0.1
    
    def reset(self):
        """Reset agent for new episode"""
        self.brain.reset()
    
    def getBrain(self) -> Brain:
        """Get the underlying brain"""
        return self.brain
    
    def enableRewardModulation(self, enable: bool):
        """Enable/disable reward modulation"""
        self.reward_modulation_enabled = enable
    
    def enableStructuralPlasticity(self, enable: bool):
        """Enable/disable structural plasticity"""
        self.structural_plasticity_enabled = enable
    
    def enableDevelopment(self, enable: bool):
        """Enable/disable development"""
        self.development_enabled = enable
    
    def enableCuriosity(self, enable: bool):
        """Enable/disable curiosity"""
        self.curiosity_enabled = enable
    
    def isRewardModulationEnabled(self) -> bool:
        """Check if reward modulation is enabled"""
        return self.reward_modulation_enabled
    
    def isStructuralPlasticityEnabled(self) -> bool:
        """Check if structural plasticity is enabled"""
        return self.structural_plasticity_enabled
    
    def isDevelopmentEnabled(self) -> bool:
        """Check if development is enabled"""
        return self.development_enabled
    
    def isCuriosityEnabled(self) -> bool:
        """Check if curiosity is enabled"""
        return self.curiosity_enabled
class SimpleWorld:
    """Enhanced simple world with Python-friendly interface"""
    
    def __init__(self):
        self.width = 800
        self.height = 600
        self.vision_width = 80
        self.vision_height = 80
        self.agent = AgentBody()
        self.objects = []
        self.simulation_time = 0.0
        self.random_seed = 42
    
    def configure(self, width: int, height: int,
                  vision_width: int, vision_height: int):
        """Configure world dimensions"""
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height
    
    def reset(self):
        """Reset world state"""
        self.agent.reset()
        self.objects.clear()
        self.simulation_time = 0.0
    
    def setAgentStart(self, x: float, y: float):
        """Set agent starting position"""
        self.agent.x = x
        self.agent.y = y
        self.agent.isMoving = False
        self.agent.isTurning = False
    
    def update(self, timestep: float):
        """Update world simulation"""
        self.simulation_time += timestep
        self.agent.age += 1
        
        # Update agent physics
        if self.agent.isMoving:
            self.agent.x += self.agent.velocityX * timestep
            self.agent.y += self.agent.velocityY * timestep
        
        # Check boundaries
        self.agent.x = max(0, min(self.width - self.agent.radius, self.agent.x))
        self.agent.y = max(0, min(self.height - self.agent.radius, self.agent.y))
    
    def applyMotorCommand(self, action, current_time: float):
        """Apply motor command to world"""
        from .motor import Action, ActionType
        
        if isinstance(action, Action):
            if action.getType() == ActionType.MoveForward:
                self.agent.isMoving = True
                self.agent.velocityX = 2.0
                self.agent.velocityY = 0.0
            elif action.getType() == ActionType.MoveBackward:
                self.agent.isMoving = True
                self.agent.velocityX = -2.0
                self.agent.velocityY = 0.0
            elif action.getType() == ActionType.TurnLeft:
                self.agent.isTurning = True
                self.agent.angularVelocity = 0.05
            elif action.getType() == ActionType.TurnRight:
                self.agent.isTurning = True
                self.agent.angularVelocity = -0.05
            else:
                self.agent.isMoving = False
                self.agent.isTurning = False
                self.agent.velocityX = 0.0
                self.agent.velocityY = 0.0
                self.agent.angularVelocity = 0.0
        
        self.agent.lastActionTime = current_time
        self.update(0.001)
    
    def getSensoryPercept(self):
        """Get sensory percept from world"""
        from .sensory import SensoryPercept
        return SensoryPercept()
    
    def getAgentBody(self):
        """Get agent body"""
        return self.agent
    
    def addObject(self, obj):
        """Add world object"""
        self.objects.append(obj)
    
    def removeObject(self, x: float, y: float):
        """Remove object at position"""
        self.objects = [obj for obj in self.objects 
                       if not (abs(obj.x - x) < 0.1 and abs(obj.y - y) < 0.1)]
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if position is valid (not occupied by wall)"""
        return not any(
            abs(obj.x - x) < 0.1 and abs(obj.y - y) < 0.1 and obj.obj_type == WorldObjectType.Wall
            for obj in self.objects
        )
    
    def getWidth(self) -> int:
        """Get world width"""
        return self.width
    
    def getHeight(self) -> int:
        """Get world height"""
        return self.height
    
    def getMaxEnergy(self) -> float:
        """Get max energy"""
        return 100.0
    
    def setMaxEnergy(self, e: float):
        """Set max energy"""
        pass
    
    def getEnergyDecayRate(self) -> float:
        """Get energy decay rate"""
        return 0.01
    
    def setEnergyDecayRate(self, r: float):
        """Set energy decay rate"""
        pass
    
    def getSimulationTime(self) -> float:
        """Get simulation time"""
        return self.simulation_time
    
    def setRandomSeed(self, seed: int):
        """Set random seed"""
        self.random_seed = seed
    
    def getRandomSeed(self) -> int:
        """Get random seed"""
        return self.random_seed

    def getObjects(self):
        """Get all objects"""
        return self.objects

    def get_region_by_type(self, obj_type):
        """Get regions by type"""
        return [obj for obj in self.objects if obj.obj_type == obj_type]
    
    def get_resource_objects(self):
        """Get all resource objects"""
        return self.get_region_by_type(WorldObjectType.Resource)
    
    def get_hazard_objects(self):
        """Get all hazard objects"""
        return self.get_region_by_type(WorldObjectType.Hazard)

    def get_wall_objects(self):
        """Get all wall objects"""
        return self.get_region_by_type(WorldObjectType.Wall)

    def get_simulation_summary(self):
        """Get world simulation summary"""
        return {
            "width": self.width,
            "height": self.height,
            "agent_x": self.agent.x,
            "agent_y": self.agent.y,
            "simulation_time": self.simulation_time,
            "resource_count": len(self.get_resource_objects()),
            "hazard_count": len(self.get_hazard_objects()),
            "wall_count": len(self.get_wall_objects()),
        }
class SensoryPercept:
    """Enhanced sensory percept with Python-friendly interface"""
    
    def __init__(self):
        self.vision = None
        self.internal = None
    
    def getVision(self):
        """Get vision"""
        return self.vision
    
    def setVision(self, vision):
        """Set vision"""
        self.vision = vision
    
    def getVisionWidth(self) -> int:
        """Get vision width"""
        return self.vision.getWidth() if self.vision else 0
    
    def getVisionHeight(self) -> int:
        """Get vision height"""
        return self.vision.getHeight() if self.vision else 0
    
    def getTouch(self):
        """Get touch"""
        return None
    
    def setTouch(self, touch):
        """Set touch"""
        pass
    
    def getInternal(self):
        """Get internal signals"""
        return self.internal
    
    def setInternal(self, internal):
        """Set internal signals"""
        self.internal = internal
    
    def getProprioception(self):
        """Get proprioception"""
        return None
    
    def setProprioception(self, proprioception):
        """Set proprioception"""
        pass
    
    def getAudio(self):
        """Get audio"""
        return None
    
    def setAudio(self, audio):
        """Set audio"""
        pass
    
    def getAllSignals(self):
        """Get all signals"""
        return []
    
    def getTimestamp(self) -> float:
        """Get timestamp"""
        return time.time()
    
    def setTimestamp(self, timestamp: float):
        """Set timestamp"""
        pass
    
    def get_vision_center(self) -> float:
        """Get center brightness of vision"""
        if self.vision:
            return self.vision.get_center_brightness() if hasattr(self.vision, 'get_center_brightness') else 0.0
        return 0.0
    
    def get_total_signal_strength(self) -> Dict[str, float]:
        """Get total signal strength from all modalities"""
        internal_signals = self.internal.get_signal_range() if self.internal else {"mean": 0.0}
        
        return {
            "vision_strength": self.get_vision_center(),
            "internal_strength": internal_signals["mean"],
            "total_strength": self.get_vision_center() + internal_signals["mean"],
        }
class Vision:
    """Enhanced Vision class with Python-friendly interface"""
    
    def __init__(self, width: int = 80, height: int = 80, channels: int = 3):
        self.width = width
        self.height = height
        self.channels = channels
        self.data = None
    
    def getWidth(self) -> int:
        """Get width"""
        return self.width
    
    def getHeight(self) -> int:
        """Get height"""
        return self.height
    
    def getChannels(self) -> int:
        """Get channels"""
        return self.channels
    
    def setData(self, data):
        """Set data"""
        self.data = data
    
    def get_center_brightness(self) -> float:
        """Get center brightness"""
        if self.data:
            if isinstance(self.data, list) and len(self.data) > 0:
                if isinstance(self.data[0], list):
                    center_x, center_y = len(self.data) // 2, len(self.data[0]) // 2
                    if center_x < len(self.data) and center_y < len(self.data[0]):
                        return self.data[center_x][center_y]
            elif isinstance(self.data, list):
                return sum(self.data) / len(self.data) if self.data else 0.0
        return 0.0
class Audio:
    """Enhanced Audio class with Python-friendly interface"""
    
    def __init__(self, sample_rate: int = 44100, num_samples: int = 1024):
        self.sample_rate = sample_rate
        self.num_samples = num_samples
        self.data = None
    
    def setData(self, data):
        """Set data"""
        self.data = data
    
    def getSampleRate(self) -> int:
        """Get sample rate"""
        return self.sample_rate
    
    def getNumSamples(self) -> int:
        """Get number of samples"""
        return self.num_samples
class InternalSignals:
    """Enhanced internal signals with Python-friendly interface"""
    
    def __init__(self):
        self.signals = []
    
    def addSignal(self, value: float):
        """Add signal"""
        self.signals.append(value)
    
    def getData(self) -> List[float]:
        """Get signals"""
        return self.signals
    
    def get_signal_range(self) -> Dict[str, float]:
        """Get signal statistics"""
        if not self.signals:
            return {"count": 0, "min": 0.0, "max": 0.0, "mean": 0.0}
        
        return {
            "count": len(self.signals),
            "min": min(self.signals),
            "max": max(self.signals),
            "mean": sum(self.signals) / len(self.signals),
        }
class Action:
    """Action representation for motor output"""
    
    def __init__(self, action_type: Optional[ActionType] = None, parameters: Optional[List[float]] = None):
        self.action_type = action_type or ActionType.Wait
        self.parameters = parameters or []
        self.timestamp = None
    
    def getType(self) -> ActionType:
        """Get action type"""
        return self.action_type
    
    def setType(self, action_type: ActionType):
        """Set action type"""
        self.action_type = action_type
    
    def getParameters(self) -> List[float]:
        """Get action parameters"""
        return self.parameters
    
    def setParameters(self, params: List[float]):
        """Set action parameters"""
        self.parameters = params
    
    def getName(self) -> str:
        """Get action name"""
        return self.action_type.name if self.action_type else "Wait"
    
    def clone(self) -> 'Action':
        """Clone action"""
        new_action = Action(self.action_type, self.parameters.copy())
        new_action.timestamp = self.timestamp
        return new_action
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary"""
        return {
            "type": self.action_type,
            "type_name": self.getName(),
            "parameters": self.parameters,
            "timestamp": self.timestamp,
        }
class AgentBody:
    """Agent body state"""
    
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
        """Reset agent body"""
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
class WorldObject:
    """World object representation"""
    
    def __init__(self, x: float = 0.0, y: float = 0.0,
                 obj_type: WorldObjectType = WorldObjectType.Empty,
                 value: float = 0.0, radius: float = 0.5):
        self.x = x
        self.y = y
        self.obj_type = obj_type
        self.value = value
        self.radius = radius
        self.active = True
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary"""
        return {
            "x": self.x,
            "y": self.y,
            "type": self.obj_type,
            "type_name": self.obj_type.name if self.obj_type else "",
            "value": self.value,
            "radius": self.radius,
            "active": self.active,
        }
# Import missing dependencies
Action = Action
WorldObjectType = WorldObjectType
