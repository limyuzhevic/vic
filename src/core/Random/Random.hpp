#pragma once

#include <cstdint>
#include <random>
#include <functional>

namespace nlm {

// Deterministic random number generator for scientific simulations
    // Supports explicit seeds for reproducible experiments, crucial for scientific validation
    // Uses mt19937_64 for high-quality random numbers with 64-bit precision

    class RandomGenerator {
    public:
        using result_type = uint32_t;  // For compatibility with std::uniform_int_distribution
        
        // Create with explicit seed for reproducible experiments
        // Example: RandomGenerator rng(42);  // Always produces same sequence
        explicit RandomGenerator(uint64_t seed);
        
        // Create with time-based seed for non-deterministic randomness
        // Use only when you need different random sequences each run
        RandomGenerator();
        
        ~RandomGenerator();
        
        // Disable copying to prevent multiple access to same generator
        RandomGenerator(const RandomGenerator&) = delete;
        RandomGenerator& operator=(const RandomGenerator&) = delete;
        
        // Enable moving - transfer ownership of random state
        RandomGenerator(RandomGenerator&&) noexcept;
        RandomGenerator& operator=(RandomGenerator&&) noexcept;
        
        // Seed operations for resetting the generator
        void seed(uint64_t seed);           // Set new seed for reproducible sequence
        uint64_t getSeed() const;          // Get current seed value
        
        // Random number generation methods
        // All methods ensure min <= result < max (or min <= result <= max where specified)
        
        // Uniform integer in [min, max] (inclusive)
        uint32_t uniformInt(uint32_t min, uint32_t max);
        uint64_t uniformInt(uint64_t min, uint64_t max);
        
        // Uniform real in [min, max) (standard uniform distribution)
        double uniformReal(double min, double max);
        float uniformReal(float min, float max);
        
        // Normal/Gaussian distribution with given mean and standard deviation
        double normal(double mean, double stddev);
        float normal(float mean, float stddev);
        
        // Bernoulli/binomial distribution for success/failure trials
        bool bernoulli(double p = 0.5);  // Probability of true result
        
        // Poisson distribution for counting events in fixed intervals
        uint32_t poisson(double lambda);  // Average event rate
        
        // Exponential distribution for inter-arrival times
        double exponential(double lambda);  // Rate parameter
        
        // Convenience methods
        bool randomBool();                    // 50/50 chance
        uint32_t randomBit();                 // Single random bit (0 or 1)
        
        // Container operations for shuffling and random selection
        template<typename Container>
        void shuffle(Container& container);   // Shuffle all elements randomly
        
        template<typename Container>
        auto pickRandom(const Container& container) -> typename Container::const_reference;
                                                    // Return random element
        
        uint64_t pickRandomIndex(uint64_t n);  // Return random index in [0, n-1]
        
        void fillBytes(void* data, size_t size);  // Fill buffer with random bytes
        
        // Get underlying engine for advanced users
        // For compatibility with standard library algorithms
        std::mt19937_64& engine();
        const std::mt19937_64& engine() const;
        
        // Static member functions for compatibility with std::numeric_limits
        static constexpr result_type min() { return std::mt19937_64::min(); }
        static constexpr result_type max() { return std::mt19937_64::max(); }
        
        // Operator for compatibility with std functions
        result_type operator()() { return static_cast<result_type>(engine()()); }
        
        // Set of utility functions for common statistical distributions
        // All functions perform input validation and throw std::invalid_argument on errors
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Template implementations
    template<typename Container>
    void RandomGenerator::shuffle(Container& container) {
        // Shuffle container using Fisher-Yates algorithm
        std::shuffle(container.begin(), container.end(), engine());
    }
    
    template<typename Container>
    auto RandomGenerator::pickRandom(const Container& container) -> typename Container::const_reference {
        // Pick random element from container
        if (container.empty()) {
            throw std::runtime_error("Cannot pick from empty container");
        }
        auto it = container.begin();
        std::advance(it, pickRandomIndex(container.size()));
        return *it;
    }

} // namespace nlm
