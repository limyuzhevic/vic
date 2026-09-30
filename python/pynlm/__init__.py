"""
NLM Plugin System Integration

This module provides integration between the original plugin system and the enhanced plugin system.
It ensures backward compatibility while enabling access to advanced plugin features.
"""

import os
import sys
import importlib
from typing import Dict, List, Any, Optional, Callable, Type

from .plugin_system import (
    Plugin, PluginInfo, PluginManager as OriginalPluginManager,
    PluginType, PluginStatus, PluginError,
    BrainExtensionPlugin, NeuralNetworkPlugin, LearningRulePlugin,
    NeuromodulationPlugin, SensoryPlugin, MotorPlugin, VisualizationPlugin,
    IntegrationPlugin, UtilityPlugin, DefaultPlugin
)

from .enhanced_plugin_system import (
    EnhancedPluginManager, initialize_plugin_system, shutdown_plugin_system,
    get_plugin_manager, set_plugin_manager
)

# Global variables
_original_plugin_manager = None
_enhanced_plugin_manager = None
_bridge_initialized = False


def initialize_bridge(interface, config: Dict[str, Any] = None):
    """Initialize the plugin system bridge"""
    global _original_plugin_manager, _enhanced_plugin_manager, _bridge_initialized
    
    if _bridge_initialized:
        return True
    
    # Create original plugin manager
    _original_plugin_manager = OriginalPluginManager(interface)
    
    # Create enhanced plugin manager
    _enhanced_plugin_manager = EnhancedPluginManager(interface, config)
    
    # Set enhanced manager as global
    set_plugin_manager(_enhanced_plugin_manager)
    
    # Copy some essential functionality to original manager
    _copy_plugin_functionality(_original_plugin_manager, _enhanced_plugin_manager)
    
    _bridge_initialized = True
    return True


def shutdown_bridge():
    """Shutdown the plugin system bridge"""
    global _original_plugin_manager, _enhanced_plugin_manager, _bridge_initialized
    
    if not _bridge_initialized:
        return True
    
    # Shutdown enhanced plugin manager
    if _enhanced_plugin_manager:
        _enhanced_plugin_manager.shutdown()
    
    # Clear global references
    _original_plugin_manager = None
    _enhanced_plugin_manager = None
    set_plugin_manager(None)
    
    _bridge_initialized = False
    return True


def get_original_plugin_manager():
    """Get the original plugin manager"""
    return _original_plugin_manager


def get_enhanced_plugin_manager():
    """Get the enhanced plugin manager"""
    return _enhanced_plugin_manager


def get_plugin_manager():
    """Get the appropriate plugin manager (enhanced if available)"""
    global _enhanced_plugin_manager
    
    if _enhanced_plugin_manager:
        return _enhanced_plugin_manager
    elif _original_plugin_manager:
        return _original_plugin_manager
    else:
        raise RuntimeError("Plugin system not initialized")


class PluginSystemBridge:
    """Bridge between original and enhanced plugin systems"""
    
    def __init__(self, interface, config: Dict[str, Any] = None):
        self.interface = interface
        self.config = config or {}
        
        # Initialize bridge
        initialize_bridge(interface, config)
        
        # Get managers
        self.original_manager = get_original_plugin_manager()
        self.enhanced_manager = get_enhanced_plugin_manager()
        
    def load_plugin(self, plugin_name: str, plugin_path: Optional[str] = None,
                   config: Optional[Dict[str, Any]] = None) -> Plugin:
        """Load plugin using the enhanced plugin manager if available"""
        if self.enhanced_manager:
            return self.enhanced_manager.load_plugin(plugin_name, plugin_path, config)
        else:
            return self.original_manager.load_plugin(plugin_name, plugin_path, config)
    
    def unload_plugin(self, plugin_name: str) -> bool:
        """Unload plugin using the enhanced plugin manager if available"""
        if self.enhanced_manager:
            return self.enhanced_manager.unload_plugin(plugin_name)
        else:
            return self.original_manager.unload_plugin(plugin_name)
    
    def hot_reload_plugin(self, plugin_name: str, plugin_path: Optional[str] = None) -> bool:
        """Hot reload plugin using enhanced features if available"""
        if self.enhanced_manager:
            return self.enhanced_manager.hot_reload_plugin(plugin_name, plugin_path)
        else:
            # Fall back to original manager with warning
            import logging
            logging.warning("Hot reload not supported in original plugin system")
            return self.unload_plugin(plugin_name)
    
    def get_plugin(self, plugin_name: str) -> Optional[Plugin]:
        """Get plugin using the enhanced plugin manager if available"""
        if self.enhanced_manager:
            return self.enhanced_manager.get_plugin(plugin_name)
        else:
            return self.original_manager.get_plugin(plugin_name)
    
    def list_plugins(self) -> List[PluginInfo]:
        """List plugins using the enhanced plugin manager if available"""
        if self.enhanced_manager:
            return self.enhanced_manager.list_plugins()
        else:
            return self.original_manager.list_plugins()
    
    def search_plugins(self, query: str, filters: Optional[Dict[str, Any]] = None) -> List[PluginInfo]:
        """Search plugins using the enhanced plugin manager if available"""
        if self.enhanced_manager:
            return self.enhanced_manager.search_plugins(query, filters)
        else:
            return self.original_manager.search_plugins(query)
    
    def install_plugin(self, plugin_spec: str) -> Plugin:
        """Install plugin using the enhanced plugin manager if available"""
        if self.enhanced_manager:
            return self.enhanced_manager.install_plugin(plugin_spec)
        else:
            return self.original_manager.install_plugin(plugin_spec)
    
    def create_custom_plugin(self, plugin_name: str, plugin_class: Type[Plugin],
                           config: Optional[Dict[str, Any]] = None) -> Plugin:
        """Create custom plugin using the enhanced plugin manager if available"""
        if self.enhanced_manager:
            return self.enhanced_manager.create_custom_plugin(plugin_name, plugin_class, config)
        else:
            return self.original_manager.create_custom_plugin(plugin_name, plugin_class, config)
    
    def get_plugin_manager_type(self) -> str:
        """Get the type of plugin manager being used"""
        if self.enhanced_manager:
            return "enhanced"
        elif self.original_manager:
            return "original"
        else:
            return "none"
    
    def enable_enhanced_features(self, enabled: bool = True):
        """Enable or disable enhanced plugin manager features"""
        if self.enhanced_manager:
            # Some features are always available in enhanced manager
            pass
    
    def get_system_capabilities(self) -> Dict[str, bool]:
        """Get system capabilities"""
        capabilities = {
            'hot_reloading': False,
            'security_sandboxing': False,
            'dependency_management': False,
            'event_system': False,
            'state_management': False,
            'performance_monitoring': False,
            'multi_language_support': False,
            'marketplace_integration': False,
            'plugin_certification': False,
            'advanced_search': False,
            'plugin_ratings': False,
            'plugin_development_kit': False,
            'cross_language_communication': False,
            'backward_compatibility': True
        }
        
        if self.enhanced_manager:
            capabilities.update({
                'hot_reloading': True,
                'security_sandboxing': True,
                'dependency_management': True,
                'event_system': True,
                'state_management': True,
                'performance_monitoring': True,
                'multi_language_support': True,
                'marketplace_integration': True,
                'plugin_certification': True,
                'advanced_search': True,
                'plugin_ratings': True,
                'plugin_development_kit': True,
                'cross_language_communication': True
            })
        
        return capabilities
