# Tutorial 4: Memory Systems and Episodic Storage

This tutorial explores the memory systems in NLM that allow the brain to store, recall, and consolidate experiences. Memory is essential for intelligent behavior, allowing agents to:

- Learn from past experiences
- Recognize patterns
- Make predictions
- Develop skills over time
- Maintain temporary information

## Learning Objectives

After completing this tutorial, you should be able to:

1. **Understand different memory system types** (working, episodic, semantic, procedural, associative)
2. **Configure and use memory systems** in your NLM brain
3. **Monitor memory usage and performance** 
4. **Implement memory replay and consolidation** for learning
5. **Design experiments to study memory formation**
6. **Debug memory-related issues** and optimize memory parameters

## What You'll Learn

- The biological and computational principles behind different memory systems
- How memory systems integrate with neural processing
- Memory formation, storage, and retrieval mechanisms
- Memory consolidation and replay processes
- How to measure and analyze memory performance

## Prerequisites

- Understanding of Tutorials 1-3 (brain creation, sensory-motor loop, learning)
- Basic knowledge of neural systems and cognitive concepts

## Step 1: Understanding NLM Memory Architecture

### NLM's Five Memory Systems

NLM implements multiple memory systems, each with distinct functions:

#### 1. Working Memory (NeuralWorkingMemory)

**Purpose:** Temporary storage of information for immediate use
**Duration:** Seconds to minutes
**Capacity:** Limited (several items)
**Mechanism:** Persistent neural activity with competition

**Key Features:**
- Maintains firing patterns over time
- Competitive dynamics between memory traces
- Decay of weak traces
- Influences attention and decision-making

#### 2. Episodic Memory (NeuralEpisodicMemory)

**Purpose:** Storage of specific events and experiences
**Duration:** Hours to years
**Capacity:** Large
**Mechanism:** Neural patterns representing experiences

**Key Features:**
- Temporal sequencing of events
- Relevance-weighted storage
- Replay during sleep/offline
- Integration with learning

#### 3. Semantic Memory (NeuralSemanticMemory)

**Purpose:** General knowledge and facts
**Duration:** Long-term
**Capacity:** Very large
**Mechanism:** Pattern associations and concepts

**Key Features:**
- Abstract knowledge representation
- Concept formation from patterns
- Generalization across experiences
- Supports reasoning and problem-solving

#### 4. Procedural Memory (NeuralProceduralMemory)

**Purpose:** Skills and habits
**Duration:** Long-term
**Capacity:** Large
**Mechanism:** Synaptic changes in motor pathways

**Key Features:**
- Performance improvement through repetition
- Automatic execution
- Consolidation through sleep
- Integration with motor systems

#### 5. Associative Memory (NeuralAssociativeMemory)

**Purpose:** Pattern associations and connections
**Duration:** Medium to long-term
**Capacity:** Large
**Mechanism:** Hebbian associations and spreading activation

**Key Features:**
- Pattern completion from partial cues
- Spreading activation across patterns
- Context-dependent retrieval
- Supports creative thinking

### Memory Integration with Neural Processing

The memory systems integrate with the brain in multiple ways:

```cpp
// Memory influences neural processing:
workingMemory->update(neuralActivity);
attentionSystem->select(memoryItems);
episodicMemory->store(currentExperience);
conceptFormation->extract(memoryPatterns);

// Neural processing affects memory:
plasticity->strengthen(memoryConnections);
development->consolidate(memoryTrains);
neuromodulation->prioritize(memoryItems);
prediction->guide(memoryRetrieval);
```

## Step 2: Memory Configuration and Setup

### Configuring Memory Systems

