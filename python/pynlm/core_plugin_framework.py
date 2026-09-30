# Core Plugin Framework

This module implements the enhanced core plugin framework with PluginManager, security sandboxing, dependency management, and lifecycle monitoring.

import importlib
import inspect
import threading
import time
import os
import json
import pickle
import hashlib
import secrets
import tempfile
import shutil
from typing import Dict, List, Any, Optional, Callable, Type, Set, Union, Tuple
from dataclasses import dataclass, field
from enum import Enum
from abc import ABC, abstractmethod
from concurrent.futures import ThreadPoolExecutor
import logging

from pynlm.interface import ScriptingInterface, ScriptingEvent, EventType
from .plugin_system import Plugin, PluginInfo, PluginType, PluginStatus, PluginError


class SecurityLevel(Enum):
    """Plugin security levels"""
    LOW = "low"
    MEDIUM = "medium"
    HIGH = "high"
    CRITICAL = "critical"


class ExecutionMode(Enum):
    """Plugin execution modes"""
    SANDBOXED = "sandboxed"
    TRUSTED = "trusted"
    RESTRICTED = "restricted"
    FULL_ACCESS = "full_access"


@dataclass
class RuntimeRequirements:
    """Runtime requirements for plugins"""
    min_python_version: str = "3.8"
    required_modules: List[str] = field(default_factory=list)
    memory_limit: int = 512  # MB
    cpu_limit: float = 1.0  # CPU cores
    network_access: bool = False
    file_system_access: bool = False
    gpu_required: bool = False
    custom_requirements: Dict[str, Any] = field(default_factory=dict)


@dataclass
class CompatibilityInfo:
    """Plugin compatibility information"""
    compatible_nlm_versions: List[str] = field(default_factory=lambda: ["1.0.0"])
    incompatible_nlm_versions: List[str] = field(default_factory=list)
    recommended_runtime: str = "standard"
    alternative_plugins: List[str] = field(default_factory=list)
    notes: List[str] = field(default_factory=list)


class SecurityPolicy:
    """Security policy for plugins"""
    
    def __init__(self, policy_type: str = "default"):
        self.policy_type = policy_type
        self.allowed_operations: Set[str] = set()
        self.allowed_files: Set[str] = set()
        self.allowed_network_ports: Set[int] = set()
        self.allowed_modules: Set[str] = set()
        self.denied_operations: Set[str] = set()
        self.resource_limits: Dict[str, Any] = {}
        self.permissions: Dict[str, bool] = {}
        
    def load_from_file(self, filepath: str):
        """Load security policy from JSON file"""
        with open(filepath, 'r') as f:
            policy_data = json.load(f)
        
        self.allowed_operations = set(policy_data.get('allowed_operations', []))
        self.allowed_files = set(policy_data.get('allowed_files', []))
        self.allowed_network_ports = set(policy_data.get('allowed_network_ports', []))
        self.allowed_modules = set(policy_data.get('allowed_modules', []))
        self.denied_operations = set(policy_data.get('denied_operations', []))
        self.resource_limits = policy_data.get('resource_limits', {})
        self.permissions = policy_data.get('permissions', {})


class SandboxEnvironment:
    """Security sandbox environment for plugins"""
    
    def __init__(self, policy: SecurityPolicy, workspace: str):
        self.policy = policy
        self.workspace = workspace
        self.isolation_id = secrets.token_hex(16)
        self.resource_usage = {}
        self.active_processes = []
        self.file_handles = {}
        self.network_connections = {}
        self.memory_usage = 0
        self.start_time = time.time()
        
    def execute_code(self, code: str, context: Dict[str, Any] = None) -> Any:
        """Execute code in sandbox"""
        # In a real implementation, this would use proper sandboxing
        # For now, we'll implement a simplified version
        
        # Check resource limits
        if self.memory_usage > self.policy.resource_limits.get('memory_mb', 512) * 1024 * 1024:
            raise SandboxSecurityError("Memory limit exceeded")
            
        # Check allowed operations
        for operation in self.detect_operations(code):
            if operation in self.policy.denied_operations:
                raise SandboxSecurityError(f"Operation not allowed: {operation}")
                
        # Check file access
        for file_path in self.detect_file_access(code):
            if not self._check_file_access(file_path):
                raise SandboxSecurityError(f"File access not allowed: {file_path}")
        
        # Execute code
        try:
            # Create isolated context
            isolated_context = self._create_isolated_context(context)
            
            # Execute in sandbox
            exec(code, isolated_context)
            
            return isolated_context.get('result')
        except Exception as e:
            raise SandboxSecurityError(f"Sandbox execution failed: {e}")
    
    def _check_file_access(self, filepath: str) -> bool:
        """Check if file access is allowed"""
        # Normalize path
        filepath = os.path.abspath(filepath)
        
        # Check against allowed files
        for allowed in self.policy.allowed_files:
            if filepath.startswith(allowed):
                return True
                
        return False
    
    def _create_isolated_context(self, context: Dict[str, Any]) -> Dict[str, Any]:
        """Create isolated context for sandbox execution"""
        isolated = {
            'os': os,
            'sys': sys,
            'json': json,
            'time': time,
            'threading': threading,
            'secrets': secrets,
        }
        
        # Add allowed modules
        for module_name in self.policy.allowed_modules:
            try:
                if module_name == 'numpy':
                    import numpy
                    isolated['numpy'] = numpy
                elif module_name == 'pandas':
                    import pandas
                    isolated['pandas'] = pandas
                elif module_name == 'scipy':
                    import scipy
                    isolated['scipy'] = scipy
                # Add other modules as needed
            except ImportError:
                pass
        
        # Add user context
        if context:
            isolated.update(context)
            
        return isolated
    
    def detect_operations(self, code: str) -> Set[str]:
        """Detect operations in code"""
        operations = set()
        
        # Simple detection - in real implementation, would use AST parsing
        if 'import ' in code:
            operations.add('import')
        if 'open(' in code:
            operations.add('file_read')
        if 'write(' in code:
            operations.add('file_write')
        if 'socket' in code:
            operations.add('network')
        if 'subprocess' in code:
            operations.add('process_execution')
        if 'eval(' in code:
            operations.add('code_execution')
            
        return operations
    
    def detect_file_access(self, code: str) -> Set[str]:
        """Detect file access in code"""
        files = set()
        
        # Simple detection - in real implementation, would use AST parsing
        import re
        file_pattern = r'["\']([^"\']+\.[a-zA-Z0-9]+)["\']'
        matches = re.findall(file_pattern, code)
        for match in matches:
            files.add(match)
            
        return files


