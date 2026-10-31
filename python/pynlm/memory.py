"""
Memory module for NLM Python bindings.
"""

import time
import math
from typing import Dict, List, Any, Optional, Tuple, Union
from enum import Enum
import json
import hashlib

try:
    from pynlm.bindings import createBrain, createDefaultConfig, createSimpleWorld
    BINDINGS_AVAILABLE = True
except ImportError:
    BINDINGS_AVAILABLE = False
    print("Warning: C++ bindings not available. Memory module limited.")


class MemoryStrength(Enum):
    """Memory strength levels."""
    WEAK = 0.1
    MODERATE = 0.5
    STRONG = 0.9
    REINFORCED = 1.0


class MemoryTrace:
    """Individual memory trace."""
    
    def __init__(self, content: Dict[str, Any], strength: float = 0.5, 
                 timestamp: float = None, reward: float = 0.0):
        """
        Initialize memory trace.
        
        Args:
            content: Memory content data
            strength: Memory strength (0.0 to 1.0)
            timestamp: Creation timestamp
            reward: Associated reward
        """
        self.content = content
        self.strength = max(0.0, min(1.0, strength))
        self.timestamp = timestamp or time.time()
        self.reward = reward
        self.last_accessed = self.timestamp
        self.access_count = 1
        self.retrieval_strength = self.strength
        
        # Calculate unique identifier
        content_str = json.dumps(content, sort_keys=True)
        self.id = hashlib.sha256(content_str.encode()).hexdigest()[:16]
    
    def update_strength(self, delta: float):
        """
        Update memory strength.
        
        Args:
            delta: Change in strength
        """
        self.strength = max(0.0, min(1.0, self.strength + delta))
        self.retrieval_strength = self.strength
    
    def retrieve(self) -> Dict[str, Any]:
        """Retrieve memory content and update access stats."""
        self.last_accessed = time.time()
        self.access_count += 1
        # Strength decays with time
        decay_factor = 0.99 ** (time.time() - self.last_accessed)
        self.retrieval_strength = self.strength * decay_factor
        return self.content.copy()
    
    def get_age(self) -> float:
        """Get memory age in seconds."""
        return time.time() - self.timestamp
    
    def should_decay(self, decay_rate: float = 0.01) -> bool:
        """
        Check if memory should decay.
        
        Args:
            decay_rate: Decay rate per second
        """
        return self.strength * decay_rate > 0.01 and self.get_age() > 100.0
    
    def __repr__(self) -> str:
        return f"MemoryTrace(id={self.id}, strength={self.strength:.3f}, age={self.get_age():.1f}s)"


