"""
Enhanced Python interface for NLM dynamics and spike system
"""

from typing import List, Dict, Any, Optional, Tuple
from .exceptions import NLMError

class SpikeEvent:
    """Spike event information"""
    
    def __init__(self, neuron_id: int, time: float, step: int):
        self.neuron_id = neuron_id
        self.time = time
        self.step = step
        
    def get_neuron_id(self) -> int:
        """Get neuron ID"""
        return self.neuron_id
        
    def get_time(self) -> float:
        """Get spike time"""
        return self.time
        
    def get_step(self) -> int:
        """Get simulation step"""
        return self.step
        
    def __repr__(self):
        return f"<SpikeEvent: neuron={self.neuron_id}, time={self.time:.3f}, step={self.step}>"

class DelayedSpikeEvent:
    """Delayed spike event for synaptic transmission"""
    
    def __init__(self, source_neuron: int, destination_neuron: int, 
                 synapse_id: int, weight: float, synapse_type: str,
                 time: float, delivery_time: float, step: int, delivery_step: int):
        self.source_neuron = source_neuron
        self.destination_neuron = destination_neuron
        self.synapse_id = synapse_id
        self.weight = weight
        self.synapse_type = synapse_type
        self.time = time
        self.delivery_time = delivery_time
        self.step = step
        self.delivery_step = delivery_step
        
    def get_source_neuron(self) -> int:
        """Get source neuron ID"""
        return self.source_neuron
        
    def get_destination_neuron(self) -> int:
        """Get destination neuron ID"""
        return self.destination_neuron
        
    def get_synapse_id(self) -> int:
        """Get synapse ID"""
        return self.synapse_id
        
    def get_weight(self) -> float:
        """Get synapse weight"""
        return self.weight
        
    def get_synapse_type(self) -> str:
        """Get synapse type"""
        return self.synapse_type
        
    def get_time(self) -> float:
        """Get spike time"""
        return self.time
        
    def get_delivery_time(self) -> float:
        """Get delivery time"""
        return self.delivery_time
        
    def get_step(self) -> int:
        """Get simulation step"""
        return self.step
        
    def get_delivery_step(self) -> int:
        """Get delivery step"""
        return self.delivery_step
        
    def __repr__(self):
        return f"<DelayedSpikeEvent: {self.source_neuron} -> {self.destination_neuron} (syn={self.synapse_id}) weight={self.weight:.3f}>"

class SpikeSystem:
    """Event-driven spike system for NLM"""
    
    def __init__(self):
        self.spike_handlers = []
        self.delayed_handlers = []
        self.pending_spikes = []
        self.pending_delayed_spikes = []
        self.spike_count = 0
        self.delayed_spike_count = 0
        
    def register_handler(self, handler) -> None:
        """Register handler for immediate spike events"""
        self.spike_handlers.append(handler)
        
    def register_delayed_handler(self, handler) -> None:
        """Register handler for delayed spike events"""
        self.delayed_handlers.append(handler)
        
    def queue_spike(self, event: SpikeEvent) -> None:
        """Queue immediate spike event"""
        self.pending_spikes.append(event)
        self.spike_count += 1
        
    def queue_delayed_spike(self, event: DelayedSpikeEvent) -> None:
        """Queue delayed spike event"""
        self.pending_delayed_spikes.append(event)
        self.delayed_spike_count += 1
        
    def process_spikes(self, current_step: int) -> None:
        """Process immediate spike events"""
        for handler in self.spike_handlers:
            for spike in self.pending_spikes:
                if spike.step == current_step:
                    handler(spike)
                    
        self.pending_spikes.clear()
        
    def process_delayed_spikes(self, current_step: int, current_time: float) -> None:
        """Process delayed spike events"""
        for handler in self.delayed_handlers:
            for spike in self.pending_delayed_spikes:
                if spike.delivery_step == current_step:
                    handler(spike)
                    
        self.pending_delayed_spikes.clear()
        
    def get_pending_spike_count(self) -> int:
        """Get count of pending immediate spikes"""
        return len(self.pending_spikes)
        
    def get_pending_delayed_count(self) -> int:
        """Get count of pending delayed spikes"""
        return len(self.pending_delayed_spikes)
        
    def get_total_spike_count(self) -> int:
        """Get total spike count"""
        return self.spike_count
        
    def get_total_delayed_spike_count(self) -> int:
        """Get total delayed spike count"""
        return self.delayed_spike_count
        
    def reset(self) -> None:
        """Reset spike system"""
        self.spike_handlers.clear()
        self.delayed_handlers.clear()
        self.pending_spikes.clear()
        self.pending_delayed_spikes.clear()
        self.spike_count = 0
        self.delayed_spike_count = 0
        
    def __repr__(self):
        return f"<SpikeSystem: {self.spike_count} total spikes, {self.delayed_spike_count} delayed, {len(self.spike_handlers)} handlers>"

class NeuralDynamics:
    """Neural dynamics calculator for LIF neurons"""
    
    @staticmethod
    def calculate_lif_step(current_potential: float, current_input: float, 
                          timestep: float, v_rest: float, v_thresh: float, 
                          v_reset: float, tau_m: float) -> Dict[str, Any]:
        """Calculate one step of LIF neuron dynamics"""
        # Leaky Integrate-and-Fire dynamics
        dV = (current_input - (current_potential - v_rest) / tau_m) * timestep
        new_potential = current_potential + dV
        
        # Check for spike
        spiked = new_potential >= v_thresh
        if spiked:
            new_potential = v_reset
            
        return {
            'new_potential': new_potential,
            'spiked': spiked,
            'delta_v': dV
        }
        
    @staticmethod
    def calculate_refractory_step(current_potential: float, 
                                 refractory_remaining: float, timestep: float,
                                 v_reset: float) -> Dict[str, Any]:
        """Calculate dynamics during refractory period"""
        if refractory_remaining > 0:
            # Stay at reset potential during refractory period
            return {
                'new_potential': v_reset,
                'spiked': False,
                'delta_v': 0.0,
                'refractory_remaining': refractory_remaining - timestep
            }
        else:
            return {
                'new_potential': current_potential,
                'spiked': False,
                'delta_v': 0.0,
                'refractory_remaining': 0.0
            }
