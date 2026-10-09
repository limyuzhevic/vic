"""
Memory Module

This module provides memory system interfaces for the NLM Python bindings.
It includes WorkingMemory, EpisodicMemory, and AssociativeMemory classes that
wrap the underlying C++ memory systems.
"""

from typing import List, Optional, Dict, Any, Union, Sequence, Tuple
import time

from .types import NeuronId, Timestamp, SimulationStep
from .brain import Brain

class WorkingMemory:
    """
    Working memory system for active information storage and manipulation.
    
    Working memory is a short-term memory system that temporarily holds and
    manipulates information for cognitive tasks. It is used to store active
    patterns, maintain activation states, and support immediate processing.
    
    Features:
    - Pattern storage and retrieval
    - Activation maintenance
    - Competitive dynamics
    - Capacity management
    - Real-time updates
    
    Examples:
        >>> from pynlm import WorkingMemory, create_brain
        >>> 
        >>> brain = create_brain({"neuron_count": 1000})
        >>> working_mem = WorkingMemory(brain)
        >>> 
        >>> # Store a pattern
        >>> working_mem.store([0.1, 0.5, 0.9], strength=0.8)
        >>> 
        >>> # Retrieve stored patterns
        >>> retrieved = working_mem.retrieve()
        >>> 
        >>> # Check if pattern is active
        >>> is_active = working_mem.contains(neuron_id)
    """
    
    def __init__(self, brain: Brain):
        """
        Initialize working memory system.
        
        Args:
            brain: Brain instance to connect to.
        """
        self._brain = brain
        self._initialized = False
        self._patterns = []
        self._activations = []
        self._timestamps = []
        self._neurons = []
        self._capacity = 100
        self._decay_rate = 0.01
        
        if self._brain:
            self._initialize()
    
    def _initialize(self):
        """Initialize working memory system."""
        self._initialized = True
    
    @property
    def capacity(self) -> int:
        """Get working memory capacity."""
        return self._capacity
    
    @capacity.setter
    def capacity(self, value: int):
        """Set working memory capacity."""
        if value < 0:
            raise ValueError("Capacity must be non-negative")
        self._capacity = value
    
    @property
    def decay_rate(self) -> float:
        """Get decay rate."""
        return self._decay_rate
    
    @decay_rate.setter
    def decay_rate(self, value: float):
        """Set decay rate."""
        if not 0.0 <= value <= 1.0:
            raise ValueError("Decay rate must be between 0.0 and 1.0")
        self._decay_rate = value
    
    def store(self, pattern: Union[List[float], Sequence[float]], 
              strength: float = 1.0) -> None:
        """
        Store a pattern in working memory.
        
        Args:
            pattern: Pattern to store (list or sequence of floats).
            strength: Pattern strength (0.0 to 1.0).
        
        Raises:
            ValueError: If pattern is invalid or strength is out of range.
        """
        if not isinstance(pattern, (list, tuple)):
            raise ValueError("Pattern must be a list or tuple")
        
        if not pattern:
            return
        
        if not 0.0 <= strength <= 1.0:
            raise ValueError("Strength must be between 0.0 and 1.0")
        
        # Convert to list and ensure proper length
        pattern_list = list(pattern)
        
        if len(pattern_list) > self._capacity:
            pattern_list = pattern_list[:self._capacity]
        
        # Store pattern
        self._patterns.append(pattern_list)
        self._activations.append(strength)
        self._timestamps.append(time.time())
        
        # Inject current into neurons (if brain is available)
        if self._brain and self._neurons:
            self._inject_pattern_to_neurons(pattern_list, strength)
    
    def _inject_pattern_to_neurons(self, pattern: List[float], strength: float):
        """Inject pattern into neurons."""
        # This will be implemented based on the C++ binding
        pass
    
    def retrieve(self) -> List[float]:
        """
        Retrieve stored patterns from working memory.
        
        Returns:
            List of retrieved patterns (most recent first).
        """
        # Sort by timestamp (most recent first)
        combined = list(zip(self._patterns, self._timestamps))
        combined.sort(key=lambda x: x[1], reverse=True)
        
        return [pattern for pattern, _ in combined]
    
    def clear(self) -> None:
        """Clear all stored patterns."""
        self._patterns.clear()
        self._activations.clear()
        self._timestamps.clear()
    
    def contains(self, neuron: Union[NeuronId, int]) -> bool:
        """
        Check if working memory contains a specific neuron.
        
        Args:
            neuron: Neuron identifier.
        
        Returns:
            True if neuron is in working memory, False otherwise.
        """
        # This will be implemented based on the C++ binding
        return False
    
    def get_neuron_activation(self, neuron: Union[NeuronId, int]) -> float:
        """
        Get activation level for a specific neuron.
        
        Args:
            neuron: Neuron identifier.
        
        Returns:
            Neuron activation level (0.0 to 1.0).
        """
        # This will be implemented based on the C++ binding
        return 0.0
    
    def update(self, dt: float) -> None:
        """
        Update working memory (decay and maintenance).
        
        Args:
            dt: Time step for update.
        """
        # Decay activations
        for i in range(len(self._activations)):
            self._activations[i] *= (1.0 - self._decay_rate * dt)
        
        # Remove weak patterns
        i = 0
        while i < len(self._patterns):
            if self._activations[i] < 0.01:  # Threshold for pattern removal
                self._patterns.pop(i)
                self._activations.pop(i)
                self._timestamps.pop(i)
            else:
                i += 1
        
        # Update timestamps
        current_time = time.time()
        for i in range(len(self._timestamps)):
            self._timestamps[i] = current_time - (self._timestamps[i] - self._timestamps[0])
    
    def get_memory_activity(self) -> float:
        """
        Get overall memory activity level.
        
        Returns:
            Average activation across all patterns.
        """
        if not self._activations:
            return 0.0
        
        return sum(self._activations) / len(self._activations)
    
    def get_active_patterns(self) -> List[List[float]]:
        """
        Get all active patterns.
        
        Returns:
            List of active patterns (above threshold).
        """
        threshold = 0.1
        active_patterns = []
        
        for pattern, activation in zip(self._patterns, self._activations):
            if activation >= threshold:
                active_patterns.append(pattern)
        
        return active_patterns
    
    def export_to_dataframe(self) -> 'pd.DataFrame':
        """
        Export working memory data to Pandas DataFrame.
        
        Returns:
            Pandas DataFrame with working memory data.
        
        Raises:
            ImportError: If pandas is not available.
        """
        try:
            import pandas as pd
        except ImportError:
            raise ImportError("Pandas is required for DataFrame export")
        
        if not self._patterns:
            return pd.DataFrame()
        
        # Create DataFrame
        data = {
            'pattern_index': list(range(len(self._patterns))),
            'activation': self._activations,
            'timestamp': self._timestamps,
        }
        
        # Add pattern columns
        for i, pattern in enumerate(self._patterns):
            for j, value in enumerate(pattern):
                data[f'pattern_{i}_neuron_{j}'] = value
        
        return pd.DataFrame(data)

