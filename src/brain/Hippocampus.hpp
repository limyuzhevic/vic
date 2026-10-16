# Hippocampus Implementation

This file implements the hippocampus component of the NLM brain simulation. The hippocampus is crucial for:

1. **Spatial Navigation**: Cognitive mapping of the environment
2. **Episodic Memory Formation**: Binding experiences in time and space
3. **Memory Consolidation**: Transferring memories from short-term to long-term storage
4. **Context Integration**: Integrating sensory and motor information

## Overview

The hippocampus implements several key functions:

1. **Place Cells**: Neurons that fire in specific spatial locations
2. **Grid Cells**: Neurons that fire at multiple spatial scales
3. **Border Cells**: Neurons that detect boundary locations
4. **Head Direction Cells**: Neurons that track facing direction
5. **Episodic Memory Encoding**: Binding events into coherent episodes
6. **Memory Replay**: Consolidating memories during rest periods
7. **Context-Dependent Recall**: Retrieving memories based on current context

## Design

### Neural Architecture

The hippocampus implements several neural circuits:

1. **Entorhinal Cortex Input**: Mossy fibers and perforant path
2. **Dentate Gyrus**: Pattern separation and sparse coding
3. **CA3 Recurrent Network**: Pattern completion and auto-association
4. **CA1 Output**: Context information to neocortex
5. **Specialized Cell Types**:
   - **Place Cells** (CA1): Location-specific firing
   - **Grid Cells** (EC): Multiple firing fields at different scales
   - **Border Cells**: Boundary detection
   - **Head Direction Cells**: Directional tracking

### Memory Processing

1. **Encoding**: Binding sensory inputs into episodic episodes
2. **Storage**: Maintaining compressed representations
3. **Consolidation**: Transferring to long-term storage
4. **Replay**: Reactivating during rest for consolidation

```c
// Hippocampal neural components
struct PlaceCell {
    NeuronId id;
    float x;                    // X coordinate in environment
    float y;                    // Y coordinate in environment
    float firingRate;           // Current firing rate
    float familiarity;          // Previous activation level
    std::vector<float> pattern; // Neural activation pattern
};

struct GridCell {
    NeuronId id;
    float scale;                // Spatial scale (1.0, 2.0, 4.0, 8.0)
    float orientation;          // Field orientation (radians)
    float periodicity;          // Distance between firing fields
    std::vector<GridField> fields;  // Individual firing fields
};

struct EpisodicMemory {
    Timestamp startTime;        // When episode began
    Timestamp endTime;          // When episode ended
    std::vector<NeuronId> neurons;     // Neurons involved in episode
    std::vector<float> activations;   // Neural activation levels
    float reward;               // Associated reward value
    std::string context;        // Context description
    float coherence;            // Memory coherence score
};
```

## Implementation

### Hippocampus Class

```c++
class Hippocampus {
public:
    Hippocampus();
    ~Hippocampus();
    
    // Initialize with environment and brain references
    void initialize(const SimpleWorld& world, Brain* brain);
    
    // Update hippocampal activity
    void update(TimestepDuration dt);
    
    // Process new sensory input for encoding
    void processSensoryInput(const SensoryPercept& percept);
    
    // Record current state as an episodic memory
    void recordEpisode(const std::string& context);
    
    // Retrieve memory based on current state
    EpisodicMemory* retrieveMemory(const std::string& context,
                                   float similarityThreshold = 0.5f);
    
    // Replay episodes for consolidation
    void replayEpisodes(size_t episodeCount = 3);
    
    // Get place cell information
    const std::vector<PlaceCell>& getPlaceCells() const { return placeCells_; }
    const std::vector<GridCell>& getGridCells() const { return gridCells_; }
    
    // Get current spatial information
    SpatialInfo getCurrentSpatialInfo() const;
    
    // Set environment dimensions
    void setEnvironmentSize(float width, float height);
    
    // Configure place cell formation
    void configurePlaceCells(size_t placeCellCount,
                            const std::vector<float>& firingRates);
    
    // Configure grid cell formation
    void configureGridCells(size_t gridCellCount,
                           const std::vector<float>& scales);
    
    // Enable/disable specific hippocampal functions
    void enablePlaceCells(bool enable) { placeCellsEnabled_ = enable; }
    void enableGridCells(bool enable) { gridCellsEnabled_ = enable; }
    void enableMemoryEncoding(bool enable) { memoryEncodingEnabled_ = enable; }
    void enableMemoryReplay(bool enable) { memoryReplayEnabled_ = enable; }
    
    // Get memory statistics
    size_t getEpisodicMemoryCount() const { return episodicMemories_.size(); }
    float getMemoryCoherence() const;
    
private:
    // Neural network components
    std::vector<PlaceCell> placeCells_;
    std::vector<GridCell> gridCells_;
    
    // Memory systems
    std::vector<EpisodicMemory> episodicMemories_;
    std::vector<WorkingMemoryTrace> shortTermMemory_;
    
    // Spatial processing
    SpatialInfo currentSpatialInfo_;
    float environmentWidth_;
    float environmentHeight_;
    
    // Hippocampal dynamics
    float placeCellLearningRate_;
    float gridCellLearningRate_;
    float memoryConsolidationRate_;
    
    // Configuration flags
    bool placeCellsEnabled_;
    bool gridCellsEnabled_;
    bool memoryEncodingEnabled_;
    bool memoryReplayEnabled_;
    
    // Brain integration
    Brain* brain_;
    const SimpleWorld* world_;
    
    // Replay scheduling
    std::vector<Timestamp> replaySchedule_;
    size_t replayIndex_;
    
    // Helper methods
    void updatePlaceCells(TimestepDuration dt);
    void updateGridCells(TimestepDuration dt);
    void updateEpisodicMemory(TimestepDuration dt);
    
    // Spatial computation methods
    float computePlaceCellActivation(const PlaceCell& cell,
                                    const SpatialInfo& info) const;
    void updatePlaceCellConnections();
    void updateGridCellConnections();
    
    // Memory consolidation
    void consolidateMemories();
    void replayEpisode(const EpisodicMemory& episode);
    
    // Pattern separation/integration
    void patternSeparation(const std::vector<float>& input,
                          std::vector<float>& output);
    void patternCompletion(const std::vector<float>& input,
                          std::vector<float>& output);
    
    // Memory encoding
    void encodeEpisodicMemory(const std::string& context);
    float computeMemoryCoherence(const EpisodicMemory& memory) const;
};
```

