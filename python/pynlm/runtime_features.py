# Plugin Runtime Features

This module implements the runtime features for plugins, including hot-plugging, state management, event systems, coordination, and monitoring.

import os
import json
import time
import threading
import pickle
import shutil
from typing import Dict, List, Any, Optional, Callable, Type, Set, Union
from dataclasses import dataclass, field
from enum import Enum
from abc import ABC, abstractmethod

from .plugin_system import Plugin, PluginInfo, PluginType, PluginStatus
from .core_plugin_framework import EnhancedPluginManager, SecurityManager


class RuntimeFeatureError(Exception):
    """Exception raised during runtime feature operations"""
    pass


class HotplugState(Enum):
    """Hotplug state"""
    IDLE = "idle"
    LOADING = "loading"
    UNLOADING = "unloading"
    RELOADING = "reloading"
    ERROR = "error"


class PluginState(Enum):
    """Plugin runtime state"""
    INITIALIZING = "initializing"
    RUNNING = "running"
    PAUSED = "paused"
    STOPPING = "stopping"
    STOPPED = "stopped"
    ERROR = "error"


@dataclass
class PluginRuntimeInfo:
    """Runtime information for a plugin"""
    plugin_name: str
    state: PluginState
    start_time: float
    last_activity: float
    memory_usage: int
    cpu_usage: float
    event_count: int
    load_count: int
    unload_count: int
    error_count: int
    performance_metrics: Dict[str, Any]
    dependencies: List[str]
    dependents: List[str]
    sandbox_id: str
    config: Dict[str, Any]


class HotPlugManager:
    """Manager for hot-plugging plugins"""
    
    def __init__(self, plugin_manager: EnhancedPluginManager):
        self.plugin_manager = plugin_manager
        self.plugin_states: Dict[str, PluginRuntimeInfo] = {}
        self.hotplug_state = HotplugState.IDLE
        self.hotplug_lock = threading.Lock()
        self.reload_queue = []
        self.state_change_callbacks: Dict[str, Callable] = {}
        
    def load_plugin(self, plugin_name: str, plugin_path: Optional[str] = None,
                   config: Optional[Dict[str, Any]] = None) -> bool:
        """Load a plugin with hot-plugging support"""
        with self.hotplug_lock:
            if self.hotplug_state != HotplugState.IDLE:
                raise RuntimeError(f"Hotplug manager not idle: {self.hotplug_state}")
            
            self.hotplug_state = HotplugState.LOADING
            
            try:
                # Create runtime info
                runtime_info = PluginRuntimeInfo(
                    plugin_name=plugin_name,
                    state=PluginState.INITIALIZING,
                    start_time=time.time(),
                    last_activity=time.time(),
                    memory_usage=0,
                    cpu_usage=0.0,
                    event_count=0,
                    load_count=0,
                    unload_count=0,
                    error_count=0,
                    performance_metrics={},
                    dependencies=[],
                    dependents=[],
                    sandbox_id=f"sandbox_{plugin_name}_{int(time.time())}",
                    config=config or {}
                )
                
                # Update plugin states
                self.plugin_states[plugin_name] = runtime_info
                
                # Load plugin
                plugin = self.plugin_manager.load_plugin(plugin_name, plugin_path, config)
                
                # Update runtime info
                runtime_info.state = PluginState.RUNNING
                runtime_info.last_activity = time.time()
                runtime_info.load_count += 1
                runtime_info.memory_usage = self._estimate_memory_usage(plugin)
                runtime_info.cpu_usage = self._measure_cpu_usage(plugin)
                
                # Notify state change callbacks
                self._notify_state_change(plugin_name, PluginState.RUNNING)
                
                return True
                
            except Exception as e:
                runtime_info.state = PluginState.ERROR
                runtime_info.error_count += 1
                self._notify_state_change(plugin_name, PluginState.ERROR)
                raise RuntimeFeatureError(f"Failed to load plugin {plugin_name}: {e}")
                
            finally:
                self.hotplug_state = HotplugState.IDLE
    
    def unload_plugin(self, plugin_name: str) -> bool:
        """Unload a plugin with hot-plugging support"""
        with self.hotplug_lock:
            if self.hotplug_state != HotplugState.IDLE:
                raise RuntimeError(f"Hotplug manager not idle: {self.hotplug_state}")
            
            self.hotplug_state = HotplugState.UNLOADING
            
            try:
                # Check if plugin is loaded
                if plugin_name not in self.plugin_states:
                    return False
                
                runtime_info = self.plugin_states[plugin_name]
                
                # Update runtime info
                runtime_info.state = PluginState.STOPPING
                runtime_info.last_activity = time.time()
                
                # Notify state change callbacks
                self._notify_state_change(plugin_name, PluginState.STOPPING)
                
                # Unload plugin
                success = self.plugin_manager.unload_plugin(plugin_name)
                
                if success:
                    # Update runtime info
                    runtime_info.state = PluginState.STOPPED
                    runtime_info.unload_count += 1
                    
                    # Remove from plugin states
                    del self.plugin_states[plugin_name]
                    
                    # Notify state change callbacks
                    self._notify_state_change(plugin_name, PluginState.STOPPED)
                    
                    return True
                else:
                    runtime_info.state = PluginState.ERROR
                    runtime_info.error_count += 1
                    self._notify_state_change(plugin_name, PluginState.ERROR)
                    return False
                    
            except Exception as e:
                if plugin_name in self.plugin_states:
                    self.plugin_states[plugin_name].state = PluginState.ERROR
                    self.plugin_states[plugin_name].error_count += 1
                    self._notify_state_change(plugin_name, PluginState.ERROR)
                raise RuntimeFeatureError(f"Failed to unload plugin {plugin_name}: {e}")
                
            finally:
                self.hotplug_state = HotplugState.IDLE
    
    def reload_plugin(self, plugin_name: str, plugin_path: Optional[str] = None) -> bool:
        """Reload a plugin with hot-plugging support"""
        with self.hotplug_lock:
            if self.hotplug_state != HotplugState.IDLE:
                raise RuntimeError(f"Hotplug manager not idle: {self.hotplug_state}")
            
            self.hotplug_state = HotplugState.RELOADING
            
            try:
                # Check if plugin is loaded
                if plugin_name not in self.plugin_states:
                    return False
                
                runtime_info = self.plugin_states[plugin_name]
                
                # Update runtime info
                runtime_info.state = PluginState.STOPPING
                runtime_info.last_activity = time.time()
                
                # Notify state change callbacks
                self._notify_state_change(plugin_name, PluginState.STOPPING)
                
                # Unload plugin
                self.plugin_manager.unload_plugin(plugin_name)
                
                # Wait for cleanup
                time.sleep(0.1)
                
                # Reload plugin
                plugin = self.plugin_manager.load_plugin(plugin_name, plugin_path)
                
                # Update runtime info
                runtime_info.state = PluginState.RUNNING
                runtime_info.last_activity = time.time()
                runtime_info.load_count += 1
                runtime_info.memory_usage = self._estimate_memory_usage(plugin)
                runtime_info.cpu_usage = self._measure_cpu_usage(plugin)
                
                # Notify state change callbacks
                self._notify_state_change(plugin_name, PluginState.RUNNING)
                
                return True
                
            except Exception as e:
                if plugin_name in self.plugin_states:
                    self.plugin_states[plugin_name].state = PluginState.ERROR
                    self.plugin_states[plugin_name].error_count += 1
                    self._notify_state_change(plugin_name, PluginState.ERROR)
                raise RuntimeFeatureError(f"Failed to reload plugin {plugin_name}: {e}")
                
            finally:
                self.hotplug_state = HotplugState.IDLE
    
    def get_plugin_state(self, plugin_name: str) -> Optional[PluginRuntimeInfo]:
        """Get plugin runtime state"""
        return self.plugin_states.get(plugin_name)
    
    def get_all_plugin_states(self) -> Dict[str, PluginRuntimeInfo]:
        """Get all plugin runtime states"""
        return self.plugin_states.copy()
    
    def register_state_change_callback(self, plugin_name: str, callback: Callable):
        """Register plugin state change callback"""
        self.state_change_callbacks[plugin_name] = callback
        
    def unregister_state_change_callback(self, plugin_name: str):
        """Unregister plugin state change callback"""
        if plugin_name in self.state_change_callbacks:
            del self.state_change_callbacks[plugin_name]
    
    def _notify_state_change(self, plugin_name: str, new_state: PluginState):
        """Notify state change callbacks"""
        if plugin_name in self.state_change_callbacks:
            try:
                callback = self.state_change_callbacks[plugin_name]
                callback(plugin_name, new_state)
            except Exception as e:
                logging.error(f"State change callback error: {e}")
    
    def _estimate_memory_usage(self, plugin: Plugin) -> int:
        """Estimate plugin memory usage"""
        # In a real implementation, this would measure actual memory usage
        # For now, return a reasonable estimate
        return 1024  # 1KB
    
    def _measure_cpu_usage(self, plugin: Plugin) -> float:
        """Measure plugin CPU usage"""
        # In a real implementation, this would measure actual CPU usage
        # For now, return a reasonable estimate
        return 0.0


