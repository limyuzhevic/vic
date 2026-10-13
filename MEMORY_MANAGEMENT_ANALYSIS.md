# Memory Management Analysis Report

## Summary

Based on analysis of the NLM codebase, the following memory management issues were identified:

## 1. Manual Memory Allocations (new/delete) without Smart Pointers

The codebase extensively uses the pImpl pattern with manual memory management:

### Files using manual new/delete for pImpl:

**src/brain/Brain.cpp**
- Line 160: `pImpl(new Impl(config))` - Constructor
- Line 170: `delete pImpl;` - Move assignment operator
- Line 162: `~Brain() = default;` - Destructor (default, but pImpl is not cleaned up)

**src/brain/Neuron.cpp**
- Line 30: `pImpl(new Impl)` - Constructor
- Line 46: `delete pImpl;` - Move assignment operator
- Line 38: `~Neuron() = default;` - Destructor (default, but pImpl is not cleaned up)

**src/core/Random/Random.cpp**
- Line 13: `pImpl(new Impl(seed))` - Constructor
- Line 15: `pImpl(new Impl(std::random_device{}()))` - Constructor
- Line 25: `delete pImpl;` - Move assignment operator
- Line 17: `~RandomGenerator() = default;` - Destructor (default, but pImpl is not cleaned up)

**src/core/SimulationClock/SimulationClock.cpp**
- Line 17: `pImpl(new Impl(timestep))` - Constructor
- Line 27: `delete pImpl;` - Move assignment operator
- Line 19: `~SimulationClock() = default;` - Destructor (default, but pImpl is not cleaned up)

**src/brain/NeuralPopulation.cpp**
- Line 17: `pImpl(new Impl(id, size))` - Constructor
- Line 27: `delete pImpl;` - Move assignment operator

**src/brain/NeuralRegion.cpp**
- Line 19: `pImpl(new Impl(id))` - Constructor
- Line 33: `delete pImpl;` - Move assignment operator

**src/brain/Synapse.cpp**
- Line 50: `pImpl(new Impl)` - Constructor
- Line 73: `delete pImpl;` - Move assignment operator

**src/motor/MotorSystem.cpp**
- Line 24: `pImpl(new Impl)` - Constructor
- Line 51: `delete action;` - Manual delete in destructor

**src/dynamics/SpikeSystem.cpp**
- Line 21: `pImpl(new Impl)` - Constructor
- Line 31: `delete pImpl;` - Move assignment operator

## 2. Resource Management Issues

### File Handle Management

**src/core/Config/Config.cpp**
- Line 38: `std::ifstream file(filepath);` - File opened but not explicitly closed
- Line 104: `std::ofstream file(filepath);` - File opened but not explicitly closed
- Line 140: `std::ifstream file(filepath);` - File opened but not explicitly closed
- Line 146: `file.close();` - Explicit close, but relies on RAII fallback

**src/core/Logger/Logger.cpp**
- Line 13: `std::ofstream fileOut;` - File handle member, should be properly managed
- Line 74: `pImpl->fileOut.open(filepath, std::ios::app);` - File opened
- Line 79: `pImpl->fileOut.close();` - Explicit close available

**src/experiments/Phase3Experiment.cpp**
- Line 10: `std::ofstream file;` - File handle member
- Line 14: `file.open(filename);` - File opened
- Line 22: `file.close();` - Explicit close in destructor

**src/experiments/Phase3Demo.cpp**
- Line 9: `#include <fstream>` - File operations included
- Line 22: File operations in main function

## 3. Smart Pointer Usage Analysis

The codebase uses smart pointers extensively, which is good practice:

**src/brain/Brain.cpp**
- Line 26: `std::shared_ptr<Config> config;` - Proper shared pointer usage
- Line 27: `std::unique_ptr<RandomGenerator> rng;` - Proper unique pointer usage
- Line 28: `std::vector<std::unique_ptr<NeuralRegion>> regions;` - Proper unique pointer usage

**src/brain/NeuralRegion.cpp**
- Line 10: `std::vector<std::unique_ptr<NeuralPopulation>> populations;` - Proper
- Line 11: `std::vector<std::unique_ptr<Synapse>> synapses;` - Proper

The smart pointer usage appears to be correct and follows modern C++ practices.

## 4. Constructor/Destructor Patterns

### Issue: Default Destructors with Manual Allocations

Multiple classes use pImpl pattern with manual memory allocation but rely on default destructors:

**Classes affected:**
- `Brain` (Brain.cpp:162)
- `Neuron` (Neuron.cpp:38)
- `RandomGenerator` (Random.cpp:17)
- `SimulationClock` (SimulationClock.cpp:19)
- And approximately 20+ other classes

**Problem:** While the destructors are defaulted (which is efficient), the pImpl object allocated with `new` is never explicitly deleted, relying on the pImpl's own destructor to clean up.

## 5. Move Constructor/Move Assignment Issues

Some move operations have manual delete patterns:

**src/brain/Neuron.cpp**
- Line 46: `delete pImpl;` - Manual delete in move assignment

**src/core/Random/Random.cpp**
- Line 25: `delete pImpl;` - Manual delete in move assignment

## 6. Potential Memory Leak Concerns

**Phase 6 Integrated Experiment (src/experiments/Phase6IntegratedExperiment.cpp)**
- Line 134: `// Create new brain and load` - Comment suggests dynamic creation
- Line 347: `// Create new brain and load` - Comment suggests dynamic creation

## Recommendations

### 1. Replace Manual pImpl with Smart Pointers

Replace `pImpl(new Impl(...))` with `pImpl(std::make_unique<Impl>(...))`:

**Before:**
```cpp
struct Brain::Impl { /* ... */ };
Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}
```

**After:**
```cpp
struct Brain::Impl { /* ... */ };
Brain::Brain(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(config)) {}
```

### 2. Ensure Proper Resource Cleanup

- Use `std::ifstream` and `std::ofstream` with RAII (let them go out of scope)
- Ensure `closeOutputFile()` is called in Logger when appropriate
- Consider using `std::filesystem::temp_directory_path()` for temporary files

### 3. Implement Proper Destructors

For classes using pImpl with manual allocation:
```cpp
Brain::~Brain() {
    // pImpl will be automatically deleted when it goes out of scope
    // No need for explicit delete with smart pointers
}
```

### 4. Memory Leak Detection

Consider adding memory leak detection for long-running simulations:
- Use tools like `valgrind`, ` AddressSanitizer`, or `MemorySanitizer`
- Add memory profiling for Phase 6 experiments

## Files Requiring Immediate Attention

1. **src/brain/Brain.cpp** - ~1117 lines of code with pImpl pattern
2. **src/brain/Neuron.cpp** - ~302 lines, manual memory management
3. **src/core/Random/Random.cpp** - ~134 lines, manual memory management
4. **src/core/SimulationClock/SimulationClock.cpp** - ~110 lines, manual memory management
5. **src/core/Logger/Logger.cpp** - ~197 lines, file handle management
6. **src/core/Config/Config.cpp** - ~566 lines, file operations

## Conclusion

The codebase has significant opportunities for memory management improvements:
- Replace approximately 30+ manual `new` allocations with smart pointers
- Improve resource cleanup patterns for file handles
- Add proper destructor implementations
- Consider memory leak detection for long-running experiments

The smart pointer usage is generally good, but the pImpl manual allocation pattern needs modernization.
