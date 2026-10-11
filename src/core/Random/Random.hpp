#pragma once

#include <cstdint>
#include <random>
#include <functional>

namespace nlm {

// Deterministic random number generator
// Supports explicit seeds for reproducible experiments
// Uses mt19937_64 (Mersenne Twister) with 64-bit state for high-quality randomness
// Thread-safe for concurrent use in multi-threaded simulations
// Compatible with C++ random number generation interfaces

class RandomGenerator {
public:
    using result_type = uint32_t;
    
    // Create with explicit seed
// Provides full reproducibility for debugging and experimental consistency
// Same seed always produces same sequence of random numbers
explicit RandomGenerator(uint64_t seed);
    
    // Create with time-based seed (not deterministic)
    // Generates random seed from system clock for uniqueness
    // Use when you want non-deterministic randomness (e.g., game simulations)
    RandomGenerator();
    
    ~RandomGenerator();
    
    // Disable copying
    // Copying random generators could share internal state, leading to bugs
    RandomGenerator(const RandomGenerator&) = delete;
    RandomGenerator& operator=(const RandomGenerator&) = delete;
    
    // Enable moving
    // Moving is safe as the internal state is transferred efficiently
    RandomGenerator(RandomGenerator&&) noexcept;
    RandomGenerator& operator=(RandomGenerator&&) noexcept;
    
    // Seed operations
    
    // Set new seed, reinitializes random number sequence
    // Calling with same seed produces same sequence
    // Invalidation of any cached state (if any)
    void seed(uint64_t seed);
    
    // Get current seed without reinitializing generator
    // Useful for saving/restoring reproducible states
    uint64_t getSeed() const;
    
    // Uniform integer in [min, max] (inclusive range)
    // Generates random integers with uniform probability distribution
    // Used for selecting random neurons, synapses, or array indices
    // Parameters: min, max - inclusive bounds for the random integer
    // Returns: Random integer in [min, max]
    uint32_t uniformInt(uint32_t min, uint32_t max);
    uint64_t uniformInt(uint64_t min, uint64_t max);
    
    // Uniform real in [min, max) (half-open interval)
    // Generates random floating-point numbers with uniform probability
    // Used for stochastic processes and continuous-time models
    // Parameters: min, max - bounds for the random float (max is exclusive)
    // Returns: Random float in [min, max)
    double uniformReal(double min, double max);
    float uniformReal(float min, float max);
    
    // Normal distribution
    // Generates random numbers from Gaussian (normal) distribution
    // Used for modeling neural noise, synaptic variability, and stochastic dynamics
    // Parameters: mean, stddev - mean and standard deviation of the distribution
    // Returns: Random sample from N(mean, stddev²)
    double normal(double mean, double stddev);
    float normal(float mean, float stddev);
    
    // Bernoulli distribution
    // Generates random boolean values with specified probability
    // Used for stochastic firing, plasticity events, and connection formation
    // Parameters: p - probability of returning true (default: 0.5)
    // Returns: true with probability p, false with probability 1-p
    bool bernoulli(double p = 0.5);
    
    // Poisson distribution
    // Generates random integer counts from Poisson distribution
    // Used for modeling spike counts, synaptic event counts, and neuronal firing
    // Parameters: lambda - expected number of events per unit time
    // Returns: Random count from Poisson(lambda)
    uint32_t poisson(double lambda);
    
    // Exponential distribution
    // Generates random samples from exponential distribution
    // Used for modeling inter-spike intervals, synaptic decay times, and temporal dynamics
    // Parameters: lambda - rate parameter (events per unit time)
    // Returns: Random time from exponential distribution
    double exponential(double lambda);
    
    // Random boolean
    // Generates true/false with equal probability (standard Bernoulli with p=0.5)
    // Convenience function for simple coin flips or binary decisions
    // Returns: true with 50% probability, false with 50% probability
    bool randomBool();
    
    // Random bit
    // Generates a single random bit (0 or 1) for bit-level operations
    // Efficiently generates random bits without floating-point conversion
    // Returns: 0 or 1 with equal probability
    uint32_t randomBit();
    
    // Shuffle container
    // Randomly reorders elements in a container using Fisher-Yates algorithm
    // Useful for randomizing neuron ordering, synaptic weights, or connection matrices
    // Parameters: container - container to be shuffled in-place
    template<typename Container>
    void shuffle(Container& container);
    
    // Pick random element from container
    // Efficiently selects a random element from any container with O(1) access
    // Used for selecting random neurons, synapses, or patterns from collections
    // Parameters: container - container from which to select
    // Returns: Reference to randomly selected element
    template<typename Container>
    auto pickRandom(const Container& container) -> typename Container::const_reference;
    
    // Pick random index from 0 to n-1
    // Generates a random integer index in the range [0, n)
    // Used for array indexing, random access, and stochastic algorithms
    // Parameters: n - upper bound (exclusive)
    // Returns: Random index in [0, n-1] or 0 if n=0
    uint64_t pickRandomIndex(uint64_t n);
    
    // Fill range with random bytes
    // Generates cryptographically secure random bytes (if available)
    // Used for creating random masks, noise, or initialization patterns
    // Parameters: data - buffer to fill with random bytes
    // Parameters: size - number of bytes to generate
    void fillBytes(void* data, size_t size);
    
    // Get underlying engine for advanced use
    // Provides access to the underlying Mersenne Twister engine for custom distributions
    // Advanced users can modify engine parameters or add custom distributions
    // Returns: Reference to internal random number engine
    std::mt19937_64& engine();
    
    // Const version for read-only access
    const std::mt19937_64& engine() const;
    
    // Static minimum/maximum for mt19937_64
    static constexpr result_type min() { return std::mt19937_64::min(); }
    static constexpr result_type max() { return std::mt19937_64::max(); }
    
    // Operator for compatibility with std functions
    // Allows RandomGenerator to be used with standard library random number algorithms
    result_type operator()() { return static_cast<result_type>(engine()()); }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Template implementations

// Shuffle container implementation
// Uses std::shuffle with the internal random engine for reproducible shuffling
// Fisher-Yates algorithm ensures uniform distribution of all possible permutations
template<typename Container>
void RandomGenerator::shuffle(Container& container) {
    std::shuffle(container.begin(), container.end(), engine());
}

// Pick random element from container implementation
// Uses std::advance for efficient O(k) random access (k = random index)
// Includes bounds checking for empty containers
template<typename Container>
auto RandomGenerator::pickRandom(const Container& container) -> typename Container::const_reference {
    if (container.empty()) {
        throw std::runtime_error("Cannot pick from empty container");
    }
    auto it = container.begin();
    std::advance(it, pickRandomIndex(container.size()));
    return *it;
}

} // namespace nlm