class StateManager:
    """Manager for plugin state and persistence"""
    
    def __init__(self, plugin_manager: EnhancedPluginManager):
        self.plugin_manager = plugin_manager
        self.state_store: Dict[str, Dict[str, Any]] = {}
        self.checkpoint_store: Dict[str, Dict[str, Any]] = {}
        self.state_lock = threading.Lock()
        self.auto_save_enabled = True
        self.auto_save_interval = 300  # 5 minutes
        self.last_save_times: Dict[str, float] = {}
        
    def save_plugin_state(self, plugin_name: str) -> bool:
        """Save plugin state"""
        with self.state_lock:
            if plugin_name not in self.plugin_manager.plugins:
                return False
            
            plugin = self.plugin_manager.plugins[plugin_name]
            
            # Create state
            state = {
                'plugin_name': plugin_name,
                'timestamp': time.time(),
                'plugin_info': plugin.info.__dict__ if hasattr(plugin.info, '__dict__') else {},
                'config': plugin.config,
                'enabled': plugin.enabled,
                'initialized': plugin.initialized,
                'event_handlers': plugin.event_handlers,
                'additional_state': self._get_plugin_additional_state(plugin)
            }
            
            # Save state
            self.state_store[plugin_name] = state
            self.last_save_times[plugin_name] = time.time()
            
            # Create checkpoint
            self._create_checkpoint(plugin_name, state)
            
            return True
    
    def load_plugin_state(self, plugin_name: str) -> Optional[Dict[str, Any]]:
        """Load plugin state"""
        with self.state_lock:
            return self.state_store.get(plugin_name)
    
    def restore_plugin_state(self, plugin_name: str, state: Dict[str, Any]) -> bool:
        """Restore plugin state"""
        with self.state_lock:
            if plugin_name not in self.plugin_manager.plugins:
                return False
            
            plugin = self.plugin_manager.plugins[plugin_name]
            
            # Restore state
            if 'config' in state:
                plugin.config = state['config']
            
            if 'enabled' in state:
                plugin.enabled = state['enabled']
            
            if 'initialized' in state:
                plugin.initialized = state['initialized']
            
            # Restore additional state
            self._restore_plugin_additional_state(plugin, state.get('additional_state', {}))
            
            return True
    
    def create_checkpoint(self, plugin_name: str) -> str:
        """Create checkpoint for plugin"""
        with self.state_lock:
            if plugin_name not in self.plugin_manager.plugins:
                raise RuntimeFeatureError(f"Plugin not loaded: {plugin_name}")
            
            # Get current state
            state = self._get_plugin_state_for_checkpoint(plugin_name)
            
            # Create checkpoint ID
            checkpoint_id = f"{plugin_name}_{int(time.time())}"
            
            # Save checkpoint
            self.checkpoint_store[checkpoint_id] = state
            
            return checkpoint_id
    
    def restore_checkpoint(self, checkpoint_id: str) -> bool:
        """Restore from checkpoint"""
        with self.state_lock:
            if checkpoint_id not in self.checkpoint_store:
                return False
            
            checkpoint = self.checkpoint_store[checkpoint_id]
            plugin_name = checkpoint.get('plugin_name')
            
            if plugin_name and plugin_name in self.plugin_manager.plugins:
                return self.restore_plugin_state(plugin_name, checkpoint)
            
            return False
    
    def cleanup_old_checkpoints(self, max_checkpoints: int = 10):
        """Clean up old checkpoints"""
        with self.state_lock:
            if len(self.checkpoint_store) <= max_checkpoints:
                return
            
            # Sort checkpoints by timestamp
            sorted_checkpoints = sorted(
                self.checkpoint_store.items(),
                key=lambda x: x[1].get('timestamp', 0)
            )
            
            # Remove oldest checkpoints
            for checkpoint_id, _ in sorted_checkpoints[:-max_checkpoints]:
                del self.checkpoint_store[checkpoint_id]
    
    def enable_auto_save(self, enabled: bool = True, interval: int = None):
        """Enable/disable auto-save"""
        self.auto_save_enabled = enabled
        if interval:
            self.auto_save_interval = interval
            
    def set_auto_save_interval(self, interval: int):
        """Set auto-save interval"""
        self.auto_save_interval = interval
        
    def _create_checkpoint(self, plugin_name: str, state: Dict[str, Any]):
        """Create checkpoint for plugin"""
        checkpoint_id = f"{plugin_name}_{int(time.time())}"
        self.checkpoint_store[checkpoint_id] = state
        
    def _get_plugin_state_for_checkpoint(self, plugin_name: str) -> Dict[str, Any]:
        """Get plugin state for checkpoint"""
        if plugin_name not in self.plugin_manager.plugins:
            raise RuntimeFeatureError(f"Plugin not loaded: {plugin_name}")
        
        plugin = self.plugin_manager.plugins[plugin_name]
        
        return {
            'plugin_name': plugin_name,
            'timestamp': time.time(),
            'plugin_info': plugin.info.__dict__ if hasattr(plugin.info, '__dict__') else {},
            'config': plugin.config,
            'enabled': plugin.enabled,
            'initialized': plugin.initialized,
            'event_handlers': plugin.event_handlers,
            'additional_state': self._get_plugin_additional_state(plugin)
        }
        
    def _get_plugin_additional_state(self, plugin: Plugin) -> Dict[str, Any]:
        """Get additional plugin state"""
        # Override in subclasses to add plugin-specific state
        return {}
        
    def _restore_plugin_additional_state(self, plugin: Plugin, state: Dict[str, Any]):
        """Restore additional plugin state"""
        # Override in subclasses to restore plugin-specific state
        pass