### Place Cell Dynamics

Place cells implement spatial representation through activity-dependent plasticity:

```c++
void Hippocampus::updatePlaceCells(TimestepDuration dt) {
    if (!placeCellsEnabled_) return;
    
    // Update place cell firing based on current position
    for (auto& cell : placeCells_) {
        float oldFiring = cell.firingRate;
        float newFiring = computePlaceCellActivation(cell, currentSpatialInfo_);
        
        // Apply plasticity rules for place cell strengthening
        if (newFiring > oldFiring) {
            // Increase firing rate for good matches
            cell.firingRate = newFiring;
            cell.familiarity = oldFiring * 0.9f + newFiring * 0.1f;
        } else {
            // Decay firing rate for poor matches
            cell.firingRate = std::max(0.0f, newFiring * 0.95f);
            cell.familiarity *= 0.98f;
        }
        
        // Update neural pattern based on firing rate
        updatePlaceCellPattern(cell);
    }
    
    // Update place cell connections
    updatePlaceCellConnections();
}

void Hippocampus::updatePlaceCellConnections() {
    // Implement associative learning for place cells
    // Cells that fire together strengthen their connections
    // This supports spatial memory and navigation
    
    for (size_t i = 0; i < placeCells_.size(); ++i) {
        for (size_t j = i + 1; j < placeCells_.size(); ++j) {
            const PlaceCell& cell1 = placeCells_[i];
            const PlaceCell& cell2 = placeCells_[j];
            
            // Compute co-activation strength
            float coActivation = computeCoActivation(cell1, cell2);
            
            if (coActivation > 0.01f) {
                // Strengthen connections between co-activated cells
                strengthenPlaceCellConnection(i, j, coActivation);
            }
        }
    }
}

void Hippocampus::updatePlaceCellPattern(PlaceCell& cell) {
    // Update the neural pattern associated with this place cell
    // The pattern represents the distributed neural representation
    
    float patternStrength = cell.firingRate / 10.0f;  // Normalize to [0, 1]
    
    // Get the brain's working memory
    auto* workingMemory = brain_->getWorkingMemory();
    if (!workingMemory) return;
    
    // Store pattern in working memory
    workingMemory->storeToNeuron(cell.id, patternStrength);
    
    // Update episodic memory if encoding is enabled
    if (memoryEncodingEnabled_) {
        encodePlaceCellMemory(cell);
    }
}
```

### Grid Cell Dynamics

Grid cells provide multi-scale spatial representation:

```c++
void Hippocampus::updateGridCells(TimestepDuration dt) {
    if (!gridCellsEnabled_) return;
    
    // Update grid cell firing
    for (auto& cell : gridCells_) {
        float oldFiring = cell.currentFiring;
        float newFiring = computeGridCellActivation(cell, currentSpatialInfo_);
        
        // Grid cells have persistent firing patterns
        cell.currentFiring = newFiring;
        
        // Update grid field connections
        updateGridCellConnections();
    }
}

void Hippocampus::updateGridCellConnections() {
    // Grid cells maintain stable connectivity patterns
    // They provide the framework for place cell development
    
    for (size_t i = 0; i < gridCells_.size(); ++i) {
        GridCell& cell = gridCells_[i];
        
        // Establish periodic firing pattern
        establishGridPeriodicity(cell);
        
        // Update orientation and scale
        updateGridCellOrientation(cell);
        updateGridCellScale(cell);
        
        // Update neural connections
        updateGridCellNeuralConnections(cell);
    }
}
```

