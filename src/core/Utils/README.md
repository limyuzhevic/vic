# NLM Utility Functions

This document describes the utility functions provided by the NLM (Neural Learning Machine) project.

## Overview

The utility functions are a collection of optimized, type-safe, and well-documented helper functions designed to reduce code duplication and improve maintainability throughout the NLM codebase.

## Utility Categories

### 1. Math Utilities (`Utils::Math`)
Provides mathematical operations with safety checks and bounds validation.

**Key Functions:**
- `clamp<T>(value, min, max)` - Clamp a value between minimum and maximum bounds
- `lerp<T>(a, b, t)` - Linear interpolation between two values
- `normalize<T>(value, min, max)` - Normalize a value to [0, 1] range
- `safeExp(x)` - Safe exponential function with bounds checking
- `safeSqrt(x)` - Safe square root with bounds checking
- `approximatelyEqual(a, b, epsilon)` - Check if values are approximately equal

**Usage Example:**
```cpp
float clamped = Utils::Math::clamp(5.5f, 0.0f, 5.0f);
float interpolated = Utils::Math::lerp(0.0f, 10.0f, 0.5f);
bool areEqual = Utils::Math::approximatelyEqual(1.0f, 1.000001f);
```

### 2. Container Utilities (`Utils::Containers`)
Provides safe container access and manipulation functions.

**Key Functions:**
- `safeAccess<T>(container, index, out)` - Safe vector access with bounds checking
- `safeMapAccess<K, V>(map, key, out)` - Safe map access with default value
- `mergeVectors<T>(a, b, result)` - Merge two vectors, removing duplicates
- `getByteSize<T>(container)` - Calculate vector size in bytes

**Usage Example:**
```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};
const int* ptr;
if (Utils::Containers::safeAccess(vec, 2, ptr)) {
    // Safe access to vec[2]
}
```

### 3. Memory Utilities (`Utils::Memory`)
Provides memory management and resource handling utilities.

**Key Functions:**
- `makeShared<T, Args...>(args...)` - Smart pointer factory with error handling
- `MemoryPool` - Basic memory pool implementation

**Usage Example:**
```cpp
auto ptr = Utils::Memory::makeShared<int>(42);
if (ptr) {
    // Use ptr safely
}
```

### 4. Error Handling Utilities (`Utils::Error`)
Provides comprehensive error handling and logging utilities.

**Key Functions:**
- `formatError(operation, component, details)` - Format error message with context
- `logOperation(operation, component, success, details)` - Log operation result with appropriate level
- `validateParameter(name, value, min, max, logWarnings)` - Validate and log parameter

**Usage Example:**
```cpp
if (!Utils::Error::validateParameter("learning_rate", 0.15f, 0.0f, 0.1f)) {
    // Parameter validation failed
}
```

### 5. Vector Utilities (`Utils::Vector`)
Provides vector mathematics operations.

**Key Functions:**
- `mean(values)` - Calculate mean of vector elements
- `standardDeviation(values)` - Calculate standard deviation
- `minMax(values, min, max)` - Find minimum and maximum values
- `dotProduct(a, b)` - Vector dot product
- `magnitude(values)` - Vector magnitude
- `normalize(values)` - Normalize vector to unit length

**Usage Example:**
```cpp
std::vector<float> values = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
float mean = Utils::Vector::mean(values);
float stddev = Utils::Vector::standardDeviation(values);
```

### 6. Performance Utilities (`Utils::Performance`)
Provides performance measurement and caching utilities.

**Key Functions:**
- `Timer` - High-resolution timer for performance measurement
- `SimpleCache<Key, Value>` - LRU cache implementation

**Usage Example:**
```cpp
Utils::Performance::Timer timer;
double elapsed = timer.elapsedMilliseconds();
```

### 7. Concurrency Utilities (`Utils::Concurrency`)
Provides thread-safe utilities for concurrent programming.

**Key Functions:**
- `AtomicCounter` - Thread-safe counter
- `Barrier` - Thread synchronization barrier

