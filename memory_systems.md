# memory_systems.md

# NLM Memory Systems Guide

## Overview

Memory in NLM is implemented through three interconnected systems:

1. **Working Memory (NeuralWorkingMemory)**: Temporary storage of current information
2. **Episodic Memory (NeuralEpisodicMemory)**: Storage of past experiences
3. **Associative Memory (NeuralAssociativeMemory)**: Connections between experiences

These systems work together to enable learning, prediction, and intelligent behavior through emergent neural dynamics.

## Memory Architecture

### NeuralWorkingMemory (src/memory/NeuralWorkingMemory.hpp)

**Purpose**: Temporary storage of current sensory input, goals, and intermediate processing results.

**Key Features**:
- Persistent neural activity without continuous input
- Competitive dynamics for selective attention
- Rapid encoding and decay of information
- Integration with attention and prediction systems

**Mechanisms**:
- **Persistent Activity**: Recurrent neural connections maintain information
- **Winner-Take-Most**: Competition between memory traces
- **Activity Decay**: Weak traces decay over time
- **Rehearsal**: Strong traces are strengthened through replay

### NeuralEpisodicMemory (src/memory/NeuralEpisodicMemory.hpp)

**Purpose**: Storage of complete experiences with temporal, spatial, and causal associations.

**Key Features**:
- Experience encoding with full sensory details
- Temporal organization of memories
- Spatial associations through place cells
- Replay mechanisms for consolidation
- Relevance-weighted retrieval

**Mechanisms**:
- **Pattern Completion**: Retrieve full episodes from partial cues
- **Temporal Indexing**: Organize memories by time of occurrence
- **Spatial Association**: Link memories to locations
- **Relevance Tracking**: Track how useful memories are
- **Replay**: Reactivate patterns during offline periods

### NeuralAssociativeMemory (src/memory/NeuralAssociativeMemory.hpp)

**Purpose**: Learn associations between patterns and experiences.

**Key Features**:
- Hebbian learning between neural patterns
- Spreading activation for recall
- Association strength modification through experience
- Integration with episodic and working memory

**Mechanisms**:
- **Hebbian Associations**: "Neurons that fire together wire together"
- **Pattern Completion**: Retrieve associated memories
- **Strength Modulation**: Adjust association strength based on rewards
- **Spreading Activation**: Activate related memories

## Memory Integration with Other Systems

### Working Memory Integration

Working memory is updated every simulation step and influences:

```cpp
void NeuralWorkingMemory::update(float dt) {
    // Decay weak traces
    for (auto& trace : traces) {
        trace.activation *= (1.0f - decay_rate * dt);
    }
    
    // Competition among traces
    runCompetition();
    
    // Rehearse important traces
    if (should_rehearse()) {
        strengthenRecentTraces();
    }
}
```

### Episodic Memory Integration

Episodic memory is updated every 10 steps and influences:

```cpp
void NeuralEpisodicMemory::storeEpisode(const Episode& episode) {
    // Encode episode as neural pattern
    neural_pattern = encodeSensoryData(episode.sensory_data);
    
    // Store with temporal and spatial context
    episodes.push_back(Episode {
        .pattern = neural_pattern,
        .timestamp = current_time,
        .location = current_position,
        .action = last_action,
        .reward = episode.reward
    });
}
```

### Associative Memory Integration

Associative memory is updated during learning and influences:

```cpp
void NeuralAssociativeMemory::recordExperience(const Experience& exp) {
    // Form new associations
    strength = calculateAssociationStrength(exp);
    
    // Store association
    associations.push_back(Association {
        .pattern_a = exp.before_state,
        .pattern_b = exp.after_state,
        .strength = strength,
        .reward = exp.reward
    });
}
```

## Memory System APIs

### NeuralWorkingMemory API

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize working memory
wm = pynlm.NeuralWorkingMemory()
wm.initialize(brain)

# Configure working memory
wm.setCapacity(100)  # Maximum number of memory traces
wm.setDecayRate(0.01)  # Decay per simulation step

# Store information
pattern1 = [0.1, 0.5, 0.9, 0.3]
wm.store(pattern1, strength=1.0)

pattern2 = [0.9, 0.8, 0.7, 0.6]
wm.store(pattern2, strength=0.8)

# Store specific neuron activations
neuron_id = 42
wm.storeToNeuron(neuron_id, activation=0.7)

