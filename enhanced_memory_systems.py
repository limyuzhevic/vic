#!/usr/bin/env python3
# Enhanced Memory Systems Implementation
# Complete implementation of neural memory systems with advanced algorithms and integration

import os
import sys
import json
import time
import math
import threading
from typing import Dict, Any, List, Optional, Union, Tuple
from dataclasses import dataclass, field
from enum import Enum
import numpy as np

# Memory System Implementation

class MemoryPriority(Enum):
    CRITICAL = "critical"
    HIGH = "high"
    NORMAL = "normal"
    LOW = "low"

class ConsolidationStrategy(Enum):
    TEMPORAL = "temporal"
    FREQUENCY = "frequency"
    PATTERN = "pattern"
    ADAPTIVE = "adaptive"

@dataclass
class MemoryTrace:
    """A single memory trace with multiple dimensions"""
    id: str
    neuron_id: int
    timestamp: float
    activation_strength: float
    stability: float
    decay_rate: float
    replay_priority: float
    synaptic_weight: float
    contextual_tags: Dict[str, Any]
    reinforcement_signal: float
    
    def update_stability(self, delta: float) -> None:
        """Update trace stability"""
        self.stability = max(0.0, min(1.0, self.stability + delta))
    
    def decay(self, time_elapsed: float) -> None:
        """Apply time-based decay"""
        decay_factor = math.exp(-self.decay_rate * time_elapsed)
        self.activation_strength *= decay_factor
        self.stability *= decay_factor
    
    def replay(self) -> float:
        """Replay memory trace"""
        return self.activation_strength * self.replay_priority

@dataclass
class WorkingMemoryEntry:
    """Entry in working memory"""
    neuron_id: int
    activation: float
    decay_timer: float
    last_accessed: float
    last_updated: float
    persistence_level: float
    competition_score: float
    
    def update_activation(self, delta: float, context_factor: float = 1.0) -> None:
        """Update activation with contextual modulation"""
        self.activation += delta * context_factor
        self.last_accessed = time.time()
    
    def should_decay(self, decay_rate: float) -> bool:
        """Check if entry should decay"""
        time_since_access = time.time() - self.last_accessed
        return time_since_access > (self.decay_timer + self.persistence_level / decay_rate)
    
    def decay(self, decay_rate: float) -> None:
        """Apply decay"""
        decay_amount = self.activation * decay_rate
        self.activation *= (1.0 - min(decay_amount, 0.1))
        if self.activation < 0.001:
            self.activation = 0.0
