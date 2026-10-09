import sys
import json
import threading
import time
from typing import Dict, List, Any, Optional, Callable, Set
from enum import Enum
from dataclasses import dataclass
from abc import ABC, abstractmethod
import weakref

class EventType(Enum):
    """Event types for NLM simulation"""
    STEP_COMPLETED = "step_completed"
    SPIKE_DETECTED = "spike_detected"
    LEARNING_OCCURRED = "learning_occurred"
    BRAIN_RESET = "brain_reset"
    BRAIN_SAVED = "brain_saved"
    BRAIN_LOADED = "brain_loaded"
    REGION_ADDED = "region_added"
    CONFIG_CHANGED = "config_changed"
    ERROR_OCCURRED = "error_occurred"
    PLUGIN_LOADED = "plugin_loaded"
    PLUGIN_UNLOADED = "plugin_unloaded"
    CUSTOM = "custom"

@dataclass
class SimulationStepEvent:
    """Data for simulation step event"""
    step_id: int
    current_time: float
    neuron_count: int
    active_neuron_count: int
    spike_count: int
    learning_events: int

@dataclass
class SpikeEvent:
    """Data for spike event"""
    neuron_id: int
    neuron_type: str
    region_id: int
    firing_rate: float
    timestamp: float

@dataclass
class LearningEvent:
    """Data for learning event"""
    event_type: str
    strength: float
    learning_rate: float
    affected_neurons: List[int]
    affected_synapses: List[int]
    timestamp: float

@dataclass
class RegionEvent:
    """Data for region event"""
    region_id: int
    region_name: str
    action: str  # "added" or "modified"
    timestamp: float

@dataclass
class ConfigEvent:
    """Data for config event"""
    key: str
    old_value: Any
    new_value: Any
    timestamp: float

@dataclass
class ErrorEvent:
    """Data for error event"""
    error_type: str
    message: str
    stack_trace: Optional[str]
    timestamp: float

@dataclass
class PluginEvent:
    """Data for plugin event"""
    plugin_name: str
    event_type: str
    data: Dict[str, Any]
    timestamp: float
class Event:
    """Event class for NLM simulation events"""
    
    def __init__(self, event_type: EventType, data: Any):
        self.type = event_type
        self.data = data
        self.timestamp = time.time()
        self._handlers: List[Callable] = []
        self._weak_handlers: List[weakref.ref] = []
        self._lock = threading.RLock()
    
    def add_handler(self, handler: Callable) -> None:
        """Add a handler for this event"""
        with self._lock:
            if inspect.ismethod(handler) and handler.__self__ is not None:
                # For bound methods, use weak reference
                self._weak_handlers.append(weakref.ref(handler))
            else:
                # For regular functions or unbound methods
                self._handlers.append(handler)
    
    def remove_handler(self, handler: Callable) -> None:
        """Remove a handler from this event"""
        with self._lock:
            if handler in self._handlers:
                self._handlers.remove(handler)
            else:
                # Try weak reference
                self._weak_handlers = [
                    h for h in self._weak_handlers 
                    if h() is not None and h() is not handler
                ]
    
    def emit(self) -> None:
        """Emit the event to all handlers"""
        handlers_to_call = []
        
        with self._lock:
            # Collect strong handlers
            handlers_to_call.extend(self._handlers)
            
            # Collect live weak handlers
            live_weak_handlers = []
            for handler_ref in self._weak_handlers:
                handler = handler_ref()
                if handler is not None:
                    live_weak_handlers.append(handler)
            handlers_to_call.extend(live_weak_handlers)
            
            # Update handlers list
            self._weak_handlers = [h for h in self._weak_handlers if h() is not None]
        
        # Call handlers
        for handler in handlers_to_call:
            try:
                handler(self)
            except Exception as e:
                print(f"Error in event handler: {e}")
    
    @property
    def handlers_count(self) -> int:
        """Get the number of handlers for this event"""
        return len(self._handlers) + len(self._weak_handlers)

