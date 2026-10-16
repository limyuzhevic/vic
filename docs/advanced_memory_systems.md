# Advanced Memory Systems Documentation

## Overview

Memory systems in NLM represent a major advancement from Phase 2 to Phase 4, enabling sophisticated experience storage, retrieval, and pattern discovery. This document covers the advanced memory capabilities available in the enhanced Python bindings.

## Memory System Architecture

### Core Memory Components

NLM implements a three-tier memory architecture:

1. **Working Memory** - Persistent activity for immediate information maintenance
2. **Episodic Memory** - Experience encoding with temporal and spatial indexing  
3. **Associative Memory** - Pattern-based learning and concept formation

### Integration with Neural Processing

```
NEURAL PROCESSING
      ↓
WORKING MEMORY ←→ SYNAPTIC WEIGHTS
      ↓
EPISODIC MEMORY ←→ CONCEPT FORMATION
      ↓
ASSOCIATIVE MEMORY ←→ PREDICTION SYSTEM
      ↓
CIRCUIT DYNAMICS
```

## Working Memory System

### Overview
Working memory maintains information through persistent neural activity, crucial for immediate tasks and planning. Unlike traditional vector stores, working memory is grounded in neural dynamics with competition and decay mechanisms.

### Key Features

#### Persistent Activity
```python
import pynlm

# Create working memory system
config = pynlm.createDefaultConfig()
config.set("brain.working_memory.enable", True)
config.set("brain.working_memory.capacity", 0.8)  # 80% capacity utilization

brain = pynlm.createBrain(config)
brain.initialize()

working_memory = brain.getWorkingMemory()

# Encode current sensory input
signals = pynlm.InternalSignals()
signals.addSignal(0.7)  # Activity level
signals.addSignal(1.0)  # Energy
signals.addSignal(0.3)  # Temperature

# Store with strength factor
working_memory.encode(signals, strength=0.8, duration=2.0)

# Check system state
load = working_memory.getLoad()
capacity = working_memory.getCapacity()
print(f"Working memory load: {load:.2f}/{capacity:.2f}")

# Get active information
active_signals = working_memory.getActiveSignals()
print(f"Active signals: {len(active_signals)} signals")
```

#### Competition Dynamics
```python
# Working memory uses competitive dynamics for selective retention
def store_with_competition(working_memory, new_signals, existing_signals):
    """Store new signals with competition for limited capacity."""
    current_load = working_memory.getLoad()
    
    if current_load + 0.2 <= working_memory.getCapacity():
        # Add with competition
        working_memory.encode(new_signals, strength=0.5)
        print(f"Successfully stored, new load: {working_memory.getLoad():.2f}")
    else:
        # Need to evict weaker signals
        weaker = find_weaker_signals(existing_signals, current_load)
        for signal in weaker:
            working_memory.decaySignal(signal, factor=0.3)
        
        working_memory.encode(new_signals, strength=0.5)
        print(f"Evicted weaker signals, new load: {working_memory.getLoad():.2f}

# Helper function
def find_weaker_signals(signals, threshold):
    """Find signals below activity threshold for eviction."""
    weak_signals = []
    for signal in signals:
        if signal.getStrength() < threshold:
            weak_signals.append(signal)
    return weak_signals

# Application example
def store_visual_scene(working_memory, image_features):
    """Store visual scene with feature-based competition."""
    # Extract features from image
    features = extract_features(image_features)
    
    # Create internal signals
    signals = pynlm.InternalSignals()
    for feature_value in features:
        signals.addSignal(feature_value)
    
    # Store with feature weighting
    working_memory.encode(signals, strength=0.7)
    
    # Retrieve for planning
    active = working_memory.getActiveSignals()
    return plan_from_features(active)
```

### Working Memory Patterns

