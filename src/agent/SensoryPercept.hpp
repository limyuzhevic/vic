#pragma once

#include "AgentBody.hpp"
#include "IntegrationAbstractions.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <span>
#include <string_view>
#include <chrono>
#include <expected>
#include <compare>
#include <concepts>
#include <optional>

namespace nlm {

// C++20: Concepts for better template constraints
template<typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

template<typename Container>
concept ContiguousContainer = std::contiguous_iterator<typename Container::iterator>;

// C++20: Type aliases for common patterns
template<typename T>
using Result = std::expected<T, std::string>;

template<typename T>
using View = std::span<T>;

// C++20: In-place new type for perfect forwarding
template<typename T>
using InPlace = std::in_place_t;

// C++20: Union type for discriminated unions
template<typename... Types>
using Variant = std::variant<Types...>;

// Forward declarations for better encapsulation
class RandomGenerator;
class Neuron;

// Sensory data packet - what the agent perceives
// Contains only low-level sensory signals, NO semantic labels
class SensoryPercept {
public:
    // C++20: Use structured binding in constructor
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0 - optimized with std::span for performance
    [[nodiscard]] std::span<const float> getVision() const noexcept { return vision_; }
    void setVision(std::span<const float> v) { vision_.assign(v.begin(), v.end()); }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Touch/proximity sensors (binary collision signals)
    [[nodiscard]] std::span<const float> getTouch() const noexcept { return touch_; }
    void setTouch(std::span<const float> t) { touch_.assign(t.begin(), t.end()); }
    
    // Internal state signals (homeostatic signals)
    [[nodiscard]] std::span<const float> getInternal() const noexcept { return internal_; }
    void setInternal(std::span<const float> i) { internal_.assign(i.begin(), i.end()); }
    
    // Proprioception: body position/velocity signals
    [[nodiscard]] std::span<const float> getProprioception() const noexcept { return proprioception_; }
    void setProprioception(std::span<const float> p) { proprioception_.assign(p.begin(), p.end()); }
    
    // Audio-like signals (if enabled)
    [[nodiscard]] std::span<const float> getAudio() const noexcept { return audio_; }
    void setAudio(std::span<const float> a) { audio_.assign(a.begin(), a.end()); }
    
    // Get all signals concatenated (for brain input) - using modern C++20 algorithms
    [[nodiscard]] std::vector<float> getAllSignals() const;
    
    // Process this percept using an integrated sensory processor
    static Result<SensoryPercept> processWith(IntegratedSensoryProcessor& processor,
                                          const AgentBody& agent,
                                          const SimpleWorld& world);
    
    // Timestamp - using chrono for better time handling
    [[nodiscard]] std::chrono::nanoseconds getTimestamp() const { return timestamp_; }
    void setTimestamp(std::chrono::nanoseconds t) { timestamp_ = t; }
    
    // C++20: Constexpr if for conditional compilation
    [[nodiscard]] size_t getTotalSignalSize() const noexcept {
        size_t total = 0;
        if constexpr (!vision_.empty()) total += vision_.size();
        if constexpr (!touch_.empty()) total += touch_.size();
        if constexpr (!internal_.empty()) total += internal_.size();
        if constexpr (!proprioception_.empty()) total += proprioception_.size();
        if constexpr (!audio_.empty()) total += audio_.size();
        return total;
    }
    
    // C++20: Three-way comparison operator
    auto operator<=>(const SensoryPercept&) const = default;
    
    // C++20: Equality comparison
    bool operator==(const SensoryPercept&) const = default;
    
    // C++20: Move semantics optimizations
    SensoryPercept(SensoryPercept&&) noexcept = default;
    SensoryPercept& operator=(SensoryPercept&&) noexcept = default;
    
    // C++20: Constexpr constructor for empty percept
    constexpr SensoryPercept()
        : visionWidth_(16), visionHeight_(16), timestamp_(0ns) {
        vision_.resize(visionWidth_ * visionHeight_, 0.0f);
        touch_.resize(8, 0.0f);
        internal_.resize(4, 0.0f);
        proprioception_.resize(6, 0.0f);
        audio_.clear(); // Audio is variable length
    }
    
    // C++20: Factory method for creating percept from span
    static Result<SensoryPercept> fromSpan(const std::span<const float>& vision,
                                         size_t visionWidth, size_t visionHeight);
    
    // C++20: Structured binding helper for better readability
    [[nodiscard]] std::tuple<std::span<const float>, std::span<const float>, 
                           std::span<const float>, std::span<const float>, 
                           std::span<const float>> getAllSignalsAsTuple() const noexcept;
    
private:
    std::vector<float> vision_;
    size_t visionWidth_;
    size_t visionHeight_;
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    std::chrono::nanoseconds timestamp_;
};

} // namespace nlm