# Retrieve working memory content
activity_levels = wm.retrieve()
print(f"Working memory activity: {len(activity_levels)} traces")

# Check what neurons are in working memory
is_in_memory = wm.contains(neuron_id)
activation_level = wm.getNeuronActivation(neuron_id)

# Update working memory (maintain/decay)
wm.update(dt=0.1)

# Competition between memory traces
wm.runCompetition()
winning_neuron = wm.isWinning(neuron_id)

# Rehearse important memories
wm.strengthenMemory(factor=1.5)

# Access statistics
active_traces = wm.getActiveTraces()
current_capacity = wm.getCapacity()
current_decay = wm.getDecayRate()
total_activity = wm.getMemoryActivity()
```

### NeuralEpisodicMemory API

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize episodic memory
mem = pynlm.NeuralEpisodicMemory()
mem.initialize(brain)

# Configure memory limits
mem.setMaxEpisodes(1000)
mem.enableReplay(True)

# Create episode data
episode1 = []  # List of sensory observations over time
episode1.append(pynlm.Vision(8, 8, 3))
episode1.append(pynlm.Vision(8, 8, 3))
episode1.append(pynlm.Vision(8, 8, 3))

mem.storeEpisode(episode1)

# Create more episodes
for i in range(5):
    episode = []
    for j in range(10):
        # Create random sensory pattern
        data = [0.1 * j + 0.05 * (i % 3) for _ in range(32)]
        vision = pynlm.Vision(8, 8, 3)
        vision.setData(data)
        episode.append(vision)
    mem.storeEpisode(episode)

# Retrieve similar episodes
query = pynlm.Vision(8, 8, 3)
similar_episodes = mem.retrieveSimilar(query, maxResults=5)
print(f"Found {len(similar_episodes)} similar episodes")

# Temporal retrieval
temporal_episodes = mem.retrieveTemporal(startTime=100, endTime=200, maxResults=10)
print(f"Retrieved {len(temporal_episodes)} episodes from time 100-200")

# Spatial retrieval
location_episodes = mem.retrieveByLocation(x=10.0, y=10.0, radius=2.0, maxResults=5)
print(f"Found {len(location_episodes)} episodes at location (10, 10)")

# Action-based retrieval
action_episodes = mem.retrieveAfterAction(pynlm.ActionType.MoveForward, maxResults=3)
print(f"Found {len(action_episodes)} episodes after moving forward")

# Replay specific episode
mem.replayEpisode(episode1)

# Replay sequence for consolidation
important_episodes = mem.getEpisodesForReplay(10)
mem.replaySequence(important_episodes)

# Memory management
mem.updateRelevance(episode_id, relevanceDelta=0.1)
mem.consolidate(relevanceThreshold=0.5)

# Access memory statistics
total_episodes = mem.getEpisodeCount()
recent = mem.getRecentEpisodes(5)
average_reward = mem.getAverageReward()
```

### NeuralAssociativeMemory API

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize associative memory
am = pynlm.NeuralAssociativeMemory()
am.initialize(brain)

# Record experiences
exp1 = {
    'before_state': [0.1, 0.2, 0.3, 0.4],
    'after_state': [0.2, 0.4, 0.6, 0.8],
    'reward': 1.0,
    'action': pynlm.ActionType.MoveForward
}
am.recordExperience(exp1)

exp2 = {
    'before_state': [0.8, 0.7, 0.6, 0.5],
    'after_state': [0.7, 0.5, 0.3, 0.1],
    'reward': 0.5,
    'action': pynlm.ActionType.TurnLeft
}
am.recordExperience(exp2)

# Retrieve associations
associations = am.getAssociations()
for assoc in associations:
    print(f"Association strength: {assoc['strength']:.3f}")
    print(f"Reward: {assoc['reward']}")

# Update association strength
am.updateAssociationStrength(association_id, new_strength)

# Clear associations
am.clear()
```

## Memory System Examples

### Example 1: Working Memory in Sensorimotor Integration

```python
import pynlm

