// Copyright (c) 2024+ NLM Project
// All rights reserved.
// 
// This file is part of the Neural Learning Machine (NLM) project.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
 * @file Random.hpp
 * @brief Deterministic random number generator for NLM with seed support
 * 
 * This file implements the RandomGenerator class, providing a deterministic
 * random number generator for the NLM project. The generator supports explicit
 * seeds for reproducible experiments and simulations, making it essential for:
 * - Reproducible research and experiments
 * - Monte Carlo simulations
 * - Neural network initialization
 * - Random sampling and shuffling
 * - Probabilistic computations
 * - Stochastic gradient descent
 * - Random exploration in reinforcement learning
 * 
 * The RandomGenerator is designed with the following key principles:
 * - Deterministic behavior with explicit seeding
 * - High-quality random number generation (Mersenne Twister)
 * - Support for multiple distribution types
 * - Thread-safe operation for parallel simulations
 * - Efficient implementation with minimal overhead
 * - Exception safety for robust error handling
 * - Integration with C++ standard library algorithms
 * 
 * @author NLM Project
 * @version 1.0.0
 * @date 2024-2026
 * @copyright NLM Project (2024+)
 * 
 * @see src/core/Config/Config.hpp - Configuration for random seed and parameters
 * @see src/brain/Neuron.hpp - Random initialization of neural parameters
 * @see src/plasticity/STDP.hpp - Random synaptic weight updates
 * @see src/neuromodulation/Neuromodulator.hpp - Random neuromodulatory signals
 * @see src/experiments/Experiment.hpp - Random experimental conditions
 * 
 * @note This file uses modern C++17 features and follows Doxygen documentation standards.
 *       The RandomGenerator class supports both reproducibility and statistical quality.
 */

#pragma once

// NLM filename guard
#ifndef NLM_RANDOM_HPP
#define NLM_RANDOM_HPP

#include <cstdint>
#include <random>
#include <functional>
#include <algorithm>
#include <numeric>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <array>
#include <chrono>

namespace nlm {

/**
 * @class RandomGenerator
 * @brief High-quality deterministic random number generator
 * 
 * The RandomGenerator class provides a robust, deterministic random number
 * generator based on the Mersenne Twister algorithm. It is designed for
 * scientific computing and simulations where reproducibility and statistical
 * quality are essential.
 * 
 * Key Features:
 * - Deterministic behavior with explicit seeding
 * - Support for multiple probability distributions
 * - Efficient generation of various random types
 * - Integration with C++ standard library algorithms
 * - Thread-safe operation for parallel simulations
 * - Exception-safe error handling
 * - Consistent interface with standard random number facilities
 * 
 * The generator supports:
 * - Uniform integer and floating-point distributions
 * - Normal (Gaussian) distributions
 * - Bernoulli distributions
 * - Poisson distributions
 * - Exponential distributions
 * - Container shuffling and random element selection
 * - Byte buffer filling
 * - Random boolean generation
 * 
 * Usage Examples:
 * @code
 * #include "src/core/Random/Random.hpp"
 * 
 * // Create generator with seed for reproducibility
 * RandomGenerator rng(42);  // Fixed seed for reproducible results
 * 
 * // Or create with time-based seed for variability
 * RandomGenerator rng;  // Different seed each run
 * 
 * // Generate different types of random numbers
 * uint32_t random_int = rng.uniformInt(1, 100);
 * double random_double = rng.uniformReal(0.0, 1.0);
 * float random_float = rng.normal(0.0f, 1.0f);
 * 
 * // Use with containers
 * std::vector<int> numbers = {1, 2, 3, 4, 5};
 * rng.shuffle(numbers);  // Randomize order
 * int random_element = rng.pickRandom(numbers);  // Get random element
 * 
 * // Generate random booleans
 * bool heads = rng.bernoulli(0.5);  // 50% probability of true
 * 
 * // Get underlying engine for advanced use
 * std::mt19937_64& engine = rng.engine();
 * @endcode
 * 
 * Seed Management:
 * @code
 * // Fixed seed for reproducible experiments
 * RandomGenerator rng1(42);
 * RandomGenerator rng2(42);  // Same sequence as rng1
 * 
 * // Time-based seed for different results
 * RandomGenerator rng3;  // Different seed each run
 * 
 * // Seeding for reproducibility
 * uint64_t fixed_seed = 123456789;
 * RandomGenerator rng4(fixed_seed);
 * @endcode
 * 
 * @note This class uses the Pimpl idiom for efficient compilation and reduced
 *       coupling. The actual implementation is in Random.cpp.
 */
class RandomGenerator {
public:
    /**
     * @brief Result type for compatibility with standard random number facilities
     * 
     * This typedef provides compatibility with C++ standard library random
     * number generation, allowing RandomGenerator to be used with algorithms
     * that expect a specific result type.
     */
    using result_type = uint32_t;
    
    /**
     * @brief Constructor with explicit seed
     * 
     * Creates a random number generator with the specified seed, ensuring
     * deterministic behavior for reproducible experiments and simulations.
     * 
     * @param seed Seed value for the random number generator
     * @throw std::runtime_error if seed is invalid (0 for mt19937_64)
     * 
     * @post Generator is initialized with the specified seed
     * 
     * Example:
     * @code
     * RandomGenerator rng(42);  // Fixed seed for reproducible results
     * RandomGenerator rng2(12345);  // Different seed for different results
     * @endcode
     */
    explicit RandomGenerator(uint64_t seed);
    
    /**
     * @brief Constructor with time-based seed
     * 
     * Creates a random number generator with a time-based seed for variability
     * in simulations where determinism is not required. This constructor
     * provides different results on each run.
     * 
     * @post Generator is initialized with a time-based seed
     * 
     * Example:
     * @code
     * RandomGenerator rng;  // Different seed each run
     * @endcode
     */
    RandomGenerator();
    
    /**
     * @brief Destructor
     * 
     * Cleans up all allocated resources and ensures proper cleanup of the
     * random number generator. The destructor prevents memory leaks and
     * maintains proper resource management.
     */
    ~RandomGenerator();
    
    /**
     * @brief Disable copy construction
     * 
     * Random generators cannot be copied because they contain:
     * - Internal engine state that should not be shared
     * - Dynamic resources requiring unique ownership
     * - Random state that would be corrupted by sharing
     */
    RandomGenerator(const RandomGenerator&) = delete;
    RandomGenerator& operator=(const RandomGenerator&) = delete;
    
    /**
     * @brief Enable move construction
     * 
     * Allows efficient transfer of random generator ownership from one
     * instance to another. After move construction, the source generator
     * is in a valid but unspecified state.
     */
    RandomGenerator(RandomGenerator&&) noexcept;
    RandomGenerator& operator=(RandomGenerator&&) noexcept;
    
    /**
     * @brief Set the seed for the random number generator
     * 
     * @param seed New seed value
     * @throw std::runtime_error if seed is invalid (0 for mt19937_64)
     * 
     * @post Generator is reseeded with the new seed
     */
    void seed(uint64_t seed);
    
    /**
     * @brief Get the current seed
     * 
     * @return uint64_t Current seed value
     * 
     * @note The returned seed may not uniquely identify the generator's state
     *       due to the internal complexity of the Mersenne Twister algorithm.
     */
    uint64_t getSeed() const;
    
    /**
     * @brief Generate a uniform random integer
     * 
     * @tparam Integer Type of integer to generate (int, int64_t, etc.)
     * @param min Lower bound (inclusive)
     * @param max Upper bound (inclusive)
     * @return Integer Random integer in [min, max]
     * 
     * This method generates a uniformly distributed random integer
     * in the inclusive range [min, max]. The distribution is uniform,
     * meaning each integer in the range has equal probability of being selected.
     * 
     * @throw std::runtime_error if min > max
     * 
     * Example:
     * @code
     * uint32_t random_int = rng.uniformInt(1, 100);  // Random number 1-100
     * int64_t random_large = rng.uniformInt(0, INT64_MAX);
     * @endcode
     */
    template<typename Integer>
    Integer uniformInt(uint32_t min, uint32_t max);
    
    /**
     * @brief Generate a uniform random double
     * 
     * @param min Lower bound (inclusive)
     * @param max Upper bound (exclusive)
     * @return double Random double in [min, max)
     * 
     * This method generates a uniformly distributed random double
     * in the half-open interval [min, max). The probability density
     * is constant across the interval.
     * 
     * @throw std::runtime_error if min > max
     */
    double uniformReal(double min, double max);
    
