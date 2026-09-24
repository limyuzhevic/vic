#pragma once

#include <cstdint>
#include <random>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <shared_mutex>

namespace nlm {

// Deterministic random number generator with enhanced features
// Supports explicit seeds for reproducible experiments
// Thread-safe for parallel simulations

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
    
    // Get a persistent seed from seed file
    static uint64_t getSeedFromFile(const std::string& filepath);
    static void saveSeedToFile(const std::string& filepath, uint64_t seed);
    
    // Thread-local generator for parallel simulations
    static RandomGenerator& getThreadLocalGenerator();
    static void resetThreadLocalGenerator();
    
    // Random number generation
    // Uniform integer in [min, max]
    uint32_t uniformInt(uint32_t min, uint32_t max);
    uint64_t uniformInt(uint64_t min, uint64_t max);
    
    // Uniform real in [min, max)
    double uniformReal(double min, double max);
    float uniformReal(float min, float max);
    
    // Normal distribution with validation
    double normal(double mean = 0.0, double stddev = 1.0);
    float normal(float mean = 0.0f, float stddev = 1.0f);
    
    // Bernoulli distribution
    bool bernoulli(double p = 0.5);
    
    // Poisson distribution (useful for spike generation)
    uint32_t poisson(double lambda);
    
    // Exponential distribution (inter-spike intervals)
    double exponential(double lambda);
    float exponential(float lambda);
    
    // Gaussian process for correlated noise
    double gaussianProcess(double correlationTime = 1.0);
    
    // Random boolean
    bool randomBool();
    
    // Random bit
    uint32_t randomBit();
    
    // Shuffle container (thread-safe version)
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
    
    // Advanced distribution helpers
    struct DistributionParams {
        double mean;
        double stddev;
        double lambda;
        double correlationTime;
        double p;
        
        DistributionParams() : mean(0.0), stddev(1.0), lambda(1.0), correlationTime(1.0), p(0.5) {}
        DistributionParams(double m, double s) : mean(m), stddev(s), lambda(1.0), correlationTime(1.0), p(0.5) {}
    };
    
    // Set distribution parameters for neuroscience use cases
    void setDistributionParams(const DistributionParams& params);
    DistributionParams getDistributionParams() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Thread-local storage for parallel generators
    static thread_local std::unique_ptr<RandomGenerator> threadLocalGenerator;
    static thread_local uint64_t threadLocalSeed;
    
    // Seed management
    static std::shared_mutex seedMutex;
    static std::unordered_map<std::thread::id, uint64_t> threadSeeds;
    
    // Default seed file path
    static const std::string DEFAULT_SEED_FILE;
};

// Template implementations
namespace detail {
    template<typename Engine>
    Engine createThreadLocalEngine() {
        thread_local Engine engine;
        return engine;
    }
}

// Convenience function for creating random generators with seed file support
inline RandomGenerator createRandomGenerator(const std::string& seedFile = "nlm_seed.txt", bool createIfNotExists = true) {
    uint64_t seed;
    if (createIfNotExists && !RandomGenerator::getSeedFromFile(seedFile)) {
        // Generate new seed and save it
        seed = std::chrono::system_clock::now().time_since_epoch().count() ^ std::hash<std::thread::id>{}(std::this_thread::get_id());
        RandomGenerator::saveSeedToFile(seedFile, seed);
    } else {
        seed = RandomGenerator::getSeedFromFile(seedFile);
    }
    return RandomGenerator(seed);
}

// Statistics and utilities for random generation
namespace random {
    struct Statistics {
        uint64_t seed;
        double minUniform;
        double maxUniform;
        double meanNormal;
        double stddevNormal;
        size_t calls;
        std::chrono::system_clock::time_point lastUsed;
    };
    
    // Get statistics about the current generator
    Statistics getStatistics();
    
    // Reset statistics
    void resetStatistics();
    
    // Generate reproducible sequence for testing
    std::vector<double> generateReproducibleSequence(size_t count, uint64_t seed = 42);
}

} // namespace nlm

#include "Random.cpp"
