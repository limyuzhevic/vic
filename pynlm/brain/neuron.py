"""Basic neuron implementation."""

from typing import List, Dict, Any
from ..neuron import NeuronId

class Neuron:
    """Basic neuron implementation."""
    
    def __init__(self, neuron_id: NeuronId):
        self._id = neuron_id
        self._type = 5  # Internal
        self._membrane_potential = -70.0
        self._threshold = -50.0
        self._current = 0.0
        self._refractory_period = 0
        self._firing_state = 0  # Resting
        self._spike_history: List[float] = []
    
    def getId(self) -> NeuronId:
        return self._id
    
    def getType(self) -> int:
        return self._type
    
    def setType(self, neuron_type: int):
        self._type = neuron_type
    
    def getMembranePotential(self) -> float:
        return self._membrane_potential
    
    def setMembranePotential(self, potential: float):
        self._membrane_potential = potential
    
    def addToMembranePotential(self, value: float):
        self._membrane_potential += value
    
    def getThreshold(self) -> float:
        return self._threshold
    
    def setThreshold(self, threshold: float):
        self._threshold = threshold
    
    def getTotalCurrent(self) -> float:
        return self._current
    
    def injectCurrent(self, current: float):
        self._current += current
    
    def clearTotalCurrent(self):
        self._current = 0.0
    
    def isFiring(self) -> bool:
        return self._firing_state == 1
    
    def isRefractory(self) -> bool:
        return self._refractory_period > 0
    
    def setFiringState(self, state: int):
        self._firing_state = state
    
    def setRefractoryPeriod(self, period: int):
        self._refractory_period = period
    
    def decrementRefractory(self):
        if self._refractory_period > 0:
            self._refractory_period -= 1
    
    def recordSpike(self, time: float):
        self._spike_history.append(time)
    
    def clearSpikeHistory(self):
        self._spike_history.clear()
    
    def getSpikeHistory(self) -> List[float]:
        return self._spike_history
    
    def reset(self):
        self._membrane_potential = -70.0
        self._current = 0.0
        self._refractory_period = 0
        self._firing_state = 0
        self._spike_history.clear()

class Synapse:
    """Basic synapse implementation."""
    
    def __init__(self, synapse_id: Any, source_neuron: Any, dest_neuron: Any):
        self._id = synapse_id
        self._source_neuron = source_neuron
        self._dest_neuron = dest_neuron
        self._weight = 0.0
        self._type = 0  # Excitatory
        self._delay = 1
        self._pre_spike_history: List[float] = []
        self._post_spike_history: List[float] = []
        self._eligibility_trace = 0.0
        self._efficacy = 1.0
    
    def getId(self):
        return self._id
    
    def getSourceNeuron(self):
        return self._source_neuron
    
    def getDestinationNeuron(self):
        return self._dest_neuron
    
    def getWeight(self) -> float:
        return self._weight
    
    def setWeight(self, weight: float):
        # Clamp weight between -1.0 and 1.0
        self._weight = max(-1.0, min(1.0, weight))
    
    def addToWeight(self, value: float):
        self.setWeight(self._weight + value)
    
    def getType(self) -> int:
        return self._type
    
    def setType(self, synapse_type: int):
        self._type = synapse_type
    
    def isExcitatory(self) -> bool:
        return self._type == 0
    
    def isInhibitory(self) -> bool:
        return self._type == 1
    
    def getDelay(self) -> int:
        return self._delay
    
    def setDelay(self, delay: int):
        self._delay = max(1, delay)
    
    def getPreSpikeHistory(self) -> List[float]:
        return self._pre_spike_history
    
    def getPostSpikeHistory(self) -> List[float]:
        return self._post_spike_history
    
    def recordPreSpike(self, time: float):
        self._pre_spike_history.append(time)
    
    def recordPostSpike(self, time: float):
        self._post_spike_history.append(time)
    
    def clearHistory(self):
        self._pre_spike_history.clear()
        self._post_spike_history.clear()
    
    def getEligibilityTrace(self) -> float:
        return self._eligibility_trace
    
    def setEligibilityTrace(self, trace: float):
        self._eligibility_trace = trace
    
    def decayEligibilityTrace(self, amount: float):
        self._eligibility_trace = max(0.0, self._eligibility_trace - amount)
    
    def getEfficacy(self) -> float:
        return self._efficacy
    
    def setEfficacy(self, efficacy: float):
        # Clamp efficacy
        self._efficacy = max(0.0, min(2.0, efficacy))
    
    def getPlasticityFlags(self) -> Dict:
        return {
            'hebbian': True,
            'stdp': True,
            'reward_modulated': False
        }
    
    def enablePlasticity(self, hebbian: bool, stdp: bool, reward_modulated: bool):
        pass  # Placeholder
    
    def reset(self):
        self._weight = 0.0
        self._pre_spike_history.clear()
        self._post_spike_history.clear()
        self._eligibility_trace = 0.0
        self._efficacy = 1.0

__all__ = ['Neuron', 'Synapse']
