# Memory Systems Module - NLM Memory Architecture

**NLM Memory Systems Module Documentation**

## Overview

The Memory Systems Module implements the complete memory architecture of the NLM brain, providing working memory for active information maintenance, episodic memory for experience storage and replay, and associative memory for pattern associations and semantic learning.

### Key Features

- **Working Memory**: Transient active information management with competitive dynamics
- **Episodic Memory**: Experience encoding and replay for consolidation and rehearsal
- **Associative Memory**: Pattern associations through Hebbian learning and spread activation
- **Memory Integration**: Seamless integration with neural processing and plasticity
- **Memory Persistence**: Checkpoint-based memory saving and loading
- **Memory Consolidation**: Sleep/rest cycle integration for memory strengthening

## Core Classes

### NeuralWorkingMemory
Transient active information management with persistent activity and competition

### NeuralEpisodicMemory
Experience encoding and replay with temporal context for memory consolidation

### NeuralAssociativeMemory
Pattern associations through Hebbian learning and concept formation

## Memory Architecture

### Memory Systems Integration
The NLM memory architecture follows a multi-level organization:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING
  ↓
MEMORY SYSTEMS
  ├─ Working Memory (Active Processing)
  │  ├─ Short-term storage (seconds to minutes)
  │  ├─ Competitive selection dynamics
  │  └─ Persistent activity maintenance
  │
  ├─ Episodic Memory (Experience Storage)
  │  ├─ Long-term storage (hours to days)
  │  ├─ Temporal context encoding
  │  └─ Replay and consolidation
  │
  └─ Associative Memory (Pattern Learning)
     ├─ Semantic memory (concepts)
     ├─ Procedural memory (skills)
     └─ Working memory (active patterns)
```

### Memory Processing Pipeline

1. **Encoding**: New experiences stored in episodic memory
2. **Consolidation**: Episodic memories transferred to associative memory
3. **Rehearsal**: Active maintenance in working memory
4. **Retrieval**: Pattern completion in associative memory
5. **Update**: Memories modified through plasticity

## Core Classes

### NeuralWorkingMemory
Transient active information management for ongoing tasks

**Purpose**: Maintains active neural representations for current tasks, supporting:
- **Working Memory Capacity**: Limited number of active items
- **Interference Management**: Competitive selection between items
- **Persistent Activity**: Sustained neural firing without continuous input
- **Decay Dynamics**: Gradual loss of inactive items
- **Refresh Mechanisms**: Active maintenance of important items

**Key Features**:
- **Capacity Limits**: Configurable number of active items
- **Competitive Dynamics**: Winner-take-all selection
- **Refractoriness**: Protection against excessive firing
- **Synaptic Integration**: Integration with hippocampal inputs
- **Replay Mechanisms**: Internal rehearsal of active contents

**Usage Examples**:
```cpp
// Configure working memory
brain->getWorkingMemory()->setCapacity(1000);
brain->getWorkingMemory()->setDecayRate(0.01f);
brain->getWorkingMemory()->enableRefresh(true);

// Record active items
brain->getWorkingMemory()->recordActivity(patternA);
brain->getWorkingMemory()->recordActivity(patternB);

// Get statistics
float activityLevel = brain->getWorkingMemory()->getActivityLevel();
int activeCount = brain->getWorkingMemory()->getActiveCount();
```

### NeuralEpisodicMemory
Experience encoding and replay for memory consolidation

**Purpose**: Stores discrete experiences with temporal context for replay and rehearsal, supporting:
- **Experience Encoding**: Sparse representation of meaningful events
- **Temporal Context**: Chronological organization of memories
- **Replay Mechanisms**: Internal replay during rest/sleep
- **Consolidation**: Gradual strengthening of memory traces
- **Pattern Completion**: Reconstruction of partial memories

**Key Features**:
- **Sparse Coding**: Only significant events stored
- **Temporal Encoding**: Time-based memory indexing
- **Replay Dynamics**: Internal reactivation for consolidation
- **Activity-Dependent Consolidation**: Strong memories strengthened
- **Chunking**: Grouping of related experiences

**Usage Examples**:
```cpp
// Configure episodic memory
brain->getEpisodicMemory()->setEncodingRate(0.5f);
brain->getEpisodicMemory()->enableReplay(true);
brain->getEpisodicMemory()->setConsolidationRate(0.1f);

