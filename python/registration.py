"""
Component registration API for NLM (Neural Learning Machine).

This module provides component registration APIs for adding and updating
components in the system.

Key features:
- Component registration and management
- Component metadata validation
- Component event handling
- Component dependency management
- Component conflict detection
- Component version management
- Component configuration management
- Component lifecycle events

The Component Registration API is part of NLM Phase 6 "FINAL INTEGRATION",
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
import time
from enum import Enum

from .exceptions import NLMError, NLMConfigurationError
from .registry import ComponentMetadata
from .discovery import DiscoveryAPI, get_discovery_api


class RegistrationEvent(Enum):
    """Registration events."""
    
    COMPONENT_REGISTERED = "component_registered"
    COMPONENT_UPDATED = "component_updated"
    COMPONENT_UNREGISTERED = "component_unregistered"


class RegistrationAPI:
    """Component registration API for NLM."""
    
    def __init__(self):
        """Initialize registration API."""
        self._registry: Dict[str, Dict] = {}
        self._event_callbacks: List[Callable] = []
        self._logger = logging.getLogger(__name__)
    
    def register_component(self, name: str, metadata: Dict[str, Any], 
                          factory: Optional[Callable] = None) -> bool:
        """
        Register a component.
        
        Args:
            name: Component name
            metadata: Component metadata
            factory: Component factory function (optional)
            
        Returns:
            True if component registered successfully, False otherwise
        """
        try:
            # Validate metadata
            self._validate_metadata(metadata)
            
            # Check if component already exists
            if name in self._registry:
                self._logger.warning(f"Component '{name}' already registered")
                return False
            
            # Register component
            metadata['registered_at'] = time.time()
            metadata['factory'] = factory
            self._registry[name] = metadata
            
            # Notify event callbacks
            for callback in self._event_callbacks:
                try:
                    callback(RegistrationEvent.COMPONENT_REGISTERED, name, metadata)
                except Exception as e:
                    self._logger.error(f"Error in event callback: {e}")
            
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
            if name not in self._registry:
                self._logger.warning(f"Component '{name}' not found")
                return False
            
            # Get component metadata before removing
            metadata = self._registry[name].copy()
            
            # Unregister component
            del self._registry[name]
            
            # Notify event callbacks
            for callback in self._event_callbacks:
                try:
                    callback(RegistrationEvent.COMPONENT_UNREGISTERED, name, metadata)
                except Exception as e:
                    self._logger.error(f"Error in event callback: {e}")
            
            self._logger.info(f"Component '{name}' unregistered successfully")
            return True
            
        except Exception as e:
            self._logger.error(f"Failed to unregister component '{name}': {e}")
            return False
    
    def update_component(self, name: str, new_metadata: Dict[str, Any]) -> bool:
        """
        Update a component.
        
        Args:
            name: Component name
            new_metadata: New component metadata
            
        Returns:
            True if component updated successfully, False otherwise
        """
        try:
            if name not in self._registry:
                self._logger.warning(f"Component '{name}' not found")
                return False
            
            # Validate new metadata
            self._validate_metadata(new_metadata)
            
            # Update component
            old_metadata = self._registry[name].copy()
            self._registry[name].update(new_metadata)
            self._registry[name]['updated_at'] = time.time()
            
            # Notify event callbacks
            for callback in self._event_callbacks:
                try:
                    callback(RegistrationEvent.COMPONENT_UPDATED, name, 
                            {'old': old_metadata, 'new': self._registry[name]})
                except Exception as e:
                    self._logger.error(f"Error in event callback: {e}")
            
            self._logger.info(f"Component '{name}' updated successfully")
            return True
            
        except Exception as e:
            self._logger.error(f"Failed to update component '{name}': {e}")
            return False
    
    def get_component(self, name: str) -> Optional[Dict[str, Any]]:
        """
        Get component by name.
        
        Args:
            name: Component name
            
        Returns:
            Component metadata if found, None otherwise
        """
        return self._registry.get(name)
    
    def has_component(self, name: str) -> bool:
        """
        Check if component exists.
        
        Args:
            name: Component name
            
        Returns:
            True if component exists, False otherwise
        """
        return name in self._registry
    
    def get_all_components(self) -> List[Dict[str, Any]]:
        """
        Get all registered components.
        
        Returns:
            List of all components
        """
        return list(self._registry.values())
    
    def get_component_count(self) -> int:
        """
        Get total number of registered components.
        
        Returns:
            Number of registered components
        """
        return len(self._registry)
    
    def clear(self):
        """Clear all registered components."""
        self._registry.clear()
        self._logger.debug("Registration API cleared all components")
    
    def _validate_metadata(self, metadata: Dict[str, Any]):
        """
        Validate component metadata.
        
        Args:
            metadata: Component metadata
            
        Raises:
            NLMConfigurationError: If metadata is invalid
        """
        required_fields = ['name', 'version', 'type', 'description']
        for field in required_fields:
            if field not in metadata:
                raise NLMConfigurationError(f"Missing required field: {field}")
        
        if not isinstance(metadata['version'], str):
            raise NLMConfigurationError("Version must be a string")
        
        if not isinstance(metadata['description'], str):
            raise NLMConfigurationError("Description must be a string")
    
    def register_event_callback(self, callback: Callable):
        """
        Register event callback.
        
        Args:
            callback: Event callback function
        """
        self._event_callbacks.append(callback)


# Global registration API instance
_global_registration_api: Optional[RegistrationAPI] = None


def get_registration_api() -> RegistrationAPI:
    """Get global registration API instance."""
    global _global_registration_api
    if _global_registration_api is None:
        _global_registration_api = RegistrationAPI()
    return _global_registration_api


def register_component(name: str, metadata: Dict[str, Any], 
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
    registration_api = get_registration_api()
    return registration_api.register_component(name, metadata, factory)


def unregister_component(name: str) -> bool:
    """
    Unregister a component globally.
    
    Args:
        name: Component name
        
    Returns:
        True if component unregistered successfully, False otherwise
    """
    registration_api = get_registration_api()
    return registration_api.unregister_component(name)


def update_component(name: str, new_metadata: Dict[str, Any]) -> bool:
    """
    Update a component globally.
    
    Args:
        name: Component name
        new_metadata: New component metadata
        
    Returns:
        True if component updated successfully, False otherwise
    """
    registration_api = get_registration_api()
    return registration_api.update_component(name, new_metadata)


def get_component(name: str) -> Optional[Dict[str, Any]]:
    """
    Get component by name from global registration API.
    
    Args:
        name: Component name
        
    Returns:
        Component metadata if found, None otherwise
    """
    registration_api = get_registration_api()
    return registration_api.get_component(name)


def has_component(name: str) -> bool:
    """
    Check if component exists globally.
    
    Args:
        name: Component name
        
    Returns:
        True if component exists, False otherwise
    """
    registration_api = get_registration_api()
    return registration_api.has_component(name)


def get_all_components() -> List[Dict[str, Any]]:
    """
    Get all registered components globally.
    
    Returns:
        List of all components
    """
    registration_api = get_registration_api()
    return registration_api.get_all_components()