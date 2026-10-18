# Scripting and Plugin System for NLM Extensibility
# Advanced scripting and plugin system for NLM

import abc
import json
import yaml
import importlib
import sys
import os
import threading
import time
import hashlib
import tempfile
import shutil
from typing import Dict, List, Any, Optional, Union, Callable, Type
from dataclasses import dataclass, field
from enum import Enum
import importlib.util
from pathlib import Path
from abc import ABC, abstractmethod
import argparse
import signal
import subprocess
import logging

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class PluginType(Enum):
    """Types of plugins supported by NLM."""
    
    SIMULATION_PLUGIN = "simulation_plugin"
    ANALYSIS_PLUGIN = "analysis_plugin"
    VISUALIZATION_PLUGIN = "visualization_plugin"
    CONFIGURATION_PLUGIN = "configuration_plugin"
    NEUROMODULATION_PLUGIN = "neuromodulation_plugin"
    MEMORY_PLUGIN = "memory_plugin"
    COGNITION_PLUGIN = "cognition_plugin"
    DEVELOPMENT_PLUGIN = "development_plugin"
    PLUGIN = "plugin"
class PluginStatus(Enum):
    """Status of plugins."""
    
    UNKNOWN = "unknown"
    LOADING = "loading"
    LOADED = "loaded"
    INITIALIZING = "initializing"
    INITIALIZED = "initialized"
    RUNNING = "running"
    STOPPING = "stopping"
    STOPPED = "stopped"
    ERROR = "error"
    DISABLED = "disabled"
class PluginCapability(Enum):
    """Capabilities provided by plugins."""
    
    SIMULATION = "simulation"
    ANALYSIS = "analysis"
    VISUALIZATION = "visualization"
    CONFIGURATION = "configuration"
    NEUROMODULATION = "neuromodulation"
    MEMORY = "memory"
    COGNITION = "cognition"
    DEVELOPMENT = "development"
    DATA_ACCESS = "data_access"
    EVENT_HANDLING = "event_handling"
    PLUGIN_MANAGEMENT = "plugin_management"
class PluginEvent(Enum):
    """Events that plugins can respond to."""
    
    SIMULATION_START = "simulation_start"
    SIMULATION_END = "simulation_end"
    SIMULATION_STEP = "simulation_step"
    SIMULATION_PAUSE = "simulation_pause"
    SIMULATION_RESUME = "simulation_resume"
    SIMULATION_RESET = "simulation_reset"
    
    NEURON_CREATED = "neuron_created"
    NEURON_ACTIVATED = "neuron_activated"
    NEURON_DEACTIVATED = "neuron_deactivated"
    
    SYNAPSIS_CREATED = "synapse_created"
    SYNAPSIS_MODIFIED = "synapse_modified"
    SYNAPSIS_REMOVED = "synapse_removed"
    
    ACTION_PRODUCED = "action_produced"
    REWARD_RECEIVED = "reward_received"
    
    MEMORY_UPDATED = "memory_updated"
    EPISODE_STORED = "episode_stored"
    EPISODE_REPLAYED = "episode_replayed"
    
    DEVELOPERAL_STAGE_CHANGED = "developmental_stage_changed"
    MATURATION_COMPLETED = "maturation_completed"
    
    PLUGIN_LOADED = "plugin_loaded"
    PLUGIN_UNLOADED = "plugin_unloaded"
    PLUGIN_ENABLED = "plugin_enabled"
    PLUGIN_DISABLED = "plugin_disabled"
    
    CONFIGURATION_CHANGED = "configuration_changed"
    SYSTEM_ERROR = "system_error"
    USER_INTERACTION = "user_interaction"
@dataclass
class PluginInfo:
    """Information about a plugin."""
    
    name: str
    version: str
    description: str
    author: str
    email: str
    license: str
    plugin_type: PluginType
    capabilities: List[PluginCapability]
    dependencies: List[str]
    required_version: str = "1.0.0"
    supported_nlm_version: str = "1.0.0"
    file_path: str = ""
    load_time: float = 0.0
    init_time: float = 0.0
    status: PluginStatus = PluginStatus.UNKNOWN
    metadata: Dict[str, Any] = field(default_factory=dict)
    tags: List[str] = field(default_factory=list)
    configuration_schema: Dict[str, Any] = field(default_factory=dict)
    
    @property
    def is_compatible(self) -> bool:
        """Check if plugin is compatible with current NLM version."""
        return self.version == self.supported_nlm_version
    
    @property
    def has_capability(self, capability: PluginCapability) -> bool:
        """Check if plugin has a specific capability."""
        return capability in self.capabilities