class WorkingMemory:
    """Short-term working memory with competition and decay."""
    
    def __init__(self, capacity: int = 7, decay_rate: float = 0.05):
        """
        Initialize working memory.
        
        Args:
            capacity: Maximum number of items
            decay_rate: Decay rate per time unit
        """
        self.capacity = capacity
        self.decay_rate = decay_rate
        self.items = []
        self.recall_order = []
        self.last_updated = time.time()
    
    def add(self, content: Dict[str, Any], strength: float = 0.5):
        """
        Add item to working memory.
        
        Args:
            content: Item content
            strength: Initial strength
        """
        # Create memory trace
        trace = MemoryTrace(content, strength)
        
        # Check for similar items (competition)
        similar_index = self._find_similar(trace)
        
        if similar_index >= 0:
            # Replace similar item with stronger version
            self.items.pop(similar_index)
            self.recall_order.pop(similar_index)
        
        # Add new item
        self.items.append(trace)
        self.recall_order.append(len(self.items) - 1)
        
        # Apply decay to all items
        self._apply_decay()
        
        # Sort by retrieval strength for recall order
        self.items.sort(key=lambda x: x.retrieval_strength, reverse=True)
        self.recall_order = list(range(len(self.items)))
    
    def retrieve(self, index: int = 0) -> Dict[str, Any]:
        """
        Retrieve item by index.
        
        Args:
            index: Item index (0 = strongest)
        
        Returns:
            Retrieved content or None if empty
        """
        if not self.items or index < 0 or index >= len(self.items):
            return None
        
        trace = self.items[index]
        content = trace.retrieve()
        
        # Boost strength slightly on retrieval (priming)
        trace.update_strength(0.1)
        
        return content
    
    def get_all(self) -> List[Dict[str, Any]]:
        """Get all memory contents."""
        return [trace.retrieve() for trace in self.items]
    
    def clear(self):
        """Clear working memory."""
        self.items.clear()
        self.recall_order.clear()
    
    def _find_similar(self, new_trace: MemoryTrace) -> int:
        """Find similar memory item."""
        for i, trace in enumerate(self.items):
            # Simple similarity check based on content keys
            if set(new_trace.content.keys()) & set(trace.content.keys()):
                return i
        return -1
    
    def _apply_decay(self):
        """Apply decay to all items."""
        current_time = time.time()
        decay_factor = self.decay_rate * (current_time - self.last_updated)
        
        for trace in self.items:
            trace.update_strength(-decay_factor)
            # Remove very weak items
            if trace.strength < 0.01:
                # Find and remove
                try:
                    index = self.items.index(trace)
                    if index < len(self.recall_order):
                        self.recall_order.pop(index)
                    self.items.pop(index)
                    # Renumber recall order
                    self.recall_order = list(range(len(self.items)))
                except ValueError:
                    pass
        
        self.last_updated = current_time
    
    def get_strength_distribution(self) -> Dict[str, Any]:
        """Get strength distribution statistics."""
        if not self.items:
            return {'count': 0, 'mean_strength': 0.0, 'max_strength': 0.0, 'min_strength': 0.0}
        
        strengths = [trace.strength for trace in self.items]
        return {
            'count': len(self.items),
            'mean_strength': sum(strengths) / len(strengths),
            'max_strength': max(strengths),
            'min_strength': min(strengths),
            'total_strength': sum(strengths),
        }
    
    def __repr__(self) -> str:
        return f"WorkingMemory(items={len(self.items)}, capacity={self.capacity})"


