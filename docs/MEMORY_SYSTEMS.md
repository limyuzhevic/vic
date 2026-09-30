# Memory Systems Documentation

## Overview

Memory systems in NLM implement the brain's ability to store, maintain, and recall information over different timescales. These systems work together to create a multi-level memory architecture that supports both short-term processing and long-term learning.

### Memory Hierarchy

1. **Working Memory (Seconds to minutes)**
2. **Episodic Memory (Minutes to days)**
3. **Associative Memory (Hours to years)**

## Working Memory (NeuralWorkingMemory)

### Purpose and Function

Working memory maintains transient neural activity patterns that represent current goals, tasks, or sensory information. It operates like the brain's "workspace" for active information processing.

### Mechanisms

#### Persistent Activity
- **Neural reverberation**: Recurrent connections maintain activity patterns
- **Stability-plasticity dilemma**: Balance between memory persistence and modification
- **Attractor dynamics**: Network states maintain information through positive feedback

#### Competition and Selection
- **Winner-take-all**: Mutual inhibition selects dominant representations
- **Allocation of attention**: Bias toward goal-relevant information
- **Noise management**: Stability against neural noise

### Implementation Details

#### Storage Model (Memory.hpp:18)
```cpp
class NeuralWorkingMemory {
public:
    // Store active neuron with activation strength
    void storeToNeuron(NeuronId neuronId, float activation) {
        // Update or create memory trace
        // Normalize if exceeding capacity
        // Maintain trace history
    }
    
    // Update all traces each timestep
    void update(TimestepDuration dt) {
        // Decay weak traces
        // Strengthen strong traces
        // Apply competition dynamics
    }
    
    // Get current memory content
    std::vector<NeuronId> getMemoryNeurons() const {
        // Return active memory traces
        // Apply competition bias
        return activeTraces;
    }
};
```

#### Integration with Brain (Brain.cpp:398-401)
```cpp
// Store to working memory - neurons that fire become part of working memory
if (pImpl->workingMemory) {
    pImpl->workingMemory->storeToNeuron(neuron->getId(), 
        std::abs(state.membranePotential - state.restingPotential) / 10.0f);
}
```

### Key Parameters

- **Capacity**: Number of memory traces (neuronCount/10 default)
- **Decay rate**: 0.95 per timestep (5% decay per step)
- **Strength limits**: Maximum activation per trace
- **Competition strength**: Mutual inhibition magnitude

### Performance Characteristics

#### Time Complexity
- **Storage**: O(1) per neuron activation
- **Update**: O(N) for N active traces
- **Selection**: O(N²) for full competition (optimized with attention)

#### Memory Usage
- **Trace storage**: O(number_of_active_traces)
- **History maintenance**: O(capacity × history_length)
- **Activation values**: Float per trace (4 bytes)

### Integration Patterns

#### With Neural Activity
- **Encoding**: Active neurons become memory traces
- **Maintenance**: Recurrent connections sustain activity
- **Updating**: New information replaces weak traces

#### With Attention
- **Selection**: Attention bias towards goal-relevant traces
- **Competition**: Mutual inhibition shapes trace dynamics
- **Priority**: Attention modulates trace strength

#### With Neuromodulation
- **Dopamine**: Modulates working memory strength
- **Novelty**: Influences trace formation
- **Attention**: Modulates trace competition

## Episodic Memory (NeuralEpisodicMemory)

### Purpose and Function

Episodic memory stores experiences as complete episodes, including temporal order, sensory content, and associated reward values. It supports replay and consolidation functions.

### Mechanisms

#### Episode Encoding
- **Experience capture**: Every 10 steps (configurable)
- **Temporal tagging**: Simulation step timestamps
- **Content recording**: Active neurons and their activations
- **Reward association**: Dopaminergic signals

#### Episode Storage
- **Database structure**: Indexed episodes for efficient retrieval
- **Compression**: Reduce redundancy while preserving information
- **Consolidation**: Move from labile to stable representation

