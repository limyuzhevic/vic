'''Brain class for pynlm neural simulation'''

from __future__ import annotations

from typing import Dict, List, Optional, Union, Any, Callable
from dataclasses import dataclass, field
import time

from .core.types import (
    NeuronId, SynapseId, RegionId, PopulationId, SimulationStep,
    Timestamp, SynapticWeight, MembranePotential, FiringRate, SpikeEvent
)
from .core.enums import (
    NeuronType, SynapseType, DevelopmentalStage, FiringState
)
from .config import Config


class NeuralRegion:
    """Represents a neural region in the brain.
    
    Neural regions are collections of neurons with specific properties
    and connectivity patterns. They represent functional areas of the brain.
    """
    
    def __init__(self, name: str = ""):
        """Initialize a NeuralRegion.
        
        Args:
            name: Name of the neural region. Defaults to empty string.
        """
        self.name: str = name
        self.region_id: RegionId = RegionId()
        self.neurons: Dict[NeuronId, Dict[str, Any]] = {}
        self.synapses: Dict[SynapseId, Dict[str, Any]] = {}
        self.population_counts: Dict[PopulationId, int] = {}
        self.plasticity_flags: Dict[str, bool] = {
            'hebbian': False,
            'stdp': False,
            'reward_modulated': False,
            'structural': False,
            'eligible': False,
        }
        self.developmental_stage: DevelopmentalStage = DevelopmentalStage.ADULT
    
    def add_neuron(self, neuron_id: NeuronId, neuron_type: NeuronType = NeuronType.INTERNAL):
        """Add a neuron to the region.
        
        Args:
            neuron_id: ID of the neuron to add.
            neuron_type: Type of the neuron. Defaults to INTERNAL.
        """
        self.neurons[neuron_id] = {
            'type': neuron_type,
            'state': FiringState.RESTING,
            'potential': 0.0,
            'firing_rate': 0.0,
            'last_spike': 0.0,
            'refractory_until': 0.0,
            'synapse_targets': [],
            'synapse_sources': [],
            'custom_properties': {},
        }
    
    def remove_neuron(self, neuron_id: NeuronId):
        """Remove a neuron from the region.
        
        Args:
            neuron_id: ID of the neuron to remove.
        """
        if neuron_id in self.neurons:
            # Remove all synapses to/from this neuron
            for syn_id, synapse in list(self.synapses.items()):
                if synapse['source'] == neuron_id or synapse['target'] == neuron_id:
                    del self.synapses[syn_id]
            
            del self.neurons[neuron_id]
    
    def add_synapse(self, synapse_id: SynapseId, source: NeuronId, target: NeuronId, 
                   weight: float = 1.0, synapse_type: SynapseType = SynapseType.EXCITATORY):
        """Add a synapse to the region.
        
        Args:
            synapse_id: ID of the synapse to add.
            source: Source neuron ID.
            target: Target neuron ID.
            weight: Initial synaptic weight. Defaults to 1.0.
            synapse_type: Type of synapse. Defaults to EXCITATORY.
        """
        self.synapses[synapse_id] = {
            'source': source,
            'target': target,
            'weight': weight,
            'type': synapse_type,
            'delay': 0,
            'plasticity_flags': self.plasticity_flags.copy(),
            'custom_properties': {},
        }
        
        # Update neuron connectivity
        if target in self.neurons:
            self.neurons[target]['synapse_sources'].append(synapse_id)
        if source in self.neurons:
            self.neurons[source]['synapse_targets'].append(synapse_id)
    
    def get_neuron_count(self) -> int:
        """Get the number of neurons in this region.
        
        Returns:
            Number of neurons in the region.
        """
        return len(self.neurons)
    
    def get_synapse_count(self) -> int:
        """Get the number of synapses in this region.
        
        Returns:
            Number of synapses in the region.
        """
        return len(self.synapses)
    
    def get_neuron_ids(self) -> List[NeuronId]:
        """Get all neuron IDs in this region.
        
        Returns:
            List of neuron IDs in the region.
        """
        return list(self.neurons.keys())
    
    def get_synapse_ids(self) -> List[SynapseId]:
        """Get all synapse IDs in this region.
        
        Returns:
            List of synapse IDs in the region.
        """
        return list(self.synapses.keys())
    
    def get_neurons_by_type(self, neuron_type: NeuronType) -> List[NeuronId]:
        """Get all neurons of a specific type in this region.
        
        Args:
            neuron_type: Type of neurons to filter by.
            
        Returns:
            List of neuron IDs of the specified type.
        """
        return [nid for nid, neuron in self.neurons.items() if neuron['type'] == neuron_type]
    
    def reset(self):
        """Reset all neurons and synapses in this region to initial state."""
        for neuron in self.neurons.values():
            neuron['state'] = FiringState.RESTING
            neuron['potential'] = 0.0
            neuron['firing_rate'] = 0.0
            neuron['last_spike'] = 0.0
            neuron['refractory_until'] = 0.0
        
        for synapse in self.synapses.values():
            synapse['weight'] = 1.0
            synapse['plasticity_flags'] = self.plasticity_flags.copy()
    
    def __repr__(self) -> str:
        """Get string representation of the neural region.
        
        Returns:
            String representation of the neural region.
        """
        return f"<NeuralRegion '{self.name}' (id: {self.region_id.index()}, " \
               f"neurons: {len(self.neurons)}, synapses: {len(self.synapses)})>"


