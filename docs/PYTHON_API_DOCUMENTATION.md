# NLM Enhanced Python API Documentation

## Overview

This document provides comprehensive documentation for the enhanced Python bindings (pynlm_enhanced) that extend the original NLM Python interface with advanced memory systems, prediction capabilities, and performance optimization features.

## Introduction

The enhanced Python bindings provide a unified interface to the NLM C++ neural simulation framework with the following key improvements:

- **Neural Memory Systems**: Semantic, procedural, working, and episodic memory
- **Prediction Integration**: Real-time prediction and error computation
- **Performance Optimizations**: Batch processing, spike optimization, memory management
- **Advanced Learning**: Concept discovery, skill learning, pattern recognition
- **Neuromodulation Controls**: Dopamine, curiosity, novelty, prediction error systems

## API Structure

### Module

```python
import pynlm_enhanced as nlm
```

### Core Classes

#### Brain
```python
class nlm.Brain:
    """Enhanced neural simulation brain class with memory and prediction integration"""
    
    def __init__(self, config=None):
        """Create a new enhanced brain with configuration"""
        pass
    
    def initialize(self):
        """Initialize the brain with configuration"""
        pass
    
    def step(self, current_step):
        """Perform a simulation step"""
        pass
    
    def step_with_time(self, current_step, current_time):
        """Perform a simulation step with timestamp"""
        pass
    
    def receive_sensory_input(self, input):
        """Inject sensory input into the brain"""
        pass
    
    def produce_action(self):
        """Produce motor action based on neural activity"""
        pass
```

#### Config
```python
class nlm.Config:
    """Configuration class for NLM system"""
    
    def load_from_file(self, filepath):
        """Load configuration from a JSON file"""
        pass
    
    def load_from_args(self, argc, argv):
        """Load configuration from command line arguments"""
        pass
    
    def save_to_file(self, filepath):
        """Save configuration to a JSON file"""
        pass
    
    def has(self, key):
        """Check if a configuration key exists"""
        pass
    
    def get_keys(self):
        """Get all configuration keys"""
        pass
    
    def set_value(self, key, value):
        """Set configuration value"""
        pass
    
    def get_value(self, key, default_value=""):
        """Get configuration value"""
        pass
```

#### AgentBrain
```python
class nlm.AgentBrain:
    """Enhanced agent brain interface with advanced memory and prediction"""
    
    def __init__(self, brain):
        """Initialize with brain reference"""
        pass
    
    def initialize(self, world):
        """Initialize with world"""
        pass
    
    def process_sensory_input(self, percept):
        """Process sensory percept and inject into brain"""
        pass
    
    def decode_motor_command(self):
        """Decode brain motor activity into motor command"""
        pass
```

## 1. Memory System API

### 1.1 Neural Semantic Memory

#### Overview
Semantic memory stores factual knowledge using neural pattern representations rather than traditional key-value storage.

#### Key Methods

```python
# Store facts as neural patterns
brain.semanticMemory.store_fact(key, value)

# Retrieve facts via pattern completion
brain.semanticMemory.retrieve_fact(key)

# Check fact existence
brain.semanticMemory.has_fact(key)

# Update existing facts
brain.semanticMemory.update_fact(key, new_value)

# Get all stored facts
brain.semanticMemory.get_all_facts()

# Find similar concepts from neural patterns
brain.semanticMemory.find_similar_concept(features, threshold=0.5)

# Learn new concepts from patterns
brain.semanticMemory.learn_concept(concept, features)

# Get semantic memory statistics
brain.semanticMemory.get_semantic_activity()
brain.semanticMemory.get_fact_count()
```

#### Usage Examples

```python
# Basic semantic memory usage
brain.semanticMemory.store_fact("temperature", "warm")
brain.semanticMemory.store_fact("location", "kitchen")
brain.semanticMemory.store_fact("time", "morning")

# Retrieve information
weather = brain.semanticMemory.retrieve_fact("temperature")
location = brain.semanticMemory.retrieve_fact("location")

# Concept discovery from neural patterns
features = [0.1, 0.5, 0.9, 0.3]
concept = brain.semanticMemory.find_similar_concept(features)

# Advanced concept learning
brain.semanticMemory.learn_concept(
    "hot_drink", 
    [0.8, 0.3, 0.7, 0.2]  # Temperature, texture, aroma, etc.
)

# Statistics and monitoring
activity = brain.semanticMemory.get_semantic_activity()
all_facts = brain.semanticMemory.get_all_facts()
print(f"Semantic memory contains {len(all_facts)} facts")
print(f"Current semantic activity level: {activity:.2f}")
```

### 1.2 Neural Procedural Memory

#### Overview
Procedural memory stores skills and habits as neural pathways with hierarchical organization and reinforcement learning.

#### Key Methods

```python
# Learn new skills from neural patterns
brain.proceduralMemory.learn_skill(skill_name, pattern)

# Retrieve skills for execution
brain.proceduralMemory.retrieve_skill(skill_name)

# Get skill proficiency for performance modulation
brain.proceduralMemory.get_skill_proficiency(skill_name)

# Update skill proficiency through practice/reinforcement
brain.proceduralMemory.update_skill_proficiency(skill_name, delta)

# Get all learned skills
brain.proceduralMemory.get_all_skill_names()

# Execute skills with pattern completion
brain.proceduralMemory.execute_skill(skill_name)

# Get procedural memory statistics
brain.proceduralMemory.get_procedural_activity()
brain.proceduralMemory.get_skill_count()
```

#### Usage Examples

```python
# Learn basic motor skills
brain.proceduralMemory.learn_skill("move_forward", [0.8, 0.9, 0.7])
brain.proceduralMemory.learn_skill("turn_left", [0.2, 0.8, 0.3])
brain.proceduralMemory.learn_skill("turn_right", [0.8, 0.2, 0.3])

# Learn compound skills
brain.proceduralMemory.learn_skill(
    "navigate_to_goal", 
    [0.5, 0.8, 0.6, 0.9, 0.4]  # Sequence of actions
)

# Execute learned skills
brain.proceduralMemory.execute_skill("move_forward")

# Get skill proficiency
move_forward_proficiency = brain.proceduralMemory.get_skill_proficiency("move_forward")
print(f"Move forward proficiency: {move_forward_proficiency:.2f}")

# Update proficiency through practice
brain.proceduralMemory.update_skill_proficiency("move_forward", 0.1)

# Get all skills and their progress
all_skills = brain.proceduralMemory.get_all_skill_names()
for skill in all_skills:
    proficiency = brain.proceduralMemory.get_skill_proficiency(skill)
    print(f"{skill}: {proficiency:.2f}")

# Hierarchical skill activation
brain.proceduralMemory.execute_skill("navigate_to_goal")
```

### 1.3 Neural Working Memory

#### Overview
Working memory provides temporary storage of current sensory information and active neural representations.

#### Key Methods

```python
# Store current neural activity
brain.workingMemory.store(neuron_id, value)

# Retrieve stored values
brain.workingMemory.retrieve(neuron_id)

# Check if item exists
brain.workingMemory.contains(neuron_id)

# Get memory capacity information
brain.workingMemory.get_capacity()
brain.workingMemory.get_current_size()

# Manage memory traces
brain.workingMemory.get_active_traces()
brain.workingMemory.get_memory_neuron_ids()

# Decay and cleanup
brain.workingMemory.decay(decay_rate)
brain.workingMemory.clear()

# Working memory statistics
brain.workingMemory.get_load_ratio()
brain.workingMemory.get_average_trace_strength()
```

