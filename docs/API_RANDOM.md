# Random Number Generator API Documentation

## Overview
The Random Number Generator provides deterministic, thread-safe random number generation for NLM simulations. It supports multiple probability distributions and ensures reproducible results across runs.

## Key Features
- **Deterministic**: Explicit seeding for reproducible experiments
- **Thread-Safe**: Atomic operations for concurrent access
- **Multiple Distributions**: Uniform, normal, Poisson, exponential, Bernoulli
- **High Performance**: Direct access to underlying mt19937_64 engine
- **Flexible Interface**: Template-based operations for containers
- **Benchmark Ready**: Suitable for performance-critical applications

## Usage Examples

### Basic Random Number Generation
```cpp
#include <nlm/core/Random.hpp>

int main() {
    // Create with explicit seed for reproducibility
    nlm::RandomGenerator rng(42);  // Always produces same sequence
    
    // Create with time-based seed (not reproducible)
    nlm::RandomGenerator rng2;  // Different each run
    
    // Uniform integers
    int value1 = rng.uniformInt(1, 100);        // [1, 100]
    long value2 = rng.uniformInt(0, 1000000);    // [0, 1000000]
    
    // Uniform reals
    double d1 = rng.uniformReal(0.0, 1.0);      // [0.0, 1.0)
    float f1 = rng.uniformReal(0.5f, 2.5f);     // [0.5f, 2.5f)
    
    // Normal distribution
    double normal = rng.normal(0.0, 1.0);       // Mean=0, StdDev=1
    float normalf = rng.normal(100.0f, 15.0f);  // For float operations
    
    // Bernoulli trials
    bool coinFlip = rng.bernoulli(0.5);         // 50% chance true
    bool biased = rng.bernoulli(0.95);          // 95% chance true
    
    // Other distributions
    int poisson = rng.poisson(5.0);             // Poisson(5)
    double exponential = rng.exponential(2.0); // Exp(2)
    
    return 0;
}
```

### Advanced Usage
```cpp
#include <nlm/core/Random.hpp>
#include <vector>
#include <algorithm>
#include <numeric>

int main() {
    nlm::RandomGenerator rng(12345);
    
    // Shuffle containers
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    rng.shuffle(numbers);  // Random order
    
    // Pick random elements
    int chosen = rng.pickRandom(numbers);  // Returns random element
    
    // Pick random index
    int index = rng.pickRandomIndex(numbers.size());  // [0, size-1]
    
    // Fill buffer with random bytes
    std::vector<uint8_t> buffer(1024);
    rng.fillBytes(buffer.data(), buffer.size());
    
    // Get underlying engine for advanced use
    auto& engine = rng.engine();  // Direct access to mt19937_64
    
    // Compatible with std algorithms
    std::random_shuffle(numbers.begin(), numbers.end(), 
                       [&rng](int n) { return rng.randomBit(); });
    
    // Custom distribution
    std::uniform_int_distribution<int> dist(10, 20);
    int custom = dist(rng.engine());  // Using underlying engine
    
    return 0;
}
```

### Thread-Safe Usage
```cpp
#include <nlm/core/Random.hpp>
#include <thread>
#include <vector>

void workerThread(nlm::RandomGenerator& rng, std::vector<int>& results, size_t id) {
    for (int i = 0; i < 100; ++i) {
        // Each thread safely generates random numbers
        int value = rng.uniformInt(0, 1000);
        results.push_back(value);
        
        // Small delay
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
}

int main() {
    nlm::RandomGenerator rng(42);  // Shared with proper thread safety
    
    std::vector<std::thread> threads;
    std::vector<std::vector<int>> allResults(4);
    
    // Create multiple threads that share the RNG
    for (size_t i = 0; i < 4; ++i) {
        threads.emplace_back(workerThread, std::ref(rng), std::ref(allResults[i]), i);
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    // All threads got thread-safe random numbers
    return 0;
}
```

