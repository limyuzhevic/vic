# Tutorial 3: Memory Management

## Overview

This tutorial provides a comprehensive guide to memory systems in NLM. It covers working memory, episodic memory, and the mathematical principles behind memory formation, consolidation, and retrieval. The tutorial explores how NLM implements biologically plausible memory mechanisms and how to work with these systems effectively.

## Memory Systems in NLM

### What is Memory in NLM?

Memory in NLM is the ability of the brain network to:

1. **Store information** temporarily or permanently
2. **Retrieve information** when needed
3. **Consolidate memories** for long-term storage
4. **Update memories** with new information
5. **Forget irrelevant information** to prevent overload

Memory systems in NLM are inspired by biological memory systems found in the brain:

- **Working Memory:** Short-term storage for immediate processing
- **Episodic Memory:** Long-term storage of specific events and experiences
- **Semantic Memory:** General knowledge and facts
- **Procedural Memory:** Skills and habits

### Memory Architecture in NLM

```
Memory System
├── Working Memory
│   ├── Buffer for immediate processing
│   ├── Limited capacity (typically 4-7 items)
│   ├── Rapid decay without rehearsal
│   └── Active maintenance
├── Episodic Memory
│   ├── Storage of specific events
│   ├── Consolidation for long-term storage
│   ├── Replay for strengthening
│   └── Integration with semantic memory
├── Consolidation System
│   ├── Process for memory stabilization
│   ├── Sleep-dependent consolidation
│   └── stress-induced enhancement
├── Retrieval System
│   ├── Pattern completion
│   ├── Content-addressable search
│   └── Context-dependent retrieval
└── Memory Monitoring
    ├── Capacity tracking
    ├── Quality assessment
    └── Forgetting mechanisms
```

## Working Memory

### Working Memory Fundamentals

Working memory is the cognitive system that temporarily holds and manipulates information for cognitive tasks. In NLM, working memory is implemented as an active buffer system that:

- **Stores sensory input** for immediate processing
- **Maintains information** against decay
- **Competes** for limited resources
- **Updates** with new information

### Working Memory Implementation