#### Usage Examples

```python
# Store current sensory patterns
brain.workingMemory.store(12345, 0.8)
brain.workingMemory.store(12346, 0.6)
brain.workingMemory.store(12347, 0.9)

# Retrieve current information
current_neuron_value = brain.workingMemory.retrieve(12345)

# Monitor working memory load
active_traces = brain.workingMemory.get_active_traces()
print(f"Active working memory traces: {active_traces}")

# Manage memory capacity
current_size = brain.workingMemory.get_current_size()
capacity = brain.workingMemory.get_capacity()
print(f"Working memory utilization: {current_size}/{capacity}")

# Natural decay mechanism
brain.workingMemory.decay(0.01)  # 1% decay per step
```

### 1.4 Neural Episodic Memory

#### Overview
Episodic memory stores experience episodes with temporal and spatial indexing for replay and consolidation.

#### Key Methods

```python
# Store experience episodes
brain.episodicMemory.store_episode(content, importance=1.0)

# Retrieve similar episodes
brain.episodicMemory.retrieve_similar_episodes(query, max_results=10)

# Get episode statistics
brain.episodicMemory.get_episode_count()
brain.episodicMemory.get_recent_episodes(count)

# Memory consolidation
brain.episodicMemory.consolidate(threshold)
brain.episodicMemory.get_consolidation_status()

# Replay mechanisms
brain.episodicMemory.enable_replay(True)
brain.episodicMemory.replay_episodes(count)

# Temporal and spatial indexing
brain.episodicMemory.get_episodes_by_time_range(start_time, end_time)
brain.episodicMemory.get_episodes_by_location(x, y, radius)

# Advanced episodic operations
brain.episodicMemory.get_memory_replay_status()
brain.episodicMemory.get_episode_statistics()
```

#### Usage Examples

```python
# Store important experiences
brain.episodicMemory.store_episode("Found food at (5, 10)", importance=1.0)
brain.episodicMemory.store_episode("Avoided predator at (8, 3)", importance=0.8)
brain.episodicMemory.store_episode("Explored new area", importance=0.5)

# Retrieve relevant experiences
relevant_episodes = brain.episodicMemory.retrieve_similar_episodes(
    "food", max_results=5
)

# Monitor memory storage
episode_count = brain.episodicMemory.get_episode_count()
print(f"Total episodes stored: {episode_count}")

# Enable memory replay for learning
brain.episodicMemory.enable_replay(True)
recent_episodes = brain.episodicMemory.get_recent_episodes(3)
brain.episodicMemory.replay_episodes(recent_episodes)

# Consolidate important memories
brain.episodicMemory.consolidate(0.7)
```

## 2. Prediction System API

### 2.1 Neural Prediction

#### Overview
Prediction system learns temporal sequences and predicts future states for proactive behavior.

#### Key Methods

```python
# Make predictions based on current state
brain.predict_next_state(current_state)

# Get prediction error for learning
brain.get_prediction_error(predicted_state, actual_state)

# Get prediction confidence
brain.get_prediction_confidence(state)

# Update prediction system with outcomes
brain.update_prediction(state, reward)

# Get prediction system statistics
brain.get_prediction_stats()
brain.get_prediction_history()
brain.clear_prediction_history()

# Prediction system controls
brain.enable_prediction(True)
brain.get_prediction_enabled()
```

#### Usage Examples

```python
# Get current state from neural activity
current_state = brain.get_current_neural_state()

# Make predictions
predicted_next = brain.predict_next_state(current_state)
print(f"Predicted next state: {predicted_next}")

# Update based on actual outcomes
brain.update_prediction(current_state, reward=1.0)

# Get prediction accuracy
prediction_error = brain.get_prediction_error(
    predicted_next, 
    actual_next_state
)
print(f"Prediction error: {prediction_error:.4f}")

# Get confidence in predictions
confidence = brain.get_prediction_confidence(current_state)
print(f"Prediction confidence: {confidence:.2f}")

# Monitor prediction system performance
stats = brain.get_prediction_stats()
print(f"Predictions made: {stats['prediction_count']}")
print(f"Successful predictions: {stats['successful_predictions']}")
print(f"Prediction accuracy: {stats['accuracy']:.2f}")
```

### 2.2 Neuromodulation Integration

#### Overview
Neuromodulation systems integrate with prediction error signals and affect memory consolidation and learning rates.

#### Key Methods

```python
# Get neuromodulator levels
brain.get_dopamine_level()
brain.get_curiosity_level()
brain.get_novelty_level()
brain.get_prediction_error_level()
brain.get_reward_level()

# Get all neuromodulation metrics
brain.get_all_neuromodulation_levels()

# Enable/disable neuromodulation systems
brain.enable_reward_modulation(True)
brain.enable_structural_plasticity(True)
brain.enable_development(True)
brain.enable_curiosity(True)

# Check neuromodulation status
brain.is_reward_modulation_enabled()
brain.is_structural_plasticity_enabled()
brain.is_development_enabled()
brain.is_curiosity_enabled()

# Neuromodulation controls
brain.set_neuromodulation_factor(factor)
brain.get_neuromodulation_stats()
```

#### Usage Examples

```python
# Monitor neuromodulator levels
brain.get_dopamine_level()
brain.get_curiosity_level()
brain.get_prediction_error_level()

# Enable adaptive learning
brain.enable_reward_modulation(True)
brain.enable_curiosity(True)

# Check system status
if brain.is_reward_modulation_enabled():
    print("Reward modulation: ACTIVE")
else:
    print("Reward modulation: INACTIVE")

# Get comprehensive neuromodulation metrics
metrics = brain.get_all_neuromodulation_levels()
print(f"Dopamine level: {metrics['dopamine_level']:.3f}")
print(f"Curiosity level: {metrics['curiosity_level']:.3f}")
print(f"Novelty level: {metrics['novelty_level']:.3f}")
print(f"Prediction error: {metrics['prediction_error_level']:.3f}")

# Adjust neuromodulation based on performance
if brain.get_prediction_error_level() > 0.5:
    brain.set_neuromodulation_factor(0.8)
```

## 3. Advanced Learning API

### 3.1 Concept Discovery

#### Overview
Automatically discover abstract concepts from neural patterns and experiences without requiring labeled training data.

#### Key Methods

```python
# Discover new concepts from data
brain.discover_concepts(min_samples=10)

# Get concept discovery results
brain.get_concept_discovery_results()

# Find concepts for new patterns
brain.find_concept(features, threshold=0.5)

# Manage concept stability
brain.is_concept_stable(concept_id)
brain.get_concept_stability(concept_id)
brain.get_concept_generalization(concept_id)

# Advanced concept operations
brain.merge_concepts(concept_a, concept_b)
brain.get_concept_instances(concept_id)
```

#### Usage Examples

```python
# Automatically discover concepts from accumulated experience
brain.discover_concepts(min_samples=20)

# Get discovered concepts
concepts = brain.get_concept_discovery_results()
for concept_id, concept_data in concepts.items():
    print(f"Concept {concept_id}:")
    print(f"  Stability: {concept_data['stability']:.2f}")
    print(f"  Category: {concept_data['category']}")
    print(f"  Instances: {concept_data['instance_count']}")

# Find similar concepts for new patterns
new_features = [0.1, 0.5, 0.9, 0.3]
concept_id = brain.find_concept(new_features, threshold=0.6)

# Check concept stability
if concept_id and brain.is_concept_stable(concept_id):
    print(f"Concept {concept_id} is stable for use")

# Get concept details
if concept_id:
    stability = brain.get_concept_stability(concept_id)
    generalization = brain.get_concept_generalization(concept_id)
    print(f"Concept stability: {stability:.2f}")
    print(f"Generalization ability: {generalization:.2f}")
```