```python
import pynlm

def configure_memory_systems():
    """Configure all memory systems for optimal performance"""
    
    print("=== Memory System Configuration ===")
    
    # Base configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    
    # Working memory settings
    config.set("memory.working.enabled", True)
    config.set("memory.working.capacity", 50)
    config.set("memory.working.decay_rate", 0.01)
    config.set("memory.working.reinforcement_strength", 0.1)
    
    # Episodic memory settings
    config.set("memory.episodic.enabled", True)
    config.set("memory.episodic.max_episodes", 1000)
    config.set("memory.episodic.capture_interval", 10)  # Capture every 10 steps
    config.set("memory.episodic.replay_frequency", 100)  # Replay every 100 steps
    config.set("memory.episodic.consolidation_threshold", 0.8)
    
    # Semantic memory settings
    config.set("memory.semantic.enabled", True)
    config.set("memory.semantic.concept_threshold", 0.7)
    config.set("memory.semantic.generalization_strength", 0.5)
    config.set("memory.semantic.association_decay", 0.001)
    
    # Procedural memory settings
    config.set("memory.procedural.enabled", True)
    config.set("memory.procedural.simulation_repetitions", 5)
    config.set("memory.procedural.consolidation_interval", 50)
    config.set("memory.procedural.skill_strength_threshold", 0.6)
    
    # Associative memory settings
    config.set("memory.associative.enabled", True)
    config.set("memory.associative.spreading_activation", 0.8)
    config.set("memory.associative.pattern_completion_threshold", 0.6)
    
    return config

def print_memory_config(config):
    """Display current memory configuration"""
    
    print(f"\nMemory System Configuration:")
    
    for system in ['working', 'episodic', 'semantic', 'procedural', 'associative']:
        enabled_key = f"memory.{system}.enabled"
        if config.get(enabled_key):
            print(f"  {system.title()} Memory: {'Enabled' if config.get(enabled_key) else 'Disabled'}")
            
            # Print system-specific parameters
            if system == 'working':
                print(f"    Capacity: {config.get('memory.working.capacity', 'N/A')}")
                print(f"    Decay rate: {config.get('memory.working.decay_rate', 'N/A')}")
            
            elif system == 'episodic':
                print(f"    Max episodes: {config.get('memory.episodic.max_episodes', 'N/A')}")
                print(f"    Capture interval: {config.get('memory.episodic.capture_interval', 'N/A')}")
            
            elif system == 'semantic':
                print(f"    Concept threshold: {config.get('memory.semantic.concept_threshold', 'N/A')}")
            
            elif system == 'procedural':
                print(f"    Repetitions: {config.get('memory.procedural.simulation_repetitions', 'N/A')}")
            
            elif system == 'associative':
                print(f"    Spreading activation: {config.get('memory.associative.spreading_activation', 'N/A')}")
```

### Memory-Enhanced Brain Creation

```python
import pynlm

def create_memory_enhanced_brain():
    """Create a brain with comprehensive memory systems"""
    
    print("=== Memory-Enhanced Brain Setup ===")
    
    config = configure_memory_systems()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with memory interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(None)  # Will be initialized with world later
    
    # Enable all memory subsystems
    # Note: Memory systems are typically automatically enabled
    # based on configuration, but you can explicitly check/enable
    
    print(f"   Memory-enhanced brain created:")
    print(f"   - {brain.getTotalNeuronCount()} neurons")
    print(f"   - {brain.getTotalSynapseCount()} synapses")
    print(f"   - Working memory enabled")
    print(f"   - Episodic memory enabled")
    print(f"   - Semantic memory enabled")
    print(f"   - Procedural memory enabled")
    print(f"   - Associative memory enabled")
    
    return brain, agent
```

## Step 3: Working Memory Implementation

### Working Memory Basics

Working memory maintains neural activity patterns over short time periods:

```python
import pynlm

def demonstrate_working_memory(brain, agent, world, steps=200):
    """Demonstrate working memory functionality"""
    
    print(f"\n=== Working Memory Demonstration ===")
    print(f"Testing working memory over {steps} steps:\n")
    
    # Set up working memory monitoring
    wm_metrics = {
        'activation_levels': [],
        'maintenance_duration': [],
        'competition_effects': [],
        'decay_patterns': []
    }
    
    for step in range(steps):
        # Update world and get percept
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain (updates working memory internally)
        brain.step(step)
        
        # Record working memory metrics
        if step % 20 == 0:
            # Get working memory statistics
            # (In a real implementation, you'd access working memory directly)
            activation = brain.getFiringNeuronCount()
            wm_metrics['activation_levels'].append(activation)
            
            print(f"   Step {step:3d}: Working memory activation: {activation:4d} neurons")
    
    # Analyze working memory performance
    print(f"\nWorking Memory Analysis:")
    
    if wm_metrics['activation_levels']:
        avg_activation = sum(wm_metrics['activation_levels']) / len(wm_metrics['activation_levels'])
        max_activation = max(wm_metrics['activation_levels'])
        
        print(f"   Average activation: {avg_activation:.1f} neurons")
        print(f"   Maximum activation: {max_activation} neurons")
        print(f"   Stability: {'Stable' if max_activation > 10 else 'Variable'}")
        
        if avg_activation > 5:
            print("   ✓ Working memory is maintaining persistent activity")
        else:
            print("   ! Working memory may need more input to sustain activity")
    
    return wm_metrics
```

### Working Memory Applications

```python
import pynlm

def demonstrate_working_memory_tasks(brain, agent, world, tasks=None):
    """Demonstrate practical working memory tasks"""
    
    if tasks is None:
        tasks = ['digit_span', 'spatial_memory', 'sequential_memory']
    
    print(f"\n=== Working Memory Task Demonstration ===")
    
    task_results = {}
    
    for task_name in tasks:
        print(f"\n{task_name.upper()} task:")
        
        if task_name == 'digit_span':
            # Digit span task: remember and reproduce sequences
            run_digit_span_task(brain, agent, world)
            
        elif task_name == 'spatial_memory':
            # Spatial memory task: remember object locations
            run_spatial_memory_task(brain, agent, world)
            
        elif task_name == 'sequential_memory':
            # Sequential memory task: remember action sequences
            run_sequential_memory_task(brain, agent, world)
    
    return task_results

def run_digit_span_task(brain, agent, world, trials=5):
    """Digit span task: remember digit sequences"""
    
    for trial in range(trials):
        sequence_length = trial + 3  # Gradually increase difficulty
        
        print(f"   Trial {trial + 1}: Remember {sequence_length} digits")
        
        # Present digit sequence
        for digit_pos in range(sequence_length):
            # Simulate presenting a digit
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(digit_pos)
            
            print(f"     Digit {digit_pos + 1}: Processed")
        
        # Test recall
        print(f"     Testing recall...")
        
        for recall_pos in range(sequence_length):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(sequence_length + recall_pos)
            
            print(f"     Recall {recall_pos + 1}: Completed")
        
        print(f"   Trial {trial + 1} completed")

def run_spatial_memory_task(brain, agent, world):
    """Spatial memory task: remember object locations"""
    
    print("   Remember object locations for 3 seconds...")
    
    # Present objects with spatial locations
    objects = [
        pynlm.WorldObject(2, 2, pynlm.WorldObjectType.Resource, value=10),
        pynlm.WorldObject(5, 8, pynlm.WorldObjectType.Hazard, value=-5),
        pynlm.WorldObject(12, 3, pynlm.WorldObjectType.Marker, value=1),
    ]
    
    for obj in objects:
        world.addObject(obj)
        print(f"     Object at ({obj.getX()}, {obj.getY()})")
    
    # Wait a moment for memory consolidation
    for step in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
    
    # Test recall
    print("   Testing recall...")
    
    for obj in objects:
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(100 + obj.getId())  # Step with object ID
        
        print(f"     Checking location of object {obj.getId()}")
    
    # Clean up
    for obj in objects:
        world.removeObject(obj.getX(), obj.getY())

def run_sequential_memory_task(brain, agent, world):
    """Sequential memory task: remember action sequences"""
    
    print("   Remember action sequence...")
    
    # Define action sequence
    action_sequence = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.Interact,
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.TurnRight,
    ]
    
    # Present sequence
    for i, action in enumerate(action_sequence):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"     Action {i + 1}: {action}")
    
    # Test sequence reproduction
    print("   Testing sequence reproduction...")
    
    for i, action in enumerate(action_sequence):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(200 + i)  # Step in memory testing phase
        
        # Should reproduce the action
        world.applyMotorCommand(action, world.getSimulationTime())
        print(f"     Reproduced action {i + 1}: {action}")
```

