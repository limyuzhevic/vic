"""
Checkpoint management commands for the Advanced CLI.
"""

import logging
from pathlib import Path
from typing import Optional

from ..api import AdvancedAPI

logger = logging.getLogger(__name__)

class CheckpointCommands:
    """
    Checkpoint management commands.
    
    This class provides checkpoint management commands including:
    - List available checkpoints
    - Create manual checkpoints
    - Restore from checkpoints
    - Delete checkpoints
    - Checkpoint statistics
    """
    
    def __init__(self, api: AdvancedAPI):
        """
        Initialize the checkpoint commands.
        
        Args:
            api: Advanced API instance
        """
        self.api = api
    
    def list_checkpoints(self) -> str:
        """List available checkpoints."""
        return self.api.list_checkpoints()
    
    def create_checkpoint(self, name: str, config_path: Optional[str] = None) -> str:
        """
        Create manual checkpoint.
        
        Args:
            name: Checkpoint name
            config_path: Configuration file path
            
        Returns:
            Success message or error
        """
        return self.api.create_checkpoint(name, config_path)
    
    def restore_checkpoint(self, name: str, config_path: Optional[str] = None) -> str:
        """
        Restore from checkpoint.
        
        Args:
            name: Checkpoint name
            config_path: Configuration file path
            
        Returns:
            Success message or error
        """
        return self.api.restore_checkpoint(name, config_path)
    
    def delete_checkpoint(self, name: str) -> str:
        """
        Delete checkpoint.
        
        Args:
            name: Checkpoint name
            
        Returns:
            Success message or error
        """
        return self.api.delete_checkpoint(name)
    
    def get_statistics(self) -> str:
        """Get checkpoint statistics."""
        return self.api.checkpoint_statistics()