### 3.2 Skill Learning

#### Overview
Learn procedural skills through neural pattern formation and reinforcement learning.

#### Key Methods

```python
# Learn new skills
brain.learn_skill(skill_name, parameters)

# Retrieve learned skills
brain.retrieve_skill(skill_name)

# Get all learned skills
brain.get_all_skills()

# Get skill learning progress
brain.get_skill_learning_progress()

# Advanced skill operations
brain.update_skill_proficiency(skill_name, delta)
brain.is_skill_consolidated(skill_name)
```

#### Usage Examples

```python
# Learn basic motor skills
brain.learn_skill("move_forward", [0.8, 0.9, 0.7])
brain.learn_skill("turn_left", [0.2, 0.8, 0.3])
brain.learn_skill("turn_right", [0.8, 0.2, 0.3])

# Learn compound skills
brain.learn_skill(
    "navigate_to_goal", 
    {
        "action_sequence": ["move_forward", "turn_left", "move_forward"],
        "parameters": [0.7, 0.5, 0.8]
    }
)

# Get learning progress
progress = brain.get_skill_learning_progress()
for skill, skill_progress in progress.items():
    print(f"{skill}: {skill_progress['proficiency']:.2f} (sessions: {skill_progress['sessions']})")

# Execute learned skills
brain.retrieve_skill("move_forward")
```

### 3.3 Pattern Recognition

#### Overview
Identify and recognize patterns in neural activity for predictive behavior and decision making.

#### Key Methods

```python
# Find pattern matches in neural activity
brain.get_pattern_matches(pattern, max_results=5)

# Get pattern recognition results
brain.get_pattern_recognition_results(pattern)

# Get pattern recognition statistics
brain.get_pattern_recognition_stats()

# Pattern management
brain.add_pattern(pattern, label)
brain.get_pattern(pattern_id)
```

#### Usage Examples

```python
# Find pattern matches for reactive decision making
current_pattern = brain.get_current_neural_pattern()
matches = brain.get_pattern_matches(current_pattern, max_results=3)

for match in matches:
    print(f"Pattern match: {match['similarity']:.2f} for {match['pattern_id']}")

# Get detailed pattern recognition results
results = brain.get_pattern_recognition_results(current_pattern)
print(f"Best match confidence: {results['best_confidence']:.2f}")
print(f"Alternative patterns: {results['alternative_count']}")

# Monitor pattern recognition performance
stats = brain.get_pattern_recognition_stats()
print(f"Patterns recognized: {stats['pattern_count']}")
print(f"Recognition accuracy: {stats['accuracy']:.2f}")
```

## 4. Performance Optimization API

### 4.1 Batch Processing

#### Overview
Process multiple sensory inputs and neural operations efficiently for improved performance.

#### Key Methods

```python
# Process batch sensory input
brain.batch_sensory_input(inputs)

# Optimize spike processing
brain.optimize_spike_processing(enable)

# Get performance metrics
brain.get_performance_metrics()
brain.get_memory_stats()
brain.get_prediction_stats()
brain.get_plasticity_stats()
```

#### Usage Examples

```python
# Process multiple sensory inputs efficiently
sensory_inputs = [
    generate_sensory_input(),
    generate_sensory_input(),
    generate_sensory_input()
]

brain.batch_sensory_input(sensory_inputs)

# Enable spike processing optimization
brain.optimize_spike_processing(True)

# Monitor system performance
performance = brain.get_performance_metrics()
print(f"Processing efficiency: {performance['efficiency']:.2f}")
print(f"Memory usage: {performance['memory_usage_mb']} MB")
print(f"CPU utilization: {performance['cpu_usage_percent']:.1f}%")
```

### 4.2 Memory Management

#### Overview
Advanced memory management for large-scale neural simulations with efficient resource utilization.

#### Key Methods

```python
# Get comprehensive memory statistics
brain.get_memory_stats()

# Get prediction system statistics
brain.get_prediction_stats()

# Get plasticity system statistics
brain.get_plasticity_stats()

# Memory optimization controls
brain.enable_memory_pools(enable)
brain.enable_event_queues(enable)
brain.enable_sparse_connectivity(enable)

# Memory monitoring
brain.get_memory_usage()
brain.get_gpu_memory_usage()
brain.get_active_memory_allocations()
```

#### Usage Examples

```python
# Get comprehensive memory statistics
memory_stats = brain.get_memory_stats()
print("=== Memory System Statistics ===")
print(f"Semantic Memory: {memory_stats['semantic_memory']['fact_count']} facts")
print(f"Working Memory: {memory_stats['working_memory']['active_traces']} traces")
print(f"Episodic Memory: {memory_stats['episodic_memory']['episode_count']} episodes")
print(f"Procedural Memory: {memory_stats['procedural_memory']['skill_count']} skills")

# Get prediction system memory
prediction_stats = brain.get_prediction_stats()
print("\n=== Prediction System ===")
print(f"Prediction models: {prediction_stats['model_count']}")
print(f"Training examples: {prediction_stats['training_examples']}")
print(f"Model accuracy: {prediction_stats['accuracy']:.2f}")

# Enable memory optimization
brain.enable_memory_pools(True)
brain.enable_event_queues(True)
brain.enable_sparse_connectivity(True)

# Monitor memory usage
usage = brain.get_memory_usage()
print(f"\nMemory usage: {usage['total_mb']} MB")
```

### 4.3 Advanced Controls

#### Overview
Fine-grained control over neural simulation parameters and performance tuning.

#### Key Methods

```python
# Developmental controls
brain.set_development_stage(stage_name)
brain.get_available_stages()
brain.is_development_enabled()
brain.enable_development(enable)

# Learning controls
brain.enable_reward_modulation(enable)
brain.enable_structural_plasticity(enable)
brain.enable_curiosity(enable)

# Performance tuning
brain.set_optimization_level(level)
brain.get_optimization_settings()

# Advanced features
brain.enable_memory_replay(enable)
brain.enable_concept_discovery(enable)
brain.enable_advanced_learning(enable)
```

#### Usage Examples

```python
# Set developmental stage
brain.set_development_stage("maturation")
available_stages = brain.get_available_stages()
print(f"Available development stages: {available_stages}")

# Enable adaptive learning features
brain.enable_reward_modulation(True)
brain.enable_curiosity(True)
brain.enable_structural_plasticity(True)

# Get development status
if brain.is_development_enabled():
    print("Development system: ACTIVE")

# Configure performance optimization
brain.set_optimization_level("balanced")
settings = brain.get_optimization_settings()
print(f"Optimization level: {settings['level']}")
print(f"Memory pools: {'enabled' if settings['memory_pools'] else 'disabled'}")
print(f"Event queues: {'enabled' if settings['event_queues'] else 'disabled'}")
```

## 5. Usage Examples

### 5.1 Basic Brain Usage