class SandboxSecurityError(Exception):
    """Exception raised by sandbox security violations"""
    pass


class DependencyResolver:
    """Resolve plugin dependencies"""
    
    def __init__(self, plugin_registry=None):
        self.plugin_registry = plugin_registry or {}
        self.dependency_graph = {}
        self.installed_plugins = {}
        
    def add_plugin(self, plugin_info: PluginInfo):
        """Add plugin to registry"""
        self.plugin_registry[plugin_info.name] = plugin_info
        self.dependency_graph[plugin_info.name] = {
            'dependencies': plugin_info.dependencies or [],
            'required_plugins': plugin_info.required_plugins or [],
            'conflicts': [],
            'children': [],
            'parents': []
        }
        
    def resolve_dependencies(self, plugin_name: str) -> List[str]:
        """Resolve plugin dependencies"""
        if plugin_name not in self.dependency_graph:
            return []
            
        graph = self.dependency_graph
        visited = set()
        result = []
        
        def dfs(node):
            if node in visited:
                return
            visited.add(node)
            
            if node in graph:
                for dep in graph[node]['dependencies'] + graph[node]['required_plugins']:
                    if dep not in self.installed_plugins:
                        raise DependencyError(f"Plugin {dep} is required by {plugin_name} but not installed")
                    dfs(dep)
                    if dep not in result:
                        result.append(dep)
        
        dfs(plugin_name)
        return result
    
    def detect_conflicts(self, plugin_specs: List[str]) -> List[Tuple[str, str]]:
        """Detect conflicts between plugins"""
        conflicts = []
        
        # Check for direct conflicts
        for i, plugin1 in enumerate(plugin_specs):
            for j, plugin2 in enumerate(plugin_specs):
                if i >= j:
                    continue
                    
                # Get plugin info
                info1 = self.plugin_registry.get(plugin1)
                info2 = self.plugin_registry.get(plugin2)
                
                if not info1 or not info2:
                    continue
                    
                # Check for conflicts
                if hasattr(info1, 'conflicts') and plugin2 in info1.conflicts:
                    conflicts.append((plugin1, plugin2))
                if hasattr(info2, 'conflicts') and plugin1 in info2.conflicts:
                    conflicts.append((plugin2, plugin1))
        
        return conflicts


class DependencyError(Exception):
    """Exception raised for dependency errors"""
    pass


class PluginConflictError(Exception):
    """Exception raised for plugin conflicts"""
    pass


class EventBus:
    """Event bus for plugin communication"""
    
    def __init__(self):
        self.subscribers = {}
        self.event_history = []
        self.event_lock = threading.Lock()
        
    def subscribe(self, event_type: EventType, handler: Callable, plugin_name: str = None):
        """Subscribe to events"""
        with self.event_lock:
            if event_type not in self.subscribers:
                self.subscribers[event_type] = {}
            
            if plugin_name:
                self.subscribers[event_type][plugin_name] = handler
            else:
                # Add as wildcard subscriber
                if 'wildcard' not in self.subscribers[event_type]:
                    self.subscribers[event_type]['wildcard'] = []
                self.subscribers[event_type]['wildcard'].append(handler)
    
    def unsubscribe(self, event_type: EventType, plugin_name: str = None, handler: Callable = None):
        """Unsubscribe from events"""
        with self.event_lock:
            if event_type not in self.subscribers:
                return
                
            if plugin_name:
                if plugin_name in self.subscribers[event_type]:
                    if handler:
                        # Remove specific handler
                        handlers = self.subscribers[event_type][plugin_name]
                        if isinstance(handlers, list) and handler in handlers:
                            handlers.remove(handler)
                        elif handlers == handler:
                            del self.subscribers[event_type][plugin_name]
                    else:
                        # Remove all handlers for plugin
                        del self.subscribers[event_type][plugin_name]
            else:
                # Remove wildcard handlers
                if handler:
                    if 'wildcard' in self.subscribers[event_type]:
                        handlers = self.subscribers[event_type]['wildcard']
                        if handler in handlers:
                            handlers.remove(handler)
                else:
                    # Remove all handlers for event type
                    del self.subscribers[event_type]
    
    def publish(self, event: ScriptingEvent, target_plugins: List[str] = None):
        """Publish event to plugins"""
        with self.event_lock:
            # Get target plugins
            if target_plugins is None:
                # Publish to all plugins
                target_plugins = list(self.subscribers.keys())
            
            # Filter subscribers by event type and target plugins
            event_subscribers = {}
            for event_type in target_plugins:
                if event_type in self.subscribers:
                    event_subscribers[event_type] = self.subscribers[event_type]
            
            # Publish to all matching subscribers
            for event_type, subscribers in event_subscribers.items():
                # Process wildcard handlers
                if 'wildcard' in subscribers:
                    for handler in subscribers['wildcard']:
                        try:
                            handler(event)
                        except Exception as e:
                            logging.error(f"Event handler error: {e}")
                
                # Process specific plugin handlers
                for plugin_name, handler in subscribers.items():
                    if plugin_name == 'wildcard':
                        continue
                    try:
                        handler(event)
                    except Exception as e:
                        logging.error(f"Event handler error: {e}")
            
            # Store event history
            self.event_history.append(event)
            if len(self.event_history) > 1000:  # Limit history size
                self.event_history.pop(0)