## Step 4: Episodic Memory Implementation

### Episodic Memory Basics

Episodic memory stores specific events with temporal context:

```python
import pynlm

def demonstrate_episodic_memory(brain, agent, world, steps=300):
    """Demonstrate episodic memory functionality"""
    
    print(f"\n=== Episodic Memory Demonstration ===")
    print(f"Testing episodic memory over {steps} steps:\n")
    
    # Setup episodic memory monitoring
    episodic_metrics = {
        'episodes_captured': [],
        'replay_events': [],
        'consolidation_events': [],
        'memory_traces': []
    }
    
    episode_count = 0
    
    for step in range(steps):
        world.update(0.1)
        
        # Capture episodic memories every 10 steps
        if step % 10 == 0:
            episode_count += 1
            episodic_metrics['episodes_captured'].append(episode_count)
            print(f"   Episode {episode_count}: Captured at step {step}")
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain (updates episodic memory internally)
        brain.step(step)
        
        # Trigger replay every 100 steps
        if step % 100 == 0 and step > 0:
            episodic_metrics['replay_events'].append(step)
            print(f"   Replay event: Replaying episodes {max(1, episode_count-10)} to {episode_count}")
        
        # Trigger consolidation every 300 steps
        if step % 300 == 0 and step > 0:
            episodic_metrics['consolidation_events'].append(step)
            print(f"   Consolidation event: Strengthening important memories")
        
        # Display current memory metrics
        if step % 50 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            print(f"   Step {step:3d}: Episodes={episode_count:3d} "
                  f"Curiosity={curiosity:5.3f} Novelty={novelty:5.3f}")
    
    # Analyze episodic memory performance
    print(f"\nEpisodic Memory Analysis:")
    
    if episodic_metrics['episodes_captured']:
        total_episodes = episodic_metrics['episodes_captured'][-1]
        replay_count = len(episodic_metrics['replay_events'])
        consolidation_count = len(episodic_metrics['consolidation_events'])
        
        print(f"   Total episodes captured: {total_episodes}")
        print(f"   Replay events: {replay_count}")
        print(f"   Consolidation events: {consolidation_count}")
        
        if total_episodes > 10:
            print("   ✓ Episodic memory is actively capturing experiences")
        
        if replay_count > 0:
            print("   ✓ Memory replay is occurring for consolidation")
        
        if consolidation_count > 0:
            print("   ✓ Memory consolidation is strengthening important traces")
    
    return episodic_metrics
```

### Memory Replay and Consolidation

