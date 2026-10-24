"""
Memory module - Memory functionality for NLM
"""

from typing import List, Dict, Any, Optional, Tuple
import time
import math

from .pynlm import (
    WorkingMemory as WorkingMemoryCore, EpisodicMemory as EpisodicMemoryCore, 
    SemanticMemory as SemanticMemoryCore, ProceduralMemory as ProceduralMemoryCore,
    AssociativeMemory as AssociativeMemoryCore, MemorySystem as MemorySystemCore,
    NeuronId, SynapseId, Timestamp, TimestepDuration, SimulationStep, PlasticityFlags,
    MembranePotential, SynapticWeight, FiringRate, FiringState, NeuronType,
    RegionId, PopulationId, ActionType, ObservationData
)

__all__ = [
    'WorkingMemory', 'EpisodicMemory', 'SemanticMemory', 'ProceduralMemory', 'AssociativeMemory',
    'MemorySystem',
    'NeuronId', 'SynapseId', 'Timestamp', 'TimestepDuration', 'SimulationStep',
    'MembranePotential', 'SynapticWeight', 'FiringRate', 'FiringState',
    'NeuronType', 'RegionId', 'PopulationId', 'ActionType', 'ObservationData',
]

class WorkingMemory:
    """Enhanced working memory with efficient storage and retrieval"""
    
    def __init__(self, capacity: int = 100, decay_rate: float = 0.01):
        """Initialize working memory
        
        Args:
            capacity: Maximum number of items
            decay_rate: Decay rate per timestep
        """
        self.capacity = capacity
        self.decay_rate = decay_rate
        self.items = {}  # neuron_id -> value
        self.timestamps = {}  # neuron_id -> timestamp
        self.timestamp = time.time()
    
    def store(self, neuron_id: NeuronId, value: float):
        """Store item in working memory
        
        Args:
            neuron_id: Neuron identifier
            value: Value to store
        """
        self.items[neuron_id] = value
        self.timestamps[neuron_id] = time.time()
        
        # Enforce capacity
        if len(self.items) > self.capacity:
            self._evict_oldest()
    
    def retrieve(self, neuron_id: NeuronId) -> float:
        """Retrieve item from working memory
        
        Args:
            neuron_id: Neuron identifier
            
        Returns:
            Retrieved value or 0.0 if not found
        """
        if neuron_id in self.items:
            # Apply decay based on age
            age = time.time() - self.timestamps[neuron_id]
            decay_factor = math.exp(-self.decay_rate * age)
            return self.items[neuron_id] * decay_factor
        return 0.0
    
    def contains(self, neuron_id: NeuronId) -> bool:
        """Check if working memory contains item
        
        Args:
            neuron_id: Neuron identifier
            
        Returns:
            True if item exists
        """
        return neuron_id in self.items
    
    def clear(self):
        """Clear all items from working memory"""
        self.items.clear()
        self.timestamps.clear()
    
    def get_capacity(self) -> int:
        """Get memory capacity
        
        Returns:
            Memory capacity
        """
        return self.capacity
    
    def get_current_size(self) -> int:
        """Get current number of items
        
        Returns:
            Current number of items
        """
        return len(self.items)
    
    def decay_all(self):
        """Apply decay to all items"""
        current_time = time.time()
        for neuron_id in self.items:
            age = current_time - self.timestamps[neuron_id]
            decay_factor = math.exp(-self.decay_rate * age)
            self.items[neuron_id] *= decay_factor
            self.timestamps[neuron_id] = current_time
    
    def get_all_items(self) -> Dict[NeuronId, float]:
        """Get all items
        
        Returns:
            Dictionary of all items
        """
        return self.items.copy()
    
    def get_items_above_threshold(self, threshold: float) -> Dict[NeuronId, float]:
        """Get items above threshold
        
        Args:
            threshold: Threshold value
            
        Returns:
            Dictionary of items above threshold
        """
        return {neuron_id: value for neuron_id, value in self.items.items() if abs(value) > threshold}
    
    def _evict_oldest(self):
        """Evict oldest item when at capacity"""
        if not self.timestamps:
            return
        
        oldest_neuron = min(self.timestamps.keys(), key=lambda k: self.timestamps[k])
        del self.items[oldest_neuron]
        del self.timestamps[oldest_neuron]