class EnhancedPluginManager:
    """Enhanced plugin manager with advanced features"""
    
    def __init__(self, interface: ScriptingInterface, config: Dict[str, Any] = None):
        self.interface = interface
        self.context = interface.context
        self.config = config or {}
        
        # Core components
        self.plugins: Dict[str, Plugin] = {}
        self.plugin_infos: Dict[str, PluginInfo] = {}
        self.plugin_paths = {}
        
        # Advanced components
        self.event_bus = EventBus()
        self.security_manager = SecurityManager(self.config)
        self.dependency_resolver = DependencyResolver()
        self.lifecycle_monitor = LifecycleMonitor()
        self.state_manager = StateManager()
        self.coordination_manager = CoordinationManager()
        self.performance_monitor = PerformanceMonitor()
        
        # Configuration
        self.plugin_registry = 'https://plugins.pynlm.org'
        self.auto_load_plugins = self.config.get('auto_load_plugins', True)
        self.enable_hot_reloading = self.config.get('enable_hot_reloading', True)
        self.security_level = SecurityLevel(self.config.get('security_level', 'medium'))
        self.max_concurrent_plugins = self.config.get('max_concurrent_plugins', 50)
        self.plugin_cache_dir = self.config.get('plugin_cache_dir', 
                                               os.path.join(os.getcwd(), '.pynlm_cache'))
        
        # Create cache directory
        os.makedirs(self.plugin_cache_dir, exist_ok=True)
        
        # Performance monitoring
        self.plugin_load_times = {}
        self.plugin_unload_times = {}
        
    def load_plugin(self, plugin_name: str, plugin_path: Optional[str] = None,
                   config: Optional[Dict] = None) -> Plugin:
        """Load a plugin with advanced features"""
        # Check if plugin is already loaded
        if plugin_name in self.plugins:
            if self.plugins[plugin_name].info.status == PluginStatus.LOADED:
                return self.plugins[plugin_name]
            else:
                # Plugin exists but needs to be reloaded
                self.unload_plugin(plugin_name)
        
        # Resolve dependencies
        start_time = time.time()
        
        try:
            # Get plugin info with advanced discovery
            plugin_info = self._get_advanced_plugin_info(plugin_name, plugin_path)
            
            # Validate plugin
            self._validate_plugin(plugin_info)
            
            # Create sandbox environment
            sandbox = self.security_manager.create_sandbox(plugin_info)
            
            # Create plugin instance with sandbox
            plugin = self._create_plugin_with_sandbox(plugin_info, sandbox)
            
            # Initialize plugin
            plugin.initialize(self.interface, config)
            
            # Enable plugin
            plugin.enable()
            
            # Register event handlers
            self._register_plugin_events(plugin)
            
            # Store plugin
            self.plugins[plugin_name] = plugin
            self.plugin_infos[plugin_name] = plugin_info
            self.plugin_paths[plugin_name] = plugin_path
            
            # Update dependency resolver
            self.dependency_resolver.add_plugin(plugin_info)
            self.dependency_resolver.installed_plugins[plugin_name] = plugin
            
            # Record performance
            load_time = time.time() - start_time
            self.plugin_load_times[plugin_name] = load_time
            self.performance_monitor.record_plugin_load(plugin_name, load_time)
            
            # Log
            logging.info(f"Enhanced plugin loaded: {plugin_name} ({plugin_info.version}) "
                        f"in {load_time:.3f}s")
            
            return plugin
            
        except Exception as e:
            # Record failure
            load_time = time.time() - start_time
            self.performance_monitor.record_plugin_load_failure(plugin_name, load_time)
            logging.error(f"Failed to load plugin {plugin_name}: {e}")
            raise
    
    def unload_plugin(self, plugin_name: str) -> bool:
        """Unload a plugin gracefully"""
        if plugin_name not in self.plugins:
            return False
        
        plugin = self.plugins[plugin_name]
        
        try:
            # Notify dependent plugins
            self._notify_plugin_unload(plugin)
            
            # Stop plugin
            plugin.disable()
            
            # Unload plugin
            plugin.unload()
            
            # Remove from registry
            del self.plugins[plugin_name]
            del self.plugin_infos[plugin_name]
            
            if plugin_name in self.plugin_paths:
                del self.plugin_paths[plugin_name]
            
            # Update dependency resolver
            if plugin_name in self.dependency_resolver.installed_plugins:
                del self.dependency_resolver.installed_plugins[plugin_name]
            
            # Record performance
            unload_time = time.time() - (self.plugin_load_times.get(plugin_name, time.time()))
            self.plugin_unload_times[plugin_name] = unload_time
            self.performance_monitor.record_plugin_unload(plugin_name, unload_time)
            
            # Log
            logging.info(f"Enhanced plugin unloaded: {plugin_name}")
            
            return True
            
        except Exception as e:
            logging.error(f"Error unloading plugin {plugin_name}: {e}")
            return False
    
    def hot_reload_plugin(self, plugin_name: str, plugin_path: Optional[str] = None) -> bool:
        """Hot reload a plugin without system restart"""
        if plugin_name not in self.plugins:
            return False
        
        try:
            # Unload current plugin
            self.unload_plugin(plugin_name)
            
            # Reload plugin with optional new path
            self.load_plugin(plugin_name, plugin_path)
            
            # Log
            logging.info(f"Enhanced plugin hot reloaded: {plugin_name}")
            
            return True
            
        except Exception as e:
            logging.error(f"Failed to hot reload plugin {plugin_name}: {e}")
            # Try to restore original plugin
            if plugin_name in self.plugin_paths:
                original_path = self.plugin_paths[plugin_name]
                try:
                    self.load_plugin(plugin_name, original_path)
                    logging.info(f"Successfully restored original plugin: {plugin_name}")
                except:
                    logging.error(f"Failed to restore original plugin: {plugin_name}")
            return False
    
    def get_plugin(self, plugin_name: str) -> Optional[Plugin]:
        """Get a loaded plugin"""
        return self.plugins.get(plugin_name)
    
    def list_plugins(self) -> List[PluginInfo]:
        """List all loaded plugins"""
        return list(self.plugin_infos.values())
    
    def search_plugins(self, query: str, filters: Optional[Dict] = None) -> List[PluginInfo]:
        """Search for plugins with advanced filtering"""
        results = []
        
        for plugin_info in self.plugin_infos.values():
            # Basic text search
            name_match = query.lower() in plugin_info.name.lower()
            desc_match = query.lower() in plugin_info.description.lower()
            
            if not (name_match or desc_match):
                continue
            
            # Apply filters
            if filters:
                if 'plugin_type' in filters and plugin_info.plugin_type != filters['plugin_type']:
                    continue
                if 'category' in filters and plugin_info.metadata and plugin_info.metadata.get('category') != filters['category']:
                    continue
                if 'tags' in filters:
                    plugin_tags = plugin_info.metadata.get('tags', []) if plugin_info.metadata else []
                    if not any(tag in plugin_tags for tag in filters['tags']):
                        continue
            
            results.append(plugin_info)
        
        return results
    
    def install_plugin(self, plugin_spec: str) -> Plugin:
        """Install a plugin from specification"""
        if plugin_spec.startswith('file://'):
            # Local file
            plugin_path = plugin_spec[7:]
            plugin_name = os.path.splitext(os.path.basename(plugin_path))[0]
            return self.load_plugin(plugin_name, plugin_path)
        elif plugin_spec.startswith('github://'):
            # GitHub repository
            github_spec = plugin_spec[8:]
            # In a real implementation, this would clone the repository
            plugin_name = github_spec.split('/')[-1]
            raise NotImplementedError("GitHub plugin installation not yet implemented")
        elif plugin_spec.startswith('pypi://'):
            # PyPI package
            pypi_spec = plugin_spec[7:]
            # In a real implementation, this would install from PyPI
            plugin_name = pypi_spec.split('==')[0].split('@')[0]
            raise NotImplementedError("PyPI plugin installation not yet implemented")
        else:
            # Try to load as built-in or local plugin
            return self.load_plugin(plugin_spec)
    
    def create_custom_plugin(self, plugin_name: str, plugin_class: Type[Plugin],
                           config: Optional[Dict] = None) -> Plugin:
        """Create a custom plugin from a class"""
        plugin_info = PluginInfo(
            name=plugin_name,
            version='1.0.0',
            description=f"Custom plugin {plugin_name}",
            author='User',
            plugin_type=PluginType.EXPERIMENTAL,
            dependencies=[],
            required_plugins=[],
            metadata={'custom': True}
        )
        
        plugin = plugin_class(plugin_info)
        plugin.initialize(self.interface, config)
        plugin.enable()
        
        self.plugins[plugin_name] = plugin
        self.plugin_infos[plugin_name] = plugin_info
        
        logging.info(f"Custom plugin created: {plugin_name}")
        
        return plugin
    
    def _get_advanced_plugin_info(self, plugin_name: str, plugin_path: Optional[str] = None) -> PluginInfo:
        """Get advanced plugin information with rich metadata"""
        # Check if plugin is already loaded
        if plugin_name in self.plugin_infos:
            return self.plugin_infos[plugin_name]
        
        # Try to discover plugin with advanced methods
        plugin_info = self._advanced_discover_plugin(plugin_name, plugin_path)
        if plugin_info:
            return plugin_info
        
        # If not found, create enhanced info
        plugin_info = PluginInfo(
            name=plugin_name,
            version='0.1.0',
            description=f'Plugin {plugin_name}',
            author='Unknown',
            plugin_type=PluginType.UTILITY,
            dependencies=[],
            required_plugins=[],
            path=plugin_path,
            metadata={
                'discovered': True,
                'advanced_discovery': True
            }
        )
        
        return plugin_info
    
    def _advanced_discover_plugin(self, plugin_name: str, plugin_path: Optional[str] = None) -> Optional[PluginInfo]:
        """Advanced plugin discovery with multiple sources"""
        # Try to load from plugin_path first
        if plugin_path and os.path.exists(plugin_path):
            return self._load_plugin_from_file_advanced(plugin_path)
        
        # Try to load from built-in location
        for spec in pkgutil.iter_modules():
            if spec.name == plugin_name:
                return self._load_plugin_from_module_advanced(spec)
        
        # Try to find in pynlm.plugins directory
        try:
            plugins_module = importlib.import_module('pynlm.plugins')
            for attr_name in dir(plugins_module):
                if attr_name.startswith(plugin_name):
                    plugin_class = getattr(plugins_module, attr_name)
                    return self._create_plugin_info_from_class_advanced(plugin_class)
        except ImportError:
            pass
        
        return None
    
    def _load_plugin_from_file_advanced(self, filepath: str) -> PluginInfo:
        """Load plugin info from file with advanced parsing"""
        filename = os.path.basename(filepath)
        plugin_name = os.path.splitext(filename)[0]
        
        # Try to read plugin metadata from file
        metadata_path = os.path.splitext(filepath)[0] + '.meta.json'
        if os.path.exists(metadata_path):
            with open(metadata_path, 'r') as f:
                metadata = json.load(f)
        else:
            metadata = {}
        
        # Extract plugin type from metadata
        plugin_type = PluginType.UTILITY
        if 'plugin_type' in metadata:
            try:
                plugin_type = PluginType(metadata['plugin_type'])
            except ValueError:
                pass
        
        return PluginInfo(
            name=plugin_name,
            version=metadata.get('version', '1.0.0'),
            description=metadata.get('description', f'Plugin loaded from {filename}'),
            author=metadata.get('author', 'File-based'),
            plugin_type=plugin_type,
            dependencies=metadata.get('dependencies', []),
            required_plugins=metadata.get('required_plugins', []),
            path=filepath,
            metadata=metadata
        )
    
    def _load_plugin_from_module_advanced(self, spec) -> Optional[PluginInfo]:
        """Load plugin info from module with advanced detection"""
        try:
            module = importlib.import_module(spec.name)
            
            # Try to get enhanced metadata
            if hasattr(module, 'plugin_metadata'):
                return module.plugin_metadata
            
            # Try to find PluginInfo in module
            if hasattr(module, 'plugin_info'):
                return module.plugin_info
            
            # Try to find Plugin class with enhanced metadata
            if hasattr(module, 'Plugin'):
                plugin_class = module.Plugin
                return self._create_plugin_info_from_class_advanced(plugin_class)
                
        except Exception as e:
            logging.error(f"Error loading plugin from module {spec.name}: {e}")
            
        return None
    
    def _create_plugin_info_from_class_advanced(self, plugin_class: Type[Plugin]) -> Optional[PluginInfo]:
        """Create enhanced PluginInfo from Plugin class"""
        try:
            # Try to get enhanced metadata
            if hasattr(plugin_class, 'ENHANCED_PLUGIN_INFO'):
                return plugin_class.ENHANCED_PLUGIN_INFO
            
            # Try to get plugin info
            if hasattr(plugin_class, 'PLUGIN_INFO'):
                return plugin_class.PLUGIN_INFO
            
            # Try to create from class metadata
            plugin_name = plugin_class.__name__
            docstring = plugin_class.__doc__ or f'Plugin {plugin_name}'
            
            # Extract metadata from class attributes
            metadata = {}
            for attr_name in dir(plugin_class):
                if attr_name.startswith('PLUGIN_'):
                    metadata[attr_name[7:]] = getattr(plugin_class, attr_name)
            
            # Determine plugin type from class name or metadata
            plugin_type = PluginType.UTILITY
            class_name_lower = plugin_class.__name__.lower()
            if 'brain' in class_name_lower:
                plugin_type = PluginType.BRAIN_EXTENSION
            elif 'neural' in class_name_lower:
                plugin_type = PluginType.NEURAL_NETWORK
            elif 'learning' in class_name_lower:
                plugin_type = PluginType.LEARNING_RULE
            elif 'neuromod' in class_name_lower:
                plugin_type = PluginType.NEUROMODULATION
            elif 'sensory' in class_name_lower:
                plugin_type = PluginType.SENSORY
            elif 'motor' in class_name_lower:
                plugin_type = PluginType.MOTOR
            elif 'visualization' in class_name_lower:
                plugin_type = PluginType.VISUALIZATION
            elif 'integration' in class_name_lower:
                plugin_type = PluginType.INTEGRATION
            
            return PluginInfo(
                name=plugin_name,
                version='1.0.0',
                description=docstring,
                author='Unknown',
                plugin_type=plugin_type,
                dependencies=[],
                required_plugins=[],
                metadata=metadata
            )
            
        except Exception as e:
            logging.error(f"Error creating plugin info from class {plugin_class.__name__}: {e}")
            return None
    
    def _create_plugin_with_sandbox(self, plugin_info: PluginInfo, sandbox: SandboxEnvironment) -> Plugin:
        """Create plugin instance with sandbox environment"""
        # Try to load from file path
        if plugin_info.path and os.path.exists(plugin_info.path):
            plugin = self._load_plugin_from_file_path_advanced(plugin_info.path, sandbox)
        elif plugin_info.module:
            plugin = self._create_plugin_from_module_advanced(plugin_info.module, sandbox)
        else:
            # Try to find plugin class
            plugin_class = self._find_plugin_class_advanced(plugin_info.name, sandbox)
            if plugin_class:
                plugin = plugin_class(plugin_info)
            else:
                # Create default plugin
                plugin = DefaultPluginAdvanced(plugin_info, sandbox)
        
        return plugin
    
    def _load_plugin_from_file_path_advanced(self, filepath: str, sandbox: SandboxEnvironment) -> Plugin:
        """Load plugin from file path with sandbox"""
        filename = os.path.basename(filepath)
        plugin_name = os.path.splitext(filename)[0]
        
        # Create enhanced plugin info
        plugin_info = PluginInfo(
            name=plugin_name,
            version='1.0.0',
            description=f'Plugin loaded from {filepath}',
            author='File-based',
            plugin_type=PluginType.UTILITY,
            dependencies=[],
            required_plugins=[],
            path=filepath
        )
        
        return DefaultPluginAdvanced(plugin_info, sandbox)
    
    def _create_plugin_from_module_advanced(self, module: Any, sandbox: SandboxEnvironment) -> Plugin:
        """Create plugin from module with sandbox"""
        # Try to find Plugin class in module
        if hasattr(module, 'Plugin'):
            plugin_class = module.Plugin
            plugin_info = PluginInfo(
                name=plugin_class.__name__,
                version='1.0.0',
                description=plugin_class.__doc__ or 'Dynamic plugin',
                author='Module-based',
                plugin_type=PluginType.UTILITY,
                dependencies=[],
                required_plugins=[],
                module=module
            )
            
            return plugin_class(plugin_info)
        
        # Default fallback
        plugin_info = PluginInfo(
            name='dynamic_plugin',
            version='1.0.0',
            description='Dynamic plugin from module',
            author='Module-based',
            plugin_type=PluginType.UTILITY,
            dependencies=[],
            required_plugins=[],
            module=module
        )
        
        return DefaultPluginAdvanced(plugin_info, sandbox)
    
    def _find_plugin_class_advanced(self, plugin_name: str, sandbox: SandboxEnvironment) -> Optional[Type[Plugin]]:
        """Find plugin class by name with sandbox"""
        # Check built-in plugins
        for plugin_class in [BrainExtensionPlugin, NeuralNetworkPlugin, LearningRulePlugin,
                            NeuromodulationPlugin, SensoryPlugin, MotorPlugin,
                            VisualizationPlugin, IntegrationPlugin, UtilityPlugin]:
            if plugin_class.__name__.lower() == plugin_name.lower():
                return plugin_class
        
        # Check in pynlm.plugins module
        try:
            plugins_module = importlib.import_module('pynlm.plugins')
            for attr_name in dir(plugins_module):
                if attr_name.endswith('Plugin') and not attr_name.startswith('_'):
                    plugin_class = getattr(plugins_module, attr_name)
                    if hasattr(plugin_class, '__bases__') and Plugin in plugin_class.__bases__:
                        if plugin_class.__name__.lower() == plugin_name.lower():
                            return plugin_class
        except ImportError:
            pass
        
        return None
    
    def _validate_plugin(self, plugin_info: PluginInfo):
        """Validate plugin metadata and requirements"""
        # Check required fields
        if not plugin_info.name:
            raise PluginError("Plugin name is required")
        if not plugin_info.version:
            raise PluginError("Plugin version is required")
        if not plugin_info.description:
            raise PluginError("Plugin description is required")
        
        # Check dependencies
        if plugin_info.dependencies:
            for dep in plugin_info.dependencies:
                if dep not in self.plugin_infos:
                    logging.warning(f"Plugin {plugin_info.name} depends on {dep} which is not loaded")
        
        # Check required plugins
        if plugin_info.required_plugins:
            for req in plugin_info.required_plugins:
                if req not in self.plugin_infos:
                    raise PluginError(f"Plugin {plugin_info.name} requires {req} but it is not loaded")
    
    def _register_plugin_events(self, plugin: Plugin):
        """Register plugin event handlers"""
        for event_type, handlers in plugin.event_handlers.items():
            for handler in handlers:
                self.event_bus.subscribe(event_type, handler, plugin.info.name)
    
    def _notify_plugin_unload(self, plugin: Plugin):
        """Notify dependent plugins of plugin unloading"""
        # Create unload event
        event = ScriptingEvent()
        event.event_type = EventType.PLUGIN_UNLOADED
        event.data = {
            'plugin_name': plugin.info.name,
            'timestamp': time.time()
        }
        
        # Emit event
        self.event_bus.publish(event)
    
    def shutdown(self):
        """Shutdown the plugin system"""
        # Unload all plugins
        for plugin_name in list(self.plugins.keys()):
            self.unload_plugin(plugin_name)
        
        # Clear components
        self.event_bus = None
        self.security_manager = None
        self.dependency_resolver = None
        self.lifecycle_monitor = None
        self.state_manager = None
        self.coordination_manager = None
        self.performance_monitor = None


