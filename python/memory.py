"""
Memory systems for NLM - working, episodic, and associative memory.

This module provides the memory systems that store and process information
in the NLM artificial brain system.
"""

import sys
from typing import List, Dict, Any, Optional

__all__ = [
    'WorkingMemory',
    'EpisodicMemory', 
    'AssociativeMemory',
]


class WorkingMemory:
    """Working memory for transient active information.
    
    Working memory maintains active information through recurrent connections
    and competitive processes. It's used for short-term storage and active
    processing during cognitive tasks.
    
    Key features:
    - Capacity-limited storage of neural activations
    - Competitive dynamics between traces
    - Decay of weak traces
    - Maintenance of active traces through recurrent connections
    
    Example:
        >>> from pynlm import createBrain
        >>> brain = createBrain()
        >>> brain.initialize()
        >>> 
        >>> working_memory = brain.getWorkingMemory()
        >>> if working_memory:
        ...     working_memory.storeToNeuron(neuron_id, activation)
        ...     active_traces = working_memory.getActiveTraces()
        ...     print(f"Active traces: {active_traces}")
    """
    
    def __init__(self, working_memory_ptr=None):
        self._working_memory = working_memory_ptr
    
    def initialize(self, brain_ptr=None) -> bool:
        """Initialize working memory.
        
        Args:
            brain_ptr: Brain pointer for neural integration.
            
        Returns:
            bool: True if initialization successful.
        """
        if self._working_memory:
            return self._working_memory.initialize(brain_ptr)
        return False
    
    def setCapacity(self, capacity: int) -> bool:
        """Set working memory capacity.
        
        Args:
            capacity: Maximum number of traces.
            
        Returns:
            bool: True if capacity set successfully.
        """
        if self._working_memory:
            return self._working_memory.setCapacity(capacity)
        return False
    
    def storeToNeuron(self, neuron_id: int, activation: float) -> bool:
        """Store neuron activation in working memory.
        
        Args:
            neuron_id: Target neuron ID.
            activation: Activation level (0.0 to 1.0).
            
        Returns:
            bool: True if stored successfully.
        """
        if self._working_memory:
            return self._working_memory.storeToNeuron(neuron_id, activation)
        return False
    
    def store(self, pattern: List[float], strength: float) -> bool:
        """Store a pattern in working memory.
        
        Args:
            pattern: Pattern to store (list of activation values).
            strength: Pattern strength (0.0 to 1.0).
            
        Returns:
            bool: True if pattern stored successfully.
        """
        if self._working_memory:
            return self._working_memory.store(pattern, strength)
        return False
    
    def update(self, dt: float) -> bool:
        """Update working memory dynamics.
        
        Args:
            dt: Time step duration.
            
        Returns:
            bool: True if update successful.
        """
        if self._working_memory:
            return self._working_memory.update(dt)
        return False
    
    def clear(self) -> bool:
        """Clear all working memory traces.
        
        Returns:
            bool: True if cleared successfully.
        """
        if self._working_memory:
            return self._working_memory.clear()
        return False
    
    def getMemoryNeurons(self) -> List[int]:
        """Get neurons currently in working memory.
        
        Returns:
            List[int]: List of neuron IDs stored in working memory.
        """
        if self._working_memory:
            return self._working_memory.getMemoryNeurons()
        return []
    
    def getActiveTraces(self) -> int:
        """Get number of active memory traces.
        
        Returns:
            int: Number of active traces.
        """
        if self._working_memory:
            return self._working_memory.getActiveTraces()
        return 0
    
    def contains(self, neuron_id: int) -> bool:
        """Check if neuron is in working memory.
        
        Args:
            neuron_id: Target neuron ID.
            
        Returns:
            bool: True if neuron is in working memory.
        """
        if self._working_memory:
            return self._working_memory.contains(neuron_id)
        return False
    
    def getNeuronActivation(self, neuron_id: int) -> float:
        """Get neuron activation from working memory.
        
        Args:
            neuron_id: Target neuron ID.
            
        Returns:
            float: Activation level (0.0 to 1.0), or 0.0 if not found.
        """
        if self._working_memory:
            return self._working_memory.getNeuronActivation(neuron_id)
        return 0.0
    
    def isWinning(self, neuron_id: int) -> bool:
        """Check if neuron is a winner in working memory competition.
        
        Args:
            neuron_id: Target neuron ID.
            
        Returns:
            bool: True if neuron is a winner.
        """
        if self._working_memory:
            return self._working_memory.isWinning(neuron_id)
        return False
    
    def getMemoryActivity(self) -> float:
        """Get overall working memory activity.
        
        Returns:
            float: Average activation across all traces.
        """
        if self._working_memory:
            return self._working_memory.getMemoryActivity()
        return 0.0
    
    def __repr__(self) -> str:
        if self._working_memory:
            return f"<WorkingMemory: {self.getActiveTraces()} active traces, {len(self.getMemoryNeurons())} stored neurons>"
        return "<WorkingMemory: uninitialized>"