```python
import nlm
import time
import numpy as np
from nlm.memory import WorkingMemory

class WorkingMemorySystem:
    def __init__(self, capacity=50, decay_rate=0.01, attention_span=10.0):
        """
        Initialize working memory system.
        
        Parameters:
        -----------
        capacity : int
            Maximum number of items the working memory can hold
        decay_rate : float
            Rate at which memory items decay (0.0 to 1.0)
        attention_span : float
            Time in seconds that items remain active
        """
        self.capacity = capacity
        self.decay_rate = decay_rate
        self.attention_span = attention_span
        
        # Working memory items
        self.items = {}
        self.active_items = []
        self.item_timestamps = {}
        self.attention_weights = {}
        
        # Competition matrix for item prioritization
        self.competition_matrix = None
        
        # Memory state
        self.is_initialized = False
    
    def add_item(self, item_id, data, strength=1.0, attention=1.0):
        """
        Add an item to working memory.
        
        Parameters:
        -----------
        item_id : str
            Unique identifier for the memory item
        data : any
            The actual data to store
        strength : float
            Initial strength of the memory item (0.0 to 1.0)
        attention : float
            Attention weight for the item (0.0 to 1.0)
        """
        if self.is_initialized:
            # Check if we need to make space
            if len(self.items) >= self.capacity:
                self._make_space()
            
            # Add new item
            self.items[item_id] = data
            self.item_timestamps[item_id] = time.time()
            self.attention_weights[item_id] = attention
            
            # Update active items list
            self.active_items.append(item_id)
            
            # Update competition matrix
            self._update_competition_matrix()
            
            print(f"Added item '{item_id}' to working memory (strength: {strength}, attention: {attention})")
        else:
            print("Working memory not initialized. Call initialize() first.")
    
    def get_item(self, item_id):
        """
        Retrieve an item from working memory.
        
        Parameters:
        -----------
        item_id : str
            Identifier of the item to retrieve
        
        Returns:
        --------
        any or None
            The retrieved data, or None if not found
        """
        if item_id in self.items:
            # Check if item is still within attention span
            if self._is_item_active(item_id):
                # Update access time
                self.item_timestamps[item_id] = time.time()
                
                # Get competition strength
                competition = self.get_competition_strength(item_id)
                
                return {
                    "data": self.items[item_id],
                    "attention": self.attention_weights[item_id],
                    "competition": competition,
                    "age": time.time() - self.item_timestamps[item_id]
                }
            else:
                print(f"Item '{item_id}' has expired (outside attention span)")
                self._remove_item(item_id)
        else:
            print(f"Item '{item_id}' not found in working memory")
        
        return None
    
    def remove_item(self, item_id):
        """
        Remove an item from working memory.
        
        Parameters:
        -----------
        item_id : str
            Identifier of the item to remove
        """
        if item_id in self.items:
            del self.items[item_id]
            del self.item_timestamps[item_id]
            del self.attention_weights[item_id]
            
            if item_id in self.active_items:
                self.active_items.remove(item_id)
            
            self._update_competition_matrix()
            print(f"Removed item '{item_id}' from working memory")
        else:
            print(f"Item '{item_id}' not found in working memory")
    
    def _is_item_active(self, item_id):
        """Check if an item is still active (within attention span)."""
        if item_id not in self.item_timestamps:
            return False
        
        age = time.time() - self.item_timestamps[item_id]
        return age < self.attention_span
    
    def _make_space(self):
        """Make space in working memory by removing least important items."""
        # Sort items by importance (attention * age)
        item_importance = []
        for item_id in self.items:
            age = time.time() - self.item_timestamps[item_id]
            importance = self.attention_weights[item_id] * age
            item_importance.append((importance, item_id))
        
        # Sort by importance (lowest first)
        item_importance.sort()
        
        # Remove least important items
        for importance, item_id in item_importance:
            if len(self.items) <= self.capacity - 1:
                break
            
            self.remove_item(item_id)
            print(f"Made space by removing item '{item_id}'")
    
    def _update_competition_matrix(self):
        """Update the competition matrix for item prioritization."""
        n = len(self.active_items)
        if n == 0:
            self.competition_matrix = np.array([])
            return
        
        # Initialize competition matrix
        self.competition_matrix = np.zeros((n, n))
        
        # Calculate competition weights
        for i, item1 in enumerate(self.active_items):
            for j, item2 in enumerate(self.active_items):
                if i != j:
                    # Competition based on attention weights
                    weight1 = self.attention_weights[item1]
                    weight2 = self.attention_weights[item2]
                    self.competition_matrix[i, j] = weight1 * weight2
    
    def get_competition_strength(self, item_id):
        """Get the competition strength for an item."n        
        Parameters:
        -----------
        item_id : str
            Identifier of the item
        
        Returns:
        --------
        float
            Competition strength (0.0 to 1.0)
        """
        if item_id not in self.active_items:
            return 0.0
        
        index = self.active_items.index(item_id)
        
        # Calculate total competition from other items
        total_competition = np.sum(self.competition_matrix[index, :]) - self.competition_matrix[index, index]
        
        # Get item's own strength
        item_strength = self.attention_weights[item_id]
        
        # Calculate competition strength
        if total_competition == 0:
            return item_strength
        else:
            return item_strength / (item_strength + total_competition)
    
    def decay_all(self, time_step):
        """
        Apply decay to all items in working memory.
        
        Parameters:
        -----------
        time_step : float
            Time step for decay calculation
        """
        decay_factor = np.exp(-self.decay_rate * time_step)
        
        for item_id in list(self.items.keys()):
            # Apply decay to attention weight
            self.attention_weights[item_id] *= decay_factor
            
            # Check if item needs to be removed
            if self.attention_weights[item_id] < 0.01:
                print(f"Item '{item_id}' removed due to low strength")
                self.remove_item(item_id)
    
    def get_active_items(self):
        """Get list of active items in working memory.
        
        Returns:
        --------
        list
            List of active item IDs
        """
        active_items = []
        current_time = time.time()
        
        for item_id in self.active_items:
            if self._is_item_active(item_id):
                active_items.append(item_id)
            else:
                # Remove expired items
                self.remove_item(item_id)
        
        return active_items
    
    def initialize(self):
        """Initialize the working memory system."""
        self.is_initialized = True
        print("Working memory system initialized")
    
    def reset(self):
        """Reset working memory to empty state."""
        self.items.clear()
        self.active_items.clear()
        self.item_timestamps.clear()
        self.attention_weights.clear()
        self.competition_matrix = None
        self.is_initialized = False
        print("Working memory system reset")
    
    def get_statistics(self):
        """Get statistics about working memory usage.
        
        Returns:
        --------
        dict
            Dictionary containing working memory statistics
        """
        return {
            "total_items": len(self.items),
            "active_items": len(self.active_items),
            "capacity": self.capacity,
            "usage_ratio": len(self.items) / self.capacity,
            "total_attention": sum(self.attention_weights.values()),
            "average_attention": np.mean(list(self.attention_weights.values())) if self.attention_weights else 0,
            "attention_variance": np.var(list(self.attention_weights.values())) if self.attention_weights else 0
        }

# Example usage of working memory system
def demonstrate_working_memory():
    """Demonstrate working memory functionality."""
    
    print("=== Working Memory Demonstration ===\n")
    
    # Create working memory system
    wm = WorkingMemorySystem(capacity=5, decay_rate=0.05, attention_span=5.0)
    wm.initialize()
    
    # Add items
    print("1. Adding items to working memory...")
    wm.add_item("stimulus_001", {"feature": "red", "shape": "square"}, strength=0.9, attention=0.8)
    wm.add_item("stimulus_002", {"feature": "blue", "shape": "circle"}, strength=0.7, attention=0.6)
    wm.add_item("stimulus_003", {"feature": "green", "shape": "triangle"}, strength=0.8, attention=0.9)
    
    # Check active items
    print("\n2. Active items:")
    active_items = wm.get_active_items()
    for item_id in active_items:
        item = wm.get_item(item_id)
        if item:
            print(f"  {item_id}: {item['data']} (attention: {item['attention']:.2f}, competition: {item['competition']:.2f})")
    
    # Demonstrate competition
    print("\n3. Competition analysis:")
    print("   Items compete for limited attention resources")
    print(f"   High attention items: {[k for k, v in wm.attention_weights.items() if v > 0.7]}")
    
    # Apply decay
    print("\n4. Applying decay...")
    wm.decay_all(1.0)
    
    # Check after decay
    print("\n5. Items after decay:")
    active_items = wm.get_active_items()
    for item_id in active_items:
        item = wm.get_item(item_id)
        if item:
            print(f"  {item_id}: {item['data']} (attention: {item['attention']:.2f})")
    
    # Get statistics
    print("\n6. Working memory statistics:")
    stats = wm.get_statistics()
    for key, value in stats.items():
        print(f"   {key}: {value:.2f}")
    
    # Reset
    print("\n7. Resetting working memory system...")
    wm.reset()

# Run demonstration
if __name__ == "__main__":
    demonstrate_working_memory()
```