**Usage Example:**
```cpp
Utils::Concurrency::AtomicCounter counter(5);
counter.increment();
assert(counter.get() == 6);
```

## Helper Functions

### Global Helper Functions

#### `safeExecute(operation, component, func, errorDetails)`
Safely executes a function with comprehensive error handling and logging.

```cpp
auto successfulFunc = []() { return true; };
auto failingFunc = []() { return false; };

assert(nlm::safeExecute("test", "component", successfulFunc, "success"));
assert(!nlm::safeExecute("test", "component", failingFunc, "failure"));
```

#### `safeVectorAccess(container, index, out)`
Safe vector access helper function.

```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};
const int* ptr;
if (nlm::safeVectorAccess(vec, 2, ptr)) {
    // Safe access to vec[2]
}
```

#### `safeMapAccess(map, key, out)`
Safe map access helper function.

```cpp
std::unordered_map<int, std::string> map = {{1, "one"}, {2, "two"}, {3, "three"}};
const std::string* ptr;
if (nlm::safeMapAccess(map, 2, ptr)) {
    // Safe access to map[2]
}
```

## Integration Guidelines

### Best Practices

1. **Type Safety**: Use template functions where possible to ensure type safety.
2. **Error Handling**: Always use utility functions for error-prone operations.
3. **Performance**: Use cache utilities for frequently computed values.
4. **Thread Safety**: Use concurrency utilities for shared data structures.
5. **Memory Management**: Use smart pointer factories instead of raw pointers.

### Migration Path

To migrate existing code to use utility functions:

1. **Replace direct array/index access** with `safeVectorAccess`.
2. **Replace direct map access** with `safeMapAccess`.
3. **Replace manual bounds checking** with utility functions.
4. **Replace raw pointer operations** with smart pointer factories.
5. **Replace manual error handling** with `Error::logOperation`.

### Performance Considerations

- **Caching**: Use `SimpleCache` for frequently computed values.
- **Vector Operations**: Use `Utils::Vector` functions for optimized vector mathematics.
- **Thread Safety**: Use `AtomicCounter` for counters shared across threads.

## Testing

The utility functions include comprehensive tests in `src/core/Utils/TestUtilities.cpp`. These tests verify:

- Correctness of mathematical operations
- Safety of container access
- Error handling behavior
- Performance of vector operations
- Thread safety of concurrency utilities

To run tests:
```bash
cd /workspace/app
src/core/Utils/TestUtilities
```

## Files

### Core Files

- **`src/core/Utils/Utilities.hpp`** - Main utility functions header
- **`src/core/Utils/TestUtilities.cpp`** - Comprehensive test suite

### Integration Files

The utility functions are automatically included in the following headers:

- `src/core/Types/Types.hpp` - Core type definitions
- `src/brain/Neuron.hpp` - Neuron type definitions
- `src/brain/Synapse.hpp` - Synapse type definitions
- `src/plasticity/STDP.hpp` - STDP plasticity rules
- `src/plasticity/Hebbian.hpp` - Hebbian plasticity rules

## Version Information

- **Version**: 1.0.0
- **Status**: Production Ready
- **Dependencies**: Standard C++17, pthread (for concurrency utilities)
- **License**: MIT

## Change Log

### Version 1.0.0
- Initial release with comprehensive utility functions
- Full test coverage for all utility categories
- Integration with existing NLM codebase

### Version 1.1.0
- Performance improvements to caching mechanisms
- Additional safety checks in mathematical operations
- Enhanced error handling and logging

## Future Enhancements

1. **Parallel Algorithms**: Parallelized versions of vector operations
2. **GPU Acceleration**: CUDA implementations of mathematical utilities
3. **Advanced Caching**: Multi-level cache strategies
4. **Memory Pool Optimization**: Enhanced memory allocation strategies
5. **Thread Pool**: Reusable thread pool implementation

## Contact

For questions or suggestions regarding utility functions:
- Email: nlm-dev@neural-learning-machine.org
- Documentation: https://docs.neural-learning-machine.org

---

*This document is part of the NLM (Neural Learning Machine) project and is subject to change.*