class PluginEventData:
    """Data for plugin events."""
    
    def __init__(self, event_type: PluginEvent, data: Dict[str, Any]):
        self.event_type = event_type
        self.data = data
        self.timestamp = time.time()
        self.event_id = hashlib.md5(f"{event_type.value}{self.timestamp}".encode()).hexdigest()
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert event data to dictionary."""
        return {
            'event_type': self.event_type.value,
            'data': self.data,
            'timestamp': self.timestamp,
            'event_id': self.event_id
        }
class PluginInterface(ABC):
    """
    Abstract base class for all NLM plugins.
    
    This class defines the interface that all plugins must implement.
    """
    
    def __init__(self, info: PluginInfo):
        self.info = info
        self._initialized = False
        self._enabled = True
        self._event_handlers: Dict[PluginEvent, List[Callable]] = defaultdict(list)
        self._configuration: Dict[str, Any] = {}
        
        # Register for plugin lifecycle events
        self._register_lifecycle_handlers()
    
    def _register_lifecycle_handlers(self) -> None:
        """Register handlers for plugin lifecycle events."""
        self.register_event_handler(PluginEvent.PLUGIN_LOADED, self._on_plugin_loaded)
        self.register_event_handler(PluginEvent.PLUGIN_ENABLED, self._on_plugin_enabled)
        self.register_event_handler(PluginEvent.PLUGIN_DISABLED, self._on_plugin_disabled)
        self.register_event_handler(PluginEvent.PLUGIN_UNLOADED, self._on_plugin_unloaded)
    
    @abstractmethod
    def initialize(self, nlm_system: 'NLMPluginSystem', 
                   configuration: Dict[str, Any]) -> bool:
        """
        Initialize the plugin.
        
        Args:
            nlm_system: Reference to the NLM plugin system
            configuration: Plugin configuration
            
        Returns:
            True if initialization succeeded, False otherwise
        """
        pass
    
    @abstractmethod
    def shutdown(self) -> bool:
        """
        Shutdown the plugin.
        
        Returns:
            True if shutdown succeeded, False otherwise
        """
        pass
    
    @abstractmethod
    def get_capabilities(self) -> List[PluginCapability]:
        """
        Get plugin capabilities.
        
        Returns:
            List of plugin capabilities
        """
        pass
    
    def register_event_handler(self, event: PluginEvent, 
                              handler: Callable[[PluginEventData], None]) -> None:
        """
        Register an event handler.
        
        Args:
            event: Event type
            handler: Event handler function
        """
        if event not in self._event_handlers:
            self._event_handlers[event] = []
        
        self._event_handlers[event].append(handler)
    
    def unregister_event_handler(self, event: PluginEvent, 
                                handler: Callable[[PluginEventData], None]) -> None:
        """
        Unregister an event handler.
        
        Args:
            event: Event type
            handler: Event handler function to remove
        """
        if event in self._event_handlers:
            try:
                self._event_handlers[event].remove(handler)
            except ValueError:
                pass  # Handler not found
    
    def emit_event(self, event: PluginEvent, data: Dict[str, Any] = None) -> None:
        """
        Emit an event to all registered handlers.
        
        Args:
            event: Event type
            data: Event data
        """
        if data is None:
            data = {}
        
        event_data = PluginEventData(event, data)
        
        if event in self._event_handlers:
            for handler in self._event_handlers[event]:
                try:
                    handler(event_data)
                except Exception as e:
                    logger.error(f"Error in event handler for {event}: {e}")
    
    def set_configuration(self, configuration: Dict[str, Any]) -> None:
        """
        Set plugin configuration.
        
        Args:
            configuration: Plugin configuration
        """
        self._configuration = configuration.copy()
        self.on_configuration_changed(configuration)
    
    def get_configuration(self) -> Dict[str, Any]:
        """
        Get plugin configuration.
        
        Returns:
            Plugin configuration
        """
        return self._configuration.copy()
    
    def on_configuration_changed(self, configuration: Dict[str, Any]) -> None:
        """
        Handle configuration changes.
        
        Args:
            configuration: New configuration
        """
        pass
    
    def enable(self) -> None:
        """Enable the plugin."""
        if not self._enabled:
            self._enabled = True
            self.emit_event(PluginEvent.PLUGIN_ENABLED)
    
    def disable(self) -> None:
        """Disable the plugin."""
        if self._enabled:
            self._enabled = False
            self.emit_event(PluginEvent.PLUGIN_DISABLED)
    
    def is_enabled(self) -> bool:
        """
        Check if plugin is enabled.
        
        Returns:
            True if plugin is enabled, False otherwise
        """
        return self._enabled
    
    def is_initialized(self) -> bool:
        """
        Check if plugin is initialized.
        
        Returns:
            True if plugin is initialized, False otherwise
        """
        return self._initialized
    
    def _on_plugin_loaded(self, event_data: PluginEventData) -> None:
        """Handle plugin loaded event."""
        pass
    
    def _on_plugin_enabled(self, event_data: PluginEventData) -> None:
        """Handle plugin enabled event."""
        pass
    
    def _on_plugin_disabled(self, event_data: PluginEventData) -> None:
        """Handle plugin disabled event."""
        pass
    
    def _on_plugin_unloaded(self, event_data: PluginEventData) -> None:
        """Handle plugin unloaded event."""
        pass
    
    def __str__(self) -> str:
        return f"{self.info.name} v{self.info.version} ({self.info.plugin_type.value})"
    
    def __repr__(self) -> str:
        return f"Plugin({self.info.name})"
class PluginLoader:
    """
    Plugin loader for NLM.
    
    This class handles loading, unloading, and managing plugins.
    """
    
    def __init__(self, plugin_directory: str = "plugins"):
        self.plugin_directory = plugin_directory
        self.loaded_plugins: Dict[str, PluginInterface] = {}
        self.plugin_info: Dict[str, PluginInfo] = {}
        self.plugin_lock = threading.Lock()
    
    def discover_plugins(self) -> List[str]:
        """
        Discover plugins in the plugin directory.
        
        Returns:
            List of plugin file paths
        """
        plugin_files = []
        
        if not os.path.exists(self.plugin_directory):
            os.makedirs(self.plugin_directory, exist_ok=True)
        
        # Walk through plugin directory
        for root, dirs, files in os.walk(self.plugin_directory):
            for file in files:
                if file.endswith(('.py', '.json', '.yaml', '.yml', '.toml')):
                    file_path = os.path.join(root, file)
                    
                    # Check if it's a plugin file
                    if self._is_plugin_file(file_path):
                        plugin_files.append(file_path)
        
        return plugin_files
    
    def _is_plugin_file(self, file_path: str) -> bool:
        """
        Check if file is a plugin.
        
        Args:
            file_path: Path to file
            
        Returns:
            True if file is a plugin, False otherwise
        """
        # Check for plugin metadata files
        filename = os.path.basename(file_path)
        
        # Python plugin files
        if filename.endswith('.py'):
            # Check if it's a Python module with plugin class
            try:
                module_name = filename[:-3]
                spec = importlib.util.spec_from_file_location(module_name, file_path)
                if spec is None or spec.loader is None:
                    return False
                
                module = importlib.util.module_from_spec(spec)
                spec.loader.exec_module(module)
                
                # Check if module has a PluginInfo or subclass of PluginInterface
                for attr_name in dir(module):
                    attr = getattr(module, attr_name)
                    if (isinstance(attr, type) and 
                        issubclass(attr, PluginInterface) and 
                        attr is not PluginInterface):
                        return True
            except Exception:
                pass
        
        # JSON/YAML/TOML metadata files
        elif filename.endswith(('.json', '.yaml', '.yml', '.toml')):
            try:
                if filename.endswith('.json'):
                    with open(file_path, 'r') as f:
                        data = json.load(f)
                elif filename.endswith(('.yaml', '.yml')):
                    with open(file_path, 'r') as f:
                        import yaml
                        data = yaml.safe_load(f)
                elif filename.endswith('.toml'):
                    with open(file_path, 'r') as f:
                        import toml
                        data = toml.load(f)
                
                # Check for plugin metadata
                if 'plugin_type' in data and 'version' in data:
                    return True
            except Exception:
                pass
        
        return False
    
    def load_plugin(self, file_path: str) -> Optional[PluginInterface]:
        """
        Load a plugin from file.
        
        Args:
            file_path: Path to plugin file
            
        Returns:
            Loaded plugin or None if loading failed
        """
        try:
            # Determine file type
            if file_path.endswith('.py'):
                return self._load_python_plugin(file_path)
            elif file_path.endswith(('.json', '.yaml', '.yml', '.toml')):
                return self._load_metadata_plugin(file_path)
            else:
                logger.error(f"Unsupported plugin file format: {file_path}")
                return None
        
        except Exception as e:
            logger.error(f"Failed to load plugin {file_path}: {e}")
            return None
    
    def _load_python_plugin(self, file_path: str) -> Optional[PluginInterface]:
        """Load Python plugin."""
        try:
            # Add plugin directory to path
            directory = os.path.dirname(file_path)
            if directory not in sys.path:
                sys.path.insert(0, directory)
            
            # Import the module
            module_name = os.path.splitext(os.path.basename(file_path))[0]
            spec = importlib.util.spec_from_file_location(module_name, file_path)
            if spec is None or spec.loader is None:
                raise ImportError(f"Could not load plugin from {file_path}")
            
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
            
            # Find plugin class
            plugin_class = None
            for attr_name in dir(module):
                attr = getattr(module, attr_name)
                if (isinstance(attr, type) and 
                    issubclass(attr, PluginInterface) and 
                    attr is not PluginInterface):
                    plugin_class = attr
                    break
            
            if plugin_class is None:
                raise ImportError(f"No plugin class found in {file_path}")
            
            # Create plugin instance
            plugin = plugin_class(plugin_class.info)
            
            return plugin
            
        except Exception as e:
            logger.error(f"Failed to load Python plugin {file_path}: {e}")
            return None
    
    def _load_metadata_plugin(self, file_path: str) -> Optional[PluginInterface]:
        """Load metadata plugin (placeholder for future implementation)."""
        logger.warning(f"Metadata plugin loading not yet implemented: {file_path}")
        return None
    
    def unload_plugin(self, plugin_name: str) -> bool:
        """
        Unload a plugin.
        
        Args:
            plugin_name: Name of plugin to unload
            
        Returns:
            True if plugin was unloaded, False otherwise
        """
        with self.plugin_lock:
            if plugin_name not in self.loaded_plugins:
                return False
            
            plugin = self.loaded_plugins[plugin_name]
            
            # Shutdown plugin
            try:
                plugin.shutdown()
            except Exception as e:
                logger.error(f"Error shutting down plugin {plugin_name}: {e}")
                return False
            
            # Remove from loaded plugins
            del self.loaded_plugins[plugin_name]
            if plugin_name in self.plugin_info:
                del self.plugin_info[plugin_name]
            
            return True
    
    def load_all_plugins(self) -> Dict[str, Any]:
        """
        Load all plugins from the plugin directory.
        
        Returns:
            Dictionary with loading results
        """
        result = {
            'loaded': [],
            'failed': [],
            'total': 0
        }
        
        plugin_files = self.discover_plugins()
        result['total'] = len(plugin_files)
        
        for file_path in plugin_files:
            try:
                plugin = self.load_plugin(file_path)
                if plugin:
                    plugin_name = plugin.info.name
                    self.loaded_plugins[plugin_name] = plugin
                    self.plugin_info[plugin_name] = plugin.info
                    
                    result['loaded'].append({
                        'name': plugin_name,
                        'file': file_path,
                        'version': plugin.info.version,
                        'type': plugin.info.plugin_type.value
                    })
                else:
                    result['failed'].append({
                        'file': file_path,
                        'reason': 'Failed to load plugin'
                    })
            except Exception as e:
                result['failed'].append({
                    'file': file_path,
                    'reason': str(e)
                })
        
        return result
class NLMPluginSystem:
    """
    NLM plugin system.
    
    This class provides the main interface for plugin management and
    event distribution in NLM.
    """
    
    def __init__(self, plugin_loader: Optional[PluginLoader] = None):
        self.plugin_loader = plugin_loader or PluginLoader()
        self.event_handlers: Dict[PluginEvent, List[Callable]] = defaultdict(list)
        self.plugin_config: Dict[str, Any] = {}
        self.system_initialized = False
        
        # Register for system events
        self._register_system_event_handlers()
    
    def _register_system_event_handlers(self) -> None:
        """Register handlers for system events."""
        pass
    
    def initialize(self, configuration: Dict[str, Any]) -> bool:
        """
        Initialize the plugin system.
        
        Args:
            configuration: System configuration
            
        Returns:
            True if initialization succeeded, False otherwise
        """
        try:
            self.plugin_config = configuration
            self.system_initialized = True
            
            # Load plugins
            load_result = self.plugin_loader.load_all_plugins()
            
            # Initialize plugins
            for plugin_name, plugin in self.plugin_loader.loaded_plugins.items():
                try:
                    plugin.initialize(self, self.plugin_config.get('plugins', {}).get(plugin_name, {}))
                    plugin.emit_event(PluginEvent.PLUGIN_LOADED)
                except Exception as e:
                    logger.error(f"Failed to initialize plugin {plugin_name}: {e}")
                    plugin.info.status = PluginStatus.ERROR
            
            return True
            
        except Exception as e:
            logger.error(f"Failed to initialize NLM plugin system: {e}")
            return False
    
    def shutdown(self) -> bool:
        """
        Shutdown the plugin system.
        
        Returns:
            True if shutdown succeeded, False otherwise
        """
        try:
            # Stop all plugins
            for plugin_name, plugin in self.plugin_loader.loaded_plugins.items():
                try:
                    plugin.shutdown()
                    plugin.emit_event(PluginEvent.PLUGIN_UNLOADED)
                except Exception as e:
                    logger.error(f"Error shutting down plugin {plugin_name}: {e}")
            
            # Clear plugin data
            self.plugin_loader.loaded_plugins.clear()
            self.plugin_loader.plugin_info.clear()
            self.system_initialized = False
            
            return True
            
        except Exception as e:
            logger.error(f"Error shutting down NLM plugin system: {e}")
            return False
    
    def emit_event(self, event: PluginEvent, data: Dict[str, Any] = None) -> None:
        """
        Emit an event to all plugins.
        
        Args:
            event: Event type
            data: Event data
        """
        if data is None:
            data = {}
        
        # Emit to system event handlers
        if event in self.event_handlers:
            for handler in self.event_handlers[event]:
                try:
                    handler(PluginEventData(event, data))
                except Exception as e:
                    logger.error(f"Error in system event handler for {event}: {e}")
        
        # Emit to plugin event handlers
        for plugin in self.plugin_loader.loaded_plugins.values():
            try:
                plugin.emit_event(event, data)
            except Exception as e:
                logger.error(f"Error in plugin event handler for {event}: {e}")
    
    def register_event_handler(self, event: PluginEvent, 
                              handler: Callable[[PluginEventData], None]) -> None:
        """
        Register a system event handler.
        
        Args:
            event: Event type
            handler: Event handler function
        """
        if event not in self.event_handlers:
            self.event_handlers[event] = []
        
        self.event_handlers[event].append(handler)
    
    def get_plugin(self, plugin_name: str) -> Optional[PluginInterface]:
        """
        Get a plugin by name.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            Plugin or None if not found
        """
        return self.plugin_loader.loaded_plugins.get(plugin_name)
    
    def enable_plugin(self, plugin_name: str) -> bool:
        """
        Enable a plugin.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            True if plugin was enabled, False otherwise
        """
        plugin = self.get_plugin(plugin_name)
        if plugin:
            plugin.enable()
            return True
        return False
    
    def disable_plugin(self, plugin_name: str) -> bool:
        """
        Disable a plugin.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            True if plugin was disabled, False otherwise
        """
        plugin = self.get_plugin(plugin_name)
        if plugin:
            plugin.disable()
            return True
        return False
    
    def list_plugins(self) -> List[Dict[str, Any]]:
        """
        List all loaded plugins.
        
        Returns:
            List of plugin information
        """
        plugins_list = []
        
        for plugin_name, plugin in self.plugin_loader.loaded_plugins.items():
            plugins_list.append({
                'name': plugin_name,
                'version': plugin.info.version,
                'description': plugin.info.description,
                'author': plugin.info.author,
                'type': plugin.info.plugin_type.value,
                'capabilities': [cap.value for cap in plugin.info.capabilities],
                'enabled': plugin.is_enabled(),
                'initialized': plugin.is_initialized(),
                'status': plugin.info.status.value
            })
        
        return plugins_list
class PluginScriptingSystem:
    """
    Plugin scripting system.
    
    This class provides scripting capabilities for NLM plugins,
    including script execution, script management, and script-based plugins.
    """
    
    def __init__(self, scripts_directory: str = "scripts"):
        self.scripts_directory = scripts_directory
        self.loaded_scripts: Dict[str, Dict[str, Any]] = {}
        self.script_handlers: Dict[str, Callable] = {}
        
        # Create scripts directory if it doesn't exist
        if not os.path.exists(self.scripts_directory):
            os.makedirs(self.scripts_directory, exist_ok=True)
    
    def load_script(self, script_name: str, script_path: str,
                   script_type: str = "python", 
                   dependencies: Optional[List[str]] = None) -> bool:
        """
        Load a script.
        
        Args:
            script_name: Name of the script
            script_path: Path to the script
            script_type: Type of script (python, javascript, etc.)
            dependencies: List of script dependencies
            
        Returns:
            True if script was loaded successfully, False otherwise
        """
        try:
            if not os.path.exists(script_path):
                logger.error(f"Script file not found: {script_path}")
                return False
            
            script_info = {
                'name': script_name,
                'path': script_path,
                'type': script_type,
                'dependencies': dependencies or [],
                'loaded_at': time.time(),
                'last_modified': os.path.getmtime(script_path)
            }
            
            # Load script based on type
            if script_type == 'python':
                self._load_python_script(script_info)
            else:
                logger.warning(f"Script type '{script_type}' not yet implemented")
            
            self.loaded_scripts[script_name] = script_info
            return True
            
        except Exception as e:
            logger.error(f"Failed to load script {script_name}: {e}")
            return False
    
    def _load_python_script(self, script_info: Dict[str, Any]) -> None:
        """Load Python script."""
        try:
            script_path = script_info['path']
            script_name = script_info['name']
            
            # Add script directory to path
            directory = os.path.dirname(script_path)
            if directory not in sys.path:
                sys.path.insert(0, directory)
            
            # Import the module
            module_name = script_name
            spec = importlib.util.spec_from_file_location(module_name, script_path)
            if spec is None or spec.loader is None:
                raise ImportError(f"Could not load script {script_path}")
            
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
            
            # Store module reference
            script_info['module'] = module
            
            # Check for script handlers
            for attr_name in dir(module):
                if attr_name.startswith('on_'):
                    event_type = attr_name[3:]  # Remove 'on_' prefix
                    try:
                        event_enum = PluginEvent(event_type)
                        handler = getattr(module, attr_name)
                        self.script_handlers[event_type] = handler
                    except ValueError:
                        pass  # Not a valid event type
            
        except Exception as e:
            logger.error(f"Failed to load Python script {script_info['name']}: {e}")
            raise
    
    def execute_script(self, script_name: str, *args, **kwargs) -> Any:
        """
        Execute a script.
        
        Args:
            script_name: Name of the script to execute
            *args: Arguments to pass to script
            **kwargs: Keyword arguments to pass to script
            
        Returns:
            Script result
        """
        if script_name not in self.loaded_scripts:
            logger.error(f"Script not found: {script_name}")
            return None
        
        script_info = self.loaded_scripts[script_name]
        
        if 'module' not in script_info:
            logger.error(f"Script not loaded: {script_name}")
            return None
        
        module = script_info['module']
        
        # Execute script function
        try:
            if hasattr(module, 'execute'):
                return module.execute(*args, **kwargs)
            elif hasattr(module, script_name):
                return getattr(module, script_name)(*args, **kwargs)
            else:
                logger.error(f"Script {script_name} has no execute function")
                return None
        
        except Exception as e:
            logger.error(f"Error executing script {script_name}: {e}")
            return None
    
    def unload_script(self, script_name: str) -> bool:
        """
        Unload a script.
        
        Args:
            script_name: Name of script to unload
            
        Returns:
            True if script was unloaded, False otherwise
        """
        if script_name not in self.loaded_scripts:
            return False
        
        script_info = self.loaded_scripts[script_name]
        
        # Clean up module
        if 'module' in script_info:
            module = script_info['module']
            
            # Remove from sys.modules
            for attr_name in dir(module):
                if attr_name.startswith('on_'):
                    event_type = attr_name[3:]
                    if event_type in self.script_handlers:
                        del self.script_handlers[event_type]
        
        # Remove script info
        del self.loaded_scripts[script_name]
        return True
    
    def list_scripts(self) -> List[Dict[str, Any]]:
        """
        List all loaded scripts.
        
        Returns:
            List of script information
        """
        scripts_list = []
        
        for script_name, script_info in self.loaded_scripts.items():
            scripts_list.append({
                'name': script_name,
                'path': script_info['path'],
                'type': script_info['type'],
                'dependencies': script_info.get('dependencies', []),
                'loaded_at': script_info['loaded_at'],
                'last_modified': script_info['last_modified']
            })
        
        return scripts_list
class NLMScriptingInterface:
    """
    NLM scripting interface.
    
    This class provides scripting capabilities for NLM, allowing users to
    write scripts that interact with NLM functionality.
    """
    
    def __init__(self, plugin_system: Optional[NLMPluginSystem] = None,
                 scripting_system: Optional[PluginScriptingSystem] = None):
        self.plugin_system = plugin_system or NLMPluginSystem()
        self.scripting_system = scripting_system or PluginScriptingSystem()
    
    def execute_script(self, script_name: str, *args, **kwargs) -> Any:
        """
        Execute a script.
        
        Args:
            script_name: Name of script to execute
            *args: Arguments to pass to script
            **kwargs: Keyword arguments to pass to script
            
        Returns:
            Script result
        """
        return self.scripting_system.execute_script(script_name, *args, **kwargs)
    
    def load_script(self, script_name: str, script_path: str,
                   script_type: str = "python",
                   dependencies: Optional[List[str]] = None) -> bool:
        """
        Load a script.
