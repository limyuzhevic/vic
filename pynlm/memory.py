from dataclasses import dataclass, field
"""
Memory Systems - Working, Episodic, and Associative Memory
===========================================================

Implementation of multiple memory systems for neural simulation:
- Working memory: temporary activation storage and competition
- Episodic memory: experience recording and replay
- Associative memory: pattern associations and retrieval
"""

from typing import List, Dict, Any, Optional
import numpy as np
@dataclass
class MemoryTrace:
    """Memory trace for working memory."""
    
    neuron_id: int
    activation: float
    timestamp: float
    decay_rate: float = 0.01
    stability: float = 1.0
    
    def decay(self, dt: float):
        """Apply decay to activation."""
        self.activation *= (1.0 - self.decay_rate * dt)
        self.stability *= 0.99
        
    def consolidate(self):
        """Consolidate into episodic memory."""
        if self.activation > 0.5 and self.stability > 0.8:
            return True
        return False
@dataclass
class EpisodicMemoryItem:
    """Episodic memory item (experience record)."""
    
    episode_id: int
    timestamp: float
    sensory_input: List[float]
    action_taken: Dict[str, Any]
    reward: float
    neural_activation_pattern: List[float]
    replay_count: int = 0
    importance: float = 1.0
    
    def calculate_importance(self):
        """Calculate episode importance based on reward and novelty."""
        base_importance = abs(self.reward) + 0.1
        self.importance = min(1.0, base_importance)
