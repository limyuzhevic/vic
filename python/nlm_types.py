import sys
import threading
import inspect
from typing import Dict, List, Any, Optional, Union, Tuple
from enum import Enum
from dataclasses import dataclass
from abc import ABC, abstractmethod

# Global registry system
class NLMTypeRegistry:
    """Registry for custom NLM types"""
    
    def __init__(self):
        self._types = {}
        self._type_classes = {}
        self._lock = threading.RLock()
    
    def register_type(self, name: str, type_class: type, schema: Optional[Dict[str, Any]] = None) -> None:
        """Register a custom type"""
        with self._lock:
            self._types[name] = type_class
            self._type_classes[name] = {
                'class': type_class,
                'schema': schema or {},
                'validators': [],
                'transformers': []
            }
    
    def get_type(self, name: str) -> Optional[type]:
        """Get a type by name"""
        return self._types.get(name)
    
    def get_type_info(self, name: str) -> Optional[Dict[str, Any]]:
        """Get type information"""
        return self._type_classes.get(name)
    
    def list_types(self) -> List[str]:
        """List all registered types"""
        return list(self._types.keys())
    
    def validate_type(self, name: str, instance: Any) -> bool:
        """Validate an instance against its type"""
        if name not in self._type_classes:
            return False
        
        type_info = self._type_classes[name]
        type_class = type_info['class']
        
        # Check if instance is of expected type
        if not isinstance(instance, type_class):
            return False
        
        # Run custom validators
        for validator in type_info['validators']:
            if not validator(instance):
                return False
        
        return True

class NLMTypeValidator(ABC):
    """Abstract base class for type validators"""
    
    @abstractmethod
    def validate(self, instance: Any) -> bool:
        """Validate an instance"""
        pass
    
    @abstractmethod
    def get_error_message(self, instance: Any) -> str:
        """Get error message for validation failure"""
        pass

class NLMTypeTransformer(ABC):
    """Abstract base class for type transformers"""
    
    @abstractmethod
    def transform(self, instance: Any) -> Any:
        """Transform an instance"""
        pass
    
    @abstractmethod
    def can_transform(self, instance: Any) -> bool:
        """Check if instance can be transformed"""
        pass

class NLMTypeConverter:
    """Converter for custom NLM types"""
    
    def __init__(self):
        self._to_python = {}
        self._from_python = {}
        self._lock = threading.RLock()
    
    def register_converter(self, 
                          to_type: str, 
                          from_type: str,
                          converter: Callable,
                          direction: str = 'both') -> None:
        """Register a converter"""
        with self._lock:
            if direction in ('both', 'to'):
                if to_type not in self._to_python:
                    self._to_python[to_type] = {}
                self._to_python[to_type][from_type] = converter
            
            if direction in ('both', 'from'):
                if from_type not in self._from_python:
                    self._from_python[from_type] = {}
                self._from_python[from_type][to_type] = converter
    
    def convert_to_python(self, value: Any, target_type: str) -> Any:
        """Convert value to Python type"""
        with self._lock:
            if target_type in self._to_python:
                for source_type, converter in self._to_python[target_type].items():
                    if isinstance(value, self._get_type_by_name(source_type)):
                        return converter(value)
            return value
    
    def convert_from_python(self, value: Any, source_type: str) -> Any:
        """Convert value from Python type"""
        with self._lock:
            if source_type in self._from_python:
                for target_type, converter in self._from_python[source_type].items():
                    if isinstance(value, self._get_type_by_name(target_type)):
                        return converter(value)
            return value
    
    def _get_type_by_name(self, type_name: str) -> type:
        """Get type by name from registry"""
        # This would need to be implemented with access to the NLM type registry
        return type_name