class EventSystem:
    """Event system for plugin communication"""
    
    def __init__(self, plugin_manager: EnhancedPluginManager):
        self.plugin_manager = plugin_manager
        self.event_bus = EventBus()
        self.event_handlers: Dict[str, List[Callable]] = {}
        self.event_filters: Dict[str, Callable] = {}
        self.event_subscribers: Dict[str, Set[str]] = {}
        self.event_queue: List[Dict[str, Any]] = []
        self.event_lock = threading.Lock()
        
    def subscribe(self, event_type: str, handler: Callable, plugin_name: str = None):
        """Subscribe to events"""
        with self.event_lock:
            if event_type not in self.event_handlers:
                self.event_handlers[event_type] = []
            
            if plugin_name:
                if event_type not in self.event_subscribers:
                    self.event_subscribers[event_type] = set()
                self.event_subscribers[event_type].add(plugin_name)
                self.event_handlers[event_type].append((plugin_name, handler))
            else:
                # Subscribe to all plugins
                self.event_handlers[event_type].append((None, handler))
        
    def unsubscribe(self, event_type: str, plugin_name: str = None, handler: Callable = None):
        """Unsubscribe from events"""
        with self.event_lock:
            if event_type not in self.event_handlers:
                return
            
            if plugin_name:
                # Remove handlers for specific plugin
                self.event_handlers[event_type] = [
                    (pn, h) for pn, h in self.event_handlers[event_type]
                    if pn != plugin_name and (handler is None or h != handler)
                ]
                
                # Remove from subscribers
                if event_type in self.event_subscribers:
                    self.event_subscribers[event_type].discard(plugin_name)
            else:
                # Remove handlers for all plugins
                if handler:
                    self.event_handlers[event_type] = [
                        (pn, h) for pn, h in self.event_handlers[event_type]
                        if h != handler
                    ]
                else:
                    del self.event_handlers[event_type]
                    
                # Remove from subscribers
                if event_type in self.event_subscribers:
                    del self.event_subscribers[event_type]
    
    def emit(self, event_type: str, data: Dict[str, Any], target_plugins: List[str] = None):
        """Emit event"""
        with self.event_lock:
            # Create event
            event = {
                'type': event_type,
                'data': data,
                'timestamp': time.time(),
                'source': 'system'
            }
            
            # Add to queue
            self.event_queue.append(event)
            
            # Process queue
            self._process_event_queue()
    
    def get_events(self, event_type: str = None, plugin_name: str = None,
                  since: Optional[float] = None) -> List[Dict[str, Any]]:
        """Get events"""
        with self.event_lock:
            events = []
            
            for event in self.event_queue:
                # Filter by event type
                if event_type and event['type'] != event_type:
                    continue
                
                # Filter by plugin name
                if plugin_name and event.get('plugin_name') != plugin_name:
                    continue
                
                # Filter by timestamp
                if since and event['timestamp'] < since:
                    continue
                
                events.append(event)
            
            return events
    
    def clear_events(self, event_type: str = None):
        """Clear events"""
        with self.event_lock:
            if event_type:
                self.event_queue = [
                    e for e in self.event_queue if e['type'] != event_type
                ]
            else:
                self.event_queue = []
    
    def _process_event_queue(self):
        """Process event queue"""
        # Process events in batches
        batch_size = 10
        processed = 0
        
        while processed < batch_size and self.event_queue:
            event = self.event_queue.pop(0)
            self._process_event(event)
            processed += 1
    
    def _process_event(self, event: Dict[str, Any]):
        """Process event"""
        event_type = event['type']
        data = event['data']
        
        # Find matching handlers
        handlers = []
        for pn, handler in self.event_handlers.get(event_type, []):
            handlers.append(handler)
        
        # Apply filters
        filtered_handlers = []
        for handler in handlers:
            if self._passes_filters(event_type, data):
                filtered_handlers.append(handler)
        
        # Call handlers
        for handler in filtered_handlers:
            try:
                handler(event)
            except Exception as e:
                logging.error(f"Event handler error: {e}")
    
    def _passes_filters(self, event_type: str, data: Dict[str, Any]) -> bool:
        """Check if event passes filters"""
        for event_type_filter, filter_func in self.event_filters.items():
            if event_type == event_type_filter and not filter_func(data):
                return False
        return True


