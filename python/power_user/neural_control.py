"""
Low-Level Neural Control for Power Users

This module provides low-level neural control for power users, including direct neuron
"""

from typing import Dict, List, Optional, Any, Tuple
from .safety import SafetyManager, PowerUserError, ValidationError
class AdvancedNeuronState:
    """Advanced neuron state with additional metadata and properties"""
    
    def __init__(self, neuron_id: int, potential: float = -70.0,
                 refractory: float = 0.0, age: float = 0.0,
                 metadata: Optional[Dict] = None):
        self.neuron_id = neuron_id
        self.potential = potential
        self.refractory = refractory
        self.age = age
        self.metadata = metadata or {}
        self.history: List[Dict] = []
        self.last_updated = 0.0
    
    def to_dict(self) -> Dict:
        """Convert state to dictionary"""
        return {
            'neuron_id': self.neuron_id,
            'potential': self.potential,
            'refractory': self.refractory,
            'age': self.age,
            'metadata': self.metadata,
            'history': self.history,
            'last_updated': self.last_updated
        }
class AdvancedSynapseState:
    """Advanced synapse state with additional properties"""
    
    def __init__(self, source_id: int, target_id: int, weight: float = 0.5,
                 delay: float = 1.0, plasticity_params: Optional[Dict] = None,
                 metadata: Optional[Dict] = None):
        self.source_id = source_id
        self.target_id = target_id
        self.weight = weight
        self.delay = delay
        self.plasticity_params = plasticity_params or {}
        self.metadata = metadata or {}
        self.history: List[Dict] = []
        self.last_updated = 0.0
    
    def to_dict(self) -> Dict:
        """Convert state to dictionary"""
        return {
            'source_id': self.source_id,
            'target_id': self.target_id,
            'weight': self.weight,
            'delay': self.delay,
            'plasticity_params': self.plasticity_params,
            'metadata': self.metadata,
            'history': self.history,
            'last_updated': self.last_updated
        }