### Working Memory in NLM Context

```python
import nlm
import numpy as np

def demonstrate_working_memory_in_nlm():
    """Demonstrate working memory integration in NLM."""
    
    print("=== Working Memory in NLM Context ===\n")
    
    # Create NLM system with working memory
    nlm_system = nlm.NLM()
    
    # Create brain
    brain = nlm_system.create_agent({
        "regions": 2,
        "neurons_per_region": 50
    })
    
    # Create working memory for agent
    agent_working_memory = nlm.WorkingMemory(capacity=10, decay_rate=0.02)
    
    # Simulate agent receiving sensory input
    print("1. Agent receiving sensory input...")
    sensory_inputs = [
        {"stimulus": "red", "location": "left"},
        {"stimulus": "blue", "location": "right"},
        {"stimulus": "green", "location": "center"},
        {"stimulus": "yellow", "location": "top"},
        {"stimulus": "purple", "location": "bottom"}
    ]
    
    for i, input_data in enumerate(sensory_inputs):
        item_id = f"input_{i}"
        agent_working_memory.add_item(
            item_id, 
            input_data, 
            strength=0.8 + np.random.random() * 0.2,
            attention=0.9 - i * 0.1  # Decreasing attention
        )
        
        # Agent processes the input
        agent_working_memory.add_item(
            f"processed_{item_id}",
            {"processed": True, "action": "approach"},
            strength=0.9,
            attention=0.8
        )
    
    # Show working memory state
    print("\n2. Working memory state:")
    active_items = agent_working_memory.get_active_items()
    print(f"   Active items: {len(active_items)}")
    
    for item_id in active_items:
        item = agent_working_memory.get_item(item_id)
        if item:
            print(f"   - {item_id}: {item['data']} (attention: {item['attention']:.2f}, competition: {item['competition']:.2f})")
    
    # Simulate agent decision making
    print("\n3. Agent decision making...")
    
    # Agent selects item with highest competition strength
    best_item = None
    best_competition = 0
    
    for item_id in active_items:
        item = agent_working_memory.get_item(item_id)
        if item and item['competition'] > best_competition:
            best_competition = item['competition']
            best_item = item_id
    
    if best_item:
        decision = agent_working_memory.get_item(best_item)
        print(f"   Selected item for processing: {best_item}")
        print(f"   Decision: {decision['data']}")
        print(f"   Competition strength: {best_competition:.2f}")
    
    # Simulate time passing and decay
    print("\n4. Simulating time passing...")
    print("   Waiting for 3 seconds...")
    agent_working_memory.decay_all(3.0)
    
    # Show working memory after decay
    active_items = agent_working_memory.get_active_items()
    print(f"\n5. Working memory after decay: {len(active_items)} active items")
    
    # Create decision based on remaining items
    if active_items:
        print("\n6. Making final decision...")
        for item_id in active_items:
            item = agent_working_memory.get_item(item_id)
            if item:
                print(f"   Considering item: {item_id} (attention: {item['attention']:.2f})")

# Run demonstration
if __name__ == "__main__":
    demonstrate_working_memory_in_nlm()
```

## Episodic Memory

### Episodic Memory Fundamentals

Episodic memory in NLM stores specific experiences and events, allowing the agent to:

- **Recall specific events** that occurred in the past
- **Learn from experiences** by replaying important memories
- **Generalize** similar experiences to novel situations
- **Plan for the future** based on past outcomes

### Episodic Memory Implementation