class PluginSystemInitializer:
    """Plugin system initializer for backward compatibility"""
    
    def __init__(self, interface):
        self.interface = interface
        
    def initialize(self, config: Dict[str, Any] = None):
        """Initialize plugin system"""
        return initialize_bridge(self.interface, config)
        
    def shutdown(self):
        """Shutdown plugin system"""
        return shutdown_bridge()
class PluginSystemFactory:
    """Factory for creating plugin systems"""
    
    @staticmethod
    def create_original_plugin_system(interface):
        """Create original plugin system"""
        return OriginalPluginManager(interface)
    
    @staticmethod
    def create_enhanced_plugin_system(interface, config: Dict[str, Any] = None):
        """Create enhanced plugin system"""
        return EnhancedPluginManager(interface, config)
    
    @staticmethod
    def create_bridge(interface, config: Dict[str, Any] = None):
        """Create plugin system bridge"""
        return PluginSystemBridge(interface, config)


def _copy_plugin_functionality(original: OriginalPluginManager, enhanced: EnhancedPluginManager):
    """Copy essential functionality from enhanced to original manager"""
    # This function copies some essential functionality from the enhanced
    # plugin manager to the original manager to maintain backward compatibility
    # while providing enhanced features.
    
    # Copy essential attributes
    original.plugins = enhanced.plugins.copy()
    original.plugin_infos = enhanced.plugin_infos.copy()
    original.plugin_paths = enhanced.plugin_paths.copy()
    
    # Copy event handlers
    original.event_handlers = getattr(enhanced, 'event_handlers', {})
    
    # Copy configuration
    original.auto_load_plugins = enhanced.auto_load_plugins
    original.plugin_registry = enhanced.plugin_registry
    original.plugin_cache_dir = enhanced.plugin_cache_dir
    
    # Copy some methods
    original.load_plugin = lambda plugin_name, plugin_path=None, config=None: enhanced.load_plugin(plugin_name, plugin_path, config)
    original.unload_plugin = lambda plugin_name: enhanced.unload_plugin(plugin_name)
    original.get_plugin = lambda plugin_name: enhanced.get_plugin(plugin_name)
    original.list_plugins = lambda: enhanced.list_plugins()
    original.search_plugins = lambda query, plugin_type=None, category=None: enhanced.search_plugins(query)
    original.install_plugin = lambda plugin_spec: enhanced.install_plugin(plugin_spec)
    original.create_custom_plugin = lambda plugin_name, plugin_class, config=None: enhanced.create_custom_plugin(plugin_name, plugin_class, config)


# Export key classes and functions
__all__ = [
    'PluginSystemBridge',
    'PluginSystemInitializer',
    'PluginSystemFactory',
    'initialize_bridge',
    'shutdown_bridge',
    'get_original_plugin_manager',
    'get_enhanced_plugin_manager',
    'get_plugin_manager',
    'initialize_plugin_system',
    'shutdown_plugin_system'
]

# Backward compatibility aliases
PluginManager = PluginSystemFactory.create_original_plugin_system
EnhancedPluginManager = EnhancedPluginManager