class EpisodicMemory:
    """
    Episodic memory system for recording experiences and events.
    
    Episodic memory stores specific events, experiences, and episodes with
    timestamps and contextual information. It enables temporal recall and
    sequence learning.
    
    Features:
    - Event recording with timestamps
    - Temporal indexing
    - Context association
    - Experience playback
    - Pattern completion
    
    Examples:
        >>> from pynlm import EpisodicMemory
        >>> 
        >>> episodic_mem = EpisodicMemory()
        >>> 
        >>> # Record an episode
        >>> episodic_mem.record(episode_data, timestamp=time.time())
        >>> 
        >>> # Search for episodes
        >>> episodes = episodic_mem.search(keywords=["exploration", "reward"])
        >>> 
        >>> # Playback experiences
        >>> replay = episodic_mem.playback(timestamp_range=(start_time, end_time))
    """
    
    def __init__(self):
        """Initialize episodic memory system."""
        self._episodes = []
        self._timestamps = []
        self._contexts = []
        self._episode_index = {}
    
    def record(self, episode: Any, timestamp: float, 
               context: Optional[Dict[str, Any]] = None) -> None:
        """
        Record an episode in episodic memory.
        
        Args:
            episode: Episode data (any type).
            timestamp: Episode timestamp.
            context: Optional contextual information.
        """
        if not isinstance(timestamp, (int, float)):
            raise ValueError("Timestamp must be a number")
        
        self._episodes.append(episode)
        self._timestamps.append(timestamp)
        self._contexts.append(context or {})
        
        # Update index
        if episode not in self._episode_index:
            self._episode_index[episode] = []
        self._episode_index[episode].append(len(self._episodes) - 1)
    
    def search(self, keywords: Optional[List[str]] = None,
               timestamp_range: Optional[Tuple[float, float]] = None,
               context_filter: Optional[Dict[str, Any]] = None) -> List[Any]:
        """
        Search for episodes based on criteria.
        
        Args:
            keywords: Keywords to search for in episode data.
            timestamp_range: (start_time, end_time) range.
            context_filter: Context filter dictionary.
        
        Returns:
            List of matching episodes.
        """
        results = []
        
        for i, (episode, timestamp, context) in enumerate(
            zip(self._episodes, self._timestamps, self._contexts)
        ):
            # Check timestamp range
            if timestamp_range:
                start, end = timestamp_range
                if not (start <= timestamp <= end):
                    continue
            
            # Check context filter
            if context_filter:
                for key, value in context_filter.items():
                    if key not in context or context[key] != value:
                        break
                else:
                    continue
            
            # Check keywords (if provided and episode supports string operations)
            if keywords:
                try:
                    episode_str = str(episode)
                    if not any(keyword.lower() in episode_str.lower() 
                              for keyword in keywords):
                        continue
                except (ValueError, TypeError):
                    pass  # Skip keyword search if conversion fails
            
            results.append(episode)
        
        return results
    
    def playback(self, start_index: int = 0, 
                  end_index: Optional[int] = None) -> List[Any]:
        """
        Playback a sequence of episodes.
        
        Args:
            start_index: Starting episode index.
            end_index: Ending episode index (None for all remaining).
        
        Returns:
            List of episodes in playback order.
        """
        if start_index < 0 or start_index >= len(self._episodes):
            raise ValueError("Invalid start index")
        
        if end_index is None:
            end_index = len(self._episodes)
        elif end_index < start_index or end_index > len(self._episodes):
            raise ValueError("Invalid end index")
        
        return self._episodes[start_index:end_index]
    
    def get_timestamps(self) -> List[float]:
        """Get all episode timestamps."""
        return self._timestamps.copy()
    
    def get_context(self, episode_index: int) -> Dict[str, Any]:
        """
        Get context for a specific episode.
        
        Args:
            episode_index: Episode index.
        
        Returns:
            Context dictionary.
        """
        if episode_index < 0 or episode_index >= len(self._episodes):
            raise ValueError("Invalid episode index")
        
        return self._contexts[episode_index].copy()
    
    def add_context(self, episode_index: int, 
                    context: Dict[str, Any]) -> None:
        """
        Add or update context for an episode.
        
        Args:
            episode_index: Episode index.
            context: Context dictionary.
        """
        if episode_index < 0 or episode_index >= len(self._episodes):
            raise ValueError("Invalid episode index")
        
        self._contexts[episode_index] = context.copy()
    
    def clear(self) -> None:
        """Clear all episodes."""
        self._episodes.clear()
        self._timestamps.clear()
        self._contexts.clear()
        self._episode_index.clear()
    
    def get_episode_count(self) -> int:
        """Get number of recorded episodes."""
        return len(self._episodes)
    
    def export_to_dataframe(self) -> 'pd.DataFrame':
        """
        Export episodic memory data to Pandas DataFrame.
        
        Returns:
            Pandas DataFrame with episodic memory data.
        
        Raises:
            ImportError: If pandas is not available.
        """
        try:
            import pandas as pd
        except ImportError:
            raise ImportError("Pandas is required for DataFrame export")
        
        if not self._episodes:
            return pd.DataFrame()
        
        # Create DataFrame
        data = {
            'episode_index': list(range(len(self._episodes))),
            'timestamp': self._timestamps,
            'context': self._contexts,
            'episode_data': self._episodes,
        }
        
        return pd.DataFrame(data)

