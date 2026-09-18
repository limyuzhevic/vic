#include "Random.hpp"
#include <random>
#include <stdexcept>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 engine;
    uint64_t seed;
    
    Impl() : engine(std::random_device{}()), seed(0) {}
    Impl(uint64_t s) : engine(s), seed(s) {}
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(new Impl(seed)) {}

RandomGenerator::RandomGenerator() : pImpl(new Impl()) {}

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
    pImpl->engine.seed(seed);
    pImpl->seed = seed;
}

uint64_t RandomGenerator::getSeed() const {
    return pImpl->seed;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(pImpl->engine);
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    std::uniform_int_distribution<uint64_t> dist(min, max);
    return dist(pImpl->engine);
}

double RandomGenerator::uniformReal(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(pImpl->engine);
}

float RandomGenerator::uniformReal(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(pImpl->engine);
}

double RandomGenerator::normal(double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    return dist(pImpl->engine);
}

float RandomGenerator::normal(float mean, float stddev) {
    std::normal_distribution<float> dist(mean, stddev);
    return dist(pImpl->engine);
}

bool RandomGenerator::bernoulli(double p) {
    std::bernoulli_distribution dist(p);
    return dist(pImpl->engine);
}

uint32_t RandomGenerator::poisson(double lambda) {
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->engine);
}

double RandomGenerator::exponential(double lambda) {
    std::exponential_distribution<double> dist(lambda);
    return dist(pImpl->engine);
}

bool RandomGenerator::randomBool() {
    return bernoulli();
}

uint32_t RandomGenerator::randomBit() {
    return uniformInt(0u, 1u);
}

uint64_t RandomGenerator::pickRandomIndex(uint64_t n) {
    if (n == 0) {
        return 0;
    }
    std::uniform_int_distribution<uint64_t> dist(0, n - 1);
    return dist(pImpl->engine);
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::uniform_int_distribution<uint8_t> dist;
    uint8_t* bytes = static_cast<uint8_t*>(data);
    for (size_t i = 0; i < size; ++i) {
        bytes[i] = dist(rng);
    }
}

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

std::mt19937_64& RandomGenerator::engine() {
    return pImpl->engine;
}

const std::mt19937_64& RandomGenerator::engine() const {
    return pImpl->engine;
}

} // namespace nlm
