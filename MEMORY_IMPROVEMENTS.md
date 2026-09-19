# NLM Memory System Improvements

## Overview
This document outlines improvements to the NLM memory system architecture, focusing on creating a memory abstraction layer and refactoring existing memory implementations for better maintainability, testability, and code reuse.

## Current Memory System

### Memory Components
1. **NeuralWorkingMemory** (426 lines) - Transient active information storage
2. **NeuralEpisodicMemory** (18KB) - Experience storage with replay capability
3. **NeuralAssociativeMemory** - Pattern associations (implementation not found)
4. **Memory.hpp** - Placeholder memory interfaces

### Issues Identified
1. Code duplication across memory systems
2. Inconsistent interfaces and patterns
3. Large monolithic implementations
4. Difficult to test and maintain
5. Poor separation of concerns

## Memory Abstraction Layer

### Base Memory Class
```cpp
#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

// Memory system interface
class BaseMemory {
public:
    virtual ~BaseMemory() = default;
    
    // Initialization
    virtual void initialize(Brain* brain) = 0;
    virtual void clear() = 0;
    virtual void reset() = 0;
    
    // Storage operations
    virtual void store(const std::vector<float>& pattern, float strength = 1.0f) = 0;
    virtual std::vector<float> retrieve() const = 0;
    virtual bool contains(NeuronId neuron) const = 0;
    
    // Capacity management
    virtual size_t getCapacity() const = 0;
    virtual void setCapacity(size_t cap) = 0;
    
    // Decay mechanisms
    virtual float getDecayRate() const = 0;
    virtual void setDecayRate(float rate) = 0;
    virtual void decay() = 0;
    
    // Statistics
    virtual size_t getActiveTraces() const = 0;
    virtual float getMemoryActivity() const = 0;
    
    // State management
    virtual void update(double timestep) = 0;
    virtual void strengthenMemory(float factor) = 0;
};

// Memory statistics
struct MemoryStats {
    size_t totalMemories;
    size_t activeMemories;
    float totalMemoryActivity;
    size_t totalCapacity;
    
    MemoryStats() : totalMemories(0), activeMemories(0), 
                   totalMemoryActivity(0.0f), totalCapacity(0) {}
};

// Memory manager for system-wide coordination
class MemoryManager {
public:
    MemoryManager();
    ~MemoryManager();
    
    // Register memory system
    void registerMemory(const std::string& name, BaseMemory* memory);
    void unregisterMemory(const std::string& name);
    
    // Get memory system
    BaseMemory* getMemory(const std::string& name) const;
    BaseMemory* getWorkingMemory() const { return workingMemory_.get(); }
    BaseMemory* getEpisodicMemory() const { return episodicMemory_.get(); }
    BaseMemory* getAssociativeMemory() const { return associativeMemory_.get(); }
    
    // System operations
    void initializeAll(Brain* brain);
    void updateAll(double timestep);
    void clearAll();
    void resetAll();
    
    // Statistics
    MemoryStats getStats() const;
    
    // Configuration
    void setMemoryCapacity(size_t capacity);
    void setDecayRate(float rate);
    
private:
    std::unordered_map<std::string, BaseMemory*> memories_;
    std::unique_ptr<BaseMemory> workingMemory_;
    std::unique_ptr<BaseMemory> episodicMemory_;
    std::unique_ptr<BaseMemory> associativeMemory_;
};

} // namespace nlm
```

### Refactored NeuralWorkingMemory