class SecurityManager:
    """Security manager for plugins"""
    
    def __init__(self, config: Dict[str, Any]):
        self.config = config
        self.policies = {}
        self.active_sandboxes = {}
        
        # Load security policies
        self._load_security_policies()
    
    def _load_security_policies(self):
        """Load security policies"""
        # Load default policy
        default_policy = SecurityPolicy('default')
        default_policy.allowed_operations = {
            'import', 'open', 'read', 'write', 'exec', 'eval',
            'threading', 'time', 'json', 'os.path'
        }
        default_policy.allowed_modules = {
            'json', 'time', 'threading', 'secrets', 'hashlib',
            'math', 'random', 'datetime', 'collections'
        }
        default_policy.resource_limits = {
            'memory_mb': 512,
            'cpu_cores': 1.0,
            'max_processes': 5
        }
        self.policies['default'] = default_policy
        
        # Load high security policy
        high_policy = SecurityPolicy('high')
        high_policy.allowed_operations = {
            'import', 'open', 'read', 'write', 'exec', 'eval'
        }
        high_policy.allowed_modules = {
            'json', 'time', 'threading', 'secrets', 'hashlib',
            'math', 'random', 'datetime', 'collections'
        }
        high_policy.resource_limits = {
            'memory_mb': 256,
            'cpu_cores': 0.5,
            'max_processes': 2
        }
        high_policy.denied_operations = {
            'socket', 'subprocess', 'os.system', 'os.exec'
        }
        self.policies['high'] = high_policy
    
    def get_policy(self, security_level: SecurityLevel) -> SecurityPolicy:
        """Get security policy by level"""
        policy_name = security_level.value
        return self.policies.get(policy_name, self.policies['default'])
    
    def create_sandbox(self, plugin_info: PluginInfo) -> SandboxEnvironment:
        """Create security sandbox for plugin"""
        # Determine security level
        security_level = plugin_info.security_level if hasattr(plugin_info, 'security_level') else SecurityLevel.MEDIUM
        
        # Get policy
        policy = self.get_policy(security_level)
        
        # Create workspace
        workspace = os.path.join(self.plugin_cache_dir, 'sandboxes', plugin_info.name)
        os.makedirs(workspace, exist_ok=True)
        
        # Create sandbox
        sandbox = SandboxEnvironment(policy, workspace)
        self.active_sandboxes[plugin_info.name] = sandbox
        
        return sandbox
    
    def remove_sandbox(self, plugin_name: str):
        """Remove sandbox for plugin"""
        if plugin_name in self.active_sandboxes:
            sandbox = self.active_sandboxes[plugin_name]
            # Cleanup sandbox resources
            shutil.rmtree(sandbox.workspace, ignore_errors=True)
            del self.active_sandboxes[plugin_name]
    
    def validate_plugin(self, plugin_info: PluginInfo) -> bool:
        """Validate plugin security"""
        # Check signature if available
        if hasattr(plugin_info, 'signature'):
            if not self._verify_signature(plugin_info):
                return False
        
        # Check permissions
        if hasattr(plugin_info, 'permissions'):
            for permission, required in plugin_info.permissions.items():
                if required and not self._check_permission(permission):
                    return False
        
        return True
    
    def _verify_signature(self, plugin_info: PluginInfo) -> bool:
        """Verify plugin signature"""
        # Implementation would verify digital signature
        return True
    
    def _check_permission(self, permission: str) -> bool:
        """Check if permission is granted"""
        # Implementation would check permission system
        return True


