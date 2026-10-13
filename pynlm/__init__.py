"""Complete Brain implementation with advanced features for NLM."""

from typing import List, Optional, Dict, Any, Tuple
import time
from enum import Enum
from abc import ABC, abstractmethod

# Core type definitions
class SimulationStep:
    def __init__(self, value: int = 0):
        self.value = value
    
    def __str__(self):
        return f"Step({self.value})"

class Timestamp:
    def __init__(self, value: float = 0.0):
        self.value = value
    
    def __str__(self):
        return f"Time({self.value}s)"

class MembranePotential:
    def __init__(self, value: float = -70.0):
        self.value = value
    
    def __str__(self):
        return f"V({self.value}mV)"

class Delay:
    def __init__(self, value: int = 1):
        self.value = value
    
    def __str__(self):
        return f"Delay({self.value})"

class PlasticityFlags:
    def __init__(self):
        self.hebbian = False
        self.stdp = False
        self.reward_modulated = False
    
    def __str__(self):
        return f"Plasticity(h={self.hebbian}, stdp={self.stdp}, reward={self.reward_modulated})"

# Core ID classes
class NeuronId:
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        return self.value
    
    def __eq__(self, other):
        return isinstance(other, NeuronId) and self.value == other.value
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self.value)
    
    def __str__(self):
        return f"NeuronId({self.value})"

class SynapseId:
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        return self.value
    
    def __eq__(self, other):
        return isinstance(other, SynapseId) and self.value == other.value
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self.value)
    
    def __str__(self):
        return f"SynapseId({self.value})"

class RegionId:
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        return self.value
    
    def __eq__(self, other):
        return isinstance(other, RegionId) and self.value == other.value
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self.value)
    
    def __str__(self):
        return f"RegionId({self.value})"

class PopulationId:
    def __init__(self, value: int = 0):
        self.value = value
    
    def index(self) -> int:
        return self.value
    
    def __eq__(self, other):
        return isinstance(other, PopulationId) and self.value == other.value
    
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __hash__(self):
        return hash(self.value)
    
    def __str__(self):
        return f"PopulationId({self.value})"

# Enumeration classes
class NeuronType(Enum):
    EXCITATORY = 0
    INHIBITORY = 1
    MODULATORY = 2
    SENSORY = 3
    MOTOR = 4
    INTERNAL = 5

class SynapseType(Enum):
    EXCITATORY = 0
    INHIBITORY = 1
    MODULATORY = 2
    ELECTRICAL = 3
    GAP_JUNCTION = 4

class FiringState(Enum):
    RESTING = 0
    ACTIVE = 1
    REFRACTORY = 2
    INHIBITED = 3

class DevelopmentalStage(Enum):
    INITIAL = 0
    CRITICAL_PERIOD = 1
    MATURATION = 2
    ADULT = 3
    AGING = 4

class ActionType(Enum):
    MOVE_FORWARD = 0
    MOVE_BACKWARD = 1
    MOVE_LEFT = 2
    MOVE_RIGHT = 3
    TURN_LEFT = 4
    TURN_RIGHT = 5
    LOOK = 6
    LOOK_UP = 7
    LOOK_DOWN = 8
    INTERACT = 9
    EAT = 10
    DRINK = 11
    REST = 12
    WAIT = 13
    CUSTOM = 14

class WorldObjectType(Enum):
    EMPTY = 0
    RESOURCE = 1
    HAZARD = 2
    WALL = 3
    MARKER = 4

# Constants for invalid IDs
INVALID_NEURON_ID = NeuronId(0)
INVALID_SYNAPSE_ID = SynapseId(0)
INVALID_REGION_ID = RegionId(0)
INVALID_POPULATION_ID = PopulationId(0)

# Event system
class Event:
    def __init__(self, event_type: str, timestamp: float, data: Dict = None):
        self.event_type = event_type
        self.timestamp = timestamp
        self.data = data or {}
    
    def __str__(self):
        return f"Event({self.event_type}, {self.timestamp}s)"

class EventSystem:
    """Event-driven neural event system."""
    
    def __init__(self):
        self.events: List[Event] = []
        self.subscribers: Dict[str, List] = {}
    
    def emit(self, event_type: str, timestamp: float, data: Dict = None):
        """Emit an event."""
        event = Event(event_type, timestamp, data)
        self.events.append(event)
        
        # Notify subscribers
        if event_type in self.subscribers:
            for callback in self.subscribers[event_type]:
                callback(event)
    
    def subscribe(self, event_type: str, callback):
        """Subscribe to events of a specific type."""
        if event_type not in self.subscribers:
            self.subscribers[event_type] = []
        self.subscribers[event_type].append(callback)
    
    def get_events(self, event_type: str = None) -> List[Event]:
        """Get events, optionally filtered by type."""
        if event_type is None:
            return self.events.copy()
        return [e for e in self.events if e.event_type == event_type]