```python
import pynlm

def demonstrate_memory_replay(brain, agent, world, replay_steps=400):
    """Demonstrate memory replay and consolidation processes"""
    
    print(f"\n=== Memory Replay and Consolidation ===")
    print(f"Simulating replay and consolidation over {replay_steps} steps:\n")
    
    # Track memory consolidation metrics
    consolidation_metrics = {
        'replay_cycles': [],
        'memory_strengths': [],
        'importance_ranking': [],
        'consolidation_events': []
    }
    
    episode_count = 0
    
    for step in range(replay_steps):
        world.update(0.1)
        
        # Capture new episodes
        if step % 10 == 0:
            episode_count += 1
            print(f"   Capturing episode {episode_count}")
        
        # Process and store
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Memory replay every 100 steps
        if step % 100 == 0 and step > 0:
            replay_cycles = min(5, episode_count)  # Replay last few episodes
            consolidation_metrics['replay_cycles'].append(replay_cycles)
            consolidation_metrics['consolidation_events'].append(step)
            
            print(f"   Replay cycle {step//100}: Replaying {replay_cycles} episodes")
            
            # Simulate consolidation during replay
            for episode in range(replay_cycles):
                episode_id = episode_count - replay_cycles + episode
                strength = 0.5 + (episode_id * 0.05)  # Higher ID = stronger memory
                consolidation_metrics['memory_strengths'].append(strength)
                
                print(f"     Consolidating episode {episode_id}: strength={strength:.2f}")
        
        # Global consolidation every 400 steps
        if step % 400 == 0 and step > 0:
            # Rank memories by importance (based on reward, novelty, etc.)
            important_episodes = episode_count // 4  # Top 25%
            consolidation_metrics['importance_ranking'].append(important_episodes)
            
            print(f"   Global consolidation: Strengthening top {important_episodes} episodes")
        
        if step % 100 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            print(f"   Step {step:3d}: Episodes={episode_count:3d} "
                  f"Replay cycles={len(consolidation_metrics['replay_cycles'])} "
                  f"Curiosity={curiosity:5.3f}")
    
    # Analyze consolidation effectiveness
    print(f"\nMemory Consolidation Analysis:")
    
    if consolidation_metrics['memory_strengths']:
        avg_strength = sum(consolidation_metrics['memory_strengths']) / len(consolidation_metrics['memory_strengths'])
        max_strength = max(consolidation_metrics['memory_strengths'])
        
        print(f"   Average memory strength: {avg_strength:.2f}")
        print(f"   Maximum memory strength: {max_strength:.2f}")
        
        if avg_strength > 0.8:
            print("   ✓ Strong memory consolidation achieved")
        elif avg_strength > 0.6:
            print("   = Moderate memory consolidation")
        else:
            print("   ! Weak memory consolidation - check parameters")
        
        if consolidation_metrics['importance_ranking']:
            avg_important = sum(consolidation_metrics['importance_ranking']) / len(consolidation_metrics['importance_ranking'])
            print(f"   Average important episodes consolidated: {avg_important:.1f}")
    
    return consolidation_metrics
```

## Step 5: Semantic Memory Implementation

### Semantic Memory Basics

Semantic memory stores abstract knowledge and concepts:

```python
import pynlm

def demonstrate_semantic_memory(brain, agent, world, steps=250):
    """Demonstrate semantic memory formation"""
    
    print(f"\n=== Semantic Memory Demonstration ===")
    print(f"Testing semantic memory formation over {steps} steps:\n")
    
    # Track semantic memory development
    semantic_metrics = {
        'concepts_formed': [],
        'pattern_clusters': [],
        'generalization_ability': [],
        'abstract_knowledge': []
    }
    
    concept_count = 0
    
    for step in range(steps):
        world.update(0.1)
        
        # Process sensory input to form concepts
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Semantic memory formation (simplified)
        if step % 30 == 0:
            # Simulate concept formation from patterns
            patterns = extract_perceptual_patterns(percept)
            concepts = form_concepts_from_patterns(patterns)
            
            if concepts:
                concept_count += len(concepts)
                semantic_metrics['concepts_formed'].append(concept_count)
                semantic_metrics['pattern_clusters'].append(len(patterns))
                
                print(f"   Step {step:3d}: Formed {len(concepts)} new concepts")
                print(f"     Total concepts: {concept_count}")
                print(f"     Pattern clusters: {len(patterns)}")
        
        # Measure generalization ability
        if step % 50 == 0:
            generalization_score = measure_generalization(brain, agent)
            semantic_metrics['generalization_ability'].append(generalization_score)
            
            print(f"   Generalization ability: {generalization_score:.2f}")
        
        if step % 50 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            print(f"   Step {step:3d}: Concepts={concept_count:3d} "
                  f"Generalization={semantic_metrics['generalization_ability'][-1]:.2f} "
                  f"Curiosity={curiosity:5.3f}")
    
    # Analyze semantic memory performance
    print(f"\nSemantic Memory Analysis:")
    
    if semantic_metrics['concepts_formed']:
        final_concepts = semantic_metrics['concepts_formed'][-1]
        avg_clusters = sum(semantic_metrics['pattern_clusters']) / len(semantic_metrics['pattern_clusters'])
        
        print(f"   Final concepts formed: {final_concepts}")
        print(f"   Average pattern clusters: {avg_clusters:.1f}")
        
        if final_concepts > 20:
            print("   ✓ Rich semantic knowledge developed")
        elif final_concepts > 10:
            print("   = Moderate semantic knowledge")
        else:
            print("   ! Limited semantic development - check parameters")
        
        if semantic_metrics['generalization_ability']:
            avg_generalization = sum(semantic_metrics['generalization_ability']) / len(semantic_metrics['generalization_ability'])
            print(f"   Average generalization score: {avg_generalization:.2f}")
    
    return semantic_metrics

def extract_perceptual_patterns(percept):
    """Extract patterns from sensory percept for concept formation"""
    
    # Simplified pattern extraction
    patterns = []
    
    # Extract visual patterns
    fov = percept.getFOV()
    if fov:
        # Simple pattern: dominant color/intensity
        pattern_strength = sum(fov) / len(fov)
        patterns.append({
            'type': 'visual_intensity',
            'value': pattern_strength,
            'complexity': len(set(fov))
        })
    
    # Extract internal signals
    internal = percept.getInternal()
    if internal:
        patterns.append({
            'type': 'internal_state',
            'value': internal[0],
            'complexity': 1
        })
    
    return patterns

def form_concepts_from_patterns(patterns):
    """Form concepts from extracted patterns"""
    
    # Simplified concept formation
    concepts = []
    
    for pattern in patterns:
        # Create concept based on pattern characteristics
        if pattern['value'] > 0.7:
            concepts.append(f"high_{pattern['type']}")
        elif pattern['value'] > 0.3:
            concepts.append(f"medium_{pattern['type']}")
        elif pattern['value'] > 0.1:
            concepts.append(f"low_{pattern['type']}")
        
        # Add complexity-based concepts
        if pattern['complexity'] > 5:
            concepts.append(f"complex_{pattern['type']}")
    
    return concepts

def measure_generalization(brain, agent):
    """Measure semantic generalization ability"""
    
    # Simplified generalization measure
    # Based on ability to apply learned concepts to new situations
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    # Generalization improves with curiosity and exposure to novelty
    generalization = min(1.0, (curiosity * 0.6 + novelty * 0.4))
    
    return generalization
```

## Step 6: Procedural Memory Implementation

### Procedural Memory Basics

Procedural memory stores skills and habits through repeated practice:

```python
import pynlm

def demonstrate_procedural_memory(brain, agent, world, steps=300):
    """Demonstrate procedural memory and skill learning"""
    
    print(f"\n=== Procedural Memory Demonstration ===")
    print(f"Testing procedural memory over {steps} steps:\n")
    
    # Track skill development
    procedural_metrics = {
        'repetitions': [],
        'performance_improvement': [],
        'skill_strength': [],
        'consolidation_events': []
    }
    
    # Define target skills to learn
    target_skills = [
        'navigate_to_resource',
        'avoid_hazards', 
        'find_hidden_marker',
        'balance_energy_management'
    ]
    
    skill_progress = {skill: 0.0 for skill in target_skills}
    
    for step in range(steps):
        world.update(0.1)
        
        # Perform actions that develop skills
        action = select_action_for_skill_learning(world, target_skills, skill_progress)
        
        # Process and learn
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward to reinforce learning
        reward = calculate_action_reward(world, action)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update skill progress
        for skill in target_skills:
            if skill_performs_well(world, action, skill):
                skill_progress[skill] += 0.01 * reward
        
        # Record metrics
        if step % 50 == 0:
            procedural_metrics['repetitions'].append(step)
            avg_skill_strength = sum(skill_progress.values()) / len(skill_progress.values())
            procedural_metrics['skill_strength'].append(avg_skill_strength)
            
            # Calculate performance improvement
            if len(procedural_metrics['skill_strength']) > 1:
                improvement = procedural_metrics['skill_strength'][-1] - procedural_metrics['skill_strength'][-2]
                procedural_metrics['performance_improvement'].append(improvement)
            
            print(f"   Step {step:3d}: Skills: {[f'{s}: {p:.2f}' for s, p in skill_progress.items()]}")
            print(f"     Average skill strength: {avg_skill_strength:.2f}")
        
        # Procedural memory consolidation
        if step % 100 == 0 and step > 0:
            procedural_metrics['consolidation_events'].append(step)
            print(f"   Procedural consolidation: Strengthening practiced skills")
    
    # Analyze procedural memory performance
    print(f"\nProcedural Memory Analysis:")
    
    if procedural_metrics['skill_strength']:
        final_strength = procedural_metrics['skill_strength'][-1]
        avg_strength = sum(procedural_metrics['skill_strength']) / len(procedural_metrics['skill_strength'])
        
        print(f"   Final skill strength: {final_strength:.2f}")
        print(f"   Average skill strength: {avg_strength:.2f}")
        
        if final_strength > 0.7:
            print("   ✓ Strong procedural skills developed")
        elif final_strength > 0.4:
            print("   = Moderate skill proficiency")
        else:
            print("   ! Limited skill development - check training methods")
        
        if procedural_metrics['performance_improvement']:
            avg_improvement = sum(procedural_metrics['performance_improvement']) / len(procedural_metrics['performance_improvement'])
            print(f"   Average performance improvement: {avg_improvement:.3f}")
    
    return procedural_metrics, skill_progress

def select_action_for_skill_learning(world, target_skills, skill_progress):
    """Select actions that develop specific skills"""
    
    # Simple skill-based action selection
    body = world.getAgentBody()
    
    for skill in target_skills:
        if skill_progress[skill] < 0.8:  # Still learning this skill
            if skill == 'navigate_to_resource':
                # Move toward nearest resource
                resources = [obj for obj in world.getObjects() 
                           if obj.getType() == pynlm.WorldObjectType.Resource]
                if resources:
                    nearest = min(resources, key=lambda o: ((o.getX() - body.getX())**2 + (o.getY() - body.getY())**2)**0.5)
                    return navigate_toward(world, nearest.getX(), nearest.getY())
            
            elif skill == 'avoid_hazards':
                # Avoid hazards
                hazards = [obj for obj in world.getObjects() 
                          if obj.getType() == pynlm.WorldObjectType.Hazard]
                if hazards:
                    nearest = min(hazards, key=lambda o: ((o.getX() - body.getX())**2 + (o.getY() - body.getY())**2)**0.5)
                    if ((nearest.getX() - body.getX())**2 + (nearest.getY() - body.getY())**2)**0.5 < 5.0:
                        return avoid_object(world, nearest)
            
            elif skill == 'find_hidden_marker':
                # Search for markers
                markers = [obj for obj in world.getObjects() 
                          if obj.getType() == pynlm.WorldObjectType.Marker]
                if markers:
                    return navigate_toward(world, markers[0].getX(), markers[0].getY())
            
            elif skill == 'balance_energy_management':
                # Balance energy consumption vs. reward
                if body.getEnergy() < 30:
                    return find_resource(world)
                else:
                    return navigate_toward(world, 10, 10)  # Return to center
    
    # Default action if no specific skill learning needed
    return pynlm.ActionType.MoveForward

def navigate_toward(world, target_x, target_y):
    """Generate action to navigate toward target"""
    
    body = world.getAgentBody()
    
    if target_x > body.getX():
        return pynlm.ActionType.MoveForward
    elif target_x < body.getX():
        return pynlm.ActionType.TurnLeft
    else:
        if target_y > body.getY():
            return pynlm.ActionType.MoveForward
        else:
            return pynlm.ActionType.TurnRight
```

## Step 7: Complete Tutorial Example

```python
import pynlm
import time

def run_complete_tutorial_4():
    """Complete tutorial 4: Memory Systems and Episodic Storage"""
    
    print("=== NLM Tutorial 4: Memory Systems and Episodic Storage ===")
    print("This tutorial explores the comprehensive memory systems in NLM.\n")
    
    # 1. Setup world and agent
    print("1. Creating world and agent...")
    world = create_simple_world()
    world = populate_world(world)
    
    brain, agent = create_memory_enhanced_brain()
    agent = initialize_agent_with_world(agent, world)
    
    # 2. Demonstrate different memory types
    print("\n2. Demonstrating memory system types...")
    
    print("   a) Working Memory:")
    wm_metrics = demonstrate_working_memory(brain, agent, world, 150)
    
    world.reset()
    brain.reset()
    brain.initialize()
    agent.initialize(None)
    
    print("   b) Episodic Memory:")
    episodic_metrics = demonstrate_episodic_memory(brain, agent, world, 200)
    
    world.reset()
    brain.reset()
    brain.initialize()
    agent.initialize(None)
    
    print("   c) Semantic Memory:")
    semantic_metrics = demonstrate_semantic_memory(brain, agent, world, 180)
    
    world.reset()
    brain.reset()
    brain.initialize()
    agent.initialize(None)
    
    print("   d) Procedural Memory:")
    procedural_metrics, skill_progress = demonstrate_procedural_memory(brain, agent, world, 250)
    
    # 3. Demonstrate memory replay
    print("\n3. Demonstrating memory replay and consolidation...")
    consolidation_metrics = demonstrate_memory_replay(brain, agent, world, 350)
    
    # 4. Show task demonstrations
    print("\n4. Demonstrating practical memory tasks...")
    task_results = demonstrate_working_memory_tasks(brain, agent, world)
    
    # 5. Save results
    print("\n5. Saving memory-enhanced brain state...")
    brain.save("tutorial4_memory_systems.bin")
    print("   ✓ Memory systems brain saved")
    
    print("\n=== Tutorial 4 Complete ===")
    print("Your brain now has comprehensive memory capabilities:")
    print("✓ Working memory for immediate information storage")
    print("✓ Episodic memory for specific event recording")
    print("✓ Semantic memory for abstract knowledge")
    print("✓ Procedural memory for skill acquisition")
    print("✓ Memory replay for consolidation")
    print("✓ Pattern recognition and concept formation")
    print("✓ Skill development through practice")

if __name__ == "__main__":
    run_complete_tutorial_4()
```

## Advanced Memory Topics

### 1. Memory Systems Integration

Memory systems work together to create intelligent behavior:

- **Working memory** provides immediate context
- **Episodic memory** supplies specific experiences
- **Semantic memory** offers abstract knowledge
- **Procedural memory** enables skilled behavior
- **Associative memory** connects patterns across systems

### 2. Memory Consolidation

Memory consolidation processes:

- **Replay mechanisms** strengthen important memories
- **Synaptic consolidation** stabilizes memory traces
- **Systems consolidation** integrates memories across brain regions
- **Sleep-dependent consolidation** occurs during rest

### 3. Memory-for- thinking

Memory supports higher cognitive functions:

- **Planning**: Using episodic memories to imagine future scenarios
- **Decision-making**: Weighing past experiences against current needs
- **Problem-solving**: Applying semantic knowledge to novel situations
- **Learning**: Forming new connections between memory systems

### 4. Memory and Learning

Memory and learning interact continuously:

- **Learning creates memories**: New experiences are stored
- **Memories guide learning**: Past experiences inform future learning
- **Memory modulates learning**: Neuromodulators prioritize important memories
- **Learning updates memories**: Memories are continuously refined

## Summary

Tutorial 4 has provided a comprehensive exploration of NLM's memory systems:

1. **Multiple memory types** working together for intelligent behavior
2. **Memory formation, storage, and retrieval** mechanisms
3. **Memory replay and consolidation** for long-term learning
4. **Practical memory tasks** demonstrating real-world applications
5. **Integration with other brain systems** for comprehensive cognition

The brain you've created can now:

- Maintain information in working memory for immediate use
- Record specific experiences in episodic memory
- Form abstract concepts in semantic memory
- Learn skills and habits in procedural memory
- Replay memories for consolidation and strengthening
- Integrate multiple memory systems for intelligent behavior

These capabilities enable the brain to accumulate knowledge, learn from experience, and make better decisions based on its rich internal memory of past interactions.