class StateManager:
    """Manager for plugin state and persistence"""
    
    def __init__(self, plugin_manager):
        self.plugin_manager = plugin_manager
        self.state_store = {}
        self.checkpoint_store = {}
        
    def save_plugin_state(self, plugin_name: str, state: Dict[str, Any]):
        """Save plugin state"""
        self.state_store[plugin_name] = state
        
    def load_plugin_state(self, plugin_name: str) -> Optional[Dict[str, Any]]:
        """Load plugin state"""
        return self.state_store.get(plugin_name)
    
    def create_checkpoint(self, plugin_name: str):
        """Create checkpoint for plugin"""
        if plugin_name in self.plugin_manager.plugins:
            plugin = self.plugin_manager.plugins[plugin_name]
            checkpoint_id = f"{plugin_name}_{int(time.time())}"
            
            checkpoint = {
                'timestamp': time.time(),
                'plugin_info': plugin.info,
                'plugin_state': plugin.config,
                'plugin_enabled': plugin.enabled
            }
            
            self.checkpoint_store[checkpoint_id] = checkpoint
            return checkpoint_id
    
    def restore_checkpoint(self, checkpoint_id: str) -> bool:
        """Restore from checkpoint"""
        if checkpoint_id not in self.checkpoint_store:
            return False
        
        checkpoint = self.checkpoint_store[checkpoint_id]
        plugin_name = checkpoint.get('plugin_name', 'unknown')
        
        if plugin_name in self.plugin_manager.plugins:
            plugin = self.plugin_manager.plugins[plugin_name]
            plugin.config = checkpoint.get('plugin_state', {})
            plugin.enabled = checkpoint.get('plugin_enabled', True)
            
            return True
        
        return False