def working_memory_demo():
    """Demonstrate working memory in sensorimotor tasks."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Initialize agent
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    agent.initialize(world)
    
    # Initialize working memory
    wm = pynlm.NeuralWorkingMemory()
    wm.initialize(brain)
    wm.setCapacity(50)
    
    # Simulate a task requiring working memory
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent
        agent.processSensoryInput(percept)
        
        # Store sensory input in working memory
        wm.store(percept.getVision().getData())
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print working memory statistics
        if step % 20 == 0:
            print(f"Step {step}: Working memory traces = {wm.getActiveTraces()}")
            print(f"  Total activity: {wm.getMemoryActivity():.3f}")
    
    print("Working memory demo completed!")

working_memory_demo()
```

### Example 2: Episodic Memory for Navigation

```python
import pynlm

def episodic_memory_navigation_demo():
    """Demonstrate episodic memory in navigation tasks."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("memory.episodic_memory.max_episodes", 500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Initialize episodic memory
    mem = pynlm.NeuralEpisodicMemory()
    mem.initialize(brain)
    mem.enableReplay(True)
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    agent.initialize(world)
    
    # Navigate through environment
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    for episode in range(5):
        print(f"\nNavigation Episode {episode}")
        
        # Navigate to random goal
        goal_x, goal_y = 25.0, 25.0
        steps = 0
        
        while steps < 200:
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process with agent
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(steps)
            
            # Decode motor command
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Get current position
            body = world.getAgentBody()
            
            # Store position as episodic memory
            episode_data = []
            episode_data.append(pynlm.Vision(8, 8, 3))
            mem.storeEpisode(episode_data)
            
            # Check if reached goal
            distance = ((body.x - goal_x)**2 + (body.y - goal_y)**2)**0.5
            if distance < 1.0:
                print(f"  Goal reached in {steps} steps")
                break
            
            # Update development
            agent.updateDevelopment(0.1)
            
            steps += 1
    
    # Analyze memory usage
    print(f"\nMemory Statistics:")
    print(f"  Total episodes stored: {mem.getEpisodeCount()}")
    print(f"  Recent episodes: {mem.getRecentEpisodes(3)}")
    print(f"  Average reward: {mem.getAverageReward():.3f}")
    
    # Replay learned episodes
    print(f"\nReplaying learned episodes...")
    learned_episodes = mem.getEpisodesForReplay(3)
    for i, episode_id in enumerate(learned_episodes):
        print(f"  Episode {i+1}: Replay successful")
    
    print("Navigation demo completed!")

episodic_memory_navigation_demo()
```

### Example 3: Associative Memory for Task Learning

```python
import pynlm

