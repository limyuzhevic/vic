# Memory system wrappers

import time
from typing import Dict, Any, List, Optional, Tuple
from dataclasses import dataclass
from enum import Enum

from .exceptions import NLMErrors

class MemoryType(Enum):
    """Memory type enumeration."""
    WORKING = 0
    EPISODIC = 1
    ASSOCIATIVE = 2
    SEMANTIC = 3
    PROCEDURAL = 4

@dataclass
class MemoryEntry:
    """Base memory entry."""
    id: str
    timestamp: float
    content: Dict[str, Any]
    importance: float = 1.0
    
    def __post_init__(self):
        if self.importance <= 0:
            raise ValueError("Importance must be positive")

@dataclass
class WorkingMemoryEntry(MemoryEntry):
    """Working memory entry."""
    neuron_id: str
    activation: float
    decay_rate: float = 0.01
    
    def __post_init__(self):
        super().__post_init__()
        if self.activation <= 0:
            raise ValueError("Activation must be positive")

@dataclass
class EpisodicMemoryEntry(MemoryEntry):
    """Episodic memory entry."""
    episode_id: int
    reward: float
    action: str
    observations: List[Dict[str, Any]]
    neurons: List[str]
    
    def __post_init__(self):
        super().__post_init__()
        if self.episode_id < 0:
            raise ValueError("Episode ID must be non-negative")
        if not self.observations:
            raise ValueError("Episodic memory entry must have observations")

class WorkingMemory:
    """Working memory system wrapper."""
    
    def __init__(self, capacity: int = 100):
        self.capacity = capacity
        self.entries: List[WorkingMemoryEntry] = []
        self.active_neurons: Dict[str, float] = {}
        
    def store(self, neuron_id: str, activation: float):
        """Store a neuron in working memory."""
        if not isinstance(neuron_id, str) or not neuron_id:
            raise NLMErrors("Invalid neuron ID")
            
        if activation <= 0:
            raise NLMErrors("Activation must be positive")
            
        # Create entry
        entry = WorkingMemoryEntry(
            id=f"wm_{neuron_id}_{time.time()}",
            timestamp=time.time(),
            content={'neuron_id': neuron_id, 'activation': activation},
            importance=activation,
            neuron_id=neuron_id,
            activation=activation
        )
        
        # Add to entries
        self.entries.append(entry)
        self.active_neurons[neuron_id] = activation
        
        # Remove oldest entries if capacity exceeded
        if len(self.entries) > self.capacity:
            oldest_entry = self.entries.pop(0)
            if oldest_entry.neuron_id in self.active_neurons:
                del self.active_neurons[oldest_entry.neuron_id]
                
    def get_active_traces(self) -> int:
        """Get number of active memory traces."""
        return len(self.entries)
        
    def get_neuron_activations(self) -> Dict[str, float]:
        """Get current neuron activations."""
        return self.active_neurons.copy()
        
    def get_memory_neurons(self) -> List[str]:
        """Get list of neurons currently in working memory."""
        return list(self.active_neurons.keys())
        
    def update(self, timestep: float):
        """Update working memory (decay activations)."""
        # Decay activations
        for neuron_id in self.active_neurons:
            self.active_neurons[neuron_id] *= (1.0 - 0.01 * timestep)
            
        # Remove zero activations
        zero_neurons = [nid for nid, act in self.active_neurons.items() if act <= 0.001]
        for neuron_id in zero_neurons:
            del self.active_neurons[neuron_id]
            
        # Update entries
        self.entries = [entry for entry in self.entries if entry.neuron_id in self.active_neurons]
        
    def clear(self):
        """Clear working memory."""
        self.entries.clear()
        self.active_neurons.clear()
        
    def get_stats(self) -> Dict[str, Any]:
        """Get working memory statistics."""
        return {
            'capacity': self.capacity,
            'current_size': len(self.entries),
            'active_neurons': len(self.active_neurons),
            'max_activation': max(self.active_neurons.values()) if self.active_neurons else 0,
            'avg_activation': sum(self.active_neurons.values()) / len(self.active_neurons) if self.active_neurons else 0,
        }