class EpisodicMemory:
    """Long-term episodic memory with replay."""
    
    def __init__(self, max_capacity: int = 1000):
        """
        Initialize episodic memory.
        
        Args:
            max_capacity: Maximum number of episodes
        """
        self.capacity = max_capacity
        self.episodes = []
        self.consolidation_threshold = 0.7
        self.replay_frequency = 0.1
        self.last_replay_check = time.time()
        self.current_episode = None
        self.replay_cooldown = 3600.0  # 1 hour
        self.last_replay_time = -self.replay_cooldown
    
    def start_episode(self, context: Dict[str, Any] = None):
        """
        Start new episode.
        
        Args:
            context: Episode context
        """
        self.current_episode = {
            'context': context or {},
            'events': [],
            'start_time': time.time(),
            'end_time': None,
            'rewards': [],
            'actions': [],
            'sensory_inputs': [],
            'quality': 0.0,
        }
    
    def add_event(self, event_type: str, data: Dict[str, Any] = None):
        """
        Add event to current episode.
        
        Args:
            event_type: Type of event
            data: Event data
        """
        if self.current_episode:
            event = {
                'type': event_type,
                'data': data or {},
                'timestamp': time.time(),
            }
            self.current_episode['events'].append(event)
    
    def add_sensory_input(self, input_data: Dict[str, Any]):
        """Add sensory input to current episode."""
        if self.current_episode:
            sensory_event = {
                'type': 'sensory_input',
                'data': input_data,
                'timestamp': time.time(),
            }
            self.current_episode['sensory_inputs'].append(sensory_event)
    
    def add_action(self, action: Dict[str, Any]):
        """Add action to current episode."""
        if self.current_episode:
            action_event = {
                'type': 'action',
                'data': action,
                'timestamp': time.time(),
            }
            self.current_episode['actions'].append(action_event)
    
    def add_reward(self, reward: float):
        """Add reward to current episode."""
        if self.current_episode:
            self.current_episode['rewards'].append({
                'value': reward,
                'timestamp': time.time(),
            })
    
    def end_episode(self):
        """End current episode and consolidate."""
        if self.current_episode:
            self.current_episode['end_time'] = time.time()
            self._calculate_episode_quality()
            
            # Add to episodes list
            self._add_to_episodes()
            
            # Check for replay
            self._check_for_replay()
            
            self.current_episode = None
    
    def _calculate_episode_quality(self):
        """Calculate episode quality based on rewards and actions."""
        if not self.current_episode:
            return
        
        rewards = self.current_episode['rewards']
        if rewards:
            total_reward = sum(r['value'] for r in rewards)
            episode_length = len(self.current_episode['events'])
            
            # Quality score (0 to 1)
            quality = min(1.0, total_reward / 100.0 + episode_length / 1000.0)
            self.current_episode['quality'] = quality
    
    def _add_to_episodes(self):
        """Add episode to storage with capacity management."""
        if not self.current_episode:
            return
        
        episode_copy = self.current_episode.copy()
        
        # Check capacity
        if len(self.episodes) >= self.capacity:
            # Find weakest episode to replace
            weakest_idx = min(range(len(self.episodes)), 
                            key=lambda i: self.episodes[i].get('quality', 0))
            self.episodes.pop(weakest_idx)
        
        self.episodes.append(episode_copy)
    
    def _check_for_replay(self):
        """Check if episode should be replayed."""
        current_time = time.time()
        
        if (current_time - self.last_replay_time > self.replay_cooldown and
            self.current_episode and
            self.current_episode['quality'] > self.consolidation_threshold):
            
            # Trigger replay (could be extended)
            self.last_replay_time = current_time
    
    def retrieve_episodes(self, context_filter: Dict[str, Any] = None, 
                         time_window: float = None, quality_threshold: float = 0.0) -> List[Dict[str, Any]]:
        """
        Retrieve episodes based on criteria.
        
        Args:
            context_filter: Context filter
            time_window: Time window (seconds ago)
            quality_threshold: Minimum quality threshold
        
        Returns:
            List of matching episodes
        """
        current_time = time.time()
        matching_episodes = []
        
        for episode in self.episodes:
            # Check quality threshold
            if episode.get('quality', 0) < quality_threshold:
                continue
            
            # Check context filter
            if context_filter:
                episode_context = episode.get('context', {})
                context_match = all(
                    episode_context.get(k) == v for k, v in context_filter.items()
                )
                if not context_match:
                    continue
            
            # Check time window
            if time_window:
                episode_time = episode.get('end_time', episode.get('start_time', 0))
                if current_time - episode_time > time_window:
                    continue
            
            matching_episodes.append(episode)
        
        return matching_episodes
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get memory statistics."""
        if not self.episodes:
            return {'episodes': 0, 'total_events': 0, 'avg_quality': 0.0}
        
        total_events = sum(len(episode['events']) for episode in self.episodes)
        total_quality = sum(episode.get('quality', 0) for episode in self.episodes)
        
        return {
            'episodes': len(self.episodes),
            'total_events': total_events,
            'avg_quality': total_quality / len(self.episodes),
            'max_quality': max(episode.get('quality', 0) for episode in self.episodes),
            'min_quality': min(episode.get('quality', 0) for episode in self.episodes),
        }
    
    def consolidate_weak_memories(self, threshold: float = 0.3):
        """
        Consolidate weak memories (placeholder for future implementation).
        
        Args:
            threshold: Memory strength threshold for consolidation
        """
        # This would be extended to actually consolidate memories
        pass
    
    def get_replay_candidates(self) -> List[Dict[str, Any]]:
        """Get episodes eligible for replay."""
        current_time = time.time()
        candidates = []
        
        for episode in self.episodes:
            quality = episode.get('quality', 0)
            if (quality > self.consolidation_threshold and
                current_time - episode.get('end_time', 0) > self.replay_cooldown):
                candidates.append(episode)
        
        return candidates
    
    def __repr__(self) -> str:
        return f"EpisodicMemory(episodes={len(self.episodes)}, capacity={self.capacity})"


class SemanticMemory:
    """Semantic memory for abstract concepts and knowledge."""
    
    def __init__(self):
        """Initialize semantic memory."""
        self.concept_embeddings = {}
        self.relation_network = {}
        self.pattern_database = {}
        self.activation_threshold = 0.1
        self.consolidation_rate = 0.05
    
    def add_concept(self, concept_name: str, features: List[float],
                   description: str = "", examples: List[str] = None):
        """
        Add concept to semantic memory.
        
        Args:
            concept_name: Name of concept
            features: Feature vector for concept
            description: Description of concept
            examples: Example instances
        """
        self.concept_embeddings[concept_name] = {
            'features': features,
            'description': description,
            'examples': examples or [],
            'activation': 0.0,
            'last_accessed': time.time(),
            'access_count': 1,
        }
    
    def add_relation(self, subject: str, predicate: str, object: str,
                    confidence: float = 1.0):
        """
        Add relation to semantic memory.
        
        Args:
            subject: Subject concept
            predicate: Relation predicate
            object: Object concept
            confidence: Relation confidence (0.0 to 1.0)
        """
        key = f"{subject}_{predicate}_{object}"
        self.relation_network[key] = {
            'subject': subject,
            'predicate': predicate,
            'object': object,
            'confidence': confidence,
            'timestamp': time.time(),
        }
    
    def add_pattern(self, pattern_id: str, pattern: Dict[str, Any],
                   category: str = ""):
        """
        Add pattern to semantic memory.
        
        Args:
            pattern_id: Pattern identifier
            pattern: Pattern data
            category: Pattern category
        """
        self.pattern_database[pattern_id] = {
            'pattern': pattern,
            'category': category,
            'timestamp': time.time(),
            'activation_count': 0,
        }
    
    def activate_concept(self, concept_name: str) -> Dict[str, Any]:
        """
        Activate concept and return its data.
        
        Args:
            concept_name: Name of concept to activate
        
        Returns:
            Concept data or None if not found
        """
        if concept_name in self.concept_embeddings:
            concept = self.concept_embeddings[concept_name]
            concept['activation'] = min(1.0, concept['activation'] + 0.1)
            concept['last_accessed'] = time.time()
            concept['access_count'] += 1
            return concept.copy()
        return None
    
    def find_related_concepts(self, concept_name: str, max_results: int = 5) -> List[str]:
        """
        Find concepts related to given concept.
        
        Args:
            concept_name: Base concept
            max_results: Maximum results to return
        
        Returns:
            List of related concept names
        """
        if concept_name not in self.concept_embeddings:
            return []
        
        related = set()
        concept = self.concept_embeddings[concept_name]
        
        # Find via relations
        for relation_id, relation in self.relation_network.items():
            if relation['subject'] == concept_name:
                related.add(relation['object'])
            elif relation['object'] == concept_name:
                related.add(relation['subject'])
        
        # Convert to list and limit
        return list(related)[:max_results]
    
    def detect_pattern(self, data: Dict[str, Any]) -> List[str]:
        """
        Detect patterns in data.
        
        Args:
            data: Data to analyze
        
        Returns:
            List of matching pattern IDs
        """
        matches = []
        
        for pattern_id, pattern_data in self.pattern_database.items():
            # Simple pattern matching
            pattern = pattern_data['pattern']
            if self._matches_pattern(data, pattern):
                matches.append(pattern_id)
                pattern_data['activation_count'] += 1
                pattern_data['last_accessed'] = time.time()
        
        return matches
    
    def _matches_pattern(self, data: Dict[str, Any], pattern: Dict[str, Any]) -> bool:
        """Check if data matches pattern."""
        for key, value in pattern.items():
            if key not in data:
                return False
            if isinstance(value, (list, tuple)):
                if not any(item in data[key] for item in value):
                    return False
            elif isinstance(value, dict):
                if not self._matches_pattern(data[key], value):
                    return False
            elif data[key] != value:
                return False
        return True
    
    def get_concept_similarities(self, concept1: str, concept2: str) -> float:
        """
        Get similarity between two concepts.
        
        Args:
            concept1, concept2: Concept names
        
        Returns:
            Similarity score (0.0 to 1.0)
        """
        if concept1 not in self.concept_embeddings or concept2 not in self.concept_embeddings:
            return 0.0
        
        features1 = self.concept_embeddings[concept1]['features']
        features2 = self.concept_embeddings[concept2]['features']
        
        if len(features1) != len(features2) or not features1:
            return 0.0
        
        # Cosine similarity
        dot_product = sum(f1 * f2 for f1, f2 in zip(features1, features2))
        norm1 = math.sqrt(sum(f ** 2 for f in features1))
        norm2 = math.sqrt(sum(f ** 2 for f in features2))
        
        if norm1 == 0 or norm2 == 0:
            return 0.0
        
        return dot_product / (norm1 * norm2)
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get semantic memory statistics."""
        return {
            'concepts': len(self.concept_embeddings),
            'relations': len(self.relation_network),
            'patterns': len(self.pattern_database),
            'most_active_concept': self._get_most_active_concept(),
        }
    
    def _get_most_active_concept(self) -> str:
        """Get most frequently accessed concept."""
        if not self.concept_embeddings:
            return ""
        
        return max(
            self.concept_embeddings.items(),
            key=lambda x: x[1]['access_count']
        )[0]
    
    def consolidate(self):
        """Consolidate memory patterns."""
        # Strengthen frequently accessed concepts
        for concept in self.concept_embeddings.values():
            concept['activation'] *= (1 + self.consolidation_rate)
            if concept['activation'] > 1.0:
                concept['activation'] = 1.0
    
    def __repr__(self) -> str:
        return f"SemanticMemory(concepts={len(self.concept_embeddings)}, relations={len(self.relation_network)})")


