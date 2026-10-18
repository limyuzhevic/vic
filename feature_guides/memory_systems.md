# NLM Feature Guide: Memory Systems

**Memory Systems Guide**

This comprehensive guide covers NLM's three complementary memory systems: Working Memory, Episodic Memory, and Associative Memory. Learn how they work together to support learning, recall, and generalization.

---

## Overview

NLM implements a biologically-inspired memory architecture with three distinct but interconnected memory systems:

### 1. Working Memory
- **Function:** Temporary information maintenance and manipulation
- **Mechanism:** Persistent neural activity through recurrent connections
- **Capacity:** Limited (~2-7 items in humans, scalable in NLM)
- **Duration:** Seconds to minutes

### 2. Episodic Memory  
- **Function:** Storage of specific experiences and events
- **Mechanism:** Neural pattern storage with pattern completion
- **Capacity:** Large, experience-dependent
- **Duration:** Days to years

### 3. Associative Memory
- **Function:** Pattern relationships, predictions, and concepts
- **Mechanism:** Hebbian learning and neural clustering
- **Capacity:** Scalable with network size
- **Duration:** Stable once formed

### Memory Integration Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        NLM BRAIN                                  │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐            │
│  │   Sensory   │───▶│ Working     │───▶│  Action     │            │
│  │   Input    │    │ Memory     │    │ Selection  │            │
│  └─────────────┘    └─────────────┘    └─────────────┘            │
│         │                  │                  │               │
│         ▼                  ▼                  ▼               │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐         │
│  │  Episodic   │    │ Associative │    │  Prediction │         │
│  │   Memory   │    │ Memory     │    │   System    │         │
│  └─────────────┘    └─────────────┘    └─────────────┘         │
│                            │                            │
│                            ▼                            │
│                    ┌─────────────┐                      │
│                    │ Neuromodul. │                      │
│                    │  Signals    │                      │
│                    └─────────────┘                      │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 1. Working Memory System

### Overview

Working memory maintains information temporarily through persistent neural activity. It's the brain's "workspace" for current tasks and immediate processing.

### Neural Implementation

#### Mechanism

Working memory in NLM uses:
1. **Persistent Activity:** Recurrent connections maintain firing
2. **Competitive Selection:** Lateral inhibition selects relevant information
3. **Attentional Focus:** Top-down and bottom-up signals modulate activity
4. **Temporal Decay:** Activity decays without reinforcement

#### Key Parameters

| Parameter | Type | Default | Range | Function |
|-----------|------|---------|-------|----------|
| `memory.working_memory.size` | int | 1000 | 10-10000 | Number of WM neurons |
| `memory.working_memory.decay_rate` | float | 0.01 | 0.001-0.1 | Activity decay rate |
| `memory.working_memory.stability` | float | 0.8 | 0.1-1.0 | Maintenance stability |
| `memory.working_memory.capacity` | int | 7 | 2-20 | Maximum items stored |

### Working Memory Functions

#### 1. Active Maintenance

```python
# Maintain information in working memory
def maintain_information_in_wm(brain, agent, world, steps=100):
    """Demonstrate working memory maintenance"""
    
    print("=== Working Memory: Active Maintenance ===")
    print("Maintaining information through persistent neural activity")
    print()
    
    # Store target information
    target_pattern = [0.8, 0.5, 0.3, 0.1]  # Example pattern to remember
    
    # Reset and initialize
    brain.reset()
    world.reset()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Maintain pattern over time
    wm_activity = []
    for step in range(steps):
        # Simulate sensory input that needs to be maintained
        input_pattern = [0.2, 0.4, 0.6, 0.8]  # New input
        agent.processSensoryInput(input_pattern)
        
        # Brain processes while maintaining
        brain.step(step)
        
        # Record WM activity (simplified measure)
        current_activity = brain.getFiringNeuronCount()
        wm_activity.append(current_activity)
        
        # Check if target pattern is still active
        if step % 20 == 0:
            print(f"Step {step}: WM activity = {current_activity} (target: {sum(target_pattern)})")
    
    # Analyze maintenance quality
    final_activity = wm_activity[-1]
    initial_activity = wm_activity[0]
    stability = 1.0 - (abs(final_activity - initial_activity) / (initial_activity + 1))
    
    print(f"\nMaintenance quality: {stability:.2f}")
    print(f"  - Initial activity: {initial_activity}")
    print(f"  - Final activity: {final_activity}")
    print(f"  - Stability: {stability:.2f}")
    
    return stability
```

#### 2. Information Selection

```python
# Select relevant information from multiple inputs
def working_memory_selection_demo(brain, agent, world, steps=150):
    """Demonstrate working memory selection"""
    
    print("\n=== Working Memory: Information Selection ===")
    print("Selecting relevant information through competitive dynamics")
    print()
    
    # Multiple potential inputs
    potential_inputs = [
        [0.1, 0.2, 0.3, 0.4],  # Input A
        [0.4, 0.3, 0.2, 0.1],  # Input B (reverse)
        [0.9, 0.1, 0.1, 0.1],  # Input C (dominant)
        [0.1, 0.1, 0.1, 0.9]   # Input D (dominant)
    ]
    
    selected_patterns = []
    
    for step in range(steps):
        # Present multiple inputs
        current_input = potential_inputs[step % len(potential_inputs)]
        agent.processSensoryInput(current_input)
        
        # Brain selects which input to maintain
        brain.step(step)
        
        # Decode which pattern is being maintained
        if step % 25 == 0:
            current_activity = brain.getFiringNeuronCount()
            selected_patterns.append((step, current_input, current_activity))
            
            print(f"Step {step}:")
            print(f"  Input: {current_input}")
            print(f"  Selected activity: {current_activity}")
            print(f"  Selection strength: {max(current_input):.2f}")
    
    # Analyze selection patterns
    print("\nSelection Analysis:")
    for step, pattern, activity in selected_patterns:
        dominant_value = max(pattern)
        print(f"  Step {step}: Selected {pattern} (dominance: {dominant_value:.2f})")
    
    return selected_patterns
```