// Encode experiences
EpisodicMemoryItem episode1 = {"sensoryInputA", "motorOutputA", 0.8f, timestamp1};
brain->getEpisodicMemory()->encode(episode1);

// Replay memories
brain->getEpisodicMemory()->replayDuringRest();

// Get statistics
float encodingRate = brain->getEpisodicMemory()->getEncodingRate();
int episodeCount = brain->getEpisodicMemory()->getEpisodeCount();
```

### NeuralAssociativeMemory
Pattern associations through Hebbian learning and spread activation

**Purpose**: Forms and retrieves pattern associations through unsupervised learning, supporting:
- **Hebbian Learning**: Correlation-based weight strengthening
- **Spread Activation**: Pattern completion through associative connections
- **Concept Formation**: Abstract pattern discovery
- **Semantic Memory**: Relational knowledge representation
- **Procedural Learning**: Sequence and skill acquisition

**Key Features**:
- **Associative Connections**: Weighted connections between patterns
- **Spread Activation**: Parallel activation of related patterns
- **Pattern Completion**: Reconstruction of incomplete patterns
- **Structural Plasticity**: Formation of new connections
- **Synaptic Tagging**: Eligibility traces for learning

**Usage Examples**:
```cpp
// Configure associative memory
brain->getAssociativeMemory()->setLearningRate(0.01f);
brain->getAssociativeMemory()->enableSpreadActivation(true);
brain->getAssociativeMemory()->setPatternCompletenessThreshold(0.7f);

// Form associations
std::vector<float> patternA = {0.1f, 0.5f, 0.3f};
std::vector<float> patternB = {0.2f, 0.6f, 0.4f};
brain->getAssociativeMemory()->associate(patternA, patternB);

// Retrieve patterns
std::vector<float> retrieved = brain->getAssociativeMemory()->retrieve(patternA);

// Get association strength
float strength = brain->getAssociativeMemory()->getAssociationStrength(patternA, patternB);
```

## Memory Integration

### Neural Working Memory Integration
```cpp
void integrateWorkingMemory(Brain* brain) {
    auto* wm = brain->getWorkingMemory();
    if (!wm) return;
    
    // Configure working memory based on neural population activity
    size_t totalActive = brain->getActiveNeuronCount();
    wm->setCapacity(std::max(100, totalActive / 10));
    wm->setDecayRate(0.01f * brain->getExcitationInhibitionRatio());
    
    // Record current neural state
    std::vector<float> currentPattern = extractPattern(brain);
    wm->recordActivity(currentPattern);
    
    // Competition between patterns
    wm->resolveCompetition();
}
```

### Episodic Memory Integration
```cpp
void integrateEpisodicMemory(Brain* brain) {
    auto* em = brain->getEpisodicMemory();
    if (!em) return;
    
    // Encode experiences during learning
    if (brain->getPredictionErrorSignal() > threshold) {
        EpisodicMemoryItem episode = {
            .sensoryInput = getCurrentSensoryInput(),
            .motorOutput = getLastAction(),
            .reward = getLastReward(),
            .timestamp = getCurrentTime()
        };
        em->encode(episode);
    }
    
    // Replay during idle periods
    if (isRestPeriod()) {
        em->replayDuringRest();
    }
}
```

### Associative Memory Integration
```cpp
void integrateAssociativeMemory(Brain* brain) {
    auto* am = brain->getAssociativeMemory();
    if (!am) return;
    
    // Update associations based on neural activity
    std::vector<float> currentPattern = extractPattern(brain);
    
    // Form new associations through Hebbian learning
    if (shouldFormNewAssociation(brain)) {
        am->updateAssociation(currentPattern, getTargetPattern());
    }
    
    // Activate related patterns
    am->spreadActivation(currentPattern);
    
    // Retrieve related memories
    std::vector<float> related = am->retrieveRelated(currentPattern);
    
    // Integrate with working memory
    brain->getWorkingMemory()->integrate(related);
}
```

## Memory Management

### Memory Lifecycle

```
Experience Encoding
    ↓
Episodic Memory Storage
    ↓
Consolidation to Associative Memory
    ↓
Active Maintenance in Working Memory
    ↓
Pattern Completion and Retrieval
    ↓
Plasticity Updates
    ↓
