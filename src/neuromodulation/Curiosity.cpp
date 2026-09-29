/// Curiosity system implementation
/// Models the brain's curiosity and exploration drive
/// Integrates novelty detection and prediction error for exploration motivation

#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

/// Implementation details for Curiosity system
struct Curiosity::Impl {
    /// Pointer to the brain being modulated
    class Brain* brain;
    
    /// Current curiosity level (0.0 = no curiosity, 1.0 = high curiosity)
    float level;
    
    /// Weight for novelty detection in curiosity calculation
    float noveltyWeight;
    
    /// Weight for prediction error in curiosity calculation
    float predictionErrorWeight;
    
    /// Decay rate for curiosity (per time unit)
    float decayRate;
    
    /// Initialize with default curiosity parameters
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f) {
            NLM_LOG_INFO("Curiosity system initialized with default parameters");
        }
};

/// Constructor for Curiosity system
Curiosity::Curiosity() : pImpl(new Impl) {}

/// Destructor for Curiosity system
Curiosity::~Curiosity() = default;

/// Initialize curiosity system with brain reference
/// @param brain Pointer to the brain that this curiosity system modulates
void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->level = 0.0f;  // Reset curiosity level
    NLM_LOG_INFO("Curiosity system initialized and connected to brain");
}

/// Get current curiosity level
/// @return Current curiosity level (0.0 to 1.0)
float Curiosity::getLevel() const {
    return pImpl->level;
}

/// Update curiosity based on environmental inputs
/// Increases curiosity when experiencing novelty or prediction errors
/// @param novelty Amount of novelty detected in environment (0.0 to 1.0)
/// @param predictionError Magnitude of prediction error (0.0 to 1.0)
/// @param dt Time step duration in simulation units
void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Calculate target curiosity level based on inputs
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update to target level
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Natural decay of curiosity over time
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

/// Get current exploration drive
/// @return Current exploration drive (same as curiosity level)
float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

/// Set weight for novelty detection in curiosity
/// @param weight Weight for novelty (default: 0.5)
void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
}

/// Set weight for prediction error in curiosity
/// @param weight Weight for prediction error (default: 0.5)
void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

/// Reset curiosity system to initial state
void Curiosity::reset() {
    pImpl->level = 0.0f;
    NLM_LOG_INFO("Curiosity system reset");
}

} // namespace nlm
