"""
Component discovery API for NLM (Neural Learning Machine).

This module provides component discovery APIs for finding components
by type, category, tags, and other criteria.

Key features:
- Component registration and discovery
- Component metadata management
- Plugin architecture
- Component factory system
- Documentation generation
- Search and filtering capabilities
- Component compatibility checking

The Component Discovery API is part of NLM Phase 6 "FINAL INTEGRATION",
powering advanced artificial brain capabilities through a comprehensive
component management and extension framework.

The system supports integration with:
- Component Registry (central component management)
- Component Factory (component creation system)
- Plugin System (dynamic component loading)
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

from typing import Dict, Any, Optional, List, Callable
from pathlib import Path
import logging
import json

from .exceptions import NLMError
from .registry import ComponentRegistry, get_registry
from .plugin import PluginManager, get_plugin_manager


class DiscoveryAPI:
    """Component discovery API for NLM."""
    
    def __init__(self):
        """Initialize discovery API."""
        self._registry: Dict[str, Dict] = {}
        self._plugin_manager = get_plugin_manager()
        self._logger = logging.getLogger(__name__)
        
        # Sync with registry
        self._sync_with_registry()
    
    def _sync_with_registry(self):
        """Sync with component registry."""
        try:
            registry = get_registry()
            components = registry.get_all_components()
            
            # Convert ComponentMetadata to dict
            for name, metadata in registry.get_all_components().items():
                self._registry[name] = {
                    'name': metadata.name,
                    'description': metadata.description,
                    'version': metadata.version,
                    'author': metadata.author,
                    'category': metadata.category,
                    'type': metadata.type,
                    'documentation': metadata.documentation,
                    'tags': metadata.tags,
                    'configuration': metadata.configuration,
                    'dependencies': metadata.dependencies,
                    'conflicts': metadata.conflicts,
                    'enabled': metadata.enabled,
                    'requires_configuration': metadata.requires_configuration,
                    'registration_time': metadata.registration_time,
                    'last_used': metadata.last_used,
                    'load_count': metadata.load_count
                }
            
            self._logger.debug(f"Synced with registry: {len(self._registry)} components")
        except Exception as e:
            self._logger.error(f"Failed to sync with registry: {e}")
    
    def register_component(self, name: str, metadata: Dict[str, Any]):
        """
        Register a component for discovery.
        
        Args:
            name: Component name
            metadata: Component metadata
        """
        self._registry[name] = metadata
        self._logger.debug(f"Component '{name}' registered for discovery")
    
    def unregister_component(self, name: str):
        """
        Unregister a component from discovery.
        
        Args:
            name: Component name
        """
        if name in self._registry:
            del self._registry[name]
            self._logger.debug(f"Component '{name}' unregistered from discovery")
    
    def find_by_type(self, type_name: str) -> List[Dict[str, Any]]:
        """
        Find components by type.
        
        Args:
            type_name: Component type
            
        Returns:
            List of matching components
        """
        return [meta for meta in self._registry.values() if meta.get('type') == type_name]
    
    def find_by_category(self, category: str) -> List[Dict[str, Any]]:
        """
        Find components by category.
        
        Args:
            category: Component category
            
        Returns:
            List of matching components
        """
        return [meta for meta in self._registry.values() if meta.get('category') == category]
    
    def find_by_tag(self, tag: str) -> List[Dict[str, Any]]:
        """
        Find components by tag.
        
        Args:
            tag: Component tag
            
        Returns:
            List of matching components
        """
        return [meta for meta in self._registry.values() 
                if tag in meta.get('tags', [])]
    
    def search_by_name(self, name: str) -> List[Dict[str, Any]]:
        """
        Search components by name.
        
        Args:
            name: Component name
            
        Returns:
            List of matching components
        """
        return [meta for meta in self._registry.values() 
                if name.lower() in meta.get('name', '').lower()]
    
    def search_by_description(self, description: str) -> List[Dict[str, Any]]:
        """
        Search components by description.
        
        Args:
            description: Component description
            
        Returns:
            List of matching components
        """
        desc_lower = description.lower()
        return [meta for meta in self._registry.values() 
                if desc_lower in meta.get('description', '').lower()]
    
    def search_by_full_text(self, query: str) -> List[Dict[str, Any]]:
        """
        Search components by full text.
        
        Args:
            query: Search query
            
        Returns:
            List of matching components
        """
        query_lower = query.lower()
        results = []
        for meta in self._registry.values():
            content = f"{meta.get('name', '')} {meta.get('description', '')} {meta.get('documentation', '')}"
            if query_lower in content.lower():
                results.append(meta)
        return results
    
    def get_all_components(self) -> List[Dict[str, Any]]:
        """
        Get all registered components.
        
        Returns:
            List of all components
        """
        return list(self._registry.values())
    
    def get_component(self, name: str) -> Optional[Dict[str, Any]]:
        """
        Get component by name.
        
        Args:
            name: Component name
            
        Returns:
            Component metadata if found, None otherwise
        """
        return self._registry.get(name)
    
    def filter_by_enabled(self, enabled: bool) -> List[Dict[str, Any]]:
        """
        Filter components by enabled status.
        
        Args:
            enabled: Enabled status
            
        Returns:
            List of matching components
        """
        return [meta for meta in self._registry.values() 
                if meta.get('enabled', True) == enabled]
    
    def filter_by_version(self, version: str) -> List[Dict[str, Any]]:
        """
        Filter components by version.
        
        Args:
            version: Component version
            
        Returns:
            List of matching components
        """
        return [meta for meta in self._registry.values() 
                if meta.get('version') == version]
    
    def get_components_by_category(self, category: str) -> List[Dict[str, Any]]:
        """
        Get components by category.
        
        Args:
            category: Component category
            
        Returns:
            List of components in the specified category
        """
        return self.find_by_category(category)
    
    def get_components_by_type(self, type_name: str) -> List[Dict[str, Any]]:
        """
        Get components by type.
        
        Args:
            type_name: Component type
            
        Returns:
            List of components of the specified type
        """
        return self.find_by_type(type_name)
    
    def get_enabled_components(self) -> List[Dict[str, Any]]:
        """
        Get all enabled components.
        
        Returns:
            List of enabled components
        """
        return self.filter_by_enabled(True)
    
    def get_disabled_components(self) -> List[Dict[str, Any]]:
        """
        Get all disabled components.
        
        Returns:
            List of disabled components
        """
        return self.filter_by_enabled(False)
    
    def get_component_count(self) -> int:
        """
        Get total number of registered components.
        
        Returns:
            Number of registered components
        """
        return len(self._registry)
    
    def get_component_count_by_category(self) -> Dict[str, int]:
        """
        Get component count by category.
        
        Returns:
            Dictionary mapping categories to counts
        """
        counts = {}
        for meta in self._registry.values():
            category = meta.get('category', 'unknown')
            counts[category] = counts.get(category, 0) + 1
        return counts
    
    def get_component_count_by_type(self) -> Dict[str, int]:
        """
        Get component count by type.
        
        Returns:
            Dictionary mapping types to counts
        """
        counts = {}
        for meta in self._registry.values():
            type_name = meta.get('type', 'unknown')
            counts[type_name] = counts.get(type_name, 0) + 1
        return counts
    
    def get_component_count_by_tag(self) -> Dict[str, int]:
        """
        Get component count by tag.
        
        Returns:
            Dictionary mapping tags to counts
        """
        counts = {}
        for meta in self._registry.values():
            for tag in meta.get('tags', []):
                counts[tag] = counts.get(tag, 0) + 1
        return counts
    
    def search_components(self, query: str) -> List[Dict[str, Any]]:
        """
        Search components by query.
        
        Args:
            query: Search query
            
        Returns:
            List of matching components
        """
        # Try different search strategies
        results = []
        
        # Search by name
        results.extend(self.search_by_name(query))
        
        # Search by description
        results.extend(self.search_by_description(query))
        
        # Search by full text
        results.extend(self.search_by_full_text(query))
        
        # Remove duplicates
        unique_results = []
        seen = set()
        for meta in results:
            key = meta.get('name', '')
            if key not in seen:
                seen.add(key)
                unique_results.append(meta)
        
        return unique_results
    
    def sync_with_registry(self):
        """Sync with component registry."""
        self._sync_with_registry()
    
    def get_plugin_components(self) -> List[Dict[str, Any]]:
        """
        Get plugin components.
        
        Returns:
            List of plugin components
        """
        plugin_components = []
        
        for plugin_name in self._plugin_manager.get_loaded_plugins():
            plugin = self._plugin_manager.get_plugin(plugin_name)
            if plugin:
                components = plugin.get_components()
                for name, component in components.items():
                    plugin_components.append({
                        'name': f"{plugin_name}_{name}",
                        'plugin': plugin_name,
                        'component': component,
                        'description': f"Component from plugin '{plugin_name}'"
                    })
        
        return plugin_components


# Global discovery API instance
_global_discovery_api: Optional[DiscoveryAPI] = None


def get_discovery_api() -> DiscoveryAPI:
    """Get global discovery API instance."""
    global _global_discovery_api
    if _global_discovery_api is None:
        _global_discovery_api = DiscoveryAPI()
    return _global_discovery_api


def register_component(name: str, metadata: Dict[str, Any]):
    """
    Register a component globally in discovery API.
    
    Args:
        name: Component name
        metadata: Component metadata
    """
    discovery_api = get_discovery_api()
    discovery_api.register_component(name, metadata)


def unregister_component(name: str):
    """
    Unregister a component globally from discovery API.
    
    Args:
        name: Component name
    """
    discovery_api = get_discovery_api()
    discovery_api.unregister_component(name)


def find_by_type(type_name: str) -> List[Dict[str, Any]]:
    """
    Find components by type in global discovery API.
    
    Args:
        type_name: Component type
        
    Returns:
        List of matching components
    """
    discovery_api = get_discovery_api()
    return discovery_api.find_by_type(type_name)


def find_by_category(category: str) -> List[Dict[str, Any]]:
    """
    Find components by category in global discovery API.
    
    Args:
        category: Component category
        
    Returns:
        List of matching components
    """
    discovery_api = get_discovery_api()
    return discovery_api.find_by_category(category)


def search_by_name(name: str) -> List[Dict[str, Any]]:
    """
    Search components by name in global discovery API.
    
    Args:
        name: Component name
        
    Returns:
        List of matching components
    """
    discovery_api = get_discovery_api()
    return discovery_api.search_by_name(name)


def get_all_components() -> List[Dict[str, Any]]:
    """
    Get all registered components globally.
    
    Returns:
        List of all components
    """
    discovery_api = get_discovery_api()
    return discovery_api.get_all_components()