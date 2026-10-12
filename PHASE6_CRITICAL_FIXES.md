# NLM Phase 6 Critical Integration Issues - Quick Fix Guide

## IMMEDIATE FIXES NEEDED (Priority 1)

### Issue 1: Memory Systems Returning nullptr
**Files to Fix**:
- `src/brain/Brain.cpp` - Lines 1008-1018: Memory system accessors need to return actual pointers
- `src/brain/Brain.cpp` - Lines 1020-1024: Prediction system accessor needs implementation
- `src/brain/Brain.cpp` - Lines 1026-1038: Cognition system accessors need implementation

**Expected**: 
```cpp
NeuralWorkingMemory* Brain::getWorkingMemory() { return pImpl->workingMemory.get(); } // Return actual, not nullptr
```

### Issue 2: Cognitive Systems Not Instantiated
**Files to Fix**:
- `src/brain/Brain.cpp` - Lines 119-122: Cognitive systems exist but are never used
- `src/brain/Brain.cpp` - Lines 519-533: Attention and concept formation frameworks exist but are stubs

**Evidence**: In Phase 6 audit, all cognitive systems show "❌ Not used"

### Issue 3: Prediction System Disconnected  
**Files to Fix**:
- `src/brain/Brain.cpp` - Lines 512-516: Prediction system update is "For now, just track prediction error history"
- `src/brain/Brain.cpp` - Lines 513-516: Should update with sensory observations

**Current**: Just empty stub, never processes sensory input

## HIGH PRIORITY FIXES (Priority 2)

### Issue 4: Neuromodulation Incomplete
**Files to Fix**:
- `src/neuromodulation/Neuromodulator.hpp`: 4 PLACEHOLDERs for ACh, NE, 5-HT
- `src/neuromodulation/Neuromodulator.cpp`: Basic dopamine only

**Status**: Only dopamine works, others are stubs

### Issue 5: Development Integration Minimal
**Files to Fix**:
- `src/development/DevelopmentSystem.hpp`: No implementation shown
- `src/brain/Brain.cpp` - Lines 549-577: Only affects structural plasticity rates

**Missing**: No effects on neural excitability, plasticity rules, attention, memory

### Issue 6: Episodic Memory Storage Issue
**Files to Fix**:
- `src/brain/Brain.cpp` - Lines 482-510: Episodic memory stores but may not integrate with other systems
- `Phase6IntegratedExperiment.cpp` - Lines 108-109: Checks if episodic memory returns nullptr

## MODERATE PRIORITY (Priority 3)

### Issue 7: Performance Infrastructure Not Integrated
**Files to Fix**:
- `src/performance/` - Various performance components exist but are not integrated into Brain
- `src/brain/Brain.cpp`: No use of memory pools, event queues, SIMD

### Issue 8: RESTART/TEST ISSUES
**Files to Check**:
- Build system may not have Phase 6 demo executable
- Phase 6 integration test may not actually verify integration properly

## QUICK ACTION ITEMS

### 1. Fix Brain.cpp Integration (Immediate)
```bash
# Fix memory system accessors - they exist but return nullptr
# Update cognitive system implementations to actually be used
# Connect prediction system to sensory processing
```

### 2. Update Integration Tests (Immediate)
```bash
# Fix Phase6Demo.cpp - integration verification should test actual functionality
# Update Phase6IntegratedExperiment.cpp - tests should verify system connections
```

### 3. Complete Neuromodulation (Next)
```bash
# Implement serotonin, norepinephrine, acetylcholine
# Connect neuromodulators to memory consolidation
# Add neuromodulation effects on attention
```

### 4. Complete Development System (Next)
```bash
# Implement DevelopmentSystem.hpp.cpp properly
# Make development affect all brain systems, not just plasticity
```

### 5. Integration Verification (Final)
```bash
# Ensure all TODO PHASE 2 comments are addressed for Phase 6
# Verify: working memory ↔ attention ↔ concept formation ↔ planning ↔ action selection
```

## CRITICAL MISSING PIECES

1. **What actually works in Phase 6**:
   - LIF neurons with real spiking ✅
   - Synaptic plasticity (STDP, Hebbian, structural) ✅  
   - Sensory-motor loop ✅
   - Reward modulation ✅
   - Development stages ✅
   - Checkpoint system ✅
   - Replay mechanism ✅
   - Episodic memory storage ✅

2. **What doesn't work in Phase 6**:
   - Working memory integration ❌ (returns nullptr in tests)
   - Prediction system integration ❌ (stub only)
   - Cognitive systems integration ❌ (never instantiated)
   - Full neuromodulation ❌ (3/4 missing)
   - Memory consolidation ❌ (partial)
   - True developmental effects ❌ (minimal)

**Bottom Line**: The brain has the **anatomy** of a cognitive system but lacks the **integration** that makes it function as a coherent artificial brain. Fix Phase 6 integration, don't add more disconnected components.