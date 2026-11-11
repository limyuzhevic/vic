# Working Memory Integration Enhancement

## Overview
This document outlines the enhancements needed for Phase 6 integration of the working memory system with the attention system.

## Current State Analysis

### Working Memory System
**Status**: Partially implemented but not fully integrated

**Components**:
- **Storage**: store(), storeToNeuron() methods
- **Retrieval**: retrieve() method
- **Update**: update() method with decay
- **Competition**: runCompetition() method
- **Selection**: isWinning() method

**Current Implementation Issues**:
1. **Limited Storage**: Only stores neuron activations, not rich memory content
2. **Basic Competition**: Simple winner-take-all mechanism
3. **No Attention Integration**: Working memory is used by attention system but doesn't actively participate in attentional selection
4. **Weak Integration**: Connection to sensory processing is minimal

### Attention System
**Status**: Implemented but not fully integrated with working memory

**Components**:
- **Competition Processing**: processCompetition() method
- **Region Focusing**: focusOnRegion(), getAttendedRegions()
- **Inhibition/Excitation Control**: getInhibitionFor(), getExcitationFor()

**Current Implementation Issues**:
1. **Limited Scope**: Only processes working memory neurons
2. **No Bidirectional Integration**: Working memory doesn't influence attention system
3. **No Priority Mechanisms**: No clear memory prioritization based on relevance
4. **Weak Connection to Sensory Input**: Limited connection to real-time sensory processing

## Integration Strategy

### Priority 1: Enhance Working Memory-Sensory Integration

**Enhance store() method to support sensory integration**:
```cpp
void NeuralWorkingMemory::storeWithContext(
    const std::vector<float>& pattern,
    float strength,
    const std::vector<float>& salience,
    const std::vector<float>& topDownBias,
    SimulationStep timestamp) {
    
    // Store pattern with rich context information
    store(pattern, strength);
    
    // Update salience and bias maps for attention integration
    for (size_t i = 0; i < pattern.size() && i < salience.size(); ++i) {
        bottomUpSalience_[memoryNeurons_[i].value] = salience[i];
    }
    
    // Record timestamp for decay calculation
    for (size_t i = 0; i < pattern.size() && i < memoryTimestamps_.size(); ++i) {
        memoryTimestamps_[i] = timestamp;
    }
}
```

### Priority 2: Implement Bidirectional Working Memory-Attention Integration

**Enhance NeuralWorkingMemory to support attention system**:
```cpp
class NeuralWorkingMemory {
public:
    // Existing methods...
    
    // New methods for attention integration
    std::vector<float> getAttentionBasedWeights() const;
    std::vector<NeuronId> getAttendedNeurons(float threshold) const;
    void applyAttentionSignal(const std::vector<NeuronId>& attended);
    float getAttentionalPriority(NeuronId neuron) const;
    
    // Integration with attention system
    void integrateWithAttention(const AttentionalSelection* attention);
    void updateBasedOnAttention(const std::vector<NeuronId>& winners);
};
```

### Priority 3: Implement Memory Priority Mechanisms

**Add priority-based memory selection**:  

```cpp
void NeuralWorkingMemory::prioritizeMemory(
    const std::vector<NeuronId>& attendedRegions,
    float decayPriority) {
    
    std::vector<size_t> priorityOrder;
    std::vector<float> priorityScores;
    
    // Calculate priority based on attended regions and salience
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float salience = getSalienceFor(neuron);
        float bias = getBiasFor(neuron);
        float timestamp = getTimestampFor(i);
        
        // Calculate priority score
        float priority = salience * 0.5f + bias * 0.3f + 
                        (1.0f - timestamp / currentTime) * 0.2f;
        
        priorityScores.push_back(priority);
        priorityOrder.push_back(i);
    }
    
    // Sort by priority
    std::sort(priorityOrder.begin(), priorityOrder.end(),
              [&priorityScores](size_t a, size_t b) {
                  return priorityScores[a] > priorityScores[b];
              });
    
    // Adjust activations based on priority
    for (size_t i = 0; i < priorityOrder.size(); ++i) {
        size_t idx = priorityOrder[i];
        float priorityBoost = (i < 10) ? 1.5f : 1.0f;  // Boost top 10
        
        if (i < memoryActivations_.size()) {
            memoryActivations_[idx] *= priorityBoost;
        }
    }
}
```

### Priority 4: Enhance Competition Mechanisms

**Implement advanced competition with attention integration**:  