### Reproducible Experiments
```cpp
#include <nlm/core/Random.hpp>
#include <nlm/core/Config.hpp>

// Common random seed for experiment reproducibility
const uint64_t DEFAULT_SEED = 123456789;

void runExperiment(nlm::RandomGenerator& rng, int experimentId) {
    // Same seed produces same results across runs
    std::cout << "Experiment " << experimentId << " results:" << std::endl;
    
    for (int i = 0; i < 5; ++i) {
        double result = rng.uniformReal(0, 1);
        std::cout << "  Trial " << i << ": " << result << std::endl;
    }
}

int main() {
    // Option 1: Explicit seed
    nlm::RandomGenerator rng1(42);
    runExperiment(rng1, 1);
    
    // Option 2: From configuration (for reproducible experiments)
    nlm::Config config;
    config.set("experiment.seed", DEFAULT_SEED);
    
    if (auto seed = config.get<uint64_t>("experiment.seed")) {
        nlm::RandomGenerator rng2(*seed);
        runExperiment(rng2, 2);
    }
    
    // Option 3: Deterministic across program runs
    nlm::RandomGenerator rng3(DEFAULT_SEED);
    runExperiment(rng3, 3);
    
    return 0;
}
```

## Thread Safety Guarantees

The RandomGenerator class provides the following thread safety guarantees:

1. **Seed Operations**: `seed()` and `getSeed()` are thread-safe
2. **Random Number Generation**: All generation methods (`uniformInt`, `uniformReal`, `normal`, etc.) are thread-safe
3. **Atomic Operations**: Internal seed state is stored in atomic variables
4. **Mutex Protection**: All operations are protected by mutexes
5. **Container Operations**: `shuffle()`, `pickRandom()`, etc. are thread-safe

## Performance Considerations

### High-Performance Usage
```cpp
#include <nlm/core/Random.hpp>
#include <chrono>

int main() {
    nlm::RandomGenerator rng(42);
    
    // Benchmark: Generate many random numbers
    auto start = std::chrono::high_resolution_clock::now();
    
    const int N = 1000000;
    int sum = 0;
    for (int i = 0; i < N; ++i) {
        sum += rng.uniformInt(0, 100);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Generated " << N << " random numbers in " 
              << duration.count() << " ms" << std::endl;
    std::cout << "Average: " << (double)sum / N << std::endl;
    
    return 0;
}
```

### Engine Access for Advanced Use
```cpp
#include <nlm/core/Random.hpp>
#include <random>

int main() {
    nlm::RandomGenerator rng(42);
    
    // Direct access to mt19937_64 engine
    auto& engine = rng.engine();
    
    // Use with custom distributions
    std::mt19937 tempEngine;  // Different engine
    std::normal_distribution<double> dist(100, 15);
    
    // Seed the temp engine from our generator
    tempEngine.seed(rng.getSeed());
    
    // Use with temp engine
    for (int i = 0; i < 10; ++i) {
        double value = dist(tempEngine);
        std::cout << value << " ";
    }
    
    return 0;
}
```

## API Reference

### Class: RandomGenerator

#### Constructors/Destructors
- `explicit RandomGenerator(uint64_t seed)` - Create with explicit seed
- `RandomGenerator()` - Create with time-based seed
- `~RandomGenerator()` - Destructor

#### Copy/Move Operations
- `RandomGenerator(const RandomGenerator&) = delete` - No copying
- `RandomGenerator& operator=(const RandomGenerator&) = delete` - No assignment
- `RandomGenerator(RandomGenerator&&) noexcept` - Move constructor
- `RandomGenerator& operator=(RandomGenerator&&) noexcept` - Move assignment

#### Seed Operations
- `void seed(uint64_t seed)` - Set seed (thread-safe)
- `uint64_t getSeed() const` - Get current seed (thread-safe)