class EventBus:
    """Event bus for plugin communication"""
    
    def __init__(self):
        self.subscribers: Dict[str, List[Callable]] = {}
        self.event_history: List[Dict[str, Any]] = []
        self.max_history = 1000
        
    def subscribe(self, event_type: str, handler: Callable):
        """Subscribe to events"""
        if event_type not in self.subscribers:
            self.subscribers[event_type] = []
        
        self.subscribers[event_type].append(handler)
    
    def unsubscribe(self, event_type: str, handler: Callable):
        """Unsubscribe from events"""
        if event_type in self.subscribers:
            if handler in self.subscribers[event_type]:
                self.subscribers[event_type].remove(handler)
    
    def publish(self, event: Dict[str, Any], target_plugins: List[str] = None):
        """Publish event"""
        event_type = event['type']
        
        # Get target subscribers
        subscribers = []
        if target_plugins:
            for subscriber in self.subscribers.get(event_type, []):
                # Check if subscriber belongs to target plugin
                # In a real implementation, this would check plugin ownership
                subscribers.append(subscriber)
        else:
            subscribers = self.subscribers.get(event_type, [])
        
        # Publish to subscribers
        for handler in subscribers:
            try:
                handler(event)
            except Exception as e:
                logging.error(f"Event handler error: {e}")
        
        # Add to history
        self.event_history.append(event)
        if len(self.event_history) > self.max_history:
            self.event_history.pop(0)


class CoordinationManager:
    """Manager for plugin coordination and lifecycle"""
    
    def __init__(self, plugin_manager: EnhancedPluginManager):
        self.plugin_manager = plugin_manager
        self.coordination_graph: Dict[str, Dict[str, Any]] = {}
        self.lifecycle_state: Dict[str, str] = {}
        self.coordination_lock = threading.Lock()
        self.dependency_resolver = DependencyResolver()
        
    def register_plugin(self, plugin_name: str, dependencies: List[str] = None,
                       dependents: List[str] = None):
        """Register plugin in coordination graph"""
        with self.coordination_lock:
            self.coordination_graph[plugin_name] = {
                'dependencies': dependencies or [],
                'dependents': dependents or [],
                'initialized': False,
                'running': False
            }
            
            # Update dependencies
            for dep in dependencies or []:
                if dep in self.coordination_graph:
                    if plugin_name not in self.coordination_graph[dep].get('dependents', []):
                        self.coordination_graph[dep].setdefault('dependents', []).append(plugin_name)
            
            # Update dependents
            for dependent in dependents or []:
                if dependent in self.coordination_graph:
                    if plugin_name not in self.coordination_graph[dependent].get('dependencies', []):
                        self.coordination_graph[dependent].setdefault('dependencies', []).append(plugin_name)
    
    def can_initialize(self, plugin_name: str) -> bool:
        """Check if plugin can be initialized"""
        with self.coordination_lock:
            if plugin_name not in self.coordination_graph:
                return False
            
            graph = self.coordination_graph[plugin_name]
            
            # Check if all dependencies are initialized
            for dep in graph.get('dependencies', []):
                if not graph.get(dep, {}).get('initialized', False):
                    return False
            
            return True
    
    def initialize_plugin(self, plugin_name: str) -> bool:
        """Initialize plugin according to dependencies"""
        with self.coordination_lock:
            if not self.can_initialize(plugin_name):
                return False
            
            # Initialize plugin
            plugin = self.plugin_manager.get_plugin(plugin_name)
            if not plugin:
                return False
            
            plugin.initialize(self.plugin_manager.interface)
            
            # Update graph
            self.coordination_graph[plugin_name]['initialized'] = True
            self.coordination_graph[plugin_name]['running'] = True
            
            return True
    
    def shutdown_plugin(self, plugin_name: str) -> bool:
        """Shutdown plugin and notify dependents"""
        with self.coordination_lock:
            if plugin_name not in self.coordination_graph:
                return False
            
            graph = self.coordination_graph[plugin_name]
            
            # Shutdown dependents first
            for dependent in graph.get('dependents', []):
                self.shutdown_plugin(dependent)
            
            # Shutdown plugin
            plugin = self.plugin_manager.get_plugin(plugin_name)
            if plugin:
                plugin.disable()
                plugin.unload()
            
            # Update graph
            graph['initialized'] = False
            graph['running'] = False
            
            return True
    
    def get_dependency_graph(self) -> Dict[str, Dict[str, Any]]:
        """Get dependency graph"""
        return self.coordination_graph.copy()
    
    def check_plugin_health(self, plugin_name: str) -> Dict[str, Any]:n""": "Check plugin health""
        if plugin_name not in self.coordination_graph:
            return {'healthy': False, 'error': 'Plugin not registered'}
        
        plugin = self.plugin_manager.get_plugin(plugin_name)
        if not plugin:
            return {'healthy': False, 'error': 'Plugin not loaded'}
        
        # Check if plugin is running
        graph = self.coordination_graph[plugin_name]
        if not graph.get('running', False):
            return {'healthy': False, 'error': 'Plugin not running'}
        
        # Check for errors
        if plugin.info.status == PluginStatus.ERROR:
            return {
                'healthy': False,
                'error': f"Plugin error: {plugin.info.error_message}"
            }
        
        return {'healthy': True}
        
    def resolve_dependencies(self, plugin_specs: List[str]) -> Dict[str, Any]:
        """Resolve plugin dependencies"""
        return self.dependency_resolver.resolve_dependencies(plugin_specs)
        
    def detect_conflicts(self, plugin_specs: List[str]) -> List[Tuple[str, str]]:
        """Detect conflicts between plugins"""
        return self.dependency_resolver.detect_conflicts(plugin_specs)


class LifecycleMonitor:
    """Monitor plugin lifecycle and health"""
    
    def __init__(self, plugin_manager: EnhancedPluginManager):
        self.plugin_manager = plugin_manager
        self.plugin_metrics: Dict[str, Dict[str, Any]] = {}
        self.health_check_interval = 60.0  # 60 seconds
        self.performance_thresholds = {
            'load_time': 5000.0,  # 5 seconds
            'unload_time': 2000.0,  # 2 seconds
            'memory_usage': 512.0,  # 512 MB
            'cpu_usage': 90.0,  # 90%
            'event_processing_time': 100.0  # 100ms
        }
        self.monitoring = False
        self.monitor_thread = None
        
    def start_monitoring(self):
        """Start monitoring plugins"""
        if self.monitoring:
            return
        
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
    def stop_monitoring(self):
        """Stop monitoring plugins"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=10.0)
            self.monitor_thread = None
    
    def get_plugin_metrics(self, plugin_name: str) -> Dict[str, Any]:n""": "Get plugin metrics"""
        return self.plugin_metrics.get(plugin_name, {})
        
    def get_all_metrics(self) -> Dict[str, Dict[str, Any]]:
        """Get all plugin metrics"""
        return self.plugin_metrics.copy()
        
    def record_event(self, plugin_name: str, event_type: str, event_data: Dict[str, Any]):n""": "Record plugin event"""
        if plugin_name not in self.plugin_metrics:
            self.plugin_metrics[plugin_name] = {
                'events': [],
                'performance': {},
                'health_checks': []
            }
        
        self.plugin_metrics[plugin_name]['events'].append({
            'timestamp': time.time(),
            'type': event_type,
            'data': event_data
        })
        
        # Keep only recent events (last hour)
        current_time = time.time()
        self.plugin_metrics[plugin_name]['events'] = [
            e for e in self.plugin_metrics[plugin_name]['events']
            if current_time - e['timestamp'] < 3600
        ]
        
    def record_health_check(self, plugin_name: str, healthy: bool, details: Dict[str, Any] = None):n""": "Record health check"""
        if plugin_name not in self.plugin_metrics:
            self.plugin_metrics[plugin_name] = {
                'events': [],
                'performance': {},
                'health_checks': []
            }
        
        health_check = {
            'timestamp': time.time(),
            'healthy': healthy,
            'details': details or {}
        }
        
        self.plugin_metrics[plugin_name]['health_checks'].append(health_check)
        
        # Keep only recent health checks (last hour)
        current_time = time.time()
        self.plugin_metrics[plugin_name]['health_checks'] = [
            h for h in self.plugin_metrics[plugin_name]['health_checks']
            if current_time - h['timestamp'] < 3600
        ]
        
    def record_performance(self, plugin_name: str, metrics: Dict[str, Any]):n""": "Record performance metrics"""
        if plugin_name not in self.plugin_metrics:
            self.plugin_metrics[plugin_name] = {
                'events': [],
                'performance': {},
                'health_checks': []
            }
        
        # Update performance metrics
        for metric_name, metric_value in metrics.items():
            if metric_name not in self.plugin_metrics[plugin_name]['performance']:
                self.plugin_metrics[plugin_name]['performance'][metric_name] = []
            
            self.plugin_metrics[plugin_name]['performance'][metric_name].append({
                'timestamp': time.time(),
                'value': metric_value
            })
        
        # Keep only recent performance data (last hour)
        current_time = time.time()
        for metric_name in self.plugin_metrics[plugin_name]['performance']:
            self.plugin_metrics[plugin_name]['performance'][metric_name] = [
                m for m in self.plugin_metrics[plugin_name]['performance'][metric_name]
                if current_time - m['timestamp'] < 3600
            ]
        
        # Check performance thresholds
        self._check_performance_thresholds(plugin_name, metrics)
        
    def _monitor_loop(self):
        """Monitor loop"""
        while self.monitoring:
            try:
                # Check plugin health
                for plugin_name in list(self.plugin_manager.plugins.keys()):
                    self._check_plugin_health(plugin_name)
                
                # Sleep
                time.sleep(self.health_check_interval)
                
            except Exception as e:
                logging.error(f"Monitoring error: {e}")
                time.sleep(self.health_check_interval)
    
    def _check_plugin_health(self, plugin_name: str):
        """Check plugin health"""
        plugin = self.plugin_manager.get_plugin(plugin_name)
        if not plugin:
            return
        
        # Check plugin status
        healthy = True
        details = {}
        
        if plugin.info.status == PluginStatus.ERROR:
            healthy = False
            details['error'] = plugin.info.error_message
        
        if not plugin.initialized:
            healthy = False
            details['initialized'] = False
        
        if not plugin.enabled:
            healthy = False
            details['enabled'] = False
        
        # Record health check
        self.record_health_check(plugin_name, healthy, details)
        
        # Record performance
        metrics = self._collect_plugin_metrics(plugin_name)
        if metrics:
            self.record_performance(plugin_name, metrics)
    
    def _collect_plugin_metrics(self, plugin_name: str) -> Dict[str, Any]:
        """Collect plugin metrics"""
        metrics = {}
        
        # Get plugin runtime info
        runtime_info = getattr(self.plugin_manager, 'get_plugin_state', lambda x: None)(plugin_name)
        if runtime_info:
            metrics['memory_usage'] = runtime_info.memory_usage
            metrics['cpu_usage'] = runtime_info.cpu_usage
            metrics['state'] = runtime_info.state.value
        
        # Get plugin load/unload times
        if hasattr(self.plugin_manager, 'plugin_load_times'):
            load_times = self.plugin_manager.plugin_load_times.get(plugin_name, [])
            if load_times:
                metrics['avg_load_time'] = sum(load_times) / len(load_times)
        
        if hasattr(self.plugin_manager, 'plugin_unload_times'):
            unload_times = self.plugin_manager.plugin_unload_times.get(plugin_name, [])
            if unload_times:
                metrics['avg_unload_time'] = sum(unload_times) / len(unload_times)
        
        return metrics
        
    def _check_performance_thresholds(self, plugin_name: str, metrics: Dict[str, Any]):n""": "Check performance thresholds"""
        for metric_name, threshold in self.performance_thresholds.items():
            if metric_name in metrics:
                if metrics[metric_name] > threshold:
                    # Alert management would go here
                    logging.warning(f"Plugin {plugin_name} performance threshold exceeded: {metric_name} = {metrics[metric_name]}")


class DependencyResolver:
    """Resolve plugin dependencies"""
    
    def __init__(self):
        self.plugin_registry: Dict[str, Dict[str, Any]] = {}
        self.dependency_graph: Dict[str, Dict[str, Any]] = {}
        
    def add_plugin(self, plugin_name: str, dependencies: List[str] = None,
                  required_plugins: List[str] = None, conflicts: List[str] = None):
        """Add plugin to registry"""
        self.plugin_registry[plugin_name] = {
            'dependencies': dependencies or [],
            'required_plugins': required_plugins or [],
            'conflicts': conflicts or []
        }
        
        # Build dependency graph
        self._build_dependency_graph()
        
    def resolve_dependencies(self, plugin_specs: List[str]) -> Dict[str, Any]:n""": "Resolve plugin dependencies"""
        result = {
            'plugins_to_load': [],
            'conflicts': [],
            'missing_dependencies': [],
            'circular_dependencies': []
        }
        
        # Check for conflicts
        conflicts = self._detect_conflicts(plugin_specs)
        result['conflicts'] = conflicts
        
        # Resolve dependencies
        plugins_to_load = self._resolve_dependency_chain(plugin_specs)
        result['plugins_to_load'] = plugins_to_load
        
        # Check for circular dependencies
        circular = self._detect_circular_dependencies(plugin_specs)
        result['circular_dependencies'] = circular
        
        # Check for missing dependencies
        missing = self._detect_missing_dependencies(plugin_specs)
        result['missing_dependencies'] = missing
        
        return result
        
    def _build_dependency_graph(self):
        """Build dependency graph"""
        self.dependency_graph = {}
        
        for plugin_name, plugin_info in self.plugin_registry.items():
            self.dependency_graph[plugin_name] = {
                'dependencies': plugin_info.get('dependencies', []),
                'required_plugins': plugin_info.get('required_plugins', []),
                'conflicts': plugin_info.get('conflicts', []),
                'children': [],
                'parents': []
            }
        
        # Build reverse dependencies
        for plugin_name, graph in self.dependency_graph.items():
            for dep in graph.get('dependencies', []) + graph.get('required_plugins', []):
                if dep in self.dependency_graph:
                    if plugin_name not in self.dependency_graph[dep].get('children', []):
                        self.dependency_graph[dep].setdefault('children', []).append(plugin_name)
            
            for conflict in graph.get('conflicts', []):
                if conflict in self.dependency_graph:
                    if plugin_name not in self.dependency_graph[conflict].get('conflicts', []):
                        self.dependency_graph[conflict].setdefault('conflicts', []).append(plugin_name)
        
    def _detect_conflicts(self, plugin_specs: List[str]) -> List[Tuple[str, str]]:
        """Detect conflicts between plugins"""
        conflicts = []
        
        for i, plugin1 in enumerate(plugin_specs):
            for j, plugin2 in enumerate(plugin_specs):
                if i >= j:
                    continue
                
                if plugin1 in self.dependency_graph and plugin2 in self.dependency_graph:
                    graph1 = self.dependency_graph[plugin1]
                    graph2 = self.dependency_graph[plugin2]
                    
                    # Check for conflicts
                    if plugin2 in graph1.get('conflicts', []):
                        conflicts.append((plugin1, plugin2))
                    
                    if plugin1 in graph2.get('conflicts', []):
                        conflicts.append((plugin2, plugin1))
        
        return conflicts
        
    def _resolve_dependency_chain(self, plugin_specs: List[str]) -> List[str]:n""": "Resolve dependency chain"""
        loaded_plugins = set()
        plugins_to_load = []
        
        # Sort plugins by dependency order
        sorted_plugins = self._topological_sort(plugin_specs)
        
        # Load plugins in order
        for plugin_name in sorted_plugins:
            # Check if all dependencies are loaded
            if self._check_dependencies_loaded(plugin_name, loaded_plugins):
                plugins_to_load.append(plugin_name)
                loaded_plugins.add(plugin_name)
        
        return plugins_to_load
        
    def _topological_sort(self, plugin_specs: List[str]) -> List[str]:n""": "Topological sort of plugins"""
        # Build adjacency list
        graph = {}
        in_degree = {}
        
        for plugin_name in plugin_specs:
            if plugin_name in self.dependency_graph:
                graph[plugin_name] = self.dependency_graph[plugin_name].get('dependencies', [])
                in_degree[plugin_name] = 0
        
        # Calculate in-degree
        for plugin_name, deps in graph.items():
            for dep in deps:
                if dep in plugin_specs:
                    in_degree[plugin_name] = in_degree.get(plugin_name, 0) + 1
        
        # Topological sort
        queue = [p for p in plugin_specs if in_degree.get(p, 0) == 0]
        result = []
        
        while queue:
            plugin_name = queue.pop(0)
            result.append(plugin_name)
            
            for dependent in self.dependency_graph.get(plugin_name, {}).get('children', []):
                if dependent in plugin_specs:
                    in_degree[dependent] -= 1
                    if in_degree[dependent] == 0:
                        queue.append(dependent)
        
        # Check for cycles
        if len(result) != len([p for p in plugin_specs if p in self.dependency_graph]):
            # There is a cycle
            return plugin_specs  # Return original order as fallback
        
        return result
        
    def _check_dependencies_loaded(self, plugin_name: str, loaded_plugins: Set[str]) -> bool:n""": "Check if all dependencies are loaded"""
        if plugin_name not in self.dependency_graph:
            return True
        
        graph = self.dependency_graph[plugin_name]
        
        for dep in graph.get('dependencies', []) + graph.get('required_plugins', []):
            if dep not in loaded_plugins:
                return False
        
        return True
        
    def _detect_circular_dependencies(self, plugin_specs: List[str]) -> List[List[str]]:
        """Detect circular dependencies"""
        circular = []
        
        for plugin_name in plugin_specs:
            if plugin_name in self.dependency_graph:
                # Simple cycle detection
                visited = set()
                path = []
                
                def dfs(node):
                    if node in path:
                        # Found cycle
                        cycle_start = path.index(node)
                        circular.append(path[cycle_start:] + [node])
                        return
                    
                    if node in visited:
                        return
                    
                    visited.add(node)
                    path.append(node)
                    
                    for dep in self.dependency_graph.get(node, {}).get('dependencies', []):
                        if dep in plugin_specs:
                            dfs(dep)
                    
                    path.pop()
                
                dfs(plugin_name)
        
        return circular
        
    def _detect_missing_dependencies(self, plugin_specs: List[str]) -> List[Tuple[str, str]]:
        """Detect missing dependencies"""
        missing = []
        
        for plugin_name in plugin_specs:
            if plugin_name in self.dependency_graph:
                graph = self.dependency_graph[plugin_name]
                
                for dep in graph.get('dependencies', []) + graph.get('required_plugins', []):
                    if dep not in plugin_specs and dep not in self.plugin_registry:
                        missing.append((plugin_name, dep))
        
        return missing


