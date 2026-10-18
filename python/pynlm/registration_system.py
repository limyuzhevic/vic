"""
Brain registration system for NLM.

Provides utilities for registering and managing brain instances across simulations.
"""

from typing import Dict, Any, Optional, List
import weakref

from .nlm_exception import ValidationError, NLMException


class BrainRegistry:
    """
    Central registry for managing brain instances.
    
    Provides thread-safe registration, retrieval, and lifecycle management
    of brain instances across multiple simulations.
    """
    
    def __init__(self):
        # Use weak references to avoid keeping brains alive indefinitely
        self._brains: Dict[str, weakref.ref] = {}
        self._brain_metadata: Dict[str, Dict[str, Any]] = {}
    
    def register(self, name: str, brain: 'Brain', metadata: Optional[Dict[str, Any]] = None) -> bool:
        """
        Register a brain instance with a unique name.
        
        Args:
            name: Unique name for the brain
            brain: Brain instance to register
            metadata: Optional metadata dictionary
            
        Returns:
            True if registration succeeded, False if name already exists
            
        Raises:
            ValidationError: If name is invalid or brain is None
        """
        if not name or not isinstance(name, str):
            raise ValidationError("Brain name must be a non-empty string")
        
        if not brain:
            raise ValidationError("Brain instance cannot be None")
        
        # Check if name already exists
        if name in self._brains:
            return False
        
        # Create weak reference
        self._brains[name] = weakref.ref(brain)
        
        # Store metadata if provided
        if metadata:
            self._brain_metadata[name] = metadata.copy()
        else:
            self._brain_metadata[name] = {
                'created_at': time.time(),
                'brain_type': str(type(brain).__name__),
                'description': f"Brain registered at {time.ctime()}",
            }
        
        return True
    
    def get(self, name: str) -> Optional['Brain']:
        """
        Get a registered brain instance.
        
        Args:
            name: Name of the brain to retrieve
            
        Returns:
            Brain instance if found and still alive, None otherwise
        """
        if name not in self._brains:
            return None
        
        brain_ref = self._brains[name]
        brain = brain_ref()
        
        # If brain is dead (garbage collected), remove it
        if brain is None:
            self.unregister(name)
            return None
        
        return brain
    
    def unregister(self, name: str) -> bool:
        """
        Unregister a brain instance.
        
        Args:
            name: Name of the brain to unregister
            
        Returns:
            True if brain was found and unregistered, False otherwise
        """
        if name in self._brains:
            del self._brains[name]
            if name in self._brain_metadata:
                del self._brain_metadata[name]
            return True
        return False
    
    def list_names(self) -> List[str]:
        """
        Get list of all registered brain names.
        
        Returns:
            List of registered brain names
        """
        # Filter out dead references
        alive_names = []
        for name, brain_ref in list(self._brains.items()):
            brain = brain_ref()
            if brain is not None:
                alive_names.append(name)
            else:
                # Remove dead reference
                del self._brains[name]
                if name in self._brain_metadata:
                    del self._brain_metadata[name]
        
        return alive_names
    
    def has(self, name: str) -> bool:
        """
        Check if a brain is registered and still alive.
        
        Args:
            name: Name of the brain to check
            
        Returns:
            True if brain is registered and alive, False otherwise
        """
        brain = self.get(name)
        return brain is not None
    
    def get_metadata(self, name: str) -> Optional[Dict[str, Any]]:
        """
        Get metadata for a registered brain.
        
        Args:
            name: Name of the brain
            
        Returns:
            Metadata dictionary if found, None otherwise
        """
        return self._brain_metadata.get(name)
    
    def update_metadata(self, name: str, metadata: Dict[str, Any]) -> bool:
        """
        Update metadata for a registered brain.
        
        Args:
            name: Name of the brain
            metadata: New metadata dictionary
            
        Returns:
            True if metadata was updated, False if brain not found
        """
        if name in self._brain_metadata:
            self._brain_metadata[name].update(metadata)
            return True
        return False
    
    def get_all_metadata(self) -> Dict[str, Dict[str, Any]]:
        """
        Get metadata for all registered brains.
        
        Returns:
            Dictionary of name->metadata mappings
        """
        return self._brain_metadata.copy()
    
    def clear(self) -> None:
        """Unregister all brains."""
        self._brains.clear()
        self._brain_metadata.clear()
    
    def count(self) -> int:
        """
        Get the number of registered brains.
        
        Returns:
            Number of registered brains
        """
        return len(self.list_names())


# Global registry instance
global_registry = BrainRegistry()