```python
import nlm
import time
import numpy as np
from nlm.memory import EpisodicMemory

class EpisodicMemorySystem:
    def __init__(self, capacity=1000, consolidation_threshold=0.7, replay_frequency=0.1):
        """
        Initialize episodic memory system.
        
        Parameters:
        -----------
        capacity : int
            Maximum number of episodes that can be stored
        consolidation_threshold : float
            Strength threshold for memory consolidation (0.0 to 1.0)
        replay_frequency : float
            Probability of replaying an episode (0.0 to 1.0)
        """
        self.capacity = capacity
        self.consolidation_threshold = consolidation_threshold
        self.replay_frequency = replay_frequency
        
        # Episodic memory entries
        self.episodes = {}
        self.episode_embeddings = {}
        self.episode_timestamps = {}
        self.episode_strength = {}
        self.episode_importance = {}
        
        # Consolidation and replay
        self.consolidation_queue = []
        self.replay_schedule = []
        
        # Memory statistics
        self.total_consolidations = 0
        self.total_replays = 0
        
        # Memory state
        self.is_initialized = False
    
    def encode(self, experience, timestamp=None):
        """
        Encode an experience into episodic memory.
        
        Parameters:
        -----------
        experience : dict
            The experience to encode (stimulus, action, outcome, context)
        timestamp : float
            Timestamp of the experience (current time if None)
        
        Returns:
        --------
        str
            Unique identifier for the encoded episode
        """
        if not self.is_initialized:
            print("Episodic memory not initialized. Call initialize() first.")
            return None
        
        # Generate unique episode ID
        episode_id = f"episode_{len(self.episodes) + 1:06d}"
        
        # Set timestamp
        if timestamp is None:
            timestamp = time.time()
        
        # Create episode entry
        episode = {
            "id": episode_id,
            "timestamp": timestamp,
            "experience": experience.copy(),
            "embedding": self._create_embedding(experience),
            "strength": self._calculate_initial_strength(experience),
            "context": self._extract_context(experience),
            "importance": self._calculate_importance(experience),
            "replay_count": 0,
            "last_accessed": timestamp,
            "next_replay": timestamp + np.random.exponential(1.0 / self.replay_frequency)
        }
        
        # Add to memory
        self.episodes[episode_id] = episode
        self.episode_embeddings[episode_id] = episode["embedding"]
        self.episode_timestamps[episode_id] = timestamp
        self.episode_strength[episode_id] = episode["strength"]
        self.episode_importance[episode_id] = episode["importance"]
        
        # Schedule replay
        self._schedule_replay(episode_id)
        
        # Check consolidation needs
        self._check_consolidation_needs()
        
        print(f"Encoded experience as episode '{episode_id}'")
        print(f"  - Strength: {episode['strength']:.3f}")
        print(f"  - Importance: {episode['importance']:.3f}")
        
        return episode_id
    
    def _create_embedding(self, experience):
        """Create a vector embedding for an experience."""
        # Convert experience to numerical vector
        # This is a simplified implementation
        embedding = np.zeros(10)  # 10-dimensional embedding
        
        # Encode features
        embedding[0] = experience.get("stimulus_intensity", 0.5)
        embedding[1] = experience.get("action_strength", 0.5)
        embedding[2] = experience.get("outcome_value", 0.5)
        embedding[3] = experience.get("reward_magnitude", 0.5)
        
        # Add random noise for variability
        embedding += np.random.normal(0, 0.1, size=10)
        
        # Normalize to unit vector
        embedding = embedding / (np.linalg.norm(embedding) + 1e-8)
        
        return embedding
    
    def _calculate_initial_strength(self, experience):
        """Calculate initial strength of an episode."""
        # Factors affecting initial strength
        stimulus_factor = experience.get("stimulus_intensity", 0.5)
        outcome_factor = experience.get("outcome_value", 0.5)
        reward_factor = experience.get("reward_magnitude", 0.5)
        
        # Combine factors
        strength = (stimulus_factor * 0.3 + outcome_factor * 0.4 + reward_factor * 0.3)
        
        # Add some random variation
        strength *= np.random.uniform(0.8, 1.2)
        
        return np.clip(strength, 0.0, 1.0)
    
    def _extract_context(self, experience):
        """Extract context information from an experience."""
        context = {
            "time_of_day": experience.get("time_of_day", "unknown"),
            "location": experience.get("location", (0, 0)),
            "objects_present": experience.get("objects", []),
            "weather": experience.get("weather", "unknown"),
            "social_context": experience.get("social_context", "unknown")
        }
        
        return context
    
    def _calculate_importance(self, experience):
        """Calculate importance of an episode."""
        # Factors affecting importance
        novelty_factor = experience.get("novelty", 0.5)
        reward_factor = experience.get("reward_magnitude", 0.5)
        prediction_error = experience.get("prediction_error", 0.5)
        
        # Combine factors
        importance = (novelty_factor * 0.3 + reward_factor * 0.5 + prediction_error * 0.2)
        
        return np.clip(importance, 0.0, 1.0)
    
    def retrieve(self, episode_id=None, filter_keywords=None, time_window=None, min_strength=0.0):
        """
        Retrieve episodes from episodic memory.
        
        Parameters:
        -----------
        episode_id : str, optional
            Specific episode ID to retrieve
        filter_keywords : list, optional
            Keywords to filter episodes by context
        time_window : tuple, optional
            Time window (start_time, end_time) for retrieval
        min_strength : float
            Minimum strength threshold for retrieval
        
        Returns:
        --------
        list
            List of retrieved episodes
        """
        retrieved = []
        
        if episode_id:
            # Retrieve specific episode
            if episode_id in self.episodes:
                retrieved.append(self.episodes[episode_id])
                # Update access time
                self.episodes[episode_id]["last_accessed"] = time.time()
        else:
            # Retrieve all episodes matching filters
            for episode_id, episode in self.episodes.items():
                # Check strength threshold
                if episode["strength"] < min_strength:
                    continue
                
                # Check time window
                if time_window:
                    start_time, end_time = time_window
                    if not (start_time <= episode["timestamp"] <= end_time):
                        continue
                
                # Check context keywords
                if filter_keywords:
                    context = episode["context"]
                    if not self._matches_keywords(context, filter_keywords):
                        continue
                
                # Check importance
                if episode["importance"] > 0.5:  # High importance episodes
                    retrieved.append(episode)
        
        # Sort by importance (most important first)
        retrieved.sort(key=lambda x: x["importance"], reverse=True)
        
        return retrieved
    
    def _matches_keywords(self, context, keywords):
        """Check if context matches keywords."""
        for keyword in keywords:
            # Check objects
            if "objects" in context and keyword in context["objects"]:
                return True
            
            # Check context keys
            for key, value in context.items():
                if isinstance(value, str) and keyword.lower() in value.lower():
                    return True
        
        return False
    
    def _schedule_replay(self, episode_id):
        """Schedule replay of an episode."""
        episode = self.episodes[episode_id]
        
        # Schedule replay based on importance and strength
        replay_interval = np.exp(-episode["importance"] * 10)  # More important = more frequent
        
        # Add to replay schedule
        replay_event = {
            "episode_id": episode_id,
            "scheduled_time": episode["timestamp"] + replay_interval,
            "importance": episode["importance"]
        }
        
        self.replay_schedule.append(replay_event)
    
    def _check_consolidation_needs(self):
        """Check which episodes need consolidation."""
        for episode_id in list(self.episodes.keys()):
            episode = self.episodes[episode_id]
            
            # Check if episode needs consolidation
            if (episode["strength"] > self.consolidation_threshold and 
                episode["importance"] > 0.5):
                
                # Schedule consolidation
                consolidation_event = {
                    "episode_id": episode_id,
                    "threshold": self.consolidation_threshold,
                    "importance": episode["importance"]
                }
                
                if consolidation_event not in self.consolidation_queue:
                    self.consolidation_queue.append(consolidation_event)
    
    def consolidate(self, importance_threshold=0.5):
        """
        Consolidate important episodes.
        
        Parameters:
        -----------
        importance_threshold : float
            Minimum importance for consolidation
        
        Returns:
        --------
        list
            List of consolidated episodes
        """
        consolidated = []
        
        for episode_id in list(self.episodes.keys()):
            episode = self.episodes[episode_id]
            
            # Check if episode needs consolidation
            if episode["importance"] >= importance_threshold:
                # Consolidate episode
                self._consolidate_episode(episode_id)
                consolidated.append(episode_id)
                self.total_consolidations += 1
        
        return consolidated
    
    def _consolidate_episode(self, episode_id):
        """Consolidate a single episode."""
        episode = self.episodes[episode_id]
        
        # Strengthen episode
        episode["strength"] = min(1.0, episode["strength"] * 1.5)
        
        # Update embedding (more stable)
        episode["embedding"] = episode["embedding"] * 0.8 + np.random.normal(0, 0.1, size=10)
        episode["embedding"] = episode["embedding"] / (np.linalg.norm(episode["embedding"]) + 1e-8)
        
        # Reduce replay frequency
        episode["next_replay"] = episode["timestamp"] + np.exp(-episode["strength"] * 5)
    
    def replay(self, episode_id, replay_mode="standard"):
        """
        Replay an episode.
        
        Parameters:
        -----------
        episode_id : str
            Episode to replay
        replay_mode : str
            Replay mode ("standard", "active", "reflective")
        
        Returns:
        --------
        dict
            Replay results
        """
        if episode_id not in self.episodes:
            return {"success": False, "reason": "Episode not found"}
        
        episode = self.episodes[episode_id]
        
        # Check if it's time to replay
        current_time = time.time()
        if current_time < episode["next_replay"]:
            return {"success": False, "reason": "Not time to replay yet"}
        
        # Update access time
        episode["last_accessed"] = current_time
        episode["replay_count"] += 1
        
        # Process episode based on replay mode
        if replay_mode == "standard":
            return self._standard_replay(episode)
        elif replay_mode == "active":
            return self._active_replay(episode)
        elif replay_mode == "reflective":
            return self._reflective_replay(episode)
        else:
            return {"success": False, "reason": f"Unknown replay mode: {replay_mode}"}
    
    def _standard_replay(self, episode):
        """Standard replay for memory consolidation."""
        # For standard replay, just increment replay count
        # Real implementation would involve actual neural processing
        
        return {
            "success": True,
            "mode": "standard",
            "replay_time": time.time(),
            "strengthening": episode["strength"] * 0.1,  # Mild strengthening
            "episode": episode["id"]
        }
    
    def _active_replay(self, episode):
        """Active replay for learning new behaviors."""
        # Active replay involves generating new actions based on old experiences
        experience = episode["experience"]
        
        # Simulate learning from the episode
        learned_behavior = {
            "action": experience["action"],
            "context": experience["context"],
            "outcome": experience["outcome"],
            "novelty": episode["importance"]
        }
        
        return {
            "success": True,
            "mode": "active",
            "replay_time": time.time(),
            "learned_behavior": learned_behavior,
            "episode": episode["id"]
        }
    
    def _reflective_replay(self, episode):
        """Reflective replay for insight and understanding."""
        # Reflective replay involves integrating the episode into existing knowledge
        experience = episode["experience"]
        
        # Create reflective insight
        insight = {
            "pattern": self._extract_pattern(episode),
            "implication": self._extract_implication(episode),
            "generalization": self._extract_generalization(episode)
        }
        
        return {
            "success": True,
            "mode": "reflective",
            "replay_time": time.time(),
            "insight": insight,
            "episode": episode["id"]
        }
    
    def _extract_pattern(self, episode):
        """Extract patterns from an episode."""
        experience = episode["experience"]
        
        # Simple pattern extraction
        pattern = {
            "stimulus_response": f"{experience['stimulus']} -> {experience['action']}",
            "outcome_relation": f"{experience['action']} -> {experience['outcome']}",
            "context_influence": experience.get("context", {})
        }
        
        return pattern
    
    def _extract_implication(self, episode):
        """Extract implications from an episode."""
        experience = episode["experience"]
        
        # Simple implication extraction
        implications = [
            f"When {experience['stimulus']} appears, consider {experience['action']}",
            f"Expected outcome of {experience['action']} is {experience['outcome']}"
        ]
        
        return implications
    
    def _extract_generalization(self, episode):
        """Extract generalizations from an episode."""
        experience = episode["experience"]
        
        # Simple generalization
        generalization = {
            "rule": f"If {experience['stimulus_type']} then {experience['recommended_action']}",
            "conditions": ["similar_context", "familiar_environment"],
            "exceptions": ["unusual_stimulus", "novel_context"]
        }
        
        return generalization
    
    def analyze_patterns(self):
        """Analyze patterns in episodic memory."""
        
        patterns = {
            "most_common_stimuli": self._get_most_common_stimuli(),
            "most_common_actions": self._get_most_common_actions(),
            "most_common_outcomes": self._get_most_common_outcomes(),
            "temporal_patterns": self._get_temporal_patterns(),
            "emotional_patterns": self._get_emotional_patterns()
        }
        
        return patterns
    
    def _get_most_common_stimuli(self):
        """Get most common stimuli."""
        stimuli_counts = {}
        for episode in self.episodes.values():
            stimulus = episode["experience"].get("stimulus", "unknown")
            stimuli_counts[stimulus] = stimuli_counts.get(stimulus, 0) + 1
        
        sorted_stimuli = sorted(stimuli_counts.items(), key=lambda x: x[1], reverse=True)
        return {"stimulus": stimuli, "count": count} for stimulus, count in sorted_stimuli[:5]
    
    def _get_most_common_actions(self):
        """Get most common actions."""
        action_counts = {}
        for episode in self.episodes.values():
            action = episode["experience"].get("action", "unknown")
            action_counts[action] = action_counts.get(action, 0) + 1
        
        sorted_actions = sorted(action_counts.items(), key=lambda x: x[1], reverse=True)
        return {"action": action, "count": count} for action, count in sorted_actions[:5]
    
    def _get_most_common_outcomes(self):
        """Get most common outcomes."""
        outcome_counts = {}
        for episode in self.episodes.values():
            outcome = episode["experience"].get("outcome", "unknown")
            outcome_counts[outcome] = outcome_counts.get(outcome, 0) + 1
        
        sorted_outcomes = sorted(outcome_counts.items(), key=lambda x: x[1], reverse=True)
        return {"outcome": outcome, "count": count} for outcome, count in sorted_outcomes[:5]
    
    def _get_temporal_patterns(self):
        """Get temporal patterns in episodes."""
        timestamps = [episode["timestamp"] for episode in self.episodes.values()]
        
        if len(timestamps) < 2:
            return {"pattern": "insufficient_data", "frequency": 0}
        
        # Calculate time intervals
        intervals = np.diff(sorted(timestamps))
        
        # Find most common interval
        if len(intervals) > 0:
            most_common_interval = np.histogram(intervals, bins=10)[1].argmax() * (np.max(intervals) / 10)
            return {"pattern": "regular", "interval_hours": most_common_interval, "frequency": len(intervals)}
        else:
            return {"pattern": "none", "frequency": 0}
    
    def _get_emotional_patterns(self):
        """Get emotional patterns in episodes."""
        emotion_counts = {}
        for episode in self.episodes.values():
            emotion = episode["experience"].get("emotion", "neutral")
            emotion_counts[emotion] = emotion_counts.get(emotion, 0) + 1
        
        return emotion_counts
    
    def get_statistics(self):
        """Get statistics about episodic memory."""
        
        if not self.episodes:
            return {"total_episodes": 0}
        
        return {
            "total_episodes": len(self.episodes),
            "total_consolidations": self.total_consolidations,
            "total_replays": self.total_replays,
            "average_strength": np.mean([e["strength"] for e in self.episodes.values()]),
            "average_importance": np.mean([e["importance"] for e in self.episodes.values()]),
            "strongest_episode": max(self.episodes.values(), key=lambda x: x["strength"])["id"],
            "most_important_episode": max(self.episodes.values(), key=lambda x: x["importance"])["id"],
            "least_accessed_episode": min(self.episodes.values(), key=lambda x: x["last_accessed"])["id"],
            "consolidation_queue_length": len(self.consolidation_queue),
            "replay_schedule_length": len(self.replay_schedule)
        }

# Example usage of episodic memory system
def demonstrate_episodic_memory():
    """Demonstrate episodic memory functionality."""
    
    print("=== Episodic Memory Demonstration ===\n")
    
    # Create episodic memory system
    ems = EpisodicMemorySystem(capacity=1000, consolidation_threshold=0.7, replay_frequency=0.1)
    ems.initialize()
    
    # Encode sample experiences
    print("1. Encoding sample experiences...")
    
    experiences = [
        {
            "stimulus": "red_square",
            "action": "approach",
            "outcome": "reward",
            "stimulus_intensity": 0.8,
            "reward_magnitude": 10,
            "time_of_day": "morning",
            "location": (10, 10),
            "emotion": "joy"
        },
        {
            "stimulus": "blue_circle",
            "action": "avoid",
            "outcome": "penalty",
            "stimulus_intensity": 0.6,
            "reward_magnitude": -5,
            "time_of_day": "afternoon",
            "location": (20, 20),
            "emotion": "fear"
        },
        {
            "stimulus": "green_triangle",
            "action": "explore",
            "outcome": "neutral",
            "stimulus_intensity": 0.4,
            "reward_magnitude": 0,
            "time_of_day": "evening",
            "location": (30, 30),
            "emotion": "curiosity"
        },
        {
            "stimulus": "yellow_rectangle",
            "action": "play",
            "outcome": "reward",
            "stimulus_intensity": 0.9,
            "reward_magnitude": 15,
            "time_of_day": "morning",
            "location": (15, 15),
            "emotion": "excitement"
        },
        {
            "stimulus": "purple_oval",
            "action": "learn",
            "outcome": "reward",
            "stimulus_intensity": 0.7,
            "reward_magnitude": 12,
            "time_of_day": "afternoon",
            "location": (25, 25),
            "emotion": "satisfaction"
        }
    ]
    
    for i, experience in enumerate(experiences):
        episode_id = ems.encode(experience)
        print(f"  Encoded: {episode_id}")
    
    # Retrieve episodes
    print("\n2. Retrieving episodes...")
    
    # Retrieve specific episode
    specific_episode = ems.retrieve(episode_id="episode_00000001")
    if specific_episode:
        print("   Most important episode:")
        for episode in specific_episode:
            print(f"     ID: {episode['id']}")
            print(f"     Strength: {episode['strength']:.3f}")
            print(f"     Importance: {episode['importance']:.3f}")
            print(f"     Experience: {episode['experience']}")
    
    # Retrieve by filter
    print("\n3. Retrieving episodes by filter (reward experiences)...")
    reward_episodes = ems.retrieve(filter_keywords=["reward"])
    print(f"   Found {len(reward_episodes)} reward episodes")
    for episode in reward_episodes:
        print(f"     ID: {episode['id']}, Reward: {episode['experience']['outcome']}")
    
    # Retrieve by strength
    print("\n4. Retrieving episodes by strength (min 0.7)...")
    strong_episodes = ems.retrieve(min_strength=0.7)
    print(f"   Found {len(strong_episodes)} strong episodes")
    for episode in strong_episodes:
        print(f"     ID: {episode['id']}, Strength: {episode['strength']:.3f}")
    
    # Consolidate episodes
    print("\n5. Consolidating episodes...")
    consolidated = ems.consolidate(importance_threshold=0.5)
    print(f"   Consolidated {len(consolidated)} episodes")
    
    # Analyze patterns
    print("\n6. Analyzing memory patterns...")
    patterns = ems.analyze_patterns()
    print("   Memory patterns:")
    for pattern_type, pattern_data in patterns.items():
        print(f"     {pattern_type}: {pattern_data}")
    
    # Get statistics
    print("\n7. Episodic memory statistics:")
    stats = ems.get_statistics()
    for key, value in stats.items():
        if isinstance(value, dict):
            print(f"   {key}:")
            for sub_key, sub_value in value.items():
                print(f"     {sub_key}: {sub_value}")
        else:
            print(f"   {key}: {value}")
    
    # Simulate replay
    print("\n8. Simulating replay...")
    if ems.episodes:
        first_episode_id = list(ems.episodes.keys())[0]
        replay_result = ems.replay(first_episode_id, replay_mode="reflective")
        print(f"   Replay result: {replay_result}")

# Run demonstration
if __name__ == "__main__":
    demonstrate_episodic_memory()
```

