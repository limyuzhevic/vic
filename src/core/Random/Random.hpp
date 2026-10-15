#pragma once

#include <cstdint>
#include <random>
#include <algorithm>

namespace nlm {

class RandomGenerator {
public:
    RandomGenerator();
    explicit RandomGenerator(uint64_t seed);
    
    // Generate random integer in range [min, max]
    int64_t uniformInt(int64_t min, int64_t max);
    
    // Generate random float in range [0.0, 1.0)
    float uniformReal();
    
    // Generate random float in range [min, max]
    float uniformReal(float min, float max);
    
    // Generate random double in range [0.0, 1.0)
    double uniformDouble();
    
    // Generate random double in range [min, max]
    double uniformDouble(double min, double max);
    
    // Generate random boolean with probability p
    bool randomBool(float p = 0.5f);
    
    // Get current seed
    uint64_t getSeed() const;
    
    // Set seed
    void setSeed(uint64_t seed);
    
    // Generate random number from normal distribution
    float normalRandom(float mean = 0.0f, float stddev = 1.0f);
    
    // Generate random number from exponential distribution
    float exponentialRandom(float lambda = 1.0f);
    
    // Shuffle array
    template<typename T>
    void shuffle(T* array, size_t n);
    
    // Generate random permutation
    template<typename T>
    std::vector<T> randomPermutation(const std::vector<T>& vec);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm