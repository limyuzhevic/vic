#pragma once

#include <cstdint>
#include <random>
#include <functional>
#include <expected>
#include <compare>
#include <concepts>
#include <numeric>
#include <span>
#include <memory>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// C++20: Concepts for better template constraints
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

template<typename T>
concept Container = requires(T c) {
    { c.size() } -> std::same_as<size_t>;
    typename T::value_type;
};

// C++20: Type aliases for common patterns
template<typename T>
using Result = std::expected<T, std::string>;

template<typename T>
using View = std::span<T>;

// C++20: Enum class for deterministic random number generator
enum class RandomMode {
    Deterministic,
    PseudoRandom,
    MersenneTwister,
    PCG
};

// Deterministic random number generator
// Supports explicit seeds for reproducible experiments
class RandomGenerator {
public:
    using result_type = uint32_t;
    
    // C++20: Attributes for better documentation
    [[nodiscard]] static constexpr result_type min() { return std::mt19937_64::min(); }
    [[nodiscard]] static constexpr result_type max() { return std::mt19937_64::max(); }
    
    // C++20: Three-way comparison operator
    auto operator<=>(const RandomGenerator&) const = default;
    bool operator==(const RandomGenerator&) const = default;
    
    // Create with explicit seed for deterministic behavior
    explicit RandomGenerator(uint64_t seed);
    
    // Create with time-based seed (not deterministic)
    RandomGenerator();
    
    ~RandomGenerator();
    
    // Disable copying for thread safety
    RandomGenerator(const RandomGenerator&) = delete;
    RandomGenerator& operator=(const RandomGenerator&) = delete;
    
    // Enable moving for efficiency
    RandomGenerator(RandomGenerator&&) noexcept;
    RandomGenerator& operator=(RandomGenerator&&) noexcept;
    
    // C++20: Factory methods for creating generators
    static Result<std::unique_ptr<RandomGenerator>> create(uint64_t seed, RandomMode mode = RandomMode::Deterministic);
    static Result<std::unique_ptr<RandomGenerator>> createFromSeed(const std::string& seedString);
    
    // Seed operations with validation
    void seed(uint64_t seed);
    [[nodiscard]] uint64_t getSeed() const noexcept;
    
    // Get current mode
    [[nodiscard]] RandomMode getMode() const noexcept { return mode_; }
    
    // Set mode with validation
    void setMode(RandomMode mode);
    
    // C++20: constexpr validation functions
    static constexpr bool isValidSeed(uint64_t seed) noexcept {
        return seed != 0 && seed != UINT64_MAX;
    }
    
    static constexpr bool isValidRange(uint64_t min, uint64_t max) noexcept {
        return min < max && max > 0;
    }
    
    // Random number generation with enhanced validation
    // Uniform integer in [min, max]
    [[nodiscard]] uint32_t uniformInt(uint32_t min, uint32_t max);
    [[nodiscard]] uint64_t uniformInt(uint64_t min, uint64_t max);
    
    // Uniform real in [min, max) with boundary validation
    [[nodiscard]] double uniformReal(double min, double max);
    [[nodiscard]] float uniformReal(float min, float max);
    
    // Normal distribution with enhanced parameters
    [[nodiscard]] double normal(double mean = 0.0, double stddev = 1.0);
    [[nodiscard]] float normal(float mean = 0.0f, float stddev = 1.0f);
    
    // Bernoulli distribution
    [[nodiscard]] bool bernoulli(double p = 0.5);
    
    // Poisson distribution
    [[nodiscard]] uint32_t poisson(double lambda);
    
    // Exponential distribution
    [[nodiscard]] double exponential(double lambda);
    
    // Random boolean
    [[nodiscard]] bool randomBool();
    
    // Random bit
    [[nodiscard]] uint32_t randomBit();
    
    // Shuffle container with validation
    template<Container Container>
    void shuffle(Container& container);
    
    // Pick random element from container with validation
    template<Container Container>
    auto pickRandom(const Container& container) -> typename Container::const_reference;
    
    // Pick random index from 0 to n-1 with boundary checks
    [[nodiscard]] uint64_t pickRandomIndex(uint64_t n);
    
    // Fill range with random bytes
    void fillBytes(void* data, size_t size);
    
    // Get underlying engine for advanced use
    [[nodiscard]] std::mt19937_64& engine();
    [[nodiscard]] const std::mt19937_64& engine() const;
    
    // C++20: Observer pattern for generator state changes
    template<typename Observer>
    void addObserver(Observer&& observer) {
        observers_.push_back(std::forward<Observer>(observer));
    }
    
    // Notify observers of state change
    void notifyObservers() {
        for (auto& observer : observers_) {
            observer(*this);
        }
    }
    
private:
    struct Impl;
    Impl* pImpl;
    RandomMode mode_;
    
    // Observers for state changes
    std::vector<std::function<void(const RandomGenerator&)>> observers_;
};

// Template implementations with C++20 improvements
template<Container Container>
void RandomGenerator::shuffle(Container& container) {
    if (container.size() <= 1) return; // Early exit for small containers
    
    // Validate container is mutable and supports iterators
    static_assert(std::is_same_v<typename Container::value_type, 
                                std::remove_reference_t<decltype(*std::declval<Container&>().begin())>>,
                  "Container type mismatch");
    
    std::shuffle(container.begin(), container.end(), engine());
}

template<Container Container>
auto RandomGenerator::pickRandom(const Container& container) -> typename Container::const_reference {
    if (container.empty()) {
        throw std::runtime_error("Cannot pick from empty container");
    }
    
    // Use pickRandomIndex for consistent behavior
    auto index = pickRandomIndex(container.size());
    
    // Get iterator using std::next for efficiency
    auto it = container.begin();
    std::advance(it, static_cast<typename Container::iterator::difference_type>(index));
    
    return *it;
}

} // namespace nlm