@dataclass
class Neuritoid:
    """Custom neuron type for extended NLM"""
    
    id: 'NeuronId'
    type: 'NeuronType'
    position: Tuple[float, float]
    velocity: Tuple[float, float]
    membrane_potential: float
    refractory_period: float
    age: float
    weight: float
    plasticity_params: Dict[str, Any]
    metadata: Dict[str, Any]
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary"""
        return {
            'id': self.id,
            'type': self.type,
            'position': self.position,
            'velocity': self.velocity,
            'membrane_potential': self.membrane_potential,
            'refractory_period': self.refractory_period,
            'age': self.age,
            'weight': self.weight,
            'plasticity_params': self.plasticity_params,
            'metadata': self.metadata
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'Neuritoid':
        """Create from dictionary"""
        return cls(**data)

@dataclass
class Synapseoid:
    """Custom synapse type for extended NLM"""
    
    id: 'SynapseId'
    pre_neuron_id: 'NeuronId'
    post_neuron_id: 'NeuronId'
    type: 'SynapseType'
    strength: float
    delay: float
    plasticity_rules: Dict[str, Any]
    metadata: Dict[str, Any]
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary"""
        return {
            'id': self.id,
            'pre_neuron_id': self.pre_neuron_id,
            'post_neuron_id': self.post_neuron_id,
            'type': self.type,
            'strength': self.strength,
            'delay': self.delay,
            'plasticity_rules': self.plasticity_rules,
            'metadata': self.metadata
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'Synapseoid':
        """Create from dictionary"""
        return cls(**data)

@dataclass
class Regionoid:
    """Custom region type for extended NLM"""
    
    id: 'RegionId'
    name: str
    neurons: List['Neuritoid']
    synapses: List['Synapseoid']
    properties: Dict[str, Any]
    metadata: Dict[str, Any]
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary"""
        return {
            'id': self.id,
            'name': self.name,
            'neurons': [neuron.to_dict() for neuron in self.neurons],
            'synapses': [synapse.to_dict() for synapse in self.synapses],
            'properties': self.properties,
            'metadata': self.metadata
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'Regionoid':
        """Create from dictionary"""
        neurons = [Neuritoid.from_dict(n) for n in data['neurons']]
        synapses = [Synapseoid.from_dict(s) for s in data['synapses']]
        return cls(**data, neurons=neurons, synapses=synapses)

class NLMTypeFactory:
    """Factory for creating custom NLM types"""
    
    def __init__(self):
        self._factories = {}
        self._lock = threading.RLock()
    
    def register_factory(self, name: str, factory: Callable) -> None:
        """Register a factory"""
        with self._lock:
            self._factories[name] = factory
    
    def create(self, name: str, *args, **kwargs) -> Any:
        """Create an instance using registered factory"""
        with self._lock:
            if name not in self._factories:
                raise ValueError(f"Unknown factory: {name}")
            return self._factories[name](*args, **kwargs)
    
    def list_factories(self) -> List[str]:
        """List all registered factories"""
        return list(self._factories.keys())

class NLMTypeValidatorRegistry:
    """Registry for type validators"""
    
    def __init__(self):
        self._validators = []
        self._lock = threading.RLock()
    
    def add_validator(self, validator: NLMTypeValidator) -> None:
        """Add a validator"""
        with self._lock:
            self._validators.append(validator)
    
    def validate_all(self, instance: Any) -> List[str]:
        """Validate instance against all validators"""
        errors = []
        for validator in self._validators:
            if not validator.validate(instance):
                errors.append(validator.get_error_message(instance))
        return errors

class NLMTypeSerializer:
    """Serializer for custom NLM types"""
    
    def __init__(self):
        self._serializers = {}
        self._deserializers = {}
        self._lock = threading.RLock()
    
    def register_serializer(self, 
                          type_name: str,
                          serializer: Callable,
                          deserializer: Optional[Callable] = None) -> None:
        """Register a serializer"""
        with self._lock:
            self._serializers[type_name] = serializer
            if deserializer:
                self._deserializers[type_name] = deserializer
    
    def serialize(self, obj: Any, type_name: Optional[str] = None) -> Any:
        """Serialize an object"""
        if type_name is None:
            type_name = type(obj).__name__
        
        with self._lock:
            if type_name in self._serializers:
                return self._serializers[type_name](obj)
            elif hasattr(obj, 'to_dict'):
                return obj.to_dict()
            else:
                raise ValueError(f"No serializer for type {type_name}")
    
    def deserialize(self, data: Any, type_name: str) -> Any:
        """Deserialize data to object"""
        with self._lock:
            if type_name in self._deserializers:
                return self._deserializers[type_name](data)
            elif hasattr(type_name, 'from_dict'):
                return type_name.from_dict(data)
            else:
                raise ValueError(f"No deserializer for type {type_name}")

# Global instances
_type_registry = NLMTypeRegistry()
_type_converter = NLMTypeConverter()
_type_factory = NLMTypeFactory()
_validator_registry = NLMTypeValidatorRegistry()
_type_serializer = NLMTypeSerializer()

# Custom type definitions for NLM

# Register built-in NLM types (these would need to be imported when pynlm is available)
try:
    from pynlm import NeuronId, SynapseId, RegionId, PopulationId
    from pynlm import NeuronType, SynapseType, DevelopmentalStage, FiringState
    from pynlm import ActionType, MotorCommand, WorldObjectType
    
    _type_registry.register_type('NeuronId', NeuronId)
    _type_registry.register_type('SynapseId', SynapseId)
    _type_registry.register_type('RegionId', RegionId)
    _type_registry.register_type('PopulationId', PopulationId)
    
    _type_registry.register_type('NeuronType', NeuronType, {
        'values': ['Excitatory', 'Inhibitory', 'Modulatory', 'Sensory', 'Motor', 'Internal']
    })
    
    _type_registry.register_type('SynapseType', SynapseType, {
        'values': ['Excitatory', 'Inhibitory', 'Modulatory', 'Electrical', 'GapJunction']
    })
    
    _type_registry.register_type('DevelopmentalStage', DevelopmentalStage, {
        'values': ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']
    })
    
    _type_registry.register_type('FiringState', FiringState, {
        'values': ['Resting', 'Active', 'Refractory', 'Inhibited']
    })
    
    _type_registry.register_type('ActionType', ActionType, {
        'values': ['MoveForward', 'MoveBackward', 'MoveLeft', 'MoveRight', 'TurnLeft', 'TurnRight', 
                   'Look', 'LookUp', 'LookDown', 'Interact', 'Eat', 'Drink', 'Rest', 'Wait', 'Custom']
    })
    
    _type_registry.register_type('MotorCommand', MotorCommand, {
        'values': ['MoveForward', 'MoveBackward', 'TurnLeft', 'TurnRight', 'LookLeft', 'LookRight', 
                   'Interact', 'Wait']
    })
    
    _type_registry.register_type('WorldObjectType', WorldObjectType, {
        'values': ['Empty', 'Resource', 'Hazard', 'Wall', 'Marker']
    })
except ImportError:
    # NLM types not available yet, will be registered when pynlm is imported
    pass

# Register custom extended types
_type_registry.register_type('Neuritoid', Neuritoid)
_type_registry.register_type('Synapseoid', Synapseoid)
_type_registry.register_type('Regionoid', Regionoid)

# Public API
__all__ = [
    'NLMTypeRegistry',
    'NLMTypeValidator',
    'NLMTypeTransformer',
    'NLMTypeConverter',
    'NLMTypeFactory',
    'NLMTypeValidatorRegistry',
    'NLMTypeSerializer',
    'Neuritoid',
    'Synapseoid',
    'Regionoid',
    'register_type',
    'get_type',
    'get_type_info',
    'list_types',
    'validate_type',
    'register_converter',
    'convert_to_python',
    'convert_from_python',
    'register_factory',
    'create',
    'list_factories',
    'add_validator',
    'validate_all',
    'register_serializer',
    'serialize',
    'deserialize',
]

# Convenience functions
def register_type(name: str, type_class: type, schema: Optional[Dict[str, Any]] = None) -> None:
    """Register a custom type"""
    _type_registry.register_type(name, type_class, schema)

def get_type(name: str) -> Optional[type]:
    """Get a type by name"""
    return _type_registry.get_type(name)

def get_type_info(name: str) -> Optional[Dict[str, Any]]:
    """Get type information"""
    return _type_registry.get_type_info(name)

def list_types() -> List[str]:
    """List all registered types"""
    return _type_registry.list_types()

def validate_type(name: str, instance: Any) -> bool:
    """Validate an instance against its type"""
    return _type_registry.validate_type(name, instance)

def register_converter(to_type: str, 
                      from_type: str,
                      converter: Callable,
                      direction: str = 'both') -> None:
    """Register a converter"""
    _type_converter.register_converter(to_type, from_type, converter, direction)

def convert_to_python(value: Any, target_type: str) -> Any:
    """Convert value to Python type"""
    return _type_converter.convert_to_python(value, target_type)

def convert_from_python(value: Any, source_type: str) -> Any:
    """Convert value from Python type"""
    return _type_converter.convert_from_python(value, source_type)

def register_factory(name: str, factory: Callable) -> None:
    """Register a factory"""
    _type_factory.register_factory(name, factory)

def create(name: str, *args, **kwargs) -> Any:
    """Create an instance using registered factory"""
    return _type_factory.create(name, *args, **kwargs)

def list_factories() -> List[str]:
    """List all registered factories"""
    return _type_factory.list_factories()

def add_validator(validator: NLMTypeValidator) -> None:
    """Add a validator"""
    _validator_registry.add_validator(validator)

def validate_all(instance: Any) -> List[str]:
    """Validate instance against all validators"""
    return _validator_registry.validate_all(instance)

def register_serializer(type_name: str, 
                       serializer: Callable,
                       deserializer: Optional[Callable] = None) -> None:
    """Register a serializer"""
    _type_serializer.register_serializer(type_name, serializer, deserializer)

def serialize(obj: Any, type_name: Optional[str] = None) -> Any:
    """Serialize an object"""
    return _type_serializer.serialize(obj, type_name)

def deserialize(data: Any, type_name: str) -> Any:
    """Deserialize data to object"""
    return _type_serializer.deserialize(data, type_name)