```python
import pynlm_enhanced as nlm
import numpy as np

# Create enhanced brain with configuration
config = nlm.Config()
config.set_value("neuron_count", 1000)
config.set_value("region_count", 2)

brain = nlm.Brain(config)
brain.initialize()

# Generate sensory input
sensory_input = nlm.Vision(width=64, height=64, channels=3)

# Process through brain
brain.receive_sensory_input(sensory_input)
for step in range(100):
    brain.step(step)

# Get motor action
action = brain.produce_action()
print(f"Produced action: {action.type}")

# Store experience in episodic memory
brain.episodicMemory.store_episode("Processed visual scene")

# Get performance statistics
stats = brain.get_performance_metrics()
print(f"Simulation efficiency: {stats['efficiency']:.2f}")
```

### 5.2 Advanced Usage with Memory Systems

```python
import pynlm_enhanced as nlm

# Create enhanced brain
brain = nlm.Brain()
brain.initialize()

# Enable all memory systems
brain.enable_advanced_learning(True)
brain.enable_memory_replay(True)

# Learn concepts from experience
for step in range(500):
    # Generate random sensory input
    sensory = nlm.SensoryInput.random(100)
    
    # Process through brain
    brain.process_sensory_input(sensory)
    brain.step(step)
    
    # Periodically discover new concepts
    if step % 50 == 0:
        brain.discover_concepts(min_samples=10)

# Analyze learned knowledge
concepts = brain.get_concept_discovery_results()
skills = brain.get_all_skills()

print(f"Discovered {len(concepts)} concepts")
print(f"Learned {len(skills)} skills")

# Use learned knowledge for decision making
for concept_id, concept_data in concepts.items():
    if concept_data['stability'] > 0.7:
        print(f"Using stable concept: {concept_id}")
```

### 5.3 Performance Optimization Examples

```python
import pynlm_enhanced as nlm
import time

# Create performance-optimized brain
brain = nlm.Brain()
brain.initialize()

# Enable performance optimizations
brain.optimize_spike_processing(True)
brain.enable_memory_pools(True)
brain.enable_event_queues(True)

# Process batch of sensory inputs
num_inputs = 100
sensory_inputs = [nlm.SensoryInput.random(50) for _ in range(num_inputs)]

# Measure performance
start_time = time.time()
brain.batch_sensory_input(sensory_inputs)

for step in range(num_inputs):
    brain.step(step)

end_time = time.time()
processing_time = end_time - start_time

print(f"Processed {num_inputs} inputs in {processing_time:.2f}s")
print(f"Average processing time: {processing_time/num_inputs*1000:.2f}ms per input")

# Get performance metrics
metrics = brain.get_performance_metrics()
print(f"Efficiency: {metrics['efficiency']:.2f}")
print(f"Memory usage: {metrics['memory_usage_mb']} MB")
```

## 6. Error Handling and Diagnostics

### 6.1 Error Handling

```python
import pynlm_enhanced as nlm

# Create brain with error checking
try:
    brain = nlm.Brain()
    brain.initialize()
    
    # Perform operations with error checking
    sensory = nlm.SensoryInput.random(100)
    brain.receive_sensory_input(sensory)
    
    action = brain.produce_action()
    print(f"Success: {action.type}")
    
except RuntimeError as e:
    print(f"Error: {e}")
    # Attempt recovery
    brain.reset()
    
except Exception as e:
    print(f"Unexpected error: {e}")
    # Log error for debugging
    log_error(str(e))
```

### 6.2 Diagnostics

```python
import pynlm_enhanced as nlm

# Diagnostic function
def diagnose_brain_system(brain):
    """Diagnose brain system health and performance"""
    issues = []
    warnings = []
    
    # Check memory systems
    memory_stats = brain.get_memory_stats()
    
    if memory_stats['semantic_memory']['fact_count'] == 0:
        warnings.append("No semantic facts stored")
    
    if memory_stats['working_memory']['active_traces'] > 0.8:
        warnings.append("Working memory near capacity")
    
    # Check prediction system
    prediction_stats = brain.get_prediction_stats()
    
    if prediction_stats['prediction_error'] > 1.0:
        issues.append("High prediction error")
    
    if prediction_stats['confidence'] < 0.3:
        warnings.append("Low prediction confidence")
    
    # Check neuromodulation
    neuromod = brain.get_all_neuromodulation_levels()
    
    if neuromod['dopamine_level'] < 0.1:
        warnings.append("Low dopamine level")
    
    if neuromod['curiosity_level'] < 0.2:
        warnings.append("Low curiosity level")
    
    # Performance checks
    performance = brain.get_performance_metrics()
    
    if performance['efficiency'] < 0.5:
        warnings.append("Low processing efficiency")
    
    return issues, warnings, {
        'memory_stats': memory_stats,
        'prediction_stats': prediction_stats,
        'neuromod_stats': neuromod,
        'performance_stats': performance
    }

# Run diagnostics
issues, warnings, system_state = diagnose_brain_system(brain)

print("=== Diagnostics Report ===")
print(f"Issues found: {len(issues)}")
print(f"Warnings: {len(warnings)}")

if issues:
    print("\nCritical Issues:")
    for issue in issues:
        print(f"  - {issue}")

if warnings:
    print("\nWarnings:")
    for warning in warnings:
        print(f"  - {warning}")

# Recovery procedures
if issues:
    print("\nAttempting recovery...")
    brain.reset()
    print("Recovery completed")
```

## 7. Advanced Features

### 7.1 Memory Replay

```python
# Enable memory replay for learning
brain.enable_memory_replay(True)

# Get replay status
replay_status = brain.get_memory_replay_status()
print(f"Replay enabled: {replay_status['enabled']}")
print(f"Replay interval: {replay_status['interval']} steps")

# Replay recent episodes
recent_episodes = brain.episodicMemory.get_recent_episodes(5)
brain.episodicMemory.replay_episodes(recent_episodes)
```

### 7.2 Concept Discovery Results

```python
# Get concept discovery results
concepts = brain.get_concept_discovery_results()

for concept_id, concept_data in concepts.items():
    print(f"\nConcept {concept_id}:")
    print(f"  Stability: {concept_data['stability']:.2f}")
    print(f"  Category: {concept_data['category']}")
    print(f"  Observations: {concept_data['observation_count']}")
    print(f"  Generalization: {concept_data['generalization']:.2f}")
    print(f"  Merged from: {concept_data.get('merged_from', [])}")
```

### 7.3 Pattern Recognition Statistics

```python
# Get pattern recognition statistics
stats = brain.get_pattern_recognition_stats()

print("=== Pattern Recognition Statistics ===")
print(f"Total patterns: {stats['pattern_count']}")
print(f"Recognition accuracy: {stats['accuracy']:.2f}")
print(f"Average confidence: {stats['average_confidence']:.2f}")
print(f"Pattern types: {stats['pattern_types']}")
print(f"Update frequency: {stats['update_frequency']:.2f}")
```

## 8. Configuration and Customization

### 8.1 Enhanced Configuration Options

```python
# Create enhanced configuration
config = nlm.Config()

# Memory system configuration
config.set_value("memory.semantic_enabled", True)
config.set_value("memory.procedural_enabled", True)
config.set_value("memory.working_memory_capacity", 1000)
config.set_value("memory.episodic_max_episodes", 5000)

# Prediction system configuration
config.set_value("prediction.enabled", True)
config.set_value("prediction.max_history", 10000)
config.set_value("prediction.confidence_threshold", 0.5)

# Neuromodulation configuration
config.set_value("neuromodulation.dopamine_strength", 1.0)
config.set_value("neuromodulation.curiosity_drive", 1.0)
config.set_value("neuromodulation.novelty_sensitivity", 1.0)

# Performance configuration
config.set_value("performance.optimization_level", "aggressive")
config.set_value("performance.enable_memory_pools", True)
config.set_value("performance.enable_event_queues", True)

# Save enhanced configuration
config.save_to_file("enhanced_config.json")
```

