# MEMORY MANAGEMENT FIXES - EXACT CODE CHANGES NEEDED

## FIX 1: Memory Classes - Memory.cpp

### Problem:
All memory classes have destructors `= default` but should delete pImpl.

### Solution:
Replace all `= default` destructors with proper pImpl deletion.

**Current code (lines 16, 77, 121, 166, 219):**
```cpp
WorkingMemory::~WorkingMemory() = default;
EpisodicMemory::~EpisodicMemory() = default;
SemanticMemory::~SemanticMemory() = default;
ProceduralMemory::~ProceduralMemory() = default;
AssociativeMemory::~AssociativeMemory() = default;
```

**Fixed code:**
```cpp
WorkingMemory::~WorkingMemory() {
    delete pImpl;
}
EpisodicMemory::~EpisodicMemory() {
    delete pImpl;
}
SemanticMemory::~SemanticMemory() {
    delete pImpl;
}
ProceduralMemory::~ProceduralMemory() {
    delete pImpl;
}
AssociativeMemory::~AssociativeMemory() {
    delete pImpl;
}
```

## FIX 2: Neural Classes - NeuralPopulation.cpp

### Problem:
NeuralPopulation destructor is `= default` but should delete pImpl.

### Solution:
Replace default destructor with proper pImpl deletion.

**Current code (line 19):**
```cpp
NeuralPopulation::~NeuralPopulation() = default;
```

**Fixed code:**
```cpp
NeuralPopulation::~NeuralPopulation() {
    delete pImpl;
}
```

## FIX 3: Neural Classes - NeuralRegion.cpp

### Problem:
NeuralRegion destructor is `= default` but should delete pImpl.

### Solution:
Replace default destructor with proper pImpl deletion.

**Current code (line 25):**
```cpp
NeuralRegion::~NeuralRegion() = default;
```

**Fixed code:**
```cpp
NeuralRegion::~NeuralRegion() {
    delete pImpl;
}
```

## FIX 4: Neural Classes - Neuron.cpp

### Problem:
Neuron destructor is `= default` but should delete pImpl.

### Solution:
Replace default destructor with proper pImpl deletion.

**Current code (line 38):**
```cpp
Neuron::~Neuron() = default;
```

**Fixed code:**
```cpp
Neuron::~Neuron() {
    delete pImpl;
}
```

## FIX 5: Neural Classes - Synapse.cpp

### Problem:
Synapse destructor is `= default` but should delete pImpl.

### Solution:
Replace default destructor with proper pImpl deletion.

**Current code (line 65):**
```cpp
Synapse::~Synapse() = default;
```

**Fixed code:**
```cpp
Synapse::~Synapse() {
    delete pImpl;
}
```

## FIX 6: Neural Classes - Brain.cpp

### Problem:
Brain destructor is `= default` but should delete pImpl.

### Solution:
Replace default destructor with proper pImpl deletion.

**Current code (line 162):**
```cpp
Brain::~Brain() = default;
```

**Fixed code:**
```cpp
Brain::~Brain() {
    delete pImpl;
}
```

## FIX 7: Neural Classes - SpikeSystem.cpp

### Problem:
SpikeSystem destructor is `= default` but should delete pImpl.

### Solution:
Replace default destructor with proper pImpl deletion.

**Current code (line 23):**
```cpp
SpikeSystem::~SpikeSystem() = default;
```

**Fixed code:**
```cpp
SpikeSystem::~SpikeSystem() {
    delete pImpl;
}
```

## FIX 8: Neural Classes - RandomGenerator.cpp

### Problem:
RandomGenerator destructor is `= default` but should delete pImpl.

### Solution:
Replace default destructor with proper pImpl deletion.

**Current code (line 17):**
```cpp
RandomGenerator::~RandomGenerator() = default;
```

**Fixed code:**
```cpp
RandomGenerator::~RandomGenerator() {
    delete pImpl;
}
```

## FIX 9: Neural Classes - SimulationClock.cpp

### Problem:
SimulationClock destructor is `= default` but should delete pImpl.

### Solution:
Replace default destructor with proper pImpl deletion.

**Current code (line 19):**
```cpp
SimulationClock::~SimulationClock() = default;
```

**Fixed code:**
```cpp
SimulationClock::~SimulationClock() {
    delete pImpl;
}
```

## FIX 10: Memory Pool - MemoryPool.hpp

### Problem:
MemoryPool destructor is empty but should ensure proper cleanup.

### Solution:
Replace empty destructor with explicit cleanup.

**Current code (lines 170-172):**
```cpp
template<typename T>
MemoryPool<T>::~MemoryPool() {
    // Blocks are automatically freed via unique_ptr
}
```

**Fixed code:**
```cpp
template<typename T>
MemoryPool<T>::~MemoryPool() {
    // Clear free list to prevent accessing deleted nodes
    while (!empty()) {
        T* dummy = nullptr;
        deallocate(dummy);  // This will pop from free list
    }
    // Blocks are automatically freed via unique_ptr
}
```

## FIX 11: SparseConnectivity - SparseConnectivity.hpp

### Problem:
freeList_ can grow without bound when synapses are removed.

### Solution:
Implement periodic cleanup of freeList_ or use a more efficient free list management.