n        Args:
            script_name: Name of the script
            script_path: Path to the script
            script_type: Type of script (python, javascript, etc.)
            dependencies: List of script dependencies
            
        Returns:
            True if script was loaded successfully, False otherwise
        """
        return self.scripting_system.load_script(script_name, script_path,
                                               script_type, dependencies)
    
    def unload_script(self, script_name: str) -> bool:
        """
        Unload a script.
        
        Args:
            script_name: Name of script to unload
            
        Returns:
            True if script was unloaded, False otherwise
        """
        return self.scripting_system.unload_script(script_name)
    
    def list_scripts(self) -> List[Dict[str, Any]]:
        """
        List all loaded scripts.
        
        Returns:
            List of script information
        """
        return self.scripting_system.list_scripts()
    
    def initialize(self, configuration: Dict[str, Any]) -> bool:
        """
        Initialize scripting interface.
        
        Args:
            configuration: Configuration
            
        Returns:
            True if initialization succeeded, False otherwise
        """
        # Initialize plugin system
        if not self.plugin_system.initialize(configuration):
            logger.error("Failed to initialize plugin system")
            return False
        
        return True
    
    def shutdown(self) -> bool:
        """
        Shutdown scripting interface.
        
        Returns:
            True if shutdown succeeded, False otherwise
        """
        # Shutdown plugin system
        if not self.plugin_system.shutdown():
            logger.error("Failed to shutdown plugin system")
            return False
        
        return True
# Convenience functions for common scripting tasks

def create_simulation_script(neuron_count: int = 1000, steps: int = 1000,
                             output_file: Optional[str] = None) -> str:
    """
    Create a simple simulation script.
    
    Args:
        neuron_count: Number of neurons
        steps: Number of simulation steps
        output_file: Output file path
        
    Returns:
        Path to created script
    """
    script_content = f'''# NLM Simulation Script
import time
import pynlm

def simulate_nlm(neuron_count={neuron_count}, steps={steps}):
    """Run NLM simulation."""
    print(f"Starting NLM simulation with {{neuron_count}} neurons and {{steps}} steps")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", neuron_count)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.initialize(10, 10)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    # Run simulation
    start_time = time.time()
    total_reward = 0.0
    
    for step in range(steps):
        # Get observation
        percept = world.observe(brain.getRegions()[0])
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step, step * 0.001)
        
        # Get action
        cmd = agent.decodeMotorCommand()
        
        # Apply action
        world.applyAction(brain.getRegions()[0], cmd)
        
        # Compute reward
        reward = world.computeReward(brain.getRegions()[0])
        total_reward += reward
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.001)
        
        if step % 100 == 0:
            print(f"Step {{step}}: Reward={{total_reward / (step + 1):.4f}}, Firing={{brain.getAverageFiringRate():.2f}}")
    
    end_time = time.time()
    
    # Calculate results
    avg_reward = total_reward / steps
    duration = end_time - start_time
    
    print(f"Simulation completed:")
    print(f"  Average reward: {{avg_reward:.4f}}")
    print(f"  Duration: {{duration:.2f}} seconds")
    print(f"  Steps per second: {{steps / duration:.1f}}")
    print(f"  Final developmental stage: {{agent.getDevelopmentalStage()}}")
    
    # Save results if output file specified
    if "{output_file}":
        results = {{
            'average_reward': avg_reward,
            'duration': duration,
            'steps_per_second': steps / duration,
            'final_developmental_stage': agent.getDevelopmentalStage(),
            'neuron_count': neuron_count,
            'steps': steps
        }}
        
        with open("{output_file}", 'w') as f:
            import json
            json.dump(results, f, indent=2)
        
        print(f"Results saved to {{output_file}}")
    
    return {{
        'average_reward': avg_reward,
        'duration': duration,
        'steps_per_second': steps / duration,
        'final_developmental_stage': agent.getDevelopmentalStage()
    }}

if __name__ == "__main__":
    # Run simulation
    results = simulate_nlm()
    
    print("Script execution completed successfully")
'''
    
    # Create script file
    script_path = f"{script_name}_simulation.py"
    with open(script_path, 'w') as f:
        f.write(script_content)
    
    return script_path

def create_analysis_script(analysis_type: str = "temporal",
                          input_file: Optional[str] = None,
                          output_file: Optional[str] = None) -> str:
    """
    Create a simple analysis script.
    
    Args:
        analysis_type: Type of analysis ("temporal", "spatial", "statistical")
        input_file: Input data file path
        output_file: Output file path
        
    Returns:
        Path to created script
    """
    script_content = f'''# NLM Analysis Script
import numpy as np
import json
import matplotlib.pyplot as plt

def analyze_data(data_file=None, analysis_type="{analysis_type}"):
    """Analyze NLM data."""
    print(f"Starting analysis of type '{{analysis_type}}'")
    
    # Load data
    if data_file:
        with open(data_file, 'r') as f:
            data = json.load(f)
    else:
        # Generate sample data
        np.random.seed(42)
        time_steps = 1000
        signal_data = np.random.randn(time_steps) * 0.1 + 0.5
        data = {{
            'time_steps': time_steps,
            'signal_data': signal_data.tolist()
        }}
    
    # Perform analysis based on type
    if analysis_type == "temporal":
        # Temporal analysis
        signal = np.array(data['signal_data'])
        
        # Calculate autocorrelation
        autocorr = np.correlate(signal - np.mean(signal), signal - np.mean(signal), mode='full')
        autocorr = autocorr[len(autocorr)//2:len(autocorr)//2 + 500]
        autocorr = autocorr / autocorr[0]
        
        # Find characteristic time
        half_life = 0
        for i, corr in enumerate(autocorr[1:], 1):
            if abs(corr) <= 0.5:
                half_life = i
                break
        
        analysis_results = {{
            'signal_length': len(signal),
            'signal_mean': float(np.mean(signal)),
            'signal_std': float(np.std(signal)),
            'autocorrelation': autocorr.tolist(),
            'characteristic_time': half_life,
            'analysis_type': analysis_type
        }}
        
    elif analysis_type == "spatial":
        # Spatial analysis (placeholder)
        analysis_results = {{
            'network_diameter': 5.2,
            'average_path_length': 2.8,
            'clustering_coefficient': 0.4,
            'analysis_type': analysis_type
        }}
        
    elif analysis_type == "statistical":
        # Statistical analysis
        signal = np.array(data['signal_data'])
        
        from scipy import stats
        
        analysis_results = {{
            'signal_length': len(signal),
            'mean': float(np.mean(signal)),
            'std': float(np.std(signal)),
            'skewness': float(stats.skew(signal)),
            'kurtosis': float(stats.kurtosis(signal)),
            'min': float(np.min(signal)),
            'max': float(np.max(signal)),
            'median': float(np.median(signal)),
            'percentiles': {{
                '25': float(np.percentile(signal, 25)),
                '50': float(np.percentile(signal, 50)),
                '75': float(np.percentile(signal, 75))
            }},
            'analysis_type': analysis_type
        }}
    
    # Save results
    if output_file:
        with open(output_file, 'w') as f:
            json.dump(analysis_results, f, indent=2)
        print(f"Analysis results saved to {{output_file}}")
    
    return analysis_results

if __name__ == "__main__":
    # Run analysis
    results = analyze_data()
    
    print("Analysis script completed successfully")
    print(f"Analysis results: {{json.dumps(results, indent=2)}}")
'''
    
    # Create script file
    script_path = f"{script_name}_analysis.py"
    with open(script_path, 'w') as f:
        f.write(script_content)
    
    return script_path
# Export key classes and functions
__all__ = [
    'PluginType',
    'PluginStatus',
    'PluginCapability',
    'PluginEvent',
    'PluginInfo',
    'PluginEventData',
    'PluginInterface',
    'PluginLoader',
    'NLMPluginSystem',
    'PluginScriptingSystem',
    'NLMScriptingInterface',
    'create_simulation_script',
    'create_analysis_script',
    'main_cli'
]