#### 3. Memory Updating

```python
# Update working memory with new information
def working_memory_updating_demo(brain, agent, world, steps=200):
    """Demonstrate working memory updating"""
    
    print("\n=== Working Memory: Dynamic Updating ===")
    print("Updating information in response to new inputs")
    print()
    
    # Initial information to maintain
    initial_info = [0.8, 0.6, 0.4, 0.2]
    
    # Reset and initialize
    brain.reset()
    world.reset()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Maintain initial info then update
    info_history = []
    
    for step in range(steps):
        # Gradually update information
        update_factor = 0.01  # Learning rate for WM updates
        
        if step % 50 == 0:
            # New information replaces old
            new_info = [0.1, 0.2, 0.3, 0.9]  # Updated pattern
            agent.processSensoryInput(new_info)
            info_history.append((step, "updated", new_info))
        else:
            # Maintain existing information
            agent.processSensoryInput(initial_info)
            info_history.append((step, "maintained", initial_info))
        
        # Brain processes updates
        brain.step(step)
        
        if step % 50 == 0:
            print(f"Step {step}: {info_history[-1][1]} information")
    
    # Analyze updating dynamics
    updates = [e for e in info_history if e[1] == "updated"]
    maintains = [e for e in info_history if e[1] == "maintained"]
    
    print(f"\nUpdating Analysis:")
    print(f"  - Total updates: {len(updates)}")
    print(f"  - Total maintains: {len(maintains)}")
    print(f"  - Update frequency: {len(updates)/steps:.2f}")
    
    return info_history
```

### Working Memory Performance Metrics

```python
# Working memory performance analysis
def analyze_working_memory_performance(brain, steps=300):
    """Analyze working memory system performance"""
    
    print("=== Working Memory Performance Analysis ===")
    print("Measuring working memory efficiency and capacity")
    print()
    
    # Collect performance data
    performance_metrics = {
        'activation_stability': [],
        'item_capacity': [],
        'maintenance_accuracy': [],
        'update_speed': []
    }
    
    # Reset and run simulation
    brain.reset()
    
    for step in range(steps):
        # Simulate WM operations
        if step % 10 == 0:
            # Maintain information
            test_pattern = [0.9, 0.5, 0.1, 0.3]
            brain.processSensoryInput(test_pattern)
            brain.step(step)
            
            # Measure performance
            activation = brain.getFiringNeuronCount()
            accuracy = calculate_maintenance_accuracy(test_pattern, activation)
            
            performance_metrics['activation_stability'].append(activation)
            performance_metrics['maintenance_accuracy'].append(accuracy)
    
    # Calculate performance statistics
    stability_score = calculate_stability_score(performance_metrics['activation_stability'])
    accuracy_score = np.mean(performance_metrics['maintenance_accuracy'])
    
    print("Performance Results:")
    print(f"  - Activation stability: {stability_score:.2f}")
    print(f"  - Maintenance accuracy: {accuracy_score:.2f}")
    print(f"  - Memory efficiency: {stability_score * accuracy_score:.2f}")
    
    return {
        'stability': stability_score,
        'accuracy': accuracy_score,
        'efficiency': stability_score * accuracy_score
    }

def calculate_maintenance_accuracy(target_pattern, current_activity):
    """Calculate how accurately target pattern is maintained"""
    
    # Simplified accuracy calculation
    # In real implementation, would compare neural activity to target
    target_strength = sum(target_pattern)
    activity_strength = current_activity / 10000  # Normalize
    
    accuracy = min(1.0, activity_strength / target_strength) if target_strength > 0 else 0.0
    return accuracy

def calculate_stability_score(activations):
    """Calculate stability of neural activations over time"""
    
    if len(activations) < 10:
        return 0.0
    
    # Calculate coefficient of variation
    mean_activation = np.mean(activations)
    std_activation = np.std(activations)
    
    if mean_activation > 0:
        cv = std_activation / mean_activation
        stability = 1.0 / (1.0 + cv)  # Higher CV = lower stability
        return stability
    
    return 0.0
```

---

## 2. Episodic Memory System

### Overview

Episodic memory stores specific experiences and events as neural patterns. It enables recall of past events and learning from experience.

### Neural Implementation

#### Memory Encoding

Episodic memory in NLM uses:
1. **Pattern Storage:** Experiences stored as neural activity patterns
2. **Pattern Completion:** Full recall from partial cues
3. **Temporal Indexing:** Experiences organized chronologically
4. **Spatial Mapping:** Environmental relationships preserved

#### Key Parameters

| Parameter | Type | Default | Range | Function |
|-----------|------|---------|-------|----------|
| `memory.episodic_memory.max_episodes` | int | 100 | 10-1000 | Maximum stored experiences |
| `memory.episodic_memory.consolidation_rate` | float | 0.05 | 0.001-0.2 | Replay frequency |
| `memory.episodic_memory.pattern_completion` | float | 0.8 | 0.5-1.0 | Recall accuracy |
| `memory.episodic_memory.similarity_threshold` | float | 0.7 | 0.1-1.0 | Pattern matching |

### Episodic Memory Functions

#### 1. Experience Encoding

