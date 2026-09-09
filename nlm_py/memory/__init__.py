"""
NLM Python Memory Module

This module contains memory systems for the NLM Python bindings.
"""

import numpy as np
from typing import Optional, List, Dict, Any, Tuple
from .exceptions import NLMError, MemoryError
from .core import Timestamp, NeuronId, SynapseId

# Import base classes
try:
    from ..brain import Brain
    _bindings_available = True
except ImportError:
    _bindings_available = False

class MemoryEntry:
    """A single memory entry."""
    
    def __init__(self, id: int, content: Any, timestamp: Timestamp, 
                 strength: float = 1.0, type: str = "episodic"):
        self.id = id
        self.content = content
        self.timestamp = timestamp
        self.strength = strength
        self.type = type  # "episodic", "working", "associative"
        self.access_count = 0
        self.last_accessed = timestamp
    
    def access(self):
        """Record an access to this memory."""
        self.access_count += 1
        self.last_accessed = Timestamp()
        # Increase strength with use (Hebbian-like)
        self.strength = min(1.0, self.strength * 1.05)
    
    def decay(self, decay_rate: float = 0.99):
        """Decay memory strength over time."""
        self.strength *= decay_rate
        self.last_accessed = Timestamp()


class MemorySystem:
    """Base class for memory systems."""
    
    def __init__(self, name: str):
        self.name = name
        self.enabled = False
        self.brain = None
        self._memory_ptr = None
        
        if _bindings_available:
            try:
                # This would need to be implemented in bindings.cpp
                pass
            except Exception:
                self._memory_ptr = None
    
    def initialize(self, brain):
        """Initialize memory system with brain reference."""
        self.brain = brain
        if _bindings_available and self._memory_ptr:
            return self._memory_ptr.initialize(brain._brain_ptr if brain._brain_ptr else None)
        return True
    
    def store(self, content: Any, type: str = "episodic", strength: float = 1.0):
        """Store content in memory."""
        raise NotImplementedError("Subclasses must implement store method")
    
    def retrieve(self, query: Any = None, type: Optional[str] = None, 
                 limit: int = 1) -> List[MemoryEntry]:
        """Retrieve memory entries matching query."""
        raise NotImplementedError("Subclasses must implement retrieve method")
    
    def update(self, dt: float):
        """Update memory system."""
        if _bindings_available and self._memory_ptr:
            return self._memory_ptr.update(dt)
        
        # Python fallback: decay all memories
        for entry in self.get_all_entries():
            entry.decay(0.99)
    
    def get_all_entries(self) -> List[MemoryEntry]:
        """Get all memory entries."""
        raise NotImplementedError("Subclasses must implement get_all_entries method")
    
    def clear(self):
        """Clear all memory."""
        if _bindings_available and self._memory_ptr:
            return self._memory_ptr.clear()
        # Python fallback
        pass


class NeuralWorkingMemory(MemorySystem):
    """Working memory - transient active information."""
    
    def __init__(self):
        super().__init__("working_memory")
        self.capacity = 10  # Maximum number of items
        self.entries = []
        self.input_buffer = []
        self.focus = None  # Currently attended item
    
    def initialize(self, brain):
        """Initialize working memory."""
        super().initialize(brain)
        self.capacity = brain.get_config().get('working_memory_capacity', 10) if brain else 10
        return True
    
    def store(self, content: Any, type: str = "episodic", strength: float = 1.0):
        """Store content in working memory."""
        if len(self.entries) >= self.capacity:
            # Remove oldest entry
            self.entries.sort(key=lambda x: x.timestamp)
            self.entries.pop(0)
        
        entry = MemoryEntry(
            id=len(self.entries),
            content=content,
            timestamp=Timestamp(),
            strength=strength,
            type=type
        )
        self.entries.append(entry)
        
        # Also add to input buffer if it's a new percept
        if type == "percept":
            self.input_buffer.append(entry)
    
    def retrieve(self, query: Any = None, type: Optional[str] = None, 
                 limit: int = 1) -> List[MemoryEntry]:
        """Retrieve memory entries matching query."""
        results = []
        
        for entry in self.entries:
            if type and entry.type != type:
                continue
            if query is not None and entry.content != query:
                continue
            
            results.append(entry)
            entry.access()
            
            if len(results) >= limit:
                break
        
        return results
    
    def get_all_entries(self) -> List[MemoryEntry]:
        """Get all memory entries."""
        return self.entries
    
    def set_focus(self, entry_id: int):
        """Set the focus to a specific memory entry."""
        for entry in self.entries:
            if entry.id == entry_id:
                self.focus = entry
                entry.access()
                return
        raise MemoryError(f"Memory entry not found: {entry_id}")
    
    def clear_focus(self):
        """Clear the focus."""
        self.focus = None
    
    def get_focus(self) -> Optional[MemoryEntry]:
        """Get the current focus."""
        return self.focus
    
    def process_input(self, percept: Any):
        """Process sensory input into working memory."""
        # Add percept to working memory
        self.store(percept, type="percept", strength=0.8)
        
        # If working memory is not focused, set focus to latest percept
        if self.focus is None and self.entries:
            self.focus = self.entries[-1]
        
        # Clear input buffer
        self.input_buffer = []


