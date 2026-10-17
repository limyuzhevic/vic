#pragma once

#include <cstdint>
#include <random>
#include <functional>
#include <memory>

namespace nlm {

// Deterministic random number generator
// Supports explicit seeds for reproducible experiments

class RandomGenerator {
public:
    using result_type = uint32_t;
    
    // Create with explicit seed
    explicit RandomGenerator(uint64_t seed);
    
    // Create with time-based seed (not deterministic)
    RandomGenerator();
    
    // Destructor with explicit cleanup
    ~RandomGenerator() noexcept;
    
    // Disable copying
    RandomGenerator(const RandomGenerator&) = delete;
    RandomGenerator& operator=(const RandomGenerator&) = delete;
    
    // Enable moving with noexcept specification
    RandomGenerator(RandomGenerator&& other) noexcept;
    RandomGenerator& operator=(RandomGenerator&& other) noexcept;
    
    // Seed operations
    void seed(uint64_t seed);
    uint64_t getSeed() const noexcept;
    
    // Random number generation
    // Uniform integer in [min, max]
    uint32_t uniformInt(uint32_t min, uint32_t max);
    uint64_t uniformInt(uint64_t min, uint64_t max);
    
    // Uniform real in [min, max)
    double uniformReal(double min, double max);
    float uniformReal(float min, float max);
    
    // Normal distribution
    double normal(double mean, double stddev);
    float normal(float mean, float stddev);
    
    // Bernoulli distribution
    bool bernoulli(double p = 0.5);
    
    // Poisson distribution
    uint32_t poisson(double lambda);
    
    // Exponential distribution
    double exponential(double lambda);
    
    // Random boolean
    bool randomBool();
    
    // Random bit
    uint32_t randomBit();
    
    // Shuffle container
    template<typename Container>
    void shuffle(Container& container);
    
    // Pick random element from container
    template<typename Container>
    auto pickRandom(const Container& container) -> typename Container::const_reference;
    
    // Pick random index from 0 to n-1
    uint64_t pickRandomIndex(uint64_t n);
    
    // Fill range with random bytes
    void fillBytes(void* data, size_t size);
    
    // Get underlying engine for advanced use
    std::mt19937_64& engine() noexcept;
    const std::mt19937_64& engine() const noexcept;
    
    // Static minimum/maximum for mt19937_64
    static constexpr result_type min() noexcept { return std::mt19937_64::min(); }
    static constexpr result_type max() noexcept { return std::mt19937_64::max(); }
    
    // Operator for compatibility with std functions
    result_type operator()() noexcept { return static_cast<result_type>(engine()()); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Validate seed value
    void validateSeed(uint64_t seed) const noexcept;
    
    // Initialize implementation
    void initializeImpl(uint64_t seed);
};

// Template implementations
template<typename Container>
void RandomGenerator::shuffle(Container& container) {
    std::shuffle(container.begin(), container.end(), engine());
}

template<typename Container>
auto RandomGenerator::pickRandom(const Container& container) -> typename Container::const_reference {
    if (container.empty()) {
        NLM_THROW_VALIDATION("Cannot pick from empty container");
    }
    auto it = container.begin();
    std::advance(it, pickRandomIndex(container.size()));
    return *it;
}

} // namespace nlm