```python
# Encode new experiences into episodic memory
def encode_experience_to_episodic(brain, world, agent, steps=200):
    """Demonstrate episodic memory encoding"""
    
    print("\n=== Episodic Memory: Experience Encoding ===")
    print("Storing experiences as neural patterns")
    print()
    
    # Initialize systems
    config = pynlm.createDefaultConfig()
    config.set("memory.episodic_memory.max_episodes", 50)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    world.setAgentStart(10, 10)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Encode a series of experiences
    experiences = []
    
    for step in range(steps):
        # World interaction creates experience
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes (stores experience)
        brain.step(step)
        
        # Record experience components
        if step % 20 == 0:
            position = world.getAgentBody()
            energy = world.getAgentBody().energy
            age = world.getAgentBody().age
            
            experience = {
                'step': step,
                'position': (position.x, position.y),
                'energy': energy,
                'age': age,
                'firing_rate': brain.getAverageFiringRate(),
                'development_stage': brain.getDevelopmentalStage()
            }
            
            experiences.append(experience)
            print(f"Experience {len(experiences)} encoded at step {step}")
            print(f"  Position: {position.x:.1f}, {position.y:.1f}")
            print(f"  Energy: {energy:.1f}, Age: {age:.1f}")
            print(f"  Development: {brain.getDevelopmentalStage().name}")
    
    # Analyze encoding efficiency
    encoding_efficiency = len(experiences) / (steps / 20)
    
    print(f"\nEncoding Efficiency: {encoding_efficiency:.2f}")
    print(f"  - Experiences encoded: {len(experiences)}")
    print(f"  - Encoding rate: {encoding_efficiency:.2f} per step")
    
    return experiences
```

#### 2. Pattern Completion

```python
# Retrieve experiences from partial cues
def pattern_completion_demo(brain, world, agent, steps=150):
    """Demonstrate pattern completion in episodic memory"""
    
    print("\n=== Episodic Memory: Pattern Completion ===")
    print("Retrieving full experiences from partial cues")
    print()
    
    # First, encode some experiences
    initial_experiences = []
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        if step % 25 == 0:
            exp = {
                'step': step,
                'position': world.getAgentBody(),
                'energy': world.getAgentBody().energy
            }
            initial_experiences.append(exp)
    
    print(f"Encoded {len(initial_experiences)} experiences")
    
    # Now demonstrate pattern completion
    completion_tests = []
    
    for test_idx in range(5):
        # Create partial cue
        partial_cue = generate_partial_cue()
        
        # Attempt recall
        recall_result = attempt_recall(partial_cue, initial_experiences)
        
        completion_tests.append({
            'cue': partial_cue,
            'recall': recall_result,
            'success': is_successful_recall(recall_result)
        })
        
        print(f"Test {test_idx + 1}:")
        print(f"  Cue: {partial_cue}")
        print(f"  Recall: {recall_result}")
        print(f"  Success: {'✓' if is_successful_recall(recall_result) else '✗'}")
        print()
    
    # Analyze completion performance
    success_rate = sum(1 for t in completion_tests if t['success']) / len(completion_tests)
    
    print(f"Pattern Completion Success Rate: {success_rate:.2f}")
    return completion_tests

def generate_partial_cue():
    """Generate a partial cue for recall"""
    # Simplified partial cue generation
    full_pattern = [0.9, 0.7, 0.5, 0.3]
    partial_indices = [0, 2]  # Partial information
    
    return {i: full_pattern[i] for i in partial_indices}

def attempt_recall(cue, experiences):
    """Attempt to recall experience based on cue"""
    # Simplified recall mechanism
    if not experiences:
        return None
    
    # Match based on cue information
    best_match = min(experiences, key=lambda exp: calculate_cue_distance(cue, exp))
    
    return {
        'matched_experience': best_match,
        'distance': calculate_cue_distance(cue, best_match),
        'confidence': 1.0 - calculate_cue_distance(cue, best_match)
    }

def calculate_cue_distance(cue, experience):
    """Calculate distance between cue and experience"""
    # Simplified distance calculation
    exp_pattern = [experience['energy'], experience['age']]
    
    if cue:
        cue_pattern = list(cue.values())
        if len(cue_pattern) == len(exp_pattern):
            return sum((c - e) ** 2 for c, e in zip(cue_pattern, exp_pattern)) ** 0.5
    
    return float('inf')

def is_successful_recall(recall_result):
    """Check if recall was successful"""
    return recall_result and recall_result['confidence'] > 0.5
```

#### 3. Memory Consolidation