# Performance Monitor
# ===================

class PerformanceMonitor:
    """Monitor plugin performance"""
    
    def __init__(self, plugin_manager: EnhancedPluginManager):
        self.plugin_manager = plugin_manager
        self.performance_data: Dict[str, Dict[str, Any]] = {}
        self.alert_thresholds = {
            'load_time_ms': 5000.0,
            'unload_time_ms': 2000.0,
            'memory_usage_mb': 512.0,
            'cpu_usage_percent': 90.0,
            'event_processing_time_ms': 100.0
        }
        
    def record_plugin_load(self, plugin_name: str, load_time: float):
        """Record plugin load time"""
        self._record_metric(plugin_name, 'load_time', load_time)
        
    def record_plugin_load_failure(self, plugin_name: str, load_time: float):
        """Record plugin load failure"""
        self._record_metric(plugin_name, 'load_time', load_time)
        
    def record_plugin_unload(self, plugin_name: str, unload_time: float):
        """Record plugin unload time"""
        self._record_metric(plugin_name, 'unload_time', unload_time)
        
    def record_health_check(self, plugin_name: str, healthy: bool):
        """Record health check"""
        self._record_metric(plugin_name, 'healthy', 1.0 if healthy else 0.0)
        
    def record_event(self, plugin_name: str, event: Dict[str, Any]):
        """Record plugin event"""
        self._record_metric(plugin_name, 'event_count', 1)
        
    def get_performance_summary(self, plugin_name: str) -> Dict[str, Any]:n""": "Get performance summary"""
        if plugin_name not in self.performance_data:
            return {}
        
        metrics = self.performance_data[plugin_name]
        
        summary = {}
        for metric_name, values in metrics.items():
            if values:
                summary[metric_name] = {
                    'current': values[-1]['value'],
                    'average': sum(v['value'] for v in values) / len(values),
                    'min': min(v['value'] for v in values),
                    'max': max(v['value'] for v in values),
                    'count': len(values)
                }
        
        return summary
        
    def get_performance_alerts(self, plugin_name: str) -> List[Dict[str, Any]]:
        """Get performance alerts"""
        alerts = []
        
        summary = self.get_performance_summary(plugin_name)
        
        for metric_name, threshold in self.alert_thresholds.items():
            if metric_name in summary:
                current = summary[metric_name]['current']
                if current > threshold:
                    alerts.append({
                        'metric': metric_name,
                        'current': current,
                        'threshold': threshold,
                        'severity': 'warning' if current > threshold * 0.9 else 'critical'
                    })
        
        return alerts
        
    def _record_metric(self, plugin_name: str, metric_name: str, value: Any):
        """Record metric"""
        if plugin_name not in self.performance_data:
            self.performance_data[plugin_name] = {}
        
        if metric_name not in self.performance_data[plugin_name]:
            self.performance_data[plugin_name][metric_name] = []
        
        self.performance_data[plugin_name][metric_name].append({
            'timestamp': time.time(),
            'value': value
        })
        
        # Keep only recent data (last 24 hours)
        current_time = time.time()
        for metric_name in list(self.performance_data[plugin_name].keys()):
            self.performance_data[plugin_name][metric_name] = [
                m for m in self.performance_data[plugin_name][metric_name]
                if current_time - m['timestamp'] < 86400
            ]


