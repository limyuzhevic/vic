pynlm/event_system.py
Name: Event system module
Summary: Event-driven programming with callbacks and event handling

from pynlm.interface import ScriptingEvent, EventType, ScriptingInterface
import time
import threading
from typing import Dict, List, Any, Optional, Callable
from dataclasses import dataclass, field
from enum import Enum
import uuid


class EventFilter:
    """Event filter for filtering events"""
    
    def __init__(self, event_type: EventType = None, 
                 source_filter: Optional[Callable] = None,
                 data_filter: Optional[Callable] = None,
                 metadata_filter: Optional[Callable] = None):
        self.event_type = event_type
        self.source_filter = source_filter
        self.data_filter = data_filter
        self.metadata_filter = metadata_filter
    
    def matches(self, event: ScriptingEvent) -> bool:
        """Check if event matches filter criteria"""
        if self.event_type and event.event_type != self.event_type:
            return False
        
        if self.source_filter and not self.source_filter(event.source):
            return False
        
        if self.data_filter and not self.data_filter(event.data):
            return False
        
        if self.metadata_filter and not self.metadata_filter(event.metadata):
            return False
        
        return True


class EventEmitter:
    """Event emitter for emitting events"""
    
    def __init__(self, name: str, interface: ScriptingInterface):
        self.name = name
        self.interface = interface
        self.context = interface.context
        self.handlers: List[Callable] = []
        self.filters: List[EventFilter] = []
        self.priority_handlers: Dict[int, List[Callable]] = {}
        self.delegated_events: Dict[EventType, List[str]] = {}
        self.handler_counts: Dict[Callable, int] = {}
        self.async_handlers = False
    
    def add_handler(self, handler: Callable, priority: int = 0,
                   async_handler: bool = False):
        """Add event handler"""
        if async_handler:
            self.handlers.append((handler, priority, async_handler))
        else:
            self.handlers.append((handler, priority, async_handler))
        
        self.handler_counts[handler] = self.handler_counts.get(handler, 0) + 1
        
        # Sort handlers by priority
        self.handlers.sort(key=lambda x: x[1], reverse=True)
    
    def remove_handler(self, handler: Callable):
        """Remove event handler"""
        # Remove from handlers list
        self.handlers = [(h, p, a) for h, p, a in self.handlers if h != handler]
        
        # Update handler counts
        if handler in self.handler_counts:
            self.handler_counts[handler] -= 1
            if self.handler_counts[handler] <= 0:
                del self.handler_counts[handler]
    
    def add_filter(self, event_type: EventType = None,
                   source_filter: Optional[Callable] = None,
                   data_filter: Optional[Callable] = None,
                   metadata_filter: Optional[Callable] = None):
        """Add event filter"""
        filter_obj = EventFilter(event_type, source_filter, data_filter, metadata_filter)
        self.filters.append(filter_obj)
    
    def remove_filter(self, filter_obj: EventFilter):
        """Remove event filter"""
        if filter_obj in self.filters:
            self.filters.remove(filter_obj)
    
    def emit(self, event: ScriptingEvent):
        """Emit event to all handlers"""
        # Apply filters
        for filter_obj in self.filters:
            if not filter_obj.matches(event):
                return
        
        # Call handlers
        for handler, priority, async_handler in self.handlers:
            try:
                if async_handler:
                    # Schedule async handler
                    asyncio.create_task(self._call_handler(handler, event))
                else:
                    self._call_handler(handler, event)
            except Exception as e:
                logger = __import__('logging').getLogger(__name__)
                logger.error(f"Event handler error in {self.name}: {e}")
    
    def emit_immediate(self, event: ScriptingEvent):
        """Emit event immediately (synchronous)"""
        # Apply filters
        for filter_obj in self.filters:
            if not filter_obj.matches(event):
                return
        
        # Call handlers synchronously
        for handler, priority, async_handler in self.handlers:
            try:
                self._call_handler(handler, event)
            except Exception as e:
                logger = __import__('logging').getLogger(__name__)
                logger.error(f"Event handler error in {self.name}: {e}")
    
    def _call_handler(self, handler: Callable, event: ScriptingEvent):
        """Call event handler"""
        if handler.__code__.co_argcount > 1:
            handler(event)
        else:
            handler()