class EpisodicMemory:
    """Episodic memory system wrapper."""
    
    def __init__(self, max_episodes: int = 1000):
        self.max_episodes = max_episodes
        self.episodes: List[EpisodicMemoryEntry] = []
        self.next_episode_id = 0
        
    def store_episode(self, reward: float, action: str, observations: List[Dict[str, Any]],
                     neurons: List[str]):
        """Store an episode in episodic memory."""
        if not isinstance(reward, (int, float)):
            raise NLMErrors("Reward must be numeric")
            
        if not isinstance(action, str):
            raise NLMErrors("Action must be a string")
            
        if not observations:
            raise NLMErrors("Observations cannot be empty")
            
        # Create episode entry
        entry = EpisodicMemoryEntry(
            id=f"ep_{self.next_episode_id}_{time.time()}",
            timestamp=time.time(),
            content={
                'reward': reward,
                'action': action,
                'observations': observations,
                'neurons': neurons,
            },
            importance=abs(reward) * 1.0,  # Importance based on reward magnitude
            episode_id=self.next_episode_id,
            reward=reward,
            action=action,
            observations=observations,
            neurons=neurons,
        )
        
        self.episodes.append(entry)
        self.next_episode_id += 1
        
        # Remove oldest episodes if capacity exceeded
        if len(self.episodes) > self.max_episodes:
            self.episodes.pop(0)
            
    def get_episode_count(self) -> int:
        """Get number of stored episodes."""
        return len(self.episodes)
        
    def get_episodes(self) -> List[EpisodicMemoryEntry]:
        """Get all stored episodes."""
        return self.episodes.copy()
        
    def get_episodes_for_replay(self, count: int = 10) -> List[EpisodicMemoryEntry]:
        """Get episodes for replay (most recent)."""
        return self.episodes[-count:] if count <= len(self.episodes) else self.episodes.copy()
        
    def replay_episode(self, episode: EpisodicMemoryEntry):
        """Replay a specific episode."""
        # This would call the C++ implementation to replay the episode
        pass
        
    def consolidate(self, threshold: float = 0.5):
        """Consolidate and prune episodic memory."""
        # Remove low-importance episodes
        self.episodes = [ep for ep in self.episodes if ep.importance >= threshold]
        
    def clear(self):
        """Clear episodic memory."""
        self.episodes.clear()
        self.next_episode_id = 0
        
    def get_stats(self) -> Dict[str, Any]:
        """Get episodic memory statistics."""
        if not self.episodes:
            return {
                'capacity': self.max_episodes,
                'current_size': 0,
                'next_episode_id': self.next_episode_id,
            }
            
        rewards = [ep.reward for ep in self.episodes]
        
        return {
            'capacity': self.max_episodes,
            'current_size': len(self.episodes),
            'next_episode_id': self.next_episode_id,
            'total_reward': sum(rewards),
            'avg_reward': sum(rewards) / len(rewards),
            'max_reward': max(rewards),
            'min_reward': min(rewards),
            'episodes_per_day': len(self.episodes) / (self.episodes[-1].timestamp - self.episodes[0].timestamp) if len(self.episodes) > 1 else 0,
        }

class MemorySystem:
    """Combined memory system wrapper."""
    
    def __init__(self):
        self.working_memory = WorkingMemory()
        self.episodic_memory = EpisodicMemory()
        
    def store_to_neuron(self, neuron_id: str, activation: float):
        """Store neuron activation to working memory."""
        self.working_memory.store(neuron_id, activation)
        
    def store_episode(self, reward: float, action: str, observations: List[Dict[str, Any]],
                     neurons: List[str]):
        """Store episode to episodic memory."""
        self.episodic_memory.store_episode(reward, action, observations, neurons)
        
    def get_working_memory_stats(self) -> Dict[str, Any]:
        """Get working memory statistics."""
        return self.working_memory.get_stats()
        
    def get_episodic_memory_stats(self) -> Dict[str, Any]:
        """Get episodic memory statistics."""
        return self.episodic_memory.get_stats()
        
    def update(self, timestep: float):
        """Update all memory systems."""
        self.working_memory.update(timestep)
        
    def clear(self):
        """Clear all memory."""
        self.working_memory.clear()
        self.episodic_memory.clear()
        
    def get_total_stats(self) -> Dict[str, Any]:
        """Get total memory system statistics."""
        return {
            'working_memory': self.get_working_memory_stats(),
            'episodic_memory': self.get_episodic_memory_stats(),
        }