#### Recall and Replay
- **Pattern completion**: Reactivate stored episode patterns
- **Spontaneous replay**: During rest or periodic consolidation
- **Cue-based retrieval**: Trigger by contextual similarity

### Implementation Details

#### Storage Structure (Memory.hpp:11, Memory.cpp:111, 111)
```cpp
class NeuralEpisodicMemory {
public:
    // Store complete episode
    void storeEpisode(const EpisodicMemoryItem& episode) {
        // Validate episode content
        // Apply compression if needed
        // Store with temporal index
        episodes.push_back(std::make_unique<EpisodicMemoryItem>(episode));
    }
    
    // Retrieve episodes for replay
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) {
        // Select most relevant episodes
        // Return pointers for efficient access
        return selectedEpisodes;
    }
    
    // Replay episode effects
    void replayEpisode(const EpisodicMemoryItem* episode) {
        // Re-inject neural activity patterns
        // Restore reward associations
        // Update plasticity based on replay
    }
};
```

#### Integration with Brain (Brain.cpp:486-516)
```cpp
// ========== STEP 7: Update episodic memory =========
pImpl->stepsSinceLastEpisode++;
if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
    pImpl->stepsSinceLastEpisode = 0;
    
    if (pImpl->episodicMemory) {
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        
        // Store active neurons
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                        episode.activeNeurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                    }
                }
            }
        }
        
        // Store reward in episode
        episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        
        pImpl->episodicMemory->storeEpisode(episode);
    }
}
```

### Episode Structure

#### EpisodicMemoryItem (Memory.hpp:8)
```cpp
struct EpisodicMemoryItem {
    SimulationStep timestamp;              // Time of episode
    std::vector<NeuronId> activeNeurons;    // Neurons that were active
    std::vector<float> neuronActivations;  // Their activation strengths
    float reward;                          // Associated reward value
    float predictionError;                 // Prediction error component
    std::vector<float> sensoryPatterns;    // Sensory input patterns
    std::vector<float> motorOutputs;       // Generated motor commands
    
    // Methods for episode processing
    float getActivationStrength() const {
        float total = 0.0f;
        for (float activation : neuronActivations) {
            total += activation;
        }
        return total / neuronActivations.size();
    }
    
    float getTemporalDistance(const EpisodicMemoryItem& other) const {
        return std::abs(timestamp - other.timestamp);
    }
};
```

### Memory Consolidation

#### Consolidation Process (Brain.cpp:585-589)
```cpp
// ========== STEP 15: Periodic memory consolidation =========
if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
    // Consolidate important memories, remove weak ones
    pImpl->episodicMemory->consolidate(0.3f);
}
```

#### Consolidation Mechanisms
1. **Pruning**: Remove weak or irrelevant episodes
2. **Compression**: Reduce memory footprint while preserving information
3. **Integration**: Extract and store abstract patterns
4. **Reconstruction**: Reconstruct episodes from compressed representations

### Replay Mechanisms

#### Replay During Rest (Brain.cpp:546-553)
```cpp
// ========== STEP 12: Replay important memories =========
if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
    // Get episodes for replay
    auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
    for (const auto* episode : episodesToReplay) {
        pImpl->episodicMemory->replayEpisode(episode);
    }
}
```

#### Replay Functions
1. **Pattern completion**: Reactivate stored episode patterns
2. **Synaptic strengthening**: Reinforce active pathways
3. **Memory integration**: Integrate replayed memories into current state
4. **Consolidation**: Move from labile to stable representation

## Associative Memory (NeuralAssociativeMemory)

### Purpose and Function

Associative memory stores relationships between patterns, enabling pattern completion and prediction. It implements the brain's ability to make associations based on experience.

### Mechanisms

#### Hebbian Associations
- **Pattern pairing**: Store relationships between co-occurring patterns
- **Spreading activation**: Retrieve associated patterns through partial cues
- **Strength modification**: Update associations based on experience

