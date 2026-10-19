# Tutorial 2: Understanding Memory Systems

This tutorial explores the three memory systems in NLM: Working Memory, Episodic Memory, and Associative Memory. These systems work together to store, retrieve, and use information for intelligent behavior.

## Learning Objectives

By the end of this tutorial, you will be able to:

- Understand the different memory systems and their purposes
- Create and manipulate working memory
- Store and retrieve episodic memories
- Create and use associative relationships
- Integrate memory systems in your simulations

## 1. Introduction to Memory Systems

NLM implements three complementary memory systems:

### 1.1 Working Memory

- **Purpose:** Transient storage of information with competition and decay
- **Duration:** Milliseconds to seconds
- **Function:** Active manipulation of information
- **Capacity:** Limited (typically 7±2 items)

### 1.2 Episodic Memory

- **Purpose:** Storage of events and experiences
- **Duration:** Seconds to lifetime
- **Function:** Memory of what happened, when, and where
- **Capacity:** Large but organized

### 1.3 Associative Memory

- **Purpose:** Relationships between representations
- **Duration:** Variable
- **Function:** Pattern completion and association recall
- **Capacity:** Large, distributed

## 2. Working Memory System

### 2.1 Working Memory Overview

```python
import pynlm

# Create working memory system
working_memory = pynlm.WorkingMemory()

# Configure capacity
working_memory.setCapacity(20)
```

### 2.2 Working Memory Operations

#### Store Information

```python
# Create a neuron ID (in practice, get from brain)
neuron_id = pynlm.NeuronId(1)

# Store value in working memory
working_memory.store(neuron_id, 0.8)
```

#### Retrieve Information

```python
# Retrieve value from working memory
value = working_memory.recall(neuron_id)
# Returns: 0.8
```

#### Check Memory Contents

```python
# Get current memory state
print(f"Current size: {working_memory.getCurrentSize()}")
print(f"Capacity: {working_memory.capacity}")
print(f"Active traces: {working_memory.activeTraces}")
```

### 2.3 Working Memory Decay

```python
# Apply decay to all stored items
working_memory.update(0.1)  # 10% decay per update
```

### 2.4 Working Memory Competition

```python
# Working memory has limited capacity
# When full, new items may cause old ones to decay or be removed
working_memory.setCapacity(5)  # Limited capacity
working_memory.store(neuron_a, 0.9)  # Store first
working_memory.store(neuron_b, 0.8)  # Store second
working_memory.store(neuron_c, 0.7)  # Store third
# At this point, memory is full (capacity=3)
# Additional stores may cause decay of existing items
```

## 3. Episodic Memory System

### 3.1 Episodic Memory Overview

```python
import pynlm

# Create episodic memory system
episodic_memory = pynlm.EpisodicMemory()

# Configure maximum episodes
episodic_memory.setMaxEpisodes(1000)
```

### 3.2 Episodic Memory Item

```python
# Create an episodic memory item
episode = pynlm.EpisodicMemoryItem(
    timestamp=100,  # Simulation step when it occurred
    neurons=[pynlm.NeuronId(1000)],  # What happened
    values=[0.8],  # How strongly it happened
    metadata="Found resource at position (5,5)"  # Context
)
```

### 3.3 Storing and Retrieving Episodes

#### Store Episode

```python
# Store an episodic memory
episodic_memory.storeEpisode(episode)
```

#### Retrieve by Index

```python
# Get specific episode
episode = episodic_memory.retrieveEpisode(0)
```

#### Get Recent Episodes

```python
# Get most recent episodes
recent_episodes = episodic_memory.getRecentEpisodes(5)
```

### 3.4 Memory Consolidation

```python
# Consolidate relevant episodes
episodic_memory.consolidate(relevance_threshold=0.7)
```

### 3.5 Memory Replay

```python
# Get episodes suitable for replay
replay_episodes = episodic_memory.getEpisodesForReplay(3)
```

## 4. Associative Memory System

### 4.1 Associative Memory Overview

```python
import pynlm

# Create associative memory system
associative_memory = pynlm.AssociativeMemory()
```

### 4.2 Creating Associations

```python
# Create association between two neurons
neuron_a = pynlm.NeuronId(1)
neuron_b = pynlm.NeuronId(2)

associative_memory.addAssociation(neuron_a, neuron_b, strength=0.9)
```

### 4.3 Retrieving Associations

```python
# Get neurons associated with a given neuron
associated_neurons = associative_memory.getAssociations(neuron_a)

# Get strength of association between two neurons
strength = associative_memory.getAssociationStrength(neuron_a, neuron_b)
```

### 4.4 Pattern Completion

