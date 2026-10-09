"""
Enhanced Python interface for NLM memory systems
"""

from typing import List, Dict, Any, Optional, Tuple
from .exceptions import NLMError, NLMMemoryError

class WorkingMemory:
    """Working memory system"""
    
    def __init__(self, capacity: int = 100):
        self.capacity = capacity
        self.traces = {}  # NeuronId -> activation value
        self.time_decay = 0.95
        self.max_age = 1000
        
    def store(self, neuron_id: int, activation: float) -> None:
        """Store a neural trace in working memory"""
        self.traces[neuron_id] = activation
        
    def get_trace(self, neuron_id: int) -> Optional[float]:
        """Get trace for specific neuron"""
        return self.traces.get(neuron_id)
        
    def get_active_traces(self) -> Dict[int, float]:
        """Get all active traces"""
        return self.traces.copy()
        
    def get_neuron_traces(self) -> List[int]:
        """Get all neuron IDs in memory"""
        return list(self.traces.keys())
        
    def clear(self) -> None:
        """Clear all traces"""
        self.traces.clear()
        
    def update(self, timestep: float) -> None:
        """Update working memory with time decay"""
        # Apply time decay to all traces
        for neuron_id in self.traces:
            self.traces[neuron_id] *= self.time_decay
        
    def get_total_active_traces(self) -> int:
        """Get total number of active traces"""
        return len(self.traces)
        
    def get_mean_activation(self) -> float:
        """Get mean activation across all traces"""
        if not self.traces:
            return 0.0
        return sum(self.traces.values()) / len(self.traces)
        
    def get_max_activation(self) -> float:
        """Get maximum activation across all traces"""
        if not self.traces:
            return 0.0
        return max(self.traces.values())
        
    def __repr__(self):
        return f"<WorkingMemory: {len(self.traces)}/{self.capacity} traces, mean_act={self.get_mean_activation():.3f}>"

class EpisodicMemory:
    """Episodic memory system"""
    
    def __init__(self, max_episodes: int = 1000):
        self.max_episodes = max_episodes
        self.episodes = []
        self.consolidation_threshold = 0.5
        
    def store_episode(self, episode_data: Dict[str, Any]) -> None:
        """Store an episode in episodic memory"""
        episode_data['timestamp'] = len(self.episodes)
        self.episodes.append(episode_data)
        
        # Maintain size limit
        if len(self.episodes) > self.max_episodes:
            self.episodes.pop(0)
            
    def get_episodes(self, limit: Optional[int] = None) -> List[Dict[str, Any]]:
        """Get episodes (most recent first)"""
        if limit is None:
            return self.episodes.copy()
        return self.episodes[-limit:]
        
    def get_episode_count(self) -> int:
        """Get total episode count"""
        return len(self.episodes)
        
    def get_episodes_for_replay(self, limit: int) -> List[Dict[str, Any]]:
        """Get episodes for replay (highest priority first)"""
        # Simple priority: most recent episodes
        return self.get_episodes(limit)
        
    def get_recent_episodes(self, count: int) -> List[Dict[str, Any]]:
        """Get most recent episodes"""
        return self.get_episodes(count)
        
    def clear(self) -> None:
        """Clear all episodes"""
        self.episodes.clear()
        
    def consolidate(self, threshold: float) -> int:
        """Consolidate important memories, remove weak ones"""
        # Simple consolidation: remove episodes with low priority
        original_count = len(self.episodes)
        self.episodes = [ep for ep in self.episodes if ep.get('priority', 1.0) >= threshold]
        return original_count - len(self.episodes)
        
    def get_memory_stats(self) -> Dict[str, Any]:
        """Get memory statistics"""
        if not self.episodes:
            return {
                'episode_count': 0,
                'avg_episode_size': 0,
                'total_size': 0,
                'consolidation_rate': 0.0
            }
            
        total_size = sum(len(str(ep)) for ep in self.episodes)
        avg_size = total_size / len(self.episodes)
        
        return {
            'episode_count': len(self.episodes),
            'avg_episode_size': avg_size,
            'total_size': total_size,
            'consolidation_rate': 1.0 - (len(self.episodes) / self.max_episodes)
        }
        
    def __repr__(self):
        stats = self.get_memory_stats()
        return f"<EpisodicMemory: {stats['episode_count']}/{self.max_episodes} episodes, size={stats['total_size']} bytes>"

class AssociativeMemory:
    """Associative memory system for pattern associations"""
    
    def __init__(self, learning_rate: float = 0.01, decay_rate: float = 0.99):
        self.associations = {}  # Pattern -> Value
        self.learning_rate = learning_rate
        self.decay_rate = decay_rate
        
    def learn(self, pattern: List[float], value: float) -> None:
        """Learn pattern association"""
        # Simple pattern storage
        pattern_key = tuple(round(x, 6) for x in pattern)
        self.associations[pattern_key] = value
        
    def recall(self, pattern: List[float]) -> Optional[float]:
        """Recall value for pattern"""
        pattern_key = tuple(round(x, 6) for x in pattern)
        return self.associations.get(pattern_key)
        
    def update(self, pattern: List[float], value: float) -> None:
        """Update pattern association"""
        # Incremental learning
        pattern_key = tuple(round(x, 6) for x in pattern)
        if pattern_key in self.associations:
            current = self.associations[pattern_key]
            self.associations[pattern_key] = current + self.learning_rate * (value - current)
        else:
            self.associations[pattern_key] = value
            
    def decay(self) -> None:
        """Apply decay to all associations"""
        for pattern in self.associations:
            self.associations[pattern] *= self.decay_rate
            
    def get_association_count(self) -> int:
        """Get number of learned associations"""
        return len(self.associations)
        
    def clear(self) -> None:
        """Clear all associations"""
        self.associations.clear()
        
    def get_statistics(self) -> Dict[str, Any]:
        """Get memory statistics"""
        if not self.associations:
            return {
                'association_count': 0,
                'mean_value': 0.0,
                'max_value': 0.0,
                'min_value': 0.0
            }
            
        values = list(self.associations.values())
        return {
            'association_count': len(self.associations),
            'mean_value': sum(values) / len(values),
            'max_value': max(values),
            'min_value': min(values)
        }
        
    def __repr__(self):
        stats = self.get_statistics()
        return f"<AssociativeMemory: {stats['association_count']} associations, mean={stats['mean_value']:.3f}>"