#### Random Number Generation
##### Uniform Integers
- `uint32_t uniformInt(uint32_t min, uint32_t max)` - `[min, max]`
- `uint64_t uniformInt(uint64_t min, uint64_t max)` - `[min, max]`

##### Uniform Reals
- `double uniformReal(double min, double max)` - `[min, max)`
- `float uniformReal(float min, float max)` - `[min, max)`

##### Normal Distribution
- `double normal(double mean, double stddev)` - Normal distribution
- `float normal(float mean, float stddev)` - Normal distribution (float)

##### Discrete Distributions
- `bool bernoulli(double p = 0.5)` - Bernoulli trials
- `uint32_t poisson(double lambda)` - Poisson distribution
- `double exponential(double lambda)` - Exponential distribution

##### Convenience Methods
- `bool randomBool()` - Returns `bernoulli(0.5)`
- `uint32_t randomBit()` - Returns `uniformInt(0, 1)`

#### Container Operations
##### Template Methods
- `template<typename Container> void shuffle(Container& container)`
- `template<typename Container> auto pickRandom(const Container& container) -> typename Container::const_reference`
- `uint64_t pickRandomIndex(uint64_t n)` - Pick index from `0` to `n-1`

#### Advanced Operations
- `void fillBytes(void* data, size_t size)` - Fill buffer with random bytes
- `std::mt19937_64& engine()` - Get reference to underlying engine
- `const std::mt19937_64& engine() const` - Get const reference to engine

#### C++ Standard Library Compatibility
- `result_type min()` - `std::mt19937_64::min()`
- `result_type max()` - `std::mt19937_64::max()`
- `result_type operator()()` - Compatibility with `std::mt19937` interface

## Error Handling

The RandomGenerator throws `std::invalid_argument` in the following cases:

1. `uniformInt(min, max)` when `min > max`
2. `uniformReal(min, max)` when `min >= max`
3. `normal(mean, stddev)` with negative `stddev`
4. `poisson(lambda)` with negative `lambda`
5. `exponential(lambda)` with negative `lambda`
6. `bernoulli(p)` with `p < 0 || p > 1`
7. `pickRandomIndex(n)` when `n == 0`
8. `pickRandom(const Container& container)` for empty containers

All exceptions are documented in the API reference.

## Performance Optimization

### Pre-instantiation Pattern
```cpp
// Best practice: instantiate once
class RandomSingleton {
public:
    static nlm::RandomGenerator& getInstance() {
        static nlm::RandomGenerator rng(42);
        return rng;
    }
};

// Use in hot loops
void hotFunction() {
    auto& rng = RandomSingleton::getInstance();
    for (int i = 0; i < 1000; ++i) {
        // Reuse the same RNG
        double value = rng.uniformReal(0, 1);
        // ... do work
    }
}
```

### Engine Reuse Pattern
```cpp
// For performance-critical applications
void highPerformanceFunction() {
    // Create your own engine for maximum control
    std::mt19937_64 engine(42);
    
    // Use with NLM's interface
    nlm::RandomGenerator rng(engine); // If constructor supports it
    
    // Or use directly with std algorithms
    std::uniform_real_distribution<double> dist(0, 1);
    for (int i = 0; i < 1000000; ++i) {
        double value = dist(engine);
        // Process value...
    }
}
```

## Testing and Validation

### Unit Testing
```cpp
#include <nlm/core/Random.hpp>
#include <gtest/gtest.h>

TEST(RandomGeneratorTest, Deterministic) {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(42);
    
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(rng1.uniformInt(0, 1000), rng2.uniformInt(0, 1000));
    }
}

TEST(RandomGeneratorTest, DifferentSeeds) {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(43);
    
    bool different = false;
    for (int i = 0; i < 100; ++i) {
        if (rng1.uniformInt(0, 1000) != rng2.uniformInt(0, 1000)) {
            different = true;
            break;
        }
    }
    EXPECT_TRUE(different);
}
```