class EventEmitter:
    """Event emitter for NLM simulation events"""
    
    def __init__(self):
        self._events: Dict[EventType, Event] = {}
        self._custom_events: Dict[str, Event] = {}
        self._lock = threading.RLock()
        self._event_filters = []
    
    def on(self, event_type: EventType, handler: Callable) -> None:
        """Listen for an event"""
        with self._lock:
            if event_type not in self._events:
                self._events[event_type] = Event(event_type, None)
            
            self._events[event_type].add_handler(handler)
    
    def on_custom(self, event_name: str, handler: Callable) -> None:
        """Listen for a custom event"""
        with self._lock:
            if event_name not in self._custom_events:
                self._custom_events[event_name] = Event(EventType.CUSTOM, None)
            
            self._custom_events[event_name].add_handler(handler)
    
    def emit(self, event_type: EventType, data: Any = None) -> None:
        """Emit an event"""
        with self._lock:
            # Check event filters
            if self._event_filters:
                should_emit = True
                for event_filter in self._event_filters:
                    if not event_filter(event_type, data):
                        should_emit = False
                        break
            else:
                should_emit = True
            
            if not should_emit:
                return
            
            # Emit to main event
            if event_type in self._events:
                # Create a new event instance with the data
                event = Event(event_type, data)
                event._handlers = self._events[event_type]._handlers.copy()
                event._weak_handlers = self._events[event_type]._weak_handlers.copy()
                event.emit()
    
    def emit_custom(self, event_name: str, data: Any = None) -> None:
        """Emit a custom event"""
        with self._lock:
            if event_name in self._custom_events:
                event = Event(EventType.CUSTOM, data)
                event._handlers = self._custom_events[event_name]._handlers.copy()
                event._weak_handlers = self._custom_events[event_name]._weak_handlers.copy()
                event.emit()
    
    def off(self, event_type: EventType, handler: Callable) -> None:
        """Remove an event listener"""
        with self._lock:
            if event_type in self._events:
                self._events[event_type].remove_handler(handler)
    
    def off_custom(self, event_name: str, handler: Callable) -> None:
        """Remove a custom event listener"""
        with self._lock:
            if event_name in self._custom_events:
                self._custom_events[event_name].remove_handler(handler)
    
    def add_event_filter(self, event_filter: Callable[[EventType, Any], bool]) -> None:
        """Add an event filter"""
        self._event_filters.append(event_filter)
    
    def remove_event_filter(self, event_filter: Callable[[EventType, Any], bool]) -> None:
        """Remove an event filter"""
        if event_filter in self._event_filters:
            self._event_filters.remove(event_filter)
    
    def get_event_count(self, event_type: EventType) -> int:
        """Get the number of handlers for an event type"""
        with self._lock:
            if event_type in self._events:
                return self._events[event_type].handlers_count
            return 0
    
    def list_events(self) -> List[EventType]:
        """List all registered event types"""
        with self._lock:
            return list(self._events.keys())
    
    def clear_event(self, event_type: EventType) -> None:
        """Clear all handlers for an event type"""
        with self._lock:
            if event_type in self._events:
                self._events[event_type]._handlers.clear()
                self._events[event_type]._weak_handlers.clear()
    
    def clear_all_events(self) -> None:
        """Clear all event handlers"""
        with self._lock:
            self._events.clear()
            self._custom_events.clear()
            self._event_filters.clear()