### 8.2 Custom Learning Parameters

```python
# Configure learning parameters
config.set_value("learning.concept_stability_threshold", 0.7)
config.set_value("learning.skill_proficiency_decay", 0.01)
config.set_value("learning.pattern_recognition_confidence", 0.6)
config.set_value("learning.development_plasticity_mod", 0.8)

# Set developmental stage
config.set_value("development.stage", "maturation")
config.set_value("development.plasticity_rate", 0.5)

# Configure neuromodulation
config.set_value("neuromodulation.dopamine_decay", 0.1)
config.set_value("neuromodulation.curiosity_increment", 0.05)
```

## 9. Integration Examples

### 9.1 Sensorimotor Loop

```python
import pynlm_enhanced as nlm

class Agent:
    def __init__(self):
        self.brain = nlm.Brain()
        self.world = nlm.SimpleWorld()
        self.initialize_agent()
    
    def initialize_agent(self):
        # Configure brain for agent interaction
        self.brain.initialize()
        
        # Set up world interaction
        self.world.configure(width=100, height=100, 
                           vision_width=32, vision_height=32)
        
        # Set agent starting position
        self.world.set_agent_start(50, 50)
    
    def run_step(self, step):
        # Get observation from world
        percept = self.world.get_sensory_percept()
        
        # Process sensory input through brain
        self.brain.process_sensory_input(percept)
        
        # Step brain
        self.brain.step(step)
        
        # Decode motor command
        action = self.brain.produce_action()
        
        # Apply action to world
        self.world.apply_motor_command(action, step)
        
        # Get reward
        reward = self.world.get_reward()
        
        # Apply neuromodulation
        self.brain.apply_reward_modulation(reward, predicted_reward=0.5)
        
        return reward
    
    def run_episode(self, max_steps=1000):
        for step in range(max_steps):
            reward = self.run_step(step)
            
            # Store experience
            self.brain.episodicMemory.store_episode(
                f"Step {step}: Reward {reward}",
                importance=min(abs(reward), 1.0)
            )
            
            # Periodically discover concepts
            if step % 100 == 0:
                self.brain.discover_concepts(min_samples=10)
            
            # Check for termination
            if self.world.get_agent_energy() <= 0:
                break
        
        return step

# Run agent
agent = Agent()
steps = agent.run_episode()
print(f"Episode completed in {steps} steps")

# Analyze learned behavior
concepts = agent.brain.get_concept_discovery_results()
skills = agent.brain.get_all_skills()

print(f"\nLearning Summary:")
print(f"  Concepts discovered: {len(concepts)}")
print(f"  Skills learned: {len(skills)}")
print(f"  Memory episodes: {agent.brain.episodicMemory.get_episode_count()}")
```

### 9.2 Multi-Agent Environment

```python
import pynlm_enhanced as nlm
import numpy as np

class MultiAgentEnvironment:
    def __init__(self, num_agents=3):
        self.agents = []
        self.world = nlm.SimpleWorld()
        
        # Initialize agents
        for i in range(num_agents):
            agent = self.create_agent(i)
            self.agents.append(agent)
        
        # Configure world
        self.world.configure(width=200, height=200,
                           vision_width=48, vision_height=48)
    
    def create_agent(self, agent_id):
        agent = Agent()
        agent.brain = nlm.Brain()
        agent.brain.initialize()
        
        # Set agent position
        x = np.random.uniform(20, 180)
        y = np.random.uniform(20, 180)
        agent.brain.get_body().set_position(x, y)
        
        return agent
    
    def step(self):
        # All agents perceive environment
        for agent in self.agents:
            # Store agent's position as sensory input
            x, y = agent.brain.get_body().get_position()
            sensory = nlm.Vision(width=48, height=48)
            agent.last_percept = sensory
            
            # Process through brain
            agent.brain.process_sensory_input(sensory)
        
        # Step all brains
        for agent in self.agents:
            agent.brain.step(agent.current_step)
            agent.current_step += 1
        
        # Apply agent interactions
        self.apply_agent_interactions()
        
        # Get rewards
        for agent in self.agents:
            agent.last_reward = self.world.get_reward(agent.brain.get_body())
    
    def apply_agent_interactions(self):
        # Check for collisions
        for i, agent1 in enumerate(self.agents):
            body1 = agent1.brain.get_body()
            
            for j, agent2 in enumerate(self.agents):
                if i != j:
                    body2 = agent2.brain.get_body()
                    distance = body1.distance_to(body2)
                    
                    if distance < body1.radius + body2.radius:
                        # Handle collision
                        self.handle_collision(agent1, agent2, distance)
    
    def handle_collision(self, agent1, agent2, distance):
        # Resolve collision
        separation = (agent1.brain.get_body().radius + 
                     agent2.brain.get_body().radius)
        
        if distance > 0:
            # Push agents apart
            overlap = separation - distance
            dx = agent2.brain.get_body().x - agent1.brain.get_body().x
            dy = agent2.brain.get_body().y - agent1.brain.get_body().y
            
            normalization = distance
            force_x = (dx / normalization) * overlap
            force_y = (dy / normalization) * overlap
            
            # Apply forces
            agent1.brain.get_body().apply_force(force_x * 0.1, force_y * 0.1, 0.1)
            agent2.brain.get_body().apply_force(-force_x * 0.1, -force_y * 0.1, 0.1)
        
        # Store collision experience
        agent1.brain.episodicMemory.store_episode(
            f"Collision with agent {agent2}", importance=0.8
        )
        agent2.brain.episodicMemory.store_episode(
            f"Collision with agent {agent1}", importance=0.8
        )
    
    def run_simulation(self, steps=500):
        for step in range(steps):
            self.step()
            
            # Periodically learn from experiences
            if step % 100 == 0:
                self.learn_from_experience()
            
            # Occasionally spawn new concepts
            if step % 200 == 0:
                self.spawn_concept_discovery()
        
        return self.get_environment_stats()
    
    def learn_from_experience(self):
        # All agents discover concepts from recent experiences
        for agent in self.agents:
            agent.brain.discover_concepts(min_samples=15)
    
    def spawn_concept_discovery(self):
        # Spawn concept discovery events
        for agent in self.agents:
            # Generate novel sensory inputs
            novel_percept = nlm.SensoryInput.random(64)
            
            # Store as concept discovery attempt
            agent.brain.episodicMemory.store_episode(
                "Concept discovery attempt", importance=0.3
            )
    
    def get_environment_stats(self):
        stats = {
            'total_steps': 0,
            'agent_count': len(self.agents),
            'total_concepts': 0,
            'total_skills': 0,
            'total_episodes': 0
        }
        
        for agent in self.agents:
            concepts = agent.brain.get_concept_discovery_results()
            skills = agent.brain.get_all_skills()
            episodes = agent.brain.episodicMemory.get_episode_count()
            
            stats['total_concepts'] += len(concepts)
            stats['total_skills'] += len(skills)
            stats['total_episodes'] += episodes
        
        return stats

# Run multi-agent simulation
env = MultiAgentEnvironment(num_agents=3)
stats = env.run_simulation(steps=500)

print("=== Multi-Agent Simulation Results ===")
print(f"Steps completed: {stats['total_steps']}")
print(f"Agents: {stats['agent_count']}")
print(f"Total concepts discovered: {stats['total_concepts']}")
print(f"Total skills learned: {stats['total_skills']}")
print(f"Total episodes stored: {stats['total_episodes']}")
```