```python
# Consolidate and strengthen important memories
def episodic_memory_consolidation(brain, world, agent, steps=300):
    """Demonstrate episodic memory consolidation"""
    
    print("\n=== Episodic Memory: Consolidation ===")
    print("Strengthening and integrating stored experiences")
    print()
    
    # Phase 1: Encoding
    encoded_experiences = []
    for step in range(150):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        if step % 30 == 0:
            exp = create_experience_entry(step, world, brain)
            encoded_experiences.append(exp)
    
    print(f"Phase 1 - Encoded {len(encoded_experiences)} experiences")
    
    # Phase 2: Consolidation
    consolidation_results = []
    
    for consolidation_step in range(150, steps):
        # Replay and reinforce important memories
        if consolidation_step % 50 == 0:
            # Select important experiences for reinforcement
            important_exp = select_important_experiences(encoded_experiences)
            
            # Consolidate through replay
            for exp in important_exp:
                reinforce_experience(brain, exp)
            
            consolidation_result = {
                'step': consolidation_step,
                'experiences_reinforced': len(important_exp),
                'consolidation_strength': len(important_exp) / max(len(encoded_experiences), 1)
            }
            
            consolidation_results.append(consolidation_result)
            print(f"Consolidation at step {consolidation_step}")
            print(f"  Experiences reinforced: {len(important_exp)}")
            print(f"  Consolidation strength: {consolidation_result['consolidation_strength']:.2f}")
        
        # Continue normal processing
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(consolidation_step)
    
    # Analyze consolidation effectiveness
    avg_consolidation = np.mean([r['consolidation_strength'] for r in consolidation_results])
    
    print(f"\nConsolidation Analysis:")
    print(f"  - Average consolidation strength: {avg_consolidation:.2f}")
    print(f"  - Total consolidation sessions: {len(consolidation_results)}")
    print(f"  - Memory integration: {avg_consolidation * 100:.1f}%")
    
    return consolidation_results

def create_experience_entry(step, world, brain):
    """Create an experience entry for storage"""
    return {
        'step': step,
        'position': (world.getAgentBody().x, world.getAgentBody().y),
        'energy': world.getAgentBody().energy,
        'age': world.getAgentBody().age,
        'firing_rate': brain.getAverageFiringRate(),
        'development_stage': brain.getDevelopmentalStage(),
        'sensory_input': world.getSensoryPercept().getAllSignals()
    }

def select_important_experiences(experiences):
    """Select experiences for consolidation based on importance"""
    
    # Calculate importance scores
    important_exp = []
    
    for exp in experiences:
        # Factors contributing to importance
        novelty_score = min(1.0, exp['age'] / 100)  # Older experiences = more novel? 
        energy_score = exp['energy'] / 100
        development_score = get_development_stage_weight(exp['development_stage'])
        
        importance = (novelty_score + energy_score + development_score) / 3
        
        if importance > 0.3:  # Threshold for importance
            exp['importance'] = importance
            important_exp.append(exp)
    
    # Sort by importance
    important_exp.sort(key=lambda x: x['importance'], reverse=True)
    
    # Limit number based on capacity
    max_capacity = 20
    return important_exp[:max_capacity]

def reinforce_experience(brain, experience):
    """Reinforce an experience through neural replay"""
    
    # Simplified reinforcement mechanism
    # In real implementation, would replay the neural pattern
    
    # Increase synaptic strength associated with this experience
    if 'reinforcement_count' not in experience:
        experience['reinforcement_count'] = 0
    
    experience['reinforcement_count'] += 1
    
    # Effect on brain (simplified)
    # brain.reinforce_pattern(experience)
```

---

## 3. Associative Memory System

### Overview

Associative memory forms relationships between patterns and events. It enables prediction, concept formation, and generalization from experience.

### Neural Implementation

#### Pattern Association

Associative memory in NLM uses:
1. **Hebbian Clustering:** Similar patterns form clusters
2. **Predictive Learning:** Patterns predict future events
3. **Concept Formation:** Abstract representations emerge
4. **Relational Memory:** Connections between concepts stored

#### Key Parameters

| Parameter | Type | Default | Range | Function |
|-----------|------|---------|-------|----------|
| `memory.associative_memory.capacity` | int | 500 | 50-2000 | Number of associations |
| `memory.associative_memory.strength_decay` | float | 0.02 | 0.001-0.1 | Association weakening |
| `memory.associative_memory.learning_rate` | float | 0.005 | 0.001-0.01 | Association formation |
| `memory.associative_memory.similarity_threshold` | float | 0.7 | 0.5-0.9 | Pattern grouping |

### Associative Memory Functions

#### 1. Pattern Clustering

```python
# Cluster similar patterns into concepts
def pattern_clustering_demo(brain, agent, world, steps=250):
    """Demonstrate pattern clustering in associative memory"""
    
    print("\n=== Associative Memory: Pattern Clustering ===")
    print("Grouping similar patterns into stable concepts")
    print()
    
    # Generate diverse input patterns
    input_patterns = generate_input_patterns(100)
    
    # Track concept formation
    concepts_formed = []
    pattern_assignments = []
    
    for step in range(steps):
        # Present input pattern
        current_pattern = input_patterns[step % len(input_patterns)]
        agent.processSensoryInput(current_pattern)
        brain.step(step)
        
        # Assign to concept (simplified)
        assigned_concept = assign_to_concept(current_pattern, concepts_formed)
        pattern_assignments.append((step, current_pattern, assigned_concept))
        
        if step % 50 == 0:
            print(f"Step {step}: Pattern assigned to concept {assigned_concept}")
            
            # New concept formed?
            if assigned_concept >= len(concepts_formed):
                new_concept = create_concept_from_pattern(current_pattern)
                concepts_formed.append(new_concept)
                print(f"  -> New concept {assigned_concept} formed!")
                print(f"     Prototype: {new_concept['prototype']}")
    
    # Analyze concept formation
    concept_stability = analyze_concept_stability(concepts_formed)
    concept_overlap = calculate_concept_overlap(concepts_formed)
    
    print(f"\nConcept Formation Analysis:")
    print(f"  - Total concepts formed: {len(concepts_formed)}")
    print(f"  - Concept stability: {concept_stability:.2f}")
    print(f"  - Concept overlap: {concept_overlap:.2f}")
    print(f"  - Pattern coherence: {concept_stability * (1 - concept_overlap):.2f}")
    
    return concepts_formed, pattern_assignments

def generate_input_patterns(num_patterns):
    """Generate diverse input patterns"""
    
    patterns = []
    
    # Base patterns with variations
    base_patterns = [
        [0.9, 0.1, 0.1, 0.1],  # Pattern A
        [0.1, 0.9, 0.1, 0.1],  # Pattern B
        [0.1, 0.1, 0.9, 0.1],  # Pattern C
        [0.1, 0.1, 0.1, 0.9]   # Pattern D
    ]
    
    for i in range(num_patterns):
        base = base_patterns[i % len(base_patterns)]
        # Add noise
        noisy_pattern = [x + (np.random.random() - 0.5) * 0.2 for x in base]
        # Clip to valid range
        noisy_pattern = [max(0.0, min(1.0, x)) for x in noisy_pattern]
        patterns.append(noisy_pattern)
    
    return patterns

def create_concept_from_pattern(pattern):
    """Create a new concept from a pattern"""
    
    return {
        'id': len(concept_prototypes),  # Global concept ID
        'prototype': pattern.copy(),
        'member_count': 1,
        'stability': 1.0,
        'creation_step': current_step,
        'last_updated': current_step
    }

def assign_to_concept(pattern, concepts):
    """Assign pattern to existing concept or create new one"""
    
    if not concepts:
        return 0  # First pattern creates concept 0
    
    # Find best matching concept
    best_match = 0
    best_similarity = 0.0
    
    for concept_id, concept in enumerate(concepts):
        similarity = calculate_pattern_similarity(pattern, concept['prototype'])
        
        if similarity > best_similarity and similarity > get_concept_threshold(concept):
            best_similarity = similarity
            best_match = concept_id
    
    # Update concept
    concept = concepts[best_match]
    concept['member_count'] += 1
    concept['stability'] = min(1.0, concept['stability'] + 0.01)
    concept['last_updated'] = current_step
    
    return best_match
```