#### Pattern 1: Working Memory Scheduler
```python
class WorkingMemoryScheduler:
    def __init__(self, brain):
        self.brain = brain
        self.working_memory = brain.getWorkingMemory()
        self.task_queue = []
        self.current_task = None
    
    def add_task(self, task_name, priority=1.0):
        """Add a task to working memory with priority."""
        signals = pynlm.InternalSignals()
        signals.addSignal(priority)
        signals.addSignal(len(self.task_queue) + 1)  # Queue position
        
        self.working_memory.encode(signals, strength=0.6)
        self.task_queue.append(task_name)
        
        print(f"Added task '{task_name}' with priority {priority}")
    
    def get_next_task(self):
        """Get next task using competitive selection."""
        # Check if we have active tasks in working memory
        active_signals = self.working_memory.getActiveSignals()
        
        if not active_signals:
            # No tasks stored, check queue
            if self.task_queue:
                return self.task_queue.pop(0)
            return None
        
        # Retrieve task using competitive selection
        return self.working_memory.retrieveForAction()
    
    def update_task_priority(self, task_name, new_priority):
        """Update priority of existing task."""
        # Decay old task
        self.working_memory.decaySignalForTask(task_name, 0.5)
        
        # Add updated task
        signals = pynlm.InternalSignals()
        signals.addSignal(new_priority)
        
        self.working_memory.encode(signals, strength=0.7)

# Use example
scheduler = WorkingMemoryScheduler(brain)
scheduler.add_task("Navigate_to_goal", priority=1.0)
scheduler.add_task("Avoid_obstacles", priority=0.8)
scheduler.add_task("Explore_new_area", priority=0.3)

next_task = scheduler.get_next_task()
print(f"Next task: {next_task}")
```

#### Pattern 2: Persistent Activity Buffer
```python
class PersistentActivityBuffer:
    def __init__(self, brain):
        self.brain = brain
        self.working_memory = brain.getWorkingMemory()
        self.activity_history = []
    
    def maintain_state(self, step):
        """Maintain persistent state through working memory."""
        # Get current neural state
        firing_pattern = self.brain.getFiringNeuronCount()
        avg_rate = self.brain.getAverageFiringRate()
        
        # Create state signals
        state_signals = pynlm.InternalSignals()
        state_signals.addSignal(firing_pattern)
        state_signals.addSignal(avg_rate)
        state_signals.addSignal(self.brain.getTotalSpikeCount())
        
        # Encode with temporal decay
        self.working_memory.encode(state_signals, strength=0.9, duration=1.0)
        self.activity_history.append({
            'step': step,
            'firing': firing_pattern,
            'rate': avg_rate,
            'spikes': self.brain.getTotalSpikeCount()
        })
    
    def recall_state(self, step_offset):
        """Recall state from working memory history."""
        if not self.activity_history:
            return None
        
        target_step = max(0, len(self.activity_history) - step_offset - 1)
        return self.activity_history[target_step]
    
    def get_state_trajectory(self, steps_back=10):
        """Get trajectory of neural state over time."""
        trajectory = []
        for i in range(min(steps_back, len(self.activity_history))):
            trajectory.append(self.activity_history[-i-1])
        
        return trajectory

# Use example
buffer = PersistentActivityBuffer(brain)
for step in range(100):
    buffer.maintain_state(step)
    if step % 50 == 0:
        recent = buffer.get_state_trajectory(5)
        print(f"State at step {step}:")
        for state in recent:
            print(f"  Step {state['step']}: {state['firing']} firing, {state['rate']:.2f} rate")
```

### Technical Implementation

#### Memory Layout and Storage
```python
# Working memory uses dynamic allocation
working_memory_config = {
    'enable': True,
    'capacity': 0.8,
    'decay_rate': 0.1,
    'competition_strength': 0.7,
    'temporal_horizon': 2.0
}

# Memory is stored as neural patterns
# Each pattern has:
# - Signal values (0-1)
# - Temporal decay factor
# - Competition weight
# - Stability indicator
```

