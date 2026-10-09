import sys
import json
import threading
import time
from typing import Callable, Dict, List, Any, Optional, Union
from enum import Enum

# Import the main NLM bindings
sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_206b7df8-50af-4a9d-949d-21e5ab9e36cf/python')

try:
    import pynlm
except ImportError:
    raise ImportError("pynlm module not found. Please ensure NLM Python bindings are installed.")

class NLMRegistry:
    """Registry for custom NLM types and classes"""
    
    _registry = {}
    _lock = threading.Lock()
    
    @classmethod
    def register(cls, name: str, factory: Callable) -> None:
        """Register a custom factory function for a type name"""
        with cls._lock:
            cls._registry[name] = factory
    
    @classmethod
    def unregister(cls, name: str) -> None:
        """Unregister a custom type"""
        with cls._lock:
            if name in cls._registry:
                del cls._registry[name]
    
    @classmethod
    def get_factory(cls, name: str) -> Optional[Callable]:
        """Get a registered factory function"""
        return cls._registry.get(name)
    
    @classmethod
    def list_types(cls) -> List[str]:
        """List all registered type names"""
        with cls._lock:
            return list(cls._registry.keys())
    
    @classmethod
    def create_instance(cls, name: str, *args, **kwargs) -> Any:
        """Create an instance of a registered type"""
        factory = cls.get_factory(name)
        if factory is None:
            raise ValueError(f"Unknown type: {name}")
        return factory(*args, **kwargs)