class WorkingMemory:
    """
    Working memory system for temporary activation storage.
    
    Implements competition-based dynamics with:
    - Trace decay and stabilization
    - Capacity limits
    - Winner-take-all competition
    - Memory consolidation
    
    Attributes:
        capacity (int): Maximum number of traces
        decay_rate (float): Trace decay rate
        competition_strength (float): Competition strength
    """
    
    def __init__(self, capacity: int = 100, decay_rate: float = 0.01,
                 competition_strength: float = 1.0):
        """
        Initialize working memory.
        
        Args:
            capacity (int): Maximum number of traces
            decay_rate (float): Trace decay rate per timestep
            competition_strength (float): Competition strength
        """
        self.capacity = capacity
        self.decay_rate = decay_rate
        self.competition_strength = competition_strength
        
        # Memory traces
        self.traces: Dict[int, MemoryTrace] = {}
        
        # Competition dynamics
        self.global_modulation = 1.0
        
    def store(self, neuron_id: int, activation: float, timestamp: float):
        """
        Store activation in working memory.
        
        Args:
            neuron_id (int): Neuron identifier
            activation (float): Activation level
            timestamp (float): Timestamp
        """
        # Apply global modulation
        activation *= self.global_modulation
        
        if neuron_id in self.traces:
            # Update existing trace
            self.traces[neuron_id].activation = activation
            self.traces[neuron_id].timestamp = timestamp
            self.traces[neuron_id].stability = min(1.0, 
                                                 self.traces[neuron_id].stability + 0.1)
        else:
            # Create new trace
            if len(self.traces) >= self.capacity:
                # Remove weakest trace
                weakest_id = min(self.traces.keys(),
                               key=lambda k: self.traces[k].activation)
                del self.traces[weakest_id]
                
            self.traces[neuron_id] = MemoryTrace(
                neuron_id=neuron_id,
                activation=activation,
                timestamp=timestamp,
                decay_rate=self.decay_rate
            )
            
    def update(self, dt: float):
        """
        Update working memory state.
        
        Args:
            dt (float): Time step duration
        """
        # Apply decay to all traces
        for trace in self.traces.values():
            trace.decay(dt)
            
        # Apply competition
        self._apply_competition()
        
    def _apply_competition(self):
        """Apply winner-take-all competition."""
        if len(self.traces) < 2:
            return
            
        # Find strongest trace
        strongest_id = max(self.traces.keys(),
                          key=lambda k: self.traces[k].activation)
        strongest_activation = self.traces[strongest_id].activation
        
        # Suppress weaker traces
        for trace_id, trace in self.traces.items():
            if trace_id != strongest_id:
                suppression = self.competition_strength * (1.0 - trace.activation / strongest_activation)
                trace.activation *= (1.0 - suppression)
                
    def get_active_traces(self, threshold: float = 0.1) -> List[int]:
        """
        Get list of active trace neuron IDs.
        
        Args:
            threshold (float): Activation threshold
            
        Returns:
            List[int]: List of active neuron IDs
        """
        return [tid for tid, trace in self.traces.items() 
                if trace.activation > threshold]
                
    def get_trace_strength(self, neuron_id: int) -> float:
        """
        Get activation strength for a neuron.
        
        Args:
            neuron_id (int): Neuron identifier
            
        Returns:
            float: Activation strength (0-1)
        """
        if neuron_id in self.traces:
            return self.traces[neuron_id].activation
        return 0.0
        
    def consolidate(self, threshold: float = 0.5) -> List[EpisodicMemoryItem]:
        """
        Consolidate stable traces into episodic memory.
        
        Args:
            threshold (float): Consolidation threshold
            
        Returns:
            List[EpisodicMemoryItem]: New episodic memory items
        """
        new_items = []
        
        for trace_id, trace in list(self.traces.items()):
            if trace.consolidate() and trace.activation > threshold:
                item = EpisodicMemoryItem(
                    episode_id=len(new_items),
                    timestamp=trace.timestamp,
                    sensory_input=[],  # Would be filled from simulation
                    action_taken={},   # Would be filled from simulation
                    reward=0.0,        # Would be filled from simulation
                    neural_activation_pattern=[trace.activation]
                )
                new_items.append(item)
                
        # Clear consolidated traces
        for item in new_items:
            if item.episode_id in self.traces:
                del self.traces[item.episode_id]
                
        return new_items
        
    def clear(self):
        """Clear all working memory traces."""
        self.traces.clear()
        
    def get_state(self) -> Dict[str, Any]:
        """
        Get working memory state.
        
        Returns:
            Dict[str, Any]: Memory state
        """
        return {
            "traces": {
                tid: {
                    "neuron_id": trace.neuron_id,
                    "activation": trace.activation,
                    "timestamp": trace.timestamp,
                    "decay_rate": trace.decay_rate,
                    "stability": trace.stability
                }
                for tid, trace in self.traces.items()
            },
            "capacity": self.capacity,
            "decay_rate": self.decay_rate,
            "competition_strength": self.competition_strength,
            "trace_count": len(self.traces)
        }
        
    def set_state(self, state: Dict[str, Any]):
        """
        Set working memory state.
        
        Args:
            state (Dict[str, Any]): Memory state
        """
        self.traces.clear()
        
        for trace_data in state["traces"].values():
            trace = MemoryTrace(
                neuron_id=trace_data["neuron_id"],
                activation=trace_data["activation"],
                timestamp=trace_data["timestamp"],
                decay_rate=trace_data["decay_rate"],
                stability=trace_data["stability"]
            )
            self.traces[trace.neuron_id] = trace
            
        self.capacity = state.get("capacity", self.capacity)
        self.decay_rate = state.get("decay_rate", self.decay_rate)
        self.competition_strength = state.get("competition_strength", 
                                             self.competition_strength)