## 10. Troubleshooting Guide

### 10.1 Common Issues and Solutions

#### Issue 1: No Concepts Discovered

**Symptoms:** `discover_concepts()` returns empty results
**Solutions:**
1. Increase `min_samples` parameter
2. Check if sensory input is being generated
3. Verify brain initialization
```python
# Solution: Increase sample size
brain.discover_concepts(min_samples=50)

# Solution: Verify input processing
sensory = generate_sensory_input()
if sensory.get_dimensions() < 10:
    print("Warning: Low-dimensional input may limit concept discovery")
```

#### Issue 2: Skills Not Learning

**Symptoms:** `learn_skill()` calls don't increase proficiency
**Solutions:**
1. Check reward signals
2. Verify skill pattern validity
3. Monitor neuromodulation levels
```python
# Solution: Check reward signals
reward = brain.get_prediction_error_level()
if reward < 0.1:
    print("Warning: Low reward may limit learning")

# Solution: Verify skill parameters
skull_pattern = brain.get_skill_neural_pattern("move_forward")
if skull_pattern and len(skull_pattern) < 3:
    print("Warning: Skill pattern may be too simple")
```

#### Issue 3: Memory Full

**Symptoms:** `get_memory_usage()` shows high utilization
**Solutions:**
1. Increase memory capacity
2. Enable memory consolidation
3. Clean up unused memories
```python
# Solution: Increase capacity
brain.config.set_value("memory.working_memory_capacity", 5000)
brain.config.set_value("memory.episodic_max_episodes", 20000)

# Solution: Enable consolidation
brain.episodicMemory.consolidate(0.5)

# Solution: Clean up unused memories
brain.workingMemory.clear_old_traces(threshold=0.01)
```

#### Issue 4: Prediction Errors High

**Symptoms:** `get_prediction_stats()` shows high error rates
**Solutions:**
1. Increase training data
2. Check prediction system configuration
3. Verify neuromodulation
```python
# Solution: Increase training data
brain.predictionSystem.add_training_examples(1000)

# Solution: Check configuration
config = brain.get_config()
if config.get_value("prediction.enabled", True):
    print("Prediction system is enabled")

# Solution: Check neuromodulation
metrics = brain.get_all_neuromodulation_levels()
print(f"Prediction error: {metrics['prediction_error_level']:.2f}")
```

### 10.2 Debug Scripts

#### Debug Script: Memory System Health Check

```python
#!/usr/bin/env python3
import pynlm_enhanced as nlm
import json
import time

def memory_system_health_check(brain):
    """Comprehensive memory system health check"""
    print("=== Memory System Health Check ===\n")
    
    # Check semantic memory
    print("1. Semantic Memory:")
    semantic_stats = brain.get_semantic_memory_stats()
    print(f"   Facts stored: {semantic_stats['fact_count']}")
    print(f"   Activity level: {semantic_stats['activity']:.2f}")
    print(f"   Pattern completion rate: {semantic_stats['completion_rate']:.2f}")
    
    # Check procedural memory
    print("\n2. Procedural Memory:")
    procedural_stats = brain.get_procedural_memory_stats()
    print(f"   Skills learned: {procedural_stats['skill_count']}")
    print(f"   Average proficiency: {procedural_stats['avg_proficiency']:.2f}")
    print(f"   Recent activity: {procedural_stats['recent_activity']:.2f}")
    
    # Check working memory
    print("\n3. Working Memory:")
    working_stats = brain.get_working_memory_stats()
    print(f"   Active traces: {working_stats['active_traces']}")
    print(f"   Capacity utilization: {working_stats['utilization']:.2f}")
    print(f"   Average trace strength: {working_stats['avg_strength']:.2f}")
    
    # Check episodic memory
    print("\n4. Episodic Memory:")
    episodic_stats = brain.get_episodic_memory_stats()
    print(f"   Episodes stored: {episodic_stats['episode_count']}")
    print(f"   Memory efficiency: {episodic_stats['efficiency']:.2f}")
    print(f"   Recent activity: {episodic_stats['recent_activity']:.2f}")
    
    # Check prediction system
    print("\n5. Prediction System:")
    prediction_stats = brain.get_prediction_stats()
    print(f"   Predictions made: {prediction_stats['prediction_count']}")
    print(f"   Prediction error: {prediction_stats['error']:.4f}")
    print(f"   Confidence: {prediction_stats['confidence']:.2f}")
    
    # Check neuromodulation
    print("\n6. Neuromodulation:")
    neuromod_stats = brain.get_all_neuromodulation_levels()
    print(f"   Dopamine: {neuromod_stats['dopamine_level']:.3f}")
    print(f"   Curiosity: {neuromod_stats['curiosity_level']:.3f}")
    print(f"   Novelty: {neuromod_stats['novelty_level']:.3f}")
    print(f"   Prediction Error: {neuromod_stats['prediction_error_level']:.3f}")
    
    # Check performance
    print("\n7. Performance:")
    performance_stats = brain.get_performance_metrics()
    print(f"   Processing efficiency: {performance_stats['efficiency']:.2f}")
    print(f"   Memory usage: {performance_stats['memory_usage_mb']} MB")
    print(f"   CPU utilization: {performance_stats['cpu_usage_percent']:.1f}%")
    
    return {
        'semantic_health': semantic_stats['activity'] > 0.1,
        'procedural_health': procedural_stats['avg_proficiency'] > 0.3,
        'working_memory_health': working_stats['utilization'] < 0.9,
        'episodic_memory_health': episodic_stats['efficiency'] > 0.5,
        'prediction_health': prediction_stats['error'] < 1.0,
        'neuromodulation_health': neuromod_stats['dopamine_level'] > 0.1,
        'performance_health': performance_stats['efficiency'] > 0.5
    }

def save_health_report(brain, filename="health_report.json"):
    """Save health report to JSON file"""
    health_data = {
        'timestamp': time.time(),
        'memory_health': memory_system_health_check(brain),
        'system_stats': {
            'semantic_facts': brain.get_semantic_fact_count(),
            'procedural_skills': brain.get_procedural_skill_count(),
            'working_traces': brain.get_working_memory_traces(),
            'episodic_episodes': brain.get_episodic_episode_count(),
            'prediction_error': brain.get_prediction_error_level(),
            'dopamine_level': brain.get_dopamine_level()
        }
    }
    
    with open(filename, 'w') as f:
        json.dump(health_data, f, indent=2)
    
    print(f"\nHealth report saved to {filename}")
    return health_data

# Usage example
if __name__ == "__main__":
    # Initialize brain
    brain = nlm.Brain()
    brain.initialize()
    
    # Run simulation steps
    for step in range(100):
        sensory = nlm.SensoryInput.random(50)
        brain.receive_sensory_input(sensory)
        brain.step(step)
        
        # Store some experiences
        if step % 10 == 0:
            brain.episodicMemory.store_episode(f"Step {step}")
    
    # Run health check
    health_status = memory_system_health_check(brain)
    
    # Save detailed report
    save_health_report(brain)
    
    # Summary
    healthy_systems = sum(health_status.values())
    total_systems = len(health_status)
    print(f"\n=== Health Summary ===")
    print(f"Healthy systems: {healthy_systems}/{total_systems}")
    
    if healthy_systems < total_systems:
        print("Warnings detected - consider running recovery procedures")
    else:
        print("All systems healthy - system is functioning normally")
```

