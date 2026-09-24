#include "Random.hpp"
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace nlm {

// Thread-local storage implementation
thread_local std::unique_ptr<RandomGenerator> RandomGenerator::threadLocalGenerator = nullptr;
thread_local uint64_t RandomGenerator::threadLocalSeed = 0;
std::shared_mutex RandomGenerator::seedMutex;
std::unordered_map<std::thread::id, uint64_t> RandomGenerator::threadSeeds;
const std::string RandomGenerator::DEFAULT_SEED_FILE = "nlm_seed.txt";

struct RandomGenerator::Impl {
    std::mt19937_64 gen;
    uint64_t currentSeed;
    DistributionParams distParams;
    std::mutex mutex;
    
    Impl(uint64_t seed) : gen(seed), currentSeed(seed) {
        // Initialize with neuroscience-appropriate defaults
        distParams.mean = 0.0;
        distParams.stddev = 1.0;
        distParams.lambda = 1.0;
        distParams.correlationTime = 1.0;
        distParams.p = 0.5;
    }
};

RandomGenerator::RandomGenerator(uint64_t seed) : pImpl(new Impl(seed)) {}

RandomGenerator::RandomGenerator() : pImpl(new Impl(std::random_device{}())) {}

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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->gen.seed(seed);
    pImpl->currentSeed = seed;
    
    // Update thread-local storage
    std::lock_guard<std::shared_mutex> threadLock(seedMutex);
    threadLocalSeed = seed;
    threadSeeds[std::this_thread::get_id()] = seed;
}

uint64_t RandomGenerator::getSeed() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentSeed;
}

uint64_t RandomGenerator::getSeedFromFile(const std::string& filepath) {
    std::lock_guard<std::shared_mutex> lock(seedMutex);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return 0; // Not found
    }
    
    uint64_t seed;
    file >> seed;
    return seed;
}

void RandomGenerator::saveSeedToFile(const std::string& filepath, uint64_t seed) {
    std::lock_guard<std::shared_mutex> lock(seedMutex);
    
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << seed;
    }
}

RandomGenerator& RandomGenerator::getThreadLocalGenerator() {
    if (!threadLocalGenerator) {
        // Check if this thread has a saved seed
        std::lock_guard<std::shared_mutex> lock(seedMutex);
        auto it = threadSeeds.find(std::this_thread::get_id());
        if (it != threadSeeds.end()) {
            threadLocalGenerator = std::make_unique<RandomGenerator>(it->second);
        } else {
            threadLocalGenerator = std::make_unique<RandomGenerator>();
        }
    }
    return *threadLocalGenerator;
}

void RandomGenerator::resetThreadLocalGenerator() {
    std::lock_guard<std::shared_mutex> lock(seedMutex);
    threadLocalGenerator.reset();
    threadLocalSeed = 0;
    threadSeeds.erase(std::this_thread::get_id());
}

uint32_t RandomGenerator::uniformInt(uint32_t min, uint32_t max) {
    if (min > max) {
        throw std::invalid_argument("min must be <= max");
    }
    if (min == max) {
        return min;
    }
    std::uniform_int_distribution<uint32_t> dist(min, max);
    std::lock_guard<std::mutex> lock(pImpl->mutex);
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return dist(pImpl->gen);
}

double RandomGenerator::uniformReal(double min, double max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    std::uniform_real_distribution<double> dist(min, max);
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return dist(pImpl->gen);
}

float RandomGenerator::uniformReal(float min, float max) {
    if (min >= max) {
        throw std::invalid_argument("min must be < max");
    }
    std::uniform_real_distribution<float> dist(min, max);
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return dist(pImpl->gen);
}

double RandomGenerator::normal(double mean, double stddev) {
    if (stddev <= 0) {
        throw std::invalid_argument("stddev must be > 0");
    }
    std::normal_distribution<double> dist(mean, stddev);
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return dist(pImpl->gen);
}

float RandomGenerator::normal(float mean, float stddev) {
    if (stddev <= 0) {
        throw std::invalid_argument("stddev must be > 0");
    }
    std::normal_distribution<float> dist(mean, stddev);
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return dist(pImpl->gen);
}

bool RandomGenerator::bernoulli(double p) {
    if (p < 0.0 || p > 1.0) {
        throw std::invalid_argument("p must be in [0, 1]");
    }
    std::bernoulli_distribution dist(p);
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return dist(pImpl->gen);
}

uint32_t RandomGenerator::poisson(double lambda) {
    if (lambda <= 0) {
        throw std::invalid_argument("lambda must be > 0");
    }
    std::poisson_distribution<uint32_t> dist(lambda);
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return dist(pImpl->gen);
}

double RandomGenerator::exponential(double lambda) {
    if (lambda <= 0) {
        throw std::invalid_argument("lambda must be > 0");
    }
    std::exponential_distribution<double> dist(lambda);
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return dist(pImpl->gen);
}

float RandomGenerator::exponential(float lambda) {
    if (lambda <= 0) {
        throw std::invalid_argument("lambda must be > 0");
    }
    std::exponential_distribution<float> dist(lambda);
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return dist(pImpl->gen);
}

double RandomGenerator::gaussianProcess(double correlationTime) {
    // Generate correlated noise using AR(1) process
    if (correlationTime < 0) {
        throw std::invalid_argument("correlationTime must be >= 0");
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // First component: white noise
    double whiteNoise = normal(0.0, 1.0);
    
    if (correlationTime == 0.0) {
        return whiteNoise;
    }
    
    // AR(1) process: x[t] = a * x[t-1] + sqrt(1-a^2) * whiteNoise
    // where a = exp(-1/correlationTime)
    double a = std::exp(-1.0 / correlationTime);
    double integratedNoise = normal(0.0, std::sqrt(1.0 - a * a));
    
    return a * previousGaussian_ + integratedNoise;
}

bool RandomGenerator::randomBool() {
    return bernoulli(0.5);
}

uint32_t RandomGenerator::randomBit() {
    return uniformInt(0, 1);
}

template<typename Container>
void RandomGenerator::shuffle(Container& container) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::shuffle(container.begin(), container.end(), pImpl->gen);
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

uint64_t RandomGenerator::pickRandomIndex(uint64_t n) {
    if (n == 0) {
        throw std::invalid_argument("Cannot pick from zero items");
    }
    return uniformInt(0, n - 1);
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    uint8_t* bytes = static_cast<uint8_t*>(data);
    for (size_t i = 0; i < size; ++i) {
        bytes[i] = static_cast<uint8_t>(uniformInt(0, 255));
    }
}

std::mt19937_64& RandomGenerator::engine() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->gen;
}

const std::mt19937_64& RandomGenerator::engine() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->gen;
}

void RandomGenerator::setDistributionParams(const DistributionParams& params) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->distParams = params;
}

RandomGenerator::DistributionParams RandomGenerator::getDistributionParams() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->distParams;
}

// Statistics implementation
namespace random {
    Statistics stats;
    std::mutex statsMutex;
    
    Statistics getStatistics() {
        std::lock_guard<std::mutex> lock(statsMutex);
        return stats;
    }
    
    void resetStatistics() {
        std::lock_guard<std::mutex> lock(statsMutex);
        stats = Statistics{};
    }
    
    std::vector<double> generateReproducibleSequence(size_t count, uint64_t seed) {
        RandomGenerator rng(seed);
        std::vector<double> sequence;
        sequence.reserve(count);
        
        for (size_t i = 0; i < count; ++i) {
            sequence.push_back(rng.uniformReal(0.0, 1.0));
        }
        
        return sequence;
    }
}

} // namespace nlm