class EpisodicMemory:
    """
    Episodic memory system for experience recording and replay.
    
    Stores and replays complete experience sequences with:
    - Temporal ordering of events
    - Pattern completion during replay
    - Conserved memory integration
    - Importance-based prioritization
    
    Attributes:
        capacity (int): Maximum number of episodes
        replay_interval (int): Replay interval in steps
        consolidation_threshold (float): Threshold for consolidation
    """
    
    def __init__(self, capacity: int = 1000, replay_interval: int = 100,
                 consolidation_threshold: float = 0.5):
        """
        Initialize episodic memory.
        
        Args:
            capacity (int): Maximum number of episodes
            replay_interval (int): Steps between replays
            consolidation_threshold (float): Threshold for consolidation
        """
        self.capacity = capacity
        self.replay_interval = replay_interval
        self.consolidation_threshold = consolidation_threshold
        
        # Episodic memory items
        self.episodes: Dict[int, EpisodicMemoryItem] = {}
        
        # Replay dynamics
        self.replay_enabled = True
        self.next_replay_step = 0
        
    def store(self, sensory_input: List[float], action_taken: Dict[str, Any],
              reward: float, neural_pattern: List[float], timestamp: float):
        """
        Store new episode.
        
        Args:
            sensory_input (List[float]): Sensory input pattern
            action_taken (Dict[str, Any]): Action taken
            reward (float): Reward received
            neural_pattern (List[float]): Neural activation pattern
            timestamp (float): Episode timestamp
        """
        episode_id = len(self.episodes)
        
        if len(self.episodes) >= self.capacity:
            # Remove oldest episode
            oldest_id = min(self.episodes.keys())
            del self.episodes[oldest_id]
            
        episode = EpisodicMemoryItem(
            episode_id=episode_id,
            timestamp=timestamp,
            sensory_input=sensory_input,
            action_taken=action_taken,
            reward=reward,
            neural_activation_pattern=neural_pattern
        )
        
        self.episodes[episode_id] = episode
        
    def update(self, dt: float):
        """
        Update episodic memory state.
n        
        Args:
            dt (float): Time step duration
        """
        # Increment replay counter for important episodes
        for episode in self.episodes.values():
            if episode.importance > 0.7:
                episode.replay_count += 1
                
    def replay(self, steps: int = None) -> List[Dict[str, Any]]:
        """
        Replay recent episodes.
        
        Args:
            steps (int, optional): Number of recent episodes to replay
                
        Returns:
            List[Dict[str, Any]]: Replay results
        """
        if not self.replay_enabled:
            return []
            
        results = []
        episodes_to_replay = list(self.episodes.values())
        
        if steps is not None and len(episodes_to_replay) > steps:
            episodes_to_replay = sorted(episodes_to_replay, 
                                       key=lambda x: x.timestamp, reverse=True)[:steps]
                                       
        for episode in episodes_to_replay:
            # Simulate replay by applying neural pattern
            replay_result = {
                "episode_id": episode.episode_id,
                "timestamp": episode.timestamp,
                "reward": episode.reward,
                "importance": episode.importance,
                "replay_count": episode.replay_count
            }
            results.append(replay_result)
            
        return results
        
    def get_episode_count(self) -> int:
        """Get number of episodes."""
        return len(self.episodes)
        
    def get_episodes_by_time_range(self, start_time: float, 
                                   end_time: float) -> List[EpisodicMemoryItem]:
        """
        Get episodes within time range.
        
        Args:
            start_time (float): Start time
            end_time (float): End time
            
        Returns:
            List[EpisodicMemoryItem]: Episodes in time range
        """
        return [ep for ep in self.episodes.values()
                if start_time <= ep.timestamp <= end_time]
                
    def clear(self):
        """Clear all episodic memory."""
        self.episodes.clear()
        
    def get_state(self) -> Dict[str, Any]:
        """
        Get episodic memory state.
        
        Returns:
            Dict[str, Any]: Memory state
        """
        return {
            "episodes": {
                eid: {
                    "episode_id": episode.episode_id,
                    "timestamp": episode.timestamp,
                    "sensory_input": episode.sensory_input,
                    "action_taken": episode.action_taken,
                    "reward": episode.reward,
                    "neural_activation_pattern": episode.neural_activation_pattern,
                    "replay_count": episode.replay_count,
                    "importance": episode.importance
                }
                for eid, episode in self.episodes.items()
            },
            "capacity": self.capacity,
            "replay_interval": self.replay_interval,
            "consolidation_threshold": self.consolidation_threshold,
            "episode_count": len(self.episodes)
        }
        
    def set_state(self, state: Dict[str, Any]):
        """
        Set episodic memory state.
        
        Args:
            state (Dict[str, Any]): Memory state
        """
        self.episodes.clear()
        
        for episode_data in state["episodes"].values():
            episode = EpisodicMemoryItem(
                episode_id=episode_data["episode_id"],
                timestamp=episode_data["timestamp"],
                sensory_input=episode_data["sensory_input"],
                action_taken=episode_data["action_taken"],
                reward=episode_data["reward"],
                neural_activation_pattern=episode_data["neural_activation_pattern"],
                replay_count=episode_data["replay_count"],
                importance=episode_data["importance"]
            )
            self.episodes[episode.episode_id] = episode
            
        self.capacity = state.get("capacity", self.capacity)
        self.replay_interval = state.get("replay_interval", self.replay_interval)
        self.consolidation_threshold = state.get("consolidation_threshold", 
                                               self.consolidation_threshold)