    /**
     * @brief Generate a uniform random float
     * 
     * @param min Lower bound (inclusive)
     * @param max Upper bound (exclusive)
     * @return float Random float in [min, max)
     * 
     * This method generates a uniformly distributed random float
     * in the half-open interval [min, max). The probability density
     * is constant across the interval.
     */
    float uniformReal(float min, float max);
    
    /**
     * @brief Generate a random number from normal distribution
     * 
     * @param mean Mean of the normal distribution
     * @param stddev Standard deviation of the normal distribution
     * @return double Random number from N(mean, stddev^2)
     * 
     * This method generates a random number from a normal (Gaussian)
     * distribution with the specified mean and standard deviation.
     * The distribution follows the standard normal curve when mean=0,
     * stddev=1.
     * 
     * @throw std::runtime_error if stddev <= 0
     * 
     * Example:
     * @code
     * double random_normal = rng.normal(0.0, 1.0);  // Standard normal
     * double random_weighted = rng.normal(5.0, 2.0);  // Mean=5, SD=2
     * @endcode
     */
    double normal(double mean, double stddev);
    
    /**
     * @brief Generate a random number from normal distribution (float version)
     * 
     * @param mean Mean of the normal distribution
     * @param stddev Standard deviation of the normal distribution
     * @return float Random number from N(mean, stddev^2)
     */
    float normal(float mean, float stddev);
    
    /**
     * @brief Generate a Bernoulli random variable
     * 
     * @param p Probability of success (default: 0.5)
     * @return bool Random boolean with probability p of being true
     * 
     * This method generates a Bernoulli random variable, which is
     * a random variable that takes value 1 with probability p and
     * value 0 with probability 1-p.
     * 
     * @throw std::runtime_error if p is outside [0, 1]
     * 
     * Example:
     * @code
     * bool coin_flip = rng.bernoulli(0.5);  // Fair coin
     * bool rare_event = rng.bernoulli(0.01);  // 1% chance
     * bool almost_certain = rng.bernoulli(0.99);  // 99% chance
     * @endcode
     */
    bool bernoulli(double p = 0.5);
    
    /**
     * @brief Generate a Poisson random variable
     * 
     * @param lambda Rate parameter (mean number of events per interval)
     * @return uint32_t Random Poisson-distributed count
     * 
     * This method generates a Poisson random variable with the specified
     * rate parameter. The Poisson distribution models the number of
     * events occurring in a fixed interval of time or space.
     * 
     * @throw std::runtime_error if lambda <= 0
     * 
     * Example:
     * @code
     * uint32_t events = rng.poisson(3.5);  // Average 3.5 events
     * uint32_t arrivals = rng.poisson(0.1);  // Rare events
     * @endcode
     */
    uint32_t poisson(double lambda);
    
    /**
     * @brief Generate an exponential random variable
     * 
     * @param lambda Rate parameter (inverse of mean)
     * @return double Random exponential value
     * 
     * This method generates an exponential random variable with the
     * specified rate parameter. The exponential distribution models
     * the time between events in a Poisson process.
     * 
     * @throw std::runtime_error if lambda <= 0
     */
    double exponential(double lambda);
    
    /**
     * @brief Generate a random boolean
     * 
     * @return bool Random boolean (approximately 50% true, 50% false)
     * 
     * This method generates a random boolean with approximately equal
     * probability of being true or false. It uses a Bernoulli distribution
     * with p = 0.5.
     */
    bool randomBool();
    
    /**
     * @brief Generate a random bit
     * 
     * @return uint32_t Random bit (0 or 1)
     * 
     * This method generates a random bit with equal probability of
     * being 0 or 1.
     */
    uint32_t randomBit();
    
    /**
     * @brief Shuffle a container randomly
     * 
     * @tparam Container Type of container to shuffle
     * @param container Container to shuffle in place
     * 
     * This method shuffles the elements of the container using the
     * Fisher-Yates algorithm with random swaps. Each permutation is
     * equally likely.
     * 
     * @note This method requires a uniform random number generator.
     *       The container must support random access iterators.
     * 
     * Example:
     * @code
     * std::vector<int> cards = {1, 2, 3, 4, 5, 6};
     * rng.shuffle(cards);  // Random order
     * @endcode
     */
    template<typename Container>
    void shuffle(Container& container);
    