### Episodic Memory in NLM Context

```python
import nlm

def demonstrate_episodic_memory_in_nlm():
    """Demonstrate episodic memory integration in NLM."""
    
    print("=== Episodic Memory in NLM Context ===\n")
    
    # Create NLM system with episodic memory
    nlm_system = nlm.NLM()
    
    # Create brain
    brain = nlm_system.create_agent({
        "regions": 3,
        "neurons_per_region": 100
    })
    
    # Create episodic memory system
    episodic_memory = nlm.EpisodicMemory(
        capacity=1000,
        consolidation_threshold=0.7,
        replay_frequency=0.1
    )
    
    # Simulate agent experiences and learns from them
    print("1. Agent experiencing and learning from events...")
    
    events = [
        {
            "stimulus": "red_ball",
            "action": "reach",
            "outcome": "success",
            "reward": 10,
            "time": 0
        },
        {
            "stimulus": "blue_ball",
            "action": "avoid",
            "outcome": "failure",
            "reward": -5,
            "time": 1
        },
        {
            "stimulus": "green_ball",
            "action": "explore",
            "outcome": "neutral",
            "reward": 0,
            "time": 2
        },
        {
            "stimulus": "yellow_ball",
            "action": "reach",
            "outcome": "success",
            "reward": 8,
            "time": 3
        },
        {
            "stimulus": "red_ball",
            "action": "reach",
            "outcome": "success",
            "reward": 10,
            "time": 4
        }
    ]
    
    for event in events:
        # Agent experiences the event
        print(f"   Event {event['time']}: {event['stimulus']} -> {event['action']} = {event['outcome']}")
        
        # Encode event in episodic memory
        episode_id = episodic_memory.encode(event)
        print(f"     Encoded as: {episode_id}")
        
        # Agent processes the event
        brain.step(event['time'])
        
        # Agent may replay important memories
        if np.random.random() < episodic_memory.replay_frequency:
            if episodic_memory.episodes:
                replay_episode = np.random.choice(list(episodic_memory.episodes.keys()))
                replay_result = episodic_memory.replay(replay_episode)
                if replay_result['success']:
                    print(f"     Replayed episode: {replay_episode}")
    
    # Agent retrieves relevant memories for decision making
    print("\n2. Agent retrieving memories for decision making...")
    
    # Current situation
    current_stimulus = "red_ball"
    current_time = 5
    
    # Retrieve relevant episodes
    relevant_episodes = episodic_memory.retrieve(
        filter_keywords=["red_ball", "reach"],
        time_window=(0, current_time)
    )
    
    print(f"   Relevant episodes found: {len(relevant_episodes)}")
    
    if relevant_episodes:
        print("   Retrieved episodes:")
        for episode in relevant_episodes:
            print(f"     ID: {episode['id']}")
            print(f"     Outcome: {episode['experience']['outcome']}")
            print(f"     Reward: {episode['experience']['reward']}")
            print(f"     Importance: {episode['importance']:.3f}")
    
    # Agent makes decision based on retrieved memories
    print("\n3. Agent decision making...")
    
    if relevant_episodes:
        # Select best episode based on importance
        best_episode = max(relevant_episodes, key=lambda x: x['importance'])
        
        # Make decision based on best experience
        recommended_action = best_episode['experience']['action']
        expected_outcome = best_episode['experience']['outcome']
        
        print(f"   Recommended action: {recommended_action}")
        print(f"   Expected outcome: {expected_outcome}")
        print(f"   Confidence: {best_episode['importance']:.2f}")
    
    # Show episodic memory statistics
    print("\n4. Episodic memory statistics:")
    stats = episodic_memory.get_statistics()
    for key, value in stats.items():
        if isinstance(value, dict):
            print(f"   {key}:")
            for sub_key, sub_value in value.items():
                print(f"     {sub_key}: {sub_value}")
        else:
            print(f"   {key}: {value}")
    
    # Simulate consolidation
    print("\n5. Consolidating memories...")
    consolidated = episodic_memory.consolidate(importance_threshold=0.3)
    print(f"   Consolidated {len(consolidated)} episodes")

# Run demonstration
if __name__ == "__main__":
    demonstrate_episodic_memory_in_nlm()
```