#### Memory Access Patterns
```python
# Read patterns
read_patterns = [
    'sequential',    # Process in order
    'parallel',      # Process all simultaneously
    'hierarchical',  # Process by priority
    'temporal'       # Process by recency
]

# Write patterns
write_patterns = [
    'overwrite',     # Replace existing
    'merge',         # Blend with existing
    'append',        # Add to end
    'replace_weak'   # Replace based on strength
]

# Example: Hierarchical read/write
def hierarchical_memory_operation(working_memory, operation='read'):
    """Perform hierarchical memory operation."""
    if operation == 'read':
        # Read strongest signals first
        active_signals = working_memory.getActiveSignals()
        
        # Sort by strength (highest first)
        sorted_signals = sorted(active_signals, 
                               key=lambda s: s.getStrength(), 
                               reverse=True)
        
        return [s.getSignal() for s in sorted_signals]
    
    elif operation == 'write':
        # Write new signals with competition
        new_signals = create_new_signals()
        
        # Encode with competition
        working_memory.encode(new_signals, strength=0.6)
        return working_memory.getLoad()
```

## Episodic Memory System

### Overview
Episodic memory stores experiences as neural patterns, enabling pattern completion from partial cues. This is biologically inspired by hippocampal function in humans.

### Key Features

#### Experience Encoding
```python
# Create episodic memory system
config = pynlm.createDefaultConfig()
config.set("brain.episodic_memory.enable", True)
config.set("brain.episodic_memory.max_episodes", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

episodic_memory = brain.getEpisodicMemory()

# Encode experience with temporal and spatial context
experience = pynlm.EpisodicExperience(
    timestamp=time.time(),
    position=(10.0, 15.0),
    sensory_input=percept,
    action_taken=action,
    reward_received=reward,
    duration=5.0
)

# Store in episodic memory
episodic_memory.store(experience)

# Encode also in working memory for immediate use
working_memory = brain.getWorkingMemory()
working_memory.encode_from_experience(experience, strength=0.8)

# Get statistics
episode_count = episodic_memory.getEpisodeCount()
print(f"Stored {episode_count} episodes")
```

#### Pattern Completion
```python
# Pattern completion from partial cues
def recall_experience(episodic_memory, cue):
    """Recall episode from partial cue."""
    # Find similar episodes
    similar_episodes = episodic_memory.findSimilar(cue, similarity_threshold=0.7)
    
    if similar_episodes:
        # Pattern completion - reconstruct full episode
        completed = episodic_memory.completePattern(similar_episodes[0], cue)
        return completed
    else:
        return None

# Example usage
def simulate_agent_exploration(brain, num_steps=100):
    """Simulate agent exploring and storing experiences."""
    episodic_memory = brain.getEpisodicMemory()
    working_memory = brain.getWorkingMemory()
    
    for step in range(num_steps):
        # Random exploration
        x = random.uniform(0, 50)
        y = random.uniform(0, 50)
        
        # Create percept
        percept = create_percept(x, y)
        
        # Store in working memory
        working_memory.encode(percept, strength=0.5)
        
        # Occasionally store in episodic memory
        if step % 10 == 0:
            experience = pynlm.EpisodicExperience(
                timestamp=time.time(),
                position=(x, y),
                sensory_input=percept,
                action_taken=None,
                reward_received=1.0 if step % 20 == 0 else 0.0,
                duration=1.0
            )
            episodic_memory.store(experience)
        
        # Try to recall based on cue
        cue = create_cue_from_environment(x, y)
        recalled = recall_experience(episodic_memory, cue)
        
        if recalled:
            print(f"Recalled episode from cue: {recalled.getSummary()}")

# Use simulation
brain = pynlm.createBrain(config)
brain.initialize()
simulate_agent_exploration(brain, num_steps=50)
```

