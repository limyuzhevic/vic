#include "Random.hpp"
#include <stdexcept>
#include <algorithm>
#include <random>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 engine;
    uint64_t seed;
    
    Impl(uint64_t seed) : engine(seed), seed(seed) {}
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(new Impl(seed)) {}

RandomGenerator::RandomGenerator() : RandomGenerator(std::chrono::system_clock::now().time_since_epoch().count()) {}

RandomGenerator::~RandomGenerator() = default;

RandomGenerator::RandomGenerator(RandomGenerator&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

RandomGenerator& RandomGenerator::operator=(RandomGenerator&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void RandomGenerator::seed(uint64_t seed) {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    pImpl->engine.seed(seed);
    pImpl->seed = seed;
}

uint64_t RandomGenerator::getSeed() const {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    return pImpl->seed;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    if (min > max) std::swap(min, max);
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(pImpl->engine);
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    if (min > max) std::swap(min, max);
    std::uniform_int_distribution<uint64_t> dist(min, max);
    return dist(pImpl->engine);
}

double RandomGenerator::uniformReal(double min, double max) {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    std::uniform_real_distribution<double> dist(min, max);
    return dist(pImpl->engine);
}

float RandomGenerator::uniformReal(float min, float max) {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    std::uniform_real_distribution<float> dist(min, max);
    return dist(pImpl->engine);
}

double RandomGenerator::normal(double mean, double stddev) {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    std::normal_distribution<double> dist(mean, stddev);
    return dist(pImpl->engine);
}

float RandomGenerator::normal(float mean, float stddev) {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    std::normal_distribution<float> dist(mean, stddev);
    return dist(pImpl->engine);
}

bool RandomGenerator::bernoulli(double p) {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    if (p < 0.0 || p > 1.0) throw std::invalid_argument("Probability must be in [0, 1]");
    std::bernoulli_distribution dist(p);
    return dist(pImpl->engine);
}

uint32_t RandomGenerator::poisson(double lambda) {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    if (lambda <= 0.0) return 0;
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->engine);
}

double RandomGenerator::exponential(double lambda) {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    if (lambda <= 0.0) throw std::invalid_argument("Lambda must be positive");
    std::exponential_distribution<double> dist(lambda);
    return dist(pImpl->engine);
}

bool RandomGenerator::randomBool() {
    return bernoulli(0.5);
}

uint32_t RandomGenerator::randomBit() {
    return uniformInt(0, 1);
}

template<typename Container>
void RandomGenerator::shuffle(Container& container) {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    std::shuffle(container.begin(), container.end(), pImpl->engine);
}

template<typename Container>
auto RandomGenerator::pickRandom(const Container& container) -> typename Container::const_reference {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    if (container.empty()) {
        throw std::runtime_error("Cannot pick from empty container");
    }
    auto it = container.begin();
    std::advance(it, pickRandomIndex(container.size()));
    return *it;
}

uint64_t RandomGenerator::pickRandomIndex(uint64_t n) {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    if (n == 0) throw std::invalid_argument("Cannot pick from zero elements");
    return uniformInt(0, n - 1);
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    auto* bytes = static_cast<std::byte*>(data);
    for (size_t i = 0; i < size; ++i) {
        bytes[i] = static_cast<std::byte>(uniformInt(0, 255));
    }
}

std::mt19937_64& RandomGenerator::engine() {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    return pImpl->engine;
}

const std::mt19937_64& RandomGenerator::engine() const {
    if (!pImpl) throw std::runtime_error("RandomGenerator not initialized");
    return pImpl->engine;
}

} // namespace nlm
