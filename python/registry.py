"""
Component Registry for NLM (Neural Learning Machine).

This module provides a centralized component registry system for NLM components,
allowing users to register, discover, and manage components easily.

Key features:
- Component registration and discovery
- Component metadata management
- Plugin architecture
- Component factory system
- Documentation generation

The Component Registry system is part of NLM Phase 6 "FINAL INTEGRATION",
powering advanced artificial brain capabilities through a comprehensive
component management and extension framework.

The system supports integration with:
- Brain (central neural coordination)
- AgentBrain (agent interface)
- World (environmental interaction)
- Memory systems (working, episodic, semantic, procedural)
- Prediction systems (forward models)
- Cognitive systems (planning, attention, concept formation)
- Neuromodulation systems (dopamine, serotonin, norepinephrine, acetylcholine)
- Development systems (maturation, learning)
- Performance infrastructure (memory pools, event queues)

See src/registry/README.md for detailed documentation.
"""

from typing import Dict, Any, Optional, List, Union, Iterator, Callable
from pathlib import Path
import json
import logging
from dataclasses import dataclass, field
from enum import Enum

from ._core import Config as CConfig
from ._core import Brain as CBrain
from .exceptions import (
    NLMError,
    NLMInitializationError,
    NLMInvalidStateError,
    NLMSimulationError,
    NLMResourceError,
)


class ComponentStatus(Enum):
    """Component status enumeration."""
    
    UNKNOWN = "unknown"
    INITIALIZING = "initializing"
    INITIALIZED = "initialized"
    RUNNING = "running"
    PAUSED = "paused"
    STOPPING = "stopping"
    STOPPED = "stopped"
    ERROR = "error"
    DISABLED = "disabled"


@dataclass
class ComponentMetadata:
    """Component metadata class."""
    
    # Basic information
    name: str = ""
    description: str = ""
    version: str = "1.0.0"
    author: str = ""
    category: str = ""
    type: str = ""
    
    # Documentation
    documentation: str = ""
    tags: List[str] = field(default_factory=list)
    
    # Configuration
    configuration: Dict[str, Any] = field(default_factory=dict)
    dependencies: List[str] = field(default_factory=list)
    conflicts: List[str] = field(default_factory=list)
    
    # Status and lifecycle
    enabled: bool = True
    requires_configuration: bool = False
    registration_time: float = 0.0
    last_used: float = 0.0
    load_count: int = 0
    
    def get_full_name(self) -> str:
        """Get full component name with version."""
        return f"{self.name} v{self.version}"
    
    def get_unique_id(self) -> str:
        """Get unique component ID."""
        return f"{self.name}_v{self.version}"
    
    def has_dependency(self, component_name: str) -> bool:
        """Check if component has a dependency."""
        return component_name in self.dependencies
    
    def conflicts_with(self, component_name: str) -> bool:
        """Check if component conflicts with another component."""
        return component_name in self.conflicts


@dataclass
class ComponentInfo:
    """Component information dataclass."""
    
    # Basic information
    name: str = ""
    description: str = ""
    version: str = "1.0.0"
    author: str = ""
    category: str = ""
    type: str = ""
    
    # Status
    enabled: bool = True
    status: ComponentStatus = ComponentStatus.UNKNOWN
    
    # Statistics
    load_count: int = 0
    last_used: float = 0.0