# Core neural components
class Synapse:
    """Complete synapse implementation with advanced features."""
    
    def __init__(self, synapse_id: SynapseId, source_neuron: NeuronId, dest_neuron: NeuronId):
        self._id = synapse_id
        self._source_neuron = source_neuron
        self._dest_neuron = dest_neuron
        self._weight = 0.0
        self._type = SynapseType.EXCITATORY
        self._delay = Delay(1)
        self._plasticity_flags = PlasticityFlags()
        self._pre_spike_history: List[Tuple[float, float]] = []  # (time, amplitude)
        self._post_spike_history: List[Tuple[float, float]] = []
        self._eligibility_trace = 0.0
        self._efficacy = 1.0
        self._last_pre_spike_time = 0.0
        self._last_post_spike_time = 0.0
    
    def getId(self) -> SynapseId:
        return self._id
    
    def getSourceNeuron(self) -> NeuronId:
        return self._source_neuron
    
    def getDestinationNeuron(self) -> NeuronId:
        return self._dest_neuron
    
    def getWeight(self) -> float:
        return self._weight
    
    def setWeight(self, weight: float):
        """Set synapse weight with bounds checking."""
        # Apply weight bounds [-1.0, 1.0]
        self._weight = max(-1.0, min(1.0, weight))
    
    def addToWeight(self, delta: float):
        """Add delta to synapse weight."""
        self.setWeight(self._weight + delta)
    
    def getType(self) -> SynapseType:
        return self._type
    
    def setType(self, synapse_type: SynapseType):
        self._type = synapse_type
    
    def isExcitatory(self) -> bool:
        return self._type == SynapseType.EXCITATORY
    
    def isInhibitory(self) -> bool:
        return self._type == SynapseType.INHIBITORY
    
    def getDelay(self) -> Delay:
        return self._delay
    
    def setDelay(self, delay: Delay):
        self._delay = delay
    
    def getPreSpikeHistory(self) -> List[Tuple[float, float]]:
        return self._pre_spike_history.copy()
    
    def getPostSpikeHistory(self) -> List[Tuple[float, float]]:
        return self._post_spike_history.copy()
    
    def recordPreSpike(self, time: float, amplitude: float = 1.0):
        """Record presynaptic spike."""
        self._pre_spike_history.append((time, amplitude))
        self._last_pre_spike_time = time
    
    def recordPostSpike(self, time: float, amplitude: float = 1.0):
        """Record postsynaptic spike."""
        self._post_spike_history.append((time, amplitude))
        self._last_post_spike_time = time
    
    def clearHistory(self):
        """Clear all spike history."""
        self._pre_spike_history.clear()
        self._post_spike_history.clear()
    
    def getEligibilityTrace(self) -> float:
        return self._eligibility_trace
    
    def setEligibilityTrace(self, trace: float):
        self._eligibility_trace = trace
    
    def decayEligibilityTrace(self, decay_rate: float, dt: float = 0.001):
        """Decay eligibility trace."""
        self._eligibility_trace *= (1.0 - decay_rate * dt)
    
    def getEfficacy(self) -> float:
        return self._efficacy
    
    def setEfficacy(self, efficacy: float):
        """Set synapse efficacy with bounds."""
        self._efficacy = max(0.0, min(2.0, efficacy))
    
    def getPlasticityFlags(self) -> PlasticityFlags:
        return self._plasticity_flags
    
    def enablePlasticity(self, hebbian: bool, stdp: bool, reward_modulated: bool):
        """Enable plasticity mechanisms."""
        self._plasticity_flags.hebbian = hebbian
        self._plasticity_flags.stdp = stdp
        self._plasticity_flags.reward_modulated = reward_modulated
    
    def getTimeSincePreSpike(self, current_time: float) -> float:
        """Get time since last presynaptic spike."""
        return current_time - self._last_pre_spike_time
    
    def getTimeSincePostSpike(self, current_time: float) -> float:
        """Get time since last postsynaptic spike."""
        return current_time - self._last_post_spike_time
    
    def reset(self):
        """Reset synapse to initial state."""
        self._weight = 0.0
        self._type = SynapseType.EXCITATORY
        self._delay = Delay(1)
        self._plasticity_flags = PlasticityFlags()
        self._pre_spike_history.clear()
        self._post_spike_history.clear()
        self._eligibility_trace = 0.0
        self._efficacy = 1.0