#### Debug Script: Performance Monitoring

```python
#!/usr/bin/env python3
import pynlm_enhanced as nlm
import matplotlib.pyplot as plt
import numpy as np
import time

class PerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.metrics = {
            'time_steps': [],
            'efficiency': [],
            'memory_usage': [],
            'cpu_usage': [],
            'prediction_error': [],
            'semantic_activity': [],
            'working_memory_load': []
        }
        self.monitoring = False
    
    def start_monitoring(self, duration_steps=1000):
        """Start collecting performance metrics"""
        self.monitoring = True
        self.duration_steps = duration_steps
        print(f"Starting performance monitoring for {duration_steps} steps...")
    
    def stop_monitoring(self):
        """Stop monitoring and generate report"""
        self.monitoring = False
        self.generate_performance_report()
        return self.metrics
    
    def collect_metrics(self, step):
        """Collect metrics for current step"""
        if not self.monitoring:
            return
        
        # Get current metrics
        perf_metrics = self.brain.get_performance_metrics()
        memory_stats = self.brain.get_memory_stats()
        prediction_stats = self.brain.get_prediction_stats()
        
        # Record metrics
        self.metrics['time_steps'].append(step)
        self.metrics['efficiency'].append(perf_metrics['efficiency'])
        self.metrics['memory_usage'].append(perf_metrics['memory_usage_mb'])
        self.metrics['cpu_usage'].append(perf_metrics['cpu_usage_percent'])
        self.metrics['prediction_error'].append(prediction_stats['error'])
        self.metrics['semantic_activity'].append(
            memory_stats['semantic_memory']['activity'])
        self.metrics['working_memory_load'].append(
            memory_stats['working_memory']['utilization'])
    
    def generate_performance_report(self):
        """Generate performance analysis report"""
        print("\n=== Performance Report ===")
        
        # Calculate statistics
        if len(self.metrics['time_steps']) == 0:
            print("No metrics collected")
            return
        
        # Efficiency trends
        initial_efficiency = self.metrics['efficiency'][0]
        final_efficiency = self.metrics['efficiency'][-1]
        efficiency_change = ((final_efficiency - initial_efficiency) / 
                           initial_efficiency) * 100
        
        print(f"Efficiency Trend:")
        print(f"  Initial: {initial_efficiency:.2f}")
        print(f"  Final: {final_efficiency:.2f}")
        print(f"  Change: {efficiency_change:+.1f}%")
        
        # Memory usage
        avg_memory = np.mean(self.metrics['memory_usage'])
        max_memory = np.max(self.metrics['memory_usage'])
        print(f"\nMemory Usage:")
        print(f"  Average: {avg_memory:.1f} MB")
        print(f"  Maximum: {max_memory:.1f} MB")
        
        # Prediction error
        avg_error = np.mean(self.metrics['prediction_error'])
        print(f"\nPrediction Error:")
        print(f"  Average: {avg_error:.4f}")
        
        # Working memory load
        avg_load = np.mean(self.metrics['working_memory_load'])
        print(f"\nWorking Memory Load:")
        print(f"  Average utilization: {avg_load:.2f}")
        
        # Generate plots
        self.generate_plots()
    
    def generate_plots(self):
        """Generate performance visualization"""
        try:
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle('NLM Enhanced Performance Metrics', fontsize=16)
            
            # Efficiency plot
            axes[0, 0].plot(self.metrics['time_steps'], 
                          self.metrics['efficiency'], 'b-')
            axes[0, 0].set_title('Processing Efficiency')
            axes[0, 0].set_xlabel('Time Step')
            axes[0, 0].set_ylabel('Efficiency (0-1)')
            axes[0, 0].grid(True, alpha=0.3)
            
            # Memory usage plot
            axes[0, 1].plot(self.metrics['time_steps'],
                          self.metrics['memory_usage'], 'r-')
            axes[0, 1].set_title('Memory Usage')
            axes[0, 1].set_xlabel('Time Step')
            axes[0, 1].set_ylabel('Memory (MB)')
            axes[0, 1].grid(True, alpha=0.3)
            
            # Prediction error plot
            axes[1, 0].plot(self.metrics['time_steps'],
                          self.metrics['prediction_error'], 'g-')
            axes[1, 0].set_title('Prediction Error')
            axes[1, 0].set_xlabel('Time Step')
            axes[1, 0].set_ylabel('Error')
            axes[1, 0].grid(True, alpha=0.3)
            
            # Working memory load plot
            axes[1, 1].plot(self.metrics['time_steps'],
                          self.metrics['working_memory_load'], 'orange')
            axes[1, 1].set_title('Working Memory Load')
            axes[1, 1].set_xlabel('Time Step')
            axes[1, 1].set_ylabel('Utilization (0-1)')
            axes[1, 1].grid(True, alpha=0.3)
            
            plt.tight_layout()
            plt.savefig('performance_metrics.png', dpi=150, bbox_inches='tight')
            print(f"\nPerformance plot saved to 'performance_metrics.png'")
            
            # plt.show()  # Uncomment for interactive display
            
        except ImportError:
            print("\nMatplotlib not available - skipping plot generation")
        except Exception as e:
            print(f"\nError generating plots: {e}")

# Usage example
if __name__ == "__main__":
    # Initialize brain
    brain = nlm.Brain()
    brain.initialize()
    
    # Create performance monitor
    monitor = PerformanceMonitor(brain)
    
    # Run simulation with monitoring
    monitor.start_monitoring(duration_steps=500)
    
    for step in range(500):
        # Generate random sensory input
        sensory = nlm.SensoryInput.random(50)
        
        # Process through brain
        brain.receive_sensory_input(sensory)
        brain.step(step)
        
        # Store experience
        if step % 50 == 0:
            brain.episodicMemory.store_episode(f"Step {step}")
        
        # Collect metrics
        monitor.collect_metrics(step)
        
        # Enable performance optimizations periodically
        if step % 100 == 0:
            brain.optimize_spike_processing(True)
    
    # Stop monitoring and generate report
    final_metrics = monitor.stop_monitoring()
    
    print(f"\n=== Monitoring Complete ===")
    print(f"Collected {len(final_metrics['time_steps'])} data points")
    print("Check 'performance_metrics.png' for visual analysis")
```

## 11. Quick Reference

### 11.1 Core Classes Summary

| Class | Description | Key Methods |
|-------|-------------|-------------|
| **Brain** | Main neural simulation controller | `initialize()`, `step()`, `produce_action()` |
| **Config** | Configuration management | `load_from_file()`, `set_value()`, `get_value()` |
| **AgentBrain** | Agent-environment interface | `process_sensory_input()`, `decode_motor_command()` |

### 11.2 Memory Systems

| System | Purpose | Key Methods |
|--------|---------|-------------|
| **Semantic Memory** | Factual knowledge | `store_fact()`, `retrieve_fact()`, `find_similar_concept()` |
| **Procedural Memory** | Skills and habits | `learn_skill()`, `retrieve_skill()`, `execute_skill()` |
| **Working Memory** | Current information | `store()`, `retrieve()`, `get_active_traces()` |
| **Episodic Memory** | Experiences | `store_episode()`, `retrieve_similar_episodes()`, `replay_episodes()` |

### 11.3 Prediction System

| Method | Description |
|--------|-------------|
| `predict_next_state()` | Predict future states based on current neural activity |
| `get_prediction_error()` | Compute prediction accuracy error |
| `get_prediction_confidence()` | Get confidence in predictions |
| `update_prediction()` | Train prediction system with outcomes |