#### Memory Replay
```python
# Replay mechanism for consolidation
def replay_episodes(episodic_memory, start_step=0, end_step=None):
    """Replay episodes for memory consolidation."""
    if end_step is None:
        end_step = episodic_memory.getEpisodeCount()
    
    # Get episodes in time range
    episodes = episodic_memory.getEpisodesInRange(start_step, end_step)
    
    for episode in episodes:
        # Replay with modified weights
        replay_strength = 0.7  # Lower than encoding
        replay_error = 0.1  # Small error for learning
        
        # Process through neural system
        process_replay(episode, replay_strength, replay_error)
    
    return len(episodes)

# Consolidation callback
class MemoryConsolidation:
    def __init__(self, brain):
        self.brain = brain
        self.episodic_memory = brain.getEpisodicMemory()
        self.working_memory = brain.getWorkingMemory()
    
    def consolidate_experiences(self, trigger_event):
        """Consolidate memories based on trigger events."""
        # Trigger events: critical moments, errors, rewards
        if trigger_event == 'reward':
            # Replay rewarded experiences
            return self.replay_rewarded_experiences()
        elif trigger_event == 'error':
            # Replay error experiences
            return self.replay_error_experiences()
        elif trigger_event == 'sleep':
            # Full consolidation during sleep-like state
            return self.full_consolidation()
        else:
            return 0
    
    def replay_rewarded_experiences(self):
        """Replay experiences that led to rewards."""
        # Find episodes with positive rewards
        rewarded_episodes = self.episodic_memory.getEpisodesWithReward(1.0)
        
        # Replay with higher strength for reinforcement
        for episode in rewarded_episodes:
            self.process_replay(episode, replay_strength=0.8, replay_error=0.05)
        
        return len(rewarded_episodes)

# Use consolidation
system = MemoryConsolidation(brain)
consolidated = system.consolidate_experiences('reward')
print(f"Consolidated {consolidated} rewarded experiences")
```

#### Temporal Indexing
```python
# Temporal indexing and querying
def temporal_queries(episodic_memory):
    """Perform temporal queries on episodic memory."""
    # Query 1: Find episodes before time T
    episodes_before = episodic_memory.getEpisodesBefore(100.0)
    
    # Query 2: Find episodes in time window
    episodes_in_window = episodic_memory.getEpisodesInTimeWindow(50.0, 150.0)
    
    # Query 3: Find most similar to pattern
    pattern = create_pattern_from_cue(cue)
    similar_episodes = episodic_memory.findSimilar(pattern, limit=5)
    
    # Query 4: Get episode sequence
    sequence = episodic_memory.getEpisodeSequence()
    
    return {
        'before': episodes_before,
        'window': episodes_in_window,
        'similar': similar_episodes,
        'sequence': sequence
    }

# Example
def agent_journey_analysis(brain):
    """Analyze agent's journey through episodic memory."""
    episodic_memory = brain.getEpisodicMemory()
    
    # Get temporal queries
    queries = temporal_queries(episodic_memory)
    
    print(f"Agent has {len(queries['sequence'])} episodes")
    print(f"Explored areas before time 100: {len(queries['before'])} episodes")
    print(f"Explored in time window 50-150: {len(queries['window'])} episodes")
    
    # Find most explored area
    if queries['similar']:
        most_similar = queries['similar'][0]
        print(f"Most similar episode: {most_similar.getPosition()}")
    
    return queries

# Use analysis
brain = pynlm.createBrain(config)
brain.initialize()
journey = agent_journey_analysis(brain)
```

### Technical Implementation

#### Storage Format
```python
# Episodic memory storage structure
episodic_memory_entry = {
    'timestamp': float,  # Time of experience
    'position': (float, float),  # Spatial coordinates
    'sensory_pattern': list,  # Neural pattern of percept
    'action_taken': dict,  # Action representation
    'reward_received': float,  # Reward magnitude
    'duration': float,  # Duration of episode
    'strength': float,  # Memory strength
    'reinforcement_count': int  # How many times replayed
}
```

#### Pattern Completion Algorithm
```python
# Pattern completion uses similarity matching
def pattern_completion(target_pattern, stored_patterns, threshold=0.7):
    """Complete pattern from partial target using stored patterns."""
    similarities = []
    
    for stored in stored_patterns:
        similarity = calculate_pattern_similarity(target_pattern, stored)
        if similarity >= threshold:
            similarities.append((similarity, stored))
    
    # Return most similar pattern
    if similarities:
        similarities.sort(reverse=True)
        return similarities[0][1]  # Return pattern
    
    return None

# Similarity calculation
def calculate_pattern_similarity(pattern1, pattern2):
    """Calculate similarity between two patterns."""
    # Normalize patterns
    norm1 = normalize_pattern(pattern1)
    norm2 = normalize_pattern(pattern2)
    
    # Calculate cosine similarity
    dot_product = sum(a * b for a, b in zip(norm1, norm2))
    magnitude1 = math.sqrt(sum(a ** 2 for a in norm1))
    magnitude2 = math.sqrt(sum(a ** 2 for a in norm2))
    
    if magnitude1 * magnitude2 == 0:
        return 0.0
    
    return dot_product / (magnitude1 * magnitude2)
```