class NeuralEpisodicMemory(MemorySystem):
    """Episodic memory - experience storage."""
    
    def __init__(self):
        super().__init__("episodic_memory")
        self.episodes = []
        self.max_episodes = 1000
        self.consolidation_threshold = 0.7
        self.replay_buffer = []
    
    def store(self, content: Any, type: str = "episodic", strength: float = 1.0):
        """Store an episode."""
        if len(self.episodes) >= self.max_episodes:
            # Remove oldest episode
            self.episodes.sort(key=lambda x: x.timestamp)
            self.episodes.pop(0)
        
        episode = MemoryEntry(
            id=len(self.episodes),
            content=content,
            timestamp=Timestamp(),
            strength=strength,
            type=type
        )
        self.episodes.append(episode)
    
    def retrieve(self, query: Any = None, type: Optional[str] = None, 
                 limit: int = 1) -> List[MemoryEntry]:
        """Retrieve episodes matching query."""
        results = []
        
        for episode in self.episodes:
            if type and episode.type != type:
                continue
            if query is not None:
                # Simple matching based on content
                if hasattr(query, '__eq__') and episode.content == query:
                    results.append(episode)
                elif str(episode.content) == str(query):
                    results.append(episode)
            else:
                results.append(episode)
            
            episode.access()
            
            if len(results) >= limit:
                break
        
        return results
    
    def get_all_entries(self) -> List[MemoryEntry]:
        """Get all memory entries."""
        return self.episodes
    
    def consolidate(self):
        """Consolidate important episodes into long-term memory."""
        strong_episodes = [e for e in self.episodes if e.strength > self.consolidation_threshold]
        
        for episode in strong_episodes:
            # Move to replay buffer for replay
            self.replay_buffer.append(episode)
            # Reduce strength to indicate it's been consolidated
            episode.strength *= 0.5
        
        return len(strong_episodes)
    
    def replay(self) -> List[MemoryEntry]:
        """Replay consolidated episodes."""
        # Simple replay implementation
        replayed = []
        for episode in self.replay_buffer[:10]:  # Replay last 10 consolidated episodes
            episode.access()
            replayed.append(episode)
        
        # Clear replay buffer after replay
        self.replay_buffer.clear()
        return replayed