class AssociativeMemory:
    """
    Associative memory system for pattern associations.
    
    Implements pattern completion and association strength learning:
    - Hebbian-like association formation
    - Pattern completion from partial cues
    - Association strength tracking
    - Memory interference management
    
    Attributes:
        learning_rate (float): Association learning rate
        pattern_dimension (int): Pattern dimensionality
        similarity_threshold (float): Similarity threshold
    """
    
    def __init__(self, learning_rate: float = 0.1, 
                 pattern_dimension: int = 100,
                 similarity_threshold: float = 0.7):
        """
        Initialize associative memory.
        
        Args:
            learning_rate (float): Association learning rate
            pattern_dimension (int): Pattern dimensionality
            similarity_threshold (float): Similarity threshold
        """
        self.learning_rate = learning_rate
        self.pattern_dimension = pattern_dimension
        self.similarity_threshold = similarity_threshold
        
        # Association matrix: key1 -> {key2: strength}
        self.associations: Dict[int, Dict[int, float]] = {}
        
        # Pattern store: key -> pattern
        self.patterns: Dict[int, List[float]] = {}
        
    def associate(self, pattern1_key: int, pattern2_key: int, 
                  strength: float = 1.0):
        """
        Form association between two patterns.
        
        Args:
            pattern1_key (int): First pattern key
            pattern2_key (int): Second pattern key
            strength (float): Association strength
        """
        if pattern1_key not in self.associations:
            self.associations[pattern1_key] = {}
        if pattern2_key not in self.associations:
            self.associations[pattern2_key] = {}
            
        # Update bidirectional association
        self.associations[pattern1_key][pattern2_key] = \
            self.associations[pattern1_key].get(pattern2_key, 0.0) + strength
        self.associations[pattern2_key][pattern1_key] = \
            self.associations[pattern2_key].get(pattern1_key, 0.0) + strength
            
        # Store patterns if not already stored
        if pattern1_key not in self.patterns:
            self.patterns[pattern1_key] = [0.0] * self.pattern_dimension
        if pattern2_key not in self.patterns:
            self.patterns[pattern2_key] = [0.0] * self.pattern_dimension
            
    def store_pattern(self, key: int, pattern: List[float]):
        """
        Store pattern for association.
        
        Args:
            key (int): Pattern key
            pattern (List[float]): Pattern vector
        """
        if len(pattern) != self.pattern_dimension:
            raise ValueError(f"Pattern dimension mismatch: expected {self.pattern_dimension}, got {len(pattern)}")
            
        self.patterns[key] = pattern.copy()
        
    def retrieve(self, pattern: List[float], top_k: int = 5) -> List[int]:
        """
        Retrieve associated patterns using pattern completion.
        
        Args:
            pattern (List[float]): Input pattern
            top_k (int): Number of top results to return
            
        Returns:
            List[int]: List of associated pattern keys
        """
        if len(pattern) != self.pattern_dimension:
            raise ValueError(f"Pattern dimension mismatch: expected {self.pattern_dimension}, got {len(pattern)}")
            
        similarities = []
        
        for pattern_key, stored_pattern in self.patterns.items():
            similarity = self._pattern_similarity(pattern, stored_pattern)
            if similarity >= self.similarity_threshold:
                similarities.append((pattern_key, similarity))
                
        # Sort by similarity (descending)
        similarities.sort(key=lambda x: x[1], reverse=True)
        
        return [key for key, _ in similarities[:top_k]]
        
    def get_association_strength(self, pattern1_key: int, 
                                pattern2_key: int) -> float:
        """
        Get association strength between patterns.
        
        Args:
            pattern1_key (int): First pattern key
            pattern2_key (int): Second pattern key
            
        Returns:
            float: Association strength
        """
        return self.associations.get(pattern1_key, {}).get(pattern2_key, 0.0)
        
    def clear(self):
        """Clear all associations."""
        self.associations.clear()
        self.patterns.clear()
        
    def _pattern_similarity(self, pattern1: List[float], 
                           pattern2: List[float]) -> float:
        """
        Calculate similarity between two patterns.
        
        Args:
            pattern1 (List[float]): First pattern
            pattern2 (List[float]): Second pattern
            
        Returns:
            float: Similarity (0-1)
        """
        if len(pattern1) != len(pattern2):
            return 0.0
            
        # Cosine similarity
        dot_product = sum(a * b for a, b in zip(pattern1, pattern2))
        norm1 = math.sqrt(sum(a * a for a in pattern1))
        norm2 = math.sqrt(sum(a * a for a in pattern2))
        
        if norm1 == 0 or norm2 == 0:
            return 0.0
            
        return dot_product / (norm1 * norm2)
        
    def get_state(self) -> Dict[str, Any]:
        """
        Get associative memory state.
        
        Returns:
            Dict[str, Any]: Memory state
        """
        return {
            "associations": self.associations,
            "patterns": self.patterns,
            "learning_rate": self.learning_rate,
            "pattern_dimension": self.pattern_dimension,
            "similarity_threshold": self.similarity_threshold,
            "pattern_count": len(self.patterns),
            "association_count": sum(len(assoc) for assoc in self.associations.values())
        }
        
    def set_state(self, state: Dict[str, Any]):
        """
        Set associative memory state.
n        
        Args:
            state (Dict[str, Any]): Memory state
        """
        self.associations = state.get("associations", {})
        self.patterns = state.get("patterns", {})
        self.learning_rate = state.get("learning_rate", self.learning_rate)
        self.pattern_dimension = state.get("pattern_dimension", self.pattern_dimension)
        self.similarity_threshold = state.get("similarity_threshold", 
                                             self.similarity_threshold)