class NLMFactory:
    """Factory class for creating NLM instances with custom logic"""
    
    @staticmethod
    def create_brain_with_config(config_dict: Dict[str, Any]) -> 'pynlm.Brain':
        """Create a brain with configuration from dictionary"""
        # Convert dictionary to Config object
        config = pynlm.createDefaultConfig()
        for key, value in config_dict.items():
            if isinstance(value, (int, float, str, bool)):
                config.set(key, value)
        return pynlm.createBrain(config)
    
    @staticmethod
    def create_brain_with_checkpoint(filepath: str) -> 'pynlm.Brain':
        """Create a brain from checkpoint file"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        brain.load(filepath)
        return brain

class NLMHookSystem:
    """Hook system for calling user code at specific points"""
    
    def __init__(self):
        self._hooks = {
            'pre_step': [],
            'post_step': [],
            'on_spike': [],
            'on_learning': [],
            'on_reset': [],
            'on_save': [],
            'on_load': []
        }
        self._lock = threading.Lock()
    
    def add_hook(self, hook_type: str, callback: Callable) -> None:
        """Add a callback to a hook"""
        if hook_type not in self._hooks:
            raise ValueError(f"Unknown hook type: {hook_type}")
        
        with self._lock:
            self._hooks[hook_type].append(callback)
    
    def remove_hook(self, hook_type: str, callback: Callable) -> None:
        """Remove a callback from a hook"""
        with self._lock:
            if callback in self._hooks.get(hook_type, []):
                self._hooks[hook_type].remove(callback)
    
    def call_hooks(self, hook_type: str, *args, **kwargs) -> List[Any]:
        """Call all callbacks for a hook type"""
        results = []
        with self._lock:
            for callback in self._hooks.get(hook_type, []):
                try:
                    result = callback(*args, **kwargs)
                    results.append(result)
                except Exception as e:
                    print(f"Error in hook {hook_type}: {e}")
        return results

class NLMDynamicAPI:
    """Dynamic API for runtime class discovery and introspection"""
    
    @staticmethod
    def get_class(name: str) -> Any:
        """Get a class by name"""
        # This would normally interface with a dynamic binding system
        # For now, return known classes based on name
        known_classes = {
            'Brain': pynlm.Brain,
            'Config': pynlm.Config,
            'SimpleWorld': pynlm.SimpleWorld,
            'AgentBrain': pynlm.AgentBrain,
            'Neuron': None,  # Would need to be exposed if available
            'Synapse': None,  # Would need to be exposed if available
        }
        return known_classes.get(name)
    
    @staticmethod
    def list_classes() -> List[str]:
        """List all available classes"""
        return list(pynlm.__dict__.keys())
    
    @staticmethod
    def get_methods(cls_name: str) -> List[str]:
        """Get methods for a class"""
        cls = NLMDynamicAPI.get_class(cls_name)
        if cls is None:
            return []
        return [method for method in dir(cls) if not method.startswith('_')]

class NLMCallbackSystem:
    """System for registering Python callbacks from C++"""
    
    def __init__(self):
        self._step_callbacks = []
        self._spike_callbacks = []
        self._learning_callbacks = []
        self._lock = threading.Lock()
    
    def add_step_callback(self, callback: Callable) -> None:
        """Add a callback to be called after each simulation step"""
        with self._lock:
            self._step_callbacks.append(callback)
    
    def add_spike_callback(self, callback: Callable) -> None:
        """Add a callback to be called when neurons spike"""
        with self._lock:
            self._spike_callbacks.append(callback)
    
    def add_learning_callback(self, callback: Callable) -> None:
        """Add a callback to be called when learning occurs"""
        with self._lock:
            self._learning_callbacks.append(callback)
    
    def execute_step_callbacks(self, step_data: Dict[str, Any]) -> None:
        """Execute all step callbacks"""
        with self._lock:
            for callback in self._step_callbacks:
                try:
                    callback(step_data)
                except Exception as e:
                    print(f"Error in step callback: {e}")
    
    def execute_spike_callbacks(self, spike_data: Dict[str, Any]) -> None:
        """Execute all spike callbacks"""
        with self._lock:
            for callback in self._spike_callbacks:
                try:
                    callback(spike_data)
                except Exception as e:
                    print(f"Error in spike callback: {e}")
    
    def execute_learning_callbacks(self, learning_data: Dict[str, Any]) -> None:
        """Execute all learning callbacks"""
        with self._lock:
            for callback in self._learning_callbacks:
                try:
                    callback(learning_data)
                except Exception as e:
                    print(f"Error in learning callback: {e}")

class NLMEventEmitter:
    """Event emitter for NLM simulation events"""
    
    def __init__(self):
        self._events = {}
        self._lock = threading.Lock()
    
    def on(self, event_name: str, callback: Callable) -> None:
        """Listen for an event"""
        if event_name not in self._events:
            self._events[event_name] = []
        self._events[event_name].append(callback)
    
    def emit(self, event_name: str, *args, **kwargs) -> None:
        """Emit an event"""
        with self._lock:
            for callback in self._events.get(event_name, []):
                try:
                    callback(*args, **kwargs)
                except Exception as e:
                    print(f"Error in event {event_name}: {e}")
    
    def off(self, event_name: str, callback: Callable) -> None:
        """Remove event listener"""
        if event_name in self._events and callback in self._events[event_name]:
            self._events[event_name].remove(callback)

class NLMSerializer:
    """Custom serialization for NLM objects"""
    
    @staticmethod
    def to_python_dict(obj: Any) -> Dict[str, Any]:
        """Convert a NLM object to a Python dictionary"""
        if hasattr(obj, '__dict__'):
            return obj.__dict__
        elif hasattr(obj, 'to_dict'):
            return obj.to_dict()
        else:
            raise ValueError(f"Object of type {type(obj)} cannot be serialized")
    
    @staticmethod
    def from_python_dict(data: Dict[str, Any], obj_type: str) -> Any:
        """Create a NLM object from a Python dictionary"""
        if obj_type == 'Config':
            config = pynlm.createDefaultConfig()
            for key, value in data.items():
                config.set(key, value)
            return config
        elif obj_type == 'Brain':
            return NLMFactory.create_brain_with_config(data)
        else:
            raise ValueError(f"Cannot deserialize object of type {obj_type}")

class NLMPlugin:
    """Base class for NLM plugins"""
    
    def setup(self, api: Any) -> None:
        """Setup plugin with NLM API"""
        pass
    
    def teardown(self) -> None:
        """Cleanup plugin"""
        pass
    
    def handle_event(self, event_name: str, data: Dict[str, Any]) -> None:
        """Handle an event"""
        pass

# Global instances
_registry = NLMRegistry()
_factory = NLMFactory()
_hook_system = NLMHookSystem()
_dynamic_api = NLMDynamicAPI()
_callback_system = NLMCallbackSystem()
_event_emitter = NLMEventEmitter()
_serializer = NLMSerializer()

# Public API
__all__ = [
    'NLMRegistry',
    'NLMFactory', 
    'NLMHookSystem',
    'NLMDynamicAPI',
    'NLMCallbackSystem',
    'NLMEventEmitter',
    'NLMSerializer',
    'NLMPlugin',
    'register',
    'unregister',
    'create_instance',
    'get_class',
    'list_classes',
    'get_methods',
    'add_hook',
    'call_hooks',
    'add_step_callback',
    'add_spike_callback',
    'add_learning_callback',
    'execute_step_callbacks',
    'execute_spike_callbacks',
    'execute_learning_callbacks',
    'on',
    'emit',
    'to_python_dict',
    'from_python_dict',
]

# Convenience functions for common operations
def register(name: str, factory: Callable) -> None:
    """Register a custom type"""
    _registry.register(name, factory)

def unregister(name: str) -> None:
    """Unregister a custom type"""
    _registry.unregister(name)

def create_instance(name: str, *args, **kwargs) -> Any:
    """Create an instance of a registered type"""
    return _registry.create_instance(name, *args, **kwargs)

def get_class(name: str) -> Any:
    """Get a class by name"""
    return _dynamic_api.get_class(name)

def list_classes() -> List[str]:
    """List all available classes"""
    return _dynamic_api.list_classes()

def get_methods(cls_name: str) -> List[str]:
    """Get methods for a class"""
    return _dynamic_api.get_methods(cls_name)

def add_hook(hook_type: str, callback: Callable) -> None:
    """Add a callback to a hook"""
    _hook_system.add_hook(hook_type, callback)

def call_hooks(hook_type: str, *args, **kwargs) -> List[Any]:
    """Call all callbacks for a hook type"""
    return _hook_system.call_hooks(hook_type, *args, **kwargs)

def add_step_callback(callback: Callable) -> None:
    """Add a callback to be called after each simulation step"""
    _callback_system.add_step_callback(callback)

def add_spike_callback(callback: Callable) -> None:
    """Add a callback to be called when neurons spike"""
    _callback_system.add_spike_callback(callback)

def add_learning_callback(callback: Callable) -> None:
    """Add a callback to be called when learning occurs"""
    _callback_system.add_learning_callback(callback)

def to_python_dict(obj: Any) -> Dict[str, Any]:
    """Convert a NLM object to a Python dictionary"""
    return _serializer.to_python_dict(obj)

def from_python_dict(data: Dict[str, Any], obj_type: str) -> Any:
    """Create a NLM object from a Python dictionary"""
    return _serializer.from_python_dict(data, obj_type)