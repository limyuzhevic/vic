#pragma once

#include <cstdint>
#include <random>
#include <functional>
#include <stdexcept>
#include <string>
#include <system_error>
#include "../Error/Error.hpp"

namespace nlm {

// Deterministic random number generator
// Supports explicit seeds for reproducible experiments

class RandomGeneratorException : public NLMException {
public:
    RandomGeneratorException(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : NLMException(ErrorCategory::System, code, ErrorSeverity::Error, message, context) {}
};

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
    
    // Random boolean
    bool randomBool();
    
    // Random bit
    uint32_t randomBit();
    
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
    
    // Operator for compatibility with std functions
    result_type operator()() { return static_cast<result_type>(engine()()); }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Template implementations
namespace detail {
    // Common exception helper for random operations
    template<typename ExceptionType = RandomGeneratorException>
    void throwRandomException(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext()) {
        throw ExceptionType(code, message, context);
    }
    
    // Check and throw if value is out of range
    template<typename T>
    void checkRange(const T& value, const T& min, const T& max, const std::string& operation) {
        if (value < min || value > max) {
            throwRandomException(ErrorCode::OutOfRange, "Random value " + std::to_string(value) + " is out of range for " + operation + " [" + 
                                std::to_string(min) + ", " + std::to_string(max) + "]");
        }
    }
    
    // Check container validity
    template<typename Container>
    void checkContainer(const Container& container, const std::string& operation) {
        if (container.empty()) {
            throwRandomException(ErrorCode::InvalidArgument, "Cannot " + operation + " empty container");
        }
    }
    
    // Safe random index selection with bounds checking
    template<typename Iterator>
    Iterator safeRandomSelect(Iterator begin, Iterator end) {
        if (begin == end) {
            throwRandomException(ErrorCode::InvalidArgument, "Cannot select from empty range");
        }
        size_t distance = std::distance(begin, end);
        if (distance == 0) {
            throwRandomException(ErrorCode::InvalidArgument, "Cannot select from empty range");
        }
        uint64_t index = pickRandomIndex(distance - 1);
        std::advance(begin, index);
        return begin;
    }
}

// Template implementations
template<typename Container>
void RandomGenerator::shuffle(Container& container) {
    try {
        if (container.empty()) {
            throw detail::throwRandomException(ErrorCode::InvalidArgument, "Cannot shuffle empty container");
        }
        std::shuffle(container.begin(), container.end(), engine());
    } catch (const RandomGeneratorException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        RandomGeneratorException genericEx(ErrorCode::UnknownError, std::string("Shuffle error: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

template<typename Container>
auto RandomGenerator::pickRandom(const Container& container) -> typename Container::const_reference {
    try {
        detail::checkContainer(container, "pickRandom");
        auto it = container.begin();
        std::advance(it, pickRandomIndex(container.size() - 1));
        return *it;
    } catch (const RandomGeneratorException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        RandomGeneratorException genericEx(ErrorCode::InvalidArgument, std::string("Pick random error: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

} // namespace nlm