## Key Takeaways

### 1. Working Memory

- **Limited capacity:** Can only hold a few items at once
- **Temporal dynamics:** Items decay over time
- **Attention mechanisms:** Items compete for limited attention
- **Active maintenance:** Continuous updating of information

### 2. Episodic Memory

- **Specific events:** Stores detailed information about experiences
- **Temporal organization:** Memories are time-stamped and ordered
- **Consolidation process:** Important memories are strengthened over time
- **Replay mechanism:** Memories are replayed for consolidation

### 3. Memory Integration

- **Hierarchical organization:** Working memory feeds into episodic memory
- **Bidirectional flow:** Episodic memory can influence working memory
- **Cross-modal interactions:** Different types of memories interact
- **Adaptive mechanisms:** Memory systems adapt to environmental demands

### 4. Learning from Memory

- **Pattern completion:** Retrieval of partial information
- **Generalization:** Applying past experiences to new situations
- **Decision making:** Using memories to guide behavior
- **Planning:** Simulating future scenarios based on past experiences

## Further Learning

### Practice Exercises:

1. **Working Memory Challenge:** Create a working memory system and test its capacity limits
2. **Episodic Memory Encoding:** Encode a series of experiences and analyze patterns
3. **Memory Consolidation:** Experiment with different consolidation thresholds
4. **Replay Mechanisms:** Test different replay modes and their effects
5. **Integration Testing:** Combine working and episodic memory systems

