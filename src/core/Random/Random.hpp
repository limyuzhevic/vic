#pragma once

#include <cstdint>
#include <random>
#include <functional>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <cmath>

namespace nlm {

// Enhanced Random Generator with advanced distributions and better seeding strategies
class RandomGenerator {
public:
    using result_type = uint32_t;
    
    // Create with explicit seed
    explicit RandomGenerator(uint64_t seed);
    
    // Create with time-based seed (not deterministic)
    RandomGenerator();
    
    // Create with entropy-based seed (most secure)
    static RandomGenerator createSeeded();
    
    // Create with seed from system time and process ID (high entropy)
    static RandomGenerator createHighEntropySeed();
    
    // Create with seed from hardware entropy if available
    static RandomGenerator createEntropySeed();
    
    ~RandomGenerator();
    
    // Disable copying
    RandomGenerator(const RandomGenerator&) = delete;
    RandomGenerator& operator=(const RandomGenerator&) = delete;
    
    // Enable moving
    RandomGenerator(RandomGenerator&&) noexcept;
    RandomGenerator& operator=(RandomGenerator&&) noexcept;
    
    // Seed operations
    void seed(uint64_t seed);
    void seedFromSystem();
    void seedFromEntropy();
    uint64_t getSeed() const;
    
    // Sequence management
    void advanceSequence(uint64_t n);
    uint64_t getSequencePosition() const;
    
    // Phase 5 checkpoint support: serialize the full mt19937_64 engine
    // state (not just the seed — reseeding would replay the whole
    // random sequence from the start, so a reloaded brain would repeat
    // its earlier random choices). The format is the standard library
    // stream format for mt19937_64; restoreState returns false on a
    // parse failure and leaves the generator untouched.
    std::string serializeState() const;
    bool restoreState(const std::string& state);
    
    // Basic random number generation
    result_type operator()();
    
    // Random number generation
    // Uniform integer in [min, max]
    uint32_t uniformInt(uint32_t min, uint32_t max);
    uint64_t uniformInt(uint64_t min, uint64_t max);
    
    // Uniform real in [min, max)
    double uniformReal(double min, double max);
    float uniformReal(float min, float max);
    
    // Normal distribution
    double normal(double mean, double stddev);
    float normal(float mean, float stddev);
    
    // Bernoulli distribution
    bool bernoulli(double p = 0.5);
    
    // Poisson distribution
    uint32_t poisson(double lambda);
    
    // Exponential distribution
    double exponential(double lambda);
    
    // Advanced distributions
    // Gamma distribution
    double gamma(double shape, double scale = 1.0);
    float gamma(float shape, float scale = 1.0f);
    
    // Beta distribution
    double beta(double alpha, double beta);
    float beta(float alpha, float beta);
    
    // Chi-squared distribution
    double chiSquared(double degreesOfFreedom);
    float chiSquared(float degreesOfFreedom);
    
    // Student's t-distribution
    double studentT(double degreesOfFreedom);
    float studentT(float degreesOfFreedom);
    
    // Weibull distribution
    double weibull(double shape, double scale = 1.0);
    float weibull(float shape, float scale = 1.0f);
    
    // Cauchy distribution
    double cauchy(double location = 0.0, double scale = 1.0);
    float cauchy(float location = 0.0f, float scale = 1.0f);
    
    // Log-normal distribution
    double logNormal(double mean, double stddev);
    float logNormal(float mean, float stddev);
    
    // Random boolean
    bool randomBool();
    
    // Random bit
    uint32_t randomBit();
    
    // Random integer with custom probability distribution
    uint64_t randomWeighted(const std::vector<double>& probabilities);
    
    // Random integer from discrete set
    template<typename T>
    T randomChoice(const std::vector<T>& choices);
    
    // Random index from weighted choices
    template<typename Container>
    auto randomWeightedChoice(const Container& items) -> typename Container::const_reference;
    
    // Generate random permutation
    template<typename Container>
    void permute(Container& container);
    
