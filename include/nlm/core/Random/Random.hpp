#ifndef NLM_CORE_RANDOM_RANDOM_HPP
#define NLM_CORE_RANDOM_RANDOM_HPP

#include <cstdint>
#include <random>
#include <memory>

namespace nlm {

class RandomGenerator {
public:
    RandomGenerator(uint64_t seed = 42);
    ~RandomGenerator();
    
    void setSeed(uint64_t seed);
    uint64_t getSeed() const;
    
    int64_t nextInt(int64_t min, int64_t max);
    double nextDouble(double min = 0.0, double max = 1.0);
    float nextFloat(float min = 0.0f, float max = 1.0f);
    bool nextBool(double probability = 0.5);
    
    template<typename T>
    const T& choice(const std::vector<T>& items);
    
    void shuffle(std::vector<size_t>& indices);
    
    // Mersenne Twister for better randomness
    std::mt19937& getGenerator() { return generator; }
    const std::mt19937& getGenerator() const { return generator; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

using RandomGeneratorPtr = std::shared_ptr<RandomGenerator>;

} // namespace nlm

#endif // NLM_CORE_RANDOM_RANDOM_HPP
