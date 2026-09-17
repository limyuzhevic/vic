# NLM Code Refactoring and Bug Fix Plan

## Critical Issues to Fix Immediately

### 1. Random Generator Null Pointer Initialization
**File**: src/brain/Brain.cpp
**Lines**: 81-101
**Risk**: `rng(nullptr)` causes undefined behavior

**Current Code**:
```cpp
Impl(std::shared_ptr<Config> cfg)
    : config(cfg)
    , rng(nullptr)  // BUG: Not initialized
    , developmentalStage(DevelopmentalStage::Initial)
    // ... other members
{
    // Initialize random generator with seed from config
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = config->get<uint64_t>("random_seed")) {
        seed = *seedOpt;
    }
    rng = std::make_unique<RandomGenerator>(seed);  // FIXED: Properly initialized
}