class EpisodicMemory:
    """Episodic memory for experience storage and replay.
    
    Episodic memory stores episodes of experience with timestamps, rewards,
    and neural activity patterns. It supports replay of important episodes
    for memory consolidation and learning.
    
    Key features:
    - Episode storage with timestamps and rewards
    - Replay during rest or periodically
    - Consolidation of important memories
    - Integration with working memory and concept formation
    
    Example:
        >>> from pynlm import createBrain
        >>> brain = createBrain()
        >>> brain.initialize()
        >>> 
        >>> episodic_memory = brain.getEpisodicMemory()
        >>> if episodic_memory:
        ...     episode_count = episodic_memory.getEpisodeCount()
        ...     print(f"Stored {episode_count} episodes")
        ...     
        ...     # Get episodes for replay
        ...     episodes = episodic_memory.getEpisodesForReplay(3)
    """
    
    def __init__(self, episodic_memory_ptr=None):
        self._episodic_memory = episodic_memory_ptr
    
    def initialize(self, brain_ptr=None) -> bool:
        """Initialize episodic memory.
        
        Args:
            brain_ptr: Brain pointer for neural integration.
            
        Returns:
            bool: True if initialization successful.
        """
        if self._episodic_memory:
            return self._episodic_memory.initialize(brain_ptr)
        return False
    
    def setMaxEpisodes(self, max_episodes: int) -> bool:
        """Set maximum number of episodes.
        
        Args:
            max_episodes: Maximum episodes to store.
            
        Returns:
            bool: True if max set successfully.
        """
        if self._episodic_memory:
            return self._episodic_memory.setMaxEpisodes(max_episodes)
        return False
    
    def storeEpisode(self, episode: Dict[str, Any]) -> bool:
        """Store an episode in episodic memory.
        
        Args:
            episode: Episode data with timestamp, reward, neurons, etc.
            
        Returns:
            bool: True if episode stored successfully.
        """
        if self._episodic_memory:
            return self._episodic_memory.storeEpisode(episode)
        return False
    
    def getEpisodeCount(self) -> int:
        """Get number of stored episodes.
        
        Returns:
            int: Number of stored episodes.
        """
        if self._episodic_memory:
            return self._episodic_memory.getEpisodeCount()
        return 0
    
    def getEpisodesForReplay(self, count: int) -> List[Dict[str, Any]]:
        """Get episodes for replay.
        
        Args:
            count: Number of episodes to retrieve.
            
        Returns:
            List[Dict[str, Any]]: List of episodes for replay.
        """
        if self._episodic_memory:
            return self._episodic_memory.getEpisodesForReplay(count)
        return []
    
    def replayEpisode(self, episode: Dict[str, Any]) -> bool:
        """Replay an episode.
        
        Args:
            episode: Episode to replay.
            
        Returns:
            bool: True if replay successful.
        """
        if self._episodic_memory:
            return self._episodic_memory.replayEpisode(episode)
        return False
    
    def consolidate(self, threshold: float) -> bool:
        """Consolidate important memories, remove weak ones.
        
        Args:
            threshold: Consolidation threshold (0.0 to 1.0).
            
        Returns:
            bool: True if consolidation successful.
        """
        if self._episodic_memory:
            return self._episodic_memory.consolidate(threshold)
        return False
    
    def clear(self) -> bool:
        """Clear all episodes.
        
        Returns:
            bool: True if cleared successfully.
        """
        if self._episodic_memory:
            return self._episodic_memory.clear()
        return False
    
    def __repr__(self) -> str:
        if self._episodic_memory:
            return f"<EpisodicMemory: {self.getEpisodeCount()} episodes>"
        return "<EpisodicMemory: uninitialized>"


class AssociativeMemory:
    """Associative memory for pattern associations.
    
    Associative memory forms associations between patterns through
    Hebbian learning and strengthens frequently co-occurring patterns.
    It supports pattern completion and similarity-based retrieval.
    
    Key features:
    - Pattern association formation
    - Similarity-based retrieval
    - Pattern completion
    - Weight-based strength learning
    
    Example:
        >>> from pynlm import createBrain
        >>> brain = createBrain()
        >>> brain.initialize()
        >>> 
        >>> associative_memory = brain.getAssociativeMemory()
        >>> if associative_memory:
        ...     associative_memory.initialize(brain_ptr)
        ...     # Form associations between patterns
        ...     # Retrieve patterns based on similarity
    """
    
    def __init__(self, associative_memory_ptr=None):
        self._associative_memory = associative_memory_ptr
    
    def initialize(self, brain_ptr=None) -> bool:
        """Initialize associative memory.
        
        Args:
            brain_ptr: Brain pointer for neural integration.
            
        Returns:
            bool: True if initialization successful.
        """
        if self._associative_memory:
            return self._associative_memory.initialize(brain_ptr)
        return False
    
    def clear(self) -> bool:
        """Clear all associations.
        
        Returns:
            bool: True if cleared successfully.
        """
        if self._associative_memory:
            return self._associative_memory.clear()
        return False
    
    def __repr__(self) -> str:
        if self._associative_memory:
            return f"<AssociativeMemory: initialized>"
        return "<AssociativeMemory: uninitialized>"