```python
# Recall pattern based on partial input
pattern = [0.5, 0.3, 0.7]  # Partial pattern
completed_pattern = associative_memory.retrieve(pattern)
```

## 5. Integrating Memory Systems

### 5.1 Complete Memory Integration Example

```python
class MemoryIntegrationExample:
    def __init__(self):
        self.working_memory = pynlm.WorkingMemory()
        self.working_memory.setCapacity(10)
        
        self.episodic_memory = pynlm.EpisodicMemory()
        self.episodic_memory.setMaxEpisodes(100)
        
        self.associative_memory = pynlm.AssociativeMemory()
        
    def demonstrate_working_memory(self):
        """Demonstrate working memory in action"""
        print("Working Memory Demonstration:")
        
        # Store current sensory information
        neuron_id = pynlm.NeuronId(100)
        sensory_value = 0.8  # Simulated sensory input
        self.working_memory.store(neuron_id, sensory_value)
        
        # Use stored information for immediate decisions
        value = self.working_memory.recall(neuron_id)
        print(f"  Retrieved sensory value: {value:.2f}")
        
        # Decay over time
        self.working_memory.update(0.1)
        
    def demonstrate_episodic_memory(self):
        """Demonstrate episodic memory"""
        print("Episodic Memory Demonstration:")
        
        # Create episodic memory of finding food
        episode = pynlm.EpisodicMemoryItem(
            timestamp=50,
            neurons=[pynlm.NeuronId(101)],
            values=[0.9],
            metadata="Found high-quality food source"
        )
        
        # Store the memory
        self.episodic_memory.storeEpisode(episode)
        print(f"  Stored episode about finding food")
        
        # Later, retrieve the memory
        recent_episodes = self.episodic_memory.getRecentEpisodes(1)
        if recent_episodes:
            memory = recent_episodes[0]
            print(f"  Recalled memory: {memory.metadata}")
            
    def demonstrate_associative_memory(self):
        """Demonstrate associative memory"""
        print("Associative Memory Demonstration:")
        
        # Create associations between concepts
        hunger_neuron = pynlm.NeuronId(102)
        food_neuron = pynlm.NeuronId(103)
        reward_neuron = pynlm.NeuronId(104)
        
        # Associate hunger with food
        self.associative_memory.addAssociation(hunger_neuron, food_neuron, 0.8)
        
        # Associate food with reward
        self.associative_memory.addAssociation(food_neuron, reward_neuron, 0.9)
        
        # Retrieve associated concepts
        food_associations = self.associative_memory.getAssociations(food_neuron)
        print(f"  Food is associated with: {[n.value for n in food_associations]}")
        
    def run_complete_example(self):
        """Run complete memory integration example"""
        print("Memory Integration Example")
        print("=" * 40)
        
        # Demonstrate each memory system
        self.demonstrate_working_memory()
        print()
        
        self.demonstrate_episodic_memory()
        print()
        
        self.demonstrate_associative_memory()
        print()
        
        print("All memory systems integrated successfully!")
```

## 6. Advanced Memory Techniques

### 6.1 Working Memory Chunking

```python
# Group related information to increase effective capacity
chunk1 = [pynlm.NeuronId(200), pynlm.NeuronId(201)]  # Chunk 1
chunk2 = [pynlm.NeuronId(202), pynlm.NeuronId(203)]  # Chunk 2

# Store chunks as single units
working_memory.store(chunk1, [0.8, 0.7])  # Store array as value
working_memory.store(chunk2, [0.6, 0.9])
```

### 6.2 Episodic Memory Consolidation Strategies

```python
# Consolidate based on different criteria
episodic_memory.consolidate(relevance_threshold=0.5)
# Consolidates all memories above 0.5 relevance

# Consolidate recent memories
recent = episodic_memory.getRecentEpisodes(10)
for episode in recent:
    # Process for long-term storage
    pass
```

### 6.3 Associative Memory Pattern Completion

```python
# Use partial pattern to complete full memory
partial_pattern = [0.5, 0.0, 0.3]  # Missing some elements
completed = associative_memory.retrieve(partial_pattern)
print(f"Completed pattern: {completed}")
```

## 7. Memory System Performance

### 7.1 Monitoring Memory Usage

```python
def monitor_memory_systems(working_memory, episodic_memory, associative_memory):
    print("Memory System Performance:")
    print(f"  Working Memory:")
    print(f"    - Usage: {working_memory.getCurrentSize()}/{working_memory.capacity}")
    print(f"    - Utilization: {working_memory.getCurrentSize()/max(1, working_memory.capacity):.1%}")
    
    print(f"  Episodic Memory:")
    print(f"    - Episodes: {episodic_memory.getEpisodeCount()}/{episodic_memory.maxEpisodes}")
    print(f"    - Utilization: {episodic_memory.getEpisodeCount()/max(1, episodic_memory.maxEpisodes):.1%}")
    
    print(f"  Associative Memory:")
    print(f"    - Associations: {associative_memory.getAssociationCount()}")
```