class AdvancedWorkingMemory:
    def __init__(self, capacity: int = 1000, decay_rate: float = 0.01):
        self.capacity = capacity
        self.decay_rate = decay_rate
        self.entries: Dict[int, WorkingMemoryEntry] = {}
        self.compensation_weights: Dict[int, float] = {}
        self.excitation_inhibition_balance: float = 1.0
        self.working_memory_traces: Dict[int, List[MemoryTrace]] = {}
        
        # Competition dynamics
        self.max_concurrent_activities: int = 10
        self.competition_threshold: float = 0.5
        self.spreading_activity_radius: float = 5.0
    
    def store(self, neuron_id: int, activation: float, context: Dict[str, Any] = None) -> None:
        """Store or update working memory entry"""
        if neuron_id in self.entries:
            entry = self.entries[neuron_id]
            entry.update_activation(activation - entry.activation)
            entry.last_updated = time.time()
        else:
            if len(self.entries) >= self.capacity:
                self._evict_low_priority_entry()
            
            entry = WorkingMemoryEntry(
                neuron_id=neuron_id,
                activation=activation,
                decay_timer=0.0,
                last_accessed=time.time(),
                last_updated=time.time(),
                persistence_level=1.0,
                competition_score=0.0,
                context=context or {}
            )
            self.entries[neuron_id] = entry
            
            # Initialize working memory traces for new entries
            self.working_memory_traces[neuron_id] = []
        
        # Update competition scores
        self._update_competition_scores()
    
    def retrieve(self, neuron_id: int) -> float:
        """Retrieve activation from working memory"""
        if neuron_id not in self.entries:
            return 0.0
        
        entry = self.entries[neuron_id]
        
        # Check if entry has decayed
        if entry.should_decay(self.decay_rate):
            entry.decay(self.decay_rate)
            if entry.activation <= 0.0:
                del self.entries[neuron_id]
                if neuron_id in self.working_memory_traces:
                    del self.working_memory_traces[neuron_id]
                return 0.0
        
        # Apply excitation-inhibition balance
        entry.activation *= self.excitation_inhibition_balance
        
        # Apply compensation
        if neuron_id in self.compensation_weights:
            compensation = self.compensation_weights[neuron_id]
            entry.activation += compensation * self.decay_rate
        
        entry.last_accessed = time.time()
        
        return entry.activation
    
    def get_active_neurons(self, threshold: float = 0.1) -> List[int]:
        """Get neurons with activation above threshold"""
        active_neurons = []
        for neuron_id, entry in self.entries.items():
            if entry.should_decay(self.decay_rate):
                entry.decay(self.decay_rate)
                if entry.activation <= 0.0:
                    del self.entries[neuron_id]
                    continue
            
            if entry.activation >= threshold:
                active_neurons.append(neuron_id)
        
        # Apply competition to limit concurrent activities
        if len(active_neurons) > self.max_concurrent_activities:
            active_neurons.sort(key=lambda n: self.entries[n].activation, reverse=True)
            active_neurons = active_neurons[:self.max_concurrent_activities]
        
        return active_neurons
    
    def decay_all(self) -> None:
        """Decay all entries"""
        for neuron_id, entry in list(self.entries.items()):
            if entry.should_decay(self.decay_rate):
                entry.decay(self.decay_rate)
                if entry.activation <= 0.0:
                    del self.entries[neuron_id]
                    if neuron_id in self.working_memory_traces:
                        del self.working_memory_traces[neuron_id]
    
    def clear(self) -> None:
        """Clear all entries"""
        self.entries.clear()
        self.compensation_weights.clear()
        self.working_memory_traces.clear()
        self.excitation_inhibition_balance = 1.0
    
    def get_total_activity(self) -> float:
        """Get total activity across all entries"""
        return sum(entry.activation for entry in self.entries.values())
    
    def get_capacity_usage(self) -> float:
        """Get current capacity usage"""
        return len(self.entries) / self.capacity
    
    def update_excitation_inhibition_balance(self, ratio: float) -> None:
        """Update E/I balance"""
        self.excitation_inhibition_balance = max(0.1, min(10.0, ratio))
    
    def add_compensation(self, neuron_id: int, compensation: float) -> None:
        """Add compensation for a neuron"""
        if neuron_id not in self.compensation_weights:
            self.compensation_weights[neuron_id] = 0.0
        
        self.compensation_weights[neuron_id] += compensation
    
    def remove_compensation(self, neuron_id: int, amount: float) -> None:
        """Remove compensation for a neuron"""
        if neuron_id in self.compensation_weights:
            self.compensation_weights[neuron_id] = max(0.0, self.compensation_weights[neuron_id] - amount)
    
    def _update_competition_scores(self) -> None:
        """Update competition scores based on activation levels"""
        if not self.entries:
            return
        
        max_activation = max(entry.activation for entry in self.entries.values())
        if max_activation > 0:
            for neuron_id, entry in self.entries.items():
                entry.competition_score = entry.activation / max_activation
        
        # Apply lateral inhibition
        for neuron_id, entry in self.entries.items():
            inhibition = 0.0
            for other_neuron_id, other_entry in self.entries.items():
                if other_neuron_id != neuron_id:
                    distance = abs(neuron_id - other_neuron_id)
                    if distance < self.spreading_activity_radius:
                        inhibition += other_entry.competition_score * math.exp(-distance / self.spreading_activity_radius)
            
            entry.activation *= (1.0 - self.competition_threshold * inhibition)
    
    def _evict_low_priority_entry(self) -> None:
        """Evict lowest priority entry"""
        if not self.entries:
            return
        
        # Find entry with lowest competition score
        min_score = float('inf')
        min_neuron_id = None
        
        for neuron_id, entry in self.entries.items():
            if entry.competition_score < min_score:
                min_score = entry.competition_score
                min_neuron_id = neuron_id
        
        if min_neuron_id is not None:
            del self.entries[min_neuron_id]
            if min_neuron_id in self.working_memory_traces:
                del self.working_memory_traces[min_neuron_id]