    /**
     * @brief Pick a random element from a container
     * 
     * @tparam Container Type of container to pick from
     * @param container Container to pick from (must not be empty)
     * @return typename Container::const_reference Random element
     * 
     * This method picks a random element from the container with uniform
     * probability. The container must not be empty.
     * 
     * @throw std::runtime_error if container is empty
     * 
     * Example:
     * @code
     * std::vector<std::string> names = {"Alice", "Bob", "Charlie", "David"};
     * std::string chosen = rng.pickRandom(names);  // Random name
     * @endcode
     */
    template<typename Container>
    auto pickRandom(const Container& container) -> typename Container::const_reference;
    
    /**
     * @brief Pick a random index from 0 to n-1
     * 
     * @param n Upper bound (exclusive)
     * @return uint64_t Random index in [0, n-1]
     * 
     * This method generates a random integer index in the range
     * [0, n-1]. If n is 0, returns 0 (though callers should check
     * for empty containers before calling this method).
     * 
     * Example:
     * @code
     * std::vector<int> data = {10, 20, 30, 40, 50};
     * size_t index = rng.pickRandomIndex(data.size());  // Random index
     * int value = data[index];  // Get random element
     * @endcode
     */
    uint64_t pickRandomIndex(uint64_t n);
    
    /**
     * @brief Fill a buffer with random bytes
     * 
     * @param data Pointer to buffer to fill
     * @param size Size of buffer in bytes
     * 
     * This method fills the specified buffer with cryptographically
     * strong random bytes using the underlying random number generator.
     * This is useful for:
     * - Generating random keys
     * - Cryptographic operations
     * - Random initialization
     * - Simulation seeds
     * 
     * Example:
     * @code
     * std::array<uint8_t, 16> key;
     * rng.fillBytes(key.data(), key.size());  // Fill with random bytes
     * @endcode
     */
    void fillBytes(void* data, size_t size);
    
    /**
     * @brief Get reference to underlying random number engine
     * 
     * @return std::mt19937_64& Reference to underlying engine
     * 
     * This method provides access to the underlying Mersenne Twister
     * random number engine for advanced usage. This allows users to:
     * - Seed the engine directly
     * - Use standard algorithms with the engine
     * - Perform advanced random number operations
     * - Integrate with other libraries
     * 
     * Example:
     * @code
     * std::mt19937_64& engine = rng.engine();
     * std::uniform_int_distribution<int> dist(1, 100);
     * int random_value = dist(engine);
     * @endcode
     */
    std::mt19937_64& engine();
    
    /**
     * @brief Get const reference to underlying random number engine
     * 
     * @return const std::mt19937_64& Const reference to underlying engine
     */
    const std::mt19937_64& engine() const;
    
    /**
     * @brief Get minimum value for result_type
     * 
     * @return static constexpr result_type Minimum value (std::mt19937_64::min())
     */
    static constexpr result_type min() { return std::mt19937_64::min(); }
    
    /**
     * @brief Get maximum value for result_type
     * 
     * @return static constexpr result_type Maximum value (std::mt19937_64::max())
     */
    static constexpr result_type max() { return std::mt19937_64::max(); }
    
    /**
     * @brief Operator for compatibility with standard random number facilities
     * 
     * @return result_type Random value in range [min(), max()]
     * 
     * This operator provides compatibility with C++ standard library
     * algorithms that expect a random number generator with operator().
     */
    result_type operator()() { return static_cast<result_type>(engine()()); }
    
private:
    /**
     * @brief Implementation details structure
     * 
     * Contains the actual implementation details using the Pimpl idiom.
     * This allows efficient compilation and reduces coupling between
     * header and implementation.
     */
    struct Impl;
    