class ComponentRegistry:
    """Centralized component registry for NLM components."""
    
    def __init__(self):
        """Initialize component registry."""
        self._components: Dict[str, ComponentMetadata] = {}
        self._factories: Dict[str, Callable] = {}
        self._plugins: Dict[str, Dict] = {}
        self._config: Dict[str, Any] = {}
        
        # Setup logging
        self._logger = logging.getLogger(__name__)
    
    def register_component(self, name: str, metadata: ComponentMetadata, 
                          factory: Optional[Callable] = None) -> bool:
        """
        Register a component.
        
        Args:
            name: Component name
            metadata: Component metadata
            factory: Component factory function
            
        Returns:
            True if component registered successfully, False otherwise
        """
        try:
            if name in self._components:
                self._logger.warning(f"Component '{name}' already registered")
                return False
            
            self._components[name] = metadata
            if factory:
                self._factories[name] = factory
            
            self._logger.info(f"Component '{name}' registered successfully")
            return True
            
        except Exception as e:
            self._logger.error(f"Failed to register component '{name}': {e}")
            return False
    
    def unregister_component(self, name: str) -> bool:
        """
        Unregister a component.
        
        Args:
            name: Component name
            
        Returns:
            True if component unregistered successfully, False otherwise
        """
        try:
            if name not in self._components:
                self._logger.warning(f"Component '{name}' not found")
                return False
            
            del self._components[name]
            if name in self._factories:
                del self._factories[name]
            
            self._logger.info(f"Component '{name}' unregistered successfully")
            return True
            
        except Exception as e:
            self._logger.error(f"Failed to unregister component '{name}': {e}")
            return False
    
    def get_component(self, name: str) -> Optional[ComponentMetadata]:
        """
        Get component metadata by name.
        
        Args:
            name: Component name
            
        Returns:
            Component metadata if found, None otherwise
        """
        return self._components.get(name)
    
    def find_components_by_type(self, type_name: str) -> List[ComponentMetadata]:
        """
        Find components by type.
        
        Args:
            type_name: Component type
            
        Returns:
            List of matching components
        """
        return [meta for meta in self._components.values() if meta.type == type_name]
    
    def find_components_by_category(self, category: str) -> List[ComponentMetadata]:
        """
        Find components by category.
        
        Args:
            category: Component category
            
        Returns:
            List of matching components
        """
        return [meta for meta in self._components.values() if meta.category == category]
    
    def find_components_by_tag(self, tag: str) -> List[ComponentMetadata]:
        """
        Find components by tag.
        
        Args:
            tag: Component tag
            
        Returns:
            List of matching components
        """
        return [meta for meta in self._components.values() if tag in meta.tags]
    
    def search_components(self, query: str) -> List[ComponentMetadata]:
        """
        Search components by query.
        
        Args:
            query: Search query
            
        Returns:
            List of matching components
        """
        query_lower = query.lower()
        return [meta for meta in self._components.values() 
                if (query_lower in meta.name.lower() or 
                    query_lower in meta.description.lower() or
                    query_lower in meta.documentation.lower())]
    
    def get_all_components(self) -> List[ComponentMetadata]:
        """
        Get all registered components.
        
        Returns:
            List of all components
        """
        return list(self._components.values())
    
    def get_component_names(self) -> List[str]:
        """
        Get all component names.
        
        Returns:
            List of component names
        """
        return list(self._components.keys())
    
    def enable_component(self, name: str) -> bool:
        """
        Enable a component.
        
        Args:
            name: Component name
            
        Returns:
            True if component enabled successfully, False otherwise
        """
        component = self._components.get(name)
        if component:
            component.enabled = True
            return True
        return False
    
    def disable_component(self, name: str) -> bool:
        """
        Disable a component.
        
        Args:
            name: Component name
            
        Returns:
            True if component disabled successfully, False otherwise
        """
        component = self._components.get(name)
        if component:
            component.enabled = False
            return True
        return False
    
    def get_enabled_components(self) -> List[ComponentMetadata]:
        """
        Get all enabled components.
        
        Returns:
            List of enabled components
        """
        return [meta for meta in self._components.values() if meta.enabled]
    
    def get_disabled_components(self) -> List[ComponentMetadata]:
        """
        Get all disabled components.
        
        Returns:
            List of disabled components
        """
        return [meta for meta in self._components.values() if not meta.enabled]


class ComponentFactory:
    """Component factory for creating components."""
    
    def __init__(self):
        """Initialize component factory."""
        self._factories: Dict[str, Dict] = {}
        self._logger = logging.getLogger(__name__)
    
    def register_component(self, name: str, version: str, type_name: str,
                          description: str, factory: Callable):
        """
        Register a component factory.
        
        Args:
            name: Component name
            version: Component version
            type_name: Component type
            description: Component description
            factory: Component factory function
        """
        key = f"{name}_{version}"
        self._factories[key] = {
            'name': name,
            'version': version,
            'type': type_name,
            'description': description,
            'factory': factory
        }
    
    def create_component(self, name: str, version: str = "") -> Any:
        """
        Create a component.
        
        Args:
            name: Component name
            version: Component version (optional)
            
        Returns:
            Created component or None if not found
        """
        if not version:
            # Find the latest version
            versions = [key.split('_')[1] for key in self._factories.keys() 
                       if key.startswith(f"{name}_")]
            if not versions:
                return None
            version = max(versions)
        
        key = f"{name}_{version}"
        if key in self._factories:
            factory_info = self._factories[key]
            return factory_info['factory']()
        
        return None
    
    def get_registered_components(self) -> List[Dict]:
        """
        Get all registered components.
        
        Returns:
            List of registered component information
        """
        return list(self._factories.values())
    
    def get_component_info(self, name: str, version: str = "") -> Optional[Dict]:
        """
        Get component information.
        
        Args:
            name: Component name
            version: Component version (optional)
            
        Returns:
            Component information or None if not found
        """
        if not version:
            # Find the latest version
            versions = [key.split('_')[1] for key in self._factories.keys() 
                       if key.startswith(f"{name}_")]
            if not versions:
                return None
            version = max(versions)
        
        key = f"{name}_{version}"
        return self._factories.get(key)


# Global registry instances
_global_registry: Optional[ComponentRegistry] = None
_global_factory: Optional[ComponentFactory] = None


def get_registry() -> ComponentRegistry:
    """Get global registry instance."""
    global _global_registry
    if _global_registry is None:
        _global_registry = ComponentRegistry()
    return _global_registry


def get_factory() -> ComponentFactory:
    """Get global factory instance."""
    global _global_factory
    if _global_factory is None:
        _global_factory = ComponentFactory()
    return _global_factory


def register_component(name: str, metadata: ComponentMetadata, 
                      factory: Optional[Callable] = None) -> bool:
    """
    Register a component globally.
    
    Args:
        name: Component name
        metadata: Component metadata
        factory: Component factory function (optional)
        
    Returns:
        True if component registered successfully, False otherwise
    """
    registry = get_registry()
    return registry.register_component(name, metadata, factory)


def get_component(name: str) -> Optional[ComponentMetadata]:
    """
    Get component by name from global registry.
    
    Args:
        name: Component name
        
    Returns:
        Component metadata if found, None otherwise
    """
    registry = get_registry()
    return registry.get_component(name)


def search_components(query: str) -> List[ComponentMetadata]:
    """
    Search components globally.
    
    Args:
        query: Search query
        
    Returns:
        List of matching components
    """
    registry = get_registry()
    return registry.search_components(query)