class NLMEventSystem:
    """Integrated event system for NLM"""
    
    def __init__(self):
        self.emitter = EventEmitter()
        self._brain_callbacks = []
        self._lock = threading.RLock()
    
    def setup_brain_events(self, brain: Any) -> None:
        """Setup event handling for a brain object"""
        # Emit brain loaded event
        self.emit(EventType.BRAIN_LOADED, {'brain': brain})
        
        # Connect brain events to system events
        if hasattr(brain, 'step'):
            # We would normally connect to brain's internal events here
            pass
    
    def add_simulation_event_handlers(self, 
                                     step_callback: Optional[Callable] = None,
                                     spike_callback: Optional[Callable] = None,
                                     learning_callback: Optional[Callable] = None,
                                     error_callback: Optional[Callable] = None) -> None:
        """Add handlers for simulation events"""
        with self._lock:
            if step_callback:
                self._brain_callbacks.append(('step', step_callback))
                self.emitter.on(EventType.STEP_COMPLETED, step_callback)
            
            if spike_callback:
                self._brain_callbacks.append(('spike', spike_callback))
                self.emitter.on(EventType.SPIKE_DETECTED, spike_callback)
            
            if learning_callback:
                self._brain_callbacks.append(('learning', learning_callback))
                self.emitter.on(EventType.LEARNING_OCCURRED, learning_callback)
            
            if error_callback:
                self._brain_callbacks.append(('error', error_callback))
                self.emitter.on(EventType.ERROR_OCCURRED, error_callback)
    
    def remove_simulation_event_handlers(self) -> None:
        """Remove all simulation event handlers"""
        with self._lock:
            for callback_type, callback in self._brain_callbacks:
                event_type_map = {
                    'step': EventType.STEP_COMPLETED,
                    'spike': EventType.SPIKE_DETECTED,
                    'learning': EventType.LEARNING_OCCURRED,
                    'error': EventType.ERROR_OCCURRED,
                }
                self.emitter.off(event_type_map.get(callback_type, EventType.CUSTOM), callback)
            
            self._brain_callbacks.clear()
    
    def emit_step_completed(self, event_data: SimulationStepEvent) -> None:
        """Emit a step completed event"""
        self.emitter.emit(EventType.STEP_COMPLETED, event_data)
    
    def emit_spike_detected(self, event_data: SpikeEvent) -> None:
        """Emit a spike detected event"""
        self.emitter.emit(EventType.SPIKE_DETECTED, event_data)
    
    def emit_learning_occurred(self, event_data: LearningEvent) -> None:
        """Emit a learning occurred event"""
        self.emitter.emit(EventType.LEARNING_OCCURRED, event_data)
    
    def emit_brain_reset(self, brain: Any) -> None:
        """Emit a brain reset event"""
        self.emitter.emit(EventType.BRAIN_RESET, {'brain': brain})
    
    def emit_brain_saved(self, brain: Any, filepath: str) -> None:
        """Emit a brain saved event"""
        self.emitter.emit(EventType.BRAIN_SAVED, {'brain': brain, 'filepath': filepath})
    
    def emit_brain_loaded(self, brain: Any, filepath: str) -> None:
        """Emit a brain loaded event"""
        self.emitter.emit(EventType.BRAIN_LOADED, {'brain': brain, 'filepath': filepath})
    
    def emit_region_added(self, event_data: RegionEvent) -> None:
        """Emit a region added event"""
        self.emitter.emit(EventType.REGION_ADDED, event_data)
    
    def emit_config_changed(self, event_data: ConfigEvent) -> None:
        """Emit a config changed event"""
        self.emitter.emit(EventType.CONFIG_CHANGED, event_data)
    
    def emit_error(self, event_data: ErrorEvent) -> None:
        """Emit an error event"""
        self.emitter.emit(EventType.ERROR_OCCURRED, event_data)
    
    def emit_plugin_event(self, event_data: PluginEvent) -> None:
        """Emit a plugin event"""
        self.emitter.emit_custom(event_data.event_type, event_data)

# Global event system instance
_event_system = NLMEventSystem()

# Public API
__all__ = [
    'EventType',
    'SimulationStepEvent',
    'SpikeEvent', 
    'LearningEvent',
    'RegionEvent',
    'ConfigEvent',
    'ErrorEvent',
    'PluginEvent',
    'Event',
    'EventEmitter',
    'NLMEventSystem',
    'setup_simulation_events',
    'emit_step_completed',
    'emit_spike_detected',
    'emit_learning_occurred',
    'emit_brain_reset',
    'emit_brain_saved',
    'emit_brain_loaded',
    'emit_region_added',
    'emit_config_changed',
    'emit_error',
    'emit_plugin_event',
]

# Convenience functions for emitting events
def setup_simulation_events(
    step_callback: Optional[Callable] = None,
    spike_callback: Optional[Callable] = None,
    learning_callback: Optional[Callable] = None,
    error_callback: Optional[Callable] = None) -> None:
    """Setup event handlers for simulation"""
    _event_system.add_simulation_event_handlers(
        step_callback, spike_callback, learning_callback, error_callback
    )

def emit_step_completed(event_data: SimulationStepEvent) -> None:
    """Emit a step completed event"""
    _event_system.emit_step_completed(event_data)

def emit_spike_detected(event_data: SpikeEvent) -> None:
    """Emit a spike detected event"""
    _event_system.emit_spike_detected(event_data)

def emit_learning_occurred(event_data: LearningEvent) -> None:
    """Emit a learning occurred event"""
    _event_system.emit_learning_occurred(event_data)

def emit_brain_reset(brain: Any) -> None:
    """Emit a brain reset event"""
    _event_system.emit_brain_reset(brain)

def emit_brain_saved(brain: Any, filepath: str) -> None:
    """Emit a brain saved event"""
    _event_system.emit_brain_saved(brain, filepath)

def emit_brain_loaded(brain: Any, filepath: str) -> None:
    """Emit a brain loaded event"""
    _event_system.emit_brain_loaded(brain, filepath)

def emit_region_added(event_data: RegionEvent) -> None:
    """Emit a region added event"""
    _event_system.emit_region_added(event_data)

def emit_config_changed(event_data: ConfigEvent) -> None:
    """Emit a config changed event"""
    _event_system.emit_config_changed(event_data)

def emit_error(event_data: ErrorEvent) -> None:
    """Emit an error event"""
    _event_system.emit_error(event_data)

def emit_plugin_event(event_data: PluginEvent) -> None:
    """Emit a plugin event"""
    _event_system.emit_plugin_event(event_data)