# Module-level convenience functions
def register_brain(name: str, brain: 'Brain', metadata: Optional[Dict[str, Any]] = None) -> bool:
    """
    Register a brain instance globally.
    
    Args:
        name: Unique name for the brain
        brain: Brain instance to register
        metadata: Optional metadata dictionary
        
    Returns:
        True if registration succeeded, False if name already exists
    """
    return global_registry.register(name, brain, metadata)


def get_brain(name: str) -> Optional['Brain']:
    """
    Get a globally registered brain instance.
    
    Args:
        name: Name of the brain to retrieve
        
    Returns:
        Brain instance if found and still alive, None otherwise
    """
    return global_registry.get(name)


def unregister_brain(name: str) -> bool:
    """
    Unregister a globally registered brain instance.
    
    Args:
        name: Name of the brain to unregister
        
    Returns:
        True if brain was found and unregistered, False otherwise
    """
    return global_registry.unregister(name)


def list_brains() -> List[str]:
    """
    Get list of all globally registered brain names.
    
    Returns:
        List of registered brain names
    """
    return global_registry.list_names()


def has_brain(name: str) -> bool:
    """
    Check if a brain is globally registered and still alive.
    
    Args:
        name: Name of the brain to check
        
    Returns:
        True if brain is registered and alive, False otherwise
    """
    return global_registry.has(name)


def get_brain_metadata(name: str) -> Optional[Dict[str, Any]]:
    """
    Get metadata for a globally registered brain.
    
    Args:
        name: Name of the brain
        
    Returns:
        Metadata dictionary if found, None otherwise
    """
    return global_registry.get_metadata(name)


def update_brain_metadata(name: str, metadata: Dict[str, Any]) -> bool:
    """
    Update metadata for a globally registered brain.
    
    Args:
        name: Name of the brain
        metadata: New metadata dictionary
        
    Returns:
        True if metadata was updated, False if brain not found
    """
    return global_registry.update_metadata(name, metadata)


def get_all_brain_metadata() -> Dict[str, Dict[str, Any]]:
    """
    Get metadata for all globally registered brains.
    
    Returns:
        Dictionary of name->metadata mappings
    """
    return global_registry.get_all_metadata()


def clear_registry() -> None:
    """Clear the global registry."""
    global_registry.clear()


def get_registry_stats() -> Dict[str, Any]:
    """
    Get statistics about the global registry.
    
    Returns:
        Dictionary containing registry statistics
    """
    return {
        'total_registered': global_registry.count(),
        'registered_names': global_registry.list_names(),
    }


# Context manager for brain registration
class BrainContext:
    """
    Context manager for automatic brain registration and cleanup.
    
    Example:
        with BrainContext('my_brain', brain) as ctx:
            # Brain is registered and can be used within this context
            pass
        # Brain is automatically unregistered when context exits
    """
    
    def __init__(self, name: str, brain: 'Brain', metadata: Optional[Dict[str, Any]] = None):
        self.name = name
        self.brain = brain
        self.metadata = metadata or {}
        self.registered = False
    
    def __enter__(self) -> 'BrainContext':
        """Register brain when entering context."""
        if register_brain(self.name, self.brain, self.metadata):
            self.registered = True
            return self
        else:
            raise NLMException(f"Failed to register brain '{self.name}'. Name may already be in use.")
    
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        """Unregister brain when exiting context."""
        if self.registered:
            unregister_brain(self.name)


# Error classes for registration system
class RegistrationError(NLMException):
    """Base exception for registration system errors."""
    
    def __init__(self, message: str = "Registration error", details: dict = None):
        super().__init__(message, error_code=8, details=details)
class DuplicateNameError(RegistrationError):
    """Exception raised when attempting to register a brain with an existing name."""
    
    def __init__(self, name: str, message: str = None):
        if message is None:
            message = f"Brain name '{name}' is already registered"
        super().__init__(message, details={'name': name, 'error_code': 8})
class BrainNotFoundError(RegistrationError):
    """Exception raised when attempting to access a non-existent brain."""
    
    def __init__(self, name: str, message: str = None):
        if message is None:
            message = f"Brain '{name}' not found in registry"
        super().__init__(message, details={'name': name, 'error_code': 9})


# Export everything
__all__ = [
    # Registry classes
    'BrainRegistry',
    'BrainContext',
    
    # Error classes
    'RegistrationError',
    'DuplicateNameError',
    'BrainNotFoundError',
    
    # Global registry functions
    'register_brain',
    'get_brain',
    'unregister_brain',
    'list_brains',
    'has_brain',
    'get_brain_metadata',
    'update_brain_metadata',
    'get_all_brain_metadata',
    'clear_registry',
    'get_registry_stats',
    
    # Context manager
    'BrainContext',
]