## Associative Memory System

### Overview
Associative memory creates connections between experiences and their outcomes, enabling learned associations and concept formation. This system is inspired by Hebbian learning and pattern completion in cortex.

### Key Features

#### Pattern Association
```python
# Create associative memory system
config = pynlm.createDefaultConfig()
config.set("brain.associative_memory.enable", True)
config.set("brain.associative_memory.association_strength", 0.5)

brain = pynlm.createBrain(config)
brain.initialize()

associative_memory = brain.getAssociativeMemory()

# Form association between cue and outcome
cue_pattern = create_pattern_from_input("red_apple")
outcome_pattern = create_pattern_from_reward(1.0)

associative_memory.form_association(cue_pattern, outcome_pattern, strength=0.8)

# Retrieve outcome from cue
retrieved_outcome = associative_memory.retrieve(cue_pattern)
print(f"Retrieved outcome: {retrieved_outcome}")

# Get statistics
pattern_count = associative_memory.getPatternCount()
association_count = associative_memory.getAssociationCount()
print(f"Patterns: {pattern_count}, Associations: {association_count}")
```

#### Concept Formation
```python
# Concept formation from multiple experiences
class ConceptFormation:
    def __init__(self, brain):
        self.brain = brain
        self.associative_memory = brain.getAssociativeMemory()
        self.concepts = {}
    
    def learn_from_experience(self, experience):
        """Learn concept from experience."""
        # Extract features from experience
        features = extract_features_from_experience(experience)
        
        # Check if we have similar concepts
        similar_concept = self.find_similar_concept(features)
        
        if similar_concept:
            # Update existing concept
            similar_concept.update(features)
            concept_id = similar_concept.id
            print(f"Updated concept '{similar_concept.name}' with new features")
        else:
            # Create new concept
            concept_id = self.create_new_concept(features, experience)
            print(f"Created new concept '{self.concepts[concept_id].name}'")
        
        return concept_id
    
    def find_similar_concept(self, features):
        """Find concept similar to given features."""
        best_match = None
        best_similarity = 0.0
        
        for concept_id, concept in self.concepts.items():
            similarity = calculate_feature_similarity(
                features, concept.features
            )
            
            if similarity > best_similarity and similarity > 0.7:
                best_similarity = similarity
                best_match = concept
        
        return best_match
    
    def create_new_concept(self, features, experience):
        """Create new concept from features."""
        concept_id = f"concept_{len(self.concepts)}"
        
        self.concepts[concept_id] = Concept(
            id=concept_id,
            name=f"Concept_{concept_id}",
            features=features,
            prototype=features,
            stability=1.0,
            experience_count=1
        )
        
        # Store concept prototype in associative memory
        prototype_pattern = create_pattern_from_features(features)
        self.associative_memory.store_pattern(prototype_pattern)
        
        return concept_id

# Concept class
class Concept:
    def __init__(self, id, name, features, prototype, stability, experience_count):
        self.id = id
        self.name = name
        self.features = features
        self.prototype = prototype
        self.stability = stability
        self.experience_count = experience_count
    
    def update(self, new_features):
        """Update concept with new features."""
        # Update prototype (running average)
        self.prototype = [
            (old + new) / 2 for old, new in zip(self.prototype, new_features)
        ]
        
        # Update features (add new)
        self.features.extend(new_features)
        
        # Update stability
        self.stability = min(1.0, self.stability * 0.9 + 0.1)
        
        # Update experience count
        self.experience_count += 1
    
    def get_stability(self):
        """Get concept stability (0-1)."""
        return self.stability
    
    def matches(self, features):
        """Check if features match this concept."""
        return calculate_feature_similarity(
            features, self.prototype
        ) > self.stability * 0.8

# Helper functions
def extract_features_from_experience(experience):
    """Extract features from experience for concept formation."""
    features = []
    
    # Extract from position
    pos = experience.getPosition()
    features.extend([pos[0], pos[1]])
    
    # Extract from sensory input
    percept = experience.getSensoryInput()
    if percept.getVision():
        vision_data = percept.getVision().getData()
        features.extend(vision_data[:10])  # First 10 pixels as features
    
    # Extract from reward
    reward = experience.getReward()
    features.append(reward)
    
    return features

def calculate_feature_similarity(features1, features2):
    """Calculate similarity between feature sets."""
    if len(features1) != len(features2):
        return 0.0
    
    # Calculate cosine similarity
    dot_product = sum(a * b for a, b in zip(features1, features2))
    magnitude1 = math.sqrt(sum(a ** 2 for a in features1))
    magnitude2 = math.sqrt(sum(a ** 2 for a in features2))
    
    if magnitude1 * magnitude2 == 0:
        return 0.0
    
    return dot_product / (magnitude1 * magnitude2)

# Use concept formation
brain = pynlm.createBrain(config)
brain.initialize()

concept_formation = ConceptFormation(brain)

# Learn from experiences
for i in range(10):
    experience = create_experience(i)
    concept_id = concept_formation.learn_from_experience(experience)
    
    # Test concept matching
    if i % 3 == 0:  # Test every 3rd experience
        test_features = extract_features_from_experience(experience)
        matching_concepts = [
            cid for cid, concept in concept_formation.concepts.items()
            if concept.matches(test_features)
        ]
        print(f"Experience {i} matches concepts: {matching_concepts}")
```