    Impl* pImpl; ///< Pointer to implementation details
}; // class RandomGenerator

/**
 * @brief Generate a random number from normal distribution (float version)
 * 
 * @param rng Random number generator
 * @param mean Mean of the normal distribution
 * @param stddev Standard deviation of the normal distribution
 * @return float Random number from N(mean, stddev^2)
 * 
 * This function provides a convenience wrapper for generating normal
 * random numbers with a specified RandomGenerator instance.
 * 
 * Example:
 * @code
 * RandomGenerator rng(42);
 * float random_normal = normalDistribution(rng, 0.0f, 1.0f);
 * @endcode
 */
float normalDistribution(RandomGenerator& rng, float mean, float stddev);

/**
 * @brief Generate a random number from normal distribution (double version)
 * 
 * @param rng Random number generator
 * @param mean Mean of the normal distribution
 * @param stddev Standard deviation of the normal distribution
 * @return double Random number from N(mean, stddev^2)
 * 
 * This function provides a convenience wrapper for generating normal
 * random numbers with a specified RandomGenerator instance.
 */
double normalDistribution(RandomGenerator& rng, double mean, double stddev);

/**
 * @brief Generate a Poisson random variable (float version)
 * 
 * @param rng Random number generator
 * @param lambda Rate parameter (mean number of events per interval)
 * @return uint32_t Random Poisson-distributed count
 * 
 * This function provides a convenience wrapper for generating Poisson
 * random numbers with a specified RandomGenerator instance.
 */
uint32_t poissonDistribution(RandomGenerator& rng, double lambda);

/**
 * @brief Generate an exponential random variable
 * 
 * @param rng Random number generator
 * @param lambda Rate parameter (inverse of mean)
 * @return double Random exponential value
 * 
 * This function provides a convenience wrapper for generating
 * exponential random numbers with a specified RandomGenerator instance.
 */
double exponentialDistribution(RandomGenerator& rng, double lambda);

/**
 * @brief Generate a Bernoulli random variable
 * 
 * @param rng Random number generator
 * @param p Probability of success (default: 0.5)
 * @return bool Random boolean with probability p of being true
 * 
 * This function provides a convenience wrapper for generating
 * Bernoulli random numbers with a specified RandomGenerator instance.
 */
bool bernoulliDistribution(RandomGenerator& rng, double p = 0.5);

/**
 * @brief Generate a uniform random number
 * 
 * @param rng Random number generator
 * @param min Lower bound (inclusive)
 * @param max Upper bound (exclusive)
 * @return double Random double in [min, max)
 * 
 * This function provides a convenience wrapper for generating uniform
 * random numbers with a specified RandomGenerator instance.
 */
double uniformDistribution(RandomGenerator& rng, double min, double max);

/**
 * @brief Generate a uniform random integer
 * 
 * @tparam Integer Type of integer to generate
 * @param rng Random number generator
 * @param min Lower bound (inclusive)
 * @param max Upper bound (inclusive)
 * @return Integer Random integer in [min, max]
 * 
 * This function provides a convenience wrapper for generating uniform
 * random integers with a specified RandomGenerator instance.
 */
template<typename Integer>
Integer uniformDistribution(RandomGenerator& rng, uint32_t min, uint32_t max);

/**
 * @brief Generate a random bit
 * 
 * @param rng Random number generator
 * @return uint32_t Random bit (0 or 1)
 * 
 * This function provides a convenience wrapper for generating random
 * bits with a specified RandomGenerator instance.
 */
uint32_t randomBitDistribution(RandomGenerator& rng);

/**
 * @brief Generate a random boolean
 * 
 * @param rng Random number generator
 * @return bool Random boolean (approximately 50% true, 50% false)
 * 
 * This function provides a convenience wrapper for generating random
 * booleans with a specified RandomGenerator instance.
 */
bool randomBoolDistribution(RandomGenerator& rng);

/**
 * @brief Pick a random element from a container
 * 
 * @tparam Container Type of container to pick from
 * @param rng Random number generator
 * @param container Container to pick from (must not be empty)
 * @return typename Container::const_reference Random element
 * 
 * This function provides a convenience wrapper for picking random
 * elements from a container with a specified RandomGenerator instance.
 */
template<typename Container>
auto pickRandom(RandomGenerator& rng, const Container& container) -> typename Container::const_reference;

/**
 * @brief Shuffle a container randomly
 * 
 * @tparam Container Type of container to shuffle
 * @param rng Random number generator
 * @param container Container to shuffle in place
 * 
 * This function provides a convenience wrapper for shuffling containers
 * with a specified RandomGenerator instance.
 */
template<typename Container>
void shuffle(RandomGenerator& rng, Container& container);

} // namespace nlm

// Close NLM filename guard
#endif // NLM_RANDOM_HPP