### 11.4 Neuromodulation

| System | Function | Key Methods |
|--------|---------|-------------|
| **Dopamine** | Reward-based learning | `get_dopamine_level()`, `enable_reward_modulation()` |
| **Curiosity** | Exploration motivation | `get_curiosity_level()`, `enable_curiosity()` |
| **Novelty** | Novelty detection | `get_novelty_level()` |
| **Prediction Error** | Error computation | `get_prediction_error_level()` |

### 11.5 Advanced Features

| Feature | Description | Methods |
|---------|-------------|---------|
| **Concept Discovery** | Automatic pattern discovery | `discover_concepts()`, `get_concept_discovery_results()` |
| **Skill Learning** | Procedural skill acquisition | `learn_skill()`, `get_skill_learning_progress()` |
| **Pattern Recognition** | Neural pattern identification | `get_pattern_matches()`, `get_pattern_recognition_stats()` |
| **Memory Replay** | Experience replay for consolidation | `enable_memory_replay()`, `replay_episodes()` |

### 11.6 Performance Optimizations

| Optimization | Purpose | Methods |
|--------------|---------|---------|
| **Batch Processing** | Efficient input processing | `batch_sensory_input()`, `optimize_spike_processing()` |
| **Memory Management** | Resource optimization | `get_memory_stats()`, `enable_memory_pools()` |
| **Event Queues** | Spike delivery optimization | `enable_event_queues()` |

## 12. Migration Guide

### 12.1 From pynlm to pynlm_enhanced

The enhanced bindings add significant functionality while maintaining backward compatibility:

#### Added Features

```python
# Original pynlm (basic functionality)
brain = pynlm.Brain(config)
brain.initialize()
action = brain.produce_action()

# Enhanced pynlm_enhanced (with memory and prediction)
brain = pynlm_enhanced.Brain(config)
brain.initialize()

# Memory systems (NEW)
brain.semanticMemory.store_fact("temperature", "warm")
brain.proceduralMemory.learn_skill("move", [0.8, 0.9])

# Prediction system (NEW)
prediction = brain.predict_next_state(current_state)

# Advanced learning (NEW)
brain.discover_concepts(min_samples=10)
brain.get_concept_discovery_results()
```

#### API Compatibility

Most methods from the original bindings continue to work with enhanced functionality:

```python
# Original API (now enhanced)
brain = pynlm_enhanced.Brain(config)  # Same constructor
brain.initialize()  # Same method
brain.step(step)  # Same method, enhanced integration
brain.produce_action()  # Same method, enhanced with memory

# Enhanced methods
brain.semanticMemory  # New - semantic memory system
brain.proceduralMemory  # New - procedural memory system
brain.get_prediction_stats()  # New - prediction system stats
brain.get_memory_stats()  # New - comprehensive memory statistics
```

### 12.2 Recommended Upgrades

#### Minimal Upgrade (Add Essential Features)

```python
# Upgrade minimal code to use enhanced features
import pynlm_enhanced as nlm

# Basic functionality (existing)
brain = nlm.Brain(config)
brain.initialize()
action = brain.produce_action()

# Add memory integration
brain.semanticMemory.store_fact("observation", "description")

# Add prediction capability
prediction = brain.predict_next_state(current_state)
```

#### Full Upgrade (All Enhanced Features)

```python
import pynlm_enhanced as nlm

# Create enhanced brain with all features
brain = nlm.Brain()
brain.initialize()

# Enable advanced learning
brain.discover_concepts(min_samples=10)
brain.enable_memory_replay(True)
brain.enable_advanced_learning(True)

# Set up performance optimizations
brain.optimize_spike_processing(True)
brain.enable_memory_pools(True)

# Run simulation with full integration
for step in range(1000):
    sensory = nlm.SensoryInput.random(50)
    brain.receive_sensory_input(sensory)
    brain.step(step)
    
    # Store experiences
    if step % 100 == 0:
        brain.episodicMemory.store_episode(f"Step {step}")

# Analyze learning results
concepts = brain.get_concept_discovery_results()
skills = brain.get_all_skills()
performance = brain.get_performance_metrics()
```

## 13. Installation and Setup

### 13.1 Installation

```bash
# Install enhanced bindings
pip install pynlm_enhanced

# Or install with development dependencies
pip install pynlm_enhanced[dev]
```

### 13.2 Quick Start

```python
import pynlm_enhanced as nlm

# Create configuration
config = nlm.Config()
config.set_value("neuron_count", 1000)
config.set_value("region_count", 2)

# Initialize enhanced brain
brain = nlm.Brain(config)
brain.initialize()

# Test basic functionality
print("Testing enhanced NLM functionality...")
print(f"Brain initialized with {brain.get_total_neuron_count()} neurons")
print(f"Performance: {brain.get_performance_metrics()['efficiency']:.2f}")

# Test memory systems
brain.semanticMemory.store_fact("test", "success")
brain.proceduralMemory.learn_skill("test_action", [0.5, 0.8])

print("Memory systems operational!")
```

## 14. References

1. Concept Formation Implementation: src/cognition/ConceptFormation.cpp
2. Prediction System Integration: src/prediction/PredictionSystem.cpp
3. Neural Memory Architecture: src/memory/Memory.hpp
4. Enhanced Python Bindings: python/bindings_enhanced.cpp
5. Brain Integration: src/brain/Brain.cpp (Phase 6 loop)

## 15. Changelog

### Version 0.1.0 (Phase 6 Integration)

#### New Features
- Neural semantic memory system
- Neural procedural memory system  
- Enhanced prediction system integration
- Advanced concept discovery
- Memory replay and consolidation
- Performance optimization framework

#### API Changes
- New classes: `SemanticMemory`, `ProceduralMemory`, `WorkingMemory`, `EpisodicMemory`
- Enhanced `Brain` class with memory and prediction integration
- New methods: `discover_concepts()`, `get_concept_discovery_results()`
- New statistics: `get_memory_stats()`, `get_prediction_stats()`

#### Backward Compatibility
- Original pynlm API preserved
- Enhanced bindings provide additional functionality
- Existing code continues to work without modification

### Future Enhancements
- Multi-agent learning coordination
- Real-world sensorimotor integration
- Advanced pattern recognition
- Memory compression and hierarchical organization
- Developmental trajectory modeling

## 16. Contact and Support

For questions about the enhanced Python bindings:

1. **Documentation**: https://nlm.readthedocs.io/en/latest/
2. **Source Code**: https://github.com/nlm-project/nlm
3. **Issue Tracker**: https://github.com/nlm-project/nlm/issues
4. **Community**: nlm-community@googlegroups.com

For technical support with specific implementation issues:
- Provide reproduction steps
- Include relevant configuration
- Include performance metrics
- Include error messages

## 17. Legal and License

**License**: MIT
**Copyright**: 2026 NLM Project Authors

The enhanced Python bindings are part of the NLM (熙然) project, an experimental artificial developmental brain research initiative.

**Disclaimer**: This software is experimental research code. It is not intended for commercial use and should not be deployed in safety-critical applications.

**Research Purpose**: NLM is designed to investigate computational brain-like systems and the emergence of cognitive abilities through neural development and learning.

---

*This documentation is continuously updated as the NLM project evolves. For the latest information, please check the official documentation repository.*

Generated: 2026-09-30
Version: Phase 6 Enhanced Integration
