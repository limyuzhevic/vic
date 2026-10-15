#include "Random.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 generator;
    uint64_t seed;
    
    Impl() {
        std::random_device rd;
        seed = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        generator.seed(seed);
    }
    
    Impl(uint64_t s) {
        seed = s;
        generator.seed(s);
    }
};

RandomGenerator::RandomGenerator() : pImpl(std::make_unique<Impl>()) {}

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(std::make_unique<Impl>(seed)) {}

int64_t RandomGenerator::uniformInt(int64_t min, int64_t max) {
    std::uniform_int_distribution<int64_t> dist(min, max);
    return dist(pImpl->generator);
}

float RandomGenerator::uniformReal() {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(pImpl->generator);
}

float RandomGenerator::uniformReal(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(pImpl->generator);
}

double RandomGenerator::uniformDouble() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(pImpl->generator);
}

double RandomGenerator::uniformDouble(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(pImpl->generator);
}

bool RandomGenerator::randomBool(float p) {
    return uniformReal() < p;
}

uint64_t RandomGenerator::getSeed() const {
    return pImpl->seed;
}

void RandomGenerator::setSeed(uint64_t seed) {
    pImpl->seed = seed;
    pImpl->generator.seed(seed);
}

float RandomGenerator::normalRandom(float mean, float stddev) {
    std::normal_distribution<float> dist(mean, stddev);
    return dist(pImpl->generator);
}

float RandomGenerator::exponentialRandom(float lambda) {
    std::exponential_distribution<float> dist(lambda);
    return dist(pImpl->generator);
}

template<typename T>
void RandomGenerator::shuffle(T* array, size_t n) {
    for (size_t i = n - 1; i > 0; --i) {
        size_t j = uniformInt(0, static_cast<int64_t>(i));
        std::swap(array[i], array[j]);
    }
}

template<typename T>
std::vector<T> RandomGenerator::randomPermutation(const std::vector<T>& vec) {
    std::vector<T> result = vec;
    shuffle(result.data(), result.size());
    return result;
}

// Template instantiations for random functions
namespace nlm {
    template void RandomGenerator::shuffle<int>(int* array, size_t n);
    template void RandomGenerator::shuffle<float>(float* array, size_t n);
    template void RandomGenerator::shuffle<double>(double* array, size_t n);
    template void RandomGenerator::shuffle<std::string>(std::string* array, size_t n);
    
    template std::vector<int> RandomGenerator::randomPermutation(const std::vector<int>& vec);
    template std::vector<float> RandomGenerator::randomPermutation(const std::vector<float>& vec);
    template std::vector<double> RandomGenerator::randomPermutation(const std::vector<double>& vec);
    template std::vector<std::string> RandomGenerator::randomPermutation(const std::vector<std::string>& vec);
}

} // namespace nlm