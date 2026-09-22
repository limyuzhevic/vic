#include "SensoryPercept.hpp"
#include "IntegrationAbstractions.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <numeric>
#include <cassert>
#include <execution>
#include <numeric>
#include <chrono>
#include <span>
#include <expected>
#include <compare>

namespace nlm {

namespace {
    // Helper functions for better performance and error handling
    constexpr float CLAMP(float value, float min, float max) noexcept {
        return (value < min) ? min : (value > max) ? max : value;
    }
    
    template<typename Container>
    size_t calculateChecksum(const Container& container) noexcept {
        size_t checksum = 0;
        for (const auto& item : container) {
            checksum ^= static_cast<size_t>(item);
        }
        return checksum;
    }
}

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0ns) {
    // Initialize with C++20 in-place construction
    vision_.resize(visionWidth_ * visionHeight_, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.clear(); // Audio is variable length
}

Result<SensoryPercept> SensoryPercept::fromSpan(const std::span<const float>& vision,
                                             size_t visionWidth, size_t visionHeight) {
    if (vision.size() != visionWidth * visionHeight) {
        return std::unexpected("Vision span size doesn't match dimensions");
    }
    
    SensoryPercept percept;
    percept.vision_.assign(vision.begin(), vision.end());
    percept.visionWidth_ = visionWidth;
    percept.visionHeight_ = visionHeight;
    
    return percept;
}

std::tuple<std::span<const float>, std::span<const float>, 
           std::span<const float>, std::span<const float>, 
           std::span<const float>> SensoryPercept::getAllSignalsAsTuple() const noexcept {
    return {
        std::span<const float>(vision_),
        std::span<const float>(touch_),
        std::span<const float>(internal_),
        std::span<const float>(proprioception_),
        std::span<const float>(audio_)
    };
}

Result<SensoryPercept> SensoryPercept::processWith(IntegratedSensoryProcessor& processor,
                                               const AgentBody& agent,
                                               const SimpleWorld& world) {
    if (!processor.isEnabled()) {
        return std::unexpected("Sensory processor is not enabled");
    }
    
    // Initialize processor if needed
    processor.initialize();
    
    // Process input with error handling
    auto result = processor.createSensoryPercept(agent, world);
    if (!result) {
        return std::unexpected("Failed to create sensory percept: " + result.error());
    }
    
    return std::move(*result);
}

std::vector<float> SensoryPercept::getAllSignals() const {
    // Use C++20 structured binding for better clarity
    auto [vision, touch, internal, proprio, audio] = getAllSignalsAsTuple();
    
    std::vector<float> all;
    all.reserve(getTotalSignalSize()); // Pre-allocate for efficiency
    
    // Use ranges-style operations with better performance
    // Vision (flattened) - using insert with iterators
    all.insert(all.end(), vision.begin(), vision.end());
    
    // Touch - using algorithm for insertion
    all.insert(all.end(), touch.begin(), touch.end());
    
    // Internal
    all.insert(all.end(), internal.begin(), internal.end());
    
    // Proprioception
    all.insert(all.end(), proprio.begin(), proprio.end());
    
    // Audio
    all.insert(all.end(), audio.begin(), audio.end());
    
    return all;
}

} // namespace nlm