#### 2. Predictive Learning

```python
# Learn to predict future events
def predictive_learning_demo(brain, agent, world, steps=300):
    """Demonstrate predictive learning in associative memory"""
    
    print("\n=== Associative Memory: Predictive Learning ===")
    print("Learning to predict future events from patterns")
    print()
    
    # Track prediction accuracy
    predictions = []
    actuals = []
    accuracy_history = []
    
    # Create training sequences
    training_data = create_predictive_sequences(200)
    
    for step in range(steps):
        # Get current state
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Make prediction for next step
        current_pattern = extract_pattern_from_percept(percept)
        prediction = predict_next_state(current_pattern, training_data)
        
        # Get actual next state
        if step < len(training_data) - 1:
            actual_next = training_data[step + 1]['pattern']
        else:
            actual_next = [0.0] * len(current_pattern)
        
        predictions.append(prediction)
        actuals.append(actual_next)
        
        # Calculate prediction accuracy
        pred_accuracy = calculate_prediction_accuracy(prediction, actual_next)
        accuracy_history.append(pred_accuracy)
        
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Prediction: {prediction}")
            print(f"  Actual: {actual_next}")
            print(f"  Accuracy: {pred_accuracy:.2f}")
            print(f"  Running average: {np.mean(accuracy_history[-5:]):.2f}")
    
    # Analyze predictive learning
    final_accuracy = accuracy_history[-1] if accuracy_history else 0.0
    learning_curve = calculate_learning_curve(accuracy_history)
    
    print(f"\nPredictive Learning Analysis:")
    print(f"  - Final prediction accuracy: {final_accuracy:.2f}")
    print(f"  - Learning efficiency: {np.mean(learning_curve):.2f}")
    print(f"  - Stability of predictions: {np.std(accuracy_history):.3f}")
    
    return predictions, actuals, accuracy_history

def create_predictive_sequences(length):
    """Create training sequences for prediction"""
    
    sequences = []
    
    # Create simple transition patterns
    base_pattern = [0.8, 0.6, 0.4, 0.2]
    
    for i in range(length):
        # Add small variations
        noise = [np.random.random() * 0.2 - 0.1 for _ in base_pattern]
        pattern = [max(0.0, min(1.0, b + n)) for b, n in zip(base_pattern, noise)]
        
        # Occasionally make larger jumps
        if i % 20 == 0:
            pattern = [np.random.random() for _ in base_pattern]
        
        sequences.append({
            'step': i,
            'pattern': pattern,
            'next_pattern': pattern  # Simplified - would normally be different
        })
    
    return sequences

def predict_next_state(current_pattern, training_data):
    """Predict next state based on current pattern"""
    
    # Find similar patterns in training data
    similar_patterns = []
    current_hash = hash(str(tuple(current_pattern)))  # Simplified
    
    for seq in training_data:
        if seq['step'] < 50:  # Use earlier data for prediction
            similarity = calculate_pattern_similarity(current_pattern, seq['pattern'])
            similar_patterns.append((similarity, seq))
    
    # Average similar patterns
    if similar_patterns:
        similar_patterns.sort(key=lambda x: x[0], reverse=True)
        top_patterns = similar_patterns[:min(5, len(similar_patterns))]
        
        # Weighted average by similarity
        predicted = [0.0] * len(current_pattern)
        total_weight = 0.0
        
        for similarity, seq in top_patterns:
            weight = similarity
            predicted = [p + weight * s for p, s in zip(predicted, seq['pattern'])]
            total_weight += weight
        
        if total_weight > 0:
            predicted = [p / total_weight for p in predicted]
        
        return predicted
    
    # Fallback to current pattern
    return current_pattern.copy()
```

---

## Memory Integration and Coordination

### Memory System Interaction

#### Working Memory → Episodic Memory

```python
# Working memory supports episodic encoding
def working_memory_supports_episodic(brain, agent, world, steps=200):
    """Demonstrate how working memory supports episodic memory"""
    
    print("\n=== Memory Integration: Working → Episodic ===")
    print("Working memory maintains information for episodic encoding")
    print()
    
    # Track memory interactions
    episodic_encodings = []
    working_memory_loads = []
    
    for step in range(steps):
        # Working memory maintains current information
        current_info = maintain_in_working_memory(agent, step)
        working_memory_loads.append(current_info)
        
        # Periodic episodic encoding
        if step % 40 == 0:
            # Encode current WM state into episodic memory
            episode = {
                'step': step,
                'wm_content': current_info.copy(),
                'neural_activity': brain.getFiringNeuronCount(),
                'development_stage': brain.getDevelopmentalStage()
            }
            episodic_encodings.append(episode)
            
            print(f"Episodic encoding at step {step}")
            print(f"  WM content: {current_info}")
            print(f"  Neural activity: {episode['neural_activity']}")
    
    # Analyze integration efficiency
    integration_efficiency = len(episodic_encodings) / (steps / 40)
    
    print(f"\nIntegration Analysis:")
    print(f"  - Episodic encodings: {len(episodic_encodings)}")
    print(f"  - Working memory loads: {len(working_memory_loads)}")
    print(f"  - Integration efficiency: {integration_efficiency:.2f}")
    
    return episodic_encodings, working_memory_loads
```