class Neuron:
    """Complete neuron implementation with LIF dynamics and advanced features."""
    
    def __init__(self, neuron_id: NeuronId):
        self._id = neuron_id
        self._type = NeuronType.INTERNAL
        
        # Membrane dynamics
        self._membrane_potential = MembranePotential(-70.0)
        self._threshold = MembranePotential(-50.0)
        self._resting_potential = MembranePotential(-70.0)
        self._reset_potential = MembranePotential(-75.0)
        
        # Time constants
        self._membrane_time_constant = 20.0  # ms
        self._refractory_time_constant = 2.0  # ms
        
        # State variables
        self._current = 0.0
        self._refractory_period = 0.0
        self._firing_state = FiringState.RESTING
        self._spike_count = 0
        
        # History tracking
        self._spike_history: List[Tuple[float, float]] = []  # (time, amplitude)
        self._membrane_history: List[Tuple[float, float]] = []
        
        # Input management
        self._external_inputs: List[Tuple[float, float]] = []  # (time, amplitude)
    
    def getId(self) -> NeuronId:
        return self._id
    
    def getType(self) -> NeuronType:
        return self._type
    
    def setType(self, neuron_type: NeuronType):
        self._type = neuron_type
    
    def getMembranePotential(self) -> MembranePotential:
        return self._membrane_potential
    
    def setMembranePotential(self, potential: MembranePotential):
        self._membrane_potential = potential
    
    def addToMembranePotential(self, delta: float):
        """Add delta to membrane potential."""
        self._membrane_potential.value += delta
    
    def getThreshold(self) -> MembranePotential:
        return self._threshold
    
    def setThreshold(self, threshold: MembranePotential):
        self._threshold = threshold
    
    def getRestingPotential(self) -> MembranePotential:
        return self._resting_potential
    
    def getResetPotential(self) -> MembranePotential:
        return self._reset_potential
    
    def getTotalCurrent(self) -> float:
        return self._current
    
    def injectCurrent(self, current: float):
        """Inject current into neuron."""
        self._current += current
    
    def clearTotalCurrent(self):
        """Clear injected current."""
        self._current = 0.0
    
    def isFiring(self) -> bool:
        return self._firing_state == FiringState.ACTIVE
    
    def isRefractory(self) -> bool:
        return self._refractory_period > 0
    
    def setFiringState(self, state: FiringState):
        self._firing_state = state
    
    def setRefractoryPeriod(self, period: float):
        self._refractory_period = period
    
    def decrementRefractory(self, dt: float = 0.001):
        """Decrement refractory period by time."""
        self._refractory_period = max(0.0, self._refractory_period - dt)
    
    def recordSpike(self, time: float, amplitude: float = 1.0):
        """Record spike event."""
        self._spike_history.append((time, amplitude))
        self._firing_state = FiringState.ACTIVE
        self._spike_count += 1
    
    def clearSpikeHistory(self):
        """Clear spike history."""
        self._spike_history.clear()
    
    def getSpikeHistory(self) -> List[Tuple[float, float]]:
        return self._spike_history.copy()
    
    def getSpikeCount(self) -> int:
        return self._spike_count
    
    def getMembranePotentialHistory(self) -> List[Tuple[float, float]]:
        return self._membrane_history.copy()
    
    def updateMembranePotential(self, dt: float, injected_current: float = 0.0):
        """Update membrane potential using LIF dynamics."""
        if self.isRefractory():
            self._membrane_potential.value = self._reset_potential.value
            self.decrementRefractory(dt)
            return
        
        # LIF equation: dV/dt = -(V - V_rest)/tau + I/C
        v_rest = self._resting_potential.value
        tau = self._membrane_time_constant
        
        dV = (-(self._membrane_potential.value - v_rest) / tau + injected_current) * dt
        
        # Update membrane potential
        new_v = self._membrane_potential.value + dV
        
        # Check for spike threshold
        if new_v >= self._threshold.value:
            # Spike occurred
            spike_time = dt  # Simplified - should use actual simulation time
            self.recordSpike(spike_time)
            self._membrane_potential.value = self._reset_potential.value
            self.setRefractoryPeriod(self._refractory_time_constant)
            self._firing_state = FiringState.REFRACTORY
        else:
            self._membrane_potential.value = new_v
        
        # Record history
        self._membrane_history.append((dt, self._membrane_potential.value))
    
    def addExternalInput(self, time: float, amplitude: float):
        """Add external input (sensory, etc.)."""
        self._external_inputs.append((time, amplitude))
        self.injectCurrent(amplitude)
    
    def processExternalInputs(self):
        """Process all external inputs."""
        total_current = sum(amplitude for _, amplitude in self._external_inputs)
        self.injectCurrent(total_current)
        self._external_inputs.clear()
    
    def reset(self):
        """Reset neuron to initial state."""
        self._membrane_potential.value = self._resting_potential.value
        self._current = 0.0
        self._refractory_period = 0.0
        self._firing_state = FiringState.RESTING
        self._spike_history.clear()
        self._external_inputs.clear()
    
    def __str__(self):
        return f"Neuron({self._id.value}, type={self._type.name}, V={self._membrane_potential.value:.1f}mV)"