class LongTermMemory:
    """Integrated long-term memory system."""
    
    def __init__(self, working_capacity: int = 7, episodic_capacity: int = 1000):
        """
        Initialize long-term memory.
        
        Args:
            working_capacity: Working memory capacity
            episodic_capacity: Episodic memory capacity
        """
        self.working_memory = WorkingMemory(capacity=working_capacity)
        self.episodic_memory = EpisodicMemory(max_capacity=episodic_capacity)
        self.semantic_memory = SemanticMemory()
        
        # Memory integration
        self.integration_strength = 0.5
        self.synaptic_consolidation_rate = 0.01
        self.reinforcement_threshold = 0.7
        
        # Performance tracking
        self.memory_stats = {
            'working_hits': 0,
            'working_misses': 0,
            'episodic_retrievals': 0,
            'semantic_activations': 0,
            'consolidation_events': 0,
        }
    
    def add_working_memory_item(self, content: Dict[str, Any], strength: float = 0.5):
        """Add item to working memory."""
        self.working_memory.add(content, strength)
    
    def retrieve_working_memory(self, index: int = 0) -> Dict[str, Any]:
        """Retrieve item from working memory."""
        content = self.working_memory.retrieve(index)
        if content is not None:
            self.memory_stats['working_hits'] += 1
        else:
            self.memory_stats['working_misses'] += 1
        return content
    
    def start_episodic_episode(self, context: Dict[str, Any] = None):
        """Start episodic episode."""
        self.episodic_memory.start_episode(context)
    
    def add_episodic_event(self, event_type: str, data: Dict[str, Any] = None):
        """Add event to episodic memory."""
        self.episodic_memory.add_event(event_type, data)
    
    def add_episodic_sensory_input(self, input_data: Dict[str, Any]):
        """Add sensory input to episodic memory."""
        self.episodic_memory.add_sensory_input(input_data)
    
    def add_episodic_action(self, action: Dict[str, Any]):
        """Add action to episodic memory."""
        self.episodic_memory.add_action(action)
    
    def add_episodic_reward(self, reward: float):
        """Add reward to episodic memory."""
        self.episodic_memory.add_reward(reward)
    
    def end_episodic_episode(self):
        """End episodic episode."""
        self.episodic_memory.end_episode()
    
    def activate_concept(self, concept_name: str) -> Dict[str, Any]:
        """Activate semantic concept."""
        concept = self.semantic_memory.activate_concept(concept_name)
        if concept:
            self.memory_stats['semantic_activations'] += 1
        return concept
    
    def find_related_concepts(self, concept_name: str, max_results: int = 5) -> List[str]:
        """Find related concepts."""
        return self.semantic_memory.find_related_concepts(concept_name, max_results)
    
    def detect_patterns(self, data: Dict[str, Any]) -> List[str]:
        """Detect patterns in data."""
        return self.semantic_memory.detect_pattern(data)
    
    def consolidate_memories(self):
        """Consolidate all memory systems."""
        # Consolidate episodic memories
        self.episodic_memory.consolidate_weak_memories()
        
        # Consolidate semantic memory
        self.semantic_memory.consolidate()
        
        # Update integration strength
        self.integration_strength = min(1.0, self.integration_strength + self.synaptic_consolidation_rate)
        
        self.memory_stats['consolidation_events'] += 1
    
    def get_retrieval_strength(self, memory_type: str) -> float:
        """
        Get overall retrieval strength for memory type.
        
        Args:
            memory_type: Type of memory ('working', 'episodic', 'semantic')
        """
        if memory_type == 'working':
            stats = self.working_memory.get_strength_distribution()
            return stats['mean_strength'] if stats['count'] > 0 else 0.0
        elif memory_type == 'episodic':
            return self.episodic_memory.get_statistics().get('avg_quality', 0.0)
        elif memory_type == 'semantic':
            # Return average concept activation
            if not self.semantic_memory.concept_embeddings:
                return 0.0
            return sum(c['activation'] for c in self.semantic_memory.concept_embeddings.values()) / len(self.semantic_memory.concept_embeddings)
        else:
            return 0.0
    
    def get_all_memories_stats(self) -> Dict[str, Any]:
        """Get statistics from all memory systems."""
        return {
            'working_memory': self.working_memory.get_strength_distribution(),
            'episodic_memory': self.episodic_memory.get_statistics(),
            'semantic_memory': self.semantic_memory.get_statistics(),
            'integration_strength': self.integration_strength,
            'performance': self.memory_stats,
        }
    
    def add_to_working_from_experience(self, experience: Dict[str, Any], reward: float = 0.0):
        """
        Add experience to working memory with reinforcement.
        
        Args:
            experience: Experience data
            reward: Associated reward
        """
        # Determine strength based on reward
        if reward > self.reinforcement_threshold:
            strength = MemoryStrength.REINFORCED.value
        elif reward > 0:
            strength = MemoryStrength.STRONG.value
        else:
            strength = MemoryStrength.MODERATE.value
        
        self.add_working_memory_item(experience, strength)
    
    def retrieve_relevant_memories(self, query: Dict[str, Any], memory_types: List[str] = None) -> Dict[str, List]:
        """
        Retrieve relevant memories based on query.
        
        Args:
            query: Query criteria
            memory_types: Memory types to search ('working', 'episodic', 'semantic')
        """
        if memory_types is None:
            memory_types = ['working', 'episodic', 'semantic']
        
        results = {}
        
        if 'working' in memory_types:
            # Working memory - get all items
            results['working'] = [item for item in self.working_memory.get_all() if self._matches_query(item, query)]
        
        if 'episodic' in memory_types:
            # Episodic memory - retrieve by context
            context_filter = {k: v for k, v in query.items() if k != 'reward'}
            results['episodic'] = self.episodic_memory.retrieve_episodes(context_filter)
        
        if 'semantic' in memory_types:
            # Semantic memory - detect patterns
            results['semantic'] = self.semantic_memory.detect_patterns(query)
        
        return results
    
    def _matches_query(self, item: Dict[str, Any], query: Dict[str, Any]) -> bool:
        """Check if item matches query."""
        for key, value in query.items():
            if key not in item:
                return False
            if isinstance(value, (list, tuple)):
                if item[key] not in value:
                    return False
            elif item[key] != value:
                return False
        return True
    
    def export_memories(self, filepath: str = "nlm_memories.json"):
        """Export memories to file."""
        data = {
            'working_memory_items': [trace.content for trace in self.working_memory.items],
            'episodic_episodes': self.episodic_memory.episodes,
            'semantic_concepts': self.semantic_memory.concept_embeddings,
            'semantic_relations': self.semantic_memory.relation_network,
            'semantic_patterns': self.semantic_memory.pattern_database,
            'statistics': self.memory_stats,
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
    
    def import_memories(self, filepath: str = "nlm_memories.json"):
        """Import memories from file."""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        # Restore working memory items
        for content in data.get('working_memory_items', []):
            self.working_memory.add(content)
        
        # Restore episodic episodes
        self.episodic_memory.episodes = data.get('episodic_episodes', [])
        
        # Restore semantic memory
        self.semantic_memory.concept_embeddings = data.get('semantic_concepts', {})
        self.semantic_memory.relation_network = data.get('semantic_relations', {})
        self.semantic_memory.pattern_database = data.get('semantic_patterns', {})
        
        # Restore statistics
        self.memory_stats.update(data.get('statistics', {}))
    
    def reset(self):
        """Reset all memory systems."""
        self.working_memory.clear()
        self.episodic_memory.episodes.clear()
        self.semantic_memory.concept_embeddings.clear()
        self.semantic_memory.relation_network.clear()
        self.semantic_memory.pattern_database.clear()
        
        self.memory_stats = {
            'working_hits': 0,
            'working_misses': 0,
            'episodic_retrievals': 0,
            'semantic_activations': 0,
            'consolidation_events': 0,
        }
    
    def __repr__(self) -> str:
        return f"LongTermMemory(working={len(self.working_memory.items)}, episodic={len(self.episodic_memory.episodes)}, semantic={len(self.semantic_memory.concept_embeddings)})"


def create_long_term_memory(working_capacity: int = 7, episodic_capacity: int = 1000) -> LongTermMemory:
    """Create long-term memory system."""
    return LongTermMemory(working_capacity, episodic_capacity)


def create_semantic_concept(name: str, features: List[float], description: str = ""):
    """Create semantic concept."""
    from . import SemanticMemory
    memory = SemanticMemory()
    memory.add_concept(name, features, description)
    return memory


def create_memory_episode(context: Dict[str, Any] = None):
    """Create new memory episode."""
    from . import EpisodicMemory
    memory = EpisodicMemory()
    memory.start_episode(context)
    return memory


def export_memory_system(memory: LongTermMemory, filepath: str = "nlm_memories.json"):
    """Export memory system to file."""
    memory.export_memories(filepath)


def import_memory_system(filepath: str = "nlm_memories.json") -> LongTermMemory:
    """Import memory system from file."""
    memory = create_long_term_memory()
    memory.import_memories(filepath)
    return memory