Memory Strengthening/Weakening
```

### Memory Allocation and Deallocation
```cpp
class MemoryManager {
public:
    // Allocate memory pools for different system components
    void initializeMemoryPools(Brain* brain) {
        // Working memory pool
        workingMemoryPool = new MemoryPool<NeuralWorkingMemory>(
            brain->getWorkingMemoryCapacity()
        );
        
        // Episodic memory pool
        episodicMemoryPool = new MemoryPool<NeuralEpisodicMemory>(
            brain->getEpisodicMemoryCapacity()
        );
        
        // Associative memory pool
        associativeMemoryPool = new MemoryPool<NeuralAssociativeMemory>(
            brain->getAssociativeMemoryCapacity()
        );
    }
    
    // Deallocate memory pools
    void cleanupMemoryPools() {
        delete workingMemoryPool;
        delete episodicMemoryPool;
        delete associativeMemoryPool;
    }
    
    // Get memory statistics
    MemoryStats getMemoryStats() const {
        MemoryStats stats;
        stats.workingMemory = workingMemoryPool->getUsage();
        stats.episodicMemory = episodicMemoryPool->getUsage();
        stats.associativeMemory = associativeMemoryPool->getUsage();
        stats.total = stats.workingMemory + stats.episodicMemory + stats.associativeMemory;
        return stats;
    }
};
```

### Memory Persistence
```cpp
void saveMemoryState(Brain* brain, const std::string& filepath) {
    // Save working memory
    auto* wm = brain->getWorkingMemory();
    if (wm) {
        wm->saveToFile(filepath + ".working");
    }
    
    // Save episodic memory
    auto* em = brain->getEpisodicMemory();
    if (em) {
        em->saveToFile(filepath + ".episodic");
    }
    
    // Save associative memory
    auto* am = brain->getAssociativeMemory();
    if (am) {
        am->saveToFile(filepath + ".associative");
    }
    
    // Save memory metadata
    saveMemoryMetadata(brain, filepath + ".meta");
}

void loadMemoryState(Brain* brain, const std::string& filepath) {
    // Load working memory
    auto* wm = brain->getWorkingMemory();
    if (wm && fileExists(filepath + ".working")) {
        wm->loadFromFile(filepath + ".working");
    }
    
    // Load episodic memory
    auto* em = brain->getEpisodicMemory();
    if (em && fileExists(filepath + ".episodic")) {
        em->loadFromFile(filepath + ".episodic");
    }
    
    // Load associative memory
    auto* am = brain->getAssociativeMemory();
    if (am && fileExists(filepath + ".associative")) {
        am->loadFromFile(filepath + ".associative");
    }
    
    // Load memory metadata
    loadMemoryMetadata(brain, filepath + ".meta");
}
```

## Memory Performance

### Time Complexity
- **Working Memory Access**: O(1) for active item access
- **Episodic Memory Encoding**: O(log n) for temporal indexing
- **Episodic Memory Retrieval**: O(log n) for temporal queries
- **Associative Memory Lookup**: O(1) for direct access, O(k) for spread activation
- **Memory Consolidation**: O(1) per consolidation event

### Memory Usage
- **Working Memory**: ~1-10% of total neural memory
- **Episodic Memory**: Scales with experience duration and significance
- **Associative Memory**: Grows with learned patterns and associations
- **Memory Overhead**: ~5-15% additional memory for management structures

### Optimization Features
- **Sparse Coding**: Only significant memories stored
- **Memory Pools**: Efficient allocation and deallocation
- **Compression**: Differential encoding for episodic memories
- **Hierarchical Storage**: Multiple storage levels for different memory types
- **Cache Management**: LRU caching for frequently accessed items

## Memory Behavior

### Working Memory Dynamics
```cpp
class WorkingMemoryBehavior {
public:
    // Competitive dynamics
    void resolveCompetition() {
        // Normalize activity levels
        normalizeActivity();
        
        // Apply winner-take-all
        selectWinner();
        
        // Apply refractory periods
        applyRefractoriness();
    }
    
    // Activity persistence
    void maintainPersistence() {
        // Apply decay
        decayActivity();
        
        // Apply noise
        addNoise();
        
        // Apply external inputs
        integrateExternalInputs();
    }
};
```

### Episodic Memory Dynamics
```cpp
class EpisodicMemoryBehavior {
public:
    // Encoding dynamics
    void encodeExperience(const EpisodicMemoryItem& item) {
        // Sparse coding
        sparseEncode(item);
        
        // Temporal context
        addTemporalContext(item);
        
        // Pattern detection
        detectPattern(item);
        
        // Consolidation eligibility
        markForConsolidation(item);
    }
    