class NeuralRegion:
    """Advanced neural region with multiple populations and connectivity."""
    
    def __init__(self, region_id: RegionId, name: str = ""):
        self._id = region_id
        self._name = name
        self._neurons: Dict[NeuronId, Neuron] = {}
        self._synapses: List[Synapse] = []
        self._populations: Dict[PopulationId, List[Neuron]] = {}
        self._event_system = EventSystem()
        self._next_neuron_id = 1
        self._next_population_id = 1
        self._next_synapse_id = 1
    
    def getId(self) -> RegionId:
        return self._id
    
    def getName(self) -> str:
        return self._name
    
    def setName(self, name: str):
        self._name = name
    
    def createNeuron(self, neuron_type: NeuronType = NeuronType.INTERNAL, 
                    membrane_potential: float = -70.0, threshold: float = -50.0) -> Neuron:
        """Create a new neuron in this region."""
        neuron_id = NeuronId(self._next_neuron_id)
        self._next_neuron_id += 1
        
        neuron = Neuron(neuron_id)
        neuron.setType(neuron_type)
        neuron.setMembranePotential(MembranePotential(membrane_potential))
        neuron.setThreshold(MembranePotential(threshold))
        
        self._neurons[neuron_id] = neuron
        self._event_system.emit("neuron_created", 0.0, {"region": self._name, "neuron": neuron})
        
        return neuron
    
    def createPopulation(self, population_type: str) -> PopulationId:
        """Create a new neural population."""
        population_id = PopulationId(self._next_population_id)
        self._next_population_id += 1
        self._populations[population_id] = []
        
        self._event_system.emit("population_created", 0.0, 
                                {"region": self._name, "population": population_type})
        
        return population_id
    
    def addNeuronToPopulation(self, neuron_id: NeuronId, population_id: PopulationId):
        """Add neuron to population."""
        neuron = self._neurons.get(neuron_id)
        if neuron:
            if population_id not in self._populations:
                self._populations[population_id] = []
            self._populations[population_id].append(neuron)
    
    def getNeuron(self, neuron_id: NeuronId) -> Optional[Neuron]:
        return self._neurons.get(neuron_id)
    
    def getNeurons(self) -> List[Neuron]:
        return list(self._neurons.values())
    
    def getNeuronCount(self) -> int:
        return len(self._neurons)
    
    def getAllNeurons(self) -> List[Neuron]:
        return self.getNeurons()
    
    def createSynapse(self, source_neuron: Neuron, dest_neuron: Neuron, 
                     weight: float = 0.5, synapse_type: SynapseType = SynapseType.EXCITATORY,
                     delay: Delay = Delay(1)) -> Synapse:
        """Create a synaptic connection between neurons."""
        synapse_id = SynapseId(self._next_synapse_id)
        self._next_synapse_id += 1
        
        synapse = Synapse(synapse_id, source_neuron.getId(), dest_neuron.getId())
        synapse.setType(synapse_type)
        synapse.setWeight(weight)
        synapse.setDelay(delay)
        
        self._synapses.append(synapse)
        
        self._event_system.emit("synapse_created", 0.0, {
            "region": self._name,
            "source": source_neuron.getId().value,
            "dest": dest_neuron.getId().value,
            "weight": weight,
            "type": synapse_type.name
        })
        
        return synapse
    
    def getSynapses(self) -> List[Synapse]:
        return self._synapses.copy()
    
    def getSynapseCount(self) -> int:
        return len(self._synapses)
    
    def getPopulationNeurons(self, population_id: PopulationId) -> List[Neuron]:
        return self._populations.get(population_id, []).copy()
    
    def getPopulations(self) -> Dict[PopulationId, List[Neuron]]:
        return self._populations.copy()
    
    def reset(self):
        """Reset all neurons and synapses in this region."""
        for neuron in self._neurons.values():
            neuron.reset()
        self._synapses.clear()
    
    def step(self, dt: float, current_time: float):
        """Process simulation step for this region."""
        # Process neuron updates
        for neuron in self._neurons.values():
            neuron.processExternalInputs()
            neuron.updateMembranePotential(dt)
        
        # Emit region step event
        self._event_system.emit("region_step", current_time, {
            "region": self._name,
            "neuron_count": self.getNeuronCount(),
            "synapse_count": self.getSynapseCount()
        })