#### Pattern Mining
```python
# Pattern mining from experiences
class PatternMiner:
    def __init__(self, brain):
        self.brain = brain
        self.associative_memory = brain.getAssociativeMemory()
        self.pattern_database = {}
    
    def mine_patterns(self, experiences):
        """Mine patterns from experiences."""
        patterns = []
        
        # Simple pattern mining: extract frequent features
        feature_counts = {}
        for exp in experiences:
            features = extract_features_from_experience(exp)
            
            for feature in features:
                if feature not in feature_counts:
                    feature_counts[feature] = 0
                feature_counts[feature] += 1
        
        # Extract frequent patterns
        threshold = len(experiences) * 0.3  # 30% frequency threshold
        frequent_features = [
            feature for feature, count in feature_counts.items()
            if count >= threshold
        ]
        
        # Create pattern entries
        for i, feature in enumerate(frequent_features):
            pattern_id = f"pattern_{i}"
            pattern = {
                'id': pattern_id,
                'features': [feature],
                'frequency': feature_counts[feature],
                'support': feature_counts[feature] / len(experiences)
            }
            
            patterns.append(pattern)
            self.pattern_database[pattern_id] = pattern
        
        return patterns
    
    def get_patterns_for_query(self, query_features):
        """Get patterns similar to query."""
        similar_patterns = []
        
        for pattern_id, pattern in self.pattern_database.items():
            similarity = calculate_pattern_similarity(
                query_features, pattern['features']
            )
            
            if similarity > 0.5:  # Similarity threshold
                pattern['similarity'] = similarity
                similar_patterns.append(pattern)
        
        # Sort by similarity
        similar_patterns.sort(key=lambda p: p['similarity'], reverse=True)
        return similar_patterns

# Use pattern mining
brain = pynlm.createBrain(config)
brain.initialize()

pattern_miner = PatternMiner(brain)

# Create some experiences
experiences = []
for i in range(20):
    experiences.append(create_experience(i))

# Mine patterns
patterns = pattern_miner.mine_patterns(experiences)
print(f"Mined {len(patterns)} patterns")

# Query patterns
test_features = extract_features_from_experience(experiences[0])
similar_patterns = pattern_miner.get_patterns_for_query(test_features)

print("Similar patterns for test experience:")
for pattern in similar_patterns[:3]:
    print(f"  Pattern {pattern['id']}: similarity={pattern['similarity']:.2f}")
```

## Memory Integration

### Memory Systems in Brain Loop