# Multi-Language Support
# ======================

class MultiLanguagePluginLoader:
    """Loader for multi-language plugins"""
    
    def __init__(self, plugin_manager: EnhancedPluginManager):
        self.plugin_manager = plugin_manager
        self.loaders = {
            'python': PythonPluginLoader(),
            'cpp': CPlusPlusPluginLoader(),
            'csharp': CSharpPluginLoader(),
        }
        
    def load_plugin(self, plugin_spec: str) -> Any:
        """Load plugin based on its format"""
        # Detect plugin format
        format = self._detect_plugin_format(plugin_spec)
        
        # Get appropriate loader
        loader = self.loaders.get(format)
        if not loader:
            raise RuntimeError(f"Unsupported plugin format: {format}")
        
        return loader.load(plugin_spec)
        
    def _detect_plugin_format(self, plugin_spec: str) -> str:
        """Detect plugin format from specification"""
        if plugin_spec.endswith('.py'):
            return 'python'
        elif plugin_spec.endswith(('.so', '.dll', '.dylib')):
            return 'cpp'  # Assuming C++ shared library
        elif plugin_spec.endswith('.cs') or plugin_spec.endswith('.dll'):
            return 'csharp'
        else:
            # Try to infer from content
            try:
                with open(plugin_spec, 'r') as f:
                    content = f.read()
                    if 'import' in content or 'def ' in content:
                        return 'python'
            except:
                pass
        
        return 'python'  # Default to Python
class PythonPluginLoader:
    """Loader for Python plugins"""
    
    def load(self, plugin_spec: str) -> Any:
        """Load Python plugin"""
        # In a real implementation, this would dynamically load the Python plugin
        # For now, we'll return a mock plugin
        return MockPythonPlugin(plugin_spec)
        
class CPlusPlusPluginLoader:
    """Loader for C++ plugins"""
    
    def load(self, plugin_spec: str) -> Any:
        """Load C++ plugin"""
        # In a real implementation, this would load the C++ plugin
        # For now, we'll return a mock plugin
        return MockCPlusPlusPlugin(plugin_spec)
        
class CSharpPluginLoader:
    """Loader for C# plugins"""
    
    def load(self, plugin_spec: str) -> Any:
        """Load C# plugin"""
        # In a real implementation, this would load the C# plugin
        # For now, we'll return a mock plugin
        return MockCSharpPlugin(plugin_spec)
class MockPythonPlugin:
    """Mock Python plugin for testing"""
    
    def __init__(self, plugin_spec: str):
        self.plugin_spec = plugin_spec
        self.name = plugin_spec.split('/')[-1].replace('.py', '')
        
    def get_info(self):
        """Get plugin information"""
        return {
            'name': self.name,
            'version': '1.0.0',
            'description': f"Mock Python plugin: {self.name}",
            'plugin_type': 'python'
        }
class MockCPlusPlusPlugin:
    """Mock C++ plugin for testing"""
    
    def __init__(self, plugin_spec: str):
        self.plugin_spec = plugin_spec
        self.name = plugin_spec.split('/')[-1].replace('.so', '').replace('.dll', '').replace('.dylib', '')
        
    def get_info(self):
        """Get plugin information"""
        return {
            'name': self.name,
            'version': '1.0.0',
            'description': f"Mock C++ plugin: {self.name}",
            'plugin_type': 'cpp'
        }
class MockCSharpPlugin:
    """Mock C# plugin for testing"""
    
    def __init__(self, plugin_spec: str):
        self.plugin_spec = plugin_spec
        self.name = plugin_spec.split('/')[-1].replace('.cs', '').replace('.dll', '')
        
    def get_info(self):
        """Get plugin information"""
        return {
            'name': self.name,
            'version': '1.0.0',
            'description': f"Mock C# plugin: {self.name}",
            'plugin_type': 'csharp'
        }


# Backward Compatibility
# ======================

