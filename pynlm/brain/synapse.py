"""Basic synapse implementation."""
from typing import Dict

from typing import List, Any
from ..neuron import NeuronId, SynapseId

class Synapse:
    """Basic synapse implementation."""
    
    def __init__(self, synapse_id: SynapseId, source_neuron: NeuronId, dest_neuron: NeuronId):
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
        self._plasticity_flags = {'hebbian': False, 'stdp': False, 'reward_modulated': False}
    
    def getId(self) -> SynapseId:
        return self._id
    
    def getSourceNeuron(self) -> NeuronId:
        return self._source_neuron
    
    def getDestinationNeuron(self) -> NeuronId:
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
        return self._plasticity_flags
    
    def enablePlasticity(self, hebbian: bool, stdp: bool, reward_modulated: bool):
        self._plasticity_flags = {
            'hebbian': hebbian,
            'stdp': stdp,
            'reward_modulated': reward_modulated
        }
    
    def reset(self):
        self._weight = 0.0
        self._pre_spike_history.clear()
        self._post_spike_history.clear()
        self._eligibility_trace = 0.0
        self._efficacy = 1.0

__all__ = ['Synapse']