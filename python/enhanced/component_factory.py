"""
Component Factory for NLM

This module provides a plugin architecture for extending NLM functionality
with custom components.
"""

import sys
from pathlib import Path

# Add parent directory to Python path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

class ComponentFactory:
    """
    Factory for creating and managing custom NLM components.
    
    This class provides a plugin architecture for:
    - Component registration
    - Component initialization
    - Configuration of components
    - Dynamic component creation
    """
    
    def __init__(self):
        """Initialize component factory"""
        try:
            from pynlm_enhanced import ComponentFactory as EnhancedFactory
            self._factory = EnhancedFactory()
        except ImportError:
            raise ImportError("Enhanced NLM bindings not available")
    
    def register_component(self, name, version, initializer=None, 
                          configurators=None, creators=None):
        """
        Register a new component.
        
        Args:
            name: Component name
            version: Component version
            initializer: Initialization function (optional)
            configurators: Configuration functions dictionary (optional)
            creators: Creation functions dictionary (optional)
        """
        if configurators is None:
            configurators = {}
        if creators is None:
            creators = {}
        
        self._factory.registerComponent(name, version, initializer, configurators, creators)
    
    def initialize_component(self, brain, component_name):
        """
        Initialize a component with a brain.
        
        Args:
            brain: Brain object to initialize with
            component_name: Name of component to initialize
            
        Returns:
            True if initialization successful
        """
        return self._factory.initializeComponent(brain, component_name)
    
    def configure_component(self, brain, component_name, config_key, config_value):
        """
        Configure a component.
        
        Args:
            brain: Brain object to configure
            component_name: Name of component to configure
            config_key: Configuration key
            config_value: Configuration value
            
        Returns:
            True if configuration successful
        """
        return self._factory.configureComponent(brain, component_name, config_key, config_value)
    
    def create_component(self, brain, component_name, instance_name):
        """
        Create an instance of a component.
        
        Args:
            brain: Brain object to create component with
            component_name: Name of component to create
            instance_name: Name for the component instance
            
        Returns:
            Component instance or None if creation failed
        """
        return self._factory.createComponent(brain, component_name, instance_name)
    
    def get_registered_components(self):
        """
        Get list of registered component names.
        
        Returns:
            List of registered component names
        """
        return self._factory.getRegisteredComponents()