#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Initialize sensory percept with standard dimensions
    //
    // Sets up a percept with biologically and empirically determined defaults:
    // - Vision: 16x16 grid (256 pixels) representing coarse visual field
    // - Touch: 8 sensors for proximity detection
    // - Internal: 4 homeostatic signals (energy, health, etc.)
    // - Proprioception: 6 signals (position, orientation, velocity)
    // - Audio: 0 signals (audio sensing not enabled by default)
    //
    // This creates a standardized input format that the brain can process
    // consistently across different perceptual situations.
    
    // Resize vision grid with zero initial intensities
    vision_.resize(16 * 16, 0.0f);
    
    // Initialize touch sensor signals
    touch_.resize(8, 0.0f);
    
    // Initialize internal homeostatic signals
    internal_.resize(4, 0.0f);
    
    // Initialize proprioceptive signals
    proprioception_.resize(6, 0.0f);
    
    // Audio sensing disabled by default (0 signals)
    audio_.resize(0, 0.0f);
}

std::vector<float> SensoryPercept::getAllSignals() const {
    // Concatenate all sensory modalities into a single vector
    //
    // This creates a flattened sensory input vector suitable for neural
    // processing and integration. The order is fixed and must match the
    // brain's input expectations:
    // - Vision signals first (256 values)
    // - Touch signals second (8 values)
    // - Internal signals third (4 values)
    // - Proprioception signals fourth (6 values)
    // - Audio signals fifth (0 values by default)
    //
    // The total size is 274 values for the standard percept configuration.
    // This standardized format enables consistent neural network design
    // and facilitates efficient sensory processing.
    
    std::vector<float> all;
    all.reserve(vision_.size() + touch_.size() + 
                internal_.size() + proprioception_.size() + audio_.size());
    
    // Vision (flattened) - coarse visual field representation
    all.insert(all.end(), vision_.begin(), vision_.end());
    
    // Touch - proximity and collision detection
    all.insert(all.end(), touch_.begin(), touch_.end());
    
    // Internal - homeostatic state (energy, health, metabolism)
    all.insert(all.end(), internal_.begin(), internal_.end());
    
    // Proprioception - body position and orientation
    all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    
    // Audio - sound signals (if enabled)
    all.insert(all.end(), audio_.begin(), audio_.end());
    
    return all;
}

} // namespace nlm
