#pragma once

#include <cstdint>
#include <random>
#include <functional>

namespace nlm {

/**
 * @file Random.hpp
 * 
 * Deterministic random number generator for NLM
 * 
 * This file provides a thread-safe, deterministic random number generator
 * that supports explicit seeding for reproducible experiments. The generator
 * is based on the MT19937 Mersenne Twister algorithm and provides a wide
 * range of probability distributions for neural simulation and algorithm needs.
 * 
 * Key features:
 * - Thread-safe operations with internal engine locking
 * - Explicit seeding for reproducible experiments
 * - Multiple probability distributions (uniform, normal, Bernoulli, Poisson)
 * - Container operations (shuffle, pick random element)
 * - Advanced engine access for specialized use cases
 * - Compatibility with C++ standard library algorithms
 * 
 * @note Default constructor uses system time for non-deterministic seeding
 * @note All operations are thread-safe due to internal synchronization
 * @note Used extensively in neural initialization, plasticity, and learning
 * 
 * @see Config.hpp - For reproducibility configuration parameters
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class RandomGenerator
 * 
 * Thread-safe deterministic random number generator.
 * 
 * This class provides a comprehensive random number generation interface
 * based on the MT19937 Mersenne Twister algorithm. It is designed for
 * neural simulations where reproducibility is critical, allowing
 * experiments to be repeated with identical random sequences when
 * the same seed is used.
 * 
 * The generator is thread-safe and can be used concurrently by multiple
 * threads without external synchronization. Each instance maintains its
 * own random state.
 * 
 * @note Used in:
 *       - Neural network initialization
 *       - Synaptic plasticity (STDP, Hebbian)
 *       - Learning rate scheduling
 *       - Weight initialization
 *       - Random sampling for algorithms
 *       - Monte Carlo methods
 * 
 * @see RandomGenerator::seed() - For reproducible experiments
 * @see uniformInt() - For integer random values
 * @see normal() - For Gaussian random values
 * 
 * @author NLM Development Team
 */
class RandomGenerator {
public:
    /**
     * @brief Result type for random number generation
     * 
     * Alias for the standard result type used by the underlying
     * Mersenne Twister engine.
     */
    using result_type = uint32_t;
    
    /**
     * @brief Constructor with explicit seed for reproducibility
     * 
     * Creates a random number generator with a specific seed, ensuring
     * deterministic behavior across runs. This is essential for
     * reproducible scientific experiments and debugging.
     * 
     * @param seed 64-bit seed value for random number generator
     * @pre seed should be a high-quality random value
     * 
     * @note Using the same seed produces identical random sequences
     * @note Seed value 0 is supported but produces predictable sequences
     * @see seed() - For reseeding an existing generator
     * @see RandomGenerator() - For time-based random seeding
     */
    explicit RandomGenerator(uint64_t seed);
    
    /**
     * @brief Default constructor with time-based seed
     * 
     * Creates a random number generator with a seed based on the current
     * system time, providing non-deterministic behavior suitable for
     * applications requiring true randomness.
     * 
     * @note Generates different sequences across program runs
     * @note Used for applications where reproducibility is not required
     * @see RandomGenerator(uint64_t) - For reproducible seeding
     */
    RandomGenerator();
    
    /**
     * @brief Destructor
     * 
     * Cleans up random generator resources and performs any necessary
     * cleanup operations.
     */
    ~RandomGenerator();
    
    // Disable copying, enable moving
    RandomGenerator(const RandomGenerator&) = delete;
    RandomGenerator& operator=(const RandomGenerator&) = delete;
    RandomGenerator(RandomGenerator&&) noexcept;
    RandomGenerator& operator=(RandomGenerator&&) noexcept;
    
    /**
     * @brief Set random seed for reproducible sequences
     * 
     * Changes the random seed of an existing generator, allowing
     * regeneration of the same random sequence. This is useful for
     * resetting random state during experiments.
     * 
     * @param seed 64-bit seed value
     * 
     * @pre Generator should not be used concurrently when calling seed()
     * @post Random sequence resets to deterministic sequence based on seed
     * @post All subsequent random operations use new seed
     * 
     * @note After calling seed(), the generator produces the same
     *       sequence as if it were created with that seed initially
     * @see RandomGenerator(uint64_t) - Initial seeding constructor
     */
    void seed(uint64_t seed);
    