#### Implementation Structure
```cpp
#pragma once

#include "Memory.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

namespace nlm {

// Enhanced NeuralWorkingMemory using BaseMemory interface
class NeuralWorkingMemory : public BaseMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();
    
    // BaseMemory interface implementation
    void initialize(Brain* brain) override;
    void clear() override;
    void reset() override;
    
    void store(const std::vector<float>& pattern, float strength = 1.0f) override;
    std::vector<float> retrieve() const override;
    bool contains(NeuronId neuron) const override;
    
    size_t getCapacity() const override;
    void setCapacity(size_t cap) override;
    
    float getDecayRate() const override;
    void setDecayRate(float rate) override;
    void decay() override;
    
    size_t getActiveTraces() const override;
    float getMemoryActivity() const override;
    
    void update(double timestep) override;
    void strengthenMemory(float factor) override;
    
    // Specialized working memory methods
    void storeToNeuron(NeuronId neuron, float activation);
    float getNeuronActivation(NeuronId neuron) const;
    bool isWinning(NeuronId neuron) const;
    void runCompetition();
    
    // Legacy API for backward compatibility
    void initialize(Brain* brain);  // Duplicate for backward compatibility
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    std::vector<float> retrieve() const;
    bool contains(NeuronId neuron) const;
    
    // Getters
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
private:
    // Internal implementation
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    
    // Memory content
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    // Active memory traces
    std::vector<size_t> activeTraces_;
    
    // Recurrent connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    
    // Winner neurons (for competition)
    std::vector<NeuronId> winners_;
};

} // namespace nlm
```

### Refactored NeuralEpisodicMemory

#### Implementation Structure
```cpp
#pragma once

#include "Memory.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <deque>

namespace nlm {

// Enhanced NeuralEpisodicMemory using BaseMemory interface
class NeuralEpisodicMemory : public BaseMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // BaseMemory interface implementation
    void initialize(Brain* brain) override;
    void clear() override;
    void reset() override;
    
    void store(const std::vector<float>& pattern, float strength = 1.0f) override;
    std::vector<float> retrieve() const override;
    bool contains(NeuronId neuron) const override;
    
    size_t getCapacity() const override;
    void setCapacity(size_t cap) override;
    
    float getDecayRate() const override;
    void setDecayRate(float rate) override;
    void decay() override;
    
    size_t getActiveTraces() const override;
    float getMemoryActivity() const override;
    
    void update(double timestep) override;
    void strengthenMemory(float factor) override;
    
    // Episodic-specific methods
    void storeEpisode(const EpisodicMemoryItem& episode);
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    size_t getEpisodeCount() const;
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Replay functionality
    void replayEpisode(const EpisodicMemoryItem* episode);
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    void consolidate(float relevanceThreshold);
    
    // Legacy API for backward compatibility
    void initialize(Brain* brain);
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Getters
    size_t getMaxEpisodes() const { return maxEpisodes_; }
    
private:
    // Internal implementation
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    size_t maxEpisodes_;
    size_t currentReplayIndex_;
    
    // Episode storage
    std::deque<EpisodicMemoryItem> episodes_;
    std::deque<bool> replayedEpisodes_;
    
    // Replay state
    std::vector<EpisodicMemoryItem> replayBuffer_;
};

} // namespace nlm
```

## Memory Layer Integration

### Updated Brain.hpp
```cpp
// Add memory manager to Brain class
class Brain {
public:
    // ... existing methods ...
    
    // Memory system access
    MemoryManager* getMemoryManager() { return memoryManager_.get(); }
    const MemoryManager* getMemoryManager() const { return memoryManager_.get(); }
    
    // Memory statistics
    MemoryStats getMemoryStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Memory manager
    std::unique_ptr<MemoryManager> memoryManager_;
};
```

### Memory Layer Implementation

#### Memory Layer Header
```cpp
// src/memory/MemoryLayer.hpp
#pragma once

#include "Memory.hpp"
#include "NeuralWorkingMemory.hpp"
#include "NeuralEpisodicMemory.hpp"
#include <memory>

namespace nlm {

// Memory layer factory functions
std::unique_ptr<BaseMemory> createWorkingMemory();
std::unique_ptr<BaseMemory> createEpisodicMemory();
std::unique_ptr<BaseMemory> createAssociativeMemory();

// Memory layer utilities
MemoryStats collectMemoryStats(const std::vector<std::unique_ptr<BaseMemory>>& memories);
void configureMemorySystems(MemoryManager& manager, const Config& config);

} // namespace nlm
```