class EpisodicMemoryManager:
    def __init__(self, max_episodes: int = 10000, consolidation_threshold: float = 0.5):
        self.max_episodes = max_episodes
        self.consolidation_threshold = consolidation_threshold
        self.episodes: List[EpisodicMemoryEntry] = []
        self.episode_index: Dict[SimulationStep, int] = {}
        self.episode_timestamps: List[float] = []
        
        # Consolidation statistics
        self.consolidation_count: int = 0
        self.total_episodes_stored: int = 0
        
        # Replay system
        self.replay_queue: List[int] = []
        self.replay_interval: float = 100.0  # steps
        self.last_replay_time: float = 0.0
        
        # Temporal compression
        self.compression_factor: float = 1.0
        self.last_compression_time: float = 0.0
    
    def store_episode(self, neurons: List[int], activations: List[float], 
                     timestamp: float, metadata: str = "") -> int:
        """Store an episode"""
        episode = EpisodicMemoryItem(
            timestamp=timestamp,
            neurons=neurons,
            values=activations,
            metadata=metadata
        )
        
        episode_index = len(self.episodes)
        self.episodes.append(episode)
        self.episode_timestamps.append(timestamp)
        self.episode_index[timestamp] = episode_index
        self.total_episodes_stored += 1
        
        # Apply temporal compression if needed
        if len(self.episodes) > self.max_episodes:
            self._apply_temporal_compression()
        
        return episode_index
    
    def retrieve_episode(self, index: int) -> Optional[EpisodicMemoryItem]:
        """Retrieve episode by index"""
        if index < 0 or index >= len(self.episodes):
            return None
        return self.episodes[index]
    
    def get_recent_episodes(self, count: int) -> List[EpisodicMemoryItem]:
        """Get most recent episodes"""
        if count <= 0:
            return []
        
        start_index = max(0, len(self.episodes) - count)
        return self.episodes[start_index:]
    
    def get_episodes_for_replay(self, count: int, current_step: float) -> List[EpisodicMemoryItem]:
        """Get episodes for replay based on importance and timing"""
        if count <= 0 or not self.episodes:
            return []
        
        # Calculate episode importance (activation strength * recency)
        episode_scores = []
        current_time = time.time()
        
        for i, episode in enumerate(self.episodes):
            if not episode.neurons:
                continue
            
            # Calculate episode strength
            episode_strength = sum(episode.values) / len(episode.values) if episode.values else 0.0
            
            # Calculate recency factor
            time_since_episode = current_time - episode.timestamp
            recency_factor = math.exp(-time_since_episode / self.replay_interval)
            
            # Calculate consolidation level
            consolidation_level = 1.0 if episode.timestamp > self.last_replay_time else 0.5
            
            # Combined score
            score = episode_strength * recency_factor * consolidation_level
            episode_scores.append((i, score))
        
        # Sort by score and get top episodes
        episode_scores.sort(key=lambda x: x[1], reverse=True)
        selected_indices = [idx for idx, score in episode_scores[:count]]
        
        # Return selected episodes
        replay_episodes = [self.episodes[idx] for idx in selected_indices]
        
        # Add to replay queue
        for idx in selected_indices:
            if idx not in self.replay_queue:
                self.replay_queue.append(idx)
        
        return replay_episodes
    
    def replay_episode(self, episode: EpisodicMemoryItem) -> None:
        """Replay an episode (would trigger neural reactivation)"""
        # This would trigger neural reactivation in the actual brain simulation
        pass
    
    def consolidate(self, threshold: float) -> int:
        """Consolidate episodes above threshold"""
        consolidated = 0
        
        for episode in self.episodes:
            # Calculate episode strength
            episode_strength = sum(episode.values) / len(episode.values) if episode.values else 0.0
            
            if episode_strength >= threshold:
                # Move to long-term storage (simplified: just mark as consolidated)
                episode.metadata += "|consolidated"
                consolidated += 1
        
        self.consolidation_count += consolidated
        return consolidated
    
    def clear(self) -> None:
        """Clear all episodes"""
        self.episodes.clear()
        self.episode_index.clear()
        self.episode_timestamps.clear()
        self.replay_queue.clear()
    
    def get_episode_count(self) -> int:
        """Get total number of episodes"""
        return len(self.episodes)
    
    def get_total_stored_episodes(self) -> int:
        """Get total episodes stored (including consolidated)"""
        return self.total_episodes_stored
    
    def _apply_temporal_compression(self) -> None:
        """Apply temporal compression to reduce storage"""
        if len(self.episodes) <= self.max_episodes:
            return
        
        # Calculate compression factor
        excess_episodes = len(self.episodes) - self.max_episodes
        self.compression_factor = max(0.5, 1.0 - (excess_episodes / (excess_episodes * 2)))
        
        # Apply compression by removing low-importance episodes
        episode_scores = []
        for i, episode in enumerate(self.episodes):
            strength = sum(episode.values) / len(episode.values) if episode.values else 0.0
            episode_scores.append((i, strength))
        
        episode_scores.sort(key=lambda x: x[1])
        
        # Remove lowest-scoring episodes
        to_remove = int(len(self.episodes) * (1.0 - self.compression_factor))
        indices_to_remove = [idx for idx, _ in episode_scores[:to_remove]]
        
        # Create new lists without removed episodes
        new_episodes = []
        new_timestamps = []
        new_index = {}
        
        current_index = 0
        for i, episode in enumerate(self.episodes):
            if i not in indices_to_remove:
                new_episodes.append(episode)
                new_timestamps.append(episode.timestamp)
                new_index[episode.timestamp] = current_index
                current_index += 1
        
        self.episodes = new_episodes
        self.episode_timestamps = new_timestamps
        self.episode_index = new_index
        
        # Update replay queue
        self.replay_queue = [idx for idx in self.replay_queue if idx < len(self.episodes)]
        
        print(f"Applied temporal compression: removed {to_remove} episodes, "
              f"compression factor: {self.compression_factor:.2f}")