class Brain:
    """Central neural simulation brain class.
    
    The Brain class manages neural computation, including neurons, synapses,
    plasticity, and sensory-motor processing. It serves as the core
    computational engine of the NLM framework.
    """
    
    def __init__(self, config: Optional[Config] = None):
        """Initialize a Brain instance.
        
        Args:
            config: Configuration object for the brain. If None, a default
                   configuration is created.
        """
        self.config: Config = config if config is not None else Config()
        self.brain_id: int = 0  # Would be set from config or generated
        self.simulation_step: SimulationStep = 0
        self.simulation_time: Timestamp = 0.0
        self.regions: Dict[RegionId, NeuralRegion] = {}
        self.pending_spikes: List[SpikeEvent] = []
        self.delayed_spikes: List[DelayedSpikeEvent] = []
        self.spike_history: List[SpikeEvent] = []
        self.plasticity_enabled: bool = True
        self.development_enabled: bool = False
        self.reward_modulation_enabled: bool = False
        self.curiosity_enabled: bool = False
        self.structural_plasticity_enabled: bool = False
        self.developmental_stage: DevelopmentalStage = DevelopmentalStage.ADULT
        self.status_log: List[str] = []
        self.custom_callbacks: Dict[str, Callable] = {}
    
    def initialize(self):
        """Initialize the brain with configuration settings.
        
        This method sets up the brain based on the configuration,
        creating neural regions and configuring plasticity mechanisms.
        """
        # Initialize from configuration
        self.plasticity_enabled = self.config.getOr("plasticity/enabled", True)
        self.development_enabled = self.config.getOr("development/enabled", False)
        self.reward_modulation_enabled = self.config.getOr("reward_modulation/enabled", False)
        self.curiosity_enabled = self.config.getOr("curiosity/enabled", False)
        self.structural_plasticity_enabled = self.config.getOr("structural_plasticity/enabled", False)
        
        # Reset developmental stage based on config
        dev_stage_str = self.config.getOr("development/initial_stage", "Adult")
        try:
            self.developmental_stage = DevelopmentalStage[dev_stage_str.upper()]
        except (KeyError, AttributeError):
            self.developmental_stage = DevelopmentalStage.ADULT
        
        self.log_status(f"Brain {self.brain_id} initialized with {len(self.regions)} regions")
    
    def step(self, current_step: SimulationStep, current_time: Optional[Timestamp] = None):
        """Perform a simulation step.
        
        This is the main simulation loop that processes neural activity,
        plasticity, and updates the state of the brain.
        
        Args:
            current_step: Current simulation step number.
            current_time: Current simulation time in seconds. If None,
                         calculated from current_step based on timestep.
        """
        if current_time is None:
            timestep = self.config.getOr("simulation/timestep", 0.1)
            current_time = current_step * timestep
        
        self.simulation_step = current_step
        self.simulation_time = current_time
        
        # Process pending spikes
        self._process_spikes()
        
        # Update development
        if self.development_enabled:
            self._update_development(current_step)
        
        # Update plasticity
        if self.plasticity_enabled:
            self._update_plasticity()
        
        # Apply current to neurons
        self._apply_current_to_neurons()
        
        # Update neuron states
        self._update_neuron_states()
        
        # Generate spikes
        self._generate_spikes()
        
        # Store spike history
        for spike in self.pending_spikes:
            self.spike_history.append(spike)
        
        self.log_status(f"Completed step {current_step} at time {current_time:.2f}s")
    
    def receiveSensoryInput(self, input_data: Any):
        """Inject sensory input into the brain.
        
        Args:
            input_data: Sensory input data to inject.
        """
        # Process sensory input (implementation depends on input type)
        # This would typically update neuron states based on sensory data
        self.log_status(f"Received sensory input of type {type(input_data).__name__}")
    
    def injectCurrent(self, neuron: NeuronId, current: float):
        """Inject current into a specific neuron.
        
        Args:
            neuron: Neuron ID to inject current into.
            current: Current value to inject (microsiemens).
        """
        for region in self.regions.values():
            if neuron in region.neurons:
                neuron_data = region.neurons[neuron]
                neuron_data['potential'] += current
                break
        
        self.log_status(f"Injected {current} current into neuron {neuron.index()}")
    
    def injectCurrentToNeurons(self, neuron_type: NeuronType, current: float):
        """Inject current into all neurons of a specific type.
        
        Args:
            neuron_type: Type of neurons to inject current into.
            current: Current value to inject (microsiemens).
        """
        for region in self.regions.values():
            for neuron_id, neuron_data in region.neurons.items():
                if neuron_data['type'] == neuron_type:
                    neuron_data['potential'] += current
        
        self.log_status(f"Injected {current} current into all {neuron_type.name} neurons")
    
    def produceAction(self) -> Dict[str, Any]:
        """Produce motor action based on neural activity.
        
        This method analyzes neural activity and produces an action
        to be sent to the motor system.
        
        Returns:
            Dictionary representing the action to produce.
        """
        # Implementation depends on the motor system interface
        # For now, return a simple action based on overall firing rate
        total_firing = self.getAverageFiringRate()
        
        action = {
            'type': 'CUSTOM',
            'value': total_firing,
            'confidence': min(total_firing / 100.0, 1.0),
        }
        
        self.log_status(f"Produced action based on firing rate: {total_firing:.2f}")
        return action
    
    def reset(self):
        """Reset brain state to initial conditions."""
        self.simulation_step = 0
        self.simulation_time = 0.0
        self.pending_spikes.clear()
        self.delayed_spikes.clear()
        self.spike_history.clear()
        
        for region in self.regions.values():
            region.reset()
        
        self.log_status("Brain state reset")
    
    def save(self, filepath: str):
        """Save brain state to a file.
        
        Args:
            filepath: Path to the file to save to.
        """
        # Implementation for saving brain state
        # This would serialize neural state, configurations, etc.
        self.log_status(f"Brain state would be saved to {filepath}")
    
    def load(self, filepath: str):
        """Load brain state from a file.
        
        Args:
            filepath: Path to the file to load from.
        """
        # Implementation for loading brain state
        # This would deserialize neural state, configurations, etc.
        self.log_status(f"Brain state would be loaded from {filepath}")
    
    def addRegion(self, name: str = "") -> RegionId:
        """Add a new neural region.
        
        Args:
            name: Name for the new region. If empty, auto-generated.
            
        Returns:
            ID of the newly created region.
        """
        if not name:
            name = f"Region_{len(self.regions)}"
        
        region = NeuralRegion(name)
        self.regions[region.region_id] = region
        self.log_status(f"Added region '{name}' with ID {region.region_id.index()}")
        return region.region_id
    
    def getRegion(self, region_id: RegionId) -> Optional[NeuralRegion]:
        """Get a region by ID.
        
        Args:
            region_id: ID of the region to retrieve.
            
        Returns:
            The neural region with the specified ID, or None if not found.
        """
        return self.regions.get(region_id)
    
    def getRegionCount(self) -> int:
        """Get the number of regions in the brain.
        
        Returns:
            Number of regions in the brain.
        """
        return len(self.regions)
    
    def getRegionIds(self) -> List[RegionId]:
        """Get all region IDs in the brain.
        
        Returns:
            List of all region IDs in the brain.
        """
        return list(self.regions.keys())
    
    def getRegions(self) -> List[NeuralRegion]:
        """Get all regions in the brain.
        
        Returns:
            List of all neural regions in the brain.
        """
        return list(self.regions.values())
    
    def getTotalNeuronCount(self) -> int:
        """Get total neuron count across all regions.
        
        Returns:
            Total number of neurons in the brain.
        """
        return sum(region.get_neuron_count() for region in self.regions.values())
    
    def getTotalSynapseCount(self) -> int:
        """Get total synapse count across all regions.
        
        Returns:
            Total number of synapses in the brain.
        """
        return sum(region.get_synapse_count() for region in self.regions.values())
    
    def getActiveNeuronCount(self) -> int:
        """Get count of active neurons.
        
        Returns:
            Number of neurons currently in ACTIVE state.
        """
        active_count = 0
        for region in self.regions.values():
            for neuron in region.neurons.values():
                if neuron['state'] == FiringState.ACTIVE:
                    active_count += 1
        return active_count
    
    def getFiringNeuronCount(self) -> int:
        """Get count of currently firing neurons.
        
        Returns:
            Number of neurons currently firing.
        """
        firing_count = 0
        for region in self.regions.values():
            for neuron in region.neurons.values():
                if neuron['state'] == FiringState.ACTIVE:
                    firing_count += 1
        return firing_count
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate across all neurons.
        
        Returns:
            Average firing rate (Hz) across all neurons.
        """
        total_rate = 0.0
        neuron_count = 0
        
        for region in self.regions.values():
            for neuron in region.neurons.values():
                total_rate += neuron['firing_rate']
                neuron_count += 1
        
        return total_rate / neuron_count if neuron_count > 0 else 0.0
    
    def getExcitationInhibitionRatio(self) -> float:
        """Get excitation/inhibition balance ratio.
        
        Returns:
            Ratio of excitation to inhibition in the network.
        """
        excitation = 0.0
        inhibition = 0.0
        
        for region in self.regions.values():
            for synapse in region.synapses.values():
                if synapse['type'] == SynapseType.EXCITATORY:
                    excitation += synapse['weight']
                elif synapse['type'] == SynapseType.INHIBITORY:
                    inhibition += synapse['weight']
        
        return excitation / inhibition if inhibition > 0 else float('inf')
    
    def getTotalSpikeCount(self) -> int:
        """Get total spike count.
        
        Returns:
            Total number of spikes in the simulation.
        """
        return len(self.spike_history)
    
    def getDevelopmentalStage(self) -> DevelopmentalStage:
        """Get current developmental stage.
        
        Returns:
            Current developmental stage.
        """
        return self.developmental_stage
    
    def setDevelopmentalStage(self, stage: DevelopmentalStage):
        """Set developmental stage.
        
        Args:
            stage: New developmental stage.
        """
        self.developmental_stage = stage
        self.log_status(f"Developmental stage set to {stage.name}")
    
    def getConfig(self) -> Config:
        """Get the configuration object.
        
        Returns:
            The brain's configuration.
        """
        return self.config
    
    def logStatus(self, message: str):
        """Log a status message.
        
        Args:
            message: Status message to log.
        """
        self.status_log.append(message)
    
    def getStatusLog(self) -> List[str]:
        """Get the status log.
        
        Returns:
            List of status messages.
        """
        return self.status_log.copy()
    
    def addCallback(self, event: str, callback: Callable):
        """Add a callback for a specific event.
        
        Args:
            event: Event name to listen for.
            callback: Function to call when event occurs.
        """
        self.custom_callbacks[event] = callback
    
    def _process_spikes(self):
        """Process pending spikes and schedule delayed spikes."""
        # Move spikes to delayed spikes
        for spike in self.pending_spikes:
            # Create delayed spike event for each spike
            delayed = DelayedSpikeEvent(
                source_neuron=spike.source_neuron,
                destination_neuron=SynapseId(),  # This should be set properly
                synapse_id=SynapseId(),  # This should be set properly
                weight=1.0,  # Default weight
                synapse_type=SynapseType.EXCITATORY,
                timestamp=spike.timestamp,
                delivery_time=spike.timestamp + self.config.getOr("simulation/delay", 0.01),
                step=spike.step,
                delivery_step=spike.step + self.config.getOr("simulation/delay", 10),
                is_excitatory=True,
            )
            self.delayed_spikes.append(delayed)
        
        self.pending_spikes.clear()
    
    def _update_development(self, current_step: SimulationStep):
        """Update developmental processes.
        
        Args:
            current_step: Current simulation step.
        """
        # Developmental update logic would go here
        pass
    
    def _update_plasticity(self):
        """Update synaptic plasticity mechanisms."""
        # Plasticity update logic would go here
        pass
    
    def _apply_current_to_neurons(self):
        """Apply injected current to neurons."""
        for region in self.regions.values():
            for neuron_data in region.neurons.values():
                # Apply current to membrane potential
                neuron_data['potential'] *= 0.9  # Simple decay
    
    def _update_neuron_states(self):
        """Update neuron states based on potentials and inputs."""
        for region in self.regions.values():
            for neuron_id, neuron_data in region.neurons.items():
                potential = neuron_data['potential']
                
                if neuron_data['state'] == FiringState.REFRACTORY:
                    # Check if refractory period has passed
                    if self.simulation_time >= neuron_data['refractory_until']:
                        neuron_data['state'] = FiringState.RESTING
                
                # Simple firing threshold logic
                if potential > self.config.getOr("neuron/threshold", 1.0) and \
                   neuron_data['state'] == FiringState.RESTING:
                    neuron_data['state'] = FiringState.ACTIVE
                    neuron_data['last_spike'] = self.simulation_time
                    neuron_data['refractory_until'] = self.simulation_time + \
                        self.config.getOr("neuron/refractory_period", 0.002)
                
                # Update firing rate (simplified)
                if neuron_data['state'] == FiringState.ACTIVE:
                    neuron_data['firing_rate'] = 1.0 / self.config.getOr("neuron/firing_period", 0.02)
                else:
                    neuron_data['firing_rate'] = 0.0
    
    def _generate_spikes(self):
        """Generate spikes based on neuron states."""
        for region in self.regions.values():
            for neuron_id, neuron_data in region.neurons.items():
                if neuron_data['state'] == FiringState.ACTIVE:
                    spike = SpikeEvent(
                        source_neuron=neuron_id,
                        timestamp=self.simulation_time,
                        step=self.simulation_step,
                    )
                    self.pending_spikes.append(spike)
                    
                    # Reset neuron state
                    neuron_data['state'] = FiringState.REFRACTORY
    
    # Note: DelayedSpikeEvent is referenced in this class but not imported.
    # This is intentional to avoid circular imports.
    DelayedSpikeEvent = DelayedSpikeEvent


__all__ = ['Brain']