#### Memory Layer Implementation
```cpp
// src/memory/MemoryLayer.cpp
#include "MemoryLayer.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

std::unique_ptr<BaseMemory> createWorkingMemory() {
    return std::make_unique<NeuralWorkingMemory>();
}

std::unique_ptr<BaseMemory> createEpisodicMemory() {
    return std::make_unique<NeuralEpisodicMemory>();
}

std::unique_ptr<BaseMemory> createAssociativeMemory() {
    // TODO: Implement AssociativeMemory
    return nullptr;
}

MemoryStats collectMemoryStats(const std::vector<std::unique_ptr<BaseMemory>>& memories) {
    MemoryStats stats;
    for (const auto& memory : memories) {
        if (memory) {
            stats.totalMemories++;
            stats.activeMemories += memory->getActiveTraces();
            stats.totalMemoryActivity += memory->getMemoryActivity();
            stats.totalCapacity += memory->getCapacity();
        }
    }
    return stats;
}

void configureMemorySystems(MemoryManager& manager, const Config& config) {
    // Configure working memory
    auto workingMemory = createWorkingMemory();
    if (auto capacity = config.getOr<size_t>("working_memory_capacity", 100)) {
        workingMemory->setCapacity(*capacity);
    }
    if (auto decayRate = config.getOr<float>("memory_decay_rate", 0.05f)) {
        workingMemory->setDecayRate(*decayRate);
    }
    manager.registerMemory("working", workingMemory.get());
    
    // Configure episodic memory
    auto episodicMemory = createEpisodicMemory();
    if (auto maxEpisodes = config.getOr<size_t>("episodic_memory_max_episodes", 1000)) {
        // TODO: Add setter method to EpisodicMemory
    }
    manager.registerMemory("episodic", episodicMemory.get());
    
    // Associate memories with manager
    manager.workingMemory_ = std::move(workingMemory);
    manager.episodicMemory_ = std::move(episodicMemory);
}

} // namespace nlm
```

## Benefits of Memory Abstraction Layer

### 1. Code Reusability
- Common memory operations abstracted
- Shared initialization and cleanup logic
- Standardized error handling

### 2. Improved Testability
- Mockable base class for unit tests
- Consistent interfaces for testing
- Isolated component testing

### 3. Better Maintainability
- Single point of change for common functionality
- Clear separation of concerns
- Reduced code duplication

### 4. Enhanced Extensibility
- Easy to add new memory types
- Plugin architecture for memory systems
- Configuration-based system setup

### 5. Performance Benefits
- Shared resource management
- Optimized common operations
- Reduced memory footprint

## Implementation Plan

### Phase 1: Core Abstraction
1. Create BaseMemory interface
2. Implement MemoryManager
3. Refactor NeuralWorkingMemory
4. Update Brain integration

### Phase 2: Episodic Memory
1. Refactor NeuralEpisodicMemory
2. Implement replay functionality
3. Add consolidation mechanisms

### Phase 3: Advanced Features
1. Implement AssociativeMemory
2. Add SemanticMemory
3. Implement ProceduralMemory
4. Complete test suite

## Files to Modify

### New Files
- `src/memory/MemoryLayer.hpp`
- `src/memory/MemoryLayer.cpp`

### Modified Files
- `src/memory/Memory.hpp`
- `src/memory/NeuralWorkingMemory.hpp`
- `src/memory/NeuralWorkingMemory.cpp`
- `src/memory/NeuralEpisodicMemory.hpp`
- `src/memory/NeuralEpisodicMemory.cpp`
- `src/brain/Brain.hpp`
- `src/brain/Brain.cpp`

### New Files
- `src/memory/AssociativeMemory.hpp`
- `src/memory/AssociativeMemory.cpp`

## Testing Strategy

### Unit Tests
- BaseMemory interface tests
- MemoryManager tests
- Memory system-specific tests
- Integration tests

### Performance Tests
- Memory usage comparison
- Initialization performance
- Storage/retrieval benchmarks
- Replay performance

### Regression Tests
- Backward compatibility tests
- API consistency tests
- Data integrity tests

## Conclusion

The memory abstraction layer provides a solid foundation for the NLM memory systems by:

1. **Standardizing interfaces** across all memory types
2. **Reducing code duplication** through shared base functionality
3. **Improving testability** with mockable abstract classes
4. **Enhancing maintainability** with clear separation of concerns
5. **Enabling extensibility** for future memory system additions

This approach aligns with NLM's goal of creating a sophisticated, brain-inspired architecture while maintaining the experimental nature of the project.