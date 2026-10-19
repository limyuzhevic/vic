"""
Configuration management commands for the Advanced CLI.
"""

import json
import logging
from pathlib import Path
from typing import Dict, Any, Optional
import yaml

from ..api import AdvancedAPI

logger = logging.getLogger(__name__)

class ConfigCommands:
    """
    Configuration management commands.
    
    This class provides configuration management commands including:
    - List all configuration parameters with descriptions
    - Get/set individual parameters
    - Load/save configuration files
    - Validate configuration
    - Create configuration templates
    """
    
    def __init__(self, api: AdvancedAPI):
        """
        Initialize the configuration commands.
        
        Args:
            api: Advanced API instance
        """
        self.api = api
    
    def list_params(self) -> str:
        """List all configuration parameters with descriptions."""
        return self.api.list_config_params()
    
    def get_param(self, key: str) -> str:
        """
        Get individual configuration parameter.
        
        Args:
            key: Parameter key
            
        Returns:
            Parameter value as string
        """
        return self.api.get_config_param(key)
    
    def set_param(self, key: str, value: Any) -> str:
        """
        Set individual configuration parameter.
        
        Args:
            key: Parameter key
            value: Parameter value
            
        Returns:
            Success message or error
        """
        return self.api.set_config_param(key, value)
    
    def load_file(self, source_path: str, target_path: Optional[str] = None) -> str:
        """
        Load configuration from source file.
        
        Args:
            source_path: Source file path
            target_path: Target file path
            
        Returns:
            Success message or error
        """
        return self.api.load_config_file(source_path, target_path)
    
    def save_file(self, target_path: str, source_path: Optional[str] = None) -> str:
        """
        Save configuration to target file.
        
        Args:
            target_path: Target file path
            source_path: Source file path
            
        Returns:
            Success message or error
        """
        return self.api.save_config_file(target_path, source_path)
    
    def validate(self) -> str:
        """Validate configuration."""
        return self.api.validate_config()
    
    def create_template(self, template_path: str) -> str:
        """
        Create configuration template.
        
        Args:
            template_path: Template file path
            
        Returns:
            Success message or error
        """
        return self.api.create_config_template(template_path)