### Thread Safety Testing
```cpp
#include <nlm/core/Random.hpp>
#include <thread>
#include <vector>
#include <mutex>

TEST(RandomGeneratorTest, ThreadSafety) {
    nlm::RandomGenerator rng(42);
    std::vector<int> results(1000);
    std::mutex mutex;
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < 100; ++j) {
                std::lock_guard<std::mutex> lock(mutex);
                results.push_back(rng.uniformInt(0, 1000));
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Verify results are all valid
    for (int val : results) {
        EXPECT_GE(val, 0);
        EXPECT_LE(val, 1000);
    }
}
```

## Integration with NLM

The RandomGenerator is used throughout NLM for:

1. **Neural Development**: Random initialization of synapses
2. **Plasticity Rules**: Stochastic elements in learning
3. **Simulation**: Random events and noise
4. **Experiments**: Reproducible random seeds
5. **Memory Systems**: Random access patterns
6. **Performance Profiling**: Benchmark random number generation

## Migration Guide

### From std::mt19937
```cpp
// Old way
std::mt19937 gen(42);
std::uniform_int_distribution<int> dist(0, 100);
int value = dist(gen);

// New way
nlm::RandomGenerator rng(42);
int value = rng.uniformInt(0, 100);
```

### From Random Device
```cpp
// Old way
std::random_device rd;
std::mt19937 gen(rd());

// New way
nlm::RandomGenerator rng;  // Time-based seed
```

### From Multiple Generators
```cpp
// Old way - manual thread-local storage
thread_local std::mt19937 threadRng;

// New way - automatic thread safety
nlm::RandomGenerator rng;  // Thread-safe for all threads
```

## Configuration Integration

```cpp
// Store random seed in configuration for experiment reproducibility
nlm::Config config;

// Set seed from config or use default
uint64_t seed = config.getOr("experiment.seed", 42ULL);
nlm::RandomGenerator rng(seed);

// Log seed for reproducibility
nlm::Logger::getGlobal().info("Using random seed: " + std::to_string(rng.getSeed()));
```

## Best Practices

1. **Use Explicit Seeds**: Always use explicit seeds for reproducible experiments
2. **Thread Safety**: Prefer shared RandomGenerator over thread-local storage
3. **Performance**: Reuse RandomGenerator instances in hot loops
4. **Testing**: Use fixed seeds for deterministic unit tests
5. **Documentation**: Document random seeds in experiment results
6. **Compatibility**: Use RandomGenerator for all NLM random needs
7. **Monitoring**: Log seed usage for debugging
8. **Benchmarking**: Profile random number generation in performance-critical paths

## Example: Integration with NLM Brain Simulation
```cpp
#include <nlm/core/Random.hpp>
#include <nlm/core/Config.hpp>
#include <nlm/core/Logger.hpp>

void initializeNeuralNetwork(nlm::RandomGenerator& rng, size_t neuronCount) {
    std::vector<double> weights(neuronCount * neuronCount);
    
    for (size_t i = 0; i < weights.size(); ++i) {
        // Use random weights with controlled variance
        weights[i] = rng.normal(0.0, 1.0 / std::sqrt(neuronCount));
    }
    
    // Log initialization statistics
    double avgWeight = std::accumulate(weights.begin(), weights.end(), 0.0) / weights.size();
    nlm::Logger::getGlobal().info("Initialized neural network: " + 
                                 std::to_string(neuronCount) + " neurons, " +
                                 std::to_string(avgWeight) + " average weight");
}

int main() {
    nlm::Config config;
    config.set("network.neuron_count", 1000);
    config.set("random.seed", 42);
    
    nlm::RandomGenerator rng = config.getOr("random.seed", 42ULL);
    
    if (auto neuronCount = config.get<size_t>("network.neuron_count")) {
        initializeNeuralNetwork(rng, *neuronCount);
    }
    
    return 0;
}
```
