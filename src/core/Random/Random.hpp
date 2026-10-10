#pragma once

#include <cstdint>
#include <random>
#include <functional>
#include <stdexcept>
#include <type_traits>

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
    [[nodiscard]] uint64_t getSeed() const;
    
    // Random number generation
    // Uniform integer in [min, max]
    [[nodiscard]] uint32_t uniformInt(uint32_t min, uint32_t max);
    [[nodiscard]] uint64_t uniformInt(uint64_t min, uint64_t max);
    
    // Uniform real in [min, max)
    [[nodiscard]] double uniformReal(double min, double max);
    [[nodiscard]] float uniformReal(float min, float max);
    
    // Normal distribution
    [[nodiscard]] double normal(double mean, double stddev);
    [[nodiscard]] float normal(float mean, float stddev);
    
    // Bernoulli distribution
    [[nodiscard]] bool bernoulli(double p = 0.5);
    
    // Poisson distribution
    [[nodiscard]] uint32_t poisson(double lambda);
    
    // Exponential distribution
    [[nodiscard]] double exponential(double lambda);
    
    // Random boolean
    [[nodiscard]] bool randomBool();
    
    // Random bit
    [[nodiscard]] uint32_t randomBit();
    
    // Shuffle container
    template<typename Container>
    void shuffle(Container& container);
    
    // Pick random element from container
    template<typename Container>
    [[nodiscard]] auto pickRandom(const Container& container) -> typename Container::const_reference;
    
    // Pick random index from 0 to n-1
    [[nodiscard]] uint64_t pickRandomIndex(uint64_t n);
    
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
    std::shuffle(container.begin(), container.end(), engine());
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