class NeuralAssociativeMemory(MemorySystem):
    """Associative memory - pattern associations."""
    
    def __init__(self):
        super().__init__("associative_memory")
        self.associations = {}  # Pattern -> associated memory
        self.patterns = []
        self.max_patterns = 100
    
    def store(self, content: Any, type: str = "episodic", strength: float = 1.0):
        """Store with associative links."""
        if not self._extract_pattern(content):
            return
        
        if len(self.patterns) >= self.max_patterns:
            # Remove oldest pattern
            oldest_pattern = self.patterns[0]
            if oldest_pattern in self.associations:
                del self.associations[oldest_pattern]
            self.patterns.pop(0)
        
        self.patterns.append(content)
        self.associations[content] = {
            'strength': strength,
            'timestamp': Timestamp(),
            'access_count': 0
        }
    
    def retrieve(self, query: Any = None, type: Optional[str] = None, 
                 limit: int = 1) -> List[MemoryEntry]:
        """Retrieve associations for query pattern."""
        if query is None or query not in self.associations:
            return []
        
        assoc_data = self.associations[query]
        assoc_data['access_count'] += 1
        
        # Create memory entry from association
        entry = MemoryEntry(
            id=len(self.associations),
            content=assoc_data,
            timestamp=assoc_data['timestamp'],
            strength=assoc_data['strength'],
            type="associative"
        )
        
        return [entry]
    
    def get_all_entries(self) -> List[MemoryEntry]:
        """Get all memory entries."""
        entries = []
        for pattern, data in self.associations.items():
            entry = MemoryEntry(
                id=id(pattern),
                content=pattern,
                timestamp=data['timestamp'],
                strength=data['strength'],
                type="associative"
            )
            entry.access_count = data['access_count']
            entries.append(entry)
        
        return entries
    
    def _extract_pattern(self, content: Any) -> bool:
        """Extract pattern from content for association."""
        if content is None:
            return False
        
        # Simple pattern extraction
        if isinstance(content, dict):
            # Use keys as pattern
            pattern = tuple(sorted(content.keys()))
        elif isinstance(content, (list, tuple)):
            pattern = tuple(content)
        else:
            pattern = str(content)
        
        return True
    
    def strengthen_association(self, pattern1: Any, pattern2: Any, strength: float = 0.1):
        """Strengthen association between two patterns."""
        # This is a simplified implementation
        pass


class Memory:
    """Convenience class for memory operations."""
    
    def __init__(self):
        self.working_memory = NeuralWorkingMemory()
        self.episodic_memory = NeuralEpisodicMemory()
        self.associative_memory = NeuralAssociativeMemory()
        self.brain = None
    
    def initialize(self, brain):
        """Initialize all memory systems."""
        self.brain = brain
        self.working_memory.initialize(brain)
        self.episodic_memory.initialize(brain)
        self.associative_memory.initialize(brain)
    
    def store_percept(self, percept: Any):
        """Store a sensory percept."""
        self.working_memory.process_input(percept)
        self.episodic_memory.store(percept, type="percept", strength=0.7)
    
    def retrieve_recent(self, type: Optional[str] = None, limit: int = 5) -> List[MemoryEntry]:
        """Retrieve recent memories."""
        if type == "working":
            return self.working_memory.retrieve(type="percept", limit=limit)
        elif type == "episodic":
            return self.episodic_memory.retrieve(type="percept", limit=limit)
        elif type == "associative":
            return self.associative_memory.retrieve(limit=limit)
        else:
            # Return from all systems
            all_memories = []
            all_memories.extend(self.working_memory.retrieve(limit=limit))
            all_memories.extend(self.episodic_memory.retrieve(limit=limit))
            all_memories.extend(self.associative_memory.retrieve(limit=limit))
            
            # Sort by timestamp (most recent first)
            all_memories.sort(key=lambda x: x.timestamp, reverse=True)
            return all_memories[:limit]
    
    def consolidate(self):
        """Consolidate episodic memories."""
        return self.episodic_memory.consolidate()
    
    def replay(self) -> List[MemoryEntry]:
        """Replay consolidated memories."""
        return self.episodic_memory.replay()
    
    def update(self, dt: float):
        """Update all memory systems."""
        self.working_memory.update(dt)
        self.episodic_memory.update(dt)
        self.associative_memory.update(dt)
    
    def clear(self):
        """Clear all memory."""
        self.working_memory.clear()
        self.episodic_memory.clear()
        self.associative_memory.clear()


# Export public interface
__all__ = [
    'MemoryEntry',
    'MemorySystem',
    'NeuralWorkingMemory',
    'NeuralEpisodicMemory',
    'NeuralAssociativeMemory',
    'Memory',
]