#### Complete Memory Integration
```python
# Create brain with full memory system
config = pynlm.createDefaultConfig()
config.set("brain.working_memory.enable", True)
config.set("brain.episodic_memory.enable", True)
config.set("brain.associative_memory.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Get all memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Create advanced agent with memory integration
agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
world.reset()
agent.initialize(world)

# Run simulation with memory integration
for step in range(200):
    # Update world
    world.update(0.1)
    
    # Get percept
    percept = world.getSensoryPercept()
    
    # Process through memory systems
    # 1. Working memory: maintain current state
    working_memory.encode(percept, strength=0.7)
    
    # 2. Episodic memory: store experience periodically
    if step % 20 == 0:
        experience = pynlm.EpisodicExperience(
            timestamp=time.time(),
            position=world.getAgentPosition(),
            sensory_input=percept,
            action_taken=None,  # Will be set after action
            reward_received=0.0,
            duration=1.0
        )
        episodic_memory.store(experience)
    
    # 3. Agent processing
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # 4. Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # 5. Apply reward (sets action taken)
    reward = 1.0 if action.get_type() == pynlm.ActionType.Eat else 0.0
    
    # Update the last experience with action and reward
    if step % 20 == 19:
        last_experience = episodic_memory.getLastEpisode()
        if last_experience:
            last_experience.setActionTaken(action)
            last_experience.setRewardReceived(reward)
            episodic_memory.updateEpisode(last_experience)
    
    # 6. Apply neuromodulation
    predicted = agent.getPredictionError()
    agent.applyRewardModulation(reward, predicted)
    
    # 7. Update development
    agent.updateDevelopment(0.1)
    
    # 8. Memory consolidation
    if step % 50 == 0:
        consolidate_memories(brain)
    
    # 9. Log memory statistics
    if step % 25 == 0:
        log_memory_stats(working_memory, episodic_memory, associative_memory)

# Memory consolidation function
def consolidate_memories(brain):
    """Consolidate all memory systems."""
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    # Working memory decay
    working_memory.decay_all(0.8)
    
    # Episodic memory replay
    episodes = episodic_memory.getAllEpisodes()
    for episode in episodes:
        # Replay with lower strength
        process_replay(episode, replay_strength=0.6, replay_error=0.1)
    
    # Pattern stabilization in associative memory
    patterns = associative_memory.getAllPatterns()
    for pattern in patterns:
        pattern.stabilize()

# Log memory statistics
def log_memory_stats(working_memory, episodic_memory, associative_memory):
    """Log comprehensive memory statistics."""
    print(f"Memory Statistics:")
    print(f"  Working memory load: {working_memory.getLoad():.2f}")
    print(f"  Episodic episodes: {episodic_memory.getEpisodeCount()}")
    print(f"  Associative patterns: {associative_memory.getPatternCount()}")
    
    # Get concept statistics
    concept_formation = brain.getConceptFormation()
    print(f"  Concepts: {concept_formation.getConceptCount()}")
    print(f"  Concept stability: {concept_formation.getAverageStability():.2f}")

# Run complete integration simulation
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize components
agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
world.reset()
agent.initialize(world)

# Run simulation with memory integration
for step in range(200):
    # Update world
    world.update(0.1)
    
    # Get percept
    percept = world.getSensoryPercept()
    
    # Process through memory systems
    working_memory = brain.getWorkingMemory()
    working_memory.encode(percept, strength=0.7)
    
    episodic_memory = brain.getEpisodicMemory()
    if step % 20 == 0:
        experience = pynlm.EpisodicExperience(
            timestamp=time.time(),
            position=world.getAgentPosition(),
            sensory_input=percept,
            action_taken=None,
            reward_received=0.0,
            duration=1.0
        )
        episodic_memory.store(experience)
    
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    reward = 1.0 if action.get_type() == pynlm.ActionType.Eat else 0.0
    
    if step % 20 == 19:
        last_experience = episodic_memory.getLastEpisode()
        if last_experience:
            last_experience.setActionTaken(action)
            last_experience.setRewardReceived(reward)
            episodic_memory.updateEpisode(last_experience)
    
    predicted = agent.getPredictionError()
    agent.applyRewardModulation(reward, predicted)
    
    agent.updateDevelopment(0.1)
    
    if step % 50 == 0:
        consolidate_memories(brain)
    
    if step % 25 == 0:
        log_memory_stats(working_memory, episodic_memory, associative_memory)

print("Memory integration simulation complete!")
```