### 7.2 Memory Efficiency Analysis

```python
# Calculate memory efficiency metrics
memory_efficiency = {
    'working_memory_efficiency': working_memory.getCurrentSize() / max(1, working_memory.capacity),
    'episodic_memory_efficiency': episodic_memory.getEpisodeCount() / max(1, episodic_memory.maxEpisodes),
    'association_density': associative_memory.getAssociationCount() / max(1, working_memory.getCurrentSize())
}

print(f"Overall memory efficiency: {np.mean(list(memory_efficiency.values())):.1%}")
```

## 8. Troubleshooting Common Memory Issues

### 8.1 Working Memory Problems

**Problem:** Working memory capacity is always full.
**Solution:** Use chunking, reduce storage, or increase capacity.

**Problem:** Information decays too quickly.
**Solution:** Adjust decay rates or use more stable storage mechanisms.

### 8.2 Episodic Memory Problems

**Problem:** Too many memories causing slowdown.
**Solution:** Implement consolidation or increase max episodes.

**Problem:** Cannot retrieve specific memories.
**Solution:** Use proper indexing or search by metadata.

### 8.3 Associative Memory Problems

**Problem:** No associations being formed.
**Solution:** Ensure neurons fire together and coincidence threshold is appropriate.

**Problem:** Pattern completion failing.
**Solution:** Check that associations are properly formed and strengths are reasonable.

## 9. Practical Applications

### 9.1 Working Memory in Decision Making

```python
# Use working memory to hold current options
current_options = [pynlm.NeuronId(301), pynlm.NeuronId(302), pynlm.NeuronId(303)]

# Store option values
for i, neuron_id in enumerate(current_options):
    working_memory.store(neuron_id, decision_values[i])

# Compare options for decision
best_option = None
best_value = -1
for neuron_id in current_options:
    value = working_memory.recall(neuron_id)
    if value > best_value:
        best_value = value
        best_option = neuron_id
```

### 9.2 Episodic Memory in Learning

```python
# Learn from past experiences
episode = pynlm.EpisodicMemoryItem(
    timestamp=current_step,
    neurons=[pynlm.NeuronId(401)],
    values=[outcome_value],
    metadata=f"Action {last_action} resulted in {outcome}"
)

episodic_memory.storeEpisode(episode)

# Retrieve relevant past experiences for decision making
relevant_episodes = episodic_memory.getEpisodesForReplay(5)
# Use these experiences to inform current decisions
```

### 9.3 Associative Memory in Pattern Recognition

```python
# Learn patterns and their associations
pattern = [0.2, 0.8, 0.3]
concept_id = pynlm.NeuronId(501)

# Store pattern with concept
associative_memory.addAssociation(concept_id, pattern, 0.9)

# Later, recognize patterns
if pynlm.EpisodicMemoryItem(
    timestamp=current_step,
    neurons=[concept_id],
    values=[0.9],
    metadata="Recognized pattern"
):
    # Take action based on recognized pattern
    pass
```

## 10. Next Steps

### 10.1 Continue to Tutorial 3

Move on to **Tutorial 3: Neuromodulation and Learning** to learn how neuromodulators (dopamine, curiosity, novelty) control learning and behavior.

### 10.2 Advanced Memory Techniques

Explore the advanced examples:

- `working_memory_example.py` - Detailed working memory demonstrations
- `neuromodulation_example.py` - Neuromodulation with memory systems
- `prediction_planning_example.py` - Memory in prediction and planning

### 10.3 Memory Research

Study the memory-related documentation:

- [Memory Systems Documentation](https://kilo.ai/docs/memory)
- [Working Memory Research](https://kilo.ai/docs/working-memory)
- [Episodic Memory Guide](https://kilo.ai/docs/episodic-memory)

## 11. Summary

Memory systems are fundamental to NLM's intelligence:

1. **Working Memory** provides temporary storage for active manipulation
2. **Episodic Memory** records experiences for long-term learning
3. **Associative Memory** connects concepts for pattern completion

Together, they enable complex cognitive functions like:

- **Attention and focus** through working memory competition
- **Learning from experience** through episodic memory
- **Pattern completion** through associative memory
- **Decision making** by integrating multiple memory systems

---

*Tutorial 2 completed successfully! Ready for Tutorial 3.*