#### Pattern Completion
- **Cue-based retrieval**: Partial patterns trigger complete memories
- **Incremental completion**: Build up patterns from partial information
- **Similarity matching**: Find best-matching stored patterns

### Implementation Details

#### Storage and Retrieval (Memory.hpp:11)
```cpp
class NeuralAssociativeMemory {
public:
    // Store association between patterns
    void storeAssociation(const std::vector<float>& pattern1,
                         const std::vector<float>& pattern2,
                         float strength) {
        // Create bidirectional association
        // Apply strength limits
        // Update associative network
    }
    
    // Retrieve pattern based on cue
    std::vector<float> retrievePattern(const std::vector<float>& cue) {
        // Find best matching patterns
        // Combine retrieved patterns
        // Apply pattern completion
        return completedPattern;
    }
    
    // Update association based on experience
    void updateAssociation(const std::vector<float>& pattern1,
                          const std::vector<float>& pattern2,
                          float delta) {
        // Modify association strength
        // Apply learning rules
    }
};
```

#### Integration with Brain (Brain.cpp:246-250)
```cpp
// Initialize associative memory
if (pImpl->associativeMemory) {
    pImpl->associativeMemory->initialize(this);
}
```

### Memory System Integration

#### Data Flow Between Systems
```
Working Memory → Episodic Memory → Associative Memory → Prediction System
      ↑               ↑                ↑              ↑
  Sensory Input   Experience       Pattern         Cognitive
  (feedforward)   (encoding)       completion     planning
```

#### Information Processing Flow
1. **Sensory Processing**: Input → Working Memory
2. **Experience Encoding**: Working Memory → Episodic Memory
3. **Pattern Association**: Episodic Memory → Associative Memory
4. **Cognitive Operations**: Associative Memory → Prediction/Cognition
5. **Feedback Loops**: Cognitive → Working Memory (goal setting)

#### Timing and Synchronization

**Working Memory Update** (Brain.cpp:411-413)
```cpp
// Update working memory each timestep
if (pImpl->workingMemory) {
    pImpl->workingMemory->update(pImpl->timestep);
}
```

**Episodic Encoding Schedule**
- **Frequency**: Every 10 simulation steps (configurable)
- **Trigger**: StepsSinceLastEpisode counter
- **Content**: Active neural patterns and reward

**Associative Integration**
- **Timing**: During each step
- **Mechanism**: Update based on co-occurring patterns
- **Scope**: Network-wide associations

### Performance Considerations

#### Time Complexity
- **Working Memory**: O(N) for N active traces
- **Episodic Encoding**: O(N) per episode (N = active neurons)
- **Replay**: O(M) for M replayed episodes
- **Consolidation**: O(K log K) for K episodes (indexing)

#### Memory Usage
- **Working Memory**: O(active_traces)
- **Episodic Memory**: O(episodes × episode_size)
- **Associative Memory**: O(pattern_pairs)

#### Scalability
- **Small systems**: Direct memory structures
- **Medium systems**: Optimized indexing and caching
- **Large systems**: Distributed memory and parallel processing

### Future Extensions

#### Memory Mechanisms

1. **Memory types**: Add new memory systems (semantic, working, procedural)
2. **Multi-scale memory**: Hierarchical memory organization
3. **Sleep and memory**: Dreaming and memory consolidation during rest
4. **Memory compression**: Lossy compression for large-scale systems

#### Integration Advances

1. **Bidirectional flow**: Enhanced information flow between systems
2. **Cross-system binding**: Links between different memory types
3. **Temporal integration**: Time-based memory integration
4. **Resource allocation**: Dynamic resource allocation across memory systems

## References

1. **Memory systems**: Baddeley et al. "Working Memory and Language"
2. **Episodic memory**: Tulving et al. "Elements of Episodic Memory"
3. **Associative memory**: Squire et al. "The Neurology of Memory"
4. **Memory consolidation**: McGaugh et al. "Neurobiology of Memory Consolidation"