class AgentBrain:
    """Agent brain interface connecting NLM to world."""
    
    def __init__(self, brain: 'Brain'):
        self._brain = brain
        self._world_interface = None
        self._sensory_percept = None
        self._reward_modulation_enabled = True
        self._structural_plasticity_enabled = True
        self._development_enabled = True
        self._curiosity_enabled = True
        
        # Neuromodulation levels
        self._curiosity_level = 0.0
        self._novelty_level = 0.0
        self._dopamine_level = 0.0
        self._prediction_error = 0.0
    
    def initialize(self, world):
        """Initialize agent brain with world interface."""
        self._world_interface = world
        self._sensory_percept = world.getSensoryPercept()
    
    def getSensoryInputSize(self) -> int:
        """Get expected sensory input size."""
        if self._sensory_percept:
            vision = self._sensory_percept.getVision()
            if vision:
                return vision.getWidth() * vision.getHeight() * vision.getChannels()
        return 0
    
    def getMotorOutputSize(self) -> int:
        """Get expected motor output size."""
        return len(list(ActionType))
    
    def processSensoryInput(self, percept):
        """Process sensory input and inject into brain."""
        self._sensory_percept = percept
        
        # Extract sensory data and inject into sensory neurons
        if percept:
            # Inject vision data
            vision = percept.getVision()
            if vision:
                for neuron in self._getSensoryNeurons():
                    vision_data = vision.getData()
                    if vision_data:
                        neuron.addExternalInput(0.0, sum(vision_data) / len(vision_data))
            
            # Inject internal signals
            internal = percept.getInternal()
            if internal:
                for i, neuron in enumerate(self._getInternalNeurons()):
                    if i < len(internal):
                        neuron.addExternalInput(0.0, internal[i])
    
    def _getSensoryNeurons(self) -> List[Neuron]:
        """Get all sensory neurons."""
        sensory_neurons = []
        for region in self._brain.getRegions():
            for neuron in region.getNeurons():
                if neuron.getType() == NeuronType.SENSORY:
                    sensory_neurons.append(neuron)
        return sensory_neurons
    
    def _getInternalNeurons(self) -> List[Neuron]:
        """Get all internal neurons."""
        internal_neurons = []
        for region in self._brain.getRegions():
            for neuron in region.getNeurons():
                if neuron.getType() == NeuronType.INTERNAL:
                    internal_neurons.append(neuron)
        return internal_neurons
    
    def decodeMotorCommand(self):
        """Decode brain motor activity into motor command."""
        # Get motor neuron activity
        motor_neurons = self._getMotorNeurons()
        
        if not motor_neurons:
            return ActionType.WAIT
        
        # Find most active motor neuron
        most_active_neuron = max(motor_neurons, key=lambda n: abs(n.getTotalCurrent()))
        
        # Map neural activity to motor command
        motor_current = most_active_neuron.getTotalCurrent()
        
        # Determine action based on activity patterns
        if motor_current > 10.0:
            return ActionType.MOVE_FORWARD
        elif motor_current > 5.0:
            return ActionType.TURN_LEFT
        elif motor_current > 0.0:
            return ActionType.TURN_RIGHT
        else:
            return ActionType.WAIT
    
    def _getMotorNeurons(self) -> List[Neuron]:
        """Get all motor neurons."""
        motor_neurons = []
        for region in self._brain.getRegions():
            for neuron in region.getNeurons():
                if neuron.getType() == NeuronType.MOTOR:
                    motor_neurons.append(neuron)
        return motor_neurons
    
    def applyRewardModulation(self, reward: float, predicted_reward: float):
        """Apply reward-based neuromodulation."""
        if not self._reward_modulation_enabled:
            return
        
        # Calculate prediction error
        prediction_error = reward - predicted_reward
        self._prediction_error = prediction_error
        
        # Update neuromodulation levels
        if reward > 0:
            # Positive reward increases dopamine
            self._dopamine_level = min(1.0, self._dopamine_level + 0.1 * reward)
        
        if prediction_error > 0.1:
            # High prediction error increases curiosity
            self._curiosity_level = min(1.0, self._curiosity_level + 0.05 * prediction_error)
        
        # Emit neuromodulation event
        if hasattr(self._brain, 'getEventSystem'):
            self._brain.getEventSystem().emit("neuromodulation", 0.0, {
                "reward": reward,
                "predicted": predicted_reward,
                "error": prediction_error,
                "dopamine": self._dopamine_level,
                "curiosity": self._curiosity_level
            })
    
    def updateDevelopment(self, dt: float):
        """Update development system."""
        if not self._development_enabled:
            return
        
        # Age the brain
        current_stage = self._brain.getDevelopmentalStage()
        
        # Simple developmental progression
        if current_stage == DevelopmentalStage.INITIAL:
            new_stage = DevelopmentalStage.CRITICAL_PERIOD
        elif current_stage == DevelopmentalStage.CRITICAL_PERIOD:
            new_stage = DevelopmentalStage.MATURATION
        elif current_stage == DevelopmentalStage.MATURATION:
            new_stage = DevelopmentalStage.ADULT
        else:
            # Adult aging
            new_stage = DevelopmentalStage.AGING
        
        self._brain.setDevelopmentalStage(new_stage)
        
        # Update neural properties based on developmental stage
        self._updateNeuralProperties(new_stage)
    
    def _updateNeuralProperties(self, stage: DevelopmentalStage):
        """Update neural properties based on developmental stage."""
        for region in self._brain.getRegions():
            for neuron in region.getNeurons():
                # Modify neuron properties based on stage
                if stage == DevelopmentalStage.CRITICAL_PERIOD:
                    # Critical period: high plasticity
                    neuron.setThreshold(MembranePotential(-45.0))
                elif stage == DevelopmentalStage.ADULT:
                    # Adult: stable properties
                    neuron.setThreshold(MembranePotential(-50.0))
                elif stage == DevelopmentalStage.AGING:
                    # Aging: reduced excitability
                    neuron.setThreshold(MembranePotential(-48.0))
    
    def getDevelopmentalStage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return self._brain.getDevelopmentalStage()
    
    def getNeuromodulationLevel(self) -> float:
        """Get neuromodulation level."""
        return self._dopamine_level
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level."""
        return self._curiosity_level
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level."""
        # Simplified novelty calculation
        return min(1.0, self._curiosity_level * 0.8 + self._dopamine_level * 0.2)
    
    def getPredictionError(self) -> float:
        """Get prediction error."""
        return self._prediction_error
    
    def reset(self):
        """Reset agent brain for new episode."""
        self._curiosity_level = 0.0
        self._novelty_level = 0.0
        self._dopamine_level = 0.0
        self._prediction_error = 0.0
        self._brain.reset()
    
    def getBrain(self) -> 'Brain':
        """Get the underlying brain."""
        return self._brain
    
    def enableRewardModulation(self, enable: bool):
        """Enable/disable reward modulation."""
        self._reward_modulation_enabled = enable
    
    def enableStructuralPlasticity(self, enable: bool):
        """Enable/disable structural plasticity."""
        self._structural_plasticity_enabled = enable
    
    def enableDevelopment(self, enable: bool):
        """Enable/disable development."""
        self._development_enabled = enable
    
    def enableCuriosity(self, enable: bool):
        """Enable/disable curiosity."""
        self._curiosity_enabled = enable
    
    def isRewardModulationEnabled(self) -> bool:
        """Check if reward modulation is enabled."""
        return self._reward_modulation_enabled
    
    def isStructuralPlasticityEnabled(self) -> bool:
        """Check if structural plasticity is enabled."""
        return self._structural_plasticity_enabled
    
    def isDevelopmentEnabled(self) -> bool:
        """Check if development is enabled."""
        return self._development_enabled
    
    def isCuriosityEnabled(self) -> bool:
        """Check if curiosity is enabled."""
        return self._curiosity_enabled

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
        self._objects: List[Any] = []
        self._agent_body = self._createDefaultAgentBody()
        self._event_system = EventSystem()
    
    def _createDefaultAgentBody(self):
        """Create default agent body."""
        class AgentBody:
            def __init__(self):
                self.x = 0.0
                self.y = 0.0
                self.orientation = 0.0
                self.velocity_x = 0.0
                self.velocity_y = 0.0
                self.angular_velocity = 0.0
                self.energy = 100.0
                self.health = 1.0
                self.age = 0
                self.is_moving = False
                self.is_turning = False
                self.last_action_time = 0.0
            
            def reset(self):
                self.x = 0.0
                self.y = 0.0
                self.orientation = 0.0
                self.velocity_x = 0.0
                self.velocity_y = 0.0
                self.angular_velocity = 0.0
                self.energy = 100.0
                self.health = 1.0
                self.age = 0
                self.is_moving = False
                self.is_turning = False
                self.last_action_time = 0.0
        
        return AgentBody()
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int):
        """Configure world dimensions."""
        self._width = width
        self._height = height
        self._vision_width = vision_width
        self._vision_height = vision_height
        
        # Emit configuration event
        self._event_system.emit("world_configured", 0.0, {
            "width": width, "height": height,
            "vision_width": vision_width, "vision_height": vision_height
        })
    
    def reset(self):
        """Reset world to initial state."""
        self._agent_x = 0.0
        self._agent_y = 0.0
        self._agent_orientation = 0.0
        self._simulation_time = 0.0
        self._agent_body.reset()
        self._objects.clear()
        
        # Emit reset event
        self._event_system.emit("world_reset", 0.0, {})
    
    def setAgentStart(self, x: float, y: float):
        """Set agent starting position."""
        self._agent_x = x
        self._agent_y = y
        self._agent_body.x = x
        self._agent_body.y = y
        
        # Emit agent start event
        self._event_system.emit("agent_start", 0.0, {"x": x, "y": y})
    
    def update(self, dt: float):
        """Update world state."""
        self._simulation_time += dt
        
        # Update agent physics
        self._updateAgentPhysics(dt)
        
        # Emit update event
        self._event_system.emit("world_updated", self._simulation_time, {
            "time": self._simulation_time,
            "agent_x": self._agent_x,
            "agent_y": self._agent_y,
            "agent_orientation": self._agent_orientation
        })
    
    def _updateAgentPhysics(self, dt: float):
        """Update agent physics (simplified)."""
        # Simple physics integration
        self._agent_x += self._agent_body.velocity_x * dt
        self._agent_y += self._agent_body.velocity_y * dt
        self._agent_orientation += self._agent_body.angular_velocity * dt
        
        # Update agent body
        self._agent_body.x = self._agent_x
        self._agent_body.y = self._agent_y
        self._agent_body.orientation = self._agent_orientation
        
        # Energy decay
        self._agent_body.energy *= (1.0 - self._energy_decay_rate * dt)
        self._agent_body.health = max(0.0, self._agent_body.health - 0.01 * dt)
        
        # Age
        self._agent_body.age += int(dt)
        
        # Clamp position to world bounds
        self._agent_x = max(0.0, min(self._agent_x, self._width - 0.1))
        self._agent_y = max(0.0, min(self._agent_y, self._height - 0.1))
    
    def getSensoryPercept(self):
        """Get sensory percept from world."""
        class SensoryPercept:
            def __init__(self, width: int, height: int):
                self._vision_width = width
                self._vision_height = height
                self._vision_data = []
                self._internal_signals = [self._agent_body.energy, self._agent_body.health]
                self._timestamp = 0.0
            
            def getVision(self):
                # Create simple vision data (grayscale)
                class Vision:
                    def __init__(self, width: int, height: int):
                        self._width = width
                        self._height = height
                        self._channels = 1
                        self._data = [0.5] * (width * height * self._channels)
                    
                    def getWidth(self) -> int:
                        return self._width
                    
                    def getHeight(self) -> int:
                        return self._height
                    
                    def getChannels(self) -> int:
                        return self._channels
                    
                    def getData(self):
                        return self._data
                    
                    def setData(self, data):
                        self._data = data
                
                return Vision(self._vision_width, self._vision_height)
            
            def getInternal(self) -> List[float]:
                return self._internal_signals.copy()
            
            def getTimestamp(self) -> float:
                return self._timestamp
            
            def setTimestamp(self, timestamp: float):
                self._timestamp = timestamp
        
        return SensoryPercept(self._vision_width, self._vision_height)
    
    def getAgentBody(self):
        """Get agent body state."""
        return self._agent_body
    
    def addObject(self, obj):
        """Add object to world."""
        self._objects.append(obj)
    
    def removeObject(self, x: float, y: float):
        """Remove object from world."""
        self._objects = [obj for obj in self._objects 
                        if not (hasattr(obj, 'x') and hasattr(obj, 'y') and 
                               abs(obj.x - x) < 0.1 and abs(obj.y - y) < 0.1)]
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if position is valid."""
        # Check bounds
        if not (0.0 <= x < self._width and 0.0 <= y < self._height):
            return False
        
        # Check collision with objects
        for obj in self._objects:
            if hasattr(obj, 'x') and hasattr(obj, 'y') and hasattr(obj, 'radius'):
                distance = ((obj.x - x) ** 2 + (obj.y - y) ** 2) ** 0.5
                if distance < obj.radius + 0.1:
                    return False
        
        return True
    
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

class WorldObject:
    """World object representation."""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, obj_type: int = 0, 
                 value: float = 0.0, radius: float = 0.5):
        self.x = x
        self.y = y
        self.type = obj_type
        self.value = value
        self.radius = radius
        self.active = True
    
    def __str__(self):
        return f"WorldObject(x={self.x}, y={self.y}, type={self.type}, value={self.value})"

class Action:
    """Action representation for motor output."""
    
    def __init__(self, action_type: ActionType = ActionType.WAIT, 
                 parameters: List[float] = None):
        self._type = action_type
        self._parameters = parameters or []
    
    def getType(self) -> ActionType:
        return self._type
    
    def setType(self, action_type: ActionType):
        self._type = action_type
    
    def getParameters(self) -> List[float]:
        return self._parameters.copy()
    
    def setParameters(self, parameters: List[float]):
        self._parameters = parameters.copy()
    
    def getName(self) -> str:
        return f"Action_{self._type.name}"
    
    def clone(self):
        """Create a copy of this action."""
        return Action(self._type, self._parameters.copy())
    
    def __str__(self):
        return f"Action(type={self._type.name}, params={self._parameters})"

# Factory functions
def createDefaultConfig():
    """Create a default NLM configuration."""
    config = type('Config', (), {})()
    
    # Set default values
    config.neuron_count = 1000
    config.region_count = 1
    config.connection_probability = 0.1
    config.stdp_ltp_weight = 0.01
    config.stdp_ltd_weight = 0.012
    config.synaptogenesis_rate = 0.0001
    config.pruning_rate = 0.00001
    
    return config

def createBrain(config=None):
    """Create a new brain with configuration."""
    return Brain(config)

def createAgentBrain(brain):
    """Create a new agent brain interface."""
    return AgentBrain(brain)

def createSimpleWorld():
    """Create a new simple world."""
    return SimpleWorld()

def createAction(action_type: ActionType = ActionType.WAIT, 
                parameters: List[float] = None):
    """Create a new action."""
    if parameters is None:
        parameters = []
    return Action(action_type, parameters)

def createWorldObject(x: float, y: float, obj_type: int, 
                     value: float = 0.0, radius: float = 0.5):
    """Create a new world object."""
    return WorldObject(x, y, obj_type, value, radius)

# Main Brain class (alias for backward compatibility)
Brain = Brain  # Use the actual Brain class

# Export all public components
__all__ = [
    # Core classes
    'Brain', 'AgentBrain', 'NeuralRegion', 'Neuron', 'Synapse',
    'SimpleWorld', 'WorldObject', 'Action', 'Config',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createAgentBrain',
    'createSimpleWorld', 'createAction', 'createWorldObject',
    
    # Type definitions
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'FiringState',
    'DevelopmentalStage', 'ActionType', 'WorldObjectType',
    'SimulationStep', 'Timestamp', 'MembranePotential', 'Delay',
    'PlasticityFlags',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Event system
    'Event', 'EventSystem',
    
    # Utility functions
    'getVersionInfo', 'validateConfig',
]

# Version and metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Utility functions
def getVersionInfo():
    """Get detailed version information for NLM."""
    import platform
    import sys
    
    return {
        'version': __version__,
        'author': __author__,
        'description': __description__,
        'python_version': f"{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}",
        'cpp_standard': 'C++20',
        'platform': platform.system(),
        'architecture': platform.machine()
    }

def validateConfig(config):
    """Validate a configuration object."""
    if config is None:
        raise ValueError("Config cannot be None")
    
    # Check that required configuration keys exist
    required_keys = [
        "neuron_count",
        "connection_probability",
        "stdp_ltp_weight",
        "stdp_ltd_weight",
        "synaptogenesis_rate",
        "pruning_rate"
    ]
    
    for key in required_keys:
        if not hasattr(config, key):
            raise ValueError(f"Missing required configuration key: {key}")
    
    return True