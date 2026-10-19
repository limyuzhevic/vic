"""
World management commands for the Advanced CLI.
"""

import logging
from typing import Dict, Any

from ..api import AdvancedAPI

logger = logging.getLogger(__name__)

class WorldCommands:
    """
    World management commands.
    
    This class provides world management commands including:
    - Add/remove world objects
    - Modify world parameters
    - Set agent start positions
    - World validation
    """
    
    def __init__(self, api: AdvancedAPI):
        """
        Initialize the world commands.
        
        Args:
            api: Advanced API instance
        """
        self.api = api
    
    def add_object(self, obj_type: str, x: float, y: float, properties: Dict[str, Any]) -> str:
        """
        Add/remove world objects.
        
        Args:
            obj_type: Object type
            x: X coordinate
            y: Y coordinate
            properties: Object properties
            
        Returns:
            Success message or error
        """
        return self.api.add_world_object(obj_type, x, y, properties)
    
    def modify_parameters(self, parameters: Dict[str, Any]) -> str:
        """
        Modify world parameters.
        
        Args:
            parameters: World parameters
            
        Returns:
            Success message or error
        """
        return self.api.modify_world_parameters(parameters)
    
    def set_agent_start_position(self, x: float, y: float) -> str:
        """
        Set agent start positions.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            Success message or error
        """
        return self.api.set_agent_start_position(x, y)
    
    def validate_world(self) -> str:
        """World validation."""
        return self.api.world_validation()
