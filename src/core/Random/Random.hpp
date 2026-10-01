#pragma once

#include <cstdint>
#include <random>
#include <functional>

namespace nlm {

// Random number generation utilities
// Provides deterministic random number generation for reproducible simulations
// Supports multiple probability distributions and flexible seeding strategies
// Designed for scientific computing and neural network simulations

class RandomGenerator {
public:
    /**
     * @brief Type alias for random number result type
     * 
     * Uses 32-bit unsigned integers for most operations, with 64-bit variants
     * where necessary for larger ranges or precision.
     */
    using result_type = uint32_t;
    
    /**
     * @brief Constructor with explicit seed
     * 
     * Creates a deterministic random number generator with a specified seed.
     * The same seed will always produce the same sequence of random numbers,
     * which is essential for reproducible scientific experiments.
     * 
     * @param seed The seed value for the random number generator
     *             (0 uses system time, other values produce deterministic sequences)
     */
    explicit RandomGenerator(uint64_t seed);
    
    /**
     * @brief Default constructor with system-time seed
     * 
     * Creates a non-deterministic random number generator with a seed based
     * on the current system time. Use this constructor when you need randomness
     * that varies between program runs (not suitable for reproducible research).
     */
    RandomGenerator();
    
    /**
     * @brief Destructor
     */
    ~RandomGenerator();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * RandomGenerator objects cannot be copied to maintain deterministic
     * behavior and avoid resource management issues.
     */
    RandomGenerator(const RandomGenerator&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     */
    RandomGenerator& operator=(const RandomGenerator&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Efficiently transfers ownership of the internal random engine from another
     * RandomGenerator object. The source object becomes unusable after the move.
     * 
     * @param other The RandomGenerator object to move from
     */
    RandomGenerator(RandomGenerator&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Efficiently transfers ownership of the internal random engine from another
     * RandomGenerator object. The source object becomes unusable after the move.
     * 
     * @param other The RandomGenerator object to move from
     * @return Reference to this RandomGenerator object
     */
    RandomGenerator& operator=(RandomGenerator&&) noexcept;
    
    /**
     * @brief Set the seed for the random number generator
     * 
     * Reseeds the generator with a new seed value. This changes the sequence
     * of random numbers that will be produced. For deterministic simulations,
     * use the constructor with an explicit seed instead.
     * 
     * @param seed The new seed value (0 uses system time, other values produce
     *             deterministic sequences)
     */
    void seed(uint64_t seed);
    
    /**
     * @brief Get the current seed value
     * 
     * Returns the seed used to initialize the random number generator.
     * For objects created with the default constructor, this returns 0.
     * 
     * @return uint64_t The current seed value
     */
    uint64_t getSeed() const;
    
    /**
     * @brief Generate a uniform random integer in [min, max]
     * 
     * Generates a uniformly distributed random integer between min and max (inclusive)
     * using the underlying random number generator.
     * 
     * @tparam T The integer type (uint32_t or uint64_t)
     * @param min The minimum value (inclusive)
     * @param max The maximum value (inclusive)
     * @return T Random integer in [min, max]
     * @throws std::runtime_error if min > max
     */
    template<typename T>
    T uniformInt(T min, T max);
    
    /**
     * @brief Generate a uniform random integer in [min, max] (uint32_t version)
     * 
     * Convenience overload for 32-bit unsigned integers.
     * 
     * @param min The minimum value (inclusive)
     * @param max The maximum value (inclusive)
     * @return uint32_t Random integer in [min, max]
     */
    uint32_t uniformInt(uint32_t min, uint32_t max);
    
    /**
     * @brief Generate a uniform random integer in [min, max] (uint64_t version)
     * 
     * Convenience overload for 64-bit unsigned integers.
     * 
     * @param min The minimum value (inclusive)
     * @param max The maximum value (inclusive)
     * @return uint64_t Random integer in [min, max]
     */
    uint64_t uniformInt(uint64_t min, uint64_t max);
    
    /**
     * @brief Generate a uniform random real number in [min, max)
     * 
     * Generates a uniformly distributed random floating-point number between
     * min (inclusive) and max (exclusive) using the underlying random number generator.
     * 
     * @tparam T The floating-point type (float or double)
     * @param min The minimum value (inclusive)
     * @param max The maximum value (exclusive)
     * @return T Random floating-point number in [min, max)
     * @throws std::runtime_error if min >= max
     */
    template<typename T>
    T uniformReal(T min, T max);
    
    /**
     * @brief Generate a uniform random real number in [min, max) (float version)
     * 
     * Convenience overload for single-precision floating-point.
     * 
     * @param min The minimum value (inclusive)
     * @param max The maximum value (exclusive)
     * @return float Random floating-point number in [min, max)
     */
    float uniformReal(float min, float max);
    
    /**
     * @brief Generate a uniform random real number in [min, max) (double version)
     * 
     * Convenience overload for double-precision floating-point.
     * 
     * @param min The minimum value (inclusive)
     * @param max The maximum value (exclusive)
     * @return double Random floating-point number in [min, max)
     */
    double uniformReal(double min, double max);
    
    /**
     * @brief Generate a normally distributed random number
     * 
     * Generates a random number from a normal (Gaussian) distribution with the specified
     * mean and standard deviation using the Box-Muller transform or similar algorithm.
     * 
     * @tparam T The floating-point type (float or double)
     * @param mean The mean of the distribution (μ)
     * @param stddev The standard deviation of the distribution (σ)
     * @return T Normally distributed random number
     * @throws std::runtime_error if stddev < 0
     */
    template<typename T>
    T normal(T mean, T stddev);
    
    /**
     * @brief Generate a normally distributed random number (float version)
     * 
     * Convenience overload for single-precision floating-point.
     * 
     * @param mean The mean of the distribution (μ)
     * @param stddev The standard deviation of the distribution (σ)
     * @return float Normally distributed random number
     */
    float normal(float mean, float stddev);
    
    /**
     * @brief Generate a normally distributed random number (double version)
     * 
     * Convenience overload for double-precision floating-point.
     * 
     * @param mean The mean of the distribution (μ)
     * @param stddev The standard deviation of the distribution (σ)
     * @return double Normally distributed random number
     */
    double normal(double mean, double stddev);
    
    /**
     * @brief Generate a Bernoulli-distributed random number
     * 
     * Generates a random boolean value where the probability of returning true
     * is p. This follows a Bernoulli distribution with success probability p.
     * 
     * @param p Probability of returning true (must be in [0, 1])
     * @return bool Random boolean with probability p of being true
     * @throws std::runtime_error if p < 0 or p > 1
     */
    bool bernoulli(double p = 0.5);
    
    /**
     * @brief Generate a Poisson-distributed random number
     * 
     * Generates a random number from a Poisson distribution with rate parameter λ.
     * The Poisson distribution models the number of events occurring in a fixed
     * interval of time or space.
     * 
     * @param lambda Rate parameter (λ > 0), the expected number of events
     * @return uint32_t Poisson-distributed random number
     * @throws std::runtime_error if lambda < 0
     */
    uint32_t poisson(double lambda);
    
    /**
     * @brief Generate an exponentially distributed random number
     * 
     * Generates a random number from an exponential distribution with rate parameter λ.
     * The exponential distribution models the time between events in a Poisson process.
     * 
     * @param lambda Rate parameter (λ > 0), the average number of events per unit time
     * @return double Exponentially distributed random number (mean = 1/λ)
     * @throws std::runtime_error if lambda < 0
     */
    double exponential(double lambda);
    
    /**
     * @brief Generate a random boolean value
     * 
     * Generates a uniformly distributed random boolean value with equal probability
     * of being true or false (p = 0.5).
     * 
     * @return bool Random boolean with 50% probability of being true
     */
    bool randomBool();
    
    /**
     * @brief Generate a random bit
     * 
     * Generates a uniformly distributed random bit (0 or 1). This is equivalent
     * to bernoulli(0.5) but typically faster as it only uses one random number.
     * 
     * @return uint32_t Random bit (0 or 1)
     */
    uint32_t randomBit();
    
    /**
     * @brief Shuffle elements in a container
     * 
     * Randomly permutes the elements in the given container using the Fisher-Yates
     * shuffle algorithm. This destroys the original order of elements.
     * 
     * @tparam Container Type of container (must support iterator operations)
     * @param container The container to shuffle in place
     * @throws std::runtime_error if the container is empty
     */
    template<typename Container>
    void shuffle(Container& container);
    
    /**
     * @brief Pick a random element from a container
     * 
     * Returns a random element from the given container. The container must be
     * non-empty; otherwise, std::runtime_error is thrown.
     * 
     * @tparam Container Type of container (must support begin() and end())
     * @tparam Container Type of container (must support begin() and end())
     * @param container The container to pick from
     * @return typename Container::const_reference Reference to a randomly selected element
     * @throws std::runtime_error if the container is empty
     */
    template<typename Container>
    auto pickRandom(const Container& container) -> typename Container::const_reference;
    
    /**
     * @brief Pick a random index from 0 to n-1
     * 
     * Generates a uniformly distributed random index in the range [0, n-1].
     * Useful for randomly selecting array positions or container indices.
     * 
     * @param n The upper bound (exclusive) of the index range
     * @return uint64_t Random index in [0, n-1]
     * @throws std::runtime_error if n == 0
     */
    uint64_t pickRandomIndex(uint64_t n);
    
    /**
     * @brief Fill memory with random bytes
     * 
     * Fills a block of memory with cryptographically secure random bytes.
     * This can be used for generating random keys, nonces, or for seeding
     * other random number generators.
     * 
     * @param data Pointer to the memory buffer to fill
     * @param size Number of bytes to fill
     * @throws std::runtime_error if data is null and size > 0
     */
    void fillBytes(void* data, size_t size);
    
    /**
     * @brief Get access to the underlying random engine
     * 
     * Provides direct access to the underlying Mersenne Twister engine (std::mt19937_64).
     * This allows for advanced usage patterns, such as custom distributions or
     * thread-local random number generation.
     * 
     * @return std::mt19937_64& Reference to the internal random engine
     */
    std::mt19937_64& engine();
    
    /**
     * @brief Get access to the underlying random engine (const version)
     * 
     * Provides read-only access to the underlying random engine.
     * 
     * @return const std::mt19937_64& Const reference to the internal random engine
     */
    const std::mt19937_64& engine() const;
    
    /**
     * @brief Get the minimum value the engine can produce
     * 
     * Returns the minimum value that the Mersenne Twister engine can generate.
     * This is typically 0 for std::mt19937_64.
     * 
     * @return result_type Minimum value
     */
    static constexpr result_type min() { return std::mt19937_64::min(); }
    
    /**
     * @brief Get the maximum value the engine can produce
     * 
     * Returns the maximum value that the Mersenne Twister engine can generate.
     * This is typically UINT32_MAX for std::mt19937_64.
     * 
     * @return result_type Maximum value
     */
    static constexpr result_type max() { return std::mt19937_64::max(); }
    
    /**
     * @brief Operator for compatibility with std::generate and other std algorithms
     * 
     * Provides the function call operator that returns a random number in the
     * range [min(), max()]. This allows RandomGenerator to be used with standard
     * library algorithms that expect a uniform random bit generator.
     * 
     * @return result_type Random number in [min(), max()]
     */
    result_type operator()() { return static_cast<result_type>(engine()()); }
    
private:
    /**
     * @brief Internal implementation details (Pimpl idiom)
     * 
     * Encapsulates the internal implementation of the RandomGenerator class,
     * allowing for better compilation times and hiding implementation details
     * from the header file.
     */
    struct Impl;
    Impl* pImpl;
};