class MemorySystem:
    """
    Complete memory system integrating all memory subsystems.
    
    Coordinates working memory, episodic memory, and associative memory
    with consolidation and integration mechanisms.
    
    Attributes:
        working_memory (WorkingMemory): Working memory subsystem
        episodic_memory (EpisodicMemory): Episodic memory subsystem
        associative_memory (AssociativeMemory): Associative memory subsystem
    """
    
    def __init__(self, capacity: int = 1000, replay_interval: int = 100):
        """
        Initialize memory system.
        
        Args:
            capacity (int): Memory capacity
            replay_interval (int): Replay interval
        """
        self.working_memory = WorkingMemory(capacity=capacity // 3)
        self.episodic_memory = EpisodicMemory(capacity=capacity // 2, 
                                             replay_interval=replay_interval)
        self.associative_memory = AssociativeMemory()
        
        self.consolidation_enabled = True
        self.next_consolidation_step = 0
        
    def initialize(self):
        """Initialize memory system."""
        self.working_memory.clear()
        self.episodic_memory.clear()
        self.associative_memory.clear()
        self.next_consolidation_step = 0
        
    def update(self, dt: float) -> Dict[str, Any]:
        """
        Update memory system.
n        
        Args:
            dt (float): Time step duration
            
        Returns:
            Dict[str, Any]: Memory statistics
        """
        # Update subsystems
        self.working_memory.update(dt)
        self.episodic_memory.update(dt)
        
        # Consolidate working memory if enabled
        if self.consolidation_enabled:
            self.next_consolidation_step += 1
            if self.next_consolidation_step >= 10:
                new_episodes = self.working_memory.consolidate()
                for episode in new_episodes:
                    self.episodic_memory.store(
                        sensory_input=episode.sensory_input,
                        action_taken=episode.action_taken,
                        reward=episode.reward,
                        neural_pattern=episode.neural_activation_pattern,
                        timestamp=episode.timestamp
                    )
                self.next_consolidation_step = 0
                
        # Get statistics
        return {
            "working_memory_usage": len(self.working_memory.get_active_traces()),
            "episodic_memory_count": self.episodic_memory.get_episode_count(),
            "working_memory_capacity": self.working_memory.capacity,
            "episodic_memory_capacity": self.episodic_memory.capacity
        }
        
    def store_working_memory(self, neuron_id: int, activation: float, 
                            timestamp: float):
        """
        Store activation in working memory.
        
        Args:
            neuron_id (int): Neuron identifier
            activation (float): Activation level
            timestamp (float): Timestamp
        """
        self.working_memory.store(neuron_id, activation, timestamp)
        
    def store_episode(self, sensory_input: List[float], action_taken: Dict[str, Any],
                     reward: float, neural_pattern: List[float], timestamp: float):
        """
        Store new episode in episodic memory.
        
        Args:
            sensory_input (List[float]): Sensory input
            action_taken (Dict[str, Any]): Action taken
            reward (float): Reward received
            neural_pattern (List[float]): Neural activation pattern
            timestamp (float): Timestamp
        """
        self.episodic_memory.store(sensory_input, action_taken, reward, 
                                   neural_pattern, timestamp)
        
    def associate(self, pattern1_key: int, pattern2_key: int, strength: float = 1.0):
        """
        Form association between patterns.
        
        Args:
            pattern1_key (int): First pattern key
            pattern2_key (int): Second pattern key
            strength (float): Association strength
        """
        self.associative_memory.associate(pattern1_key, pattern2_key, strength)
        
    def store_pattern(self, key: int, pattern: List[float]):
        """
        Store pattern for association.
        
        Args:
            key (int): Pattern key
            pattern (List[float]): Pattern vector
        """
        self.associative_memory.store_pattern(key, pattern)
        
    def retrieve_associations(self, pattern: List[float], top_k: int = 5) -> List[int]:
        """
        Retrieve associated patterns.
n        
        Args:
            pattern (List[float]): Input pattern
            top_k (int): Number of top results
            
        Returns:
            List[int]: List of associated pattern keys
        """
        return self.associative_memory.retrieve(pattern, top_k)
        
    def replay_episodes(self, steps: int = None) -> List[Dict[str, Any]]:
        """
        Replay recent episodes.
        
        Args:
            steps (int, optional): Number of recent episodes
            
        Returns:
            List[Dict[str, Any]]: Replay results
        """
        return self.episodic_memory.replay(steps)
        
    def get_episode_count(self) -> int:
        """Get number of episodes in episodic memory."""
        return self.episodic_memory.get_episode_count()
        
    def get_working_memory_usage(self) -> int:
        """Get working memory usage."""
        return len(self.working_memory.get_active_traces())
        
    def clear(self):
        """Clear all memory."""
        self.working_memory.clear()
        self.episodic_memory.clear()
        self.associative_memory.clear()
        
    def get_state(self) -> Dict[str, Any]:
        """
        Get memory system state.
        
        Returns:
            Dict[str, Any]: Memory system state
        """
        return {
            "working_memory": self.working_memory.get_state(),
            "episodic_memory": self.episodic_memory.get_state(),
            "associative_memory": self.associative_memory.get_state()
        }
        
    def set_state(self, state: Dict[str, Any]):
        """
        Set memory system state.
        
        Args:
            state (Dict[str, Any]): Memory system state
        """
        self.working_memory.set_state(state["working_memory"])
        self.episodic_memory.set_state(state["episodic_memory"])
        self.associative_memory.set_state(state["associative_memory"])
        
    def __repr__(self) -> str:
        return f"<MemorySystem working={len(self.working_memory.get_active_traces())} episodic={len(self.episodes) if hasattr(self, 'episodes') else 0}>"
