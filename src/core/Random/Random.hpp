#pragma once

#include <cstdint>
#include <random>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <type_traits>
#include <iterator>

namespace nlm {

/**
 * @class RandomGenerator
 * @brief Deterministic random number generator with explicit seeding
 * 
 * This class provides a thread-safe, deterministic random number generator
 * using the Mersenne Twister algorithm (mt19937_64). It supports explicit
 * seeding for reproducible experiments and includes a wide variety of
 * probability distribution functions.
 * 
 * Key features:
 * - Thread-safe random number generation
 * - Explicit seeding for reproducible experiments
 * - Multiple probability distributions (uniform, normal, exponential, etc.)
 * - Container operations (shuffle, random pick)
 * - Memory-efficient implementation
 * 
 * The generator uses std::mt19937_64 internally, which has an extremely
 * long period (2^19937 - 1) and excellent statistical properties.
 * 
 * @note Thread-safe for concurrent access during simulation
 * @note Exception-safe with proper resource management
 * @warning All seeding should be done before simulation starts
 * @warning Not cryptographically secure
 */
class RandomGenerator {
public:
    using result_type = uint32_t;
    
    /**
     * @brief Constructs a deterministic random generator with explicit seed
     * 
     * @param seed Seed value for deterministic random sequence
     * 
     * @note Same seed produces identical random sequence
     * @note For non-deterministic seeding, use default constructor
     */
    explicit RandomGenerator(uint64_t seed);
    
    /**
     * @brief Constructs with time-based seed (non-deterministic)
     * 
     * @note Provides different random sequences on each run
     * @note Use only for non-experimental applications
     */
    RandomGenerator();
    
    /**
     * @brief Destructor cleans up internal implementation
     */
    ~RandomGenerator();
    
    /**
     * @brief Copy constructor deleted to prevent copying (Pimpl idiom)
     */
    RandomGenerator(const RandomGenerator&) = delete;
    
    /**
     * @brief Copy assignment operator deleted to prevent copying
     */
    RandomGenerator& operator=(const RandomGenerator&) = delete;
    
    /**
     * @brief Move constructor transfers ownership of internal resources
     * 
     * @param other Generator to move from (leaves in valid but unspecified state)
     */
    RandomGenerator(RandomGenerator&&) noexcept;
    
    /**
     * @brief Move assignment operator transfers ownership of internal resources
     * 
     * @param other Generator to move from (leaves in valid but unspecified state)
     * @return Reference to this generator
     */
    RandomGenerator& operator=(RandomGenerator&&) noexcept;
    
    /**
     * @brief Reseeds the random generator with new seed
     * 
     * @param seed New seed value for random sequence
     * 
     * @note Reseeding changes the entire random sequence
     * @warning Should not be called during active simulation for reproducible results
     */
    void seed(uint64_t seed);
    
    /**
     * @brief Gets the current seed value
     * 
     * @return uint64_t Current seed value
     */
    uint64_t getSeed() const;
    
    /**
     * @brief Generates uniform integer in [min, max] (inclusive)
     * 
     * @param min Minimum value (inclusive)
     * @param max Maximum value (inclusive)
     * @return uint32_t Random integer in [min, max]
     * 
     * @note Uses std::uniform_int_distribution
     * @exception std::invalid_argument if min > max
     */
    uint32_t uniformInt(uint32_t min, uint32_t max);
    
    /**
     * @brief Generates uniform integer in [min, max] (inclusive)
     * 
     * @param min Minimum value (inclusive)
     * @param max Maximum value (inclusive)
     * @return uint64_t Random integer in [min, max]
     * 
     * @note Uses std::uniform_int_distribution
     * @exception std::invalid_argument if min > max
     */
    uint64_t uniformInt(uint64_t min, uint64_t max);
    
    /**
     * @brief Generates uniform real number in [min, max) (half-open interval)
     * 
     * @param min Minimum value (inclusive)
     * @param max Maximum value (exclusive)
     * @return double Random real in [min, max)
     * 
     * @note Uses std::uniform_real_distribution
     * @exception std::invalid_argument if min >= max
     */
    double uniformReal(double min, double max);
    
    /**
     * @brief Generates uniform float number in [min, max) (half-open interval)
     * 
     * @param min Minimum value (inclusive)
     * @param max Maximum value (exclusive)
     * @return float Random float in [min, max)
     * 
     * @note Uses std::uniform_real_distribution
     * @exception std::invalid_argument if min >= max
     */
    float uniformReal(float min, float max);
    
    /**
     * @brief Generates normal (Gaussian) distribution
     * 
     * @param mean Mean of the distribution
     * @param stddev Standard deviation of the distribution
     * @return double Random value from N(mean, stddev^2)
     * 
     * @note Uses std::normal_distribution
     * @exception std::invalid_argument if stddev < 0
     */
    double normal(double mean, double stddev);
    
    /**
     * @brief Generates normal (Gaussian) distribution as float
     * 
     * @param mean Mean of the distribution
     * @param stddev Standard deviation of the distribution
     * @return float Random value from N(mean, stddev^2)
     * 
     * @note Uses std::normal_distribution
     * @exception std::invalid_argument if stddev < 0
     */
    float normal(float mean, float stddev);
    