class EventSystem:
    """Event system for plugin communication"""
    
    def __init__(self, plugin_manager):
        self.plugin_manager = plugin_manager
        self.event_bus = EventBus()
        
    def subscribe(self, event_type: EventType, handler: Callable, plugin_name: str = None):
        """Subscribe to events"""
        self.event_bus.subscribe(event_type, handler, plugin_name)
    
    def unsubscribe(self, event_type: EventType, plugin_name: str = None, handler: Callable = None):
        """Unsubscribe from events"""
        self.event_bus.unsubscribe(event_type, plugin_name, handler)
    
    def emit(self, event: ScriptingEvent, target_plugins: List[str] = None):
        """Emit event"""
        self.event_bus.publish(event, target_plugins)


class CoordinationManager:
    """Manager for plugin coordination and lifecycle"""
    
    def __init__(self, plugin_manager):
        self.plugin_manager = plugin_manager
        self.coordination_graph = {}
        
    def register_plugin(self, plugin_name: str, dependencies: List[str]):
        """Register plugin in coordination graph"""
        self.coordination_graph[plugin_name] = {
            'dependencies': dependencies,
            'dependents': [],
            'initialized': False
        }
        
        # Update dependents
        for dep in dependencies:
            if dep in self.coordination_graph:
                if plugin_name not in self.coordination_graph[dep]['dependents']:
                    self.coordination_graph[dep]['dependents'].append(plugin_name)
    
    def initialize_plugin(self, plugin_name: str):
        """Initialize plugin according to dependencies"""
        if plugin_name not in self.coordination_graph:
            return True
        
        graph = self.coordination_graph
        
        # Check if dependencies are initialized
        for dep in graph[plugin_name]['dependencies']:
            if not graph.get(dep, {}).get('initialized', False):
                return False
        
        # Initialize plugin
        plugin = self.plugin_manager.get_plugin(plugin_name)
        if plugin:
            plugin.initialize(self.plugin_manager.interface)
            graph[plugin_name]['initialized'] = True
            return True
        
        return False
    
    def shutdown_plugin(self, plugin_name: str):
        """Shutdown plugin and notify dependents"""
        if plugin_name not in self.coordination_graph:
            return True
        
        # Shutdown dependents first
        for dependent in graph[plugin_name]['dependents']:
            self.shutdown_plugin(dependent)
        
        # Shutdown plugin
        plugin = self.plugin_manager.get_plugin(plugin_name)
        if plugin:
            plugin.disable()
        
        graph[plugin_name]['initialized'] = False
        return True


