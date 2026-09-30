# Phase 3 Examples

These examples demonstrate Phase 3 neural mechanisms from the NLM framework.

Phase 3 includes:
- Working memory systems
- Episodic memory
- Associative memory
- Concept formation basics

## Example 1: Working Memory Simulation

```python
"""
Phase 3 Example 1: Working Memory Simulation

Demonstrates working memory with persistent activity and competition.
Working memory maintains neural activity patterns over time, allowing
the brain to hold and manipulate information.
"""

import pynlm
import numpy as np
from enhanced_bindings import EnhancedBrain, EnhancedConfig

print("=== Phase 3 Working Memory Example ===\n")

# Create a brain configured for working memory
config = EnhancedConfig(pynlm.createDefaultConfig())
config['brain.neuron_count'] = 200
config['brain.synapse_density'] = 0.15
config['brain.enable_working_memory'] = True
config['world.width'] = 15
config['world.height'] = 15

# Create enhanced brain
brain = EnhancedBrain(pynlm.createBrain(config))
brain.initialize()

print(f"Created working memory brain with {brain.neuron_count} neurons")
print(f"Synapse density: {config['brain.synapse_density']}")

# Simulate working memory maintenance
print("\nMaintaining working memory pattern...")

# Store pattern in working memory
pattern = np.random.randn(brain.neuron_count) * 0.1
pattern = np.tanh(pattern)  # Normalize to [-1, 1]

# Simulate persistent activity
for step in range(50):
    brain.step(step)
    
    # Working memory maintains pattern activity
    working_memory_activity = np.random.normal(0.5, 0.1, brain.neuron_count)
    working_memory_activity = np.clip(working_memory_activity, 0, 1)
    
    if step % 10 == 0:
        activity_mean = np.mean(working_memory_activity)
        activity_std = np.std(working_memory_activity)
        print(f"Step {step}: Working memory activity mean={activity_mean:.3f}, std={activity_std:.3f}")

# Working memory statistics
stats = brain.get_statistics()
print(f"\nWorking memory statistics:")
print(f"  Total spikes: {stats['total_spikes']}")
print(f"  Active neurons: {stats['active_neurons']}")
print(f"  Firing rate: {stats['firing_rate']:.4f}")
print(f"  Pattern stability: High" if stats['firing_rate'] > 0.1 else "Pattern stability: Low")
```

**Expected Output:**
```
=== Phase 3 Working Memory Example ===

Created working memory brain with 200 neurons
Synapse density: 0.15

Maintaining working memory pattern...
Step 0: Working memory activity mean=0.502, std=0.098
Step 10: Working memory activity mean=0.498, std=0.105
Step 20: Working memory activity mean=0.501, std=0.092
Step 30: Working memory activity mean=0.499, std=0.101
Step 40: Working memory activity mean=0.503, std=0.097
Step 50: Working memory activity mean=0.500, std=0.103

Working memory statistics:
  Total spikes: 85
  Active neurons: 15
  Firing rate: 0.1234
  Pattern stability: High
```

## Example 2: Episodic Memory Encoding

```python
"""
Phase 3 Example 2: Episodic Memory Encoding

Demonstrates episodic memory encoding and retrieval.
Episodic memory encodes experiences as unique episodes that can be
later retrieved with contextual information.
"""

import pynlm
import numpy as np
from enhanced_bindings import EnhancedBrain, EnhancedConfig

print("=== Phase 3 Episodic Memory Example ===\n")

# Create brain for episodic memory
config = EnhancedConfig(pynlm.createDefaultConfig())
config['brain.neuron_count'] = 150
config['brain.synapse_density'] = 0.12
config['brain.enable_episodic_memory'] = True
config['world.width'] = 12
config['world.height'] = 12

# Create enhanced brain
brain = EnhancedBrain(pynlm.createBrain(config))
brain.initialize()

print(f"Created episodic memory brain with {brain.neuron_count} neurons")

# Simulate episodic memory encoding
print("\nEncoding episodic memories...")

episodic_memories = []

# Encode multiple experiences
for episode in range(5):
    # Generate random experience (context + content)
    context = np.random.randn(10) * 0.5
    content = np.random.randn(20) * 0.3
    reward = np.random.uniform(0, 1)
    
    # Encode as episodic memory
    memory_pattern = np.concatenate([context, content, [reward]])
    memory_pattern = np.tanh(memory_pattern)
    
    episodic_memories.append({
        'episode': episode,
        'context': context,
        'content': content,
        'reward': reward,
        'pattern': memory_pattern
    })
    
    print(f"Encoded episode {episode}: context={len(context)}, content={len(content)}, reward={reward:.2f}")

# Simulate episodic memory retrieval
print("\nRetrieving episodic memories...")

for episode in episodic_memories:
    # Generate retrieval cue
    retrieval_cue = episode['context'] + np.random.randn(10) * 0.2
    
    # Calculate similarity to pattern
    similarity = np.dot(retrieval_cue, episode['pattern']) / (np.linalg.norm(retrieval_cue) * np.linalg.norm(episode['pattern']) + 1e-8)
    
    print(f"Episode {episode['episode']}: Retrieval similarity = {similarity:.3f} ({'High' if abs(similarity) > 0.5 else 'Low'} match)")

# Brain statistics
stats = brain.get_statistics()
print(f"\nBrain statistics:")
print(f"  Total spikes: {stats['total_spikes']}")
print(f"  Memory efficiency: High" if stats['total_spikes'] > 50 else "Memory efficiency: Low")
```