    /**
     * @brief Get current random seed
     * 
     * Retrieves the current seed value that determines the random
     * sequence. Useful for saving and restoring random state.
     * 
     * @return Current 64-bit seed value
     * 
     * @note Seed is only meaningful for reproducible sequences
     * @see seed() - For setting the random seed
     */
    uint64_t getSeed() const;
    
    /**
     * @brief Generate random integer in specified range
     * 
     * Produces a uniformly distributed random 32-bit integer
     * within the inclusive range [min, max]. This method uses
     * rejection sampling to ensure perfect uniformity.
     * 
     * @param min Lower bound of range (inclusive)
     * @param max Upper bound of range (inclusive)
     * @return Random integer in [min, max]
     * 
     * @pre min <= max
     * @post Returned value is within specified bounds
     * @complexity O(1) average time for reasonable ranges
     * 
     * @note Uses modulo operation with rejection sampling for uniformity
     * @see uniformReal() - For floating-point uniform distribution
     * @see uniformInt(uint64_t, uint64_t) - For 64-bit version
     */
    uint32_t uniformInt(uint32_t min, uint32_t max);
    
    /**
     * @brief Generate random 64-bit integer in specified range
     * 
     * Produces a uniformly distributed random 64-bit integer
     * within the inclusive range [min, max]. Uses rejection sampling
     * to ensure perfect uniformity for large ranges.
     * 
     * @param min Lower bound of range (inclusive)
     * @param max Upper bound of range (inclusive)
     * @return Random 64-bit integer in [min, max]
     * 
     * @pre min <= max
     * @post Returned value is within specified bounds
     * @complexity O(1) average time for reasonable ranges
     * 
     * @note More computationally expensive than 32-bit version
     * @see uniformInt(uint32_t, uint32_t) - For 32-bit version
     */
    uint64_t uniformInt(uint64_t min, uint64_t max);
    
    /**
     * @brief Generate random double in specified range
     * 
     * Produces a uniformly distributed random double-precision
     * floating-point number within the range [min, max).
     * 
     * @param min Lower bound of range (inclusive)
     * @param max Upper bound of range (exclusive)
     * @return Random double in [min, max)
     * 
     * @pre min <= max
     * @post Returned value is within specified bounds
     * @complexity O(1)
     * 
     * @note Uses std::uniform_real_distribution internally
     * @see uniformReal(float, float) - For single-precision version
     */
    double uniformReal(double min, double max);
    
    /**
     * @brief Generate random float in specified range
     * 
     * Produces a uniformly distributed random single-precision
     * floating-point number within the range [min, max).
     * 
     * @param min Lower bound of range (inclusive)
     * @param max Upper bound of range (exclusive)
     * @return Random float in [min, max)
     * 
     * @pre min <= max
     * @post Returned value is within specified bounds
     * @complexity O(1)
     * 
     * @note Uses std::uniform_real_distribution internally
     * @see uniformReal(double, double) - For double-precision version
     */
    float uniformReal(float min, float max);
    
    /**
     * @brief Generate random value from normal distribution
     * 
     * Produces a random value from a Gaussian (normal) distribution
     * with specified mean and standard deviation. Uses the Box-Muller
     * transform or Ziggurat algorithm depending on implementation.
     * 
     * @param mean Mean of the normal distribution
     * @param stddev Standard deviation of the distribution
     * @return Random value from normal distribution
     * 
     * @pre stddev >= 0
     * @post Returned value has specified mean and standard deviation
     * @complexity O(1)
     * 
     * @note Approximates standard normal distribution (mean=0, stddev=1)
     *       when called with standard parameters
     * @see bernoulli() - For Bernoulli distribution
     */
    double normal(double mean, double stddev);
    
    /**
     * @brief Generate random float from normal distribution
     * 
     * Produces a random single-precision value from a Gaussian
     * (normal) distribution with specified mean and standard deviation.
     * 
     * @param mean Mean of the normal distribution
     * @param stddev Standard deviation of the distribution
     * @return Random float from normal distribution
     * 
     * @pre stddev >= 0
     * @post Returned value has specified mean and standard deviation
     * @complexity O(1)
     */
    float normal(float mean, float stddev);
    