class LifecycleMonitor:
    """Monitor plugin lifecycle and health"""
    
    def __init__(self, plugin_manager):
        self.plugin_manager = plugin_manager
        self.plugin_metrics = {}
        
    def start_monitoring(self, plugin_name: str):
        """Start monitoring a plugin"""
        self.plugin_metrics[plugin_name] = {
            'start_time': time.time(),
            'events': [],
            'health_checks': [],
            'performance_data': []
        }
    
    def stop_monitoring(self, plugin_name: str):
        """Stop monitoring a plugin"""
        if plugin_name in self.plugin_metrics:
            del self.plugin_metrics[plugin_name]
    
    def record_event(self, plugin_name: str, event_type: str, data: Dict[str, Any]):
        """Record plugin event"""
        if plugin_name not in self.plugin_metrics:
            return
        
        self.plugin_metrics[plugin_name]['events'].append({
            'timestamp': time.time(),
            'event_type': event_type,
            'data': data
        })
    
    def record_health_check(self, plugin_name: str, is_healthy: bool):
        """Record health check result"""
        if plugin_name not in self.plugin_metrics:
            return
        
        self.plugin_metrics[plugin_name]['health_checks'].append({
            'timestamp': time.time(),
            'healthy': is_healthy
        })
    
    def record_performance(self, plugin_name: str, metrics: Dict[str, Any]):
        """Record performance metrics"""
        if plugin_name not in self.plugin_metrics:
            return
        
        self.plugin_metrics[plugin_name]['performance_data'].append({
            'timestamp': time.time(),
            'metrics': metrics
        })


class PerformanceMonitor:
    """Monitor plugin performance"""
    
    def __init__(self, plugin_manager):
        self.plugin_manager = plugin_manager
        self.performance_data = {}
        self.alert_thresholds = {
            'load_time_ms': 5000,
            'unload_time_ms': 2000,
            'memory_usage_mb': 1024,
            'cpu_usage_percent': 90.0
        }
    
    def record_plugin_load(self, plugin_name: str, load_time: float):
        """Record plugin load time"""
        if plugin_name not in self.performance_data:
            self.performance_data[plugin_name] = {
                'load_times': [],
                'unload_times': [],
                'health_checks': [],
                'events': []
            }
        
        self.performance_data[plugin_name]['load_times'].append(load_time)
        
        # Check threshold
        if load_time > self.alert_thresholds['load_time_ms'] / 1000:
            self._trigger_alert('slow_plugin_load', {
                'plugin_name': plugin_name,
                'load_time': load_time,
                'threshold': self.alert_thresholds['load_time_ms'] / 1000
            })
    
    def record_plugin_load_failure(self, plugin_name: str, load_time: float):
        """Record plugin load failure"""
        if plugin_name not in self.performance_data:
            self.performance_data[plugin_name] = {
                'load_times': [],
                'unload_times': [],
                'health_checks': [],
                'events': []
            }
        
        self.performance_data[plugin_name]['load_times'].append(load_time)
        
        # Check threshold
        if load_time > self.alert_thresholds['load_time_ms'] / 1000:
            self._trigger_alert('failed_plugin_load', {
                'plugin_name': plugin_name,
                'load_time': load_time,
                'threshold': self.alert_thresholds['load_time_ms'] / 1000
            })
    
    def record_plugin_unload(self, plugin_name: str, unload_time: float):
        """Record plugin unload time"""
        if plugin_name not in self.performance_data:
            self.performance_data[plugin_name] = {
                'load_times': [],
                'unload_times': [],
                'health_checks': [],
                'events': []
            }
        
        self.performance_data[plugin_name]['unload_times'].append(unload_time)
        
        # Check threshold
        if unload_time > self.alert_thresholds['unload_time_ms'] / 1000:
            self._trigger_alert('slow_plugin_unload', {
                'plugin_name': plugin_name,
                'unload_time': unload_time,
                'threshold': self.alert_thresholds['unload_time_ms'] / 1000
            })
    
    def record_health_check(self, plugin_name: str, is_healthy: bool):
        """Record health check result"""
        if plugin_name not in self.performance_data:
            self.performance_data[plugin_name] = {
                'load_times': [],
                'unload_times': [],
                'health_checks': [],
                'events': []
            }
        
        self.performance_data[plugin_name]['health_checks'].append({
            'timestamp': time.time(),
            'healthy': is_healthy
        })
    
    def record_event(self, plugin_name: str, event: ScriptingEvent):
        """Record plugin event"""
        if plugin_name not in self.performance_data:
            self.performance_data[plugin_name] = {
                'load_times': [],
                'unload_times': [],
                'health_checks': [],
                'events': []
            }
        
        self.performance_data[plugin_name]['events'].append({
            'timestamp': event.timestamp,
            'event_type': event.event_type
        })
    
    def _trigger_alert(self, alert_type: str, data: Dict[str, Any]):
        """Trigger performance alert"""
        # Implementation would send alerts
        logging.warning(f"Performance alert: {alert_type} - {data}")


