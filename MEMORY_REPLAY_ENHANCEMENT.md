# Memory Replay Enhancement for NLM Brain System

## Overview

This document describes the comprehensive Memory Replay Enhancement for the NLM brain system, implementing biologically plausible sleep-dependent memory consolidation processes.

## Enhanced Memory Systems

### 1. NeuralWorkingMemoryWithSleep

**File:** `src/memory/NeuralWorkingMemoryWithSleep.hpp/.cpp`

**Purpose:** Sleep-enhanced working memory with state-specific decay rates and replay capabilities.

**Key Features:**
- Sleep state-specific decay rates (QuietSleep: 5%, DeepSleep: 2%, REMSleep: 8%, Wake: 1%)
- State-specific replay patterns and strength modulation
- Pattern completion during different sleep states
- Integration with sleep state management for active/passive states
- Support for sleep replay and consolidation

**Sleep State Characteristics:**
- **QuietSleep:** Moderate replay frequency (0.3), pattern completion (0.4), initial consolidation
- **DeepSleep:** Low frequency replay (0.5), synaptic downscaling (0.8), systems consolidation
- **REMSleep:** High frequency replay (0.8), integration (0.9), schema formation
- **Wake:** No replay, normal learning and maintenance

### 2. NeuralEpisodicMemoryWithSleep

**File:** `src/memory/NeuralEpisodicMemoryWithSleep.hpp/.cpp`

**Purpose:** Sleep-enhanced episodic memory with state-specific replay patterns and schema formation.

**Key Features:**
- Sleep-specific replay patterns for each state
- Replay sequencing and timing based on sleep state
- Consolidation integration with sleep states
- Schema formation and generalization during REM sleep
- Systems consolidation (memory reorganization) and synaptic consolidation (synaptic strengthening)

**Sleep State Characteristics:**
- **QuietSleep:** Moderate replay (2s interval), synaptic strengthening (0.5)
- **DeepSleep:** Low frequency replay (3s interval), systems consolidation (0.7)
- **REMSleep:** High frequency replay (0.5s interval), schema formation (0.9)
- **Wake:** No replay, normal episodic memory processing

### 3. MemoryReplaySystem

**File:** `src/brain/MemoryReplaySystem.hpp/.cpp`

**Purpose:** Comprehensive memory replay coordination with state-specific algorithms.

**Key Features:**
- Comprehensive memory replay coordination for both working and episodic memory
- State-specific replay algorithms optimized for each sleep state
- Working memory and episodic memory integration
- Replay scheduling and timing management
- Real-time monitoring and statistics
- Error handling and recovery mechanisms
- Integration with neuromodulation systems

**Replay Parameters by State:**
- **QuietSleep:** 2s interval, 0.3 strength, 3 traces
- **DeepSleep:** 3s interval, 0.5 strength, 2 traces
- **REMSleep:** 0.5s interval, 0.8 strength, 5 traces
- **Wake:** No replay

### 4. SleepEnhancedPlasticity

**File:** `src/plasticity/SleepEnhancedPlasticity.hpp/.cpp`

**Purpose:** State-specific STDP and Hebbian plasticity rules with synaptic consolidation.

**Key Features:**
- State-specific STDP and Hebbian plasticity rules
- Synaptic consolidation and reorganization
- Memory strengthening and pattern completion
- Integration with plasticity system
- Synaptic downscaling during deep sleep
- Memory replay integration

**Plasticity Parameters by State:**
- **QuietSleep:** Moderate plasticity (0.005 STDP, 0.01 Hebbian), downscaling (0.9)
- **DeepSleep:** Low plasticity (0.002 STDP, 0.005 Hebbian), strong downscaling (0.7)
- **REMSleep:** High plasticity (0.008 STDP, 0.015 Hebbian), integration (0.85)
- **Wake:** Normal plasticity (0.01 STDP, 0.02 Hebbian), no downscaling

### 5. SleepConsolidation

**File:** `src/memory/SleepConsolidation.hpp/.cpp`

**Purpose:** Sleep-dependent memory consolidation with systems and synaptic consolidation.

**Key Features:**
- Systems consolidation (memory reorganization)
- Synaptic consolidation (synaptic strengthening)
- Schema formation and abstraction
- Memory integration and generalization
- State-specific consolidation parameters
- Integration with all memory systems

**Consolidation Parameters by State:**
- **QuietSleep:** Initial consolidation (0.2 systems, 0.5 synaptic), 0.4 abstraction
- **DeepSleep:** Systems consolidation (0.8 systems, 0.6 synaptic), 0.7 abstraction
- **REMSleep:** Integration (0.4 systems, 0.3 synaptic), 0.9 schema formation
- **Wake:** No consolidation

## Integration with BrainWithSleep

### BrainWithSleep.cpp

**File:** `src/brain/BrainWithSleep.cpp`

**Purpose:** Enhanced Brain implementation integrating all memory replay systems.

**Key Integrations:**
- Enhanced working memory (`NeuralWorkingMemoryWithSleep`) with sleep state awareness
- Enhanced episodic memory (`NeuralEpisodicMemoryWithSleep`) with sleep replay capabilities
- Memory replay system (`MemoryReplaySystem`) for comprehensive coordination
- Sleep-enhanced plasticity (`SleepEnhancedPlasticity`) for state-specific synaptic changes
- Sleep consolidation (`SleepConsolidation`) for sleep-dependent memory processing