**Add to clear() method (line 443):**
```cpp
inline void SparseConnectivity::clear() {
    outgoing_.clear();
    incoming_.clear();
    synapses_.clear();
    freeList_.clear();  // Already cleared, but add this for safety
    numNeurons_ = 0;
    maxNeuronId_ = 0;
}
```

**Add limit to freeList_ size:**
```cpp
inline void SparseConnectivity::removeSynapse(size_t synapseIndex) {
    if (synapseIndex >= synapses_.size()) return;
    
    const auto& syn = synapses_[synapseIndex];
    
    // Remove from outgoing list
    auto& outList = outgoing_[syn.sourceNeuron];
    outList.erase(std::remove(outList.begin(), outList.end(), synapseIndex), outList.end());
    
    // Remove from incoming list
    auto& inList = incoming_[syn.destNeuron];
    inList.erase(std::remove(inList.begin(), inList.end(), synapseIndex), inList.end());
    
    // Mark as free (don't actually erase to keep indices stable)
    freeList_.push_back(synapseIndex);
    synapses_[synapseIndex].sourceNeuron = UINT64_MAX;  // Mark as invalid
    
    // Optional: Limit freeList size to prevent memory bloat
    if (freeList_.size() > 10000) {
        // Sort freeList to keep smallest indices first (better locality)
        std::sort(freeList_.begin(), freeList_.end());
        // Keep only first 10000 entries
        freeList_.resize(10000);
    }
}
```

## FIX 12: Checkpoint System - CheckpointSystem.hpp

### Problem:
CheckpointReader and CheckpointWriter lack proper resource cleanup in destructors.

### Solution:
Add explicit close() calls in destructors.

**For CheckpointReader (around line 183):**
```cpp
CheckpointReader::~CheckpointReader() {
    if (isOpen()) {
        close();
    }
}
```

**For CheckpointWriter (around line 250):**
```cpp
CheckpointWriter::~CheckpointWriter() {
    if (isOpen()) {
        // Try to close but don't throw exception in destructor
        try {
            close();
        } catch (...) {
            // Ignore exceptions in destructor
        }
    }
}
```

## FIX 13: Motor System - MotorSystem.cpp

### Problem:
MotorSystem uses raw pointers in actionHistory vector.

### Solution:
Replace raw pointers with unique_ptr.

**Current code (line 7):**
```cpp
std::vector<const Action*> actionHistory;
```

**Fixed code:**
```cpp
std::vector<std::unique_ptr<Action>> actionHistory;
```

**Update clearHistory() method (lines 49-54):**
```cpp
void MotorSystem::clearHistory() {
    // vector of unique_ptr will automatically delete contents
    pImpl->actionHistory.clear();
}
```

## FIX 14: Additional Cognitive Classes

Add proper destructors to all cognitive and planner classes:

**Example for NeuralPlanner.cpp (line 23):**
```cpp
NeuralPlanner::~NeuralPlanner() {
    delete pImpl;
}
```

**Example for ConceptFormation.cpp (line 25):**
```cpp
ConceptFormation::~ConceptFormation() {
    delete pImpl;
}
```

**Example for DevelopmentSystem.cpp (line 18):**
```cpp
DevelopmentSystem::~DevelopmentSystem() {
    delete pImpl;
}
```

## FIX 15: SmartPtr Implementation

**Create SmartPtr.cpp** with basic SmartPtr implementation:

**SmartPtr.cpp**:
```cpp
#include "SmartPtr.hpp"

template<typename T>
SmartPtr<T>::SmartPtr(T* ptr) : ptr_(ptr) {}

template<typename T>
SmartPtr<T>::~SmartPtr() {
    delete ptr_;
}

template<typename T>
T& SmartPtr<T>::operator*() {
    return *ptr_;
}

template<typename T>
T* SmartPtr<T>::operator->() {
    return ptr_;
}

// Include template implementation
#include "SmartPtr.inl"
```

**SmartPtr.hpp**:
```cpp
#pragma once

template<typename T>
class SmartPtr {
public:
    SmartPtr(T* ptr) : ptr_(ptr) {}
    ~SmartPtr() {
        delete ptr_;
    }
    
    T& operator*() { return *ptr_; }
    T* operator->() { return ptr_; }
    
private:
    T* ptr_;
};

// Include template implementation
#include "SmartPtr.inl"
```

## IMPLEMENTATION ORDER:

1. **Priority 1:** Fix Memory.cpp classes (WorkingMemory, EpisodicMemory, etc.)
2. **Priority 2:** Fix Brain.cpp and all neural/structural classes
3. **Priority 3:** Fix MemoryPool.hpp and SparseConnectivity.hpp
4. **Priority 4:** Fix CheckpointSystem.hpp
5. **Priority 5:** Fix MotorSystem.cpp
6. **Priority 6:** Implement SmartPtr.cpp (if needed)

## VERIFICATION:

After implementing fixes:
1. Run memory leak detection tools (valgrind, address sanitizer)
2. Verify all destructors properly delete pImpl
3. Test move constructors and assignment operators
4. Ensure no resource leaks in long-running simulations

All fixes should be implemented in the exact locations specified in this document.