def associative_memory_learning_demo():
    """Demonstrate associative memory in task learning."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Initialize associative memory
    am = pynlm.NeuralAssociativeMemory()
    am.initialize(brain)
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    agent.initialize(world)
    
    # Define task patterns
    task_patterns = {
        'move_forward': {
            'before': [0.1, 0.2, 0.3, 0.4],
            'after': [0.2, 0.4, 0.6, 0.8],
            'reward': 1.0
        },
        'turn_left': {
            'before': [0.8, 0.7, 0.6, 0.5],
            'after': [0.7, 0.5, 0.3, 0.1],
            'reward': 0.8
        },
        'turn_right': {
            'before': [0.9, 0.8, 0.7, 0.6],
            'after': [0.8, 0.6, 0.4, 0.2],
            'reward': 0.6
        }
    }
    
    # Learn task associations
    print("Learning task associations...")
    for task_name, pattern in task_patterns.items():
        am.recordExperience(pattern)
        print(f"  Learned {task_name}: strength = {pattern['reward']}")
    
    # Test learned associations
    print("\nTesting learned associations...")
    for test_task in ['move_forward', 'turn_left', 'turn_right']:
        # Simulate action
        before = task_patterns[test_task]['before']
        
        # Use associative memory to predict outcome
        association = am.getAssociation(before, task_patterns[test_task]['after'])
        
        if association:
            predicted_reward = association['reward']
            expected_after = task_patterns[test_task]['after']
            
            print(f"  {test_task}:")
            print(f"    Predicted reward: {predicted_reward:.3f}")
            print(f"    Expected outcome: {expected_after}")
    
    # Clear and relearn with different rewards
    print("\nRelearning with modified rewards...")
    for task_name, pattern in task_patterns.items():
        pattern['reward'] *= 0.5  # Reduce reward
        am.recordExperience(pattern)
        print(f"  Updated {task_name}: new strength = {pattern['reward']}")
    
    print("Associative memory demo completed!")

associative_memory_learning_demo()
```

## Memory System Configuration

### Working Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_memory.capacity` | int | 100 | Maximum number of memory traces |
| `memory.working_memory.decay_rate` | float | 0.01 | Decay rate per simulation step |
| `memory.working_memory.rehearsal_enabled` | bool | true | Enable memory rehearsal |

### Episodic Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.episodic_memory.max_episodes` | int | 1000 | Maximum episodes stored |
| `memory.episodic_memory.replay_interval` | int | 100 | Steps between replays |
| `memory.episodic_memory.consolidation_interval` | int | 1000 | Steps between consolidation |
| `memory.episodic_memory.pattern_completion_threshold` | float | 0.7 | Similarity threshold for recall |

### Associative Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.associative_memory.association_strength` | float | 0.5 | Default association strength |
| `memory.associative_memory.spreading_activation` | float | 0.8 | Strength of spreading activation |
| `memory.associative_memory.learning_rate` | float | 0.01 | Rate of association learning |

## Memory System Analysis

### Working Memory Analysis

```python
import pynlm

def analyze_working_memory(wm_log):
    """Analyze working memory usage and performance."""
    
    # Calculate key metrics
    traces_over_time = [entry['active_traces'] for entry in wm_log]
    activity_over_time = [entry['total_activity'] for entry in wm_log]
    
    # Find memory peaks
    max_traces = max(traces_over_time)
    max_activity = max(activity_over_time)
    avg_traces = sum(traces_over_time) / len(traces_over_time)
    
    # Calculate memory efficiency
    memory_efficiency = avg_traces / max(1, len(wm_log))
    
    # Analyze decay patterns
    decay_rates = []
    for i in range(1, len(traces_over_time)):
        if traces_over_time[i-1] > 0:
            decay_rate = (traces_over_time[i] - traces_over_time[i-1]) / traces_over_time[i-1]
            decay_rates.append(decay_rate)
    
    avg_decay_rate = sum(decay_rates) / len(decay_rates) if decay_rates else 0
    
    # Generate analysis report
    report = {
        'max_traces': max_traces,
        'max_activity': max_activity,
        'average_traces': avg_traces,
        'memory_efficiency': memory_efficiency,
        'average_decay_rate': avg_decay_rate,
        'total_steps_analyzed': len(wm_log)
    }
    
    return report
```

### Episodic Memory Analysis

```python
import pynlm

def analyze_episodic_memory(mem_log):
    """Analyze episodic memory storage and retrieval."""
    
    # Extract episodic memory metrics
    episodes_over_time = [entry['episode_count'] for entry in mem_log]
    replay_frequency = [entry['replay_count'] for entry in mem_log]
    avg_reward = [entry['average_reward'] for entry in mem_log]
    
    # Calculate memory statistics
    total_episodes = max(episodes_over_time)
    total_replays = sum(replay_frequency)
    avg_recall_accuracy = sum([entry['recall_accuracy'] for entry in mem_log]) / len(mem_log)
    
    # Analyze consolidation patterns
    consolidation_events = [entry['consolidation_event'] for entry in mem_log]
    consolidation_frequency = sum(consolidation_events) / len(mem_log)
    
    # Analyze episode characteristics
    episode_lengths = [entry['episode_length'] for entry in mem_log]
    avg_episode_length = sum(episode_lengths) / len(episode_lengths)
    
    # Generate analysis report
    report = {
        'total_episodes': total_episodes,
        'total_replays': total_replays,
        'replay_frequency': consolidation_frequency,
        'average_recall_accuracy': avg_recall_accuracy,
        'average_episode_length': avg_episode_length,
        'memory_stability': avg_recall_accuracy,
        'storage_efficiency': total_episodes / max(1, len(mem_log))
    }
    
    return report
```

### Memory System Integration Analysis

```python
import pynlm

def analyze_memory_integration(memory_log):
    """Analyze integration between memory systems."""
    
    # Correlate memory systems
    wm_activity = [entry['working_memory_activity'] for entry in memory_log]
    em_recall = [entry['episodic_memory_recall'] for entry in memory_log]
    am_strength = [entry['associative_memory_strength'] for entry in memory_log]
    
    # Calculate correlations
    import numpy as np
    
    wm_em_correlation = np.corrcoef(wm_activity, em_recall)[0, 1]
    wm_am_correlation = np.corrcoef(wm_activity, am_strength)[0, 1]
    em_am_correlation = np.corrcoef(em_recall, am_strength)[0, 1]
    
    # Find synergy effects
    synergy_score = (wm_em_correlation + wm_am_correlation + em_am_correlation) / 3
    
    # Analyze temporal dynamics
    temporal_patterns = []
    for i in range(1, len(memory_log)):
        pattern = {
            'step': memory_log[i]['step'],
            'wm_change': memory_log[i]['working_memory_activity'] - memory_log[i-1]['working_memory_activity'],
            'em_change': memory_log[i]['episodic_memory_recall'] - memory_log[i-1]['episodic_memory_recall'],
            'am_change': memory_log[i]['associative_memory_strength'] - memory_log[i-1]['associative_memory_strength'],
            'prediction_error': memory_log[i]['prediction_error']
        }
        temporal_patterns.append(pattern)
    
    # Identify key integration events
    integration_events = []
    for pattern in temporal_patterns:
        if abs(pattern['wm_change']) > 0.5 or abs(pattern['em_change']) > 0.5 or abs(pattern['am_change']) > 0.5:
            integration_events.append(pattern)
    
    return {
        'correlations': {
            'wm_em': wm_em_correlation,
            'wm_am': wm_am_correlation,
            'em_am': em_am_correlation,
            'synergy_score': synergy_score
        },
        'integration_events': len(integration_events),
        'temporal_patterns': temporal_patterns[:100]  # First 100 for analysis
    }
```

## Memory System Troubleshooting

### Common Memory Issues and Solutions

#### 1. Working Memory Overflow
**Problem**: Working memory exceeds capacity, causing interference.

**Symptoms**:
- High working memory activity levels
- Poor task performance
- Forgetting of earlier information

**Solutions**:
```python
# Increase capacity
wm.setCapacity(200)

# Reduce decay rate
wm.setDecayRate(0.005)

# Add rehearsal
wm.enableRehearsal(True)
```

#### 2. Episodic Memory Saturation
**Problem**: Episodic memory reaches maximum capacity, preventing new learning.

**Symptoms**:
- Memory full messages
- Older episodes not accessible
- Poor learning of new experiences

**Solutions**:
```python
# Increase memory capacity
mem.setMaxEpisodes(5000)

# Enable consolidation
mem.consolidate(relevanceThreshold=0.3)

# Enable replay for consolidation
mem.enableReplay(True)
```

#### 3. Associative Memory Weakening
**Problem**: Association strength too weak, poor recall.

**Symptoms**:
- Low association strength
- Poor pattern completion
- Weak behavioral responses

**Solutions**:
```python
# Increase learning rate
am.setLearningRate(0.05)

# Strengthen existing associations
am.strengthenAllAssociations(factor=1.5)

# Add rehearsal
am.enableRehearsal(True)
```

### Memory System Testing

#### Working Memory Test

```python
import pynlm

def test_working_memory():
    """Test working memory capacity and performance."""
    
    # Create test configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Initialize working memory
    wm = pynlm.NeuralWorkingMemory()
    wm.initialize(brain)
    wm.setCapacity(50)
    
    # Test 1: Memory capacity
    print("Test 1: Memory Capacity")
    for i in range(100):
        pattern = [0.1 * i + 0.05 for _ in range(10)]
        wm.store(pattern, strength=1.0)
        
        if wm.getActiveTraces() > wm.getCapacity():
            print(f"  Capacity exceeded at trace {i}")
            break
    
    print(f"  Final traces: {wm.getActiveTraces()}")
    print(f"  Capacity: {wm.getCapacity()}")
    
    # Test 2: Memory decay
    print("\nTest 2: Memory Decay")
    wm.store([0.5, 0.5, 0.5, 0.5, 0.5], strength=1.0)
    
    for step in range(100):
        wm.update(0.1)
        activation = wm.getNeuronActivation(neuron_id)
        
        if activation < 0.01:
            print(f"  Memory decayed to near zero at step {step}")
            break
    
    print(f"  Decay test completed")
    
    # Test 3: Memory competition
    print("\nTest 3: Memory Competition")
    wm.store([0.1, 0.2, 0.3, 0.4], strength=1.0)  # Weak pattern
    wm.store([0.9, 0.8, 0.7, 0.6], strength=1.0)  # Strong pattern
    
    wm.runCompetition()
    strong_active = wm.isWinning(strong_neuron_id)
    weak_active = wm.isWinning(weak_neuron_id)
    
    print(f"  Strong pattern active: {strong_active}")
    print(f"  Weak pattern active: {weak_active}")
    
    return True

test_working_memory()
```

#### Episodic Memory Test

```python
import pynlm

def test_episodic_memory():
    """Test episodic memory storage and retrieval."""
    
    # Create test configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Initialize episodic memory
    mem = pynlm.NeuralEpisodicMemory()
    mem.initialize(brain)
    mem.setMaxEpisodes(100)
    mem.enableReplay(True)
    
    # Test 1: Episode storage
    print("Test 1: Episode Storage")
    for i in range(50):
        episode = []
        for j in range(10):
            # Create test sensory data
            data = [0.1 * j + 0.05 * (i % 3) for _ in range(32)]
            vision = pynlm.Vision(8, 8, 3)
            vision.setData(data)
            episode.append(vision)
        
        mem.storeEpisode(episode)
        
        if mem.getEpisodeCount() > mem.getMaxEpisodes():
            print(f"  Storage capacity exceeded at episode {i}")
            break
    
    print(f"  Final episode count: {mem.getEpisodeCount()}")
    print(f"  Maximum capacity: {mem.getMaxEpisodes()}")
    
    # Test 2: Memory retrieval
    print("\nTest 2: Memory Retrieval")
    query = pynlm.Vision(8, 8, 3)
    query_data = [0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 0.85, 0.95, 0.05] * 3.2
    query.setData(query_data)
    
    similar_episodes = mem.retrieveSimilar(query, maxResults=5)
    print(f"  Similar episodes found: {len(similar_episodes)}")
    
    # Test 3: Temporal retrieval
    print("\nTest 3: Temporal Retrieval")
    early_episodes = mem.retrieveTemporal(startTime=0, endTime=50, maxResults=10)
    print(f"  Episodes from time 0-50: {len(early_episodes)}")
    
    # Test 4: Replay
    print("\nTest 4: Memory Replay")
    if mem.getEpisodeCount() > 0:
        episodes_for_replay = mem.getEpisodesForReplay(3)
        print(f"  Episodes selected for replay: {len(episodes_for_replay)}")
        
        mem.replaySequence(episodes_for_replay)
        print(f"  Replay completed successfully")
    
    return True

test_episodic_memory()
```

## Memory System Performance Optimization

### 1. Memory Size Optimization

```python
# For memory-constrained systems
config.set("memory.working_memory.capacity", 50)
config.set("memory.episodic_memory.max_episodes", 200)
config.set("memory.working_memory.decay_rate", 0.02)
```

### 2. Memory Speed Optimization

```python
# For fast simulation
config.set("memory.working_memory.decay_rate", 0.05)
config.set("memory.episodic_memory.replay_interval", 200)
config.set("memory.episodic_memory.consolidation_interval", 2000)
```

### 3. Memory Accuracy Optimization

```python
# For maximum memory accuracy
config.set("memory.working_memory.decay_rate", 0.001)
config.set("memory.episodic_memory.max_episodes", 5000)
config.set("memory.episodic_memory.pattern_completion_threshold", 0.8)
```

## Conclusion

Memory systems in NLM provide the foundation for intelligent behavior by:

1. **Working Memory**: Maintaining current information for immediate processing
2. **Episodic Memory**: Storing past experiences for reference and learning
3. **Associative Memory**: Learning connections between patterns and experiences

These systems work together to enable:

- **Temporal Integration**: Linking past experiences with current processing
- **Pattern Completion**: Retrieving related memories
- **Learning Generalization**: Applying learned patterns to new situations
- **Behavioral Adaptation**: Modifying behavior based on memory

Effective memory management requires balancing:

- **Capacity vs. Performance**: More memory improves recall but reduces speed
- **Storage vs. Consolidation**: Frequent storage vs. periodic consolidation
- **Accuracy vs. Efficiency**: Precise memory vs. fast processing
- **Recency vs. Relevance**: Recent vs. important memories

The memory systems provide the infrastructure for NLM to develop intelligent, adaptive behavior through interaction with its environment and experience over time.
