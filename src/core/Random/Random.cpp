#include "Random.hpp"
#include <sstream>
#include <stdexcept>

namespace nlm {

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed) {}
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(new Impl(seed)) {}

RandomGenerator::RandomGenerator() : pImpl(new Impl(std::random_device{}())) {}

RandomGenerator::~RandomGenerator() = default;

RandomGenerator::RandomGenerator(RandomGenerator&& other) noexcept : pImpl(std::move(other.pImpl)) {}

RandomGenerator& RandomGenerator::operator=(RandomGenerator&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void RandomGenerator::seed(uint64_t seed) {
    pImpl->gen.seed(seed);
    pImpl->currentSeed = seed;
}

uint64_t RandomGenerator::getSeed() const {
    return pImpl->currentSeed;
}

std::string RandomGenerator::serializeState() const {
    std::ostringstream out;
    out << pImpl->gen;
    return out.str();
}

bool RandomGenerator::restoreState(const std::string& state) {
    std::istringstream in(state);
    std::mt19937_64 gen;
    if (!(in >> gen)) return false;
    pImpl->gen = gen;
    return true;
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    if (min > max) {
        throw std::invalid_argument("min must be <= max");
    }
    if (min == max) {
        return min;
    }
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(pImpl->gen);
}

uint64_t RandomGenerator::uniformInt(uint64_t min, uint64_t max) {
    if (min > max) {
        throw std::invalid_argument("min must be <= max");
    }
    if (min == max) {
        return min;
    }
    std::uniform_int_distribution<uint64_t> dist(min, max);
    return dist(pImpl->gen);
}

double RandomGenerator::uniformReal(double min, double max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    std::uniform_real_distribution<double> dist(min, max);
    return dist(pImpl->gen);
}

float RandomGenerator::uniformReal(float min, float max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    std::uniform_real_distribution<float> dist(min, max);
    return dist(pImpl->gen);
}

double RandomGenerator::normal(double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    return dist(pImpl->gen);
}

float RandomGenerator::normal(float mean, float stddev) {
    std::normal_distribution<float> dist(mean, stddev);
    return dist(pImpl->gen);
}

bool RandomGenerator::bernoulli(double p) {
    std::bernoulli_distribution dist(p);
    return dist(pImpl->gen);
}

uint32_t RandomGenerator::poisson(double lambda) {
    std::poisson_distribution<uint32_t> dist(lambda);
    return dist(pImpl->gen);
}

double RandomGenerator::exponential(double lambda) {
    std::exponential_distribution<double> dist(lambda);
    return dist(pImpl->gen);
}

bool RandomGenerator::randomBool() {
    return bernoulli(0.5);
}

uint32_t RandomGenerator::randomBit() {
    return uniformInt(uint32_t(0), uint32_t(1));
}

uint64_t RandomGenerator::pickRandomIndex(uint64_t n) {
    if (n == 0) {
        throw std::invalid_argument("Cannot pick from zero items");
    }
    return uniformInt(uint64_t(0), n - 1);
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    uint8_t* bytes = static_cast<uint8_t*>(data);
    for (size_t i = 0; i < size; ++i) {
        bytes[i] = static_cast<uint8_t>(uniformInt(uint32_t(0), uint32_t(255)));
    }
}

std::vector<uint32_t> RandomGenerator::uniformIntBatch(size_t count, uint32_t min, uint32_t max) {
    std::vector<uint32_t> results;
    results.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        results.push_back(uniformInt(min, max));
    }
    return results;
}

std::vector<double> RandomGenerator::uniformRealBatch(size_t count, double min, double max) {
    std::vector<double> results;
    results.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        results.push_back(uniformReal(min, max));
    }
    return results;
}

std::vector<float> RandomGenerator::uniformFloatBatch(size_t count, float min, float max) {
    std::vector<float> results;
    results.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        results.push_back(uniformReal(min, max));
    }
    return results;
}

std::vector<bool> RandomGenerator::bernoulliBatch(size_t count, double p) {
    std::vector<bool> results;
    results.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        results.push_back(bernoulli(p));
    }
    return results;
}

void RandomGenerator::resetWithSeed(uint64_t seed) {
    seed(seed);
}

double RandomGenerator::calculateMean(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    double sum = 0.0;
    for (double v : values) sum += v;
    return sum / values.size();
}

double RandomGenerator::calculateStdDev(const std::vector<double>& values) const {
    if (values.size() <= 1) return 0.0;
    double mean = calculateMean(values);
    double sumSq = 0.0;
    for (double v : values) {
        double diff = v - mean;
        sumSq += diff * diff;
    }
    return std::sqrt(sumSq / (values.size() - 1));
}

bool RandomGenerator::isDeterministic() const {
    return pImpl && pImpl->currentSeed != std::random_device{}();
}

std::vector<double> RandomGenerator::normalBatch(size_t count, double mean, double stddev) {
    std::vector<double> results;
    results.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        results.push_back(normal(mean, stddev));
    }
    return results;
}

} // namespace nlm