    // Replay dynamics
    void replayDuringRest() {
        // Select memories to replay
        selectMemoriesForReplay();
        
        // Reactivate patterns
        reactivatePatterns();
        
        // Update consolidation
        updateConsolidation();
    }
};
```

### Associative Memory Dynamics
```cpp
class AssociativeMemoryBehavior {
public:
    // Hebbian learning dynamics
    void hebbianLearning(const std::vector<float>& patternA, 
                        const std::vector<float>& patternB) {
        // Compute correlation
        float correlation = computeCorrelation(patternA, patternB);
        
        // Update weights
        updateWeights(patternA, patternB, correlation);
        
        // Apply decay
        decayWeights();
        
        // Update eligibility traces
        updateEligibilityTraces(patternA, patternB);
    }
    
    // Spread activation dynamics
    void spreadActivation(const std::vector<float>& pattern) {
        // Activate related patterns
        activateRelatedPatterns(pattern);
        
        // Apply competition
        applyCompetition();
        
        // Normalize activation
        normalizeActivation();
    }
};
```

## Testing and Validation

### Memory System Testing
```bash
# Test working memory
./nlm_memory_test --type=working --capacity=1000 --duration=3600

# Test episodic memory
./nlm_memory_test --type=episodic --encoding=0.5 --replay=true

# Test associative memory
./nlm_memory_test --type=associative --learning=0.01 --spread=true

# Test memory integration
./nlm_memory_test --integration=true --benchmark=true
```

### Memory Performance Testing
```cpp
void benchmarkMemoryPerformance(Brain* brain) {
    auto* wm = brain->getWorkingMemory();
    auto* em = brain->getEpisodicMemory();
    auto* am = brain->getAssociativeMemory();
    
    // Benchmark working memory
    benchmarkWorkingMemory(wm, 10000);
    
    // Benchmark episodic memory
    benchmarkEpisodicMemory(em, 1000);
    
    // Benchmark associative memory
    benchmarkAssociativeMemory(am, 5000);
    
    // Benchmark memory integration
    benchmarkMemoryIntegration(brain, 1000);
}
```

## Troubleshooting

### Common Memory Issues and Solutions

**Issue**: Working memory capacity exceeded
**Solution**: Increase capacity or implement more efficient encoding

**Issue**: Episodic memory encoding failed
**Solution**: Check encoding rate and temporal context

**Issue**: Associative memory pattern completion failed
**Solution**: Verify association strength and spread activation

**Issue**: Memory persistence corrupted
**Solution**: Check file permissions and backup strategy

**Issue**: Memory performance degraded
**Solution**: Profile memory usage and optimize algorithms

## Configuration Options

### Working Memory Configuration
- `memory.working.capacity`: Working memory capacity (default: 1000)
- `memory.working.decayRate`: Decay rate (default: 0.01)
- `memory.working.competition`: Enable competition (default: true)
- `memory.working.refresh`: Enable refresh (default: true)

### Episodic Memory Configuration
- `memory.episodic.encodingRate`: Encoding rate (default: 0.5)
- `memory.episodic.maxDuration`: Maximum duration (default: 3600)
- `memory.episodic.consolidationRate`: Consolidation rate (default: 0.1)
- `memory.episodic.replay`: Enable replay (default: true)

### Associative Memory Configuration
- `memory.associative.learningRate`: Learning rate (default: 0.01)
- `memory.associative.spreadActivation`: Enable spread activation (default: true)
- `memory.associative.patternCompletionThreshold`: Completion threshold (default: 0.7)
- `memory.associative.structureLearning`: Enable structural learning (default: true)

## Future Enhancements

### Planned Memory Features
1. **Dynamic Capacity**: Adaptive memory capacity based on task demands
2. **Memory Prioritization**: Priority-based memory allocation
3. **Memory Compression**: Advanced compression for episodic memories
4. **Memory Prediction**: Predictive memory management
5. **Memory Transplantation**: Cross-system memory transfer

## License

MIT

## References

1. **Brain Module** (`src/brain/README.md`)
2. **Config Module** (`src/core/Config/README.md`)
3. **Plasticity Module** (`src/plasticity/README.md`)
4. **Neuromodulation Module** (`src/neuromodulation/README.md`)

---

*This documentation is continuously updated as the memory systems evolve. For the latest information, refer to inline source code documentation and recent commits.*

**Version**: Phase 6 Memory Systems
**Last Updated**: Current
**Status**: Production Ready