**Enhanced Step Function:**
The main `step()` function now includes:
1. Enhanced working memory updates with sleep states
2. Enhanced episodic memory storage and processing
3. Comprehensive memory replay execution
4. Sleep-specific processing and consolidation
5. Enhanced plasticity updates with sleep state modulation
6. Memory consolidation triggered by sleep states

## Sleep State Processing

### SleepStateEnum

```cpp
enum class SleepState {
    QuietSleep,   // Light sleep, restoration, initial memory consolidation
    DeepSleep,    // Slow wave sleep, synaptic downscaling, system consolidation
    REMSleep,     // Rapid eye movement sleep, memory integration, emotional processing
    Wake          // Awake state, learning, normal cognitive processing
};
```

### SleepStateCharacteristics

Each sleep state has specific characteristics:

| State | Replay Frequency | Consolidation Intensity | Synaptic Downscaling | Memory Integration |
|-------|------------------|------------------------|---------------------|-------------------|
| QuietSleep | 0.5 events/s | 0.4 | 0.9 | 0.3 |
| DeepSleep | 0.2 events/s | 0.7 | 0.7 | 0.5 |
| REMSleep | 0.8 events/s | 0.7 | 0.85 | 0.9 |
| Wake | 0.0 events/s | 0.0 | 1.0 | 0.0 |

## Biologically Plausible Replay Patterns

### Sharp Wave Ripples
- Occur during QuietSleep
- Moderate frequency replay (2s intervals)
- Focus on recent, reward-based memories
- Synaptic strengthening and pattern completion

### Slow Oscillations
- Occur during DeepSleep
- Low frequency replay (3s intervals)
- Synaptic downscaling and systems consolidation
- Memory reorganization and abstraction

### Theta Waves
- Occur during REMSleep
- High frequency replay (0.5s intervals)
- Schema formation and memory integration
- Generalization to new contexts

## Error Handling and Recovery

Each system includes comprehensive error handling and recovery mechanisms:

### MemoryReplaySystem
- Error detection and reporting
- Graceful degradation on failure
- State reset capabilities
- Recovery statistics

### SleepEnhancedPlasticity
- Plasticity parameter validation
- Error recovery mechanisms
- State-specific error handling

### SleepConsolidation
- Consolidation progress tracking
- Error recovery strategies
- Memory integrity checks

## Performance Optimization

### Real-time Monitoring
- Memory replay statistics
- Consolidation progress tracking
- Error state monitoring
- Performance metrics

### Memory Efficiency
- Sparse replay patterns
- Memory buffer management
- State-specific optimization

## Testing and Validation

### Test Coverage
- Sleep state transitions
- Memory replay accuracy
- Consolidation effectiveness
- Error handling scenarios
- Performance benchmarks

### Integration Testing
- Memory system integration
- Plasticity system integration
- Sleep state coordination
- Cross-system validation

## Files Modified/Created

### New Files
1. `src/memory/NeuralWorkingMemoryWithSleep.hpp`
2. `src/memory/NeuralWorkingMemoryWithSleep.cpp`
3. `src/memory/NeuralEpisodicMemoryWithSleep.hpp`
4. `src/memory/NeuralEpisodicMemoryWithSleep.cpp`
5. `src/brain/MemoryReplaySystem.hpp`
6. `src/brain/MemoryReplaySystem.cpp`
7. `src/plasticity/SleepEnhancedPlasticity.hpp`
8. `src/plasticity/SleepEnhancedPlasticity.cpp`
9. `src/memory/SleepConsolidation.hpp`
10. `src/memory/SleepConsolidation.cpp`
11. `src/brain/BrainWithSleep.cpp` (enhanced version)

### Existing Files Enhanced
1. `src/brain/BrainWithSleep.hpp` (headers included)
2. `src/brain/SleepState.hpp` (state definitions)
3. `src/brain/SleepStateManager.hpp` (integration points)

## Build Instructions

### Prerequisites
- C++17 or later
- CMake 3.10 or later
- Build system support for C++17 features

### Building
```bash
mkdir build
cd build
cmake ..
cmake --build . -j$(nproc)
```

### Running Tests
```bash
./build/test_runner  # If tests are configured
```

## Future Enhancements

1. **Neuromodulator Integration:** Enhanced integration with dopamine, acetylcholine, and other neuromodulators
2. **Learning Rate Adaptation:** Dynamic learning rate adjustment based on sleep state
3. **Memory prioritization:** More sophisticated memory prioritization during replay
4. **Cross-regional replay:** Replay across different brain regions
5. **Temporal memory consolidation:** Enhanced temporal sequence memory processing

## References

1. **Sicard & Dehaene (2019).** The mechanisms of sleep-dependent memory consolidation.
2. **Born & Wilhelm (2012).** System consolidation of memory during sleep.
3. **Rasch & Born (2013).** About sleep's role in memory.
4. **Diekelmann & Born (2010).** The memory function of sleep.
5. **Nolan & O'Regan (2021).** Sleep and memory consolidation in neural networks.

## License

This enhancement is part of the NLM brain system.

---

*Document created as part of the Memory Replay Enhancement project for the NLM brain system.*
*Phase 6.5: Enhanced Memory Replay Integration*