    // Statistical tests
    double calculateMean(const std::vector<double>& values) const;
    double calculateStdDev(const std::vector<double>& values, double mean = 0.0) const;
    double calculateVariance(const std::vector<double>& values, double mean = 0.0) const;
    
    // Generate pseudo-random sequence
    std::vector<double> generateSequence(size_t count, double mean = 0.0, double stddev = 1.0);
    
    // Shuffle container
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
    
    // Probability distribution utilities
    static double probabilityDensity(const std::string& distribution, double x, double param1 = 0, double param2 = 0);
    static double cumulativeDistribution(const std::string& distribution, double x, double param1 = 0, double param2 = 0);
    
    // Statistical analysis
    static double correlationCoefficient(const std::vector<double>& x, const std::vector<double>& y);
    static double covariance(const std::vector<double>& x, const std::vector<double>& y, double meanX = 0.0, double meanY = 0.0);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Template implementations
// Utility function to generate gamma random variable using Marsaglia method
template<typename T>
T gammaRandom(T shape, T scale = T(1.0));

// Template function to generate beta random variable using transformation method
template<typename T>
T betaRandom(T alpha, T beta);

// Template function to generate chi-squared random variable using gamma method
template<typename T>
T chiSquaredRandom(T degreesOfFreedom);

// Template function to generate t-distribution random variable using transformation method
template<typename T>
T studentTRandom(T degreesOfFreedom);

// Template function to generate weibull random variable
template<typename T>
T weibullRandom(T shape, T scale = T(1.0));

// Template function to generate log-normal random variable
template<typename T>
T logNormalRandom(T mean, T stddev);

// Template implementations for container operations
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

template<typename T>
T RandomGenerator::randomChoice(const std::vector<T>& choices) {
    if (choices.empty()) {
        throw std::runtime_error("Cannot choose from empty container");
    }
    return choices[pickRandomIndex(choices.size())];
}

template<typename Container>
auto RandomGenerator::randomWeightedChoice(const Container& items) -> typename Container::const_reference {
    if (items.empty()) {
        throw std::runtime_error("Cannot choose from empty container");
    }
    return *std::next(items.begin(), pickRandomIndex(items.size()));
}

// Helper functions
template<typename T>
T gammaRandom(T shape, T scale) {
    if (shape <= 0 || scale <= 0) {
        throw std::invalid_argument("Shape and scale parameters must be positive");
    }
    
    // Marsaglia method for gamma distribution
    double d = shape - 1.0 / 3.0;
    double c = 1.0 / std::sqrt(9.0 * d);
    
    while (true) {
        double x = gammaRandom<T>(d);
        double u = uniformReal(0.0, 1.0);
        
        if (u + c * std::exp(-x / 3.0) > 0) {
            return static_cast<T>(scale * std::pow(x, 1.0 / 3.0));
        }
    }
}

template<typename T>
T betaRandom(T alpha, T beta) {
    if (alpha <= 0 || beta <= 0) {
        throw std::invalid_argument("Alpha and beta parameters must be positive");
    }
    
    T gamma1 = gammaRandom(alpha);
    T gamma2 = gammaRandom(beta);
    
    return gamma1 / (gamma1 + gamma2);
}

template<typename T>
T chiSquaredRandom(T degreesOfFreedom) {
    return gammaRandom(degreesOfFreedom / 2.0, T(2.0));
}

template<typename T>
T studentTRandom(T degreesOfFreedom) {
    T normal = normal(T(0.0), T(1.0));
    T chi2 = chiSquaredRandom(degreesOfFreedom);
    
    return normal / std::sqrt(chi2 / degreesOfFreedom);
}

template<typename T>
T weibullRandom(T shape, T scale) {
    T u = uniformReal(T(0.0), T(1.0));
    return scale * std::pow(-std::log(u), T(1.0) / shape);
}

template<typename T>
T logNormalRandom(T mean, T stddev) {
    T normal = normal(mean, stddev);
    return std::exp(normal);
}

} // namespace nlm