#### Episodic Memory → Associative Memory

```python
# Episodic memory supports concept formation
def episodic_supports_associative(brain, agent, world, steps=250):
    """Demonstrate how episodic memory supports associative learning"""
    
    print("\n=== Memory Integration: Episodic → Associative ===")
    print("Episodic memories provide data for concept formation")
    print()
    
    # Store episodic memories
    episodic_memories = []
    
    for step in range(steps):
        # Create episodic memory
        episode = {
            'step': step,
            'context': create_context_from_input(world, agent),
            'outcome': create_outcome_from_behavior(world, agent),
            'neural_signature': brain.getFiringPattern()
        }
        episodic_memories.append(episode)
        
        brain.step(step)
        
        if step % 50 == 0:
            print(f"Episodic memory {len(episodic_memories)} created")
            print(f"  Context: {episode['context']}")
            print(f"  Outcome: {episode['outcome']}")
    
    # Form associations from episodic memories
    associations = form_associations_from_memories(episodic_memories)
    
    print(f"\nAssociation Formation:")
    print(f"  - Episodic memories: {len(episodic_memories)}")
    print(f"  - Associations formed: {len(associations)}")
    print(f"  - Association density: {len(associations)/max(len(episodic_memories), 1):.2f}")
    
    return episodic_memories, associations
```

#### Memory Coordination

```python
# Coordinate all memory systems
def coordinated_memory_operation(brain, agent, world, steps=300):
    """Demonstrate coordinated memory system operation"""
    
    print("\n=== Memory Integration: Coordinated Operation ===")
    print("All memory systems working together")
    print()
    
    # System state tracking
    system_state = {
        'working_memory': {'load': 0, 'stability': 0},
        'episodic_memory': {'capacity': 0, 'consolidation': 0},
        'associative_memory': {'connections': 0, 'activation': 0}
    }
    
    for step in range(steps):
        # 1. Working memory: maintain current information
        wm_load = maintain_current_information(agent, step)
        system_state['working_memory']['load'] = wm_load
        
        # 2. Processing with working memory
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # 3. Decision with WM support
        if step % 30 == 0:
            action = agent.decodeMotorCommand()
            # Store action outcome in episodic memory
            store_in_episodic_memory(world, action, brain)
            system_state['episodic_memory']['capacity'] = len(get_episodic_memories())
        
        # 4. Association formation
        form_associations(agent, world, brain)
        system_state['associative_memory']['connections'] = count_associations()
        
        # 5. Consolidation
        if step % 60 == 0:
            consolidate_memories()
            system_state['episodic_memory']['consolidation'] = get_consolidation_strength()
        
        # Log system state
        if step % 60 == 0:
            print(f"Step {step}:")
            print(f"  WM load: {system_state['working_memory']['load']:.2f}")
            print(f"  EM capacity: {system_state['episodic_memory']['capacity']}")
            print(f"  AM connections: {system_state['associative_memory']['connections']}")
            print(f"  EM consolidation: {system_state['episodic_memory']['consolidation']:.2f}")
            print()
    
    return system_state
```

---

## Memory Performance Analysis

### System Metrics

```python
# Comprehensive memory system analysis
def analyze_memory_system_performance(brain, agent, world, steps=400):
    """Analyze performance of all memory systems"""
    
    print("=== Memory System Performance Analysis ===")
    print("Comprehensive evaluation of memory subsystems")
    print()
    
    # Collect performance data
    performance_data = {
        'working_memory': collect_working_memory_metrics(brain, steps),
        'episodic_memory': collect_episodic_memory_metrics(brain, steps),
        'associative_memory': collect_associative_memory_metrics(brain, steps)
    }
    
    # Analyze each system
    wm_performance = analyze_working_memory_performance(performance_data['working_memory'])
    em_performance = analyze_episodic_memory_performance(performance_data['episodic_memory'])
    am_performance = analyze_associative_memory_performance(performance_data['associative_memory'])
    
    # Overall system performance
    overall_performance = calculate_overall_memory_performance(
        wm_performance, em_performance, am_performance
    )
    
    # Print results
    print_performance_summary(wm_performance, em_performance, am_performance)
    print_system_integration_analysis(overall_performance)
    
    return {
        'individual_systems': {
            'working_memory': wm_performance,
            'episodic_memory': em_performance,
            'associative_memory': am_performance
        },
        'overall_performance': overall_performance
    }

def calculate_overall_memory_performance(wm_perf, em_perf, am_perf):
    """Calculate overall memory system performance"""
    
    # Weighted performance (adjust weights as needed)
    overall = (
        wm_perf['efficiency'] * 0.3 +
        em_perf['effectiveness'] * 0.4 +
        am_perf['coherence'] * 0.3
    )
    
    return {
        'total_efficiency': overall,
        'system_balance': calculate_memory_system_balance(wm_perf, em_perf, am_perf),
        'integration_quality': calculate_system_integration(wm_perf, em_perf, am_perf)
    }
```

---

## Interactive Memory Laboratory

### Memory System Comparison

```python
# Compare different memory system configurations
print("=== Memory System Comparison Laboratory ===")

# Test different configurations
configurations = [
    {
        'name': 'Minimal Memory',
        'wm_size': 50,
        'em_capacity': 20,
        'am_connections': 30,
        'description': 'Basic memory with limited capacity'
    },
    {
        'name': 'Balanced Memory',
        'wm_size': 200,
        'em_capacity': 100,
        'am_connections': 150,
        'description': 'Well-balanced memory systems'
    },
    {
        'name': 'Rich Memory',
        'wm_size': 500,
        'em_capacity': 300,
        'am_connections': 400,
        'description': 'Extensive memory with high capacity'
    }
]

for config in configurations:
    print(f"\n{config['name']}:")
    print(f"  Description: {config['description']}")
    print(f"  Working Memory size: {config['wm_size']}")
    print(f"  Episodic Memory capacity: {config['em_capacity']}")
    print(f"  Associative Memory connections: {config['am_connections']}")
    
    # Simulate performance
    efficiency = (config['wm_size'] * 0.3 + config['em_capacity'] * 0.4 + config['am_connections'] * 0.3) / 1000
    print(f"  Expected efficiency: {efficiency:.2f}")

print("\n=== Memory Configuration Analysis Complete ===")
```