    /**
     * @brief Generate Bernoulli distributed random value
     * 
     * Produces a random boolean value where true occurs with
     * probability p. This models a Bernoulli trial.
     * 
     * @param p Probability of true value (0.0 to 1.0)
     * @return true with probability p, false otherwise
     * 
     * @pre 0.0 <= p <= 1.0
     * @post Returns true with specified probability
     * @complexity O(1)
     * 
     * @note p = 0.5 is the default value
     * @see uniformReal() - Alternative implementation
     */
    bool bernoulli(double p = 0.5);
    
    /**
     * @brief Generate Poisson distributed random value
     * 
     * Produces a random integer from a Poisson distribution
     * with specified rate parameter λ (lambda). The Poisson
     * distribution models the number of events in a fixed interval.
     * 
     * @param lambda Rate parameter (events per interval)
     * @return Random count from Poisson distribution
     * 
     * @pre lambda >= 0
     * @post Returns count with expected value lambda
     * @complexity O(lambda) but optimized for common cases
     * 
     * @note Used for modeling rare events and neural spike counts
     * @see exponential() - Related exponential distribution
     */
    uint32_t poisson(double lambda);
    
    /**
     * @brief Generate exponential distributed random value
     * 
     * Produces a random floating-point value from an exponential
     * distribution with specified rate parameter. Models waiting
     * times between events in a Poisson process.
     * 
     * @param lambda Rate parameter (events per unit time)
     * @return Random waiting time from exponential distribution
     * 
     * @pre lambda > 0
     * @post Returns waiting time with mean 1/lambda
     * @complexity O(1)
     * 
     * @note Models inter-arrival times in neural spike trains
     * @see poisson() - Related Poisson distribution
     */
    double exponential(double lambda);
    
    /**
     * @brief Generate random boolean value
     * 
     * Produces a uniformly distributed random boolean value where
     * true and false are equally likely.
     * 
     * @return Random boolean value (true or false)
     * 
     * @post Returns true with probability 0.5
     * @complexity O(1)
     * 
     * @note Equivalent to bernoulli(0.5)
     */
    bool randomBool();
    
    /**
     * @brief Generate random bit value
     * 
     * Produces a uniformly distributed random single bit (0 or 1).
     * This is more efficient than randomBool() for bit-level operations.
     * 
     * @return Random bit (0 or 1)
     * 
     * @post Returns 0 or 1 with equal probability
     * @complexity O(1)
     */
    uint32_t randomBit();
    
    /**
     * @brief Shuffle container elements uniformly
     * 
     * Randomly permutes the elements of a container using the Fisher-Yates
     * shuffle algorithm. All n! permutations are equally likely.
     * 
     * @tparam Container Type of container to shuffle (must support random access)
     * @param container Container to be shuffled (modified in place)
     * 
     * @pre Container must support bidirectional iterators
     * @pre Container size must be representable as size_t
     * @post Elements of container are randomly permuted
     * @complexity O(n) where n is container size
     * 
     * @note Thread-safe - internal engine access is synchronized
     * @note Stable - equal elements may change relative order
     * @see pickRandom() - For selecting random elements
     */
    template<typename Container>
    void shuffle(Container& container);
    
    /**
     * @brief Pick random element from container
     * 
     * Selects a single element from a container with uniform probability.
     * The container must not be empty, or an exception will be thrown.
     
     * @tparam Container Type of container to select from
     * @param container Container to select from (must not be empty)
     * @return Reference to randomly selected element
     * 
     * @pre container must not be empty
     * @post Selected element is uniformly random
     * @complexity O(n) for standard containers, O(1) for weighted selections
     * 
     * @throw std::runtime_error if container is empty
     * @note Thread-safe - internal engine access is synchronized
     * @see shuffle() - For shuffling all elements
     * @see pickRandomIndex() - For selecting random indices
     */
    template<typename Container>
    auto pickRandom(const Container& container) -> typename Container::const_reference;
    
