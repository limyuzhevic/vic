#pragma once

#include <cstdint>
#include <random>
#include <functional>
#include <format>
#include <span>
#include <ranges>
#include <optional>
#include <stdexcept>

namespace nlm {

// Advanced deterministic random number generator with C++20 features
// Supports explicit seeds for reproducible experiments with modern patterns

class RandomGenerator {
public:
    using result_type = uint32_t;
    using engine_type = std::mt19937_64;
    
    // Create with explicit seed
    explicit RandomGenerator(uint64_t seed);
    
    // Create with time-based seed (not deterministic) - using C++20 chrono
    RandomGenerator();
    
    ~RandomGenerator();
    
    // Disable copying
    RandomGenerator(const RandomGenerator&) = delete;
    RandomGenerator& operator=(const RandomGenerator&) = delete;
    
    // Enable moving with noexcept
    RandomGenerator(RandomGenerator&&) noexcept;
    RandomGenerator& operator=(RandomGenerator&&) noexcept;
    
    // Seed operations
    void seed(uint64_t seed);
    uint64_t getSeed() const;
    
    // Random number generation with C++20 distributed support
    // Uniform integer in [min, max] with range-checked operations
    uint32_t uniformInt(uint32_t min, uint32_t max);
    uint64_t uniformInt(uint64_t min, uint64_t max);
    
    // Uniform real in [min, max) with template parameter support
    double uniformReal(double min, double max);
    float uniformReal(float min, float max);
    
    // Normal distribution with C++20 math constants
    double normal(double mean, double stddev);
    float normal(float mean, float stddev);
    
    // Bernoulli distribution with probability validation
    bool bernoulli(double p = 0.5);
    
    // Poisson distribution with lambda validation
    uint32_t poisson(double lambda);
    
    // Exponential distribution with lambda validation
    double exponential(double lambda);
    
    // Random boolean with improved entropy source
    bool randomBool();
    
    // Random bit using engine directly
    uint32_t randomBit();
    
    // Shuffle container using range-based algorithms (C++20)
    template<typename Container>
    void shuffle(Container& container) {
        if (container.empty()) {
            throw std::invalid_argument("Cannot shuffle empty container");
        }
        std::ranges::shuffle(container.begin(), container.end(), engine());
    }
    
    // Pick random element from container using views (C++20)
    template<typename Container>
    auto pickRandom(const Container& container) -> typename Container::const_reference {
        if (container.empty()) {
            throw std::runtime_error("Cannot pick from empty container");
        }
        auto view = container | std::views::reverse;
        auto it = view.begin();
        std::advance(it, pickRandomIndex(container.size()));
        return *it;
    }
    
    // Pick random index from 0 to n-1 with validation
    uint64_t pickRandomIndex(uint64_t n);
    
    // Fill range with random bytes using span (C++20)
    void fillBytes(void* data, size_t size);
    
    // Get underlying engine for advanced use with const correctness
    std::mt19937_64& engine();
    const std::mt19937_64& engine() const;
    
    // Static minimum/maximum for mt19937_64 with better naming
    static constexpr result_type min() { return std::mt19937_64::min(); }
    static constexpr result_type max() { return std::mt19937_64::max(); }
    
    // Operator for compatibility with std functions
    result_type operator()() { return static_cast<result_type>(engine()()); }
    
    // C++20 generator functions for modern parallel processing
    template<typename T>
    auto generateSequence(size_t count, T start, T step = 1) {
        return std::views::iota(start, start + count * step) | 
               std::views::transform([this](T val) { return static_cast<T>(uniformReal(0, 1) * val); });
    }
    
    // Modern C++20 range-based random generation utilities
    template<typename Container, typename Callable>
    void for_each_random(const Container& container, Callable&& func, size_t samples = 10) {
        if (container.empty()) return;
        
        auto indices = std::views::iota(size_t(0), container.size()) | 
                       std::views::filter([this](size_t) { return bernoulli(0.5); }) | 
                       std::views::take(samples);
        
        for (size_t idx : indices) {
            func(container[idx]);
        }
    }
    
    // Utility function for C++20 structured bindings
    auto get_distribution_stats(double mean, double stddev) {
        struct Stats {
            double mean;
            double stddev;
            double variance;
            double skew;
            double kurtosis;
        };
        
        double variance = stddev * stddev;
        return Stats{mean, stddev, variance, 0.0, 3.0}; // Simplified stats
    }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Template implementations with C++20 improvements

// Enhanced shuffle implementation using range algorithms
namespace detail {
    template<typename Iterator>
    void shuffle_range(Iterator begin, Iterator end, std::mt19937_64& engine) {
        auto distance = std::distance(begin, end);
        if (distance <= 1) return;
        
        for (size_t i = distance - 1; i > 0; --i) {
            auto j = std::uniform_int_distribution<size_t>(0, i)(engine);
            std::iter_swap(std::next(begin, i), std::next(begin, j));
        }
    }
}

// Enhanced pickRandom implementation with better error handling
template<typename Container>
auto RandomGenerator::pickRandom(const Container& container) -> typename Container::const_reference {
    if (container.empty()) {
        throw std::runtime_error("Cannot pick from empty container");
    }
    
    // Use C++20 views for more efficient random access
    auto view = std::views::reverse(container);
    size_t random_index = pickRandomIndex(container.size());
    auto it = std::rbegin(view) + random_index;
    return *it;
}

// C++20 optimized distribution implementations
namespace distributions {
    inline uint32_t uniform_int(std::mt19937_64& engine, uint32_t min, uint32_t max) {
        if (min > max) std::swap(min, max);
        return std::uniform_int_distribution<uint32_t>(min, max)(engine);
    }
    
    inline double uniform_real(std::mt19937_64& engine, double min, double max) {
        return std::uniform_real_distribution<double>(min, max)(engine);
    }
    
    inline double normal_dist(std::mt19937_64& engine, double mean, double stddev) {
        return std::normal_distribution<double>(mean, stddev)(engine);
    }
    
    inline bool bernoulli_dist(std::mt19937_64& engine, double p) {
        return std::bernoulli_distribution(p)(engine);
    }
    
    inline uint32_t poisson_dist(std::mt19937_64& engine, double lambda) {
        return std::poisson_distribution<uint32_t>(lambda)(engine);
    }
    
    inline double exponential_dist(std::mt19937_64& engine, double lambda) {
        return std::exponential_distribution<double>(lambda)(engine);
    }
}

// Factory function for creating optimized random generators
inline RandomGenerator create_fast_random_generator(uint64_t seed) {
    return RandomGenerator(seed);
}

// C++20 generator adapter for modern C++ algorithms
struct random_generator_adaptor {
    std::mt19937_64& engine;
    
    template<typename T>
    T operator()(T min, T max) {
        return static_cast<T>(std::uniform_real_distribution<double>(min, max)(engine));
    }
    
    bool operator()(double p) {
        return std::bernoulli_distribution(p)(engine);
    }
};

// Modern C++20 integration helpers
namespace modern {
    // Helper for creating random distributions with type safety
    template<typename Distribution, typename... Args>
    Distribution make_distribution(Args&&... args) {
        return Distribution(std::forward<Args>(args)...);
    }
    
    // Helper for sampling from distributions
    template<typename Distribution, typename Engine>
    auto sample(Engine& engine, Distribution& dist, size_t count) {
        return std::views::iota(0ULL, count) | 
               std::views::transform([&](size_t) { return dist(engine); });
    }
}

} // namespace nlm