### Memory Optimization

```python
# Optimize memory system parameters
def optimize_memory_parameters(brain, agent, world, optimization_goals):
    """Optimize memory system parameters for specific goals"""
    
    print("\n=== Memory System Optimization ===")
    print(f"Optimization goals: {optimization_goals}")
    print()
    
    # Define parameter ranges to test
    parameter_ranges = {
        'working_memory_size': [50, 200, 500, 1000],
        'episodic_capacity': [20, 100, 300, 1000],
        'association_strength': [0.5, 1.0, 2.0, 5.0],
        'consolidation_rate': [0.01, 0.05, 0.1, 0.2]
    }
    
    # Evaluate each combination
    best_config = None
    best_score = 0.0
    
    for wm_size in parameter_ranges['working_memory_size']:
        for em_cap in parameter_ranges['episodic_capacity']:
            for assoc_str in parameter_ranges['association_strength']:
                for cons_rate in parameter_ranges['consolidation_rate']:
                    
                    # Test configuration
                    config = {
                        'wm_size': wm_size,
                        'em_capacity': em_cap,
                        'assoc_strength': assoc_str,
                        'consolidation_rate': cons_rate
                    }
                    
                    score = evaluate_memory_configuration(config, optimization_goals)
                    
                    if score > best_score:
                        best_score = score
                        best_config = config
    
    print("Optimization Results:")
    if best_config:
        print(f"Best configuration: {best_config}")
        print(f"Optimization score: {best_score:.2f}")
        
        # Explain why this configuration is best
        explain_optimal_configuration(best_config, optimization_goals)
    
    return best_config, best_score

# Example optimization goals
goals_examples = [
    ['fast_learning', 'quick_acquisition'],
    ['high_accuracy', 'precise_recall'],
    ['balanced', 'sustainable_performance'],
    ['adaptive', 'responsive_to_environment']
]

for goal in goals_examples:
    print(f"\nOptimizing for {goal}:")
    optimal_config, score = optimize_memory_parameters(brain, agent, world, goal)
```

---

## Key Insights and Applications

### 1. Working Memory Insights

- **Active Maintenance:** Working memory uses persistent neural activity
- **Limited Capacity:** System gracefully handles capacity limits
- **Dynamic Updating:** Information updates smoothly in response to new inputs
- **Selection Mechanism:** Competitive dynamics prioritize relevant information

### 2. Episodic Memory Insights

- **Pattern Completion:** Partial cues can trigger full memory recall
- **Consolidation Process:** Important memories are strengthened over time
- **Temporal Organization:** Experiences stored chronologically
- **Contextual Binding:** Memories link actions to contexts and outcomes

### 3. Associative Memory Insights

- **Concept Formation:** Similar patterns group into abstract concepts
- **Predictive Learning:** Patterns anticipate future events
- **Pattern Regularities:** System detects and exploits statistical regularities
- **Generalization:** Learns to apply knowledge to new situations

### 4. System Integration Insights

- **Hierarchical Processing:** Different memory systems handle different types of information
- **Complementary Functions:** Each memory system supports and enhances others
- **Resource Allocation:** System balances memory storage with processing efficiency
- **Adaptive Optimization:** Memory parameters adjust to environmental demands

### 5. Real-World Applications

#### Biological Systems

- **Animal Navigation:** Remembering food locations and predator threats
- **Human Memory:** Learning skills and forming concepts
- **Social Learning:** Observing and imitating others' behaviors
- **Expert Performance:** Developing intuitive knowledge through practice

#### Artificial Systems

- **Robot Learning:** Adapting to new environments and tasks
- **Machine Learning:** Few-shot learning and transfer learning
- **Adaptive Systems:** Respond to changing environments
- **Intelligent Assistants:** Maintaining context across interactions

#### Educational Applications

- **Study Techniques:** Spaced repetition and retrieval practice
- **Concept Maps:** Organizing knowledge hierarchically
- **Problem Solving:** Applying learned principles to new problems
- **Skill Development:** Building expertise through practice

---

## Technical Implementation Notes

### 1. Neural Implementation Details

Working memory in NLM uses:
- **Recurrent Networks:** Persistent activity through feedback connections
- **Attentional Modulation:** Top-down signals control information maintenance
- **Competitive Dynamics:** Lateral inhibition selects prioritized items
- **Temporal Integration:** Time-dependent processes stabilize representations

Episodic memory uses:
- **Pattern Storage:** Experiences stored as neural activity patterns
- **Synaptic Consolidation:** Repeated replay strengthens connections
- **Context Binding:** Links actions, contexts, and outcomes
- **Pattern Completion:** Partial cues trigger full memory recall

Associative memory uses:
- **Hebbian Learning:** Correlated activity strengthens connections
- **Concept Formation:** Abstraction through pattern clustering
- **Predictive Coding:** Patterns anticipate future sensory input
- **Relational Memory:** Networks of concept connections

### 2. Performance Considerations

- **Memory Capacity:** Trade-off between storage and processing speed
- **Encoding Speed:** Balance between rapid learning and stability
- **Retrieval Efficiency:** Optimize pattern completion algorithms
- **Resource Management:** Balance between memory and computational resources

### 3. Scalability