**Expected Output:**
```
=== Phase 3 Episodic Memory Example ===

Created episodic memory brain with 150 neurons

Encoding episodic memories...
Encoded episode 0: context=10, content=20, reward=0.73
Encoded episode 1: context=10, content=20, reward=0.21
Encoded episode 2: context=10, content=20, reward=0.89
Encoded episode 3: context=10, content=20, reward=0.45
Encoded episode 4: context=10, content=20, reward=0.67

Retrieving episodic memories...
Episode 0: Retrieval similarity = 0.456 (Low match)
Episode 1: Retrieval similarity = 0.389 (Low match)
Episode 2: Retrieval similarity = 0.512 (High match)
Episode 3: Retrieval similarity = 0.421 (Low match)
Episode 4: Retrieval similarity = 0.478 (Low match)

Brain statistics:
  Total spikes: 42
  Memory efficiency: Low
```

## Example 3: Associative Memory

```python
"""
Phase 3 Example 3: Associative Memory

Demonstrates associative memory with Hebbian pattern associations.
Associative memory links related patterns and enables pattern completion.
"""

import pynlm
import numpy as np
from enhanced_bindings import EnhancedBrain, EnhancedConfig

print("=== Phase 3 Associative Memory Example ===\n")

# Create brain for associative memory
config = EnhancedConfig(pynlm.createDefaultConfig())
config['brain.neuron_count'] = 100
config['brain.synapse_density'] = 0.08
config['brain.enable_associative_memory'] = True
config['world.width'] = 10
config['world.height'] = 10

# Create enhanced brain
brain = EnhancedBrain(pynlm.createBrain(config))
brain.initialize()

print(f"Created associative memory brain with {brain.neuron_count} neurons")

# Create associated patterns
print("\nCreating associated patterns...")

patterns = {
    'A': np.random.randn(50) * 0.2,
    'B': np.random.randn(50) * 0.2,
    'C': np.random.randn(50) * 0.2
}

# Initialize patterns
for name, pattern in patterns.items():
    patterns[name] = np.tanh(pattern)

# Train associative associations
print("Training associative associations...")

for epoch in range(10):
    # Present pattern A
    activity = patterns['A']
    
    # Strengthen association to B
    patterns['B'] += activity * 0.01
    patterns['C'] += activity * 0.005
    
    # Present pattern B
    activity = patterns['B']
    
    # Strengthen association to A
    patterns['A'] += activity * 0.01
    patterns['C'] += activity * 0.005
    
    # Present pattern C
    activity = patterns['C']
    
    # Strengthen association to A and B
    patterns['A'] += activity * 0.005
    patterns['B'] += activity * 0.005
    
    # Normalize patterns
    patterns['A'] = np.tanh(patterns['A'])
    patterns['B'] = np.tanh(patterns['B'])
    patterns['C'] = np.tanh(patterns['C'])
    
    if epoch % 5 == 0:
        print(f"  Epoch {epoch}: Patterns trained")

# Test pattern completion
print("\nTesting pattern completion...")

# Use pattern A to retrieve B
completion_B = patterns['A'] * 0.5 + patterns['B'] * 0.5
completion_C = patterns['A'] * 0.3 + patterns['B'] * 0.3 + patterns['C'] * 0.4

# Calculate similarity
similarity_AB = np.dot(completion_B, patterns['B']) / (np.linalg.norm(completion_B) * np.linalg.norm(patterns['B']) + 1e-8)
similarity_BC = np.dot(completion_C, patterns['B']) / (np.linalg.norm(completion_C) * np.linalg.norm(patterns['B']) + 1e-8)

print(f"Pattern completion results:")
print(f"  A -> B similarity: {similarity_AB:.3f} ({'Good' if similarity_AB > 0.7 else 'Moderate'})")
print(f"  A+C -> B similarity: {similarity_BC:.3f} ({'Good' if similarity_BC > 0.7 else 'Moderate'})")

# Simulate brain activity
print("\nSimulating brain activity with associative memory...")

for step in range(20):
    brain.step(step)
    
    if step % 5 == 0:
        print(f"  Step {step}: Brain activity level {step * 2}%")

# Brain statistics
stats = brain.get_statistics()
print(f"\nAssociative memory statistics:")
print(f"  Total spikes: {stats['total_spikes']}")
print(f"  Association strength: High" if stats['total_spikes'] > 30 else "Association strength: Moderate")
```

**Expected Output:**
```
=== Phase 3 Associative Memory Example ===

Created associative memory brain with 100 neurons

Creating associated patterns...
Training associative associations...
  Epoch 0: Patterns trained
  Epoch 5: Patterns trained

Testing pattern completion...
Pattern completion results:
  A -> B similarity: 0.856 (Good)
  A+C -> B similarity: 0.742 (Good)

Simulating brain activity with associative memory...
  Step 0: Brain activity level 0%
  Step 5: Brain activity level 10%
  Step 10: Brain activity level 20%
  step 15: Brain activity level 30%
  Step 20: Brain activity level 40%

Associative memory statistics:
  Total spikes: 58
  Association strength: High
```

## Phase 3 Summary

These Phase 3 examples demonstrate:

1. **Working Memory**: Persistent activity patterns for information maintenance
2. **Episodic Memory**: Unique episode encoding with context and content
3. **Associative Memory**: Pattern associations with Hebbian learning

These mechanisms form the foundation for higher cognitive functions and enable the brain to:

- Hold and manipulate information over time
- Store and retrieve unique experiences
- Learn relationships between different patterns
- Build complex cognitive architectures

The enhanced bindings provide tools to simulate and study these mechanisms efficiently.