class EpisodicMemory:
    """Enhanced episodic memory for storing events and experiences"""
    
    def __init__(self, max_episodes: int = 1000, consolidation_threshold: float = 0.5):
        """Initialize episodic memory
        
        Args:
            max_episodes: Maximum number of episodes
            consolidation_threshold: Threshold for consolidation
        """
        self.max_episodes = max_episodes
        self.consolidation_threshold = consolidation_threshold
        self.episodes = []  # List of episode dictionaries
        self.timestamp = time.time()
    
    def store_episode(self, episode_data: Dict[str, Any]):
        """Store episode
        
        Args:
            episode_data: Episode data
        """
        # Add timestamp
        episode_data['timestamp'] = time.time()
        episode_data['id'] = len(self.episodes)
        
        # Apply consolidation if needed
        if episode_data.get('importance', 0.0) > self.consolidation_threshold:
            episode_data['consolidated'] = True
        
        # Add episode
        self.episodes.append(episode_data)
        
        # Enforce capacity
        if len(self.episodes) > self.max_episodes:
            self._evict_oldest()
    
    def retrieve_episode(self, episode_id: int) -> Optional[Dict[str, Any]]:
        """Retrieve episode
        
        Args:
            episode_id: Episode identifier
            
        Returns:
            Episode data or None if not found
        """
        if 0 <= episode_id < len(self.episodes):
            return self.episodes[episode_id].copy()
        return None
    
    def get_recent_episodes(self, count: int = 10) -> List[Dict[str, Any]]:
        """Get recent episodes
        
        Args:
            count: Number of recent episodes to retrieve
            
        Returns:
            List of recent episodes
        """
        count = min(count, len(self.episodes))
        return [episode.copy() for episode in self.episodes[-count:]]
    
    def get_episode_count(self) -> int:
        """Get episode count
        
        Returns:
            Episode count
        """
        return len(self.episodes)
    
    def clear(self):
        """Clear all episodes"""
        self.episodes.clear()
    
    def consolidate(self, relevance_threshold: float = 0.5):
        """Consolidate episodes
        
        Args:
            relevance_threshold: Threshold for relevance
        """
        # Move relevant episodes to long-term storage
        relevant_episodes = []
        irrelevant_episodes = []
        
        for episode in self.episodes:
            importance = episode.get('importance', 0.0)
            relevance = episode.get('relevance', 0.0)
            
            if importance > relevance_threshold or relevance > relevance_threshold:
                episode['consolidated'] = True
                irrelevant_episodes.append(episode)
            else:
                episode['consolidated'] = False
                irrelevant_episodes.append(episode)
        
        self.episodes = irrelevant_episodes
    
    def get_consolidated_episodes(self) -> List[Dict[str, Any]]:
        """Get consolidated episodes
        
        Returns:
            List of consolidated episodes
        """
        return [episode.copy() for episode in self.episodes if episode.get('consolidated', False)]
    
    def get_non_consolidated_episodes(self) -> List[Dict[str, Any]]:
        """Get non-consolidated episodes
        
        Returns:
            List of non-consolidated episodes
        """
        return [episode.copy() for episode in self.episodes if not episode.get('consolidated', False)]
    
    def get_highly_important_episodes(self, threshold: float = 0.8) -> List[Dict[str, Any]]:
        """Get highly important episodes
        
        Args:
            threshold: Importance threshold
            
        Returns:
            List of highly important episodes
        """
        return [episode.copy() for episode in self.episodes 
                if episode.get('importance', 0.0) > threshold]
    
    def get_episodes_in_time_range(self, start_time: float, end_time: float) -> List[Dict[str, Any]]:
        """Get episodes in time range
        
        Args:
            start_time: Start time
            end_time: End time
            
        Returns:
            List of episodes in time range
        """
        return [episode.copy() for episode in self.episodes 
                if start_time <= episode.get('timestamp', 0) <= end_time]
    
    def _evict_oldest(self):
        """Evict oldest episode when at capacity"""
        if not self.episodes:
            return
        
        # Remove oldest episode (first in list)
        self.episodes.pop(0)