### Advanced Topics:

1. **Neural Basis of Memory:** Understand the neural mechanisms underlying memory
2. **Memory Disorders:** Study how memory systems fail in neurological conditions
3. **Computational Models:** Develop more sophisticated memory models
4. **Applications:** Apply memory systems to real-world problems
5. **Extensions:** Create custom memory systems for specific applications

### Research Applications:

1. **Cognitive Psychology:** Model human memory processes
2. **Neuroscience:** Study neural mechanisms of memory
3. **Artificial Intelligence:** Develop intelligent memory systems
4. **Education:** Design effective learning strategies
5. **Clinical Psychology:** Understand and treat memory disorders

## Conclusion

This tutorial has provided a comprehensive guide to memory systems in NLM. You have learned how to:

1. **Implement working memory:** Create systems for short-term information storage and processing
2. **Build episodic memory:** Develop systems for storing and retrieving specific experiences
3. **Integrate memory systems:** Connect different memory types for comprehensive information processing
4. **Analyze memory patterns:** Understand how memories are organized and retrieved
5. **Apply memory to learning:** Use memories to guide behavior and decision making

Memory is fundamental to intelligent behavior. In NLM, memory systems provide the foundation for:

- **Learning from experience:** Storing and using past information
- **Generalization:** Applying learned knowledge to new situations
- **Adaptation:** Adjusting behavior based on environmental feedback
- **Planning:** Simulating future scenarios based on past experiences
- **Problem solving:** Using memory to find solutions

By mastering these memory systems, you can create NLM agents that:

- Learn effectively from their environment
- Adapt to changing conditions
- Make informed decisions based on experience
- Generalize knowledge to novel situations
- Plan for future goals and challenges

The memory systems you've learned here form the backbone of intelligent behavior in NLM. As you continue to explore and develop your NLM applications, you'll find that effective memory usage is key to creating truly intelligent and adaptive systems.

Happy modeling!