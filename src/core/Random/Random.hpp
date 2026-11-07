#pragma once

#include <cstdint>
#include <random>
#include <functional>
#include <stdexcept>

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
    
    ~RandomGenerator();
    
    // Disable copying
    RandomGenerator(const RandomGenerator&) = delete;
    RandomGenerator& operator=(const RandomGenerator&) = delete;
    
    // Enable moving
    RandomGenerator(RandomGenerator&&) noexcept;
    RandomGenerator& operator=(RandomGenerator&&) noexcept;
    
    // Seed operations
    void seed(uint64_t seed);
    uint64_t getSeed() const;

    // Phase 5 checkpoint support: serialize the full mt19937_64 engine
    // state (not just the seed — reseeding would replay the whole
    // random sequence from the start, so a reloaded brain would repeat
    // its earlier random choices). The format is the standard library
    // stream format for mt19937_64; restoreState returns false on a
    // parse failure and leaves the generator untouched.
    std::string serializeState() const;
    bool restoreState(const std::string& state);
    
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
    std::mt19937_64& engine();
    const std::mt19937_64& engine() const;
    
    // Static minimum/maximum for mt19937_64
    static constexpr result_type min() { return std::mt19937_64::min(); }
    static constexpr result_type max() { return std::mt19937_64::max(); }
    
    // Operator for compatibility with std functions
    result_type operator()() { return static_cast<result_type>(engine()()); }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Template implementations
template<typename Container>
void RandomGenerator::shuffle(Container& container) {
    // Use Fisher-Yates shuffle algorithm as a workaround for std::shuffle
    if (container.size() <= 1) return;
    
    auto rng = engine();
    size_t n = container.size();
    for (size_t i = n - 1; i > 0; --i) {
        size_t j = rng() % (i + 1);
        if (i != j) {
            auto it1 = std::next(container.begin(), i);
            auto it2 = std::next(container.begin(), j);
            std::iter_swap(it1, it2);
        }
    }
}

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