class SemanticMemory:
    """Enhanced semantic memory for storing facts and knowledge"""
    
    def __init__(self, max_facts: int = 10000):
        """Initialize semantic memory
        
        Args:
            max_facts: Maximum number of facts
        """
        self.max_facts = max_facts
        self.facts = {}  # key -> value
        self.associations = {}  # key -> set of related keys
        self.timestamp = time.time()
    
    def store_fact(self, key: str, value: str):
        """Store fact
        
        Args:
            key: Fact key
            value: Fact value
        """
        self.facts[key] = value
        self.timestamp = time.time()
        
        # Extract associations from key
        self._extract_associations(key)
        
        # Enforce capacity
        if len(self.facts) > self.max_facts:
            self._evict_least_used()
    
    def retrieve_fact(self, key: str) -> str:
        """Retrieve fact
        
        Args:
            key: Fact key
            
        Returns:
            Fact value or empty string if not found
        """
        return self.facts.get(key, "")
    
    def has_fact(self, key: str) -> bool:
        """Check if fact exists
        
        Args:
            key: Fact key
            
        Returns:
            True if fact exists
        """
        return key in self.facts
    
    def get_all_facts(self) -> List[Tuple[str, str]]:
        """Get all facts
        
        Returns:
            List of (key, value) tuples
        """
        return list(self.facts.items())
    
    def get_related_facts(self, key: str, max_related: int = 10) -> List[Tuple[str, str]]:
        """Get related facts
        
        Args:
            key: Fact key
            max_related: Maximum number of related facts
            
        Returns:
            List of related (key, value) tuples
        """
        if key not in self.associations:
            return []
        
        related_keys = list(self.associations[key])[:max_related]
        return [(k, self.facts[k]) for k in related_keys if k in self.facts]
    
    def get_facts_by_prefix(self, prefix: str) -> List[Tuple[str, str]]:
        """Get facts by prefix
        
        Args:
            prefix: Fact prefix
            
        Returns:
            List of matching (key, value) tuples
        """
        return [(k, v) for k, v in self.facts.items() if k.startswith(prefix)]
    
    def clear(self):
        """Clear all facts"""
        self.facts.clear()
        self.associations.clear()
    
    def merge(self, other: 'SemanticMemory'):
        """Merge with another semantic memory
        
        Args:
            other: Other semantic memory
        """
        # Merge facts
        self.facts.update(other.facts)
        
        # Merge associations
        for key, other_associations in other.associations.items():
            if key not in self.associations:
                self.associations[key] = set()
            self.associations[key].update(other_associations)
        
        self.timestamp = max(self.timestamp, other.timestamp)
    
    def get_confidence(self, key: str) -> float:
        """Get confidence for fact
        
        Args:
            key: Fact key
            
        Returns:
            Confidence value (0 to 1)
        """
        # Simple confidence based on number of times accessed
        # In real implementation, this would use more sophisticated methods
        return 1.0  # Placeholder
    
    def _extract_associations(self, key: str):
        """Extract associations from key
        
        Args:
            key: Fact key
        """
        if key not in self.associations:
            self.associations[key] = set()
        
        # Simple associations: extract words from key
        words = key.lower().split()
        for word in words:
            if len(word) > 3:  # Ignore short words
                self.associations[key].add(word)
        
        # Add to other keys' associations
        for word in words:
            if len(word) > 3:
                for other_key in self.facts.keys():
                    if word in other_key.lower() and other_key != key:
                        if other_key not in self.associations:
                            self.associations[other_key] = set()
                        self.associations[other_key].add(key)
    
    def _evict_least_used(self):
        """Evict least used fact"""
        if not self.facts:
            return
        
        # Simple eviction: remove oldest fact (based on timestamp)
        # In real implementation, this would use more sophisticated eviction
        oldest_key = min(self.facts.keys(), key=lambda k: self.timestamps.get(k, 0))
        del self.facts[oldest_key]
        
        # Clean up associations
        if oldest_key in self.associations:
            for related_key in self.associations[oldest_key]:
                if related_key in self.associations:
                    self.associations[related_key].discard(oldest_key)
            del self.associations[oldest_key]