class EventBus:
    """Event bus for managing event emitters and event routing"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.emitters: Dict[str, EventEmitter] = {}
        self.global_filters: List[EventFilter] = []
        self.event_history: List[ScriptingEvent] = []
        self.history_limit = 1000
        self.event_statistics: Dict[str, Any] = {}
        self.subscribers: Dict[EventType, List[EventEmitter]] = {}
        self.delegation_rules: Dict[EventType, List[Dict[str, Any]]] = {}
    
    def create_emitter(self, name: str) -> EventEmitter:
        """Create a new event emitter"""
        emitter = EventEmitter(name, self.interface)
        self.emitters[name] = emitter
        return emitter
    
    def get_emitter(self, name: str) -> Optional[EventEmitter]:
        """Get an event emitter"""
        return self.emitters.get(name)
    
    def remove_emitter(self, name: str) -> bool:
        """Remove an event emitter"""
        if name in self.emitters:
            del self.emitters[name]
            
            # Remove from subscribers
            for event_type, emitters in self.subscribers.items():
                self.subscribers[event_type] = [e for e in emitters if e.name != name]
            
            return True
        return False
    
    def list_emitters(self) -> List[str]:
        """List all emitter names"""
        return list(self.emitters.keys())
    
    def add_global_filter(self, filter_obj: EventFilter):
        """Add global filter for all events"""
        self.global_filters.append(filter_obj)
    
    def remove_global_filter(self, filter_obj: EventFilter):
        """Remove global filter"""
        if filter_obj in self.global_filters:
            self.global_filters.remove(filter_obj)
    
    def subscribe_to_event(self, event_type: EventType, emitter_name: str):
        """Subscribe an emitter to a specific event type"""
        emitter = self.get_emitter(emitter_name)
        if not emitter:
            return False
        
        if event_type not in self.subscribers:
            self.subscribers[event_type] = []
        
        if emitter not in self.subscribers[event_type]:
            self.subscribers[event_type].append(emitter)
        
        return True
    
    def unsubscribe_from_event(self, event_type: EventType, emitter_name: str):
        """Unsubscribe an emitter from an event type"""
        if event_type in self.subscribers:
            self.subscribers[event_type] = [
                e for e in self.subscribers[event_type] if e.name != emitter_name
            ]
    
    def set_delegation_rule(self, event_type: EventType, target_emitter: str,
                          conditions: Optional[Dict[str, Any]] = None):
        """Set delegation rule for event type"""
        if event_type not in self.delegation_rules:
            self.delegation_rules[event_type] = []
        
        self.delegation_rules[event_type].append({
            'target': target_emitter,
            'conditions': conditions or {}
        })
    
    def remove_delegation_rule(self, event_type: EventType, target_emitter: str):
        """Remove delegation rule"""
        if event_type in self.delegation_rules:
            self.delegation_rules[event_type] = [
                rule for rule in self.delegation_rules[event_type]
                if rule['target'] != target_emitter
            ]
    
    def emit_event(self, event: ScriptingEvent):
        """Emit event through the bus"""
        # Apply global filters
        for filter_obj in self.global_filters:
            if not filter_obj.matches(event):
                return
        
        # Add to event history
        self.event_history.append(event)
        
        # Update statistics
        self._update_event_statistics(event)
        
        # Emit to all emitters
        for emitter in self.emitters.values():
            if self._emitter_should_receive_event(emitter, event):
                emitter.emit(event)
        
        # Handle delegation
        self._handle_delegation(event)
    
    def emit_event_immediate(self, event: ScriptingEvent):
        """Emit event immediately through the bus"""
        # Apply global filters
        for filter_obj in self.global_filters:
            if not filter_obj.matches(event):
                return
        
        # Add to event history
        self.event_history.append(event)
        
        # Update statistics
        self._update_event_statistics(event)
        
        # Emit to all emitters synchronously
        for emitter in self.emitters.values():
            if self._emitter_should_receive_event(emitter, event):
                emitter.emit_immediate(event)
        
        # Handle delegation
        self._handle_delegation(event)
    
    def get_event_history(self, limit: Optional[int] = None) -> List[ScriptingEvent]:
        """Get event history"""
        if limit is not None:
            return self.event_history[-limit:]
        return self.event_history.copy()
    
    def clear_event_history(self):
        """Clear event history"""
        self.event_history.clear()
    
    def get_event_statistics(self) -> Dict[str, Any]:
        """Get event statistics"""
        return self.event_statistics.copy()
    
    def get_events_by_type(self, event_type: EventType, 
                         limit: Optional[int] = None) -> List[ScriptingEvent]:
        """Get events by type"""
        events = [e for e in self.event_history if e.event_type == event_type]
        
        if limit is not None:
            events = events[-limit:]
        
        return events
    
    def _emitter_should_receive_event(self, emitter: EventEmitter, 
                                    event: ScriptingEvent) -> bool:
        """Check if emitter should receive event"""
        # Check if emitter subscribes to this event type
        if event.event_type in self.subscribers:
            if emitter in self.subscribers[event.event_type]:
                return True
        
        # Check emitter's own filters
        for filter_obj in emitter.filters:
            if filter_obj.matches(event):
                return True
        
        # Default: all emitters receive all events
        return True
    
    def _handle_delegation(self, event: ScriptingEvent):
        """Handle event delegation"""
        if event.event_type in self.delegation_rules:
            for rule in self.delegation_rules[event.event_type]:
                target_name = rule['target']
                conditions = rule.get('conditions', {})
                
                # Check conditions
                if self._check_delegation_conditions(event, conditions):
                    target_emitter = self.get_emitter(target_name)
                    if target_emitter:
                        target_emitter.emit(event)
    
    def _check_delegation_conditions(self, event: ScriptingEvent,
                                    conditions: Dict[str, Any]) -> bool:
        """Check if delegation conditions are met"""
        for key, value in conditions.items():
            if key == 'event_type':
                if value != event.event_type:
                    return False
            elif key == 'source':
                if value != event.source:
                    return False
            elif key == 'data_key':
                if key not in event.data:
                    return False
                if value != event.data[key]:
                    return False
            elif key == 'metadata_key':
                if key not in event.metadata:
                    return False
                if value != event.metadata[key]:
                    return False
        
        return True
    
    def _update_event_statistics(self, event: ScriptingEvent):
        """Update event statistics"""
        event_type = event.event_type.value
        
        if event_type not in self.event_statistics:
            self.event_statistics[event_type] = {
                'count': 0,
                'sources': {},
                'timestamps': []
            }
        
        stats = self.event_statistics[event_type]
        stats['count'] += 1
        
        source_name = str(event.source) if event.source else 'unknown'
        stats['sources'][source_name] = stats['sources'].get(source_name, 0) + 1
        stats['timestamps'].append(event.timestamp)
        
        # Keep only recent timestamps
        if len(stats['timestamps']) > 100:
            stats['timestamps'] = stats['timestamps'][-100:]


class EventDrivenInterface:
    """Event-driven interface for NLM brain control"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.event_bus = EventBus(interface)
        self.monitoring_enabled = False
        self.monitoring_thread = None
        self.simulation_subscriptions = {}
        self.brain_state_subscriptions = []
    
    def setup_monitoring(self, interval: float = 1.0,
                        metrics: Optional[List[str]] = None):
        """Setup event monitoring"""
        if metrics is None:
            metrics = ['brain_state', 'simulation_events', 'neuron_activity']
        
        self.monitoring_enabled = True
        
        # Create default emitters
        brain_emitter = self.event_bus.create_emitter('brain')
        simulation_emitter = self.event_bus.create_emitter('simulation')
        monitoring_emitter = self.event_bus.create_emitter('monitoring')
        
        # Subscribe emitters to events
        self.event_bus.subscribe_to_event(EventType.STATE_CHANGED, 'brain')
        self.event_bus.subscribe_to_event(EventType.SIMULATION_STARTED, 'simulation')
        self.event_bus.subscribe_to_event(EventType.SIMULATION_STOPPED, 'simulation')
        
        # Start monitoring thread
        self.monitoring_thread = threading.Thread(
            target=self._monitoring_loop,
            daemon=True,
            args=(interval, metrics)
        )
        self.monitoring_thread.start()
    
    def stop_monitoring(self):
        """Stop event monitoring"""
        self.monitoring_enabled = False
        
        if self.monitoring_thread:
            self.monitoring_thread.join(timeout=5.0)
            self.monitoring_thread = None
    
    def subscribe_to_simulation_events(self, callback: Callable,
                                      event_types: Optional[List[EventType]] = None,
                                      emitter_name: str = 'simulation'):
        """Subscribe to simulation events"""
        emitter = self.event_bus.get_emitter(emitter_name)
        if not emitter:
            return False
        
        if event_types is None:
            event_types = [
                EventType.SIMULATION_STARTED,
                EventType.SIMULATION_STOPPED,
                EventType.STATE_CHANGED,
                EventType.ACTION_PRODUCED
            ]
        
        for event_type in event_types:
            emitter.add_handler(callback)
        
        self.simulation_subscriptions[emitter_name] = {
            'callback': callback,
            'event_types': event_types
        }
        
        return True
    
    def subscribe_to_brain_state_changes(self, callback: Callable,
                                        event_types: Optional[List[EventType]] = None):
        """Subscribe to brain state changes"""
        if event_types is None:
            event_types = [
                EventType.STATE_CHANGED,
                EventType.NEURON_FIRED,
                EventType.ACTION_PRODUCED
            ]
        
        for event_type in event_types:
            self.interface.context.emit_event(
                ScriptingEvent(event_type, time.time(), None, {})
            )
        
        self.brain_state_subscriptions.append({
            'callback': callback,
            'event_types': event_types
        })
    
    def emit_state_change_event(self, source: Any, data: Dict[str, Any],
                              metadata: Optional[Dict[str, Any]] = None):
        """Emit state change event"""
        event = ScriptingEvent(
            EventType.STATE_CHANGED,
            time.time(),
            source,
            data,
            metadata
        )
        
        self.event_bus.emit_event(event)
    
    def emit_simulation_event(self, event_type: EventType, source: Any,
                            data: Dict[str, Any], metadata: Optional[Dict[str, Any]] = None):
        """Emit simulation event"""
        event = ScriptingEvent(event_type, time.time(), source, data, metadata)
        
        self.event_bus.emit_event(event)
    
    def get_event_statistics(self) -> Dict[str, Any]:
        """Get event statistics"""
        return self.event_bus.get_event_statistics()
    
    def get_event_history(self, limit: Optional[int] = None) -> List[ScriptingEvent]:
        """Get event history"""
        return self.event_bus.get_event_history(limit)
    
    def _monitoring_loop(self, interval: float, metrics: List[str]):
        """Internal monitoring loop"""
        while self.monitoring_enabled:
            try:
                # Collect metrics
                metrics_data = self._collect_metrics(metrics)
                
                # Emit monitoring event
                event = ScriptingEvent(
                    EventType.STATE_CHANGED,
                    time.time(),
                    'monitoring',
                    {'metrics': metrics_data}
                )
                
                self.event_bus.emit_event(event)
                
                time.sleep(interval)
                
            except Exception as e:
                logger = __import__('logging').getLogger(__name__)
                logger.error(f"Monitoring loop error: {e}")
                time.sleep(1.0)
    
    def _collect_metrics(self, metrics: List[str]) -> Dict[str, Any]:
        """Collect monitoring metrics"""
        collected = {}
        
        if 'brain_state' in metrics and self.context.brain:
            collected['brain_state'] = self.interface.get_state()
        
        if 'simulation_events' in metrics:
            collected['simulation_events'] = len(self.event_bus.get_events_by_type(
                EventType.SIMULATION_STARTED
            ))
        
        if 'neuron_activity' in metrics and self.context.brain:
            regions = self.context.brain.getRegions()
            neuron_activity = {}
            
            for region in regions:
                region_id = region.getId()
                neuron_count = 0
                active_count = 0
                
                for neuron in region.getNeurons():
                    neuron_count += 1
                    if neuron.getFiringState() == 'Active':
                        active_count += 1
                
                neuron_activity[region_id] = {
                    'neuron_count': neuron_count,
                    'active_count': active_count,
                    'activity_rate': active_count / max(neuron_count, 1)
                }
            
            collected['neuron_activity'] = neuron_activity
        
        return collected


# Export key classes and functions
__all__ = [
    'EventFilter',
    'EventEmitter',
    'EventBus',
    'EventDrivenInterface'
]