NLM's memory systems scale:
- **Working Memory:** Scales with available neurons and recurrent connections
- **Episodic Memory:** Scales with storage capacity and consolidation time
- **Associative Memory:** Scales with network size and learning rate
- **Integration:** All systems scale together as brain complexity increases

---

## Future Research Directions

### 1. Advanced Memory Mechanisms

- **Active Maintenance:** Enhanced working memory persistence
- **Memory Integration:** Better coordination between systems
- **Meta-Learning:** Learning how to learn across memory systems
- **Memory Compression:** Efficient storage of long-term knowledge

### 2. Adaptive Memory Systems

- **Context-Aware Memory:** Memories adapt based on current goals
- **Cross-Modal Integration:** Combine different sensory inputs
- **Developmental Memory:** Track and adapt based on age
- **Social Memory:** Learn from interactions with others

### 3. Computational Advances

- **Neural Implementation:** More biologically plausible mechanisms
- **Efficiency Optimizations:** Better resource utilization
- **Parallel Processing:** Simultaneous operation across systems
- **Self-Organization:** Emergent memory structure

---

## Conclusion

NLM's memory systems provide a comprehensive framework for artificial intelligence:

1. **Working Memory:** Handles current tasks and active processing
2. **Episodic Memory:** Stores specific experiences for later use
3. **Associative Memory:** Forms concepts and predictions for generalization
4. **Integration:** Systems work together seamlessly for intelligent behavior

**Key Takeaways:**

- **Multi-level Architecture:** Different memory types handle different information
- **Neural Grounding:** Memories are implemented through biological mechanisms
- **Adaptive Function:** Memory systems optimize based on experience
- **System Integration:** Coordinated operation enables complex cognition

**Next Steps:**

1. **Practice with Working Memory:** Implement active maintenance mechanisms
2. **Explore Episodic Memory:** Design experience storage and retrieval
3. **Develop Associative Learning:** Create pattern clustering and prediction
4. **Integrate Systems:** Coordinate all memory types for intelligent behavior

The memory systems in NLM provide the foundation for learning, reasoning, and intelligent behavior. Master these systems, and you'll create truly intelligent artificial brains! 🧠✨

---

*This feature guide provides comprehensive coverage of NLM's memory systems. Practice these techniques to build intelligent, learning-capable artificial brains.*

**Next Steps:**
1. **Explore Neuromodulation Guide** - Learn about motivation and learning signals
2. **Try Advanced Examples** - See memory systems in action
3. **Check API Reference** - Technical details for all memory functions
4. **Read Best Practices** - Proven approaches for memory system design

---

## Interactive Memory System Demonstration

Try this memory system demonstration (simulated):

```python
# Memory System Interactive Demo
print("=== NLM Memory System Interactive Demo ===")

# Initialize memory systems
print("Initializing memory systems...")
print("Working Memory: Active maintenance of information")
print("Episodic Memory: Storage of specific experiences") 
print("Associative Memory: Formation of concepts and predictions")
print()

# Demonstrate each system
print("Working Memory Demo:")
print("  - Maintaining information through persistent activity")
print("  - Selective attention through competitive dynamics")
print("  - Dynamic updating in response to new inputs")
print()

print("Episodic Memory Demo:")
print("  - Encoding experiences as neural patterns")
print("  - Pattern completion from partial cues")
print("  - Consolidation through repeated replay")
print()

print("Associative Memory Demo:")
print("  - Clustering similar patterns into concepts")
print("  - Predictive learning from experience")
print("  - Formation of abstract knowledge")
print()

print("=== Memory System Integration Demo ===")
print("Working Memory ↔ Episodic Memory: Information flow")
print("Episodic Memory → Associative Memory: Concept formation")
print("Associative Memory ↔ Working Memory: Prediction and planning")
print()

print("=== Performance Analysis ===")
performance_metrics = {
    'working_memory': {'stability': 0.85, 'accuracy': 0.78, 'efficiency': 0.82},
    'episodic_memory': {'storage': 0.73, 'recall': 0.81, 'consolidation': 0.68},
    'associative_memory': {'clustering': 0.88, 'prediction': 0.75, 'generalization': 0.79}
}

for system, metrics in performance_metrics.items():
    avg_metric = sum(metrics.values()) / len(metrics.values())
    print(f"{system.title()}: {avg_metric:.2f} average performance")
    for metric, value in metrics.items():
        print(f"  - {metric}: {value:.2f}")

print("\n=== Memory System Analysis Complete ===")
print("\nAll memory systems working together to create intelligent behavior! 🧠✨")
```

---

*This interactive demonstration shows how NLM's memory systems work together to support intelligent behavior.*

**Memory System Evolution:**

1. **Foundation:** Working memory maintains current information
2. **Experience:** Episodic memory stores specific events
3. **Knowledge:** Associative memory forms concepts and predictions
4. **Integration:** Systems coordinate for intelligent action

The memory systems in NLM provide the complete architecture for artificial intelligence, from simple reactive behavior to complex reasoning and learning! 🚀

---

*This feature guide provides comprehensive coverage of NLM's memory systems. Practice these techniques to build intelligent, learning-capable artificial brains.*

**Final Note:** NLM's memory systems represent a complete artificial intelligence architecture. Master them, and you'll have the foundation for creating truly intelligent machines! 🎯

---

## Memory System Documentation Complete

✅ **Working Memory:** Active maintenance and information selection
✅ **Episodic Memory:** Experience storage and pattern completion
✅ **Associative Memory:** Concept formation and predictive learning
✅ **System Integration:** Coordinated memory operation
✅ **Performance Analysis:** Comprehensive system evaluation

**Next Documentation Features:**
- 🧠 Neuromodulation Systems Guide
- 🤖 Development System Guide
- ⚡ Performance Optimization Guide
- 📊 Experiment Design Guide

The NLM memory systems provide the foundation for intelligent artificial brains! 🚀✨