class AdvancedAssociativeMemory:
    def __init__(self, max_associations: int = 100000):
        self.max_associations = max_associations
        self.associations: Dict[int, Dict[int, float]] = {}
        
        # Hebbian learning parameters
        self.hebbian_learning_rate: float = 0.01
        self.stdp_learning_rate: float = 0.01
        
        # Forgetting mechanisms
        self.forgetting_rate: float = 0.001
        self.sparse_forgetting_threshold: float = 0.01
        
        # Pattern completion
        self.pattern_completion_threshold: float = 0.7
        self.max_patterns_per_neuron: int = 50
        
        # Memory consolidation
        self.consolidation_threshold: float = 0.5
        self.long_term_associations: Dict[int, Dict[int, float]] = {}
        
        # Statistics
        self.total_associations_created: int = 0
        self.total_associations_consolidated: int = 0
        self.average_association_strength: float = 0.0
    
    def associate(self, neuron_a: int, neuron_b: int, initial_strength: float = 0.0) -> None:
        """Create or strengthen association between two neurons"""
        # Ensure both neurons have association dictionaries
        if neuron_a not in self.associations:
            self.associations[neuron_a] = {}
        if neuron_b not in self.associations:
            self.associations[neuron_b] = {}
        
        # Get current strength
        current_strength = self.associations[neuron_a].get(neuron_b, 0.0)
        
        # Update strength
        new_strength = current_strength + initial_strength
        self.associations[neuron_a][neuron_b] = new_strength
        self.associations[neuron_b][neuron_a] = new_strength
        
        # Track statistics
        self.total_associations_created += 1
        self._update_average_strength()
        
        # Check for consolidation
        if new_strength >= self.consolidation_threshold:
            self._consolidate_association(neuron_a, neuron_b, new_strength)
    
    def update_association(self, neuron_a: int, neuron_b: int, delta: float) -> None:
        """Update association strength"""
        if neuron_a not in self.associations or neuron_b not in self.associations[neuron_a]:
            return
        
        # Apply Hebbian learning
        new_strength = self.associations[neuron_a][neuron_b] + delta * self.hebbian_learning_rate
        
        # Apply constraints
        new_strength = max(0.0, min(1.0, new_strength))
        
        # Update both directions
        self.associations[neuron_a][neuron_b] = new_strength
        self.associations[neuron_b][neuron_a] = new_strength
        
        # Apply forgetting
        self._apply_forgetting()
    
    def get_associations(self, neuron: int) -> List[Tuple[int, float]]:
        """Get all associations for a neuron, sorted by strength"""
        if neuron not in self.associations:
            return []
        
        associations = [(other_neuron, strength) 
                       for other_neuron, strength in self.associations[neuron].items()]
        associations.sort(key=lambda x: x[1], reverse=True)
        
        return associations
    
    def get_association_strength(self, neuron_a: int, neuron_b: int) -> float:
        """Get association strength between two neurons"""
        if neuron_a not in self.associations:
            return 0.0
        return self.associations[neuron_a].get(neuron_b, 0.0)
    
    def pattern_completion(self, partial_pattern: List[int]) -> List[int]:
        """Complete partial pattern based on associations"""
        if not partial_pattern:
            return []
        
        completed_pattern = []
        visited = set()
        
        # Start from first neuron in pattern
        current_neuron = partial_pattern[0]
        completed_pattern.append(current_neuron)
        visited.add(current_neuron)
        
        # Expand pattern using strongest associations
        for _ in range(len(partial_pattern) - 1):
            # Get strongest association from current neuron
            associations = self.get_associations(current_neuron)
            
            if not associations:
                break
            
            # Find neuron that matches expected pattern (if we have more neurons)
            next_neuron = None
            expected_neuron = partial_pattern[len(completed_pattern)] if len(completed_pattern) < len(partial_pattern) else None
            
            if expected_neuron is not None:
                # Look for specific neuron
                for assoc_neuron, strength in associations:
                    if assoc_neuron == expected_neuron and assoc_neuron not in visited:
                        next_neuron = assoc_neuron
                        break
            else:
                # Take strongest association
                next_neuron = associations[0][0]
            
            if next_neuron is None or next_neuron in visited:
                break
            
            completed_pattern.append(next_neuron)
            visited.add(next_neuron)
            current_neuron = next_neuron
        
        return completed_pattern
    
    def consolidate_low_strength_associations(self) -> int:
        """Consolidate low-strength associations to long-term storage"""
        consolidated = 0
        
        for neuron_a, associations in list(self.associations.items()):
            to_remove = []
            
            for neuron_b, strength in associations.items():
                if strength < self.consolidation_threshold:
                    # Move to long-term storage
                    if neuron_a not in self.long_term_associations:
                        self.long_term_associations[neuron_a] = {}
                    if neuron_b not in self.long_term_associations:
                        self.long_term_associations[neuron_b] = {}
                    
                    self.long_term_associations[neuron_a][neuron_b] = strength
                    self.long_term_associations[neuron_b][neuron_a] = strength
                    
                    to_remove.append(neuron_b)
                    consolidated += 1
            
            # Remove from short-term storage
            for neuron_b in to_remove:
                associations.pop(neuron_b, None)
        
        self.total_associations_consolidated += consolidated
        return consolidated
    
    def _consolidate_association(self, neuron_a: int, neuron_b: int, strength: float) -> None:
        """Consolidate association to long-term storage"""
        # Move to long-term storage
        if neuron_a not in self.long_term_associations:
            self.long_term_associations[neuron_a] = {}
        if neuron_b not in self.long_term_associations:
            self.long_term_associations[neuron_b] = {}
        
        self.long_term_associations[neuron_a][neuron_b] = strength
        self.long_term_associations[neuron_b][neuron_a] = strength
        
        # Remove from short-term if strength is very high
        if neuron_a in self.associations and neuron_b in self.associations[neuron_a]:
            if strength > 0.8:
                self.associations[neuron_a].pop(neuron_b, None)
                self.associations[neuron_b].pop(neuron_a, None)
    
    def _apply_forgetting(self) -> None:
        """Apply forgetting mechanism to weak associations"""
        # Decay all associations
        for neuron_a, associations in self.associations.items():
            for neuron_b in list(associations.keys()):
                current_strength = associations[neuron_b]
                
                # Apply forgetting based on sparsity
                if len(associations) > self.max_patterns_per_neuron:
                    # Sparse forgetting
                    if current_strength < self.sparse_forgetting_threshold:
                        associations.pop(neuron_b, None)
                else:
                    # Standard exponential decay
                    new_strength = current_strength * (1.0 - self.forgetting_rate)
                    associations[neuron_b] = new_strength
                    
                    # Remove if strength becomes too low
                    if new_strength < 0.001:
                        associations.pop(neuron_b, None)
        
        self._update_average_strength()
    
    def _update_average_strength(self) -> None:
        """Update average association strength"""
        total_strength = 0.0
        total_pairs = 0
        
        for neuron_a, associations in self.associations.items():
            for neuron_b, strength in associations.items():
                total_strength += strength
                total_pairs += 1
        
        self.average_association_strength = total_strength / max(total_pairs, 1)
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get memory system statistics"""
        total_associations = sum(len(associations) for associations in self.associations.values())
        total_long_term = sum(len(associations) for associations in self.long_term_associations.values())
        
        return {
            'short_term_associations': total_associations,
            'long_term_associations': total_long_term,
            'total_associations': total_associations + total_long_term,
            'average_association_strength': self.average_association_strength,
            'consolidation_rate': self.total_associations_consolidated / max(self.total_associations_created, 1),
            'capacity_usage': total_associations / self.max_associations
        }
    
    def clear(self) -> None:
        """Clear all associations"""
        self.associations.clear()
        self.long_term_associations.clear()
        self.total_associations_created = 0
        self.total_associations_consolidated = 0
        self.average_association_strength = 0.0
class IntegratedMemorySystem:
    def __init__(self, config: Dict[str, Any] = None):
        self.config = config or {}
        
        # Initialize memory subsystems
        self.working_memory = AdvancedWorkingMemory(
            capacity=self.config.get('working_memory_capacity', 1000),
            decay_rate=self.config.get('working_memory_decay', 0.01)
        )
        
        self.episodic_memory = EpisodicMemoryManager(
            max_episodes=self.config.get('max_episodes', 10000),
            consolidation_threshold=self.config.get('consolidation_threshold', 0.5)
        )
        
        self.associative_memory = AdvancedAssociativeMemory(
            max_associations=self.config.get('max_associations', 100000)
        )
        
        # Integration statistics
        self.memory_usage_history: List[Dict[str, float]] = []
        self.performance_metrics: Dict[str, Any] = {}
        
        # Cross-system integration
        self.neural_activity_cache: Dict[int, float] = {}
        self.memory_integration_enabled: bool = True
        
        # Learning control
        self.consolidation_scheduler = ConsolidationScheduler()
        self.replay_scheduler = ReplayScheduler()
    
    def store_experience(self, neurons: List[int], activations: List[float], 
                         timestamp: float, metadata: str = "") -> Dict[str, Any]:
        """Store complete learning experience across all memory systems"""
        results = {}
        
        # Store in working memory (fast, temporary)
        for neuron_id, activation in zip(neurons, activations):
            self.working_memory.store(neuron_id, activation)
        
        results['working_memory'] = {
            'neurons_stored': len(neurons),
            'total_activity': sum(activations)
        }
        
        # Store in episodic memory (event-based)
        episode_index = self.episodic_memory.store_episode(
            neurons, activations, timestamp, metadata
        )
        
        results['episodic_memory'] = {
            'episode_index': episode_index,
            'neuron_count': len(neurons),
            'activity_strength': sum(activations) / len(activations) if activations else 0.0
        }
        
        # Store in associative memory (relationships)
        for i, neuron_a in enumerate(neurons):
            for neuron_b in neurons[i+1:]:
                activation_product = activations[i] * activations[i+1]
                self.associative_memory.associate(neuron_a, neuron_b, activation_product)
        
        results['associative_memory'] = {
            'associations_created': len(neurons) * (len(neurons) - 1) // 2,
            'average_association_strength': self.associative_memory.average_association_strength
        }
        
        # Update performance metrics
        self._update_performance_metrics()
        
        return results
    
    def retrieve_experience(self, retrieval_type: str, **kwargs) -> Dict[str, Any]:
        """Retrieve experience from memory systems"""
        results = {}
        
        if retrieval_type == 'working_memory':
            neurons = kwargs.get('neurons', [])
            activations = [self.working_memory.retrieve(n) for n in neurons]
            results['working_memory'] = {
                'neurons': neurons,
                'activations': activations,
                'total_activity': sum(activations)
            }
        
        elif retrieval_type == 'episodic_memory':
            time_range = kwargs.get('time_range', (0, float('inf')))
            min_activity = kwargs.get('min_activity', 0.0)
            
            episodes = self.episodic_memory.get_recent_episodes(1000)
            filtered_episodes = []
            
            for episode in episodes:
                if (episode.timestamp >= time_range[0] and 
                    episode.timestamp <= time_range[1] and
                    sum(episode.values) / len(episode.values) >= min_activity):
                    filtered_episodes.append(episode)
            
            results['episodic_memory'] = {
                'episodes': filtered_episodes,
                'count': len(filtered_episodes)
            }
        
        elif retrieval_type == 'pattern_completion':
            partial_pattern = kwargs.get('pattern', [])
            completed = self.associative_memory.pattern_completion(partial_pattern)
            results['pattern_completion'] = {
                'input_pattern': partial_pattern,
                'completed_pattern': completed,
                'similarity': len(completed) / len(partial_pattern) if partial_pattern else 0.0
            }
        
        return results
    
    def consolidate_memories(self, consolidation_type: str = 'automatic') -> Dict[str, Any]:
        """Consolidate memories across systems"""
        results = {}
        
        if consolidation_type == 'automatic':
            # Automatic consolidation based on usage patterns
            results['episodic_consolidation'] = self.episodic_memory.consolidate(
                self.config.get('consolidation_threshold', 0.5)
            )
            
            results['working_memory_cleanup'] = self._cleanup_working_memory()
            
            results['associative_consolidation'] = self.associative_memory.consolidate_low_strength_associations()
        
        elif consolidation_type == 'replay':
            # Replay important memories
            episodes_for_replay = self.episodic_memory.get_episodes_for_replay(
                self.config.get('replay_count', 10),
                time.time()
            )
            
            for episode in episodes_for_replay:
                self.episodic_memory.replay_episode(episode)
            
            results['replay'] = {
                'episodes_replayed': len(episodes_for_replay),
                'replay_queue_size': len(self.episodic_memory.replay_queue)
            }
        
        # Update performance metrics
        self._update_performance_metrics()
        
        return results
    
    def update_neural_activity(self, neuron_id: int, activity: float, timestamp: float) -> None:
        """Update neural activity and integrate with memory systems"""
        # Store in cache
        self.neural_activity_cache[neuron_id] = activity
        
        # Update working memory
        self.working_memory.store(neuron_id, activity)
        
        # Trigger consolidation if needed
        if self.memory_integration_enabled:
            self.consolidation_scheduler.schedule_consolidation(timestamp)
    
    def get_system_state(self) -> Dict[str, Any]:
        """Get complete system state"""
        return {
            'working_memory': {
                'capacity': self.working_memory.capacity,
                'current_size': self.working_memory.get_capacity_usage() * 100,
                'total_activity': self.working_memory.get_total_activity()
            },
            'episodic_memory': {
                'episode_count': self.episodic_memory.get_episode_count(),
                'total_stored': self.episodic_memory.get_total_stored_episodes(),
                'consolidation_count': self.episodic_memory.consolidation_count
            },
            'associative_memory': self.associative_memory.get_statistics(),
            'performance': self.performance_metrics,
            'neural_activity_cache': len(self.neural_activity_cache)
        }
    
    def clear(self) -> None:
        """Clear all memory"""
        self.working_memory.clear()
        self.episodic_memory.clear()
        self.associative_memory.clear()
        self.neural_activity_cache.clear()
        self.memory_usage_history.clear()
        self.performance_metrics.clear()
    
    def _update_performance_metrics(self) -> None:
        """Update performance metrics"""
        current_time = time.time()
        
        state = self.get_system_state()
        
        # Calculate performance metrics
        wm_efficiency = min(1.0, state['working_memory']['current_size'] / 0.8)  # Optimal at 80% capacity
        em_storage_efficiency = state['episodic_memory']['episode_count'] / max(state['episodic_memory']['total_stored'], 1)
        am_connectivity = state['associative_memory']['capacity_usage']
        
        # Calculate overall system performance
        performance = (wm_efficiency * 0.3 + em_storage_efficiency * 0.3 + am_connectivity * 0.4)
        
        self.performance_metrics = {
            'timestamp': current_time,
            'working_memory_efficiency': wm_efficiency,
            'episodic_memory_efficiency': em_storage_efficiency,
            'associative_memory_connectivity': am_connectivity,
            'overall_performance': performance,
            'memory_usage_mb': sum(state['working_memory']['total_activity'] for _ in range(10))  # Simplified
        }
        
        # Store in history
        self.memory_usage_history.append({
            'timestamp': current_time,
            'working_memory_size': state['working_memory']['current_size'],
            'episodic_memory_count': state['episodic_memory']['episode_count'],
            'associative_memory_pairs': state['associative_memory']['total_associations'],
            'performance': performance
        })
        
        # Keep only recent history
        if len(self.memory_usage_history) > 1000:
            self.memory_usage_history = self.memory_usage_history[-1000:]
    
    def _cleanup_working_memory(self) -> int:
        """Clean up working memory by removing low-priority entries"""
        active_neurons = self.working_memory.get_active_neurons(0.01)
        total_before = len(self.working_memory.entries)
        
        # Keep only active neurons
        for neuron_id in list(self.working_memory.entries.keys()):
            if neuron_id not in active_neurons:
                # Reduce activation for non-active neurons
                self.working_memory.entries[neuron_id].activation *= 0.1
                
                # Remove if activation becomes too low
                if self.working_memory.entries[neuron_id].activation < 0.001:
                    del self.working_memory.entries[neuron_id]
        
        return total_before - len(self.working_memory.entries)
class ConsolidationScheduler:
    def __init__(self):
        self.consolidation_queue: List[Dict[str, Any]] = []
        self.last_consolidation: float = 0.0
        self.consolidation_interval: float = 60.0  # seconds
        self.priority_scores: Dict[str, float] = {}
    
    def schedule_consolidation(self, timestamp: float) -> None:
        """Schedule consolidation based on activity patterns"""
        # Simple time-based scheduling
        if timestamp - self.last_consolidation > self.consolidation_interval:
            self.consolidate()
        
        # Check for activity-based triggers
        # This would integrate with the brain's activity monitoring
        pass
    
    def consolidate(self) -> Dict[str, Any]:
        """Perform consolidation"""
        if not self.consolidation_queue:
            return {'consolidated': 0, 'reason': 'no_queue'}
        
        # Process queue
        items_processed = 0
        for item in self.consolidation_queue:
            # Process consolidation item
            items_processed += 1
        
        self.consolidation_queue.clear()
        self.last_consolidation = time.time()
        
        return {
            'consolidated': items_processed,
            'reason': 'scheduled',
            'timestamp': self.last_consolidation
        }
class ReplayScheduler:
    def __init__(self):
        self.replay_queue: List[Dict[str, Any]] = []
        self.last_replay: float = 0.0
        self.replay_interval: float = 300.0  # 5 minutes
        self.importance_threshold: float = 0.5
    
    def schedule_replay(self, episode_data: Dict[str, Any]) -> None:
        """Schedule episode for replay"""
        # Calculate episode importance
        importance = self._calculate_importance(episode_data)
        
        if importance >= self.importance_threshold:
            self.replay_queue.append({
                'episode': episode_data,
                'importance': importance,
                'scheduled_time': time.time() + self.replay_interval
            })
    
    def get_replay_queue(self) -> List[Dict[str, Any]]:
        """Get current replay queue"""
        return self.replay_queue
    
    def process_replay_queue(self) -> List[Dict[str, Any]]:
        """Process replay queue and return episodes to replay"""
        current_time = time.time()
        episodes_to_replay = []
        
        for item in self.replay_queue[:]:
            if current_time >= item['scheduled_time']:
                episodes_to_replay.append(item['episode'])
                self.replay_queue.remove(item)
        
        return episodes_to_replay
    
    def _calculate_importance(self, episode_data: Dict[str, Any]) -> float:
        """Calculate importance of episode for replay"""
        # Simplified importance calculation
        neurons = episode_data.get('neurons', [])
        activations = episode_data.get('activations', [])
        
        if not neurons or not activations:
            return 0.0
        
        # Calculate activation strength
        avg_activation = sum(activations) / len(activations)
        
        # Calculate novelty (simplified: recent episodes are more novel)
        time_since = episode_data.get('time_since', 0)
        novelty = math.exp(-time_since / 100.0)
        
        # Calculate reward prediction error (simplified)
        reward = episode_data.get('reward', 0)
        rpe = abs(reward)
        
        # Combined importance score
        importance = (avg_activation * 0.4 + novelty * 0.3 + rpe * 0.3)
        
        return min(1.0, importance)
# Main execution
def main():
    # Initialize integrated memory system
    config = {
        'working_memory_capacity': 2000,
        'working_memory_decay': 0.02,
        'max_episodes': 50000,
        'consolidation_threshold': 0.3,
        'max_associations': 200000
    }
    
    memory_system = IntegratedMemorySystem(config)
    
    print("Enhanced Memory Systems for NLM")
    print("=" * 50)
    
    # Simulate some learning experience
    print("\n1. Storing learning experience...")
    experience_result = memory_system.store_experience(
        neurons=[1, 2, 3, 4, 5],
        activations=[0.8, 0.6, 0.9, 0.3, 0.7],
        timestamp=time.time(),
        metadata="Exploration of neural patterns"
    )
    
    print(f"   Working memory: {experience_result['working_memory']}")
    print(f"   Episodic memory: {experience_result['episodic_memory']}")
    print(f"   Associative memory: {experience_result['associative_memory']}")
    
    # Retrieve experience
    print("\n2. Retrieving experience...")
    retrieval_result = memory_system.retrieve_experience(
        'working_memory',
        neurons=[1, 2, 3]
    )
    print(f"   Retrieved working memory: {retrieval_result['working_memory']}")
    
    # Pattern completion
    print("\n3. Pattern completion...")
    completion_result = memory_system.retrieve_experience(
        'pattern_completion',
        pattern=[1, 2]
    )
    print(f"   Pattern completion: {completion_result['pattern_completion']}")
    
    # Consolidate memories
    print("\n4. Consolidating memories...")
    consolidation_result = memory_system.consolidate_memories('automatic')
    print(f"   Consolidation result: {consolidation_result}")
    
    # Get system state
    print("\n5. System state...")
    system_state = memory_system.get_system_state()
    print(f"   Working memory capacity: {system_state['working_memory']['current_size']:.1f}%")
    print(f"   Episodic memory episodes: {system_state['episodic_memory']['episode_count']}")
    print(f"   Associative memory connections: {system_state['associative_memory']['total_associations']}")
    print(f"   Overall performance: {system_state['performance']['overall_performance']:.3f}")
    
    # Update neural activity
    print("\n6. Updating neural activity...")
    for neuron_id in [1, 2, 3, 4, 5]:
        activity = np.random.random() * 0.5 + 0.5  # [0.5, 1.0]
        memory_system.update_neural_activity(neuron_id, activity, time.time())
    
    # Final state
    print("\n7. Final system state...")
    final_state = memory_system.get_system_state()
    print(f"   Final performance: {final_state['performance']['overall_performance']:.3f}")
    
    # Cleanup
    memory_system.clear()
    print("\n8. System cleared.")
    
    print("\n" + "=" * 50)
    print("Enhanced Memory Systems completed successfully!")
    return 0
if __name__ == '__main__':
    exit(main())