# Default enhanced plugin implementations
class DefaultPluginAdvanced(Plugin):
    """Enhanced default plugin implementation"""
    
    def __init__(self, plugin_info: PluginInfo, sandbox: SandboxEnvironment):
        super().__init__(plugin_info)
        self.sandbox = sandbox
        
    def _initialize_plugin(self):
        pass
        
    def _enable_plugin(self):
        pass
        
    def _disable_plugin(self):
        pass
        
    def _unload_plugin(self):
        pass
class BrainExtensionPlugin(Plugin):
    """Enhanced brain extension plugin"""
    
    def __init__(self, plugin_info: PluginInfo):
        super().__init__(plugin_info)
        self.info.plugin_type = PluginType.BRAIN_EXTENSION
        
    def _initialize_plugin(self):
        if self.interface:
            self.register_event_handler(EventType.STATE_CHANGED, self._on_state_change)
    
    def _on_state_change(self, event: ScriptingEvent):
        """Handle state change events"""
        pass
class NeuralNetworkPlugin(Plugin):
    """Enhanced neural network plugin"""
    
    def __init__(self, plugin_info: PluginInfo):
        super().__init__(plugin_info)
        self.info.plugin_type = PluginType.NEURAL_NETWORK
        self.network_architecture = None
        self.custom_connections = []
        
    def create_network(self, architecture: Dict[str, Any]) -> Any:
        """Create custom neural network"""
        self.network_architecture = architecture
        return architecture
        
    def add_connection(self, source: str, target: str, weight: float):
        """Add custom connection between neurons"""
        self.custom_connections.append({
            'source': source,
            'target': target,
            'weight': weight
        })
        
    def _initialize_plugin(self):
        if self.interface:
            self.register_event_handler(EventType.SIMULATION_STARTED, self._on_simulation_start)
    
    def _on_simulation_start(self, event: ScriptingEvent):
        """Handle simulation start"""
        if self.network_architecture:
            self._apply_network_architecture()
            
    def _apply_network_architecture(self):
        """Apply custom network architecture"""
        pass
class LearningRulePlugin(Plugin):
    """Enhanced learning rule plugin"""
    
    def __init__(self, plugin_info: PluginInfo):
        super().__init__(plugin_info)
        self.info.plugin_type = PluginType.LEARNING_RULE
        self.learning_rule = None
        self.plasticity_params = {}
        
    def set_learning_rule(self, rule: Dict[str, Any]):
        """Set custom learning rule"""
        self.learning_rule = rule
        
    def set_plasticity_params(self, params: Dict[str, Any]):
        """Set plasticity parameters"""
        self.plasticity_params.update(params)
        
    def calculate_weight_change(self, pre_neuron: Any, post_neuron: Any,
                               activity_diff: float, time: float) -> float:
        """Calculate weight change using custom rule"""
        if not self.learning_rule:
            return activity_diff * 0.1
        return activity_diff * self.learning_rule.get('learning_rate', 0.1)
        
    def _enable_plugin(self):
        if self.interface:
            self.register_event_handler(EventType.NEURON_FIRED, self._on_neuron_fire)
class NeuromodulationPlugin(Plugin):
    """Enhanced neuromodulation plugin"""
    
    def __init__(self, plugin_info: PluginInfo):
        super().__init__(plugin_info)
        self.info.plugin_type = PluginType.NEUROMODULATION
        self.modulation_effects = {}
        self.neuromodulation_level = 0.0
        
    def add_modulation_effect(self, name: str, effect: Callable, params: Dict[str, Any] = None):
        """Add neuromodulation effect"""
        self.modulation_effects[name] = {
            'effect': effect,
            'params': params or {}
        }
        
    def update_modulation(self, time: float, inputs: Dict[str, Any]):
        """Update neuromodulation level"""
        total_effect = 0.0
        
        for effect_name, effect_data in self.modulation_effects.items():
            effect = effect_data['effect']
            params = effect_data['params']
            total_effect += effect(time, inputs, params)
        
        self.neuromodulation_level = max(0.0, min(1.0, total_effect))
        
        if self.interface:
            self.interface.context.brain.setNeuromodulationLevel(self.neuromodulation_level)
            
    def _enable_plugin(self):
        if self.interface:
            self.register_event_handler(EventType.REWARD_RECEIVED, self._on_reward)
            self.register_event_handler(EventType.STATE_CHANGED, self._on_state_change)


# Plugin categories follow the same pattern with enhanced implementations
# (BrainPlugin, PlasticityPlugin, etc. are implemented similarly above)