class ProceduralMemory:
    """Enhanced procedural memory for storing skills and habits"""
    
    def __init__(self, max_skills: int = 1000):
        """Initialize procedural memory
        
        Args:
            max_skills: Maximum number of skills
        """
        self.max_skills = max_skills
        self.skills = {}  # skill_name -> skill_data
        self.timestamp = time.time()
    
    def learn_skill(self, name: str, neural_pattern: List[NeuronId], proficiency: float = 0.0):
        """Learn skill
        
        Args:
            name: Skill name
            neural_pattern: Neural pattern for skill
            proficiency: Initial proficiency (0 to 1)
        """
        self.skills[name] = {
            'neural_pattern': neural_pattern,
            'proficiency': min(max(proficiency, 0.0), 1.0),
            'acquisition_time': time.time(),
            'usage_count': 0
        }
        
        self.timestamp = time.time()
        
        # Enforce capacity
        if len(self.skills) > self.max_skills:
            self._evict_least_used()
    
    def get_skill(self, name: str) -> Optional[Dict[str, Any]]:
        """Get skill
        
        Args:
            name: Skill name
            
        Returns:
            Skill data or None if not found
        """
        if name in self.skills:
            return self.skills[name].copy()
        return None
    
    def get_all_skills(self) -> List[str]:
        """Get all skill names
        
        Returns:
            List of skill names
        """
        return list(self.skills.keys())
    
    def update_proficiency(self, name: str, delta: float):
        """Update skill proficiency
        
        Args:
            name: Skill name
            delta: Proficiency change
            
        Returns:
            True if skill found and updated
        """
        if name in self.skills:
            skill = self.skills[name]
            skill['proficiency'] = min(max(skill['proficiency'] + delta, 0.0), 1.0)
            skill['usage_count'] += 1
            self.timestamp = time.time()
            return True
        return False
    
    def get_proficiency(self, name: str) -> float:
        """Get skill proficiency
        
        Args:
            name: Skill name
            
        Returns:
            Proficiency value (0 to 1)
        """
        if name in self.skills:
            return self.skills[name]['proficiency']
        return 0.0
    
    def can_execute(self, name: str) -> bool:
        """Check if skill can be executed
        
        Args:
            name: Skill name
            
        Returns:
            True if skill can be executed
        """
        skill = self.get_skill(name)
        if skill is None:
            return False
        
        # Check proficiency threshold
        return skill['proficiency'] > 0.1
    
    def execute_skill(self, name: str, parameters: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Execute skill
        
        args:
            name: Skill name
            parameters: Optional parameters
            
        Returns:
            Execution result
        """
        skill = self.get_skill(name)
        if skill is None:
            return {'success': False, 'reason': 'skill_not_found'}
        
        if not self.can_execute(name):
            return {'success': False, 'reason': 'insufficient_proficiency'}
        
        # Update usage count
        skill['usage_count'] += 1
        self.timestamp = time.time()
        
        # Create execution result
        result = {
            'success': True,
            'skill_name': name,
            'neural_pattern': skill['neural_pattern'],
            'proficiency': skill['proficiency'],
            'parameters': parameters or {},
            'timestamp': time.time()
        }
        
        return result
    
    def clear(self):
        """Clear all skills"""
        self.skills.clear()
    
    def _evict_least_used(self):
        """Evict least used skill"""
        if not self.skills:
            return
        
        # Evict skill with lowest usage count
        least_used = min(self.skills.items(), key=lambda x: x[1]['usage_count'])
        del self.skills[least_used[0]]

class AssociativeMemory:
    """Enhanced associative memory for storing relationships between representations"""
    
    def __init__(self, max_associations: int = 50000):
        """Initialize associative memory
        
        Args:
            max_associations: Maximum number of associations
        """
        self.max_associations = max_associations
        self.associations = {}  # (neuron_a, neuron_b) -> strength
        self.timestamp = time.time()
    
    def associate(self, neuron_a: NeuronId, neuron_b: NeuronId, strength: float = 1.0):
        """Create association
        
        Args:
            neuron_a: First neuron
            neuron_b: Second neuron
            strength: Association strength (0 to 1)
        """
        # Ensure strength is within bounds
        strength = min(max(strength, 0.0), 1.0)
        
        # Create bidirectional association
        pair1 = (neuron_a, neuron_b)
        pair2 = (neuron_b, neuron_a)
        
        self.associations[pair1] = strength
        self.associations[pair2] = strength
        
        self.timestamp = time.time()
        
        # Enforce capacity
        if len(self.associations) > self.max_associations:
            self._evict_weakest()
    
    def get_associations(self, neuron: NeuronId) -> List[NeuronId]:
        """Get associated neurons
        
        Args:
            neuron: Neuron identifier
            
        Returns:
            List of associated neurons
        """
        associated = []
        
        for (neuron_a, neuron_b), strength in self.associations.items():
            if neuron_a == neuron and neuron_b != neuron:
                associated.append(neuron_b)
            elif neuron_b == neuron and neuron_a != neuron:
                associated.append(neuron_a)
        
        return associated
    
    def get_association_strength(self, neuron_a: NeuronId, neuron_b: NeuronId) -> float:
        """Get association strength
        
        Args:
            neuron_a: First neuron
            neuron_b: Second neuron
            
        Returns:
            Association strength
        """
        pair1 = (neuron_a, neuron_b)
        pair2 = (neuron_b, neuron_a)
        
        return self.associations.get(pair1, self.associations.get(pair2, 0.0))
    
    def update_association(self, neuron_a: NeuronId, neuron_b: NeuronId, delta: float):
        """Update association strength
        
        Args:
            neuron_a: First neuron
            neuron_b: Second neuron
            delta: Strength change
            
        Returns:
            True if association updated
        """
        pair1 = (neuron_a, neuron_b)
        pair2 = (neuron_b, neuron_a)
        
        if pair1 in self.associations:
            self.associations[pair1] = min(max(self.associations[pair1] + delta, 0.0), 1.0)
            self.associations[pair2] = self.associations[pair1]
            self.timestamp = time.time()
            return True
        
        return False
    
    def get_strong_associations(self, threshold: float = 0.5) -> List[Tuple[NeuronId, NeuronId, float]]:
        """Get strong associations
        
        Args:
            threshold: Strength threshold
            
        Returns:
            List of (neuron_a, neuron_b, strength) tuples
        """
        strong_associations = []
        
        for (neuron_a, neuron_b), strength in self.associations.items():
            if strength >= threshold:
                strong_associations.append((neuron_a, neuron_b, strength))
        
        return strong_associations
    
    def clear(self):
        """Clear all associations"""
        self.associations.clear()
    
    def _evict_weakest(self):
        """Evict weakest associations"""
        if not self.associations:
            return
        
        # Evict associations with lowest strength
        weakest = min(self.associations.items(), key=lambda x: x[1])
        del self.associations[weakest[0]]
        
        # Also remove the reverse association
        reverse_pair = (weakest[0][1], weakest[0][0])
        if reverse_pair in self.associations:
            del self.associations[reverse_pair]

# Convenience functions
def create_working_memory(capacity: int = 100, decay_rate: float = 0.01) -> WorkingMemory:
    """Create working memory
    
    Args:
        capacity: Memory capacity
        decay_rate: Decay rate
        
    Returns:
        WorkingMemory instance
    """
    return WorkingMemory(capacity, decay_rate)

def create_episodic_memory(max_episodes: int = 1000, consolidation_threshold: float = 0.5) -> EpisodicMemory:
    """Create episodic memory
    
    Args:
        max_episodes: Maximum episodes
        consolidation_threshold: Consolidation threshold
        
    Returns:
        EpisodicMemory instance
    """
    return EpisodicMemory(max_episodes, consolidation_threshold)

def create_semantic_memory(max_facts: int = 10000) -> SemanticMemory:
    """Create semantic memory
    
    Args:
        max_facts: Maximum facts
        
    Returns:
        SemanticMemory instance
    """
    return SemanticMemory(max_facts)

def create_procedural_memory(max_skills: int = 1000) -> ProceduralMemory:
    """Create procedural memory
    
    Args:
        max_skills: Maximum skills
        
    Returns:
        ProceduralMemory instance
    """
    return ProceduralMemory(max_skills)

def create_associative_memory(max_associations: int = 50000) -> AssociativeMemory:
    """Create associative memory
    
    Args:
        max_associations: Maximum associations
        
    Returns:
        AssociativeMemory instance
    """
    return AssociativeMemory(max_associations)

class MemoryUtils:
    """Utility functions for memory operations"""
    
    @staticmethod
    def create_working_memory(capacity: int = 100, decay_rate: float = 0.01) -> WorkingMemory:
        """Create working memory
        
        Args:
            capacity: Memory capacity
            decay_rate: Decay rate
            
        Returns:
            WorkingMemory instance
        """
        return WorkingMemory(capacity, decay_rate)
    
    @staticmethod
    def create_episodic_memory(max_episodes: int = 1000, consolidation_threshold: float = 0.5) -> EpisodicMemory:
        """Create episodic memory
        
        Args:
            max_episodes: Maximum episodes
            consolidation_threshold: Consolidation threshold
            
        Returns:
            EpisodicMemory instance
        """
        return EpisodicMemory(max_episodes, consolidation_threshold)
    
    @staticmethod
    def create_semantic_memory(max_facts: int = 10000) -> SemanticMemory:
        """Create semantic memory
        
        Args:
            max_facts: Maximum facts
            
        Returns:
            SemanticMemory instance
        """
        return SemanticMemory(max_facts)
    
    @staticmethod
    def create_procedural_memory(max_skills: int = 1000) -> ProceduralMemory:
        """Create procedural memory
        
        Args:
            max_skills: Maximum skills
            
        Returns:
            ProceduralMemory instance
        """
        return ProceduralMemory(max_skills)
    
    @staticmethod
    def create_associative_memory(max_associations: int = 50000) -> AssociativeMemory:
        """Create associative memory
        
        Args:
            max_associations: Maximum associations
            
        Returns:
            AssociativeMemory instance
        """
        return AssociativeMemory(max_associations)
    
    @staticmethod
    def calculate_memory_usage(memory_objects: List) -> Dict[str, float]:
        """Calculate memory usage
        
        Args:
            memory_objects: List of memory objects
            
        Returns:
            Memory usage statistics
        """
        stats = {
            'total_objects': len(memory_objects),
            'total_capacity': 0,
            'utilization': 0.0,
            'oldest_timestamp': float('inf'),
            'newest_timestamp': 0.0
        }
        
        for obj in memory_objects:
            if hasattr(obj, 'capacity'):
                stats['total_capacity'] += obj.capacity
            
            if hasattr(obj, 'timestamp'):
                stats['oldest_timestamp'] = min(stats['oldest_timestamp'], obj.timestamp)
                stats['newest_timestamp'] = max(stats['newest_timestamp'], obj.timestamp)
        
        if stats['total_capacity'] > 0:
            stats['utilization'] = stats['total_objects'] / stats['total_capacity']
        
        return stats
    
    @staticmethod
    def recommend_memory_configuration(memory_type: str, expected_size: int) -> Dict[str, Any]:
        """Recommend memory configuration
        
        Args:
            memory_type: Memory type
            expected_size: Expected number of objects
            
        Returns:
            Recommended configuration
        """
        recommendations = {
            'working_memory': {
                'capacity': max(expected_size * 2, 100),
                'decay_rate': 0.01,
                'eviction_policy': 'LRU'
            },
            'episodic_memory': {
                'max_episodes': max(expected_size, 1000),
                'consolidation_threshold': 0.5,
                'eviction_policy': 'FIFO'
            },
            'semantic_memory': {
                'max_facts': max(expected_size * 10, 10000),
                'association_strength': 0.1,
                'eviction_policy': 'LRU'
            },
            'procedural_memory': {
                'max_skills': max(expected_size, 100),
                'proficiency_threshold': 0.1,
                'eviction_policy': 'least_used'
            },
            'associative_memory': {
                'max_associations': max(expected_size * 5, 50000),
                'strength_threshold': 0.5,
                'eviction_policy': 'weakest'
            }
        }
        
        return recommendations.get(memory_type, {})
