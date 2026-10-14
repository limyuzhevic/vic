#include "SensoryPercept.hpp"

namespace nlm {

/**
 * @brief Constructor for SensoryPercept - initializes default sensory dimensions
 * 
 * Sets up the sensory percept with standard dimensions that match the agent's
 * expected sensory capabilities. The constructor initializes all sensory
 * buffers to their default sizes and sets initial values to zero, preparing
 * the agent for processing environmental inputs.
 * 
 * The default sensory configuration provides:
 * - Vision: 16×16 grid (256 values) - low-resolution visual input
 * - Touch: 8 proximity sensors - basic collision detection
 * - Internal: 4 homeostatic signals - physiological state representation
 * - Proprioception: 6 body state signals - position/velocity feedback
 * - Audio: 0 sensors - audio not implemented by default
 * 
 * This standardization allows for consistent processing across different
 * agent instances and simplifies the neural network input requirements.
 */
SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Initialize vision buffer (16×16 grayscale grid)
    // Each element represents pixel intensity in range [0.0, 1.0]
    vision_.resize(16 * 16, 0.0f);
    
    // Initialize touch sensors (8 proximity/collision sensors)
    // Each element represents contact strength, [0.0, 1.0] range
    touch_.resize(8, 0.0f);
    
    // Initialize internal signals (4 homeostatic variables)
    // Represents agent's internal physiological state
    internal_.resize(4, 0.0f);
    
    // Initialize proprioceptive sensors (6 body state variables)
    // Includes position, velocity, orientation for body awareness
    proprioception_.resize(6, 0.0f);
    
    // Audio sensors are not implemented by default (0 elements)
    // Audio perception can be added separately if needed
    audio_.resize(0, 0.0f);
}

/**
 * @brief Concatenate all sensory signals into a single vector
 * 
 * This method combines all available sensory modalities into one continuous
 * vector for efficient processing by the brain's input systems. The concatenated
 * signal preserves the order of modalities: vision, touch, internal, proprioception,
 * and audio (if implemented). This organization allows the brain to easily
 * identify and process each sensory channel's contribution to perception.
 * 
 * The concatenated vector enables the neural network to receive all sensory
 * inputs simultaneously without requiring complex indexing or separate input
 * channels, simplifying the brain's processing of multimodal sensory information.
 * 
 * @return Vector containing all sensory signals in order: vision, touch, internal,
         proprioception, audio
 */
std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    all.reserve(vision_.size() + touch_.size() + internal_.size() +
                proprioception_.size() + audio_.size());
    
    // Vision (flattened) - first 256 elements represent 16×16 grid
    all.insert(all.end(), vision_.begin(), vision_.end());
    
    // Touch (next 8 elements) - proximity/collision sensors
    all.insert(all.end(), touch_.begin(), touch_.end());
    
    // Internal (next 4 elements) - homeostatic signals
    all.insert(all.end(), internal_.begin(), internal_.end());
    
    // Proprioception (next 6 elements) - body state signals
    all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    
    // Audio (remaining elements, if any) - sound information
    all.insert(all.end(), audio_.begin(), audio_.end());
    
    return all;
}

} // namespace nlm
