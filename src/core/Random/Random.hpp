// Random number generation system for NLM
#pragma once

#include <random>
#include <cstdint>

namespace nlm {

class RandomGenerator {
public:
    RandomGenerator(uint64_t seed = std::mt19937::default_seed);
    ~RandomGenerator() = default;
    
    // Set seed
    void setSeed(uint64_t seed);
    
    // Get current seed
    uint64_t getSeed() const;
    
    // Uniform random number generation
    float uniformReal(float min, float max);
    double uniformReal(double min, double max);
    int64_t uniformInt(int64_t min, int64_t max);
    uint64_t uniformUint(uint64_t min, uint64_t max);
    
    // Gaussian random numbers
    float gaussian(float mean, float stddev);
    double gaussian(double mean, double stddev);
    
    // Random choice from range
    template<typename T>
    T randomChoice(const std::vector<T>& choices);
    
    // Random permutation
    std::vector<size_t> shuffle(size_t count);
    
    // Bernoulli trial
    bool bernoulli(float probability);
    
    // Exponential distribution
    float exponential(float lambda);
    
    // Normal distribution (Box-Muller transform)
    std::pair<float, float> normalPair(float mean, float stddev);
    
    // Get underlying generator (for advanced usage)
    std::mt19937& getGenerator() { return rng_; }
    const std::mt19937& getGenerator() const { return rng_; }
    
private:
    std::mt19937 rng_;
    std::uniform_real_distribution<float> uniformFloatDist_;
    std::uniform_int_distribution<int64_t> uniformIntDist_;
    std::normal_distribution<float> normalDist_;
    std::exponential_distribution<float> exponentialDist_;
};

} // namespace nlm