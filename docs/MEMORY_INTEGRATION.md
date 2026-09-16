# Memory Integration Documentation

## Overview

Memory integration is the foundation of Phase 6's artificial brain. The memory systems work together to store, maintain, and retrieve information, enabling learning and adaptive behavior.

### Key Components

#### Working Memory (NeuralWorkingMemory)
- **Purpose**: Maintains current neural activity patterns
- **Mechanism**: Persistent activity with recurrent connections
- **Integration**: Updated each simulation step, influences attention competition

#### Episodic Memory (NeuralEpisodicMemory)
- **Purpose**: Stores experience episodes as neural patterns
- **Mechanism**: Temporal ordering with relevance-weighted recall
- **Integration**: Captured every 10 steps, replay every 100 steps, consolidation every 1000 steps

#### Associative Memory (NeuralAssociativeMemory)
- **Purpose**: Forms pattern associations through Hebbian learning
- **Mechanism**: Spreading activation for recall
- **Integration**: Connected to episodic memory for experience encoding

## Integration Details

### Memory Flow
```
Sensory Input → Working Memory → Episodic Memory → Replay → Behavior
    ↓              ↓              ↓            ↓            ↓
Neural Activity   Patterns      Episodes    Learning    Adaptation
```

### Storage and Retrieval
- **Storage**: Every 10 simulation steps (Episodic Memory)
- **Maintenance**: Continuous in Working Memory
- **Replay**: Every 100 steps for consolidation
- **Consolidation**: Every 1000 steps for long-term storage

## API Reference

```cpp
// Working Memory
brain->getWorkingMemory()->storeToNeuron(neuronId, activation);  // Store pattern
brain->getWorkingMemory()->update(timestep);                     // Update each step
brain->getWorkingMemory()->retrieve();                           // Get patterns
brain->getWorkingMemory()->getActiveTraces();                    // Get trace count

// Episodic Memory
EpisodicMemoryItem episode;
episode.timestamp = currentStep;
// ... fill with active neurons
brain->getEpisodicMemory()->storeEpisode(episode);               // Store experience

auto episodes = brain->getEpisodicMemory()->getEpisodesForReplay(3);  // Get episodes
int count = brain->getEpisodicMemory()->getEpisodeCount();             // Get count

// Integration
brain->getWorkingMemory()  // Returns working memory system
brain->getEpisodicMemory() // Returns episodic memory system
```

## Integration Verification

```cpp
bool testMemoryIntegration() {
    if (!brain->getWorkingMemory()) {
        NLM_LOG_ERROR("Working memory not integrated");
        return false;
    }
    
    if (!brain->getEpisodicMemory()) {
        NLM_LOG_ERROR("Episodic memory not integrated");
        return false;
    }
    
    // Test actual functionality
    auto* wm = brain->getWorkingMemory();
    auto* em = brain->getEpisodicMemory();
    
    for (int i = 0; i < 100; ++i) {
        brain->step(i, i * 0.001);
    }
    
    if (wm->getActiveTraces() > 0) {
        NLM_LOG_INFO("Working memory active traces: " + std::to_string(wm->getActiveTraces()));
    }
    
    if (em->getEpisodeCount() > 0) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(em->getEpisodeCount()));
    }
    
    return true;
}
```

## Python API

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check memory integration
if brain.getWorkingMemory():
    print("Working memory: INTEGRATED")
    print(f"Active traces: {brain.getWorkingMemory().getActiveTraces()}")

if brain.getEpisodicMemory():
    print("Episodic memory: INTEGRATED")
    print(f"Episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
```

## Configuration

```cpp
// Memory system configuration
config->set("brain.memory.working.capacity", 100);              // Working memory capacity
config->set("brain.memory.episodic.max_episodes", 1000);       // Max episodes
config->set("brain.memory.episodic.capture_interval", 10);    // Capture every 10 steps
config->set("brain.memory.episodic.replay_interval", 100);    // Replay every 100 steps
config->set("brain.memory.episodic.consolidation_interval", 1000); // Consolidate every 1000 steps
```

## Usage Examples

### Basic Memory Usage
```cpp
// Simulate and store working memory
for (uint64_t step = 0; step < 1000; ++step) {
    brain->step(step, step * 0.001);
    
    if (step % 10 == 0 && brain->getWorkingMemory()) {
        // Store current state in working memory
        brain->getWorkingMemory()->update(0.001f);
    }
}
```

### Memory Integration with Behavior
```cpp
// Example: Learning from experience through memory
for (uint64_t step = 0; step < 2000; ++step) {
    // Process sensory input
    agent->processSensoryInput(world->observe());
    
    // Run brain loop (integrates memory systems)
    brain->step(step, step * 0.001);
    
    // Check if working memory has patterns to influence behavior
    if (brain->getWorkingMemory() && brain->getWorkingMemory()->getActiveTraces() > 0) {
        // Memory patterns influence action selection
        auto patterns = brain->getWorkingMemory()->retrieve();
        // ... use patterns for decision making
    }
    
    // Episodic memory stored every 10 steps
    if (step % 10 == 0 && brain->getEpisodicMemory()) {
        // Memory episode captured
    }
}
```

## Summary

Memory integration in Phase 6 provides:
- **Real-time storage** of neural activity through Working Memory
- **Experience recording** through Episodic Memory
- **Replay mechanisms** for learning consolidation
- **Pattern associations** through Associative Memory

This creates a continuous memory loop that enables the brain to learn from experience and adapt its behavior over time.