### Episodic Memory Processing

Episodic memory encoding and consolidation:

```c++
void Hippocampus::updateEpisodicMemory(TimestepDuration dt) {
    // Process short-term memory traces into episodic memories
    if (!memoryEncodingEnabled_) return;
    
    // Check if it's time to consolidate short-term memories
    if (shouldConsolidateMemory()) {
        consolidateMemories();
    }
    
    // Update existing episodic memories
    for (auto& memory : episodicMemories_) {
        updateEpisodicMemoryConsolidation(memory, dt);
    }
}

void Hippocampus::consolidateMemories() {
    // Transfer important short-term memories to long-term storage
    std::vector<WorkingMemoryTrace> importantTraces;
    
    auto* workingMemory = brain_->getWorkingMemory();
    if (!workingMemory) return;
    
    // Identify high-salience memories for consolidation
    for (size_t i = 0; i < shortTermMemory_.size(); ++i) {
        const WorkingMemoryTrace& trace = shortTermMemory_[i];
        
        // Calculate trace importance based on activation and novelty
        float importance = calculateMemoryImportance(trace);
        
        if (importance > 0.7f) {  // High importance threshold
            importantTraces.push_back(trace);
        }
    }
    
    // Convert to episodic memories
    for (const auto& trace : importantTraces) {
        createEpisodicFromTrace(trace);
    }
    
    // Clear processed traces
    shortTermMemory_.clear();
}

void Hippocampus::encodeEpisodicMemory(const std::string& context) {
    // Create new episodic memory from current neural state
    
    EpisodicMemory memory;
    memory.startTime = getCurrentTime();
    memory.context = context;
    memory.coherence = 1.0f;
    
    // Collect neural activity patterns
    auto neurons = brain_->getAllNeurons();
    auto* workingMemory = brain_->getWorkingMemory();
    
    if (workingMemory) {
        for (Neuron* neuron : neurons) {
            float activation = workingMemory->getNeuronActivation(neuron->getId());
            memory.neurons.push_back(neuron->getId());
            memory.activations.push_back(activation);
        }
    }
    
    // Calculate reward
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        memory.reward = dopamine->getLevel();
    }
    
    // Store the episodic memory
    episodicMemories_.push_back(memory);
}
```

### Memory Replay

Memory consolidation through replay during rest periods:

```c++
void Hippocampus::replayEpisodes(size_t episodeCount) {
    if (!memoryReplayEnabled_) return;
    
    // Select episodes for replay
    std::vector<const EpisodicMemory*> episodesToReplay;
    
    if (episodicMemories_.size() > episodeCount) {
        // Randomly select episodes (weighted by coherence)
        std::vector<float> weights;
        for (const auto& memory : episodicMemories_) {
            weights.push_back(memory.coherence);
        }
        
        episodesToReplay = selectEpisodes(episodesToReplay, weights, episodeCount);
    } else {
        // Use all available episodes
        for (const auto& memory : episodicMemories_) {
            episodesToReplay.push_back(&memory);
        }
    }
    
    // Replay selected episodes
    for (const EpisodicMemory* episode : episodesToReplay) {
        replayEpisode(*episode);
    }
}

void Hippocampus::replayEpisode(const EpisodicMemory& episode) {
    // Reactivate neural patterns associated with the episode
    // This strengthens synaptic connections and consolidates memory
    
    auto* workingMemory = brain_->getWorkingMemory();
    if (!workingMemory) return;
    
    // Reactivate the neural pattern
    for (size_t i = 0; i < episode.neurons.size(); ++i) {
        NeuronId neuronId = episode.neurons[i];
        float activation = episode.activations[i] * 0.8f;  // Slight attenuation
        
        // Store in working memory for replay
        workingMemory->storeToNeuron(neuronId, activation);
    }
    
    // Apply neuromodulation to enhance consolidation
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        // Positive prediction error during replay
        dopamine->setLevel(dopamine->getLevel() * 1.1f);
    }
}
```

### Spatial Information Processing

Current position and orientation tracking:

```c++
SpatialInfo Hippocampus::getCurrentSpatialInfo() const {
    SpatialInfo info;
    
    if (!world_) {
        return info;
    }
    
    const auto& agentBody = world_->getAgentBody();
    
    info.x = agentBody.x;
    info.y = agentBody.y;
    info.orientation = agentBody.orientation;
    info.velocityX = agentBody.velocityX;
    info.velocityY = agentBody.velocityY;
    info.angularVelocity = agentBody.angularVelocity;
    info.energy = agentBody.energy;
    
    // Add sensory context
    const auto& percept = world_->getSensoryPercept();
    info.vision = percept.getVision();
    info.touch = percept.getTouch();
    
    return info;
}

float Hippocampus::computePlaceCellActivation(const PlaceCell& cell,
                                            const SpatialInfo& info) const {
    // Compute distance from place cell location
    float dx = info.x - cell.x;
    float dy = info.y - cell.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    // Gaussian firing field centered at cell location
    float sigma = cell.fieldRadius;  // Field radius
    float activation = std::exp(-(distance * distance) / (2.0f * sigma * sigma));
    
    // Apply familiarization factor
    activation *= cell.familiarity;
    
    // Add noise for biological variability
    activation *= (1.0f + (std::rand() % 100 - 50) * 0.01f);
    
    return std::max(0.0f, std::min(1.0f, activation));
}
```

## Integration

### Hippocampus in Brain Class

The hippocampus integrates with the main Brain class:

```c++
class Brain {
public:
    // ... existing methods ...
    
    // Hippocampus interface
    Hippocampus* getHippocampus() { return hippocampus_.get(); }
    const Hippocampus* getHippocampus() const { return hippocampus_.get(); }
    
    // Spatial memory interface
    void updateSpatialMemory(const SpatialInfo& info);
    SpatialInfo getCurrentLocation() const;
    
    // Memory interface
    void storeEpisodicMemory(const std::string& context);
    EpisodicMemory* retrieveEpisodicMemory(const std::string& context,
                                           float similarityThreshold = 0.5f);
    
    // Navigation interface
    std::vector<float> getPlaceCellActivations() const;
    float getSpatialCoherence() const;
    
private:
    struct Impl {
        std::unique_ptr<Hippocampus> hippocampus_;
        // ... other members ...
    };
    
    std::unique_ptr<Impl> pImpl;
};
```

## Performance Considerations

### Memory Efficiency

- **Sparse Coding**: Place cells maintain sparse activation patterns
- **Memory Compression**: Episodic memories store compressed representations
- **Replay Efficiency**: Batch processing of memory replay events

### Computational Complexity

- **Place Cell Updates**: O(N) where N is number of place cells
- **Grid Cell Updates**: O(M) where M is number of grid cells
- **Memory Encoding**: O(P) where P is pattern size
- **Memory Retrieval**: O(E×P) where E is number of episodes

### Scalability

The hippocampus can scale to large environments:

- **Environment Size**: Handles environments from small rooms to large worlds
- **Memory Capacity**: Scales with available working memory
- **Processing Speed**: Parallel processing for multiple simultaneous updates

## Testing and Validation

### Hippocampus Tests

```bash
# Test place cell formation
./test-hippocampus --test-type place-cells

# Test grid cell integration
./test-hippocampus --test-type grid-cells

# Test episodic memory
./test-hippocampus --test-type episodic-memory

# Test spatial navigation
./test-hippocampus --test-type spatial-navigation

# Run all hippocampus tests
./test-hippocampus --all
```

### Validation Metrics

1. **Place Cell Formation**: Cells develop stable spatial firing patterns
2. **Grid Cell Coherence**: Consistent firing patterns across multiple scales
3. **Memory Encoding**: Accurate storage of episodic information
4. **Memory Retrieval**: Faithful reconstruction of stored episodes
5. **Spatial Navigation**: Accurate path planning and navigation

## Future Enhancements

1. **Dynamic Environment Adaptation**: Hippocampus adapts to changing environments
2. **Memory Forgetting**: Implements natural memory decay mechanisms
3. **Social Memory**: Extends to social and cultural memory
4. **Emotional Memory**: Integrates with emotional processing systems
5. **Memory Integration**: Better integration with neocortical memory systems

## Conclusion

The hippocampus implementation provides:

1. **Spatial Navigation**: Cognitive mapping of the environment
2. **Episodic Memory**: Binding experiences into coherent episodes
3. **Memory Consolidation**: Transfer between memory systems
4. **Pattern Separation**: Distinguishing similar experiences
5. **Pattern Completion**: Reconstructing partial memories

This hippocampus component is essential for creating a biologically plausible artificial brain with spatial awareness, episodic memory, and the ability to navigate and learn from experiences in complex environments.

---

This hippocampus implementation provides the spatial and memory foundations for the NLM brain, enabling it to:

1. **Navigate environments** using place and grid cell representations
2. **Form episodic memories** by binding experiences in time and space
3. **Consolidate memories** through replay mechanisms
4. **Maintain spatial context** for memory retrieval
5. **Support complex behaviors** based on spatial and memory information

The hippocampus is a critical component that bridges low-level neural processing with high-level cognitive functions, enabling the NLM brain to interact intelligently with its environment and learn from its experiences.