    /**
     * @brief Pick random index from 0 to n-1
     * 
     * Generates a uniformly distributed random index in the range
     * [0, n-1] where n is the number of elements. Used for random
     * access selection and container operations.
     * 
     * @param n Maximum index value + 1 (must be > 0)
     * @return Random index in [0, n-1]
     * 
     * @pre n > 0
     * @post Returned index is uniformly distributed
     * @complexity O(1)
     * 
     * @note n must be convertible to size_t
     * @see pickRandom() - For selecting random elements
     */
    uint64_t pickRandomIndex(uint64_t n);
    
    /**
     * @brief Fill memory range with random bytes
     * 
     * Fills a memory buffer with cryptographically secure random
     * bytes using the underlying random engine. This is useful for
     * random key generation, nonces, and other security-related
     * operations that require high-quality randomness.
     * 
     * @param data Pointer to buffer to fill
     * @param size Number of bytes to fill
     * 
     * @pre data must be valid and writable
     * @pre size must be representable as size_t
     * @post Buffer contains random bytes
     * @complexity O(size)
     * 
     * @note Uses the random engine's native distribution
     * @see randomBit() - For bit-level random operations
     */
    void fillBytes(void* data, size_t size);
    
    /**
     * @brief Access underlying random engine
     * 
     * Provides access to the underlying MT19937 random engine
     * for advanced use cases. This allows custom random distributions
     * and other advanced engine operations.
     * 
     * @return Reference to internal random engine
     * 
     * @note Returned engine is already seeded
     * @note Thread-safe - engine access is synchronized
     * @note Modifying the engine directly may affect determinism
     * 
     * @see operator()() - Standard function call operator
     */
    std::mt19937_64& engine();
    
    /**
     * @brief Access underlying random engine (const version)
     * 
     * Const version of engine() for read-only access to the
     * underlying random engine.
     * 
     * @return Const reference to internal random engine
     * 
     * @see engine() - Non-const version
     */
    const std::mt19937_64& engine() const;
    
    /**
     * @brief Get minimum possible result value
     * 
     * Returns the minimum value that can be returned by the
     * random number generator, typically 0.
     * 
     * @return Minimum result value
     */
    static constexpr result_type min() { return std::mt19937_64::min(); }
    
    /**
     * @brief Get maximum possible result value
     * 
     * Returns the maximum value that can be returned by the
     * random number generator, typically 2^32-1.
     * 
     * @return Maximum result value
     */
    static constexpr result_type max() { return std::mt19937_64::max(); }
    
    /**
     * @brief Function call operator for compatibility
     * 
     * Allows RandomGenerator to be used wherever a function
     * object with the signature result_type() is required.
     * This enables compatibility with C++ standard library
     * algorithms and functions.
     * 
     * @return Random result in range [min(), max()]
     * 
     * @note Can be used with std::bind, std::function, etc.
     * @see engine() - For direct engine access
     */
    result_type operator()() { return static_cast<result_type>(engine()()); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Template implementation of shuffle method
 * 
 * Implementation of the shuffle method template. This is provided
 * here rather than in the class definition to keep the header clean
 * while still providing the functionality.
 * 
 * @tparam Container Type of container to shuffle
 * @param container Container to be shuffled
 * 
 * @post All permutations are equally likely
 * @complexity O(n) where n is container size
 */
template<typename Container>
void RandomGenerator::shuffle(Container& container) {
    std::shuffle(container.begin(), container.end(), engine());
}

/**
 * @brief Template implementation of pickRandom method
 * 
 * Implementation of the pickRandom method template. This is provided
 * here rather than in the class definition to keep the header clean
 * while still providing the functionality. Includes bounds checking.
 * 
 * @tparam Container Type of container to select from
 * @param container Container to select from
 * @return Reference to randomly selected element
 * 
 * @throw std::runtime_error if container is empty
 * @complexity O(n) for standard containers
 */
template<typename Container>
auto RandomGenerator::pickRandom(const Container& container) -> typename Container::const_reference {
    if (container.empty()) {
        throw std::runtime_error("Cannot pick from empty container");
    }
    auto it = container.begin();
    std::advance(it, pickRandomIndex(container.size()));
    return *it;
}

} // namespace nlm