class BackwardCompatibilityManager:
    """Manager for backward compatibility"""
    
    def __init__(self, plugin_manager: EnhancedPluginManager):
        self.plugin_manager = plugin_manager
        self.adapters: Dict[str, Any] = {}
        self.compatibility_layer = CompatibilityLayer()
        
    def load_legacy_plugin(self, plugin_spec: str) -> Any:
        """Load legacy plugin with compatibility layer"""
        # Detect if plugin is legacy
        if self._is_legacy_plugin(plugin_spec):
            # Load with compatibility layer
            return self.compatibility_layer.adapt(plugin_spec)
        else:
            # Load normally
            return self.plugin_manager.load_plugin(plugin_spec)
    
    def adapt_legacy_interface(self, plugin: Any) -> Any:
        """Adapt legacy plugin interface"""
        # Adapt legacy plugin to new interface
        if hasattr(plugin, 'legacy_interface'):
            return self.adapters.get('legacy_to_new', lambda p: p)(plugin)
        return plugin
    
    def _is_legacy_plugin(self, plugin_spec: str) -> bool:
        """Check if plugin is legacy"""
        # In a real implementation, this would check plugin version or format
        return False


class CompatibilityLayer:
    """Compatibility layer for plugins"""
    
    def adapt(self, plugin_spec: str) -> Any:
        """Adapt plugin to compatibility layer"""
        # In a real implementation, this would adapt the plugin
        # For now, we'll return a mock plugin
        return MockPlugin(plugin_spec)
        
    def validate_compatibility(self, plugin_spec: str) -> List[str]:
        """Validate plugin compatibility"""
        # In a real implementation, this would validate compatibility
        return []
class MockPlugin:
    """Mock plugin for backward compatibility testing"""
    
    def __init__(self, plugin_spec: str):
        self.plugin_spec = plugin_spec
        self.name = plugin_spec.split('/')[-1]
        
    def get_info(self):
        """Get plugin information"""
        return {
            'name': self.name,
            'version': '1.0.0',
            'description': f"Mock plugin: {self.name}",
            'plugin_type': 'unknown'
        }


# Plugin Management Interface
# ==========================

class PluginManagementInterface:
    """Interface for plugin management"""
    
    def __init__(self, plugin_manager: EnhancedPluginManager):
        self.plugin_manager = plugin_manager
        self.ui_components = UIComponents()
        self.dashboard = PluginDashboard()
        self.monitoring = PluginMonitoring()
        
    def get_plugin_status(self, plugin_name: str) -> Dict[str, Any]:
        """Get plugin status"""
        plugin = self.plugin_manager.get_plugin(plugin_name)
        if not plugin:
            return {'status': 'not_loaded', 'error': 'Plugin not found'}
        
        return {
            'status': plugin.info.status.value,
            'name': plugin.info.name,
            'version': plugin.info.version,
            'description': plugin.info.description,
            'author': plugin.info.author,
            'plugin_type': plugin.info.plugin_type.value,
            'enabled': plugin.enabled,
            'initialized': plugin.initialized,
            'error': plugin.info.error_message or ''
        }
    
    def get_plugin_performance(self, plugin_name: str) -> Dict[str, Any]:n""": "Get plugin performance"""
        if hasattr(self.plugin_manager, 'performance_monitor'):
            return self.plugin_manager.performance_monitor.get_performance_summary(plugin_name)
        return {}
        
    def get_plugin_dependencies(self, plugin_name: str) -> List[str]:
        """Get plugin dependencies"""
        if hasattr(self.plugin_manager, 'dependency_resolver'):
            if plugin_name in self.plugin_manager.dependency_resolver.plugin_registry:
                return self.plugin_manager.dependency_resolver.plugin_registry[plugin_name].get('dependencies', [])
        return []
        
    def manage_plugin_lifecycle(self, plugin_name: str, action: str, **kwargs) -> bool:
        """Manage plugin lifecycle"""
        if action == 'load':
            return self.plugin_manager.load_plugin(plugin_name, kwargs.get('path'), kwargs.get('config'))
        elif action == 'unload':
            return self.plugin_manager.unload_plugin(plugin_name)
        elif action == 'reload':
            return self.plugin_manager.hot_reload_plugin(plugin_name, kwargs.get('path'))
        elif action == 'enable':
            plugin = self.plugin_manager.get_plugin(plugin_name)
            if plugin:
                plugin.enable()
                return True
            return False
        elif action == 'disable':
            plugin = self.plugin_manager.get_plugin(plugin_name)
            if plugin:
                plugin.disable()
                return True
            return False
        else:
            raise ValueError(f"Unknown action: {action}")


class UIComponents:
    """UI components for plugin management"""
    
    def __init__(self):
        self.components = {}
        
    def get_status_display(self, plugin_name: str) -> Dict[str, Any]:
        """Get status display component"""
        return {
            'type': 'status_display',
            'plugin_name': plugin_name,
            'status': 'loading',
            'progress': 0.0
        }
    
    def get_performance_chart(self, plugin_name: str) -> Dict[str, Any]:
        """Get performance chart component"""
        return {
            'type': 'performance_chart',
            'plugin_name': plugin_name,
            'metrics': ['load_time', 'memory_usage', 'cpu_usage'],
            'time_range': '24h'
        }
class PluginDashboard:
    """Plugin dashboard"""
    
    def __init__(self, plugin_manager: EnhancedPluginManager):
        self.plugin_manager = plugin_manager
        self.widgets = []
        
    def add_widget(self, widget: Dict[str, Any]):
        """Add widget to dashboard"""
        self.widgets.append(widget)
        
    def remove_widget(self, widget_id: str):
        """Remove widget from dashboard"""
        self.widgets = [w for w in self.widgets if w.get('id') != widget_id]
        
    def get_dashboard_config(self) -> Dict[str, Any]:
        """Get dashboard configuration"""
        return {
            'widgets': self.widgets,
            'layout': 'grid',
            'refresh_interval': 30
        }
class PluginMonitoring:
    """Plugin monitoring"""
    
    def __init__(self, plugin_manager: EnhancedPluginManager):
        self.plugin_manager = plugin_manager
        self.alert_rules = []
        
    def add_alert_rule(self, rule: Dict[str, Any]):
        """Add alert rule"""
        self.alert_rules.append(rule)
        
    def check_alerts(self) -> List[Dict[str, Any]]:
        """Check for alerts"""
        alerts = []
        
        for rule in self.alert_rules:
            if self._evaluate_alert_rule(rule):
                alerts.append({
                    'rule': rule,
                    'timestamp': time.time(),
                    'severity': rule.get('severity', 'warning')
                })
        
        return alerts
        
    def _evaluate_alert_rule(self, rule: Dict[str, Any]) -> bool:
        """Evaluate alert rule"""
        # In a real implementation, this would evaluate the alert rule
        return False