class AssociativeMemory:
    """
    Associative memory system for linking concepts and associations.
    
    Associative memory stores associations between different pieces of
    information, enabling pattern completion and meaningful recall based
    on partial cues.
    
    Features:
    - Bidirectional associations
    - Pattern completion
    - Strength-based activation
    - Contextual associations
    - Retrieval optimization
    
    Examples:
        >>> from pynlm import AssociativeMemory
        >>> 
        >>> assoc_mem = AssociativeMemory()
        >>> 
        >>> # Create associations
        >>> assoc_mem.associate("concept1", "concept2", strength=0.8)
        >>> assoc_mem.associate("concept2", "concept3", strength=0.6)
        >>> 
        >>> # Retrieve associations
        >>> related = assoc_mem.get_associations("concept2")
        >>> 
        >>> # Pattern completion
        >>> completed = assoc_mem.complete_pattern("concept1")
    """
    
    def __init__(self):
        """Initialize associative memory system."""
        self._associations = {}  # bidirectional mapping
        self._strengths = {}     # association strengths
        self._contexts = {}      # association contexts
    
    def associate(self, concept_a: Any, concept_b: Any, 
                  strength: float = 0.5,
                  context: Optional[Dict[str, Any]] = None) -> None:
        """
        Create or strengthen an association between two concepts.
        
        Args:
            concept_a: First concept.
            concept_b: Second concept.
            strength: Association strength (0.0 to 1.0).
            context: Optional contextual information.
        """
        if not 0.0 <= strength <= 1.0:
            raise ValueError("Strength must be between 0.0 and 1.0")
        
        # Create bidirectional associations
        key_ab = (concept_a, concept_b)
        key_ba = (concept_b, concept_a)
        
        self._associations.setdefault(key_ab, set()).add(concept_b)
        self._associations.setdefault(key_ba, set()).add(concept_a)
        
        self._strengths[key_ab] = strength
        self._strengths[key_ba] = strength
        
        # Store context
        if context:
            self._contexts[key_ab] = context.copy()
            self._contexts[key_ba] = context.copy()
    
    def get_associations(self, concept: Any) -> Dict[str, float]:
        """
        Get all associations for a concept.
        
        Args:
            concept: Concept to query.
        
        Returns:
            Dictionary of associated concepts and strengths.
        """
        result = {}
        
        # Find all keys that include this concept
        for (a, b), associations in self._associations.items():
            if concept == a or concept == b:
                # Get the other concept and strength
                if concept == a:
                    other = b
                    strength_key = (a, b)
                else:
                    other = a
                    strength_key = (b, a)
                
                result[str(other)] = self._strengths.get(strength_key, 0.0)
        
        return result
    
    def complete_pattern(self, cue: Any, 
                         max_depth: int = 3) -> List[Any]:
        """
        Perform pattern completion based on a cue.
        
        Args:
            cue: Initial cue concept.
            max_depth: Maximum depth of association chain.
        
        Returns:
            List of completed concepts in order of relevance.
        """
        if cue not in [a for (a, _) in self._associations.keys()]:
            return []  # Cue not found
        
        visited = set()
        result = []
        
        def dfs(current: Any, depth: int):
            if depth > max_depth:
                return
            
            visited.add(current)
            
            # Get associated concepts
            associations = self.get_associations(current)
            for related in associations.keys():
                if related not in visited:
                    result.append(related)
                    dfs(related, depth + 1)
        
        dfs(cue, 0)
        return result
    
    def get_strength(self, concept_a: Any, concept_b: Any) -> float:
        """
        Get strength of association between two concepts.
        
        Args:
            concept_a: First concept.
            concept_b: Second concept.
        
        Returns:
            Association strength (0.0 to 1.0), or 0.0 if no association.
        """
        key_ab = (concept_a, concept_b)
        key_ba = (concept_b, concept_a)
        
        return self._strengths.get(key_ab, self._strengths.get(key_ba, 0.0))
    
    def update_strength(self, concept_a: Any, concept_b: Any,
                       new_strength: float) -> None:
        """
        Update association strength.
        
        Args:
            concept_a: First concept.
            concept_b: Second concept.
            new_strength: New strength value (0.0 to 1.0).
        """
        if not 0.0 <= new_strength <= 1.0:
            raise ValueError("Strength must be between 0.0 and 1.0")
        
        key_ab = (concept_a, concept_b)
        key_ba = (concept_b, concept_a)
        
        if key_ab in self._strengths:
            self._strengths[key_ab] = new_strength
        if key_ba in self._strengths:
            self._strengths[key_ba] = new_strength
    
    def get_context(self, concept_a: Any, concept_b: Any) -> Dict[str, Any]:
        """
        Get context for an association.
        
        Args:
            concept_a: First concept.
            concept_b: Second concept.
        
        Returns:
            Context dictionary, or empty dict if no context.
        """
        key_ab = (concept_a, concept_b)
        return self._contexts.get(key_ab, {})
    
    def clear(self) -> None:
        """Clear all associations."""
        self._associations.clear()
        self._strengths.clear()
        self._contexts.clear()
    
    def export_to_dataframe(self) -> 'pd.DataFrame':
        """
        Export associative memory data to Pandas DataFrame.
        
        Returns:
            Pandas DataFrame with associative memory data.
        
        Raises:
            ImportError: If pandas is not available.
        """
        try:
            import pandas as pd
        except ImportError:
            raise ImportError("Pandas is required for DataFrame export")
        
        if not self._associations:
            return pd.DataFrame()
        
        # Create DataFrame
        data = {
            'concept_a': [],
            'concept_b': [],
            'strength': [],
        }
        
        for (a, b), strength in self._strengths.items():
            if a <= b:  # Avoid duplicates
                data['concept_a'].append(a)
                data['concept_b'].append(b)
                data['strength'].append(strength)
        
        return pd.DataFrame(data)

# Export commonly used symbols

__all__ = ['WorkingMemory', 'EpisodicMemory', 'AssociativeMemory']