    /**
     * @brief Generates Bernoulli distribution (true/false with probability p)
     * 
     * @param p Probability of returning true (default: 0.5)
     * @return bool Random boolean with probability p of being true
     * 
     * @note Uses std::bernoulli_distribution
     * @exception std::invalid_argument if p not in [0, 1]
     */
    bool bernoulli(double p = 0.5);
    
    /**
     * @brief Generates Poisson distribution
     * 
     * @param lambda Rate parameter (lambda > 0)
     * @return uint32_t Random value from Poisson(lambda)
     * 
     * @note Uses std::poisson_distribution
     * @exception std::invalid_argument if lambda < 0
     */
    uint32_t poisson(double lambda);
    
    /**
     * @brief Generates exponential distribution
     * 
     * @param lambda Rate parameter (lambda > 0)
     * @return double Random value from Exp(lambda)
     * 
     * @note Uses std::exponential_distribution
     * @exception std::invalid_argument if lambda < 0
     */
    double exponential(double lambda);
    
    /**
     * @brief Generates random boolean with 50% probability
     * 
     * @return bool Random boolean (true or false)
     * 
     * @note Equal probability of true or false
     */
    bool randomBool();
    
    /**
     * @brief Generates random bit (0 or 1 with equal probability)
     * 
     * @return uint32_t Random bit (0 or 1)
     */
    uint32_t randomBit();
    
    /**
     * @brief Shuffles elements in container using Fisher-Yates algorithm
     * 
     * @tparam Container Random Access container type
     * @param container Container to shuffle (modified in-place)
     * 
     * @note Uses std::shuffle with generator
     * @note Requires container to support begin(), end(), and random access iterator
     */
    template<typename Container>
    void shuffle(Container& container);
    
    /**
     * @brief Picks random element from container
     * 
     * @tparam Container Container type
     * @param container Const reference to container
     * @return typename Container::const_reference Reference to random element
     * 
     * @return Throws std::runtime_error if container is empty
     * 
     * @note Uses std::advance for efficient random selection
     * @note Requires container to support size() and iterators
     */
    template<typename Container>
    auto pickRandom(const Container& container) -> typename Container::const_reference;
    
    /**
     * @brief Picks random index from 0 to n-1
     * 
     * @param n Maximum index (exclusive)
     * @return uint64_t Random index in [0, n-1]
     * 
     * @return Throws std::invalid_argument if n == 0
     * 
     * @note Uses std::uniform_int_distribution
     */
    uint64_t pickRandomIndex(uint64_t n);
    
    /**
     * @brief Fills memory buffer with random bytes
     * 
     * @param data Pointer to buffer to fill
     * @param size Size of buffer in bytes
     * 
     * @note Uses std::uniform_int_distribution<>
     * @note Suitable for non-cryptographic random data
     */
    void fillBytes(void* data, size_t size);
    
    /**
     * @brief Gets reference to underlying random engine for advanced use
     * 
     * @return std::mt19937_64& Reference to internal engine
     * @note For advanced users who need direct engine access
     */
    std::mt19937_64& engine();
    
    /**
     * @brief Gets const reference to underlying random engine
     * 
     * @return const std::mt19937_64& Const reference to internal engine
     */
    const std::mt19937_64& engine() const;
    
    /**
     * @brief Gets minimum possible value from the distribution
     * 
     * @return result_type Minimum possible value
     */
    static constexpr result_type min() { return std::mt19937_64::min(); }
    
    /**
     * @brief Gets maximum possible value from the distribution
     * 
     * @return result_type Maximum possible value
     */
    static constexpr result_type max() { return std::mt19937_64::max(); }
    
    /**
     * @brief Random number generation operator
     * 
     * @return result_type Random value
     * 
     * @note Provides compatibility with std::generate_canonical and other std functions
     */
    result_type operator()() { return static_cast<result_type>(engine()()); }
    
private:
    /**
     * @brief Internal implementation details (Pimpl idiom)
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     */
    Impl* pImpl;
};

// Template implementations with improved error handling
namespace {

template<typename Container>
void validateContainer(const Container& container, const std::string& operation) {
    if (container.empty()) {
        throw std::runtime_error("Cannot " + operation + " empty container");
    }
}

} // anonymous namespace

template<typename Container>
void RandomGenerator::shuffle(Container& container) {
    static_assert(std::is_same_v<typename Container::iterator, typename Container::const_iterator>,
                  "Container must support iterator arithmetic");
    
    validateContainer(container, "shuffle container");
    std::shuffle(container.begin(), container.end(), engine());
}

template<typename Container>
auto RandomGenerator::pickRandom(const Container& container) -> typename Container::const_reference {
    validateContainer(container, "pick random element from");
    
    auto it = container.begin();
    uint64_t index = pickRandomIndex(container.size());
    std::advance(it, static_cast<typename std::iterator_traits<decltype(it)>::difference_type>(index));
    return *it;
}

} // namespace nlm
