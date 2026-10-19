"""
Memory system commands for the Advanced CLI.
"""

import logging
from typing import Dict, Any

from ..api import AdvancedAPI

logger = logging.getLogger(__name__)

class MemoryCommands:
    """
    Memory system commands.
    
    This class provides memory system commands including:
    - Inspect memory contents (working, episodic)
    - Search memories by criteria
    - Consolidate memories
    - Replay specific memories
    """
    
    def __init__(self, api: AdvancedAPI):
        """
        Initialize the memory commands.
        
        Args:
            api: Advanced API instance
        """
        self.api = api
    
    def inspect_contents(self, memory_type: str) -> str:
        """
        Inspect memory contents (working, episodic).
        
        Args:
            memory_type: Type of memory to inspect
            
        Returns:
            Success message or error
        """
        return self.api.inspect_memory_contents(memory_type)
    
    def search_memories(self, criteria: Dict[str, Any]) -> str:
        """
        Search memories by criteria.
        
        Args:
            criteria: Search criteria dictionary
            
        Returns:
            Success message or error
        """
        return self.api.search_memories(criteria)
    
    def consolidate(self) -> str:
        """Consolidate memories."""
        return self.api.consolidate_memories()
    
    def replay(self, memory_id: str) -> str:
        """
        Replay specific memory.
        
        Args:
            memory_id: Memory ID to replay
            
        Returns:
            Success message or error
        """
        return self.api.replay_memory(memory_id)