```cpp
void NeuralWorkingMemory::runAdvancedCompetition() {
    // Enhanced competition that integrates attention signals
    std::vector<float> competitionScores;
    
    // Calculate competition scores based on multiple factors
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        float salience = getSalienceFor(neuron);
        float bias = getBiasFor(neuron);
        float age = memoryTimestamps_[i];
        
        // Advanced competition formula
        float score = activation * 0.4f + 
                     salience * 0.3f + 
                     bias * 0.2f + 
                     (1.0f - age / currentTime) * 0.1f;
        
        competitionScores.push_back(score);
    }
    
    // Apply attention-based modulation
    std::vector<NeuronId> attendedNeurons = getAttendedNeurons(0.5f);
    for (const auto& neuron : attendedNeurons) {
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            if (memoryNeurons_[i] == neuron) {
                competitionScores[i] *= 1.5f;  // Boost attended neurons
            }
        }
    }
    
    // Select winners based on competition scores
    float threshold = calculateCompetitionThreshold(competitionScores);
    winners_.clear();
    
    for (size_t i = 0; i < competitionScores.size(); ++i) {
        if (competitionScores[i] >= threshold) {
            winners_.push_back(memoryNeurons_[i]);
        }
    }
}
```

## Implementation Plan

### Phase 1: Core Enhancement (Weeks 1-2)

1. **Enhance store() method** to support rich context storage
2. **Add attention integration methods** to NeuralWorkingMemory
3. **Update runCompetition()** to use attention-based scoring
4. **Add priority mechanisms** for memory selection
5. **Enhance decay mechanisms** with attention-based modulation

### Phase 2: Integration (Weeks 3-4)

1. **Update Brain.cpp** to use enhanced working memory
2. **Integrate attention signals** with working memory update
3. **Add working memory influence** on attention system
4. **Implement feedback loops** between systems
5. **Add monitoring and logging** for system integration

### Phase 3: Testing and Validation (Weeks 5-6)

1. **Test memory-attention integration** with various scenarios
2. **Validate competition mechanisms** with different stimulus patterns
3. **Ensure system stability** under various conditions
4. **Measure performance improvement** over baseline implementation
5. **Document integration results** for future development

## Expected Benefits

### Memory System Improvements
1. **Richer Memory Content**: Store and retrieve more detailed memory representations
2. **Attention-Based Selection**: Select memories based on attention priorities
3. **Dynamic Memory Management**: Adapt memory based on attentional focus
4. **Enhanced Competition**: More sophisticated memory competition mechanisms
5. **Better Integration**: Seamless integration with attention system

### Attention System Improvements
1. **Better Memory Integration**: Attention system can influence working memory
2. **Richer Input Data**: Attention system receives richer memory information
3. **Enhanced Selection**: Better selection of attentional focus based on memory
4. **Improved Performance**: Better overall system performance
5. **More Adaptive**: System can adapt to changing attentional demands

### Overall System Improvements
1. **More Integrated**: Memory and attention systems work together as a cohesive unit
2. **More Adaptive**: System can better adapt to changing environmental demands
3. **More Efficient**: Better allocation of cognitive resources
4. **More Effective**: Better performance on memory and attention tasks
5. **More Scalable**: System can handle more complex tasks

## Technical Details

### Enhanced NeuralWorkingMemory Methods

```cpp
// Core enhancement methods
void storeWithContext(const std::vector<float>& pattern, float strength,
                     const std::vector<float>& salience,
                     const std::vector<float>& topDownBias,
                     SimulationStep timestamp);

std::vector<float> getAttentionBasedWeights() const;
std::vector<NeuronId> getAttendedNeurons(float threshold) const;
void applyAttentionSignal(const std::vector<NeuronId>& attended);
float getAttentionalPriority(NeuronId neuron) const;

// Integration methods
void integrateWithAttention(const AttentionalSelection* attention);
void updateBasedOnAttention(const std::vector<NeuronId>& winners);

// Enhanced competition
void runAdvancedCompetition();
void prioritizeMemory(const std::vector<NeuronId>& attendedRegions,
                      float decayPriority);
```

### Enhanced AttentionalSelection Methods

```cpp
// Enhanced attention methods
void setWorkingMemory(NeuralWorkingMemory* workingMemory);
void updateWorkingMemoryBasedOnAttention();
std::vector<NeuronId> getMostActiveWorkingMemoryNeurons() const;
float getWorkingMemoryActivityLevel() const;
```

### Integration in Brain.cpp

```cpp
// Enhanced brain integration
void Brain::updateWorkingMemoryBasedOnAttention() {
    if (pImpl->attention && pImpl->workingMemory) {
        // Get attended neurons from attention system
        std::vector<NeuronId> attended = pImpl->attention->getAttendedNeurons(0.5f);
        
        // Apply attentional signals to working memory
        pImpl->workingMemory->applyAttentionSignal(attended);
        
        // Update working memory based on attention
        pImpl->workingMemory->updateBasedOnAttention(attended);
        
        // Enhance attention based on working memory
        pImpl->attention->updateWorkingMemoryBasedOnAttention();
    }
}
```

## Conclusion

This enhancement plan provides a comprehensive approach to integrating the working memory system with the attention system for Phase 6. The plan focuses on:

1. **Enhancing working memory** to support richer content storage
2. **Improving attention system** to better integrate with working memory
3. **Implementing advanced competition** mechanisms
4. **Adding priority-based selection** for memories
5. **Creating bidirectional integration** between systems

The result will be a more integrated, adaptive, and efficient cognitive architecture that better models human-like memory and attention processing.