class LowLevelNeuralController:
    """Low-level neural control for power users"""
    
    def __init__(self, brain_interface, safety_manager: SafetyManager):
        self.brain = brain_interface
        self.safety = safety_manager
        self.neuron_cache: Dict[int, AdvancedNeuronState] = {}
        self.synapse_cache: Dict[Tuple[int, int], AdvancedSynapseState] = {}
        self.neuron_id_pool = set()
        self.synapse_id_pool = set()
    
    def add_neuron(self, neuron_type: str = "LIF", properties: Optional[Dict] = None,
                   position: Optional[Dict] = None) -> int:
        """Add a new neuron with advanced properties"""
        self.safety.validate_safety('add_neuron', {'type': neuron_type})
        
        neuron_id = self._allocate_neuron_id()
        
        if properties is None:
            properties = {
                'potential': -70.0,
                'refractory_period': 5.0,
                'threshold': -55.0,
                'rest_potential': -70.0,
                'max_potential': 50.0,
                'capacitance': 250.0,
                'time_constant': 10.0,
                'type': neuron_type
            }
        
        # Create advanced neuron state
        neuron_state = AdvancedNeuronState(
            neuron_id=neuron_id,
            potential=properties.get('potential', -70.0),
            refractory=properties.get('refractory_period', 0.0),
            age=0.0,
            metadata={
                'type': neuron_type,
                'properties': properties,
                'position': position or {'x': 0, 'y': 0, 'z': 0},
                'added_time': 0.0
            }
        )
        
        self.neuron_cache[neuron_id] = neuron_state
        
        # Note: This would integrate with the underlying C++ brain
        # For now, we cache locally and would need to sync with the brain
        return neuron_id
    
    def remove_neuron(self, neuron_id: int, delete_synapses: bool = True) -> bool:
        """Remove a neuron and optionally its synapses"""
        self.safety.validate_safety('remove_neuron', {'neuron_id': neuron_id})
        
        if neuron_id not in self.neuron_cache:
            raise PowerUserError(f"Neuron {neuron_id} not found")
        
        # Remove associated synapses if requested
        if delete_synapses:
            self._remove_synapses_for_neuron(neuron_id)
        
        # Deallocate neuron ID
        self._deallocate_neuron_id(neuron_id)
        
        # Remove from cache
        del self.neuron_cache[neuron_id]
        
        return True
    
    def add_synapse(self, source_id: int, target_id: int, weight: float = 0.5,
                    delay: float = 1.0, plasticity_rule: Optional[str] = None,
                    properties: Optional[Dict] = None) -> Tuple[int, int]:
        """Add a synapse between two neurons"""
        self.safety.validate_safety('add_synapse', {
            'source_id': source_id,
            'target_id': target_id
        })
        
        # Validate neurons exist
        if source_id not in self.neuron_cache:
            raise PowerUserError(f"Source neuron {source_id} not found")
        
        if target_id not in self.neuron_cache:
            raise PowerUserError(f"Target neuron {target_id} not found")
        
        if properties is None:
            properties = {
                'weight': weight,
                'delay': delay,
                'plasticity_rule': plasticity_rule,
                'enabled': True,
                'added_time': 0.0
            }
        
        # Create advanced synapse state
        synapse_state = AdvancedSynapseState(
            source_id=source_id,
            target_id=target_id,
            weight=properties.get('weight', weight),
            delay=properties.get('delay', delay),
            plasticity_params={'rule': plasticity_rule} if plasticity_rule else {},
            metadata=properties
        )
        
        synapse_key = (source_id, target_id)
        self.synapse_cache[synapse_key] = synapse_state
        
        return source_id, target_id
    
    def remove_synapse(self, source_id: int, target_id: int) -> bool:
        """Remove a synapse"""
        self.safety.validate_safety('remove_synapse', {
            'source_id': source_id,
            'target_id': target_id
        })
        
        synapse_key = (source_id, target_id)
        if synapse_key not in self.synapse_cache:
            raise PowerUserError(f"Synapse from {source_id} to {target_id} not found")
        
        del self.synapse_cache[synapse_key]
        return True
    
    def modify_neuron_state(self, neuron_id: int, updates: Dict) -> AdvancedNeuronState:
        """Modify neuron state with validation"""
        self.safety.validate_safety('modify_neuron_state', {
            'neuron_id': neuron_id,
            'updates': updates
        })
        
        if neuron_id not in self.neuron_cache:
            raise PowerUserError(f"Neuron {neuron_id} not found")
        
        neuron_state = self.neuron_cache[neuron_id]
        
        # Apply updates with validation
        valid_fields = ['potential', 'refractory', 'age', 'metadata']
        for field, value in updates.items():
            if field not in valid_fields:
                raise ValidationError(f"Invalid neuron field: {field}")
            
            setattr(neuron_state, field, value)
        
        neuron_state.last_updated = 0.0
        neuron_state.history.append({
            'timestamp': 0.0,
            'updates': updates.copy(),
            'operation': 'modify'
        })
        
        return neuron_state
    
    def modify_synapse_state(self, source_id: int, target_id: int,
                           updates: Dict) -> AdvancedSynapseState:
        """Modify synapse state with validation"""
        self.safety.validate_safety('modify_synapse_state', {
            'source_id': source_id,
            'target_id': target_id,
            'updates': updates
        })
        
        synapse_key = (source_id, target_id)
        if synapse_key not in self.synapse_cache:
            raise PowerUserError(f"Synapse from {source_id} to {target_id} not found")
        
        synapse_state = self.synapse_cache[synapse_key]
        
        # Apply updates with validation
        valid_fields = ['weight', 'delay', 'plasticity_params', 'metadata']
        for field, value in updates.items():
            if field not in valid_fields:
                raise ValidationError(f"Invalid synapse field: {field}")
            
            setattr(synapse_state, field, value)
        
        synapse_state.last_updated = 0.0
        synapse_state.history.append({
            'timestamp': 0.0,
            'updates': updates.copy(),
            'operation': 'modify'
        })
        
        return synapse_state
    
    def get_neuron_state(self, neuron_id: int) -> AdvancedNeuronState:
        """Get advanced neuron state"""
        if neuron_id not in self.neuron_cache:
            raise PowerUserError(f"Neuron {neuron_id} not found")
        
        return self.neuron_cache[neuron_id]
    
    def get_synapse_state(self, source_id: int, target_id: int) -> AdvancedSynapseState:
        """Get advanced synapse state"""
        synapse_key = (source_id, target_id)
        if synapse_key not in self.synapse_cache:
            raise PowerUserError(f"Synapse from {source_id} to {target_id} not found")
        
        return self.synapse_cache[synapse_key]
    
    def get_all_neurons(self) -> List[AdvancedNeuronState]:
        """Get all neurons"""
        return list(self.neuron_cache.values())
    
    def get_all_synapses(self) -> List[AdvancedSynapseState]:
        """Get all synapses"""
        return list(self.synapse_cache.values())
    
    def find_neurons(self, criteria: Dict) -> List[AdvancedNeuronState]:
        """Find neurons matching criteria"""
        results = []
        
        for neuron_state in self.neuron_cache.values():
            match = True
            for key, value in criteria.items():
                if key == 'type':
                    if neuron_state.metadata.get('type') != value:
                        match = False
                        break
                elif key == 'age_range':
                    if not (value[0] <= neuron_state.age <= value[1]):
                        match = False
                        break
                elif key == 'potential_range':
                    if not (value[0] <= neuron_state.potential <= value[1]):
                        match = False
                        break
                else:
                    if neuron_state.metadata.get(key) != value:
                        match = False
                        break
            
            if match:
                results.append(neuron_state)
        
        return results
    
    def find_synapses(self, criteria: Dict) -> List[AdvancedSynapseState]:
        """Find synapses matching criteria"""
        results = []
        
        for synapse_state in self.synapse_cache.values():
            match = True
            for key, value in criteria.items():
                if key == 'weight_range':
                    if not (value[0] <= synapse_state.weight <= value[1]):
                        match = False
                        break
                elif key == 'delay_range':
                    if not (value[0] <= synapse_state.delay <= value[1]):
                        match = False
                        break
                elif key == 'rule':
                    if synapse_state.plasticity_params.get('rule') != value:
                        match = False
                        break
                elif synapse_state.metadata.get(key) != value:
                    match = False
                    break
            
            if match:
                results.append(synapse_state)
        
        return results
    
    def _allocate_neuron_id(self) -> int:
        """Allocate a neuron ID from pool"""
        if self.neuron_id_pool:
            return self.neuron_id_pool.pop()
        else:
            # Generate new ID (would need to integrate with brain's ID system)
            return len(self.neuron_cache) + 1000
    
    def _deallocate_neuron_id(self, neuron_id: int):
        """Deallocate a neuron ID to pool"""
        self.neuron_id_pool.add(neuron_id)
    
    def _remove_synapses_for_neuron(self, neuron_id: int):
        """Remove all synapses connected to a neuron"""
        synapses_to_remove = []
        for synapse_key in self.synapse_cache.keys():
            if synapse_key[0] == neuron_id or synapse_key[1] == neuron_id:
                synapses_to_remove.append(synapse_key)
        
        for synapse_key in synapses_to_remove:
            del self.synapse_cache[synapse_key]